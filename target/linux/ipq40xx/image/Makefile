include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Build/UbootFw
	$(CP) ./uboot_fw/ $(STAGING_DIR_HOST)/
	if [ -e $(STAGING_DIR_HOST)/uboot_fw/tz.mbn ]; then \
		$(SED) 's~file\.ubi~$@~g' $(STAGING_DIR_HOST)/uboot_fw/norplusnand-flash.conf; \
		$(SED) 's~file\.elf~$(BIN_DIR)/openwrt-ipq40xx-u-boot-stripped.elf~g'  $(STAGING_DIR_HOST)/uboot_fw/norplusnand-flash.conf; \
		python $(TOPDIR)/target/linux/ipq40xx/image/uboot_fw/pack.py -t norplusnand -B -F boardconfig_premium_tlt -o $@ $(STAGING_DIR_HOST)/uboot_fw; \
	else \
		$(SED) 's~file\.ubi~$@~g' $(STAGING_DIR_HOST)/uboot_fw/norplusnand-apps-flash.conf; \
		$(SED) 's~file\.elf~$(BIN_DIR)/openwrt-ipq40xx-u-boot-stripped.elf~g'  $(STAGING_DIR_HOST)/uboot_fw/norplusnand-apps-flash.conf; \
		python $(TOPDIR)/target/linux/ipq40xx/image/uboot_fw/pack.py -t norplusnand -B -F appsboardconfig_premium_tlt -o $@ $(STAGING_DIR_HOST)/uboot_fw; \
	fi
endef

define Device/Default
	DEVICE_DTS_DIR := ../dts
	PROFILES := Default
	KERNEL = kernel-bin | gzip | fit gzip "$$(KDIR)/{$$(subst $$(space),$$(comma),$$(addprefix image-,$$(addsuffix .dtb,$$(DEVICE_DTS))))}"
	KERNEL_LOADADDR := 0x80208000
	DEVICE_DTS := $(foreach dts,$(notdir $(wildcard $(PLATFORM_DIR)/dts/*.dts)),$(patsubst %.dts,%,$(dts)))
	IMAGES := sysupgrade.bin
	IMAGE/sysupgrade.bin = sysupgrade-tar | append-metadata
	IMAGE/sysupgrade.bin/squashfs :=
endef

define Device/teltonika_rutx
	DEVICE_VENDOR := TELTONIKA
	DEVICE_MODEL := RUTX
	BOARD_NAME := rutx
	SOC := qcom-ipq4018
	DEVICE_DTS_CONFIG := config@5
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	FILESYSTEMS := squashfs
	DEVICE_FEATURES := usb-port gps serial modbus io wifi bt dualsim port-mirror ntrip hw-offload
	# Default common packages for RUTX series
	# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	# Essential must-have:
	DEVICE_PACKAGES := sysupgrade-helper uboot-ipq40xx kmod-cypress-serial

	# Wi-Fi related:
	DEVICE_PACKAGES += ipq-wifi-teltonika_rutx ipq-wifi-firmware-teltonika_rutx

	# USB related:
	DEVICE_PACKAGES += kmod-usb-uhci

	# Uncaterizored stuff
	DEVICE_PACKAGES.basic += kmod-r2ec stm32flasher crconf

	# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	KERNEL_IN_UBI := 1
	IMAGES := sysupgrade.bin
	IMAGE/sysupgrade.bin := append-ubi | UbootFw | append-metadata | finalize-tlt-webui

	# Hardware supprot
	# HW_SUPPORT := type1%type1_value1:type1_value2 type2%type2_value1 ...
	# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	HW_SUPPORT := io_expander%stm32:shiftreg_1

	HW_MODS := mod1%W25N02KV mod2%NAND_GD5F2GXX

endef
TARGET_DEVICES += teltonika_rutx

$(eval $(call BuildImage))
