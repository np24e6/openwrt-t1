#!/bin/sh

. /lib/functions.sh
. /lib/config/uci.sh
. /usr/share/libubox/jshn.sh

SECTION=$1
FILE="/tmp/wget_check_file"
PINGCMD="/bin/ping"
PINGCMDV6="/bin/ping6"

log() {
	/usr/bin/logger -t ping_reboot.sh "$@"
}

get_router_name() {
	local name=""

	config_load system
	config_get name system "devicename"

	[ -z "$name" ] && name=$(mnf_info -n 2>/dev/null)

	echo "${name:0:3}"
}

get_config() {
	config_get ENABLE "$SECTION" "enable" 0
	config_get TIMEOUT "$SECTION" "time_out" 0
	config_get TIME "$SECTION" "time" 0
	config_get RETRIES "$SECTION" "retry" 0
	config_get HOST "$SECTION" "host" ""
	config_get PORT_HOST "$SECTION" "port_host" ""
	config_get ACTION "$SECTION" "action" 0
	config_get CURRENT_TRY "$SECTION" "current_try" 0
	config_get PACKET_SIZE "$SECTION" "packet_size" 0
	config_get MODEM "$SECTION" "modem" ""
	config_get TYPE "$SECTION" "type" ""
	config_get STOP_ACTION "$SECTION" "stop_action" 0
	# Determines if ping is done using any available interface (1) or any mobile interface (2)
	config_get IF_TYPE "$SECTION" "interface" 1
	config_get PHONE_LIST "$SECTION" "number" ""
	config_get MESSAGE "$SECTION" "message" ""
	config_get MODEM_ID_SMS "$SECTION" "modem_id_sms" ""
	config_get IP_TYPE "$SECTION" ip_type "ipv4"
	config_get PING_PORT_TYPE "$SECTION" ping_port_type ""
	config_get PORT "$SECTION" port ""
}

get_modem_num() {

	local modem="$1"
	local found_modem=""

	local modem_objs=$(ubus list gsm.*)

	for i in $modem_objs
	do
		found_modem=$(ubus call "$i" info 2> /dev/null | grep usb_id | sed 's/.* //g')
		[ "$modem" == "${found_modem:1:-2}" ] && echo "${i//[!0-9]/}" && return 0
	done

	return 1
}

set_uci_fail_counter() {
	# Check if non-negative integer
	if echo "$1" | grep -qE '^[0-9]+$'; then
		uci_set ping_reboot "$SECTION" current_try "$1"
		uci_commit ping_reboot
	fi
}

restart_modem() {
	mctl --reboot --number "$MODEM_NUM"
}

restart_mobile_interface() {
	ifdown "$1"
	sleep 1
	ifup "$1"
}

restart_poe() {
	/bin/ubus call poeman set "{\"port\":\"$1\",\"enable\":false}"
	sleep 3
	/bin/ubus call poeman set "{\"port\":\"$1\",\"enable\":true}"
}

restart_port() {
	ubus list poeman 2>&1 >/dev/null || return 0
	config_load poe
	if [ -n "$1" ]; then
		config_get POE_ENABLE "$1" "poe_enable" 0
		config_get NAME "$1" "name" ""
		if [ "$POE_ENABLE" = 1 ] && [ -n "$NAME" ]; then
			restart_poe "$NAME"
		fi
	elif [ -n "$PORT" ]; then
		config_get POE_ENABLE "$PORT" "poe_enable" 0
		config_get NAME "$PORT" "name" ""
		if [ "$POE_ENABLE" = 1 ] && [ -n "$NAME" ]; then
			restart_poe "$NAME"
		fi
	fi

}

get_l3_device() {
	local interface=$1
	local suffix=$2

	local status=$(ubus call network.interface status "{ \"interface\" : \"${interface}${suffix}\" }" 2>/dev/null)
	[ -z "$status" ] && return

	json_init
	json_load "$status"
	json_get_var up "up"

	[ "$up" -ne 1 ] && return

	ACTIVE_INTERFACE_SUFFIX="${interface}${suffix}"
	ACTIVE_INTERFACE="$interface"

	json_get_var l3_device "l3_device"
	IF_OPTION="-I ${l3_device}"
}

get_active_mobile_interface() {
	local section_name="$1"

	config_get modem "$section_name" "modem"

	[ -z "$modem" ] || [ "$modem" != "$MODEM" ] && return

	# Check IPv6, IPv4 and legacy interface names for an l3_device
	#FIXME: what if IPV4 type selected but IPV6 interface is available?
	[ -z "$IF_OPTION" ] && get_l3_device "$section_name" "_6"
	[ -z "$IF_OPTION" ] && get_l3_device "$section_name" "_4"
	[ -z "$IF_OPTION" ] && get_l3_device "$section_name"
}

