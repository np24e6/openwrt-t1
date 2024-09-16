include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=dlms_client
PKG_VERSION:=2024-01-10
PKG_SOURCE_VERSION:=7.6.3-1
PKG_BUILD_DEPENDS:=libdlms

PKG_UPSTREAM_VERSION:=0b368c2b7986027dfdb500334b3c70cadb32a21e
PKG_UPSTREAM_URL:=https://git.teltonika.lt/api/v4/projects/352/packages/generic/GuruDLMS/$(PKG_UPSTREAM_VERSION)
PKG_UPSTREAM_FILE:=GuruxDLMS.c-$(PKG_UPSTREAM_VERSION).tar.gz
PKG_LICENSE:=GPL-2.0

GPL_INCLUDE_SRC:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=net
	CATEGORY:=Network
	TITLE:=DLMS Client for COSEM device monitoring
	DEPENDS:=+libsqlite3 +libubus +libubox +libuci +libtlt_uci +libtlt-logger +libpthread \
		+vuci-app-dlms-ui +data-sender-mod-dlms
	PKG_TLT_NAME:=DLMS Client
        PKG_ROUTER:=$(TLT_PLATFORM_NAME)
endef

define Package/$(PKG_NAME)/description
	DLMS Client for COSEM device monitoring by Teltonika
endef

define Package/$(PKG_NAME)/conffiles
/etc/config/$(PKG_NAME)
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/config $(1)/etc/init.d $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/dlms_client.conf $(1)/etc/config/dlms_client
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/dlms_client.init $(1)/etc/init.d/dlms_client
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dlms_client $(1)/usr/sbin/dlms_client
endef

define Build/Test
endef

define Package/$(PKG_NAME)/postinst
	#!/bin/sh

	[ -z "$${IPKG_INSTROOT}" ] || exit 0
	[ -e /lib/data_sender/libdata_sender.sh ] || exit 0

	. /lib/data_sender/libdata_sender.sh

	ds_find_plugin dlms && /etc/init.d/data_sender restart

	exit 0
endef

define Package/$(PKG_NAME)/prerm
	#!/bin/sh
	. /lib/functions.sh
	. /lib/config/uci.sh

	[ -f "/etc/config/data_sender" ] || return 0

	config_load data_sender

	list_length=0
	dlms_list_count=0

	check_list_dlms_in() {
		list_length=$$((list_length+1))
	}

	count_dlms_in() {
		config_get plugin "$$1" plugin ""
		[ "$$plugin" = "dlms" ] && dlms_list_count=$$((dlms_list_count+1))
	}

	del_dlms_in() {
		uci_remove data_sender "$$1"
	}

	check_dlms_col() {
		list_length=0
		dlms_list_count=0

		config_foreach count_dlms_in "input"
		config_list_foreach "$$1" input check_list_dlms_in

		if [ "$$list_length" -ge 2 ] && [ "$$list_length" -ne "$$dlms_list_count" ]; then
			uci_remove data_sender "$$2"
			uci_remove_list data_sender "$$1" input "$$2"
		elif [ "$$list_length" -ge 2 ] && [ "$$list_length" -eq "$$dlms_list_count" ]; then
			config_list_foreach "$$1" input del_dlms_in

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

	start_dlms_in() {
		config_get plugin "$$1" plugin ""
		[ "$$plugin" = "dlms" ] || return 0

		config_foreach check_dlms_col "collection" "$$1"
		uci_commit data_sender 2> /dev/null
		config_load data_sender
	}

	config_foreach start_dlms_in "input"
	/etc/init.d/data_sender reload 2> /dev/null || true

endef

$(eval $(call BuildPackage,$(PKG_NAME)))
