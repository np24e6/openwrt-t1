
#ifndef __LIBPARAM_H
#define __LIBPARAM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <libubus.h>

#define GSM_ATTR_NONE -1

#define PARAM_SIZE 2

#define LPARAM_BUFF_16 16
#define LPARAM_BUFF_32 32
#define LPARAM_BUFF_64 64
#define LPARAM_BUFF_128 128
#define LPARAM_BUFF_256 256
#define IO_BUFF 100

typedef enum { // TODO: ADC param
	PARAM_INVALID, // must be the first element
	PARAM_NEW_LINE,
	PARAM_TIME,
	PARAM_UTC_TIME,
	PARAM_UNIX_TIME, // UNIX seconds since epoch
	PARAM_ROUTER_NAME,
	PARAM_PRODUCT_CODE,
	PARAM_ROUTER_SERIAL,
	PARAM_FW_VER,
	PARAM_IP_WAN,
	PARAM_MAC_WAN,
	PARAM_IP_LAN,
	PARAM_MAC_LAN,
	PARAM_MONITORING,
	PARAM_MONITORING_ERR,
#ifdef MOBILE_SUPPORT
	PARAM_IP_MOBILE,
	PARAM_OPERATOR,
	PARAM_SIM_STATE,
	PARAM_SIM_SLOT,
	PARAM_SIM_PIN_STATE,
	PARAM_SIM_ICCID,
	PARAM_MODEM_MODEL,
	PARAM_MODEM_SERIAL,
	PARAM_RSRP,
	PARAM_CELL_NETSTATE,
	PARAM_CELL_CONNSTATE,
	PARAM_CELL_NETWORK_INFO,
	PARAM_CELL_SERVING,
	PARAM_CELLID,
	PARAM_CELL_CONNTYPE,
	PARAM_CELL_SIGNAL,
	PARAM_CELL_NEIGHBOURS,
	PARAM_IMSI,
	PARAM_RSCP,
	PARAM_SINR,
	PARAM_RSRQ,
	PARAM_ECIO,
	PARAM_IMEI,
#endif
#ifdef GPS_SUPPORT
	PARAM_GPS,
#endif
#ifdef IO_SUPPORT
	PARAM_INPUT_NAME,
	PARAM_INPUT_STATE, 
	PARAM_GPIO_STATE,
#endif
#ifdef GPS_SUPPORT
	PARAM_GPS_INFO,
#endif
	PARAM_FIRMWARE_ON_SERVER,
#if defined(RUT9_PLATFORM) || defined(RUT9M_PLATFORM) || defined(RUT952_PLATFORM)
	PARAM_EVENT_TEXT,
#endif
	PARAM__LEN, // must remain the last element
} param_topic_t;

typedef enum {
	LPARAM_SUCCESS,
	LPARAM_ERROR
} lparam_stat;

typedef struct lparam_info lparam_info;

typedef struct {
	const lparam_info *extra;
	size_t n_extra;
	const char *input_name;
	int modem_num;
} param_ctx;

typedef struct {
	const lparam_info *param;
	const char *param_nm;
	param_topic_t topic;
} func_args;

typedef struct lparam_info {
	/* tag e.g. "ts" */
	const char param_nm[PARAM_SIZE + 1];
	/* "Time stamp" - description of parameter */
	const char *descr;
	/* function for expand the parameter */
	char *(*cb)(struct ubus_context *ubus, param_ctx *ctx, func_args *args);
	/* Static data */
	const char *data;
} lparam_info;


/**
 * @brief get param info by name
 * 
 * @param topic the pointer where the topic number is stored
 * @param name paramter name e.g. 'tz'
 * @return const lparam_info* pointer to parmater info
 */
const lparam_info *libparam_get_param_by_name(param_topic_t *topic, const char *name);

 /**
  * @brief Expand parameters in string
  * 
  * @param ubus UBUS context
  * @param ctx libparam comtext
  * @param text parameter for eavaluation ex.: %tz
  * @return char* poiner to expanded string
  */
char *libparam_str_expand(struct ubus_context *ubus, param_ctx *ctx, const char *text);

/**
 * @brief Evaluate %## parameter to string
 * 
 * @param ubus UBUS context
 * @param ctx libparam comtext
 * @param name parameter for eavaluation ex.: %tz
 * @param out must be freed()! pointer to a string pointer, which holds evaluated parameter string
 * @return size_t evaluated string len
 */
size_t libparam_eval(struct ubus_context *ubus, param_ctx *ctx,
		const char *name, char **out);

/**
  * Get parameter by index
  * @param[in] pos parameter position in array
  * @return poiner to parameter structure or NULL
  */
const lparam_info *libparam_get_param(int pos);

/**
  * Get all parameters
  * @return poiner to parameter structure array
  */
lparam_info* libparam_get_all_params(void);

void libparam_report(const char *te, bool ex);

#endif //__LIBPARAM_H
