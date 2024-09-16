#!/bin/ash

boot_target_pre_board_detect() {
	local param="$(/sbin/mnf_info --name)"
	local router_name="${param:0:6}"

	[ "$router_name" = "RUTX08" ] || [ "$router_name" = "RUTX09" ] && \
		rm /etc/modules.d/ath10k
}
