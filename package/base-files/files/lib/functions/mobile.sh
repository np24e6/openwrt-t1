#Mobile configuration management lib

. /usr/share/libubox/jshn.sh
. /lib/functions.sh

find_mdm_ubus_obj() {
	local modem_id="$1"
	local mdm_list curr_id

	mdm_list="$(ubus list gsm.modem*)"
	[ -z "$mdm_list" ] && echo "" && return

	for val in $mdm_list; do
		json_load "$(ubus call "$val" info)"
		json_get_var curr_id usb_id
		[ "$curr_id" = "$modem_id" ] && {
			echo "$val"
			return
		}
	done

	echo ""
}

find_mdm_mobifd_obj() {
	local modem_id="$1"
	mdm_ubus_obj="$(find_mdm_ubus_obj "$modem_id")"
	[ -z "$mdm_ubus_obj" ] && echo "" || echo "${mdm_ubus_obj:4}"
}

handle_retry() {
	local retry="$1"
	local interface="$2"

	if [ "$retry" -ge 5 ]; then
		rm /tmp/conn_retry_$interface >/dev/null 2>/dev/null
		object=$(find_mdm_mobifd_obj $modem)
		[ -z "$object" ] && echo "Can't find modem $modem gsm object" && return
		echo "$modem $interface reloading mobifd"
		ubus -t 180 call mobifd.$object reload
		ifdown "$interface"
	else
		retry="$((retry + 1))"
		rm /tmp/conn_retry_$interface >/dev/null 2>/dev/null
		echo "$retry" > /tmp/conn_retry_$interface
	fi
}

# finds esim_profile where status is 1 and returns index of that profile
# returns 0 if not and esim or no active esim profile
get_active_esim_profile_index() {
	local modem="$1"
	local esim_profiles
	local esim_profile_index
	local esim_profile_status
	local is_esim

	object=$(find_mdm_mobifd_obj $modem)
	[ -z "$object" ] && echo "Can't find modem $modem gsm object" && return

	# todo: once rpcd esim support is added, use that instead of mobifd
	esim_profiles="$(ubus call mobifd.$object list_esim_profiles)"
	[ -z "$esim_profiles" ] && echo "-1" && return

	json_load "$esim_profiles"

	# check if is_esim is true
	json_get_var is_esim is_esim
	[ "$is_esim" != "1" ] && echo "0" && return

	#find esim_profile with status 1
	json_select profiles
	json_get_keys profiles
	for profile in $profiles; do
		json_select "$profile"
		json_get_var esim_profile_status status
		json_select ..
		[ "$esim_profile_status" = "1" ] && {
			esim_profile_index="$profile"
			#convert to 0 based index
			esim_profile_index=$((esim_profile_index-1))
			echo "$esim_profile_index"
			return
		}
	done

	[ -z "$esim_profile_index" ] && logger -t "mobile.sh" "No active esim profile"

	echo "-1"
}

gsm_soft_reset() {
	local modem_id="$1"
	local mdm_ubus_obj

	mdm_ubus_obj="$(find_mdm_ubus_obj "$modem_id")"
	[ -z "$mdm_ubus_obj" ] && echo "gsm.modem object not found" && return

	ubus call "$mdm_ubus_obj" set_func "{\"func\":\"rf\",\"reset\":false}" >/dev/null 2>/dev/null

	sleep 2

	ubus call "$mdm_ubus_obj" set_func "{\"func\":\"full\",\"reset\":false}" >/dev/null 2>/dev/null
}

kill_uqmi_processes() {
	local device="$1"
	logger -t "mobile.sh" "Clearing uqmi processes for $device"
	killall "uqmi -d $device" 2>/dev/null
	killall "uqmi -s -d $device" 2>/dev/null
}

gsm_hard_reset() {
	local modem_id="$1"
	echo "Calling \"mctl --reboot -i $modem_id\""
	mctl --reboot -i "$modem_id"
}

