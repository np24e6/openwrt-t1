. /lib/functions/network.sh
. /lib/functions.sh

wpa_supplicant_add_rate() {
	local var="$1"
	local val="$(($2 / 1000))"
	local sub="$((($2 / 100) % 10))"
	append $var "$val" ","
	[ $sub -gt 0 ] && append $var "."
}

hostapd_add_rate() {
	local var="$1"
	local val="$(($2 / 100))"
	append $var "$val" " "
}

hostapd_append_wep_key() {
	local var="$1"

	wep_keyidx=0
	set_default key 1
	case "$key" in
		[1234])
			for idx in 1 2 3 4; do
				local zidx
				zidx=$(($idx - 1))
				json_get_var ckey "key${idx}"
				[ -n "$ckey" ] && \
					append $var "wep_key${zidx}=$(prepare_key_wep "$ckey")" "$N$T"
			done
			wep_keyidx=$((key - 1))
		;;
		*)
			append $var "wep_key0=$(prepare_key_wep "$key")" "$N$T"
		;;
	esac
}

hostapd_append_wpa_key_mgmt() {
	local auth_type_l="$(echo $auth_type | tr 'a-z' 'A-Z')"

	case "$auth_type" in
		psk|eap)
			append wpa_key_mgmt "WPA-$auth_type_l"
			[ "${ieee80211r:-0}" -gt 0 ] && append wpa_key_mgmt "FT-${auth_type_l}"
			[ "${ieee80211w:-0}" -gt 0 ] && append wpa_key_mgmt "WPA-${auth_type_l}-SHA256"
		;;
		eap192)
			append wpa_key_mgmt "WPA-EAP-SUITE-B-192"
			[ "${ieee80211r:-0}" -gt 0 ] && append wpa_key_mgmt "FT-EAP-SHA384"
		;;
		eap-eap192)
			append wpa_key_mgmt "WPA-EAP-SUITE-B-192"
			append wpa_key_mgmt "WPA-EAP"
			[ "${ieee80211r:-0}" -gt 0 ] && {
				append wpa_key_mgmt "FT-EAP-SHA384"
				append wpa_key_mgmt "FT-EAP"
			}
			[ "${ieee80211w:-0}" -gt 0 ] && append wpa_key_mgmt "WPA-EAP-SHA256"
		;;
		sae)
			append wpa_key_mgmt "SAE"
			[ "${ieee80211r:-0}" -gt 0 ] && append wpa_key_mgmt "FT-SAE"
		;;
		psk-sae)
			append wpa_key_mgmt "WPA-PSK"
			[ "${ieee80211r:-0}" -gt 0 ] && append wpa_key_mgmt "FT-PSK"
			[ "${ieee80211w:-0}" -gt 0 ] && append wpa_key_mgmt "WPA-PSK-SHA256"
			append wpa_key_mgmt "SAE"
			[ "${ieee80211r:-0}" -gt 0 ] && append wpa_key_mgmt "FT-SAE"
		;;
		owe)
			append wpa_key_mgmt "OWE"
		;;
	esac

	[ "$auth_osen" = "1" ] && append wpa_key_mgmt "OSEN"
}

hostapd_add_log_config() {
	config_add_boolean \
		log_80211 \
		log_8021x \
		log_radius \
		log_wpa \
		log_driver \
		log_iapp \
		log_mlme

	config_add_int log_level
}

hostapd_common_add_device_config() {
	config_add_array basic_rate
	config_add_array supported_rates

	config_add_string country country3
	config_add_boolean country_ie doth
	config_add_boolean spectrum_mgmt_required
	config_add_int local_pwr_constraint
	config_add_string require_mode
	config_add_boolean legacy_rates
	config_add_int cell_density

	config_add_string acs_chan_bias
	config_add_array hostapd_options

	config_add_int airtime_mode

	hostapd_add_log_config
}

hostapd_prepare_device_config() {
	local config="$1"
	local driver="$2"

	local base_cfg=

	json_get_vars country country3 country_ie beacon_int:100 dtim_period:2 doth require_mode legacy_rates \
		acs_chan_bias local_pwr_constraint spectrum_mgmt_required airtime_mode cell_density

	hostapd_set_log_options base_cfg

	set_default country_ie 1
	set_default spectrum_mgmt_required 0
	set_default doth 1
	set_default legacy_rates 0
	set_default airtime_mode 0
	set_default cell_density 0

	[ -n "$country" ] && {
		append base_cfg "country_code=$country" "$N"
		[ -n "$country3" ] && append base_cfg "country3=$country3" "$N"

		[ "$country_ie" -gt 0 ] && {
			append base_cfg "ieee80211d=1" "$N"
			[ -n "$local_pwr_constraint" ] && append base_cfg "local_pwr_constraint=$local_pwr_constraint" "$N"
			[ "$spectrum_mgmt_required" -gt 0 ] && append base_cfg "spectrum_mgmt_required=$spectrum_mgmt_required" "$N"
		}
		[ "$hwmode" = "a" -a "$doth" -gt 0 ] && append base_cfg "ieee80211h=1" "$N"
	}

	[ -n "$acs_chan_bias" ] && append base_cfg "acs_chan_bias=$acs_chan_bias" "$N"

	local brlist= br
	json_get_values basic_rate_list basic_rate
	local rlist= r
	json_get_values rate_list supported_rates

	[ -n "$hwmode" ] && append base_cfg "hw_mode=$hwmode" "$N"
	if [ "$hwmode" = "g" ] || [ "$hwmode" = "a" ]; then
		[ -n "$require_mode" ] && legacy_rates=0
		case "$require_mode" in
			n) append base_cfg "require_ht=1" "$N";;
			ac) append base_cfg "require_vht=1" "$N";;
		esac
	fi
	case "$hwmode" in
		b)
			if [ "$cell_density" -eq 1 ]; then
				set_default rate_list "5500 11000"
				set_default basic_rate_list "5500 11000"
			elif [ "$cell_density" -ge 2 ]; then
				set_default rate_list "11000"
				set_default basic_rate_list "11000"
			fi
		;;
		g)
			if [ "$cell_density" -eq 0 ] || [ "$cell_density" -eq 1 ]; then
				if [ "$legacy_rates" -eq 0 ]; then
					set_default rate_list "6000 9000 12000 18000 24000 36000 48000 54000"
					set_default basic_rate_list "6000 12000 24000"
				elif [ "$cell_density" -eq 1 ]; then
					set_default rate_list "5500 6000 9000 11000 12000 18000 24000 36000 48000 54000"
					set_default basic_rate_list "5500 11000"
				fi
			elif [ "$cell_density" -ge 3 ] && [ "$legacy_rates" -ne 0 ] || [ "$cell_density" -eq 2 ]; then
				if [ "$legacy_rates" -eq 0 ]; then
					set_default rate_list "12000 18000 24000 36000 48000 54000"
					set_default basic_rate_list "12000 24000"
				else
					set_default rate_list "11000 12000 18000 24000 36000 48000 54000"
					set_default basic_rate_list "11000"
				fi
			elif [ "$cell_density" -ge 3 ]; then
				set_default rate_list "24000 36000 48000 54000"
				set_default basic_rate_list "24000"
			fi
		;;
		a)
			if [ "$cell_density" -eq 1 ]; then
				set_default rate_list "6000 9000 12000 18000 24000 36000 48000 54000"
				set_default basic_rate_list "6000 12000 24000"
			elif [ "$cell_density" -eq 2 ]; then
				set_default rate_list "12000 18000 24000 36000 48000 54000"
				set_default basic_rate_list "12000 24000"
			elif [ "$cell_density" -ge 3 ]; then
				set_default rate_list "24000 36000 48000 54000"
				set_default basic_rate_list "24000"
			fi
		;;
	esac

	for r in $rate_list; do
		hostapd_add_rate rlist "$r"
	done

	for br in $basic_rate_list; do
		hostapd_add_rate brlist "$br"
	done

	[ -n "$rlist" ] && append base_cfg "supported_rates=$rlist" "$N"
	[ -n "$brlist" ] && append base_cfg "basic_rates=$brlist" "$N"
	append base_cfg "beacon_int=$beacon_int" "$N"
	append base_cfg "dtim_period=$dtim_period" "$N"
	[ "$airtime_mode" -gt 0 ] && append base_cfg "airtime_mode=$airtime_mode" "$N"

	json_get_values opts hostapd_options
	for val in $opts; do
		append base_cfg "$val" "$N"
	done

	cat > "$config" <<EOF
