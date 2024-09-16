#!/bin/sh

. /usr/share/libubox/jshn.sh

bname=$(basename $0)
alias logger="logger -s -t $bname"

usage() {
	echo -e "\
Usage:
	$bname exfat|ext4 device [label]    format last inserted USB MSD, partition label is optional
	$bname devices                      list USB MSDs in json
	$bname unmount [device|mountpoint]  unmount an USB MSD
"
}

strstr() {
	[ "${1#*$2*}" != "$1" ]
}

# list of all external blockdevs
blockdevs() {
	for f in $(find -L /sys/block -maxdepth 3 -name 'io_timeout'); do
		basename ${f%/*/io_timeout}
	done
}

refresh_msd() {
	sync
	partprobe $msd
	return 0
}

msdos_pt() {
	local r;
	sync
	for dev in $msd*; do
		[ -e $dev ] && r=$(umount -f $dev 2>&1)
		strstr "$r" "Resource busy" && {
			logger "$r"
			return 1
		}
	done

	dd if=/dev/zero of=$msd bs=1M count=16 conv=fsync || {
		logger "failed to clear partitions on $msd"
		return 1
	}

	refresh_msd

	[ "$1" = "ntfs" ] || [ "$1" = "exfat" ] && local system_id="7" || local system_id="83"
	# create msdos partition table with a single partition spanning the whole disk
	echo -e "o\nn\np\n1\n\n\nt\n$system_id\nw\n" | fdisk $msd || {
		logger "failed to create partition table on $msd"
		return 1
	}

	refresh_msd

	local c=0
	while [ $c -lt 60 ]; do
		[ -e $msd1 ] && return 0
		logger "waiting for $msd1..."
		c=$((c+1))
		sleep 1
	done

	logger "failed to create partition table on $msd: timed out waiting for device"
	return 1
}

is_sdcard() {
	[ "$(cat /sys/block/$1/device/type 2>&-)" = 'SD' ]
}

basedev() {
	local dev="$(readlink -f /sys/class/block/$(basename $1))"
	if [ ! -e "$dev/device" ]; then
		dev="$(readlink -f "$dev/..")"
	fi
	echo -n "$(basename "$dev")"
}

validate_set_msd() {
	local bd="$(basedev $1)" bi="$(block info | grep -m 1 $1)"

	local MOUNT="${bi##*MOUNT=\"}"
	MOUNT="${MOUNT%%\"*}"

	msd="/dev/$bd"
	if is_sdcard $bd; then
		msd1=$msd"p1"
	else
		msd1=$msd"1"
	fi

	[ ! -b "$msd" ] || [ "$MOUNT" = "/overlay" ] && {
		logger "'$1' storage device unavailable"
		return 1
	}
	is_in_use "$MOUNT" samba smbd && {
		logger "'$1' storage device in use by samba"
		return 2
	}
	is_in_use "$MOUNT" minidlna minidlna && {
		logger "'$1' storage device in use by minidlna"
		return 3
	}

	return 0
}

add_space_consumption() {
	local dev=$(df -h | grep "$1") || return

	local IFS=' ' c=0
	for w in $dev; do
		case $c in
		2)
			json_add_string used $w
		;;
		3)
			json_add_string available $w
		;;
		4)
			json_add_string used_percentage $w
		;;
		esac

		c=$((c + 1))
	done
}

is_in_use() {
	grep -q "$1" /etc/config/$2 2>&- && killall -0 $3 2>&-
}

get_usb() {
	local file="/tmp/board.json"
	local path usb_jack
	path=$(readlink -f $1)
	usb_jack="$(jsonfilter -i $file -e '@.usb_jack')"
	strstr "$path" "$usb_jack" && echo usb || echo internal
}

add_description() {
	local path="/sys/block/$1/device"
	local d t

	if is_sdcard $1; then
		d="$(cat $path/name) $(cat $path/date)"
		t='sd'
	else
		d="$(cat $path/model)"
		t="$(get_usb $path)"
	fi

	d="${d#${d%%[![:space:]]*}}"
	d="${d%${d##*[![:space:]]}}"

	json_add_string type $t
	json_add_string description "$d"
}

extract_field() {
	local field="$1"
	local line="$2"

	strstr "$line" "$field" || return

	local value="${line#*${field}=\"}"
	value="${value%%\"*}"

	echo "$value"
}

add_unmountable() {
	local line="$1"
	local DEVICE=$(extract_field DEVICE $line)
	local UUID=$(extract_field UUID $line)
	local MOUNT=$(extract_field MOUNT $line)
	local TYPE=$(extract_field TYPE $line)
	local LABEL=$(extract_field LABEL $line)
	
	[ -z "$MOUNT" ] && return 1

	# if not overlay and not mounted on /mnt/*, return
	local overlay_uuid="$(uci -q get fstab.overlay.uuid)"
	if [ -n "$overlay_uuid" -a "$overlay_uuid" = "$UUID" -a "$MOUNT" = "/overlay" ]; then
		local is_overlay=1
	elif [ -z "$MOUNT" -o -n "${MOUNT%%/mnt/*}" ]; then
		return 0
	else
		local is_overlay=0
	fi

	local bn=$(basename $DEVICE)
	local bd=$(basedev $bn)
	json_add_object $bn
		json_add_string mountpoint $MOUNT
		json_add_string dev $DEVICE
		add_description $bd
		add_space_consumption $DEVICE
		[ -n "$TYPE" ] && json_add_string fs $TYPE
		[ -n "$LABEL" ] && json_add_string label $LABEL

		[ $is_overlay -eq 1 ] && json_add_string in_use memexp || {
			is_in_use "$MOUNT" samba smbd && json_add_string in_use samba
			is_in_use "$MOUNT" minidlna minidlna && json_add_string in_use dlna
		}
		json_add_string status mounted
	json_close_object
	return 0
}

devices() {
	local IFS=$'\n' bi="$(block info)"

	[ -z "$bi" ] && return 1

	local bd=$(blockdevs)
	json_init
	for line in $bi; do
		for d in $bd; do
			strstr "$line" "$d" || continue
			bd=${bd%$d*}${bd#*$d}
			break
		done
		add_unmountable "DEVICE=\"${line/:*/}\" ${line/*: /}" || bd="$bd$d"
	done
	for d in $bd; do
		[ "$(cat "/sys/block/$d/size")" = '0' ] && continue
		json_add_object $d
			json_add_string dev /dev/$d
			add_description $d
			json_add_string status unformatted
		json_close_object
	done
	json_close_object

	json_dump -i
}

