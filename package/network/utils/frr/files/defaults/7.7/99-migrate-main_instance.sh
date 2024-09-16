#!/bin/sh

. /lib/functions.sh

main_to_general(){
	local section="$1"
	local instance

	config_get "instance" "$section" "instance"
	if [ "$instance" = "main_instance" ]; then
		uci_set "bgp" "$section" "instance" "general"
	fi
}

check_map_filter(){
	local section="$1"

	config_get "instance" "$section" "instance"
	[ -n "$instance" ] && return
	uci_set "bgp" "$section" "instance" "general"
}

if uci_get "bgp" "main_instance" 1>/dev/null; then
	uci_rename "bgp" "main_instance" "general"
	uci_commit "bgp"
fi

config_load bgp
config_foreach main_to_general
config_foreach check_map_filter bgp_route_map_filters
uci_commit "bgp"
