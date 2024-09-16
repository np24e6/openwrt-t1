#
# Copyright (C) 2022 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=libparam

PKG_SOURCE_VERSION:=e677d30bd4cdc760eda3c37ee6ef0c148cacfa6a

include $(INCLUDE_DIR)/package.mk

define Package/libparam
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=A library for decoding and replacing params in string.
	DEPENDS:= +libubox +libtlt_uci +libubus +MOBILE_SUPPORT:rpcd-mod-simd \
		+IO_SUPPORT:iomand +MOBILE_SUPPORT:libgsm +librms +librut_fota +libmnfinfo
endef

define Package/libparam/description
	A library for decoding and replacing params in string.
endef

TARGET_CFLAGS += -D_GNU_SOURCE

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/*.so $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/include/*.h $(STAGING_DIR)/usr/include/
endef


define Package/libparam/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libparam.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libparam))
