#ifndef TEST
#define TEST
#endif
#ifdef TEST

#include "unity.h"
#include "config.h"
#include "mock_tlt_logger.h"

#include "mock_stub_external.h"
#include "stub_config_free_2.h"

#include "mock_stub_pthread.h"

#include "mock_bytebuffer.h"
#include "mock_gxobjects.h"
#include "mock_client.h"
#include "mock_dlmssettings.h"
#include "mock_cosem.h"
#include "mock_ciphering.h"

PRIVATE void cfg_free_connection_cfg(connection_params_cfg *c);
PRIVATE void cfg_free_physical_device_cfg(physical_device_cfg *d);
PRIVATE void cfg_free_cosem_object_cfg(cosem_object_cfg *o);
PRIVATE void cfg_free_cosem_group_cfg(cosem_group_cfg *g);

PRIVATE void cfg_free_connection(connection *c);
PRIVATE void cfg_free_physical_device(physical_device *d);
PRIVATE void cfg_free_cosem_object(cosem_object *o);
PRIVATE void cfg_free_cosem_group(cosem_group *g);

master *g_master = NULL;

void test_cfg_free_connection_cfg_empty_connection(void)
{
	cfg_free_connection_cfg(NULL);
}

void test_cfg_free_connection_cfg_successfully_clean_tcp(void)
{
        connection_params_cfg c = { .type = TCP_CFG };

        myfree_Expect(c.name);
        myfree_Expect(c.parameters.tcp.host);
        myfree_Expect(&c);

	cfg_free_connection_cfg(&c);
}

void test_cfg_free_connection_cfg_successfully_clean_serial(void)
{
        connection_params_cfg c = { .type = SERIAL };

        myfree_Expect(c.name);
        myfree_Expect(c.parameters.serial.device);
        myfree_Expect(c.parameters.serial.parity);
        myfree_Expect(c.parameters.serial.flow_control);
        myfree_Expect(&c);

	cfg_free_connection_cfg(&c);
}

void test_cfg_free_physical_device_cfg_empty_physical_device(void)
{
	cfg_free_physical_device_cfg(NULL);
}

void test_cfg_free_physical_device_cfg_successfully(void)
{
        physical_device_cfg d = { 0 };

        myfree_Expect(d.name);
        myfree_Expect(d.password);
        myfree_Expect(d.authentication_key);
        myfree_Expect(d.block_cipher_key);
        myfree_Expect(d.dedicated_key);
        myfree_Expect(d.invocation_counter);
        myfree_Expect(&d);

	cfg_free_physical_device_cfg(&d);
}

void test_cfg_free_cosem_object_cfg_empty_cosem_object(void)
{
	cfg_free_cosem_object_cfg(NULL);
}

void test_cfg_free_connection_cfg_successfully(void)
{
        cosem_object_cfg o = { 0 };

        myfree_Expect(o.name);
        myfree_Expect(o.physical_devices);
        myfree_Expect(o.obis);
        myfree_Expect(&o);

	cfg_free_cosem_object_cfg(&o);
}


void test_cfg_free_cosem_group_cfg_empty_cosem_group(void)
{
	cfg_free_cosem_group_cfg(NULL);
}

void test_cfg_free_cosem_group_cfg_successfully(void)
{
        cosem_group_cfg g = { 0 };

        myfree_Expect(g.name);
        myfree_Expect(&g);

	cfg_free_cosem_group_cfg(&g);
}

void test_cfg_free_connection_empty_connection(void)
{
        cfg_free_connection(NULL);
}

void test_cfg_free_connection_clean_tcp(void)
{
        connection c = { .type = TCP };

        myfree_Expect(c.name);
        bb_clear_ExpectAndReturn(&c.data, 0);
        myfree_Expect(c.parameters.tcp.host);
        pthread_mutex_destroy_ExpectAndReturn(c.mutex, 0);
        myfree_Expect(&c);

        cfg_free_connection(&c);
}

void test_cfg_free_connection_clean_serial(void)
{
        connection c = { .type = SERIAL };

        myfree_Expect(c.name);
        bb_clear_ExpectAndReturn(&c.data, 0);
        myfree_Expect(c.parameters.serial.device);
        myfree_Expect(c.parameters.serial.parity);
        myfree_Expect(c.parameters.serial.flow_control);
        myfree_Expect(&c);

        cfg_free_connection(&c);
}

void test_cfg_free_physical_device_empty_physical_device(void)
{
        cfg_free_physical_device(NULL);
}

void test_cfg_free_physical_device_successfully(void)
{
        physical_device d = { 0 };

	cip_clear_Expect(&d.settings.cipher);
	cl_clear_Expect(&d.settings);

        myfree_Expect(d.name);
        myfree_Expect(d.invocation_counter);
        myfree_Expect(&d);

        cfg_free_physical_device(&d);
}

void test_cfg_free_cosem_object_empty_cosem_object(void)
{
        cfg_free_cosem_object(NULL);
}

void test_cfg_free_cosem_object_successfully(void)
{
        cosem_object o = { 0 };

        obj_clear_Expect(o.object);
        myfree_Expect(o.name);
        myfree_Expect(o.object);
        myfree_Expect(o.devices);
        myfree_Expect(&o);

        cfg_free_cosem_object(&o);
}

void test_cfg_free_cosem_group_empty_cosem_group(void)
{
        cfg_free_cosem_group(NULL);
}

void test_cfg_free_cosem_group_successfully(void)
{
        cosem_group g = { 0 };

        myfree_Expect(g.name);
        myfree_Expect(g.cosem_objects);
        myfree_Expect(&g);

        cfg_free_cosem_group(&g);
}

#endif
