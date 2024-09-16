#!/bin/sh
# Copyright (C) 2021 Teltonika

. /lib/functions.sh

PACKAGE_FILE="/etc/package_restore.txt"
BACKUP_PACKAGES="/etc/backup_packages/"
FAILED_PACKAGES="/etc/failed_packages"
TIME_OF_SLEEP=10
PKG_REBOOT=0
DEVICENAME=$(mnf_info --name)
PLATFORM="$(jsonfilter -i /etc/board.json -e @.model.platform)"

[ -d "$BACKUP_PACKAGES" ] && {
	packages=$(ls "$BACKUP_PACKAGES"*.ipk)
	mkdir "$BACKUP_PACKAGES""main"

	for i in $packages; do
		tar x -zf "$i" -C "$BACKUP_PACKAGES" ./control.tar.gz
		tar x -zf "$BACKUP_PACKAGES""control.tar.gz" -C "$BACKUP_PACKAGES" ./control
		[ $(grep -c tlt_name "$BACKUP_PACKAGES"control) -eq 0 ] || mv "$i" "$BACKUP_PACKAGES"main
	done

	while [ -e "/var/lock/opkg.lock" ]; do
		sleep "$TIME_OF_SLEEP"
	done
	
	opkg install $(ls "$BACKUP_PACKAGES"main/*.ipk) $(ls "$BACKUP_PACKAGES"*.ipk)

	rm -rf "$BACKUP_PACKAGES" 2> /dev/null
	/etc/init.d/rpcd reload; /etc/init.d/vuci restart
	touch /tmp/vuci/reload_routes
}

[ -s "$PACKAGE_FILE" ] || exit 0

needed_packets=$(awk '/ - /{print $1}' "$PACKAGE_FILE")
[ -z "$needed_packets" ] && exit 0

third_party_pkg=$(uci_get package_restore package_restore 3rd_party_pkg)
while :; do
	needed_packets=$(awk '/ - /{print $1}' "$PACKAGE_FILE")
	if [ "$third_party_pkg" = "1" ]; then
		/bin/opkg --force_feeds /etc/opkg/openwrt/distfeeds.conf -f /etc/tlt_opkg.conf update 2> /dev/null
	else
		/bin/opkg --force_feeds /etc/opkg/teltonikafeeds.conf -f /etc/tlt_opkg.conf update 2> /dev/null
	fi
	available_packages=$(/sbin/opkg-call)

	[ -z "$available_packages" ] || break
	sleep "$TIME_OF_SLEEP"
done

pkg_names="" # names mapped from AppName, ready to be used for pkg install. e.g. azure_iothub=vuci-app-azure-iothub-ui
for i in $needed_packets; do
	p=$(echo "$available_packages" | awk  "BEGIN { FS=\"\n\" ; RS = \"\n\n\" } /AppName: $i\n/" | grep "Package:" | sed 's/.* //g') # or default
	[ -z "$p" ] && p="$i"
	pkg_names="$pkg_names
		$i=$p"
done

for i in $pkg_names; do
	p_name="${i#*=}"
	app_name="${i%=*}"
	[ -s /usr/lib/opkg/info/"$p_name".control ] && sed -i "/$app_name/d" "$PACKAGE_FILE"
done

hotspot_themes=$(echo "$pkg_names" | grep "hs_theme")

[ -z "$hotspot_themes" ] || {
	for i in $hotspot_themes; do
		p_name="${i#*=}"
		theme=$(echo "$p_name" | awk -F "hs_theme_" '{print $2}')
		uci -q delete landingpage."$theme"
	done
	uci -q commit landingpage
}

languages=$(echo "$pkg_names" | grep "vuci-i18n-")
[ -z "$languages" ] || {
	for i in $languages; do
		p_name="${i#*=}"
		lang=$(echo "$p_name" | awk -F "vuci-i18n-" '{print $2}')
		case "$lang" in
			french) short_code="fr" ;;
			german) short_code="de" ;;
			japanese) short_code="ja" ;;
			portuguese) short_code="pt" ;;
			russian) short_code="ru" ;;
			spanish) short_code="es" ;;
			turkish) short_code="tr" ;;
			*) continue ;;
		esac
		uci -q delete vuci.languages."$short_code"
	done
	uci -q commit vuci
}

while [ -e "/var/lock/opkg.lock" ]; do
	sleep "$TIME_OF_SLEEP"
done

for i in $pkg_names; do
	p_name="${i#*=}"
	app_name="${i%=*}"
	router_check=0
	exists=$(echo "$available_packages" | grep -wc "Package: $p_name")
	router=$(echo "$available_packages" | sed -n "/Package: $p_name$/,/Package:/p" | grep -w "Router:" | awk -F ": " '{print $2}')

	if [ "$router" = "$PLATFORM" ]; then
		router_check=1
	else
		for r in $router; do
			[ $(echo "$DEVICENAME" | grep -c "$r") -ne 0 ] && {
				router_check=1
				break;
			}
		done
	fi

	flash_free=$(df -k | grep -w overlayfs | tail -1 | awk '{print $4}')
	flash_free=$((flash_free * 1000))
	pkg_size=$(/sbin/opkg-call "$p_name")
	[ "$flash_free" -le "$pkg_size" ] && router_check=0
	[ "$exists" -ne 0 -a "$router_check" -ne 0 ] && /bin/opkg install -f /etc/tlt_opkg.conf "$p_name" 2> /dev/null

	[ -s /usr/lib/opkg/info/"$p_name".control ] || {
		echo "$(cat $PACKAGE_FILE | grep -w -m 1 $app_name)" >> "$FAILED_PACKAGES"
		sed -i "/$app_name/d" "$PACKAGE_FILE"
		continue
	}

	pkg_reboot=$(cat /usr/lib/opkg/info/"$p_name".control | grep -w 'pkg_reboot:' | awk '{print $2}')
	[ "$pkg_reboot" = "1" ] && PKG_REBOOT=1
	sed -i "/$app_name/d" "$PACKAGE_FILE"
	/etc/init.d/rpcd reload; /etc/init.d/vuci restart
	touch /tmp/vuci/reload_routes
done

[ "$third_party_pkg" = "1" ] && {
	for i in $pkg_names; do
		p_name="${i#*=}"
		app_name="${i%=*}"
		status=$(opkg status "$i")
		[ -z "$status" ] && opkg install "$p_name" >/dev/null 2>&1
		[ -s /usr/lib/opkg/info/"$p_name".control ] || logger "Failed to install package '$p_name'"
		sed -i "/$app_name/d" "$PACKAGE_FILE"
	done
}

[ "$PKG_REBOOT" -eq 1 ] && /etc/init.d/network restart
rm "$PACKAGE_FILE" 2> /dev/null
