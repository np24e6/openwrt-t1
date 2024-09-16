#!/bin/sh

hotspot_hook() {
	local log_file="${PACK_DIR}hotspot.log"

	troubleshoot_init_log "Sessions" "$log_file"
	state="$(ubus -v list chilli 2>/dev/null | grep list)"
	[ -z "$state"] || troubleshoot_add_log "$(ubus call chilli list)" "$log_file"

	troubleshoot_init_log "Logs" "$log_file"
	troubleshoot_add_log "$(ubus call vuci.services.hotspot log)" "$log_file"
}

troubleshoot_hook_init hotspot_hook