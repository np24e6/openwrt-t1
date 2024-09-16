#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mnfinfo

PKG_SOURCE_VERSION:=2.16.1

PKG_LICENSE:=Teltonika-closed

GPL_INCLUDE_HEADERS:=1

include $(INCLUDE_DIR)/package.mk

define Package/mnfinfo
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Device mnf-info command line interface
  DEPENDS:=+libmnfinfo
endef

define Package/libmnfinfo
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Device mnf-info API library
endef

define Package/rpcd-mod-mnfinfo
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=mnfinfo rpcd module
  DEPENDS:=+rpcd +libmnfinfo
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR)/usr/lib $(STAGING_DIR)/usr/include/libmnfinfo
	$(CP) $(PKG_BUILD_DIR)/*.so $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/src/include/*.h $(STAGING_DIR)/usr/include/libmnfinfo/
endef


define Package/mnfinfo/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mnf_info $(1)/sbin/mnf_info

	$(if $(CONFIG_x86_64),\
		$(INSTALL_DIR) $(1)/lib/firmware/mnfinfo; \
		$(CP) $(PKG_BUILD_DIR)/x86_data/* $(1)/lib/firmware/mnfinfo/; \
		printf 2 >$(1)/lib/firmware/mnfinfo/sim_count; \
	)
endef

define Package/libmnfinfo/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libmnfinfo.so $(1)/usr/lib/
endef

define Package/rpcd-mod-mnfinfo/install
	$(INSTALL_DIR) $(1)/usr/lib/rpcd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mnfinfo.so $(1)/usr/lib/rpcd/
endef

$(eval $(call BuildPackage,mnfinfo))
$(eval $(call BuildPackage,libmnfinfo))
$(eval $(call BuildPackage,rpcd-mod-mnfinfo))
