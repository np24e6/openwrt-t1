#
# Copyright (C) 2023 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=modbus_server
PKG_SOURCE_VERSION:=7.5

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=net
	CATEGORY:=Network
	TITLE:=Modbus server daemon by Teltonika
	DEPENDS:=+libuci +libtlt_uci +libubus +libubox +libjson-c +libblobmsg-json +libmdcollect +libgsm \
		+vuci-app-modbus-server-ui +IO_SUPPORT:iomand +libmnfinfo +libmodbus +libboardjson +USE_GLIBC:glib2

	ifeq (m, $(CONFIG_PACKAGE_modbus_server))
		PKG_TLT_NAME:=MODBUS Server
		PKG_ROUTER:=$(TLT_PLATFORM_NAME)
		PKG_FIRMWARE:=7.0
	endif
endef

ifeq ($(CONFIG_USE_GLIBC),y)
TARGET_CPPFLAGS +=-Dstrlcpy=g_strlcpy
TARGET_LDFLAGS +=-lglib-2.0
endif

define Package/$(PKG_NAME)/description
	Modbus server daemon by Teltonika
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
