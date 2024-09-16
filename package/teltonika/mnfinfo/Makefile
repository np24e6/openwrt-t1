#
# Copyright (C) 2022 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=mnfinfo

PKG_SOURCE_VERSION:=2.13

GPL_INCLUDE_HEADERS:=1

include $(INCLUDE_DIR)/package.mk

define Package/mnfinfo
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Device mnf-info command line interface
  DEPENDS:=+libpthread +libmnfinfo
endef

define Package/libmnfinfo
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Device mnf-info API library
  DEPENDS:=+libubus +libubox +libboardjson
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

	if [ $(TLT_PLATFORM_X86_64) ]; then \
		$(INSTALL_DIR) $(1)/lib/firmware; \
		$(INSTALL_DIR) $(1)/lib/firmware/mnfinfo; \
		$(CP) $(PKG_BUILD_DIR)/x86_data/mac $(1)/lib/firmware/mnfinfo/mac; \
		$(CP) $(PKG_BUILD_DIR)/x86_data/name $(1)/lib/firmware/mnfinfo/name; \
		$(CP) $(PKG_BUILD_DIR)/x86_data/wps $(1)/lib/firmware/mnfinfo/wps; \
		$(CP) $(PKG_BUILD_DIR)/x86_data/batch $(1)/lib/firmware/mnfinfo/batch; \
		$(CP) $(PKG_BUILD_DIR)/x86_data/hwver $(1)/lib/firmware/mnfinfo/hwver; \
		$(CP) $(PKG_BUILD_DIR)/x86_data/simpin1 $(1)/lib/firmware/mnfinfo/simpin1; \
		$(CP) $(PKG_BUILD_DIR)/x86_data/simpin2 $(1)/lib/firmware/mnfinfo/simpin2; \
		$(CP) $(PKG_BUILD_DIR)/x86_data/wpass $(1)/lib/firmware/mnfinfo/wpass; \
		$(CP) $(PKG_BUILD_DIR)/x86_data/pass $(1)/lib/firmware/mnfinfo/pass; \
	fi
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
