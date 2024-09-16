#
# Copyright (C) 2006-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sstp-client
PKG_VERSION:=1.0.19
PKG_RELEASE:=2

PKG_HASH:=e2652365f69f5037102e78f4e115ff764a390b27bb3fd513a8a50b10a61bb613

PKG_SOURCE_VERSION:=1.0.19
PKG_SOURCE=sstp-client-$(PKG_SOURCE_VERSION).tar.gz
PKG_SOURCE_URL:=https://gitlab.com/sstp-project/sstp-client/-/archive/$(PKG_SOURCE_VERSION)/

PKG_MAINTAINER:=Federico Di Marco <fededim@gmail.com>
PKG_LICENSE:=GPL-2.0-or-later
PKG_LICENSE_FILES:=COPYING

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/sstp-client
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=VPN
  DEPENDS=+libevent2 +libopenssl +ppp +resolveip
  TITLE:=SSTP-Client is a SSTP client for Linux.
  URL:=http://sstp-client.sourceforge.net/
  PKG_NETWORK_RESTART:=1
endef

define Package/sstp-client/description
	Secure Socket Tunneling Protocol (SSTP) is a VPN protocol
	designed to transport PPP traffic via a secure SSL/TLS channel.
endef

define Package/sstp-client/conffiles
/etc/ppp/chap-secrets
/etc/ppp/peers/peer-sstp-example-nopty.txt
/etc/ppp/peers/peer-sstp-example.txt
endef

TARGET_CPPFLAGS += -I$(PKG_BUILD_DIR)/include

define Package/sstp-client/install
	$(INSTALL_DIR) $(1)/usr/{bin,lib}
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/.libs/sstpc $(1)/usr/bin/
	$(CP) $(PKG_BUILD_DIR)/src/libsstp-api/.libs/*.so* $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/src/pppd-plugin/.libs/*.so* $(1)/usr/lib/

	$(INSTALL_DIR) $(1)/etc
	$(CP) files/etc $(1)
	$(INSTALL_DIR) $(1)/lib/netifd/proto
	$(LN) /lib/netifd/proto/ppp.sh $(1)/lib/netifd/proto/sstp.sh
endef

define Package/sstp-client/prerm
	#!/bin/sh
	. /lib/functions.sh
	
	clean_config() {
		config_get proto "$$1" proto ""
		[ "$$proto" = "sstp" ] && {
			uci_remove network "$$1";
			ifdown "$$1";
		}
	}

	clean_forward() {
		config_get dest "$$1" dest ""
		config_get src "$$1" src ""

		[ "$$dest" = "sstp" ] || [ "$$src" = "sstp" ] && {
			uci_remove firewall "$$1"
		}
	}

	clean_zone() {
		config_get name "$$1" name ""
		
		[ "$$name" = "sstp" ] && {
			uci_remove firewall "$$1"
		}
	}

	config_load 'network'
	config_foreach clean_config "interface"
	config_load 'firewall'
	config_foreach clean_forward "forwarding"
	config_foreach clean_zone "zone"
	uci_commit network
	uci_commit firewall
	/etc/init.d/firewall reload > /dev/null 2>&1
endef

$(eval $(call BuildPackage,sstp-client))
