#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=iojuggler
PKG_VERSION:=2024-04-15
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=1.8.1
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/iojuggler
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=iojuggler
	DEPENDS:=+libubox +libblobmsg-json +libjson-c +libubus +libuci \
		+libtlt-logger +MOBILE_SUPPORT:libgsm +libcurl +iomand +libmnfinfo \
		+libtlt_smtp +libparam +libmosquitto +libboardjson
endef

define Package/iojuggler/description
	Responds to digital input changes by executing user-defined actions in accordance with user-defined conditions
endef

define Package/iojuggler/conffiles
/etc/config/iojuggler
endef


define Package/iojuggler/install
	$(INSTALL_DIR) $(1)/etc/config $(1)/usr/sbin $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/iojuggler $(1)/usr/sbin/
	$(if $(IOJ_DEBUG),, \
		$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/iojuggler.init $(1)/etc/init.d/iojuggler;\
		$(if $(CONFIG_POWER_CONTROL_SUPPORT),\
			$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/iojuggler_atrm50.conf $(1)/etc/config/iojuggler;,\
			$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/iojuggler_default.conf $(1)/etc/config/iojuggler;))
endef

$(eval $(call BuildPackage,iojuggler))
