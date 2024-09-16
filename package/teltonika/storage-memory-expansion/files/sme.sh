#!/bin/sh

#
# Copyright (C) 20XX Teltonika
#

alias logger="logger -s -t sme.sh"
usb_msd_fs="ext4"
usb_msd_label="RUTOS_overlay"
pending_reboot="/tmp/.sme_pending_reboot"
orig_overlay="/rwm"
conf_backup="pre_sme.tgz"
mmc_driver='/etc/modules-late.d/mmc-spi'
uuid_file='.sme_uuid'
sme_maj_min='/tmp/.sme_major_minor'

[[ ! -e '/tmp/.fmt-usb-msd_blockdev_hotplug_paused' && -n "$HOTPLUG_TYPE" ]] && {
	# handle accidental storage dev removal and re-insertion. must execute after block hotplug.

	if [[ -z "$DEVPATH" || -n "${DEVPATH##*usb*}" || "$DEVTYPE" != "partition" || -z "$DEVNAME" ]]; then
		exit 0
	fi

	case $ACTION in
		add)
			[ -e $sme_maj_min ] && exit 0
			uuid="$(uci -q get fstab.overlay.uuid)"
			[[ -n "$uuid" && "$uuid" = "$(cat /mnt/$DEVNAME/$uuid_file)" ]] && reboot -f
		;;
		remove)
			[ -e $sme_maj_min ] || exit 0
			. $sme_maj_min
			[[ $SME_MAJOR = $MAJOR && $SME_MINOR = $MINOR ]] && reboot -f
		;;
	esac

	exit 0
}

source /lib/functions.sh

must_expand_in_bg() {
	[ ${expand_in_bg:-0} -eq 1 ]
}

block_info_vars() {
	[ -z "$1" ] && return

	local info=$(block info | grep -m 1 "$1" | tr -d '"')
	echo "DEVICE=${info/:*/} ${info/*: /}"
}

rm_overlay_entry_exit_on_fail() {
	config_get uuid "$1" uuid
	config_get target "$1" target
	config_get is_rootfs "$1" is_rootfs 0
	[[ $is_rootfs -eq 0 && "$target" != "/overlay" && "$uuid" != $UUID ]] && \
		return 0

	uci delete fstab.$1 || {
		logger "failed, couldn't remove mount entry"
		exit 1
	}
}

fstab_entries() {
	config_load fstab
	config_foreach rm_overlay_entry_exit_on_fail mount

	uci batch <<-EOF
		set fstab.rwm="mount"
		set fstab.rwm.device="$(awk -e '/\s\/overlay\s/{print $1}' /etc/mtab)"
		set fstab.rwm.target=$orig_overlay
		set fstab.overlay="mount"
		set fstab.overlay.uuid=$UUID
		set fstab.overlay.target="/overlay"
		set fstab.overlay.sme="1"
		set fstab.log="mount"
		set fstab.log.enabled="0"
		set fstab.log.device="$(awk -e '/\s\/log\s/{print $1}' /etc/mtab)"
		commit fstab
	EOF
}

fs_state_ready() {
	# prevent mount_root:overlay.c:mount_overlay():435 from wiping overlay during pre-init
	local fstate="$1/.fs_state"
	rm -f $fstate
	ln -s 2 $fstate # FS_STATE_READY == 2
}

# Matching an exact _binary_ signature of a legacy SME fstab config
is_legacy_fstab() {
	local uuid="$(sed -n 11p /etc/config/fstab | cut -b 15-50)"
	printf "config 'global'
	option	anon_swap	'0'
	option	anon_mount	'0'
	option	auto_swap	'1'
	option	auto_mount	'1'
	option	delay_root	'5'
	option	check_fs	'0'

config 'mount'
	option	target	'/overlay'
	option	uuid	'%s'
	option	enabled	'1'

" "$uuid" > /tmp/fstab_check
	cmp -s /etc/config/fstab /tmp/fstab_check
	local ret=$?
	rm -f /tmp/fstab_check
	return $ret
}

