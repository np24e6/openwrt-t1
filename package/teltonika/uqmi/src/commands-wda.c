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

#include <stdlib.h>

#include "qmi-message.h"

static const struct {
	const char *name;
	QmiWdaLinkLayerProtocol val;
} link_modes[] = {
	{ "802.3", QMI_WDA_LINK_LAYER_PROTOCOL_802_3 },
	{ "raw-ip", QMI_WDA_LINK_LAYER_PROTOCOL_RAW_IP },
};

static const struct {
	const char *name;
	QmiWdaDataAggregationProtocol aggreg;
} aggreg_modes[] = {
	{ "disabled", QMI_WDA_DATA_AGGREGATION_PROTOCOL_DISABLED },
	{ "tlp", QMI_WDA_DATA_AGGREGATION_PROTOCOL_TLP },
	{ "qc-cm", QMI_WDA_DATA_AGGREGATION_PROTOCOL_QC_NCM },
	{ "mbim", QMI_WDA_DATA_AGGREGATION_PROTOCOL_MBIM },
	{ "rndis", QMI_WDA_DATA_AGGREGATION_PROTOCOL_RNDIS },
	{ "qmap", QMI_WDA_DATA_AGGREGATION_PROTOCOL_QMAP },
	{ "qmapv5", QMI_WDA_DATA_AGGREGATION_PROTOCOL_QMAPV5 },
};

static struct {
	uint32_t iface_number;
	uint32_t dl_max_size;
	uint32_t dl_max_datagrams;
	uint32_t ul_max_size;
	uint32_t ul_max_datagrams;
	uint32_t dl_min_padding;
	uint8_t qos_format;
	QmiDataEndpointType endpoint_type;
	QmiWdaDataAggregationProtocol ul_aggreg;
	QmiWdaDataAggregationProtocol dl_aggreg;
	QmiWdaLinkLayerProtocol val;
} wda_endpoint_info;

#define cmd_wda_set_data_format_cb no_cb

static enum qmi_cmd_result
cmd_wda_set_data_format_prepare(struct qmi_dev *qmi, struct qmi_request *req,
				struct qmi_msg *msg, char *arg)
{
	struct qmi_wda_set_data_format_request wda_sdf_req = {
		QMI_INIT(link_layer_protocol, wda_endpoint_info.val),
		QMI_INIT(uplink_data_aggregation_protocol, wda_endpoint_info.ul_aggreg),
		QMI_INIT(downlink_data_aggregation_protocol, wda_endpoint_info.dl_aggreg),
		QMI_INIT(downlink_data_aggregation_max_datagrams, wda_endpoint_info.dl_max_datagrams),
		QMI_INIT(downlink_data_aggregation_max_size, wda_endpoint_info.dl_max_size),
		QMI_INIT(uplink_data_aggregation_max_datagrams, wda_endpoint_info.ul_max_datagrams),
		QMI_INIT(uplink_data_aggregation_max_size, wda_endpoint_info.ul_max_size),
		QMI_INIT(downlink_minimum_padding, wda_endpoint_info.dl_min_padding),
		QMI_INIT(qos_format, wda_endpoint_info.qos_format),
	};

	if (wda_endpoint_info.endpoint_type) {
		wda_sdf_req.data.endpoint_info.endpoint_type = wda_endpoint_info.endpoint_type;
	}

	if (wda_endpoint_info.iface_number) {
		wda_sdf_req.data.endpoint_info.interface_number = wda_endpoint_info.iface_number;
	}

	qmi_set_wda_set_data_format_request(msg, &wda_sdf_req);
	return QMI_CMD_REQUEST;
}

#define cmd_wda_link_layer_protocol_cb no_cb

static enum qmi_cmd_result cmd_wda_link_layer_protocol_prepare(
	struct qmi_dev * qmi, struct qmi_request * req, struct qmi_msg * msg,
	char *arg)
{
	for (uint32_t i = 0; i < ARRAY_SIZE(link_modes); i++) {
		if (strcasecmp(link_modes[i].name, arg))
		    continue;

		wda_endpoint_info.val = link_modes[i].val;
		return QMI_CMD_DONE;
	}

	uqmi_add_error("Invalid value (valid: 802.3, raw-ip");
	return QMI_CMD_EXIT;
}

#define cmd_wda_uplink_data_aggregation_protocol_cb no_cb

static enum qmi_cmd_result
cmd_wda_uplink_data_aggregation_protocol_prepare(struct qmi_dev *qmi,
						 struct qmi_request *req,
						 struct qmi_msg *msg, char *arg)
{
	for (uint32_t i = 0; i < ARRAY_SIZE(aggreg_modes); i++) {
		if (strcasecmp(aggreg_modes[i].name, arg))
			continue;

		wda_endpoint_info.ul_aggreg = aggreg_modes[i].aggreg;
		return QMI_CMD_DONE;
	}

	uqmi_add_error("Invalid value (valid: disabled, tlp, qc-cm, mbim, rndis, qmap, qmapv5");
	return QMI_CMD_EXIT;
}

#define cmd_wda_downlink_data_aggregation_protocol_cb no_cb

static enum qmi_cmd_result cmd_wda_downlink_data_aggregation_protocol_prepare(
	struct qmi_dev *qmi, struct qmi_request *req, struct qmi_msg *msg,
	char *arg)
{
	for (uint32_t i = 0; i < ARRAY_SIZE(aggreg_modes); i++) {
		if (strcasecmp(aggreg_modes[i].name, arg))
			continue;

		wda_endpoint_info.dl_aggreg = aggreg_modes[i].aggreg;
		return QMI_CMD_DONE;
	}

	uqmi_add_error("Invalid value (valid: disabled, tlp, qc-cm, mbim, rndis, qmap, qmapv5");
	return QMI_CMD_EXIT;
}

