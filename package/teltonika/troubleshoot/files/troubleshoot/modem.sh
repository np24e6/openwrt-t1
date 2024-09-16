#!/bin/sh

top_priority_methods="get_firmware get_iccid get_serial get_sim_slot get_pin_count get_pin_state get_temperature \
get_func get_network_info get_signal_query get_serving_cell get_neighbour_cells get_net_reg_stat \
get_net_greg_stat get_net_ereg_stat get_net_5greg_stat get_service_provider get_mbn_list get_ps_att_state \
get_pdp_ctx_list get_active_pdp_ctx_list get_pdp_addr_list get_ims_state get_volte_state"

get_special_info() {
	modem_num="$2"
	model="$1"

	MEIG_AT_LIST="AT+EFSRW=0,0,\"/nv/item_files/ims/IMS_enable\" \
AT+NVBURS=2"
	QUEC_AT_LIST="AT+QPRTPARA=4 \
AT+QCFG=\"dbgctl\""

	case "$model" in
	*SLM750*) CMD_LIST=$MEIG_AT_LIST ;;
	*) CMD_LIST=$QUEC_AT_LIST ;;
	esac

	{
		for cmd in ${CMD_LIST}; do
			echo "$cmd"
			ubus call "$modem_num" exec "{\"command\":'$cmd'}" 2>&1
		done
	} >>"$log_file"
}

get_gsm_info() {
	local log_file="$1"

	troubleshoot_init_log "GSM INFORMATION" "$log_file"
	ubus call gsm info >>"$log_file" 2>&1

	#iterating each modem ubus object
	for mdm in $(ubus list | grep "gsm.modem"); do
		troubleshoot_init_log "INFO for $mdm" "$log_file"

		at_ans="$(ubus call "$mdm" exec "{\"command\":\"AT\"}")"
		if [ -z "${at_ans##*\\r\\nOK\\r\\n*}" ]; then
			info_output="$(ubus call "$mdm" info 2>&1)"
			printf "%-40s\n%s\n" "Running info..." "$info_output" >>"$log_file"

			#foreach top_priority_methods
			for method in $top_priority_methods; do
				cmd="$(ubus call "$mdm" "$method" 2>&1)"
				[ "$cmd" = "Command failed: Operation not supported" ] && continue
				printf "%s:\n%s\n" "$method" "$cmd" >>"$log_file"
			done

			#foreach 'get' command without arguments
			for l in $(ubus -v list $mdm); do
				[ "${l#\"get*:}" != "{}" ] && continue

				#took off clip_mode because it switch RG50X modems to 3G every time
				[ "$l" = "\"get_clip_mode\":{}" ] && continue

				method_name="$(echo ${l%:*} | xargs)"

				#skip methods from top_priority_methods list
				echo "$top_priority_methods" | grep -q "$method_name" && continue

				cmd="$(ubus call "$mdm" "$method_name" 2>&1)"
				[ "$cmd" = "Command failed: Operation not supported" ] && continue
				printf "%s:\n%s\n" "$method_name" "$cmd" >>"$log_file"
			done

			json_init
			json_load "$info_output"
			json_get_vars model

			get_special_info "$model" "$mdm"
		else
			troubleshoot_add_log "Modem not responding to AT commands. Skipping.." "$log_file"
		fi
	done
}

modem_hook() {
	local log_file="${PACK_DIR}gsm.log"

	get_gsm_info "$log_file"
}

troubleshoot_hook_init modem_hook
