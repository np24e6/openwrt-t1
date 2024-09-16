#
# Copyright (C) 2022 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=libtlt_utils

PKG_SOURCE_VERSION:=1.3

GPL_INCLUDE_HEADERS:=1

include $(INCLUDE_DIR)/package.mk

define Package/libtlt_utils
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=Utils library by Teltonika
	MENU:=1
	DEPENDS:=+libubox +libuci
endef

define Package/libtlt_utils/description
	Utilities library. Created by Teltonika
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/*.so $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/include/*.h $(STAGING_DIR)/usr/include/
endef


define Package/libtlt_utils/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libtlt_utils.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libtlt_utils))
