#
# Copyright (C) 2021-2022 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=ntpclient
PKG_VERSION:=2024-06-17
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=2015_365
PKG_SOURCE_URL:=http://doolittle.icarus.com/ntpclient/
PKG_SOURCE:=$(PKG_NAME)_$(PKG_SOURCE_VERSION).tar.gz
PKG_HASH:=95c3646919a9a2278fa384aa8e37117b250577ff8b09b8c93ba53e04e729b54b


PKG_ORIGIN_URL:=https://github.com/Telconet/ntpclient
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=README

include $(INCLUDE_DIR)/package.mk

define Package/ntpclient
  SUBMENU:=Time Synchronization
  SECTION:=net
  CATEGORY:=Network
  TITLE:=NTP (Network Time Protocol) client
  DEPENDS:=+librt +libubus +libubox +libblobmsg-json +libuci +MOBILE_SUPPORT:libgsm
endef

define Package/ntpclient/description
	NTP client for setting system time from NTP servers.
endef

define Package/ntpclient/conffiles
/etc/config/ntpclient
endef

MAKE_FLAGS += all adjtimex

define Package/ntpclient/install
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/etc/hotplug.d/iface $(1)/etc/hotplug.d/net $(1)/etc/config $(1)/usr/sbin

	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/ntpclient.hotplug $(1)/etc/hotplug.d/iface/20-ntpclient

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/ntpclient.init $(1)/etc/init.d/ntpclient
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/ntpclient.config $(1)/etc/config/ntpclient

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ntpclient $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/adjtimex $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/rate.awk $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,ntpclient))
