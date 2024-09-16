#ifndef TEST
#define TEST
#endif
#ifdef TEST

#include "unity.h"
#include "config.h"
#include "mock_tlt_logger.h"

#include "mock_stub_external.h"
#include "mock_stub_config_free.h"

#include "mock_stub_pthread.h"

#include "mock_bytebuffer.h"
#include "mock_gxobjects.h"
#include "mock_client.h"
#include "mock_dlmssettings.h"
#include "mock_cosem.h"
#include "mock_ciphering.h"

PRIVATE void cfg_free_dlms_cfg(dlms_cfg *dlms);
PUBLIC void cfg_free_master(master *m);

master *g_master = NULL;

void test_cfg_free_dlms_cfg_empty_dlms_cfg(void)
{
	cfg_free_dlms_cfg(NULL);
}

void test_cfg_free_dlms_cfg_successfully(void)
{
	dlms_cfg cfg = {
		.connection_cfg_count	   = 1,
		.connections		   = (connection_params_cfg *[]) {
                        (connection_params_cfg []) {
                                { 0 }
                        },
                },
		.physical_device_cfg_count = 1,
		.physical_devices	   = (physical_device_cfg *[]) {
                        (physical_device_cfg []) {
                                { 0 }
                        },
                },
		.cosem_object_cfg_count	   = 1,
		.cosem_objects		   = (cosem_object_cfg *[]) {
                        (cosem_object_cfg []) {
                                { 0 }
                        },
                },
		.cosem_group_cfg_count	   = 1,
		.cosem_groups		   = (cosem_group_cfg *[]) {
                        (cosem_group_cfg []) {
                                { 0 }
                        },
                },
	};

        cfg_free_connection_cfg_Expect(cfg.connections[0]);
        cfg_free_physical_device_cfg_Expect(cfg.physical_devices[0]);
        cfg_free_cosem_object_cfg_Expect(cfg.cosem_objects[0]);
        cfg_free_cosem_group_cfg_Expect(cfg.cosem_groups[0]);

        myfree_Expect(cfg.connections);
        myfree_Expect(cfg.physical_devices);
        myfree_Expect(cfg.cosem_objects);
        myfree_Expect(cfg.cosem_groups);
        myfree_Expect(&cfg);

	cfg_free_dlms_cfg(&cfg);
}

void test_cfg_free_dlms_empty_master(void)
{
	cfg_free_master(NULL);
}

void test_cfg_free_master_successfully(void)
{
	master m = {
		.connection_count = 1,
		.connections =
			(connection *[]){
				(connection[]){
					{ 0 },
				},
			},
		.physical_dev_count = 1,
		.physical_devices =
			(physical_device *[]){
				(physical_device[]){
					{ 0 },
				},
			},
		.cosem_object_count = 1,
		.cosem_objects =
			(cosem_object *[]){
				(cosem_object[]){
					{ 0 },
				},
			},
		.cosem_group_count = 1,
		.cosem_groups =
			(cosem_group *[]){
				(cosem_group[]){
					{ 0 },
				},
			},
	};

	cfg_free_connection_Expect(m.connections[0]);
        cfg_free_physical_device_Expect(m.physical_devices[0]);
        cfg_free_cosem_object_Expect(m.cosem_objects[0]);
        cfg_free_cosem_group_Expect(m.cosem_groups[0]);

        pthread_mutex_destroy_ExpectAndReturn(m.mutex_rs232, 0);
        myfree_Expect(m.mutex_rs232);
        pthread_mutex_destroy_ExpectAndReturn(m.mutex_rs485, 0);
        myfree_Expect(m.mutex_rs485);

        myfree_Expect(m.connections);
        myfree_Expect(m.physical_devices);
        myfree_Expect(m.cosem_objects);
        myfree_Expect(m.cosem_groups);
        myfree_Expect(&m);

	cfg_free_master(&m);
}

#endif
