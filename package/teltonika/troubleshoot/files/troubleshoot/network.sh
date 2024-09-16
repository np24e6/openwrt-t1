#!/bin/sh

network_hook() {
	local log_file="${PACK_DIR}network.log"

	is_executable "netstat" && {
		troubleshoot_init_log "ACTIVE CONNECTIONS" "$log_file"
		troubleshoot_add_log "$(netstat -tupan 2>/dev/null)" "$log_file"
	}

	troubleshoot_init_log "IP RULES" "$log_file"
	troubleshoot_add_log "$(ip rule)" "$log_file"
	troubleshoot_init_log "IP ROUTES" "$log_file"
	troubleshoot_add_log "$(ip route show table all)" "$log_file"

	is_executable "ipsec" && {
		troubleshoot_init_log "IPSEC STATUS" "$log_file"
		troubleshoot_add_log_ext "ipsec" "statusall" "$log_file"
	}
	is_executable "mwan3" && {
		troubleshoot_init_log "MULTIWAN STATUS" "$log_file"
		troubleshoot_add_log_ext "mwan3" "status" "$log_file"
	}

}

firewall_hook() {
	local log_file="${PACK_DIR}firewall.log"

	is_executable "iptables" && {
		troubleshoot_init_log "IPtables FILTER" "$log_file"
		troubleshoot_add_log_ext "ip" "tun" "$log_file"
		troubleshoot_add_log "$(iptables -L -nv)" "$log_file"

		troubleshoot_init_log "IPtables NAT" "$log_file" "$log_file"
		troubleshoot_add_log "$(iptables -t nat -L -nv)" "$log_file"

		troubleshoot_init_log "IPtables MANGLE" "$log_file" "$log_file"
		troubleshoot_add_log "$(iptables -t mangle -L -nv)" "$log_file"

		troubleshoot_add_log "$(iptables-save)" "$log_file"
	}

	is_executable "ebtables" && {
		troubleshoot_init_log "EBtables FILTER" "$log_file"
		troubleshoot_add_log_ext "ip" "tun" "$log_file"
		troubleshoot_add_log "$(ebtables -t filter -L --Lc)" "$log_file"

		troubleshoot_init_log "EBtables NAT" "$log_file"
		troubleshoot_add_log "$(ebtables -t nat -L --Lc)" "$log_file"

		troubleshoot_init_log "EBtables BROUTE" "$log_file"
		troubleshoot_add_log "$(ebtables -t broute -L --Lc)" "$log_file"
	}

	troubleshoot_init_log "NF Conntrack entry count" "$log_file"
	troubleshoot_add_log "$(wc -l /proc/net/nf_conntrack | awk '{print $1}')" "$log_file"
	troubleshoot_init_log "NF Conntrack first entries (up to 1000)" "$log_file"
	troubleshoot_add_log "$(head -1000 /proc/net/nf_conntrack)" "$log_file"
}

interfaces_hook() {
	local log_file="${PACK_DIR}network.log"
	local dhcp_list_data

	troubleshoot_init_log "Interfaces" "$log_file"
	{
		for iface in /sys/class/net/*; do
			ifconfig "$(basename "$iface")"
			ip a sh dev "$(basename "$iface")"
			printf "\n\n"
		done
	} >>"$log_file" 2>/dev/null

	troubleshoot_init_log "Tunnels" "$log_file"
	troubleshoot_add_log "$(ip tun)" "$log_file"

	is_executable "brctl" && {
		troubleshoot_init_log "Bridges" "$log_file"
		troubleshoot_add_log "$(brctl show)" "$log_file"
	}

	troubleshoot_init_log "Routing table" "$log_file"
	troubleshoot_add_log "$(route -n -e)" "$log_file"

	troubleshoot_init_log "DHCP leases" "$log_file"
	[ -f "/tmp/dhcp.leases" ] && dhcp_list_data=$(cat /tmp/dhcp.leases)
	troubleshoot_add_log "${dhcp_list_data:-no DHCP leases..}" "$log_file"

	troubleshoot_init_log "ARP Data" "$log_file"
	troubleshoot_add_log "$(cat /proc/net/arp)" "$log_file"
}

vpns_hook() {
	local log_file="${PACK_DIR}vpns.log"

	is_executable "wg" && local wg_show="$(wg show)"
	[ -n "$wg_show" ] && {
		troubleshoot_init_log "Wireguard Status" "$log_file"
		troubleshoot_add_log "$wg_show" "$log_file"
	}
}

troubleshoot_hook_init interfaces_hook
troubleshoot_hook_init network_hook
troubleshoot_hook_init firewall_hook
troubleshoot_hook_init vpns_hook
