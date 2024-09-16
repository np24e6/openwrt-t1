#!/bin/sh
# symlinks serial chips regardless of vendor and driver - differentiates by DEVPATH

[ "$SUBSYSTEM" != "tty" ] ||
	{ [ -n "$DEVPATH" ] && [ -z "${DEVPATH##*virtual*}" ]; } ||
	{ [ "$ACTION" != "remove" ] && [ "$ACTION" != "add" ]; } && return 0

usb_serial="/dev/usb_serial_"
tty_dev="/dev/$DEVICENAME"

err() {
	logger -s -p 3 -t "$(basename $script)" "$@" 2>/dev/console
	exit 1
}

warn() {
	logger -p 4 -t "$(basename $script)" "$@"
}

kinfo() {
	echo "[$(basename $script)] $@" > /dev/kmsg
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
	[ -e "$usb_serial${csum:0:8}" ] && {
		warn "an identical converter is already plugged in" # so let's include the path
		csum="$({
			echo -ne "$strings${DEVPATH%%/tty*}"
			cat $descriptors
		} | sha256sum)"
	}

	ln -s "$tty_dev" "$usb_serial${csum:0:8}" # /dev/usb_serial_7e97db3b
	kinfo "Creating $tty_dev -> $usb_serial${csum:0:8} symlink..."
}

handle_multi_symlink() {
	case "$ACTION" in
	add)
		add_symlink
		;;
	remove)
		for f in $usb_serial*; do
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
		kinfo "Creating $tty_dev -> $1 symlink..."
		;;
	remove)
		[ -h "$1" ] && [ "$(readlink $1)" = "$tty_dev" ] && rm "$1"
		;;
	esac
}

strstr() {
	[ "${1#*$2*}" != "$1" ]
}

manage_serial() {
	local usb_jack path modem_id idx usb_jack dev_type dev_path
	local file="/tmp/board.json"

	kinfo "New device $DEVICENAME at $DEVPATH appeared!"

	[ -f "$file" ] || file="/etc/board.json"

	# skip modems
	modem_id="$(jsonfilter -i $file -e '@.modems[0].id')"
	strstr "$DEVPATH" "$modem_id" && exit 0

	idx=0
	while [ "$idx" -lt 2 ]
	do
		dev_type="$(jsonfilter -i $file -e '@.serial['$idx'].devices[0]')"
		dev_path="$(jsonfilter -i $file -e '@.serial['$idx'].path')"
		strstr "$DEVPATH" "$dev_path" && {
			handle_symlink "/dev/$dev_type"
			exit 0
		}
		idx=$((idx+1))
	done

	# usb-to-serial adapter
	usb_jack="$(jsonfilter -i $file -e '@.usb_jack')"
	strstr "$DEVPATH" "$usb_jack" && \
		handle_multi_symlink "$usb_serial"
}

manage_serial
