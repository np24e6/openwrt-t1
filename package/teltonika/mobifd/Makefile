#
# Copyright (C) 2023 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=mobifd

PKG_SOURCE_VERSION:=7.1

PKG_LICENSE:=Teltonika-closed

PKG_CONFIG_DEPENDS += \
	CONFIG_USE_PROCD \
	CONFIG_USE_OPENRC

include $(INCLUDE_DIR)/package.mk

define Package/mobifd
	MENU:=1
	SECTION:=net
	CATEGORY:=Network
	TITLE:=GSM Modem manager
	DEPENDS:= +libubus +libubox +libgsm +liburc +libuci +libtlt_uci +libtlt-logger \
			+libsqlite3 +libmnfinfo +libboardjson +liblpac
endef

define Package/mobifd/description
	Daemon for managing operator connection establishment and data connection handling.
endef

define Package/mobifd/config
	source "$(SOURCE)/Config.in"
endef

define Package/mobifd/conffiles
/etc/config/operctl
endef

TARGET_CFLAGS += $(if $(CONFIG_MOBIFD_DEBUG),-DMOBIFD_DEBUG) \
		 $(if $(CONFIG_MOBIFD_WHITELIST_AFRICA),-DMOBIFD_WHITELIST_AFRICA) \
		 $(if $(CONFIG_MOBIFD_WHITELIST_AFRICA_EU),-DMOBIFD_WHITELIST_AFRICA_EU)


define Package/mobifd/install
	$(INSTALL_DIR) $(1)/usr/share/gsm/
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/config/ $(1)/etc/init.d/ $(1)/etc/uci-defaults/etc
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mobifd $(1)/usr/sbin/mobifd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/mobifd.init $(1)/etc/init.d/mobifd
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/mobifd.conf $(1)/etc/config/operctl
	$(CP) $(PKG_BUILD_DIR)/files/defaults/02_simcard $(1)/etc/uci-defaults/etc/02_simcard
	$(CP) $(PKG_BUILD_DIR)/files/defaults/106_operctl $(1)/etc/uci-defaults/etc/106_operctl
	$(CP) $(PKG_BUILD_DIR)/files/defaults/107_network_service_mode $(1)/etc/uci-defaults/etc/107_network_service_mode
	$(CP) $(PKG_BUILD_DIR)/files/hotplug/remove_operlist.hotplug $(1)/usr/share/gsm/5-remove-operlist

	$(if $(CONFIG_USE_OPENRC),\
		$(OPENRC_INSTALL) boot mobifd ./mobifd.openrc $(1))
endef

define Build/Test
endef

$(eval $(call BuildPackage,mobifd))
