#pragma once

#include <tlt_logger.h>
#include <libubus.h>
#include <libubox/list.h>
#include <azureiot/iothub_client_properties.h>

extern struct list_head handler_list;

typedef enum {
	HANDLER_SUCCESS,
	HANDLER_ERROR,
} handler_status;

typedef enum {
	PNP_STATUS_SUCCESS	  = 200,
	PNP_STATUS_BAD_FORMAT	  = 400,
	PNP_STATUS_NOT_FOUND	  = 404,
	PNP_STATUS_INTERNAL_ERROR = 500
} pnp_stat;

struct handler {
	/* Handler is dynamically allocated */
	int is_dynamic;
	/* list of the handlers */
	struct list_head list;
	/* unique name of the handler */
	char *name;
	/* description for help messages*/
	const char *desc;
	/* custom user data */
	void *user_context;
	/* direct method / plug and play command handling function */
	pnp_stat (*cb)(char *method_name, char *method_payload,
		       IOTHUB_CLIENT_COMMAND_RESPONSE *command_response, void **user_context);
	/* optional function that gets executed during handler initialization */
	handler_status (*start)(void **user_context);
	/* optional function that gets executed when handler is unregistered */
	handler_status (*stop)(void **user_context);
};

/**
 * @brief linst all registered handlers
 * 
 */
void list_handlers(void);

/**
 * @brief find handler by name
 * 
 * @param name handle rname
 * @return struct handler* pointer to handler structure. NULL if not found.
 */
struct handler *find_handler_by_name(const char *name);

/**
 * @brief run hadler
 * 
 * @param ctx sms utils context
 * @param h pointer to handler
 * @param s UCi section
 * @return smsu_status 
 */
pnp_stat run_handler(char *json_str_args, IOTHUB_CLIENT_COMMAND_RESPONSE *command_response,
				 struct handler *h);

/**
 * @brief init rule handler
 * 
 * @param h poiter to the handler
 * @return call_status 
 */
void init_handler(struct handler *h);

/**
 * @brief execute each handler's stop() function
 * 
 */
void stop_handlers();
