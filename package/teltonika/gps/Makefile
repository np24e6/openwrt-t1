#
# Copyright (C) 2021-2023 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=gps

PKG_SOURCE_VERSION:=7.5

GPL_INCLUDE_HEADERS:=1

include $(INCLUDE_DIR)/package.mk

define Package/gpsd
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Deamon providing gps related functionality.
	DEPENDS:=+lsqlite3 +libpthread +libuci +liblog +libubus +liburc +libgsm +zlib +libcurl +IO_SUPPORT:iomand +libmnfinfo
endef

define Package/gpsctl
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Console line interface for gpsd daemon.
	DEPENDS:=+libgps +gpsd
endef

define Package/ntp_gps
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Daemon meant for syncing system time with GPS data.
	DEPENDS:=+libgps +gpsd +libuci
endef

define Package/libgps
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=Library for communication with the gpsd daemon.
	DEPENDS:=+libubus
endef

define Package/avl
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Daemon for sending data to AVL server.
	DEPENDS:=+libpthread +libuci +liblog +libubus +libgsm +zlib +libgps +iomand
endef

define Package/avl/conffiles
/etc/config/avl
endef

ifeq ($(CONFIG_GPS_SUPPORT),y)
define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include/libgps
	$(CP) $(PKG_BUILD_DIR)/libgps/libgps.so $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/libgps/include/libgps.h $(1)/usr/include/libgps/
endef
endif


define Package/libgps/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libgps/libgps.so $(1)/usr/lib/
endef

define Package/gpsd/conffiles
/etc/config/gps
endef

define Package/gpsd/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/config $(1)/etc/init.d $(1)/etc/hotplug.d/usb
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/gpsd/gpsd $(1)/usr/sbin/gpsd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/gpsd.init $(1)/etc/init.d/gpsd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/06_gpsd.sh $(1)/etc/hotplug.d/usb/06_gpsd

	if [ $(TLT_PLATFORM_RUTX) ] || [ $(TLT_PLATFORM_RUTM) ]; then \
		$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/gps_RUTX.conf $(1)/etc/config/gps; \
	elif [ $(TLT_PLATFORM_TRB2) ] || [ $(TLT_PLATFORM_TRB2M) ]; then \
		$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/gps_TRB2.conf $(1)/etc/config/gps; \
	elif [ $(TLT_PLATFORM_RUT30X) ] || [ $(TLT_PLATFORM_RUT36X) ]; then \
		$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/gps_RUT3.conf $(1)/etc/config/gps; \
	elif [ $(TLT_PLATFORM_RUT9) ] || [ $(TLT_PLATFORM_RUT9M) ]; then \
		$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/gps_RUT9.conf $(1)/etc/config/gps; \
	fi;
endef

define Package/gpsctl/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/gpsctl/gpsctl $(1)/usr/sbin/gpsctl
endef

define Package/ntp_gps/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/ntp_gps.init $(1)/etc/init.d/ntp_gps
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ntp_gps/ntp_gps $(1)/usr/sbin/
endef

define Package/avl/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/avl.init $(1)/etc/init.d/avl
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/avl/avl $(1)/usr/sbin/avl

	if [ $(TLT_PLATFORM_RUTX) ] || [ $(TLT_PLATFORM_RUTM) ]; then \
		$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/avl_RUTX.conf $(1)/etc/config/avl; \
	elif [ $(TLT_PLATFORM_TRB2) ] || [ $(TLT_PLATFORM_TRB2M) ]; then \
		$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/avl_TRB2.conf $(1)/etc/config/avl; \
	elif [ $(TLT_PLATFORM_RUT30X) ] || [ $(TLT_PLATFORM_RUT36X) ]; then \
		$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/avl_RUT3.conf $(1)/etc/config/avl; \
	elif [ $(TLT_PLATFORM_RUT9) ] || [ $(TLT_PLATFORM_RUT9M) ]; then \
		$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/avl_RUT9.conf $(1)/etc/config/avl; \
	fi;
endef

$(eval $(call BuildPackage,libgps))
$(eval $(call BuildPackage,gpsd))
$(eval $(call BuildPackage,gpsctl))
$(eval $(call BuildPackage,ntp_gps))
$(eval $(call BuildPackage,avl))
