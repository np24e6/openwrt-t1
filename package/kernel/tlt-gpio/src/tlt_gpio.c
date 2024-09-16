#include <linux/sysfs-mnfinfo.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/string.h>
#include <linux/of.h>

// #define DEBUG // Enable debug
#define GPIOCHIP_STR_SIZE 20
#define PROPERTY_STR_SIZE 35
#define TOKEN_STR_SIZE    10

#ifdef DEBUG
#define DEBUG_MESSAGE(...) pr_warn("tlt-gpio: " __VA_ARGS__)
#else
#define DEBUG_MESSAGE(...)
#endif

#define MNF_ERROR_STRING(s) pr_err("\033[1;31mtlt_gpio: error setting GPIO's - %s\033[0m\n", s)

static int read_mnf(const char **model, const char **hwver)
{
	*model = mnf_info_get_device_name();
	if (*model == NULL) {
		MNF_ERROR_STRING("mnf name is empty");
		return 1;
	}
	if(strlen(*model) < 6) {
		MNF_ERROR_STRING("mnf name is too short");
		return 1;
	}

	*hwver = mnf_info_get_hw_version();
	if (*hwver == NULL) {
		MNF_ERROR_STRING("mnf hwver is empty");
		return 1;
	}
	if (strlen(*hwver) < 4) {
		MNF_ERROR_STRING("mnf hwver is too short");
		return 1;
	}

	return 0;
}

static short get_gpio_offset(u32 mnf_hwver, u32 chip_id)
{
	struct device_node *version_node, *curr_node, *highest_ver_node = NULL;
	u32 curr_version = 0, highest_version = 0, chip_count = 0, chip_size = 0;
	int offset = -1;
	char *str_chip;
	int i;

	version_node = of_find_node_by_path("/tlt_gpios/gpiochip_info");
	if (!version_node) {
		DEBUG_MESSAGE("gpiochip_info node was not found\n");
		return offset;
	}

	for_each_child_of_node(version_node, curr_node) {
		if (of_property_read_u32(curr_node, "hwver", &curr_version)) {
			continue;
		}

		if ( (mnf_hwver >= curr_version) && (curr_version >= highest_version)) {
			highest_version = curr_version;
			highest_ver_node = curr_node;
		}
	}

	if (!highest_ver_node) {
		DEBUG_MESSAGE("gpiochip_info version property was not found\n");
		goto cleanup;
	}

	if (of_property_read_u32(highest_ver_node, "gpiochip_count", &chip_count)) {
		DEBUG_MESSAGE("gpiochip_count was not found\n");
		goto cleanup;
	}

	str_chip = kmalloc(GPIOCHIP_STR_SIZE, GFP_KERNEL);
	offset = 0;

	for(i = 0; i < chip_id; i++) {
		memset(str_chip, 0, GPIOCHIP_STR_SIZE);
		snprintf(str_chip, GPIOCHIP_STR_SIZE, "gpiochip_%d", i);
		if (of_property_read_u32(highest_ver_node, str_chip, &chip_size)) {
			DEBUG_MESSAGE("failed to read individual gpiochip size\n");
			offset = -1;
			goto cleanup;
		}
		offset += chip_size;
	}

cleanup:
	of_node_put(version_node);

	return offset;
}

