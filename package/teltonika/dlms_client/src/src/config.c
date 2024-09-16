#include "master.h"

#include <libtlt_uci.h>

// TODO: check all reallocs
// TODO: check all strdups
// TODO: make terminal settings validator
// TODO: validate auth keys.

#define CFG_NAME "dlms_client"
#define SEPARATOR log(L_INFO, "----------------------------");

typedef struct connection_params_cfg {
	int enabled;
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

PRIVATE dlms_cfg *cfg_read_dlms_cfg();
PRIVATE master *cfg_read_master(dlms_cfg *cfg);

PRIVATE connection_params_cfg *cfg_read_connection(struct uci_context *uci, struct uci_section *section);
PRIVATE physical_device_cfg *cfg_read_physical_device(struct uci_context *uci, struct uci_section *section);
PRIVATE cosem_object_cfg *cfg_read_cosem_object(struct uci_context *uci, struct uci_section *section);
PRIVATE cosem_group_cfg *cfg_read_cosem_group(struct uci_context *uci, struct uci_section *section);

PRIVATE connection **cfg_get_connections(dlms_cfg *cfg, size_t *connection_count);
PRIVATE connection *cfg_get_connection(connection_params_cfg *cfg);
PRIVATE physical_device **cfg_get_physical_devices(dlms_cfg *cfg, connection **connections, size_t connection_count, size_t *physical_dev_count);
PRIVATE physical_device *cfg_get_physical_device(physical_device_cfg *cfg, connection **connections, size_t connection_count);
PRIVATE cosem_object **cfg_get_cosem_objects(dlms_cfg *cfg, physical_device **physical_devices, size_t physical_dev_count, size_t *cosem_object_count);
PRIVATE cosem_object *cfg_get_cosem_object(cosem_object_cfg *cfg, physical_device **physical_devices, size_t physical_dev_count);
PRIVATE cosem_group **cfg_get_cosem_groups(dlms_cfg *cfg, cosem_object **cosem_objects, size_t cosem_object_count, size_t *cosem_group_count);
PRIVATE cosem_group *cfg_get_cosem_group(dlms_cfg *cfg, cosem_group_cfg *cosem_group_cfg, cosem_object **cosem_objects, size_t cosem_object_count);

PRIVATE int cfg_init_mutexes(master *m);
PRIVATE int cfg_init_mutex(pthread_mutex_t **mutex);

PRIVATE void cfg_free_dlms_cfg(dlms_cfg *dlms);
PRIVATE void cfg_free_connection_cfg(connection_params_cfg *conn);
PRIVATE void cfg_free_physical_device_cfg(physical_device_cfg *dev);
PRIVATE void cfg_free_cosem_object_cfg(cosem_object_cfg *cosem);
PRIVATE void cfg_free_cosem_group_cfg(cosem_group_cfg *group);

PRIVATE void cfg_free_connection(connection *c);
PRIVATE void cfg_free_physical_device(physical_device *d);
PRIVATE void cfg_free_cosem_object(cosem_object *o);
PRIVATE void cfg_free_cosem_group(cosem_group *g);

PUBLIC master *cfg_get_master()
{
	dlms_cfg *cfg = NULL;
	master *m     = NULL;

	cfg = cfg_read_dlms_cfg();
	if (!cfg) {
		log(L_ERROR, "Failed to read configuration file");
		return NULL;
	}

	SEPARATOR
	log(L_INFO, "Configuration read successfully, transfering data to main struct");

	m = cfg_read_master(cfg);
	if (!m) {
		log(L_ERROR, "Failed to initiate main struct");
		goto err;
	}

	log(L_INFO, "Data transfer to main struct is complete");

	if (cfg_init_mutexes(m)) {
		log(L_ERROR, "Failed to initiate mutexes");
		goto err;
	}

	cfg_free_dlms_cfg(cfg);
	return m;
err:
	cfg_free_dlms_cfg(cfg);
	cfg_free_master(m);
	return NULL;
}

PRIVATE dlms_cfg *cfg_read_dlms_cfg()
{
	struct uci_context *uci	 = NULL;
	struct uci_package *pkg	 = NULL;
	struct uci_element *elem = NULL;
	dlms_cfg *cfg		 = NULL;

	cfg = calloc(1, sizeof(dlms_cfg));
	if (!cfg) {
		log(L_ERROR, "Failed to allocate memory for dlms_cfg struct");
		return NULL;
	}

	uci = uci_alloc_context();
	if (!uci) {
		log(L_ERROR, "Failed to allocate uci context");
		goto err;
	}

	uci_load(uci, CFG_NAME, &pkg);
	if (!pkg) {
		log(L_ERROR, "Failed to find uci package %s", CFG_NAME);
		goto err;
	}

	uci_foreach_element (&pkg->sections, elem) {
		struct uci_section *section = uci_to_section(elem);
		if (strcmp(section->type, "connection")) {
			continue;
		}

		connection_params_cfg *conn = cfg_read_connection(uci, section);
		if (conn) {
			cfg->connections = realloc(cfg->connections, (cfg->connection_cfg_count + 1) * sizeof(connection_params_cfg));
			cfg->connections[cfg->connection_cfg_count++] = conn;
		}

		if (cfg->connection_cfg_count >= MAX_CONNECTIONS_COUNT) {
			log(L_NOTICE, "Reached connections limit, breaking");
			break;
		}
	}

	if(!cfg->connection_cfg_count) {
		log(L_ERROR, "Failed to find any 'connections' sections");
		goto err;
	}

	uci_foreach_element (&pkg->sections, elem) {
		struct uci_section *section = uci_to_section(elem);
		if (strcmp(section->type, "physical_device")) {
			continue;
		}

		physical_device_cfg *dev = cfg_read_physical_device(uci, section);
		if (dev) {
			cfg->physical_devices = realloc(cfg->physical_devices, (cfg->physical_device_cfg_count + 1) * sizeof(physical_device_cfg));
			cfg->physical_devices[cfg->physical_device_cfg_count++] = dev;
		}

		if (cfg->physical_device_cfg_count >= MAX_PHYSICAL_DEVICES_COUNT) {
			log(L_NOTICE, "Reached physical devices limit, breaking");
			break;
		}
	}

	if(!cfg->physical_device_cfg_count) {
		log(L_ERROR, "Failed to find any physical device sections");
		goto err;
	}

	uci_foreach_element (&pkg->sections, elem) {
		struct uci_section *section = uci_to_section(elem);
		if (strcmp(section->type, "cosem")) {
			continue;
		}

		cosem_object_cfg *cosem = cfg_read_cosem_object(uci, section);
		if (cosem) {
			cfg->cosem_objects = realloc(cfg->cosem_objects, (cfg->cosem_object_cfg_count + 1) * sizeof(cosem_object_cfg));
			cfg->cosem_objects[cfg->cosem_object_cfg_count++] = cosem;
		}
	}

	uci_foreach_element (&pkg->sections, elem) {
		struct uci_section *section = uci_to_section(elem);
		if (strcmp(section->type, "cosem_group")) {
			continue;
		}

		cosem_group_cfg *cosem_group = cfg_read_cosem_group(uci, section);
		if (cosem_group) {
			cfg->cosem_groups = realloc(cfg->cosem_groups, (cfg->cosem_group_cfg_count + 1) * sizeof(cosem_group_cfg));
			cfg->cosem_groups[cfg->cosem_group_cfg_count++] = cosem_group;
		}

		if (cfg->cosem_group_cfg_count >= MAX_COSEM_GROUPS_COUNT) {
			log(L_NOTICE, "Reached COSEM group limit, breaking");
			break;
		}
	}

	uci_free_context(uci);
	return cfg;
err:
	cfg_free_dlms_cfg(cfg);
	uci_free_context(uci);
	return NULL;
}

PRIVATE connection_params_cfg *cfg_read_connection(struct uci_context *uci, struct uci_section *section)
{
	SEPARATOR
	connection_params_cfg *connection_params = NULL;
	const char *section_id			 = NULL;

	connection_params = calloc(1, sizeof(connection_params_cfg));
	if (!connection_params) {
		log(L_ERROR, "Failed to allocate memory for connection_params struct");
		return NULL;
	}

	if (!section->e.name) {
		log(L_ERROR, "Failed to find section name");
		goto err;
	}

	section_id = section->e.name;
	log(L_NOTICE, "Reading connection ('%s')", section_id);

	connection_params->id = strtol(section->e.name, NULL, 10);
	if (!connection_params->id) {
		log(L_ERROR, "Section ID can not be 0");
		goto err;
	}

	connection_params->enabled = ucix_get_option_int(uci, CFG_NAME, section_id, "enabled", 0);
	if (!connection_params->enabled) {
		log(L_ERROR, "Connection object ('%d') is disabled", connection_params->id);
		goto err;
	}

	connection_params->name = ucix_get_option_cfg(uci, CFG_NAME, section_id, "name");
	if (!connection_params->name) {
		log(L_ERROR, "Option 'name' cannot be empty");
		goto err;
	}

	connection_params->type = ucix_get_option_int(uci, CFG_NAME, section_id, "connection_type", 0);
	if (connection_params->type < 0 || connection_params->type > 1) {
		log(L_ERROR, "Option 'type' (%d) is outside range [0;1]", connection_params->type);
		goto err;
	}

	if (connection_params->type == TCP_CFG) {
		connection_params->parameters.tcp.host =
			ucix_get_option_cfg(uci, CFG_NAME, section_id, "address");
		if (!connection_params->parameters.tcp.host) {
			log(L_ERROR, "Option 'host' cannot be empty");
			goto err;
		}

		connection_params->parameters.tcp.port =
			ucix_get_option_int(uci, CFG_NAME, section_id, "port", 0);
		if ((connection_params->parameters.tcp.port < 1 ||
		     connection_params->parameters.tcp.port > 65535)) {
			log(L_ERROR, "Option 'port' cannot be empty [1;65535]");
			goto err;
		}
	} else if (connection_params->type == SERIAL_CFG) {
		connection_params->parameters.serial.device =
			ucix_get_option_cfg(uci, CFG_NAME, section_id, "device");
		if (!connection_params->parameters.serial.device) {
			log(L_ERROR, "Option 'device' cannot be empty");
			goto err;
		}

		connection_params->parameters.serial.baudrate =
			ucix_get_option_int(uci, CFG_NAME, section_id, "baudrate", 0);
		if (!connection_params->parameters.serial.baudrate) {
			log(L_ERROR, "Option 'baudrate' cannot be empty");
			goto err;
		}

		connection_params->parameters.serial.databits =
			ucix_get_option_int(uci, CFG_NAME, section_id, "databits", 0);
		if (connection_params->parameters.serial.databits < 5 ||
		    connection_params->parameters.serial.databits > 8) {
			log(L_ERROR, "Option 'databits' cannot be empty");
			goto err;
		}

		connection_params->parameters.serial.stopbits =
			ucix_get_option_int(uci, CFG_NAME, section_id, "stopbits", 0);
		if (connection_params->parameters.serial.stopbits < 1 ||
		    connection_params->parameters.serial.stopbits > 2) {
			log(L_ERROR, "Option 'stopbits' cannot be empty");
			goto err;
		}

		connection_params->parameters.serial.parity =
			ucix_get_option_cfg(uci, CFG_NAME, section_id, "parity");
		if (!connection_params->parameters.serial.parity) {
			log(L_ERROR, "Option 'parity' cannot be empty");
			goto err;
		}

		connection_params->parameters.serial.flow_control =
			ucix_get_option_cfg(uci, CFG_NAME, section_id, "flowcontrol");
		if (!connection_params->parameters.serial.flow_control) {
			log(L_ERROR, "Option 'flowcontrol' cannot be empty");
			goto err;
		}
	}

	connection_params->wait_time = 5000;

	log(L_NOTICE, "Connection ('%s', '%s') successfully added", section_id, UTL_SAFE_STR(connection_params->name));
	return connection_params;
err:
	log(L_NOTICE, "Failed to add connection ('%s', '%s')", section_id, UTL_SAFE_STR(connection_params->name));
	cfg_free_connection_cfg(connection_params);
	return NULL;
}

PRIVATE physical_device_cfg *cfg_read_physical_device(struct uci_context *uci, struct uci_section *section)
{
	SEPARATOR
	physical_device_cfg *dev   = NULL;
	const char *section_id	   = NULL;

	dev = calloc(1, sizeof(physical_device_cfg));
	if (!dev) {
		log(L_ERROR, "Failed to allocate memory for physical device struct");
		return NULL;
	}

	if (!section->e.name) {
		log(L_ERROR, "Failed to find section name");
		goto err;
	}

	section_id = section->e.name;
	log(L_NOTICE, "Reading physical device ('%s')", section_id);

	dev->id = strtol(section->e.name, NULL, 10);
	if (!dev->id) {
		log(L_ERROR, "Section ID can not be 0");
		goto err;
	}

	dev->enabled = ucix_get_option_int(uci, CFG_NAME, section_id, "enabled", 0);
	dev->name = ucix_get_option_cfg(uci, CFG_NAME, section_id, "name");
	if (!dev->name) {
		log(L_ERROR, "Option 'name' cannot be empty");
		goto err;
	}

	dev->server_address = ucix_get_option_int(uci, CFG_NAME, section_id, "server_addr", 0);
	if (dev->server_address < 0) {
		log(L_ERROR, "Option 'server_addr' cannot be < 0");
		goto err;
	}

	dev->logical_server_address = ucix_get_option_int(uci, CFG_NAME, section_id, "log_server_addr", 0);
	if (dev->logical_server_address < 0) {
		log(L_ERROR, "Option 'server_addr' cannot be < 0");
		goto err;
	}

	dev->client_address = ucix_get_option_int(uci, CFG_NAME, section_id, "client_addr", 0);
	if (dev->client_address < 0) {
		log(L_ERROR, "Option 'client_addr' cannot be < 0");
		goto err;
	}

	// TODO: this won't be visible for now;
	// by default use logical name referencing
	dev->logical_name_ref = ucix_get_option_int(uci, CFG_NAME, section_id, "use_logical_name_ref", 1);
	if (dev->logical_name_ref < 0 || dev->logical_name_ref > 1) {
		log(L_ERROR, "Option 'logical_name_ref' (%d) is outside range [0;1]", dev->logical_name_ref);
		goto err;
	}

	dev->connection = ucix_get_option_int(uci, CFG_NAME, section_id, "connection", 0);
	if (!dev->connection) {
		log(L_ERROR, "Option 'connection' can not be 0");
		goto err;
	}

	dev->access_security = ucix_get_option_int(uci, CFG_NAME, section_id, "access_security", 0);
	if (!dev->access_security) {
		log(L_NOTICE, "Option 'access_security'is NONE");
	}

	dev->interface_type = ucix_get_option_int(uci, CFG_NAME, section_id, "interface", 0);
	if (!dev->interface_type) {
		log(L_NOTICE, "Option 'interface_type' is HDLC");
	}

	dev->password = ucix_get_option_cfg(uci, CFG_NAME, section_id, "password");
	if (!dev->password) {
		log(L_NOTICE, "Option 'password' is empty");
	}

	dev->transport_security = ucix_get_option_int(uci, CFG_NAME, section_id, "transport_security", 0);
	if (!dev->transport_security) {
		log(L_NOTICE, "Option 'transport_security' is NONE");
	}

	dev->authentication_key = ucix_get_option_cfg(uci, CFG_NAME, section_id, "authentication_key");
	if (!dev->authentication_key) {
		log(L_NOTICE, "Option 'authentication_key' is empty or value is invalid");
	}

	dev->block_cipher_key = ucix_get_option_cfg(uci, CFG_NAME, section_id, "block_cipher_key");
	if (!dev->block_cipher_key) {
		log(L_NOTICE, "Option 'block_cipher_key' is empty or value is invalid");
	}

	dev->dedicated_key = ucix_get_option_cfg(uci, CFG_NAME, section_id, "dedicated_key");
	if (!dev->dedicated_key) {
		log(L_NOTICE, "Option 'dedicated_key' is empty or value is invalid");
	}

	dev->invocation_counter = ucix_get_option_cfg(uci, CFG_NAME, section_id, "invocation_counter");
	if (!dev->invocation_counter) {
		log(L_NOTICE, "Option 'invocation_counter' is empty");
	}

	log(L_NOTICE, "Physical device ('%s', '%s') is successfully added", section_id, UTL_SAFE_STR(dev->name));
	return dev;
err:
	log(L_NOTICE, "Failed to add physical device ('%s', '%s')", section_id, UTL_SAFE_STR(dev->name));
	cfg_free_physical_device_cfg(dev);
	return NULL;
}

PRIVATE cosem_object_cfg *cfg_read_cosem_object(struct uci_context *uci, struct uci_section *section)
{
	SEPARATOR
	cosem_object_cfg *cosem = NULL;
	const char *section_id	= NULL;

	cosem = calloc(1, sizeof(cosem_object_cfg));
	if (!cosem) {
		log(L_ERROR, "Failed to allocate memory for cosem struct");
		return NULL;
	}

	if (!section->e.name) {
		log(L_ERROR, "Failed to find section name");
		goto err;
	}

	section_id = section->e.name;
	log(L_NOTICE, "Reading COSEM object ('%s')", section_id);

	cosem->id = strtol(section->e.name, NULL, 10);
	if (!cosem->id) {
		log(L_ERROR, "Section ID can not be 0");
		goto err;
	}

	cosem->enabled = ucix_get_option_int(uci, CFG_NAME, section_id, "enabled", 0);
	if (!cosem->enabled) {
		log(L_ERROR, "COSEM object ('%d') is disabled", cosem->id);
		goto err;
	}

	cosem->name = ucix_get_option_cfg(uci, CFG_NAME, section_id, "name");
	if (!cosem->name) {
		log(L_ERROR, "Option 'name' can not be empty");
		goto err;
	}

	cosem->obis = ucix_get_option_cfg(uci, CFG_NAME, section_id, "obis");
	if (!cosem->obis) {
		log(L_ERROR, "Option 'OBIS' can not be empty");
		goto err;
	}

	cosem->cosem_id = ucix_get_option_int(uci, CFG_NAME, section_id, "cosem_id", 0);
	if (!utl_validate_cosem_id(cosem->cosem_id)) {
		log(L_ERROR, "Option 'cosem_id' (%d) is not supported", cosem->cosem_id);
		goto err;
	}

	if (cosem->cosem_id == DLMS_OBJECT_TYPE_PROFILE_GENERIC) {
		cosem->entries = ucix_get_option_int(uci, CFG_NAME, section_id, "entries", 0);
		if (cosem->entries < 1) {
			log(L_ERROR, "Option 'entries' (%d) can not be < 1", cosem->entries);
			goto err;
		}
	}

	cosem->physical_devices = ucix_get_list_option(uci, CFG_NAME, section_id, "physical_device");
	if (!cosem->physical_devices) {
		log(L_ERROR, "Option 'physical_devices' (%s) can not be empty", cosem->physical_devices);
		goto err;
	}

	cosem->cosem_group = ucix_get_option_int(uci, CFG_NAME, section_id, "cosem_group", 0);
	if (!cosem->cosem_group) {
		log(L_ERROR, "Option 'cosem_group' (%d) can not be 0", cosem->cosem_group);
		goto err;
	}

	log(L_NOTICE, "COSEM object ('%s', '%s') successfully added", section_id, UTL_SAFE_STR(cosem->name));
	return cosem;
err:
	log(L_NOTICE, "Failed to add COSEM object ('%s', '%s')", section_id, UTL_SAFE_STR(cosem->name));
	cfg_free_cosem_object_cfg(cosem);
	return NULL;
}

PRIVATE cosem_group_cfg *cfg_read_cosem_group(struct uci_context *uci, struct uci_section *section)
{
	SEPARATOR
	cosem_group_cfg *cosem_group = NULL;
	const char *section_id	     = NULL;

	cosem_group = calloc(1, sizeof(cosem_group_cfg));
	if (!cosem_group) {
		log(L_ERROR, "Failed to allocate memory for cosem_group struct");
		return NULL;
	}

	if (!section->e.name) {
		log(L_ERROR, "Failed to find section name");
		goto err;
	}

	section_id = section->e.name;
	log(L_NOTICE, "Reading COSEM group ('%s')", section_id);

	cosem_group->id = strtol(section->e.name, NULL, 10);
	if (!cosem_group->id) {
		log(L_ERROR, "Section ID can not be 0");
		goto err;
	}

	cosem_group->enabled = ucix_get_option_int(uci, CFG_NAME, section_id, "enabled", 0);
	if (!cosem_group->enabled) {
		log(L_ERROR, "COSEM group ('%d') is disabled", cosem_group->id);
		goto err;
	}

	cosem_group->name = ucix_get_option_cfg(uci, CFG_NAME, section_id, "name");
	if (!cosem_group->name) {
		log(L_ERROR, "Option 'name' can not be empty");
		goto err;
	}

	cosem_group->interval = ucix_get_option_int(uci, CFG_NAME, section_id, "interval", 1);
	if (cosem_group->interval < 1) {
		log(L_ERROR, "Option 'interval' (%ld) can not be < 1", cosem_group->interval);
		goto err;
	}

	log(L_NOTICE, "COSEM group ('%s', '%s') is successfully added", section_id, UTL_SAFE_STR(cosem_group->name));
	return cosem_group;
err:
	log(L_NOTICE, "Failed to add COSEM group ('%s', '%s')", section_id, UTL_SAFE_STR(cosem_group->name));
	cfg_free_cosem_group_cfg(cosem_group);
	return NULL;
}

PRIVATE master *cfg_read_master(dlms_cfg *cfg)
{
	master *m		  = NULL;
	size_t connection_count	  = 0;
	size_t physical_dev_count = 0;
	size_t cosem_object_count = 0;
	size_t cosem_group_count  = 0;

	m = calloc(1, sizeof(master));
	if (!m) {
		log(L_ERROR, "Failed to allocate memory for main struct");
		return NULL;
	}

	connection **connections = cfg_get_connections(cfg, &connection_count);
	if (!connections) {
		log(L_ERROR, "Failed to transfer connections from dlms_cfg to main");
		return NULL;
	}

	physical_device **physical_devices =
		cfg_get_physical_devices(cfg, connections, connection_count, &physical_dev_count);
	if (!physical_devices) {
		log(L_ERROR, "Failed to transfer physical devices from dlms_cfg to main");
		return NULL;
	}

	cosem_object **cosem_objects =
		cfg_get_cosem_objects(cfg, physical_devices, physical_dev_count, &cosem_object_count);
	cosem_group **cosem_groups =
		cfg_get_cosem_groups(cfg, cosem_objects, cosem_object_count, &cosem_group_count);

	m->connections	      = connections;
	m->connection_count   = connection_count;

	m->physical_devices   = physical_devices;
	m->physical_dev_count = physical_dev_count;

	m->cosem_groups	      = cosem_groups;
	m->cosem_group_count  = cosem_group_count;

	m->cosem_object_count = cosem_object_count;
	m->cosem_objects      = cosem_objects;

	return m;
}

PRIVATE connection **cfg_get_connections(dlms_cfg *cfg, size_t *connection_count)
{
	connection **connections = calloc(cfg->connection_cfg_count, sizeof(connection *));
	if (!connections) {
		log(L_ERROR, "Failed to allocate memory for connections struct");
		return NULL;
	}

	for (size_t i = 0; i < cfg->connection_cfg_count; i++) {
		connection *conn = cfg_get_connection(cfg->connections[i]);
		if (conn) {
			connections[(*connection_count)++] = conn;
		}
	}

	if (!(*connection_count)) {
		free(connections);
		return NULL;
	}

	return connections;
}

PRIVATE connection *cfg_get_connection(connection_params_cfg *cfg)
{
	connection *conn = calloc(1, sizeof(connection));
	if (!conn) {
		log(L_ERROR, "Failed to allocate memory for connection struct");
		return NULL;
	}

	conn->enabled = conn->enabled;
	conn->type    = cfg->type;
	conn->id      = cfg->id;
	conn->name    = strdup(cfg->name);

	if (cfg->type == TCP_CFG) {
		conn->parameters.tcp.host = strdup(cfg->parameters.tcp.host);
		conn->parameters.tcp.port = cfg->parameters.tcp.port;
	} else if (cfg->type == SERIAL_CFG) {
		conn->parameters.serial.device	     = strdup(cfg->parameters.serial.device);
		conn->parameters.serial.baudrate     = cfg->parameters.serial.baudrate;
		conn->parameters.serial.parity	     = strdup(cfg->parameters.serial.parity);
		conn->parameters.serial.flow_control = strdup(cfg->parameters.serial.flow_control);
		conn->parameters.serial.databits     = cfg->parameters.serial.databits;
		conn->parameters.serial.stopbits     = cfg->parameters.serial.stopbits;
	} else {
		log(L_ERROR, "Connection ('%d', '%s') has invalid connection type", conn->id, UTL_SAFE_STR(conn->name));
		goto err;
	}

	conn->socket	= -1;
	conn->wait_time = cfg->wait_time;

	bb_init(&conn->data);
	bb_capacity(&conn->data, 500);

	return conn;
err:
	free(conn->name);
	free(conn);
	return NULL;
}

PRIVATE physical_device **cfg_get_physical_devices(dlms_cfg *cfg, connection **connections, size_t connection_count, size_t *physical_dev_count)
{
	physical_device **devices = calloc(cfg->physical_device_cfg_count, sizeof(physical_device *));
	if (!devices) {
		log(L_ERROR, "Failed to allocate memory for devices struct");
		return NULL;
	}

	for (size_t i = 0; i < cfg->physical_device_cfg_count; i++) {
		physical_device *dev = cfg_get_physical_device(cfg->physical_devices[i], connections, connection_count);
		if (dev) {
			devices[(*physical_dev_count)++] = dev;
		}
	}

	if (!(*physical_dev_count)) {
		free(devices);
		return NULL;
	}

	return devices;
}

PRIVATE physical_device *cfg_get_physical_device(physical_device_cfg *cfg, connection **connections, size_t connection_count)
{
	physical_device *dev = calloc(1, sizeof(physical_device));
	if (!dev) {
		log(L_ERROR, "Failed to allocate memory for device struct");
		return NULL;
	}

	dev->enabled = cfg->enabled;
	dev->id	     = cfg->id;
	dev->name    = strdup(cfg->name);
	if (cfg->invocation_counter) {
		dev->invocation_counter = strdup(cfg->invocation_counter);
	}

	// find connection for device
	for (size_t i = 0; i < connection_count; i++) {
		if (connections[i]->id == cfg->connection) {
			dev->connection = connections[i];
			break;
		}
	}

	if (!dev->connection) {
		log(L_ERROR, "Physical device does not contain connection");
		goto err;
	}

	cfg->server_address = cl_getServerAddress(cfg->logical_server_address, cfg->server_address, 0);
	cl_init(&dev->settings, cfg->logical_name_ref, cfg->client_address, cfg->server_address,
		cfg->access_security, cfg->password, cfg->interface_type);

	dev->settings.cipher.security = cfg->transport_security;

	if (cfg->authentication_key) {
		bb_clear(&dev->settings.cipher.authenticationKey);
		bb_addHexString(&dev->settings.cipher.authenticationKey, cfg->authentication_key);
	}

	if (cfg->block_cipher_key) {
		bb_clear(&dev->settings.cipher.blockCipherKey);
		bb_addHexString(&dev->settings.cipher.blockCipherKey, cfg->block_cipher_key);
	}

	if (cfg->dedicated_key) {
		dev->settings.cipher.dedicatedKey = (gxByteBuffer *)calloc(1, sizeof(gxByteBuffer));
		bb_init(dev->settings.cipher.dedicatedKey);
		bb_addHexString(dev->settings.cipher.dedicatedKey, cfg->dedicated_key);
	}

	return dev;
err:
	free(dev->name);
	free(dev->invocation_counter);
	free(dev);
	return NULL;
}

PRIVATE cosem_object **cfg_get_cosem_objects(dlms_cfg *cfg, physical_device **physical_devices, size_t physical_dev_count, size_t *cosem_object_count)
{
	cosem_object **cosem_objects = calloc(cfg->cosem_object_cfg_count, sizeof(cosem_object *));
	if (!cosem_objects) {
		log(L_ERROR, "Failed to allocate memory for COSEM objects struct");
		return NULL;
	}

	for (size_t i = 0; i < cfg->cosem_object_cfg_count; i++) {
		cosem_object *obj = cfg_get_cosem_object(cfg->cosem_objects[i], physical_devices, physical_dev_count);
		if (obj) {
			cosem_objects[(*cosem_object_count)++] = obj;
		}
	}

	if (!(*cosem_object_count)) {
		free(cosem_objects);
		return NULL;
	}

	return cosem_objects;
}

PRIVATE cosem_object *cfg_get_cosem_object(cosem_object_cfg *cfg, physical_device **physical_devices, size_t physical_dev_count)
{
	cosem_object *obj = calloc(1, sizeof(cosem_object));
	if (!obj) {
		log(L_ERROR, "Failed to allocate memory for COSEM object struct");
		return NULL;
	}

	obj->enabled = cfg->enabled;
	obj->id	     = cfg->id;
	obj->name    = strdup(cfg->name);
	obj->entries = cfg->entries;

	// find devices for COSEM object
	char *tok_save = NULL;
	for (char *tok = strtok_r(cfg->physical_devices, " ", &tok_save); tok; tok = strtok_r(NULL, " ", &tok_save)) {
		int physical_dev_id = strtol(tok, NULL, 10);
		for (size_t i = 0; i < physical_dev_count; i++) {
			if (physical_devices[i]->id == physical_dev_id) {
				obj->devices = realloc(obj->devices, (obj->device_count + 1) * sizeof(physical_device *));
				obj->devices[obj->device_count++] = physical_devices[i];
			}
		}
	}

	if (!obj->devices) {
		log(L_ERROR, "COSEM object ('%d', '%s') does not contain any physical devices", obj->id, UTL_SAFE_STR(obj->name));
		goto err;
	}

	if (cosem_createObject2(cfg->cosem_id, cfg->obis, &obj->object)) {
		log(L_ERROR, "Failed to create COSEM object ('%d', '%s')", obj->id, UTL_SAFE_STR(obj->name));
		goto err;
	}

	if (cosem_init(obj->object, cfg->cosem_id, cfg->obis)) {
		log(L_ERROR, "Failed to init COSEM object ('%d', '%s')", obj->id, UTL_SAFE_STR(obj->name));
		goto err;
	}

	return obj;
err:
	free(obj->name);
	free(obj);
	return NULL;
}

PRIVATE cosem_group **cfg_get_cosem_groups(dlms_cfg *cfg, cosem_object **cosem_objects, size_t cosem_object_count, size_t *cosem_group_count)
{
	cosem_group **cosem_groups = calloc(cfg->cosem_group_cfg_count, sizeof(cosem_group *));
	if (!cosem_groups) {
		log(L_ERROR, "Failed to allocate memory for COSEM groups struct");
		return NULL;
	}

	for (size_t i = 0; i < cfg->cosem_group_cfg_count; i++) {
		cosem_group *group = cfg_get_cosem_group(cfg, cfg->cosem_groups[i], cosem_objects, cosem_object_count);
		if (group) {
			cosem_groups[(*cosem_group_count)++] = group;
		}
	}

	if (!(*cosem_group_count)) {
		free(cosem_groups);
		return NULL;
	}

	return cosem_groups;
}

PRIVATE cosem_group *cfg_get_cosem_group(dlms_cfg *cfg, cosem_group_cfg *cosem_group_cfg,
					cosem_object **cosem_objects, size_t cosem_object_count)
{
	cosem_group *group = calloc(1, sizeof(cosem_group));
	if (!group) {
		log(L_ERROR, "Failed to allocate memory for COSEM group struct");
		return NULL;
	}

	group->enabled	= cosem_group_cfg->enabled;
	group->id	= cosem_group_cfg->id;
	group->name	= strdup(cosem_group_cfg->name);
	group->interval = cosem_group_cfg->interval;

	// find COSEM objects for cosem_group
	for (size_t i = 0; i < cosem_object_count; i++) {
		for (size_t j = 0; j < cfg->cosem_object_cfg_count; j++) {
			if ((cosem_objects[i]->id == cfg->cosem_objects[j]->id) &&
			    (cfg->cosem_objects[i]->cosem_group == group->id)) {
				group->cosem_objects = realloc(group->cosem_objects, (group->cosem_object_count + 1) * sizeof(cosem_object *));
				group->cosem_objects[group->cosem_object_count++] = cosem_objects[i];
			}
		}

		if (group->cosem_object_count >= MAX_COSEM_OBJECTS_COUNT) {
			log(L_NOTICE, "Reached COSEM object limit for a group ('%d'), breaking", group->id);
			break;
		}
	}

	if (!group->cosem_objects) {
		log(L_ERROR, "COSEM group ('%d', '%s') does not contain any COSEM objects", group->id, UTL_SAFE_STR(group->name));
		goto err;
	}

	return group;
err:
	free(group->name);
	free(group);
	return NULL;
}

PRIVATE int cfg_init_mutexes(master *m)
{
	for (size_t i = 0; i < m->connection_count; i++) {
		connection *c		       = m->connections[i];
		pthread_mutex_t **mutex_target = NULL;
		if (c->type == TCP) {
			mutex_target = &c->mutex;
		} else {
			mutex_target = (strstr(c->parameters.serial.device, "rs485")) ? &m->mutex_rs485 : &m->mutex_rs232;
		}

		if (cfg_init_mutex(mutex_target)) {
			log(L_ERROR, "Failed to initialize connection ('%d') mutex", c->id);
			return 1;
		}
		c->mutex = *mutex_target;
	}

	return 0;
}

PRIVATE int cfg_init_mutex(pthread_mutex_t **mutex)
{
	if (*mutex) {
		return 0;
	}

	*mutex = calloc(1, sizeof(pthread_mutex_t));
	if (!*mutex) {
		log(L_ERROR, "Failed to allocate memory for mutex");
		return 1;
	}

	if (pthread_mutex_init(*mutex, NULL)) {
		free(*mutex);
		*mutex = NULL;
		return 1;
	}

	return 0;
}

PRIVATE void cfg_free_dlms_cfg(dlms_cfg *dlms)
{
	if (!dlms) {
		return;
	}

	for (size_t i = 0; i < dlms->connection_cfg_count; i++) {
		cfg_free_connection_cfg(dlms->connections[i]);
	}

	for (size_t i = 0; i < dlms->physical_device_cfg_count; i++) {
		cfg_free_physical_device_cfg(dlms->physical_devices[i]);
	}

	for (size_t i = 0; i < dlms->cosem_object_cfg_count; i++) {
		cfg_free_cosem_object_cfg(dlms->cosem_objects[i]);
	}

	for (size_t i = 0; i < dlms->cosem_group_cfg_count; i++) {
		cfg_free_cosem_group_cfg(dlms->cosem_groups[i]);
	}

	free(dlms->connections);
	free(dlms->physical_devices);
	free(dlms->cosem_objects);
	free(dlms->cosem_groups);
	free(dlms);
}

PRIVATE void cfg_free_connection_cfg(connection_params_cfg *c)
{
	if (!c) {
		return;
	}

	free(c->name);

	if (c->type == TCP_CFG) {
		free(c->parameters.tcp.host);
	} else if (c->type == SERIAL_CFG) {
		free(c->parameters.serial.device);
		free(c->parameters.serial.parity);
		free(c->parameters.serial.flow_control);
	}

	free(c);
}

PRIVATE void cfg_free_physical_device_cfg(physical_device_cfg *d)
{
	if (!d) {
		return;
	}

	free(d->name);
	free(d->password);
	free(d->authentication_key);
	free(d->block_cipher_key);
	free(d->dedicated_key);
	free(d->invocation_counter);
	free(d);
}

PRIVATE void cfg_free_cosem_object_cfg(cosem_object_cfg *o)
{
	if (!o) {
		return;
	}

	free(o->name);
	free(o->physical_devices);
	free(o->obis);
	free(o);
}

PRIVATE void cfg_free_cosem_group_cfg(cosem_group_cfg *g)
{
	if (!g) {
		return;
	}

	free(g->name);
	free(g);
}

PUBLIC void cfg_free_master(master *m)
{
	if (!m) {
		return;
	}

	for (size_t i = 0; i < m->connection_count; i++) {
		cfg_free_connection(m->connections[i]);
	}

	for (size_t i = 0; i < m->physical_dev_count; i++) {
		cfg_free_physical_device(m->physical_devices[i]);
	}

	for (size_t i = 0; i < m->cosem_object_count; i++) {
		cfg_free_cosem_object(m->cosem_objects[i]);
	}
	
	for (size_t i = 0; i < m->cosem_group_count; i++) {
		cfg_free_cosem_group(m->cosem_groups[i]);
	}

	pthread_mutex_destroy(m->mutex_rs232);
	free(m->mutex_rs232);
	pthread_mutex_destroy(m->mutex_rs485);
	free(m->mutex_rs485);

	free(m->connections);
	free(m->physical_devices);
	free(m->cosem_objects);
	free(m->cosem_groups);
	free(m);
}

PRIVATE void cfg_free_connection(connection *c)
{
	if (!c) {
		return;
	}

	free(c->name);
	bb_clear(&c->data);

	if (c->type == TCP) {
		free(c->parameters.tcp.host);
		pthread_mutex_destroy(c->mutex);
	} else {
		free(c->parameters.serial.device);
		free(c->parameters.serial.parity);
		free(c->parameters.serial.flow_control);
	}

	free(c);
}

PRIVATE void cfg_free_physical_device(physical_device *d)
{
	if (!d) {
		return;
	}

	cip_clear(&d->settings.cipher);
	cl_clear(&d->settings);

	free(d->name);
	free(d->invocation_counter);
	free(d);
}

PRIVATE void cfg_free_cosem_object(cosem_object *o)
{
	if (!o) {
		return;
	}

	obj_clear(o->object);
	free(o->name);
	free(o->object);
	free(o->devices);
	free(o);
}

PRIVATE void cfg_free_cosem_group(cosem_group *g)
{
	if (!g) {
		return;
	}

	free(g->name);
	free(g->cosem_objects);
	free(g);
}
