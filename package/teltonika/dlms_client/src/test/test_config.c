#ifndef TEST
#define TEST
#endif
#ifdef TEST

#include "unity.h"
#include "config.h"
#include "mock_tlt_logger.h"

#include "mock_stub_external.h"
#include "mock_stub_config.h"

#include "mock_stub_uci.h"

#include "mock_bytebuffer.h"
#include "mock_gxobjects.h"
#include "mock_client.h"
#include "mock_dlmssettings.h"
#include "mock_cosem.h"
#include "mock_ciphering.h"

PRIVATE dlms_cfg *cfg_read_dlms_cfg();
PRIVATE master *cfg_read_master(dlms_cfg *cfg);

PRIVATE connection *cfg_get_connection(connection_params_cfg *cfg);
PRIVATE physical_device *cfg_get_physical_device(physical_device_cfg *cfg, connection **connections, size_t connection_count);
PRIVATE cosem_object *cfg_get_cosem_object(cosem_object_cfg *cfg, physical_device **physical_devices, size_t physical_dev_count);
PRIVATE cosem_group *cfg_get_cosem_group(dlms_cfg *cfg, cosem_group_cfg *cosem_group_cfg, cosem_object **cosem_objects, size_t cosem_object_count);

PRIVATE int cfg_init_mutexes(master *m);

master *g_master = NULL;

void test_cfg_read_dlms_cfg_calloc_failure(void)
{
	mycalloc_ExpectAndReturn(1, sizeof(dlms_cfg), NULL);
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(cfg_read_dlms_cfg());
}

void test_cfg_read_dlms_cfg_uci_alloc_failure(void)
{
	dlms_cfg cfg = { 0 };

	mycalloc_ExpectAndReturn(1, sizeof(dlms_cfg), &cfg);

	uci_alloc_context_ExpectAndReturn(NULL);
	_log_ExpectAnyArgs();

	uci_free_context_Ignore();
	cfg_free_dlms_cfg_Expect(&cfg);

	TEST_ASSERT_NULL(cfg_read_dlms_cfg());
}

void test_cfg_read_dlms_cfg_uci_load_returns_error(void)
{
	struct uci_context uci	= { 0 };
	struct uci_package *pkg = NULL;
	dlms_cfg cfg		= { 0 };

	mycalloc_ExpectAndReturn(1, sizeof(dlms_cfg), &cfg);
	uci_alloc_context_ExpectAndReturn(&uci);

	uci_load_ExpectAndReturn(&uci, "dlms_master", &pkg, 0);
	uci_load_IgnoreArg_package();
	_log_ExpectAnyArgs();

	uci_free_context_Ignore();
	cfg_free_dlms_cfg_Expect(&cfg);

	TEST_ASSERT_NULL(cfg_read_dlms_cfg());
}

void test_cfg_read_dlms_cfg_connections_are_not_found(void)
{
	struct uci_context uci	= { 0 };
	struct uci_package *pkg = (struct uci_package []) { 0 };
	dlms_cfg cfg		= { 0 };
	struct uci_section sec1 = { .type = "master" };
	pkg->sections.next = &sec1.e.list;
	sec1.e.list.next   = &pkg->sections;

	mycalloc_ExpectAndReturn(1, sizeof(dlms_cfg), &cfg);
	uci_alloc_context_ExpectAndReturn(&uci);

	uci_load_ExpectAndReturn(&uci, "dlms_master", &pkg, 0);
	uci_load_ReturnThruPtr_package(&pkg);
	uci_load_IgnoreArg_package();

	mystrcmp_ExpectAndReturn(sec1.type, "connection", 1);
	_log_ExpectAnyArgs();

	uci_free_context_Ignore();
	cfg_free_dlms_cfg_Expect(&cfg);

	TEST_ASSERT_NULL(cfg_read_dlms_cfg());
}

