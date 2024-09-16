#!/bin/sh
. /lib/functions/mobile.sh

#~ Script for mobile connection tracking
#~ Exit when connection lost
device="$1"
modem="$2"
drop_timer=0
drop_timer_limit=45 #15 minutes
shift
shift

gsm_modem=$(find_mdm_ubus_obj "$modem")
[ -z "$gsm_modem" ] && echo "gsm.modem \"$gsm_modem\" object not found. Downing connection." && ifdown $ifname

gsm_info=$(ubus call $gsm_modem info)
red_cap=$(jsonfilter -q -s "$gsm_info" -e '@.red_cap')

check_state() {
	local serv_status="$1"
	case "$serv_status" in
		*'"registration":"registered"'*'"PS":"attached"'*)
			conn_state=1
			;;
		*)
			conn_state=0
			if [ "$drop_timer" = "0" ]; then
				logger -t "qmux_track" "Connection lost."
			fi
			;;
	esac
}

handle_uqmi_ps_state() {
	serv_status=$(uqmi -s -d $device -t 3000 --set-client-id wds,"$cid" --get-serving-system)
	check_state "$serv_status"
	# If any of them are zero consider connection temporarily lost
	if [ "$conn_state" != 1 ]; then
		drop_timer=$((drop_timer + 1));
	else
		drop_timer=0
	fi
}

handle_at_ps_state() {
	gsm_call=$(ubus call $gsm_modem get_ps_att_state)
	reg_status=$(jsonfilter -s "$gsm_call" -e '@.state')
	# If any of them are zero consider connection temporarily lost
	if [ "$reg_status" != "attached" ]; then
		drop_timer=$((drop_timer + 1));
		logger -t "qmux_track" "Module deattached. Connection lost."
	else
		drop_timer=0
	fi
}

while true; do
	for cid in $@; do
		connstat=$(uqmi -s -d $device -t 3000 --set-client-id wds,"$cid" --get-data-status | awk -F '"' '{print $2}')
		if [ "$connstat" != "connected" ]; then
			echo "Mobile connection lost!"
			exit 1
		fi

		[ "$red_cap" = "true" ] && handle_at_ps_state || handle_uqmi_ps_state

		#if drop timer limit passes limit consider connection lost.
		if [ "$drop_timer" = "$drop_timer_limit" ]; then
			echo "Mobile connection lost!"
			exit 1
		fi
	done
	for iface in $IFACE4 $IFACE6; do
		ifstatus "$iface" 2>&1 >/dev/null || exit 1
	done

	sleep 20
done
