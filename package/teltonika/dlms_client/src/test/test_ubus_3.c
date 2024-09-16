#ifndef TEST
#define TEST
#endif
#ifdef TEST

#include "unity.h"
#include "ubus.h"
#include "mock_tlt_logger.h"

#include "mock_stub_external.h"
#include "mock_stub_ubus_3.h"

#include "mock_stub_blobmsg.h"
#include "mock_stub_blob.h"

#include "mock_bytebuffer.h"
#include "mock_gxobjects.h"
#include "mock_client.h"
#include "mock_dlmssettings.h"
#include "mock_cosem.h"
#include "mock_ciphering.h"

struct blob_attr *tb_device[] = {
	(struct blob_attr *)"type",
	(struct blob_attr *)"address",
	(struct blob_attr *)"port",
	(struct blob_attr *)"serial_device",
	(struct blob_attr *)"serial_baudrate",
	(struct blob_attr *)"serial_databits",
	(struct blob_attr *)"serial_stopbits",
	(struct blob_attr *)"serial_parity",
	(struct blob_attr *)"serial_flowcontrol",
	(struct blob_attr *)"server_addr",
	(struct blob_attr *)"log_server_addr",
	(struct blob_attr *)"client_addr",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};

enum {
	TEST_DEVICE_OPT_TYPE,
	TEST_DEVICE_OPT_TCP_ADDRESS,
	TEST_DEVICE_OPT_TCP_PORT,
	TEST_DEVICE_OPT_SERIAL_DEVICE,
	TEST_DEVICE_OPT_SERIAL_BAUDRATE,
	TEST_DEVICE_OPT_SERIAL_DATABITS,
	TEST_DEVICE_OPT_SERIAL_STOPBITS,
	TEST_DEVICE_OPT_SERIAL_PARITY,
	TEST_DEVICE_OPT_SERIAL_FLOWCONTROL,
	TEST_DEVICE_OPT_SERVER_ADDR,
	TEST_DEVICE_OPT_LOG_SERVER_ADDR,
	TEST_DEVICE_OPT_CLIENT_ADDR,
	TEST_DEVICE_OPT_USE_LN,
	TEST_DEVICE_OPT_TRANSPORT_SECURITY,
	TEST_DEVICE_OPT_INTERFACE_TYPE,
	TEST_DEVICE_OPT_ACCESS_SECURITY,
	TEST_DEVICE_OPT_PASSWORD,
	TEST_DEVICE_OPT_SYSTEM_TITLE,
	TEST_DEVICE_OPT_AUTHENTICATION_KEY,
	TEST_DEVICE_OPT_BLOCK_CIPHER_KEY,
	TEST_DEVICE_OPT_DEDICATED_KEY,
	TEST_DEVICE_OPT_INVOCATION_COUNTER,
	TEST_DEVICE_OPT_MAX
};

static void expect_blobmsg_parse(struct blob_attr *msg, struct blob_attr **tb, int count);

PRIVATE int device_cb_args(physical_device *d, struct blob_attr *msg);
PRIVATE int cosem_group_args(cosem_group *g, struct blob_attr *msg);

master *g_master = NULL;

void test_device_cb_args_msg_is_null(void)
{
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(DLMS_ERROR_CODE_OTHER_REASON, device_cb_args(NULL, NULL));
}

void test_device_cb_args_fail_to_blobmsg_parse(void)
{
	struct blob_attr msg = { 0 };

	blob_data_ExpectAndReturn(&msg, msg.data);
	blob_len_ExpectAndReturn(&msg, msg.id_len);
	blobmsg_parse_ExpectAndReturn(NULL, 22, NULL, msg.data, msg.id_len, 1);
	blobmsg_parse_IgnoreArg_policy();
	blobmsg_parse_IgnoreArg_tb();
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(DLMS_ERROR_CODE_OTHER_REASON, device_cb_args(NULL, &msg));
}

static void expect_blobmsg_parse(struct blob_attr *msg, struct blob_attr **tb, int count)
{
	blob_data_ExpectAndReturn(msg, msg->data);
	blob_len_ExpectAndReturn(msg, msg->id_len);
	blobmsg_parse_ExpectAndReturn(NULL, count, NULL, msg->data, msg->id_len, 0);
	blobmsg_parse_IgnoreArg_policy();
	blobmsg_parse_IgnoreArg_tb();
	blobmsg_parse_ReturnMemThruPtr_tb(tb, sizeof(struct blob_attr) * count);
}

