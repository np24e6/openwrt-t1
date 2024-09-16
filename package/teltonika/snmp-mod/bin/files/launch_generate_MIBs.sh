#!/bin/sh

. /lib/functions.sh

get_router_name() {
	local name

	config_load system
	config_get name system "devicename"

	[ -z "$name" ] && name=$(mnf_info -n 2>/dev/null | cut -c -6)
	[ -z "$name" ] && {
		name="Teltonika"
		logger -st "$0" -p 4 "Failed to get device name, defaulting to '$name'"
	}

	echo "$name"
}

MIB_file="/etc/snmp/$(get_router_name).mib"

uci_set snmpd general mibfile "$MIB_file"
uci_commit snmpd

script="/etc/snmp/generate_MIBs.sh"

($script && rm $script) &
