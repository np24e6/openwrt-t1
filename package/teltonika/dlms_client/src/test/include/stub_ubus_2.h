#include "master.h"

PRIVATE int reply(struct ubus_context *ctx, struct ubus_request_data *req, const int err_code, const char *y);

PRIVATE int device_cb_args(physical_device *d, struct blob_attr *msg);
PUBLIC int cg_make_connection(physical_device *dev);
PUBLIC int com_close(connection *c, dlmsSettings *s);
PRIVATE void free_device(physical_device *dev);

PRIVATE int cosem_group_args(cosem_group *g, struct blob_attr *msg);
PUBLIC char *cg_read_group_codes(cosem_group *group, int *rc);
PRIVATE void free_cosem_group(cosem_group *g);

PUBLIC void utl_lock_mutex_if_required(physical_device *d);
PUBLIC void utl_unlock_mutex_if_required(physical_device *d);
