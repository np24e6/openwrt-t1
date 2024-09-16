#ifndef TEST
#define TEST
#endif
#ifdef TEST

#include "unity.h"
#include "master.h"
#include "mock_tlt_logger.h"

#include "mock_stub_external.h"

#include "mock_stub_sqlite3.h"
#include "mock_stub_pthread.h"

master *g_master = NULL;

void setUp(void)
{
	static master m = { 0 };
	g_master = &m;
}

void test_mstr_create_db_check_compiled_without_threadsafe(void)
{
	sqlite3_threadsafe_ExpectAndReturn(0);
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, mstr_create_db(g_master));
}

void test_mstr_create_db_fail_to_open_db(void)
{
	sqlite3_threadsafe_ExpectAndReturn(1);
	sqlite3_open_ExpectAndReturn("/tmp/dlms_master.db", &g_master->db, 1);
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, mstr_create_db(g_master));
}

void test_mstr_create_db_fail_to_set_db_timeout(void)
{
	sqlite3 *db = (sqlite3 *)"database";

	sqlite3_threadsafe_ExpectAndReturn(1);
	sqlite3_open_ExpectAndReturn("/tmp/dlms_master.db", &g_master->db, 0);
	sqlite3_open_ReturnThruPtr_db(&db);

	sqlite3_busy_timeout_ExpectAndReturn(db, 30000, 1);
	sqlite3_errmsg_ExpectAndReturn(db, "Busy timeout error");
	_log_ExpectAnyArgs();

	sqlite3_close_v2_ExpectAndReturn(db, 0);
	TEST_ASSERT_EQUAL_INT(1, mstr_create_db(g_master));
}

void test_mstr_create_db_fail_to_create_data_table(void)
{
	sqlite3 *db = (sqlite3 *)"database";

	sqlite3_threadsafe_ExpectAndReturn(1);
	sqlite3_open_ExpectAndReturn("/tmp/dlms_master.db", &g_master->db, 0);
	sqlite3_open_ReturnThruPtr_db(&db);
	sqlite3_busy_timeout_ExpectAndReturn(db, 30000, 0);

	sqlite3_exec_ExpectAndReturn(db, NULL, NULL, NULL, NULL, 1);
	sqlite3_exec_IgnoreArg_sql();
	sqlite3_errmsg_ExpectAndReturn(db, "Exec error");
	_log_ExpectAnyArgs();

	sqlite3_close_v2_ExpectAndReturn(db, 0);

	TEST_ASSERT_EQUAL_INT(1, mstr_create_db(g_master));
}

void test_mstr_create_db_fail_to_create_sent_table(void)
{
	sqlite3 *db = (sqlite3 *)"database";

	sqlite3_threadsafe_ExpectAndReturn(1);
	sqlite3_open_ExpectAndReturn("/tmp/dlms_master.db", &g_master->db, 0);
	sqlite3_open_ReturnThruPtr_db(&db);

	sqlite3_busy_timeout_ExpectAndReturn(db, 30000, 0);
	sqlite3_exec_ExpectAndReturn(db, NULL, NULL, NULL, NULL, 0);
	sqlite3_exec_IgnoreArg_sql();

	sqlite3_exec_ExpectAndReturn(db, NULL, NULL, NULL, NULL, 1);
	sqlite3_exec_IgnoreArg_sql();
	sqlite3_errmsg_ExpectAndReturn(db, "Exec error");
	_log_ExpectAnyArgs();

	sqlite3_close_v2_ExpectAndReturn(db, 0);

	TEST_ASSERT_EQUAL_INT(1, mstr_create_db(g_master));
}

void test_mstr_create_db_fail_to_set_page_size(void)
{
	sqlite3 *db = (sqlite3 *)"database";

	sqlite3_threadsafe_ExpectAndReturn(1);
	sqlite3_open_ExpectAndReturn("/tmp/dlms_master.db", &g_master->db, 0);
	sqlite3_open_ReturnThruPtr_db(&db);

	sqlite3_busy_timeout_ExpectAndReturn(db, 30000, 0);

	for (size_t i = 0; i < 2; i++) {
		sqlite3_exec_ExpectAndReturn(db, NULL, NULL, NULL, NULL, 0);
		sqlite3_exec_IgnoreArg_sql();
	}

	sqlite3_exec_ExpectAndReturn(db, NULL, NULL, NULL, NULL, 1);
	sqlite3_exec_IgnoreArg_sql();
	sqlite3_errmsg_ExpectAndReturn(db, "Exec error");
	_log_ExpectAnyArgs();

	sqlite3_close_v2_ExpectAndReturn(db, 0);
	TEST_ASSERT_EQUAL_INT(1, mstr_create_db(g_master));
}

