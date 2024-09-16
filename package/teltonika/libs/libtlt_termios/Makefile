#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libtlt_termios
PKG_SOURCE_VERSION:=7.9
PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=Terminal settings handling library by Teltonika
	ABI_VERSION:=1.0
	DEPENDS:=+libubox +libuci +libboardjson +libgpiod
endef

define Package/$(PKG_NAME)/description
	Terminal settings handling library. Created by Teltonika
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib/rpcd $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/libtlt_termios.so $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/serial.so $(1)/usr/lib/rpcd/
	$(CP) $(PKG_BUILD_DIR)/include/*.h $(1)/usr/include/
endef


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/lib/rpcd
	$(CP) $(PKG_BUILD_DIR)/libtlt_termios.so $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/serial.so $(1)/usr/lib/rpcd/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
