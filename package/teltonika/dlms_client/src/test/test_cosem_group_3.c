#ifndef TEST
#define TEST
#endif
#ifdef TEST

#include "unity.h"
#include "cosem_group.h"
#include "mock_tlt_logger.h"

#include "mock_stub_external.h"
#include "mock_stub_cosem_group_3.h"

#include "mock_bytebuffer.h"
#include "mock_gxobjects.h"
#include "mock_helpers.h"
#include "mock_converters.h"

void test_cg_read_group_codes_device_is_disabled(void)
{
	cosem_group g = {
		.id		    = 3,
		.name		    = "group_one",
		.cosem_object_count = 1,
		.cosem_objects =
			(cosem_object *[]){
				(cosem_object[]){
					{ .id		= 4,
					  .device_count = 1,
					  .name		= "device_one",
					  .devices =
						  (physical_device *[]){
							  (physical_device[]){
								  { .id		= 6,
								    .enabled	= 0,
								    .connection = (connection[]){ { 0 } } } },
						  } } },
			},
	};

	int rc = 0;

	char *expected_data = "{";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);

	expected_data = "{\"device_one\": ";
	utl_append_obj_name_ExpectAnyArgs();
	utl_append_obj_name_ReturnMemThruPtr_data(&expected_data, sizeof(expected_data));

	expected_data = "{\"device_one\": {";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);

	utl_lock_mutex_if_required_Expect(g.cosem_objects[0]->devices[0]);

	utl_add_error_message_ExpectAnyArgs();
	_log_ExpectAnyArgs();

	utl_unlock_mutex_if_required_Expect(g.cosem_objects[0]->devices[0]);

	utl_append_if_needed_ExpectAnyArgs();
	attr_free_ExpectAnyArgs();

	expected_data = "{\"device_one\": {}";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);
	utl_append_if_needed_ExpectAnyArgs();

	expected_data = "{\"device_one\": {}}";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);

	TEST_ASSERT_NOT_NULL(cg_read_group_codes(&g, &rc));
	TEST_ASSERT_EQUAL(1, rc);
}

void test_cg_read_group_codes_fail_to_make_connection(void)
{
	cosem_group g = {
		.id		    = 3,
		.name		    = "group_one",
		.cosem_object_count = 1,
		.cosem_objects =
			(cosem_object *[]){
				(cosem_object[]){
					{ .id		= 4,
					  .device_count = 1,
					  .name		= "device_one",
					  .devices =
						  (physical_device *[]){
							  (physical_device[]){
								  { .id		= 6,
								    .enabled	= 1,
								    .connection = (connection[]){ { 0 } } } },
						  } } },
			},
	};

	int rc = 0;

	char *expected_data = "{";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);

	expected_data = "{\"device_one\": ";
	utl_append_obj_name_ExpectAnyArgs();
	utl_append_obj_name_ReturnMemThruPtr_data(&expected_data, sizeof(expected_data));

	expected_data = "{\"device_one\": {";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);

	utl_lock_mutex_if_required_Expect(g.cosem_objects[0]->devices[0]);

	cg_make_connection_ExpectAndReturn(g.cosem_objects[0]->devices[0], 1);
	utl_add_error_message_ExpectAnyArgs();
	_log_ExpectAnyArgs();

	utl_unlock_mutex_if_required_Expect(g.cosem_objects[0]->devices[0]);

	utl_append_if_needed_ExpectAnyArgs();
	attr_free_ExpectAnyArgs();

	expected_data = "{\"device_one\": {}";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);
	utl_append_if_needed_ExpectAnyArgs();

	expected_data = "{\"device_one\": {}}";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);

	TEST_ASSERT_NOT_NULL(cg_read_group_codes(&g, &rc));
	TEST_ASSERT_EQUAL(1, rc);
}

