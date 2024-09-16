#include "config_info.h"

PRIVATE void cfg_free_connection_cfg(connection_params_cfg *conn);
PRIVATE void cfg_free_physical_device_cfg(physical_device_cfg *dev);
PRIVATE void cfg_free_cosem_object_cfg(cosem_object_cfg *cosem);
PRIVATE void cfg_free_cosem_group_cfg(cosem_group_cfg *group);

PRIVATE void cfg_free_cosem_object(cosem_object *o);
PRIVATE void cfg_free_cosem_group(cosem_group *g);
PRIVATE void cfg_free_connection(connection *c);
PRIVATE void cfg_free_physical_device(physical_device *d);
