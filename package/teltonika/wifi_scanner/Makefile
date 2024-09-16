#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=wifi_scanner
PKG_VERSION:=2023-08-31
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=1.6
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=$(PKG_NAME)
	DEPENDS:=+libuci +libubox +libtlt-logger +libubus +lsqlite3 +libiwinfo \
			+vuci-app-wifi-scanner-ui +data-sender-mod-wifiscan
	ifeq (m, $(CONFIG_PACKAGE_$(PKG_NAME)))
		PKG_TLT_NAME:=Wifi Scanner
		PKG_ROUTER:=$(TLT_PLATFORM_NAME)
	endif
endef

define Package/$(PKG_NAME)/description
	Wireless devices beacon and probe packet analyzer
endef

define Package/$(PKG_NAME)/conffiles
/etc/config/wifi_scanner
endef


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/config $(1)/etc/init.d
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/wifi_scanner.conf $(1)/etc/config/wifi_scanner
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/wifi_scanner.init $(1)/etc/init.d/wifi_scanner
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wifi_scanner $(1)/usr/sbin/
endef

define Package/$(PKG_NAME)/postinst
	#!/bin/sh

	[ -z "$${IPKG_INSTROOT}" ] || exit 0
	[ -e /lib/data_sender/libdata_sender.sh ] || exit 0

	. /lib/data_sender/libdata_sender.sh

	ds_find_plugin wifiscan && /etc/init.d/data_sender restart
	
	exit 0
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
