#
# Copyright (C) 2023 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=apn_db

PKG_SOURCE_VERSION:=1.0.1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=One APN database for all projects
endef

define Package/$(PKG_NAME)/description
	One APN database for all projects
endef



define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/share/mobifd $(1)/usr/bin $(1)/etc/uci-defaults/etc
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/apn.db $(1)/usr/share/mobifd/apn.db
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/backup_apn_db $(1)/usr/bin/backup_apn_db
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/99_apn_db $(1)/etc/uci-defaults/etc/99_apn_db
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
