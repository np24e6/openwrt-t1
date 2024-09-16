#!/bin/sh

data_sender_hook() {
	local log_file="${PACK_DIR}data_sender.log"

    ubus list datasender.collection.* &>/dev/null || return 0

    troubleshoot_init_log "Data sender" "$log_file"
    for collection in $(ubus list datasender.collection.* 2>/dev/null); do
        troubleshoot_add_log "$(ubus -t 5 call $collection dump '{"verbose": false}')" "$log_file"
    done
}

troubleshoot_hook_init data_sender_hook
