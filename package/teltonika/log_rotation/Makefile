#
# Copyright (C) 2022 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=log_rotation
PKG_VERSION:=2023-08-31
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=1.2

include $(INCLUDE_DIR)/package.mk

define Package/log_rotation
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Log rotation manager
endef

define Package/log_rotation/description
	Script that performs log (or any file) rotation
endef



define Package/log_rotation/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/log_rotation.sh $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/log_rotation.init $(1)/etc/init.d/log_rotation
endef

$(eval $(call BuildPackage,log_rotation))
