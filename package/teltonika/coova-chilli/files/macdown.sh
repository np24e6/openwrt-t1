#!/bin/sh

[ "$KNAME" != "" ] || return
[ "$DHCPIF" != "" ] || return

# Do not route if configured for single interface
[ -n "$MOREIF" ] || return

ip rule del from "$FRAMED_IP_ADDRESS" table "$IF" >/dev/null 2>&1
ip rule del to "$FRAMED_IP_ADDRESS" table "$IF" >/dev/null 2>&1