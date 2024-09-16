include $(TOPDIR)/rules.mk

PKG_NAME:=libdlms
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/Gurux/GuruxDLMS.c.git
PKG_SOURCE_VERSION:=e5cc16e14208fe35301ea63e950e26f225480331
PKG_MIRROR_HASH:=3c93205c7422690fdb043077c16f8fa48ea4088c6fbace66fdfb56adada32b0d
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

define Build/Prepare
	$(call Build/Prepare/Default)
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/development/obj $(PKG_BUILD_DIR)/development/lib
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/development/include/*.h $(1)/usr/include/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/development/lib/libgurux_dlms_c.a $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libdlms))