void test_cfg_read_dlms_cfg_uci_fail_to_read_connection_section(void)
{
	struct uci_context uci	= { 0 };
	struct uci_package *pkg = (struct uci_package []) { 0 };
	dlms_cfg cfg		= { 0 };
	struct uci_section sec1 = { .type = "connection" };
	pkg->sections.next = &sec1.e.list;
	sec1.e.list.next   = &pkg->sections;

	mycalloc_ExpectAndReturn(1, sizeof(dlms_cfg), &cfg);
	uci_alloc_context_ExpectAndReturn(&uci);

	uci_load_ExpectAndReturn(&uci, "dlms_master", &pkg, 0);
	uci_load_ReturnThruPtr_package(&pkg);
	uci_load_IgnoreArg_package();

	mystrcmp_ExpectAndReturn(sec1.type, "connection", 0);
	cfg_read_connection_ExpectAndReturn(&uci, &sec1, NULL);

	_log_ExpectAnyArgs();
	uci_free_context_Ignore();
	cfg_free_dlms_cfg_Expect(&cfg);

	TEST_ASSERT_NULL(cfg_read_dlms_cfg());
}


void test_cfg_read_dlms_cfg_physical_devices_are_not_found(void)
{
	struct uci_context uci	= { 0 };
	struct uci_package *pkg = (struct uci_package []) { 0 };
	dlms_cfg cfg		= { 0 };
	struct uci_section sec1 = { .type = "connection" };
	pkg->sections.next = &sec1.e.list;
	sec1.e.list.next   = &pkg->sections;

	mycalloc_ExpectAndReturn(1, sizeof(dlms_cfg), &cfg);
	uci_alloc_context_ExpectAndReturn(&uci);

	uci_load_ExpectAndReturn(&uci, "dlms_master", &pkg, 0);
	uci_load_ReturnThruPtr_package(&pkg);
	uci_load_IgnoreArg_package();

	mystrcmp_ExpectAndReturn(sec1.type, "connection", 0);
	cfg_read_connection_ExpectAndReturn(&uci, &sec1, (connection_params_cfg []) { 0 });
	myrealloc_ExpectAndReturn(cfg.connections,
				  (cfg.connection_cfg_count + 1) * sizeof(connection_params_cfg),
				  (connection_params_cfg[]){ 0 });

	mystrcmp_ExpectAndReturn(sec1.type, "physical_device", 1);
	_log_ExpectAnyArgs();

	uci_free_context_Ignore();
	cfg_free_dlms_cfg_Expect(&cfg);

	TEST_ASSERT_NULL(cfg_read_dlms_cfg());
}

void test_cfg_read_dlms_cfg_uci_fail_to_read_physical_device_section(void)
{
	struct uci_context uci	= { 0 };
	struct uci_package *pkg = (struct uci_package []) { 0 };
	dlms_cfg cfg		= { 0 };
	struct uci_section sec1 = { .type = "connection" };
	struct uci_section sec2 = { .type = "physical_device" };
	pkg->sections.next = &sec1.e.list;
	sec1.e.list.next   = &sec2.e.list;
	sec2.e.list.next   = &pkg->sections;

	mycalloc_ExpectAndReturn(1, sizeof(dlms_cfg), &cfg);
	uci_alloc_context_ExpectAndReturn(&uci);

	uci_load_ExpectAndReturn(&uci, "dlms_master", &pkg, 0);
	uci_load_ReturnThruPtr_package(&pkg);
	uci_load_IgnoreArg_package();

	mystrcmp_ExpectAndReturn(sec1.type, "connection", 0);
	cfg_read_connection_ExpectAndReturn(&uci, &sec1, (connection_params_cfg []) { 0 });
	myrealloc_ExpectAndReturn(cfg.connections,
				  (cfg.connection_cfg_count + 1) * sizeof(connection_params_cfg),
				  (connection_params_cfg[]){ 0 });
	mystrcmp_ExpectAndReturn(sec2.type, "connection", 1);

	mystrcmp_ExpectAndReturn(sec1.type, "physical_device", 1);
	mystrcmp_ExpectAndReturn(sec2.type, "physical_device", 0);
	cfg_read_physical_device_ExpectAndReturn(&uci, &sec2, NULL);

	_log_ExpectAnyArgs();
	uci_free_context_Ignore();
	cfg_free_dlms_cfg_Expect(&cfg);

	TEST_ASSERT_NULL(cfg_read_dlms_cfg());
}

