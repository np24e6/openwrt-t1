include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=dlms_client
PKG_SOURCE_VERSION:=7.5
PKG_BUILD_DEPENDS:=libdlms

GPL_INCLUDE_SRC:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=net
	CATEGORY:=Network
	TITLE:=DLMS Client for COSEM device monitoring
	DEPENDS:=+libsqlite3 +libubus +libubox +libuci +libtlt_uci +libtlt-logger +libpthread \
		+vuci-app-dlms-ui +data-sender-mod-dlms
	PKG_TLT_NAME:=DLMS Client
        PKG_ROUTER:=$(TLT_PLATFORM_NAME)
endef

define Package/$(PKG_NAME)/description
	DLMS Client for COSEM device monitoring by Teltonika
endef

define Package/$(PKG_NAME)/conffiles
/etc/config/$(PKG_NAME)
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/config $(1)/etc/init.d $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/dlms_client.conf $(1)/etc/config/dlms_client
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/dlms_client.init $(1)/etc/init.d/dlms_client
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dlms_client $(1)/usr/sbin/dlms_client
endef

define Build/Test
endef

define Package/$(PKG_NAME)/postinst
	#!/bin/sh

	[ -z "$${IPKG_INSTROOT}" ] || exit 0
	[ -e /lib/data_sender/libdata_sender.sh ] || exit 0

	. /lib/data_sender/libdata_sender.sh

	ds_find_plugin dlms && /etc/init.d/data_sender restart
	
	exit 0
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
