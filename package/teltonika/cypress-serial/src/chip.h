#pragma once

#include <linux/usb/serial.h>
#include <linux/tty.h>

#define CYP_VID 0x04b4
#define CYP_PID_CDC 0x0003
#define CYP_PID_VENDOR 0x0006
#define UCM_TIMEOUT 5000

int chip_uart_cfg_set(struct ktermios *termios, struct usb_serial_port *port);
int chip_uart_tiocmset(struct tty_struct *tty, unsigned int set, unsigned int clear);
void chip_uart_dtr_rts(struct usb_serial_port *port, int on);
//int chip_uart_write(struct tty_struct *tty, struct usb_serial_port *port,
					//const unsigned char *buf, int count);
int chip_uart_probe(struct usb_serial_port *);
int chip_uart_open(struct tty_struct *, struct usb_serial_port *);
void chip_uart_close(struct usb_serial_port *);
int chip_uart_remove(struct usb_serial_port *);

int chip_reconfig(struct usb_device *, bool cdc);