void test_cg_read_group_codes_fail_to_read_cosem_object(void)
{
	cosem_group g = {
		.id		    = 3,
		.name		    = "group_one",
		.cosem_object_count = 1,
		.cosem_objects =
			(cosem_object *[]){
				(cosem_object[]){
					{ .id		= 4,
					  .device_count = 1,
					  .name		= "device_one",
					  .devices =
						  (physical_device *[]){
							  (physical_device[]){
								  { .id		= 6,
								    .enabled	= 1,
								    .connection = (connection[]){ { 0 } } } },
						  } } },
			},
	};

	int rc = 0;

	char *expected_data = "{";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);

	expected_data = "{\"device_one\": ";
	utl_append_obj_name_ExpectAnyArgs();
	utl_append_obj_name_ReturnMemThruPtr_data(&expected_data, sizeof(expected_data));

	expected_data = "{\"device_one\": {";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);

	utl_lock_mutex_if_required_Expect(g.cosem_objects[0]->devices[0]);

	cg_make_connection_ExpectAndReturn(g.cosem_objects[0]->devices[0], 0);

	cg_read_cosem_object_ExpectAndReturn(g.cosem_objects[0], g.cosem_objects[0]->devices[0], 1);
	hlp_getErrorMessage_ExpectAnyArgsAndReturn("COSEM object error");
	utl_add_error_message_ExpectAnyArgs();
	_log_ExpectAnyArgs();


	com_close_ExpectAnyArgsAndReturn(0);
	utl_unlock_mutex_if_required_Expect(g.cosem_objects[0]->devices[0]);
	utl_append_if_needed_ExpectAnyArgs();
	attr_free_ExpectAnyArgs();

	expected_data = "{\"device_one\": {}";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);
	utl_append_if_needed_ExpectAnyArgs();

	expected_data = "{\"device_one\": {}}";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);

	TEST_ASSERT_NOT_NULL(cg_read_group_codes(&g, &rc));
	TEST_ASSERT_EQUAL(1, rc);
}

void test_cg_read_group_codes_fail_to_convert_attributes_to_string(void)
{
	cosem_group g = {
		.id		    = 3,
		.name		    = "group_one",
		.cosem_object_count = 1,
		.cosem_objects =
			(cosem_object *[]){
				(cosem_object[]){
					{ .id		= 4,
					  .device_count = 1,
					  .name		= "device_one",
					  .devices =
						  (physical_device *[]){
							  (physical_device[]){
								  { .id		= 6,
								    .enabled	= 1,
								    .connection = (connection[]){ { 0 } } } },
						  } } },
			},
	};

	int rc = 0;

	char *expected_data = "{";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);

	expected_data = "{\"device_one\": ";
	utl_append_obj_name_ExpectAnyArgs();
	utl_append_obj_name_ReturnMemThruPtr_data(&expected_data, sizeof(expected_data));

	expected_data = "{\"device_one\": {";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);

	utl_lock_mutex_if_required_Expect(g.cosem_objects[0]->devices[0]);

	cg_make_connection_ExpectAndReturn(g.cosem_objects[0]->devices[0], 0);

	cg_read_cosem_object_ExpectAndReturn(g.cosem_objects[0], g.cosem_objects[0]->devices[0], 0);

	attr_init_ExpectAnyArgs();
	attr_to_string_ExpectAnyArgsAndReturn(1);
	utl_add_error_message_ExpectAnyArgs();
	_log_ExpectAnyArgs();


	com_close_ExpectAnyArgsAndReturn(0);
	utl_unlock_mutex_if_required_Expect(g.cosem_objects[0]->devices[0]);
	utl_append_if_needed_ExpectAnyArgs();
	attr_free_ExpectAnyArgs();

	expected_data = "{\"device_one\": {}";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);
	utl_append_if_needed_ExpectAnyArgs();

	expected_data = "{\"device_one\": {}}";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);

	TEST_ASSERT_NOT_NULL(cg_read_group_codes(&g, &rc));
	TEST_ASSERT_EQUAL(1, rc);
}

