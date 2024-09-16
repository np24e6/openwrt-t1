#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tcpdump
PKG_VERSION:=4.99.4
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://www.tcpdump.org/release/
PKG_HASH:=0232231bb2f29d6bf2426e70a08a7e0c63a0d59a9b44863b7f5e2357a6e49fea

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>
PKG_LICENSE:=BSD-3-Clause
PKG_CPE_ID:=cpe:/a:tcpdump:tcpdump

PKG_INSTALL:=1
PKG_BUILD_FLAGS:=gc-sections
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/tcpdump/default
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libpcap
  TITLE:=Network monitoring and data acquisition tool
  URL:=http://www.tcpdump.org/
endef

define Package/tcpdump
  $(Package/tcpdump/default)
  VARIANT:=full
  ifeq (m, $(CONFIG_PACKAGE_tcpdump))
    PKG_TLT_NAME:=TCPdump
    PKG_ROUTER:=$(TLT_PLATFORM_NAME)
  endif
endef

define Package/tcpdump-mini
  $(Package/tcpdump/default)
  TITLE+= (minimal version)
  VARIANT:=mini
endef

# Remove TCPdump configuration from system config file
define Package/tcpdump/prerm
  #!/bin/sh
  . /lib/functions.sh
  config_load system
  uci_remove system system tcp_dump
  uci_remove system system tcp_dump_interface
  uci_remove system system tcp_dump_filter
  uci_remove system system tcp_inout
  uci_remove system system tcp_host
  uci_remove system system tcp_port
  uci_remove system system tcp_mount
  uci_commit
endef

CONFIGURE_ARGS += \
	--without-cap-ng \
	--without-crypto \
	$(call autoconf_bool,CONFIG_IPV6,ipv6)

ifeq ($(BUILD_VARIANT),mini)
  TARGET_CFLAGS += -DTCPDUMP_MINI
  CONFIGURE_ARGS += --disable-smb
  MAKE_FLAGS += TCPDUMP_MINI=1
endif

define Package/tcpdump/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/tcpdump $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/tcpdebug.init $(1)/etc/init.d/tcpdebug
endef

Package/tcpdump-mini/install = $(Package/tcpdump/install)

$(eval $(call BuildPackage,tcpdump))
$(eval $(call BuildPackage,tcpdump-mini))