driver=$driver
$base_cfg
EOF
}

hostapd_common_add_bss_config() {
	config_add_string 'bssid:macaddr' 'ssid:string'
	config_add_boolean wds wmm uapsd hidden utf8_ssid

	config_add_int maxassoc max_inactivity
	config_add_boolean disassoc_low_ack isolate short_preamble skip_inactivity_poll

	config_add_int \
		wep_rekey eap_reauth_period \
		wpa_group_rekey wpa_pair_rekey wpa_master_rekey
	config_add_boolean wpa_strict_rekey
	config_add_boolean wpa_disable_eapol_key_retries

	config_add_boolean tdls_prohibit

	config_add_boolean rsn_preauth auth_cache
	config_add_int ieee80211w
	config_add_int eapol_version

	config_add_string 'auth_server:host' 'server:host'
	config_add_string auth_secret key
	config_add_int 'auth_port:port' 'port:port'

	config_add_string acct_server
	config_add_string acct_secret
	config_add_int acct_port
	config_add_int acct_interval

	config_add_int bss_load_update_period chan_util_avg_period

	config_add_string dae_client
	config_add_string dae_secret
	config_add_int dae_port

	config_add_string nasid
	config_add_string ownip
	config_add_string radius_client_addr
	config_add_string iapp_interface
	config_add_string eap_type ca_cert client_cert identity anonymous_identity auth priv_key priv_key_pwd
	config_add_string use_pkcs pkcs_cert pkcs_passwd
	config_add_boolean ca_cert_usesystem ca_cert2_usesystem
	config_add_string subject_match subject_match2
	config_add_array altsubject_match altsubject_match2
	config_add_array domain_match domain_match2 domain_suffix_match domain_suffix_match2
	config_add_string ieee80211w_mgmt_cipher

	config_add_int dynamic_vlan vlan_naming
	config_add_string vlan_tagged_interface vlan_bridge
	config_add_string vlan_file

	config_add_string 'key1:wepkey' 'key2:wepkey' 'key3:wepkey' 'key4:wepkey' 'password:wpakey'

	config_add_string wpa_psk_file

	config_add_int multi_ap

	config_add_boolean wps_pushbutton wps_label ext_registrar wps_pbc_in_m1
	config_add_int wps_ap_setup_locked wps_independent
	config_add_string wps_device_type wps_device_name wps_manufacturer wps_pin
	config_add_string multi_ap_backhaul_ssid multi_ap_backhaul_key

	config_add_boolean wnm_sleep_mode wnm_sleep_mode_no_keys bss_transition
	config_add_int time_advertisement
	config_add_string time_zone
	config_add_string vendor_elements

	config_add_boolean ieee80211k rrm_neighbor_report rrm_beacon_report

	config_add_boolean ftm_responder stationary_ap
	config_add_string lci civic

	config_add_boolean ieee80211r pmk_r1_push ft_psk_generate_local ft_over_ds
	config_add_int r0_key_lifetime reassociation_deadline
	config_add_string mobility_domain r1_key_holder
	config_add_array r0kh r1kh

	config_add_int ieee80211w_max_timeout ieee80211w_retry_timeout

	config_add_string macfilter 'macfile:file'
	config_add_array 'maclist:list(macaddr)'

	config_add_array bssid_blacklist
	config_add_array bssid_whitelist

	config_add_int mcast_rate
	config_add_array basic_rate
	config_add_array supported_rates

	config_add_boolean sae_require_mfp
	config_add_int sae_pwe

	config_add_string 'owe_transition_bssid:macaddr' 'owe_transition_ssid:string'
	config_add_string owe_transition_ifname

	config_add_boolean interworking internet asra esr uesa
	config_add_int access_network_type venue_group venue_type
	config_add_int ipaddr_type_availability ipaddr6_type_availability iw_gas_address3
	config_add_string hessid network_auth_type redirect_url iw_qos_map_set osu_friendly_name osu_ssid osu_server_uri osu_nai osu_service_desc
	config_add_array roaming_consortium venue_name venue_url domain_name anqp_3gpp_cell_net nai_realm osu_method_list
	config_add_array anqp_elem

	config_add_boolean hs20 disable_dgaf osen
	config_add_int anqp_domain_id
	config_add_int hs20_deauth_req_timeout
	config_add_array hs20_oper_friendly_name
	config_add_array operator_icon
	config_add_array hs20_conn_capab
	config_add_string hs20_operating_class hs20_t_c_filename hs20_t_c_timestamp hs20_wan_status hs20_wan_dw_speed hs20_wan_up_speed

	config_add_array airtime_sta_weight
	config_add_int airtime_bss_weight airtime_bss_limit

	config_add_boolean multicast_to_unicast per_sta_vif

	config_add_array hostapd_bss_options

	config_add_string bgscan
	config_add_boolean db_permanent
	config_add_int short_interval
	config_add_int signal_thresh
	config_add_int long_interval
}

hostapd_set_vlan_file() {
	local ifname="$1"
	local vlan="$2"
	json_get_vars name vid
	echo "${vid} ${ifname}-${name}" >> /var/run/hostapd-${ifname}.vlan
	wireless_add_vlan "${vlan}" "${ifname}-${name}"
}

hostapd_set_vlan() {
	local ifname="$1"

	rm -f /var/run/hostapd-${ifname}.vlan
	for_each_vlan hostapd_set_vlan_file ${ifname}
}

hostapd_set_psk_file() {
	local ifname="$1"
	local vlan="$2"
	local vlan_id=""

	json_get_vars mac vid key
	set_default mac "00:00:00:00:00:00"
	[ -n "$vid" ] && vlan_id="vlanid=$vid "
	echo "${vlan_id} ${mac} ${key}" >> /var/run/hostapd-${ifname}.psk
}

hostapd_set_psk() {
	local ifname="$1"

	rm -f /var/run/hostapd-${ifname}.psk
	for_each_station hostapd_set_psk_file ${ifname}
}



append_roaming_consortium() {
	[ -n "$1" ] && append bss_conf "roaming_consortium=$1" "$N"
}

append_domain_name() {
	if [ -z "$domain_name_conf" ]; then
		domain_name_conf="$1"
	else
		domain_name_conf="$domain_name_conf,$1"
	fi
}

append_anqp_elem() {
	[ -n "$1" ] && append bss_conf "anqp_elem=$1" "$N"
}

append_hs20_icon() {
	local width height lang type path
	config_get width "$1" width "0"
	config_get height "$1" height "0"
	config_get lang "$1" lang "xx"
	config_get type "$1" type "null"
	config_get path "$1" path

	append bss_conf "hs20_icon=$width:$height:$lang:$type:$1:$path" "$N"
}

append_hs20_icons() {
	config_load wireless
	config_foreach append_hs20_icon hs20-icon
}

append_operator_icon() {
	append bss_conf "operator_icon=$1" "$N"
}

append_airtime_sta_weight() {
	[ -n "$1" ] && append bss_conf "airtime_sta_weight=$1" "$N"
}

