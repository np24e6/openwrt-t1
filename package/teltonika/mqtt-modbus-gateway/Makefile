include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=mqtt-modbus-gateway
PKG_VERSION:=2024-05-06
PKG_SOURCE_VERSION:=7.7.2
PKG_LICENSE:=Teltonika-closed

# TARGET_CFLAGS += -ggdb3 -O0

include $(INCLUDE_DIR)/package.mk

define Package/mqtt-modbus-gateway
		SECTION:=net
		CATEGORY:=Network
		TITLE:=MQTT Modbus Gateway
		DEPENDS:=+libmosquitto +libmodbus +libmnfinfo +libtlt_uci +libtlt_termios +libuci
endef

define Package/mqtt-modbus-gateway/description
		MQTT-Modbus Gateway is a service meant to read/write MODBUS TCP/RTU servers via MQTT messages.
endef

define Package/mqtt-modbus-gateway/conffiles
/etc/config/modbusgateway
endef


define Build/Test
	test_modbusgateway.c
	test_parse_mqtt_request.c
	test_validate_request_parameters.c
	test_config_utils.c
endef

define Package/mqtt-modbus-gateway/install
		$(INSTALL_DIR) $(1)/etc/config
		$(INSTALL_DIR) $(1)/etc/init.d
		$(INSTALL_DIR) $(1)/usr/sbin
		$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/modbusgateway.conf $(1)/etc/config/modbusgateway
		$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/modbusgateway.init $(1)/etc/init.d/modbusgateway
		$(INSTALL_BIN) $(PKG_BUILD_DIR)/modbusgateway $(1)/usr/sbin/modbusgateway
endef

$(eval $(call BuildPackage,mqtt-modbus-gateway))
