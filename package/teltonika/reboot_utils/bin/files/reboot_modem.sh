#!/bin/sh

. /usr/share/libubox/jshn.sh

MODEM_ID=""
MODEM_NUM=""

get_modem_num() {

    local modem="$1"
    local found_modem=""

    local modem_objs=$(ubus list gsm.*)

    for i in $modem_objs
    do
        found_modem=$(ubus call "$i" info 2> /dev/null | grep usb_id | sed 's/.* //g')
        [ "$modem" == "${found_modem:1:-2}" ] && echo "${i//[!0-9]/}" && return 0
    done

    return 1
}

get_modem() {
    local modem modems id builtin primary
    local primary_modem=""
    local builtin_modem=""
    json_init
    json_load "$(cat /etc/board.json)"
    json_get_keys modems modems
    json_select modems

    for modem in $modems; do
        json_select "$modem"
        json_get_vars builtin primary id
        if [ "$builtin" -a "$id" ]; then
            builtin_modem=$id
        fi
        if [ "$primary" -a "$id" ]; then
            primary_modem=$id
            break
        fi
        json_select ..
    done

    if [ "$primary_modem" = "" ]; then
        if [ "$builtin_modem" = "" ]; then
            json_load "$(/bin/ubus call gsm.modem0 info)"
            json_get_vars usb_id
            primary_modem="$usb_id"
        else
            primary_modem=$builtin_modem
        fi
    fi
    MODEM_ID=$primary_modem
}

if [[ -z "$1" ]]; then
	get_modem
else
	MODEM_ID="$1"
fi

MODEM_NUM=$(get_modem_num "$MODEM_ID")

/bin/ubus call mctl reboot "{\"num\":$MODEM_NUM}"