append_venue() {
	local key="$1"
	local object_num="$2"
	local url name

	json_select "$object_num"
		json_get_vars url name

		set_default url "http://null.nl"
		set_default name "eng:null"

		append bss_conf "venue_name=$name" "$N"
		append bss_conf "venue_url=$venue_count:$url" "$N"
	json_select ..
	venue_count=$((venue_count + 1))
}

append_nai_realm() {
	local key="$1"
	local object_num="$2"
	local number hostname auth_num param

	json_select "$object_num"
		json_get_vars number hostname auth_num param

		set_default number 0
		set_default hostname "null"
		set_default auth_num 0
		set_default param "[0:0]"

		append bss_conf "nai_realm=${number},${hostname},${auth_num}${param}" "$N"
	json_select ..
}

append_conn_capab() {
	local key="$1"
	local object_num="$2"
	local proto port state

	json_select "$object_num"
		json_get_vars proto port state

		set_default proto "null"
		set_default port 0
		set_default state 0

		append bss_conf "hs20_conn_capab=${proto}:${port}:${state}" "$N"
	json_select ..
}

append_oper_friendly_name() {
	local key="$1"
	local object_num="$2"
	local country_code name

	json_select "$object_num"
		json_get_vars country_code name

		set_default country_code "eng"
		set_default name "null"

		append bss_conf "hs20_oper_friendly_name=${country_code}:${name}" "$N"
	json_select ..
}

append_anqp_3gpp_cell_net() {
	local key="$1"
	local object_num="$2"
	local mobile_country_code mobile_network_code

	json_select "$object_num"
		json_get_vars mobile_country_code mobile_network_code

		set_default mobile_country_code "000"
		set_default mobile_network_code "00"

		if [ -z "$anqp_3gpp_cell_net_conf" ]; then
			anqp_3gpp_cell_net_conf="${mobile_country_code},${mobile_network_code}"
		else
			anqp_3gpp_cell_net_conf="$anqp_3gpp_cell_net_conf;${mobile_country_code},${mobile_network_code}"
		fi
	json_select ..
}

