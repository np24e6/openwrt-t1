#!/bin/sh
[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. /lib/functions/mobile.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

proto_pppmobile_init_config() {
	#~ This proto usable only with proto wwan
	available=1
	no_device=1
	disable_auto_up=1
	teardown_on_l3_link_down=1

	proto_config_add_string delay
	proto_config_add_string modes
	proto_config_add_boolean dhcp
	proto_config_add_boolean dhcpv6
	proto_config_add_int ip4table
	proto_config_add_int ip6table

	#teltonika specific
	proto_config_add_string modem
	proto_config_add_string pdp
	proto_config_add_string pdptype
	proto_config_add_string sim
	proto_config_add_string method
	proto_config_add_string passthrough_mode
	proto_config_add_string leasetime
	proto_config_add_string mac
	proto_config_add_int mtu

	proto_config_add_defaults
}

add_ipv4_dynamic_ifname() {
	local ifname_="$1"

	json_init
	json_add_string pppname "ppp-$interface"
	json_add_string name "$ifname_"
	json_add_string device "$ctl_device"
	json_add_string proto "ppp"
	json_add_string ipv6 "0"
	json_add_string nopersist
	[ -n "$ip4table" ] && json_add_string ip4table "$ip4table"
	[ -n "$zone" ] && json_add_string zone "$zone"

	proto_add_dynamic_defaults
	ubus call network add_dynamic "$(json_dump)"

	json_init
	json_add_string modem "$modem"
	json_add_string sim "$sim"
	[ -n "$zone" ] && json_add_string zone "$zone"
	ubus call network.interface."$ifname_" set_data "$(json_dump)"
}

add_ipv6_dynamic_ifname() {
	local ifname_="$1"

	json_init
	json_add_string pppname "ppp-$interface"
	json_add_string name "$ifname_"
	json_add_string device "$ctl_device"
	json_add_string proto "ppp"
	json_add_string ipv6 "1"
	json_add_string extendprefix 1
	json_add_boolean ignore_valid 1
	[ -n "$ip6table" ] && json_add_string ip6table "$ip6table"
	[ -n "$zone" ] && json_add_string zone "$zone"

	proto_add_dynamic_defaults
	ubus call network add_dynamic "$(json_dump)"

	[ "$method" != "bridge" ] && [ "$method" != "passthrough" ] && {
		json_init
		json_add_string modem "$modem"
		json_add_string sim "$sim"
		[ -n "$zone" ] && json_add_string zone "$zone"
		ubus call network.interface."$ifname_" set_data "$(json_dump)"
	}
}

update_mobile_ifname() {
	proto_init_update "ppp-$interface" 1
	proto_set_keep 1
	proto_add_data
	json_add_string "pdp" "$pdp"
	json_add_string "method" "$method"
	json_add_boolean "static_mobile" "1" # Required for mdcollect
	proto_close_data

	proto_send_update "$interface"
}

add_hide_dynamic_ipv6_interface() {
	local ifname_="$1"

	json_init
	json_add_string name "$ifname_"
	json_add_string proto "dhcpv6"
	json_add_string _area_type "wan"
	json_add_string device "ppp-$interface"

	json_add_string extendprefix 1
	json_add_boolean ignore_valid 1
	[ -n "$ip6table" ] && json_add_string ip6table "$ip6table"
	[ -n "$zone" ] && json_add_string zone "$zone"
	proto_add_dynamic_defaults
	ubus call network add_dynamic "$(json_dump)"

	# this need to mdcollect to recognize interface as mobile
	[ "$method" != "bridge" ] && [ "$method" != "passthrough" ] && {
		json_init
		json_add_string modem "$modem"
		json_add_string sim "$sim"
		[ -n "$zone" ] && json_add_string zone "$zone"
		ubus call network.interface."$ifname_" set_data "$(json_dump)"
	}
}

down_mobile_interfaces() {
	local interface="$1"
	ubus call network.interface down "{\"interface\":\"${interface}_4\"}" 2>/dev/null
	ubus call network.interface down "{\"interface\":\"${interface}_6\"}" 2>/dev/null
	ubus call network.interface down "{\"interface\":\"ppp-${interface}_6\"}" 2>/dev/null
}

wait_data_connection() {
	local ifname_="$1"
	local i

	## need to wait while interface appear
	local max_wait=10
	echo "Looking l3 device for $ifname_ interface"
	for i in $(seq $max_wait); do
		l3_device=$(ifstatus "$ifname_" | grep l3_device)
		if [ -z "$l3_device" ]; then
			sleep 1
		else
		 	return 0
		fi
		if [ $i -eq $max_wait ]; then
			echo "Can't find l3 device for $ifname_ interface"
			ubus call network.interface remove "{\"interface\":\"$ifname_\"}" 2>/dev/null
			return 1
		fi
	done

    	return 1
}

proto_pppmobile_setup() {
	#~ This proto usable only with proto wwan
	local interface="$1"
	local pdp modem sim delay $PROTO_DEFAULT_OPTIONS
	local ip4table ip6table
	local multisim="0" active_sim="1"
	local ifname

	json_get_vars pdp pdptype modem sim delay ip4table ip6table $PROTO_DEFAULT_OPTIONS

        local gsm_modem="$(find_mdm_ubus_obj "$modem")"

        [ -z "$gsm_modem" ] && {
                echo "Failed to find gsm modem ubus object, exiting."
                return 1
        }

	[ -n $modem ] && modem="$devicename"
	config_load network
	config_get sim "$interface" "sim" "1"
	config_get pdp "$interface" "pdp" "1"
	config_get method "$interface" "method" "1"

	local zone="$(fw3 -q network "$interface" 2>/dev/null)"

	[ -n "$delay" ] || [ "$pdp" = "1" ] && delay=0 || delay=3
	sleep "$delay"

	echo "Got SIM$sim for modem $modem"

	check_pdp_context "$pdp" "$modem" || {
		proto_notify_error "$interface" "NO_DEVICE"
		proto_block_restart "$interface"
	}

#~ Parameters part------------------------------------------------------

# 	Check sim positions count in simcard config
	get_sim_count(){
		local section="$1"
		local sim_modem sim_position
		config_get sim_modem "$section" modem
		config_get sim_position "$section" position
		[ "$modem" = "$sim_modem" ] && [ "$sim_position" -gt 1 ] && multisim="1"
	}
	config_load simcard
	config_foreach get_sim_count sim

# 	Check sim position in simd if modem is registered as multisim
	[ "$multisim" = "1" ] && {
		echo "Quering active sim position"
		json_set_namespace gobinet old_cb
		json_load "$(ubus call $gsm_modem get_sim_slot)"
		json_get_var active_sim index
		json_set_namespace $old_cb
	}

# 	Restart if check failed
	[ "$active_sim" -ge 1 ] && [ "$active_sim" -le 2 ] || return
# 	check if current sim and interface sim match
	[ "$active_sim" = "$sim" ] || {
		echo "Active sim: $active_sim. \
		This interface uses different simcard: $sim."
		proto_notify_error "$interface" WRONG_SIM
		proto_block_restart "$interface"
		return
	}

	get_simcard_parameters() {
		local section="$1"
		local mdm
		config_get position "$section" position
		config_get mdm "$section" modem

		[ "$modem" = "$mdm" ] && \
		[ "$position" = "$active_sim" ] && {
			config_get deny_roaming "$section" deny_roaming "0"
		}
	}
	config_foreach get_simcard_parameters "sim"

#~ Connection part------------------------------------------------------
	dialnumber=${dialnumber:-"*99#"}
	echo -ne "ATD$dialnumber\r\n" > "$ctl_device"

#~ Network setup part------------------------------------------------------

	pdptype="$(echo "$pdptype" | awk '{print tolower($0)}')"

	case $pdptype in
	ip)
		echo "PDP type IP"
		ifname="${interface}_4"

		add_ipv4_dynamic_ifname "$ifname"
		wait_data_connection "$ifname" || return
		update_mobile_ifname "$ifname"
	;;
	ipv6)
		echo "PDP type IPv6"
		ifname="${interface}_6"

		add_ipv6_dynamic_ifname "ppp-$ifname"
		wait_data_connection "ppp-$ifname" || return
		update_mobile_ifname "$ifname"
		add_hide_dynamic_ipv6_interface "$ifname"
	;;
	ipv4v6)
		echo "PDP type IPv4v6"
		ifname_4="${interface}_4"
		ifname_6="${interface}_6"

		add_ipv6_dynamic_ifname "$ifname_4"
		wait_data_connection "$ifname_4"
		if [ "$?" = "0" ]; then
			update_mobile_ifname "$interface"
		# 	## need to add secondary interface to run dhcpv6 protocol
			add_hide_dynamic_ipv6_interface "$ifname_6"
		else
			down_mobile_interfaces "$interface"
			sleep 20
			return
		fi

	;;
	esac
}

#~ ---------------------------------------------------------------------
proto_pppmobile_teardown() {
	local interface="$1"
	local bridge_ipaddr method

	json_load "$(ubus call network.interface.$interface status)"
	json_select data
	json_get_vars method bridge_ipaddr

	echo "Stopping network $interface"
	gsmctl -A 'AT&D2' > /dev/null
	down_mobile_interfaces "$interface"

	[ "$method" = "bridge" ] || [ "$method" = "passthrough" ] && {
		ip rule del pref 5042
		ip rule del pref 5043
		ip route flush table 42
		ip route flush table 43
		ip route del "$bridge_ipaddr"
		swconfig dev switch0 set soft_reset 5 &
	}

	#Clear passthrough and bridge params
	iptables -t nat -F postrouting_rule

	local zone="$(fw3 -q network "$interface" 2>/dev/null)"
	iptables -F forwarding_${zone}_rule

	rm -f "/tmp/dnsmasq.d/bridge" 2>/dev/null
	ip neigh flush proxy
}

[ -n "$INCLUDE_ONLY" ] || {
	add_protocol pppmobile
}
