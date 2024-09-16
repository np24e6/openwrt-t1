/*
 * Copyright (c) 2021 Teltonika-Networks
 * @author:  Jokubas Maciulaitis (ubis)
 * @project: gsmd-ng
 * @date:    2021 02 17
 * @file:    ujson.h
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 */

#ifndef UJSON_H
#define UJSON_H

#include <libubox/blobmsg.h>

/** @file ujson.h
 * uJson is a higher-level wrapper for blobmsg and blobmsg_json libraries.
 *
 * It supports the following data types: string, int(32-bit), long(64-bit),
 *  bool, double(64-bit), array, table.
 * To use with ubus, blobmsg can be retrievied via get_blobmsg() method.
 *
 * It is possible to use uJson to read data from already created blob_buf or
 *  blob_attr structures or even JSON data files. See ujson_create(),
 *  ujson_create_ex(), ujson_create_file() methods.
 */

typedef void ujson_table; //!< ujson table void typedef
typedef void ujson_array; //!< ujson array void typedef

/**
 * uJson context structure
 */
struct ujson_context {
	// @cond private
	bool self_blob; // determine if blob needs to be freed on cleanup
	struct blob_buf *blob; // pointer of blob_buf structure
	struct blob_attr *attr; // pointer of current blob attribute
	// @endcond

	/**
	 * Add string to uJson structure.
	 *
	 * Note: NULL for value is not allowed.
	 * @param[in]	*self	Pointer of ujson context structure.
	 * @param[in]	*name	String name to store.
	 * @param[in]	*value	String value to store.
	 */
	void (*add_string)(struct ujson_context *, const char *, const char *);

	/**
	 * Retrieve string value from uJson structure.
	 *
	 * Note: NULL for name is not allowed.
	 * @param[in]	*self	Pointer of ujson context structure.
	 * @param[in]	*name	String name to retrieve.
	 * @return const char *. String value.
	 * NULL is returned when string name is not found.
	 */
	const char *(*get_string)(struct ujson_context *, const char *);

	/**
	 * Add signed 32-bit integer to uJson structure.
	 * @param[in]	*self	Pointer of ujson context structure.
	 * @param[in]	*name	Integer name to store.
	 * @param[in]	value	Integer value to store.
	 */
	void (*add_int)(struct ujson_context *, const char *, int);

	/**
	 * Retrieve signed 32-bit integer value from uJson structure.
	 *
	 * Note: NULL for name is not allowed.
	 * @param[in]	*self	Pointer of ujson context structure.
	 * @param[in]	*name	Integer name to retrieve.
	 * @return int. Integer value.
	 * 0 is returned when integer name is not found.
	 */
	int (*get_int)(struct ujson_context *, const char *);

	/**
	 * Add signed 64-bit integer to uJson structure.
	 * @param[in]	*self	Pointer of ujson context structure.
	 * @param[in]	*name	Integer name to store.
	 * @param[in]	value	Integer value to store.
	 */
	void (*add_long)(struct ujson_context *, const char *, long long);

	/**
	 * Retrieve signed 64-bit integer value from uJson structure.
	 *
	 * Note: NULL for name is not allowed.
	 * @param[in]	*self	Pointer of ujson context structure.
	 * @param[in]	*name	Integer name to retrieve.
	 * @return long long. Integer value.
	 * 0 is returned when integer name is not found.
	 */
	long long (*get_long)(struct ujson_context *, const char *);

	/**
	 * Add boolean value to uJson structure.
	 * @param[in]	*self	Pointer of ujson context structure.
	 * @param[in]	*name	Boolean name to store.
	 * @param[in]	value	Boolean value to store.
	 */
	void (*add_bool)(struct ujson_context *, const char *, bool);

	/**
	 * Retrieve boolean value from uJson structure.
	 *
	 * Note: NULL for name is not allowed.
	 * @param[in]	*self	Pointer of ujson context structure.
	 * @param[in]	*name	Boolean name to retrieve.
	 * @return bool. Boolean value.
	 * false is returned when boolean name is not found.
	 */
	bool (*get_bool)(struct ujson_context *, const char *);

	/**
	 * Add double value to uJson structure.
	 * @param[in]	*self	Pointer of ujson context structure.
	 * @param[in]	*name	Double name to store.
	 * @param[in]	value	Double value to store.
	 */
	void (*add_double)(struct ujson_context *, const char *, double);

	/**
	 * Retrieve double value from uJson structure.
	 *
	 * Note: NULL for name is not allowed.
	 * @param[in]	*self	Pointer of ujson context structure.
	 * @param[in]	*name	Double name to retrieve.
	 * @return Double. Double value.
	 * 0.0f is returned when double name is not found.
	 */
	double (*get_double)(struct ujson_context *, const char *);

	/**
	 * Open new JSON table.
	 *
	 * On success, this should return pointer of ujson_table which must be
	 *  closed with close_array().
	 * @param[in]	*self	Pointer of ujson context structure.
	 * @param[in]	*name	Table name.
	 * @return ujson_table *. Pointer of ujson_table.
	 * NULL is returned on failed memory allocation.
	 */
	ujson_table *(*open_table)(struct ujson_context *, const char *);

