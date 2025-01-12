#!/bin/sh

. /lib/functions.sh
. ../netifd-proto.sh
. /lib/functions/mobile.sh

init_proto "$@"

INCLUDE_ONLY=1

ctl_device=""

proto_gobinet_setup() { echo "wwan[$$] gobinet proto is missing"; }
proto_mbim_setup() { echo "wwan[$$] mbim proto is missing"; }
proto_qmi_setup() { echo "wwan[$$] qmi proto is missing"; }
proto_qmux_setup() { echo "wwan[$$] qmi proto is missing"; }
proto_ncm_setup() { echo "wwan[$$] ncm proto is missing"; }
proto_pppmobile_setup() { echo "wwan[$$] pppmobile proto is missing"; }
proto_directip_setup() { echo "wwan[$$] directip proto is missing"; }
proto_trb_qmapv5_setup() { echo "wwan[$$] qmapv5 proto is missing"; }

[ -f ./gobinet.sh ] && . ./gobinet.sh
[ -f ./mbim.sh ] && . ./mbim.sh
[ -f ./ncm.sh ] && . ./ncm.sh
[ -f ./qmi.sh ] && . ./qmi.sh
[ -f ./qmux.sh ] && . ./qmux.sh
[ -f ./pppmobile.sh ] && { . ./ppp.sh; . ./pppmobile.sh; }
[ -f ./directip.sh ] && . ./directip.sh
[ -f ./qmapv5.sh ] && . ./qmapv5.sh

proto_wwan_init_config() {
	available=1
	no_device=1
	disable_auto_up=1
	teardown_on_l3_link_down=1

	proto_config_add_string apn
	proto_config_add_string auth
	proto_config_add_string username
	proto_config_add_string password
	proto_config_add_string delay
	proto_config_add_string modes
	proto_config_add_string modem
	proto_config_add_boolean dhcp
	proto_config_add_boolean dhcpv6
	proto_config_add_int ip4table
	proto_config_add_int ip6table

	#teltonika specific
	proto_config_add_string pdp
	proto_config_add_string pdptype
	proto_config_add_string sim
	proto_config_add_string method
	proto_config_add_string passthrough_mode
	proto_config_add_string leasetime
	proto_config_add_string mac
	proto_config_add_int mtu

	proto_config_add_defaults
}

check_ppp_driver() {
	json_set_namespace global_json old_cb
	for ob in "$(ubus list | grep gsm.modem)"; do
		local usb_id=""
		local tty_port=""
		json_load "$(ubus call $ob info)"
		json_get_var usb_id usb_id
		json_get_var tty_port tty_port
		if [ "$usb_id" = "$modem" ] && [ $tty_port = "/dev/ttyS2" ]; then
			json_get_var ctl_device data_port
			driver="pppmobile"
			json_set_namespace $old_cb
			return
		fi
	done
	json_set_namespace $old_cb
}

