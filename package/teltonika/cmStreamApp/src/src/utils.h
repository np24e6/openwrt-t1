#ifndef UTILS_H
#define UTILS_H

#include "config.h"
#ifdef MOBILE_SUPPORT
#include "modem.h"
#endif

#define CM_MODEM_ERR	   10
#define CM_DEVICE_ERR	   11
#define AGNT_INTEGR_ERR	   12
#define CONN_VAL_ERR	   13
#define AGNT_BOOTSTRAP_ERR 14
#define LUA_ERR		   15
#define REPORTER_ERR	   16
#define PUSH_ERR	   17
#define CONFIG_SCAN_ERR	   18
#define ROUTER_STR_ERR	   19

#define LUAPATH	     "/usr/lib/lua/cm"
#define LUA_LIB_PATH LUAPATH "/?.lua"
#define LUA_EXT	     ".lua"
#define LUA_TMPLT    LUAPATH "/srtemplate.txt"

#define S_BUFF	  10
#define BUFF_LEN  100
#define ROUTER_LEN (DEV_DATA_MAX * BUFF_LEN) + DEV_DATA_MAX + S_BUFF

#define IDD	 "104"
#define RDEV_STR "RouterDevice"
#define RTR_STR	 "Router"
#define URL	 "https://wiki.teltonika-networks.com/"

enum dev_data {
	DEV_RDEVICE,
	DEV_ROUTER,
	DEV_MODEL,
	DEV_HWVER,
	DEV_SERIAL,
	DEV_RTRNAME,
	DEV_FWVERSION,
	DEV_FWURL,
	DEV_IMEI,
	DEV_CELL,
	DEV_ICCID,
	DEV_IMSI,
	DEV_OPER,
	DEV_CONNTYPE,
	DEV_OPERNUM,

	DEV_DATA_MAX
};

/**
* @brief Collects device data and concatinates results to one string
* @param[ptr] cnf	config data structure
* @return amount of concated bites
**/
int get_router_data(config *cnf);

/**
* @brief Main function to start connection to
* @param[ptr] service		individual structure for cloud service
* @param[ptr] router_str	concated string consisting of router values
* @return success: 0
*/
int connect_to(cloud_service *service, char *router_str);

/**
* @brief Releases memory from config structure
*/
void clean_up(config *cnf);

#endif