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

get_router_name() {
	local name

	config_load system
	config_get name system "routername"

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
		mib_name=traps_mib_${io}
		eval "echo -n \"\$$mib_name\""
	done
}

router_name=$(get_router_name)
MIB_file="/etc/snmp/${router_name}.mib"

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
unset port_vlan
unset if_vlan
unset sqm
unset port

device=1
# To support external modems, mobile MIB should also be included
# on devices that don't have mobile modems but have a USB port.
(is_true "mobile" || is_true "usb") && mobile=1
is_installed mdcollectd && mdcollect=1
is_true "gps" && gps=1
traps=1
# There are devices that don't have wifi but have hotspot package
# installed (e.g. RUTX08). Include hotspot for them as well.
(is_true "wifi" || is_installed "coova-chilli") && hotspot=1
is_true "ios" && ios=1
is_true "wifi" && wireless=1
# assuming that interface-based VLAN support is implicitly available for any layer3 device
if_vlan=1
( jsonfilter -q -i $board_json_file -e "$.switch" 1>/dev/null ) && port_vlan=1
sqm=1
(is_installed "port_eventsd") && port=1

# Unset 'traps' if neither 'mobile' nor 'ios' are supported
traps=${mobile:-${ios:-''}}

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
port_vlan_mib=$(cat $MODULES_DIR/vlan_port.mib)
sqm_mib=$(cat $MODULES_DIR/sqm.mib)
port_mib=$(cat $MODULES_DIR/port.mib)

beginning_mib="TELTONIKA-MIB DEFINITIONS ::= BEGIN

IMPORTS
	OBJECT-TYPE, NOTIFICATION-TYPE, MODULE-IDENTITY,
	Integer32, Opaque, enterprises, Counter64,
	IpAddress					FROM SNMPv2-SMI
	TEXTUAL-CONVENTION, DisplayString, TruthValue,
	PhysAddress					FROM SNMPv2-TC
	NetworkAddress					FROM RFC1155-SMI;

teltonika MODULE-IDENTITY
	LAST-UPDATED	'$(date "+%Y%m%d%H%MZ")'
	ORGANIZATION	'TELTONIKA'
	CONTACT-INFO	'TELTONIKA'
	DESCRIPTION	'The MIB module for TELTONIKA ${router_name} routers.'
	REVISION	'202206200000Z'
	DESCRIPTION	'Initial version'
	::= { enterprises 48690 }"
end_mib='END'

echo -e "${beginning_mib}

${device:+device			OBJECT IDENTIFIER ::= { teltonika 1 \}\n}\
${mobile:+mobile			OBJECT IDENTIFIER ::= { teltonika 2 \}\n}\
${gps:+gps			OBJECT IDENTIFIER ::= { teltonika 3 \}\n}\
${traps:+notifications		OBJECT IDENTIFIER ::= { teltonika 4 \}\n}\
${traps:+${mobile:+mobileNotifications	OBJECT IDENTIFIER ::= { notifications 1 \}\n}}\
${traps:+${ios:+ioNotifications		OBJECT IDENTIFIER ::= { notifications 2 \}\n}}\
${hotspot:+hotspot			OBJECT IDENTIFIER ::= { teltonika 5 \}\n}\
${ios:+io			OBJECT IDENTIFIER ::= { teltonika 6 \}\n}\
${wireless:+wireless		OBJECT IDENTIFIER ::= { teltonika 7 \}\n}\
${if_vlan:+vlan			OBJECT IDENTIFIER ::= { teltonika 8 \}\n}\
${sqm:+sqm			OBJECT IDENTIFIER ::= { teltonika 9 \}\n}\
${port:+port			OBJECT IDENTIFIER ::= { teltonika 10 \}\n}\


${device:+$device_mib\n\n}\
${mobile:+$mobile_mib\n\n}\
${gps:+$gps_mib\n\n}\
${traps:+-- Traps --\n${mobile:+$traps_mib_gsm}${ios:+$(get_traps_io_mib)}\n}\
${hotspot:+$hotspot_mib\n\n}\
${ios:+$io_mib\n\n}\
${wireless:+$wireless_mib\n\n}\
${if_vlan:+$if_vlan_mib\n\n}\
${port_vlan:+$port_vlan_mib\n\n}\
${sqm:+$sqm_mib\n\n}\
${port:+$port_mib\n\n}\

${end_mib}" >"$MIB_file"

rm -r "$MODULES_DIR"