void test_mstr_create_db_fail_to_prepare_statement(void)
{
	sqlite3 *db = (sqlite3 *)"database";

	sqlite3_threadsafe_ExpectAndReturn(1);
	sqlite3_open_ExpectAndReturn("/tmp/dlms_master.db", &g_master->db, 0);
	sqlite3_open_ReturnThruPtr_db(&db);

	sqlite3_busy_timeout_ExpectAndReturn(db, 30000, 0);

	for (size_t i = 0; i < 3; i++) {
		sqlite3_exec_ExpectAndReturn(db, NULL, NULL, NULL, NULL, 0);
		sqlite3_exec_IgnoreArg_sql();
	}

	sqlite3_prepare_v2_ExpectAndReturn(db, NULL, -1, &g_master->stmt_insert, NULL, 1);
	sqlite3_prepare_v2_IgnoreArg_zSql();
	sqlite3_errmsg_ExpectAndReturn(db, "");
	_log_ExpectAnyArgs();

	sqlite3_close_v2_ExpectAndReturn(db, 0);
	TEST_ASSERT_EQUAL_INT(1, mstr_create_db(g_master));
}

void test_mstr_create_db_successfully(void)
{
	sqlite3 *db = (sqlite3 *)"database";

	sqlite3_threadsafe_ExpectAndReturn(1);
	sqlite3_open_ExpectAndReturn("/tmp/dlms_master.db", &g_master->db, 0);
	sqlite3_open_ReturnThruPtr_db(&db);

	sqlite3_busy_timeout_ExpectAndReturn(db, 30000, 0);

	for (size_t i = 0; i < 3; i++) {
		sqlite3_exec_ExpectAndReturn(db, NULL, NULL, NULL, NULL, 0);
		sqlite3_exec_IgnoreArg_sql();
	}

	sqlite3_prepare_v2_ExpectAndReturn(db, NULL, -1, &g_master->stmt_insert, NULL, 0);
	sqlite3_prepare_v2_IgnoreArg_zSql();

	TEST_ASSERT_EQUAL_INT(0, mstr_create_db(g_master));
}

void test_mstr_initialize_cosem_group_initialisation_failed(void)
{
	master m = {
		.cosem_group_count = 1,
		.cosem_groups =
			(cosem_group *[]){
				(cosem_group[]) { { .id = 4 } },
			},
	};

	pthread_t thread = { 0 };
	pthread_create_ExpectAndReturn(&thread, NULL, NULL, m.cosem_groups[0], 1);
	pthread_create_IgnoreArg_start_routine();
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, mstr_initialize_cosem_groups(&m));
}

void test_mstr_initialize_cosem_group_initialisation_failed_with_name(void)
{
	master m = {
		.cosem_group_count = 1,
		.cosem_groups =
			(cosem_group *[]){
				(cosem_group[]) { { .id = 4,
				.name = "group_one" } },
			},
	};

	pthread_t thread = { 0 };
	pthread_create_ExpectAndReturn(&thread, NULL, NULL, m.cosem_groups[0], 1);
	pthread_create_IgnoreArg_start_routine();
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, mstr_initialize_cosem_groups(&m));
}

void test_mstr_initialize_cosem_group_initialisation_success(void)
{
	master m = {
		.cosem_group_count = 1,
		.cosem_groups =
			(cosem_group *[]){
				(cosem_group[]) { { .id = 4 } },
			},
	};

	pthread_t thread = { 0 };
	pthread_create_ExpectAndReturn(&thread, NULL, NULL, m.cosem_groups[0], 0);
	pthread_create_IgnoreArg_start_routine();

	TEST_ASSERT_EQUAL_INT(0, mstr_initialize_cosem_groups(&m));
}

static void expect_binding_failure()
{
	sqlite3_errmsg_ExpectAndReturn(g_master->db, "failure");
	_log_ExpectAnyArgs();
	sqlite3_reset_IgnoreAndReturn(0);
	sqlite3_clear_bindings_IgnoreAndReturn(0);
}

