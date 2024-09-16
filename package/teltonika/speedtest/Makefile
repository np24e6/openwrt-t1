#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=speedtest
PKG_VERSION:=2024-02-06
PKG_RELEASE:=3

PKG_SOURCE_VERSION:=1.7
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += $(FPIC)

define Package/speedtest
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=Speedtest
  DEPENDS:=+liblua +libcurl
endef

define Package/speedtest/description
	Speed Test package provides with the possibility to test
	the data transfer speed of your WAN connection.
endef


define Package/speedtest/install
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libspeedtest.so $(1)/usr/lib/lua/libspeedtest.so
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/speedtest.lua $(1)/usr/bin/speedtest
endef

$(eval $(call BuildPackage,speedtest))
