#!/bin/sh
. /lib/functions/mobile.sh

WHILE_INTERVAL=5
ifname="$1"
pdp="$2"
modem="$3"

init() {
	[ -f /var/run/ncm_conn.pid ] && exit 0
	echo "$$" > /var/run/ncm_conn.pid
}

check_connection() {
	local retry=7
	local count=0
	local parsed_pdp parsed_status mdm_ubus_obj ubus_call errno

	mdm_ubus_obj=$(find_mdm_ubus_obj "$modem")
	[ -z "$mdm_ubus_obj" ] && echo "gsm.modem object not found. Downing connection." && ifdown $ifname

	while true; do
		pdp_call=$(ubus call "$mdm_ubus_obj" get_pdp_call "{\"cid\":${pdp}}" > /dev/null 2>&1 )
		if [ "$pdp_call" = "" ]; then
			addr=$(ubus call "$mdm_ubus_obj" get_pdp_addr "{\"index\":${pdp}}" | grep addr)
			if [ "$addr" = "" ]; then
				count=$((count+1))
				[ "$count" -gt "$retry" ] && {
					echo "Connection was lost!"
					exit 1
				}
				sleep "$WHILE_INTERVAL"
				continue
			fi
			count=0
			sleep "$WHILE_INTERVAL"
			continue
		fi

		json_load $pdp_call
		json_get_var parsed_pdp cid
		json_get_var parsed_status state_id
		json_get_var errno errno

		if [ -z "$errno" ] && [ "$parsed_pdp" -eq "$pdp" ] && [ "$parsed_status" -eq 2 ]; then
			count=0
		else
			count=$((count+1))
			[ "$count" -gt "$retry" ] && {
				echo "Connection was lost!"
				json_load "$(ubus call "$mdm_ubus_obj" get_net_reg_stat)"
				json_get_var network_state status
				echo "Network state: $network_state. Trying to reconnect."
				exit 1
			}
		fi

		sleep "$WHILE_INTERVAL"
	done
}

echo "Starting connection tracker for ${ifname}!"
init
check_connection