get_modem() {
	local modem modems id builtin primary
	local primary_modem=""
	local builtin_modem=""
	json_init
	json_load_file "/etc/board.json"
	json_get_keys modems modems
	json_select modems

	for modem in $modems; do
		json_select "$modem"
		json_get_vars builtin primary id

		[ -z "$id" ] && {
			json_select ..
			continue
		}

		[ "$builtin" ] && builtin_modem=$id
		[ "$primary" ] && {
			primary_modem=$id
			break
		}

		json_select ..
	done

	[ -n "$primary_modem" ] && {
		MODEM=$primary_modem
		return
	}

	if [ -n "$builtin_modem" ]; then
		primary_modem=$builtin_modem
	else
		json_load "$(/bin/ubus call gsm.modem0 info)"
		json_get_vars usb_id
		primary_modem="$usb_id"
	fi

	MODEM=$primary_modem
}

send_sms() {
	for phone in $PHONE_LIST; do
		local res=$(ubus call gsm.modem"$MODEM_NUM" send_sms "{\"number\":\"${phone}\", \"text\": \"${MESSAGE}\"}")
		res=$(echo "$res" | grep -o OK)

		if [ "$res" != "OK" ]; then
			set_uci_fail_counter "$CURRENT_TRY"
		fi
	done
}

exec_action() {
	case "$ACTION" in
	"1")
		log "Rebooting router after ${CURRENT_TRY} unsuccessful tries"
		reboot "$1"
		;;
	"2")
		log "Restarting modem after ${CURRENT_TRY} unsuccessful tries"
		ubus call log write_ext "{
			\"event\": \"Restarting modem after ${CURRENT_TRY} unsuccessful tries\",
			\"sender\": \"Ping Reboot\",
			\"table\": 0,
			\"write_db\": 1,
		}"
		restart_modem "$MODEM"
		;;
	"4")
		log "Reregistering after ${CURRENT_TRY} unsuccessful tries"
		ubus call log write_ext "{
			\"event\": \"Reregistering after ${CURRENT_TRY} unsuccessful tries\",
			\"sender\": \"Ping Reboot\",
			\"table\": 0,
			\"write_db\": 1,
		}"
		restart_modem "$MODEM"
		;;
	"5")
		log "Restarting mobile data connection after ${CURRENT_TRY} unsuccessful retries"
		ubus call log write_ext "{
			\"event\": \"Restarting mobile data connection after ${CURRENT_TRY} unsuccessful retries\",
			\"sender\": \"Ping Reboot\",
			\"table\": 0,
			\"write_db\": 1,
		}"

		#FIXME: what if we have multiple mobile interfaces (multi APN mode)?
		#which one should we restart?	
		restart_mobile_interface "$ACTIVE_INTERFACE_SUFFIX"
		;;
	"6")
		log "Sending message after ${CURRENT_TRY} unsuccessful retries"
		ubus call log write_ext "{
			\"event\": \"Sending message after ${CURRENT_TRY} unsuccessful retries\",
			\"sender\": \"Ping Reboot\",
			\"table\": 0,
			\"write_db\": 1,
		}"
		send_sms
		;;
	"7")
		log "Restarting port after ${CURRENT_TRY} unsuccessful retries"
		ubus call log write_ext "{
			\"event\": \"Restarting port after ${CURRENT_TRY} unsuccessful retries\",
			\"sender\": \"Ping Reboot\",
			\"table\": 0,
			\"write_db\": 1,
		}"
		restart_port
		;;
	"3" | *)
		log "${CURRENT_TRY} unsuccessful ${TYPE} tries"
		;;
	esac
}

is_over_limit() {
	local over_limit

	if [ "$STOP_ACTION" -eq 1 ]; then
		json_init
		json_load "$(ubus call quota_limit.${ACTIVE_INTERFACE} status)"
		json_get_var over_limit "event_sent"
	else
		over_limit=0
	fi

	[ "$over_limit" -eq 1 ]
}

check_tries() {
	if [ -n "$2" ]; then
		log "$2"
	fi

	if [ "$CURRENT_TRY" -ge "$RETRIES" ]; then
		if is_over_limit; then
			log "Action stopped. Data limit reached."
		else
			set_uci_fail_counter 0
			exec_action "$1"
		fi
	else
		log "$3"
	fi
}

get_hex_from_position() {
	position=$(($1 + 1))
	local hex_value=""
	if [ -z "$1" ] || [ "$1" -le 1 ]; then
		hex_value="0x01"
	else
		local decimal_value=$((2 ** (position - 1)))
		hex_value=$(printf "0x%02x" "$decimal_value")
	fi
	echo "$hex_value"
}

