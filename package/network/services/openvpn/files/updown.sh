#!/bin/sh
# source jshn shell library
. /usr/share/libubox/jshn.sh

INSTANCE=$(echo "$config" | cut -d"-" -f2 | cut -d"." -f1)
TYPE=$(uci get openvpn."$INSTANCE".type)
STATUS_FILE="/tmp/state/openvpn-$INSTANCE.info"

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
		[ "$TYPE" = "client" ] && json_add_string "ip_remote" "$ifconfig_remote"
		json_add_string "time" "$daemon_start_time"
		json_dump > "$STATUS_FILE"
		;;
	down)
		[ "$TYPE" = "client" ] && rm /tmp/dnsmasq.d/$dev.dns 2> /dev/null
		rm "$STATUS_FILE" 2> /dev/null
		rm /var/run/openvpn."$INSTANCE".status 2> /dev/null
		;;
esac
[ "$TYPE" = "client" ] && /etc/init.d/dnsmasq reload &
