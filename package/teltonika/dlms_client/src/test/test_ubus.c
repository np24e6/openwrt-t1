#ifndef TEST
#define TEST
#endif
#ifdef TEST

#include "unity.h"
#include "ubus.h"
#include "mock_tlt_logger.h"

#include "mock_stub_external.h"
#include "mock_stub_ubus.h"

#include "mock_stub_blobmsg.h"
#include "mock_stub_libubus.h"
#include "mock_stub_blob.h"

#include "mock_bytebuffer.h"
#include "mock_gxobjects.h"
#include "mock_client.h"
#include "mock_dlmssettings.h"
#include "mock_cosem.h"
#include "mock_ciphering.h"

PRIVATE void find_corresponding_mutex(connection *c);
PRIVATE void free_device(physical_device *dev);
PRIVATE void free_cosem_group(cosem_group *g);
PRIVATE int reply(struct ubus_context *ctx, struct ubus_request_data *req, int err_code, const char *y);
PRIVATE int read_security_settings(struct blob_attr **tb, physical_device *d);

master *g_master = NULL;

void test_init_ubus_test_functions_fail_to_connect_to_ubus(void)
{
	ubus_connect_ExpectAndReturn(NULL, NULL);
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, init_ubus_test_functions());
}

void test_init_ubus_test_functions_fail_add_ubus_object(void)
{
	struct ubus_context *ubus = (struct ubus_context[]){ 0 };

	ubus_connect_ExpectAndReturn(NULL, ubus);
	ubus_add_object_ExpectAndReturn(ubus, NULL, 1);
	ubus_add_object_IgnoreArg_obj();
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, init_ubus_test_functions());
}

void test_init_ubus_test_functions_successful(void)
{
	struct ubus_context *ubus = (struct ubus_context[]){ 0 };

	ubus_connect_ExpectAndReturn(NULL, ubus);
	ubus_add_object_ExpectAndReturn(ubus, NULL, 0);
	ubus_add_object_IgnoreArg_obj();

	myubus_add_uloop_Expect(ubus);

	TEST_ASSERT_EQUAL_INT(0, init_ubus_test_functions());
}

void test_reply_blob_buf_fail_to_init(void)
{
	blob_buf_init_ExpectAndReturn(NULL, 0, 1);
	blob_buf_init_IgnoreArg_buf();
	_log_ExpectAnyArgs();

	blob_buf_free_Expect(NULL);
	blob_buf_free_IgnoreArg_buf();

	TEST_ASSERT_EQUAL_INT(1, reply(NULL, NULL, 0, "data"));
}

void test_reply_fail_to_add_error_code(void)
{
	blob_buf_init_ExpectAndReturn(NULL, 0, 0);
	blob_buf_init_IgnoreArg_buf();

	blobmsg_add_u32_ExpectAndReturn(NULL, "error", 1, 1);
	blobmsg_add_u32_IgnoreArg_buf();
	_log_ExpectAnyArgs();

	blob_buf_free_Expect(NULL);
	blob_buf_free_IgnoreArg_buf();

	TEST_ASSERT_EQUAL_INT(1, reply(NULL, NULL, 1, "data"));
}

void test_reply_fail_to_add_reply_message(void)
{
	blob_buf_init_ExpectAndReturn(NULL, 0, 0);
	blob_buf_init_IgnoreArg_buf();

	blobmsg_add_u32_ExpectAndReturn(NULL, "error", 1, 0);
	blobmsg_add_u32_IgnoreArg_buf();
	blobmsg_add_string_ExpectAndReturn(NULL, "result", "data", 1);
	blobmsg_add_string_IgnoreArg_buf();
	_log_ExpectAnyArgs();

	blob_buf_free_Expect(NULL);
	blob_buf_free_IgnoreArg_buf();

	TEST_ASSERT_EQUAL_INT(1, reply(NULL, NULL, 1, "data"));
}

void test_reply_fail_to_send_reply(void)
{
	struct ubus_context ubus     = { 0 };
	struct ubus_request_data req = { 0 };

	blob_buf_init_ExpectAndReturn(NULL, 0, 0);
	blob_buf_init_IgnoreArg_buf();

	blobmsg_add_u32_ExpectAndReturn(NULL, "error", 1, 0);
	blobmsg_add_u32_IgnoreArg_buf();
	blobmsg_add_string_ExpectAndReturn(NULL, "result", "data", 0);
	blobmsg_add_string_IgnoreArg_buf();

	ubus_send_reply_ExpectAndReturn(&ubus, &req, NULL, 1);
	_log_ExpectAnyArgs();

	blob_buf_free_Expect(NULL);
	blob_buf_free_IgnoreArg_buf();

	TEST_ASSERT_EQUAL_INT(1, reply(&ubus, &req, 1, "data"));
}

