#ifndef TEST
#include <libubus.h>
#include <libgsm.h>
#else
#include "stub_libubus.h"
#include "stub_libgsm.h"
#endif

typedef enum {
	LSMS_LIMIT_OK,
	LSMS_LIMIT_ERROR_UBUS,
	LSMS_LIMIT_WILL_REACH,
	LSMS_LIMIT_REACHED
} lsms_limit_t;

struct check_limit {
	int max;
	int current;
	bool reached;
};

lsms_limit_t lsms_limit_inc(struct ubus_context *ctx, lgsm_sim_t sim,
			    char *modem_id);
lsms_limit_t lsms_limit_reset(struct ubus_context *ctx, lgsm_sim_t sim,
			      char *modem_id);
lsms_limit_t lsms_limit_check(struct ubus_context *ctx, lgsm_sim_t sim,
			      char *modem_id, struct check_limit *limit);

#ifdef TEST
void recv_inc_cb(struct ubus_request *req, int type, struct blob_attr *blob);
void recv_check_cb(struct ubus_request *req, int type, struct blob_attr *blob);
#endif