qmi_error_handle() {
	local error="$1"
	local error_cnt_in="$2"
	local modem_id="$3"
	local skip_reset="$4"

	echo "$error" | grep -qi "Unknown error" && {
		error_cnt=$((error_cnt_in+1))
		logger -t "mobile.sh" "Received unknown error($error_cnt/5): $error"
		[ $error_cnt = "5" ] && {
			gsm_hard_reset "$modem_id"
			kill_uqmi_processes "$device"
			return 1
		}
		return 0
	}

	echo "$error" | grep -qi "error" && {
		logger -t "mobile.sh" "$error"
	}

	echo "$error" | grep -qi "Client IDs exhausted" && {
			logger -t "mobile.sh" "ClientIdsExhausted! resetting counter..."
			proto_notify_error "$interface" NO_CID
			uqmi -s -d "$device" --sync
			return 1
	}

	echo "$error" | grep -qi "Call Failed" && {
		[ "$skip_reset" != "true" ] && {
			logger -t "mobile.sh" "Device not responding, resetting mobile network"
			sleep 10
			gsm_soft_reset "$modem_id"
		}
		return 1
	}

	echo "$error" | grep -qi "Failed to connect to service" && {
		logger -t "mobile.sh" "Device not responding, restarting module"
		gsm_hard_reset "$modem_id"
		kill_uqmi_processes "$device"
		return 1
	}

	echo "$error" | grep -qi "Request canceled" && {
		logger -t "mobile.sh" "Device returns bad packages, restarting module"
		gsm_hard_reset "$modem_id"
		kill_uqmi_processes "$device"
		return 1
	}

	return 0
}

sim1_pass=
sim2_pass=
get_passthrough_interfaces() {
	local sim method
	config_get method "$1" "method"
	config_get sim "$1" "sim"
	[ "$method" = "passthrough" ] && [ "$sim" = "1" ] && sim1_pass="$1"
	[ "$method" = "passthrough" ] && [ "$sim" = "2" ] && sim2_pass="$1"
}

passthrough_mode=
get_passthrough() {
	config_get primary "$1" primary
	[ "$primary" = "1" ] && {
		config_get sim "$1" position;
		passthrough_mode=$(eval uci -q get network.\${sim${sim}_pass}.passthrough_mode 2>/dev/null);
	}
}

setup_bridge_v4() {
	local dev="$1"
	local modem_num="$2"
	local p2p
	local dhcp_param_file="/tmp/dnsmasq.d/bridge"
	local model
	echo "$parameters4"

	[[ "$dev" = "rmnet_data"* ]] && { ## TRB5 uses qmicli - different format
		bridge_ipaddr="$(echo "$parameters4" | sed -n "s_.*IPv4 address: \([0-9.]*\)_\1_p")"
		bridge_mask="$(echo "$parameters4" | sed -n "s_.*IPv4 subnet mask: \([0-9.]*\)_\1_p")"
		bridge_gateway="$(echo "$parameters4" | sed -n "s_.*IPv4 gateway address: \([0-9.]*\)_\1_p")"
		bridge_dns1="$(echo "$parameters4" | sed -n "s_.*IPv4 primary DNS: \([0-9.]*\)_\1_p")"
		bridge_dns2="$(echo "$parameters4" | sed -n "s_.*IPv4 secondary DNS: \([0-9.]*\)_\1_p")"
	} || {
		json_load "$parameters4"
		json_select "ipv4"
		json_get_var bridge_ipaddr ip
		json_get_var bridge_mask subnet
		json_get_var bridge_gateway gateway
		json_get_var bridge_dns1 dns1
		json_get_var bridge_dns2 dns2
	}

	json_init
	json_add_string name "${interface}_4"
	json_add_string ifname "$dev"
	json_add_string proto "none"
	json_add_object "data"
	ubus call network add_dynamic "$(json_dump)"
	IFACE4="${interface}_4"

	json_init
	json_add_string interface "${interface}_4"
	[ -n "$zone" ] && {
		json_add_string zone "$zone"
		iptables -w -A forwarding_${zone}_rule -m comment --comment "!fw3: Mobile bridge" -j zone_lan_dest_ACCEPT
	}
	ubus call network.interface set_data "$(json_dump)"

	json_init
	json_add_string interface "${interface}"
	json_add_string bridge_ipaddr "$bridge_ipaddr"
	json_add_string bridge_gateway "$bridge_gateway"
	ubus call network.interface set_data "$(json_dump)"

	json_init
	json_add_string modem "$modem"
	json_add_string sim "$sim"
	ubus call network.interface."${interface}_4" set_data "$(json_dump)"
	json_close_object

	json_init
	json_add_string name "mobile_bridge"
	json_add_string ifname "br-lan"
	json_add_string proto "static"
	json_add_string gateway "0.0.0.0"
	json_add_array ipaddr
	json_add_string "" "$bridge_gateway"
	json_close_array
	json_add_string ip4table "40"
	ubus call network add_dynamic "$(json_dump)"

	ip route add default dev "$dev" table 42
	ip route add "$bridge_ipaddr" dev br-lan
	ip route add default via "$bridge_ipaddr" dev br-lan table 43

	ip rule add pref 5042 iif br-lan lookup 42
	ip rule add pref 5043 iif "$dev" lookup 43
	#sysctl -w net.ipv4.conf.br-lan.proxy_arp=1 #2>/dev/null
	model="$(gsmctl --model ${modem_num:+-O "$modem_num"})"
	[ "${model:0:4}" = "UC20" ] || [ "${model:0:6}" = "RG500U" ] && ip neighbor add proxy "$bridge_ipaddr" dev "$dev" 2>/dev/null

	[ -n "$mac" ] && {
		ip neigh flush dev br-lan
		ip neigh add "$bridge_ipaddr" dev br-lan lladdr "$mac"
	}

	iptables -w -A postrouting_rule -m comment --comment "Bridge mode" -o "$dev" -j ACCEPT -tnat

	config_load network
	config_foreach get_passthrough_interfaces interface
	config_get p2p "$interface" p2p "0"

	config_load simcard
	config_foreach get_passthrough sim

	> $dhcp_param_file
	[ -z "$mac" ] && mac="*:*:*:*:*:*"
	[ "$p2p" -eq 1 ] && bridge_mask=255.255.255.255
	[ "$passthrough_mode" != "no_dhcp" ] && {
		echo "dhcp-range=tag:mobbridge,$bridge_ipaddr,static,$bridge_mask,${leasetime:-1h}" > "$dhcp_param_file"
		echo "shared-network=br-lan,$bridge_ipaddr" >> "$dhcp_param_file"
		echo "dhcp-host=$mac,set:mobbridge,$bridge_ipaddr" >> "$dhcp_param_file"
		echo "dhcp-option=tag:mobbridge,br-lan,3,$bridge_gateway" >> "$dhcp_param_file"

		[ -n "$bridge_dns1" ] || [ -n "$bridge_dns2" ] && {
			echo "dhcp-option=tag:mobbridge,br-lan,6${bridge_dns1:+,$bridge_dns1}${bridge_dns2:+,$bridge_dns2}" >> "$dhcp_param_file"
			echo "server=$bridge_dns1" >> "$dhcp_param_file"
			echo "server=$bridge_dns2" >> "$dhcp_param_file"
		}
	}
	[ "$passthrough_mode" = "no_dhcp" ] && {
		echo "server=$bridge_dns1" >> "$dhcp_param_file"
		echo "server=$bridge_dns2" >> "$dhcp_param_file"
	}

	echo "method:$method bridge_ipaddr:$bridge_ipaddr" > "/var/run/${interface}_braddr"

	/etc/init.d/dnsmasq reload

	if is_device_dsa ; then
		restart_dsa_interfaces
	else
		swconfig dev 'switch0' set soft_reset 5 &
	fi

}

