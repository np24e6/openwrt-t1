#
# Copyright (C) 2022 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=opcua_client
PKG_SOURCE_VERSION:=7.5

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=net
	CATEGORY:=Network
	TITLE:=OPC UA client by Teltonika
	DEPENDS:=+libuci +libtlt_uci +libtlt_utils +libubox +libubus +libsqlite3 \
		+open62541 +vuci-app-opcua-ui +data-sender-mod-opcua
	PKG_TLT_NAME:=OPC UA Client
	PKG_ROUTER:=$(TLT_PLATFORM_NAME)
endef

define Package/$(PKG_NAME)/description
	OPC UA client by Teltonika
endef

define Package/$(PKG_NAME)/conffiles
/etc/config/$(PKG_NAME)
endef


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/opcua_client $(1)/usr/bin/opcua_client
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/opcua_client.init $(1)/etc/init.d/opcua_client
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/opcua_client.conf $(1)/etc/config/opcua_client
endef

define Build/Test
endef

define Package/$(PKG_NAME)/postinst
        #!/bin/sh

        [ -z "$${IPKG_INSTROOT}" ] || exit 0
        [ -e /lib/data_sender/libdata_sender.sh ] || exit 0

        . /lib/data_sender/libdata_sender.sh

        ds_find_plugin opcua && /etc/init.d/data_sender restart
        
        exit 0
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
