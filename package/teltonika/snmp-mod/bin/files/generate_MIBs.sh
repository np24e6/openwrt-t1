#!/bin/sh

MODULES_DIR=/etc/snmp/modules

. /lib/functions.sh

err_and_exit() {
	logger -st "$0" -p 3 "$1"
	exit
}

is_true() {
	local var
	json_get_var var "$1"
	[ "$var" -eq 1 ]
}

is_installed() {
	[ -f "/usr/lib/opkg/info/${1}.control" ]
}

is_switch() {
	mnf_info -n | grep "TSW" > /dev/null 2>&1
}

is_trb5() {
	mnf_info -n | grep "TRB5" > /dev/null 2>&1
}

is_swm() {
	mnf_info -n | grep "SWM" > /dev/null 2>&1
}

get_device_name() {
	local name

	config_load system
	config_get name system "devicename"

	[ -z "$name" ] && name=$(mnf_info -n 2>/dev/null | cut -c -6)
	[ -z "$name" ] && {
		name="Teltonika"
		logger -st "$0" -p 4 "Failed to get device name, defaulting to '$name'"
	}

	echo "$name"
}

get_io() {
	local io_list
	local counter=0

	while which iomand >/dev/null 2>&1 && [ -z "$io_list" ]; do
		io_list=$(ubus list ioman.* 2>/dev/null | awk -F'.' '{print $3}')

		[ "$counter" -eq 1 ] && logger -t "$0" "Waiting for ioman to appear on ubus"
		counter=$((counter + 1))
		sleep 1
	done
	logger -t "$0" "Found ioman on ubus"
	echo "$io_list"
}

get_traps_io_mib() {
	local mib_name

	for io in $(get_io); do
		mib_name=traps_mib${1}${io}
		eval "echo -n \"\$$mib_name\""
	done
}

device_name=$(get_device_name)
MIB_file="/etc/snmp/${device_name}.mib"
board_json_file="/etc/board.json"

[ -e "$board_json_file" ] || err_and_exit "File '$board_json_file' not found"

. /usr/share/libubox/jshn.sh

json_init
json_load_file "$board_json_file" || err_and_exit "Failed to load $board_json_file"
json_select "hwinfo" || err_and_exit "'hwinfo' object not found in $board_json_file"

unset device
unset mobile
unset mdcollect
unset gps
unset traps
unset hotspot
unset ios
unset wireless
unset vlan_gen
unset port_vlan
unset if_vlan
unset sqm
unset port
unset iface

device=1
# To support external modems, mobile MIB should also be included
# on devices that don't have mobile modems but have a USB port.
(is_true "mobile" || is_true "usb") && mobile=1
is_installed mdcollectd && mdcollect=1
is_true "gps" && gps=1
traps=1
iface=1
# Hotspot is available on all devices except TSW switches
( ! is_switch ) && ( ! is_swm ) && hotspot=1
is_true "ios" && ios=1
is_true "wifi" && wireless=1
! is_switch && if_vlan=1
( jsonfilter -q -i $board_json_file -e "$.switch" 1>/dev/null || is_switch || is_true "dsa") && port_vlan=1
[ $if_vlan -eq 1 ]  || [ $port_vlan -eq 1 ] && vlan_gen=1
( ! is_switch ) && ( ! is_trb5 ) && ( ! is_swm ) && sqm=1
(is_installed "port_eventsd") && port=1

# Unset 'traps' if neither 'mobile' nor 'ios' or 'hotspot' are supported
traps=${mobile:-${ios:-${hotspot:-''}}}

export MODULES_DIR="$MODULES_DIR"

# Import variables for traps' MIB definitions
. "$MODULES_DIR/traps"
# Read definitions of other MIBs directly
device_mib=$(cat $MODULES_DIR/device.mib)
mobile_mib=$($MODULES_DIR/mobile.sh "$mdcollect")
gps_mib=$(cat $MODULES_DIR/gps.mib)
hotspot_mib=$(cat $MODULES_DIR/hotspot.mib)
io_mib=$(cat $MODULES_DIR/io.mib)
wireless_mib=$(cat $MODULES_DIR/wireless.mib)
if_vlan_mib=$(cat $MODULES_DIR/vlan_if.mib)
iface_mib=$(cat $MODULES_DIR/iface.mib)

if is_switch; then
	port_vlan_mib=$(cat $MODULES_DIR/vlan_port_switch.mib)
