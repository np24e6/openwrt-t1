#include <fstream>
#include <string>
#include <uci.h>
#include <syslog.h>
#include <iostream>
extern "C" {
	#include <libmnfinfo/mnfinfo.h>
}

#include "config.h"

using namespace std;

int scan_config(const char *conf_f, size_t vals_size, config *cnf)
{
	struct uci_package *pkg = NULL;
	struct uci_context *ctx = NULL;
	struct uci_element *e	= NULL;
	struct uci_section *s	= NULL;

	const char *tmp	= NULL;
	const char *option[CONF_OPTIONS_MAX] = CONF_OPTIONS;
	cnf->services = 0;
	int service = 0;

	ctx = uci_alloc_context();
	if (!ctx) {
		syslog(LOG_INFO, "Failed to allocate uci context, exiting.");
		return service;
	}

	if (uci_load(ctx, conf_f, &pkg) != UCI_OK) {
		syslog(LOG_INFO, "Failed to load uci config, exiting.");
		goto clean_up;
	}

	uci_foreach_element (&pkg->sections, e) {
		s = uci_to_section(e);

		tmp = uci_lookup_option_string(ctx, s, option[CONF_ENABLED]);
		if (!tmp || tmp[0] != '1') {
			continue;
		}

		cnf->cloud[service] = (cloud_service *)calloc(1, sizeof(cloud_service));
		if (!cnf->cloud[service]) {
			syslog(LOG_INFO, "Failed to malloc memory for cloud service, exiting.");
			goto unload;
		}

		strncpy(cnf->cloud[service]->opts[CONF_ENABLED], tmp, UCI_BUFF - 1);

		for (size_t i = 1; i < vals_size; i++) {

			tmp = uci_lookup_option_string(ctx, s, option[i]);
			if (!tmp) {
				continue;
			}
	
			strncpy(cnf->cloud[service]->opts[i], tmp, UCI_BUFF - 1);
		}

		strncpy(cnf->cloud[service]->name, s->e.name, UCI_BUFF - 1);
		snprintf(cnf->cloud[service]->auth_f, sizeof(cnf->cloud[service]->auth_f) - 1, "/tmp/%s",
			 s->e.name);

		service++;
	}

	cnf->services = service;

	if (!service) {
		syslog(LOG_INFO, "No enabled services found");
	}

unload:
	uci_unload(ctx, pkg);

clean_up:
	uci_free_context(ctx);

	return service;
}

static int set_config_options(const char *p, const char *s, char creds[CREDS_OPTIONS_MAX][UCI_BUFF])
{
	int ret = EXIT_SUCCESS;
	struct uci_context *ctx = NULL;
	struct uci_ptr ptr = {};

	const char *options[CREDS_OPTIONS_MAX] = { "tenant", "username", "password" };

	ctx = uci_alloc_context();
	if (!ctx) {
		syslog(LOG_INFO, "Failed to allocate uci context");
		return UCI_ERR_MEM;
	}

	ptr.package = p;
	ptr.section = s;

	if (uci_lookup_ptr(ctx, &ptr, NULL, false) != UCI_OK) {
		ret = UCI_ERR_NOTFOUND;
		syslog(LOG_INFO, "Couldn't find config file");
		goto clean;
	}

	for (size_t i = 0; i < CREDS_OPTIONS_MAX; i++) {
		ptr.option = options[i];
		ptr.value  = creds[i];

		if (uci_set(ctx, &ptr) != UCI_OK) {
			syslog(LOG_INFO, "Failed to save %s to config", ptr.option);
		}
	}

	uci_commit(ctx, &ptr.p, false);

clean:
	uci_free_context(ctx);

	return ret;
}

static int create_auth_f(const char *tenant, const char *username, const char *password, const char *auth_f)
{
	if (tenant[0] == 0 || username[0] == 0 || password[0] == 0) {
		syslog(LOG_INFO, "No credentials found, might need to re-register device");
		return EXIT_FAILURE;
	}

	ofstream f(auth_f, ios::out | ofstream::trunc);

	if (!f.is_open()) {
		syslog(LOG_INFO, "Unable to open %s file.", auth_f);
		return EXIT_FAILURE;
	}

	f << tenant << endl << username << endl << password;

	f.close();

	return EXIT_SUCCESS;
}

int get_conn_vals(dev_conn *conn_vals, cloud_service *cloud)
{
	const char *s = "";
	char *tmp = lmnfinfo_get_sn();
	if (!tmp) {
		syslog(LOG_INFO, "Device serial number wasn't found, exiting.");
		return EXIT_FAILURE;
	}

	strncpy(conn_vals->serial, tmp, sizeof(conn_vals->serial) - 1);

	if (!strncmp(cloud->name, "cloudofthings", sizeof(cloud->name)) || cloud->opts[CONF_SSL][0] == '1') {
		s = "s";
	}
	
	snprintf(conn_vals->addr, S_BUFF, "http%s://", s);

	strncat(conn_vals->addr, cloud->opts[CONF_SERVER], ADRR_LEN - S_BUFF - 1);

	create_auth_f(cloud->opts[CONF_TENANT], cloud->opts[CONF_USERNAME], cloud->opts[CONF_PASSWORD],
		      cloud->auth_f);

	return EXIT_SUCCESS;
}

int save_credentials(const char *conf_f, const char *service_name, const char *auth_f)
{
	string tmp = "";
	int line = 0;
	char credentials[CREDS_OPTIONS_MAX][UCI_BUFF] = { 0 };

	ifstream f(auth_f);

	if (!f.is_open()) {
		syslog(LOG_INFO, "Unable to open %s file.", auth_f);
		return EXIT_FAILURE;
	}

	while (getline(f, tmp) && line != CREDS_OPTIONS_MAX) {
		strncpy(credentials[line], tmp.c_str(), UCI_BUFF - 1);
		line++;
	}

	f.close();

	if (line != CREDS_OPTIONS_MAX) {
		syslog(LOG_INFO, "Missing some credentials");
		return EXIT_FAILURE;
	}

	if (set_config_options(conf_f, service_name, credentials)) {
		syslog(LOG_INFO, "Failed to save credentials");
	}

	return EXIT_SUCCESS;
}

char *get_config_value(const char *p, const char *s, const char *o)
{
	struct uci_context *uci = NULL;
	struct uci_ptr ptr = {};
	static char option_value[UCI_BUFF] = {0};

	uci = uci_alloc_context();
	if (!uci) {
		return option_value;
	}

	ptr.package = p;
	ptr.section = s;
	ptr.option = o;
	
	if (uci_lookup_ptr(uci, &ptr, NULL, false) != UCI_OK) {
		syslog(LOG_INFO, "Couldn't %s option value", ptr.option);
		goto clean;
	}

	if (ptr.o->v.string) {
		strncpy(option_value, ptr.o->v.string, UCI_BUFF - 1);
	}

clean:
	uci_free_context(uci);

	return option_value;
}