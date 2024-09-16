#!/bin/sh
. /lib/netifd/uqmi_shared_functions.sh
[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. /lib/functions/mobile.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

proto_qmi_init_config() {
	#~ This proto usable only with proto wwan
	available=1
	disable_auto_up=1
	no_device=1
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

proto_qmi_setup() {
	#~ This proto usable only with proto wwan
	local interface="$1"

	local ipv4 ipv6 delay mtu net_device deny_roaming ret
	local multisim="0" active_sim="1"

	local dataformat connstat old_cb gobinet method
	local device apn auth username password pdp modem pdptype sim dhcp dhcpv6 leasetime mac
	local $PROTO_DEFAULT_OPTIONS IFACE4 IFACE6 ip4table ip6table parameters4 parameters6
	local pdh_4 pdh_6 cid cid_4 cid_6
	local retry_before_reinit
	local error_cnt

	json_get_vars device modem pdptype sim delay method mtu dhcp ip4table ip6table dhcpv6 \
	leasetime mac $PROTO_DEFAULT_OPTIONS

        local gsm_modem="$(find_mdm_ubus_obj "$modem")"

        [ -z "$gsm_modem" ] && {
                echo "Failed to find gsm modem ubus object, exiting."
                return 1
        }

	pdp=$(get_pdp "$interface")

	[ -n "$delay" ] || [ "$pdp" = "1" ] && delay=0 || delay=3
	sleep "$delay"

	[ -z "$sim" ] && sim=$(get_config_sim "$interface")
	check_pdp_context "$pdp" "$modem" || {
		proto_notify_error "$interface" "NO_DEVICE"
		proto_block_restart "$interface"
	}

#~ Parameters part------------------------------------------------------

	service_name="wds"
	options="--timeout 3000"

	#Check sim positions count in simcard config
	get_sim_count(){
		local section=$1
		local sim_modem sim_position
		config_get sim_modem "$section" modem
		config_get sim_position "$section" position
		[ "$modem" = "$sim_modem" ] && [ "$sim_position" -gt 1 ] && multisim="1"
	}
	config_load simcard
	config_foreach get_sim_count sim

	#Check sim position in simd if modem is registered as multisim
	[ "$multisim" = "1" ] && {
		echo "Quering active sim position"
		json_set_namespace gobinet old_cb
		json_load "$(ubus call $gsm_modem get_sim_slot)"
		json_get_var active_sim index
		json_set_namespace $old_cb
	}

	#Restart if check failed
	[ "$active_sim" -ge 1 ] && [ "$active_sim" -le 2 ] || return

	# check if current sim and interface sim match
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

#~ ---------------------------------------------------------------------

	[ -n "$ctl_device" ] && device="$ctl_device"
	[ -z "$timeout" ] && timeout="30"
	[ -z "$metric" ] && metric="1"

	#~ Find interface name
	devname="$(basename "$device")"
	devpath="$(readlink -f /sys/class/usbmisc/$devname/device/)"
	ifname="$(ls "$devpath"/net)"

	[ -n "$ifname" ] || {
		echo "The interface could not be found."
		proto_notify_error "$interface" NO_IFACE
		return 1
	}

#~ Connectivity part----------------------------------------------------
	net_device="$ifname"

	first_uqmi_call "uqmi -d $device --timeout 10000 --set-autoconnect disabled" || return 1

	echo "Using net device: $net_device"

	dataformat="$(uqmi -d "$device" $options --wda-get-data-format | \
	sed -n "s_\s*.Link Layer Protocol.: .\([a-z-]*\)._\1_p")"
	qmi_error_handle "$dataformat" "$error_cnt" "$modem" || return 1

	if [ "$dataformat" = 'raw-ip' ]; then

		[ -f "/sys/class/net/$ifname/qmi/raw_ip" ] || {
			echo "Device only supports raw-ip mode but is missing this required driver \
attribute: /sys/class/net/$ifname/qmi/raw_ip"
			proto_notify_error "$interface" NO_RAW_IP
			return 1
		}

		echo "Informing driver of raw-ip for $ifname .."
		ret=$(uqmi -d "$device" $options --set-expected-data-format raw-ip)
		qmi_error_handle "$ret" "$error_cnt" "$modem" || return 1

		echo "Y" > "/sys/class/net/${ifname}/qmi/raw_ip" 2>/dev/null
	fi

	uqmi_modify_data_format "qmi"
	[ $? -ne 0 ] && return 1

	pdptype="$(echo "$pdptype" | awk '{print tolower($0)}')"
	[ "$pdptype" = "ip" ] || [ "$pdptype" = "ipv6" ] || [ "$pdptype" = "ipv4v6" ] || pdptype="ip"

	[ "$deny_roaming" -ne "0" ] && deny_roaming="yes" || deny_roaming="no"

	apn="$(uci -q get network.$interface.apn)"
	echo "Starting network $interface using APN: $apn"

	auth="$(uci -q get network.$interface.auth)"
	[ -n "$auth" ] && [ "$auth" != "none" ] && {
		username="$(uci -q get network.$interface.username)"
		password="$(uci -q get network.$interface.password)"
	}

	cid="$(uqmi -d "$device" $options --get-client-id wds)"
	qmi_error_handle "$cid" "$error_cnt" "$modem" || return 1

#~ Do not add TABS!
call_uqmi_command "uqmi -d $device $options --set-client-id wds,$cid --release-client-id wds \
--modify-profile --profile-identifier 3gpp,${pdp} \
--profile-name profile${pdp} \
--roaming-disallowed-flag ${deny_roaming} \
${username:+ --username $username} \
${password:+ --password $password} \
${auth:+ --auth-type $auth} \
${apn:+ --apn $apn} \
${pdptype:+ --pdp-type $pdptype}"

	retry_before_reinit="$(cat /tmp/conn_retry_$interface)" 2>/dev/null
	[ -z "$retry_before_reinit" ] && retry_before_reinit="0"

	wait_for_serving_system || return 1

	[ "$pdptype" = "ip" ] || [ "$pdptype" = "ipv4v6" ] && {

		cid_4=$(call_uqmi_command "uqmi -d $device $options --get-client-id wds")
		[ $? -ne 0 ] && return 1

		check_digits $cid_4
		if [ $? -ne 0 ]; then
			echo "Unable to obtain client IPV4 ID"
		fi
		echo "cid4: $cid_4"

		#~ Set ipv4 on CID
		call_uqmi_command "uqmi -d $device $options --set-ip-family ipv4 \
--set-client-id wds,$cid_4"
		[ $? -ne 0 ] && return 1

		#~ Start PS call
		pdh_4=$(call_uqmi_command "uqmi -d $device $options --set-client-id wds,$cid_4 \
--start-network ${apn:+--apn $apn} --profile $pdp ${auth:+ --auth-type $auth --username $username \
--password $password}" "true")

		echo "pdh4: $pdh_4"

		check_digits $pdh_4
		if [ $? -ne 0 ]; then
		# pdh_4 is a numeric value on success
			echo "Unable to connect IPv4"
		else
			# Check data connection state
			connstat="$(uqmi -d $device $options --set-client-id wds,"$cid_4" \
					--get-data-status | awk -F '"' '{print $2}')"
			if [ "$connstat" = "connected" ]; then
				ipv4=1
			else
				echo "No IPV4 data link!"
			fi
			parameters4="$(uqmi -d $device $options --set-client-id wds,"$cid_4" \
--get-current-settings)"
			get_dynamic_mtu "$parameters4" "$ipv4" "$interface"
		fi
	}

	[ "$pdptype" = "ipv6" ] || [ "$pdptype" = "ipv4v6" ] && {

		cid_6=$(call_uqmi_command "uqmi -d $device $options --get-client-id wds")
		[ $? -ne 0 ] && return 1

		check_digits $cid_6
		if [ $? -ne 0 ]; then
			echo "Unable to obtain client IPV6 ID"
		fi
		echo "cid6: $cid_6"

		#~ Set ipv6 on CID
		ret=$(call_uqmi_command "uqmi -d $device $options --set-ip-family ipv6 \
--set-client-id wds,"$cid_6"")
		[ $? -ne 0 ] && return 1

		#~ Start PS call
		pdh_6=$(call_uqmi_command "uqmi -d $device $options --set-client-id wds,$cid_6 \
--start-network ${apn:+--apn $apn} --profile $pdp ${auth:+ --auth-type $auth --username $username \
--password $password}" "true")

		echo "pdh6: $pdh_6"

		# pdh_6 is a numeric value on success
		check_digits $pdh_6
		if [ $? -ne 0 ]; then
			echo "Unable to connect IPv6"
		else
			# Check data connection state
			connstat="$(uqmi -d $device $options --set-client-id wds,"$cid_6" \
					--get-data-status | awk -F '"' '{print $2}')"
			if [ "$connstat" = "connected" ]; then
				ipv6=1
			else
				echo "No IPV6 data link!"
			fi
			parameters6="$(uqmi -d $device $options --set-client-id wds,"$cid_6" \
--get-current-settings)"
			get_dynamic_mtu "$parameters6" "$ipv6" "$interface"
		fi
	}

	fail_timeout=$((delay+10))
	verify_data_connection "$pdptype" || return 1

	echo "Setting up $net_device"

	set_mtu "$ifname"

	proto_init_update "$ifname" 1
	proto_set_keep 1
	proto_add_data

	[ -n "$pdh_4" ] && [ "$ipv4" = "1" ] && {
		json_add_string "cid_4" "$cid_4"
		json_add_string "pdh_4" "$pdh_4"
	}

	[ -n "$pdh_6" ] && [ "$ipv6" = "1" ] && {
		json_add_string "cid_6" "$cid_6"
		json_add_string "pdh_6" "$pdh_6"
	}

	json_add_string "pdp" "$pdp"
	json_add_string "method" "$method"
	json_add_boolean "static_mobile" "1" # Required for mdcollect
	proto_close_data
	proto_send_update "$interface"

	local zone="$(fw3 -q network "$interface" 2>/dev/null)"

	[ "$method" = "bridge" ] || [ "$method" = "passthrough" ] && [ -n "$cid_4" ] && {
		setup_bridge_v4 "$ifname" "$modem"

		#Passthrough
		[ "$method" = "passthrough" ] && {
			iptables -w -tnat -I postrouting_rule -o "$net_device" -j SNAT --to "$bridge_ipaddr"
			ip route add default dev "$net_device"
		}
	}

	[ "$method" != "bridge" ] && [ "$method" != "passthrough" ] && \
	[ -n "$pdh_4" ] && [ "$ipv4" = "1" ] && {
		if [ "$dhcp" = 0 ]; then
			setup_static_v4 "$net_device"
		else
			setup_dhcp_v4 "$net_device"
		fi

		IFACE4="${interface}_4"
		ubus_set_interface_data "$modem" "$sim" "$zone" "$IFACE4"
	}

	[ -n "$pdh_6" ] && [ "$ipv6" = "1" ] && {
		if [ "$dhcpv6" = 0 ]; then
			setup_static_v6 "$net_device"
		else
			setup_dhcp_v6 "$net_device"
		fi

		IFACE6="${interface}_6"
		ubus_set_interface_data "$modem" "$sim" "$zone" "$IFACE6"
	}

	[ "$ipv4" != "1" ] && [ "$pdptype" = "ipv4v6" ] && proto_notify_error "$interface" "$pdh_4"
	[ "$ipv6" != "1" ] && [ "$pdptype" = "ipv4v6" ] && proto_notify_error "$interface" "$pdh_6"

	#cid is lost after script shutdown so we should create temp files for that
	mkdir -p "/var/run/qmux/"
	echo "$cid_4" > "/var/run/qmux/$interface.cid_4"
	echo "$cid_6" > "/var/run/qmux/$interface.cid_6"

	proto_export "IFACE4=$IFACE4"
	proto_export "IFACE6=$IFACE6"
	proto_run_command "$interface" qmuxtrack "$device" "$cid_4" "$cid_6"
}
#~ ---------------------------------------------------------------------
proto_qmi_teardown() {
	#~ netifd has 15 seconds timeout to finish this function
	#~ it is killed if not finished
	#~ Slow routers can fail to finish teardown
	#~ We need to put some time taking actions to background
	local interface="$1"
	local conn_proto
	local device bridge_ipaddr method
	json_get_vars device

	[ -n "$ctl_device" ] && device="$ctl_device"

	echo "Stopping network $interface"

	json_load "$(ubus call network.interface.$interface status)"
	json_select data
	json_get_vars method bridge_ipaddr

	conn_proto="qmux"

	clear_connection_values $interface $device "4" $conn_proto
	ubus call network.interface down "{\"interface\":\"${interface}_4\"}"

	clear_connection_values $interface $device "6" $conn_proto
	ubus call network.interface down "{\"interface\":\"${interface}_6\"}"

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

	proto_init_update "*" 0
	proto_send_update "$interface"
}

[ -n "$INCLUDE_ONLY" ] || {
	add_protocol qmi
}
