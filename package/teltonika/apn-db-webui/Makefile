#
# Copyright (C) 2023 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=apn-db-webui
PKG_VERSION:=2023-09-13

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	CATEGORY:=Utilities
	TITLE:=APN Database page for WebUI
	DEPENDS:=+vuci-app-apn-database-ui
	PKG_TLT_NAME:=APN Database WebUI
	PKG_ROUTER:=$(TLT_PLATFORM_NAME)
endef

define Package/$(PKG_NAME)/description
	APN Database management page for WebUI
endef



define Package/$(PKG_NAME)/install
	true
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