void test_reply_successful(void)
{
	struct ubus_context ubus     = { 0 };
	struct ubus_request_data req = { 0 };

	blob_buf_init_ExpectAndReturn(NULL, 0, 0);
	blob_buf_init_IgnoreArg_buf();

	blobmsg_add_u32_ExpectAndReturn(NULL, "error", 1, 0);
	blobmsg_add_u32_IgnoreArg_buf();
	blobmsg_add_string_ExpectAndReturn(NULL, "result", "data", 0);
	blobmsg_add_string_IgnoreArg_buf();

	ubus_send_reply_ExpectAndReturn(&ubus, &req, NULL, 0);

	blob_buf_free_Expect(NULL);
	blob_buf_free_IgnoreArg_buf();

	TEST_ASSERT_EQUAL_INT(0, reply(&ubus, &req, 1, "data"));
}

void test_find_corresponding_mutex_g_master_is_null(void)
{
	connection c = { 0 };

	find_corresponding_mutex(&c);
}

void test_find_corresponding_mutex_g_master_connection_count_is_zero(void)
{
	master m     = { 0 };
	g_master     = &m;
	connection c = { 0 };

	find_corresponding_mutex(&c);
}

void test_find_corresponding_mutex_exact_tcp_connection_host_does_not_exist(void)
{
	master m = {
		.connection_count = 1,
		.connections =
			(connection *[]){
				(connection[]){ { 0 } },
			},
	};
	g_master	= &m;
	connection c = { 0 };


	mystrlen_ExpectAndReturn(g_master->connections[0]->parameters.tcp.host, 0);
	mystrncmp_ExpectAndReturn(g_master->connections[0]->parameters.tcp.host, c.parameters.tcp.host, 0, 1);

	find_corresponding_mutex(&c);
}

void test_find_corresponding_mutex_exact_tcp_connection_port_does_not_exist(void)
{
	master m = {
		.connection_count = 1,
		.connections =
			(connection *[]){
				(connection[]){ { .parameters.tcp.host = "1.1", .parameters.tcp.port = 55 } },
			},
	};
	g_master	= &m;
	connection c = { .parameters.tcp.host = "1.1" , .parameters.tcp.port = 54 };


	mystrlen_ExpectAndReturn(g_master->connections[0]->parameters.tcp.host, 3);
	mystrncmp_ExpectAndReturn(g_master->connections[0]->parameters.tcp.host, c.parameters.tcp.host, 3, 0);

	find_corresponding_mutex(&c);
}

void test_find_corresponding_mutex_exact_tcp_connection_exist()
{
	master m = {
		.connection_count = 1,
		.connections =
			(connection *[]){
				(connection[]){ { .parameters.tcp.host = "1.1", .parameters.tcp.port = 55 } },
			},
	};
	g_master	= &m;
	connection c = { .parameters.tcp.host = "1.1" , .parameters.tcp.port = 55 };


	mystrlen_ExpectAndReturn(g_master->connections[0]->parameters.tcp.host, 3);
	mystrncmp_ExpectAndReturn(g_master->connections[0]->parameters.tcp.host, c.parameters.tcp.host, 3, 0);

	find_corresponding_mutex(&c);
	// TEST_ASSERT_EQUAL(true, c.mutex_needed);
}

void test_find_corresponding_mutex_exact_serial_connection_does_not_exist(void)
{
	master m = {
		.connection_count = 1,
		.connections =
			(connection *[]){
				(connection[]){ { 0 } },
			},
	};
	g_master = &m;

	connection c = { .type = SERIAL };

	mystrstr_ExpectAndReturn(c.parameters.serial.device, "rs485", NULL);

	find_corresponding_mutex(&c);
}

void test_find_corresponding_mutex_exact_serial_rs232_connection_does_exist(void)
{
	master m = {
		.connection_count = 1,
		.connections =
			(connection *[]){
				(connection[]){ { 0 } },
			},
	};
	g_master	= &m;
	connection c = { .type = SERIAL };

	mystrstr_ExpectAndReturn(c.parameters.serial.device, "rs485", NULL);

	find_corresponding_mutex(&c);
	// TEST_ASSERT_EQUAL(true, c.mutex_needed);
}

void test_find_corresponding_mutex_exact_serial_rs485_connection_does_exist(void)
{
	master m = {
		.connection_count = 1,
		.connections =
			(connection *[]){
				(connection[]){ { 0 } },
			},
	};
	g_master	= &m;
	connection c = { .type = SERIAL };

	mystrstr_ExpectAndReturn(c.parameters.serial.device, "rs485", c.parameters.serial.device + 5);

	find_corresponding_mutex(&c);
	// TEST_ASSERT_EQUAL(true, c.mutex_needed);
}

