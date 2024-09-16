#pragma once

#include <libubox/blobmsg_json.h>
#include <libubox/blob.h>
#include <libubus.h>

typedef enum {
	LNET_OK,
	LNET_ERROR_UBUS,
	LNET_ERROR,
} lnetifd_err_t;

typedef enum {
	LNET_STATIC,
	LNET_WWAN,
	LNET_DHCP,
	LNET_PROTO_UNKNOWN,
} lnetifd_proto_t;

typedef enum {
	IFACE_VR_NAME,
	IFACE_UP,
	IFACE_L3_DEV,
	IFACE_METRIC,
	IFACE_DEV,
	IFACE_PROTO,
	IFACE_ROUTE,
	IFACE_MODEM,
	IFACE_DATA,
	IFACE_CNT,
} lnetifd_policy_t;

typedef enum {
	MWAN_UP,
	MWAN_UPTIME,
	MWAN_STATUS,
	MWAN_ENABLED,
	MWAN_CNT,
} lnetifd_mwan_t;

typedef enum {
	LNET_TYPE_UNKOWN,
	LNET_WIRED,
	LNET_WIFI,
	LNET_MOBILE,
} lnetifd_type_t;

typedef enum {
	LNET_ONLINE,
	LNET_OFFLINE,
	LNET_STATUS_DISABLED,
	LNET_STATUS_UNKNOWN,
} lnetifd_status_t;

typedef struct {
	char l3_dev[64];
	char dev[64];
	char vr_name[256];
	bool up;
	bool modem;
	bool static_mobile;
	lnetifd_proto_t proto;
	lnetifd_type_t type;
} lnetifd_t;

typedef struct {
	lnetifd_t *iface;
	int length;
} lnetifd_arr_t;

extern const struct blobmsg_policy g_iface_policy[];

lnetifd_err_t lnetifd_iface_list(struct ubus_context *ctx,
				 lnetifd_arr_t *array);
lnetifd_err_t lnetifd_wan_state(struct ubus_context *ctx,
				lnetifd_type_t *state);
lnetifd_err_t lnetifd_sub_netifd(struct ubus_context *ctx,
				 struct ubus_subscriber *netifd_sub,
				 ubus_handler_t cb);
lnetifd_err_t lnetifd_sub_iface(struct ubus_context *ctx,
				struct ubus_subscriber *netifd_sub,
				const char *vr_iface, ubus_handler_t cb);
lnetifd_err_t lnetifd_add_iface_data(struct ubus_context *ctx,
				     const char *vr_iface, struct blob_buf *b);

const char *lnetifd_strerror(lnetifd_err_t);
