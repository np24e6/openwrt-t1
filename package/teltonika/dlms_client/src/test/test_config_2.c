#ifndef TEST
#define TEST
#endif
#ifdef TEST

#include "unity.h"
#include "config.h"
#include "mock_tlt_logger.h"

#include "mock_stub_external.h"
#include "mock_stub_config_2.h"

#include "mock_libtlt_uci.h"
#include "mock_stub_pthread.h"

#include "mock_bytebuffer.h"
#include "mock_gxobjects.h"
#include "mock_client.h"
#include "mock_dlmssettings.h"
#include "mock_cosem.h"
#include "mock_ciphering.h"

PRIVATE connection_params_cfg *cfg_read_connection(struct uci_context *uci, struct uci_section *section);
PRIVATE physical_device_cfg *cfg_read_physical_device(struct uci_context *uci, struct uci_section *section);
PRIVATE cosem_object_cfg *cfg_read_cosem_object(struct uci_context *uci, struct uci_section *section);
PRIVATE cosem_group_cfg *cfg_read_cosem_group(struct uci_context *uci, struct uci_section *section);

PRIVATE connection **cfg_get_connections(dlms_cfg *cfg, size_t *connection_count);
PRIVATE physical_device **cfg_get_physical_devices(dlms_cfg *cfg, connection **connections, size_t connection_count, size_t *physical_dev_count);
PRIVATE cosem_object **cfg_get_cosem_objects(dlms_cfg *cfg, physical_device **physical_devices, size_t physical_dev_count, size_t *cosem_object_count);
PRIVATE cosem_group **cfg_get_cosem_groups(dlms_cfg *cfg, cosem_object **cosem_objects, size_t cosem_object_count, size_t *cosem_group_count);

PRIVATE int cfg_init_mutex(pthread_mutex_t **mutex);

master *g_master = NULL;

void test_cfg_get_master_dlms_cfg_is_null(void)
{
	cfg_read_dlms_cfg_ExpectAndReturn(NULL);
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(cfg_get_master());
}

void test_cfg_get_master_m_is_null(void)
{
	dlms_cfg cfg = { 0 };
	master m     = { 0 };

	cfg_read_dlms_cfg_ExpectAndReturn(&cfg);
	_log_ExpectAnyArgs();
	_log_ExpectAnyArgs();
	cfg_read_master_ExpectAndReturn(&cfg, NULL);
	_log_ExpectAnyArgs();

	cfg_free_dlms_cfg_Expect(&cfg);
	cfg_free_master_Expect(&m);
	cfg_free_master_IgnoreArg_m();

	TEST_ASSERT_NULL(cfg_get_master());
}

void test_cfg_get_master_fail_to_init_mutexes(void)
{
	dlms_cfg cfg = { 0 };
	master m     = { 0 };

	cfg_read_dlms_cfg_ExpectAndReturn(&cfg);
	_log_ExpectAnyArgs();
	_log_ExpectAnyArgs();
	cfg_read_master_ExpectAndReturn(&cfg, &m);
	_log_ExpectAnyArgs();
	cfg_init_mutexes_ExpectAndReturn(&m, 1);
	_log_ExpectAnyArgs();

	cfg_free_dlms_cfg_Expect(&cfg);
	cfg_free_master_Expect(&m);

	TEST_ASSERT_NULL(cfg_get_master());
}

void test_cfg_get_master_successful(void)
{
	dlms_cfg cfg = { 0 };
	master m     = { 0 };

	cfg_read_dlms_cfg_ExpectAndReturn(&cfg);
	_log_ExpectAnyArgs();
	_log_ExpectAnyArgs();
	cfg_read_master_ExpectAndReturn(&cfg, &m);
	_log_ExpectAnyArgs();
	cfg_init_mutexes_ExpectAndReturn(&m, 0);

	cfg_free_dlms_cfg_Expect(&cfg);

	TEST_ASSERT_NOT_NULL(cfg_get_master());
}

void test_cfg_read_connection_calloc_failure(void)
{
	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(connection_params_cfg), NULL);
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(cfg_read_connection(NULL, NULL));
}