void test_cfg_read_dlms_cfg_uci_read_all_sections(void)
{
	struct uci_context uci	= { 0 };
	struct uci_package *pkg = (struct uci_package []) { 0 };
	dlms_cfg cfg		= { 0 };
	struct uci_section sec1 = { .type = "connection" };
	struct uci_section sec2 = { .type = "physical_device" };
	struct uci_section sec3 = { .type = "cosem" };
	struct uci_section sec4 = { .type = "cosem_group" };

	pkg->sections.next = &sec1.e.list;
	sec1.e.list.next   = &sec2.e.list;
	sec2.e.list.next   = &sec3.e.list;
	sec3.e.list.next   = &sec4.e.list;
	sec4.e.list.next   = &pkg->sections;

	mycalloc_ExpectAndReturn(1, sizeof(dlms_cfg), &cfg);
	uci_alloc_context_ExpectAndReturn(&uci);

	uci_load_ExpectAndReturn(&uci, "dlms_master", &pkg, 0);
	uci_load_ReturnThruPtr_package(&pkg);
	uci_load_IgnoreArg_package();

	mystrcmp_ExpectAndReturn(sec1.type, "connection", 0);
	cfg_read_connection_ExpectAndReturn(&uci, &sec1, (connection_params_cfg []) { 0 });
	myrealloc_ExpectAndReturn(cfg.connections,
				  (cfg.connection_cfg_count + 1) * sizeof(connection_params_cfg),
				  (connection_params_cfg[]){ 0 });
	mystrcmp_ExpectAndReturn(sec2.type, "connection", 1);
	mystrcmp_ExpectAndReturn(sec3.type, "connection", 1);
	mystrcmp_ExpectAndReturn(sec4.type, "connection", 1);

	mystrcmp_ExpectAndReturn(sec1.type, "physical_device", 1);
	mystrcmp_ExpectAndReturn(sec2.type, "physical_device", 0);
	cfg_read_physical_device_ExpectAndReturn(&uci, &sec2, (physical_device_cfg []) { 0 });
	myrealloc_ExpectAndReturn(cfg.physical_devices,
				  (cfg.physical_device_cfg_count + 1) * sizeof(physical_device_cfg),
				  (physical_device_cfg[]){ 0 });
	mystrcmp_ExpectAndReturn(sec3.type, "physical_device", 1);
	mystrcmp_ExpectAndReturn(sec4.type, "physical_device", 1);

	mystrcmp_ExpectAndReturn(sec1.type, "cosem", 1);
	mystrcmp_ExpectAndReturn(sec2.type, "cosem", 1);
	mystrcmp_ExpectAndReturn(sec3.type, "cosem", 0);
	cfg_read_cosem_object_ExpectAndReturn(&uci, &sec3, NULL);
	mystrcmp_ExpectAndReturn(sec4.type, "cosem", 1);

	mystrcmp_ExpectAndReturn(sec1.type, "cosem_group", 1);
	mystrcmp_ExpectAndReturn(sec2.type, "cosem_group", 1);
	mystrcmp_ExpectAndReturn(sec3.type, "cosem_group", 1);
	mystrcmp_ExpectAndReturn(sec4.type, "cosem_group", 0);
	cfg_read_cosem_group_ExpectAndReturn(&uci, &sec4, NULL);

	uci_free_context_Ignore();

	TEST_ASSERT_NOT_NULL(cfg_read_dlms_cfg());
}

