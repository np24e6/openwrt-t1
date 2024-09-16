#!/bin/sh

. /lib/functions.sh
. /usr/share/libubox/jshn.sh

[ "$#" -ne 1 ] && {
	echo "Usage: multiple_ap.sh <timeout>"
	exit 1
}

TIMEOUT="$1"
PRIORITY=0
SSID=""
ENCRYPTION=""
AP_JSON="{}"
MULTI_AP_IFACE=""

get_wifi_section() {
	local multiple
	local section="$1"
	config_get multiple "$section" multiple ""
	[ "$multiple" = "1" ] || return
	SECTION="$1"
	config_get DEVICE "$section" device ""
	config_get WIFI_ID "$section" wifi_id ""
}

find_available() {
	local enabled ssid_name priority key encryption
	local is_available="0"
	local section="$1"

	config_get enabled "$section" "enabled" 0
	config_get ssid_name "$section" "ssid" ""
	config_get priority "$section" "priority" 0

	[ "$enabled" -ne 1 ] || [ "$priority" -eq 0 ] || [ -z "$ssid_name" ] && return
	config_get key "$section" key ""
	for ap in $LIST; do
		json_select "$ap"
		json_get_var ssid ssid
		owe=0 sae=0 psk=0 wpa1=0 wpa2=0 ccmp=0 tkip=0 aes=0

		[ "$ssid" != "$ssid_name" ] && {
			json_select ..
			continue
		}

		is_available="1"
		json_select encryption
		json_get_vars enabled
		[ "$enabled" ] && [ "$enabled" -ne 0 ] || {
			encryption="none"
			json_select ..
			json_select ..
			continue
		}

		json_select authentication
		idx=1
		while json_is_a ${idx} string; do
			json_get_var auth $idx
			[ "$auth" = "owe" ] && owe=1 && break
			[ "$auth" = "sae" ] && sae=1
			[ "$auth" = "psk" ] && psk=1
			idx=$((idx + 1))
		done
		json_select ..

		json_select wpa
		idx=1
		while json_is_a ${idx} int; do
			json_get_var wpa_psk $idx
			[ "$wpa_psk" -eq 1 ] && wpa1=1
			[ "$wpa_psk" -eq 2 ] && wpa2=1
			idx=$((idx + 1))
		done
		json_select ..

		json_select ciphers
		idx=1
		while json_is_a ${idx} string; do
			json_get_var cip $idx
			[ "$cip" = "ccmp" ] && ccmp=1
			[ "$cip" = "tkip" ] && tkip=1
			[ "$cip" = "aes" ] && aes=1
			idx=$((idx + 1))
		done

		json_select ..
		json_select ..
		json_select ..

		[ "$owe" -eq 1 ] && encryption="owe" && continue
		[ "$sae" -eq 1 ] && {
			encryption="sae"
			[ "$psk" -eq 1 ] && encryption="$encryption-mixed"
			continue
		}
		[ "$psk" -eq 1 ] && {
			encryption="psk"
			[ "$wpa1" -eq 1 ] && [ "$wpa2" -eq 1 ] && encryption="$encryption-mixed"
			[ "$wpa1" -eq 0 ] && [ "$wpa2" -eq 1 ] && encryption="${encryption}2"
			[ "$tkip" -eq 1 ] && encryption="$encryption+tkip"
			[ "$ccmp" -eq 1 ] && encryption="$encryption+ccmp" && continue
			[ "$aes" -eq 1 ] && encryption="$encryption+aes" && continue
		}

	done

	[ "$is_available" -eq 0 ] && return
	[ -z "$encryption" ] && return
	[ "$PRIORITY" -ne 0 ] && [ "$PRIORITY" -lt "$priority" ] && {
		return
	}
	local temp_json="$(json_dump)"
	json_load "$AP_JSON"
	json_select "$ssid_name"
	json_get_vars password_correct
	[ "$password_correct" -eq 2 ] && {
		check_connection "$ssid_name" "$key" "$encryption"
		password_correct=$?
	}

	[ "$password_correct" -eq 1 ] && {
		PRIORITY="$priority"
		SSID="$ssid_name"
		KEY="$key"
		ENCRYPTION="$encryption"
	}

	json_load "$temp_json"
	json_get_keys LIST results
	json_select results
}

check_connection() {
	local ssid key encryption
	ssid="$1"
	key="$2"
	encryption="$3"

	write_wireless_config "$ssid" "$key" "$encryption"
	[ -z "$MULTI_AP_IFACE" ] && {
		get_multi_ap_iface
		json_load "$AP_JSON"
		json_select "$ssid"
		json_get_vars password_correct
	}
	local i=0

	while [ "$i" -le 25 ]; do
		sleep 5

		[ -n "$MULTI_AP_IFACE" ] && ubus wait_for wpa_supplicant.$MULTI_AP_IFACE
		local output=$(/bin/ubus call wpa_supplicant.$MULTI_AP_IFACE get_status)
		[ -z "$output" ] && {
			i=$(($i + 5))
			continue
		}

		json_load "$output"
		json_get_vars wpa_state disconnect_reason
		json_load "$AP_JSON"
		json_select "$ssid"
		[ "$wpa_state" = "COMPLETED" ] && {
			json_add_int "password_correct" "1"
			break
		}
		[ "$disconnect_reason" -eq 15 ] && {
			json_add_int "password_correct" "0"
			break
		}
		i=$(($i + 5))

	done

	json_get_vars password_correct
	json_select ..
	AP_JSON="$(json_dump)"
	return "$password_correct"
}

