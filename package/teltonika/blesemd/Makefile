#
# Copyright (C) 2022 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=blesemd
PKG_SOURCE_VERSION:=7.5-2

include $(INCLUDE_DIR)/package.mk

define Package/blesemd
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Bluetooth Low Energy Sensor management daemon
	DEPENDS:=+libubus +libuci +libblobmsg-json +bluez-libs +lsqlite3
endef

define Package/blesemd/description
	Management utility for Bluetooth Low Energy devices
endef

define Package/blesemd/conffiles
/etc/config/blesem
/etc/config/ble_devices
endef


define Package/blesemd/install
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/etc/config $(1)/usr/bin

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/blesem.init $(1)/etc/init.d/blesem
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/blesemd $(1)/usr/bin/blesemd
endef

$(eval $(call BuildPackage,blesemd))
