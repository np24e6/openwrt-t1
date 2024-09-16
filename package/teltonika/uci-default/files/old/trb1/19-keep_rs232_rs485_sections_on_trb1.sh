#!/bin/sh
#
# Copyright (C), 2022 Teltonika
#

. /lib/functions.sh

case "$(mnf_info --name)" in
	TRB142*)
		uci_remove "rs" "rs485"
	;;
	TRB143*)
		uci_remove "rs" "rs232"
		uci_remove "rs" "rs485"
	;;
	TRB145*)
		uci_remove "rs" "rs232"
	;;
	TRB14*)
		rm /etc/hotplug.d/tty/01-serial-symlink.sh
		uci_remove "rs" "rs232"
		uci_remove "rs" "rs485"
	;;
esac

uci_commit "rs"
