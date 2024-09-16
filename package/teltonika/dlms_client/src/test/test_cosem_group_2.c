#ifndef TEST
#define TEST
#endif
#ifdef TEST

#include "unity.h"
#include "cosem_group.h"
#include "mock_tlt_logger.h"

#include "mock_stub_external.h"
#include "mock_stub_cosem_group_2.h"

#include "mock_bytebuffer.h"
#include "mock_gxobjects.h"
#include "mock_helpers.h"
#include "mock_converters.h"

#undef strdup
#undef free

void test_cg_format_group_data(void)
{
	char *data = strdup("data");

	attr_to_json_ExpectAndReturn(NULL, "{ \"data\" :");
	utl_append_obj_name_Expect(&data, "device_one");
	utl_append_to_str_Expect(&data, "{");
	utl_append_to_str_Expect(&data, "{ \"data\" :");
	utl_append_to_str_Expect(&data, "}");
	myfree_Ignore();

	TEST_ASSERT_EQUAL_INT(0, cg_format_group_data(&data, NULL, "device_one"));
	free(data);
}

void test_cg_format_group_data_return_attribute_NULL(void)
{
	char *data = strdup("data");

	attr_to_json_ExpectAndReturn(NULL, NULL);
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, cg_format_group_data(&data, NULL, "device_one"));
	free(data);
}

void test_cg_read_cosem_object_attribute_index_failure(void)
{
	cosem_object o = {
		.device_count = 1,
		.devices =
			(physical_device *[]){
				(physical_device[]){
					{
						.id = 6,
						.connection =
							(connection[]){
								{ .socket = 1 },
							},
					},
				},
			},
		.object =
			(gxObject[]){
				{ .logicalName = "0668", .objectType = DLMS_OBJECT_TYPE_PROFILE_GENERIC },
			},
	};

	gxByteBuffer attributes = { 0 };

	hlp_getLogicalNameToString_ExpectAndReturn(o.object->logicalName, NULL, 0);
	hlp_getLogicalNameToString_IgnoreArg_ln();
	obj_typeToString2_ExpectAndReturn(o.object->objectType, "Register");
	_log_ExpectAnyArgs();

	bb_init_IgnoreAndReturn(0);
	obj_getAttributeIndexToRead_ExpectAndReturn(o.object, &attributes, 1);
	hlp_getErrorMessage_ExpectAndReturn(1, "Error");
	_log_ExpectAnyArgs();

	bb_clear_ExpectAndReturn(&attributes, 0);

	TEST_ASSERT_EQUAL_INT(1, cg_read_cosem_object(&o, o.devices[0]));
}

void test_cg_read_cosem_object_bb_getuint8byindex_failure(void)
{
	cosem_object o = {
		.device_count = 1,
		.devices =
			(physical_device *[]){
				(physical_device[]){
					{
						.id = 6,
						.connection =
							(connection[]){
								{ .socket = 1 },
							},
					},
				},
			},
		.object =
			(gxObject[]){
				{ .logicalName = "50", .objectType = DLMS_OBJECT_TYPE_PROFILE_GENERIC },
			},
	};

	gxByteBuffer attributes = { .size = 2 };

	hlp_getLogicalNameToString_ExpectAndReturn(o.object->logicalName, NULL, 0);
	hlp_getLogicalNameToString_IgnoreArg_ln();
	obj_typeToString2_ExpectAndReturn(o.object->objectType, "Register");
	_log_ExpectAnyArgs();

	bb_init_IgnoreAndReturn(0);
	obj_getAttributeIndexToRead_ExpectAnyArgsAndReturn(0);
	obj_getAttributeIndexToRead_ReturnThruPtr_ba(&attributes);

	//entering loop here
	bb_getUInt8ByIndex_ExpectAnyArgsAndReturn(1);
	_log_ExpectAnyArgs();

	bb_clear_IgnoreAndReturn(0);

	TEST_ASSERT_EQUAL_INT(1, cg_read_cosem_object(&o, o.devices[0]));
}

