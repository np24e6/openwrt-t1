#include "config_info.h"

PRIVATE dlms_cfg *cfg_read_dlms_cfg();
PRIVATE master *cfg_read_master(dlms_cfg *cfg);

PRIVATE int cfg_init_mutexes(master *m);

PRIVATE connection *cfg_get_connection(connection_params_cfg *cfg);
PRIVATE physical_device *cfg_get_physical_device(physical_device_cfg *cfg, connection **connections, size_t connection_count);
PRIVATE cosem_object *cfg_get_cosem_object(cosem_object_cfg *cfg, physical_device **physical_devices, size_t physical_dev_count);
PRIVATE cosem_group *cfg_get_cosem_group(dlms_cfg *cfg, cosem_group_cfg *cosem_group_cfg, cosem_object **cosem_objects, size_t cosem_object_count);

PRIVATE void cfg_free_connection_cfg(connection_params_cfg *conn);
PRIVATE void cfg_free_physical_device_cfg(physical_device_cfg *dev);
PRIVATE void cfg_free_cosem_object_cfg(cosem_object_cfg *cosem);
PRIVATE void cfg_free_cosem_group_cfg(cosem_group_cfg *group);

PRIVATE void cfg_free_dlms_cfg(dlms_cfg *dlms);
PRIVATE void cfg_free_master(master *m);
