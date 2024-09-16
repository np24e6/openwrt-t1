include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=azure_iothub
PKG_VERSION:=2023-08-24

PKG_SOURCE_VERSION:=2.9
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

include $(INCLUDE_DIR)/package.mk

define Package/azure_iothub
	SECTION:=base
	CATEGORY:=Base system
	DEPENDS:=+libc +libuci +libubus +libblobmsg-json +libtlt-logger \
		 +libubox +libazure-iot-sdk-c +vuci-app-azure-iothub-ui
	TITLE:=Azure IoT Hub client
	PKG_TLT_NAME:=Azure IoT Hub
	PKG_ROUTER:=$(TLT_PLATFORM_NAME)
endef

define Package/azure_iothub/conffiles
/etc/config/azure_iothub
endef


define Package/azure_iothub/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/azure_iothub $(1)/sbin/azure_iothub

	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/azure_iothub.init $(1)/etc/init.d/azure_iothub

	$(INSTALL_DIR) $(1)/etc/config/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/azure_iothub.conf $(1)/etc/config/azure_iothub

	$(INSTALL_DIR) $(1)/etc/uci-defaults/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/98_azure_to_data_sender $(1)/etc/uci-defaults/
endef

$(eval $(call BuildPackage,azure_iothub))
