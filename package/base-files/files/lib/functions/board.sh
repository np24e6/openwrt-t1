
# when device contains 2 internal modems, this function will return '2' if
#  selected modem(inc_id) is builtin and primary.
# And if it's only builtin, then '1'

is_builtin_modem() {
	local builtin primary

	eval "$(jsonfilter -q -i "/etc/board.json" \
		-e "builtin=@['modems'][@.id='$1'].builtin" \
		-e "primary=@['modems'][@.id='$1'].primary" \
		)"

	[ -z "$builtin" ] && echo 0 && return

	[ "$builtin" -eq 1 ] && {
		[ -z "$primary" ] && echo 1 && return
		[ "$primary" -eq 1 ] && echo 2 && return
		echo 1
		return
	}

	echo 0
}

is_dual_modem() {
	[ "$(jsonfilter -i /etc/board.json -e '@.hwinfo.dual_modem')" = "true" ] && echo 1 || echo 0
}
