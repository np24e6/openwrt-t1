#ifndef TEST
#define TEST
#endif
#ifdef TEST

#include "unity.h"
#include "ubus.h"
#include "mock_tlt_logger.h"

#include "mock_stub_external.h"
#include "mock_stub_ubus_2.h"

#include "mock_stub_blobmsg.h"
#include "mock_stub_blob.h"

#include "mock_bytebuffer.h"
#include "mock_gxobjects.h"
#include "mock_client.h"
#include "mock_dlmssettings.h"
#include "mock_cosem.h"
#include "mock_ciphering.h"

struct blob_attr *tb_object[] = {
	(struct blob_attr *)"id",
	(struct blob_attr *)"enabled",
	(struct blob_attr *)"name",
	(struct blob_attr *)"physical_devices",
	(struct blob_attr *)"obis",
	(struct blob_attr *)"cosem_id",
	(struct blob_attr *)"entries",
};

enum {
	TEST_COSEM_OBJECT_OPT_ID,
	TEST_COSEM_OBJECT_OPT_ENABLED,
	TEST_COSEM_OBJECT_OPT_NAME,
	TEST_COSEM_OBJECT_OPT_PHYSICAL_DEVICES,
	TEST_COSEM_OBJECT_OPT_OBIS,
	TEST_COSEM_OBJECT_OPT_COSEM_ID,
	TEST_COSEM_OBJECT_OPT_ENTRIES,
	TEST_COSEM_OBJECT_OPT_COUNT
};

struct blob_attr *tb_group[] = { (struct blob_attr *)"objects" };

PRIVATE int device_cb(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method, struct blob_attr *msg);
PRIVATE int cosem_group_cb(struct ubus_context *ctx, struct ubus_object *obj,
			   struct ubus_request_data *req, const char *method, struct blob_attr *msg);
PRIVATE int validate_key(const char *key, size_t len);
PRIVATE cosem_object *parse_cosem_object(struct blob_attr *b);
static int expect_blobmsg_parse();

master *g_master = NULL;

void test_device_cb_fail_to_parse_parameters(void)
{
	device_cb_args_ExpectAndReturn(NULL, NULL, 1);
	device_cb_args_IgnoreArg_d();

	utl_unlock_mutex_if_required_ExpectAnyArgs();
	reply_ExpectAndReturn(NULL, NULL, 1, NULL, 1);
	reply_IgnoreArg_y();

	free_device_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, device_cb(NULL, NULL, NULL, NULL, NULL));
}

void test_device_cb_fail_to_make_connection(void)
{
	device_cb_args_ExpectAndReturn(NULL, NULL, 0);
	device_cb_args_IgnoreArg_d();

	utl_lock_mutex_if_required_ExpectAnyArgs();

	cg_make_connection_ExpectAndReturn(NULL, 1);
	cg_make_connection_IgnoreArg_dev();

	utl_unlock_mutex_if_required_ExpectAnyArgs();

	reply_ExpectAndReturn(NULL, NULL, 1, NULL, 1);
	reply_IgnoreArg_y();

	free_device_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, device_cb(NULL, NULL, NULL, NULL, NULL));
}

void test_device_cb_reply_failure(void)
{
	device_cb_args_ExpectAndReturn(NULL, NULL, 0);
	device_cb_args_IgnoreArg_d();

	utl_lock_mutex_if_required_ExpectAnyArgs();

	cg_make_connection_ExpectAndReturn(NULL, 0);
	cg_make_connection_IgnoreArg_dev();

	com_close_ExpectAnyArgsAndReturn(0);
	utl_unlock_mutex_if_required_ExpectAnyArgs();

	reply_ExpectAndReturn(NULL, NULL, 0, NULL, 1);
	reply_IgnoreArg_y();

	free_device_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, device_cb(NULL, NULL, NULL, NULL, NULL));
}

void test_device_cb_successful(void)
{
	device_cb_args_ExpectAndReturn(NULL, NULL, 0);
	device_cb_args_IgnoreArg_d();

	utl_lock_mutex_if_required_ExpectAnyArgs();

	cg_make_connection_ExpectAndReturn(NULL, 0);
	cg_make_connection_IgnoreArg_dev();

	com_close_ExpectAnyArgsAndReturn(0);
	utl_unlock_mutex_if_required_ExpectAnyArgs();

	reply_ExpectAndReturn(NULL, NULL, 0, NULL, 0);
	reply_IgnoreArg_y();

	free_device_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(0, device_cb(NULL, NULL, NULL, NULL, NULL));
}

void test_cosem_group_cb_fail_to_parse_parameters(void)
{
	cosem_group_args_ExpectAndReturn(NULL, NULL, 1);
	cosem_group_args_IgnoreArg_g();

	reply_ExpectAndReturn(NULL, NULL, 1, NULL, 1);
	reply_IgnoreArg_y();

	free_cosem_group_ExpectAnyArgs();
	myfree_Ignore();

	TEST_ASSERT_EQUAL_INT(1, cosem_group_cb(NULL, NULL, NULL, NULL, NULL));
}

