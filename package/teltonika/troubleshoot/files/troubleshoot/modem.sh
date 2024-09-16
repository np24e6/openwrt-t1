#!/bin/sh

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
	ubus call gsm enable_debug '{"enabled":"true"}'

	troubleshoot_init_log "GSM INFORMATION" "$log_file"
	ubus call gsm info >>"$log_file" 2>&1

	#iterating each modem ubus object
	for mdm in $(ubus list | grep "gsm.modem"); do
		troubleshoot_init_log "INFO for $mdm" "$log_file"

		at_ans="$(ubus call "$mdm" exec "{\"command\":\"AT\"}")"
		if [ -z "${at_ans##*\\r\\nOK\\r\\n*}" ]; then
			info_output="$(ubus call "$mdm" info 2>&1)"
			printf "%-40s\n%s\n" "Running info..." "$info_output" >>"$log_file"

			#foreach 'get' command without arguments
			for l in $(ubus -v list $mdm); do
				[ "${l#\"get*:}" != "{}" ] && continue

				#took off clip_mode because it switch RG50X modems to 3G every time
				[ "$l" = "\"get_clip_mode\":{}" ] && continue

				method_name="$(echo ${l%:*} | xargs)"

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

	ubus call gsm dump_log
	ubus call gsm enable_debug '{"enabled":"false"}'
}

modem_hook() {
	local log_file="${PACK_DIR}gsm.log"

	get_gsm_info "$log_file"
}

troubleshoot_hook_init modem_hook
