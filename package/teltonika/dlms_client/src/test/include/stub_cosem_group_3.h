#include "master.h"

PUBLIC void attr_init(object_attributes *attributes, gxObject *object);
PUBLIC void attr_free(object_attributes *attributes);
PUBLIC int attr_to_string(gxObject *object, object_attributes *attributes);

PUBLIC void utl_append_if_needed(char **data, int index, int value, char *str);
PUBLIC void utl_append_obj_name(char **data, char *name);
PUBLIC void utl_append_to_str(char **source, const char *values);
PUBLIC void utl_add_error_message(char **data, char *device_name, const char *err_msg, const int err_num);
PUBLIC void utl_lock_mutex_if_required(physical_device *d);
PUBLIC void utl_unlock_mutex_if_required(physical_device *d);

PUBLIC int cg_make_connection(physical_device *dev);
PRIVATE int cg_read_cosem_object(cosem_object *cosem_object, physical_device *dev);
PRIVATE int cg_format_group_data(char **data, object_attributes *attr, char *device_name);

PUBLIC int com_read(connection *c, dlmsSettings *s, gxObject *object, unsigned char attributeOrdinal);
PUBLIC int com_close(connection *c, dlmsSettings *s);