void test_find_corresponding_mutex_connection_type_is_invalid(void)
{
	master m = {
		.connection_count = 1,
		.connections =
			(connection *[]){
				(connection[]){ { 0 } },
			},
	};
	g_master	= &m;
	connection c = { .type = 3 };

	find_corresponding_mutex(&c);
}

void test_free_device_device_with_connection(void)
{
	physical_device dev = {
		.connection =
			(connection[]){
				{ 0 },
			},
	};

	cip_clear_Expect(&dev.settings.cipher);
	cl_clear_Expect(&dev.settings);
	bb_clear_ExpectAndReturn(&dev.connection->data, 0);

	free_device(&dev);
}

void test_free_cosem_group_cosem_objects_are_null(void)
{
	cosem_group g = { 0 };

	free_cosem_group(&g);
}

void test_free_cosem_group_successful(void)
{
	cosem_group g = {
		.cosem_object_count = 1,
		.cosem_objects =
			(cosem_object *[]){
				(cosem_object[]){
					{
						.devices = (physical_device *[]){ 0 },
					},
				},
			},
	};

	myfree_Expect(g.cosem_objects[0]->devices);
	obj_clear_Expect(g.cosem_objects[0]->object);
	myfree_Expect(g.cosem_objects[0]->object);
	myfree_Expect(g.cosem_objects[0]);
	myfree_Expect(g.cosem_objects);

	free_cosem_group(&g);
}

void test_ubus_exit(void)
{
	ubus_free_ExpectAnyArgs();

	ubus_exit();
}

struct blob_attr *tb[] = {
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
	(struct blob_attr *)"use_ln",
	(struct blob_attr *)"transport_security",
	(struct blob_attr *)"interface_type",
	(struct blob_attr *)"access_security",
	(struct blob_attr *)"password",
	(struct blob_attr *)"system_title",
	(struct blob_attr *)"authentication_key",
	(struct blob_attr *)"block_cipher_key",
	(struct blob_attr *)"dedicated_key",
	(struct blob_attr *)"invocation_counter",
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

void test_read_security_settings_fail_to_validate_system_title(void)
{
	physical_device d = { 0 };

	blobmsg_get_u32_ExpectAndReturn(tb[TEST_DEVICE_OPT_TRANSPORT_SECURITY], 1);
	blobmsg_get_string_ExpectAndReturn(tb[TEST_DEVICE_OPT_INVOCATION_COUNTER], "0.0.0.0");
	blobmsg_get_string_ExpectAndReturn(tb[TEST_DEVICE_OPT_SYSTEM_TITLE], "abcdefghjkl");

	validate_key_ExpectAndReturn("abcdefghjkl", 16, 1);
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, read_security_settings(tb, &d));
}

void test_read_security_settings_fail_to_validate_authentication_key(void)
{
	physical_device d = { 0 };

	blobmsg_get_u32_ExpectAndReturn(tb[TEST_DEVICE_OPT_TRANSPORT_SECURITY], 1);
	blobmsg_get_string_ExpectAndReturn(tb[TEST_DEVICE_OPT_INVOCATION_COUNTER], "0.0.0.0");
	blobmsg_get_string_ExpectAndReturn(tb[TEST_DEVICE_OPT_SYSTEM_TITLE], "abcdefghjkl1234");
	validate_key_ExpectAndReturn("abcdefghjkl1234", 16, 0);
	bb_clear_IgnoreAndReturn(0);
	bb_addHexString_IgnoreAndReturn(0);

	blobmsg_get_string_ExpectAndReturn(tb[TEST_DEVICE_OPT_AUTHENTICATION_KEY], "abcdefghjkl1234");
	validate_key_ExpectAndReturn("abcdefghjkl1234", 32, 1);
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, read_security_settings(tb, &d));
}

void test_read_security_settings_fail_to_validate_block_cipher_key(void)
{
	physical_device d = { 0 };

	blobmsg_get_u32_ExpectAndReturn(tb[TEST_DEVICE_OPT_TRANSPORT_SECURITY], 1);
	blobmsg_get_string_ExpectAndReturn(tb[TEST_DEVICE_OPT_INVOCATION_COUNTER], "0.0.0.0");
	blobmsg_get_string_ExpectAndReturn(tb[TEST_DEVICE_OPT_SYSTEM_TITLE], "abcdefghjkl1234");
	validate_key_ExpectAndReturn("abcdefghjkl1234", 16, 0);
	bb_clear_IgnoreAndReturn(0);
	bb_addHexString_IgnoreAndReturn(0);

	blobmsg_get_string_ExpectAndReturn(tb[TEST_DEVICE_OPT_AUTHENTICATION_KEY], "abcdefghjkl1234");
	validate_key_ExpectAndReturn("abcdefghjkl1234", 32, 0);

	blobmsg_get_string_ExpectAndReturn(tb[TEST_DEVICE_OPT_BLOCK_CIPHER_KEY], "abcdefghjkl1234");
	validate_key_ExpectAndReturn("abcdefghjkl1234", 32, 1);
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, read_security_settings(tb, &d));
}

