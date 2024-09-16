/*
 * uqmi -- tiny QMI support implementation
 *
 * Copyright (C) 2014-2015 Felix Fietkau <nbd@openwrt.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>

#include "uqmi.h"
#include "commands.h"

static struct blob_buf status;
bool single_line = false;

static void no_cb(struct qmi_dev *qmi, struct qmi_request *req, struct qmi_msg *msg)
{
}

static void cmd_version_cb(struct qmi_dev *qmi, struct qmi_request *req, struct qmi_msg *msg)
{
	struct qmi_ctl_get_version_info_response res;
	void *c;
	char name_buf[16];
	int i;

	qmi_parse_ctl_get_version_info_response(msg, &res);

	c = blobmsg_open_table(&status, NULL);
	for (i = 0; i < res.data.service_list_n; i++) {
		sprintf(name_buf, "service_%d", res.data.service_list[i].service);
		blobmsg_printf(&status, name_buf, "%d,%d",
			res.data.service_list[i].major_version,
			res.data.service_list[i].minor_version);
	}
	blobmsg_close_table(&status, c);
}

static enum qmi_cmd_result
cmd_version_prepare(struct qmi_dev *qmi, struct qmi_request *req, struct qmi_msg *msg, char *arg)
{
	qmi_set_ctl_get_version_info_request(msg);
	return QMI_CMD_REQUEST;
}

#define cmd_sync_cb no_cb
static enum qmi_cmd_result
cmd_sync_prepare(struct qmi_dev *qmi, struct qmi_request *req, struct qmi_msg *msg, char *arg)
{
	qmi_set_ctl_sync_request(msg);
	return QMI_CMD_REQUEST;
}

#define cmd_get_client_id_cb no_cb
static enum qmi_cmd_result
cmd_get_client_id_prepare(struct qmi_dev *qmi, struct qmi_request *req, struct qmi_msg *msg, char *arg)
{
	QmiService svc = qmi_service_get_by_name(arg);

	if (svc < 0) {
		fprintf(stderr, "Invalid service name '%s'\n", arg);
		return QMI_CMD_EXIT;
	}

	if (qmi_service_connect(qmi, svc, -1)) {
		fprintf(stderr, "Failed to connect to service\n");
		return QMI_CMD_EXIT;
	}

	printf("%d\n", qmi_service_get_client_id(qmi, svc));
	return QMI_CMD_DONE;
}

#define cmd_set_client_id_cb no_cb
static enum qmi_cmd_result
cmd_set_client_id_prepare(struct qmi_dev *qmi, struct qmi_request *req, struct qmi_msg *msg, char *arg)
{
	QmiService svc;
	int id;
	char *s;

	s = strchr(arg, ',');
	if (!s) {
		fprintf(stderr, "Invalid argument\n");
		return QMI_CMD_EXIT;
	}
	*s = 0;
	s++;

	id = strtoul(s, &s, 0);
	if (s && *s) {
		fprintf(stderr, "Invalid argument\n");
		return QMI_CMD_EXIT;
	}

	svc = qmi_service_get_by_name(arg);
	if (svc < 0) {
		fprintf(stderr, "Invalid service name '%s'\n", arg);
		return QMI_CMD_EXIT;
	}

	if (qmi_service_connect(qmi, svc, id)) {
		fprintf(stderr, "Failed to connect to service\n");
		return QMI_CMD_EXIT;
	}

	return QMI_CMD_DONE;
}

static int
qmi_get_array_idx(const char **array, int size, const char *str)
{
	int i;

	for (i = 0; i < size; i++) {
		if (!array[i])
			continue;

		if (!strcmp(array[i], str))
			return i;
	}

	return -1;
}

#define cmd_ctl_set_data_format_cb no_cb
static enum qmi_cmd_result
cmd_ctl_set_data_format_prepare(struct qmi_dev *qmi, struct qmi_request *req, struct qmi_msg *msg, char *arg)
{
	struct qmi_ctl_set_data_format_request sreq = {};
	const char *modes[] = {
		[QMI_CTL_DATA_LINK_PROTOCOL_802_3] = "802.3",
		[QMI_CTL_DATA_LINK_PROTOCOL_RAW_IP] = "raw-ip",
	};
	int mode = qmi_get_array_idx(modes, ARRAY_SIZE(modes), arg);

	if (mode < 0) {
		uqmi_add_error("Invalid mode (modes: 802.3, raw-ip)");
		return QMI_CMD_EXIT;
	}

	qmi_set_ctl_set_data_format_request(msg, &sreq);
	return QMI_CMD_DONE;
}

static void
reload_wwan_iface_name(struct qmi_dev *qmi)
{
	const char *cdc_wdm_device_name;
	static const char *driver_names[] = { "usbmisc", "usb" };
	uint32_t i;

	cdc_wdm_device_name = strrchr(qmi->fd_path, '/');
	if (!cdc_wdm_device_name) {
		uqmi_add_error("invalid path for cdc-wdm control port");
		return;
	}
	cdc_wdm_device_name++;

	for (i = 0; i < ARRAY_SIZE(driver_names) && !qmi->wwan_iface; i++) {
		char *sysfs_path;
		struct dirent *sysfs_entry;
		DIR *sysfs_fold;

		asprintf(&sysfs_path, "/sys/class/%s/%s/device/net",
			 driver_names[i], cdc_wdm_device_name);
		sysfs_fold = opendir(sysfs_path);
		if (!sysfs_fold) {
//			uqmi_add_error("Failed to open");
			continue;
		}

		while ((sysfs_entry = readdir(sysfs_fold))) {
			if (strstr(sysfs_entry->d_name, ".")) {
				continue;
			}
			/* We only expect ONE file in the sysfs directory corresponding
                         * to this control port, if more found for any reason, warn about it */
			if (qmi->wwan_iface) {
				uqmi_add_error("Invalid additional wwan iface found");
			} else {
				qmi->wwan_iface = strdup(sysfs_entry->d_name);
				break;
			}
		}

		closedir(sysfs_fold);
		free(sysfs_path);
	}

	if (!qmi->wwan_iface) {
		uqmi_add_error("wwan iface not found");
	}
}

