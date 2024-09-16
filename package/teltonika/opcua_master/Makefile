#
# Copyright (C) 2022 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=opcua_master
PKG_VERSION:=2022-12-21
PKG_SOURCE_VERSION:=f9c0cc56587e8c0ec02fa443154f514b23e87e98

include $(INCLUDE_DIR)/package.mk

define Package/opcua_master
	SECTION:=net
	CATEGORY:=Network
	TITLE:=OPC UA master by Teltonika
	DEPENDS:=+libuci +libtlt_uci +libtlt_utils +libubox +libubus +libsqlite3 +open62541 +vuci-app-opcua-ui
	PKG_TLT_NAME:=OPC UA
	PKG_ROUTER:=$(TLT_PLATFORM_NAME)
endef

define Package/opcua_master/description
	OPC UA master by Teltonika
endef

define Package/opcua_master/conffiles
/etc/config/opcua_master
endef


define Package/opcua_master/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/opcua_master $(1)/usr/bin/opcua_master
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/opcua_master.init $(1)/etc/init.d/opcua_master
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/opcua_master.conf $(1)/etc/config/opcua_master
endef

$(eval $(call BuildPackage,opcua_master))
