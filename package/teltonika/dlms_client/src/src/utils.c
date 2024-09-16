#include "master.h"

PUBLIC int utl_parse_args(int argc, char **argv, log_level_type *debug_lvl)
{
	if (argc <= 1) {
		return 0;
	}

	if (argc != 3 || strcmp(argv[1], "-D")) {
		fprintf(stderr, "Usage: %s [-D DEBUG_LEVEL]\n", argv[0]);
		return 1;
	}

	char *endptr = NULL;
	long int value = strtol(argv[2], &endptr, 10);
	if (errno || *endptr) {
		fprintf(stderr, "Invalid -D argument\n");
		return 1;
	}

	*debug_lvl = (log_level_type)value;
	return 0;
}

PUBLIC void utl_append_to_str(char **destination, const char *source)
{
	if (!source) {
		log(L_ERROR, "Source is NULL");
		return;
	}

	if (!(*destination)) {
		*destination = strdup(source);
		return;
	}

	size_t source_len = strlen(source);
	size_t sum_len = strlen(*destination) + source_len + 1;

	*destination = realloc(*destination, sum_len);
	if (!(*destination)) {
		log(L_ERROR, "Failed to reallocate memory for destination");
		return;
	}

	strncat(*destination, source, source_len);
}

PUBLIC void utl_append_obj_name(char **data, char *name)
{
	char device_name[256] = { 0 };

	snprintf(device_name, sizeof(device_name), "\"%s\":", name);
	utl_append_to_str(data, device_name);
}

PUBLIC void utl_append_if_needed(char **data, int index, int value, char *str)
{
	if (index != value) {
		utl_append_to_str(data, str);
	}
}

PUBLIC void utl_add_error_message(char **data, char *device_name, const char *err_msg, const int err_num)
{
	char str[256] = { 0 };

	utl_append_obj_name(data, device_name);
	snprintf(str, sizeof(str), "{\"error\": %d, \"result\": \"%s\"}", err_num, err_msg);
	utl_append_to_str(data, str);
}

PUBLIC void utl_smart_sleep(time_t *t0, unsigned long *tn, unsigned p)
{
	++*tn;

	time_t target = *t0 + *tn * p;
	time_t now    = time(NULL);

	if (target > now) {
		sleep(target - now);
	}

	if (llabs(target - now) > p * 2) {
		*t0 = now;
		*tn = 0;
	}
}

// Could be useful to check errno here
PUBLIC void utl_lock_mutex_if_required(physical_device *d)
{
	if (d->connection->mutex) {
		pthread_mutex_lock(d->connection->mutex);
	}
}

PUBLIC void utl_unlock_mutex_if_required(physical_device *d)
{
	if (d->connection->mutex) {
		pthread_mutex_unlock(d->connection->mutex);
	}
}

PUBLIC int utl_validate_cosem_id(const int cosem_id)
{
	return (cosem_id > 0 || cosem_id < 72);
}

__attribute__((unused)) PUBLIC void utl_debug_master(master *m)
{
	if (!m) {
		log(L_DEBUG, "Nothing to debug");
		return;
	}

	printf("\n\n********* PHYSICAL DEVICES *********\n");
	for (size_t i = 0; i < m->physical_dev_count; i++) {
		physical_device *p = m->physical_devices[i];
		printf("\t---------------- device %d ----------------\n", i);
		printf("\tid:\t\t\t%d\n", p->id);
		printf("\tname:\t\t\t%s\n", UTL_SAFE_STR(p->name));
		printf("\tinvocation counter:\t%s\n", UTL_SAFE_STR(p->invocation_counter));
		if (p->connection) {
			printf("\tconnection type:\t%s\n", (p->connection->type == TCP) ? "TCP" : "SERIAL");
			printf("\twait_time:\t\t%ld\n", p->connection->wait_time);
			if (p->connection->type == TCP) {
				printf("\thost:\t\t\t%s\n", UTL_SAFE_STR(p->connection->parameters.tcp.host));
				printf("\tport:\t\t\t%d\n", p->connection->parameters.tcp.port);
			} else {
				printf("\tdevice:\t\t\t%s\n", UTL_SAFE_STR(p->connection->parameters.serial.device));
				printf("\tparity:\t\t\t%s\n", UTL_SAFE_STR(p->connection->parameters.serial.parity));
				printf("\tbaudrate:\t\t%d\n", p->connection->parameters.serial.baudrate);
				printf("\tdatabits:\t\t%d\n", p->connection->parameters.serial.databits);
				printf("\tstopbits:\t\t%d\n", p->connection->parameters.serial.stopbits);
				printf("\tflow control:\t\t%s\n", p->connection->parameters.serial.flow_control);
			}
		}
	}

	printf("\n\n********* COSEM GROUPS *********\n");
	for (size_t i = 0; i < m->cosem_group_count; i++) {
		cosem_group *c = m->cosem_groups[i];
		printf("\t---------------- group %d ----------------\n", i);
		printf("\tid:\t\t\t%d\n", c->id);
		printf("\tname:\t\t\t%s\n", UTL_SAFE_STR(c->name));
		printf("\tinterval:\t\t%ld\n", c->interval);
		printf("\tCOSEM object count:\t%d\n", c->cosem_object_count);
		printf("********* COSEM GROUP OBJECTS *********\n");
		for (size_t j = 0; j < c->cosem_object_count; j++) {
			cosem_object *o = c->cosem_objects[j];
			printf("\t---------------- object %d ----------------\n", j);
			printf("\tname:\t\t%s\n", UTL_SAFE_STR(o->name));
			printf("\tid:\t\t%d\n", o->id);
			if (o->object) {
				printf("\tobject type:\t%s\n", obj_typeToString2(o->object->objectType));
				if (o->object->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC) {
					printf("\tentries:\t%d\n", o->entries);
				}
			}
			printf("\tdevices:\t");
			for (size_t k = 0; k < o->device_count; k++) {
				printf("'%s', '%d' |", UTL_SAFE_STR(o->devices[k]->name), o->devices[k]->id);
				printf(" ");
			}
			printf("\t\n");
		}
		printf("\t\n");
	}
}