hostapd_set_bss_options() {
	local var="$1"
	local phy="$2"
	local vif="$3"

	wireless_vif_parse_encryption

	local bss_conf bss_md5sum
	local wep_rekey wpa_group_rekey wpa_pair_rekey wpa_master_rekey wpa_key_mgmt

	json_get_vars \
		wep_rekey wpa_group_rekey wpa_pair_rekey wpa_master_rekey wpa_strict_rekey \
		wpa_disable_eapol_key_retries tdls_prohibit \
		maxassoc max_inactivity disassoc_low_ack isolate auth_cache \
		wps_pushbutton wps_label ext_registrar wps_pbc_in_m1 wps_ap_setup_locked \
		wps_independent wps_device_type wps_device_name wps_manufacturer wps_pin \
		macfilter ssid utf8_ssid wmm uapsd hidden short_preamble rsn_preauth \
		iapp_interface eapol_version dynamic_vlan ieee80211w nasid \
		acct_server acct_secret acct_port acct_interval \
		bss_load_update_period chan_util_avg_period sae_require_mfp \
		skip_inactivity_poll \
		airtime_bss_weight airtime_bss_limit airtime_sta_weight \
		multicast_to_unicast per_sta_vif interworking hs20 wifi_id

	set_default isolate 0
	set_default maxassoc 0
	set_default max_inactivity 0
	set_default short_preamble 1
	set_default disassoc_low_ack 1
	set_default skip_inactivity_poll 0
	set_default hidden 0
	set_default wmm 1
	set_default uapsd 1
	set_default wpa_disable_eapol_key_retries 0
	set_default tdls_prohibit 0
	set_default eapol_version $((wpa & 1))
	set_default acct_port 1813
	set_default bss_load_update_period 60
	set_default chan_util_avg_period 600
	set_default utf8_ssid 1
	set_default multi_ap 0
	set_default airtime_bss_weight 0
	set_default airtime_bss_limit 0

	append bss_conf "ctrl_interface=/var/run/hostapd"
	if [ "$isolate" -gt 0 ]; then
		append bss_conf "ap_isolate=$isolate" "$N"
	fi
	if [ "$maxassoc" -gt 0 ]; then
		append bss_conf "max_num_sta=$maxassoc" "$N"
	fi
	if [ "$max_inactivity" -gt 0 ]; then
		append bss_conf "ap_max_inactivity=$max_inactivity" "$N"
	fi

	[ "$airtime_bss_weight" -gt 0 ] && append bss_conf "airtime_bss_weight=$airtime_bss_weight" "$N"
	[ "$airtime_bss_limit" -gt 0 ] && append bss_conf "airtime_bss_limit=$airtime_bss_limit" "$N"
	json_for_each_item append_airtime_sta_weight airtime_sta_weight

	append bss_conf "bss_load_update_period=$bss_load_update_period" "$N"
	append bss_conf "chan_util_avg_period=$chan_util_avg_period" "$N"
	append bss_conf "disassoc_low_ack=$disassoc_low_ack" "$N"
	append bss_conf "skip_inactivity_poll=$skip_inactivity_poll" "$N"
	append bss_conf "preamble=$short_preamble" "$N"
	append bss_conf "wmm_enabled=$wmm" "$N"
	append bss_conf "ignore_broadcast_ssid=$hidden" "$N"
	append bss_conf "uapsd_advertisement_enabled=$uapsd" "$N"
	append bss_conf "utf8_ssid=$utf8_ssid" "$N"
	append bss_conf "multi_ap=$multi_ap" "$N"

	[ "$tdls_prohibit" -gt 0 ] && append bss_conf "tdls_prohibit=$tdls_prohibit" "$N"

	[ "$wpa" -gt 0 ] && {
		[ -n "$wpa_group_rekey"  ] && append bss_conf "wpa_group_rekey=$wpa_group_rekey" "$N"
		[ -n "$wpa_pair_rekey"   ] && append bss_conf "wpa_ptk_rekey=$wpa_pair_rekey"    "$N"
		[ -n "$wpa_master_rekey" ] && append bss_conf "wpa_gmk_rekey=$wpa_master_rekey"  "$N"
		[ -n "$wpa_strict_rekey" ] && append bss_conf "wpa_strict_rekey=$wpa_strict_rekey" "$N"
	}

	[ -n "$nasid" ] && append bss_conf "nas_identifier=$nasid" "$N"
	[ -n "$acct_server" ] && {
		append bss_conf "acct_server_addr=$acct_server" "$N"
		append bss_conf "acct_server_port=$acct_port" "$N"
		[ -n "$acct_secret" ] && \
			append bss_conf "acct_server_shared_secret=$acct_secret" "$N"
		[ -n "$acct_interval" ] && \
			append bss_conf "radius_acct_interim_interval=$acct_interval" "$N"
	}

	case "$auth_type" in
		sae|owe|eap192|eap-eap192)
			set_default ieee80211w 2
			set_default sae_require_mfp 1
			set_default sae_pwe 2
		;;
		psk-sae)
			set_default ieee80211w 1
			set_default sae_require_mfp 1
			set_default sae_pwe 2
		;;
	esac
	[ -n "$sae_require_mfp" ] && append bss_conf "sae_require_mfp=$sae_require_mfp" "$N"
	[ -n "$sae_pwe" ] && append bss_conf "sae_pwe=$sae_pwe" "$N"

	local vlan_possible=""
	wps_not_configured=

	case "$auth_type" in
		none|owe)
			json_get_vars owe_transition_bssid owe_transition_ssid owe_transition_ifname

			[ -n "$owe_transition_ssid" ] && append bss_conf "owe_transition_ssid=\"$owe_transition_ssid\"" "$N"
			[ -n "$owe_transition_bssid" ] && append bss_conf "owe_transition_bssid=$owe_transition_bssid" "$N"
			[ -n "$owe_transition_ifname" ] && append bss_conf "owe_transition_ifname=$owe_transition_ifname" "$N"
			wps_possible=1
			# Here we make the assumption that if we're in open mode
			# with WPS enabled, we got to be in unconfigured state.

		;;
		psk|sae|psk-sae)
			json_get_vars key wpa_psk_file
			if [ ${#key} -eq 64 ]; then
				append bss_conf "wpa_psk=$key" "$N"
			elif [ ${#key} -ge 8 ] && [ ${#key} -le 63 ]; then
				append bss_conf "wpa_passphrase=$key" "$N"
			elif [ -n "$key" ] || [ -z "$wpa_psk_file" ]; then
				wireless_setup_vif_failed INVALID_WPA_PSK
				return 1
			fi
			mkdir -p /etc/hostapd
			[ -z "$wpa_psk_file" ] && set_default wpa_psk_file /etc/hostapd/${ifname%%-*}.psk
			[ -n "$wpa_psk_file" ] && {
				[ -e "$wpa_psk_file" ] || touch "$wpa_psk_file"
				append bss_conf "wpa_psk_file=$wpa_psk_file" "$N"
			}
			[ "$eapol_version" -ge "1" -a "$eapol_version" -le "2" ] && append bss_conf "eapol_version=$eapol_version" "$N"

			set_default dynamic_vlan 0
			vlan_possible=1
			wps_possible=1
		;;
		eap|eap192|eap-eap192)
			json_get_vars \
				auth_server auth_secret auth_port \
				dae_client dae_secret dae_port \
				ownip radius_client_addr \
				eap_reauth_period

			# radius can provide VLAN ID for clients
			vlan_possible=1

			# legacy compatibility
			[ -n "$auth_server" ] || json_get_var auth_server server
			[ -n "$auth_port" ] || json_get_var auth_port port
			[ -n "$auth_secret" ] || json_get_var auth_secret key

			set_default auth_port 1812
			set_default dae_port 3799


			append bss_conf "auth_server_addr=$auth_server" "$N"
			append bss_conf "auth_server_port=$auth_port" "$N"
			append bss_conf "auth_server_shared_secret=$auth_secret" "$N"

			[ -n "$eap_reauth_period" ] && append bss_conf "eap_reauth_period=$eap_reauth_period" "$N"

			[ -n "$dae_client" -a -n "$dae_secret" ] && {
				append bss_conf "radius_das_port=$dae_port" "$N"
				append bss_conf "radius_das_client=$dae_client $dae_secret" "$N"
			}

			[ -n "$ownip" ] && append bss_conf "own_ip_addr=$ownip" "$N"
			[ -n "$radius_client_addr" ] && append bss_conf "radius_client_addr=$radius_client_addr" "$N"
			append bss_conf "eapol_key_index_workaround=1" "$N"
			append bss_conf "ieee8021x=1" "$N"

			[ "$eapol_version" -ge "1" -a "$eapol_version" -le "2" ] && append bss_conf "eapol_version=$eapol_version" "$N"
		;;
		wep)
			local wep_keyidx=0
			json_get_vars key
			hostapd_append_wep_key bss_conf
			append bss_conf "wep_default_key=$wep_keyidx" "$N"
			[ -n "$wep_rekey" ] && append bss_conf "wep_rekey_period=$wep_rekey" "$N"
		;;
	esac

	case "$auth_type" in
		none|owe|psk|sae|psk-sae|wep)
			json_get_vars \
			auth_server auth_port auth_secret \
			ownip radius_client_addr

			[ -n "$auth_server" ] &&  {
				set_default auth_port 1812

				json_for_each_item append_auth_server auth_server
				[ -n "$ownip" ] && append bss_conf "own_ip_addr=$ownip" "$N"
				[ -n "$radius_client_addr" ] && append bss_conf "radius_client_addr=$radius_client_addr" "$N"
				append bss_conf "macaddr_acl=2" "$N"
			}
		;;
	esac

	local auth_algs="$((($auth_mode_shared << 1) | $auth_mode_open))"
	append bss_conf "auth_algs=${auth_algs:-1}" "$N"
	append bss_conf "wpa=$wpa" "$N"
	[ -n "$wpa_pairwise" ] && append bss_conf "wpa_pairwise=$wpa_pairwise" "$N"

	set_default wps_pushbutton 0
	set_default wps_label 0
	set_default wps_pbc_in_m1 0

	config_methods=
	[ "$wps_pushbutton" -gt 0 ] && append config_methods push_button
	[ "$wps_label" -gt 0 ] && append config_methods label

	# WPS not possible on Multi-AP backhaul-only SSID
	[ "$multi_ap" = 1 ] && wps_possible=

	[ -n "$wps_possible" -a -n "$config_methods" ] && {
		set_default ext_registrar 0
		set_default wps_device_type "6-0050F204-1"
		set_default wps_device_name "OpenWrt AP"
		set_default wps_manufacturer "www.openwrt.org"
		set_default wps_independent 1

		wps_state=2
		[ -n "$wps_not_configured" ] && wps_state=1

		[ "$ext_registrar" -gt 0 -a -n "$network_bridge" ] && append bss_conf "upnp_iface=$network_bridge" "$N"

		append bss_conf "eap_server=1" "$N"
		[ -n "$wps_pin" ] && append bss_conf "ap_pin=$wps_pin" "$N"
		append bss_conf "wps_state=$wps_state" "$N"
		append bss_conf "device_type=$wps_device_type" "$N"
		append bss_conf "device_name=$wps_device_name" "$N"
		append bss_conf "manufacturer=$wps_manufacturer" "$N"
		append bss_conf "config_methods=$config_methods" "$N"
		append bss_conf "wps_independent=$wps_independent" "$N"
		[ -n "$wps_ap_setup_locked" ] && append bss_conf "ap_setup_locked=$wps_ap_setup_locked" "$N"
		[ "$wps_pbc_in_m1" -gt 0 ] && append bss_conf "pbc_in_m1=$wps_pbc_in_m1" "$N"
		[ "$multi_ap" -gt 0 ] && [ -n "$multi_ap_backhaul_ssid" ] && {
			append bss_conf "multi_ap_backhaul_ssid=\"$multi_ap_backhaul_ssid\"" "$N"
			if [ -z "$multi_ap_backhaul_key" ]; then
				:
			elif [ ${#multi_ap_backhaul_key} -lt 8 ]; then
				wireless_setup_vif_failed INVALID_WPA_PSK
				return 1
			elif [ ${#multi_ap_backhaul_key} -eq 64 ]; then
				append bss_conf "multi_ap_backhaul_wpa_psk=$multi_ap_backhaul_key" "$N"
			else
				append bss_conf "multi_ap_backhaul_wpa_passphrase=$multi_ap_backhaul_key" "$N"
			fi
		}
	}

	append bss_conf "ssid=$ssid" "$N"
	[ -n "$network_bridge" ] && append bss_conf "bridge=$network_bridge" "$N"
	[ -n "$iapp_interface" ] && {
		local ifname
		network_get_device ifname "$iapp_interface" || ifname="$iapp_interface"
		append bss_conf "iapp_interface=$ifname" "$N"
	}

	json_get_vars time_advertisement time_zone wnm_sleep_mode wnm_sleep_mode_no_keys bss_transition
	set_default bss_transition 0
	set_default wnm_sleep_mode 0
	set_default wnm_sleep_mode_no_keys 0

	[ -n "$time_advertisement" ] && append bss_conf "time_advertisement=$time_advertisement" "$N"
	[ -n "$time_zone" ] && append bss_conf "time_zone=$time_zone" "$N"
	if [ "$wnm_sleep_mode" -eq "1" ]; then
		append bss_conf "wnm_sleep_mode=1" "$N"
		[ "$wnm_sleep_mode_no_keys" -eq "1" ] && append bss_conf "wnm_sleep_mode_no_keys=1" "$N"
	fi
	[ "$bss_transition" -eq "1" ] && append bss_conf "bss_transition=1" "$N"

	json_get_vars ieee80211k rrm_neighbor_report rrm_beacon_report
	set_default ieee80211k 0
	if [ "$ieee80211k" -eq "1" ]; then
		set_default rrm_neighbor_report 1
		set_default rrm_beacon_report 1
	else
		set_default rrm_neighbor_report 0
		set_default rrm_beacon_report 0
	fi

	[ "$rrm_neighbor_report" -eq "1" ] && append bss_conf "rrm_neighbor_report=1" "$N"
	[ "$rrm_beacon_report" -eq "1" ] && append bss_conf "rrm_beacon_report=1" "$N"

	json_get_vars ftm_responder stationary_ap lci civic
	set_default ftm_responder 0
	if [ "$ftm_responder" -eq "1" ]; then
		set_default stationary_ap 0
		iw phy "$phy" info | grep -q "ENABLE_FTM_RESPONDER" && {
			append bss_conf "ftm_responder=1" "$N"
			[ "$stationary_ap" -eq "1" ] && append bss_conf "stationary_ap=1" "$N"
			[ -n "$lci" ] && append bss_conf "lci=$lci" "$N"
			[ -n "$civic" ] && append bss_conf "lci=$civic" "$N"
		}
	fi

	if [ "$wpa" -ge "1" ]; then
		json_get_vars ieee80211r
		set_default ieee80211r 0

		if [ "$ieee80211r" -gt "0" ]; then
			json_get_vars mobility_domain ft_psk_generate_local ft_over_ds reassociation_deadline

			set_default mobility_domain "$(echo "$ssid" | md5sum | head -c 4)"
			set_default ft_over_ds 1
			set_default reassociation_deadline 1000

			case "$auth_type" in
				psk|sae|psk-sae)
					set_default ft_psk_generate_local 1
				;;
				*)
					set_default ft_psk_generate_local 0
				;;
			esac

			append bss_conf "mobility_domain=$mobility_domain" "$N"
			append bss_conf "ft_psk_generate_local=$ft_psk_generate_local" "$N"
			append bss_conf "ft_over_ds=$ft_over_ds" "$N"
			append bss_conf "reassociation_deadline=$reassociation_deadline" "$N"
			[ -n "$nasid" ] || append bss_conf "nas_identifier=${macaddr//\:}" "$N"

			if [ "$ft_psk_generate_local" -eq "0" ]; then
				json_get_vars r0_key_lifetime r1_key_holder pmk_r1_push
				json_get_values r0kh r0kh
				json_get_values r1kh r1kh

				set_default r0_key_lifetime 10000
				set_default pmk_r1_push 0

				[ -n "$r1_key_holder" ] && append bss_conf "r1_key_holder=$r1_key_holder" "$N"
				append bss_conf "r0_key_lifetime=$r0_key_lifetime" "$N"
				append bss_conf "pmk_r1_push=$pmk_r1_push" "$N"

				for kh in $r0kh; do
					append bss_conf "r0kh=${kh//,/ }" "$N"
				done
				for kh in $r1kh; do
					append bss_conf "r1kh=${kh//,/ }" "$N"
				done
			fi
		fi

		append bss_conf "wpa_disable_eapol_key_retries=$wpa_disable_eapol_key_retries" "$N"

		hostapd_append_wpa_key_mgmt
		[ -n "$wpa_key_mgmt" ] && append bss_conf "wpa_key_mgmt=$wpa_key_mgmt" "$N"
	fi

	if [ "$wpa" -ge "2" ]; then
		if [ -n "$network_bridge" -a "$rsn_preauth" = 1 ]; then
			set_default auth_cache 1
			append bss_conf "rsn_preauth=1" "$N"
			append bss_conf "rsn_preauth_interfaces=$network_bridge" "$N"
		else
			case "$auth_type" in
			sae|psk-sae|owe)
				set_default auth_cache 1
			;;
			*)
				set_default auth_cache 0
			;;
			esac
		fi

		append bss_conf "okc=$auth_cache" "$N"
		[ "$auth_cache" = 0 ] && append bss_conf "disable_pmksa_caching=1" "$N"

		# RSN -> allow management frame protection
		case "$ieee80211w" in
			[012])
				json_get_vars ieee80211w_mgmt_cipher ieee80211w_max_timeout ieee80211w_retry_timeout
				append bss_conf "ieee80211w=$ieee80211w" "$N"
				[ "$ieee80211w" -gt "0" ] && {
					if [ "$auth_type" = "eap192" ]; then
						append bss_conf "group_mgmt_cipher=BIP-GMAC-256" "$N"
					else
						append bss_conf "group_mgmt_cipher=${ieee80211w_mgmt_cipher:-AES-128-CMAC}" "$N"
					fi
					[ -n "$ieee80211w_max_timeout" ] && \
						append bss_conf "assoc_sa_query_max_timeout=$ieee80211w_max_timeout" "$N"
					[ -n "$ieee80211w_retry_timeout" ] && \
						append bss_conf "assoc_sa_query_retry_timeout=$ieee80211w_retry_timeout" "$N"
				}
			;;
		esac
	fi

	_macfile="/var/run/hostapd-$ifname.maclist"
	case "$macfilter" in
		allow)
			append bss_conf "macaddr_acl=1" "$N"
			append bss_conf "accept_mac_file=$_macfile" "$N"
			# accept_mac_file can be used to set MAC to VLAN ID mapping
			vlan_possible=1
		;;
		deny)
			append bss_conf "macaddr_acl=0" "$N"
			append bss_conf "deny_mac_file=$_macfile" "$N"
		;;
		*)
			_macfile=""
		;;
	esac

	[ -n "$_macfile" ] && {
		json_get_vars macfile
		json_get_values maclist maclist

		rm -f "$_macfile"
		(
			for mac in $maclist; do
				echo "$mac"
			done
			[ -n "$macfile" -a -f "$macfile" ] && cat "$macfile"
		) > "$_macfile"
	}

	[ -n "$vlan_possible" -a -n "$dynamic_vlan" ] && {
		json_get_vars vlan_naming vlan_tagged_interface vlan_bridge vlan_file vlan_no_bridge
		set_default vlan_naming 1
		[ -z "$vlan_file" ] && set_default vlan_file /var/run/hostapd-$ifname.vlan
		append bss_conf "dynamic_vlan=$dynamic_vlan" "$N"
		append bss_conf "vlan_naming=$vlan_naming" "$N"
		if [ -n "$vlan_bridge" ]; then
			append bss_conf "vlan_bridge=$vlan_bridge" "$N"
		else
			set_default vlan_no_bridge 1
		fi
		append bss_conf "vlan_no_bridge=$vlan_no_bridge" "$N"
		[ -n "$vlan_tagged_interface" ] && \
			append bss_conf "vlan_tagged_interface=$vlan_tagged_interface" "$N"
		[ -n "$vlan_file" ] && {
			[ -e "$vlan_file" ] || touch "$vlan_file"
			append bss_conf "vlan_file=$vlan_file" "$N"
		}
	}

	json_get_vars interworking internet asra esr uesa access_network_type
	json_get_vars hessid venue_group venue_type network_auth_type
	json_get_vars roaming_consortium domain_name anqp_3gpp_cell_net nai_realm

	json_get_vars anqp_elem iw_qos_map_set ipaddr_type_availability iw_gas_address3
	json_get_vars redirect_url ipaddr6_type_availability venue

	set_default interworking 0

	if [ "$interworking" = "1" ]; then
		append bss_conf "interworking=1" "$N"
		set_default internet 1
		set_default asra 0
		set_default esr 0
		set_default uesa 0

		append bss_conf "internet=$internet" "$N"
		append bss_conf "asra=$asra" "$N"
		append bss_conf "esr=$esr" "$N"
		append bss_conf "uesa=$uesa" "$N"

		local venue_count=1
		json_for_each_item append_venue venue

		[ -n "$access_network_type" ] && \
			append bss_conf "access_network_type=$access_network_type" "$N"
		[ -n "$hessid" ] && append bss_conf "hessid=$hessid" "$N"
		[ -n "$venue_group" ] && \
			append bss_conf "venue_group=$venue_group" "$N"
		[ -n "$venue_type" ] && append bss_conf "venue_type=$venue_type" "$N"
		[ -n "$network_auth_type" ] && {
			[ "$network_auth_type" = "02" ] && \
				append bss_conf "network_auth_type=$network_auth_type$redirect_url" "$N" || \
				append bss_conf "network_auth_type=$network_auth_type" "$N"
		}
		[ -n "$iw_gas_address3" ] && append bss_conf "gas_address3=$iw_gas_address3" "$N"
		[ -n "$iw_qos_map_set" ] && append bss_conf "qos_map_set=$iw_qos_map_set" "$N"
		[ -n "$ipaddr_type_availability" ] || [ -n "$ipaddr6_type_availability" ] && {
			set_default ipaddr_type_availability 7 #unknown
			set_default ipaddr6_type_availability 7 #unknown
			local encoden_val=$((($ipaddr_type_availability & 0x3f) << 2 | ($ipaddr6_type_availability & 0x3)))
			local hex_val=$(printf "%02x\n" $encoden_val)
			append bss_conf "ipaddr_type_availability=$hex_val" "$N"
		}

		json_for_each_item append_roaming_consortium roaming_consortium
		json_for_each_item append_anqp_elem anqp_elem
		json_for_each_item append_nai_realm nai_realm

		domain_name_conf=
		json_for_each_item append_domain_name domain_name
		[ -n "$domain_name_conf" ] && \
			append bss_conf "domain_name=$domain_name_conf" "$N"

		anqp_3gpp_cell_net_conf=
		json_for_each_item append_anqp_3gpp_cell_net anqp_3gpp_cell_net
		[ -n "$anqp_3gpp_cell_net_conf" ] && \
			append bss_conf "anqp_3gpp_cell_net=$anqp_3gpp_cell_net_conf" "$N"
	fi

	set_default iw_qos_map_set 0,0,2,16,1,1,255,255,18,22,24,38,40,40,44,46,48,56
	case "$iw_qos_map_set" in
		*,*);;
		*) iw_qos_map_set="";;
	esac
	[ -n "$iw_qos_map_set" ] && append bss_conf "qos_map_set=$iw_qos_map_set" "$N"

	local hs20 disable_dgaf osen anqp_domain_id hs20_deauth_req_timeout \
		osu_server_uri osu_service_desc osu_friendly_name osu_nai osu_ssid \
		hs20_operating_class hs20_t_c_filename hs20_t_c_timestamp \
		hs20_t_c_server_url  hs20_wan_status hs20_wan_dw_speed \
		hs20_wan_up_speed osu_method_list hs20_conn_capab hs20_oper_friendly_name
	json_get_vars hs20 disable_dgaf osen anqp_domain_id hs20_deauth_req_timeout \
		osu_server_uri osu_service_desc osu_friendly_name osu_nai osu_ssid \
		hs20_operating_class hs20_t_c_filename hs20_t_c_timestamp \
		hs20_t_c_server_url hs20_wan_status hs20_wan_dw_speed \
		hs20_wan_up_speed osu_method_list hs20_conn_capab hs20_oper_friendly_name

	[ -n "$osu_ssid" ] && append bss_conf "osu_ssid=\"$osu_ssid\"" "$N"
	[ -n "$osu_server_uri" ] && append bss_conf "osu_server_uri=$osu_server_uri" "$N"
	[ -n "$osu_friendly_name" ] && {
		[ "$osu_friendly_name" != ":" ] && append bss_conf "osu_friendly_name=$osu_friendly_name" "$N"
	}
	[ -n "$osu_nai" ] && {
		append bss_conf "osu_nai=$osu_nai" "$N"
		append bss_conf "osu_nai2=$osu_nai" "$N"
	}
	[ -n "$osu_method_list" ] && {
		osu_method_string=""
		json_select osu_method_list
			json_get_keys osu_method_list_idxs
			for key in $osu_method_list_idxs; do 
				json_get_var value "$key"
				osu_method_string="$osu_method_string $value"
			done
		json_select ".."
		osu_method_string="$(echo "$osu_method_string" | xargs)"

		append bss_conf "osu_method_list=$osu_method_string" "$N"
	}
	[ -n "$osu_service_desc" ] && {
		[ "$osu_service_desc" != ":" ] && append bss_conf "osu_service_desc=$osu_service_desc" "$N"
	} 

	set_default hs20 0
	set_default disable_dgaf $hs20
	set_default osen 0
	set_default anqp_domain_id 0
	set_default hs20_deauth_req_timeout 60
	if [ "$hs20" -eq "1" -a "$wpa" -ge 2 ]; then
		append bss_conf "hs20=$hs20" "$N"
		append_hs20_icons
		append bss_conf "disable_dgaf=$disable_dgaf" "$N"
		append bss_conf "osen=$osen" "$N"
		append bss_conf "anqp_domain_id=$anqp_domain_id" "$N"
		append bss_conf "hs20_deauth_req_timeout=$hs20_deauth_req_timeout" "$N"
		[ -n "$hs20_operating_class" ] && append bss_conf "hs20_operating_class=$hs20_operating_class" "$N"
		[ -n "$hs20_t_c_filename" ] && append bss_conf "hs20_t_c_filename=$hs20_t_c_filename" "$N"
		[ -n "$hs20_t_c_timestamp" ] && append bss_conf "hs20_t_c_timestamp=$hs20_t_c_timestamp" "$N"
		[ -n "$hs20_t_c_server_url" ] && append bss_conf "hs20_t_c_server_url=$hs20_t_c_server_url" "$N"
		[ -n "$hs20_wan_status" ] && {
			append bss_conf "hs20_wan_metrics=${hs20_wan_status}:${hs20_wan_dw_speed:-0}:${hs20_wan_up_speed:-0}:0:0:0" "$N"
		}

		json_for_each_item append_oper_friendly_name hs20_oper_friendly_name
		json_for_each_item append_conn_capab hs20_conn_capab
		json_for_each_item append_operator_icon operator_icon
	fi

	set_default multicast_to_unicast 0
	if [ "$multicast_to_unicast" -gt 0 ]; then
		append bss_conf "multicast_to_unicast=$multicast_to_unicast" "$N"
	fi

	set_default per_sta_vif 0
	if [ "$per_sta_vif" -gt 0 ]; then
		append bss_conf "per_sta_vif=$per_sta_vif" "$N"
	fi

	json_get_values opts hostapd_bss_options
	for val in $opts; do
		append bss_conf "$val" "$N"
	done

	bss_md5sum=$(echo $bss_conf | md5sum | cut -d" " -f1)
	append bss_conf "config_id=$bss_md5sum" "$N"

	append "$var" "$bss_conf" "$N"
	return 0
}

