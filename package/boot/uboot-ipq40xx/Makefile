#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=uboot

PKG_SOURCE_VERSION:=2.4

PKG_LICENSE:=GPL-2.0-or-later

DEVICE_VENDOR='teltonika'

include $(INCLUDE_DIR)/package.mk

define Package/uboot-ipq40xx
  SECTION:=boot
  CATEGORY:=Boot Loaders
  TITLE:=U-boot for ipq40xx based platforms
  URL:=http://www.denx.de/wiki/U-Boot
endef

define Build/Configure
	$(MAKE) -C $(PKG_BUILD_DIR) ipq40xx_cdp_config
endef

define Build/Compile
	VERSION_CODE='$(VERSION_NICK) $(VERSION_NUMBER)' \
	REVISION='$(REVISION)' DEVICE_VENDOR='$(DEVICE_VENDOR)' \
	$(MAKE) -C $(PKG_BUILD_DIR) HOSTCC=$(TARGET_CC) \
		HOSTSTRIP=true CROSS_COMPILE=$(TARGET_CROSS) \
		QSDK_CFLAGS="$(TARGET_CFLAGS)" \
		QSDK_LDFLAGS="$(TARGET_LDFLAGS)"

endef

define Package/uboot-ipq40xx/install
	$(INSTALL_DIR) $(1)
	$(CP) $(PKG_BUILD_DIR)/u-boot $(BIN_DIR)/openwrt-ipq40xx-u-boot.elf
	$(CP) $(PKG_BUILD_DIR)/striped-u-boot.elf $(BIN_DIR)/openwrt-ipq40xx-u-boot-stripped.elf
	$(CP) $(PKG_BUILD_DIR)/u-boot.bin $(BIN_DIR)/openwrt-ipq40xx-u-boot.img
endef

$(eval $(call BuildPackage,uboot-ipq40xx))
