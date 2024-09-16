#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/tty.h>
#include <linux/usb.h>
#include <linux/usb/serial.h>
#include <linux/byteorder/generic.h>

#include "chip.h"

#define CONF_LEN 512
#define CONF_CSUM_LEN 4

static inline int set_flash_rw_access(struct usb_device *dev)
{
	return usb_control_msg(dev,
	                       usb_sndctrlpipe(dev, 0),
	                       0xE2, // CY_VENDOR_ENTER_MFG_MODE
	                       USB_TYPE_VENDOR | USB_RECIP_INTERFACE | USB_DIR_OUT,
	                       0xA6BC, // expects in machine endianess
	                       0xB1B0,
	                       NULL,
	                       0,
	                       UCM_TIMEOUT);
}

static inline int get_silicon_id(struct usb_device *dev, u32 *id)
{
#define SID_LEN 4

	unsigned char *buffer = kmalloc(SID_LEN, GFP_KERNEL);
	if (!buffer) {
		return -ENOMEM;
	}

	int status = usb_control_msg(dev,
	                             usb_rcvctrlpipe(dev, 0),
	                             0xB1, // CY_BOOT_CMD_GET_SILICON_ID
	                             USB_TYPE_VENDOR | USB_RECIP_INTERFACE | USB_DIR_IN,
	                             0,
	                             0,
	                             buffer, // complains if this is stack mem
	                             SID_LEN,
	                             UCM_TIMEOUT);

	if (status != SID_LEN) {
		if (status >= 0) {
			status = -EIO;
		}

	} else {
		memcpy(id, buffer, SID_LEN);
		le32_to_cpus(id);
		status = 0;
	}

	kfree(buffer);
	return status;
}

static inline int reset_chip(struct usb_device *dev)
{
	return usb_control_msg(dev,
	                       usb_rcvctrlpipe(dev, 0),
	                       0xE3, // CY_DEVICE_RESET_CMD
	                       USB_TYPE_VENDOR | USB_RECIP_INTERFACE | USB_DIR_IN,
	                       0xA6B6,
	                       0xADBA,
	                       NULL,
	                       0,
	                       UCM_TIMEOUT);
}

static inline int read_conf(struct usb_device *dev, unsigned char **buffer)
{
	*buffer = kmalloc(CONF_LEN, GFP_KERNEL);
	if (!*buffer) {
		return -ENOMEM;
	}

	int status = usb_control_msg(dev,
	                             usb_rcvctrlpipe(dev, 0),
	                             0xB5, // CY_BOOT_CMD_READ_CONFIG
	                             USB_TYPE_VENDOR | USB_RECIP_INTERFACE | USB_DIR_IN,
	                             0,
	                             0,
	                             *buffer,
	                             CONF_LEN,
	                             UCM_TIMEOUT);

	if (status != CONF_LEN) {
		kfree(*buffer);

		if (status >= 0) {
			return -EIO;
		}

		return status;
	}

	return 0;
}

static inline int write_conf(struct usb_device *dev, unsigned char *buffer)
{
	int status = usb_control_msg(dev,
	                             usb_sndctrlpipe(dev, 0),
	                             0xB6, // CY_BOOT_CMD_PROG_CONFIG
	                             USB_TYPE_VENDOR | USB_RECIP_INTERFACE | USB_DIR_OUT,
	                             0,
	                             0,
	                             buffer,
	                             CONF_LEN,
	                             UCM_TIMEOUT);

	if (status != CONF_LEN) {
		if (status >= 0) {
			return -EIO;
		}

		return status;
	}

	return 0;
}

static inline void calc_conf_checksum(unsigned char *buffer, unsigned char *out)
{
	u32 tmp, sum = 0;
	for (size_t i = 12; i < 500; i += CONF_CSUM_LEN) {
		memcpy(&tmp, buffer + i, CONF_CSUM_LEN);
		le32_to_cpus(&tmp); // if only there was a memcpy_le32_to_cpu()

		sum += tmp;
	}

	cpu_to_le32s(&sum);
	memcpy(out, &sum, CONF_CSUM_LEN);
}

int chip_reconfig(struct usb_device *dev, bool cdc)
{
	int ret = -ENODEV;
	u32 silicon_id;
	if ((ret = get_silicon_id(dev, &silicon_id))) {
		dev_err(&dev->dev, "failed to retrieve silicon ID (%d)", ret);
		goto end;
	}

	switch (silicon_id) {
	case 0x8A1:
	case 0x8A2:
	case 0x8A3:
		dev_info(&dev->dev, "known silicon ID: 0x%x, proceeding with reconfig", silicon_id);
		break;

	default:
		dev_err(&dev->dev, "unknown silicon ID: 0x%x", silicon_id);
		goto end;
	}

	if ((ret = set_flash_rw_access(dev))) {
		dev_err(&dev->dev, "failed to acquire flash rw access (%d)", ret);
		goto end;
	}

	unsigned char *conf;
	if ((ret = read_conf(dev, &conf))) {
		dev_err(&dev->dev, "failed to read flash (%d)", ret);
		goto end; // read_conf() frees the buff on failure
	}

	unsigned char checksum_sanity[CONF_CSUM_LEN];
	calc_conf_checksum(conf, checksum_sanity);
	unsigned char *checksum_conf = conf + 8;

	for (size_t i = 0; i < CONF_CSUM_LEN; i++) {
		if (checksum_sanity[i] != checksum_conf[i]) {
			// unsure what just happened
			// maybe check the endianess handling in calc_conf_checksum()
			// could be other reasons
			dev_err(&dev->dev, "sanity check #1 failed - config checksum mismatch. won't proceed with chip reconfig");
			goto end_free_conf;
		}
	}

	static const u8 offset1 = 0x1d, offset2 = 0x96;
	static const u8 settings[2][2] = {{0x3, 0x6}, {0x1, 0x3}};
	//                                  vendor        cdc

	if (conf[offset1] != settings[!cdc][0] || conf[offset2] != settings[!cdc][1]) {
		// chip contains a different factory conf?
		dev_err(&dev->dev, "sanity check #2 failed - factory config fields different than expected");
		goto end_free_conf;
	}

	// changing the two bytes that reconfigure chip to proprietary mode
	conf[offset1] = settings[cdc][0];
	conf[offset2] = settings[cdc][1];

	calc_conf_checksum(conf, checksum_conf);

	if ((ret = write_conf(dev, conf))) {
		dev_err(&dev->dev, "failed to write flash (%d)", ret);
		goto end_free_conf;
	}

	if ((ret = reset_chip(dev))) { // so it reconnects with the new pid
		dev_err(&dev->dev, "\033[33;1;1mchip reset failed (%d)\033[0m, might need to power-cycle the chip", ret);

	} else {
		dev_info(&dev->dev, "chip reconfigured successfully");
	}

end_free_conf:
	kfree(conf);
end:
	return -ENODEV; // we don't want to attach it, ever
}