void test_device_cb_args_blob_attr_type_is_null(void)
{
	struct blob_attr msg = { 0 };
	physical_device d    = { .connection = (connection[]){ 0 } };

	tb_device[TEST_DEVICE_OPT_TYPE] = NULL; // make type NULL

	expect_blobmsg_parse(&msg, tb_device, TEST_DEVICE_OPT_MAX);

	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(DLMS_ERROR_CODE_OTHER_REASON, device_cb_args(&d, &msg));
	tb_device[TEST_DEVICE_OPT_TYPE] = (struct blob_attr *)"type"; // restore type
}

void test_device_cb_args_tcp_blob_attr_type_is_invalid(void)
{
	struct blob_attr msg = { 0 };
	physical_device d    = { .connection = (connection[]){ 0 } };

	expect_blobmsg_parse(&msg, tb_device, TEST_DEVICE_OPT_MAX);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TYPE], 4);

	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(DLMS_ERROR_CODE_OTHER_REASON, device_cb_args(&d, &msg));
}

void test_device_cb_args_tcp_blob_attr_address_is_null(void)
{
	struct blob_attr msg = { 0 };
	physical_device d    = { .connection = (connection[]){ 0 } };

	tb_device[TEST_DEVICE_OPT_TCP_ADDRESS] = NULL; // make address NULL

	expect_blobmsg_parse(&msg, tb_device, TEST_DEVICE_OPT_MAX);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TYPE], TCP);

	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(DLMS_ERROR_CODE_OTHER_REASON, device_cb_args(&d, &msg));
	tb_device[TEST_DEVICE_OPT_TCP_ADDRESS] = (struct blob_attr *)"address"; // restore address
}

void test_device_cb_args_tcp_blob_attr_port_is_null(void)
{
	struct blob_attr msg = { 0 };
	physical_device d    = { .connection = (connection[]){ 0 } };

	tb_device[TEST_DEVICE_OPT_TCP_PORT] = NULL; // make port NULL

	expect_blobmsg_parse(&msg, tb_device, TEST_DEVICE_OPT_MAX);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TYPE], TCP);

	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(DLMS_ERROR_CODE_OTHER_REASON, device_cb_args(&d, &msg));
	tb_device[TEST_DEVICE_OPT_TCP_PORT] = (struct blob_attr *)"port"; // restore port
}

void test_device_cb_args_tcp_blob_attr_port_is_lower_than_minimum(void)
{
	struct blob_attr msg = { 0 };
	physical_device d    = { .connection = (connection[]){ 0 } };

	expect_blobmsg_parse(&msg, tb_device, TEST_DEVICE_OPT_MAX);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TYPE], TCP);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TCP_PORT], 0);

	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(DLMS_ERROR_CODE_OTHER_REASON, device_cb_args(&d, &msg));
}

void test_device_cb_args_tcp_blob_attr_port_is_higher_than_maximum(void)
{
	struct blob_attr msg = { 0 };
	physical_device d    = { .connection = (connection[]){ 0 } };

	expect_blobmsg_parse(&msg, tb_device, TEST_DEVICE_OPT_MAX);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TYPE], TCP);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TCP_PORT], 65536);

	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(DLMS_ERROR_CODE_OTHER_REASON, device_cb_args(&d, &msg));
}

void test_device_cb_args_serial_blob_attr_device_is_null(void)
{
	struct blob_attr msg = { 0 };
	physical_device d    = { .connection = (connection[]){ 0 } };

	expect_blobmsg_parse(&msg, tb_device, TEST_DEVICE_OPT_MAX);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TYPE], SERIAL);

	tb_device[TEST_DEVICE_OPT_SERIAL_DEVICE] = NULL;

	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(DLMS_ERROR_CODE_OTHER_REASON, device_cb_args(&d, &msg));
	tb_device[TEST_DEVICE_OPT_SERIAL_DEVICE] = (struct blob_attr *)"serial_device"; // restore serial device
}

void test_device_cb_args_serial_blob_attr_baudrate_is_null(void)
{
	struct blob_attr msg = { 0 };
	physical_device d    = { .connection = (connection[]){ 0 } };

	expect_blobmsg_parse(&msg, tb_device, TEST_DEVICE_OPT_MAX);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TYPE], SERIAL);

	tb_device[TEST_DEVICE_OPT_SERIAL_BAUDRATE] = NULL;

	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(DLMS_ERROR_CODE_OTHER_REASON, device_cb_args(&d, &msg));
	tb_device[TEST_DEVICE_OPT_SERIAL_BAUDRATE] = (struct blob_attr *)"serial_baudrate"; // restore serial baudrate
}

