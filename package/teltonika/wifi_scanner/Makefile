#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=wifi_scanner
PKG_VERSION:=2023-01-30
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=d549842bddd1d23c3eb8aff167a9aef824debb0a
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=$(PKG_NAME)
	DEPENDS:=+libuci +libubox +libtlt-logger +libubus +lsqlite3 +libiwinfo +vuci-app-wifi-scanner-ui
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

$(eval $(call BuildPackage,$(PKG_NAME)))
