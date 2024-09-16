#ifndef TEST
#include <libubus.h>
#else
#include "stub_libubus.h"
#endif

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

#ifdef TEST
void recv_status_cb(struct ubus_request *req, int type, struct blob_attr *blob);
#endif
lrms_t lrms_get_status(struct ubus_context *ubus, struct lrms_status_st *status);
lrms_t lrms_publish_event(struct ubus_context *ubus, int action, int id, const char *message, const char *serial);

/**
 * @brief Free status structure
 * 
 * @param status pointer to status structure
 */
static inline void lrms_free_status(struct lrms_status_st *status)
{
	free(status->error_text);
}