void test_cfg_read_dlms_cfg_uci_check_limits(void)
{
	struct uci_context uci	= { 0 };
	struct uci_package *pkg = (struct uci_package []) { 0 };
	dlms_cfg cfg		= {
			   .connection_cfg_count      = 9,
			   .physical_device_cfg_count = 29,
			   .cosem_group_cfg_count     = 9,
	};
	struct uci_section sec1 = { .type = "connection" };
	struct uci_section sec2 = { .type = "physical_device" };
	struct uci_section sec3 = { .type = "cosem" };
	struct uci_section sec4 = { .type = "cosem_group" };

	pkg->sections.next = &sec1.e.list;
	sec1.e.list.next   = &sec2.e.list;
	sec2.e.list.next   = &sec3.e.list;
	sec3.e.list.next   = &sec4.e.list;
	sec4.e.list.next   = &pkg->sections;

	mycalloc_ExpectAndReturn(1, sizeof(dlms_cfg), &cfg);
	uci_alloc_context_ExpectAndReturn(&uci);

	uci_load_ExpectAndReturn(&uci, "dlms_master", &pkg, 0);
	uci_load_ReturnThruPtr_package(&pkg);
	uci_load_IgnoreArg_package();

	mystrcmp_ExpectAndReturn(sec1.type, "connection", 0);
	cfg_read_connection_ExpectAndReturn(&uci, &sec1, (connection_params_cfg []) { 0 });
	myrealloc_ExpectAndReturn(cfg.connections, (cfg.connection_cfg_count + 1) * sizeof(connection_params_cfg), (connection_params_cfg []) { 0 });
	_log_ExpectAnyArgs();

	mystrcmp_ExpectAndReturn(sec1.type, "physical_device", 1);
	mystrcmp_ExpectAndReturn(sec2.type, "physical_device", 0);
	cfg_read_physical_device_ExpectAndReturn(&uci, &sec2, (physical_device_cfg []) { 0 });
	myrealloc_ExpectAndReturn(cfg.physical_devices, (cfg.physical_device_cfg_count + 1) * sizeof(physical_device_cfg), (physical_device_cfg []) { 0 });
	_log_ExpectAnyArgs();

	mystrcmp_ExpectAndReturn(sec1.type, "cosem", 1);
	mystrcmp_ExpectAndReturn(sec2.type, "cosem", 1);
	mystrcmp_ExpectAndReturn(sec3.type, "cosem", 0);
	cfg_read_cosem_object_ExpectAndReturn(&uci, &sec3, (cosem_object_cfg []) { 0 });
	myrealloc_ExpectAndReturn(cfg.cosem_objects, (cfg.cosem_object_cfg_count + 1) * sizeof(cosem_object_cfg), (cosem_object_cfg []) { 0 });
	mystrcmp_ExpectAndReturn(sec4.type, "cosem", 1);

	mystrcmp_ExpectAndReturn(sec1.type, "cosem_group", 1);
	mystrcmp_ExpectAndReturn(sec2.type, "cosem_group", 1);
	mystrcmp_ExpectAndReturn(sec3.type, "cosem_group", 1);
	mystrcmp_ExpectAndReturn(sec4.type, "cosem_group", 0);
	cfg_read_cosem_group_ExpectAndReturn(&uci, &sec4, (cosem_group_cfg []) { 0 });
	myrealloc_ExpectAndReturn(cfg.cosem_groups, (cfg.cosem_group_cfg_count + 1) * sizeof(cosem_group_cfg), (cosem_group_cfg []) { 0 });
	_log_ExpectAnyArgs();

	uci_free_context_Ignore();

	TEST_ASSERT_NOT_NULL(cfg_read_dlms_cfg());
	TEST_ASSERT_EQUAL(cfg.connection_cfg_count, 10);
	TEST_ASSERT_EQUAL(cfg.physical_device_cfg_count, 30);
	TEST_ASSERT_EQUAL(cfg.cosem_group_cfg_count, 10);
}

void test_cfg_read_master_calloc_failure(void)
{
	mycalloc_ExpectAndReturn(1, sizeof(master), NULL);
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(cfg_read_master(NULL));
}

void test_cfg_read_master_connections_is_null(void)
{
	dlms_cfg cfg = { 0 };
	master m     = { 0 };

	size_t connections_count     = 0;

	mycalloc_ExpectAndReturn(1, sizeof(master), &m);

	cfg_get_connections_ExpectAndReturn(&cfg, &connections_count, NULL);
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(cfg_read_master(&cfg));
}

void test_cfg_read_master_physical_devices_is_null(void)
{
	dlms_cfg cfg = { 0 };
	master m     = { 0 };

	size_t devices_count	     = 0;
	size_t connections_count     = 0;
	size_t connections_count_ret = 1;

	mycalloc_ExpectAndReturn(1, sizeof(master), &m);

	connection **c = (connection *[]) { 0 };

	cfg_get_connections_ExpectAndReturn(&cfg, &connections_count, c);
	cfg_get_connections_ReturnThruPtr_connection_count(&connections_count_ret);
	cfg_get_physical_devices_ExpectAndReturn(&cfg, c, 1, &devices_count, NULL);
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(cfg_read_master(&cfg));
}

