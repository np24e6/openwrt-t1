#!/bin/sh

[ "$KNAME" != "" ] || return

tc_outgoing() {
	local iface="$1"
	local speed="$2"
	local addr="$3"

	local class_id="1$(echo $addr | awk -F. '{print $4}')"

	tc qdisc show dev $iface root 2>/dev/null | grep -q '^qdisc htb 1:' || tc qdisc add dev $iface root handle 1: htb

	tc class add dev $iface parent 1: classid 1:$class_id htb rate $speed
	tc filter add dev $iface protocol ip parent 1: prio 1 u32 match ip dst $addr flowid 1:$class_id
}

tc_incoming() {
	local iface="$1"
	local speed="$2"
	local addr="$3"

	local virtual="ifb0"
	local class_id="1$(echo $addr | awk -F. '{print $4}')"

	ifconfig $virtual >/dev/null 2>&1 || ip link add name $virtual type ifb
	ip link set dev $virtual up

	tc qdisc show dev $virtual root 2>/dev/null | grep -q '^qdisc htb 2:' || \
		tc qdisc add dev $virtual root handle 2: htb

	tc qdisc show dev $iface ingress 2>/dev/null | grep -q '^qdisc ingress ffff:' || {
		tc qdisc add dev $iface handle ffff: ingress
		tc filter add dev $iface parent ffff: protocol ip u32 match u32 0 0 action mirred egress redirect dev $virtual
	}

	tc class add dev $virtual parent 2: classid 2:$class_id htb rate $speed
	tc filter add dev $virtual protocol ip parent 2: prio 1 u32 match ip src $addr flowid 2:$class_id
}

[ "$WISPR_BANDWIDTH_MAX_DOWN" -gt 0 ] && tc_outgoing "$IF" "$WISPR_BANDWIDTH_MAX_DOWN" "$FRAMED_IP_ADDRESS"
[ "$WISPR_BANDWIDTH_MAX_UP" -gt 0 ] && tc_incoming "$IF" "$WISPR_BANDWIDTH_MAX_UP" "$FRAMED_IP_ADDRESS"