#!/bin/sh

[ "$#" != 4 ] && {
	echo "Usage: $0 <fw_version> <supported_devices> <metadata_template> <output>"
	exit 1
}

FW_VERSION="$1"
SUPPORTED_DEVICES="$2"
METADATA_TMPL="$3"
OUTPUT="$4"

dd if=/dev/urandom of="$OUTPUT" bs=1M count=1

sed \
	-e "s@<FW_VERSION>@$FW_VERSION@" \
	-e "s@<SUPPORTED_DEVICES>@$SUPPORTED_DEVICES@" \
	"$METADATA_TMPL" > /tmp/sysupgrade.meta.gen

fwtool -I /tmp/sysupgrade.meta.gen "$OUTPUT"
rm /tmp/sysupgrade.meta.gen
