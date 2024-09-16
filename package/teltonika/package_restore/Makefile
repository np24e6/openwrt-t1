#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=package_restore
PKG_VERSION:=2024-04-15
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=3.4.1
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/package_restore
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Auto package restore
endef

define Package/package_restore/conffiles
/etc/config/package_restore
endef



define Package/package_restore/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/package_restore.sh $(1)/sbin/package_restore.sh

	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/package_restore.init $(1)/etc/init.d/package_restore

	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/package_restore.config $(1)/etc/config/package_restore
	
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/tlt_opkg.conf $(1)/etc/tlt_opkg.conf

endef

$(eval $(call BuildPackage,package_restore))
