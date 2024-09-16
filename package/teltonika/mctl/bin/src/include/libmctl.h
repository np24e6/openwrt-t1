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

void lmctl_gpio_modem_off(const char *modem_id, mctl_opt_t opt);
void lmctl_gpio_modem_reboot(const char *modem_id, mctl_opt_t opt);
void lmctl_gpio_modem_power(const char *modem_id, mctl_opt_t opt);

#endif // __LIBMCTL_H_
