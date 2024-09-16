#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=web_filter
PKG_VERSION:=2021-02-11
PKG_RELEASE:=1
PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/web_filter
	CATEGORY:=Base system
	TITLE:=Meta package for Web filter packages
	DEPENDS:=+host_block +privoxy
endef

define Package/web_filter/description
	Web Filter package provides you with the possibility to set up
	lists of wanted or unwanted websites (Blacklists or Whitelists).
endef



define Package/web_filter/install
	true
endef

$(eval $(call BuildPackage,web_filter))
