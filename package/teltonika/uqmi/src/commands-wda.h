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

#define __uqmi_wda_commands \
	__uqmi_command(wda_set_data_format, wda-set-data-format, no, QMI_SERVICE_WDA), \
	__uqmi_command(wda_link_layer_protocol, link-layer-protocol, required, CMD_TYPE_OPTION), \
	__uqmi_command(wda_uplink_data_aggregation_protocol, ul-protocol, required, CMD_TYPE_OPTION), \
	__uqmi_command(wda_downlink_data_aggregation_protocol, dl-protocol, required, CMD_TYPE_OPTION), \
	__uqmi_command(wda_endpoint_type, endpoint-type, required, CMD_TYPE_OPTION), \
	__uqmi_command(wda_endpoint_interface_number, endpoint-iface-number, required, CMD_TYPE_OPTION), \
	__uqmi_command(wda_downlink_data_aggregation_max_datagrams, dl-max-datagrams, required, CMD_TYPE_OPTION), \
	__uqmi_command(wda_uplink_data_aggregation_max_datagrams, ul-max-datagrams, required, CMD_TYPE_OPTION), \
	__uqmi_command(wda_downlink_data_aggregation_max_size, dl-datagram-max-size, required, CMD_TYPE_OPTION), \
	__uqmi_command(wda_uplink_data_aggregation_max_size, ul-datagram-max-size, required, CMD_TYPE_OPTION), \
	__uqmi_command(wda_downlink_minimum_padding, dl-min-padding, required, CMD_TYPE_OPTION), \
	__uqmi_command(wda_qos_format, qos-format, required, CMD_TYPE_OPTION), \
	__uqmi_command(wda_get_data_format, wda-get-data-format, no, QMI_SERVICE_WDA)

#define wda_helptext \
		"  --wda-set-data-format:     	     Set data format (Use with options below)\n" \
		"    --qos-size <size>:   	     Set qos format (number)\n" \
		"    --endpoint-type <type>:	     Set EP endpoint type (type: hsusb|pcie)\n" \
		"    --enpoint-iface-number <number> Set EP endpoint iface number (number)\n" \
		"    --link-layer-protocol <type>:   Set data format (type: 802.3|raw-ip)\n" \
		"    --ul-protocol <proto>:	     Set upload protocol (proto: tlp|qc-cm|mbim|rndis|qmap|qmapv5)\n" \
		"    --dl-protocol <proto>:	     Set downlink protocol (proto: tlp|qc-cm|mbim|rndis|qmap|qmapv5)\n" \
		"    --dl-max-datagrams <size>:      Set downlink max datagrams (number)\n" \
		"    --ul-max-datagrams <size>:      Set uplink max datagrams (number)\n" \
		"    --dl-datagram-max-size <size>:  Set downlink datagram max size (number)\n" \
		"    --ul-datagram-max-size <size>:  Set uplink datagram max size (number)\n" \
		"    --dl-min-padding <size>:  	     Set downlink minimum padding (number)\n" \
		"  --wda-get-data-format:            Get data format\n" \

