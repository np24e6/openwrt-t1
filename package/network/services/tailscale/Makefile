#
# Copyright (C) 2021 CZ.NIC, z. s. p. o. (https://www.nic.cz/)
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tailscale
PKG_VERSION:=1.68.1
PKG_RELEASE:=1

PKG_HASH:=d7fe30282d2f5eabdc76a5a89f11d935ed3a5d93d55f5fd5b40f9a9f49e19490

PKG_SOURCE_VERSION:=1.68.1
PKG_SOURCE=tailscale-$(PKG_SOURCE_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/tailscale/tailscale/tar.gz/v$(PKG_SOURCE_VERSION)?

PKG_MAINTAINER:=Jan Pavlinec <jan.pavlinec1@gmail.com>
PKG_LICENSE:=BSD-3-Clause
PKG_LICENSE_FILES:=LICENSE

PKG_BUILD_DEPENDS:=golang/host upx/host
PKG_BUILD_PARALLEL:=1
PKG_BUILD_FLAGS:=no-mips16

GO_PKG:=\
	tailscale.com/cmd/tailscale \
	tailscale.com/cmd/tailscaled
GO_PKG_LDFLAGS:=-s -w -X 'tailscale.com/version.longStamp=$(PKG_VERSION)-$(PKG_RELEASE) (OpenWrt)'
GO_PKG_LDFLAGS_X:=tailscale.com/version.shortStamp=$(PKG_VERSION)
GO_PKG_TAGS:=ts_include_cli


include $(INCLUDE_DIR)/package.mk
include $(TOPDIR)/package/lang/golang/golang-package.mk

define Package/tailscale
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=VPN
  TITLE:=Zero config VPN
  URL:=https://tailscale.com
  DEPENDS:=$(GO_ARCH_DEPENDS) +ca-bundle +kmod-tun
endef

define Build/Compile
	$(call GoPackage/Build/Compile)
	$(STAGING_DIR_HOST)/bin/upx --lzma --best $(GO_PKG_BUILD_BIN_DIR)/tailscaled
endef

define Package/tailscale/description
  It creates a secure network between your servers, computers,
  and cloud instances. Even when separated by firewalls or subnets.
endef

define Package/tailscale/conffiles
/etc/config/tailscale
/etc/tailscale/
endef

define Package/tailscale/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(GO_PKG_BUILD_BIN_DIR)/tailscaled $(1)/usr/sbin/tailscale.combined
	ln -s /usr/sbin/tailscale.combined $(1)/usr/sbin/tailscale
	ln -s /usr/sbin/tailscale.combined $(1)/usr/sbin/tailscaled
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/tailscale.init $(1)/etc/init.d/tailscale
	$(INSTALL_DIR) $(1)/etc/config/
	$(INSTALL_DATA) ./files/tailscale.conf $(1)/etc/config/tailscale
	$(INSTALL_DIR) $(1)/etc/uci-defaults/7.6
	$(CP) ./files/defaults/99_tailscale_fw_rules $(1)/etc/uci-defaults/7.6/99_tailscale_fw_rules
endef

$(eval $(call BuildPackage,tailscale))
