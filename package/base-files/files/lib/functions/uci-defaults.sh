#!/bin/ash

. /lib/functions.sh
. /usr/share/libubox/jshn.sh

json_select_array() {
	local _json_no_warning=1

	json_select "$1"
	[ $? = 0 ] && return

	json_add_array "$1"
	json_close_array

	json_select "$1"
}

json_select_object() {
	local _json_no_warning=1

	json_select "$1"
	[ $? = 0 ] && return

	json_add_object "$1"
	json_close_object

	json_select "$1"
}

ucidef_set_interface() {
	local network=$1; shift

	[ -z "$network" ] && return

	json_select_object network
	json_select_object "$network"

	while [ -n "$1" ]; do
		local opt=$1; shift
		local val=$1; shift

		[ -n "$opt" -a -n "$val" ] || break

		[ "$opt" = "device" -a "$val" != "${val/ //}" ] && {
			json_select_array "ports"
			for e in $val; do json_add_string "" "$e"; done
			json_close_array
		} || {
			json_add_string "$opt" "$val"
		}
	done

	if ! json_is_a proto string; then
		case "$network" in
			lan) json_add_string proto static ;;
			wan) json_add_string proto dhcp ;;
			*) json_add_string proto none ;;
		esac
	fi

	json_select ..
	json_select ..
}

ucidef_set_interface_default_macaddr() {
	local network="$1" ifname

	json_select_object 'network'
		json_select_object "$network"
		if json_is_a ports array; then
			json_select_array 'ports'
			json_get_keys port_id
			for i in $port_id; do
				json_get_var port "$i"
				ifname="${ifname} $port"
			done
			json_select ..
		else
			json_get_var ifname 'device'
		fi
		json_select ..
	json_select ..

	for i in $ifname; do
		local macaddr="$2"; shift
		[ -n "$macaddr" ] || break
		json_select_object 'network-device'
			json_select_object "$i"
				json_add_string 'macaddr' "$macaddr"
			json_select ..
		json_select ..
	done
}

ucidef_set_board_id() {
	json_select_object model
	json_add_string id "$1"
	json_select ..
}

ucidef_set_board_platform() {
	json_select_object model
	json_add_string platform "$1"
	json_select ..
}

ucidef_set_model_name() {
	json_select_object model
	json_add_string name "$1"
	json_select ..
}

ucidef_set_compat_version() {
	json_select_object system
	json_add_string compat_version "${1:-1.0}"
	json_select ..
}

ucidef_set_interface_lan() {
	ucidef_set_interface "lan" device "$1" proto "${2:-static}"
}

ucidef_set_interface_wan() {
	ucidef_set_interface "wan" device "$1" proto "${2:-dhcp}"
}

ucidef_set_interfaces_lan_wan() {
	local lan_if="$1"
	local wan_if="$2"

	ucidef_set_interface_lan "$lan_if"
	ucidef_set_interface_wan "$wan_if"
}

_ucidef_add_switch_port() {
	# inherited: $num $device $need_tag $want_untag $role $index $prev_role
	# inherited: $n_cpu $n_ports $n_vlan $cpu0 $cpu1 $cpu2 $cpu3 $cpu4 $cpu5

	n_ports=$((n_ports + 1))

	json_select_array ports
		json_add_object
			json_add_int num "$num"
			[ -n "$device"     ] && json_add_string  device     "$device"
			[ -n "$need_tag"   ] && json_add_boolean need_tag   "$need_tag"
			[ -n "$want_untag" ] && json_add_boolean want_untag "$want_untag"
			[ -n "$role"       ] && json_add_string  role       "$role"
			[ -n "$index"      ] && json_add_int     index      "$index"
			[ -n "$sfp"        ] && json_add_boolean sfp	    "$sfp"
		json_close_object
	json_select ..

	# record pointer to cpu entry for lookup in _ucidef_finish_switch_roles()
	[ -n "$device" ] && {
		export "cpu$n_cpu=$n_ports"
		n_cpu=$((n_cpu + 1))
	}

	# create/append object to role list
	[ -n "$role" ] && {
		json_select_array roles

		if [ "$role" != "$prev_role" ]; then
			json_add_object
				json_add_string role "$role"
				json_add_string ports "$num"
			json_close_object

			prev_role="$role"
			n_vlan=$((n_vlan + 1))
		else
			json_select_object "$n_vlan"
				json_get_var port ports
				json_add_string ports "$port $num"
			json_select ..
		fi

		json_select ..
	}
}

