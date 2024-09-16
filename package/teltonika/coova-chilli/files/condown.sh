#!/bin/sh

[ "$KNAME" != "" ] || return

tc_outgoing() {
	local iface="$1"
	local addr="$2"

	local class_id="1$(echo $addr | awk -F. '{print $4}')"
	local ip_hex=$(printf '%02x' $(echo $addr | tr '.' ' '))

	local handle="$(tc filter show dev $iface 2>/dev/null | grep -B1 "match $ip_hex/ffffffff " | grep -o "fh [0-9a-f:]\+" | awk '{ print $2 }')"
	[ -z "$handle" ] || {
		for i in $(seq 1 5); do
			tc filter del dev $iface parent 1: handle $handle prio 1 protocol ip u32 >/dev/null 2>&1 && break
			sleep 2
		done
	}

	tc class show dev $iface 2>/dev/null | grep -q "^class htb 1:$class_id" && {
		for i in $(seq 1 5); do
			tc class del dev $iface classid 1:$class_id && break
			sleep 2
		done
	}
}

tc_incoming() {
	local iface="$1"
	local addr="$2"

	local virtual="ifb0"
	local class_id="1$(echo $addr | awk -F. '{print $4}')"
	local ip_hex=$(printf '%02x' $(echo $addr | tr '.' ' '))

	local handle="$(tc filter show dev $virtual 2>/dev/null | grep -B1 "match $ip_hex/ffffffff " | grep -o "fh [0-9a-f:]\+" | awk '{ print $2 }')"
	[ -z "$handle" ] || {
		for i in $(seq 1 5); do
			tc filter del dev $virtual parent 2: handle $handle prio 1 protocol ip u32 >/dev/null 2>&1 && break
			sleep 2
		done
	}

	tc class show dev $virtual 2>/dev/null | grep -q "^class htb 2:$class_id" && {
		for i in $(seq 1 5); do
			tc class del dev $virtual classid 2:$class_id && break
			sleep 2
		done
	}
}

interfaces=$DHCPIF
[ -n "$MOREIF" ] && {
	for iface in $MOREIF; do
		interfaces="$interfaces $iface"
	done
}

for iface in $interfaces; do
	tc_outgoing "$iface" "$FRAMED_IP_ADDRESS"
	tc_incoming "$iface" "$FRAMED_IP_ADDRESS"
done