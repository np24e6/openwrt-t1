#ifndef TEST
#define TEST
#endif
#ifdef TEST

#include "unity.h"
#include "utils.h"
#include "mock_tlt_logger.h"

#include "mock_stub_external.h"
#include "mock_stub_utils_2.h"

#include "mock_converters.h"

void test_append_obj_name_should_append_json_name(void)
{
    char *data = "";
    char device_name[64] = { 0 };
    char *expected_data = "\"object_name\":";

    mysnprintf_ExpectAnyArgsAndReturn(1);
    mysnprintf_ReturnMemThruPtr_str(expected_data, sizeof(expected_data));

    utl_append_to_str_ExpectAnyArgs();
    utl_append_to_str_ReturnThruPtr_source(&expected_data);

    utl_append_obj_name(&data, device_name);

    TEST_ASSERT_EQUAL_STRING(data, "\"object_name\":");
}

void test_append_obj_name_should_append_data(void)
{
    char *data = "";
    char *expected_data = ",";

    utl_append_to_str_ExpectAnyArgs();
    utl_append_to_str_ReturnThruPtr_source(&expected_data);
    utl_append_if_needed(&data, 1, 0, ",");

    TEST_ASSERT_EQUAL_STRING(data, ",");
}

void test_append_obj_name_should_not_append_data(void)
{
    char *data = "";

    utl_append_if_needed(&data, 1, 1, ",");

    TEST_ASSERT_EQUAL_STRING(data, "");
}

void test_utl_debug_print_do_nothing_if_master_is_null(void)
{
    _log_ExpectAnyArgs();
    utl_debug_master(NULL);
}

void test_utl_print_debug_physical_devices_without_connection(void)
{
    physical_device **ptr1 = (physical_device *[]){
	    (physical_device[]){
		    { .id = 4, .name = "device_one", .invocation_counter = "101.25.25.25.0.255" },
	    },
	    (physical_device[]){
		    { .id = 5, .name = "device_two", .invocation_counter = "102.26.26.25.0.255" },
	    }
    };

    master master = { .physical_dev_count = 2, .physical_devices = ptr1 };

    utl_debug_master(&master);
}

void test_utl_print_debug_physical_devices_with_tcp_connection(void)
{
    physical_device **dev = (physical_device *[]){
	    (physical_device[]){
		    {
			    .id			= 4,
			    .name		= "device_one",
			    .invocation_counter = "101.25.25.25.0.255",
		    },
	    },
	    (physical_device[]){
		    {
			    .id			= 5,
			    .name		= "device_two",
			    .invocation_counter = "102.26.26.25.0.255",
			    .connection =
				    (connection[]){
					    { .id		   = 5,
					      .type		   = TCP,
					      .parameters.tcp.host = "0.0.0.0",
					      .parameters.tcp.port = 42168,
					      .socket		   = 7,
					      .wait_time	   = 99 },
				    },
		    },
	    },
    };

    master master = { .physical_dev_count = 2, .physical_devices = dev };

    utl_debug_master(&master);
}

void test_utl_print_debug_physical_devices_with_serial_connection(void)
{
    physical_device **dev = (physical_device *[]){
	    (physical_device[]){
		    {
			    .id			= 4,
			    .name		= "device_one",
			    .invocation_counter = "101.25.25.25.0.255",
		    },
	    },
	    (physical_device[]){
		    {
			    .id			= 5,
			    .name		= "device_two",
			    .invocation_counter = "102.26.26.25.0.255",
			    .connection =
				    (connection[]){
					    { .id			= 4,
					      .type			= SERIAL,
					      .parameters.serial.device = "/dev/rs485",
					      .parameters.serial.parity = "None",
					      .socket			= 7,
					      .wait_time		= 99 },
				    },
		    },
	    },
    };

    master master = { .physical_dev_count = 2, .physical_devices = dev };

    utl_debug_master(&master);
}

