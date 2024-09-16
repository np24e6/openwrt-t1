#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=fwcert

PKG_SOURCE_VERSION:=1.0

PKG_CHECK_FORMAT_SECURITY:=1

PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/fwcert
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=+usign
  TITLE:=Firmware certificate verification utility
endef


define Package/fwcert/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/fwcert $(1)/usr/bin
endef

$(eval $(call BuildPackage,fwcert))
