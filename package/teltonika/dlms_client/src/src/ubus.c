#include "master.h"

#define __UNUSED   __attribute__((unused))
#define KEY_SIZE   32
#define TITLE_SIZE 16

PRIVATE int reply(struct ubus_context *ctx, struct ubus_request_data *req, const int err_code, const char *y);
PRIVATE int device_cb(struct ubus_context *ctx, __UNUSED struct ubus_object *obj,
		      struct ubus_request_data *req, __UNUSED const char *method, struct blob_attr *msg);
PRIVATE int device_cb_args(physical_device *d, struct blob_attr *msg);
PRIVATE int read_security_settings(struct blob_attr **tb, physical_device *d);
PRIVATE int validate_key(const char *key, size_t len);
PRIVATE void find_corresponding_mutex(connection *c);
PRIVATE int tcp_connection_match(connection *g_c, connection *c);
PRIVATE void free_device(physical_device *d);
PRIVATE int cosem_group_cb(struct ubus_context *ctx, __UNUSED struct ubus_object *obj,
			   struct ubus_request_data *req, __UNUSED const char *method, struct blob_attr *msg);
PRIVATE int cosem_group_args(cosem_group *d, struct blob_attr *msg);
PRIVATE cosem_object *parse_cosem_object(struct blob_attr *b);
PRIVATE void free_cosem_group(cosem_group *g);

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
	TEST_DEVICE_OPT_COUNT
};

