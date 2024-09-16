#!/bin/sh
# source jshn shell library
. /usr/share/libubox/jshn.sh

INSTANCE=$(echo "$config" | cut -d"-" -f2 | cut -d"." -f1)
TYPE=$(uci get openvpn."$INSTANCE".type)
STATUS_FILE="/tmp/state/openvpn-$INSTANCE.info"

arping_net() {
	local tun_dev="$1"
	local net="$2"
	local start="$3"
	local end="$4"
	for i in $(seq "$start" "$end"); do
		{ sleep 3 && arping -I "$tun_dev" -c1 -w1 "${net}.${i}" >/dev/null 2>&1 & }
	done
}

case $script_type in
	up)
		[ "$TYPE" = "client" ] && {
			env | sed -n -e "
			/^foreign_option_.*=dhcp-option.*DNS /s//server=/p
			/^foreign_option_.*=dhcp-option.*DOMAIN /s//domain=/p
			" | sort -u > /tmp/dnsmasq.d/$dev.dns
			echo "strict-order" >> /tmp/dnsmasq.d/$dev.dns
		}
		# generating json data
		json_init
		json_add_string "name" "$INSTANCE"
		json_add_string "ip" "$ifconfig_local"
		json_add_string "ipv6" "$ifconfig_ipv6_local"
		[ "$TYPE" = "client" ] && {
			json_add_string "ip_remote" "$ifconfig_remote"
			json_add_string "ipv6_remote" "$ifconfig_ipv6_remote"
		}
		json_add_string "time" "$(awk '{print $1}' /proc/uptime)"
		json_dump > "$STATUS_FILE"
		if [ "$dev" != "${dev/tun_c/}" ]; then
			if [ -n "$route_network_1" ]; then
				i=1
				route_network="$route_network_1"
				while [ -n "$route_network" ]; do
					[ "${route_network##*.}" = "0" ] && route_network="${route_network%.*}.1"
					{ sleep 3 && ping -c1 -W1 -I "$dev" "$route_network" >/dev/null 2>&1; } &
					i=$(( i+1 ))
					eval "route_network=\$route_network_$i"
				done
			elif [ -n "$ifconfig_remote" ]; then
				{ sleep 3 && ping -c1 -W1 -I "$dev" "$ifconfig_remote" >/dev/null 2>&1; } &
			fi
		elif [ "$dev" != "${dev/tap_c/}" ]; then
			bridge="$(uci -q get openvpn.${INSTANCE}.to_bridge)"
			if [ -n "$bridge" ] && [ "$bridge" != "none" ]; then
				dev_bridge="$(uci -q get network.${bridge}.name)"
				for ip in $(ip -f inet addr show "$dev_bridge" | awk '/inet / {print $2}'); do
					eval "$(ipcalc.sh "$ip")"
					if [ "$PREFIX" -lt "24" ]; then
						arping_net "$dev" "${IP%.*}" "1" "254"
					else
						arping_net "$dev" "${IP%.*}" "${NETWORK##*.}" "${BROADCAST##*.}"
					fi
				done
			fi
		fi
		;;
	down)
		[ "$TYPE" = "client" ] && rm /tmp/dnsmasq.d/$dev.dns 2> /dev/null
		rm "$STATUS_FILE" 2> /dev/null
		;;
esac
[ "$TYPE" = "client" ] && /etc/init.d/dnsmasq reload &
