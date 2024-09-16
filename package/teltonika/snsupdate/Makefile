#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=snsupdate

PKG_SOURCE_VERSION:=1.0
PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/snsupdate
	SECTION:=net
	CATEGORY:=Network
	SUBMENU:=IP Addresses and Names
	DEPENDS:=+libopenssl
	TITLE:=small dynamic DNS client
endef

define Package/snsupdate/description
	Small nsupdate clone.
endef

define Package/snsupdate/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/snsupdate $(1)/usr/bin/snsupdate
endef



$(eval $(call BuildPackage,snsupdate))
