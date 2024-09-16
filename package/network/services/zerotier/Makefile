#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=zerotier
PKG_VERSION:=1.14.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=1.14.0
PKG_SOURCE_URL:=https://github.com/zerotier/ZeroTierOne.git

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_HASH:=7191623a81b0d1b552b9431e8864dd3420783ee518394ac1376cee6aaf033291

PKG_MAINTAINER:=Moritz Warning <moritzwarning@web.de>
PKG_LICENSE:=BUSL-1.1
PKG_LICENSE_FILES:=LICENSE.txt

PKG_ASLR_PIE:=0
PKG_BUILD_PARALLEL:=1
PKG_BUILD_FLAGS:=gc-sections

include $(INCLUDE_DIR)/package.mk

define Package/zerotier
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libpthread +libstdcpp +kmod-tun +ip +libminiupnpc +libnatpmp +libatomic
  TITLE:=Create flat virtual Ethernet networks of almost unlimited size
  URL:=https://www.zerotier.com
  SUBMENU:=VPN
endef

define Package/zerotier/description
	ZeroTier package can establish Peer to Peer VPN (P2PVPN) connection
	between various devices running various operating systems. It also provides
	network management possibilities such as routing and creating firewall rules.
endef

define Package/zerotier/config
	source "$(SOURCE)/Config.in"
endef

ifeq ($(CONFIG_ZEROTIER_ENABLE_DEBUG),y)
MAKE_FLAGS += ZT_DEBUG=1
endif

MAKE_FLAGS += \
	ZT_EMBEDDED=1 \
	ZT_SSO_SUPPORTED=0 \
	DEFS="" \
	OSTYPE="Linux" \

define Build/Compile
	$(call Build/Compile/Default,one)
ifeq ($(CONFIG_ZEROTIER_ENABLE_SELFTEST),y)
	$(call Build/Compile/Default,selftest)
endif
endef

# Make binary smaller
TARGET_CFLAGS += -Wl,-z,noexecstack
TARGET_LDFLAGS += -Wl,--as-needed -Wl,-z,noexecstack

define Package/zerotier/conffiles
/etc/config/zerotier
endef

define Package/zerotier/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/etc/config $(1)/etc/mwan3
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/zerotier-one $(1)/usr/bin/
	$(LN) zerotier-one $(1)/usr/bin/zerotier-cli
	$(LN) zerotier-one $(1)/usr/bin/zerotier-idtool
	$(INSTALL_CONF) /dev/null $(1)/etc/config/zerotier

ifeq ($(CONFIG_ZEROTIER_ENABLE_SELFTEST),y)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/zerotier-selftest $(1)/usr/bin/
endif

	$(CP) ./files/etc/init.d $(1)/etc/
	$(INSTALL_BIN) ./files/etc/mwan3/mwan3.zerotier $(1)/etc/mwan3/
endef

define Package/zerotier/prerm
	#!/bin/sh
	. /lib/functions.sh

	clean_zones() {
		config_get name "$$1" name ""
		[ "$$name" = "zerotier" ] && uci_remove firewall "$$1"
	}

	clean_forwardings() {
		config_get dest "$$1" dest ""
		config_get src "$$1" src ""
		[ "$$dest" = "zerotier" ] || [ "$$src" = "zerotier" ] && uci_remove firewall "$$1"
	}

	config_load 'firewall'
	config_foreach clean_zones "zone"
	config_foreach clean_forwardings "forwarding"
	uci_commit firewall
	/etc/init.d/firewall reload > /dev/null 2>&1
	exit 0
endef

define Package/zerotier/postinst
	/etc/init.d/zerotier reload > /dev/null 2>&1
	exit 0
endef

$(eval $(call BuildPackage,zerotier))