void test_cg_read_cosem_object_com_read_failure(void)
{
	cosem_object o = {
		.device_count = 1,
		.devices =
			(physical_device *[]){
				(physical_device[]){
					{
						.id = 6,
						.connection =
							(connection[]){
								{ .socket = 1 },
							},
					},
				},
			},
		.object =
			(gxObject[]){
				{ .logicalName = "50", .objectType = 4 },
			},
	};

	gxByteBuffer attributes = { .size = 2 };

	hlp_getLogicalNameToString_ExpectAndReturn(o.object->logicalName, NULL, 0);
	hlp_getLogicalNameToString_IgnoreArg_ln();
	obj_typeToString2_ExpectAndReturn(o.object->objectType, "Register");
	_log_ExpectAnyArgs();

	bb_init_IgnoreAndReturn(0);
	obj_getAttributeIndexToRead_ExpectAnyArgsAndReturn(0);
	obj_getAttributeIndexToRead_ReturnThruPtr_ba(&attributes);

	//entering loop here
	bb_getUInt8ByIndex_ExpectAnyArgsAndReturn(0);
	_log_ExpectAnyArgs();
	com_read_ExpectAnyArgsAndReturn(1);
	_log_ExpectAnyArgs();

	bb_clear_IgnoreAndReturn(0);

	TEST_ASSERT_EQUAL_INT(1, cg_read_cosem_object(&o, o.devices[0]));
}

void test_cg_read_cosem_object_com_read_failure_with_permission_error(void)
{
	cosem_object o = {
		.device_count = 1,
		.devices =
			(physical_device *[]){
				(physical_device[]){
					{
						.id = 6,
						.connection =
							(connection[]){
								{ .socket = 1 },
							},
					},
				},
			},
		.object =
			(gxObject[]){
				{ .logicalName = "50", .objectType = 4 },
			},
	};

	gxByteBuffer attributes = { .size = 2 };

	hlp_getLogicalNameToString_ExpectAndReturn(o.object->logicalName, NULL, 0);
	hlp_getLogicalNameToString_IgnoreArg_ln();
	obj_typeToString2_ExpectAndReturn(o.object->objectType, "Register");
	_log_ExpectAnyArgs();

	bb_init_IgnoreAndReturn(0);
	obj_getAttributeIndexToRead_ExpectAnyArgsAndReturn(0);
	obj_getAttributeIndexToRead_ReturnThruPtr_ba(&attributes);

	//entering loop here
	bb_getUInt8ByIndex_ExpectAnyArgsAndReturn(0);
	_log_ExpectAnyArgs();
	com_read_ExpectAnyArgsAndReturn(DLMS_ERROR_CODE_READ_WRITE_DENIED);
	_log_ExpectAnyArgs();

	bb_getUInt8ByIndex_ExpectAnyArgsAndReturn(0);
	_log_ExpectAnyArgs();
	com_read_ExpectAnyArgsAndReturn(1);
	_log_ExpectAnyArgs();

	bb_clear_IgnoreAndReturn(0);

	TEST_ASSERT_EQUAL_INT(1, cg_read_cosem_object(&o, o.devices[0]));
}

void test_cg_read_cosem_object_read_profile_generic_failure(void)
{
	cosem_object o = {
		.device_count = 1,
		.devices =
			(physical_device *[]){
				(physical_device[]){
					{
						.id = 6,
						.connection =
							(connection[]){
								{ .socket = 1 },
							},
					},
				},
			},
		.object =
			(gxObject[]){
				{ .logicalName = "50", .objectType = DLMS_OBJECT_TYPE_PROFILE_GENERIC },
			},
	};

	gxByteBuffer attributes = { .size = 2 };

	hlp_getLogicalNameToString_ExpectAndReturn(o.object->logicalName, NULL, 0);
	hlp_getLogicalNameToString_IgnoreArg_ln();
	obj_typeToString2_ExpectAndReturn(o.object->objectType, "Register");
	_log_ExpectAnyArgs();

	bb_init_IgnoreAndReturn(0);
	obj_getAttributeIndexToRead_ExpectAnyArgsAndReturn(0);
	obj_getAttributeIndexToRead_ReturnThruPtr_ba(&attributes);

	//entering loop here
	bb_getUInt8ByIndex_ExpectAnyArgsAndReturn(0);
	_log_ExpectAnyArgs();
	com_read_ExpectAnyArgsAndReturn(0);

	bb_getUInt8ByIndex_ExpectAnyArgsAndReturn(0);
	bb_getUInt8ByIndex_ReturnThruPtr_value((unsigned char[]){ 2 });
	_log_ExpectAnyArgs();
	_log_ExpectAnyArgs();
	// out of loop

	cg_read_profile_generic_data_ExpectAndReturn(&o, (gxProfileGeneric *)o.object, o.devices[0], 1);
	_log_ExpectAnyArgs();

	bb_clear_IgnoreAndReturn(0);

	TEST_ASSERT_EQUAL_INT(1, cg_read_cosem_object(&o, o.devices[0]));
}

