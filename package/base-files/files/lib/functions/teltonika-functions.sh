#!/bin/sh

is_ios_enabled() {
	[ "$(jsonfilter -q -i /etc/board.json -e '@.hwinfo.ios')" = "true" ]
}

# set_tty_duplex <device_path> <duplex>
set_tty_duplex() {
	# duplex mode only applies for rs485
	[ "$1" != "/dev/rs485" ] && return

	output=$(gpiofind GPIO_RS485_RX_EN)

	[ -z "$output" ] && return

	gpioset $output=$2
}

# set_tty_options <device_path> <baudrate> <databits> <parity> <stopbits> <flowcontrol> <duplex> <echo>
set_tty_options() {
	local PARITY_TMP=""
	local SBITS_TMP=""
	local FCTRL_TMP=""

	case "$4" in
	"odd") PARITY_TMP="parenb parodd -cmspar" ;;
	"even") PARITY_TMP="parenb -parodd -cmspar" ;;
	"mark") PARITY_TMP="parenb parodd cmspar";;
	"space") PARITY_TMP="parenb -parodd cmspar";;
	*) PARITY_TMP="-parenb -parodd -cmspar" ;;
	esac

	case "$5" in
	1) SBITS_TMP="-cstopb" ;;
	2) SBITS_TMP="cstopb" ;;
	*) SBITS_TMP="-cstopb" ;;
	esac

	case "$6" in
	"none") FCTRL_TMP="-crtscts -ixon -ixoff" ;;
	"rts/cts") FCTRL_TMP="crtscts -ixon -ixoff" ;;
	"xon/xoff") FCTRL_TMP="-crtscts ixon ixoff" ;;
	*) FCTRL_TMP="-crtscts -ixon -ixoff" ;;
	esac

	set_tty_duplex "$1" "$7"

	if [ "$8" == "1" ]; then
		FCTRL_TMP="$FCTRL_TMP echo"
	else
		FCTRL_TMP="$FCTRL_TMP -echo"
	fi

	local stty_retries=0
	while ! stty -F "$1" "$2" cs"$3" $PARITY_TMP "$SBITS_TMP" $FCTRL_TMP; do
		if [ $stty_retries -lt 5 ]; then
			stty_retries=$((stty_retries + 1))
			echo "stty was unable to set all the parameters, retrying in 10 seconds"
			sleep 10
		else
			echo "stty failed, continuing anyway"
			break
		fi
	done
}

to_lower() {
	tr '[A-Z]' '[a-z]' <<-EOF
		$@
	EOF
}
