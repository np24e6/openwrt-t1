#
# Copyright (C) 2023 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=modbus_client
PKG_SOURCE_VERSION:=7.5

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=net
	CATEGORY:=Network
	TITLE:=MODBUS TCP/Serial client daemon by Teltonika
	DEPENDS:=+libuci +libtlt_uci +libsqlite3 +vuci-app-modbus-client-ui +libmodbus \
		+SERIAL_SUPPORT:vuci-app-modbus-serial-ui +IO_SUPPORT:iomand \
		+libgsm +libparam +libmosquitto +data-sender-mod-modbus

	ifeq (m, $(CONFIG_PACKAGE_modbus_client))
		PKG_TLT_NAME:=MODBUS Client
		PKG_ROUTER:=$(TLT_PLATFORM_NAME)
	endif
endef

define Package/$(PKG_NAME)/description
	MODBUS TCP/Serial client daemon by Teltonika
endef

define Package/$(PKG_NAME)/conffiles
/etc/config/$(PKG_NAME)
endef


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/modbus_client.conf $(1)/etc/config/modbus_client
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/modbus_client.init $(1)/etc/init.d/modbus_client
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/modbus_client $(1)/usr/sbin/modbus_client
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libalarms.so $(1)/usr/lib/libalarms.so
endef

define Package/$(PKG_NAME)/postinst
        #!/bin/sh

        [ -z "$${IPKG_INSTROOT}" ] || exit 0
        [ -e /lib/data_sender/libdata_sender.sh ] || exit 0

        . /lib/data_sender/libdata_sender.sh

        ds_find_plugin modbus && /etc/init.d/data_sender restart

        exit 0
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
