#ifndef LIBBOARDJSON_H
#define LIBBOARDJSON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <syslog.h>
#include <libubox/blob.h>
#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>
#include <libubox/blob.h>

// filepaths
#define BJSON_FILEPATH "/etc/board.json"
#define BJSON_FILEPATH_TEMP "/tmp/board.json"

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
#define BJSON_SERIAL_DEVICES_MAX 3 // rs232, rs485 and usb

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

	char *id;
	char *vendor;
	char *product;
	char *type;
	char *desc;

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
	char *name;
	char *ifname; // linux iface id
	char *proto; // could be turned into an enum
};

// represents 'hwinfo' section in /etc/board.json
struct lbjson_hwinfo {
	bool dual_sim         : 1;
	bool usb              : 1;
	bool bluetooth        : 1;
	bool wifi             : 1;
	bool dual_band_ssid   : 1;
	bool wps              : 1;
	bool mobile           : 1;
	bool gps              : 1;
	bool sfp_port         : 1;
	bool ios              : 1;
	bool at_sim           : 1;
	bool poe              : 1;
	bool ethernet         : 1;
	bool sfp_switch       : 1;
	bool rs232            : 1;
	bool rs485            : 1;
	bool console          : 1;
	bool dual_modem       : 1;
	bool m2_modem         : 1;
	bool sd_card          : 1;
	bool sw_rst_on_init   : 1;
	bool dsa              : 1;
	bool nat_offloading   : 1;
	bool hw_nat           : 1;
	bool vcert            : 1;
	bool port_link        : 1;
	bool multi_tag        : 1;
	bool micro_usb        : 1;
	bool soft_port_mirror : 1;
	bool gigabit_port     : 1;
	bool gigabit_port_2_5 : 1;
	bool esim             : 1;
	bool custom_usbcfg    : 1;
	bool mbus	      : 1;
};

///////////////////////////////////////////////

#include <termios.h>

struct lbjson_baudrate {
	uint32_t bps;
	bool available;
};

struct lbjson_data_bits {
	uint8_t data_bits;
	bool available;
};

struct lbjson_stop_bits {
	uint8_t stop_bits;
	bool available;
};

struct lbjson_parity {
	char type[16];
	bool available;
};

struct lbjson_flow_control {
	char type[16];
	bool available;
};

struct lbjson_duplex {
	char type[16];
	bool available;
};

struct lbjson_serial_device {
	struct lbjson_baudrate bauds[24];
	struct lbjson_data_bits data_bits[4];
	struct lbjson_stop_bits stop_bits[2];
	struct lbjson_parity parity[5];
	struct lbjson_flow_control flow_control[3];
	struct lbjson_duplex duplex[3];

	char device[64];
	char path[64];
};

int lbjson_parse_serial(struct lbjson_serial_device *devices, int max_count, struct blob_attr *data, struct lbjson_hwinfo *hw);

////////////////////////////

// struct that represents most of /etc/board.json
struct lbjson_board {
	// model info
	char *model_id;
	char *model_name;
	char *model_platform;

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
	struct lbjson_hwinfo hw;

	// serial info
	int device_count;
	struct lbjson_serial_device devices[BJSON_SERIAL_DEVICES_MAX];

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

// Frees internal cache of board struct.
// Invalidates all structs gotten from other functions.
// A good place to call this function would be just
// before the program closes so that valgrind is happy.
void lbjson_free();

bool is_string_defined(const char *str);
void free_string(char **text);

// macros that retreive and null-check parsed blobmsg values
#define blob_get_str_nullchk(value) (value) ? strdup(blobmsg_get_string(value)) : ""
#define blob_get_bool_nullchk(value) (value) ? blobmsg_get_bool(value) : 0
#define blob_get_num_nullchk(value) (value) ? atoi(blobmsg_get_string(value)) : 0 // for string->int conversion
#define blob_get_u32_nullchk(value) (value) ? blobmsg_get_u32(value) : 0
#define blob_memdup_nullchk(value) (value) ? blob_memdup(value) : NULL

#ifdef __cplusplus
}
#endif

#endif /* LIBBOARDJSON_H */