void test_cg_read_cosem_object_succesful_profile_generic_tests(void)
{
	cosem_object o = {
		.device_count = 1,
		.devices =
			(physical_device *[]){
				(physical_device[]){
					{
						.id = 6,
						.connection =
							(connection[]){
								{ .socket = 1 },
							},
					},
				},
			},
		.object =
			(gxObject[]){
				{ .logicalName = "50", .objectType = DLMS_OBJECT_TYPE_PROFILE_GENERIC },
			},
	};

	gxByteBuffer attributes = { .size = 2 };

	hlp_getLogicalNameToString_ExpectAndReturn(o.object->logicalName, NULL, 0);
	hlp_getLogicalNameToString_IgnoreArg_ln();
	obj_typeToString2_ExpectAndReturn(o.object->objectType, "Register");
	_log_ExpectAnyArgs();

	bb_init_IgnoreAndReturn(0);
	obj_getAttributeIndexToRead_ExpectAnyArgsAndReturn(0);
	obj_getAttributeIndexToRead_ReturnThruPtr_ba(&attributes);

	//entering loop here
	bb_getUInt8ByIndex_ExpectAnyArgsAndReturn(0);
	_log_ExpectAnyArgs();
	com_read_ExpectAnyArgsAndReturn(0);

	bb_getUInt8ByIndex_ExpectAnyArgsAndReturn(0);
	bb_getUInt8ByIndex_ReturnThruPtr_value((unsigned char[]){ 2 });
	_log_ExpectAnyArgs();
	_log_ExpectAnyArgs();
	// out of loop

	cg_read_profile_generic_data_ExpectAndReturn(&o, (gxProfileGeneric *)o.object, o.devices[0], 0);

	bb_clear_IgnoreAndReturn(0);

	TEST_ASSERT_EQUAL_INT(0, cg_read_cosem_object(&o, o.devices[0]));
}

void test_cg_read_cosem_object_succesful_non_profile_generic_tests(void)
{
	cosem_object o = {
		.device_count = 1,
		.devices =
			(physical_device *[]){
				(physical_device[]){
					{
						.id = 6,
						.connection =
							(connection[]){
								{ .socket = 1 },
							},
					},
				},
			},
		.object =
			(gxObject[]){
				{ .logicalName = "50", .objectType = 1 },
			},
	};

	gxByteBuffer attributes = { .size = 1 };

	hlp_getLogicalNameToString_ExpectAndReturn(o.object->logicalName, NULL, 0);
	hlp_getLogicalNameToString_IgnoreArg_ln();
	obj_typeToString2_ExpectAndReturn(o.object->objectType, "Data");
	_log_ExpectAnyArgs();

	bb_init_IgnoreAndReturn(0);
	obj_getAttributeIndexToRead_ExpectAnyArgsAndReturn(0);
	obj_getAttributeIndexToRead_ReturnThruPtr_ba(&attributes);

	//entering loop here
	bb_getUInt8ByIndex_ExpectAnyArgsAndReturn(0);
	_log_ExpectAnyArgs();
	com_read_ExpectAnyArgsAndReturn(0);
	// out of loop

	bb_clear_IgnoreAndReturn(0);

	TEST_ASSERT_EQUAL_INT(0, cg_read_cosem_object(&o, o.devices[0]));
}

#endif
