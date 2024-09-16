#!/bin/sh

. /lib/functions.sh

if uci_get "bgp" "main_instance" 1>/dev/null; then
	uci_rename "bgp" "main_instance" "general"
	uci_commit "bgp"
fi
