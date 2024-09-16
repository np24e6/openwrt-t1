include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=dnp3
PKG_VERSION:=2023-12-08
PKG_RELEASE:=1

PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=7.6
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

include $(INCLUDE_DIR)/package.mk

define Package/dnp3
	SECTION:=net
	CATEGORY:=Network
	TITLE:=dnp3 stack
	DEPENDS:=+libstdcpp +libuci +libubus +libopenssl +libsqlite3 +MOBILE_SUPPORT:libgsm \
		+GPS_SUPPORT:libgps +libtlt_uci +MOBILE_SUPPORT:libmdcollect +libjson-c +opendnp3 \
		+vuci-app-dnp3-ui +libubox +libblobmsg-json +libmnfinfo +data-sender-mod-dnp3
	PKG_TLT_NAME:=DNP3
	PKG_ROUTER:=$(TLT_PLATFORM_NAME)
endef

ifeq ($(CONFIG_GPS_SUPPORT),y)
CMAKE_OPTIONS=-DGPS_SUPPORT=TRUE
endif

ifeq ($(CONFIG_MOBILE_SUPPORT),y)
CMAKE_OPTIONS=+ -DMOBILE_SUPPORT=TRUE
endif

define Package/dnp3/conffiles
/etc/config/dnp3_client
/etc/config/dnp3_outstation
endef

define Package/dnp3/description
	Teltonika dnp3 application stack
endef


define Package/dnp3/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dnp3_client $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dnp3_outstation $(1)/usr/sbin/

	$(INSTALL_DIR) $(1)/etc/config/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/dnp3_client.conf $(1)/etc/config/dnp3_client
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/dnp3_outstation.conf $(1)/etc/config/dnp3_outstation

	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/dnp3_client.init $(1)/etc/init.d/dnp3_client
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/dnp3_outstation.init $(1)/etc/init.d/dnp3_outstation
endef

define Package/dnp3/postinst
	#!/bin/sh

	[ -z "$${IPKG_INSTROOT}" ] || exit 0
	[ -e /lib/data_sender/libdata_sender.sh ] || exit 0

	. /lib/data_sender/libdata_sender.sh

	ds_find_plugin dnp3 && /etc/init.d/data_sender restart

	exit 0
endef

define Package/dnp3/prerm
	#!/bin/sh
	. /lib/functions.sh
	. /lib/config/uci.sh

	[ -f "/etc/config/data_sender" ] || return 0

	config_load data_sender

	list_length=0
	dnp3_list_count=0

	check_list_dnp3_in() {
		list_length=$$((list_length+1))
	}

	count_dnp3_in() {
		config_get plugin "$$1" plugin ""
		[ "$$plugin" = "dnp3" ] && dnp3_list_count=$$((dnp3_list_count+1))
	}

	del_dnp3_in() {
		uci_remove data_sender "$$1"
	}

	check_dnp3_col() {
		list_length=0
		dnp3_list_count=0

		config_foreach count_dnp3_in "input"
		config_list_foreach "$$1" input check_list_dnp3_in

		if [ "$$list_length" -ge 2 ] && [ "$$list_length" -ne "$$dnp3_list_count" ]; then
			uci_remove data_sender "$$2"
			uci_remove_list data_sender "$$1" input "$$2"
		elif [ "$$list_length" -ge 2 ] && [ "$$list_length" -eq "$$dnp3_list_count" ]; then
			config_list_foreach "$$1" input del_dnp3_in

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

	start_dnp3_in() {
		config_get plugin "$$1" plugin ""
		[ "$$plugin" = "dnp3" ] || return 0

		config_foreach check_dnp3_col "collection" "$$1"
		uci_commit data_sender 2> /dev/null
		config_load data_sender
	}

	config_foreach start_dnp3_in "input"
	/etc/init.d/data_sender reload 2> /dev/null || true

endef

$(eval $(call BuildPackage,dnp3))
