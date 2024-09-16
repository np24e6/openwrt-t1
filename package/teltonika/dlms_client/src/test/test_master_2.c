#ifndef TEST
#define TEST
#endif
#ifdef TEST

#include "unity.h"
#include "master.h"
#include "mock_tlt_logger.h"

#include "mock_stub_external.h"
#include "mock_stub_master_2.h"

PRIVATE void *mstr_thread_routine(void *p);

master *g_master = NULL;

void setUp(void)
{
	static master m = { 0 };
	g_master = &m;
}

void test_mstr_thread_routine_returned_data_is_NULL()
{
	cosem_group g = { .name = "group_one" };
	int rc = 0;
	char *data = NULL;

	cg_read_group_codes_ExpectAndReturn(&g, &rc, data);
	_log_ExpectAnyArgs();

	myfree_Expect(data);
	utl_smart_sleep_Ignore();

	mstr_thread_routine(&g);
}

void test_mstr_thread_routine_rc_is_not_zero()
{
	cosem_group g = { .name = "group_one" };
	char *data = "data";
	int rc = 0;
	int rc_returned = 1;

	cg_read_group_codes_ExpectAndReturn(&g, &rc, data);
	cg_read_group_codes_ReturnThruPtr_rc(&rc_returned);

	_log_ExpectAnyArgs();
	myfree_Expect(data);
	utl_smart_sleep_Ignore();

	mstr_thread_routine(&g);
}

void test_mstr_thread_routine_rc_is_not_zero_without_name()
{
	cosem_group g = { 0 };
	char *data = "data";
	int rc = 0;
	int rc_returned = 1;

	cg_read_group_codes_ExpectAndReturn(&g, &rc, data);
	cg_read_group_codes_ReturnThruPtr_rc(&rc_returned);

	_log_ExpectAnyArgs();
	myfree_Expect(data);
	utl_smart_sleep_Ignore();

	mstr_thread_routine(&g);
}

void test_mstr_thread_routine_rc_success()
{
	cosem_group g = { .name = "group_one" };
	char *data = "data";
	int rc = 0;

	cg_read_group_codes_ExpectAndReturn(&g, &rc, data);
        mstr_write_group_data_to_db_Expect(&g, data);

	myfree_Expect(data);
	utl_smart_sleep_Ignore();

	mstr_thread_routine(&g);
}

#endif
