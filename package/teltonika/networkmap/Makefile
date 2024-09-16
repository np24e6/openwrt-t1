#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk
PKG_RELEASE:=1

PKG_NAME:=networkmap
PKG_VERSION:=2024-03-06

PKG_SOURCE_VERSION:=1.2
PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/networkmap
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Networkmap tool
endef

define Package/networkmap/description
        Devices scanner.
endef



define Package/networkmap/install
	$(INSTALL_DIR) $(1)/usr/libexec/rpcd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/networkmap $(1)/usr/libexec/rpcd/
endef

$(eval $(call BuildPackage,networkmap))