void test_cfg_read_connection_section_name_is_null(void)
{
	struct uci_section sec	= { 0 };
	connection_params_cfg c = { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(connection_params_cfg), &c);
	_log_ExpectAnyArgs();
	
	_log_ExpectAnyArgs();
	cfg_free_connection_cfg_Expect(&c);

	TEST_ASSERT_NULL(cfg_read_connection(NULL, &sec));
}

void test_cfg_read_connection_invalid_section_id(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "labas" };
	connection_params_cfg c = { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(connection_params_cfg), &c);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 0);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_connection_cfg_Expect(&c);

	TEST_ASSERT_NULL(cfg_read_connection(&uci, &sec));
}

void test_cfg_read_connection_invalid_section_name(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	connection_params_cfg c = { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(connection_params_cfg), &c);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);

	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", NULL);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_connection_cfg_Expect(&c);

	TEST_ASSERT_NULL(cfg_read_connection(&uci, &sec));
}

void test_cfg_read_connection_invalid_connection_type(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	connection_params_cfg c = { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(connection_params_cfg), &c);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);

	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "connection_one");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "connection_type", 0, 2);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_connection_cfg_Expect(&c);

	TEST_ASSERT_NULL(cfg_read_connection(&uci, &sec));
}

void test_cfg_read_connection_invalid_TCP_host(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	connection_params_cfg c = { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(connection_params_cfg), &c);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);

	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "connection_one");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "connection_type", 0, 0);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "address", NULL);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_connection_cfg_Expect(&c);

	TEST_ASSERT_NULL(cfg_read_connection(&uci, &sec));
}

void test_cfg_read_connection_invalid_TCP_port_min_port(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	connection_params_cfg c = { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(connection_params_cfg), &c);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "connection_one");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "connection_type", 0, 0);

	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "address", "192.168.1.1");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "port", 0, 0);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_connection_cfg_Expect(&c);

	TEST_ASSERT_NULL(cfg_read_connection(&uci, &sec));
}

void test_cfg_read_connection_invalid_TCP_port_max_port(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	connection_params_cfg c = { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(connection_params_cfg), &c);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "connection_one");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "connection_type", 0, 0);

	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "address", "192.168.1.1");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "port", 0, 999999);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_connection_cfg_Expect(&c);

	TEST_ASSERT_NULL(cfg_read_connection(&uci, &sec));
}

void test_cfg_read_connection_invalid_SERIAL_settings_device(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	connection_params_cfg c = { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(connection_params_cfg), &c);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);

	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "connection_one");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "connection_type", 0, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "device",  NULL);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_connection_cfg_Expect(&c);

	TEST_ASSERT_NULL(cfg_read_connection(&uci, &sec));
}


void test_cfg_read_connection_invalid_SERIAL_settings_baudrate(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	connection_params_cfg c = { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(connection_params_cfg), &c);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "connection_one");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "connection_type", 0, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "device",  "/dev/rs232");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "baudrate", 0, 0);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_connection_cfg_Expect(&c);

	TEST_ASSERT_NULL(cfg_read_connection(&uci, &sec));
}

void test_cfg_read_connection_invalid_SERIAL_settings_databits(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	connection_params_cfg c = { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(connection_params_cfg), &c);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "connection_one");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "connection_type", 0, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "device",  "/dev/rs232");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "baudrate", 0, 115200);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "databits", 0, 3);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_connection_cfg_Expect(&c);

	TEST_ASSERT_NULL(cfg_read_connection(&uci, &sec));
}

void test_cfg_read_connection_invalid_SERIAL_settings_stopbits(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	connection_params_cfg c = { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(connection_params_cfg), &c);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "connection_one");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "connection_type", 0, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "device",  "/dev/rs232");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "baudrate", 0, 115200);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "databits", 0, 8);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "stopbits", 0, 3);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_connection_cfg_Expect(&c);

	TEST_ASSERT_NULL(cfg_read_connection(&uci, &sec));
}

void test_cfg_read_connection_invalid_SERIAL_settings_parity(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	connection_params_cfg c = { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(connection_params_cfg), &c);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "connection_one");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "connection_type", 0, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "device",  "/dev/rs232");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "baudrate", 0, 115200);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "databits", 0, 8);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "stopbits", 0, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "parity",  NULL);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_connection_cfg_Expect(&c);

	TEST_ASSERT_NULL(cfg_read_connection(&uci, &sec));
}

