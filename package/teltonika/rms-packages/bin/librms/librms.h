#include <libubus.h>

typedef enum {
	LRMS_OK,
	LRMS_UBUS_ERR,
} lrms_t;

typedef enum {
	RMS_CONNECTED,
	RMS_DISCONNECTED,
} lconnection_status_t;

typedef enum {
	RMS_NO_ERROR,
	RMS_ERROR,
} lrms_error;

struct lrms_status_st {
	int next_retry;
	lconnection_status_t connection_status;
	lrms_error error;
	char *error_text;
	int error_code;
};

lrms_t lrms_get_status(struct ubus_context *ubus, struct lrms_status_st *status);
lrms_t lrms_publish_event(struct ubus_context *ubus, int action, int id, const char *message, const char *serial);