get_position_from_hex() {
	local value=$1
	local position=0

	# Remove the "0x" prefix and convert to decimal
	local decimal_value=$(echo "$value" | sed 's/0x//' | awk '{printf "%d", "0x" $0}')

	while [ $((decimal_value % 2)) -eq 0 ]; do
		decimal_value=$((decimal_value / 2))
		position=$((position + 1))
	done

	return "$position"
}

perform_double_ping() {
	local ping_cmd="$PINGCMD"
	local ipv4_ping=0
	local ipv6_ping=0

	for i in $(cat /proc/net/arp | grep -w "$1" | awk '{print $1}'); do
		if [ -n "$i" ] && [ "$i" != "IP" ]; then
			if $ping_cmd $IF_OPTION -W "$TIMEOUT" -s "$PACKET_SIZE" -q -c 1 "$i" >/dev/null 2>&1; then
				ipv4_ping=1
			fi
		fi
	done

	ping_cmd="$PINGCMDV6"
	for j in $(ip -6 neigh | grep -w "$1" | awk '{print $5}'); do
		if [ -n "$j" ]; then
			if $ping_cmd $IF_OPTION -W "$TIMEOUT" -s "$PACKET_SIZE" -q -c 1 "$j" >/dev/null 2>&1; then
				ipv6_ping=1
			fi
		fi
	done

	if [ "$ipv4_ping" = 1 ] || [ "$ipv6_ping" = 1 ] && [ "$2" -ge "$3" ]; then
		return 1
	else
		return 0
	fi
}

perform_ping() {
	local ping_cmd="$PINGCMD"

	[ "$IP_TYPE" = "ipv6" ] && ping_cmd="$PINGCMDV6"

	if $ping_cmd $IF_OPTION -W "$TIMEOUT" -s "$PACKET_SIZE" -q -c 1 "$HOST" >/dev/null 2>&1; then
		set_uci_fail_counter 0
		log "Ping successful."
	else
		check_tries "-p" "Host ${HOST} unreachable" "${TIME} min. until next ping retry"
	fi
}

is_ips_equal_to_number() {
	local contains=0
	local number=$2

	if [ "$3" = "TSW" ]; then
		for i in $(bridge fdb | grep "$1" | grep "self" | awk '{print $1}'); do
			local ip4=$(cat /proc/net/arp | grep -w "$i" | awk '{print $1}')
			local ip6=$(ip -6 neigh | grep -w "$i" | awk '{print $1}')

			[ -n "$ip4" ] || [ -n "$ip6" ] && contains=$((contains + 1))
		done
	else
		local port_num=$(echo "$1" | sed 's/port\([0-9]*\).*/\1/')
		local oldIFS="$IFS"
		while IFS= read -r line; do
			set -- $line
			i=$1
			j=$2
			[ -z "$i" ] || [ -z "$j" ] && break

			get_position_from_hex "$j"

			local position=$?

			if [ "$position" = "$port_num" ]; then
				local ip4=$(cat /proc/net/arp | grep -w "$i" | awk '{print $1}')
				local ip6=$(ip -6 neigh | grep -w "$i" | awk '{print $1}')

				[ -n "$ip4" ] || [ -n "$ip6" ] && contains=$((contains + 1))
			fi
		done <<EOF
		$(swconfig dev switch0 get dump_arl | awk '{print $2, $4}')
EOF
		IFS="$oldIFS"
	fi

	[ "$contains" -ge "$number" ]
}