void test_cfg_read_connection_invalid_SERIAL_settings_flowcontrol(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	connection_params_cfg c = { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(connection_params_cfg), &c);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "connection_one");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "connection_type", 0, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "device",  "/dev/rs232");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "baudrate", 0, 115200);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "databits", 0, 8);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "stopbits", 0, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "parity",  "none");
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "flowcontrol",  NULL);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_connection_cfg_Expect(&c);

	TEST_ASSERT_NULL(cfg_read_connection(&uci, &sec));
}

// TODO: add struct variables check ASSERTS
void test_cfg_read_connection_successful(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	connection_params_cfg c = { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(connection_params_cfg), &c);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "connection_one");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "connection_type", 0, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "device",  "/dev/rs232");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "baudrate", 0, 115200);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "databits", 0, 8);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "stopbits", 0, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "parity",  "none");
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "flowcontrol",  "none");

	_log_ExpectAnyArgs();

	TEST_ASSERT_NOT_NULL(cfg_read_connection(&uci, &sec));
}

void test_cfg_read_physical_device_calloc_failure(void)
{
	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(physical_device_cfg), NULL);
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(cfg_read_physical_device(NULL, NULL));
}

void test_cfg_read_physical_device_section_name_is_null(void)
{
	struct uci_section sec	= { 0 };
	physical_device_cfg d = { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(physical_device_cfg), &d);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_physical_device_cfg_Expect(&d);

	TEST_ASSERT_NULL(cfg_read_physical_device(NULL, &sec));
}

void test_cfg_read_physical_device_invalid_section_id(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "labas" };
	physical_device_cfg d	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(physical_device_cfg), &d);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 0);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_physical_device_cfg_Expect(&d);

	TEST_ASSERT_NULL(cfg_read_physical_device(&uci, &sec));
}

void test_cfg_read_physical_device_invalid_section_name(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	physical_device_cfg d	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(physical_device_cfg), &d);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);

	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "enabled", 0, 0);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", NULL);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_physical_device_cfg_Expect(&d);

	TEST_ASSERT_NULL(cfg_read_physical_device(&uci, &sec));
}

void test_cfg_read_physical_device_invalid_server_address(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	physical_device_cfg d	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(physical_device_cfg), &d);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "enabled", 0, 0);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "device_one");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "server_addr", 0, -1);
	
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_physical_device_cfg_Expect(&d);

	TEST_ASSERT_NULL(cfg_read_physical_device(&uci, &sec));
}

void test_cfg_read_physical_device_invalid_logical_server_address(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	physical_device_cfg d	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(physical_device_cfg), &d);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "enabled", 0, 0);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "device_one");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "server_addr", 0, 0);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "log_server_addr", 0, -1);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_physical_device_cfg_Expect(&d);

	TEST_ASSERT_NULL(cfg_read_physical_device(&uci, &sec));
}

void test_cfg_read_physical_device_invalid_client_address(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	physical_device_cfg d	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(physical_device_cfg), &d);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "enabled", 0, 0);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "device_one");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "server_addr", 0, 0);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "log_server_addr", 0, 0);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "client_addr", 0, -1);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_physical_device_cfg_Expect(&d);

	TEST_ASSERT_NULL(cfg_read_physical_device(&uci, &sec));
}

void test_cfg_read_physical_device_invalid_use_logical_name_ref(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	physical_device_cfg d	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(physical_device_cfg), &d);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "enabled", 0, 0);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "device_one");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "server_addr", 0, 0);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "log_server_addr", 0, 0);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "client_addr", 0, 1);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "use_logical_name_ref", 1, 2);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_physical_device_cfg_Expect(&d);

	TEST_ASSERT_NULL(cfg_read_physical_device(&uci, &sec));
}

void test_cfg_read_physical_device_invalid_connection(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	physical_device_cfg d	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(physical_device_cfg), &d);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "enabled", 0, 0);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "device_one");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "server_addr", 0, 0);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "log_server_addr", 0, 0);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "client_addr", 0, 1);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "use_logical_name_ref", 1, 1);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "connection", 0, 0);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_physical_device_cfg_Expect(&d);

	TEST_ASSERT_NULL(cfg_read_physical_device(&uci, &sec));
}