static const char *gpio_find_node(char *node_name, const char *model, u32 mnf_hwver, char *active_low)
{
	struct device_node *gpio_node;
	struct property *prop;
	const char *ret_str = NULL;
	char *version_property_name;
	char *model_property_name;
	char *line_property_name;
	int prop_count = 0;
	int i;

	gpio_node = of_find_node_by_name(of_find_node_by_name(NULL, "tlt_gpios"), node_name);
	if (!gpio_node) {
		DEBUG_MESSAGE("Didn't find node tlt_gpios or %s\n", node_name);
		return ret_str;
	}

	DEBUG_MESSAGE("Processing node:%s\n", node_name);
	for_each_property_of_node(gpio_node, prop) {
		if (strstr(prop->name, "line_name")) {
			prop_count++;
		}
	}

	if (prop_count < 1) {
		goto cleanup;
	}

	version_property_name = kmalloc(PROPERTY_STR_SIZE, GFP_KERNEL);
	model_property_name = kmalloc(PROPERTY_STR_SIZE, GFP_KERNEL);
	line_property_name = kmalloc(PROPERTY_STR_SIZE, GFP_KERNEL);

	for (i = 0; i < prop_count; i++) {
		const char *str_model;
		const char *str_line;
		u32 lowest_version = 0;
		u32 highest_version = 0;

		memset(version_property_name, 0, PROPERTY_STR_SIZE);
		memset(model_property_name, 0, PROPERTY_STR_SIZE);
		memset(line_property_name, 0, PROPERTY_STR_SIZE);

		if (prop_count == 1) {
			snprintf(version_property_name, PROPERTY_STR_SIZE, "compatible_versions");
			snprintf(model_property_name, PROPERTY_STR_SIZE, "compatible_model");
			snprintf(line_property_name, PROPERTY_STR_SIZE, "line_name");
		} else {
			snprintf(version_property_name, PROPERTY_STR_SIZE, "compatible_versions_%d", i);
			snprintf(model_property_name, PROPERTY_STR_SIZE, "compatible_model_%d", i);
			snprintf(line_property_name, PROPERTY_STR_SIZE, "line_name_%d", i);
		}

		if (of_property_read_string(gpio_node, line_property_name, &str_line) != 0) {
			DEBUG_MESSAGE("Didn't find line_name property\n");
			goto cleanup_mem_free;
		}

		if (of_property_read_string(gpio_node, model_property_name, &str_model) == 0) {
			if (strstr(str_model, model) == NULL) {
				DEBUG_MESSAGE("Models don't match in %s\n", line_property_name);
				continue;
			}
		}

		if (of_property_read_u32_index(gpio_node, version_property_name, 0, &lowest_version) == 0) {
			if (of_property_read_u32_index(gpio_node, version_property_name, 1, &highest_version) == 0) {
				if ((mnf_hwver < lowest_version) || (mnf_hwver > highest_version)) {
					continue;
				}
			}
		}

		DEBUG_MESSAGE("Gpio line name is %s\n", str_line);
		if (of_find_property(gpio_node, "active_low", NULL)) {
			DEBUG_MESSAGE("Found active_low property\n");
			*active_low = 1;
		} else {
			*active_low = 0;
		}

		ret_str = str_line;
		break;
	}

cleanup_mem_free:
	kfree(version_property_name);
	kfree(model_property_name);
	kfree(line_property_name);
cleanup:
	of_node_put(gpio_node);

	return ret_str;
}

static int set_line_name(struct gpio_chip *gc, int gpio_offset, const char *device, u32 hwver)
{
	char active_low = 0;
	char ret = 0;
	int i = 0;

	for (i = gpio_offset; i < (gc->ngpio + gpio_offset); i++) {
		const char *str = NULL;
		char *token = NULL;

		token = kmalloc(TOKEN_STR_SIZE, GFP_KERNEL);
		snprintf(token, TOKEN_STR_SIZE, "GPIO_%d", i);

		if ((str = gpio_find_node(token, device, hwver, &active_low)) != NULL) {
			if (active_low) {
				tlt_gpio_set_active_low(gc, i-gpio_offset);
			}
			tlt_gpio_set_line_name(gc, str, i-gpio_offset);
		} else {
			ret = tlt_gpio_set_line_name(gc, token, i-gpio_offset);
			if (!ret) {
				continue;
			}
		}
		kfree(token); // Only free if tlt_gpio_set_line_name failed
	}

	return 0;
}

static int __init tlt_gpio_init(void)
{
	struct gpio_chip *gc = NULL;
	const char *device_long = NULL;
	const char *hwver_long = NULL;
	char *device = NULL;
	int gpio_offset = 0, chip_index = 0;
	u32 hwver = 0;
	char ret = 0;

	ret = read_mnf(&device_long, &hwver_long);
	if (ret) {
		DEBUG_MESSAGE("MNF read failed, exiting\n");
		return -1;
	}

	device = (char *) kzalloc(sizeof(char) * 7, GFP_KERNEL);
	strncpy(device, device_long, 6);
	hwver = 10 * (hwver_long[0] - '0') + (hwver_long[1] - '0');
	if(hwver == 0) {
		hwver = 10 * (hwver_long[2] - '0') + (hwver_long[3] - '0');
	}

	while ((gc = gpiochip_find_by_id(chip_index))) {
		DEBUG_MESSAGE("Found a chip with name %s\n", gc->label);
		if (chip_index > 0) {
			gpio_offset = get_gpio_offset(hwver, chip_index);
			if (gpio_offset < 0) {
				DEBUG_MESSAGE("Failed to get gpiochip offset, exiting\n");
				return -1;
			}
		}
		set_line_name(gc, gpio_offset, (const char *) device, hwver);
		chip_index++;
		pr_info("tlt-gpio: set names for %s chip\n", gc->label);
	}

	return 0;
}

static void __exit tlt_gpio_exit(void) {
	return;
}

module_init(tlt_gpio_init);
module_exit(tlt_gpio_exit);

MODULE_AUTHOR("Kasparas Elzbutas <kasparas.elzbutas@teltonika.lt>");
MODULE_DESCRIPTION("GPIOCHIP modification kernel module");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");