_ucidef_finish_switch_roles() {
	# inherited: $name $n_cpu $n_vlan $cpu0 $cpu1 $cpu2 $cpu3 $cpu4 $cpu5
	local index role roles num device need_tag want_untag port ports

	json_select switch
		json_select "$name"
			json_get_keys roles roles
		json_select ..
	json_select ..

	for index in $roles; do
		eval "port=\$cpu$(((index - 1) % n_cpu))"

		json_select switch
			json_select "$name"
				json_select ports
					json_select "$port"
						json_get_vars num device need_tag want_untag
					json_select ..
				json_select ..

				if [ ${need_tag:-0} -eq 1 -o ${want_untag:-0} -ne 1 ]; then
					num="${num}t"
					device="${device}.${index}"
				fi

				json_select roles
					json_select "$index"
						json_get_vars role ports
						json_add_string ports "$ports $num"
						json_add_string device "$device"
					json_select ..
				json_select ..
			json_select ..
		json_select ..

		json_select_object network
			local devices

			json_select_object "$role"
				# attach previous interfaces (for multi-switch devices)
				json_get_var devices device
				if ! list_contains devices "$device"; then
					devices="${devices:+$devices }$device"
				fi
			json_select ..
		json_select ..

		ucidef_set_interface "$role" device "$devices"
	done
}

ucidef_set_ar8xxx_switch_mib() {
	local name="$1"
	local type="$2"
	local interval="$3"

	json_select_object switch
		json_select_object "$name"
			json_add_int ar8xxx_mib_type $type
			json_add_int ar8xxx_mib_poll_interval $interval
		json_select ..
	json_select ..
}

ucidef_add_switch() {	
	local enabled=1
	if [ "$1" = "enabled" ]; then
		shift
		enabled="$1"
		shift
	fi

	local name="$1"; shift
	local port num role device index need_tag prev_role sfp
	local cpu0 cpu1 cpu2 cpu3 cpu4 cpu5
	local n_cpu=0 n_vlan=0 n_ports=0

	json_select_object switch
		json_select_object "$name"
			json_add_boolean enable "$enabled"
			json_add_boolean reset 1

			for port in "$@"; do
				case "$port" in
					[0-9]*@*)
						num="${port%%@*}"
						device="${port##*@}"
						need_tag=0
						want_untag=0
						[ "${num%t}" != "$num" ] && {
							num="${num%t}"
							need_tag=1
						}
						[ "${num%u}" != "$num" ] && {
							num="${num%u}"
							want_untag=1
						}
					;;
					[0-9]*:*:[0-9]*)
						num="${port%%:*}"
						index="${port##*:}"
						role="${port#[0-9]*:}"; role="${role%:*}"
					;;
					[0-9]*:*)
						[ "${port: -2}" = "#s" ] && sfp=1 && port="${port%%#s}"
						num="${port%%:*}"
						role="${port##*:}"
					;;
				esac

				if [ -n "$num" ] && [ -n "$device$role" ]; then
					_ucidef_add_switch_port
				fi

				unset num device role index need_tag want_untag sfp
			done
		json_select ..
	json_select ..

	_ucidef_finish_switch_roles
}

ucidef_add_switch_attr() {
	local name="$1"
	local key="$2"
	local val="$3"

	json_select_object switch
		json_select_object "$name"

		case "$val" in
			true|false) [ "$val" != "true" ]; json_add_boolean "$key" $? ;;
			[0-9]) json_add_int "$key" "$val" ;;
			*) json_add_string "$key" "$val" ;;
		esac

		json_select ..
	json_select ..
}

ucidef_add_switch_port_attr() {
	local name="$1"
	local port="$2"
	local key="$3"
	local val="$4"
	local ports i num

	json_select_object switch
	json_select_object "$name"

	json_get_keys ports ports
	json_select_array ports

	for i in $ports; do
		json_select "$i"
		json_get_var num num

		if [ -n "$num" ] && [ $num -eq $port ]; then
			json_select_object attr

			case "$val" in
				true|false) [ "$val" != "true" ]; json_add_boolean "$key" $? ;;
				[0-9]) json_add_int "$key" "$val" ;;
				*) json_add_string "$key" "$val" ;;
			esac

			json_select ..
		fi

		json_select ..
	done

	json_select ..
	json_select ..
	json_select ..
}

ucidef_set_interface_macaddr() {
	local network="$1"
	local macaddr="$2"

	ucidef_set_interface "$network" macaddr "$macaddr"
}