void test_device_cb_args_serial_blob_attr_databits_is_null(void)
{
	struct blob_attr msg = { 0 };
	physical_device d    = { .connection = (connection[]){ 0 } };

	expect_blobmsg_parse(&msg, tb_device, TEST_DEVICE_OPT_MAX);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TYPE], SERIAL);

	tb_device[TEST_DEVICE_OPT_SERIAL_DATABITS] = NULL;

	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(DLMS_ERROR_CODE_OTHER_REASON, device_cb_args(&d, &msg));
	tb_device[TEST_DEVICE_OPT_SERIAL_DATABITS] = (struct blob_attr *)"serial_databits"; // restore serial databits
}

void test_device_cb_args_serial_blob_attr_stopbits_is_null(void)
{
	struct blob_attr msg = { 0 };
	physical_device d    = { .connection = (connection[]){ 0 } };

	expect_blobmsg_parse(&msg, tb_device, TEST_DEVICE_OPT_MAX);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TYPE], SERIAL);

	tb_device[TEST_DEVICE_OPT_SERIAL_STOPBITS] = NULL;

	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(DLMS_ERROR_CODE_OTHER_REASON, device_cb_args(&d, &msg));
	tb_device[TEST_DEVICE_OPT_SERIAL_STOPBITS] = (struct blob_attr *)"serial_stopbits"; // restore serial stopbits
}

void test_device_cb_args_serial_blob_attr_parity_is_null(void)
{
	struct blob_attr msg = { 0 };
	physical_device d    = { .connection = (connection[]){ 0 } };

	expect_blobmsg_parse(&msg, tb_device, TEST_DEVICE_OPT_MAX);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TYPE], SERIAL);

	tb_device[TEST_DEVICE_OPT_SERIAL_PARITY] = NULL;

	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(DLMS_ERROR_CODE_OTHER_REASON, device_cb_args(&d, &msg));
	tb_device[TEST_DEVICE_OPT_SERIAL_PARITY] = (struct blob_attr *)"serial_parity"; // restore serial parity
}

void test_device_cb_args_serial_blob_attr_flowcontrol_is_null(void)
{
	struct blob_attr msg = { 0 };
	physical_device d    = { .connection = (connection[]){ 0 } };

	expect_blobmsg_parse(&msg, tb_device, TEST_DEVICE_OPT_MAX);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TYPE], SERIAL);

	tb_device[TEST_DEVICE_OPT_SERIAL_FLOWCONTROL] = NULL;

	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(DLMS_ERROR_CODE_OTHER_REASON, device_cb_args(&d, &msg));
	tb_device[TEST_DEVICE_OPT_SERIAL_FLOWCONTROL] = (struct blob_attr *)"serial_flowcontrol"; // restore serial flowcontrol
}

void test_device_cb_args_serial_blob_attr_server_addr_is_null(void)
{
	struct blob_attr msg = { 0 };
	physical_device d    = { .connection = (connection[]){ 0 } };

	expect_blobmsg_parse(&msg, tb_device, TEST_DEVICE_OPT_MAX);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TYPE], SERIAL);

	blobmsg_get_string_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_SERIAL_DEVICE], "/dev/rs232");
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_SERIAL_BAUDRATE], 0);
	blobmsg_get_string_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_SERIAL_DATABITS], "none");
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_SERIAL_STOPBITS], 0);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_SERIAL_PARITY], 0);
	blobmsg_get_string_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_SERIAL_FLOWCONTROL], "none");

	tb_device[TEST_DEVICE_OPT_SERVER_ADDR] = NULL;

	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(DLMS_ERROR_CODE_OTHER_REASON, device_cb_args(&d, &msg));
	tb_device[TEST_DEVICE_OPT_SERVER_ADDR] = (struct blob_attr *)"server_addr"; // restore server addr
}

