
#ifndef RUTX_GSM_H
#define RUTX_GSM_H

#include <libgsm.h>

#define GSM_DEFAULT_USB_ID "3-1"

typedef enum {
    CHILLI_GSM_OK,
    CHILLI_GSM_ERR
} chilli_gsm_t;

chilli_gsm_t chilli_send_sms(const char *phone, const char *msg,
		const char *modem_id);
chilli_gsm_t chilli_send_sms_async(const char *phone, const char *msg,
		const char *modem_id);

#endif //RUTX_GSM_H

