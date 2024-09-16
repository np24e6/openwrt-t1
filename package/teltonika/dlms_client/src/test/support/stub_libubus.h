#ifndef STUB_LIBUBUS_H
#define STUB_LIBUBUS_H
#ifdef TEST
#include "stub_blobmsg.h"
#else
#include <libubox/blobmsg.h>
#endif

#define ubus_connect		 ubus_connect_orig
#define ubus_free		 ubus_free_orig
#define ubus_strerror		 ubus_strerror_orig
#define ubus_lookup_id		 ubus_lookup_id_orig
#define ubus_add_object		 ubus_add_object_orig
#define ubus_register_subscriber ubus_register_subscriber_orig
#define ubus_invoke		 ubus_invoke_orig
#define ubus_subscribe		 ubus_subscribe_orig
#define ubus_send_event		 ubus_send_event_orig
#define ubus_invoke_fd		 ubus_invoke_fd_orig
#define ubus_send_reply		 ubus_send_reply_orig
#include <libubus.h>
#undef ubus_connect
#undef ubus_free
#undef ubus_strerror
#undef ubus_lookup_id
#undef ubus_add_object
#undef ubus_register_subscriber
#undef ubus_invoke
#undef ubus_subscribe
#undef ubus_send_event
#undef ubus_invoke_fd
#undef ubus_send_reply

struct ubus_context *ubus_connect(const char *path);
void ubus_free(struct ubus_context *ctx);
const char *ubus_strerror(int error);
int ubus_lookup_id(struct ubus_context *ctx, const char *path, uint32_t *id);
int ubus_add_object(struct ubus_context *ctx, struct ubus_object *obj);
int ubus_register_subscriber(struct ubus_context *ctx, struct ubus_subscriber *obj);
int ubus_subscribe(struct ubus_context *ctx, struct ubus_subscriber *obj, uint32_t id);
int ubus_invoke(struct ubus_context *ctx, uint32_t obj, const char *method, struct blob_attr *msg,
		ubus_data_handler_t cb, void *priv, int timeout);
int ubus_send_event(struct ubus_context *ctx, const char *id, struct blob_attr *data);
int ubus_invoke_fd(struct ubus_context *ctx, uint32_t obj, const char *method, struct blob_attr *msg,
		   ubus_data_handler_t cb, void *priv, int timeout, int fd);
int ubus_send_reply(struct ubus_context *ctx, struct ubus_request_data *req, struct blob_attr *msg);

#endif // STUB_LIBUBUS_H
