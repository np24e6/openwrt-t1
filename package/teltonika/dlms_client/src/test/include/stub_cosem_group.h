#include "master.h"

PUBLIC void utl_append_to_str(char **source, const char *values);

PUBLIC int com_open_connection(connection *connection);
PUBLIC int com_disconnect(connection *connection, dlmsSettings *settings);
PUBLIC int com_close(connection *connection, dlmsSettings *settings);
PUBLIC int com_update_invocation_counter(connection *connection, dlmsSettings *settings,
				  const char *invocationCounter);
PUBLIC int com_initialize_connection(connection *connection, dlmsSettings *settings);
PUBLIC int com_readRowsByEntry(connection *c, dlmsSettings *s, gxProfileGeneric *obj, int index, int count);
PUBLIC int com_close(connection *connection, dlmsSettings *settings);

PRIVATE int cg_read_cosem_object(cosem_object *cosem_object, physical_device *dev);
PRIVATE int cg_format_group_data(char **data, object_attributes *attr, char *device_name);

PUBLIC void attr_init(object_attributes *attributes, gxObject *object);
PUBLIC int attr_to_string(gxObject *object, object_attributes *attributes);
PUBLIC void attr_free(object_attributes *attributes);

PUBLIC void utl_append_to_str(char **source, const char *values);
PUBLIC void utl_append_if_needed(char **data, int index, int value, char *str);
PUBLIC void utl_append_obj_name(char **data, char *name);
PUBLIC void utl_add_error_message(char **data, char *device_name, const char *err_msg, const int err_num);
