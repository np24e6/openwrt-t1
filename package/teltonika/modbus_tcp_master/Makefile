#
# Copyright (C) 2022 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=modbus_tcp_master
PKG_VERSION:=2023-02-24
PKG_SOURCE_VERSION:=81511cbd66981c70acf891462980a431dce3eeb4

include $(INCLUDE_DIR)/package.mk

define Package/modbus_tcp_master
	SECTION:=net
	CATEGORY:=Network
	TITLE:=MODBUS TCP/Serial master daemon by Teltonika
	DEPENDS:=+libuci +libtlt_uci +libsqlite3 +vuci-app-modbus-ui +libmodbus \
		+MODBUS_SERIAL_SUPPORT:vuci-app-modbus-serial-ui +IO_SUPPORT:iomand \
		+libgsm +libparam +libmosquitto

	ifeq (m, $(CONFIG_PACKAGE_modbus_tcp_master))
		PKG_TLT_NAME:=MODBUS Master
		PKG_ROUTER:=$(TLT_PLATFORM_NAME)
	endif
endef

define Package/modbus_tcp_master/description
	MODBUS TCP/Serial master daemon by Teltonika
endef

define Package/modbus_tcp_master/conffiles
/etc/config/modbus_master
endef


define Package/modbus_tcp_master/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/modbus_master.conf $(1)/etc/config/modbus_master
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/modbus_master.init $(1)/etc/init.d/modbus_master
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/modbus_master $(1)/usr/sbin/modbus_master
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libalarms.so $(1)/usr/lib/libalarms.so
endef

$(eval $(call BuildPackage,modbus_tcp_master))
