include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=opcua_server
PKG_VERSION:=2023-12-08
PKG_RELEASE:=1
PKG_SOURCE_VERSION:=1.1

include $(INCLUDE_DIR)/package.mk

define Package/opcua_server
	SECTION:=net
	CATEGORY:=Network
	TITLE:=OPC UA server
	DEPENDS:=+open62541 +libmnfinfo +libuci +libtlt_uci +libubus +libboardjson +vuci-app-opcua-server-ui \
		+GPS_SUPPORT:libgps +MOBILE_SUPPORT:libgsm +IO_SUPPORT:iomand
	PKG_TLT_NAME:=OPC UA Server
	PKG_ROUTER:=$(TLT_PLATFORM_NAME)
endef

define Package/opcua_master/description
	OPC UA server by Teltonika
endef

define Package/opcua_master/conffiles
/etc/config/opcua_server
endef


define Package/opcua_server/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/opcua_server $(1)/usr/bin/opcua_server
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/opcua_server.init $(1)/etc/init.d/opcua_server
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/opcua_server.conf $(1)/etc/config/opcua_server
endef

define Build/Test
endef

$(eval $(call BuildPackage,opcua_server))
