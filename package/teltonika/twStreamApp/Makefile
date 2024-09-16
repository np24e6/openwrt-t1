#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=twStreamApp
PKG_VERSION:=2024-08-13
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=1.7
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz

PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/twStreamApp
  SECTION:=net
  CATEGORY:=Network
  TITLE:=ThingWorx Stream application.
  DEPENDS:=+libtwCSdk +MOBILE_SUPPORT:libgsm +libtlt_uci +libuci +libopenssl +libubus
endef

define Package/twStreamApp/description
	Thingworx is a package meant for interaction with
	industrial innovation platform ThingWorx.
endef

define Package/twStreamApp/conffiles
/etc/config/iottw
endef

MAKE_FLAGS += \
	TW_OS_INCLUDE="twLinux-openssl.h" \
	INCLUDE_DIR=$(STAGING_DIR)/usr/include/libtwCSdk


define Package/twStreamApp/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/twStreamApp $(1)/usr/bin/twStreamApp
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/twd.init $(1)/etc/init.d/twd
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/iottw $(1)/etc/config/iottw
endef

$(eval $(call BuildPackage,twStreamApp))
