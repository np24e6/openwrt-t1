. /lib/upgrade/common.sh

PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	platform_check_image_ipq "$1"
}

platform_do_upgrade() {
	platform_do_upgrade_ipq "$1"
}

compatible_to_eg060k() {
	{ ! find_hw_mod "EG060K"; } && {
		echo "EG060K modem detected but fw does not support it"
		return 1
	}
	return 0
}

prepare_metadata_hw_supp () {
	local metadata="/tmp/sysupgrade.meta"

	[ -e "$metadata" ] || { fwtool -q -i $metadata "$1"; } && {
		json_load_file "$metadata"
		json_select hw_support 1> /dev/null && {
			json_get_values hw_support
			json_select io_expander
			json_get_values io_exp_values
		}
		return 0
	}
	return 1
}

find_io_exp() {
	for val in $io_exp_values; do
		echo "$val" | grep -q "$1" && return 0
	done
	return 1
}

platform_check_hw_support() {
	local io_expander_file="/proc/device-tree/io_expander"
	local nand_model_file="/sys/class/mtd/mtd16/nand_model"

	{ !  prepare_metadata_hw_supp "$1"; } && return 1

	# io_expander type validation
	if grep -q '^shiftreg_1$' "$io_expander_file"; then
		{ ! find_io_exp "shiftreg_1"; } && {
			echo "Shift registers detected but fw does not support it"
			return 1
		}
	else
		# fail if not default/initial type
		grep -q '^stm32$' "$io_expander_file" || return 1
	fi

	{ ! prepare_metadata_hw_mods "$1"; } && return 1

	# nand type validation
	grep -q '^W25N02KV' "$nand_model_file" && { ! find_hw_mod "W25N02KV"; } && {
		echo "Winbond NAND detected but fw does not support it"
		return 1
	}
	grep -q '^GD5F2GM7\|^GD5F2GQ5' "$nand_model_file" && { ! find_hw_mod "NAND_GD5F2GXX"; } && {
		echo "GigaDevices NAND detected but fw does not support it"
		return 1
	}

	eval "$( jsonfilter -q -i "/etc/board.json" \
		-e "vendor=@['modems'][@.builtin=true].vendor" \
		-e "product=@['modems'][@.builtin=true].product" \
		)"

	# device without modem
	[ -z "$vendor" ] || [ -z "$product" ] && return 0

	[ "${vendor}:${product}" != "2c7c:030b" ] && return 0

	compatible_to_eg060k

	return "$?"
}