void test_cfg_read_master_physical_successful(void)
{
	dlms_cfg cfg	      = { 0 };
	master m	      = { 0 };
	physical_device **dev = (physical_device *[]){ 0 };
	connection **c	      = (connection *[]){ 0 };

	size_t devices_count	     = 0;
	size_t connections_count     = 0;
	size_t connections_count_ret = 1;
	size_t cosem_object_count    = 0;
	size_t cosem_group_count     = 0;

	mycalloc_ExpectAndReturn(1, sizeof(master), &m);

	cfg_get_connections_ExpectAndReturn(&cfg, &connections_count, c);
	cfg_get_connections_ReturnThruPtr_connection_count(&connections_count_ret);
	cfg_get_physical_devices_ExpectAndReturn(&cfg, c, 1, &devices_count, dev);
	cfg_get_physical_devices_ReturnThruPtr_physical_dev_count(&connections_count_ret);
	cfg_get_cosem_objects_ExpectAndReturn(&cfg, dev, 1, &cosem_object_count, NULL);
	cfg_get_cosem_groups_ExpectAndReturn(&cfg, NULL, 0, &cosem_group_count, NULL);

	TEST_ASSERT_NOT_NULL(cfg_read_master(&cfg));
}

void test_cfg_get_connection_calloc_failure(void)
{
	connection_params_cfg cfg = { 0 };

	mycalloc_ExpectAndReturn(1, sizeof(connection), NULL);
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(cfg_get_connection(&cfg));
}

////////////////////  TODO: ASSERT ALL STRUCT VARIABLES <<<<<<<<<<<<<<<<<<<<<<<
void test_cfg_get_connection_TCP_connection_type(void)
{
	connection_params_cfg cfg = { 0 };
	connection *c = (connection []) { 0 };

	mycalloc_ExpectAndReturn(1, sizeof(connection), c);

	mystrdup_ExpectAndReturn(cfg.name, "");
	mystrdup_ExpectAndReturn(cfg.parameters.tcp.host, "");

	bb_init_ExpectAndReturn(&c->data, 0);
	bb_capacity_ExpectAndReturn(&c->data, 500, 0);

	TEST_ASSERT_NOT_NULL(cfg_get_connection(&cfg));
}

void test_cfg_get_connection_SERIAL_connection_type(void)
{
	connection_params_cfg cfg = { .type = 1 };
	connection *c = (connection []) { 0 };

	mycalloc_ExpectAndReturn(1, sizeof(connection), c);

	mystrdup_ExpectAndReturn(cfg.name, "");
	mystrdup_ExpectAndReturn(cfg.parameters.serial.device, "");
	mystrdup_ExpectAndReturn(cfg.parameters.serial.parity, "");
	mystrdup_ExpectAndReturn(cfg.parameters.serial.flow_control, "");

	bb_init_ExpectAndReturn(&c->data, 0);
	bb_capacity_ExpectAndReturn(&c->data, 500, 0);

	TEST_ASSERT_NOT_NULL(cfg_get_connection(&cfg));
}

void test_cfg_get_connection_unknown_connection_type(void)
{
	connection_params_cfg cfg = { .type = 9 };
	connection *c = (connection []) { 0 };

	mycalloc_ExpectAndReturn(1, sizeof(connection), c);

	mystrdup_ExpectAndReturn(cfg.name, NULL);

	_log_ExpectAnyArgs();

	myfree_Expect(c->name);
	myfree_Expect(c);

	TEST_ASSERT_NULL(cfg_get_connection(&cfg));
}

void test_cfg_get_physical_device_calloc_failure(void)
{
	physical_device_cfg cfg = { 0 };
	connection **connections  = (connection *[]){ 0 };
	size_t connection_count	  = 0;

	mycalloc_ExpectAndReturn(1, sizeof(physical_device), NULL);
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(cfg_get_physical_device(&cfg, connections, connection_count));
}

void test_cfg_get_physical_device_fail_to_find_connection(void)
{
	physical_device_cfg cfg = { .invocation_counter = "0.4.5.6.7.8", .name = "labas" };
	connection **connections  = (connection *[]){ 0 };
	size_t connection_count	  = 0;

	physical_device *dev = (physical_device[]) { 0 };

	mycalloc_ExpectAndReturn(1, sizeof(physical_device), dev);
	mystrdup_ExpectAndReturn(cfg.name, cfg.name);
	mystrdup_ExpectAndReturn(cfg.invocation_counter, cfg.invocation_counter);
	_log_ExpectAnyArgs();

	myfree_Ignore();

	TEST_ASSERT_NULL(cfg_get_physical_device(&cfg, connections, connection_count));
}

