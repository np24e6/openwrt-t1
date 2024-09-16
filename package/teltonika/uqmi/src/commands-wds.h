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

#define __uqmi_wds_commands \
	__uqmi_command(wds_start_network, start-network, no, QMI_SERVICE_WDS), \
	__uqmi_command(wds_set_apn, apn, required, CMD_TYPE_OPTION), \
	__uqmi_command(wds_set_auth, auth-type, required, CMD_TYPE_OPTION), \
	__uqmi_command(wds_set_username, username, required, CMD_TYPE_OPTION), \
	__uqmi_command(wds_set_password, password, required, CMD_TYPE_OPTION), \
	__uqmi_command(wds_set_ip_family_pref, ip-family, required, CMD_TYPE_OPTION), \
	__uqmi_command(wds_set_autoconnect, autoconnect, no, CMD_TYPE_OPTION), \
	__uqmi_command(wds_set_profile, profile, required, CMD_TYPE_OPTION), \
	__uqmi_command(wds_modify_profile, modify-profile, no, QMI_SERVICE_WDS), \
	__uqmi_command(wds_set_profile_identifier, profile-identifier, required, CMD_TYPE_OPTION), \
	__uqmi_command(wds_set_roaming_disallowed_flag, roaming-disallowed-flag, required, CMD_TYPE_OPTION), \
	__uqmi_command(wds_set_profile_name, profile-name, required, CMD_TYPE_OPTION), \
	__uqmi_command(wds_set_pdp_type, pdp-type, required, CMD_TYPE_OPTION), \
	__uqmi_command(wds_stop_network, stop-network, required, QMI_SERVICE_WDS), \
	__uqmi_command(wds_get_packet_service_status, get-data-status, no, QMI_SERVICE_WDS), \
	__uqmi_command(wds_set_ip_family, set-ip-family, required, QMI_SERVICE_WDS), \
	__uqmi_command(wds_set_autoconnect_settings, set-autoconnect, required, QMI_SERVICE_WDS), \
	__uqmi_command(wds_reset, reset-wds, no, QMI_SERVICE_WDS), \
	__uqmi_command(wds_get_current_settings, get-current-settings, no, QMI_SERVICE_WDS), \
	__uqmi_command(wds_get_dormancy_status, get-dormancy-status, no, QMI_SERVICE_WDS), \
	__uqmi_command(wds_bind_mux_data_port, wds-bind-mux-data-port, no, QMI_SERVICE_WDS), \
	__uqmi_command(wds_mux_id, mux-id, required, CMD_TYPE_OPTION), \
	__uqmi_command(wds_ep_iface_number, ep-iface-number, required, CMD_TYPE_OPTION) \

#define wds_helptext \
		"  --start-network:                  Start network connection (use with options below)\n" \
		"    --apn <apn>:                    Use APN\n" \
		"    --auth-type pap|chap|both|none: Use network authentication type\n" \
		"    --username <name>:              Use network username\n" \
		"    --password <password>:          Use network password\n" \
		"    --ip-family <family>:           Use ip-family for the connection (ipv4, ipv6, unspecified)\n" \
		"    --autoconnect:                  Enable automatic connect/reconnect\n" \
		"    --profile <index>:              Use connection profile\n" \
		"  --modify-profile:                 Modify pdp configuration (with options below)\n" \
		"    --profile-identifier <type>,<pdp>: Use profile identifier (3gpp,3gpp2|number)\n" \
		"    --roaming-disallowed-flag <bool> : Use roaming disallowed flag\n" \
		"    --profile-name <name>:	     Use profile name\n" \
		"    --pdp-type <type>:		     Use pdp profile type (ipv4, ipv6, ppp, ipv4v6)\n" \
		"  --stop-network <pdh>:             Stop network connection (use with option below)\n" \
		"    --autoconnect:                  Disable automatic connect/reconnect\n" \
		"  --get-data-status:                Get current data access status\n" \
		"  --set-ip-family <val>:            Set ip-family (ipv4, ipv6, unspecified)\n" \
		"  --set-autoconnect <val>:          Set automatic connect/reconnect (disabled, enabled, paused)\n" \
		"  --get-current-settings:           Get current connection settings\n" \
		"  --get-dormancy-status:	     Get current dormancy status\n" \
		"  --wds-bind-mux-data-port:	     Bind qmux data port to controller device (use with options below)\n" \
		"    --mux-id <id>:		     Set qmux port id\n" \
		"    --ep-iface-number <number>:     Set endpoint interface number\n" \

void cmd_wds_start_network_cb(struct qmi_dev *qmi, struct qmi_request *req, struct qmi_msg *msg);

