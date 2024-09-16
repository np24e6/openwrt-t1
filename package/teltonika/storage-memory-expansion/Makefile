#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=storage-memory-expansion
PKG_VERSION:=2023-07-25
PKG_RELEASE:=1

PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=utils
	CATEGORY:=Utilities
	DEPENDS:=+fstools +fmt-usb-msd
	TITLE:=Storage Memory Expansion script
	SUBMENU:=Filesystem
endef

define Package/$(PKG_NAME)/description
	Sets up overlay on a USB Mass Storage Device
endef



define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/bin $(1)/etc/hotplug.d/block
	$(INSTALL_BIN) ./files/sme.sh $(1)/bin/
	$(LN) /bin/sme.sh $(1)/etc/hotplug.d/block/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