	/**
	 * Close JSON table.
	 * @param[in]	*self	Pointer of ujson context structure.
	 * @param[in]	*table	Pointer of ujson_table.
	 */
	void (*close_table)(struct ujson_context *, ujson_table *);

	/**
	 * Open new JSON array.
	 *
	 * On success, this should return pointer of ujson_array which must be
	 *  closed with close_array().
	 * @param[in]	*self	Pointer of ujson context structure.
	 * @param[in]	*name	Array name.
	 * @return ujson_array *. Pointer of ujson_array.
	 * NULL is returned on failed memory allocation.
	 */
	ujson_array *(*open_array)(struct ujson_context *, const char *);

	/**
	 * Close JSON table.
	 * @param[in]	*self	Pointer of ujson context structure.
	 * @param[in]	*table	Pointer of ujson_array.
	 */
	void (*close_array)(struct ujson_context *, ujson_array *);

	/**
	 * Select JSON array to read data from it.
	 * @param[in]	*self	Pointer of ujson context structure.
	 * @param[in]	*name	Array name.
	 * @return bool. Status of selected array.
	 * False is returned if array is not found.
	 */
	bool (*select_array)(struct ujson_context *, const char *);

	/**
	 * Select JSON any json field, table or array as current attribute.
	 * This helps iterating nested arrays
	 * @param[in]	*self	Pointer of ujson context structure.
	 * @param[in]	*attr 	Pointer to blob msg that belong to current json.
	 * NULL is equivalent to blob->head
	 * @return bool. Status of selected array.
	 * False is returned if array is not found.
	 */
	void (*select_attr)(struct ujson_context *, struct blob_attr *attr);

	/**
	 * Select next JSON item to read data from it.
	 * Note: only supported with select_array function.
	 * @param[in]	*self	Pointer of ujson context structure.
	 * @return bool. Status of selected item.
	 * False is returned if item cannot be selected.
	 */
	bool (*next_item)(struct ujson_context *);

	/**
	 * Retrieve current length of ujson data elements.
	 * @param[in]	*self	Pointer of ujson context structure.
	 * @return int. Length of ujson data elements.
	 */
	int (*len)(struct ujson_context *);

	/**
	 * Get blobmsg attribute from ujson structure.
	 * @param[in]	*self	Pointer of ujson context structure.
	 * @return struct blob_attr *. Pointer of blob_attr structure.
	 */
	struct blob_attr *(*get_blobmsg)(struct ujson_context *);

	/**
	 * Add blobmsg attribute to ujson structure.
	 * @param[in]	*self	Pointer of ujson context structure.
	 * @param[in]   *blobmsg Pointer to a blob msg.
	 * @return struct blob_attr *. Pointer of blob_attr structure.
	 */
	int (*add_blobmsg)(struct ujson_context *, struct blob_attr *);

	/**
	 * Deep clone ujson context.
	 * @param[in]	*self	Pointer of ujson context structure.
	 * @return struct ujson_context*. Pointer of blob_attr structure.
	 */
	struct ujson_context *(*clone)(struct ujson_context *);
};

/**
 * Create uJson context instance.
 *
 * It is possible to use externally allocated blob_buf structure by passing it
 *  via argument, however, it must be cleaned up manually. ujson_destroy() will
 *  not clean externally allocated blob_buf structure.
 *
 * Instance must be cleaned up with ujson_destroy() when not needed.
 * @param[in]	*b	Optional pointer to blob_buf structure.
 * @return struct ujson_context *. Pointer to ujson context structure.
 *  NULL can be returned on failed memory allocation.
 */
struct ujson_context *ujson_create(struct blob_buf *b);

/**
 * Create uJson context instance with blobmsg attribute.
 *
 * This is used to retrieve data via ujson methods from already created blobmsg
 *  attribute.
 *
 * Instance must be cleaned up with ujson_destroy() when not needed.
 * @param[in]	*msg	Pointer to blob_attr structure.
 * @return struct ujson_context *. Pointer to ujson context structure.
 *  NULL can be returned on failed memory allocation.
 */
struct ujson_context *ujson_create_ex(struct blob_attr *msg);

/**
 * Create uJson context instance and load JSON data from file path.
 *
 * This is used to retrieve data via ujson methods from JSON data file.
 *
 * Instance must be cleaned up with ujson_destroy() when not needed.
 * @param[in]	*file	Full path of JSON data file to load.
 * @return struct ujson_context *. Pointer to ujson context structure.
 *  NULL can be returned on failed memory allocation or provided file cannot be
 *   opened.
 */
struct ujson_context *ujson_create_file(const char *file);

/**
 * Clean up and destroy uJson context instance allocated with ujson_create().
 *
 * If needed, internally allocated blob_buf structre is cleared as well.
 * @param[in]	**ujson	Double pointer to ujson context instance.
 */
void ujson_destroy(struct ujson_context **ujson);

#endif // UJSON_H
