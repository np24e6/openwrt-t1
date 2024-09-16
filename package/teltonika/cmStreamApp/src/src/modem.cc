#include "modem.h"

#include <syslog.h>
#include <stdio.h>
#include <string>

void cm_get_gsm_operator(struct ubus_context *ctx, char *out, int modem_num, lgsm_structed_info_t *rsp) 
{
	int len = strlen(out);

	if (lgsm_handle_methods_structed(ctx, LGSM_UBUS_SERVICE_PROVIDER, modem_num, rsp) !=
	    LGSM_SUCCESS) {
		syslog(LOG_INFO, "Failed to get operator info.");
	}

	len += snprintf(out + len, MODEM_BUFF - 1, "%s,", rsp->label == LGSM_LABEL_ERROR ? "N/A" : rsp->data.srvc_provider.provider_name);
}

void cm_get_gsm_signal(struct ubus_context *ctx, char *out, int modem_num, lgsm_structed_info_t *rsp) 
{
	int len = strlen(out);

	if (lgsm_handle_methods_structed(ctx, LGSM_UBUS_GET_SIGNAL_QRY, modem_num, rsp) !=
	    LGSM_SUCCESS) {
		syslog(LOG_INFO, "Failed to get signal info.");
	}

	len += snprintf(out + len, MODEM_BUFF - 1, "%s,", rsp->label == LGSM_LABEL_ERROR ? "N/A" : net_mode_str(rsp->data.signal_info.net_id));
}

void cm_get_gsm_cell(struct ubus_context *ctx, char *out, int modem_num, lgsm_structed_info_t *rsp)
{
	int len = strlen(out);

	if (lgsm_handle_methods_structed(ctx, LGSM_UBUS_GET_NET_REQ_STAT, modem_num, rsp) !=
		LGSM_SUCCESS) {
		syslog(LOG_INFO, "Failed to get signal info.");
	}

	len += snprintf(out + len, MODEM_BUFF - 1, "%s,", rsp->label == LGSM_LABEL_ERROR ? "N/A" : rsp->data.net_reg.net_ci);
}

void cm_get_opernum(struct ubus_context *ctx, char *out, int modem_num, lgsm_structed_info_t *rsp)
{
	int len = strlen(out);

	if (lgsm_handle_methods_structed(ctx, LGSM_UBUS_GET_NETWORK_INFO, modem_num, rsp) !=
		LGSM_SUCCESS) {
		syslog(LOG_INFO, "Failed to get opernum");
	}

	if (rsp->label == LGSM_LABEL_ERROR) {
		len += snprintf(out + len, MODEM_BUFF - 1, "N/A,");		
		return;
	}

	len += snprintf(out + len, MODEM_BUFF - 1, "%d,", rsp->data.net_info.opernum);
}

void cm_get_iccid(struct ubus_context *ctx, char *out, int modem_num, lgsm_structed_info_t *rsp)
{
	int len = strlen(out);

	if (lgsm_handle_methods_structed(ctx, LGSM_UBUS_GET_ICCID, modem_num, rsp) !=
		LGSM_SUCCESS) {
		syslog(LOG_INFO, "Failed to get iccid");
	}

	len += snprintf(out + len, MODEM_BUFF - 1, "%s,", rsp->label == LGSM_LABEL_ERROR ? "N/A" : rsp->data.s);
}