setup_static_v4() {
	local dev="$1"
	echo "Setting up $dev V4 static"
	echo "$parameters4"

	[[ "$dev" = "rmnet_data"* ]] && { ## TRB5 uses qmicli - different format
		ip_4="$(echo "$parameters4" | sed -n "s_.*IPv4 address: \([0-9.]*\)_\1_p")"
		dns1_4="$(echo "$parameters4" | sed -n "s_.*IPv4 primary DNS: \([0-9.]*\)_\1_p")"
		dns2_4="$(echo "$parameters4" | sed -n "s_.*IPv4 secondary DNS: \([0-9.]*\)_\1_p")"
	} || {
		json_load "$parameters4"
		json_select "ipv4"
		json_get_var ip_4 ip
		json_get_var dns1_4 dns1
		json_get_var dns2_4 dns2
	}

	json_init
	json_add_string name "${interface}_4"
	json_add_string ifname "$dev"
	json_add_string proto static
	json_add_string gateway "0.0.0.0"

	json_add_array ipaddr
		json_add_string "" "$ip_4"
	json_close_array

	json_add_array dns
		[ -n "$dns1_4" ] && json_add_string "" "$dns1_4"
		[ -n "$dns2_4" ] && json_add_string "" "$dns2_4"
	json_close_array

	[ -n "$ip4table" ] && json_add_string ip4table "$ip4table"
	proto_add_dynamic_defaults

	ubus call network add_dynamic "$(json_dump)"
}

setup_dhcp_v4() {
	local dev="$1"
	echo "Setting up $dev V4 DCHP"
	json_init
	json_add_string name "${interface}_4"
	json_add_string ifname "$dev"
	json_add_string proto "dhcp"
	json_add_string script "/lib/netifd/dhcp_mobile.script"
	json_add_boolean ismobile "1"
	[ -n "$ip4table" ] && json_add_string ip4table "$ip4table"
	proto_add_dynamic_defaults
	ubus call network add_dynamic "$(json_dump)"
}

