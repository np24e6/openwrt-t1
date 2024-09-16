#!/bin/sh

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

proto_vrf_init_config() {

	proto_config_add_int 'table'
	proto_config_add_string 'ifname'
	proto_config_add_array 'link'

	available=1
	no_device=1
}

proto_vrf_setup() {

	local config="$1"
	local iface="$2"

	json_get_vars table ifname
	json_get_values values link

	ip link add dev "$ifname" type vrf table "$table"
	ip link set dev "$ifname" up

	for val in $values
	do
		ip link set "$val" vrf "$ifname"
	done

	ip rule | grep -q "from all lookup \[l3mdev-table\]"
	[ $? -eq 1 ]  && ip rule add l3mdev pref 1000

	proto_init_update "$1" 1
	proto_set_keep 1
	proto_send_update "$config"
}

proto_vrf_teardown() {

	json_get_vars ifname
	ip link del dev "$ifname"
}

[ -n "$INCLUDE_ONLY" ] || {
       add_protocol vrf
}