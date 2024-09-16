#include "master.h"

PRIVATE void cg_monitor_cb(struct uloop_timeout *timeout);
PRIVATE int cg_read_profile_generic_data(cosem_object *obj, gxProfileGeneric *pg, physical_device *dev);
PRIVATE int cg_read_cosem_object(cosem_object *cosem_object, physical_device *dev);
PRIVATE int cg_format_group_data(char **data, object_attributes *attr, char *device_name);