void test_utl_print_debug_physical_devices_with_serial_without__stty_settings(void)
{
    physical_device **dev = (physical_device *[]){
	    (physical_device[]){
		    {
			    .id			= 4,
			    .invocation_counter = "101.25.25.25.0.255",
		    },
	    },
	    (physical_device[]){
		    {
			    .id			= 5,
			    .invocation_counter = "102.26.26.25.0.255",
			    .connection =
				    (connection[]){
					    { .id			= 4,
					      .type			= SERIAL,
					      .socket			= 7,
					      .wait_time		= 99 },
				    },
		    },
	    },
    };

    master master = { .physical_dev_count = 2, .physical_devices = dev };

    utl_debug_master(&master);
}

void test_utl_print_debug_cosem_groups(void)
{
    physical_device **dev = (physical_device *[]){
	    (physical_device[]){
		    { .id = 4, .name = "device_one" },
	    },
    };

    cosem_object **ptr1 = (cosem_object *[]){
	    (cosem_object[]){
		    { .id = 88, .name = "object_one", .entries = 0 },
	    },
	    (cosem_object[]){
		    {
			    .id		  = 89,
			    .name	  = "object_two",
			    .entries	  = 15,
			    .device_count = 1,
			    .devices	  = dev,
			    .object =
				    (gxObject[]){
					    { .objectType = DLMS_OBJECT_TYPE_PROFILE_GENERIC },
				    },
		    },
	    },
    };

    cosem_group group[] = {
	    [0] = { .id			= 3,
		    .name		= "group_one",
		    .interval		= 4,
		    .cosem_object_count = 2,
		    .cosem_objects	= ptr1 },
	    [1] = { .id			= 7,
		    .name		= "group_two",
		    .interval		= 8,
		    .cosem_object_count = 2,
		    .cosem_objects	= ptr1 },
    };

    cosem_group **ptr2 = (cosem_group *[]){
	    (cosem_group[]){
		    { .id		  = 3,
		      .name		  = "group_one",
		      .interval		  = 4,
		      .cosem_object_count = 2,
		      .cosem_objects	  = ptr1 },
	    },
	    (cosem_group[]){
		    { .id		  = 7,
		      .name		  = "group_two",
		      .interval		  = 8,
		      .cosem_object_count = 2,
		      .cosem_objects	  = ptr1 },
	    },
    };

    master master = { .cosem_group_count = 2, .cosem_groups = ptr2 };
    obj_typeToString2_ExpectAndReturn(ptr1[1]->object->objectType, "Profile Generic");
    obj_typeToString2_ExpectAndReturn(ptr1[1]->object->objectType, "Profile Generic");
    utl_debug_master(&master);
}

void test_utl_print_debug_cosem_groups_without_name(void)
{
    physical_device **dev = (physical_device *[]){
	    (physical_device[]){
		    { .id = 4 },
	    },
    };

    cosem_object **ptr1 = (cosem_object *[]){
	    (cosem_object[]){
		    { .id = 88, .entries = 0 },
	    },
	    (cosem_object[]){
		    {
			    .id		  = 89,
			    .entries	  = 15,
			    .device_count = 1,
			    .devices	  = dev,
			    .object =
				    (gxObject[]){
					    { .objectType = DLMS_OBJECT_TYPE_PROFILE_GENERIC },
				    },
		    },
	    },
    };

    cosem_group group[] = {
	    [0] = { .id			= 3,
		    .interval		= 4,
		    .cosem_object_count = 2,
		    .cosem_objects	= ptr1 },
	    [1] = { .id			= 7,
		    .interval		= 8,
		    .cosem_object_count = 2,
		    .cosem_objects	= ptr1 },
    };

    cosem_group **ptr2 = (cosem_group *[]){
	    (cosem_group[]){
		    { .id		  = 3,
		      .interval		  = 4,
		      .cosem_object_count = 2,
		      .cosem_objects	  = ptr1 },
	    },
	    (cosem_group[]){
		    { .id		  = 7,
		      .interval		  = 8,
		      .cosem_object_count = 2,
		      .cosem_objects	  = ptr1 },
	    },
    };

    master master = { .cosem_group_count = 2, .cosem_groups = ptr2 };
    obj_typeToString2_ExpectAndReturn(ptr1[1]->object->objectType, "Profile Generic");
    obj_typeToString2_ExpectAndReturn(ptr1[1]->object->objectType, "Profile Generic");
    utl_debug_master(&master);
}

#endif