proto_wwan_setup() {
	local driver usb devicename desc modem
	json_get_vars modem

	if [ -L "/sys/bus/usb/devices/${modem}" ]; then
		if [ -f "/sys/bus/usb/devices/${modem}/idVendor" ] \
			&& [ -f "/sys/bus/usb/devices/${modem}/idProduct" ]; then
			local vendor product
			vendor=$(cat /sys/bus/usb/devices/${modem}/idVendor)
			product=$(cat /sys/bus/usb/devices/${modem}/idProduct)
			[ -f /lib/network/wwan/$vendor:$product ] && {
				usb=/lib/network/wwan/$vendor:$product
				devicename=$modem
			}
		else
			echo "wwan[$$]" "Specified usb bus ${modem} was not found"
			proto_notify_error "$interface" NO_USB
			proto_block_restart "$interface"
			return 1
		fi
	else
		echo "wwan[$$]" "Searching for a valid wwan usb device..."
		for a in $(ls /sys/bus/usb/devices); do
			local vendor product
			[ -z "$usb" -a -f /sys/bus/usb/devices/$a/idVendor -a  -f /sys/bus/usb/devices/$a/idProduct ] || continue
			vendor=$(cat /sys/bus/usb/devices/$a/idVendor)
			product=$(cat /sys/bus/usb/devices/$a/idProduct)
			[ -f /lib/network/wwan/$vendor:$product ] && {
				usb=/lib/network/wwan/$vendor:$product
				devicename=$a
			}
		done

		if [ "$vendor" = "" ] && [ "$product" = "" ]; then
			qmap_type=$(jsonfilter -i /etc/board.json -e @.epinfo.qmap_type)
			if [ -n "$qmap_type" ]; then
				driver="trb_qmapv5"
				ctl_device="/dev/cdc-wdm0"
			fi
		fi
	fi

	echo "wwan[$$]" "Using wwan usb device on bus $devicename"

	[ -n "$usb" ] && {
		local old_cb control data ep_iface dl_max_size dl_max_datagrams ul_max_size ul_max_datagrams

		json_set_namespace wwan old_cb
		json_init
		json_load "$(cat "$usb")"
		json_select
		json_get_vars desc control data ep_iface dl_max_size dl_max_datagrams ul_max_size ul_max_datagrams
		json_set_namespace "$old_cb"
	}


	[ -z "$ctl_device" ] && for net in $(ls /sys/class/net/ | grep -e wwan -e usb); do
		[ -z "$ctl_device" ] || continue
		[ -n "$modem" ] && {
			[ $(readlink "/sys/class/net/$net" | grep "$modem") ] || continue
		}
		driver=$(grep DRIVER /sys/class/net/$net/device/uevent | cut -d= -f2)
		case "$driver" in
		qmi_wwan)
			if [ -n "$devicename" ]; then
				ctl_device=/dev/"$(ls /sys/bus/usb/devices/$devicename/*/usbmisc | tail -1)"
			else
				ctl_device=/dev/$(ls /sys/class/net/$net/device/usbmisc)
			fi
			#EC21/EC25/EG06/EP06/EM06/EG12/EP12/EM12/EG16/EG18/EM20/RG500 all support QMAP/QMUX.
			[ -f ./qmux.sh ] && [ -n "$ep_iface" ] && {
				driver=qmux
				[ $dl_max_size -gt 16384 ] && driver=qmapv5
			}
			;;
		cdc_mbim)
			ctl_device=/dev/$(ls /sys/class/net/$net/device/usbmisc)
			;;
		sierra_net|*cdc_ncm)
			ctl_device=/dev/$(cd /sys/class/net/$net/; find ../../../ -name ttyUSB* |xargs -n1 basename | head -n1)
			;;
		cdc_ether)
			[ -n "$devicename" ] && ctl_device=$(ls /sys/bus/usb/devices/$devicename/*/net/)
			;;
		GobiNet)
			[ -n "$devicename" ] && ctl_device=$(ls /sys/bus/usb/devices/$devicename/*/net/)
			;;
		*) continue;;
		esac
		echo "wwan[$$]" "Using proto:$driver device:$ctl_device iface:$net desc:$desc"
	done

	[ -z "$ctl_device" ] && check_ppp_driver

	[ -n "$ctl_device" ] || {
		echo "wwan[$$]" "No valid device was found"
		proto_notify_error "$interface" NO_WWAN_DEVICE
		proto_block_restart "$interface"
		return 1
	}

	uci_set_state network "$interface" driver "$driver"
	uci_set_state network "$interface" ctl_device "$ctl_device"

	case $driver in
	GobiNet)		proto_gobinet_setup $@ ;;
	cdc_mbim)		proto_mbim_setup $@ ;;
	sierra_net)		proto_directip_setup $@ ;;
	pppmobile)		proto_pppmobile_setup $@ ;;
	cdc_ether|*cdc_ncm) 	proto_ncm_setup $@ ;;
	qmi_wwan)		proto_qmi_setup $@ ;;
	qmux|qmapv5)		proto_qmux_setup $@ ;;
	trb_qmapv5)		proto_trb_qmapv5_setup $@ ;;
	esac
}

proto_wwan_teardown() {
	local interface=$1
	local driver=$(uci_get_state network "$interface" driver)
	ctl_device=$(uci_get_state network "$interface" ctl_device)

	case $driver in
	GobiNet)		proto_gobinet_teardown $@ ;;
	cdc_mbim)		proto_mbim_teardown $@ ;;
	sierra_net)		proto_directip_teardown $@ ;;
	pppmobile)		proto_pppmobile_teardown $@ ;;
	cdc_ether|*cdc_ncm) 	proto_ncm_teardown $@ ;;
	qmi_wwan)		proto_qmi_teardown $@ ;;
	qmux|qmapv5)		proto_qmux_teardown $@ ;;
	trb_qmapv5)		proto_trb_qmapv5_teardown $@ ;;
	#Generic teardown
	*)
		#Lets assume that we are using qmux proto by default
		proto_qmux_teardown $@
		ifdown "${interface}_4"
		ifdown "${interface}_6"
		;;
	esac
}

add_protocol wwan
