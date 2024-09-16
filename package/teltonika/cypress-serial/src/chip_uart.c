#include <linux/slab.h>
#include <linux/tty.h>
#include <linux/usb.h>
#include <linux/usb/serial.h>
#include <linux/byteorder/generic.h>

#include "chip.h"

#define UART_CFG_LEN 16
#define ERR_LEN 32
#define TX_BUFF_LEN 2048

typedef struct __attribute__((__packed__)) {
	__le32 baud;
	__u8 pin_type;   // ???
	__u8 data_width; // either 7 or 8
	__u8 stop_bits;  // either 1 or 2
	__u8 mode;       // ???
	__u8 parity;
	__u8 is_msb_first;
	__u8 tx_retry;
	__u8 rx_invert_polarity;
	__u8 rx_drop_on_error;
	__u8 is_flow_control;
	__u8 is_loop_back;
	__u8 flags;
} cfg_t;

typedef struct {
	cfg_t cfg;
	u8 rts, dtr;
} priv_t;

static void closest_baud(u32 *baud, char **warn)
{
	static const u32 bauds[] = {
	 300,
	 600,
	 1200,
	 2400,
	 4800,
	 9600,
	 14400,
	 19200,
	 38400,
	 56000,
	 57600,
	 115200,
	 230400,
	 460800,
	 921600,
	 1000000,
	 3000000,
	};
	static const size_t last = sizeof(bauds) / sizeof(bauds[0]) - 1;

	if (*baud < bauds[0]) {
		*warn = "baud too low";
		*baud = bauds[0];
		return;
	}

	if (*baud > bauds[last]) {
		*baud = bauds[last];
		*warn = "baud too high";
		return;
	}

	for (size_t i = 0; i <= last; i++) {
		const u32 *b = &bauds[i];
		if (*b == *baud) {
			return;
		}

		if (*baud < *b) {
			*warn = "incompatible baud";

			const u32 *prev = b - 1;
			if (*baud - *prev > (*b - *prev) / 2) {
				*baud = *b;

			} else {
				*baud = *--b;
			}

			return;
		}
	}
}

static inline int cfg_get(cfg_t *cfg, struct usb_device *dev)
{
	int status = usb_control_msg(dev,
	                             usb_rcvctrlpipe(dev, 0),
	                             0xC0, // CY_UART_GET_CONFIG_CMD
	                             USB_TYPE_VENDOR | USB_RECIP_INTERFACE | USB_DIR_IN,
	                             0,
	                             0,
	                             cfg,
	                             UART_CFG_LEN,
	                             UCM_TIMEOUT);

	if (status != UART_CFG_LEN) {
		return status >= 0 ? -EIO : status;
	}

	return 0;
}

static inline int cfg_set(cfg_t *cfg, struct usb_device *dev)
{
	int status = usb_control_msg(dev,
	                             usb_sndctrlpipe(dev, 0),
	                             0xC1, // CY_UART_SET_CONFIG_CMD
	                             USB_TYPE_VENDOR | USB_RECIP_INTERFACE | USB_DIR_OUT,
	                             0,
	                             0,
	                             cfg,
	                             UART_CFG_LEN,
	                             UCM_TIMEOUT);

	if (status != UART_CFG_LEN) {
		return status >= 0 ? -EIO : status;
	}

	return 0;
}

static inline int set_rts_dtr(struct usb_device *dev, u8 rts, u8 dtr)
{
	return usb_control_msg(dev,
	                       usb_sndctrlpipe(dev, 0),
	                       0x22, // CY_SET_LINE_CONTROL_STATE_CMD - set rts & dtr
	                       USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_OUT,
	                       (u16)(rts << 1) | dtr,
	                       0, // interface num
	                       NULL,
	                       0,
	                       UCM_TIMEOUT);
}

void chip_uart_dtr_rts(struct usb_serial_port *port, int on)
{
	u8 bool_on   = !!on;
	priv_t *priv = usb_get_serial_port_data(port);
	priv->rts    = bool_on;
	priv->dtr    = bool_on;

	int status;
	if ((status = set_rts_dtr(port->serial->dev, bool_on, bool_on))) {
		dev_err(&port->dev, "setting control lines failed (%d)", status);
	}
}