// TODO: add struct variables check ASSERTS
void test_cfg_read_physical_device_success(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	physical_device_cfg d	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(physical_device_cfg), &d);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "enabled", 0, 0);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "device_one");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "server_addr", 0, 0);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "log_server_addr", 0, 0);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "client_addr", 0, 1);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "use_logical_name_ref", 1, 1);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "connection", 0, 1);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "access_security", 0, 0);
	_log_ExpectAnyArgs();
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "interface_type", 0, 0);
	_log_ExpectAnyArgs();
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "password", NULL);
	_log_ExpectAnyArgs();
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "transport_security", 0, 0);
	_log_ExpectAnyArgs();
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "authentication_key", NULL);
	_log_ExpectAnyArgs();
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "block_cipher_key", NULL);
	_log_ExpectAnyArgs();
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "dedicated_key", NULL);
	_log_ExpectAnyArgs();
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "invocation_counter", NULL);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();

	TEST_ASSERT_NOT_NULL(cfg_read_physical_device(&uci, &sec));
}

void test_cfg_read_cosem_object_calloc_failure(void)
{
	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(cosem_object_cfg), NULL);
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(cfg_read_cosem_object(NULL, NULL));
}

void test_cfg_read_cosem_object_section_name_is_null(void)
{
	struct uci_section sec	= { 0 };
	cosem_object_cfg o	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(cosem_object_cfg), &o);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_cosem_object_cfg_Expect(&o);

	TEST_ASSERT_NULL(cfg_read_cosem_object(NULL, &sec));
}

void test_cfg_read_cosem_object_invalid_section_id(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "labas" };
	cosem_object_cfg o	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(cosem_object_cfg), &o);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 0);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_cosem_object_cfg_Expect(&o);

	TEST_ASSERT_NULL(cfg_read_cosem_object(&uci, &sec));
}

void test_cfg_read_cosem_object_invalid_enabled(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	cosem_object_cfg o	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(cosem_object_cfg), &o);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);

	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "enabled", 0, 0);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_cosem_object_cfg_Expect(&o);

	TEST_ASSERT_NULL(cfg_read_cosem_object(&uci, &sec));
}

void test_cfg_read_cosem_object_invalid_section_name(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	cosem_object_cfg o	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(cosem_object_cfg), &o);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);

	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "enabled", 0, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", NULL);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_cosem_object_cfg_Expect(&o);

	TEST_ASSERT_NULL(cfg_read_cosem_object(&uci, &sec));
}

void test_cfg_read_cosem_object_invalid_obis(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	cosem_object_cfg o	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(cosem_object_cfg), &o);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);

	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "enabled", 0, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "object_one");
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "obis", NULL);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_cosem_object_cfg_Expect(&o);

	TEST_ASSERT_NULL(cfg_read_cosem_object(&uci, &sec));
}

void test_cfg_read_cosem_object_invalid_cosem_id(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	cosem_object_cfg o	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(cosem_object_cfg), &o);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);

	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "enabled", 0, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "object_one");
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "obis", "0.0.42.0.0.255");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "cosem_id", 0, 0);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_cosem_object_cfg_Expect(&o);

	TEST_ASSERT_NULL(cfg_read_cosem_object(&uci, &sec));
}

void test_cfg_read_cosem_object_invalid_profile_generic_entries(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	cosem_object_cfg o	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(cosem_object_cfg), &o);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);

	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "enabled", 0, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "object_one");
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "obis", "0.0.42.0.0.255");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "cosem_id", 0, DLMS_OBJECT_TYPE_PROFILE_GENERIC);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "entries", 0, 0);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_cosem_object_cfg_Expect(&o);

	TEST_ASSERT_NULL(cfg_read_cosem_object(&uci, &sec));
}

void test_cfg_read_cosem_object_invalid_phyiscal_devices(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	cosem_object_cfg o	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(cosem_object_cfg), &o);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);

	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "enabled", 0, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "object_one");
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "obis", "0.0.42.0.0.255");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "cosem_id", 0, DLMS_OBJECT_TYPE_PROFILE_GENERIC);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "entries", 0, 1);
	ucix_get_list_option_ExpectAndReturn(&uci, "dlms_master", "1", "physical_device", NULL);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_cosem_object_cfg_Expect(&o);

	TEST_ASSERT_NULL(cfg_read_cosem_object(&uci, &sec));
}

