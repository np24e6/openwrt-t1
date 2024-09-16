#!/bin/sh

[ -x /usr/sbin/xl2tpd ] || exit 0

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

proto_l2tp_init_config() {
	proto_config_add_string "username"
	proto_config_add_string "password"
	proto_config_add_string "keepalive"
	proto_config_add_string "pppd_options"
	proto_config_add_boolean "ipv6"
	proto_config_add_int "mtu"
	proto_config_add_int "checkup_interval"
	proto_config_add_string "server"
	proto_config_add_boolean "defaultroute"
	available=1
	no_device=1
	no_proto_task=1
	teardown_on_l3_link_down=1
}

check_bind() {
        config_get bind_to "$1" bind_to
                                       
        [ "$bind_to" = "$2" ] || continue
                                         
        [ -z "$(ipsec status ${1%%_*}-${1} | grep "1 up")" ] && BIND_STOP=1
}                                                                          

proto_l2tp_setup() {
	local interface="$1"
	local optfile="/tmp/l2tp/options.${interface}"
	local static_opt="/etc/ppp/options.l2tp"
	local ip serv_addr server host defaultroute

	json_get_vars server defaultroute

	BIND_STOP=0                                                        
	config_load ipsec         
	config_foreach check_bind connection "$interface"
	[ "$BIND_STOP" -eq 1 ] && exit 1

	host="${server%:*}"
	mkdir -p /tmp/l2tp
	[ "$defaultroute" -eq 1 ] && echo "$interface" > "/tmp/l2tp/default-status"

	for ip in $(resolveip -t 5 "$host"); do
		[ "$defaultroute" -eq 1 ] && {
			default="$(ip route show default | head -n 1 | awk -F"dev " '{print $2}' | sed 's/\s.*$//')"
			gw="$(ip route show default dev $default | head -n 1 | awk -F"via " '{print $2}' | sed 's/\s.*$//')"
			metric="$(ip route show default dev $default | awk -F"metric " '{print $2}' | sed 's/\s.*$//')"

			[ -n "$gw" ] && ip route add "$ip" via "$gw" dev "$default" metric "$metric" || ip route add "$ip" dev "$default" metric "$metric"
			echo "$default" >> /tmp/l2tp/default-status
		}
		serv_addr=1
	done

	[ -n "$serv_addr" ] || {
		echo "Could not resolve server address" >&2
		sleep 5
		proto_setup_failed "$interface"
		exit 1
	}

	# Start and wait for xl2tpd
	if [ ! -p /var/run/xl2tpd/l2tp-control -o -z "$(pidof xl2tpd)" ]; then
		/etc/init.d/xl2tpd restart

		local wait_timeout=0
		while [ ! -p /var/run/xl2tpd/l2tp-control ]; do
			wait_timeout=$(($wait_timeout + 1))
			[ "$wait_timeout" -gt 5 ] && {
				echo "Cannot find xl2tpd control file." >&2
				proto_setup_failed "$interface"
				exit 1
			}
			sleep 1
		done
	fi

	local ipv6 demand keepalive username password pppd_options mtu
	json_get_vars ipv6 demand keepalive username password pppd_options mtu
	[ "$ipv6" = 1 ] || ipv6=""
	if [ "${demand:-0}" -gt 0 ]; then
		demand="precompiled-active-filter /etc/ppp/filter demand idle $demand"
	else
		demand="persist"
	fi

	local interval="${keepalive##*[, ]}"
	[ "$interval" != "$keepalive" ] || interval=5

	keepalive="${keepalive:+lcp-echo-interval $interval lcp-echo-failure ${keepalive%%[, ]*}}"
	username="${username:+user \"$username\" password \"$password\"}"
	ipv6="${ipv6:++ipv6}"
	mtu="${mtu:+mtu $mtu mru $mtu}"

	config_load network
	config_get auth_pap "$interface" auth_pap "0"
	[ "$auth_pap" = "0" ] && allow_pap="refuse-pap"
	config_get auth_chap "$interface" auth_chap "1"
	[ "$auth_chap" = "0" ] && allow_chap="refuse-chap"
	config_get auth_mschap2 "$interface" auth_mschap2 "1"
	[ "$auth_mschap2" = "0" ] && allow_mschap2="refuse-mschap-v2"

	cat "$static_opt" > "$optfile"
	cat <<EOF >>"$optfile"
ipparam "$interface"
ifname "l2tp-$interface"
$keepalive
$username
$ipv6
$mtu
$pppd_options
$allow_pap
$allow_chap
$allow_mschap2
EOF

	xl2tpd-control add-lac l2tp-${interface} pppoptfile=${optfile} lns=${server} || {
		echo "xl2tpd-control: Add l2tp-$interface failed" >&2
		proto_setup_failed "$interface"
		exit 1
	}
	xl2tpd-control connect-lac l2tp-${interface} || {
		echo "xl2tpd-control: Connect l2tp-$interface failed" >&2
		proto_setup_failed "$interface"
		exit 1
	}
}

proto_l2tp_teardown() {
	local interface="$1"
	local optfile="/tmp/l2tp/options.${interface}"

	rm -f ${optfile}
	if [ -p /var/run/xl2tpd/l2tp-control ]; then
		xl2tpd-control remove-lac l2tp-${interface} || {
			echo "xl2tpd-control: Remove l2tp-$interface failed" >&2
		}
	fi
	# Wait for interface to go down
        while [ -d /sys/class/net/l2tp-${interface} ]; do
		sleep 1
	done

	[ -f "/tmp/l2tp/default-status" ] && rm "/tmp/l2tp/default-status" >/dev/null 2>&1
}

[ -n "$INCLUDE_ONLY" ] || {
	add_protocol l2tp
}
