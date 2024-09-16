#
# Copyright (C) 2022 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=mctl

PKG_SOURCE_VERSION:=2fdb622367699baa8bf0f31c83af36d8f8925ec3

GPL_INCLUDE_HEADERS:=1

include $(INCLUDE_DIR)/package.mk

define Package/modem_trackd
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Simple server to track modem state. Created by Teltonika
	DEPENDS:=+libubus +libubox +libmctl +libgsm +libboardjson +libuci
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
	DEPENDS:=+BASEBAND_SUPPORT:libubus +libboardjson +libblobmsg-json
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
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/modem_trackd $(1)/usr/sbin/modem_trackd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/modem_trackd.init $(1)/etc/init.d/modem_trackd
endef

define Package/mctl/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mctl $(1)/sbin/mctl
endef

$(eval $(call BuildPackage,modem_trackd))
$(eval $(call BuildPackage,libmctl))
$(eval $(call BuildPackage,mctl))