void test_cfg_read_cosem_object_invalid_cosem_group(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	cosem_object_cfg o	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(cosem_object_cfg), &o);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);

	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "enabled", 0, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "object_one");
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "obis", "0.0.42.0.0.255");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "cosem_id", 0, DLMS_OBJECT_TYPE_PROFILE_GENERIC);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "entries", 0, 1);
	ucix_get_list_option_ExpectAndReturn(&uci, "dlms_master", "1", "physical_device", "4 5 6");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "cosem_group", 0, 0);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_cosem_object_cfg_Expect(&o);

	TEST_ASSERT_NULL(cfg_read_cosem_object(&uci, &sec));
}

// TODO: add struct variables check ASSERTS
void test_cfg_read_cosem_object_successful(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	cosem_object_cfg o	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(cosem_object_cfg), &o);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);

	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "enabled", 0, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "object_one");
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "obis", "0.0.42.0.0.255");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "cosem_id", 0, DLMS_OBJECT_TYPE_PROFILE_GENERIC);
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "entries", 0, 1);
	ucix_get_list_option_ExpectAndReturn(&uci, "dlms_master", "1", "physical_device", "4 5 6");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "cosem_group", 0, 5);
	_log_ExpectAnyArgs();

	TEST_ASSERT_NOT_NULL(cfg_read_cosem_object(&uci, &sec));
}

void test_cfg_read_cosem_group_calloc_failure(void)
{
	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(cosem_group_cfg), NULL);
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(cfg_read_cosem_group(NULL, NULL));
}

void test_cfg_read_cosem_group_section_name_is_null(void)
{
	struct uci_section sec	= { 0 };
	cosem_group_cfg o	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(cosem_group_cfg), &o);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_cosem_group_cfg_Expect(&o);

	TEST_ASSERT_NULL(cfg_read_cosem_group(NULL, &sec));
}

void test_cfg_read_cosem_group_invalid_section_id(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "labas" };
	cosem_group_cfg g	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(cosem_group_cfg), &g);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 0);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_cosem_group_cfg_Expect(&g);

	TEST_ASSERT_NULL(cfg_read_cosem_group(&uci, &sec));
}

void test_cfg_read_cosem_group_invalid_enabled(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	cosem_group_cfg g	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(cosem_group_cfg), &g);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);

	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "enabled", 0, 0);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_cosem_group_cfg_Expect(&g);

	TEST_ASSERT_NULL(cfg_read_cosem_group(&uci, &sec));
}

void test_cfg_read_cosem_group_invalid_section_name(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	cosem_group_cfg g	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(cosem_group_cfg), &g);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);

	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "enabled", 0, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", NULL);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_cosem_group_cfg_Expect(&g);

	TEST_ASSERT_NULL(cfg_read_cosem_group(&uci, &sec));
}

void test_cfg_read_cosem_group_invalid_interval(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	cosem_group_cfg g	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(cosem_group_cfg), &g);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);

	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "enabled", 0, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "group_one");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "interval", 1, 0);
	_log_ExpectAnyArgs();

	_log_ExpectAnyArgs();
	cfg_free_cosem_group_cfg_Expect(&g);

	TEST_ASSERT_NULL(cfg_read_cosem_group(&uci, &sec));
}

// TODO: add struct variables check ASSERTS
void test_cfg_read_cosem_group_successful(void)
{
	struct uci_context uci	= { 0 };
	struct uci_section sec	= { .e.name = "1" };
	cosem_group_cfg g	= { 0 };

	_log_ExpectAnyArgs();
	mycalloc_ExpectAndReturn(1, sizeof(cosem_group_cfg), &g);
	_log_ExpectAnyArgs();

	mystrtol_ExpectAndReturn(sec.e.name, NULL, 10, 1);

	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "enabled", 0, 1);
	ucix_get_option_cfg_ExpectAndReturn(&uci, "dlms_master", "1", "name", "group_one");
	ucix_get_option_int_ExpectAndReturn(&uci, "dlms_master", "1", "interval", 1, 50);
	_log_ExpectAnyArgs();

	TEST_ASSERT_NOT_NULL(cfg_read_cosem_group(&uci, &sec));
}

void test_cfg_get_connections_calloc_failure(void)
{
	dlms_cfg cfg = { 0 };

	mycalloc_ExpectAndReturn(0, sizeof(connection *), NULL);
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(cfg_get_connections(&cfg, NULL));
}

