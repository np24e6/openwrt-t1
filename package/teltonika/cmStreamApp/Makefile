#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=cmStreamApp
PKG_VERSION:=2024-06-17
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=2.2
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz

PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/cmStreamApp
	SECTION:=net
	CATEGORY:=Network
	TITLE:=Cumulocity - Cloud of Things stream application.
	DEPENDS:=+libsera +libuci +MOBILE_SUPPORT:libgsm +libmnfinfo
endef

define Package/cmStreamApp/description
	Cumulocity - Cloud of Things IoT application meant
	for interaction with cloud-based Cumulocity IoT platform and
	Telekom Cloud of Things.
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

	if [ ! $(CONFIG_HNAT) ]; then \
		$(SED) "s#Teltonika-Networks_RUTX#Teltonika-Networks_$(call device_shortname)#g" $(1)/usr/lib/lua/cm/srtemplate.txt; \
	fi;
endef

$(eval $(call BuildPackage,cmStreamApp))
