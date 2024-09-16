#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=rms-packages
PKG_VERSION:=2023-09-08
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=3.6
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

include $(INCLUDE_DIR)/package.mk

define Package/librms
	SUBMENU:=RMS
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Library for handling rms information
	DEPENDS:=+libubus +libubox +libblobmsg-json
endef

define Package/rms_mqtt
	SUBMENU:=RMS
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=MQTT application for RMS by Teltonika
	DEPENDS:=+libmosquitto +libmnfinfo +libopenssl +libuci +MOBILE_SUPPORT:libgsm +libblobmsg-json +libubox +libubus +libpthread +libtlt_uci
endef

define Package/rms_json
  	SUBMENU:=RMS
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Collects and prints out information about device.
	DEPENDS:=+libuci +MOBILE_SUPPORT:libgsm +GPS_SUPPORT:libgps +libmnfinfo +libblobmsg-json +libubus \
		+libubox +libjson-c +MOBILE_SUPPORT:libmdcollect +IO_SUPPORT:iomand +libboardjson
endef

define Package/rms_mqtt/conffiles
/etc/config/rms_mqtt
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/librms/librms.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/librms/librms.so $(1)/usr/lib/
endef


define Package/rms_mqtt/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rms_mqtt/rms_mqtt $(1)/usr/sbin/rms_mqtt

	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/rms_mqtt.init $(1)/etc/init.d/rms_mqtt

	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/rms_mqtt.conf $(1)/etc/config/rms_mqtt

	$(INSTALL_DIR) $(1)/etc/rms_mqtt/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/RutOS_teltonika.lt.ca $(1)/etc/rms_mqtt/RutOS@teltonika.lt.ca
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/RutOS_teltonika.lt.crt $(1)/etc/rms_mqtt/RutOS@teltonika.lt.crt
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/RutOS_teltonika.lt.key $(1)/etc/rms_mqtt/RutOS@teltonika.lt.key
endef

define Package/rms_json/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rms_json/rms_json $(1)/sbin/rms_json
endef

define Package/librms/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/librms/librms.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,rms_mqtt))
$(eval $(call BuildPackage,rms_json))
$(eval $(call BuildPackage,librms))