#define cmd_wda_downlink_data_aggregation_max_datagrams_cb no_cb

static enum qmi_cmd_result cmd_wda_downlink_data_aggregation_max_datagrams_prepare(
	struct qmi_dev * qmi, struct qmi_request * req, struct qmi_msg * msg,
	char *arg)
{
	uint32_t max_datagrams = strtoul(arg, NULL, 10);

	wda_endpoint_info.dl_max_datagrams = max_datagrams;
	return QMI_CMD_DONE;
}

#define cmd_wda_uplink_data_aggregation_max_datagrams_cb no_cb

static enum qmi_cmd_result cmd_wda_uplink_data_aggregation_max_datagrams_prepare(
	struct qmi_dev * qmi, struct qmi_request * req, struct qmi_msg * msg,
	char *arg)
{
	uint32_t max_datagrams = strtoul(arg, NULL, 10);

	wda_endpoint_info.ul_max_datagrams = max_datagrams;
	return QMI_CMD_DONE;
}

#define cmd_wda_downlink_data_aggregation_max_size_cb no_cb

static enum qmi_cmd_result cmd_wda_downlink_data_aggregation_max_size_prepare(
	struct qmi_dev *qmi, struct qmi_request *req, struct qmi_msg *msg,
	char *arg)
{
	uint32_t max_size = strtoul(arg, NULL, 10);

	wda_endpoint_info.dl_max_size = max_size;
	return QMI_CMD_DONE;
}

#define cmd_wda_uplink_data_aggregation_max_size_cb no_cb

static enum qmi_cmd_result cmd_wda_uplink_data_aggregation_max_size_prepare(
	struct qmi_dev *qmi, struct qmi_request *req, struct qmi_msg *msg,
	char *arg)
{
	uint32_t max_size = strtoul(arg, NULL, 10);

	wda_endpoint_info.ul_max_size = max_size;
	return QMI_CMD_DONE;
}

#define cmd_wda_downlink_minimum_padding_cb no_cb

static enum qmi_cmd_result cmd_wda_downlink_minimum_padding_prepare(
	struct qmi_dev *qmi, struct qmi_request *req, struct qmi_msg *msg,
	char *arg)
{
	uint32_t min_pad = strtoul(arg, NULL, 10);

	wda_endpoint_info.dl_min_padding = min_pad;
	return QMI_CMD_DONE;
}

#define cmd_wda_qos_format_cb no_cb

static enum qmi_cmd_result cmd_wda_qos_format_prepare(
	struct qmi_dev *qmi, struct qmi_request *req, struct qmi_msg *msg,
	char *arg)
{
	uint8_t format = strtoul(arg, NULL, 10);

	wda_endpoint_info.qos_format = format;
	return QMI_CMD_DONE;
}


#define cmd_wda_endpoint_type_cb no_cb

static enum qmi_cmd_result
cmd_wda_endpoint_type_prepare(struct qmi_dev *qmi, struct qmi_request *req,
			      struct qmi_msg *msg, char *arg)
{
	static const char *endpoint_type[] = {
		[QMI_DATA_ENDPOINT_TYPE_HSUSB] = "hsusb",
		[QMI_DATA_ENDPOINT_TYPE_UNDEFINED] = "undefined",
	};

	for (uint32_t i = 0; i < ARRAY_SIZE(endpoint_type); i++) {
		if (endpoint_type[i] && !strcmp(endpoint_type[i], arg)) {
			wda_endpoint_info.endpoint_type = i;
			return QMI_CMD_DONE;
		}
	}

	uqmi_add_error("Invalid value (valid: undefined, hsusb");
	return QMI_CMD_EXIT;
}

#define cmd_wda_endpoint_interface_number_cb no_cb

static enum qmi_cmd_result
cmd_wda_endpoint_interface_number_prepare(struct qmi_dev *qmi,
					  struct qmi_request *req,
					  struct qmi_msg *msg, char *arg)
{
	uint32_t iface_num = strtoul(arg, NULL, 10);

	wda_endpoint_info.iface_number = iface_num;
	return QMI_CMD_DONE;
}

static void cmd_wda_get_data_format_cb(struct qmi_dev *qmi,
				       struct qmi_request *req,
				       struct qmi_msg *msg)
{
	void *t;
	struct qmi_wda_get_data_format_response res;
	const char *name = "unknown";
	int i;

	qmi_parse_wda_get_data_format_response(msg, &res);
	for (i = 0; i < ARRAY_SIZE(link_modes); i++) {
		if (link_modes[i].val != res.data.link_layer_protocol)
			continue;

		name = link_modes[i].name;
		break;
	}

	t = blobmsg_open_table(&status, NULL);

	blobmsg_add_u32(&status, "dl data aggregation max size", res.data.downlink_data_aggregation_max_size);

	blobmsg_add_string(&status, "Link Layer Protocol", name);

	blobmsg_close_table(&status, t);
}

static enum qmi_cmd_result
cmd_wda_get_data_format_prepare(struct qmi_dev *qmi, struct qmi_request *req, struct qmi_msg *msg, char *arg)
{
	struct qmi_wda_get_data_format_request data_req = {};
	qmi_set_wda_get_data_format_request(msg, &data_req);
	return QMI_CMD_REQUEST;
}