void test_cosem_group_cb_payload_is_null(void)
{
	cosem_group_args_ExpectAndReturn(NULL, NULL, 0);
	cosem_group_args_IgnoreArg_g();

	cg_read_group_codes_ExpectAnyArgsAndReturn(NULL);
	_log_ExpectAnyArgs();

	reply_ExpectAndReturn(NULL, NULL, 0, NULL, 1);
	reply_IgnoreArg_y();

	free_cosem_group_ExpectAnyArgs();
	myfree_Ignore();

	TEST_ASSERT_EQUAL_INT(1, cosem_group_cb(NULL, NULL, NULL, NULL, NULL));
}

void test_cosem_group_cb_successful(void)
{
	cosem_group_args_ExpectAndReturn(NULL, NULL, 0);
	cosem_group_args_IgnoreArg_g();

	cg_read_group_codes_ExpectAnyArgsAndReturn("data");
	_log_ExpectAnyArgs();

	reply_ExpectAndReturn(NULL, NULL, 0, NULL, 0);
	reply_IgnoreArg_y();

	free_cosem_group_ExpectAnyArgs();
	myfree_Ignore();

	TEST_ASSERT_EQUAL_INT(0, cosem_group_cb(NULL, NULL, NULL, NULL, NULL));
}

void test_validate_key_is_equal(void)
{
	const char *str = "labas";

	mystrlen_ExpectAndReturn(str, 5);
	TEST_ASSERT_EQUAL_INT(0, validate_key(str, 5));
}

void test_validate_key_is_not_equal(void)
{
	const char *str = "labas";

	mystrlen_ExpectAndReturn(str, 5);
	TEST_ASSERT_EQUAL_INT(1, validate_key(str, 10));
}

void test_parse_cosem_object_fail_to_blobmsg_parse(void)
{
	struct blob_attr msg = { 0 };

	blob_data_ExpectAndReturn(tb_group[0], tb_group[0]->data);
	blob_len_ExpectAndReturn(tb_group[0], tb_group[0]->id_len);
	blobmsg_parse_ExpectAndReturn(NULL, 7, NULL, tb_group[0]->data, tb_group[0]->id_len, 1);
	blobmsg_parse_IgnoreArg_policy();
	blobmsg_parse_IgnoreArg_tb();
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(parse_cosem_object(tb_group[0]));
}

static int expect_blobmsg_parse()
{
	blob_data_ExpectAndReturn(tb_group[0], tb_group[0]->data);
	blob_len_ExpectAndReturn(tb_group[0], tb_group[0]->id_len);
	blobmsg_parse_ExpectAndReturn(NULL, 7, NULL, tb_group[0]->data, tb_group[0]->id_len, 0);
	blobmsg_parse_ReturnMemThruPtr_tb(tb_object, sizeof(struct blob_attr) * 7);
	blobmsg_parse_IgnoreArg_policy();
	blobmsg_parse_IgnoreArg_tb();
}

void test_parse_cosem_object_id_is_null(void)
{
	struct blob_attr msg = { 0 };

	expect_blobmsg_parse();
	tb_object[TEST_COSEM_OBJECT_OPT_ID] = NULL;
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(parse_cosem_object(tb_group[0]));
	tb_object[TEST_COSEM_OBJECT_OPT_ID] = (struct blob_attr *)"id"; // restore id
}

void test_parse_cosem_object_enabled_is_null(void)
{
	struct blob_attr msg = { 0 };

	expect_blobmsg_parse();
	tb_object[TEST_COSEM_OBJECT_OPT_ENABLED] = NULL;
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(parse_cosem_object(tb_group[0]));
	tb_object[TEST_COSEM_OBJECT_OPT_ENABLED] = (struct blob_attr *)"enabled"; // restore enabled
}

void test_parse_cosem_object_name_is_null(void)
{
	struct blob_attr msg = { 0 };

	expect_blobmsg_parse();
	tb_object[TEST_COSEM_OBJECT_OPT_NAME] = NULL;
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(parse_cosem_object(tb_group[0]));
	tb_object[TEST_COSEM_OBJECT_OPT_NAME] = (struct blob_attr *)"name"; // restore name
}

void test_parse_cosem_object_physical_devices_is_null(void)
{
	struct blob_attr msg = { 0 };

	expect_blobmsg_parse();
	tb_object[TEST_COSEM_OBJECT_OPT_PHYSICAL_DEVICES] = NULL;
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(parse_cosem_object(tb_group[0]));
	tb_object[TEST_COSEM_OBJECT_OPT_PHYSICAL_DEVICES] = (struct blob_attr *)"physical_devices"; // restore physical_devices
}

void test_parse_cosem_object_obis_is_null(void)
{
	struct blob_attr msg = { 0 };

	expect_blobmsg_parse();
	tb_object[TEST_COSEM_OBJECT_OPT_OBIS] = NULL;
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(parse_cosem_object(tb_group[0]));
	tb_object[TEST_COSEM_OBJECT_OPT_OBIS] = (struct blob_attr *)"obis"; // restore obis
}

