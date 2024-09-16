#
# Copyright (C) 2013-2016 OpenWrt.org
# Copyright (C) 2016 LEDE project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=firewall
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(PROJECT_GIT)/project/firewall3.git
PKG_SOURCE_DATE:=2021-03-23
PKG_SOURCE_VERSION:=61db17edddb1f05e8107f0dbef6f7d060ce67483
PKG_MIRROR_HASH:=b2eb09816640e14e2dae21fb54ea05c33858fe0004844fe8d99e541a2e19e9c0
PKG_MAINTAINER:=Jo-Philipp Wich <jo@mein.io>
PKG_LICENSE:=ISC

PKG_CONFIG_DEPENDS := CONFIG_IPV6

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/firewall
  SECTION:=net
  CATEGORY:=Base system
  TITLE:=OpenWrt C Firewall
  DEPENDS:=+libubox +libubus +libuci +libip4tc +IPV6:libip6tc +libxtables +kmod-ipt-core +kmod-ipt-conntrack +IPV6:kmod-nf-conntrack6 +kmod-ipt-nat +libnetfilter-conntrack +libmnl +AP_DEVICE:iptables-mod-filter
endef

define Package/firewall/description
 This package provides a config-compatible C implementation of the UCI firewall.
endef

define Package/firewall/conffiles
/etc/config/firewall
/etc/firewall.user
endef

TARGET_CFLAGS += -ffunction-sections -fdata-sections -flto
TARGET_LDFLAGS += -Wl,--gc-sections -flto -lmnl -lnetfilter_conntrack
CMAKE_OPTIONS += $(if $(CONFIG_IPV6),,-DDISABLE_IPV6=1)

FIREWALL_CONFIG="firewall.config"

ifeq ($(TLT_PLATFORM_TAP100), y)
	FIREWALL_CONFIG=firewall.config-tap100
endif

ifeq ($(TLT_PLATFORM_TAP200), y)
	FIREWALL_CONFIG=firewall.config-tap200
endif

ifeq ($(CONFIG_HW_OFFLOAD), y)
	FIREWALL_CONFIG=firewall.config-offload-full
endif

ifeq ($(CONFIG_SW_OFFLOAD), y)
	FIREWALL_CONFIG=firewall.config-offload-sw
endif

ifeq ($(CONFIG_GATEWAY_DEVICE), y)
	FIREWALL_CONFIG=firewall.config-without-wan
endif

ifeq ($(CONFIG_SW_OFFLOAD), y)
ifeq ($(CONFIG_GATEWAY_DEVICE), y)
	FIREWALL_CONFIG=firewall.config-offload-sw-without-wan
endif
endif

ifeq ($(TLT_PLATFORM_TCR1), y)
	FIREWALL_CONFIG=firewall.config-tcr100
endif

define Package/firewall/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/firewall3 $(1)/sbin/fw3
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/firewall.init $(1)/etc/init.d/firewall
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_CONF) ./files/firewall.hotplug $(1)/etc/hotplug.d/iface/20-firewall
	$(INSTALL_DIR) $(1)/etc/config/
	$(INSTALL_CONF) ./files/$(FIREWALL_CONFIG) $(1)/etc/config/firewall
	$(INSTALL_DIR) $(1)/etc/
	$(INSTALL_CONF) ./files/firewall.user $(1)/etc/firewall.user
	$(INSTALL_DIR) $(1)/usr/share/fw3
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/helpers.conf $(1)/usr/share/fw3

	$(if $(CONFIG_AP_DEVICE),,\
		$(INSTALL_DIR) $(1)/usr $(1)/usr/bin; \
		$(INSTALL_BIN) ./files/attack_prevention.sh $(1)/usr/bin/attack_prevention;)

	if [ $(TLT_PLATFORM_RUTX) ]; then \
		$(INSTALL_DIR) $(1)/usr/bin; \
		$(INSTALL_BIN) ./files/rutx_hnat.sh $(1)/usr/bin/rutx_hnat; \
	fi;
endef

$(eval $(call BuildPackage,firewall))