ucidef_add_atm_bridge() {
	local vpi="$1"
	local vci="$2"
	local encaps="$3"
	local payload="$4"
	local nameprefix="$5"

	json_select_object dsl
		json_select_object atmbridge
			json_add_int vpi "$vpi"
			json_add_int vci "$vci"
			json_add_string encaps "$encaps"
			json_add_string payload "$payload"
			json_add_string nameprefix "$nameprefix"
		json_select ..
	json_select ..
}

ucidef_add_adsl_modem() {
	local annex="$1"
	local firmware="$2"

	json_select_object dsl
		json_select_object modem
			json_add_string type "adsl"
			json_add_string annex "$annex"
			json_add_string firmware "$firmware"
		json_select ..
	json_select ..
}

ucidef_add_vdsl_modem() {
	local annex="$1"
	local tone="$2"
	local xfer_mode="$3"

	json_select_object dsl
		json_select_object modem
			json_add_string type "vdsl"
			json_add_string annex "$annex"
			json_add_string tone "$tone"
			json_add_string xfer_mode "$xfer_mode"
		json_select ..
	json_select ..
}

ucidef_set_rssimon() {
	local dev="$1"
	local refresh="$2"
	local threshold="$3"

	json_select_object rssimon

	json_select_object "$dev"
	[ -n "$refresh" ] && json_add_int refresh "$refresh"
	[ -n "$threshold" ] && json_add_int threshold "$threshold"
	json_select ..

	json_select ..
}

ucidef_add_gpio_switch() {
	local cfg="$1"
	local name="$2"
	local pin="$3"
	local default="${4:-0}"

	json_select_object gpioswitch
		json_select_object "$cfg"
			json_add_string name "$name"
			json_add_int pin "$pin"
			json_add_int default "$default"
		json_select ..
	json_select ..
}

ucidef_set_hostname() {
	local hostname="$1"

	json_select_object system
		json_add_string hostname "$hostname"
	json_select ..
}

ucidef_set_ntpserver() {
	local server

	json_select_object system
		json_select_array ntpserver
			for server in "$@"; do
				json_add_string "" "$server"
			done
		json_select ..
	json_select ..
}

ucidef_set_network_options() {
	json_select_object "network_options"
	n=$#

	for i in $(seq $((n / 2))); do
		opt="$1"
		val="$2"

		if [ "$val" -eq "$val" ] 2>/dev/null; then
			json_add_int "$opt" "$val"
		else
			[ "$val" = "true" ] && val=1 || val=0
			json_add_boolean "$opt" "$val"
		fi
		shift; shift
	done
	json_select ..
}

ucidef_set_poe() {
	local poe

	json_get_var poe poe
	[ -n "$poe" ] && return

	json_select_object poe
		json_add_string "bus" "/dev/$1"
		json_add_int "chip_count" "$2"
		json_add_int "budget" "$3"
		json_add_int "poe_ports" "$4"
		shift 4
		json_select_array ports
			while [ $# -gt 0 ]; do
				json_add_object ""
					json_add_string "name" "$1"
					json_add_string "class" "$2"
					json_add_int "budget" "$3"
				json_close_object
				shift 3
			done
		json_select ..
	json_select ..
}

ucidef_set_poe_chip() {
	json_select_object poe
		json_select_array poe_chips
			json_add_object ""
				for port in "$@"; do
					case "$port" in
						0X*)
							json_add_string address "$port"
						;;
						[0-9]:*)
							json_add_string chan"${port%%:*}" "${port##*:}"
						;;
					esac
				done
			json_close_object
		json_select ..
	json_select ..
}

ucidef_usbcheck() {
	json_add_object usbcheck
		json_add_string path "$1"
	json_close_object
}

ucidef_usbhubcheck() {
	json_add_object usbhubcheck
		json_add_string usb_id "$1"
		json_add_string gpio "$2"
	json_close_object
}

board_config_update() {
	json_init
	[ -f ${CFG} ] && json_load "$(cat ${CFG})"

	# auto-initialize model id and name if applicable
	if ! json_is_a model object; then
		json_select_object model
			[ -f "/tmp/sysinfo/board_name" ] && \
				json_add_string id "$(cat /tmp/sysinfo/board_name)"
			[ -f "/tmp/sysinfo/model" ] && \
				json_add_string name "$(cat /tmp/sysinfo/model)"
		json_select ..
	fi
}

board_config_flush() {
	json_dump -i -o ${CFG}
	[ "$CFG" = "/etc/board.json" ] && md5sum ${CFG} > /etc/board.hash
}