static enum qmi_cmd_result
set_expected_data_format(char *sysfs_path,
			 QmiCtlDataLinkProtocol requested)
{
	enum qmi_cmd_result ret = QMI_CMD_EXIT;
	char value;
	FILE *f = NULL;

	if (requested == QMI_CTL_DATA_LINK_PROTOCOL_RAW_IP)
		value = 'Y';
	else if (requested == QMI_CTL_DATA_LINK_PROTOCOL_802_3)
		value = 'N';
	else
		goto out;

	if (!(f = fopen(sysfs_path, "w"))) {
		uqmi_add_error("Failed to open file for R/W");
		goto out;
	}

	if (fwrite(&value, 1, 1, f) != 1) {
		uqmi_add_error("Failed to write to file");
		goto out;
	}

	ret = QMI_CMD_DONE;
out:
	if (f)
		fclose(f);
	return ret;
}

static QmiCtlDataLinkProtocol get_expected_data_format(char *sysfs_path)
{
	QmiCtlDataLinkProtocol expected = QMI_CTL_DATA_LINK_PROTOCOL_UNKNOWN;
	char value = '\0';
	FILE *f;

	if (!(f = fopen(sysfs_path, "r"))) {
		uqmi_add_error("Failed to open file");
		goto out;
	}

	if (fread(&value, 1, 1, f) != 1) {
		uqmi_add_error("Failed to read from file");
		goto out;
	}

	if (value == 'Y')
		expected = QMI_CTL_DATA_LINK_PROTOCOL_RAW_IP;
	else if (value == 'N')
		expected = QMI_CTL_DATA_LINK_PROTOCOL_802_3;
	else
		uqmi_add_error("Unexpected sysfs file contents");

out:
	if (f)
		fclose(f);
	return expected;
}

static enum qmi_cmd_result
cmd_ctl_get_set_expected_data_format(struct qmi_dev *qmi,
				     QmiCtlDataLinkProtocol requested)
{
	char *sysfs_path = NULL;
	QmiCtlDataLinkProtocol expected = QMI_CTL_DATA_LINK_PROTOCOL_UNKNOWN;
	bool read_only;
	enum qmi_cmd_result ret = QMI_CMD_EXIT;

	read_only = (requested == QMI_CTL_DATA_LINK_PROTOCOL_UNKNOWN);

	reload_wwan_iface_name(qmi);
	if (!qmi->wwan_iface) {
		uqmi_add_error("Unknown wwan iface");
		goto out;
	}

	asprintf(&sysfs_path, "/sys/class/net/%s/qmi/raw_ip", qmi->wwan_iface);

	if (!read_only && set_expected_data_format(sysfs_path, requested))
		goto out;

	if ((expected = get_expected_data_format(sysfs_path)) ==
	    QMI_CTL_DATA_LINK_PROTOCOL_UNKNOWN)
		goto out;

	if (!read_only && (requested != expected)) {
		uqmi_add_error("Expected data format not updated properly");
		expected = QMI_CTL_DATA_LINK_PROTOCOL_UNKNOWN;
		goto out;
	}

	ret = QMI_CMD_DONE;
out:
	free(sysfs_path);
	return ret;
}

