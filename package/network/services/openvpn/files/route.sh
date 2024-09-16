#!/bin/sh

add_del="$1"
route="$2"
cname="$3"

case "$route" in
	*:*) six=-6 ;;
	*)   six="" ;;
esac

if [ -n "$dev" ]
then
	device="dev $dev"
else
	device=""
fi

if ip $six route "$add_del" "$route" $device; then
	if [ -n "$cname" ] && [ -n "$dev" ]; then
		logger -t "openvpn(${dev##*_s_})" "${cname}/route $add_del $route"
	else
		file=$(grep -I "learn-address /etc/openvpn/route.sh" /tmp/etc/openvpn-* | head -n 1)
		if [ -n "$file" ]; then
			instance="${file%%.conf*}"
			instance=${instance##*/openvpn-}
			[ -n "$instance" ] && logger -t "openvpn($instance)" "route $add_del $route" || logger -t "openvpn" "route $add_del $route"
		fi
	fi
fi