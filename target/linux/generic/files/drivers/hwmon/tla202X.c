/*SPDX-License-Identifier: GPL-2.0
*
*Texas Instruments TLA2021/TLA2022/TLA2024 12-bit ADC driver
*
*Teltonika Networks 2023
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hwmon.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/delay.h>

#define TLA2021_DATA	       0x00
#define TLA2021_DATA_RES_MASK  GENMASK(15, 4)
#define TLA2021_DATA_RES_SHIFT 4

#define TLA2021_CONF		0x01
#define TLA2021_CONF_OS_MASK	BIT(15)
#define TLA2021_CONF_OS_SHIFT	15
#define TLA2021_CONF_MUX_MASK	GENMASK(14, 12)
#define TLA2021_CONF_MUX_SHIFT	12
#define TLA2021_CONF_PGA_MASK	GENMASK(11, 9)
#define TLA2021_CONF_PGA_SHIFT	9
#define TLA2021_CONF_MODE_MASK	BIT(8)
#define TLA2021_CONF_MODE_SHIFT 8
#define TLA2021_CONF_DR_MASK	GENMASK(7, 5)
#define TLA2021_CONF_DR_SHIFT	5

#define TLA2021_CONV_RETRY 10

enum chips {
	tla2021,
};

struct tla2021_data {
	struct device *hwmon_dev;
	u8 output_res;
	struct mutex lock;
};

static int tla2021_get(struct i2c_client *client, u8 addr, u16 mask, u16 shift, u16 *val)
{
	int ret;
	u16 data;

	ret = i2c_smbus_read_word_swapped(client, addr);
	if (ret < 0)
		return ret;

	data = ret;
	*val = (mask & data) >> shift;

	return 0;
}

static int tla2021_set(struct i2c_client *client, u8 addr, u16 mask, u16 shift, u16 val)
{
	int ret;
	u16 data;
	ret = i2c_smbus_read_word_swapped(client, addr);

	if (ret < 0)
		return ret;

	data = ret;
	data &= ~mask;
	data |= mask & (val << shift);

	ret = i2c_smbus_write_word_swapped(client, addr, data);

	return ret;
}

static int tla2021_wait(struct i2c_client *client)
{
	int ret;
	unsigned int retry = TLA2021_CONV_RETRY;
	u16 status;

	do {
		if (!--retry)
			return -EIO;
		ret = tla2021_get(client, TLA2021_CONF, TLA2021_CONF_OS_MASK, TLA2021_CONF_OS_SHIFT, &status);
		if (ret < 0)
			return ret;
		if (!status)
			usleep_range(25, 1000);
	} while (!status);

	return ret;
}

static int tla2021_singleshot_conv(struct i2c_client *client, int *val)
{
	int ret;
	u16 data;
	s16 tmp;

	ret = tla2021_set(client, TLA2021_CONF, TLA2021_CONF_MODE_MASK, TLA2021_CONF_MODE_SHIFT, 1);
	if (ret < 0)
		return ret;

	ret = tla2021_set(client, TLA2021_CONF, TLA2021_CONF_MUX_MASK, TLA2021_CONF_MUX_SHIFT, 0);
	if (ret < 0)
		return ret;

	ret = tla2021_set(client, TLA2021_CONF, TLA2021_CONF_OS_MASK, TLA2021_CONF_OS_SHIFT, 1);
	if (ret < 0)
		return ret;

	ret = tla2021_wait(client);
	if (ret < 0)
		return ret;

	ret = tla2021_get(client, TLA2021_DATA, TLA2021_DATA_RES_MASK, TLA2021_DATA_RES_SHIFT, &data);
	if (ret < 0)
		return ret;

	tmp  = (s16)(data << TLA2021_DATA_RES_SHIFT);
	*val = tmp >> TLA2021_DATA_RES_SHIFT;
	return 0;
}

static ssize_t in0_input_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct tla2021_data *data = i2c_get_clientdata(client);
	int ret, raw_val, val;

	mutex_lock(&data->lock);
	ret = tla2021_singleshot_conv(client, &raw_val);
	mutex_unlock(&data->lock);

	if (raw_val < 0 || ret < 0) {
		val = 0;
	} else {
		val = (raw_val * 1621) / 1000; // diff between ADC MCP3021 and TLA2021 raw values is 1.621
	}

	return sprintf(buf, "%d\n", val);
}

static DEVICE_ATTR_RO(in0_input);

static int tla2021_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int err, ret, idx;
	struct tla2021_data *data = NULL;
	struct device_node *np	  = client->dev.of_node;
	u16 status;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
		return -ENODEV;

	data = devm_kzalloc(&client->dev, sizeof(struct tla2021_data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	i2c_set_clientdata(client, data);

	//ADC not always respond to first data request, when booting. Second request seems to be successful.
	for (idx = 0; idx < 3; idx++) {
		ret = tla2021_get(client, TLA2021_CONF, TLA2021_CONF_OS_MASK, TLA2021_CONF_OS_SHIFT, &status);
		if (ret > 0)
			break;
		usleep_range(100, 1000);
	}

	if (ret < 0)
		return -ENODEV;

	dev_info(&client->dev, "Found ADC: TLA2021");

	err = sysfs_create_file(&client->dev.kobj, &dev_attr_in0_input.attr);
	if (err)
		return err;

	data->hwmon_dev = hwmon_device_register_with_info(&client->dev, "tla2021", NULL, NULL, NULL);

	if (IS_ERR(data->hwmon_dev)) {
		err = PTR_ERR(data->hwmon_dev);
		goto exit_remove;
	}

	return 0;

exit_remove:
	sysfs_remove_file(&client->dev.kobj, &dev_attr_in0_input.attr);
	return err;
}

static int tla2021_remove(struct i2c_client *client)
{
	struct tla2021_data *data = i2c_get_clientdata(client);

	hwmon_device_unregister(data->hwmon_dev);
	sysfs_remove_file(&client->dev.kobj, &dev_attr_in0_input.attr);

	return 0;
}

static const struct i2c_device_id tla2021_id[] = { { "tla2021", tla2021 }, {} };
MODULE_DEVICE_TABLE(i2c, tla2021_id);

#ifdef CONFIG_OF
static const struct of_device_id of_tla2021_match[] = { { .compatible = "ti,tla2021",
							  .data	      = (void *)tla2021 },
							{} };
MODULE_DEVICE_TABLE(of, of_tla2021_match);
#endif

static struct i2c_driver tla2021_driver = {
	.driver = {
		.name = "tla2021",
		.of_match_table = of_match_ptr(of_tla2021_match),
	},
	.probe = tla2021_probe,
	.remove = tla2021_remove,
	.id_table = tla2021_id,
};

module_i2c_driver(tla2021_driver);

MODULE_DESCRIPTION("Texas Instruments TLA2021/TLA2022/TLA2024 driver");
MODULE_LICENSE("GPL");