include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=sodog-tlt
PKG_VERSION:=2023-02-13
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=7e19bc4812bf007a275129901234ef120ae419f0

include $(INCLUDE_DIR)/package.mk

MAKE_VARS += \
	     CFLAGS+=" -DVERSION='\"$(PKG_VERSION)\"'"

define Package/$(PKG_NAME)
	SECTION:=net
	CATEGORY:=Network
	TITLE:=SOdog establishes two bidirectional byte streams and transfers data between them by Teltonika.
	DEPENDS:=+USE_GLIBC:glib2 +kmod-usb-serial
endef

define Package/$(PKG_NAME)/description
	SOdog establishes two bidirectional byte streams and transfers data between them by Teltonika.
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
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
