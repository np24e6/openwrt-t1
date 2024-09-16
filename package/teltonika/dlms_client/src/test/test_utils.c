#ifndef TEST
#define TEST
#endif
#ifdef TEST

#include "unity.h"
#include "utils.h"
#include "mock_tlt_logger.h"

#include "mock_stub_external.h"

#include "mock_converters.h"

#undef calloc
#undef free
#undef strlen

static long mystrtol_cb_1(const char *nptr, char **endptr, int base, int call_count)
{
	errno = 1;
	return 0;
}

static long mystrtol_cb_2(const char *nptr, char **endptr, int base, int call_count)
{
	errno = 0;
	return 0;
}

void test_utl_parse_args_zero_arguments(void)
{
	TEST_ASSERT_EQUAL(0, utl_parse_args(0, NULL, NULL));
}

void test_utl_parse_args_argc_is_not_three(void)
{
	char *argv[] = { "dlms_master", "-D", "4", "too much" };

	myfprintf_ExpectAnyArgsAndReturn(0);

	TEST_ASSERT_EQUAL(1, utl_parse_args(4, argv, NULL));
}

void test_utl_parse_args_argv_second_parameter_is_not_valid(void)
{
	char *argv[] = { "dlms_master", "-E", "4" };

	mystrcmp_ExpectAndReturn(argv[1], "-D", 1);
	myfprintf_ExpectAnyArgsAndReturn(0);

	TEST_ASSERT_EQUAL(1, utl_parse_args(3, argv, NULL));
}

void test_utl_parse_args_invalid_debug_parameter(void)
{
	char *argv[] = { "dlms_master", "-D", "labas" };
	log_level_type dbg = 0;

	mystrcmp_ExpectAndReturn(argv[1], "-D", 0);
	mystrtol_AddCallback(mystrtol_cb_1);
	mystrtol_ExpectAndReturn(argv[2], NULL, 10, 0);
	mystrtol_IgnoreArg_endptr();
	myfprintf_ExpectAnyArgsAndReturn(0);

	TEST_ASSERT_EQUAL(1, utl_parse_args(3, argv, &dbg));
}

void test_utl_parse_args_endptr_not_null(void)
{
	char *argv[] = { "dlms_master", "-D", "4asd" };
	log_level_type dbg = 0;

	mystrcmp_ExpectAndReturn(argv[1], "-D", 0);
	mystrtol_AddCallback(mystrtol_cb_2);
	mystrtol_ExpectAndReturn(argv[2], NULL, 10, 4);
	mystrtol_ReturnThruPtr_endptr((char *[]){"asd"});
	mystrtol_IgnoreArg_endptr();
	myfprintf_ExpectAnyArgsAndReturn(0);

	TEST_ASSERT_EQUAL(1, utl_parse_args(3, argv, &dbg));
}

void test_utl_parse_args_successful(void)
{
	char *argv[] = { "dlms_master", "-D", "4" };
	log_level_type dbg = 0;

	mystrcmp_ExpectAndReturn(argv[1], "-D", 0);
	mystrtol_AddCallback(mystrtol_cb_2);
	mystrtol_ExpectAndReturn(argv[2], NULL, 10, 4);
	mystrtol_ReturnThruPtr_endptr((char *[]){""});
	mystrtol_IgnoreArg_endptr();

	TEST_ASSERT_EQUAL(0, utl_parse_args(3, argv, &dbg));
}

void test_append_if_destination_null_should_copy_target(void)
{
	char *destination = NULL;
	char *target = "NOT NULL";

	mystrdup_ExpectAndReturn(target, target);
	utl_append_to_str(&destination, target);

	TEST_ASSERT_NOT_NULL(destination);
}

void test_append_target_if_source_null_do_nothing(void)
{
	char *source = NULL;
	char *destination = "NOT NULL";

	_log_ExpectAnyArgs();
	utl_append_to_str(&destination, source);

	TEST_ASSERT_NULL(source);
	TEST_ASSERT_NOT_NULL(destination);
}

void test_append_if_realloc_succeed_should_append_string(void)
{
	char *destination = calloc(10, sizeof(char));
	char *source = "appending";
	char *result = "appending";

	mystrlen_ExpectAndReturn(source, strlen(source));
	mystrlen_ExpectAndReturn(destination, strlen(destination));
	myrealloc_ExpectAndReturn(destination, strlen(destination) + strlen(source) + 1, destination);
	mystrncat_ExpectAndReturn(destination, result, 9, result);
	mystrncat_ReturnMemThruPtr_dest(result, strlen(result));

	utl_append_to_str(&destination, source);
	TEST_ASSERT_EQUAL_STRING(destination, "appending");
	free(destination);
}

void test_append_if_realloc_fail_should_exit(void)
{
	char *destination = calloc(1, sizeof(char));
	char *source = "appending";

	mystrlen_ExpectAndReturn(source, 9);
	mystrlen_ExpectAndReturn(destination, 0);
	myrealloc_ExpectAndReturn(destination, 10, NULL);
	_log_ExpectAnyArgs();
	utl_append_to_str(&destination, source);
	TEST_ASSERT_NULL(destination);
	free(destination);
}

void test_utl_smart_sleep_case1(void)
{
	int period	 = 2;
	time_t t0	 = time(NULL);
	unsigned long tn = 0;

	utl_smart_sleep(&t0, &tn, period);
	TEST_ASSERT_EQUAL(period, time(NULL) - t0);
	TEST_ASSERT_EQUAL(1, tn);
}

void test_utl_smart_sleep_case2(void)
{
	time_t t0	 = time(NULL);
	unsigned long tn = 2;

	utl_smart_sleep(&t0, &tn, 1);
	TEST_ASSERT_EQUAL(0, tn);
}

void test_utl_lock_mutex_if_required_mutex_is_NULL(void)
{
	physical_device d = { .connection = (connection[]){ {
				      0,
			      } } };

	utl_lock_mutex_if_required(&d);
}

void test_utl_lock_mutex_if_required_mutex_is_NOT_NULL(void)
{
	physical_device d = { .connection = (connection[]){ { .mutex = (pthread_mutex_t *)"NOT NULL" } } };

	mypthread_mutex_lock_ExpectAndReturn(d.connection->mutex, 0);

	utl_lock_mutex_if_required(&d);
}

void test_utl_unlock_mutex_if_required_mutex_is_NULL(void)
{
	physical_device d = { .connection = (connection[]){ {
				      0,
			      } } };

	utl_unlock_mutex_if_required(&d);
}

void test_utl_unlock_mutex_if_required_mutex_is_NOT_NULL(void)
{
	physical_device d = { .connection = (connection[]){ { .mutex = (pthread_mutex_t *)"NOT NULL" } } };

	mypthread_mutex_unlock_ExpectAndReturn(d.connection->mutex, 0);

	utl_unlock_mutex_if_required(&d);
}

#endif
