#ifndef MODEM_H
#define MODEM_H

extern "C" {
	#include <libubus.h>
	#include <libgsm.h>
	#include <libgsm_utils.h>
}

#define MODEM_BUFF 100
#define DEFAULT_MODEM 1
#define MODEM_LEN (MODEM_DATA_MAX * MODEM_BUFF) + MODEM_DATA_MAX  + 10

enum modem_data {
	MODEM_IMEI,
	MODEM_CELL,
	MODEM_ICCID,
	MODEM_IMSI,
	MODEM_OPER,
	MODEM_CONNTYPE,
	MODEM_OPERNUM,

	MODEM_DATA_MAX,
};

/**
* Find currently used modem number and concat its info
* @param[ptr] ctx       ubus context    
* @param[ptr] modem     modem name
* @param[ptr] out       buffer to concate values
* @param[ptr] rsp		lgsm_arr_t struct
* @return int. Active modem number
*/
int cm_get_gsm_modem_data(struct ubus_context *ctx, char *modem, lgsm_arr_t *gsm_ar);

/**
* Concat retrieved mobile operator values
* @param[ptr] ctx           ubus context
* @param[ptr] out           buffer to concate values
* @param[int] modem_num     modems found on router
* @param[ptr] rsp			lgsm_structed_info_t struct
* @note Ptr out must be malloced with sizeof MODEM_LEN. Before adding new value register it to enum
*/
void cm_get_gsm_operator(struct ubus_context *ctx, char *out, int modem_num, lgsm_structed_info_t *rsp);

/**
* Concat retrieved mobile signal values
* @param[ptr] ctx           ubus context
* @param[ptr] out           buffer to concate values
* @param[int] modem_num     modems found on router
* @param[ptr] rsp			lgsm_structed_info_t struct
* @note Ptr out must be malloced with sizeof MODEM_LEN. Before adding new value register it to enum
*/
void cm_get_gsm_signal(struct ubus_context *ctx, char *out, int modem_num, lgsm_structed_info_t *rsp);

/**
* Concat retrieved mobile cell id
* @param[ptr] ctx           ubus context
* @param[ptr] out           buffer to concate values
* @param[int] modem_num     modems found on router
* @param[ptr] rsp			lgsm_structed_info_t struct
* @note Ptr out must be malloced with sizeof MODEM_LEN. Before adding new value register it to enum
*/
void cm_get_gsm_cell(struct ubus_context *ctx, char *out, int modem_num, lgsm_structed_info_t *rsp);

/**
* Concat retrieved mobile opernum
* @param[ptr] ctx           ubus context
* @param[ptr] out           buffer to concate values
* @param[int] modem_num     modems found on router
* @param[ptr] rsp			lgsm_structed_info_t struct
* @note Ptr out must be malloced with sizeof MODEM_LEN. Before adding new value register it to enum
*/
void cm_get_opernum(struct ubus_context *ctx, char *out, int modem_num, lgsm_structed_info_t *rsp);

/**
* Concat retrieved mobile iccid
* @param[ptr] ctx           ubus context
* @param[ptr] out           buffer to concate values
* @param[int] modem_num     modems found on router
* @param[ptr] rsp			lgsm_structed_info_t struct
* @note Ptr out must be malloced with sizeof MODEM_LEN. Before adding new value register it to enum
*/
void cm_get_iccid(struct ubus_context *ctx, char *out, int modem_num, lgsm_structed_info_t *rsp);

#endif