#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=relayd
PKG_VERSION:=2020-04-25
PKG_SOURCE_URL:=$(PROJECT_GIT)/project/relayd.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2020-04-25
PKG_SOURCE_VERSION:=f4d759be54ceb37714e9a6ca320d5b50c95e9ce9
PKG_MIRROR_HASH:=b1ff6e99072867be0975ba0be52ba9da3a876c8b8da893d68301e8238243a51e

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/relayd
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Routing and Redirection
  TITLE:=Transparent routing / relay daemon
  DEPENDS:=+libubox +vuci-app-relayd-ui +libubus
  ifeq (m, $(CONFIG_PACKAGE_relayd))
    PKG_TLT_NAME:=Relay configuration
    PKG_ROUTER:=$(TLT_PLATFORM_NAME)
  endif
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include

define Package/relayd/conffiles
/etc/config/relayd
endef

define Package/relayd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/relayd $(1)/usr/sbin/relayd

	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/etc/init.d/relayd $(1)/etc/init.d/

	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/etc/config/relayd $(1)/etc/config/

	$(INSTALL_DIR) $(1)/etc/uci-defaults/7.6
	$(INSTALL_DATA) ./files/relayd.defaults $(1)/etc/uci-defaults/7.6/10-move-relayd-from-network-conf
endef

$(eval $(call BuildPackage,relayd))
