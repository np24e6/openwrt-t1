#!/bin/sh

hotspot_hook() {
	local log_file="${PACK_DIR}hotspot.log"

	troubleshoot_init_log "Sessions" "$log_file"
	local state="$(ubus -v list chilli 2>/dev/null | grep list)"
	[ -z "$state" ] || troubleshoot_add_log "$(ubus call chilli list)" "$log_file"

	troubleshoot_init_log "Logs" "$log_file"
	local response="$(/sbin/api GET /hotspot/logs/status)"
	[ -z "$response" ] && return
	local success="$(echo "$response" | jsonfilter -e '@.http_body.success')"
	[ "$success" != "true" ] && return

	troubleshoot_add_log "$(echo "$response" | jsonfilter -e '@.http_body.data')" "$log_file"
}

troubleshoot_hook_init hotspot_hook