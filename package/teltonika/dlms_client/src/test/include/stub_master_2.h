#include "master.h"

PUBLIC char *cg_read_group_codes(cosem_group *group, int *rc);
PUBLIC void utl_smart_sleep(time_t *t0, unsigned long *tn, unsigned p);
PUBLIC void mstr_write_group_data_to_db(cosem_group *group, char *data);
