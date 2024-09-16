#ifndef TEST
#define TEST
#endif
#ifdef TEST

#include "unity.h"
#include "cosem_group.h"
#include "mock_tlt_logger.h"

#include "mock_stub_external.h"
#include "mock_stub_cosem_group.h"

#include "mock_bytebuffer.h"
#include "mock_gxobjects.h"
#include "mock_helpers.h"
#include "mock_converters.h"

void test_cg_read_group_codes_empty_group()
{
	cosem_group g = { 0 };

	int rc	   = 0;
	char *data = "";

	char *expected_data = "{";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);

	expected_data = "{}";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);

	cg_read_group_codes(&g, &rc);
	TEST_ASSERT_EQUAL(1, rc);
}

void test_cg_read_profile_generic_data(void)
{
	cosem_object o = { .entries = 5 };
	physical_device d   = { .connection = (connection[]){ { .socket = 1 } } };
	gxProfileGeneric pg = { .profileEntries = 50 };
	_log_ExpectAnyArgs();
	com_readRowsByEntry_ExpectAndReturn(d.connection, &d.settings, &pg, 45, o.entries + 1, 0);
	TEST_ASSERT_EQUAL_INT(0, cg_read_profile_generic_data(&o, &pg, &d));
}

void test_cg_make_connection_already_connected(void)
{
	physical_device d = { .connection = (connection[]){ { .socket = 1 } } };
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(0, cg_make_connection(&d));
}

void test_cg_make_connection_device_NULL(void)
{
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, cg_make_connection(NULL));
}

void test_cg_make_connection_open_connection_failure(void)
{
	physical_device d = { .connection = (connection[]){ { .socket = -1 } } };

	com_open_connection_ExpectAndReturn(d.connection, 1);
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, cg_make_connection(&d));
}

void test_cg_make_connection_updating_invocation_counter_failure(void)
{
	physical_device d = { .connection	  = (connection[]){ { .socket = -1 } },
			      .invocation_counter = "255.255" };

	com_open_connection_ExpectAndReturn(d.connection, 0);
	com_update_invocation_counter_ExpectAndReturn(d.connection, &d.settings, d.invocation_counter, 1);
	_log_ExpectAnyArgs();
	com_close_IgnoreAndReturn(0);

	TEST_ASSERT_EQUAL_INT(1, cg_make_connection(&d));
}

void test_cg_make_connection_initialisation_failure(void)
{
	physical_device d = { .connection	  = (connection[]){ { .socket = -1 } },
			      .invocation_counter = "255.255" };

	com_open_connection_ExpectAndReturn(d.connection, 0);
	com_update_invocation_counter_ExpectAndReturn(d.connection, &d.settings, d.invocation_counter, 0);
	com_initialize_connection_ExpectAndReturn(d.connection, &d.settings, 1);
	_log_ExpectAnyArgs();
	com_close_IgnoreAndReturn(0);

	TEST_ASSERT_EQUAL_INT(1, cg_make_connection(&d));
}

void test_cg_make_connection_initialisation_successful(void)
{
	physical_device d = { .connection	  = (connection[]){ { .socket = -1 } },
			      .invocation_counter = "255.255" };

	com_open_connection_ExpectAndReturn(d.connection, 0);
	com_update_invocation_counter_ExpectAndReturn(d.connection, &d.settings, d.invocation_counter, 0);
	com_initialize_connection_ExpectAndReturn(d.connection, &d.settings, 0);

	TEST_ASSERT_EQUAL_INT(0, cg_make_connection(&d));
}

#endif
