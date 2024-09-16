#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=profiler
PKG_VERSION:=2023-02-14
PKG_RELEASE:=1

PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=2fa29fc9eba0f6110e43b21596a608471462a1ea

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz

include $(INCLUDE_DIR)/package.mk

define Package/profiler
	SECTION:=base
	CATEGORY:=Base system
	DEPENDS:= +libubox +libuci +libubus
	TITLE:=Configuration profiles scheduler
endef

define Package/profiler/description
	Configuration profiles scheduler
endef

define Package/profiler/conffiles
/etc/config/profiles
endef


define Package/profiler/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/config $(1)/usr/libexec/rpcd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/profiler $(1)/usr/sbin/
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/profiles.conf $(1)/etc/config/profiles
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/profiler.init $(1)/etc/init.d/profiler
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/profile.sh $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/profile-rpcd.sh $(1)/usr/libexec/rpcd/profile
endef

$(eval $(call BuildPackage,profiler))