void test_cg_read_group_codes_fail_to_format_group_data(void)
{
	cosem_group g = {
		.id		    = 3,
		.name		    = "group_one",
		.cosem_object_count = 1,
		.cosem_objects =
			(cosem_object *[]){
				(cosem_object[]){
					{ .id		= 4,
					  .device_count = 1,
					  .name		= "device_one",
					  .devices =
						  (physical_device *[]){
							  (physical_device[]){
								  { .id		= 6,
								    .enabled	= 1,
								    .connection = (connection[]){ { 0 } } } },
						  } } },
			},
	};

	int rc = 0;

	char *expected_data = "{";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);

	expected_data = "{\"device_one\": ";
	utl_append_obj_name_ExpectAnyArgs();
	utl_append_obj_name_ReturnMemThruPtr_data(&expected_data, sizeof(expected_data));

	expected_data = "{\"device_one\": {";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);

	utl_lock_mutex_if_required_Expect(g.cosem_objects[0]->devices[0]);

	cg_make_connection_ExpectAndReturn(g.cosem_objects[0]->devices[0], 0);

	cg_read_cosem_object_ExpectAndReturn(g.cosem_objects[0], g.cosem_objects[0]->devices[0], 0);

	attr_init_ExpectAnyArgs();
	attr_to_string_ExpectAnyArgsAndReturn(0);

	cg_format_group_data_ExpectAnyArgsAndReturn(1);
	utl_add_error_message_ExpectAnyArgs();
	_log_ExpectAnyArgs();


	com_close_ExpectAnyArgsAndReturn(0);
	utl_unlock_mutex_if_required_Expect(g.cosem_objects[0]->devices[0]);
	utl_append_if_needed_ExpectAnyArgs();
	attr_free_ExpectAnyArgs();

	expected_data = "{\"device_one\": {}";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);
	utl_append_if_needed_ExpectAnyArgs();

	expected_data = "{\"device_one\": {}}";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);

	TEST_ASSERT_NOT_NULL(cg_read_group_codes(&g, &rc));
	TEST_ASSERT_EQUAL(1, rc);
}

void test_cg_read_group_codes_get_successful_message(void)
{
	cosem_group g = {
		.id		    = 3,
		.name		    = "group_one",
		.cosem_object_count = 1,
		.cosem_objects =
			(cosem_object *[]){
				(cosem_object[]){
					{ .id		= 4,
					  .device_count = 1,
					  .name		= "device_one",
					  .devices =
						  (physical_device *[]){
							  (physical_device[]){
								  { .id		= 6,
								    .enabled	= 1,
								    .connection = (connection[]){ { 0 } } } },
						  } } },
			},
	};

	int rc = 0;

	char *expected_data = "{";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);

	expected_data = "{\"device_one\": ";
	utl_append_obj_name_ExpectAnyArgs();
	utl_append_obj_name_ReturnMemThruPtr_data(&expected_data, sizeof(expected_data));

	expected_data = "{\"device_one\": {";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);

	utl_lock_mutex_if_required_Expect(g.cosem_objects[0]->devices[0]);

	cg_make_connection_ExpectAndReturn(g.cosem_objects[0]->devices[0], 0);

	cg_read_cosem_object_ExpectAndReturn(g.cosem_objects[0], g.cosem_objects[0]->devices[0], 0);

	attr_init_ExpectAnyArgs();
	attr_to_string_ExpectAnyArgsAndReturn(0);

	cg_format_group_data_ExpectAnyArgsAndReturn(0);

	com_close_ExpectAnyArgsAndReturn(0);

	utl_unlock_mutex_if_required_Expect(g.cosem_objects[0]->devices[0]);

	utl_append_if_needed_ExpectAnyArgs();
	attr_free_ExpectAnyArgs();

	expected_data = "{\"device_one\": {}";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);
	utl_append_if_needed_ExpectAnyArgs();

	expected_data = "{\"device_one\": {}}";
	utl_append_to_str_ExpectAnyArgs();
	utl_append_to_str_ReturnThruPtr_source(&expected_data);

	TEST_ASSERT_NOT_NULL(cg_read_group_codes(&g, &rc));
	TEST_ASSERT_EQUAL(0, rc);
}

#endif