#!/bin/sh

. /lib/functions.sh

rm_section() {
	uci batch << EOF
delete rs.rs232_usb
commit rs
EOF
	exit 0
}

# delete the section if usb_serial has never been configured or no adapter is currently present
uci_get "rs" "rs232_usb" "type" >&- || rm_section
first_inserted=$(basename "$(ls -tr1 /dev/rs232_usb_* | head -n 1)") 2>&- || rm_section

uci batch << EOF
set rs.rs232_usb.id=${first_inserted##*_}
set rs.rs232_usb.name=Unnamed
rename rs.rs232_usb=$first_inserted
commit rs
EOF

exit 0
