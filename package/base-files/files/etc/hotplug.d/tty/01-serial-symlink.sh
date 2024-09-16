#!/bin/sh
# symlinks serial chips regardless of vendor and driver - differentiates by DEVPATH

[ "$SUBSYSTEM" != "tty" ] ||
	{ [ -n "$DEVPATH" ] && [ -z "${DEVPATH##*virtual*}" ]; } ||
	{ [ "$ACTION" != "remove" ] && [ "$ACTION" != "add" ]; } && return 0

. /usr/share/libubox/jshn.sh

rs232_usb="/dev/rs232_usb_"
tty_dev="/dev/$DEVICENAME"

err() {
	logger -s -p 3 -t "$(basename $script)" "$@" 2>/dev/console
	exit 1
}

warn() {
	logger -p 4 -t "$(basename $script)" "$@"
}

add_symlink() {
	local path="/sys$DEVPATH/../../../../"
	# attempts to create a unique symlink for each converter
	local descriptors="${path}descriptors"
	[ -e "$descriptors" ] || err "${DEVICENAME}: descriptors file not found"
	local strings="$(cat ${path}version ${path}serial ${path}manufacturer ${path}product 2>/dev/null)"

	local csum="$({
		echo -ne "$strings"
		cat "$descriptors"
	} | sha256sum)"
	[ -e "$rs232_usb${csum:0:8}" ] && {
		warn "an identical converter is already plugged in" # so let's include the path
		csum="$({
			echo -ne "$strings${DEVPATH%%/tty*}"
			cat $descriptors
		} | sha256sum)"
	}

	ln -s "$tty_dev" "$rs232_usb${csum:0:8}" # /dev/rs232_usb_7e97db3b
}

handle_multi_symlink() {
	case "$ACTION" in
	add)
		add_symlink
		;;
	remove)
		for f in $rs232_usb*; do
			[ -h "$f" ] && [ "$(readlink $f)" = "$tty_dev" ] || continue

			rm "$f"
			break
		done
		;;
	esac
}

handle_symlink() {
	local path="/sys$DEVPATH/../../../../"

	case "$ACTION" in
	add)
		ln -s "$tty_dev" "$1"
		;;
	remove)
		[ -h "$1" ] && [ "$(readlink $1)" = "$tty_dev" ] && rm "$1"
		;;
	esac
}

strstr() {
	[ "${1#*$2*}" != "$1" ]
}

check_inner_serial() {
	local object_num="$2"
	local path serial_type

	json_select "$object_num"
	json_get_var path path

	json_select devices
		json_get_var serial_type 1	
	json_close_object devices		

	if strstr $DEVPATH "$path"; then
		handle_symlink "/dev/$serial_type" # built-in chip
	fi	

	json_close_object "$object_num"
}

manage_serial() {
	local serial usb_jack

	json_init
	[ -f /etc/board.json ] && json_load_file /etc/board.json || json_load_file /tmp/board.json
	json_get_vars serial usb_jack	
	
	if [ -n $serial ]; then
		json_for_each_item check_inner_serial serial
	fi
	
	if [ -n $usb_jack ]; then
		if strstr $DEVPATH "$usb_jack"; then
			handle_multi_symlink "$rs232_usb" # usb-to-serial adapter
		fi	
	fi	
}

manage_serial
