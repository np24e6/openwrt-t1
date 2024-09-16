#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=vrrpd
PKG_VERSION:=1.15
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-or-later

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/fredbcode/Vrrpd.git
PKG_SOURCE_VERSION:=a318281271973c7430cfa520b540585153454c4b
PKG_HASH:=9005ea2c42734ccbb1646c685a1f6321653f6b01e1b219135872780c243e07b4

PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz

include $(INCLUDE_DIR)/package.mk

define Package/vrrpd
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Routing and Redirection
  TITLE:=Virtual Router Redundancy Protocol daemon
  URL:=http://www.sourceforge.net/projects/vrrpd
  DEPENDS:=+liblog +librt +libubus
endef

define Package/vrrpd/description
	VRRPd is an implementation of Virtual Router Redundancy 
	Protocol as specified in rfc2338. VRRPd is interoperable with 
	other RFC-based VRRP implementations, including Cisco and 
	Juniper, and is included as a standard feature on ImageStream 
	routers.
endef

define Build/Compile
	$(call Build/Compile/Default, \
		DBG_OPT="" \
		MAIN_OPT="$(TARGET_CFLAGS)" \
		vrrpd \
	)
endef

define Package/vrrpd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/vrrpd $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/vrrp_check.sh $(1)/usr/sbin/vrrp_check.sh
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/vrrpd.config $(1)/etc/config/vrrpd
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/vrrpd.init $(1)/etc/init.d/vrrpd
	$(INSTALL_BIN) ./files/vrrp_check.init $(1)/etc/init.d/vrrp_check
	$(INSTALL_DIR) $(1)/etc/uci-defaults/7.6
	$(INSTALL_BIN) ./files/uci-defaults/99_virtual_mac_invert $(1)/etc/uci-defaults/7.6/
endef

define Package/vrrpd/conffiles
/etc/config/vrrpd
endef

$(eval $(call BuildPackage,vrrpd))
