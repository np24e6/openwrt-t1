include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=ipq40xx
BOARDNAME:=Qualcomm Atheros IPQ40XX
FEATURES:=squashfs fpu nand smp
CPU_TYPE:=cortex-a7
CPU_SUBTYPE:=neon-vfpv4
SUBTARGETS:=generic

KERNEL_PATCHVER:=5.4

KERNELNAME:=zImage Image dtbs

include $(INCLUDE_DIR)/target.mk
include $(INCLUDE_DIR)/target-gpl.mk
DEFAULT_PACKAGES += \
	kmod-usb-dwc3-qcom \
	kmod-leds-gpio swconfig \
	kmod-ath10k  \
	kmod-usb3 kmod-usb-dwc3 ath10k-firmware-qca4019

define GPL/Target/Build
	$(GPL/Target/Build/Default)
	rm -f "$(1)/image/uboot_fw/cdt-AP.DK01.1-C2.bin"
	rm -f "$(1)/image/uboot_fw/nor_bootconfig.bin"
	rm -f "$(1)/image/uboot_fw/nor_bootconfig1.bin"
	rm -f "$(1)/image/uboot_fw/sbl1_nor.mbn"
	rm -f "$(1)/image/uboot_fw/tz.mbn"
endef

$(eval $(call BuildTarget))