PRIVATE struct blobmsg_policy device_policy[] = {
	[TEST_DEVICE_OPT_TYPE]		     = { .name = "type", .type = BLOBMSG_TYPE_INT32 },
	[TEST_DEVICE_OPT_TCP_ADDRESS]	     = { .name = "tcp_address", .type = BLOBMSG_TYPE_STRING },
	[TEST_DEVICE_OPT_TCP_PORT]	     = { .name = "tcp_port", .type = BLOBMSG_TYPE_INT32 },
	[TEST_DEVICE_OPT_SERIAL_DEVICE]	     = { .name = "serial_dev", .type = BLOBMSG_TYPE_STRING },
	[TEST_DEVICE_OPT_SERIAL_BAUDRATE]    = { .name = "serial_baudrate", .type = BLOBMSG_TYPE_INT32 },
	[TEST_DEVICE_OPT_SERIAL_DATABITS]    = { .name = "serial_databits", .type = BLOBMSG_TYPE_INT32 },
	[TEST_DEVICE_OPT_SERIAL_STOPBITS]    = { .name = "serial_stopbits", .type = BLOBMSG_TYPE_INT32 },
	[TEST_DEVICE_OPT_SERIAL_PARITY]	     = { .name = "serial_parity", .type = BLOBMSG_TYPE_STRING },
	[TEST_DEVICE_OPT_SERIAL_FLOWCONTROL] = { .name = "serial_flowcontrol", .type = BLOBMSG_TYPE_STRING },
	[TEST_DEVICE_OPT_SERVER_ADDR]	     = { .name = "server_addr", .type = BLOBMSG_TYPE_INT32 },
	[TEST_DEVICE_OPT_LOG_SERVER_ADDR]    = { .name = "logical_server_addr", .type = BLOBMSG_TYPE_INT32 },
	[TEST_DEVICE_OPT_CLIENT_ADDR]	     = { .name = "client_addr", .type = BLOBMSG_TYPE_INT32 },
	[TEST_DEVICE_OPT_USE_LN]	     = { .name = "use_ln_ref", .type = BLOBMSG_TYPE_BOOL },
	[TEST_DEVICE_OPT_TRANSPORT_SECURITY] = { .name = "transport_security", .type = BLOBMSG_TYPE_INT32 },
	[TEST_DEVICE_OPT_INTERFACE_TYPE]     = { .name = "interface", .type = BLOBMSG_TYPE_INT32 },
	[TEST_DEVICE_OPT_ACCESS_SECURITY]    = { .name = "access_security", .type = BLOBMSG_TYPE_INT32 },
	[TEST_DEVICE_OPT_PASSWORD]	     = { .name = "password", .type = BLOBMSG_TYPE_STRING },
	[TEST_DEVICE_OPT_SYSTEM_TITLE]	     = { .name = "system_title", .type = BLOBMSG_TYPE_STRING },
	[TEST_DEVICE_OPT_AUTHENTICATION_KEY] = { .name = "auth_key", .type = BLOBMSG_TYPE_STRING },
	[TEST_DEVICE_OPT_BLOCK_CIPHER_KEY]   = { .name = "block_cipher_key", .type = BLOBMSG_TYPE_STRING },
	[TEST_DEVICE_OPT_DEDICATED_KEY]	     = { .name = "dedicated_key", .type = BLOBMSG_TYPE_STRING },
	[TEST_DEVICE_OPT_INVOCATION_COUNTER] = { .name = "invocation_counter", .type = BLOBMSG_TYPE_STRING }
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

PRIVATE struct blobmsg_policy cosem_object_policy[] = {
	[TEST_COSEM_OBJECT_OPT_ID]		 = { .name = "id", .type = BLOBMSG_TYPE_INT32 },
	[TEST_COSEM_OBJECT_OPT_ENABLED]		 = { .name = "enabled", .type = BLOBMSG_TYPE_INT32 },
	[TEST_COSEM_OBJECT_OPT_NAME]		 = { .name = "name", .type = BLOBMSG_TYPE_STRING },
	[TEST_COSEM_OBJECT_OPT_PHYSICAL_DEVICES] = { .name = "devices", .type = BLOBMSG_TYPE_ARRAY },
	[TEST_COSEM_OBJECT_OPT_OBIS]		 = { .name = "obis", .type = BLOBMSG_TYPE_STRING },
	[TEST_COSEM_OBJECT_OPT_COSEM_ID]	 = { .name = "cosem_id", .type = BLOBMSG_TYPE_INT32 },
	[TEST_COSEM_OBJECT_OPT_ENTRIES]		 = { .name = "entries", .type = BLOBMSG_TYPE_INT32 }
};

enum {
	TEST_COSEM_GROUP_OPT_OBJECTS,
	TEST_COSEM_GROUP_OPT_COUNT
};

PRIVATE struct blobmsg_policy cosem_group_policy[] = {
	[TEST_COSEM_GROUP_OPT_OBJECTS] = { .name = "objects", .type = BLOBMSG_TYPE_ARRAY },
};

PRIVATE const struct ubus_method ubus_methods[] = {
	UBUS_METHOD("test_device", device_cb, device_policy),
	UBUS_METHOD("test_cosem_group", cosem_group_cb, cosem_group_policy),
};

PRIVATE struct ubus_object_type ubus_obj_type = UBUS_OBJECT_TYPE("dlms", ubus_methods);

PRIVATE struct ubus_object ubus_obj = {
	.name	   = "dlms",
	.type	   = &ubus_obj_type,
	.methods   = ubus_methods,
	.n_methods = ARRAY_SIZE(ubus_methods),
};

PRIVATE struct ubus_context *ubus = NULL;

PUBLIC int init_ubus_test_functions()
{
	ubus = ubus_connect(NULL);
	if (!ubus) {
		log(L_ERROR, "Failed to connect to UBUS\n");
		return 1;
	}

	if (ubus_add_object(ubus, &ubus_obj)) {
		log(L_ERROR, "Failed to add UBUS object\n");
		return 1;
	}

	ubus_add_uloop(ubus);
	return 0;
}

PRIVATE int reply(struct ubus_context *ctx, struct ubus_request_data *req, const int err_code, const char *y)
{
	int rc		  = 1;
	struct blob_buf b = { 0 };

	if (blob_buf_init(&b, 0)) {
		log(L_WARNING, "Failed to initiate blob buffer\n");
		goto end;
	}

	if (blobmsg_add_u32(&b, "error", err_code)) {
		log(L_WARNING, "Failed to add integer to blob message\n");
		goto end;
	}

	if (blobmsg_add_string(&b, "result", y)) {
		log(L_WARNING, "Failed to add string to blob message");
		goto end;
	}

	if (ubus_send_reply(ctx, req, b.head)) {
		log(L_WARNING, "Failed to send UBUS reply\n");
		goto end;
	}

	rc = 0;
end:
	blob_buf_free(&b);
	return rc;
}

PRIVATE int device_cb(struct ubus_context *ctx, __UNUSED struct ubus_object *obj,
		      struct ubus_request_data *req, __UNUSED const char *method, struct blob_attr *msg)
{
	int rc		    = DLMS_ERROR_CODE_OK;
	const char *result  = "OK";
	physical_device dev = {
		.connection = (connection[]){ { .socket = -1, .name = "UBUS TEST" } },
		.settings   = { 0 },
	};

	rc = device_cb_args(&dev, msg);
	if (rc != DLMS_ERROR_CODE_OK) {
		result = "Failed to parse device parameters.";
		goto end;
	}

	utl_lock_mutex_if_required(&dev);

	rc = cg_make_connection(&dev);
	if (rc != DLMS_ERROR_CODE_OK) {
		result = "Failed to make connection with device.";
		goto end;
	}

	com_close(dev.connection, &dev.settings);
end:
	utl_unlock_mutex_if_required(&dev);
	rc = reply(ctx, req, !!rc, result);
	free_device(&dev);
	return rc;
}

PRIVATE int device_cb_args(physical_device *d, struct blob_attr *msg)
{
	int rc = DLMS_ERROR_CODE_OTHER_REASON;
	struct blob_attr *tb[TEST_DEVICE_OPT_COUNT] = { 0 };

	if (!msg) {
		log(L_WARNING, "UBUS message is null\n");
		goto end;
	}

	if (blobmsg_parse(device_policy, TEST_DEVICE_OPT_COUNT, tb, blob_data(msg), blob_len(msg))) {
		log(L_WARNING, "Failed to parse device policy\n");
		goto end;
	}

	if (!tb[TEST_DEVICE_OPT_TYPE]) {
		log(L_WARNING, "Option 'type' is not specified. It should be 0 (TCP) or 1 (SERIAL)\n");
		goto end;
	}
	d->connection->type = blobmsg_get_u32(tb[TEST_DEVICE_OPT_TYPE]);

	if (d->connection->type == TCP) {
		if (!tb[TEST_DEVICE_OPT_TCP_ADDRESS] || !tb[TEST_DEVICE_OPT_TCP_PORT]) {
			log(L_WARNING, "'address' or 'port' options can't be empty\n");
			goto end;
		}
	} else if (d->connection->type == SERIAL) {
		if (!tb[TEST_DEVICE_OPT_SERIAL_DEVICE] || !tb[TEST_DEVICE_OPT_SERIAL_BAUDRATE] ||
		    !tb[TEST_DEVICE_OPT_SERIAL_DATABITS] || !tb[TEST_DEVICE_OPT_SERIAL_STOPBITS] ||
		    !tb[TEST_DEVICE_OPT_SERIAL_PARITY] || !tb[TEST_DEVICE_OPT_SERIAL_FLOWCONTROL]) {
			log(L_WARNING, "Serial settings are missing\n");
			goto end;
		}
	} else {
		log(L_WARNING, "Option 'type' is invalid. It should be 0 (TCP) or 1 (SERIAL)\n");
		goto end;
	}

	if (d->connection->type == TCP) {
		int port = blobmsg_get_u32(tb[TEST_DEVICE_OPT_TCP_PORT]);
		if (port < 1 || port > 65535) {
			log(L_WARNING, "Option 'port' (%d) is outside range [1;65535]\n", port);
			goto end;
		}
		d->connection->parameters.tcp.port = port;
		d->connection->parameters.tcp.host = blobmsg_get_string(tb[TEST_DEVICE_OPT_TCP_ADDRESS]);
	} else {
		d->connection->parameters.serial.device =
			blobmsg_get_string(tb[TEST_DEVICE_OPT_SERIAL_DEVICE]);
		d->connection->parameters.serial.baudrate =
			blobmsg_get_u32(tb[TEST_DEVICE_OPT_SERIAL_BAUDRATE]);
		d->connection->parameters.serial.parity =
			blobmsg_get_string(tb[TEST_DEVICE_OPT_SERIAL_PARITY]);
		d->connection->parameters.serial.databits =
			blobmsg_get_u32(tb[TEST_DEVICE_OPT_SERIAL_DATABITS]);
		d->connection->parameters.serial.stopbits =
			blobmsg_get_u32(tb[TEST_DEVICE_OPT_SERIAL_STOPBITS]);
		d->connection->parameters.serial.flow_control =
			blobmsg_get_string(tb[TEST_DEVICE_OPT_SERIAL_FLOWCONTROL]);
	}

	if (!tb[TEST_DEVICE_OPT_SERVER_ADDR]) {
		log(L_WARNING, "Option 'server_addr' is empty\n");
		goto end;
	}

	if (!tb[TEST_DEVICE_OPT_LOG_SERVER_ADDR]) {
		log(L_WARNING, "Option 'logical_server_addr' is empty\n");
		goto end;
	}
	int server_address = cl_getServerAddress(blobmsg_get_u32(tb[TEST_DEVICE_OPT_LOG_SERVER_ADDR]),
					     blobmsg_get_u32(tb[TEST_DEVICE_OPT_SERVER_ADDR]), 0);

	if (!tb[TEST_DEVICE_OPT_CLIENT_ADDR]) {
		log(L_WARNING, "Option 'client_addr' is empty\n");
		goto end;
	}
	int client_address = blobmsg_get_u32(tb[TEST_DEVICE_OPT_CLIENT_ADDR]);

	// if (!tb[TEST_DEVICE_OPT_USE_LN]) {
	// 	log(L_WARNING, "Option 'use_ln_ref' is empty\n");
	// 	goto end;
	// }
	// default is always 1 for now.
	int use_logical_name_referencing = 1;

	int interface_type = DLMS_INTERFACE_TYPE_HDLC;
	if (tb[TEST_DEVICE_OPT_INTERFACE_TYPE]) {
		interface_type = blobmsg_get_u32(tb[TEST_DEVICE_OPT_INTERFACE_TYPE]);
	}

	int access_security = DLMS_AUTHENTICATION_NONE;
	if (tb[TEST_DEVICE_OPT_ACCESS_SECURITY]) {
		access_security = blobmsg_get_u32(tb[TEST_DEVICE_OPT_ACCESS_SECURITY]);
	}

	const char *password = NULL;
	if (tb[TEST_DEVICE_OPT_PASSWORD]) {
		password = blobmsg_get_string(tb[TEST_DEVICE_OPT_PASSWORD]);
	}

	cl_init(&d->settings, use_logical_name_referencing, client_address, server_address, access_security,
		password, interface_type);

	if (read_security_settings(tb, d)) {
		log(L_WARNING, "Failed to read security settings\n");
		goto end;
	}

	d->connection->wait_time = 5000;
	bb_init(&d->connection->data);
	bb_capacity(&d->connection->data, 500);
	find_corresponding_mutex(d->connection);

	rc = 0;
end:
	return rc;
}

PRIVATE int read_security_settings(struct blob_attr **tb, physical_device *d)
{
	int rc = 1;

	if (tb[TEST_DEVICE_OPT_TRANSPORT_SECURITY]) {
		d->settings.cipher.security = blobmsg_get_u32(tb[TEST_DEVICE_OPT_TRANSPORT_SECURITY]);
	}

	if (tb[TEST_DEVICE_OPT_INVOCATION_COUNTER]) {
		d->invocation_counter = blobmsg_get_string(tb[TEST_DEVICE_OPT_INVOCATION_COUNTER]);
	}

	if (tb[TEST_DEVICE_OPT_SYSTEM_TITLE]) {
		const char *system_title = blobmsg_get_string(tb[TEST_DEVICE_OPT_SYSTEM_TITLE]);
		if (validate_key(system_title, TITLE_SIZE)) {
			log(L_ERROR, "system title size must be 16 bytes");
			goto end;
		}

		bb_clear(&d->settings.cipher.systemTitle);
		bb_addHexString(&d->settings.cipher.systemTitle, system_title);
	}


	if (tb[TEST_DEVICE_OPT_AUTHENTICATION_KEY]) {
		const char *authentication_key = blobmsg_get_string(tb[TEST_DEVICE_OPT_AUTHENTICATION_KEY]);
		if (validate_key(authentication_key, KEY_SIZE)) {
			log(L_ERROR, "Authentication key size must be 32 bytes");
			goto end;
		}

		bb_clear(&d->settings.cipher.authenticationKey);
		bb_addHexString(&d->settings.cipher.authenticationKey, authentication_key);
	}

	if (tb[TEST_DEVICE_OPT_BLOCK_CIPHER_KEY]) {
		const char *block_cipher_key = blobmsg_get_string(tb[TEST_DEVICE_OPT_BLOCK_CIPHER_KEY]);
		if (validate_key(block_cipher_key, KEY_SIZE)) {
			log(L_ERROR, "Block cipher key size must be 32 bytes");
			goto end;
		}

		bb_clear(&d->settings.cipher.blockCipherKey);
		bb_addHexString(&d->settings.cipher.blockCipherKey, block_cipher_key);
	}

	if (tb[TEST_DEVICE_OPT_DEDICATED_KEY]) {
		const char *dedicated_key = blobmsg_get_string(tb[TEST_DEVICE_OPT_DEDICATED_KEY]);
		if (validate_key(dedicated_key, KEY_SIZE)) {
			log(L_ERROR, "Dedicated key size must be 32 bytes");
			goto end;
		}

		d->settings.cipher.dedicatedKey = calloc(1, sizeof(gxByteBuffer));
		bb_init(d->settings.cipher.dedicatedKey);
		bb_addHexString(d->settings.cipher.dedicatedKey, dedicated_key);
	}

	rc = 0;
end:
	return rc;
}

PRIVATE int validate_key(const char *key, size_t len)
{
	return (strlen(key) != len);
}

/// if a connection already exists, we must first find the connection mutex to prevent simultaneous connections.
PRIVATE void find_corresponding_mutex(connection *c)
{
	for (size_t i = 0; g_master && i < g_master->connection_count; i++) {
		if (c->type == TCP) {
			connection *g_master_c = g_master->connections[i];
			if (tcp_connection_match(g_master_c, c)) {
				c->mutex = g_master_c->mutex;
				break;
			}
		} else if (c->type == SERIAL) {
			c->mutex = (strstr(c->parameters.serial.device, "rs485")) ? g_master->mutex_rs485 : g_master->mutex_rs232;
			break;
		}
	}
}

PRIVATE int tcp_connection_match(connection *g_c, connection *c)
{
	return (!strncmp(g_c->parameters.tcp.host, c->parameters.tcp.host, strlen(g_c->parameters.tcp.host)) &&
	       (g_c->parameters.tcp.port == c->parameters.tcp.port));
}

PRIVATE void free_device(physical_device *d)
{
	cip_clear(&d->settings.cipher);
	cl_clear(&d->settings);
	bb_clear(&d->connection->data);
}

PRIVATE int cosem_group_cb(struct ubus_context *ctx, __UNUSED struct ubus_object *obj,
			   struct ubus_request_data *req, __UNUSED const char *method, struct blob_attr *msg)
{
	int rc		    = DLMS_ERROR_CODE_OK;
	char *result	    = NULL;
	const char *err_msg = NULL;
	cosem_group g	    = { 0 };

	rc = cosem_group_args(&g, msg);
	if (rc != DLMS_ERROR_CODE_OK) {
		err_msg = "Failed to parse COSEM group parameters.";
		goto end;
	}

	result = cg_read_group_codes(&g, &rc);
	log(L_INFO, "Payload: %s", UTL_SAFE_STR(result));
end:
	rc = reply(ctx, req, rc, err_msg ? err_msg : result);
	free_cosem_group(&g);
	free(result);
	return rc;
}

PRIVATE int cosem_group_args(cosem_group *g, struct blob_attr *msg)
{
	int rc = 1;
	struct blob_attr *tb[TEST_COSEM_GROUP_OPT_COUNT];

	if (!msg) {
		log(L_ERROR, "UBUS message is null\n");
		goto end;
	}

	if (blobmsg_parse(cosem_group_policy, TEST_COSEM_GROUP_OPT_COUNT, tb, blob_data(msg), blob_len(msg))) {
		log(L_ERROR, "Failed to parse COSEM group policy\n");
		goto end;
	}

	if (!tb[TEST_COSEM_GROUP_OPT_OBJECTS]) {
		log(L_ERROR, "Option 'objects' is missing\n");
		goto end;		
	}

	if (blobmsg_check_array(tb[TEST_COSEM_GROUP_OPT_OBJECTS], BLOBMSG_TYPE_TABLE) <= 0) {
		log(L_ERROR, "Validation of 'objects' failed\n");
		goto end;
	}

	#ifndef TEST
	struct blob_attr *tb_obj = NULL;
	int r			 = 0;
	blobmsg_for_each_attr(tb_obj, tb[TEST_COSEM_GROUP_OPT_OBJECTS], r) {
		cosem_object *cosem = parse_cosem_object(tb_obj);
		if (!cosem) {
			log(L_WARNING, "Failed to parse COSEM object, continuing\n");
			continue;
		}
		g->cosem_objects = realloc(g->cosem_objects, (g->cosem_object_count + 1) * sizeof(cosem_object));
		g->cosem_objects[g->cosem_object_count++] = cosem;
	}

	if (!g->cosem_object_count) {
		log(L_WARNING, "Valid COSEM objects not found\n");
		goto end;
	}
	#endif

	rc = 0;
end:
	return rc;
}

PRIVATE cosem_object *parse_cosem_object(struct blob_attr *b)
{
	struct blob_attr *tb[TEST_COSEM_OBJECT_OPT_COUNT];

	if (blobmsg_parse(cosem_object_policy, TEST_COSEM_OBJECT_OPT_COUNT, tb, blobmsg_data(b), blobmsg_len(b))) {
		log(L_ERROR, "Failed to parse COSEM object policy\n");
		return NULL;
	}

	if (!tb[TEST_COSEM_OBJECT_OPT_ID] || !tb[TEST_COSEM_OBJECT_OPT_ENABLED] ||
	    !tb[TEST_COSEM_OBJECT_OPT_NAME] || !tb[TEST_COSEM_OBJECT_OPT_PHYSICAL_DEVICES] ||
	    !tb[TEST_COSEM_OBJECT_OPT_OBIS] || !tb[TEST_COSEM_OBJECT_OPT_COSEM_ID]) {
		log(L_ERROR, "COSEM object options are missing\n");
		return NULL;
	}

	cosem_object *obj = calloc(1, sizeof(cosem_object));
	if (!obj) {
		log(L_ERROR, "Failed to allocate memory for COSEM object\n");
		return NULL;
	}

	obj->id	     = blobmsg_get_u32(tb[TEST_COSEM_OBJECT_OPT_ID]);
	obj->enabled = blobmsg_get_u32(tb[TEST_COSEM_OBJECT_OPT_ENABLED]);
	obj->name    = blobmsg_get_string(tb[TEST_COSEM_OBJECT_OPT_NAME]);

	if (tb[TEST_COSEM_OBJECT_OPT_ENTRIES]) {
		obj->entries = blobmsg_get_u32(tb[TEST_COSEM_OBJECT_OPT_ENTRIES]);
	}

	int size = blobmsg_check_array(tb[TEST_COSEM_OBJECT_OPT_PHYSICAL_DEVICES], BLOBMSG_TYPE_STRING);
	if (size < 0) {
		log(L_ERROR, "Option 'devices' should be an array of strings with section IDs\n");
		goto err0;
	}

	obj->devices = calloc(size, sizeof(*obj->devices));
	if (!obj->devices) {
		log(L_ERROR, "Failed to allocate memory for COSEM object devices\n");
		goto err0;
	}

	int cosem_id = blobmsg_get_u32(tb[TEST_COSEM_OBJECT_OPT_COSEM_ID]);
	if (!utl_validate_cosem_id(cosem_id)) {
		log(L_ERROR, "cosem id (%d) is not supported\n", cosem_id);
		goto err0;
	}

	#ifndef TEST
	struct blob_attr *cur = NULL;
	int r		      = 0;
	bool found	      = false;
	blobmsg_for_each_attr (cur, tb[TEST_COSEM_OBJECT_OPT_PHYSICAL_DEVICES], r) {
		const int id = strtol(blobmsg_get_string(cur), NULL, 10);
		for (size_t i = 0; g_master && i < g_master->physical_dev_count; i++) {
			if (id == g_master->physical_devices[i]->id) {
				obj->devices[obj->device_count++] = g_master->physical_devices[i];
				found = true;
				break;
			}
		}
	}

	if (!found) {
		log(L_WARNING, "Physical devices with listed names were not found\n");
		goto err1;
	}
	#endif

	char *obis   = blobmsg_get_string(tb[TEST_COSEM_OBJECT_OPT_OBIS]);
	cosem_createObject2(cosem_id, obis, &obj->object);
	cosem_init(obj->object, cosem_id, obis);

	return obj;
err1:
	free(obj->devices);
err0:
	free(obj);
	return NULL;
}

PRIVATE void free_cosem_group(cosem_group *g)
{
	if (!g->cosem_objects) {
		return;
	}

	for (size_t i = 0; i < g->cosem_object_count; i++) {
		free(g->cosem_objects[i]->devices);
		obj_clear(g->cosem_objects[i]->object);
		free(g->cosem_objects[i]->object);
		free(g->cosem_objects[i]);
	}

	free(g->cosem_objects);
}

PUBLIC void ubus_exit()
{
	ubus_free(ubus);
}
