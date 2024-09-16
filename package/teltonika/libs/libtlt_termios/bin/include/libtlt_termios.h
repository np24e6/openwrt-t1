#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <termios.h>
#include <uci.h>
#include <libboardjson.h>

#define MAX_DEVICE_LENGTH 64
#define MAX_ERROR_LENGTH  256

struct libtlt_termios_settings {
	uint32_t bps;
	uint8_t data_bits;
	uint8_t stop_bits;
	char parity[8];
	char flow_control[16];
	bool full_duplex;
	bool echo;
	char device[MAX_DEVICE_LENGTH];
	int fd;

	char error[MAX_ERROR_LENGTH];
	struct termios termios;
};

int ltermios_read_settings(struct libtlt_termios_settings *s, struct uci_context *ctx, const char *config_name, const char *uci_section);
int ltermios_set_settings(struct libtlt_termios_settings *s);
int ltermios_validate_settings(struct libtlt_termios_settings *s);
void ltermios_debug(struct libtlt_termios_settings *s);
int ltermios_set_duplex(const char *device, const bool full_duplex);

#ifdef __cplusplus
}
#endif
