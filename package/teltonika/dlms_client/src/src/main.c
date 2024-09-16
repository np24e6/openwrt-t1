#include "master.h"

master *g_master = NULL;
log_level_type g_debug_level = L_EMERG;

int MAIN(int argc, char **argv)
{
	int rc = EXIT_FAILURE;

	if (utl_parse_args(argc, argv, &g_debug_level)) {
		fprintf(stderr, "Failed to parse arguments\n");
		return rc;
	}

	logger_init(g_debug_level, L_TYPE_STDOUT, "dlms_client");

	if (init_ubus_test_functions()) {
		log(L_ERROR, "Failed to initiate UBUS");
		return rc;
	}

	if (uloop_init()) {
		log(L_ERROR, "Failed to initiate uloop");
		goto err_free_ubus;
	}

	g_master = cfg_get_master();
	if (!g_master) {
		log(L_ERROR, "Failed to read configuration. Only UBUS will be initiated\n");
		goto uloop;
	}

	// utl_debug_master(g_master);

	if (mstr_create_db(g_master)) {
		log(L_ERROR, "Failed to create database\n");
		goto err_free_master;
	}

	if (mstr_initialize_cosem_groups(g_master)) {
		log(L_ERROR, "Failed to initialize COSEM groups");
		goto err_free_db;
	}

uloop:
	if (uloop_run()) {
		log(L_ERROR, "uloop_run returned an error");
		goto err_free_db;
	}

	rc = 0;
err_free_db:
	mstr_db_free(g_master);
err_free_master:
	cfg_free_master(g_master);
err_free_ubus:
	ubus_exit();
	return rc;
}