write_wireless_config() {
	local ssid key encryption
	ssid="$1"
	key="$2"
	encryption="$3"
	[ -z "$ssid" ] || [ -z "$encryption" ] && return
	uci_set "wireless" "$SECTION" "ssid" "$ssid"
	uci_remove "wireless" "$SECTION" "bssid"
	uci_set "wireless" "$SECTION" "encryption" "$encryption"
	[ "$encryption" != "none" ] && [ "$encryption" != "owe" ] && uci_set "wireless" "$SECTION" "key" "$key"
	uci_set "wireless" "$SECTION" "disabled" "0"
	uci_commit "wireless"
	ubus call network reload
	[ -z "$MULTI_AP_IFACE" ] && sleep 10 || ubus wait_for wpa_supplicant.$MULTI_AP_IFACE
}

add_ap_to_json() {
	local ap_ssid
	config_get ap_ssid "$1" ssid
	json_load "$AP_JSON"
	json_add_object "$ap_ssid"
	json_add_int "password_correct" "2" #  0 incorrect, 1 correct, 2 not tested
	json_close_object
	AP_JSON="$(json_dump)"
}

get_multi_ap_iface() {
	status_json="$(wifi status)"
	json_load "$status_json"
	json_select "$DEVICE"
	json_select interfaces
	idx=1
	while json_is_a ${idx} object; do
		json_select $idx
		json_get_var ifname ifname
		json_select config
		json_get_var wifi_id wifi_id
		json_select ..
		json_select ..
		[ "$wifi_id" = "$WIFI_ID" ] && {
			MULTI_AP_IFACE="$ifname"
			break
		}
		idx=$((idx + 1))
	done
}

config_load "wireless"
config_foreach get_wifi_section "wifi-iface"
[ -z "$SECTION" ] && {
	echo "No multiple AP wifi section created in wireless configuration"
	exit 1
}
[ -z "$DEVICE" ] && {
	echo "No device found in wireless configuration"
	exit 1
}

ssid_exists=$(uci_get "multi_wifi" "@wifi-iface[0]")
ssid_disabled=$(uci_get "wireless" "$SECTION" "disabled")
[ -z "$ssid_exists" ] && [ "$ssid_disabled" = "0" ] && uci_set "wireless" "$SECTION" "disabled" "1" \
	&& uci_commit "wireless" && wifi up
multi_ap_enabled=$(uci_get "multi_wifi" "general" "enabled")
[ "$multi_ap_enabled" != 1 ] && exit 1

json_init
config_load "multi_wifi"
config_foreach add_ap_to_json "wifi-iface"
while uci_get "multi_wifi" "@wifi-iface[0]" >/dev/null; do
	[ -n "$MULTI_AP_IFACE" ] && {
		ubus wait_for wpa_supplicant.$MULTI_AP_IFACE
		output=$(/bin/ubus call wpa_supplicant.$MULTI_AP_IFACE get_status)
		[ -z "$output" ] && sleep $TIMEOUT && continue

		json_load "$output"
		json_get_vars wpa_state disconnect_reason
		[ "$wpa_state" = "COMPLETED" ] && [ -n "$SSID" ] && [ "$PRIORITY" -eq 1 ] && sleep "$TIMEOUT" && continue
		json_load "$AP_JSON"
		json_select "$SSID"
		[ "$disconnect_reason" -ne 0 ] && [ "$wpa_state" != "COMPLETED" ] && {
			if [ "$disconnect_reason" -eq 15 ]; then # reason=15 "4-Way handshake timeout"
				json_add_int "password_correct" "0"
			else
				json_add_int "password_correct" "2"
			fi
		}
		json_select ..
		AP_JSON="$(json_dump)"
	}

	output=$(/bin/ubus call iwinfo scan "{\"device\":\"$DEVICE\"}")
	[ -z "$output" ] && sleep $TIMEOUT && continue
	json_load "$output"
	json_get_keys LIST results
	json_select results
	PRIORITY=0

	config_foreach find_available "wifi-iface"
	[ "$PRIORITY" -eq 0 ] || [ -z "$SSID" ] || [ -z "$ENCRYPTION" ] && sleep "$TIMEOUT" && continue
	write_wireless_config "$SSID"  "$KEY" "$ENCRYPTION" && sleep "$TIMEOUT"
done

exit 0