hostapd_set_log_options() {
	local var="$1"

	local log_level log_80211 log_8021x log_radius log_wpa log_driver log_iapp log_mlme
	json_get_vars log_level log_80211 log_8021x log_radius log_wpa log_driver log_iapp log_mlme

	set_default log_level 2
	set_default log_80211  1
	set_default log_8021x  1
	set_default log_radius 1
	set_default log_wpa    1
	set_default log_driver 1
	set_default log_iapp   1
	set_default log_mlme   1

	local log_mask=$(( \
		($log_80211  << 0) | \
		($log_8021x  << 1) | \
		($log_radius << 2) | \
		($log_wpa    << 3) | \
		($log_driver << 4) | \
		($log_iapp   << 5) | \
		($log_mlme   << 6)   \
	))

	append "$var" "logger_syslog=$log_mask" "$N"
	append "$var" "logger_syslog_level=$log_level" "$N"
	append "$var" "logger_stdout=$log_mask" "$N"
	append "$var" "logger_stdout_level=$log_level" "$N"

	return 0
}

_wpa_supplicant_common() {
	local ifname="$1"

	_rpath="/var/run/wpa_supplicant"
	_config="${_rpath}-$ifname.conf"
}

wpa_supplicant_teardown_interface() {
	_wpa_supplicant_common "$1"
	rm -rf "$_rpath/$1" "$_config"
}

