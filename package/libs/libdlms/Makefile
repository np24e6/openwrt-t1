include $(TOPDIR)/rules.mk

PKG_NAME:=libdlms
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/Gurux/GuruxDLMS.c.git
PKG_SOURCE_VERSION:=fa855d2e82db1a7d27c419d09ed7a11fe86b86f0
PKG_MIRROR_HASH:=bdd2e9444419a300d18f77bce58bac6cc011b7330638a8e165ec440775912ea9
PKG_LICENSE:=GPL-2.0

MAKE_PATH:=development

include $(INCLUDE_DIR)/package.mk

define Package/libdlms
	SECTION:=libs
	CATEGORY:=Libraries
	URL:=https://github.com/Gurux/GuruxDLMS.c
	TITLE:=libdlms
endef

define Package/libdlms/description
	Gurux DLMS library
endef

define Package/libdlms/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/development/lib/libgurux_dlms_c.so* $(1)/usr/lib
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/development/obj $(PKG_BUILD_DIR)/development/lib
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/development/include/*.h $(1)/usr/include/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/development/lib/libgurux_dlms_c.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libdlms))
