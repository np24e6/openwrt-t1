#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=apn_db
PKG_VERSION:=2023-01-06
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=2cc88da7f626db09d6bbfcb0295c19e33601625e
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

include $(INCLUDE_DIR)/package.mk

define Package/apn_db
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=One APN database for all projects
endef

define Package/apn_db/description
	One APN database for all projects
endef



define Package/apn_db/install
	$(INSTALL_DIR) $(1)/usr/share/mobifd/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/apn.xml.db $(1)/usr/share/mobifd/apn.db
endef

$(eval $(call BuildPackage,apn_db))
