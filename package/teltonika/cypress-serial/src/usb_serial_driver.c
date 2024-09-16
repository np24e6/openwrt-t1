#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/spinlock.h>
#include <linux/usb.h>
#include <linux/usb/serial.h>
#include <linux/serial.h>
#include <linux/kfifo.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <asm/unaligned.h>
#include <linux/device.h>

#include "chip.h"

static int reconf_probe(struct usb_interface *intf, const struct usb_device_id *id);
static int cyp_write_room(struct tty_struct *tty);
static void cyp_set_termios(struct tty_struct *tty, struct usb_serial_port *port, struct ktermios *old_termios);
static void cyp_read_int_callback(struct urb *urb);
static int cyp_prepare_write_buffer(struct usb_serial_port *port, void *dest, size_t size);

static bool cdc;
static char *path_only;

module_param(cdc, bool, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(cdc, "N == reconf to vendor | Y == reconf to CDC");

module_param(path_only, charp, 0000);
MODULE_PARM_DESC(path_only, "A string of USB dev path to reconfigure");


static const struct usb_device_id id_table_reconf[] = {
 {USB_DEVICE_AND_INTERFACE_INFO(CYP_VID, CYP_PID_CDC, 255, 5, 0)},
 {USB_DEVICE_AND_INTERFACE_INFO(CYP_VID, CYP_PID_VENDOR, 255, 5, 0)},
 {0},
};

static const struct usb_device_id id_table_serial[] = {
 {USB_DEVICE_AND_INTERFACE_INFO(CYP_VID, CYP_PID_VENDOR, 255, 1, 0)},
 {0},
};

static const struct usb_device_id id_table_combined[] = {
 {USB_DEVICE_AND_INTERFACE_INFO(CYP_VID, CYP_PID_CDC, 255, 5, 0)},
 {USB_DEVICE_AND_INTERFACE_INFO(CYP_VID, CYP_PID_VENDOR, 255, 5, 0)},
 {USB_DEVICE_AND_INTERFACE_INFO(CYP_VID, CYP_PID_VENDOR, 255, 1, 0)},
 {0},
};

MODULE_DEVICE_TABLE(usb, id_table_combined)

static struct usb_driver reconf_driver = {
 .name     = "cypress_reconf",
 .probe    = reconf_probe,
 .id_table = id_table_reconf,
};

static struct usb_serial_driver serial_device = {
 .driver = {
  .owner = THIS_MODULE,
  .name  = "cypress_serial",
 },
 .description = "CY7C65213",
 .id_table    = id_table_serial,
 .num_ports   = 1,
 .port_probe  = chip_uart_probe,
 .port_remove = chip_uart_remove,
 .open        = chip_uart_open,
 .close       = chip_uart_close,
 .dtr_rts     = chip_uart_dtr_rts,
 .set_termios = cyp_set_termios,
 //.init_termios = cyp_init_termios,
 .tiocmset             = chip_uart_tiocmset,
 .read_int_callback    = cyp_read_int_callback,
 .write_room           = cyp_write_room,
 .prepare_write_buffer = cyp_prepare_write_buffer,

 .tiocmiwait          = usb_serial_generic_tiocmiwait,
 .chars_in_buffer     = usb_serial_generic_chars_in_buffer,
 .write               = usb_serial_generic_write,
 .throttle            = usb_serial_generic_throttle,
 .unthrottle          = usb_serial_generic_unthrottle,
 .resume              = usb_serial_generic_resume,
 .process_read_urb    = usb_serial_generic_process_read_urb,
 .read_bulk_callback  = usb_serial_generic_read_bulk_callback,
 .write_bulk_callback = usb_serial_generic_write_bulk_callback,
 .get_icount          = usb_serial_generic_get_icount,
};

static struct usb_serial_driver *const serial_drivers[] = {
 &serial_device,
 NULL,
};


#define PATH_BUF_LEN 16
static int reconf_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	struct usb_device *udev;
	char usb_path[PATH_BUF_LEN];

	udev = interface_to_usbdev(intf);

	if ((!cdc && id->idProduct == CYP_PID_CDC) ||
		(cdc && id->idProduct == CYP_PID_VENDOR)) {

		if (path_only && strlen(path_only) > 0) {
			snprintf(usb_path, PATH_BUF_LEN, "%s-%s",udev->parent->devpath, udev->devpath);

			if(strcmp(usb_path, path_only) == 0) {
				dev_info(&udev->dev, "Matching device for reconf found on: %s", usb_path);
			} else {
				dev_info(&udev->dev, "Device path not match! Skipping reconf. [%s]", usb_path);
				return -ENODEV;
			}
		}

		return chip_reconfig(udev, cdc);
	}

	return -ENODEV;
}

//////////////////////////////////////////////////////////////////////////////
static int cyp_prepare_write_buffer(struct usb_serial_port *port, void *dest, size_t size)
{
	return kfifo_out_locked(&port->write_fifo, dest, size, &port->lock);
}

static int cyp_write_room(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	unsigned long flags;
	int room;

	if (!port->bulk_out_size)
		return 0;

	spin_lock_irqsave(&port->lock, flags);
	room = kfifo_avail(&port->write_fifo);
	spin_unlock_irqrestore(&port->lock, flags);

	dev_dbg(&port->dev, "%s - returns %d", __func__, room);
	return room;
}

static void cyp_set_termios(struct tty_struct *tty,
                            struct usb_serial_port *port, struct ktermios *old_termios)
{
	if (old_termios && !tty_termios_hw_change(&tty->termios, old_termios)) {
		dev_dbg(&port->dev, "termios didn't change");
		return;
	}

	if (chip_uart_cfg_set(&tty->termios, port)) {
		dev_err(&port->dev, "setting termios failed");
	}
}

static void cyp_read_int_callback(struct urb *urb)
{
	// the generic read funcs require this
}
//////////////////////////////////////////////////////////////////////////////

static int __init cyp_init(void)
{
	int retval = usb_register(&reconf_driver);
	if (retval) {
		return retval;
	}

	retval = usb_serial_register_drivers(serial_drivers, KBUILD_MODNAME, id_table_serial);
	if (retval) {
		return retval;
	}

	return 0;
}

static void __exit cyp_exit(void)
{
	usb_serial_deregister_drivers(serial_drivers);
	usb_deregister(&reconf_driver);
}

module_init(cyp_init);
module_exit(cyp_exit);

MODULE_AUTHOR("Eimantas Bunevičius <eimantas.bunevicius@teltonika.lt>");
MODULE_DESCRIPTION("Cypress CY7C65213 USB to Serial chip driver with flow control support");
MODULE_LICENSE("GPL v2");