void test_mstr_write_group_data_to_db_bind_null_failure(void)
{
	cosem_group g = { .name = "group_one" };
	_log_ExpectAnyArgs();

	sqlite3_bind_null_ExpectAndReturn(g_master->stmt_insert, 1, 1);
	expect_binding_failure();

	mstr_write_group_data_to_db(&g, "data");
}

void test_mstr_write_group_data_to_db_bind_time_failure(void)
{
	cosem_group g = { .name = "group_one" };
	_log_ExpectAnyArgs();

	sqlite3_bind_null_ExpectAndReturn(g_master->stmt_insert, 1, 0);
	sqlite3_bind_int_ExpectAndReturn(g_master->stmt_insert, 2, time(NULL), 1);
	expect_binding_failure();

	mstr_write_group_data_to_db(&g, "data");
}

void test_mstr_write_group_data_to_db_bind_group_name_failure(void)
{
	cosem_group g = { .name = "group_one" };
	_log_ExpectAnyArgs();

	sqlite3_bind_null_ExpectAndReturn(g_master->stmt_insert, 1, 0);
	sqlite3_bind_int_ExpectAndReturn(g_master->stmt_insert, 2, time(NULL), 0);
	mystrlen_ExpectAndReturn(g.name, 9);
	sqlite3_bind_text_ExpectAndReturn(g_master->stmt_insert, 3, g.name, 9, SQLITE_STATIC, 1);
	expect_binding_failure();

	mstr_write_group_data_to_db(&g, "data");
}

void test_mstr_write_group_data_to_db_bind_group_name_length_zero(void)
{
	cosem_group g = { .name = "group_one" };
	_log_ExpectAnyArgs();

	sqlite3_bind_null_ExpectAndReturn(g_master->stmt_insert, 1, 0);
	sqlite3_bind_int_ExpectAndReturn(g_master->stmt_insert, 2, time(NULL), 0);
	mystrlen_ExpectAndReturn(g.name, 9);
	sqlite3_bind_text_ExpectAndReturn(g_master->stmt_insert, 3, g.name, 9, SQLITE_STATIC, 1);
	expect_binding_failure();

	mstr_write_group_data_to_db(&g, "data");
}

void test_mstr_write_group_data_to_db_bind_group_name_is_null(void)
{
	cosem_group g = { 0 };
	_log_ExpectAnyArgs();

	sqlite3_bind_null_ExpectAndReturn(g_master->stmt_insert, 1, 0);
	sqlite3_bind_int_ExpectAndReturn(g_master->stmt_insert, 2, time(NULL), 0);
	sqlite3_bind_text_ExpectAndReturn(g_master->stmt_insert, 3, g.name, 0, SQLITE_STATIC, 0);
	mystrlen_ExpectAndReturn("data", 4);
	sqlite3_bind_int_ExpectAndReturn(g_master->stmt_insert, 4, 4, 1);
	expect_binding_failure();

	mstr_write_group_data_to_db(&g, "data");
}

void test_mstr_write_group_data_to_db_bind_data_failure(void)
{
	cosem_group g = { .name = "group_one" };
	_log_ExpectAnyArgs();

	sqlite3_bind_null_ExpectAndReturn(g_master->stmt_insert, 1, 0);
	sqlite3_bind_int_ExpectAndReturn(g_master->stmt_insert, 2, time(NULL), 0);
	mystrlen_ExpectAndReturn(g.name, 9);
	sqlite3_bind_text_ExpectAndReturn(g_master->stmt_insert, 3, g.name, 9, SQLITE_STATIC, 0);
	mystrlen_ExpectAndReturn("data", 4);
	sqlite3_bind_int_ExpectAndReturn(g_master->stmt_insert, 4, 4, 0);
	mystrlen_ExpectAndReturn("data", 4);
	sqlite3_bind_text_ExpectAndReturn(g_master->stmt_insert, 5, "data", 4, SQLITE_STATIC, 1);
	expect_binding_failure();

	mstr_write_group_data_to_db(&g, "data");
}

static void successful_bindings(cosem_group g)
{
	_log_ExpectAnyArgs();

	sqlite3_bind_null_ExpectAndReturn(g_master->stmt_insert, 1, 0);
	sqlite3_bind_int_ExpectAndReturn(g_master->stmt_insert, 2, time(NULL), 0);
	mystrlen_ExpectAndReturn(g.name, 9);
	sqlite3_bind_text_ExpectAndReturn(g_master->stmt_insert, 3, g.name, 9, SQLITE_STATIC, 0);
	mystrlen_ExpectAndReturn("data", 4);
	sqlite3_bind_int_ExpectAndReturn(g_master->stmt_insert, 4, 4, 0);
	mystrlen_ExpectAndReturn("data", 4);
	sqlite3_bind_text_ExpectAndReturn(g_master->stmt_insert, 5, "data", 4, SQLITE_STATIC, 0);
}

