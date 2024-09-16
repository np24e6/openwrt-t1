#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=libtlt_smtp

PKG_SOURCE_VERSION:=1.2

GPL_INCLUDE_HEADERS:=1

include $(INCLUDE_DIR)/package.mk

define Package/libtlt_smtp
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=SMTP library by Teltonika
	MENU:=1
	DEPENDS:=+libcurl
endef

define Package/libtlt_smtp/description
	SMTP library. Created by Teltonika
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/*.so $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/include/*.h $(STAGING_DIR)/usr/include/
endef


define Package/libtlt_smtp/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libtlt_smtp.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libtlt_smtp))
