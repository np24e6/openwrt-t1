#!/bin/sh

. /usr/share/libubox/jshn.sh

bname=$(basename $0)
alias logger="logger -s -t $bname"

if [ "$mmc" = "1" ]; then
	msd="$(cat /tmp/.fmt-mmc-msd_last 2>&-)"
	msd1=$msd"p1"
elif [ -n "$msd" ]; then
	msd="$(tr -d '0-9' <<-EOF
		$msd
		EOF
	)"
	msd1=$msd"1"
else
	msd="$(cat /tmp/.fmt-usb-msd_last 2>&-)"
	msd1=$msd"1"
fi

usage() {
	echo -e "\
Usage:
	$bname exfat|ext4 [label]           format last inserted USB MSD, partition label is optional
	$bname target                       print the block device that would be used for formatting
	$bname unmountable                  get unmountable USB MSD list in json format
	$bname unmount [device|mountpoint]  unmount an USB MSD
"
}

refresh_msd() {
	sync
	partprobe $msd
	return 0
}

msdos_pt() {
	sync
	for dev in $msd*; do
		umount -f $dev
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

msd_is_overlay() {
	local info=$(block info | grep -m 1 $msd)
	eval local "${info/*: /}"
	[ "$MOUNT" = "/overlay" ]
}

last_inserted_ok() {
	[ ! -b "$msd" ] || msd_is_overlay && {
		logger "storage device unavailable"
		return 1
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
	grep -q $1 /etc/config/$2 2>&- && killall -0 $3 2>&-
}

add_unmountable() {
	eval local $1

	# if not overlay and not mounted on /mnt/*, return
	local overlay_uuid="$(uci -q get fstab.overlay.uuid)"
	if [ -n "$overlay_uuid" -a "$overlay_uuid" = "$UUID" -a "$MOUNT" = "/overlay" ]; then
		local is_overlay=1
	elif [ -z "$MOUNT" -o -n "${MOUNT%%/mnt/*}" ]; then
		return
	else
		local is_overlay=0
	fi

	json_add_object $DEVICE
		json_add_string mountpoint $MOUNT
		json_add_string dev $DEVICE
		add_space_consumption $DEVICE
		[ -n "$TYPE" ] && json_add_string fs $TYPE
		[ -n "$LABEL" ] && json_add_string label $LABEL

		[ $is_overlay -eq 1 ] && json_add_string in_use memexp || {
			is_in_use $MOUNT samba smbd && json_add_string in_use samba
			is_in_use $MOUNT minidlna minidlna && json_add_string in_use dlna
		}
	json_close_object
}

unmountable() {
	local IFS=$'\n' bi="$(block info)"

	[ -z "$bi" ] && return 1

	json_init
	for line in $bi; do
		add_unmountable "DEVICE=${line/:*/} ${line/*: /}"
	done
	json_close_object

	json_dump -i
}

unmount() {
	local info=$(block info | grep -m 1 $1)
	[ -z "$info" ] && return 1
	eval local "${info/*: /}"
	[ -z "$MOUNT" ] && return 2

	umount $MOUNT || umount -l $MOUNT && rmdir $MOUNT
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
		last_inserted_ok || return
		blockdev_hotplug_pause 1
		msdos_pt $1 || return
		mkfs.exfat -L "$2" $msd1 || {
			logger "exfat volume creation failed"
			return 1
		}
		refresh_msd
		block mount
		blockdev_hotplug_pause 0
	;;
	ext?)
		last_inserted_ok || return
		blockdev_hotplug_pause 1
		msdos_pt $1 || return
		mkfs.ext4 -O ^has_journal -F -L "$2" $msd1 || {
			logger "EXT4 volume creation failed"
			return 1
		}
		refresh_msd
		block mount
		blockdev_hotplug_pause 0
	;;
	target)
		last_inserted_ok || return
		echo $msd
		return 0
	;;
	unmountable)
		unmountable
		return
	;;
	unmount)
		[ -z "$2" ] && { usage; return 1; }
		unmount $2
		return
	;;
	*)
		usage
		return 1
	;;
esac

[ $? -eq 0 ] && logger "$msd1 formatted successfully"
