#include "master.h"

PRIVATE void find_corresponding_mutex(connection *c);
PRIVATE int read_security_settings(struct blob_attr **tb, physical_device *d);
PRIVATE cosem_object *parse_cosem_object(struct blob_attr *b);
