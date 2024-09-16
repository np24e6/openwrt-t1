#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=esim-lpac

PKG_SOURCE_VERSION:=1.1.3
PKG_LICENSE:=Teltonika-closed

GPL_INCLUDE_HEADERS:=1


include $(INCLUDE_DIR)/package.mk

export VERSION_FLAGS = -DLPA_VERSION=\"$(PKG_SOURCE_VERSION)\"

define Package/esim-lpac
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=eSIM Lpac manager
	DEPENDS:= +liblpac
endef

define Package/liblpac
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=A library for eSIM Lpac manager
	DEPENDS:= +libgsm +libubus +libcurl
endef

define Package/lua_lpac
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=A lua library for eSIM Lpac manager
	DEPENDS:= +liblpac +libubus +lua
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR)/usr/lib $(STAGING_DIR)/usr/include
	$(CP) $(PKG_BUILD_DIR)/*.so $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/include/*.h $(STAGING_DIR)/usr/include/
endef


define Package/liblpac/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/liblpac.so $(1)/usr/lib
endef

define Package/lua_lpac/install
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(CP) $(PKG_BUILD_DIR)/lua/lua_lpac.so $(1)/usr/lib/lua
	$(CP) $(PKG_BUILD_DIR)/lua/lpac.lua $(1)/usr/lib/lua
endef

define Package/esim-lpac/install
	$(INSTALL_DIR) $(1)/sbin $(1)/etc
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lpac $(1)/sbin/lpac
	$(INSTALL_CONF) files/lpac_config.json $(1)/etc
endef

$(eval $(call BuildPackage,liblpac))
$(eval $(call BuildPackage,lua_lpac))
$(eval $(call BuildPackage,esim-lpac))