setup_dhcp_v6() {
	local dev="$1"
	echo "Setting up $dev V6 DHCP"
	json_init
	json_add_string name "${interface}_6"
	json_add_string ifname "$dev"
	json_add_string proto "dhcpv6"
	[ -n "$ip6table" ] && json_add_string ip6table "$ip6table"
	json_add_boolean ignore_valid 1
	proto_add_dynamic_defaults
	# RFC 7278: Extend an IPv6 /64 Prefix to LAN
	json_add_string extendprefix 1
	ubus call network add_dynamic "$(json_dump)"
}

setup_static_v6() {
	local dev="$1"
	echo "Setting up $dev V6 static"
	echo "$parameters6"

		local custom="$(uci get network.${interface}.dns)"

	[[ "$dev" = "rmnet_data"* ]] && { ## TRB5 uses qmicli - different format
		ip6_with_prefix="$(echo "$parameters6" | sed -n "s_.*IPv6 address: \([0-9a-f:]*\)_\1_p")"
		ip_6="${ip6_with_prefix%/*}"
		[[ -z "$custom" ]] && {
			dns1_6="$(echo "$parameters6" | sed -n "s_.*IPv6 primary DNS: \([0-9a-f:]*\)_\1_p")"
			dns2_6="$(echo "$parameters6" | sed -n "s_.*IPv6 secondary DNS: \([0-9a-f:]*\)_\1_p")"
		}
		} || {
		json_load "$parameters6"
		json_select "ipv6"
		json_get_var ip_6 ip
		json_get_var ip_prefix_length ip-prefix-length
		ip_6="${ip_6%/*}"
		ip6_with_prefix="$ip_6/$ip_prefix_length"
			[[ -z "$custom" ]] && {
				json_get_var dns1_6 dns1
				json_get_var dns2_6 dns2
			}
		json_get_var ip_pre_len ip-prefix-length
	}

	json_init
	json_add_string name "${interface}_6"
	json_add_string ifname "$dev"
	json_add_string proto static
	json_add_string ip6gw "::0"

	json_add_array ip6prefix
		json_add_string "" "$ip6_with_prefix"
	json_close_array

	json_add_array ip6addr
		json_add_string "" "${ip_6}/128"
	json_close_array

	json_add_array dns
		[ -n "$dns1_6" ] && json_add_string "" "$dns1_6"
		[ -n "$dns2_6" ] && json_add_string "" "$dns2_6"
	json_close_array

	[ -n "$ip6table" ] && json_add_string ip6table "$ip6table"
	proto_add_dynamic_defaults

	ubus call network add_dynamic "$(json_dump)"
}

check_digits() {
	var="$1"
	echo "$var" | grep -E '^[+-]?[0-9]+$'
}

ubus_set_interface_data() {
	local modem sim zone iface_and_type
	modem="$1"
	sim="$2"
	zone="$3"
	iface_and_type="$4"

	json_init
	json_add_string modem "$modem"
	json_add_string sim "$sim"
	[ -n "$zone" ] && json_add_string zone "$zone"

	ubus call network.interface."${iface_and_type}" set_data "$(json_dump)"
}

get_pdp() {
	local pdp
	config_load network
	config_get pdp "$1" "pdp" "1"
	echo "$pdp"
}

get_config_sim() {
	local sim
	local DEFAULT_SIM="1"
	config_load network
	config_get sim "$1" "sim" "1"
	[ -z "$sim" ] && logger -t "mobile.sh" "sim option not found in config. Taking default: $DEFAULT_SIM" \
				  && sim="$DEFAULT_SIM"
	echo "$sim"
}

get_config_esim() {
	local esim
	local DEFAULT_ESIM="0"
	config_load network
	config_get esim "$1" "esim_profile" "0"
	[ -z "$esim" ] && esim="$DEFAULT_ESIM"
	echo "$esim"
}

notify_mtu_diff(){
	local operator_mtu="$1"
	local interface_name="$2"
	local current_mtu="$3"
	[ -n "$operator_mtu" ] && [ "$operator_mtu" != "$current_mtu" ] && {
		echo "Notifying WebUI that operator ($operator_mtu) and configuration MTU ($current_mtu) differs"
		touch "/tmp/vuci/mtu_${interface_name}_${operator_mtu}"
	}
}

