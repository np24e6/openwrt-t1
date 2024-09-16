#!/bin/sh
# shellcheck disable=3010,3043,2317,3057

. /lib/functions.sh
. /lib/upgrade/common.sh

TMP_DIR="/tmp/tmp_root/"
TAR_PATH=/etc/profiles/
export CONFFILES=/tmp/profile.conffiles
RM_CONFFILES=/tmp/rm_profile.conffiles
PROFILE_VERSION_FILE=/etc/profile_version
EXCEPTIONS="etc/config/rms_connect_timer etc/config/profiles etc/crontabs/root etc/hosts etc/config/luci etc/config/vuci
	etc/inittab etc/group etc/passwd etc/profile etc/shadow etc/shells etc/sysctl.conf etc/rc.local  etc/config/teltonika
	etc/default-config"

EXTRA_FILES="/etc/firewall.user /etc/profile_version"

KNOWN_CLEANS="/etc/config/iojuggler"

log() {
	logger -s -t "$(basename "$0")" "$1"
}

add_uci_conffiles() {
	local opkg_command=""
	local file="$1"
	local misc_files=""

	if [ -f "/bin/opkg" ]; then
		opkg_command="opkg list-changed-conffiles"
	else
		# if opkg doesn't exist, save the entire /etc directory
		find "/etc" -type f -o -type l 2>/dev/null >"$file"
		# exclude these directories
		sed -i "/\/etc\/uci-defaults\//d;
			/\/etc\/init.d\//d;
			/\/etc\/modules-boot.d\//d;
			/\/etc\/modules.d\//d;
			/\/etc\/profiles\//d;
			/\/etc\/rc.button\//d;
			/\/etc\/rc.d\//d;
			/\/etc\/hotplug.d\//d" "$file"
	fi

	misc_files=$(sed -ne '/^[[:space:]]*$/d; /^#/d; p' \
		/etc/sysupgrade.conf /lib/upgrade/keep.d/* 2>/dev/null)

	[ -n "$opkg_command" ] && eval "$opkg_command" >"$file"
	#Do not qoute ${misc_files} !!!!
	# shellcheck disable=2086 # Splitting is intended here
	[ -n "$misc_files" ] && find ${misc_files} -type f -o -type l 2>/dev/null >>"$file"

	# removes duplicates
	[ -f "$file" ] && {
		local temp="$(sort -u "$file")"
		printf '%s' "$temp" >"$file"
	}

	return 0
}

remove_exceptions() {
	# shellcheck disable=2086 # Splitting is intended here
	sed -i'' -e "$(printf "\,%s,d; " $EXCEPTIONS)" "$1"
}

remove_exceptions_from_file() {
	local dir="$1"
	cd "$dir" || return 1

	# shellcheck disable=2086 # Splitting is intended here
	rm -f $EXCEPTIONS

	cd - >/dev/null || return 1
}

add_extras() {
	# shellcheck disable=2086 # Splitting is intended here
	printf "%s\n" $EXTRA_FILES >>"$CONFFILES"
}

__add_conf_files() {
	local profile="$1" filelist="$2"
	local cfg_name misc_files keep_files

	for i in $(grep -s "^/etc/config/" "$filelist"); do
		cfg_name=$(basename "$i")

		cp "$i" "${TMP_DIR}/etc/config/"
		sed -i "/$cfg_name/d" "/etc/profiles/${profile}.md5"
		md5sum "$i" >>"/etc/profiles/${profile}.md5"
	done

	keep_files=$(grep -s "^/lib/upgrade/keep.d/" "$filelist")
	[ -z "$keep_files" ] && return

	misc_files=$(sed -ne '/^[[:space:]]*$/d; /^#/d; p' ${keep_files} 2>/dev/null)
	[ -n "$misc_files" ] &&
		find ${misc_files} -type f -o -type l >/tmp/keep_files
}

__rm_conf_files() {
	local profile=$1 filelist=$2
	local cfg_name keep_files

	for i in $(grep -s "^/etc/config/" "$filelist"); do
		cfg_name=$(basename "$i")

		rm -f "${TMP_DIR}/etc/config/${cfg_name}"
		sed -i "/$cfg_name/d" "/etc/profiles/${profile}.md5"
	done

	keep_files=$(grep -s "^/lib/upgrade/keep.d/" "$filelist")
	[ -z "$keep_files" ] && return

	for i in $(sed -ne '/^[[:space:]]*$/d; /^#/d; p' ${keep_files} 2>/dev/null); do
		rm -rf "${TMP_DIR:?}/${i:1}"
	done
}

__update_tar() {
	local cb=$1
	local filelist=$2
	local profile name

	mkdir -p "$TMP_DIR"
	for profile in /etc/profiles/*.tar.gz; do
		tar xzf "$profile" -C "$TMP_DIR"
		name=$(basename "$profile" .tar.gz)
		eval "$cb '$name' '$filelist'"
		[ -e "/tmp/keep_files" ] && keep=" -T /tmp/keep_files"

		tar cz${keep} -f "$profile" -C "$TMP_DIR" "."
		rm -rf "${TMP_DIR:?}/*" /tmp/keep_files
	done

	rm -rf "$TMP_DIR"
}

install_pkg() {
	__update_tar __add_conf_files "$1"
}

remove_pkg() {
	__update_tar __rm_conf_files "$1"
}

do_save_conffiles() {
	local conf_tar=$1

	[ -z "$conf_tar" ] && return 1
	[ -z "$(rootfs_type)" ] && {
		echo "Cannot save config while running from ramdisk."
		ask_bool 0 "Abort" && exit
		return 0
	}

	add_uci_conffiles "$CONFFILES"
	# Do not save these configs
	remove_exceptions "$CONFFILES"
	echo -en "\n" >>"$CONFFILES"
	add_extras
	cp /etc/version "$PROFILE_VERSION_FILE"
	tar czf "$conf_tar" -T "$CONFFILES" 2>/dev/null
	rm -f "$CONFFILES" "$PROFILE_VERSION_FILE"
}

create_md5() {
	local md5_file=$1
	[ -z "$md5_file" ] && return 1
	md5sum /etc/config/* /etc/shadow | grep -vE "profiles|rms_connect_timer" >"$md5_file"
}

is_legacy_profile() {
	#There is no correct way to indicate legacy profile, so we searching for dinosaurs here
	[ -e "/etc/config/coovachilli" ] && [ -e "/etc/config/sms_gateway" ] &&
		[ -e "/etc/config/data_limit" ]
}

uci_apply_defaults() {
	mkdir -p /tmp/uci-defaults
	cp -r /rom/etc/uci-defaults/* /tmp/uci-defaults/
	chmod -R +x /tmp/uci-defaults/
	cd /tmp/uci-defaults || return 0
	files="$(find . -type f | sort)"
	[ -z "$files" ] && return 0

	#for legacy rut9/rut2 migration
	is_legacy_profile && {
		touch /etc/config/teltonika
		cp /rom/etc/migrate.conf/* /etc/migrate.conf/
	}

	for file in $files; do
		(. "./$file" 2>/dev/null) && rm -f "$file"
	done

	cd /
	rm -rf /tmp/uci-defaults/
	rm -f /etc/migrate.conf/*
}

call_config_event() {
	echo "apply $1"
	ubus call service event "{ \"type\": \"config.change\", \"data\": { \"package\": \"$1\" }}"
}

apply_config() {
	local config_check_path="/var/run/config.check"
	local md5file="${1:-/var/run/config.md5}"

	rm -rf "${config_check_path}"
	mkdir -p "${config_check_path}"

	cp /etc/config/* ${config_check_path}/
	rm ${config_check_path}/profiles

	[ -f "$md5file" ] && {
		local network=0

		for c in $(diff "$md5file"); do
			[ "$c" = "network" ] && {
				network=1
				continue
			}
			call_config_event "$c"
		done

		/bin/ubus -t 180 call mobifd reload >/dev/null
		[ "$network" -eq 1 ] && call_config_event "network"
	}

	md5sum "${config_check_path}"/* >"$md5file"
	rm -rf "${config_check_path}"

	return 0
}

change_config() {
	local md5file="/var/run/config.md5"
	local new="$1"

	uci -q get "profiles.$new" >/dev/null || {
		log "Profile '$new' not found"
		return 1
	}

	local archive
	archive="${TAR_PATH}$(uci -q get "profiles.${new}.archive")" || {
		log "Unable to retrieve profile '$new' archive name"
		return 1
	}

	[ ! -r "$archive" ] && {
		log "Unable to read '$archive'"
		return 1
	}

	rm -f "$md5file"
	apply_config "$md5file"

	mkdir -p "$TMP_DIR"
	tar xzf "$archive" -C "$TMP_DIR" 2>&- || {
		log "Unable to extract '$archive'"
		return 1
	}

	cmp -s "${TMP_DIR}${PROFILE_VERSION_FILE}" /etc/version || {
		#Legacy profiles do not have some config files so we need to reset
		#these files before applying profile
		for file in $KNOWN_CLEANS; do
			cp "/rom$file" "$file"
		done
	}

	#Fixing legacy profiles
	remove_exceptions_from_file "$TMP_DIR"
	cp -r "$TMP_DIR"/* /

	#Apply uci defaults only if profile is created on different FW version.
	cmp -s "${TMP_DIR}${PROFILE_VERSION_FILE}" /etc/version || {
		uci_apply_defaults
	}

	rm -rf "$TMP_DIR"
	uci -q set "profiles.general.profile=$new" 2>&- || {
		log "Unable to set new profile via uci"
		return 1
	}

	uci -q commit profiles 2>&- || {
		log "Unable to commit new profile changes via uci"
		return 1
	}

	apply_config "$md5file"
	mkdir -p /tmp/vuci && touch /tmp/vuci/profile_changed
}

diff() {
	local md5file="$1"

	[ -z "$md5file" ] && return 1

	for c in $(md5sum -c "$md5file" 2>/dev/null | sed -nE -e 's/(.*):\s+FAILED/\1/p'); do
		echo "${c##*/}" # for known input (in this case regular fullpath filenames) it's equivalent to basename, but faster
	done

	return 0
}

rm_conffiles() {
	add_uci_conffiles "$RM_CONFFILES"
	# Do not save these configs
	remove_exceptions "$RM_CONFFILES"

	# shellcheck disable=2046 # Splitting is intended here
	rm $(cat $RM_CONFFILES)
}

[ -z "$1" ] && exit

case "$1" in
-b)
	do_save_conffiles "$2"
	;;
-m)
	create_md5 "$2"
	;;
-a)
	apply_config "$2"
	;;
-c)
	change_config "$2"
	;;
-d)
	[ -f "$2" ] && diff "$2"
	;;
-u)
	uci_apply_defaults
	;;
-r)
	rm_conffiles
	;;
-i)
	install_pkg "$2"
	;;
-p)
	remove_pkg "$2"
	;;
*)
	log "unrecognised option '$1'"
	exit 1
	;;
esac

exit $?
