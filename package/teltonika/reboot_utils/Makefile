#
# Copyright (C) 2022 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=reboot_utils
PKG_VERSION:=2024-04-05
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=3.3.9
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz

PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/periodic_reboot
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Periodic reboot by Teltonika
endef

define Package/periodic_reboot/conffiles
/etc/config/periodic_reboot
endef

define Package/ping_reboot
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=ping_reboot
endef

define Package/ping_reboot/conffiles
/etc/config/ping_reboot
endef



define Package/periodic_reboot/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/config $(1)/etc/init.d $(1)/etc/reboot_utils
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/periodic_reboot.conf $(1)/etc/config/periodic_reboot
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/periodic_reboot.init $(1)/etc/init.d/periodic_reboot
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/reboot_modem.sh $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/periodic_reboot_init.sh $(1)/etc/reboot_utils
endef

define Package/ping_reboot/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d/ $(1)/etc/config/ $(1)/etc/reboot_utils
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/ping_reboot.init $(1)/etc/init.d/ping_reboot
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/ping_reboot.conf $(1)/etc/config/ping_reboot
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/ping_reboot.sh $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/ping_reboot_init.sh $(1)/etc/reboot_utils
endef

$(eval $(call BuildPackage,periodic_reboot))
$(eval $(call BuildPackage,ping_reboot))
