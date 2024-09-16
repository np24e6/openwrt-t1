#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=smslimit

PKG_SOURCE_VERSION:=4.0

GPL_INCLUDE_HEADERS:=1

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += $(FPIC)

define Package/rpcd-mod-smslimit
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=SMS Limit rpcd module
  DEPENDS:=+rpcd +lsqlite3 +libubus +libgsm
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/*.so $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/src/libsms_limit.h $(1)/usr/include/
endef


define Package/rpcd-mod-smslimit/install
	$(INSTALL_DIR) $(1)/usr/lib/rpcd  $(1)/etc/uci-defaults/etc
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/smslimit.so $(1)/usr/lib/rpcd/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libsms_limit.so $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/files/defaults/99_sms_limit $(1)/etc/uci-defaults/etc/99_sms_limit
endef

$(eval $(call BuildPackage,rpcd-mod-smslimit))
