#
# Copyright (C) 2022 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=ledman

PKG_SOURCE_VERSION:=2.14.4

include $(INCLUDE_DIR)/package.mk

define Package/ledman-full
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=LED status manager
	PROVIDES:=ledman
	VARIANT:=full
	DEPENDS:= +libubus +libubox +libgsm +libuci +libtlt_uci +libmnfinfo +libnl-tiny +libgpiod +gpiod-tools
	DEPENDS+= +POE_SUPPORT:libpoeman
endef

# TODO SOMEHOW DEPS FROM 'FULL' IS NEEDED TO COMPILE 'TINY'
define Package/ledman
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=LED status manager
	PROVIDES:=ledman
	CONFLICTS:=ledman-full
	VARIANT:=tiny
endef

ifeq ($(BUILD_VARIANT),full)
	TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include/libnl-tiny
	TARGET_CFLAGS += -I$(PKG_BUILD_DIR)/src
endif

define Package/ledman-full/install
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/usr/bin $(1)/etc/hotplug.d/iface
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/ledman.init $(1)/etc/init.d/ledman
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/broadcast-event.hotplug $(1)/etc/hotplug.d/iface/01-broadcast-event
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ledman $(1)/usr/bin/ledman
endef


define Package/ledman/install
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/ledman_lite.sh $(1)/usr/bin/ledman
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/ledman_lite.init $(1)/etc/init.d/ledman
endef








$(eval $(call BuildPackage,ledman))
$(eval $(call BuildPackage,ledman-full))
