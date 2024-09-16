include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=luci-bwc
PKG_VERSION:=2024-06-17
PKG_RELEASE:=1
PKG_LICENSE:=Apache-2.0 license



include $(INCLUDE_DIR)/package.mk

define Package/luci-bwc
	SECTION:=vuci
	CATEGORY:=VuCI
	SUBMENU:=api-core
	DEPENDS:=+WIFI_SUPPORT:iwinfo
	TITLE:=A program for realtime bandwidth data collection
endef

define Package/luci-bwc/description
	A program for realtime bandwidth data collection 
endef

define Package/luci-bwc/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/src/luci-bwc $(1)/usr/bin/luci-bwc
endef

$(eval $(call BuildPackage,luci-bwc))