void test_cfg_get_physical_device_successfully_get_device(void)
{
	physical_device_cfg cfg = {
		.invocation_counter = "0.4.5.6.7.8",
		.connection	    = 4,
		.authentication_key = "987654321",
		.block_cipher_key   = "7777",
		.dedicated_key	    = "123456",
	};
	connection **connections  = (connection *[]){
		 (connection[]){
			 { .id = 4 },
		 },
	};
	size_t connection_count = 1;

	mycalloc_ExpectAndReturn(1, sizeof(physical_device), (physical_device[]){ 0 });
	mystrdup_ExpectAndReturn(cfg.name, "");
	mystrdup_ExpectAndReturn(cfg.invocation_counter, "");

	// TODO: take real life example, calculate serveraddress return yourself
	cl_getServerAddress_IgnoreAndReturn(0);
	cl_init_Ignore();
	bb_clear_IgnoreAndReturn(0);
	bb_addHexString_IgnoreAndReturn(0);
	bb_init_IgnoreAndReturn(0);
	mycalloc_ExpectAndReturn(1, sizeof(gxByteBuffer), (gxByteBuffer[]) { 0 });

	TEST_ASSERT_NOT_NULL(cfg_get_physical_device(&cfg, connections, connection_count));
}

void test_cfg_get_cosem_object_calloc_failure(void)
{
	cosem_object_cfg cfg = { 0 };

	mycalloc_ExpectAndReturn(1, sizeof(cosem_object), NULL);
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(cfg_get_cosem_object(&cfg, NULL, 0));
}

void test_cfg_get_cosem_object_fail_to_find_physical_devices(void)
{
	cosem_object_cfg cfg = { 0 };
	cosem_object *o	     = (cosem_object[]){ 0 };

	mycalloc_ExpectAndReturn(1, sizeof(cosem_object), o);

	mystrdup_ExpectAndReturn(cfg.name, "");
	char *tok_save = NULL;
	mystrtok_r_ExpectAndReturn(cfg.physical_devices, " ", &tok_save, NULL);
	mystrtok_r_IgnoreArg_saveptr();
	_log_ExpectAnyArgs();

	myfree_Ignore();

	TEST_ASSERT_NULL(cfg_get_cosem_object(&cfg, NULL, 0));
}

void test_cfg_get_cosem_object_fail_to_create_object(void)
{
	cosem_object_cfg cfg = { .physical_devices = "4" };
	cosem_object *o	     = (cosem_object[]){ 0 };
	physical_device **dev = (physical_device *[]){
		(physical_device[]){
			{ .id = 4 },
		},
	};

	mycalloc_ExpectAndReturn(1, sizeof(cosem_object), o);

	mystrdup_ExpectAndReturn(cfg.name, "");
	char *tok_save = NULL;
	mystrtok_r_ExpectAndReturn(cfg.physical_devices, " ", &tok_save, cfg.physical_devices);
	mystrtok_r_IgnoreArg_saveptr();
	mystrtol_ExpectAnyArgsAndReturn(4);


	myrealloc_ExpectAndReturn(o->devices, 1 * sizeof(physical_device *), (physical_device *[]) { 0 });
	mystrtok_r_ExpectAndReturn(NULL, " ", &tok_save, NULL);
	mystrtok_r_IgnoreArg_saveptr();

	cosem_createObject2_ExpectAndReturn(cfg.cosem_id, cfg.obis, &o->object, 1);
	_log_ExpectAnyArgs();
	myfree_Ignore();

	TEST_ASSERT_NULL(cfg_get_cosem_object(&cfg, dev, 1));
}

