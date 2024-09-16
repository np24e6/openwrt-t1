#
# Copyright (C) 2023 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=modbus_server
PKG_VERSION:=2024-04-02
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=7.7
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=net
	CATEGORY:=Network
	TITLE:=Modbus server daemon by Teltonika
	DEPENDS:=+libuci +libtlt_uci +libtlt_termios +libubus +libubox +libjson-c +libblobmsg-json \
		 +MOBILE_SUPPORT:libmdcollect +MOBILE_SUPPORT:libgsm \
		 +IO_SUPPORT:iomand +libmnfinfo +libmodbus +libboardjson
endef

define Package/$(PKG_NAME)/description
	MODBUS TCP/Serial server designed for MODBUS clients to read and write device data.
endef

define Package/$(PKG_NAME)/conffiles
/etc/config/modbus_server
endef


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/modbus_server.config $(1)/etc/config/modbus_server
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/modbus_server.init $(1)/etc/init.d/modbus_server
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/modbus_server $(1)/usr/sbin/modbus_server
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
