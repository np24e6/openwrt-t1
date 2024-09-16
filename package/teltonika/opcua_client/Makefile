#
# Copyright (C) 2022 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=opcua_client
PKG_VERSION:=2023-12-08
PKG_SOURCE_VERSION:=7.6

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=net
	CATEGORY:=Network
	TITLE:=OPC UA client by Teltonika
	DEPENDS:=+libuci +libtlt_uci +libtlt_utils +libubox +libubus +libsqlite3 \
		+open62541 +vuci-app-opcua-ui +data-sender-mod-opcua
	PKG_TLT_NAME:=OPC UA Client
	PKG_ROUTER:=$(TLT_PLATFORM_NAME)
endef

define Package/$(PKG_NAME)/description
	OPC UA client by Teltonika
endef

define Package/$(PKG_NAME)/conffiles
/etc/config/$(PKG_NAME)
endef


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/opcua_client $(1)/usr/bin/opcua_client
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/opcua_client.init $(1)/etc/init.d/opcua_client
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/opcua_client.conf $(1)/etc/config/opcua_client
endef

define Build/Test
endef

define Package/$(PKG_NAME)/postinst
        #!/bin/sh

        [ -z "$${IPKG_INSTROOT}" ] || exit 0
        [ -e /lib/data_sender/libdata_sender.sh ] || exit 0

        . /lib/data_sender/libdata_sender.sh

        ds_find_plugin opcua && /etc/init.d/data_sender restart

        exit 0
endef

define Package/$(PKG_NAME)/prerm
	#!/bin/sh
	. /lib/functions.sh
	. /lib/config/uci.sh

	[ -f "/etc/config/data_sender" ] || return 0

	config_load data_sender

	list_length=0
	opcua_list_count=0

	check_list_opc_ua_in() {
		list_length=$$((list_length+1))
	}

	count_opc_ua_in() {
		config_get plugin "$$1" plugin ""
		[ "$$plugin" = "opcua" ] && opcua_list_count=$$((opcua_list_count+1))
	}

	del_opcua_in() {
		uci_remove data_sender "$$1"
	}

	check_opc_ua_col() {
		list_length=0
		opcua_list_count=0

		config_foreach count_opc_ua_in "input"
		config_list_foreach "$$1" input check_list_opc_ua_in

		if [ "$$list_length" -ge 2 ] && [ "$$list_length" -ne "$$opcua_list_count" ]; then
			uci_remove data_sender "$$2"
			uci_remove_list data_sender "$$1" input "$$2"
		elif [ "$$list_length" -ge 2 ] && [ "$$list_length" -eq "$$opcua_list_count" ]; then
			config_list_foreach "$$1" input del_opcua_in

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

	start_opc_ua_in() {
		config_get plugin "$$1" plugin ""
		[ "$$plugin" = "opcua" ] || return 0

		config_foreach check_opc_ua_col "collection" "$$1"
		uci_commit data_sender 2> /dev/null
		config_load data_sender
	}

	config_foreach start_opc_ua_in "input"
	/etc/init.d/data_sender reload 2> /dev/null || true

endef

$(eval $(call BuildPackage,$(PKG_NAME)))
