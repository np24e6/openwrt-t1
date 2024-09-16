#!/bin/sh

[ "$KNAME" != "" ] || return
[ "$DHCPIF" != "" ] || return

# Do not route if configured for single interface
[ -n "$MOREIF" ] || return

interfaces=$DHCPIF
[ -n "$MOREIF" ] && {
	for iface in $MOREIF; do
		interfaces="$interfaces $iface"
	done
}

# Make sure IP is only assigned to single interface
for iface in $interfaces; do
	ip rule del from "$FRAMED_IP_ADDRESS" table "$iface" >/dev/null 2>&1
	ip rule del to "$FRAMED_IP_ADDRESS" table "$iface" >/dev/null 2>&1
done

ip rule add from "$FRAMED_IP_ADDRESS" table "$IF" >/dev/null 2>&1
ip rule add to "$FRAMED_IP_ADDRESS" table "$IF" >/dev/null 2>&1

ip route add table "$IF" $NET/$MASK dev "$IF" >/dev/null 2>&1