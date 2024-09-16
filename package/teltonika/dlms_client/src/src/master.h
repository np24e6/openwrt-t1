#pragma once

#ifdef TEST
#include "stub_external.h"
#else
#define PRIVATE static
#define PUBLIC
#define MAIN main
#define TEST_BREAK
#endif

#include <libubox/uloop.h>
#include <libubox/utils.h>
#include <tlt_logger.h>
#include <libubus.h>
#include <sqlite3.h>
#include <pthread.h>

//!< DLMS headers
#include <converters.h>
#include <gxignore.h>
#include <cosem.h>
#include <client.h>

#define UTL_SAFE_STR(x) ((x) ? (x) : "")
#define UTL_STR(x) UTL_STR2(x)
#define UTL_STR2(x) #x

#define MAX_CONNECTIONS_COUNT	   10
#define MAX_PHYSICAL_DEVICES_COUNT 30 // overall
#define MAX_COSEM_OBJECTS_COUNT	   20 // each group
#define MAX_COSEM_GROUPS_COUNT	   10

typedef struct {
	int enabled;
	int id;
	char *name;
	enum { TCP, SERIAL } type;
	union {
		struct {
			char *host;
			int port;
		} tcp;
		struct {
			char *device;
			int baudrate;
			char *parity;
			int databits;
			int stopbits;
			char *flow_control;
		} serial;
	} parameters;
	int socket;
	unsigned long wait_time;
	gxByteBuffer data;
	pthread_mutex_t *mutex;
} connection;

typedef struct {
	int enabled;
	int id;
	char *name;
	char *invocation_counter;
	connection *connection;
	dlmsSettings settings;
} physical_device;

typedef struct {
	int enabled;
	int id;
	char *name;
	int entries;
	gxObject *object;
	physical_device **devices;
	size_t device_count;
} cosem_object;

typedef struct {
	int enabled;
	int id;
	char *name;
	uint32_t interval;
	size_t cosem_object_count;
	cosem_object **cosem_objects;
} cosem_group;

typedef struct {
	size_t physical_dev_count;
	size_t cosem_group_count;
	physical_device **physical_devices;
	cosem_group **cosem_groups;
	sqlite3 *db;
	sqlite3_stmt *stmt_insert;
	pthread_mutex_t *mutex_rs232;
	pthread_mutex_t *mutex_rs485;

	//!< these only needed for free at the end:
	size_t connection_count;
	size_t cosem_object_count;
	connection **connections;
	cosem_object **cosem_objects;
} master;

typedef struct {
	char **values;
	int count;
} object_attributes;

extern master *g_master;
extern log_level_type g_debug_level;

// config.c
//////////////////////////////////////////////////////////////////////////////////////////

PUBLIC master *cfg_get_master();
PUBLIC void cfg_free_master(master *m);

// master.c
//////////////////////////////////////////////////////////////////////////////////////////

PUBLIC int mstr_create_db(master *m);
PUBLIC int mstr_initialize_cosem_groups(master *m);
PUBLIC void mstr_write_group_data_to_db(cosem_group *group, char *data);
PUBLIC void mstr_db_free(master *m);

// attribute_converter.c
//////////////////////////////////////////////////////////////////////////////////////////

PUBLIC void attr_init(object_attributes *attributes, gxObject *object);
PUBLIC void attr_free(object_attributes *attributes);
PUBLIC char *attr_to_json(object_attributes *attributes);
PUBLIC int attr_to_string(gxObject *object, object_attributes *attributes);

// cosem_group.c
//////////////////////////////////////////////////////////////////////////////////////////

PUBLIC char *cg_read_group_codes(cosem_group *group, int *rc);
PUBLIC int cg_make_connection(physical_device *dev);

// utils.c
//////////////////////////////////////////////////////////////////////////////////////////

PUBLIC int utl_parse_args(int argc, char **argv, log_level_type *debug_lvl);
PUBLIC void utl_append_to_str(char **destination, const char *source);
PUBLIC void utl_append_obj_name(char **data, char *name);
PUBLIC void utl_append_if_needed(char **data, int index, int value, char *str);
PUBLIC void utl_add_error_message(char **data, char *device_name, const char *err_msg, const int err_num);
PUBLIC void utl_smart_sleep(time_t *t0, unsigned long *tn, unsigned p);
PUBLIC void utl_lock_mutex_if_required(physical_device *d);
PUBLIC void utl_unlock_mutex_if_required(physical_device *d);
PUBLIC int utl_validate_cosem_id(const int cosem_id);
__attribute__((unused)) PUBLIC void utl_debug_master(master *m);

// ubus.c
//////////////////////////////////////////////////////////////////////////////////////////

PUBLIC int init_ubus_test_functions();
PUBLIC void ubus_exit();

// communication.c
//////////////////////////////////////////////////////////////////////////////////////////

PUBLIC int com_open_connection(physical_device *dev);
PUBLIC void com_close(connection *c, dlmsSettings *s);
PUBLIC void com_close_socket(connection *c);
PUBLIC int com_update_invocation_counter(connection *c, dlmsSettings *s, const char *invocationCounter);
PUBLIC int com_initialize_connection(connection *c, dlmsSettings *s);
PUBLIC int com_read(connection *c, dlmsSettings *s, gxObject *object, unsigned char attributeOrdinal);
PUBLIC int com_getKeepAlive(connection *c, dlmsSettings *s);
PUBLIC int com_readRowsByEntry(connection *c, dlmsSettings *s, gxProfileGeneric *obj, int index, int count);
