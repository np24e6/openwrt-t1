#
# Copyright (C) 2022 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=libboardjson

PKG_SOURCE_VERSION:=7.5

GPL_INCLUDE_HEADERS:=1

include $(INCLUDE_DIR)/package.mk

define Package/libboardjson
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=A shell utility and a library that provides an easy way to read /etc/board.json
	DEPENDS:=+libubus +libblobmsg-json +rpcd
endef

define Package/libboardjson/description
	A shell utility and a library that provides an easy way to read /etc/board.json. Refer to this package's git repo for further documentation. Created by Teltonika.
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib/rpcd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bjson_rpc.so $(1)/usr/lib/rpcd
	$(CP) $(PKG_BUILD_DIR)/libboardjson.so $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/src/include/*.h $(STAGING_DIR)/usr/include/
endef


define Package/libboardjson/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/lib/rpcd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bjson_rpc.so $(1)/usr/lib/rpcd
	$(CP) $(PKG_BUILD_DIR)/libboardjson.so $(1)/usr/lib
endef

$(eval $(call BuildPackage,libboardjson))
