#ifndef TEST
#define TEST
#endif
#ifdef TEST

#include "unity.h"
#include "main.h"
#include "mock_tlt_logger.h"

#include "mock_stub_external.h"
#include "mock_stub_main.h"

#include "mock_stub_uloop.h"

int mymain(int argc, char **argv);

void test_mymain_fail_to_parse_arguments(void)
{
	char *argv[] = { [0] = "dlms_master" };

	utl_parse_args_ExpectAndReturn(1, argv, NULL, 1);
    utl_parse_args_IgnoreArg_debug_lvl();
	myfprintf_ExpectAnyArgsAndReturn(0);

	TEST_ASSERT_EQUAL_INT(1, mymain(1, argv));
}

static void expect_parse_arg()
{
    utl_parse_args_ExpectAndReturn(1, NULL, NULL, 0);
    utl_parse_args_IgnoreArg_debug_lvl();
}

void test_mymain_fail_to_init_ubus(void)
{
    expect_parse_arg();
    logger_init_ExpectAndReturn(L_EMERG, L_TYPE_STDOUT, "dlms_master", 0);

    init_ubus_test_functions_ExpectAndReturn(1);
    _log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, mymain(1, NULL));
}

void test_mymain_fail_to_init_uloop(void)
{
    expect_parse_arg();
    logger_init_ExpectAndReturn(L_EMERG, L_TYPE_STDOUT, "dlms_master", 0);
    init_ubus_test_functions_ExpectAndReturn(0);

    uloop_init_ExpectAndReturn(1);
    _log_ExpectAnyArgs();

    ubus_exit_Ignore();

	TEST_ASSERT_EQUAL_INT(1, mymain(1, NULL));
}

void test_mymain_fail_to_read_config(void)
{
    expect_parse_arg();
    logger_init_ExpectAndReturn(L_EMERG, L_TYPE_STDOUT, "dlms_master", 0);
    init_ubus_test_functions_ExpectAndReturn(0);
    uloop_init_ExpectAndReturn(0);

    cfg_get_master_ExpectAndReturn(NULL);
    _log_ExpectAnyArgs();

    uloop_run_ExpectAndReturn(0);

    mstr_db_free_ExpectAnyArgs();
    cfg_free_master_ExpectAnyArgs();
    ubus_exit_Ignore();

	TEST_ASSERT_EQUAL_INT(0, mymain(1, NULL));
}

void test_mymain_fail_to_create_db(void)
{
    master g = { 0 };

    expect_parse_arg();
    logger_init_ExpectAndReturn(L_EMERG, L_TYPE_STDOUT, "dlms_master", 0);
    init_ubus_test_functions_ExpectAndReturn(0);
    uloop_init_ExpectAndReturn(0);

    cfg_get_master_ExpectAndReturn(&g);
    mstr_create_db_ExpectAndReturn(&g, 1);
    _log_ExpectAnyArgs();

    cfg_free_master_ExpectAnyArgs();
    ubus_exit_Ignore();

	TEST_ASSERT_EQUAL_INT(1, mymain(1, NULL));
}

void test_mymain_fail_to_initialize_cosem_groups(void)
{
    master g = { 0 };

    expect_parse_arg();
    logger_init_ExpectAndReturn(L_EMERG, L_TYPE_STDOUT, "dlms_master", 0);
    init_ubus_test_functions_ExpectAndReturn(0);
    uloop_init_ExpectAndReturn(0);
    cfg_get_master_ExpectAndReturn(&g);
    mstr_create_db_ExpectAndReturn(&g, 0);
    mstr_initialize_cosem_groups_ExpectAndReturn(&g, 1);
    _log_ExpectAnyArgs();

    mstr_db_free_Expect(&g);
    cfg_free_master_ExpectAnyArgs();
    ubus_exit_Ignore();

	TEST_ASSERT_EQUAL_INT(1, mymain(1, NULL));
}

void test_mymain_fail_to_run_uloop(void)
{
    master g = { 0 };

    expect_parse_arg();
    logger_init_ExpectAndReturn(L_EMERG, L_TYPE_STDOUT, "dlms_master", 0);
    init_ubus_test_functions_ExpectAndReturn(0);
    uloop_init_ExpectAndReturn(0);
    cfg_get_master_ExpectAndReturn(&g);
    mstr_create_db_ExpectAndReturn(&g, 0);
    mstr_initialize_cosem_groups_ExpectAndReturn(&g, 0);
    uloop_run_ExpectAndReturn(1);
    _log_ExpectAnyArgs();

    mstr_db_free_Expect(&g);
    cfg_free_master_ExpectAnyArgs();
    ubus_exit_Ignore();

	TEST_ASSERT_EQUAL_INT(1, mymain(1, NULL));
}

void test_mymain_every_method_return_success(void)
{
    master g = { 0 };

    expect_parse_arg();
    logger_init_ExpectAndReturn(L_EMERG, L_TYPE_STDOUT, "dlms_master", 0);
    init_ubus_test_functions_ExpectAndReturn(0);
    uloop_init_ExpectAndReturn(0);
    cfg_get_master_ExpectAndReturn(&g);
    mstr_create_db_ExpectAndReturn(&g, 0);
    mstr_initialize_cosem_groups_ExpectAndReturn(&g, 0);
    uloop_run_ExpectAndReturn(0);

    mstr_db_free_Expect(&g);
    cfg_free_master_ExpectAnyArgs();
    ubus_exit_Ignore();

	TEST_ASSERT_EQUAL_INT(0, mymain(1, NULL));
}

#endif
