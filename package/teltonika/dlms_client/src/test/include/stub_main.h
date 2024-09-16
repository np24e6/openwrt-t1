#include "master.h"

PUBLIC master *cfg_get_master();
PUBLIC void cfg_free_master(master *m);

PUBLIC int mstr_create_db(master *m);
PUBLIC void mstr_db_free(master *m);
PUBLIC int mstr_initialize_cosem_groups(master *m);

PUBLIC int init_ubus_test_functions();
PUBLIC void ubus_exit();

PUBLIC int utl_parse_args(int argc, char **argv, log_level_type *debug_lvl);