void test_parse_cosem_object_cosem_id_is_null(void)
{
	struct blob_attr msg = { 0 };

	expect_blobmsg_parse();
	tb_object[TEST_COSEM_OBJECT_OPT_COSEM_ID] = NULL;
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(parse_cosem_object(tb_group[0]));
	tb_object[TEST_COSEM_OBJECT_OPT_COSEM_ID] = (struct blob_attr *)"cosem_id"; // restore cosem_id
}

void test_parse_cosem_object_obj_calloc_failure(void)
{
	struct blob_attr msg = { 0 };

	expect_blobmsg_parse();
	mycalloc_ExpectAndReturn(1, sizeof(cosem_object), NULL);

	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(parse_cosem_object(tb_group[0]));
}

void test_parse_cosem_object_blobmsg_check_array_failure(void)
{
	struct blob_attr msg = { 0 };
	cosem_object *obj = (cosem_object[]){ 0 };

	expect_blobmsg_parse();
	mycalloc_ExpectAndReturn(1, sizeof(cosem_object), obj);
	blobmsg_get_u32_ExpectAndReturn(tb_object[TEST_COSEM_OBJECT_OPT_ID], 0);
	blobmsg_get_u32_ExpectAndReturn(tb_object[TEST_COSEM_OBJECT_OPT_ENABLED], 0);
	blobmsg_get_string_ExpectAndReturn(tb_object[TEST_COSEM_OBJECT_OPT_NAME], "none");
	blobmsg_get_u32_ExpectAndReturn(tb_object[TEST_COSEM_OBJECT_OPT_ENTRIES], 0);

	blobmsg_check_array_ExpectAndReturn(tb_object[TEST_COSEM_OBJECT_OPT_PHYSICAL_DEVICES], BLOBMSG_TYPE_STRING, -1);
	_log_ExpectAnyArgs();
	myfree_Expect(obj);

	TEST_ASSERT_NULL(parse_cosem_object(tb_group[0]));
}

void test_parse_cosem_object_devices_calloc_failure(void)
{
	struct blob_attr msg = { 0 };
	cosem_object *obj = (cosem_object[]){ 0 };

	expect_blobmsg_parse();
	mycalloc_ExpectAndReturn(1, sizeof(cosem_object), obj);
	blobmsg_get_u32_ExpectAndReturn(tb_object[TEST_COSEM_OBJECT_OPT_ID], 0);
	blobmsg_get_u32_ExpectAndReturn(tb_object[TEST_COSEM_OBJECT_OPT_ENABLED], 0);
	blobmsg_get_string_ExpectAndReturn(tb_object[TEST_COSEM_OBJECT_OPT_NAME], "none");
	blobmsg_get_u32_ExpectAndReturn(tb_object[TEST_COSEM_OBJECT_OPT_ENTRIES], 0);

	blobmsg_check_array_ExpectAndReturn(tb_object[TEST_COSEM_OBJECT_OPT_PHYSICAL_DEVICES], BLOBMSG_TYPE_STRING, 1);
	mycalloc_ExpectAndReturn(1, sizeof(*obj->devices), NULL);
	_log_ExpectAnyArgs();
	myfree_Expect(obj);

	TEST_ASSERT_NULL(parse_cosem_object(tb_group[0]));
}

void test_parse_cosem_object_successful(void)
{
	struct blob_attr msg = { 0 };
	cosem_object *obj = (cosem_object[]){ 0 };

	expect_blobmsg_parse();
	tb_object[TEST_COSEM_OBJECT_OPT_ENTRIES] = NULL;

	mycalloc_ExpectAndReturn(1, sizeof(cosem_object), obj);
	blobmsg_get_u32_ExpectAndReturn(tb_object[TEST_COSEM_OBJECT_OPT_ID], 0);
	blobmsg_get_u32_ExpectAndReturn(tb_object[TEST_COSEM_OBJECT_OPT_ENABLED], 0);
	blobmsg_get_string_ExpectAndReturn(tb_object[TEST_COSEM_OBJECT_OPT_NAME], "none");

	physical_device **devices = (physical_device *[]){ 0 };
	blobmsg_check_array_ExpectAndReturn(tb_object[TEST_COSEM_OBJECT_OPT_PHYSICAL_DEVICES], BLOBMSG_TYPE_STRING, 1);
	mycalloc_ExpectAndReturn(1, sizeof(*obj->devices), devices);

	int cosem_id = 4;
	char *obis = "0.0.42.0.0.255";
	blobmsg_get_u32_ExpectAndReturn(tb_object[TEST_COSEM_OBJECT_OPT_COSEM_ID], cosem_id);
	blobmsg_get_string_ExpectAndReturn(tb_object[TEST_COSEM_OBJECT_OPT_OBIS], obis);
	cosem_createObject2_ExpectAndReturn(cosem_id, obis, &obj->object, 0);
	cosem_init_ExpectAndReturn(obj->object, cosem_id, obis, 0);

	TEST_ASSERT_NOT_NULL(parse_cosem_object(tb_group[0]));
	tb_object[TEST_COSEM_OBJECT_OPT_ENTRIES] = (struct blob_attr *)"entries"; // restore entries
}

#endif