unmount() {
	local info=$(block info | grep -m 1 $1)
	[ -z "$info" ] && return 1

	local MOUNT="${info##*MOUNT=\"}"
	MOUNT="${MOUNT%%\"*}"
	[ -z "$MOUNT" ] && return 2

	umount $MOUNT || return 3
	rmdir $MOUNT

	local eject="$(readlink -f /sys/class/block/$(basename $1))"
	local subdir='/device/delete'
	local part="$eject/..$subdir"
	local unpart="$eject$subdir"
	if [ -e "$part" ]; then
		eject="$part"
	else
		eject="$unpart"
	fi
	echo 1 > "$eject"
}

blockdev_hotplug_pause() {
	local path='/tmp/.fmt-usb-msd_blockdev_hotplug_paused'
	if [ $1 -eq 1 ]; then
		touch $path
	else
		rm $path
	fi

	return 0
}

case $1 in
	ntfs|exfat)
		validate_set_msd $2 || return
		blockdev_hotplug_pause 1
		msdos_pt $1 || return
		mkfs.exfat -L "$3" $msd1 || {
			logger "exfat volume creation failed"
			return 1
		}
		refresh_msd
		block mount
		blockdev_hotplug_pause 0
	;;
	ext?)
		validate_set_msd $2 || return
		blockdev_hotplug_pause 1
		msdos_pt $1 || return
		mkfs.ext4 -O ^has_journal -F -L "$3" $msd1 || {
			logger "EXT4 volume creation failed"
			return 1
		}
		refresh_msd
		block mount
		blockdev_hotplug_pause 0
	;;
	devices)
		devices
		return
	;;
	unmount)
		[ -z "$2" ] && { usage; return 1; }
		unmount $2
		return
	;;
	basedev)
		validate_set_msd $2 || return
		echo "$msd"
		return
	;;
	*)
		usage
		return 1
	;;
esac

[ $? -eq 0 ] && logger "$msd1 formatted successfully"