void test_cfg_get_connections_fail_to_get_connection(void)
{
	dlms_cfg cfg = {
		.connection_cfg_count = 1,
		.connections =
			(connection_params_cfg *[]){
				(connection_params_cfg[]){ 0 },
			},
	};

	connection **c = (connection *[]){ 0 };
	size_t conn    = 0;

	mycalloc_ExpectAndReturn(1, sizeof(connection *), c);
	cfg_get_connection_ExpectAndReturn(cfg.connections[0], NULL);
	myfree_Expect(c);

	TEST_ASSERT_NULL(cfg_get_connections(&cfg, &conn));
}

void test_cfg_get_connections_successfully_get_connection(void)
{
	dlms_cfg cfg = {
		.connection_cfg_count = 1,
		.connections =
			(connection_params_cfg *[]){
				(connection_params_cfg[]){ 0 },
			},
	};

	connection **c = (connection *[]){ 0 };
	size_t conn    = 0;

	mycalloc_ExpectAndReturn(1, sizeof(connection *), c);
	cfg_get_connection_ExpectAndReturn(cfg.connections[0], (connection []) { 0 });

	TEST_ASSERT_NOT_NULL(cfg_get_connections(&cfg, &conn));
}

void test_cfg_get_physical_devices_calloc_failure(void)
{
	dlms_cfg cfg = { 0 };

	mycalloc_ExpectAndReturn(0, sizeof(physical_device *), NULL);
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(cfg_get_physical_devices(&cfg, NULL, 0, NULL));
}

void test_cfg_get_physical_devices_fail_to_get_physical_device(void)
{
	dlms_cfg cfg = {
		.physical_device_cfg_count = 1,
		.physical_devices =
			(physical_device_cfg *[]){
				(physical_device_cfg[]){ 0 },
			},
	};

	physical_device **dev = (physical_device *[]){ 0 };
	connection **c	      = (connection *[]){ 0 };
	size_t conn	      = 0;
	size_t dev_count      = 0;

	mycalloc_ExpectAndReturn(1, sizeof(physical_device *), dev);
	cfg_get_physical_device_ExpectAndReturn(cfg.physical_devices[0], c, 0, NULL);
	myfree_Expect(dev);

	TEST_ASSERT_NULL(cfg_get_physical_devices(&cfg, c, 0, &dev_count));
}

void test_cfg_get_physical_devices_successfully_get_physical_device(void)
{
	dlms_cfg cfg = {
		.physical_device_cfg_count = 1,
		.physical_devices =
			(physical_device_cfg *[]){
				(physical_device_cfg[]){ 0 },
			},
	};

	physical_device **dev = (physical_device *[]){ 0 };
	connection **c	    = (connection *[]){ 0 };
	size_t conn	    = 1;
	size_t dev_count    = 0;

	mycalloc_ExpectAndReturn(1, sizeof(physical_device *), dev);
	cfg_get_physical_device_ExpectAndReturn(cfg.physical_devices[0], c, conn, (physical_device[]){ 0 });

	TEST_ASSERT_NOT_NULL(cfg_get_physical_devices(&cfg, c, conn, &dev_count));
}

void test_cfg_get_cosem_objects_calloc_failure(void)
{
	dlms_cfg cfg = { 0 };

	mycalloc_ExpectAndReturn(0, sizeof(cosem_object *), NULL);
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(cfg_get_cosem_objects(&cfg, NULL, 0, NULL));
}

void test_cfg_get_cosem_objects_fail_to_get_cosem_object(void)
{
	dlms_cfg cfg = {
		.cosem_object_cfg_count = 1,
		.cosem_objects =
			(cosem_object_cfg *[]){
				(cosem_object_cfg[]){ 0 },
			},
	};

	cosem_object **o = (cosem_object *[]){ 0 };
	size_t obj_count = 0;

	mycalloc_ExpectAndReturn(1, sizeof(cosem_object *), o);
	cfg_get_cosem_object_ExpectAndReturn(cfg.cosem_objects[0], NULL, 0, NULL);
	myfree_Expect(o);

	TEST_ASSERT_NULL(cfg_get_cosem_objects(&cfg, NULL, 0, &obj_count));
}

