#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libtlt_termios
PKG_SOURCE_VERSION:=7.7
PKG_LICENSE:=Teltonika-closed

GPL_INCLUDE_HEADERS:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=Terminal settings handling library by Teltonika
	DEPENDS:=+libubox +libuci +libboardjson +libgpiod
endef

define Package/$(PKG_NAME)/description
	Terminal settings handling library. Created by Teltonika
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/*.so $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/include/*.h $(1)/usr/include/
endef


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libtlt_termios.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
