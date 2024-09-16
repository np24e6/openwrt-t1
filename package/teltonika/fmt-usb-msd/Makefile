#
# Copyright (C) 2023 Teltonika
#

include $(TOPDIR)/rules.mk

PKG_NAME:=fmt-usb-msd
PKG_VERSION:=2023-01-11

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=utils
	CATEGORY:=Utilities
	DEPENDS:=+fstools +libubox +e2fsprogs
	TITLE:=USB Mass Storage Device formatting utility
	SUBMENU:=Filesystem
endef

define Package/$(PKG_NAME)/description
	Formats USB Mass Storage Devices to exFat or journal-less EXT4
endef



define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/bin $(1)/etc/hotplug.d/block
	$(INSTALL_BIN) ./files/$(PKG_NAME).sh $(1)/bin/
	$(INSTALL_BIN) ./files/$(PKG_NAME)_last.sh $(1)/etc/hotplug.d/block/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
