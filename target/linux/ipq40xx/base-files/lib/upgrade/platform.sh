PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	platform_check_image_ipq "$1"
}

platform_do_upgrade() {
	platform_do_upgrade_ipq "$1"
}

prepare_metadata_hw_mods () {
	local metadata="/tmp/sysupgrade.meta"

	[ -e "$metadata" ] || { fwtool -q -i $metadata "$1"; } && {
		json_load_file "$metadata"
		json_select hw_mods 1> /dev/null && {
			json_get_values hw_mods
		}
		return 0
	}
	return 1
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

find_hw_mod() {
	echo "$hw_mods" | grep -q "$1"
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

	if prepare_metadata_hw_mods "$1"; then
		# nand type validation
		grep -q '^W25N02KV$' "$nand_model_file" && { ! find_hw_mod "W25N02KV"; } && {
			echo "Winbond NAND detected but fw does not support it"
			return 1
		}
	else
		return 1
	fi

	if prepare_metadata_hw_supp "$1"; then
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
	else
		return 1
	fi

	return 0
}

