#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=cmStreamApp
PKG_VERSION:=2023-09-01
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=1.9

PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

GPL_INCLUDE_SRC:=1

include $(INCLUDE_DIR)/package.mk

define Package/cmStreamApp
	SECTION:=net
	CATEGORY:=Network
	TITLE:=Cumulocity - Cloud of Things stream application.
	DEPENDS:=+libsera +libuci +libgsm +libmnfinfo +vuci-app-cumulocity-ui
	PKG_TLT_NAME:=Cumulocity - Cloud of Things
	PKG_ROUTER:=$(TLT_PLATFORM_NAME)
endef

define Package/cmStreamApp/description
	A simple Cumulocity - Cloud of Things IoT Stream App package.
endef

define Package/cmStreamApp/conffiles
/etc/config/iot
endef

define Package/cmStreamApp/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/cmStreamApp $(1)/usr/bin/cmStreamApp
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/iot.init $(1)/etc/init.d/iot
	$(INSTALL_DIR) $(1)/usr/lib/lua/cm
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/srtemplate $(1)/usr/lib/lua/cm/srtemplate.txt
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/stream.lua $(1)/usr/lib/lua/cm/stream.lua
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/iot $(1)/etc/config/iot
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/iot.hotplug $(1)/etc/hotplug.d/iface/97-iot

	if [ ! $(TLT_PLATFORM_RUTX) ]; then \
		$(SED) "s#Teltonika-Networks_RUTX#Teltonika-Networks_$(TLT_PLATFORM_NAME)#g" $(1)/usr/lib/lua/cm/srtemplate.txt; \
    fi;
endef

$(eval $(call BuildPackage,cmStreamApp))
