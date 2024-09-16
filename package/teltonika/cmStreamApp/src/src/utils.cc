#include <string>
#include <cstdlib>
#include <iostream>
#include <dirent.h>
#include <syslog.h>
#include <libsera/sragent.h>
#include <libsera/srreporter.h>
#include <libsera/srlogger.h>
#include <libsera/srdevicepush.h>
#include <libsera/srutils.h>
#include <libsera/srluapluginmanager.h>

extern "C" {
	#include <libmnfinfo/mnfinfo.h>	
}

#include "integrate.h"
#include "utils.h"
#ifdef MOBILE_SUPPORT
#include "modem.h"
#endif

using namespace std;

static void send_router_dataset(SrAgent *agnt, const char *device_data, const char *modem_data)
{
	string router_dataset = IDD;

	router_dataset += "," + agnt->ID1() + ",";
	router_dataset += device_data;
	router_dataset += modem_data;
	router_dataset += "-";

	agnt->send(router_dataset);
}

static int getdir(string dir, vector<string> &files)
{
	DIR *dp = NULL;
	struct dirent *dirp = {0};

	if ((dp = opendir(dir.c_str())) == NULL) {
		return EXIT_FAILURE;
	}

	while ((dirp = readdir(dp)) != NULL) {
		files.push_back(dir + "/" + string(dirp->d_name));
	}

	closedir(dp);

	return EXIT_SUCCESS;
}

static int load_lua(SrLuaPluginManager *lua)
{
	vector<string> files;

	lua->addLibPath(LUA_LIB_PATH);

	if (getdir(LUAPATH, files)) {
		return EXIT_FAILURE;
	}

	for (size_t i = 0; i < files.size(); i++) {
		if (files[i].find(LUA_EXT) != string::npos) {
			lua->load(files[i].c_str());
		}
	}

	return EXIT_SUCCESS;
}

