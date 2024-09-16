include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=azure_iothub
PKG_VERSION:=2024-06-17

PKG_SOURCE_VERSION:=3.0
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/azure_iothub
	SECTION:=base
	CATEGORY:=Base system
	DEPENDS:=+libc +libuci +libubus +libblobmsg-json +libtlt-logger \
		 +libubox +libazure-iot-sdk-c +libmnfinfo +liblua +libtlt_uci
	TITLE:=Azure IoT Hub client
endef

define Package/azure_iothub/description
	Azure IoT Hub is a utility meant to interact with
	Microsoft Azure IoT Hub cloud platform.
endef

define Package/azure_iothub/conffiles
/etc/config/azure_iothub
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/libaz_plugin_handler/*.h $(STAGING_DIR)/usr/include/

	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/libaz_plugin_handler/*.so $(STAGING_DIR)/usr/lib/
endef


define Package/azure_iothub/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/lib/az_plugins

	$(CP) $(PKG_BUILD_DIR)/libaz_plugin_handler/*.so $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/plugins/*.so $(1)/usr/lib/az_plugins/

	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/azure_iothub $(1)/sbin/azure_iothub

	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/azure_iothub.init $(1)/etc/init.d/azure_iothub

	$(INSTALL_DIR) $(1)/etc/azure_iothub/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/azure_api_list.json $(1)/etc/azure_iothub/azure_api_list.json

	$(INSTALL_DIR) $(1)/etc/config/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/azure_iothub.conf $(1)/etc/config/azure_iothub

	$(INSTALL_DIR) $(1)/etc/uci-defaults/
	$(INSTALL_DIR) $(1)/etc/uci-defaults/7.5
	$(INSTALL_DIR) $(1)/etc/uci-defaults/7.7
	$(INSTALL_DIR) $(1)/etc/uci-defaults/7.8
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/uci-default/7.5/98_azure_to_data_sender $(1)/etc/uci-defaults/7.5/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/uci-default/7.7/98_azure_data_sender_collection_fix $(1)/etc/uci-defaults/7.7/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/uci-default/7.8/99_azure_pnp $(1)/etc/uci-defaults/7.8/
endef

define Package/azure_iothub/prerm
	#!/bin/sh
	. /lib/functions.sh

	[ -f "/etc/config/data_sender" ] || return 0

	config_load data_sender

	check_azure_col() {
		config_get out "$$1" output ""
		[ "$$out" = "$$2" ] || return 0

		uci_set data_sender "$$1" enabled 0
	}

	check_azure_out() {
		config_get plugin "$$1" plugin ""
		[ "$$plugin" = "ubus" ] || return 0

		config_get obj "$$1" ubus_object ""
		[ "$${obj%.*}" = "azure" ] || return 0

		config_foreach check_azure_col "collection" "$$1"
		uci_remove data_sender $$1 plugin
		uci_remove data_sender $$1 ubus_object
		uci_remove data_sender $$1 ubus_method
	}

	config_foreach check_azure_out "output"
	uci_commit data_sender 2> /dev/null
	/etc/init.d/data_sender reload 2> /dev/null || true

endef

$(eval $(call BuildPackage,azure_iothub))
