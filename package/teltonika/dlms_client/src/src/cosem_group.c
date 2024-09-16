#include "master.h"

#define BUFFER_ATTRIBUTE_INDEX 2

PRIVATE int cg_read_cosem_object(cosem_object *cosem_object, physical_device *dev);
PRIVATE int cg_format_group_data(char **data, object_attributes *attr, char *device_name);
PRIVATE int cg_read_profile_generic_data(cosem_object *obj, gxProfileGeneric *pg, physical_device *dev);

PUBLIC char *cg_read_group_codes(cosem_group *group, int *rc)
{
	*rc = 1;
	char *data = NULL;

	utl_append_to_str(&data, "{");
	for (size_t i = 0; i < group->cosem_object_count; i++) {
		cosem_object *cosem_object = group->cosem_objects[i];

		utl_append_obj_name(&data, cosem_object->name);
		utl_append_to_str(&data, "{");
		for (size_t j = 0; j < cosem_object->device_count; j++) {
			physical_device *dev	= cosem_object->devices[j];
			object_attributes attrs = { 0 };
			int err			= 0;

			utl_lock_mutex_if_required(dev);

			err = !dev->enabled;
			if (err) {
				utl_add_error_message(&data, dev->name, "Physical device is disabled", err);
				log(L_WARNING, "Physical device ('%d') is disabled!", dev->id);
				goto loopend_without_close;
			}

			err = cg_make_connection(dev);
			if (err != DLMS_ERROR_CODE_OK) {
				utl_add_error_message(&data, dev->name, "Failed to make connection with device", err);
				log(L_ERROR, "Failed to make connection with device ('%d', '%s')", dev->id, dev->name);
				goto loopend_without_close;
			}

			err = cg_read_cosem_object(cosem_object, dev);
			if (err != DLMS_ERROR_CODE_OK) {
				utl_add_error_message(&data, dev->name, hlp_getErrorMessage(err), err);
				log(L_ERROR, "Failed to read COSEM object");
				goto loopend;
			}

			attr_init(&attrs, cosem_object->object);
			err = attr_to_string(cosem_object->object, &attrs);
			if (err != DLMS_ERROR_CODE_OK) {
				utl_add_error_message(&data, dev->name, "Failed to get COSEM object attributes", err);
				log(L_ERROR, "Failed to get COSEM object attributes");
				goto loopend;
			}

			err = cg_format_group_data(&data, &attrs, dev->name);
			if (err != DLMS_ERROR_CODE_OK) {
				utl_add_error_message(&data, dev->name, "Failed to format read attributes", err);
				log(L_ERROR, "Failed to format read attributes\n");
				goto loopend;
			}

			*rc = 0;
		loopend:
			com_close(dev->connection, &dev->settings);
		loopend_without_close:
			utl_unlock_mutex_if_required(dev);
			utl_append_if_needed(&data, j, cosem_object->device_count - 1, ",");
			attr_free(&attrs);
		}
		utl_append_to_str(&data, "}");
		utl_append_if_needed(&data, i, group->cosem_object_count - 1, ",");
	}
	utl_append_to_str(&data, "}");

	return data;
}

PUBLIC int cg_make_connection(physical_device *dev)
{
	int ret = DLMS_ERROR_CODE_OK;

	if (!dev) {
		log(L_ERROR, "Device is null!");
		return 1;
	}

	if (dev->connection->socket != -1) {
		log(L_INFO, "Already connected: %d", dev->connection->socket);
		return ret;
	}

	ret = com_open_connection(dev);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_ERROR, "Failed to open connection ('%d', '%s')", dev->connection->id, dev->connection->name);
		com_close_socket(dev->connection);
		return ret;
	}

	ret = com_update_invocation_counter(dev->connection, &dev->settings, dev->invocation_counter);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_ERROR, "Failed to update invocation_counter");
		goto err;
	}

	ret = com_initialize_connection(dev->connection, &dev->settings);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_ERROR, "Failed to initialize connection");
		goto err;
	}

	return ret;
err:
	com_close(dev->connection, &dev->settings);
	return ret;
}

PRIVATE int cg_read_cosem_object(cosem_object *cosem_object, physical_device *dev)
{
	gxObject *object	= cosem_object->object;
	gxByteBuffer attributes = { 0 };
	unsigned char ch	= 0;
	int ret			= DLMS_ERROR_CODE_OK;

	char LN[32] = { 0 };
	hlp_getLogicalNameToString(object->logicalName, LN);
	log(L_INFO, "Reading object: '%s', OBIS '%s'\n", obj_typeToString2(object->objectType), LN);

	bb_init(&attributes);
	ret = obj_getAttributeIndexToRead(object, &attributes);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_ERROR, "Failed to read object. Error: %s", hlp_getErrorMessage(ret));
		goto err;
	}

	for (unsigned long index = 0; index < attributes.size; index++) {
		ret = bb_getUInt8ByIndex(&attributes, index, &ch);
		if (ret != DLMS_ERROR_CODE_OK) {
			log(L_ERROR, "Failed to get uint by index %d", ch);
			goto err;
		}

		log(L_INFO, "Index: %ld, Reading attribute: %d", index, ch);

		if (object->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC && ch == BUFFER_ATTRIBUTE_INDEX) {
			log(L_INFO, "Skipping profile generic 'buffer' attribute for later reading\n");
			continue;
		}

		ret = com_read(dev->connection, &dev->settings, object, ch);
		if (ret != DLMS_ERROR_CODE_OK) {
			log(L_ERROR, "Failed to read object attribute, index: %d", ch);

			if ((object->objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME) &&
			    (ret == DLMS_ERROR_CODE_ACCESS_VIOLATED || ret == DLMS_ERROR_CODE_OTHER_REASON)) {
				log(L_ERROR, "Device reports access violation, skipping...");
				ret = 0;
				continue;
			}

			if (ret != DLMS_ERROR_CODE_READ_WRITE_DENIED) {
				goto err;
			}
		}
	}

	if (object->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC) {
		ret = cg_read_profile_generic_data(cosem_object, (gxProfileGeneric *)object, dev);
		if (ret != DLMS_ERROR_CODE_OK) {
			log(L_ERROR, "Failed to read profile generic data");
			goto err;
		}
	}

err:
	bb_clear(&attributes);
	return ret;
}

PRIVATE int cg_read_profile_generic_data(cosem_object *obj, gxProfileGeneric *pg, physical_device *dev)
{
	log(L_INFO, "Reading profile generic rows, total entries: %ld", pg->profileEntries);
	int calc = pg->profileEntries - obj->entries;

	return com_readRowsByEntry(dev->connection, &dev->settings, pg, calc, obj->entries + 1);
}

PRIVATE int cg_format_group_data(char **data, object_attributes *attr, char *device_name)
{
	char *json = attr_to_json(attr);
	if (!json) {
		log(L_ERROR, "Failed to format attributes to JSON");
		return 1;
	}

	utl_append_obj_name(data, device_name);
	utl_append_to_str(data, "{");
	utl_append_to_str(data, json);
	utl_append_to_str(data, "}");
	free(json);

	return 0;
}
