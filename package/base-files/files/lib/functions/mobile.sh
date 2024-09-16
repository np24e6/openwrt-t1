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
	else
		retry="$((retry + 1))"
		rm /tmp/conn_retry_$interface >/dev/null 2>/dev/null
		echo "$retry" > /tmp/conn_retry_$interface
	fi
}

check_pdp_context() {
	local pdp="$1"
	local modem_id="$2"
	local mdm_ubus_obj list cid found

	mdm_ubus_obj="$(find_mdm_ubus_obj "$modem_id")"
	[ -z "$mdm_ubus_obj" ] && echo "gsm.modem object not found" && return 1
	found=0

	json_load "$(ubus call "$mdm_ubus_obj" get_pdp_ctx_list)"
	json_get_keys list list
	json_select list

	[ -z "$list" ] && echo "PDP list empty" && return

	for ctx in $list; do
		json_select "$ctx"
		json_get_vars cid
		[ "$cid" = "$pdp" ] && {
			found=1
			break
		}
		json_select ..
	done

	[ $found -eq 0 ] && {
		echo "Creating context with PDP: $pdp"
		ubus call "$mdm_ubus_obj" set_pdp_ctx "{\"cid\":${pdp},\"type\":\"ip\",\"apn\":\"\",\"addr\":\"\",\
		\"dt_comp\":\"off\",\"hdr_comp\":\"off\",\"ipv4_alloc\":\"nas\",\"req_tp\":\"generic\"}" >/dev/null 2>/dev/null

		ubus call "$mdm_ubus_obj" set_func "{\"func\":\"rf\",\"reset\":false}" >/dev/null 2>/dev/null

		sleep 2

		ubus call "$mdm_ubus_obj" set_func "{\"func\":\"full\",\"reset\":false}" >/dev/null 2>/dev/null
	}
	return 0
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
	ubus call mctl reboot "{\"id\":\"$modem_id\"}"
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

	echo "$error" | grep -qi "Policy Mismatch" && {
		logger -t "mobile.sh" "Policy Mismatch. Resetting mobile network..."
		gsm_soft_reset "$modem_id"
		return 1
	}

	echo "$error" | grep -qi "Failed to connect to service" && {
		logger -t "mobile.sh" "Device not responding, restarting module"
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
		iptables -A forwarding_${zone}_rule -m comment --comment "!fw3: Mobile bridge" -j zone_lan_dest_ACCEPT
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
	ip route add default dev br-lan table 43
	ip route add "$bridge_ipaddr" dev br-lan

	ip rule add pref 5042 from "$bridge_ipaddr" lookup 42
	ip rule add pref 5043 iif "$dev" lookup 43
	#sysctl -w net.ipv4.conf.br-lan.proxy_arp=1 #2>/dev/null
	model="$(gsmctl --model ${modem_num:+-O "$modem_num"})"
	[ "${model:0:4}" = "UC20" ] && ip neighbor add proxy "$bridge_ipaddr" dev "$dev" 2>/dev/null

	iptables -A postrouting_rule -m comment --comment "Bridge mode" -o "$dev" -j ACCEPT -tnat

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

	/etc/init.d/dnsmasq reload
	swconfig dev 'switch0' set soft_reset 5 &
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
	[ -n "$zone" ] && json_add_string zone "$zone"
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
	[ -n "$zone" ] && json_add_string zone "$zone"
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

notify_mtu_diff(){
	local operator_mtu="$1"
	local interface_name="$2"
	local current_mtu="$3"
	[ -n "$operator_mtu" ] && [ "$operator_mtu" != "$current_mtu" ] && {
		echo "Notifying WebUI that operator ($operator_mtu) and configuration MTU ($current_mtu) differs"
		touch "/tmp/vuci/mtu_${interface_name}_${operator_mtu}"
	}
}