fix_legacy_fstab() {
	local UUID="$(sed -n 11p /etc/config/fstab | cut -b 15-50)"
	cp /rom/etc/config/fstab /etc/config/fstab

	mkdir -p /mnt/sda1
	mount -t $usb_msd_fs /dev/sda1 /mnt/sda1
	rm -rf /mnt/sda1/* /mnt/sda1/.[!.]* /mnt/sda1/..?*
	fstab_entries
	cp -a /overlay/. /mnt/sda1
	umount /mnt/sda1
	rmdir /mnt/sda1
}

expand() {
	logger "started $msd formatting"
	fmt-usb-msd.sh $usb_msd_fs $msd $usb_msd_label || {
		logger "failed, couldn't format $msd"
		return 1
	}

	# some storage devices are slow and their partitions take a while to appear
	# retry every second, max 15 times
	local c=0
	while [ $c -lt 15 ] && ! {
		local $(block_info_vars $msd)
		[[ -n "$UUID" && -n "$MOUNT" ]]
	}; do c=$((c+1)); sleep 1; done

	[[ -n "$UUID" && -n "$MOUNT" ]] || {
		logger "failed, couldn't get $msd info"
		return 1
	}

	[ "$TYPE" = $usb_msd_fs ] || {
		logger "failed, expecting $msd to be formatted as $usb_msd_fs"
		return 1
	}

	sysupgrade --create-backup $MOUNT/$conf_backup && chmod -w $MOUNT/$conf_backup || \
		logger "warning: failed to backup config: it won't be restored on shrinkage"

	fstab_entries || {
		logger "failed, couldn't modify fstab"
		return 1
	}

	cp -a /overlay/./ $MOUNT/ || {
		logger "failed, couldn't copy overlay files"
		return 1
	}

	sync
	umount $MOUNT
}

expand_wrapper() {
	expand
	local ret=$?

	must_expand_in_bg && { [ $ret -eq 0 ] && operation success || operation fail; }

	[ $ret -eq 0 ] && {
		touch $pending_reboot
		logger "expansion successful. pending reboot"
	}

	return $ret
}

expand_prelude() {
	[ -e $pending_reboot ] && {
		logger "expand failed, already pending reboot"
		return 1
	}

	msd="$(fmt-usb-msd.sh basedev $1)"
	[ -z "$msd" ] && {
		logger "failed, target device unavailable"
		return 1
	}

	must_expand_in_bg && {
		{ expand_wrapper </dev/null &>/dev/null & } &
		return 0
	}

	expand_wrapper
}

shrink() {
	[ -e $pending_reboot ] && {
		logger "shrink failed, already pending reboot"
		return 1
	}

	local $(block_info_vars $(uci -q get fstab.overlay.uuid))

	cp $MOUNT/$conf_backup $orig_overlay/sysupgrade.tgz || \
		logger "warning: config backup not found: original config won't be restored"

	rm -f $orig_overlay/upper/etc/config/fstab
	fs_state_ready $orig_overlay

	sync
	touch $pending_reboot
	logger "shrinkage successful. pending reboot"
	return 0
}

is_expanded() {
	[ "$(uci -q get fstab.overlay.sme)" != "1" ] && return 1
	local $(block_info_vars $(uci -q get fstab.overlay.uuid))
	[ "$MOUNT" = "/overlay" ]
}

mk_sd_node() {
	local maj_min='/sys/block/mmcblk0/mmcblk0p1/dev' dev='/dev/mmcblk0p1'
	[[ ! -e $mmc_driver || ! -e $maj_min || -e $dev ]] && return 0

	local mm="$(cat $maj_min)"
	mknod $dev b ${mm%%:*} ${mm##*:}
}

operation() {
	# intended for webui: to be called every few seconds to get operation status
	local ongoing_file="/tmp/.sme_ongoing"

	case $1 in
		ongoing|success|fail)
			echo $1 > $ongoing_file
		;;
		new)
			[ "$(cat $ongoing_file 2>&-)" = "ongoing" ] && {
				logger "an operation is in progress"
				exit 1
			}

			operation ongoing
		;;
		get)
			[ ! -e $ongoing_file ] && {
				echo "none"
				return 0
			}

			cat $ongoing_file
		;;
		*)
			return 1
		;;
	esac
}

preboot() {
	if is_legacy_fstab; then
		fix_legacy_fstab
		reboot -f
		return 0
	fi

	[ "$(uci -q get fstab.overlay.sme)" = "1" ] || return 0

	[ -e "$mmc_driver" ] && /sbin/kmodloader "$mmc_driver"

	local c=0
	while [ $((c++)) -lt 15 ]; do
		mk_sd_node
		local $(block_info_vars "$(uci -q get fstab.overlay.uuid)")
		[ -b "$DEVICE" ] && break
		sleep 1; ! :
	done || return 1

	local mnt="/tmp/.tmp_overlay"
	mkdir -p $mnt
	mount -t $usb_msd_fs $DEVICE $mnt || return 2
	cd $mnt

	find -maxdepth 1 -mindepth 1 ! -name "$conf_backup" -exec rm -rf {} \;

	cp -a /overlay/./ ./ || return 3
	fs_state_ready $mnt || return 4
	fs_state_ready /overlay || return 5

	cd -
	sync
	umount $mnt
	mount_root
	umount /rom/overlay
	return 0
}

case $1 in
	--expand|-e)
		operation new

		if is_expanded; then
			logger "already expanded"
			operation fail
			exit 1
		else
			expand_prelude $2 && {
				must_expand_in_bg || operation success
				exit 0
			} || {
				operation fail
				exit 1
			}
		fi
	;;
	--shrink|-s)
		operation new

		if is_expanded; then
			shrink && {
				operation success
				exit 0
			} || {
				operation fail
				exit 1
			}
		else
			logger "not expanded, won't shrink"
			operation fail
			exit 1
		fi
	;;
	--status|-t)
		[ "$(operation get)" = "ongoing" ] && {
			echo "in_progress"
			exit 0
		}

		if [ -e $pending_reboot ]; then
			echo "reboot"
		elif is_expanded; then
			echo "expanded"
		else
			echo "unexpanded"
		fi
	;;
	--operation|-o)
		echo $(operation get)
	;;
	--preboot)
		preboot
	;;
	*)
		echo -e "\
Usage:
	-e, --expand <device>   expands storage if all conditions are met
	-s, --shrink            disable/undo expansion
	-t, --status            reports whether storage is 'expanded', 'unexpanded' or waiting for 'reboot'
	-o, --operation         reports status/result of an ongoing/finished expansion or shrinkage

	Overridable ENV vars (listed with default values): expand_in_bg=0
"
		exit 1
	;;
esac