void test_cfg_get_cosem_objects_successfully_get_cosem_object(void)
{
	dlms_cfg cfg = {
		.cosem_object_cfg_count = 1,
		.cosem_objects =
			(cosem_object_cfg *[]){
				(cosem_object_cfg[]){ 0 },
			},
	};

	cosem_object **o = (cosem_object *[]){ 0 };
	size_t obj_count = 0;

	mycalloc_ExpectAndReturn(1, sizeof(cosem_object *), o);
	cfg_get_cosem_object_ExpectAndReturn(cfg.cosem_objects[0], NULL, 0, (cosem_object []) { 0 });

	TEST_ASSERT_NOT_NULL(cfg_get_cosem_objects(&cfg, NULL, 0, &obj_count));
}

void test_cfg_get_cosem_groups_calloc_failure(void)
{
	dlms_cfg cfg = { 0 };

	mycalloc_ExpectAndReturn(0, sizeof(cosem_group *), NULL);
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(cfg_get_cosem_groups(&cfg, NULL, 0, NULL));
}

void test_cfg_get_cosem_groups_fail_to_get_cosem_group(void)
{
	dlms_cfg cfg = {
		.cosem_group_cfg_count = 1,
		.cosem_groups =
			(cosem_group_cfg *[]){
				(cosem_group_cfg[]){ 0 },
			},
	};

	cosem_group **g = (cosem_group *[]){ 0 };
	size_t group_count = 0;

	mycalloc_ExpectAndReturn(1, sizeof(cosem_group *), g);
	cfg_get_cosem_group_ExpectAndReturn(&cfg, cfg.cosem_groups[0], NULL, 0, NULL);
	myfree_Expect(g);

	TEST_ASSERT_NULL(cfg_get_cosem_groups(&cfg, NULL, 0, &group_count));
}

void test_cfg_get_cosem_groups_successfully_get_cosem_group(void)
{
	dlms_cfg cfg = {
		.cosem_group_cfg_count = 1,
		.cosem_groups =
			(cosem_group_cfg *[]){
				(cosem_group_cfg[]){ 0 },
			},
	};

	cosem_group **g = (cosem_group *[]){ 0 };
	size_t group_count = 0;

	mycalloc_ExpectAndReturn(1, sizeof(cosem_group *), g);
	cfg_get_cosem_group_ExpectAndReturn(&cfg, cfg.cosem_groups[0], NULL, 0, (cosem_group []){ 0 });

	TEST_ASSERT_NOT_NULL(cfg_get_cosem_groups(&cfg, NULL, 0, &group_count));
}

void test_cfg_init_mutex_already_allocated_mutex(void)
{
	pthread_mutex_t *mutex = (pthread_mutex_t[]){ 0 };

	TEST_ASSERT_EQUAL_INT(0, cfg_init_mutex(&mutex));
}

void test_cfg_init_mutex_fail_to_allocate_memory(void)
{
	pthread_mutex_t *mutex = NULL;

	mycalloc_ExpectAndReturn(1, sizeof(pthread_mutex_t), NULL);
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL_INT(1, cfg_init_mutex(&mutex));
}

void test_cfg_init_mutex_fail_to_init_mutex(void)
{
	pthread_mutex_t *mutex	 = NULL;
	pthread_mutex_t *mutex_2 = (pthread_mutex_t[]){ 0 };

	mycalloc_ExpectAndReturn(1, sizeof(pthread_mutex_t), mutex_2);
	pthread_mutex_init_ExpectAndReturn(mutex_2, NULL, 1);
	myfree_Expect(mutex_2);

	TEST_ASSERT_EQUAL_INT(1, cfg_init_mutex(&mutex));
	TEST_ASSERT_NULL(mutex);
}

void test_cfg_init_mutex_successful(void)
{
	pthread_mutex_t *mutex	 = NULL;
	pthread_mutex_t *mutex_2 = (pthread_mutex_t[]){ 0 };

	mycalloc_ExpectAndReturn(1, sizeof(pthread_mutex_t), mutex_2);
	pthread_mutex_init_ExpectAndReturn(mutex_2, NULL, 0);

	TEST_ASSERT_EQUAL_INT(0, cfg_init_mutex(&mutex));
	TEST_ASSERT_NOT_NULL(mutex);
}

#endif
