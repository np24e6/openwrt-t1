#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mctl

PKG_SOURCE_VERSION:=4.5
PKG_LICENSE:=Teltonika-closed

GPL_INCLUDE_HEADERS:=1

include $(INCLUDE_DIR)/package.mk

define Package/modem_trackd
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Simple server to track modem state. Created by Teltonika
	DEPENDS:=+libubus +libubox +libboardjson +libuci +libmctl
endef

define Package/mctl
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Simple executable application for modem restart. Created by Teltonika
	DEPENDS:=+libubus +libubox +libmctl +modem_trackd
endef

define Package/libmctl
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=A library for manage modem restart. Created by Teltonika
	DEPENDS:=+libubus +libboardjson +libblobmsg-json
endef

define Package/modem_trackd/description
	Simple server to track modem state. Created by Teltonika
endef

define Package/mctl/description
	Simple executable application for modem restart. Created by Teltonika
endef

define Package/libmctl/description
	A library for manage modem restart. Created by Teltonika
endef


define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include/libmctl
	$(CP) $(PKG_BUILD_DIR)/*.so $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/src/include/*.h $(1)/usr/include/
endef


define Package/libmctl/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libmctl.so $(1)/usr/lib/
endef

define Package/modem_trackd/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d/ $(1)/etc/hotplug.d/mctl/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/modem_trackd $(1)/usr/sbin/modem_trackd

	if [ $(CONFIG_MODEM_RESET_QUIRK) ]; then \
		$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/track_timeout.hotplug $(1)/etc/hotplug.d/mctl/01-mctl-event; \
	fi;

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/modem_trackd.init $(1)/etc/init.d/modem_trackd
endef

define Package/mctl/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mctl $(1)/sbin/mctl
endef

$(eval $(call BuildPackage,modem_trackd))
$(eval $(call BuildPackage,libmctl))
$(eval $(call BuildPackage,mctl))