int chip_uart_cfg_set(struct ktermios *termios, struct usb_serial_port *port)
{
	u32 baud = (u32)tty_termios_baud_rate(termios);

	char *warn = NULL;
	closest_baud(&baud, &warn);
	if (warn) {
		dev_warn(&port->dev, "%s, setting to the closest supported: %u", warn, baud);
	}

	cpu_to_le32s(&baud);

	cfg_t *cfg = &((priv_t *)usb_get_serial_port_data(port))->cfg;

	cfg->baud             = baud;
	cfg->data_width       = (termios->c_cflag & CSIZE) == CS7 ? 7 : 8;
	cfg->stop_bits        = termios->c_cflag & CSTOPB ? 2 : 1;
	cfg->parity           = termios->c_cflag & PARENB ? (termios->c_cflag & PARODD ? 1 : 2) : 0;
	cfg->rx_drop_on_error = !!(termios->c_cflag & IGNPAR);

	int status = cfg_set(cfg, port->serial->dev);
	if (status) {
		goto end;
	}

	// flow control setting is not in the cfg
	status = usb_control_msg(port->serial->dev,
	                         usb_sndctrlpipe(port->serial->dev, 0),
	                         0x60, // CY_UART_SET_FLOW_CONTROL_CMD
	                         USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_OUT,
	                         termios->c_cflag & CRTSCTS ? 2 : 0,
	                         0,
	                         NULL,
	                         0,
	                         UCM_TIMEOUT);

end:
	if (status) {
		dev_err(&port->dev, "error setting termios (%d)", status);
	}

	return status;
}

// not doing the TIOCM_OUTn
int chip_uart_tiocmset(struct tty_struct *tty, unsigned int set, unsigned int clear)
{
#define SET_IF_CHANGED(_param, _tiocm) \
	do {                                  \
		if (set & _tiocm) {                  \
			_param    = 1;                      \
			unchanged = false;                  \
									   \
		} else if (clear & _tiocm) {         \
			_param    = 0;                      \
			unchanged = false;                  \
		}                                    \
	} while (0)

	struct usb_serial_port *port = tty->driver_data;
	priv_t *priv                 = usb_get_serial_port_data(port);

	int status;
	bool unchanged = true;
	SET_IF_CHANGED(priv->rts, TIOCM_RTS);
	SET_IF_CHANGED(priv->dtr, TIOCM_DTR);

	if (!unchanged && (status = set_rts_dtr(port->serial->dev, priv->rts, priv->dtr))) {
		dev_err(&port->dev, "error setting modem control lines (%d)", status);
		return status;
	}

	/////////////////////////////
	unchanged = true;
	SET_IF_CHANGED(priv->cfg.is_loop_back, TIOCM_LOOP);
	if (unchanged) {
		return 0;
	}

	status = cfg_set(&priv->cfg, port->serial->dev);
	if (status) {
		dev_err(&port->dev, "error setting TIOCM_LOOP (%d)", status);
	}
	return status;
}

int chip_uart_probe(struct usb_serial_port *port)
{
	priv_t *priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv) {
		return -ENOMEM;
	}

	usb_set_serial_port_data(port, priv);

	int status;
	// so that the mysterious cfg fields remain whatever they are
	if ((status = cfg_get(&priv->cfg, port->serial->dev))) {
		goto end;
	}

	status = chip_uart_cfg_set(&tty_std_termios, port);

end:
	return status;
}

int chip_uart_open(struct tty_struct *tty, struct usb_serial_port *port)
{
	int status;
	// so that driver and chip termios are in sync
	if ((status = chip_uart_cfg_set(&tty->termios, port))) {
		return status;
	}

	status = usb_submit_urb(port->interrupt_in_urb, GFP_KERNEL);
	if (status) {
		dev_err(&port->dev, "failed to submit interrupt urb: %d\n", status);
		return status;
	}

	status = usb_serial_generic_open(tty, port);
	if (status) {
		usb_kill_urb(port->interrupt_in_urb);
		return status;
	}

	return 0;
}

void chip_uart_close(struct usb_serial_port *port)
{
	usb_serial_generic_close(port);
	usb_kill_urb(port->interrupt_in_urb);
}

int chip_uart_remove(struct usb_serial_port *port)
{
	kfree(usb_get_serial_port_data(port));
	return 0;
}