void test_read_security_settings_fail_to_validate_dedicated_key(void)
{
	physical_device d = { 0 };

	blobmsg_get_u32_ExpectAndReturn(tb[TEST_DEVICE_OPT_TRANSPORT_SECURITY], 1);
	blobmsg_get_string_ExpectAndReturn(tb[TEST_DEVICE_OPT_INVOCATION_COUNTER], "0.0.0.0");
	blobmsg_get_string_ExpectAndReturn(tb[TEST_DEVICE_OPT_SYSTEM_TITLE], "abcdefghjkl1234");
	validate_key_ExpectAndReturn("abcdefghjkl1234", 16, 0);
	bb_clear_IgnoreAndReturn(0);
	bb_addHexString_IgnoreAndReturn(0);

	blobmsg_get_string_ExpectAndReturn(tb[TEST_DEVICE_OPT_AUTHENTICATION_KEY], "abcdefghjkl1234");
	validate_key_ExpectAndReturn("abcdefghjkl1234", 32, 0);

	blobmsg_get_string_ExpectAndReturn(tb[TEST_DEVICE_OPT_BLOCK_CIPHER_KEY], "abcdefghjkl1234");
	validate_key_ExpectAndReturn("abcdefghjkl1234", 32, 0);

	blobmsg_get_string_ExpectAndReturn(tb[TEST_DEVICE_OPT_DEDICATED_KEY], "abcdefghjkl1234");
	validate_key_ExpectAndReturn("abcdefghjkl1234", 32, 1);
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, read_security_settings(tb, &d));
}

void test_read_security_settings_successful(void)
{
	physical_device d = { 0 };

	blobmsg_get_u32_ExpectAndReturn(tb[TEST_DEVICE_OPT_TRANSPORT_SECURITY], 1);
	blobmsg_get_string_ExpectAndReturn(tb[TEST_DEVICE_OPT_INVOCATION_COUNTER], "0.0.0.0");
	blobmsg_get_string_ExpectAndReturn(tb[TEST_DEVICE_OPT_SYSTEM_TITLE], "abcdefghjkl1234");
	validate_key_ExpectAndReturn("abcdefghjkl1234", 16, 0);
	bb_clear_IgnoreAndReturn(0);
	bb_addHexString_IgnoreAndReturn(0);

	blobmsg_get_string_ExpectAndReturn(tb[TEST_DEVICE_OPT_AUTHENTICATION_KEY], "abcdefghjkl1234");
	validate_key_ExpectAndReturn("abcdefghjkl1234", 32, 0);

	blobmsg_get_string_ExpectAndReturn(tb[TEST_DEVICE_OPT_BLOCK_CIPHER_KEY], "abcdefghjkl1234");
	validate_key_ExpectAndReturn("abcdefghjkl1234", 32, 0);

	blobmsg_get_string_ExpectAndReturn(tb[TEST_DEVICE_OPT_DEDICATED_KEY], "abcdefghjkl1234");
	validate_key_ExpectAndReturn("abcdefghjkl1234", 32, 0);

	mycalloc_ExpectAndReturn(1, sizeof(gxByteBuffer), (gxByteBuffer[]){ 0 });
	bb_init_IgnoreAndReturn(0);
	bb_addHexString_IgnoreAndReturn(0);

	TEST_ASSERT_EQUAL_INT(0, read_security_settings(tb, &d));
}

void test_read_security_settings_blob_data_is_empty(void)
{
	physical_device d = { 0 };

	tb[TEST_DEVICE_OPT_TRANSPORT_SECURITY] = NULL;
	tb[TEST_DEVICE_OPT_INVOCATION_COUNTER] = NULL;
	tb[TEST_DEVICE_OPT_SYSTEM_TITLE]       = NULL;
	tb[TEST_DEVICE_OPT_AUTHENTICATION_KEY] = NULL;
	tb[TEST_DEVICE_OPT_BLOCK_CIPHER_KEY]   = NULL;
	tb[TEST_DEVICE_OPT_DEDICATED_KEY]      = NULL;

	TEST_ASSERT_EQUAL_INT(0, read_security_settings(tb, &d));
}

#endif
