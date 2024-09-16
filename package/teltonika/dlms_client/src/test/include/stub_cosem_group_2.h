#include "master.h"

PUBLIC char *cg_read_group_codes(cosem_group *group, int *rc);
PRIVATE int cg_read_profile_generic_data(cosem_object *obj, gxProfileGeneric *pg, physical_device *dev);

PUBLIC void utl_append_obj_name(char **data, char *name);
PUBLIC void utl_append_to_str(char **source, const char *values);

PUBLIC int com_read(connection *c, dlmsSettings *s, gxObject *object,
		    unsigned char attributeOrdinal);

PUBLIC void mstr_write_group_data_to_db(cosem_group *group, char *data);

char *attr_to_json(object_attributes *attributes);