get_modem_type() {
	local modem_id="$1"
	local modem modems id primary

	json_init
	json_load_file "/etc/board.json"
	json_get_keys modems modems
	json_select modems

	arr_len(){
		echo $#
	}

	local modem_count="$(arr_len $modems)"

	for modem in $modems; do
		json_select "$modem"
		json_get_vars id builtin primary

		[ "$id" != "$modem_id" ] && json_select .. && continue
		[ "$builtin" != "1" ] && json_select .. && continue

		[ "$modem_count" -gt 1 ] && {
			[ "$primary" = "1" ] && echo "primary" || echo "secondary"
		} || echo "internal"
		return
	done
	echo "external"
}

is_device_dsa(){
	[ "$(jsonfilter -i /etc/board.json -e '@.hwinfo.dsa')" = "true" ]
}

restart_dsa_interfaces(){
	restart_dsa_interfaces_cb() {
		local ifname
		config_get ifname "$1" "ifname"
		[ ${ifname:0:3} = "lan"  ] && {
			ethtool -r "$ifname"
		}
	}
	config_load network
	config_foreach restart_dsa_interfaces_cb port
}

get_braddr_var() {
	grep -o "$1:[^ ]*" "/var/run/${2}_braddr" 2>/dev/null | cut -d':' -f2
}

get_simcount_by_modem_num() {
	local modem_num="$1"
	local simcount=0

	for sim in $(seq 3); do
		sim_cfg="$(mnf_info -C $sim 2> /dev/null)"
		# Find the amount of matches to the modem_num
		if [ -n "$sim_cfg" ] && [ "${sim_cfg:1:1}" = "$modem_num" ]; then
			simcount=$((simcount + 1))
		fi
	done

	echo $simcount
}

reload_mobifd() {
	local gsm_modem="$1"
	local interface="$2"
	object=$(find_mdm_mobifd_obj $gsm_modem)
	if [ -z "$object" ]; then
		echo "Can't find modem $gsm_modem gsm object, reloading mobifd"
		ubus -t 180 call mobifd reload
		return
	fi
	echo "$gsm_modem $interface reloading mobifd"
	ubus -t 180 call mobifd.$object reload
}

get_active_sim() {
	local interface="$1"
	local old_cb="$2"
	local gsm_modem="$3"
	local active_sim="0"
	local max_retries=5
	local retry=0

	json_set_namespace gobinet old_cb

	while [ "$retry" -le "$max_retries" ]; do
		json_load "$(ubus call $gsm_modem get_sim_slot)"
		json_get_var active_sim index
		[ -n "$active_sim" ] && [ "$active_sim" != "0" ] && break
		retry="$((retry + 1))"
		sleep 2
	done

	json_set_namespace $old_cb

	[ -z "$active_sim" ] && active_sim="0"

	echo "$active_sim"
}

get_deny_roaming() {
	local active_sim="$1"
	local modem="$2"
	local esim_profile="$3"

	deny_roaming="0"

	deny_roaming_parse() {
		local section="$1"
		local mdm
		local esim_prof
		local position
		config_get position "$section" position
		config_get mdm "$section" modem
		config_get esim_prof "$section" esim_profile "0"

		[ "$modem" = "$mdm" ] && \
		[ "$position" = "$active_sim" ] && \
		[ "$esim_prof" = "$esim_profile" ] && {
			config_get deny_roaming "$section" deny_roaming "0"
		}
	}

	config_load simcard
	config_foreach deny_roaming_parse "sim"

	echo "$deny_roaming"
}

verify_active_sim() {
	local sim="$1"
	local active_sim="$2"
	local interface="$3"

	[ -z "$sim" ] && sim=$(get_config_sim "$interface")

# 	Restart if check failed
	if [ "$active_sim" -lt 1 ] || [ "$active_sim" -gt 4 ]; then
		echo "Bad active sim: $active_sim."
		return 1
	fi

# check if current sim and interface also if current esim match as well
	[ "$active_sim" = "$sim" ] || {
		echo "Active sim: $active_sim. \
		This interface uses different simcard: $sim."
		proto_notify_error "$interface" WRONG_SIM
		proto_block_restart "$interface"
		return 1
	}

	return 0
}

verify_active_esim() {
	local esim_profile="$1"
	local interface="$2"

	config_esim_profile=$(get_config_esim "$interface")

	[ "$esim_profile" = "$config_esim_profile" ] || {
		echo "This interface uses different esim profile: $config_esim_profile."
		proto_notify_error "$interface" WRONG_ESIM
		proto_block_restart "$interface"
		return 1
	}

	return 0
}
