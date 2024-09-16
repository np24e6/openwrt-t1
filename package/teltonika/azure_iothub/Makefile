include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=azure_iothub
PKG_VERSION:=2024-03-28

PKG_SOURCE_VERSION:=2.10
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/azure_iothub
	SECTION:=base
	CATEGORY:=Base system
	DEPENDS:=+libc +libuci +libubus +libblobmsg-json +libtlt-logger \
		 +libubox +libazure-iot-sdk-c
	TITLE:=Azure IoT Hub client
endef

define Package/azure_iothub/description
	Azure IoT Hub is a utility meant to interact with
	Microsoft Azure IoT Hub cloud platform.
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

define Package/azure_iothub/prerm
	#!/bin/sh
	. /lib/functions.sh

	[ -f "/etc/config/data_sender" ] || return 0

	config_load data_sender

	check_azure_in() {
		uci_remove data_sender "$$1"
	}

	check_azure_col() {
		config_get out "$$1" output ""
		[ "$$out" = "$$2" ] || return 0

		config_list_foreach "$$1" input check_azure_in

		uci_remove data_sender "$$1"
	}

	check_azure_out() {
		config_get plugin "$$1" plugin ""
		[ "$$plugin" = "ubus" ] || return 0

		config_get obj "$$1" ubus_object ""
		[ "$${obj%.*}" = "azure" ] || return 0

		config_foreach check_azure_col "collection" "$$1"
		uci_remove data_sender "$$1"
	}

	config_foreach check_azure_out "output"
	uci_commit data_sender 2> /dev/null
	/etc/init.d/data_sender reload 2> /dev/null || true

endef

$(eval $(call BuildPackage,azure_iothub))
