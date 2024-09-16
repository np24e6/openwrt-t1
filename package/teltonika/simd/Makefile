#
# Copyright (C) 2022 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=simd

PKG_SOURCE_VERSION:=9c1f4153250903928a2fb9df0584577c949ab287

include $(INCLUDE_DIR)/package.mk

define Package/rpcd-mod-simd/description
	This application provides sim utility methods through ubus
endef

define Package/rpcd-mod-simd
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=simd rpcd module
  DEPENDS:=+rpcd
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/include/libsim.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/libsim.so $(1)/usr/lib/
endef


define Package/rpcd-mod-simd/install
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/usr/lib/rpcd $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/simcard.init $(1)/etc/init.d/simcard
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/simd.so $(1)/usr/lib/rpcd/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libsim.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,rpcd-mod-simd))