#define cmd_ctl_set_expected_data_format_cb no_cb
static enum qmi_cmd_result
cmd_ctl_set_expected_data_format_prepare(struct qmi_dev *qmi, struct qmi_request *req, struct qmi_msg *msg, char *arg)
{
	const char *modes[] = {
		[QMI_CTL_DATA_LINK_PROTOCOL_802_3] = "802.3",
		[QMI_CTL_DATA_LINK_PROTOCOL_RAW_IP] = "raw-ip",
	};
	int mode = qmi_get_array_idx(modes, ARRAY_SIZE(modes), arg);

	if (mode < 0) {
		uqmi_add_error("Invalid mode (modes: 802.3, raw-ip)");
		return QMI_CMD_EXIT;
	}

	cmd_ctl_get_set_expected_data_format(qmi, mode);
	return QMI_CMD_DONE;
}

#include "commands-wds.c"
#include "commands-dms.c"
#include "commands-nas.c"
#include "commands-wms.c"
#include "commands-wda.c"
#include "commands-uim.c"

#define __uqmi_command(_name, _optname, _arg, _type) \
	[__UQMI_COMMAND_##_name] = { \
		.name = #_optname, \
		.type = _type, \
		.prepare = cmd_##_name##_prepare, \
		.cb = cmd_##_name##_cb, \
	}

const struct uqmi_cmd_handler uqmi_cmd_handler[__UQMI_COMMAND_LAST] = {
	__uqmi_commands
};
#undef __uqmi_command

static struct uqmi_cmd *cmds;
static int n_cmds;

void uqmi_add_command(char *arg, int cmd)
{
	int idx = n_cmds++;

	cmds = realloc(cmds, n_cmds * sizeof(*cmds));
	cmds[idx].handler = &uqmi_cmd_handler[cmd];
	cmds[idx].arg = arg;
}

static void uqmi_print_result(struct blob_attr *data)
{
	char *str;

	if (!blob_len(data))
		return;

	str = blobmsg_format_json_indent(blob_data(data), false, single_line ? -1 : 0);
	if (!str)
		return;

	printf("%s\n", str);
	free(str);
}

static bool __uqmi_run_commands(struct qmi_dev *qmi, bool option)
{
	static struct qmi_request req;
	char *buf = qmi->buf;
	int i;

	for (i = 0; i < n_cmds; i++) {
		enum qmi_cmd_result res;
		bool cmd_option = cmds[i].handler->type == CMD_TYPE_OPTION;
		bool do_break = false;

		if (cmd_option != option)
			continue;

		blob_buf_init(&status, 0);
		if (cmds[i].handler->type > QMI_SERVICE_CTL &&
		    qmi_service_connect(qmi, cmds[i].handler->type, -1)) {
			uqmi_add_error("Failed to connect to service");
			res = QMI_CMD_EXIT;
		} else {
			res = cmds[i].handler->prepare(qmi, &req, (void *) buf, cmds[i].arg);
		}

		if (res == QMI_CMD_REQUEST) {
			qmi_request_start(qmi, &req, cmds[i].handler->cb);
			req.no_error_cb = true;
			if (qmi_request_wait(qmi, &req)) {
				uqmi_add_error(qmi_get_error_str(req.ret));
				do_break = true;
			}
		} else if (res == QMI_CMD_EXIT) {
			do_break = true;
		}

		uqmi_print_result(status.head);
		if (do_break)
			return false;
	}
	return true;
}

int uqmi_add_error(const char *msg)
{
	blobmsg_add_string(&status, NULL, msg);
	return QMI_CMD_EXIT;
}

bool uqmi_run_commands(struct qmi_dev *qmi)
{
	bool ret;

	ret = __uqmi_run_commands(qmi, true) &&
	      __uqmi_run_commands(qmi, false);

	free(cmds);
	cmds = NULL;
	n_cmds = 0;

	return ret;
}