wpa_supplicant_prepare_interface() {
	local ifname="$1"
	_w_driver="$2"

	_wpa_supplicant_common "$1"

	json_get_vars mode wds multi_ap

	[ -n "$network_bridge" ] && {
		fail=
		case "$mode" in
			adhoc)
				fail=1
			;;
			sta)
				[ "$wds" = 1 -o "$multi_ap" = 1 ] || fail=1
			;;
		esac

		[ -n "$fail" ] && {
			wireless_setup_vif_failed BRIDGE_NOT_ALLOWED
			return 1
		}
	}

	local ap_scan=

	_w_mode="$mode"

	[ "$mode" = adhoc ] && {
		ap_scan="ap_scan=2"
	}

	local country_str=
	[ -n "$country" ] && {
		country_str="country=$country"
	}

	multiap_flag_file="${_config}.is_multiap"
	if [ "$multi_ap" = "1" ]; then
		touch "$multiap_flag_file"
	else
		[ -e "$multiap_flag_file" ] && rm "$multiap_flag_file"
	fi
	wpa_supplicant_teardown_interface "$ifname"
	cat > "$_config" <<EOF
${scan_list:+freq_list=$scan_list}
$ap_scan
$country_str
EOF
	return 0
}

wpa_supplicant_set_fixed_freq() {
	local freq="$1"
	local htmode="$2"

	append network_data "fixed_freq=1" "$N$T"
	append network_data "frequency=$freq" "$N$T"
	case "$htmode" in
		NOHT) append network_data "disable_ht=1" "$N$T";;
		HE20|HT20|VHT20) append network_data "disable_ht40=1" "$N$T";;
		HT40*|VHT40|VHT80|VHT160|HE40|HE80|HE160) append network_data "ht40=1" "$N$T";;
	esac
	case "$htmode" in
		VHT*) append network_data "vht=1" "$N$T";;
	esac
	case "$htmode" in
		HE80|VHT80) append network_data "max_oper_chwidth=1" "$N$T";;
		HE160|VHT160) append network_data "max_oper_chwidth=2" "$N$T";;
		HE20|HE40|VHT20|VHT40) append network_data "max_oper_chwidth=0" "$N$T";;
		*) append network_data "disable_vht=1" "$N$T";;
	esac
}

