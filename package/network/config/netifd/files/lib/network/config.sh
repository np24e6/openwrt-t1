#!/bin/sh
# Copyright (C) 2011 OpenWrt.org

. /usr/share/libubox/jshn.sh

find_config() {
	local device="$1"
	local ifdev ifl3dev ifobj
	for ifobj in $(ubus list network.interface.\*); do
		interface="${ifobj##network.interface.}"
		(
			json_load "$(ifstatus $interface)"
			json_get_var ifdev device
			json_get_var ifl3dev l3_device
			if [ "$device" = "$ifdev" ] || [ "$device" = "$ifl3dev" ]; then
				echo "$interface"
				exit 0
			else
				exit 1
			fi
		) && return
	done
}

unbridge() {
	return
}

ubus_call() {
	json_init
	local _data="$(ubus -S call "$1" "$2")"
	[ -z "$_data" ] && return 1
	json_load "$_data"
	return 0
}


fixup_interface() {
	local config="$1"
	local ifname type device l3dev

	config_get type "$config" type
	config_get ifname "$config" ifname
	[ "bridge" = "$type" ] && ifname="br-$config"
	ubus_call "network.interface.$config" status || return 0
	json_get_var l3dev l3_device
	[ -n "$l3dev" ] && ifname="$l3dev"
	json_init
	config_set "$config" ifname "$ifname"
}

scan_interfaces() {
	config_load network
	config_foreach fixup_interface interface
}

prepare_interface_bridge() {
	local config="$1"

	[ -n "$config" ] || return 0
	ubus call network.interface."$config" prepare
}

setup_interface() {
	local iface="$1"
	local config="$2"

	[ -n "$config" ] || return 0
	ubus call network.interface."$config" add_device "{ \"name\": \"$iface\" }"
}

do_sysctl() {
	[ -n "$2" ] && \
		sysctl -n -e -w "$1=$2" >/dev/null || \
		sysctl -n -e "$1"
}

add_wan_ifname() {

	local given_role="$1"
	local num="$2"

	json_select ..

	if ! json_is_a roles array; then
		json_select ports
		return 1
	fi

	json_get_keys roles roles
	json_select roles

	for role in $roles; do

		json_select "$role"
			json_get_vars role
			json_get_vars device
		json_select ..

		[ "$given_role" != "$role" ] || [ -z "$device" ] && continue

		uci set network._$role$num.ifname=$device

	done

	json_select ..
	json_select ports

	return 0
}

check_regular_switch() {

	local platform="$1"

	json_select switch0

	if ! json_is_a ports array; then
		json_select ..
		json_select ..
		return 1
	fi

	json_get_keys ports ports
	json_select ports

	for port in $ports; do

		json_select "$port"
			json_get_vars role
			json_get_vars num
		json_select ..

		[ "$role" != "lan" ] && [ "$role" != "wan" ] && continue

		uci -q batch <<-EOF
			set network._$role$num="port"
			set network._$role$num.enabled="1"
			set network._$role$num.autoneg="on"
			set network._$role$num.role=$role
			set network._$role$num.port_num="$num"
		EOF

		[ "$platform" = "RUTX" ] && [ "$role" = "wan" ] && add_wan_ifname "$role" "$num"

	done

	json_select ..
	json_select ..
	json_select ..

	return 0
}

check_dsa_switch() {

	json_select network

		json_select lan
			json_get_values ports ports
		json_select ..

		json_select wan
			json_get_vars device
		json_select ..

	json_select ..

	[ -n "$device" ] && {
		uci -q batch <<-EOF
			set network._$device="port"
			set network._$device.enabled="1"
			set network._$device.autoneg="on"
			set network._$device.ifname="$device"
			set network._"$device"_mtu="device"
			set network._"$device"_mtu.name="$device"
		EOF
	}

	[ -n "$ports" ] || return 1

	for port in $ports; do
		uci -q batch <<-EOF
			set network._$port="port"
			set network._$port.enabled="1"
			set network._$port.autoneg="on"
			set network._$port.ifname="$port"
			set network._"$port"_mtu="device"
			set network._"$port"_mtu.name="$port"
		EOF
	done

	return 0
}

check_rest_ports() {

	local i=0

	json_select network

		json_select lan
			json_get_values ports ports
		json_select ..

	json_select ..

	[ -n "$ports" ] || return 1

	for port in $ports; do

		[[ "$port" != "eth"* ]] && continue

		i=$((i + 1))

		uci -q batch <<-EOF
			set network._lan$i="port"
			set network._lan$i.enabled="1"
			set network._lan$i.autoneg="on"
			set network._lan$i.role="lan"
			set network._lan$i.ifname="$port"
		EOF

	done

	return 0
}

generate_ports_template() {        
	# Load board.json before calling this func

	json_select hwinfo
		json_get_vars port_link
		json_get_vars dsa
	json_select ..

	json_select model
		json_get_vars platform
	json_select ..

	[ "$port_link" = "0" ] && return 0

	[ "$dsa" = "1" ] && {
		check_dsa_switch
		return 0
	}

	if json_select switch; then
		check_regular_switch "$platform"
		return 0
	fi

	check_rest_ports

	return 0
}
