#!/bin/sh

. /lib/functions.sh

[ -f "/etc/config/frr" ] || exit 0

add_ebgp() {
	local section="$1"

	uci_set "frr" "$section" "ebgp_requires_policy" 0
}

get_ospf_area() {
	local section="$1"
	local network_area="$2"
	local network_name="$3"
	local area_area

	config_get area_area "$section" area
	[ "$area_area" = "$network_area" ] && uci_set "frr" "$network_name" "area" "$section"
}

fix_ospf_area() {
	local network_area network_name
	local section="$1"

	config_get network_area "$section" area
	config_foreach get_ospf_area ospf_area "$network_area" "$section"
}

fix_bgp_maps() {
	local metric
	local sequence
	local section="$1"
	config_get metric "$section" metric
	config_get sequence "$section" sequence
	if [ -n "$metric" ] && [ -z "$sequence" ]; then
		uci_rename "frr" "$section" "metric" "sequence"
	fi
}

# migrate legacy
[ -f "/etc/config/quagga" ] && mv "/etc/config/quagga" "/etc/config/frr"

config_load frr
config_foreach add_ebgp bgp_instance
config_foreach fix_ospf_area ospf_network
config_foreach fix_bgp_maps route_maps
uci_rename "frr" "rip" "bgpd_custom_conf" "ripd_custom_conf" 2>/dev/null
uci_commit frr
