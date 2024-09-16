#
# Copyright (C) 2022 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=mobutils

PKG_SOURCE_VERSION:=3.9

PKG_LICENSE:=Teltonika-closed

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/mobutils/Default
	SECTION:=base
	CATEGORY:=Base system
	DEPENDS:=+libuci +libtlt_uci +libubus +libubox +libtlt-logger \
		+libmnfinfo
endef

define Package/mobutils
	$(call Package/mobutils/Default)
	TITLE:=Mobile utilities
	MENU:=1
endef

define Package/mobutils-call_utilities
	$(call Package/mobutils/Default)
	TITLE:=Call utilities
	DEPENDS+=mobutils +libgsm +liblog +libboardjson \
		+CALL_UTILS_HANDLER_IO:iomand +libparam
endef

define Package/mobutils/conffiles
/etc/config/sms_gateway
endef

define Package/mobutils-call_utilities/conffiles
/etc/config/call_utils
endef

define Package/mobutils-call_utilities/config
	source "$(SOURCE)/Config-call-utils.in"
endef

define Package/mobutils-sms_utilities
	$(call Package/mobutils/Default)
	TITLE:=SMS utilities
	DEPENDS+=mobutils +libgsm +libcgi +MOBILE_SUPPORT:libmdcollect \
		+libboardjson +liblog +SMS_UTILS_HANDLER_IO:iomand \
		+SMS_UTILS_HANDLER_GPS:libgps +libcurl +@LIBCURL_SMTP \
		+libparam +SMS_UTILS_HANDLER_RMS:librms +liblpac
endef

define Package/mobutils-sms_utilities/conffiles
/etc/config/sms_utils
endef

define Package/mobutils-sms_utilities/config
	source "$(SOURCE)/Config-sms-utils.in"
endef

ifeq ($(CONFIG_USE_GLIBC),y)
TARGET_LDFLAGS += -lcrypt
endif

CONFIGURE_ARGS += \
	$(if $(CONFIG_SMS_UTILS_HANDLER_IO),--enable-io) \
	$(if $(CONFIG_SMS_UTILS_HANDLER_GPS),--enable-gps) \
	$(if $(CONFIG_SMS_UTILS_HANDLER_WIFI),--enable-wifi) \
	$(if $(CONFIG_SMS_UTILS_HANDLER_VPN),--enable-vpn) \
	$(if $(CONFIG_SMS_UTILS_HANDLER_RMS),--enable-rms) \
	$(if $(CONFIG_SMS_UTILS_HANDLER_EXEC),--enable-exec) \
	$(if $(CONFIG_SMS_UTILS_HANDLER_API),--enable-api) \
	$(if $(CONFIG_SMS_UTILS_HANDLER_ESIM),--enable-esim) \
	$(if $(CONFIG_SMS_FWD_HANDLER_HTTP),--enable-fwd-http) \
	$(if $(CONFIG_SMS_FWD_HANDLER_REPLY),--enable-fwd-reply) \
	$(if $(CONFIG_SMS_FWD_HANDLER_SMS),--enable-fwd-sms) \
	$(if $(CONFIG_SMS_FWD_HANDLER_SMTP),--enable-fwd-smtp) \

CONFIGURE_ARGS += \
	$(if $(CONFIG_CALL_UTILS_HANDLER_WIFI),--enable-call-wifi) \
	$(if $(CONFIG_CALL_UTILS_HANDLER_IO),--enable-call-io)



define Package/mobutils/install
	$(INSTALL_DIR) $(1)/etc/init.d/ $(1)/etc/config/
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/sms_gateway.conf $(1)/etc/config/sms_gateway
endef

define Package/mobutils-call_utilities/install
	$(INSTALL_DIR) $(1)/sbin/ $(1)/etc/config/ $(1)/etc/init.d/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/call_utils $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/call_utils/files/call_utils.init $(1)/etc/init.d/call_utils
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/call_utils/files/call_utils.conf $(1)/etc/config/call_utils
endef

define Package/mobutils-sms_utilities/install
	$(INSTALL_DIR) $(1)/usr/sbin/ $(1)/etc/init.d/ $(1)/etc/config/ \
			$(1)/etc/sms_utils/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sms_utils/files/sms_utils.init $(1)/etc/init.d/sms_utils
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/sms_utils $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/uci-defaults/etc

	$(CP) $(PKG_BUILD_DIR)/sms_utils/files/api_list.json $(1)/etc/sms_utils/api_list.json
	$(CP) $(PKG_BUILD_DIR)/files/99_sms_utils-mobutils $(1)/etc/uci-defaults/etc
	$(CP) $(PKG_BUILD_DIR)/files/99_sms_utils-mobutils_3 $(1)/etc/uci-defaults/etc
	$(CP) $(PKG_BUILD_DIR)/files/99_sms_gateway $(1)/etc/uci-defaults/etc

	$(INSTALL_CONF) $(PKG_BUILD_DIR)/sms_utils/files/sms_utils.conf $(1)/etc/config/sms_utils;

	$(if  $(CONFIG_DUAL_SIM_SUPPORT), \
		cat $(PKG_BUILD_DIR)/sms_utils/files/dualsim.conf >> $(1)/etc/config/sms_utils)

	$(if  $(CONFIG_GPS_SUPPORT), \
		cat $(PKG_BUILD_DIR)/sms_utils/files/gps.conf >> $(1)/etc/config/sms_utils)

	$(if  $(CONFIG_WIFI_SUPPORT), \
		cat $(PKG_BUILD_DIR)/sms_utils/files/wifi.conf >> $(1)/etc/config/sms_utils)

	$(if  $(CONFIG_ESIM_SUPPORT), \
		cat $(PKG_BUILD_DIR)/sms_utils/files/esim.conf >> $(1)/etc/config/sms_utils)

endef

$(eval $(call BuildPackage,mobutils))
$(eval $(call BuildPackage,mobutils-call_utilities))
$(eval $(call BuildPackage,mobutils-sms_utilities))
