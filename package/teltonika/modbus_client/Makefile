#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=modbus_client
PKG_VERSION:=2024-07-23
PKG_SOURCE_VERSION:=7.9
PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=net
	CATEGORY:=Network
	TITLE:=MODBUS TCP/Serial client daemon by Teltonika
	DEPENDS:=+libuci +libtlt_uci +libtlt_termios +libsqlite3 +libmodbus +IO_SUPPORT:iomand \
		 +MOBILE_SUPPORT:libgsm +libparam +libmosquitto
endef

define Package/$(PKG_NAME)/description
	MODBUS TCP/Serial client designed for reading/writing data from MODBUS servers and storing read data in local database.
endef

define Package/$(PKG_NAME)/conffiles
/etc/config/$(PKG_NAME)
endef


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/lib/rpcd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/modbus_client.conf $(1)/etc/config/modbus_client
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/modbus_client.init $(1)/etc/init.d/modbus_client
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/modbus_client $(1)/usr/sbin/modbus_client
	$(CP) $(PKG_BUILD_DIR)/libalarms.so $(1)/usr/lib/libalarms.so
	$(CP) $(PKG_BUILD_DIR)/modbus_client.so $(1)/usr/lib/rpcd/modbus_client.so
endef

define Package/$(PKG_NAME)/postinst
        #!/bin/sh

	/etc/init.d/rpcd reload

        [ -z "$${IPKG_INSTROOT}" ] || exit 0
        [ -e /lib/data_sender/libdata_sender.sh ] || exit 0

        . /lib/data_sender/libdata_sender.sh

        ds_find_plugin modbus && /etc/init.d/data_sender restart

        exit 0
endef

define Package/$(PKG_NAME)/prerm
	#!/bin/sh
	. /lib/functions.sh
	. /lib/config/uci.sh

	[ -f "/etc/config/data_sender" ] || return 0

	config_load data_sender

	list_length=0
	modbus_list_count=0

	check_list_modbus_in() {
		list_length=$$((list_length+1))
	}

	count_modbus_in() {
		config_get plugin "$$1" plugin ""
		[ "$$plugin" = "modbus" ] && modbus_list_count=$$((modbus_list_count+1))
	}

	del_modbus_in() {
		uci_remove data_sender "$$1"
	}

	check_modbus_col() {
		list_length=0
		modbus_list_count=0

		config_foreach count_modbus_in "input"
		config_list_foreach "$$1" input check_list_modbus_in
		echo "$$list_length"
		echo "$$modbus_list_count"
		if [ "$$list_length" -ge 2 ] && [ "$$list_length" -ne "$$modbus_list_count" ]; then
			uci_remove data_sender "$$2"
			uci_remove_list data_sender "$$1" input "$$2"
		elif [ "$$list_length" -ge 2 ] && [ "$$list_length" -eq "$$modbus_list_count" ]; then
			config_list_foreach "$$1" input del_modbus_in

			config_get out "$$1" output ""
			uci_remove data_sender "$$out"

			uci_remove data_sender "$$1"
		else
			config_get in "$$1" input ""
			uci_remove data_sender "$$in"

			config_get out "$$1" output ""
			uci_remove data_sender "$$out"

			uci_remove data_sender "$$1"

		fi

	}

	start_modbus_in() {
		config_get plugin "$$1" plugin ""
		[ "$$plugin" = "modbus" ] || return 0

		config_foreach check_modbus_col "collection" "$$1"
		uci_commit data_sender 2> /dev/null
		config_load data_sender
	}

	config_foreach start_modbus_in "input"
	/etc/init.d/data_sender reload 2> /dev/null || true

endef

$(eval $(call BuildPackage,$(PKG_NAME)))