void test_device_cb_args_tcp_blob_attr_server_addr_is_null(void)
{
	struct blob_attr msg = { 0 };
	physical_device d    = { .connection = (connection[]){ 0 } };

	expect_blobmsg_parse(&msg, tb_device, TEST_DEVICE_OPT_MAX);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TYPE], TCP);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TCP_PORT], 9999);
	blobmsg_get_string_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TCP_ADDRESS], "0.0.0.0");

	tb_device[TEST_DEVICE_OPT_SERVER_ADDR] = NULL;

	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(DLMS_ERROR_CODE_OTHER_REASON, device_cb_args(&d, &msg));
	tb_device[TEST_DEVICE_OPT_SERVER_ADDR] = (struct blob_attr *)"server_addr"; // restore server addr
}

void test_device_cb_args_blob_attr_log_server_addr_is_null(void)
{
	struct blob_attr msg = { 0 };
	physical_device d    = { .connection = (connection[]){ 0 } };

	expect_blobmsg_parse(&msg, tb_device, TEST_DEVICE_OPT_MAX);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TYPE], TCP);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TCP_PORT], 9999);
	blobmsg_get_string_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TCP_ADDRESS], "0.0.0.0");

	tb_device[TEST_DEVICE_OPT_LOG_SERVER_ADDR] = NULL;

	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(DLMS_ERROR_CODE_OTHER_REASON, device_cb_args(&d, &msg));
	tb_device[TEST_DEVICE_OPT_LOG_SERVER_ADDR] = (struct blob_attr *)"log_server_addr"; // restore logical server addr
}

void test_device_cb_args_blob_attr_client_addr_is_null(void)
{
	struct blob_attr msg = { 0 };
	physical_device d    = { .connection = (connection[]){ 0 } };

	expect_blobmsg_parse(&msg, tb_device, TEST_DEVICE_OPT_MAX);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TYPE], TCP);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TCP_PORT], 9999);
	blobmsg_get_string_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TCP_ADDRESS], "0.0.0.0");
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_LOG_SERVER_ADDR], 1);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_SERVER_ADDR], 0);
	cl_getServerAddress_ExpectAnyArgsAndReturn(0);

	tb_device[TEST_DEVICE_OPT_CLIENT_ADDR] = NULL;

	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(DLMS_ERROR_CODE_OTHER_REASON, device_cb_args(&d, &msg));
	tb_device[TEST_DEVICE_OPT_CLIENT_ADDR] = (struct blob_attr *)"client_addr"; // restore client addr
}

void test_device_cb_args_fail_to_read_security_settings(void)
{
	struct blob_attr msg = { 0 };
	physical_device d    = { .connection = (connection[]){ 0 } };

	expect_blobmsg_parse(&msg, tb_device, TEST_DEVICE_OPT_MAX);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TYPE], TCP);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TCP_PORT], 9999);
	blobmsg_get_string_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TCP_ADDRESS], "0.0.0.0");
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_LOG_SERVER_ADDR], 1);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_SERVER_ADDR], 0);
	cl_getServerAddress_ExpectAnyArgsAndReturn(0);
        blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_CLIENT_ADDR], 1);

	cl_init_Ignore();

	read_security_settings_ExpectAndReturn(tb_device, &d, 1);
	read_security_settings_IgnoreArg_tb();
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(250, device_cb_args(&d, &msg));
}

void test_device_cb_args_successful_without_additional_security_settings(void)
{
	struct blob_attr msg = { 0 };
	physical_device d    = { .connection = (connection[]){ 0 } };

	expect_blobmsg_parse(&msg, tb_device, TEST_DEVICE_OPT_MAX);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TYPE], TCP);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TCP_PORT], 9999);
	blobmsg_get_string_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TCP_ADDRESS], "0.0.0.0");
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_LOG_SERVER_ADDR], 1);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_SERVER_ADDR], 0);
	cl_getServerAddress_ExpectAnyArgsAndReturn(0);
        blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_CLIENT_ADDR], 1);

	cl_init_Ignore();

	read_security_settings_ExpectAndReturn(tb_device, &d, 0);
	read_security_settings_IgnoreArg_tb();

	bb_init_IgnoreAndReturn(0);
	bb_capacity_ExpectAnyArgsAndReturn(0);
	find_corresponding_mutex_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(0, device_cb_args(&d, &msg));
}

