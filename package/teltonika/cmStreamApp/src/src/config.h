#ifndef CONFIG_H
#define CONFIG_H

#define S_BUFF 10
#define UCI_BUFF 256
#define ADRR_LEN UCI_BUFF + 100
#define SERVICES_QNT 2
#define CONFIG "iot"

#define CONF_OPTIONS { "enabled", "ssl", "server", "tenant", "username", "password", "modem" }
enum conf {
	CONF_ENABLED,
	CONF_SSL,
	CONF_SERVER,
	CONF_TENANT,
	CONF_USERNAME,
	CONF_PASSWORD,
	CONF_MODEM,

	CONF_OPTIONS_MAX
};

#define CREDS_OPTIONS { "tenant", "username", "password" }
enum creds {
	CREDS_TENANT,
	CREDS_USERNAME,
	CREDS_PASSWORD,

	CREDS_OPTIONS_MAX,
};

typedef struct cloud_service {
	char *modem_data;
	char name[UCI_BUFF];
	char auth_f[UCI_BUFF];
	char opts[CONF_OPTIONS_MAX][UCI_BUFF];
} cloud_service;

typedef struct config {
	int services;
	char *router_str;
	cloud_service *cloud[SERVICES_QNT];
} config;

typedef struct dev_conn {
	char serial[UCI_BUFF];
	char addr[ADRR_LEN];
} dev_conn;

/**
* @brief Retrieve configuration values
* @param[ptr] 		config_name    		name of config  
* @param[size_t] 	config_options    	amount of options scanned in config
* @param[ptr] 		cnf					config data structure
* @return int. Count of enabled services
*/
int scan_config(const char *config_name, size_t config_options, config *cnf);

/**
* @brief Save needed values for connecting to server
* @param[ptr] conn_vals 		@struct dev_conn  
* @param[ptr] cloud				@struct cloud_service
* @return exit_success || exit_failure if no serial was found
*/
int get_conn_vals(dev_conn *conn_vals, cloud_service *cloud);

/**
* @brief Save auth file credentials to config
* @param[ptr] conf_f       		config file name   
* @param[ptr] service_name    	cloud service name
* @param[ptr] auth_f   			auth file location
*/
int save_credentials(const char *conf_f, const char *service_name, const char *auth_f);

/**
* @param[ptr] p      		config filename
* @param[ptr] p      		config section
* @param[ptr] p      		config option
* @return static string with option value | null
*/
char *get_config_value(const char *p, const char *s, const char *o);

#endif