elif is_true "dsa"; then
	port_vlan_mib=$(cat $MODULES_DIR/vlan_port_dsa.mib)
else
	port_vlan_mib=$(cat $MODULES_DIR/vlan_port_non_dsa.mib)
fi

sqm_mib=$(cat $MODULES_DIR/sqm.mib)
port_mib=$(cat $MODULES_DIR/port.mib)

beginning_mib="TELTONIKA-MIB DEFINITIONS ::= BEGIN

IMPORTS
	OBJECT-TYPE, NOTIFICATION-TYPE, MODULE-IDENTITY,
	Integer32, enterprises, Counter64,
	IpAddress, Unsigned32				FROM SNMPv2-SMI
	DisplayString,
	PhysAddress					FROM SNMPv2-TC
	OBJECT-GROUP, NOTIFICATION-GROUP		FROM SNMPv2-CONF;

teltonika MODULE-IDENTITY
	LAST-UPDATED	\"$(date "+%Y%m%d%H%MZ")\"
	ORGANIZATION	\"TELTONIKA\"
	CONTACT-INFO	\"TELTONIKA\"
	DESCRIPTION	\"The MIB module for TELTONIKA ${device_name} routers.\"
	REVISION	\"$(date "+%Y%m%d%H%MZ")\"
	DESCRIPTION	\"Latest version\"
	::= { enterprises 48690 }
	
teltonikaSnmpGroups	OBJECT IDENTIFIER ::= { teltonika 0 }"
end_mib='END'

echo -e "${beginning_mib}
${device:+device			OBJECT IDENTIFIER ::= { teltonika 1 \}\n}\
${mobile:+mobile			OBJECT IDENTIFIER ::= { teltonika 2 \}\n}\
${gps:+gps			OBJECT IDENTIFIER ::= { teltonika 3 \}\n}\
${traps:+notifications		OBJECT IDENTIFIER ::= { teltonika 4 \}\n}\
${traps:+${mobile:+mobileNotifications	OBJECT IDENTIFIER ::= { notifications 1 \}\n}}\
${traps:+${ios:+ioNotifications	OBJECT IDENTIFIER ::= { notifications 2 \}\n}}\
${traps:+${hotspot:+hotspotNotifications	OBJECT IDENTIFIER ::= { notifications 3 \}\n}}\
${traps:+eventNotifications	OBJECT IDENTIFIER ::= { notifications 4 \}\n}\
${hotspot:+hotspot		OBJECT IDENTIFIER ::= { teltonika 5 \}\n}\
${ios:+io			OBJECT IDENTIFIER ::= { teltonika 6 \}\n}\
${wireless:+wireless		OBJECT IDENTIFIER ::= { teltonika 7 \}\n}\
${vlan_gen:+vlan			OBJECT IDENTIFIER ::= { teltonika 8 \}\n}\
${sqm:+sqm			OBJECT IDENTIFIER ::= { teltonika 9 \}\n}\
${port:+port			OBJECT IDENTIFIER ::= { teltonika 10 \}\n}\
${iface:+iface			OBJECT IDENTIFIER ::= { teltonika 11 \}\n}\

${device:+$device_mib\n\n}\
${mobile:+$mobile_mib\n\n}\
${gps:+$gps_mib\n\n}\
${traps:+-- Traps --\n\
\n\
trapGroup NOTIFICATION-GROUP\n\
	NOTIFICATIONS {\n\
		${mobile:+$traps_mib_group_gsm\n}\
		${ios:+$(get_traps_io_mib _group_)\n}\
		${hotspot:+$traps_mib_group_hotspot\n}\
		eventLogNotification \}\n\
	STATUS current\n\
	DESCRIPTION \"Traps SNMP group defined according to RFC 2580\"\n\
	::= { teltonikaSnmpGroups 4 \}\n\
	${mobile:+$traps_mib_gsm}${ios:+$(get_traps_io_mib _)\n}\
	${hotspot:+$traps_mib_hotspot}${traps_mib_eventlog}\n}\
${hotspot:+$hotspot_mib\n\n}\
${ios:+$io_mib\n\n}\
${wireless:+$wireless_mib\n\n}\
${if_vlan:+$if_vlan_mib\n\n}\
${port_vlan:+$port_vlan_mib\n\n}\
${sqm:+$sqm_mib\n\n}\
${port:+$port_mib\n\n}\
${iface:+$iface_mib\n\n}\

${end_mib}" >"$MIB_file"

rm -r "$MODULES_DIR"
