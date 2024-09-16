
#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#endif

#include "system.h"
#include <libgsm.h>
#include <libgsm_utils.h>

#include "gsm.h"

chilli_gsm_t chilli_send_sms(const char *phone, const char *msg,
		const char *modem_id)
{
	struct ubus_context *ubus;
	lgsm_structed_info_t data = { 0 };
	int ret = CHILLI_GSM_ERR;
	int modem_num;

	ubus = ubus_connect(NULL);
	if (!ubus) {
		return CHILLI_GSM_ERR;
	}

	if ((modem_num = lgsmu_modem_id_to_num(ubus, modem_id)) < 0) {
		syslog(LOG_ERR, "Unable to get modem '%s' number", modem_id);

		goto out;
	}

	if (lgsm_send_sms(ubus, phone, msg, &data, modem_num) == LGSM_SUCCESS) {
		ret = CHILLI_GSM_OK;
	} else {
		syslog(LOG_ERR, "Unable to send SMS message to %s", phone);
	}

out:
	ubus_free(ubus);
	handle_gsm_structed_info_free(&data);

	return ret;
}

chilli_gsm_t chilli_send_sms_async(const char *phone, const char *msg,
		const char *modem_id)
{
	int status;

	if ((status = fork()) < 0) {
		syslog(LOG_ERR, "%s: fork() returned -1!", strerror(errno));
		return CHILLI_GSM_ERR;
	}

	if (status > 0) { /* Parent */
		return CHILLI_GSM_OK;
	}

	chilli_send_sms(phone, msg, modem_id);

	exit(0);
}