void test_mstr_write_group_data_to_db_step_failure(void)
{
	cosem_group g = { .name = "group_one" };
	successful_bindings(g);

	sqlite3_step_ExpectAndReturn(g_master->stmt_insert, 1);
	sqlite3_errmsg_ExpectAndReturn(g_master->db, "sqlite3_step failure");
	_log_ExpectAnyArgs();

	sqlite3_reset_IgnoreAndReturn(0);
	sqlite3_clear_bindings_IgnoreAndReturn(0);

	mstr_write_group_data_to_db(&g, "data");
}

void test_mstr_write_group_data_to_db_database_is_full_and_cleanup_failure(void)
{
	cosem_group g = { .name = "group_one" };
	successful_bindings(g);

	sqlite3_step_ExpectAndReturn(g_master->stmt_insert, SQLITE_FULL);
	sqlite3_exec_ExpectAndReturn(g_master->db, NULL, NULL, NULL, NULL, 1);
	sqlite3_exec_IgnoreArg_sql();
	sqlite3_errmsg_ExpectAndReturn(g_master->db, "Exec error");
	_log_ExpectAnyArgs();

	sqlite3_reset_IgnoreAndReturn(0);
	sqlite3_clear_bindings_IgnoreAndReturn(0);

	mstr_write_group_data_to_db(&g, "data");
}

void test_mstr_write_group_data_to_db_database_is_full_and_step_failure(void)
{
	cosem_group g = { .name = "group_one" };
	successful_bindings(g);

	sqlite3_step_ExpectAndReturn(g_master->stmt_insert, SQLITE_FULL);
	sqlite3_exec_ExpectAndReturn(g_master->db, NULL, NULL, NULL, NULL, SQLITE_OK);
	sqlite3_exec_IgnoreArg_sql();

	_log_ExpectAnyArgs();
	sqlite3_step_ExpectAndReturn(g_master->stmt_insert, SQLITE_FULL);
	sqlite3_errmsg_ExpectAndReturn(g_master->db, "step error");
	_log_ExpectAnyArgs();

	sqlite3_reset_IgnoreAndReturn(0);
	sqlite3_clear_bindings_IgnoreAndReturn(0);

	mstr_write_group_data_to_db(&g, "data");
}

void test_mstr_write_group_data_to_db_database_is_full_and_step_success_after_delete(void)
{
	cosem_group g = { .name = "group_one" };
	successful_bindings(g);

	sqlite3_step_ExpectAndReturn(g_master->stmt_insert, SQLITE_FULL);
	sqlite3_exec_ExpectAndReturn(g_master->db, NULL, NULL, NULL, NULL, SQLITE_OK);
	sqlite3_exec_IgnoreArg_sql();

	_log_ExpectAnyArgs();
	sqlite3_step_ExpectAndReturn(g_master->stmt_insert, SQLITE_DONE);

	sqlite3_reset_IgnoreAndReturn(0);
	sqlite3_clear_bindings_IgnoreAndReturn(0);

	mstr_write_group_data_to_db(&g, "data");
}

void test_mstr_write_group_data_to_db_success(void)
{
	cosem_group g = { .name = "group_one" };
	successful_bindings(g);

	sqlite3_step_ExpectAndReturn(g_master->stmt_insert, SQLITE_DONE);

	sqlite3_reset_IgnoreAndReturn(0);
	sqlite3_clear_bindings_IgnoreAndReturn(0);

	mstr_write_group_data_to_db(&g, "data");
}

void test_mstr_db_free(void)
{
	master m = {
		.db	     = (sqlite3 *)"database",
		.stmt_insert = (sqlite3_stmt *)"stmt",
	};

	sqlite3_finalize_ExpectAndReturn(m.stmt_insert, 0);
	sqlite3_close_v2_ExpectAndReturn(m.db, 0);

	mstr_db_free(&m);
}

void test_mstr_db_free_master_struct_is_null(void)
{
	mstr_db_free(NULL);
}

#endif
