#include "config_info.h"

PRIVATE connection_params_cfg *cfg_read_connection(struct uci_context *uci, struct uci_section *section);
PRIVATE physical_device_cfg *cfg_read_physical_device(struct uci_context *uci, struct uci_section *section);
PRIVATE cosem_object_cfg *cfg_read_cosem_object(struct uci_context *uci, struct uci_section *section);
PRIVATE cosem_group_cfg *cfg_read_cosem_group(struct uci_context *uci, struct uci_section *section);

PRIVATE connection **cfg_get_connections(dlms_cfg *cfg, size_t *connection_count);
PRIVATE physical_device **cfg_get_physical_devices(dlms_cfg *cfg, connection **connections, size_t connection_count, size_t *physical_dev_count);
PRIVATE cosem_object **cfg_get_cosem_objects(dlms_cfg *cfg, physical_device **physical_devices, size_t physical_dev_count, size_t *cosem_object_count);
PRIVATE cosem_group **cfg_get_cosem_groups(dlms_cfg *cfg, cosem_object **cosem_objects, size_t cosem_object_count, size_t *cosem_group_count);

PRIVATE int cfg_init_mutex(pthread_mutex_t **mutex);

PRIVATE void cfg_free_dlms_cfg(dlms_cfg *dlms);
