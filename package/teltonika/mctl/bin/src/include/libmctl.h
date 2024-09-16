#ifndef __LIBMCTL_H_
#define __LIBMCTL_H_

#define MODEM1_RESET "/sys/class/gpio/modem_reset/value"
#define MODEM1_POWER "/sys/class/gpio/modem_power/value"
#define MODEM2_RESET "/sys/class/gpio/modem2_reset/value"
#define MODEM2_POWER "/sys/class/gpio/modem2_power/value"

#define MODEM_BASEBAND_PATH "/sys/modem/reboot"

#define MCTL_CLEAR "0"
#define MCTL_SET   "1"

typedef enum {
	MCTL_OPT_CHECK,
	MCTL_OPT_SKIP,
} mctl_opt_t;

typedef enum {
	MCTL_METHOD_UBUS,
	MCTL_METHOD_DIRECT,
} mctl_method_t;

typedef enum {
	MTCL_MODEM_UNKNOWN,
	MCTL_MODEM_ID,
	MCTL_MODEM_NUM,
	MCTL_MODEM_ALL,
} mctl_specified_modem;

typedef enum {
	MCTL_ACTION_UNKNOWN,
	MCTL_ACTION_ON,
	MCTL_ACTION_OFF,
	MCTL_ACTION_RESET,
	MCTL_ACTION_REBOOT,
	MCTL_ACTION_START_T,
	MCTL_ACTION_STOP_T
} mctl_action;

struct modems_data {
	char *id;
	int num;
	mctl_action action;
	mctl_specified_modem apply_for;
	mctl_opt_t opt;
	mctl_method_t method;
};

void lmctl_gpio_modem_off(const char *modem_id, mctl_opt_t opt);
void lmctl_gpio_modem_reboot(const char *modem_id, mctl_opt_t opt);
void lmctl_gpio_modem_power(const char *modem_id, mctl_opt_t opt);
int  lmctl_execute(struct modems_data *modem);
/* Use example of the start trac method for default modem

	struct modems_data modem = { 0 };
	int ret;

	modem.apply_for = MCTL_MODEM_NUM;
	modem.action    = MCTL_ACTION_START_T;
	ret = lmctl_execute(&modem);

*/	
#endif // __LIBMCTL_H_