void test_cfg_get_cosem_object_fail_to_init_object(void)
{
	cosem_object_cfg cfg  = { .physical_devices = "4" };
	cosem_object *o	      = (cosem_object[]){ 0 };
	physical_device **dev = (physical_device *[]){
		(physical_device[]){
			{ .id = 4 },
		},
	};

	mycalloc_ExpectAndReturn(1, sizeof(cosem_object), o);

	mystrdup_ExpectAndReturn(cfg.name, "");
	char *tok_save = NULL;
	mystrtok_r_ExpectAndReturn(cfg.physical_devices, " ", &tok_save, cfg.physical_devices);
	mystrtok_r_IgnoreArg_saveptr();
	mystrtol_ExpectAnyArgsAndReturn(4);

	myrealloc_ExpectAndReturn(o->devices, 1 * sizeof(physical_device *), (physical_device *[]) { 0 });
	mystrtok_r_ExpectAndReturn(NULL, " ", &tok_save, NULL);
	mystrtok_r_IgnoreArg_saveptr();

	cosem_createObject2_ExpectAndReturn(cfg.cosem_id, cfg.obis, &o->object, 0);
	cosem_init_ExpectAndReturn(o->object, cfg.cosem_id, cfg.obis, 1);
	_log_ExpectAnyArgs();
	myfree_Ignore();

	TEST_ASSERT_NULL(cfg_get_cosem_object(&cfg, dev, 1));
}

void test_cfg_get_cosem_object_successfully_get_devices(void)
{
	cosem_object_cfg cfg = { .physical_devices = "4" };
	cosem_object *o	     = (cosem_object[]){ 0 };
	physical_device **dev = (physical_device *[]){
		(physical_device[]){
			{ .id = 4 },
		},
	};

	mycalloc_ExpectAndReturn(1, sizeof(cosem_object), o);

	mystrdup_ExpectAndReturn(cfg.name, "");
	char *tok_save = NULL;
	mystrtok_r_ExpectAndReturn(cfg.physical_devices, " ", &tok_save, cfg.physical_devices);
	mystrtok_r_IgnoreArg_saveptr();
	mystrtol_ExpectAnyArgsAndReturn(4);


	myrealloc_ExpectAndReturn(o->devices, 1 * sizeof(physical_device *), (physical_device *[]) { 0 });


	mystrtok_r_ExpectAndReturn(NULL, " ", &tok_save, NULL);
	mystrtok_r_IgnoreArg_saveptr();

	cosem_createObject2_ExpectAndReturn(cfg.cosem_id, cfg.obis, &o->object, 0);
	cosem_init_ExpectAndReturn(o->object, cfg.cosem_id, cfg.obis, 0);

	TEST_ASSERT_NOT_NULL(cfg_get_cosem_object(&cfg, dev, 1));
}

void test_cfg_get_cosem_group_calloc_failure(void)
{
	mycalloc_ExpectAndReturn(1, sizeof(cosem_group), NULL);
	_log_ExpectAnyArgs();

	TEST_ASSERT_NULL(cfg_get_cosem_group(NULL, NULL, NULL, 0));
}

void test_cfg_get_cosem_group_zero_cosem_objects(void)
{
	dlms_cfg cfg	      = { 0 };
	cosem_group g	      = { 0 };
	cosem_group_cfg g_cfg = { 0 };

	mycalloc_ExpectAndReturn(1, sizeof(cosem_group), &g);
	mystrdup_ExpectAndReturn(g_cfg.name, "");

	_log_ExpectAnyArgs();
	myfree_Ignore();

	TEST_ASSERT_NULL(cfg_get_cosem_group(&cfg, &g_cfg, NULL, 0));
}

void test_cfg_get_cosem_group_fail_to_find_cosem_objects(void)
{
	dlms_cfg cfg = {
		.cosem_object_cfg_count = 1,
		.cosem_objects =
			(cosem_object_cfg *[]){
				(cosem_object_cfg[]){
					{ .id = 3 },
				},
			},
	};
	cosem_group g	      = { 0 };
	cosem_group_cfg g_cfg = { 0 };
	cosem_object **obj    = (cosem_object *[]){
		   (cosem_object[]){
			{ .id = 4 }
		   },
	};

	mycalloc_ExpectAndReturn(1, sizeof(cosem_group), &g);
	mystrdup_ExpectAndReturn(g_cfg.name, "");

	_log_ExpectAnyArgs();
	myfree_Ignore();

	TEST_ASSERT_NULL(cfg_get_cosem_group(&cfg, &g_cfg, obj, 1));
}

