#include "master.h"

typedef struct connection_params_cfg {
	int id;
	char *name;
	enum { TCP_CFG, SERIAL_CFG } type;
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
	int wait_time;
} connection_params_cfg;

typedef struct {
	int id;
	char *name;
	int enabled;
	int server_address;
	int logical_server_address;
	int client_address;
	int access_security;
	int interface_type;
	int logical_name_ref;
	char *password;
	int transport_security;
	int connection;
	char *authentication_key;
	char *block_cipher_key;
	char *dedicated_key;
	char *invocation_counter;
} physical_device_cfg;

typedef struct {
	int id;
	char *name;
	int enabled;
	uint32_t interval;
} cosem_group_cfg;

typedef struct {
	int id;
	char *name;
	int enabled;
	char *obis;
	int cosem_id;
	int entries;
	char *physical_devices;
	int cosem_group;
} cosem_object_cfg;

typedef struct {
	size_t connection_cfg_count;
	size_t physical_device_cfg_count;
	size_t cosem_object_cfg_count;
	size_t cosem_group_cfg_count;

	connection_params_cfg **connections;
	physical_device_cfg **physical_devices;
	cosem_object_cfg **cosem_objects;
	cosem_group_cfg **cosem_groups;
} dlms_cfg;
