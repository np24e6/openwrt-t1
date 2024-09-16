include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=modbusgwd
PKG_VERSION:=2024-04-25

PKG_SOURCE_VERSION:=7.7.1
PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/modbusgwd
	SECTION:=net
	CATEGORY:=Network
	TITLE:=Modbus RTU to TCP gateway by Teltonika
	DEPENDS:=+kmod-usb-serial +libuci +libtlt_uci +libtlt_termios
endef

define Package/modbusgwd/description
	MODBUS RTU to TCP gateway designed to receive MODBUS TCP message, convert to MODBUS RTU and send it to MODBUS RTU device.
endef

define Package/modbusgwd/conffiles
/etc/config/rs_modbus
endef



define Package/modbusgwd/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/modbusgwd $(1)/usr/sbin/modbusgwd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/rs_modbus.conf $(1)/etc/config/rs_modbus
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/rs_modbus.init $(1)/etc/init.d/rs_modbus
endef

$(eval $(call BuildPackage,modbusgwd))
