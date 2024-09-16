#!/bin/sh
# Copyright 2016-2017 Dan Luedtke <mail@danrl.com>
# Licensed to the public under the Apache License 2.0.

DEFAULT_ROUTE=0
DEFAULT_STATUS="/tmp/wireguard/default-status"

WG=/usr/bin/wg
if [ ! -x $WG ]; then
	logger -t "wireguard" "error: missing wireguard-tools (${WG})"
	exit 0
fi

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. /lib/functions/network.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

proto_wireguard_init_config() {
	proto_config_add_string "private_key"
	proto_config_add_int "listen_port"
	proto_config_add_int "mtu"
	proto_config_add_string "fwmark"
	proto_config_add_string "tunlink"
	available=1
	no_proto_task=1
}

proto_wireguard_setup_peer() {
	local peer_config="$1"

	local public_key
	local preshared_key
	local allowed_ips
	local route_allowed_ips
	local endpoint_host
	local endpoint_port
	local persistent_keepalive
	local table
	local tunlink

	config_get public_key "${peer_config}" "public_key"
	config_get preshared_key "${peer_config}" "preshared_key"
	config_get allowed_ips "${peer_config}" "allowed_ips"
	config_get_bool route_allowed_ips "${peer_config}" "route_allowed_ips" 0
	config_get endpoint_host "${peer_config}" "endpoint_host"
	config_get endpoint_port "${peer_config}" "endpoint_port"
	config_get persistent_keepalive "${peer_config}" "persistent_keepalive"
	config_get table "${peer_config}" "table"
	config_get tunlink "${peer_config}" "tunlink"

	if [ -z "$public_key" ]; then
		echo "Skipping peer config $peer_config because public key is not defined."
		return 0
	fi

	echo "[Peer]" >> "${wg_cfg}"
	echo "PublicKey=${public_key}" >> "${wg_cfg}"
	if [ "${preshared_key}" ]; then
		echo "PresharedKey=${preshared_key}" >> "${wg_cfg}"
	fi
	for allowed_ip in $allowed_ips; do
		echo "AllowedIPs=${allowed_ip}" >> "${wg_cfg}"
		if [ "${route_allowed_ips}" != 0 ] || [ "$tunlink" != "any" ]
		then
			DEFAULT_ROUTE=1
			echo -n "$peer_config " >> "$DEFAULT_STATUS"
		fi
	done
	if [ "${endpoint_host}" ]; then
		case "${endpoint_host}" in
			*:*)
				endpoint="[${endpoint_host}]"
				;;
			*)
				endpoint="${endpoint_host}"
				;;
		esac
		if [ "${endpoint_port}" ]; then
			endpoint="${endpoint}:${endpoint_port}"
		else
			endpoint="${endpoint}:51820"
		fi
		echo "Endpoint=${endpoint}" >> "${wg_cfg}"
	fi
	if [ "${persistent_keepalive}" ]; then
		echo "PersistentKeepalive=${persistent_keepalive}" >> "${wg_cfg}"
	fi

	if [ ${route_allowed_ips} -ne 0 ]; then
		for allowed_ip in ${allowed_ips}; do
			if [ -n "$table" ];then
				case "${allowed_ip}" in
					*:*/*)
						proto_add_ipv6_route "${allowed_ip%%/*}" "${allowed_ip##*/}" "" "" "" "" "$table"
						;;
					*.*/*)
						proto_add_ipv4_route "${allowed_ip%%/*}" "${allowed_ip##*/}" "" "" "" "$table"
						;;
					*:*)
						proto_add_ipv6_route "${allowed_ip%%/*}" "128" "" "" "" "" "$table"
						;;
					*.*)
						proto_add_ipv4_route "${allowed_ip%%/*}" "32" "" "" "" "$table"
						;;
				esac
			else
				case "${allowed_ip}" in
					*:*/*)  
						proto_add_ipv6_route "${allowed_ip%%/*}" "${allowed_ip##*/}"
						;;
					*.*/*)
						proto_add_ipv4_route "${allowed_ip%%/*}" "${allowed_ip##*/}"
						;;
					*:*)
						proto_add_ipv6_route "${allowed_ip%%/*}" "128"
						;;
					*.*)
						proto_add_ipv4_route "${allowed_ip%%/*}" "32"
						;;
				esac
			fi
		done
	fi
}