static int integr_to_agnt(SrAgent *agnt)
{
	string ver = "";
	string tpl = "";

	if (readSrTemplate(LUA_TMPLT, ver, tpl) != 0) {
		syslog(LOG_INFO, "Failed to read template, exiting.");
		return EXIT_FAILURE;
	}

	if (agnt->integrate(ver, tpl)) {
		syslog(LOG_INFO, "Failed to integrate template, exiting.");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int get_router_data(config *cnf)
{
	char *tmp   = NULL;
	int len = 0;

	cnf->router_str = (char *)calloc(ROUTER_LEN, sizeof(char));
	if (!cnf->router_str) {
		return EXIT_FAILURE;
	}

	len += snprintf(cnf->router_str + len, BUFF_LEN - 1, "%s,", RDEV_STR);
	len += snprintf(cnf->router_str + len, BUFF_LEN - 1, "%s,", RTR_STR);

	tmp = lmnfinfo_get_name();
	len += snprintf(cnf->router_str + len, BUFF_LEN - 1, "%s,", tmp ? tmp : "");

	tmp = lmnfinfo_get_hwver();
	len += snprintf(cnf->router_str + len, BUFF_LEN - 1, "%s,", tmp ? tmp : "");

	tmp = lmnfinfo_get_sn();
	len += snprintf(cnf->router_str + len, BUFF_LEN - 1, "%s,", tmp ? tmp : "");

	tmp = get_config_value("system", "system", "devicename");
	len += snprintf(cnf->router_str + len, BUFF_LEN - 1, "%s,", tmp);

	tmp = get_config_value("system", "system", "device_fw_version");
	len += snprintf(cnf->router_str + len, BUFF_LEN - 1, "%s,", tmp);
	
	len += snprintf(cnf->router_str + len, BUFF_LEN - 1, "%s,", URL);

	if (len > ROUTER_LEN || len < 0) {
		return CM_DEVICE_ERR;
	}

	return EXIT_SUCCESS;
}

#ifdef MOBILE_SUPPORT
static int get_modem_data(cloud_service *service)
{
	struct ubus_context *ctx = NULL;
	lgsm_err_t gsm_err = LGSM_MODEM_ERROR;
	lgsm_t single_modem_arr = { 0 };
	lgsm_structed_info_t rsp = {};

	int modem_num = 0;
	char *modem_id = service->opts[CONF_MODEM];
	int len = 0;
	int ret = EXIT_SUCCESS;

	ctx = ubus_connect(NULL);
	if (!ctx) {
		syslog(LOG_INFO, "Ubus connect failed.");
		return EXIT_FAILURE;
	}

	service->modem_data = (char *)calloc(MODEM_LEN, sizeof(char));
	if (!service->modem_data) {
		ret = EXIT_FAILURE;
		goto end;
	}

	if (!modem_id || modem_id[0] == '\0') {
		modem_id = lgsmu_get_default_modem_id(ctx);
	}

	if ((modem_num = lgsmu_modem_id_to_num(ctx, modem_id)) < 0) {
		ret = CM_MODEM_ERR;
		memset(service->modem_data, ',', MODEM_DATA_MAX);
		goto end;
	}

	if (gsm_err = lgsm_get_modem_info(ctx, &single_modem_arr, modem_num)) {
		syslog(LOG_INFO, "Failed to handle detailed modem info gather. ERR: %s\n", lgsm_strerror(gsm_err));
	}

	snprintf(service->modem_data + strlen(service->modem_data), 
			MODEM_BUFF - 1, "%s,", single_modem_arr.imei);

	cm_get_gsm_cell(ctx, service->modem_data, modem_num, &rsp);

	cm_get_iccid(ctx, service->modem_data, modem_num, &rsp);

	snprintf(service->modem_data + strlen(service->modem_data), 
			MODEM_BUFF - 1, "%s,", single_modem_arr.imsi);

	cm_get_gsm_operator(ctx, service->modem_data, modem_num, &rsp);

	cm_get_gsm_signal(ctx, service->modem_data, modem_num, &rsp);

	cm_get_opernum(ctx, service->modem_data, modem_num, &rsp);

	handle_gsm_free(&single_modem_arr);

	handle_gsm_structed_info_free(&rsp);

end:
	ubus_free(ctx);

	return ret;
}
#endif // MOBILE_SUPPORT

static void release_service(cloud_service *cloud)
{
	free(cloud->modem_data);
	free(cloud);
}

void clean_up(config *cnf)
{
	for (int i = 0; i < cnf->services; i++) {
		free(cnf->cloud[i]);
	}

	free(cnf->router_str);
}

/* Combine connection function for thread */
int connect_to(cloud_service *service, char *router_str)
{
	Integrate igt;
	dev_conn conn_vals = {};

	if (get_conn_vals(&conn_vals, service)) {
		return CONN_VAL_ERR;
	}
#ifdef MOBILE_SUPPORT
	if (get_modem_data(service)) {
		syslog(LOG_INFO, "Couldn't retrieve modem data");
	}
#endif
	srLogSetLevel(SRLOG_INFO);

	SrAgent agnt(conn_vals.addr, conn_vals.serial, &igt);

	if (agnt.bootstrap(service->auth_f)) {
		return AGNT_BOOTSTRAP_ERR;
	}

	save_credentials(CONFIG, service->name, service->auth_f);

	integr_to_agnt(&agnt);
	
	send_router_dataset(&agnt, router_str, service->modem_data);

	release_service(service);

	SrLuaPluginManager lua(agnt);
	if (load_lua(&lua)) {
		return LUA_ERR;
	}

	SrReporter reporter(conn_vals.addr, agnt.XID(), agnt.auth(), agnt.egress, agnt.ingress);
	if (reporter.start()) {
		return REPORTER_ERR;
	}

	SrDevicePush push(conn_vals.addr, agnt.XID(), agnt.auth(), agnt.ID1(), agnt.ingress);
	if (push.start()) {
		return PUSH_ERR;
	}

	agnt.loop();

	return EXIT_SUCCESS;
}