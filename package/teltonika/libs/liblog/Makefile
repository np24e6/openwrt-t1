#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=liblog
PKG_VERSION:=2024-08-02
PJG_RELEASE:=1

PKG_SOURCE_VERSION:=1.4
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/liblog
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=A library for logd DB management. Select, insert etc.
	DEPENDS:=+libubus +libubox
endef

define Package/liblog/description
	A simple lib with ubus to manage logd.
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/*.so $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/src/include/*.h $(STAGING_DIR)/usr/include/
endef


define Package/liblog/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/liblog.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,liblog))
