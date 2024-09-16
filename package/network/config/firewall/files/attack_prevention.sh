#! /bin/sh
. /lib/functions.sh

add_limit_rules() {
	config_get limit "$1" "limit"
	config_get limit_burst "$1" "limit_burst"
	config_get name "$1" "name"

	[ -n "$limit" ] || return

	[ "$name" = "Allow-Ping" ] && {
		iptables -I INPUT -p icmp -m icmp --icmp-type 8 -m comment --comment "!fw3: Reject ICMP" -j REJECT
		iptables -I INPUT -p icmp -m icmp --icmp-type 8 -m limit --limit $limit --limit-burst $limit_burst -m comment --comment "!fw3: Limit ICMP" -j ACCEPT
	}
}

add_rules(){
	local path port_scan hitcount syn_fin syn_rst x_max nmap_fin null_flags
	config_get path "$1" path

	[ "$path" = "/usr/bin/attack_prevention" ] || return

	config_get port_scan    "$1" port_scan  "0"
	config_get seconds      "$1" seconds    "30"
	config_get hitcount     "$1" hitcount   "10"
	config_get syn_fin      "$1" syn_fin    "0"
	config_get syn_rst      "$1" syn_rst    "0"
	config_get x_max        "$1" x_max      "0"
	config_get nmap_fin     "$1" nmap_fin   "0"
	config_get null_flags   "$1" null_flags "0"

	iptables -t filter --list zone_port_scan > /dev/null 2>&1
	ret="$?"
	[ "$ret" -eq 1 ] && [ "$port_scan" -eq 1 ] && {
		iptables -N zone_port_scan
		iptables -A zone_port_scan -p tcp -m conntrack --ctstate NEW -m recent --set
		iptables -A zone_port_scan -p tcp -m conntrack --ctstate NEW -m recent --update --seconds "$seconds" --hitcount "$hitcount" -j DROP
		iptables -t filter -I zone_wan_input -j zone_port_scan
		iptables -t filter -I zone_wan_forward -j zone_port_scan
	}
	[ "$ret" -eq 0 ] && [ "$port_scan" -eq 0 ] && {
		iptables -D zone_port_scan -p tcp -m conntrack --ctstate NEW -m recent --set
		iptables -D zone_port_scan -p tcp -m conntrack --ctstate NEW -m recent --update --seconds "$seconds" --hitcount "$hitcount" -j DROP
		iptables -t filter -D zone_wan_input -j zone_port_scan
		iptables -t filter -D zone_wan_forward -j zone_port_scan
		iptables -X zone_port_scan
	}

	iptables -t raw -C PREROUTING -p tcp --tcp-flags FIN,SYN FIN,SYN -j DROP
	ret="$?"
	[ "$ret" -eq 1 ] && [ "$syn_fin" -eq 1 ] && {
		iptables -t raw -I PREROUTING -p tcp --tcp-flags FIN,SYN FIN,SYN -j DROP
	}
	[ "$ret" -eq 0 ] && [ "$syn_fin" -eq 0 ] && {
		iptables -t raw -D PREROUTING -p tcp --tcp-flags FIN,SYN FIN,SYN -j DROP
	}

	iptables -t raw -C PREROUTING -p tcp --tcp-flags SYN,RST SYN,RST -j DROP
	ret="$?"
	[ "$ret" -eq 1 ] && [ "$syn_rst" -eq 1 ] && {
		iptables -t raw -I PREROUTING -p tcp --tcp-flags SYN,RST SYN,RST -j DROP
	}
	[ "$ret" -eq 0 ] && [ "$syn_rst" -eq 0 ] && {
		iptables -t raw -D PREROUTING -p tcp --tcp-flags SYN,RST SYN,RST -j DROP
	}

	iptables -t raw -C PREROUTING -p tcp --tcp-flags FIN,SYN,RST,PSH,ACK,URG FIN,PSH,URG -j DROP
	ret="$?"
	[ "$ret" -eq 1 ] && [ "$x_max" -eq 1 ] && {
		iptables -t raw -I PREROUTING -p tcp --tcp-flags FIN,SYN,RST,PSH,ACK,URG FIN,PSH,URG -j DROP
	}
	[ "$ret" -eq 0 ] && [ "$x_max" -eq 0 ] && {
		iptables -t raw -D PREROUTING -p tcp --tcp-flags FIN,SYN,RST,PSH,ACK,URG FIN,PSH,URG -j DROP
	}

	iptables -t raw -C PREROUTING -p tcp --tcp-flags FIN,SYN,RST,PSH,ACK,URG FIN -j DROP
	ret="$?"
	[ "$ret" -eq 1 ] && [ "$nmap_fin" -eq 1 ] && {
		iptables -t raw -I PREROUTING -p tcp --tcp-flags FIN,SYN,RST,PSH,ACK,URG FIN -j DROP
	}
	[ "$ret" -eq 0 ] && [ "$nmap_fin" -eq 0 ] && {
		iptables -t raw -D PREROUTING -p tcp --tcp-flags FIN,SYN,RST,PSH,ACK,URG FIN -j DROP
	}

	iptables -t raw -C PREROUTING -p tcp --tcp-flags FIN,SYN,RST,PSH,ACK,URG NONE -j DROP
	ret="$?"
	[ "$ret" -eq 1 ] && [ "$null_flags" -eq 1 ] && {
		iptables -t raw -I PREROUTING -p tcp --tcp-flags FIN,SYN,RST,PSH,ACK,URG NONE -j DROP
	}
	[ "$ret" -eq 0 ] && [ "$null_flags" -eq 0 ] && {
		iptables -t raw -D PREROUTING -p tcp --tcp-flags FIN,SYN,RST,PSH,ACK,URG NONE -j DROP
	}

}

config_load firewall
config_foreach add_rules include
config_foreach add_limit_rules rule
