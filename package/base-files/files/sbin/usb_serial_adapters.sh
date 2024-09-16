#!/bin/sh

. /usr/share/libubox/jshn.sh

json_init
json_add_array "usb_serial"
ls /dev/usb_serial_* >/dev/null 2>/dev/null && for d in /dev/usb_serial_*; do
	json_add_string "" "${d##*usb_serial_}"
done
json_close_array
json_close_object

json_dump -i
