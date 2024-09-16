include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=sodog-tlt
PKG_SOURCE_VERSION:=7.5.1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=net
	CATEGORY:=Network
	TITLE:=SOdog establishes multiple bidirectional byte streams and transfers data between them by Teltonika.
	DEPENDS:=+kmod-usb-serial +libubox +libuci +libtlt_uci +libubus +libpthread \
		+vuci-app-modem-control-ui +vuci-app-overip-ui +libgsm
endef

define Package/$(PKG_NAME)/description
	SOdog establishes multiple bidirectional byte streams and transfers data between them by Teltonika.
endef

ifeq ($(CONFIG_USE_GLIBC),y)
TARGET_CFLAGS += -Dstrlcpy=g_strlcpy
TARGET_LDFLAGS += -lglib-2.0
endif


define Package/$(PKG_NAME)/conffiles
/etc/config/rs_overip
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sodog $(1)/usr/sbin/sodog
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/rs_overip.init $(1)/etc/init.d/rs_overip
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/rs_overip.config $(1)/etc/config/rs_overip
	$(INSTALL_DIR) $(1)/etc/uci-defaults/etc
	$(CP) $(PKG_BUILD_DIR)/files/defaults/23_migrate_overip $(1)/etc/uci-defaults/etc/23_migrate_overip
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
