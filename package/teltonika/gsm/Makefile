#
# Copyright (C) 2022 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=gsm

PKG_SOURCE_VERSION:=1.3.5

GPL_INCLUDE_HEADERS:=1

include $(INCLUDE_DIR)/package.mk

define Package/gsmd
	SECTION:=net
	CATEGORY:=Network
	TITLE:=Simple server to route AT commands to modem
	DEPENDS:= +libusb-1.0 +libubus +libubox +libuci +libblobmsg-json +modems +libe4c +kmod-usb-serial-option
endef

define Package/gsmctl
	SECTION:=net
	CATEGORY:=Network
	TITLE:=Sending AT commands by ubus to gsmd deamon app
	DEPENDS:=+libblobmsg-json +libubus +libgsm +gsmd +libtlt_uci
endef

define Package/libgsm
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=A library for manage gsm API. Read, list, delete, etc. For parse AT commands
	DEPENDS:=+libblobmsg-json +libubus +libubox +libboardjson +IO_SUPPORT:iomand
	ABI_VERSION:=1.0
endef

define Package/liburc
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=A library to handle URC
	DEPENDS:=+libubus +libubox
endef

define Package/modem-sync
	SECTION:=net
	CATEGORY:=Network
	TITLE:=Automated time synchronization with operator station
	DEPENDS:=+libgsm +libubus +libubox +libblobmsg-json +gsmd +libjson-c
endef

define Package/gsmd/description
	Simple server to route GSM modem AT commands. Created by Teltonika
endef

define Package/gsmctl/description
	Simple executable application to execute and read GSM modem AT commands. Created by Teltonika
endef

define Package/modem-sync/description
	Simple daemon meant for time synchronization with operator station. Created by Teltonika
endef

define Package/libgsm/description
	A simple lib with AT commands parser. Created by Teltonika
endef

define Build/test
	test_pdu.c
endef

define Package/gsmd/conffiles
/etc/config/simcard
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include/libgsm
	$(CP) $(PKG_BUILD_DIR)/libgsm/*.so $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/libgsm/include/*.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/libgsm_utils/*.so $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/libgsm_utils/include/*.h $(1)/usr/include/

	$(CP) $(PKG_BUILD_DIR)/liburc/*.so $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/liburc/include/*.h $(1)/usr/include/

	$(CP) $(PKG_BUILD_DIR)/api/include/* $(1)/usr/include
endef


define Package/libgsm/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libgsm/libgsm.so $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/libgsm_utils/libgsm_utils.so $(1)/usr/lib/
endef

define Package/gsmd/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/config/ $(1)/etc/init.d/ $(1)/usr/share/usb/ \
		$(1)/usr/share/iface/ $(1)/usr/share/gsm/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/gsmd/gsmd $(1)/usr/sbin/gsmd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/gsmd.init $(1)/etc/init.d/gsmd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/gsmd.hotplug $(1)/usr/share/usb/05_gsmd.sh
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/gsm_event.hotplug $(1)/usr/share/iface/89-gsm-event
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/check_vcert.hotplug $(1)/usr/share/gsm/1-check_vcert
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/fill_modem.hotplug $(1)/usr/share/gsm/2-fill-modem-info
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/notify_send_sms.hotplug $(1)/usr/share/gsm/4-notify-send-sms
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/notify_sim_state.hotplug $(1)/usr/share/gsm/4-notify-sim-state
	if [ $(CONFIG_MULTI_DEVICE_QUIRK) ]; then \
		$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/validate_cefs.hotplug $(1)/usr/share/gsm/5-validate-cefs; \
	fi;
	$(INSTALL_CONF) /dev/null $(1)/etc/config/simcard
endef

define Package/gsmctl/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/gsmctl/gsmctl $(1)/usr/sbin/gsmctl
endef

define Package/liburc/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/liburc/liburc.so $(1)/usr/lib/
endef

define Package/modem-sync/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/modem_sync $(1)/usr/sbin/modem_sync
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/modem_sync.init $(1)/etc/init.d/modem_sync
endef

$(eval $(call BuildPackage,libgsm))
$(eval $(call BuildPackage,liburc))
$(eval $(call BuildPackage,gsmd))
$(eval $(call BuildPackage,gsmctl))
#$(eval $(call BuildPackage,modem-sync))
