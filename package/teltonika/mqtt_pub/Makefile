include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=mqtt_pub
PKG_VERSION:=2023-09-01
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=2.1

include $(INCLUDE_DIR)/package.mk

define Package/mqtt_pub
	SECTION:=net
	CATEGORY:=Network
	TITLE:=MQTT client for device monitoring
	DEPENDS:=+libuci +libtlt_uci +libmosquitto-ssl +libubus +libjson-c \
		+libubox +libblobmsg-json +mosquitto-ssl +vuci-app-mqtt-ui \
		+IO_SUPPORT:iomand +USE_GLIBC:glib2 +libgsm +libmnfinfo

	ifeq (m, $(CONFIG_PACKAGE_mqtt_pub))
		PKG_TLT_NAME:=MQTT
		PKG_ROUTER:=$(TLT_PLATFORM_NAME)
	endif
endef

ifeq ($(CONFIG_USE_GLIBC),y)
TARGET_CPPFLAGS +=-Dstrlcpy=g_strlcpy
TARGET_LDFLAGS +=-lglib-2.0
endif

define Package/mqtt_pub/description
	MQTT client for device monitoring by Teltonika
endef

define Package/mqtt_pub/conffiles
/etc/config/mqtt_pub
endef


define Package/mqtt_pub/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/mqtt_pub.conf $(1)/etc/config/mqtt_pub
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/mqtt_pub.init $(1)/etc/init.d/mqtt_pub
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mqtt_pub $(1)/usr/sbin/mqtt_pub
endef

$(eval $(call BuildPackage,mqtt_pub))
