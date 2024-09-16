#!/bin/sh

top_priority_methods="get_firmware get_iccid get_serial get_sim_slot get_pin_count get_pin_state get_temperature \
get_func get_network_info get_signal_query get_serving_cell get_neighbour_cells get_net_reg_stat \
get_net_greg_stat get_net_ereg_stat get_net_5greg_stat get_service_provider get_mbn_list get_ps_att_state \
get_pdp_ctx_list get_active_pdp_ctx_list get_pdp_addr_list get_ims_state get_volte_state"

methods_with_args="get_time{\"mode\":\"gmt\"} get_time{\"mode\":\"local\"} get_pdp_call{\"cid\":1}"

info_output=""

ALLOWED_TIMEOUTS=2

check_output()
{
	local output="$1"
	local log_file="$2"

	case "$output" in
	*"Request timeout expired"*) TIMEOUT_COUNTER=$((TIMEOUT_COUNTER+1)) ;;
	esac

	if [ $TIMEOUT_COUNTER -ge $ALLOWED_TIMEOUTS ]; then
		printf "GSM commands appear to be timing out. Skipping the rest...\n" >> "$log_file"
	fi
}

# Executes method and tracks how long the call took
invoke_and_print_cmd() {
	local modem="$1"
	local method="$2"
	local log_file="$3"
	local cmd_and_time cmd_time

	if [ $TIMEOUT_COUNTER -ge $ALLOWED_TIMEOUTS ]; then
		printf "Skipping: %s\n" "$method" >> "$log_file"
		return
	fi

	cmd_and_time=$(time -f '%e' ubus call "$modem" "$method" 2>&1) || return
	info_output=$(echo "$cmd_and_time" | head -n -1)
	cmd_time=$(echo "$cmd_and_time" | tail -n1)

	printf "%s(%ss):\n%s\n" "$method" "$cmd_time" "$info_output" >> "$log_file"
	check_output "$info_output" "$log_file"
}

invoke_ubus_with_args() {
	method=$(echo "$1" | sed "s/{/ &/")
	invoke_and_print_cmd "$2" "$method" "$3"
}

get_special_info() {
	model="$1"
	modem_num="$2"
	log_file="$3"

	if [ $TIMEOUT_COUNTER -ge $ALLOWED_TIMEOUTS ]; then
		printf "Skipping AT commands\n" >> "$log_file"
		return
	fi

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
			TIMEOUT_COUNTER=0
			invoke_and_print_cmd "$mdm" "info" "$log_file"

			#foreach top_priority_methods
			for method in $top_priority_methods; do
				invoke_and_print_cmd "$mdm" "$method" "$log_file"
			done

			#foreach 'get' command without arguments
			for l in $(ubus -v list $mdm); do
				[ "${l#\"get*:}" != "{}" ] && continue

				#took off clip_mode because it switch RG50X modems to 3G every time
				[ "$l" = "\"get_clip_mode\":{}" ] && continue

				method_name="$(echo ${l%:*} | xargs)"

				#skip methods from top_priority_methods list
				echo "$top_priority_methods" | grep -q "$method_name" && continue

				invoke_and_print_cmd "$mdm" "$method_name" "$log_file"
			done

			#foreach methods_with_args
			for method in $methods_with_args; do
				invoke_ubus_with_args "$method" "$mdm" "$log_file"
			done

			json_init
			json_load "$info_output"
			json_get_vars model

			get_special_info "$model" "$mdm" "$log_file"
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