proto_wireguard_setup() {
	local config="$1"
	local DEFAULT_STATUS="/tmp/wireguard/default-status_${config}"
	local wg_dir="/tmp/wireguard"
	local wg_cfg="${wg_dir}/${config}"

	local private_key
	local listen_port
	local mtu wan_interface external_mtu
	local iter=0

	config_load network
	config_get private_key "${config}" "private_key"
	config_get listen_port "${config}" "listen_port"
	config_get addresses "${config}" "addresses"
	config_get mtu "${config}" "mtu"
	config_get fwmark "${config}" "fwmark"
	config_get ip6prefix "${config}" "ip6prefix"
	config_get nohostroute "${config}" "nohostroute"

	ip link del dev "${config}" 2>/dev/null
	ip link add dev "${config}" type wireguard
	[ -e "/etc/hotplug.d/iface/18-wireguard" ] || ln -s /usr/share/wireguard/18-wireguard /etc/hotplug.d/iface/18-wireguard

	if [ -z "${mtu}" ]; then
		wan_interface="$(ip route show default | awk -F 'dev' '{print $2}' | awk 'NR==1 {print $1}')"
		if [ -n "$wan_interface" ];then
			external_mtu=$(cat /sys/class/net/${wan_interface}/mtu)
			mtu=$(( external_mtu - 80 ))
		else
			mtu=1420
		fi
	fi

	if [ -n "${mtu}" ]; then
		ip link set mtu "${mtu}" dev "${config}"
	fi

	proto_init_update "${config}" 1

	umask 077
	mkdir -p "${wg_dir}"
	echo "[Interface]" > "${wg_cfg}"
	echo "PrivateKey=${private_key}" >> "${wg_cfg}"
	if [ "${listen_port}" ]; then
		echo "ListenPort=${listen_port}" >> "${wg_cfg}"
	fi
	if [ "${fwmark}" ]; then
		echo "FwMark=${fwmark}" >> "${wg_cfg}"
	fi
	config_foreach proto_wireguard_setup_peer "wireguard_${config}"

	# apply configuration file
	${WG} setconf ${config} "${wg_cfg}"
	WG_RETURN=$?

	rm -f "${wg_cfg}"

	if [ ${WG_RETURN} -ne 0 ]; then
		sleep 5
		proto_setup_failed "${config}"
		exit 1
	fi

	for address in ${addresses}; do
		case "${address}" in
			*:*/*)
				proto_add_ipv6_address "${address%%/*}" "${address##*/}"
				;;
			*.*/*)
				proto_add_ipv4_address "${address%%/*}" "${address##*/}"
				;;
			*:*)
				proto_add_ipv6_address "${address%%/*}" "128"
				;;
			*.*)
				proto_add_ipv4_address "${address%%/*}" "32"
				;;
		esac
	done

	for prefix in ${ip6prefix}; do
		proto_add_ipv6_prefix "$prefix"
	done

	# endpoint dependency
	if [ "${nohostroute}" != "1" ] && [ "$DEFAULT_ROUTE" -eq 1 ]; then
		echo "" >> "$DEFAULT_STATUS"
		wg show "${config}" endpoints | \
		sed -E 's/\[?([0-9.:a-f]+)\]?:([0-9]+)/\1 \2/' | \
		while IFS=$'\t ' read -r key address port; do
			[ -n "${port}" ] || continue
			iter=$(($iter + 1))
			echo "$config" >> "$DEFAULT_STATUS"
			peer_config="$(cat "$DEFAULT_STATUS" | sed '1q;d' | awk -v col="$iter" '{print $col}')"
			config_get peer "$peer_config" endpoint_host
			config_get tunlink "$peer_config" tunlink
			defaults="$(ip route show default | grep -v tata | awk -F"dev " '{print $2}' | sed 's/\s.*$//')"
			for dev in ${defaults}; do
				metric="$(ip route show default dev $dev | awk -F"metric " '{print $2}' | sed 's/\s.*$//')"
				gw="$(ip route show default dev $dev | awk -F"via " '{print $2}' | sed 's/\s.*$//')"
				for ip in $(resolveip -4 "$peer"); do
					if [ -n "$tunlink" ] && [ "$tunlink" != "any" ]; then
						network_get_device tunlink_dev $tunlink
						[ "$tunlink_dev" = "wwan0" ] && { network_get_device tunlink_dev ${tunlink}_4 || network_get_device tunlink_dev ${tunlink}_6; }
						tunlink_gw="$(ip route show default dev $tunlink_dev | awk -F"via " '{print $2}' | sed 's/\s.*$//')"
						if ip route add "$ip" ${tunlink_gw:+via "$tunlink_gw"} dev "$tunlink_dev" metric "1" &>/dev/null; then
							echo "ip route del "$ip" dev "$tunlink_dev" metric 1" >> "$DEFAULT_STATUS"
						fi
						continue
					fi
					if ip route add "$ip" ${gw:+via "$gw"} dev "$dev" metric "$metric"; then
						echo "ip route del "$ip" dev "$dev" metric $metric" >> "$DEFAULT_STATUS"
					fi
				done
			done
		done
	fi

	proto_send_update "${config}"
}

count_sections(){
	config_get proto "$1" proto
	config_get disabled "$1" disabled
	[ "$proto" = "wireguard" ] && [ "$disabled" != "1" ] && count=$(($count+1))
}

proto_wireguard_teardown() {
	local config="$1" count=0
	local DEFAULT_STATUS="${DEFAULT_STATUS}_$config"
	config_load network
	config_foreach count_sections interface
	[ "$count" = 0 ] && rm /etc/hotplug.d/iface/18-wireguard &>/dev/null
	ip link del dev "${config}" >/dev/null 2>&1	
	if [ -e "$DEFAULT_STATUS" ]; then
		while read -r line; do
			if echo "$line" | grep -q "ip route del"; then
				eval "$line"
			fi
		done < "$DEFAULT_STATUS"
		rm "$DEFAULT_STATUS" >/dev/null 2>&1
	fi
}

[ -n "$INCLUDE_ONLY" ] || {
	add_protocol wireguard
}