multiple_ports() {
	local sec="$2"
	local port=$(echo "$1" | cut -d'=' -f1)
	local number=$(echo "$1" | cut -d'=' -f2)
	local decimal_value=$(echo "$port" | sed 's/[^0-9]*//g')
	local router_name=$(get_router_name)
	local passed_pings=0
	local devices_count

	if [ -z "$PORT" ]; then
		PORT=${port}
	fi

	is_ips_equal_to_number "$port" "$number" "$router_name"

	local equal=$?

	if [ "$equal" -eq 1 ] && [ "$ACTION" -eq "7" ]; then
		restart_port "$port"
	else
		if [ "$router_name" = "TSW" ]; then
			devices_count=$(bridge fdb | grep "$port" | grep "self" | awk '{print $1}' | wc -l)
		else
			local hex=$(get_hex_from_position "$decimal_value")
			devices_count=$(swconfig dev switch0 get dump_arl | grep -c "$hex")
		fi

		if [ "$router_name" = "TSW" ]; then
			for i in $(bridge fdb | grep "$port" | grep "self" | awk '{print $1}'); do
				perform_double_ping "$i" "$devices_count" "$number"
				local passed=$?
				passed_pings=$((passed_pings+passed))
			done
		else
			local oldIFS="$IFS"
			while IFS= read -r line; do
				set -- $line
				i=$1
				j=$2
				[ -z "$i" ] && [ -z "$j" ] && break

				get_position_from_hex "$j"
				local position=$?

				if [ "$position" = "$decimal_value" ]; then
					perform_double_ping "$i" "$devices_count" "$number"
					local passed=$?
					passed_pings=$((passed_pings+passed))
				fi
			done <<EOF
			$(swconfig dev switch0 get dump_arl | awk '{print $2, $4}')
EOF
			IFS="$oldIFS"
		fi
	fi

	if [ "$passed_pings" -ge "$number" ]; then
		set_uci_fail_counter 0
		log "Ping successful."
	elif [ "$equal" -eq 0 ] && [ "$ACTION" -eq "7" ]; then
		check_tries "-p" "" "${TIME} min. until next ping retry"
	fi
}

perform_port_ping() {
	if [ "$PING_PORT_TYPE" = "ping_ip" ]; then
		local mac=""
		if [ "$IP_TYPE" = "ipv6" ]; then
			mac=$(ip -6 neigh | grep -w "$HOST" | awk '{print $5}')
		else
			#Ping before getting MAC address. In other case, ARP table will could be empty in some situations.
			#Basicaly it is an arp-scan equivalent since we do not have such a command in a TSW devices.
			/bin/ping -W 2 -s 56 -q -c 1 "$HOST" >/dev/null 2>&1
			mac=$(cat /proc/net/arp | grep -w "$HOST" | awk '{print $4}')
		fi
		if [ -n "$mac" ] && [ "$mac" != "00:00:00:00:00:00" ]; then
			local router_name=$(get_router_name)
			local port=""
			if [ "$router_name" = "TSW" ]; then
				port=$(bridge fdb | grep -w "$mac" | grep "self" | awk '{print $3}')
			else
				local portmap=$(swconfig dev switch0 get dump_arl | grep -w "$mac" | awk '{print $4}')
				get_position_from_hex "$portmap"
				local position=$?
				port="port${position}"
			fi

			if [ -n "$port" ]; then
				uci_set ping_reboot "$SECTION" port "$port"
				uci_commit ping_reboot
				config_load ping_reboot
			fi
		else
			log "Was not successful in getting device MAC adress."
		fi

		perform_ping
	else
		config_get len "$SECTION" "port_host_LENGTH" 0
		if [ -n "$len" ]; then
			config_list_foreach "$SECTION" port_host multiple_ports "$SECTION"
		fi
	fi
}

perform_wget() {
	wget -q -T "$TIMEOUT" "$HOST" -O $FILE >/dev/null 2>&1

	if [ ! -s $FILE ]; then
		check_tries "-g" "Can't wget URL." "Will be retrying wget"
	else
		set_uci_fail_counter 0
		log "Wget URL successful."
	fi

	rm $FILE >/dev/null 2>&1
}
config_load ping_reboot
get_config

[ "$ENABLE" -ne 1 ] && return

CURRENT_TRY=$((CURRENT_TRY + 1))
set_uci_fail_counter $CURRENT_TRY

[ -z "$MODEM" ] && {
	get_modem
	uci_set ping_reboot "$SECTION" modem "$MODEM"
	uci_commit ping_reboot
}

MODEM_NUM=$(get_modem_num "$MODEM")

[ -z "$ACTIVE_INTERFACE" ] && {
	config_load network
	config_foreach get_active_mobile_interface "interface"

	config_load ping_reboot
}

if [ "$PING_PORT_TYPE" = "ping_port" ]; then
	IF_OPTION=""
elif [ "$IF_TYPE" = "2" ] || [ -z "$HOST" ]; then
	[ -z "$IF_OPTION" ] && {
		check_tries "-p" "No mobile data connection active" "${TIME} min. until next ping retry"
		exit
	}

	#FIXME: what if not default interface name?
	if echo "$ACTIVE_INTERFACE" | grep "2"; then
		config_get HOST "$SECTION" "host2"
		config_get IP_TYPE "$SECTION" ip_type2 "ipv4"
	else
		config_get HOST "$SECTION" "host1"
		config_get IP_TYPE "$SECTION" ip_type1 "ipv4"
	fi
else
	IF_OPTION=""
fi

case "$TYPE" in
"ping")
	perform_ping
	;;
"wget")
	perform_wget
	;;
"port")
	perform_port_ping
	;;
esac
