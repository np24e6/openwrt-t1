#ifndef LIBBOARDJSON_H
#define LIBBOARDJSON_H

#include <stdbool.h>
#include <libubox/blob.h>

// filepaths
#define BJSON_FILEPATH "/etc/board.json"

// ubus values
#define BJSON_UBUS_OBJECT "boardjson"
#define BJSON_UBUS_METHOD_GET "get"
#define BJSON_POLICY_SECTION "section"

// array limits
#define BJSON_NETWORK_MAX 15
#define BJSON_MODEM_MAX 4 // theoretical max is 3 (2 built in + usb), four is for good measure :)
#define BJSON_SWITCH_MAX 5
#define BJSON_ROLE_MAX 20
#define BJSON_PORT_MAX 20

// enum that represents possible
// output states of any lbjson function
typedef enum {
	BJSON_SUCCESS = 0,
	BJSON_ERR_NOT_FOUND,
	BJSON_ERR_NO_MEM,
	BJSON_ERR
} lbjson_status;

// enum that represents all possible modem types
enum lbjson_modem_type {
	BJSON_INTERNAL_PRIMARY,
	BJSON_INTERNAL_SECONDARY,
	BJSON_INTERNAL, 
	BJSON_EXTERNAL,
};

// struct that represents a port within a switch object.
struct lbjson_switch_port {
	// NOTE: null-check everything within. Values might be missing in the actual json.
	int num;
	int need_tag;
	int want_untag;
	int index;
	char *device;
	char *role;
};

// struct that represents a role within a switch object.
struct lbjson_switch_role {
	char *role;
	char *ports;
	char *device;
};

// struct that represents a switch within board.json
struct lbjson_switch {
	char *name;
	int enable;
	int reset;

	int port_count;
	struct lbjson_switch_port ports[BJSON_PORT_MAX];

	int role_count;
	struct lbjson_switch_role roles[BJSON_ROLE_MAX];
};


// struct that represents a modem in /etc/board.json
struct lbjson_modem {
	bool builtin;
	bool primary;
	bool gps_out;

	const char *id;
	const char *vendor;
	const char *product;
	const char *type;
	const char *desc;

	int num;
	int simcount;

	int gps;
	int stop_bits;
	int baudrate;
	int control;

	int lbjson_type; // determined at parsetime

	// TODO: serialize this blob_attr properly
	// NOTE: null-check this before use.
	struct blob_attr *service_modes;
};


// struct that represents a network description in /etc/board.json
struct lbjson_network {
	const char *name;
	const char *ifname; // linux iface id
	const char *proto; // could be turned into an enum
};


// struct that represents most of /etc/board.json
struct lbjson_board {
	// model info
	const char *model_id;
	const char *model_name;
	const char *model_platform;

	// network info
	int network_count;
	struct lbjson_network networks[BJSON_NETWORK_MAX];

	// modem info
	int modem_count;
	struct lbjson_modem modems[BJSON_MODEM_MAX];

	// switch info
	int switch_count;
	struct lbjson_switch switches[BJSON_SWITCH_MAX];

	// hardware info
	bool hw_dual_sim;
	bool hw_usb;
	bool hw_bluetooth;
	bool hw_wifi;
	bool hw_dual_band_ssid;
	bool hw_wps;
	bool hw_mobile;
	bool hw_gps;
	bool hw_eth;
	bool hw_sfp_port;
	bool hw_ios;
	bool hw_at_sim;

	// board release version? May or may not exist.
	char *release_version;
};


// Function that calls the libboardjson ubus object and 
// fills out the *output blob_attr with the result.
// *section_name is optional, and can be passed as NULL.
// returns BJSON_SUCCESS on successful retreival
// returns a variant of BJSON_ERR upon failure.
lbjson_status lbjson_get(const char *section_name, struct blob_attr **output);


// Function that calls lbjson_get, and parses it's output into
// a labled struct.
// returns BJSON_SUCCESS on successful retreival
// returns a variant of BJSON_ERR upon failure.
lbjson_status lbjson_get_struct(struct lbjson_board *output);


// Helper function that calls lbjson_get_struct,
// automatically retreives the default modem.
// returns BJSON_SUCCESS on successful retreival
// returns a variant of BJSON_ERR upon failure.
lbjson_status lbjson_get_default_modem(struct lbjson_modem *output);


// Helper function that calls lbjson_get_struct,
// retreives a modem by it's id.
// returns BJSON_SUCCESS on successful retreival
// returns a variant of BJSON_ERR upon failure.
lbjson_status lbjson_get_modem_by_id(const char *id, struct lbjson_modem *output);


// Helper function that calls lbjson_get_struct,
// retreives a modem by it's 'num' value.
// returns BJSON_SUCCESS on successful retreival
// returns a variant of BJSON_ERR upon failure.
lbjson_status lbjson_get_modem_by_num(int num, struct lbjson_modem *output);

/**
 * @brief converts modem type to string
 * 
 * @param type modem type
 * @return const char* string of the modem type
 */
const char *lbjson_modem_type_itoa(enum lbjson_modem_type type);

#endif /* LIBBOARDJSON_H */