void test_device_cb_args_successful_with_additional_security_settings(void)
{
	struct blob_attr msg = { 0 };
	physical_device d    = { .connection = (connection[]){ 0 } };

	tb_device[TEST_DEVICE_OPT_INTERFACE_TYPE] = (struct blob_attr *)"interface_type";
	tb_device[TEST_DEVICE_OPT_ACCESS_SECURITY] = (struct blob_attr *)"access_security";
	tb_device[TEST_DEVICE_OPT_PASSWORD] = (struct blob_attr *)"password";
	tb_device[TEST_DEVICE_OPT_TRANSPORT_SECURITY] = (struct blob_attr *)"transport_security";

	expect_blobmsg_parse(&msg, tb_device, TEST_DEVICE_OPT_MAX);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TYPE], TCP);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TCP_PORT], 9999);
	blobmsg_get_string_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_TCP_ADDRESS], "0.0.0.0");
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_LOG_SERVER_ADDR], 1);
	blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_SERVER_ADDR], 0);
	cl_getServerAddress_ExpectAnyArgsAndReturn(0);
        blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_CLIENT_ADDR], 1);

        blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_INTERFACE_TYPE], 1);
        blobmsg_get_u32_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_ACCESS_SECURITY], 1);
	blobmsg_get_string_ExpectAndReturn(tb_device[TEST_DEVICE_OPT_PASSWORD], "Gurux");

	cl_init_Ignore();

	read_security_settings_ExpectAndReturn(tb_device, &d, 0);
	read_security_settings_IgnoreArg_tb();

	bb_init_IgnoreAndReturn(0);
	bb_capacity_ExpectAnyArgsAndReturn(0);
	find_corresponding_mutex_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(0, device_cb_args(&d, &msg));
}

void test_cosem_group_args_msg_is_null(void)
{
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, cosem_group_args(NULL, NULL));
}

struct blob_attr *tb_cosem_group[] = {
	(struct blob_attr *)"objects",
};

enum {
	TEST_COSEM_GROUP_OPT_OBJECTS,
	TEST_COSEM_GROUP_OPT_COUNT,
};

void test_cosem_group_args_fail_to_blobmsg_parse(void)
{
	struct blob_attr msg = { 0 };

	blob_data_ExpectAndReturn(&msg, msg.data);
	blob_len_ExpectAndReturn(&msg, msg.id_len);
	blobmsg_parse_ExpectAndReturn(NULL, 1, NULL, msg.data, msg.id_len, 1);
	blobmsg_parse_IgnoreArg_policy();
	blobmsg_parse_IgnoreArg_tb();
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, cosem_group_args(NULL, &msg));
}

void test_cosem_group_args_blob_attr_type_is_null(void)
{
	struct blob_attr msg = { 0 };
	cosem_group g	     = { 0 };

	tb_cosem_group[0] = NULL; // make objects NULL

	expect_blobmsg_parse(&msg, tb_cosem_group, 1);
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, cosem_group_args(&g, &msg));
	tb_cosem_group[0] = (struct blob_attr *)"objects"; // restore objects
}

void test_cosem_group_args_blobmsg_check_array_failure(void)
{
	struct blob_attr msg = { 0 };
	cosem_group g	     = { 0 };

	expect_blobmsg_parse(&msg, tb_cosem_group, 1);
	blobmsg_check_array_ExpectAndReturn(tb_cosem_group[TEST_COSEM_GROUP_OPT_OBJECTS], BLOBMSG_TYPE_TABLE, -1);
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, cosem_group_args(&g, &msg));
}

void test_cosem_group_args_successful(void)
{
	struct blob_attr msg = { 0 };
	cosem_group g	     = { 0 };

	expect_blobmsg_parse(&msg, tb_cosem_group, 1);
	blobmsg_check_array_ExpectAndReturn(tb_cosem_group[TEST_COSEM_GROUP_OPT_OBJECTS], BLOBMSG_TYPE_TABLE, 1);

	TEST_ASSERT_EQUAL_INT(0, cosem_group_args(&g, &msg));
}

//!!!! maybe someday: test for blobmsg_for_each_attr
// void test_cosem_group_args_parse_cosem_object_is_NULL(void)
// {
// 	struct blob_attr msg = { 0 };
// 	cosem_group g	     = { 0 };

// 	blobmsg_data_len_IgnoreAndReturn(1);
// 	blobmsg_data_IgnoreAndReturn("object");

// 	expect_blobmsg_parse(&msg, tb_cosem_group, 1);
// 	blobmsg_check_array_ExpectAndReturn(tb_cosem_group[TEST_COSEM_GROUP_OPT_OBJECTS], BLOBMSG_TYPE_TABLE, 1);
// 	_log_ExpectAnyArgs();

// 	TEST_ASSERT_EQUAL_INT(1, cosem_group_args(&g, &msg));
// }

#endif
