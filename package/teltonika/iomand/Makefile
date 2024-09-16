#
# Copyright (C) 2022 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=iomand

PKG_SOURCE_VERSION:=2.14.1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/migrations.mk

define Package/iomand
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Daemon providing I/O related functionality.
	DEPENDS:=+libuci +libubus +libubox +libcgi +libmnfinfo +libtlt-logger +liblog
endef

define Package/iomand/conffiles
/etc/config/ioman
endef


define Package/iomand/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/etc/config $(1)/etc/init.d $(1)/etc/hotplug.d/usb \
		$(1)/www/cgi-bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/iomand $(1)/usr/bin/iomand
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scheduler $(1)/usr/bin/scheduler
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/iomand.init $(1)/etc/init.d/ioman
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/ioman_scheduler.init $(1)/etc/init.d/ioman_scheduler

	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/libioman.so $(1)/usr/lib/

	$(INSTALL_DIR) $(1)/etc/uci-defaults/etc
	$(CP) $(PKG_BUILD_DIR)/files/99_sms_utils-iomand $(1)/etc/uci-defaults/etc

	if [ $(TLT_PLATFORM_RUTX) ] || [ $(TLT_PLATFORM_X86_64) ] || [ $(TLT_PLATFORM_RUTM) ]; then \
		$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/ioman_RUTX.config $(1)/etc/config/ioman; \
	elif [ $(TLT_PLATFORM_TRB1) ] || [ $(TLT_PLATFORM_TRB5) ]; then \
		$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/ioman_TRB14x.config $(1)/etc/config/ioman; \
	elif [ $(TLT_PLATFORM_TRB2) ] || [ $(TLT_PLATFORM_TRB2M) ]; then \
		$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/ioman_TRB2.config $(1)/etc/config/ioman; \
	elif [ $(TLT_PLATFORM_RUT30X) ] || [ $(TLT_PLATFORM_RUT36X) ] || [ $(TLT_PLATFORM_RUT301) ] || [ $(TLT_PLATFORM_RUT361) ]; then \
		$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/ioman_RUT3.config $(1)/etc/config/ioman; \
	elif [ $(TLT_PLATFORM_RUT9) ] || [ $(TLT_PLATFORM_RUT9M) ]; then \
		$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/ioman_RUT9.config $(1)/etc/config/ioman; \
	elif [ $(TLT_PLATFORM_RUT2) ] || [ $(TLT_PLATFORM_RUT2M) ]; then \
		$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/ioman_RUT2.config $(1)/etc/config/ioman; \
	fi;

endef

ifeq ($(CONFIG_IO_SUPPORT),y)
define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/libioman.so $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/include/libioman.h $(1)/usr/include/
endef
endif

$(eval $(call BuildPackage,iomand))
