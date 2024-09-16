include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=modbusgwd

PKG_SOURCE_VERSION:=7.5

include $(INCLUDE_DIR)/package.mk

define Package/modbusgwd
	SECTION:=net
	CATEGORY:=Network
	TITLE:=Modbus RTU to TCP gateway by Teltonika
	DEPENDS:=+kmod-usb-serial
endef

define Package/modbusgwd/description
	Modbus RTU to TCP gateway by Teltonika
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