#undef calloc
void test_cfg_get_cosem_group_check_limits(void)
{
	dlms_cfg cfg = {
		.cosem_object_cfg_count = 1,
		.cosem_objects =
			(cosem_object_cfg *[]){
				(cosem_object_cfg[]){
					{ .id = 4 },
				},
			},
	};
	cosem_group g	      = { .cosem_object_count = 20 };
	cosem_group_cfg g_cfg = { 0 };
	cosem_object **obj    = (cosem_object *[]){
		   (cosem_object[]){
			{ .id = 4 }
		   },
	};

	mycalloc_ExpectAndReturn(1, sizeof(cosem_group), &g);
	mystrdup_ExpectAndReturn(g_cfg.name, "");
	myrealloc_ExpectAndReturn(g.cosem_objects, (g.cosem_object_count + 1) * sizeof(cosem_object *), (cosem_object[20]){ 0 });
	_log_ExpectAnyArgs();

	TEST_ASSERT_NOT_NULL(cfg_get_cosem_group(&cfg, &g_cfg, obj, 1));
}

void test_cfg_get_cosem_group_successfully_get_cosem_group(void)
{
	dlms_cfg cfg = {
		.cosem_object_cfg_count = 1,
		.cosem_objects =
			(cosem_object_cfg *[]){
				(cosem_object_cfg[]){
					{ .id = 4 },
				},
			},
	};
	cosem_group g	      = { 0 };
	cosem_group_cfg g_cfg = { 0 };
	cosem_object **obj    = (cosem_object *[]){
		   (cosem_object[]){
			{ .id = 4 }
		   },
	};

	mycalloc_ExpectAndReturn(1, sizeof(cosem_group), &g);
	mystrdup_ExpectAndReturn(g_cfg.name, "");
	myrealloc_ExpectAndReturn(g.cosem_objects, (g.cosem_object_count + 1) * sizeof(cosem_object *), (cosem_object []) { 0 });

	TEST_ASSERT_NOT_NULL(cfg_get_cosem_group(&cfg, &g_cfg, obj, 1));
}

void test_cfg_init_mutexes_connection_count_is_zero(void)
{
	master m = { 0 };

	TEST_ASSERT_EQUAL(0, cfg_init_mutexes(&m));
}

void test_cfg_init_mutexes_connection_tcp_fail_to_initiate(void)
{
	master m = {
		.connection_count = 1,
		.connections	  = (connection *[]){ (connection[]){ {
			     .type = TCP,
			     .id   = 4,
		     } } },
	};

	cfg_init_mutex_ExpectAndReturn(&m.connections[0]->mutex, 1);
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL(1, cfg_init_mutexes(&m));
}

void test_cfg_init_mutexes_connection_serial_rs232_fail_to_initiate(void)
{
	master m = {
		.connection_count = 1,
		.connections	  = (connection *[]){ (connection[]){ {
			     .type = SERIAL,
			     .id   = 4,
		     } } },
	};

	mystrstr_ExpectAndReturn(m.connections[0]->parameters.serial.device, "rs485", NULL);
	cfg_init_mutex_ExpectAndReturn(&m.mutex_rs232, 1);
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL(1, cfg_init_mutexes(&m));
}

void test_cfg_init_mutexes_connection_serial_rs485_fail_to_initiate(void)
{
	master m = {
		.connection_count = 1,
		.connections	  = (connection *[]){ (connection[]){ {
			     .type = SERIAL,
			     .id   = 4,
		     } } },
	};

	mystrstr_ExpectAndReturn(m.connections[0]->parameters.serial.device, "rs485", "r");
	cfg_init_mutex_ExpectAndReturn(&m.mutex_rs485, 1);
	_log_ExpectAnyArgs();

	TEST_ASSERT_EQUAL(1, cfg_init_mutexes(&m));
}

void test_cfg_init_mutexes_connection_tcp_succesfful_initiation(void)
{
	master m = {
		.connection_count = 1,
		.connections	  = (connection *[]){ (connection[]){ {
			     .type = TCP,
			     .id   = 4,
		     } } },
	};

	cfg_init_mutex_ExpectAndReturn(&m.connections[0]->mutex, 0);

	TEST_ASSERT_EQUAL(0, cfg_init_mutexes(&m));
}

#endif
