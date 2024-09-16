#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=iomand

PKG_SOURCE_VERSION:=2.18.6

PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/iomand
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Daemon providing I/O related functionality.
	DEPENDS:=+libuci +libubus +libubox +libcgi +libmnfinfo +libtlt-logger +liblog +libgpiod +gpiod-tools
endef

define Package/iomand/conffiles
/etc/config/ioman
endef


define Package/iomand/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/etc/config $(1)/etc/init.d $(1)/etc/hotplug.d/usb \
		$(1)/www/cgi-bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/iomand $(1)/usr/bin/iomand
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/iomand.init $(1)/etc/init.d/ioman

	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/libioman.so $(1)/usr/lib/

	$(INSTALL_DIR) $(1)/etc/uci-defaults/etc

	$(if $(CONFIG_IO_SUPPORT),\
		$(INSTALL_BIN) $(PKG_BUILD_DIR)/scheduler $(1)/usr/bin/scheduler; \
		$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/ioman_scheduler.init $(1)/etc/init.d/ioman_scheduler; \
		$(CP) $(PKG_BUILD_DIR)/files/99_sms_utils-iomand $(1)/etc/uci-defaults/etc; \
	)
endef

ifneq ($(CONFIG_POWER_CONTROL_SUPPORT)$(CONFIG_IO_SUPPORT),)
define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/libioman.so $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/include/libioman.h $(1)/usr/include/
endef
endif

$(eval $(call BuildPackage,iomand))