wpa_supplicant_add_network() {
	local ifname="$1"
	local freq="$2"
	local htmode="$3"
	local noscan="$4"

	local wpa_md5sum wpa_network_config

	_wpa_supplicant_common "$1"
	wireless_vif_parse_encryption

	json_get_vars \
		ssid bssid key \
		basic_rate mcast_rate \
		ieee80211w ieee80211r \
		multi_ap \
		bgscan short_interval \
		signal_thresh \
		long_interval \
		db_permanent

	case "$auth_type" in
		sae|owe|eap192|eap-eap192)
			set_default ieee80211w 2
		;;
		psk-sae)
			set_default ieee80211w 1
		;;
	esac

	set_default ieee80211r 0
	set_default multi_ap 0
	set_default short_interval 30
	set_default signal_thresh -45
	set_default long_interval 300
	set_default db_permanent 0


	local key_mgmt='NONE'
	local network_data=
	local T="	"

	local scan_ssid="scan_ssid=1"
	local freq wpa_key_mgmt

	[ "$_w_mode" = "adhoc" ] && {
		append network_data "mode=1" "$N$T"
		[ -n "$freq" ] && wpa_supplicant_set_fixed_freq "$freq" "$htmode"
		[ "$noscan" = "1" ] && append network_data "noscan=1" "$N$T"

		scan_ssid="scan_ssid=0"

		[ "$_w_driver" = "nl80211" ] ||	append wpa_key_mgmt "WPA-NONE"
	}

	[ "$_w_mode" = "mesh" ] && {
		json_get_vars mesh_id mesh_fwding mesh_rssi_threshold
		[ -n "$mesh_id" ] && ssid="${mesh_id}"

		append network_data "mode=5" "$N$T"
		[ -n "$mesh_fwding" ] && append network_data "mesh_fwding=${mesh_fwding}" "$N$T"
		[ -n "$mesh_rssi_threshold" ] && append network_data "mesh_rssi_threshold=${mesh_rssi_threshold}" "$N$T"
		[ -n "$freq" ] && wpa_supplicant_set_fixed_freq "$freq" "$htmode"
		[ "$noscan" = "1" ] && append network_data "noscan=1" "$N$T"
		append wpa_key_mgmt "SAE"
		scan_ssid=""
	}

	[ "$multi_ap" = 1 -a "$_w_mode" = "sta" ] && append network_data "multi_ap_backhaul_sta=1" "$N$T"

	case "$auth_type" in
		none) ;;
		owe)
			hostapd_append_wpa_key_mgmt
			key_mgmt="$wpa_key_mgmt"
		;;
		wep)
			local wep_keyidx=0
			hostapd_append_wep_key network_data
			append network_data "wep_tx_keyidx=$wep_keyidx" "$N$T"
		;;
		wps)
			key_mgmt='WPS'
		;;
		psk|sae|psk-sae)
			local passphrase

			if [ "$_w_mode" != "mesh" ]; then
				hostapd_append_wpa_key_mgmt
			fi

			key_mgmt="$wpa_key_mgmt"

			if [ ${#key} -eq 64 ]; then
				passphrase="psk=${key}"
			else
				if [ "$_w_mode" = "mesh" ]; then
					passphrase="sae_password=\"${key}\""
				else
					passphrase="psk=\"${key}\""
				fi
			fi
			append network_data "$passphrase" "$N$T"
		;;
		eap|eap192|eap-eap192)
			hostapd_append_wpa_key_mgmt
			key_mgmt="$wpa_key_mgmt"

			json_get_vars eap_type identity anonymous_identity ca_cert ca_cert_usesystem

			if [ "$ca_cert_usesystem" -eq "1" -a -f "/etc/ssl/certs/ca-certificates.crt" ]; then
				append network_data "ca_cert=\"/etc/ssl/certs/ca-certificates.crt\"" "$N$T"
			else
				[ -n "$ca_cert" ] && append network_data "ca_cert=\"$ca_cert\"" "$N$T"
			fi
			[ -n "$identity" ] && append network_data "identity=\"$identity\"" "$N$T"
			[ -n "$anonymous_identity" ] && append network_data "anonymous_identity=\"$anonymous_identity\"" "$N$T"
			case "$eap_type" in
				tls)
					json_get_vars client_cert priv_key priv_key_pwd use_pkcs pkcs_cert pkcs_passwd

					if [ "$use_pkcs" = "1" ]; then
						append network_data "private_key=\"$pkcs_cert\"" "$N$T"
						append network_data "private_key_passwd=\"$pkcs_passwd\"" "$N$T"
					else
						append network_data "client_cert=\"$client_cert\"" "$N$T"
						append network_data "private_key=\"$priv_key\"" "$N$T"
						append network_data "private_key_passwd=\"$priv_key_pwd\"" "$N$T"
					fi

					json_get_vars subject_match
					[ -n "$subject_match" ] && append network_data "subject_match=\"$subject_match\"" "$N$T"

					json_get_values altsubject_match altsubject_match
					if [ -n "$altsubject_match" ]; then
						local list=
						for x in $altsubject_match; do
							append list "$x" ";"
						done
						append network_data "altsubject_match=\"$list\"" "$N$T"
					fi

					json_get_values domain_match domain_match
					if [ -n "$domain_match" ]; then
						local list=
						for x in $domain_match; do
							append list "$x" ";"
						done
						append network_data "domain_match=\"$list\"" "$N$T"
					fi

					json_get_values domain_suffix_match domain_suffix_match
					if [ -n "$domain_suffix_match" ]; then
						local list=
						for x in $domain_suffix_match; do
							append list "$x" ";"
						done
						append network_data "domain_suffix_match=\"$list\"" "$N$T"
					fi
				;;
				fast|peap|ttls)
					json_get_vars auth password ca_cert2 ca_cert2_usesystem client_cert2 priv_key2 priv_key2_pwd
					set_default auth MSCHAPV2

					if [ "$auth" = "EAP-TLS" ]; then
						if [ "$ca_cert2_usesystem" -eq "1" -a -f "/etc/ssl/certs/ca-certificates.crt" ]; then
							append network_data "ca_cert2=\"/etc/ssl/certs/ca-certificates.crt\"" "$N$T"
						else
							[ -n "$ca_cert2" ] && append network_data "ca_cert2=\"$ca_cert2\"" "$N$T"
						fi
						append network_data "client_cert2=\"$client_cert2\"" "$N$T"
						append network_data "private_key2=\"$priv_key2\"" "$N$T"
						append network_data "private_key2_passwd=\"$priv_key2_pwd\"" "$N$T"
					else
						append network_data "password=\"$password\"" "$N$T"
					fi

					json_get_vars subject_match
					[ -n "$subject_match" ] && append network_data "subject_match=\"$subject_match\"" "$N$T"

					json_get_values altsubject_match altsubject_match
					if [ -n "$altsubject_match" ]; then
						local list=
						for x in $altsubject_match; do
							append list "$x" ";"
						done
						append network_data "altsubject_match=\"$list\"" "$N$T"
					fi

					json_get_values domain_match domain_match
					if [ -n "$domain_match" ]; then
						local list=
						for x in $domain_match; do
							append list "$x" ";"
						done
						append network_data "domain_match=\"$list\"" "$N$T"
					fi

					json_get_values domain_suffix_match domain_suffix_match
					if [ -n "$domain_suffix_match" ]; then
						local list=
						for x in $domain_suffix_match; do
							append list "$x" ";"
						done
						append network_data "domain_suffix_match=\"$list\"" "$N$T"
					fi

					phase2proto="auth="
					case "$auth" in
						"auth"*)
							phase2proto=""
						;;
						"EAP-"*)
							auth="$(echo $auth | cut -b 5- )"
							[ "$eap_type" = "ttls" ] &&
								phase2proto="autheap="
							json_get_vars subject_match2
							[ -n "$subject_match2" ] && append network_data "subject_match2=\"$subject_match2\"" "$N$T"

							json_get_values altsubject_match2 altsubject_match2
							if [ -n "$altsubject_match2" ]; then
								local list=
								for x in $altsubject_match2; do
									append list "$x" ";"
								done
								append network_data "altsubject_match2=\"$list\"" "$N$T"
							fi

							json_get_values domain_match2 domain_match2
							if [ -n "$domain_match2" ]; then
								local list=
								for x in $domain_match2; do
									append list "$x" ";"
								done
								append network_data "domain_match2=\"$list\"" "$N$T"
							fi

							json_get_values domain_suffix_match2 domain_suffix_match2
							if [ -n "$domain_suffix_match2" ]; then
								local list=
								for x in $domain_suffix_match2; do
									append list "$x" ";"
								done
								append network_data "domain_suffix_match2=\"$list\"" "$N$T"
							fi
						;;
					esac
					append network_data "phase2=\"$phase2proto$auth\"" "$N$T"
				;;
			esac
			append network_data "eap=$(echo $eap_type | tr 'a-z' 'A-Z')" "$N$T"
		;;
	esac

	case "$wpa_cipher" in
		*GCMP*)
			append network_data "pairwise=$wpa_cipher" "$N$T"
			append network_data "group=$wpa_cipher" "$N$T"
		;;
	esac

	[ "$mode" = mesh ] || {
		case "$wpa" in
			1)
				append network_data "proto=WPA" "$N$T"
			;;
			2)
				append network_data "proto=RSN" "$N$T"
			;;
		esac

		case "$ieee80211w" in
			[012])
				[ "$wpa" -ge 2 ] && append network_data "ieee80211w=$ieee80211w" "$N$T"
			;;
		esac
	}
	[ -n "$bssid" ] && append network_data "bssid=$bssid" "$N$T"
	[ -n "$beacon_int" ] && append network_data "beacon_int=$beacon_int" "$N$T"

	local bssid_blacklist bssid_whitelist
	json_get_values bssid_blacklist bssid_blacklist
	json_get_values bssid_whitelist bssid_whitelist

	[ -n "$bssid_blacklist" ] && append network_data "bssid_blacklist=$bssid_blacklist" "$N$T"
	[ -n "$bssid_whitelist" ] && append network_data "bssid_whitelist=$bssid_whitelist" "$N$T"

	[ -n "$basic_rate" ] && {
		local br rate_list=
		for br in $basic_rate; do
			wpa_supplicant_add_rate rate_list "$br"
		done
		[ -n "$rate_list" ] && append network_data "rates=$rate_list" "$N$T"
	}

	[ -n "$mcast_rate" ] && {
		local mc_rate=
		wpa_supplicant_add_rate mc_rate "$mcast_rate"
		append network_data "mcast_rate=$mc_rate" "$N$T"
	}

	[ -n "$bgscan" ] && {
		local db_fname
		[ "$bgscan" = "learn" ] && {
			if [ "$db_permanent" = "0" ]; then
				db_fname="/tmp/wpa_supplicant/$ifname.bgscan"
			else
				db_fname="/etc/wpa_supplicant/$ifname.bgscan"
			fi
			local db_dir=$(dirname "${db_fname}")
			[ -d "$db_dir" ] || {
				/bin/mkdir "$db_dir"
			}
			db_fname=":$db_fname"
		}
		append network_data "bgscan=\"$bgscan:$short_interval:$signal_thresh:$long_interval$db_fname\"" "$N$T"
	}


	if [ "$key_mgmt" = "WPS" ]; then
		echo "wps_cred_processing=1" >> "$_config"
	else
		read -r -d '' wpa_network_config <<EOF
network={
	$scan_ssid
	ssid="$ssid"
	key_mgmt=$key_mgmt
	$network_data
}
EOF
		wpa_md5sum=$(echo $wpa_network_config | md5sum | cut -d" " -f1)
		append network_data "config_id=\"$wpa_md5sum\"" "$N$T"

		cat >> "$_config" <<EOF
network={
	$scan_ssid
	ssid="$ssid"
	key_mgmt=$key_mgmt
	$network_data
}
EOF
	fi
	return 0
}
