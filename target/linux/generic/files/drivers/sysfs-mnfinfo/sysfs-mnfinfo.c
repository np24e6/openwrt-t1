#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/ctype.h>
#include <linux/mtd/mtd.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>

#include <linux/sysfs-mnfinfo.h>

#define DRV_NAME "sysfs-mnfinfo"

#define SCAN_BLK_SIZE 512

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#define GEN_KOBJ_ATTR_RO(_name)                                                                              \
	static struct kobj_attribute kobj_attr_##_name = {                                                   \
		.attr = { .name = __stringify(_name), .mode = 0444 },                                        \
		.show = mnf_attr_show,                                                                       \
	};

#define LIST_KOBJ_ATTR(_name) &kobj_attr_##_name.attr,

#define GEN_MNF_ENTRIES(_field)                                                                              \
	{                                                                                                    \
		.is_set = false,                                                                             \
		.is_visible = false,                                                                         \
		.name	    = __stringify(_field),                                                           \
		.dt_len	    = 0,                                                                             \
		.data	    = NULL,                                                                          \
		.in_log	    = false,                                                                         \
	},

// info: https://en.wikipedia.org/wiki/X_macro
#define LIST_MNF_FIELDS(ACT, ...)                                                                            \
	ACT(mac, ##__VA_ARGS__)                                                                              \
	ACT(name, ##__VA_ARGS__)                                                                             \
	ACT(wps, ##__VA_ARGS__)                                                                              \
	ACT(serial, ##__VA_ARGS__)                                                                           \
	ACT(hwver, ##__VA_ARGS__)                                                                            \
	ACT(branch, ##__VA_ARGS__)                                                                           \
	ACT(batch, ##__VA_ARGS__)                                                                            \
	ACT(sim_cfg, ##__VA_ARGS__)                                                                          \
	ACT(sim_count, ##__VA_ARGS__)                                                                        \
	ACT(simpin1, ##__VA_ARGS__)                                                                          \
	ACT(simpin2, ##__VA_ARGS__)                                                                          \
	ACT(simpin3, ##__VA_ARGS__)                                                                          \
	ACT(simpin4, ##__VA_ARGS__)                                                                          \
	ACT(wpass, ##__VA_ARGS__)                                                                            \
	ACT(pass, ##__VA_ARGS__)                                                                             \
	ACT(blver, ##__VA_ARGS__)

struct mnfinfo_entry {
	bool is_set;
	bool is_visible;
	const char *name;
	size_t dt_len;
	char *data;
	bool in_log;
};

static ssize_t mnf_attr_show(struct kobject *kobj, struct kobj_attribute *attr, char *buffer);
static int mnfinfo_probe(struct platform_device *pdev);
static int mnfinfo_remove(struct platform_device *pdev);

static struct mnfinfo_entry mnf_data[] = { LIST_MNF_FIELDS(GEN_MNF_ENTRIES) };

LIST_MNF_FIELDS(GEN_KOBJ_ATTR_RO)

static struct attribute *g_mnfinfo_attr[] = {
	LIST_MNF_FIELDS(LIST_KOBJ_ATTR) NULL,
};

static ssize_t mnf_attr_show(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
	int i;

	(void)kobj;
	(void)attr;

	for (i = 0; i < ARRAY_SIZE(mnf_data); i++) {
		if (!strcmp(mnf_data[i].name, attr->attr.name)) {
			return sprintf(buffer, "%s", mnf_data[i].data);
		}
	}
	return -1;
}

static umode_t mnfinfo_attr_is_visible(struct kobject *kobj, struct attribute *attr, int n)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(mnf_data); i++) {
		if (!strcmp(mnf_data[i].name, attr->name) && mnf_data[i].is_visible) {
			return attr->mode;
		}
	}
	return 0;
}

static struct attribute_group g_mnfinfo_attr_group = {
	.attrs	    = g_mnfinfo_attr,
	.is_visible = mnfinfo_attr_is_visible,
};

static struct kobject *g_kobj;

static struct of_device_id mnfinfo_dt_ids[] = {
	{
		.compatible = DRV_NAME,
	},
	{},
};

MODULE_DEVICE_TABLE(of, mnfinfo_dt_ids);

static struct platform_driver mnf_driver = {
	.probe = mnfinfo_probe,
	.remove= mnfinfo_remove,
	.driver = {
		.name = DRV_NAME,
		.of_match_table = mnfinfo_dt_ids,
	},
};

static const char *get_data_of(const char *name) {
	struct mnfinfo_entry *e;

	for (e = mnf_data; e < e + ARRAY_SIZE(mnf_data); e++)
		if (!strcmp(e->name, name)) return e->data;

	return NULL;
}

static void strip_whitespaces(struct mnfinfo_entry *e)
{
	int i;
	u8 c;

	for (i = e->dt_len - 1; i >= 0; i--) {
		c = e->data[i];

		if (!c || c == 0xff || isspace(c)) {
			e->data[i] = 0;
		} else {
			return;
		}
	}
}

static void fix_rs_allign(struct mnfinfo_entry *e)
{
	int i;
	u8 c, *buf;
	int off = 0;
	bool found = false;
	buf = e->data;

	for (i = 0; i < e->dt_len; i++) {
		c = buf[i];

		if (!found) {
			found = !(!c || c == 0xff);
			off += !found;
			continue;
		}
		break;
	}

	memmove(buf, buf + off, e->dt_len - off);
	memset(buf + (e->dt_len - off), 0, off);
}

static int fix_mac_type(struct mnfinfo_entry *e, size_t len, const char *def)
{
	u8 tmp[6];

	const u8 empty_mac[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	if (def && (len != sizeof(empty_mac) || !memcmp(e->data, empty_mac, sizeof(empty_mac)))) {
		snprintf(e->data, e->dt_len, "%s", def);
		return 0;
	}

	memcpy(tmp, e->data, sizeof(tmp));

	snprintf(e->data, e->dt_len, "%02X%02X%02X%02X%02X%02X", tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], tmp[5]);
	return 0;
}

static int fix_sim_count(void)
{
	char simcnt;
	const char *simcfg;
	struct mnfinfo_entry *e;

	for (e = mnf_data; e < e + ARRAY_SIZE(mnf_data); e++)
		if (!strcmp(e->name, "sim_count") && e->data) break;
	if (e == e + ARRAY_SIZE(mnf_data)) return 1;

	if ((simcfg = get_data_of("sim_cfg")) == NULL) return 1;
	for (simcnt = 0; simcnt < 4; simcnt++, simcfg += 8) {
		if (*simcfg < '0' || *simcfg > '2') {
			if(simcnt == 0) return 1;
			else break;
		}
	}
	snprintf(e->data, e->dt_len, "%d", simcnt);
	return 0;
}

static int fix_simcfg_type(struct mnfinfo_entry *e, size_t len, const char *def)
{
	size_t i;
	char *simcfg;
	if ((simcfg = e->data) == NULL) return 1;

	for (i = 0; i < len; i++) {
		if ((*simcfg < '0' || *simcfg > '2') && *simcfg != '_') {
			if(i == 0) {
				if(!def) return 1;
				snprintf(e->data, e->dt_len, "%s", def);
				return 0;
			}
			*simcfg = 0;
		}
		simcfg++;
	}
	e->is_set = true;
	return 0;
}

static int fix_ascii_type(struct mnfinfo_entry *e, size_t len, const char *def)
{
	char *buf = e->data;

	if (!*buf || !isascii(*buf)) {
		if (!def) {
			*buf = 0;
			return 1;
		}
		else snprintf(e->data, e->dt_len, "%s", def);
		return 0;
	}

	for (buf++; buf < (e->data + len); buf++)
		if (!*buf || !isascii(*buf)) *buf = 0;

	return 0;
}

static int fix_alnum_type(struct mnfinfo_entry *e, size_t len, const char *def)
{
	char *buf = e->data;

	if (!*buf || !isalnum(*buf)) {
		if (!def) {
			*buf = 0;
			return 1;
		}
		else snprintf(e->data, e->dt_len, "%s", def);
		return 0;
	}

	for (buf++; buf < (e->data + len); buf++)
		if (!*buf || !isalnum(*buf)) *buf = 0;

	return 0;
}

static int fix_digit_type(struct mnfinfo_entry *e, size_t len, const char *def)
{
	char *buf = e->data;

	if (!*buf || !isdigit(*buf)) {
		if (!def) {
			*buf = 0;
			return 1;
		}
		else snprintf(e->data, e->dt_len, "%s", def);
		return 0;
	}

	for (buf++; buf < (e->data + len); buf++)
		if (!*buf || !isdigit(*buf)) *buf = 0;

	return 0;
}

static int fix_types(struct mnfinfo_entry *e, size_t len, const char *type, const char *def, bool strip,
		     bool rs_alligned)
{
	int rc = 0;

	if (rs_alligned) {
		fix_rs_allign(e);
	}

	if (!strcmp(type, "mac")) {
		rc = fix_mac_type(e, len, def);
	} else if (!strcmp(type, "ascii")) {
		rc = fix_ascii_type(e, len, def);
	} else if (!strcmp(type, "simcfg")) {
		rc = fix_simcfg_type(e, len, def);
	} else if (!strcmp(type, "alnum")) {
		rc = fix_alnum_type(e, len, def);
	} else if (!strcmp(type, "digit")) {
		rc = fix_digit_type(e, len, def);
	}

	if (!rc && strip) {
		strip_whitespaces(e);
	}

	return rc;
}

static long find_last_non_null_byte_idx(struct mtd_info *mtd, loff_t from)
{
	unsigned char buf[SCAN_BLK_SIZE + 1] = { 0 };
	long blk_off;
	size_t retlen;
	int i, status;

	blk_off = from - SCAN_BLK_SIZE; //last block can be smaller than block size

	while (blk_off > 0) {
		status = mtd_read(mtd, blk_off, SCAN_BLK_SIZE, &retlen, buf);
		if (status < 0) {
			return -1;
		}

		for (i = SCAN_BLK_SIZE - 1; i >= 0; i--) {
			if (buf[i] && buf[i] != 0xff) {
				return blk_off + i;
			}
		}

		blk_off -= SCAN_BLK_SIZE;
	}

	return -1;
}

static int find_trailling_data(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u_char *buf)
{
	long off;
	int status;

	off = find_last_non_null_byte_idx(mtd, from);
	if (off < 0) {
		pr_err("MTD partition %s was empty\n", mtd->name);
		return -1;
	}

	off++; // One byte after data in interest
	off -= len; // First byte of data in interest
	if (off < 0) {
		pr_err("Data not found\n");
		return -1;
	}

	status = mtd_read(mtd, off, len, retlen, buf);
	if (status || *retlen != len) {
		pr_err("Read %zu bytes from %lx failed with %d\n", len, off, status);
		return -1;
	}

	return 0;
}

const char *mnf_info_get_device_name(void)
{
	return get_data_of("name");
}
EXPORT_SYMBOL(mnf_info_get_device_name);

const char *mnf_info_get_hw_version(void)
{
	return get_data_of("hwver");
}
EXPORT_SYMBOL(mnf_info_get_hw_version);

const char *mnf_info_get_batch(void)
{
	return get_data_of("batch");
}
EXPORT_SYMBOL(mnf_info_get_batch);

static int parse_prop(struct device_node *node)
{
	struct device_node *mtdnode;
	struct mtd_info *mtd;
	size_t retlen;
	int status, size, i, z, di;
	const char *part, *def, *type, *str, *dev_name;
	int arr_hwver[2], cur_hwver;
	const __be32 *list, *min_rlen;
	phandle phandle;
	bool strip, right_side_alligned;
	struct mnfinfo_entry *e;

	for (e = mnf_data; e < e + ARRAY_SIZE(mnf_data); e++) {
		if (!strcmp(e->name, node->name)) {
			if(e->is_set) return 0;
			break;
		}
	}

	di = of_property_count_strings(node, "tlt-mnf,device");
	if (di > 0 && (dev_name = mnf_info_get_device_name())) {
		of_property_read_string_array(node, "tlt-mnf,device", &str, di);
		for (i = 0; i < di; i++, str++) {
			for (z=0; z < strlen(str); z++)
				if (str[z] != '*' && str[z] != dev_name[z]) break;
			if (z == strlen(str)) goto devcompat;
		}
		pr_debug("Property \"%s\" is incompatible with %s\n", node->full_name, dev_name);
		return 0;
	}
devcompat:

	if (of_property_read_u32_array(node, "tlt-mnf,hwver", arr_hwver, 2) == 0) {
		str = mnf_info_get_hw_version();
		if (kstrtoint(str, 10, &cur_hwver) != 0 || cur_hwver == 0) goto hvcompat;
		if (cur_hwver > 99) cur_hwver %= 100;
		if (arr_hwver[0] <= cur_hwver && cur_hwver <= arr_hwver[1])
			goto hvcompat;
		pr_debug("Property \"%s\" is incompatible with hw v%d\n", node->full_name, cur_hwver);
		return 0;
	}
hvcompat:

	if (e->data) {
		pr_debug("Data buffer of %s is not empty. Clearing... \n", e->name);
		kfree(e->data);
		e->data = NULL;
	}

	def = of_get_property(node, "default", NULL);
	list = of_get_property(node, "reg", &size);
	if (!list && def) {
		// If not "reg" property was found, then we assume that value is hardcoded directly in "default" property

		e->dt_len = strlen(def) + 1;
		e->data	  = kzalloc(e->dt_len, GFP_KERNEL);
		if (!e->data) {
			return -1;
		}

		strcpy(e->data, def);

		goto end;
	} else if (!list || (size != (3 * sizeof(*list)))) {
		pr_err("Bad \"reg\" attribute\n");
		return -1;
	}

	type = of_get_property(node, "type", NULL);
	if (!type) {
		pr_err("\"%s\" property not found\n", "type");
		return -1;
	}

	phandle = be32_to_cpup(list);
	if (phandle)
		mtdnode = of_find_node_by_phandle(phandle);

	if (!mtdnode) {
		pr_err("Bad phandle for mtd partition\n");
		return -1;
	}

	part = of_get_property(mtdnode, "label", NULL);
	of_node_put(mtdnode);
	if (!part) {
		pr_err("MTD label not found\n");
		return -1;
	}

	mtd = get_mtd_device_nm(part);
	if (IS_ERR(mtd)) {
		pr_warn("MTD partition: '%s' not found\n", part);
		return PTR_ERR(mtd);
	}

	min_rlen = of_get_property(node, "min-res-len", &size);
	if (min_rlen && (size == sizeof(*min_rlen)) && be32_to_cpup(list + 2) < be32_to_cpup(min_rlen)) {
		e->dt_len = be32_to_cpup(min_rlen) + 1;
	} else {
		e->dt_len = be32_to_cpup(list + 2) + 1;
	}

	e->data = kzalloc(e->dt_len, GFP_KERNEL);
	if (!e->data) {
		return -1;
	}

	if (of_find_property(node, "trailling-data", NULL)) {
		status = find_trailling_data(mtd, be32_to_cpup(list + 1), be32_to_cpup(list + 2), &retlen,
					     e->data);
	} else {
		status = mtd_read(mtd, be32_to_cpup(list + 1), be32_to_cpup(list + 2), &retlen, e->data);
	}

	put_mtd_device(mtd);
	if (status) {
		pr_err("Read %zu of %d bytes from \"%s\" mtd failed with: %d\n", retlen,
		       be32_to_cpup(list + 2), part, status);
		kfree(e->data);
		return -1;
	}

	strip		    = !!of_find_property(node, "strip-whitespaces", NULL);
	right_side_alligned = !!of_find_property(node, "right-side-alligned", NULL);

	if (fix_types(e, retlen, type, def, strip, right_side_alligned)) {
		pr_debug("Failed to fix type for \"%s\"\n", e->name);
		kfree(e->data);
		return -1;
	}

end:
	if (of_find_property(node, "log", NULL))
		e->in_log = true;

	e->is_visible = true;

	return 0;
}

static int mnfinfo_probe(struct platform_device *pdev)
{
	struct device *dev;
	struct device_node *dnode;
	struct device_node *cnode;
	struct mnfinfo_entry *e;
	int ret, i;

	dev   = &pdev->dev;
	dnode = dev->of_node;

	for_each_child_of_node(dnode, cnode)
	{
		ret = parse_prop(cnode);
		if (ret == -ENODEV)
			return -EPROBE_DEFER;
	}

	fix_sim_count();

	g_kobj = kobject_create_and_add("mnf_info", NULL);
	if (!g_kobj) {
		pr_err("Unable to create \"mnf_info\" kobject\n");
		return -ENOMEM;
	}

	if (sysfs_create_group(g_kobj, &g_mnfinfo_attr_group)) {
		pr_err("Unable to create \"mnf_info\" sysfs group\n");
		kobject_put(g_kobj);
		return -ENOMEM;
	}

	pr_info("MNFINFO PROPERTIES:\n");
	for (i = 0; i < ARRAY_SIZE(mnf_data); i++) {
		e = &mnf_data[i];
		if (e->in_log)
			pr_info(" %-9s: %s\n", e->name, e->data);
	}

	return 0;
}

static int mnfinfo_remove(struct platform_device *pdev)
{
	int i;

	kobject_put(g_kobj);

	for (i = 0; i < ARRAY_SIZE(mnf_data); i++) {
		kfree(mnf_data[i].data);
	}

	return 0;
}

module_platform_driver(mnf_driver);

MODULE_AUTHOR("Linas Perkauskas <linas.perkauskas@teltonika.lt>");
MODULE_DESCRIPTION("Module to read device individual RO data via sysfs");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRV_NAME);
