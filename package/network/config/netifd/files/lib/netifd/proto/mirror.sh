#!/bin/sh

[ -L /sbin/tc ] || exit 0

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. /lib/functions/network.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

proto_mirror_init_config() {
	no_device=1
	available=1
	no_proto_task=1
	proto_config_add_string mirror_monitor_port
	proto_config_add_string mirror_source_port
	proto_config_add_boolean enable_mirror_rx
	proto_config_add_boolean enable_mirror_tx
}

proto_mirror_setup() {
	local config="$1"
	local iface="$2"

	local mirror_monitor_port mirror_source_port enable_mirror_rx enable_mirror_tx
	json_get_vars mirror_monitor_port mirror_source_port enable_mirror_rx enable_mirror_tx

	[ -z "$mirror_source_port" ] || [ -z "$mirror_monitor_port" ] && return
	[ "$enable_mirror_rx" != "1" ] && [ "$enable_mirror_tx" != "1" ] && return
	
	ip link set up dev "$mirror_monitor_port"
	tc qdisc add dev "$mirror_source_port" clsact
	[ "$enable_mirror_rx" == "1" ] && {
		tc filter add dev "$mirror_source_port" ingress matchall skip_sw action mirred egress mirror dev "$mirror_monitor_port"
	}
	[ "$enable_mirror_tx" == "1" ] && {
		tc filter add dev "$mirror_source_port" egress matchall skip_sw action mirred egress mirror dev "$mirror_monitor_port"
	}

	proto_add_data
		json_add_string "mirror_monitor_port" "$mirror_monitor_port"
		json_add_string "mirror_source_port" "$mirror_source_port"
		json_add_boolean "enable_mirror_rx" "$enable_mirror_rx"
		json_add_boolean "enable_mirror_tx" "$enable_mirror_tx"
	proto_close_data
}

proto_mirror_teardown() {
	local mirror_monitor_port mirror_source_port
	json_get_vars mirror_monitor_port mirror_source_port

	tc filter del dev "$mirror_source_port" ingress
	tc filter del dev "$mirror_source_port" egress
	tc qdisc del dev "$mirror_source_port" clsact
}

[ -n "$INCLUDE_ONLY" ] || {
	add_protocol mirror
}
