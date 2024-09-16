include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=data-sender

PKG_SOURCE_VERSION:=1.3.1

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/data-sender/Default
  SECTION:=net
  CATEGORY:=Network
endef

define Package/data-sender-pkg/Default
  PKG_ROUTER:=$(TLT_PLATFORM_NAME)
endef

define Package/data-sender
  $(call Package/data-sender/Default)
  DEPENDS:=+libuci +libtlt-logger +libubox +libblobmsg-json +libubus +vuci-app-data-sender-ui
  TITLE:=Data sender daemon by Teltonika
  MENU:=1
endef

define Package/data-sender/config
  source "$(SOURCE)/Config.in"
endef

define Package/data-sender-mod-bluetooth
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libsqlite3
  TITLE:=Input plugin for bluetooth data gathering
endef

define Package/data-sender-mod-modbus
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libsqlite3
  TITLE:=Input plugin for modbus data gathering
endef

define Package/data-sender-mod-dnp3
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libsqlite3
  TITLE:=Input plugin for DNP3 data gathering
endef

define Package/data-sender-mod-opcua
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libsqlite3
  TITLE:=Input plugin for OPC UA data gathering
endef

define Package/data-sender-mod-mbus
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libsqlite3
  TITLE:=Input plugin for M-Bus data gathering
endef

define Package/data-sender-mod-wifiscan
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libsqlite3
  TITLE:=Input plugin for wifi scaner data gathering
endef

define Package/data-sender-mod-mnfinfo
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libmnfinfo
  TITLE:=Input plugin for manufacturer information
endef

define Package/data-sender-mod-gsm
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libgsm
  TITLE:=Input plugin for gsm information
endef

define Package/data-sender-mod-sms_in
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libgsm +liburc
  TITLE:=SMS input plugin
endef

define Package/data-sender-mod-mdcollect
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libgsm +libmdcollect
  TITLE:=Input plugin for data usage information
endef

define Package/data-sender-mod-gps
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libgps
  TITLE:=Input plugin for GPS data gathering
endef

define Package/data-sender-mod-chilli
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libuci +libtltchilli
  TITLE:=Input plugin for CoovaChilli Hotspot data gathering
endef

define Package/data-sender-mod-io
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libuci +libubus +iomand
  TITLE:=Input plugin for Input/Output data gathering
endef

define Package/data-sender-mod-custom
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libblobmsg-json
  TITLE:=Filter plugin for data concatenation
endef

define Package/data-sender-mod-http
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libcurl
  TITLE:=Output plugin for sending data over http
endef

define Package/data-sender-mod-kinesis
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libcurl +libopenssl +libblobmsg-json
  TITLE:=Output plugin for sending data over kinesis
endef

define Package/data-sender-mod-mqtt
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libmosquitto
  TITLE:=Output plugin for sending data over MQTT
endef

define Package/data-sender-mod-socket
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender
  TITLE:=Output plugin for sending data over a UNIX socket
endef

define Package/data-sender-mod-sms
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libgsm
  TITLE:=Output plugin for sending data over SMS
endef

define Package/data-sender-mod-smtp
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libcurl +libuci +@LIBCURL_SMTP
  TITLE:=Output plugin for sending data over SMTP
endef

define Package/data-sender-mod-nflog
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libnetfilter-log +libxtables
  TITLE:=Input plugin for NFLOG
endef

define Package/data-sender-mod-eventlog
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender
  TITLE:=Events log input plugin
endef

define Package/data-sender-mod-lua
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +liblua
  TITLE:=LUA input plugin for custom scripts
endef

define Package/data-sender-mod-lua_out
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +liblua
  TITLE:=LUA output plugin for custom scrip
endef

define Package/data-sender-mod-ftp
 $(call Package/data-sender/Default)
 DEPENDS:=data-sender +libcurl
 TITLE:=Output plugin for sending data over FTP
endef

define Package/data-sender-mod-base
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender
  TITLE:=Base input plugin
endef

define Package/data-sender-mod-lua_format
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +liblua
  TITLE:=LUA filter plugin for custom formating
endef

define Package/data-sender-mod-telegram
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libcurl +libopenssl
  TITLE:=Output plugin for sending data over Telegram
endef

define Package/data-sender-mod-ubus
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libubus
  TITLE:=Output plugin for sending data over UBUS
endef

define Package/data-sender-mod-file
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender
  TITLE:=Output plugin for writing data to a file
  PKG_ROUTER:=$(TLT_PLATFORM_NAME)
endef

define Package/data-sender-mod-dlms
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libsqlite3
  TITLE:=Input plugin for M-Bus data gathering
endef

define Package/data-sender-mod-mqtt_in
  $(call Package/data-sender/Default)
  DEPENDS:=data-sender +libmosquitto
  TITLE:=Input plugin for subscribing MQTT data
endef

CONFIGURE_ARGS += \
		--with-max_instances=$(CONFIG_PACKAGE_DATA_SENDER_COLLECTIONS) \
		--with-max_inputs=$(CONFIG_PACKAGE_DATA_SENDER_INPUTS) \
		$(if $(CONFIG_PACKAGE_DATA_SENDER_BASE64),--enable-base64) \
		$(if $(CONFIG_PACKAGE_DATA_SENDER_SCHEDULER),--enable-scheduler) \
		$(if $(CONFIG_PACKAGE_DATA_SENDER_DEBUG),--enable-debug) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-gps),--enable-gps) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-gsm),--enable-gsm) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-socket),--enable-socket) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-sms),--enable-sms) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-smtp),--enable-smtp) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-chilli),--enable-chilli) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-io),--enable-io) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-modbus),--enable-modbus) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-mbus),--enable-mbus) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-dnp3),--enable-dnp3) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-opcua),--enable-opcua) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-bluetooth),--enable-bluetooth) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-wifiscan),--enable-wifiscan) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-nflog),--enable-nflog) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-mdcollect),--enable-mdcollect) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-lua),--enable-lua) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-lua_out),--enable-lua_out) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-lua_format),--enable-lua_format) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-telegram),--enable-telegram) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-ftp),--enable-ftp) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-file),--enable-file) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-dlms),--enable-dlms) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-mqtt),--enable-mqtt_out) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-mqtt_in),--enable-mqtt_in) \
		$(if $(CONFIG_PACKAGE_data-sender-mod-kinesis),--enable-kinesis) \


define Package/data-sender/install
	$(INSTALL_DIR) $(1)/etc/config $(1)/etc/init.d $(1)/usr/sbin \
			$(1)/lib/troubleshoot $(1)/lib/data_sender
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/data_sender.init $(1)/etc/init.d/data_sender
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/data_sender.conf $(1)/etc/config/data_sender
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/datasender $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/data_sender_troubleshoot.sh \
			$(1)/lib/troubleshoot/data_sender.sh
	$(CP) $(PKG_BUILD_DIR)/files/libdata_sender.sh $(1)/lib/data_sender/
endef

define BuildPlugin
  define Package/data-sender-mod-$(1)/install
	$(INSTALL_DIR) $$(1)/usr/lib/data_sender
	for m in $(2); do \
		$(CP) $(PKG_INSTALL_DIR)/usr/lib/data_sender/ds_$$$$$$$${m}.so $$(1)/usr/lib/data_sender/ ; \
	done
  endef
  $$(eval $$(call BuildPackage,data-sender-mod-$(1)))
endef

$(eval $(call BuildPackage,data-sender))
$(eval $(call BuildPlugin,bluetooth,input_bluetooth))
$(eval $(call BuildPlugin,modbus,input_modbus))
$(eval $(call BuildPlugin,dnp3,input_dnp3))
$(eval $(call BuildPlugin,opcua,input_opcua))
$(eval $(call BuildPlugin,mbus,input_mbus))
$(eval $(call BuildPlugin,wifiscan,input_wifiscan))
$(eval $(call BuildPlugin,mnfinfo,input_mnfinfo))
$(eval $(call BuildPlugin,gsm,input_gsm))
$(eval $(call BuildPlugin,sms_in,input_sms))
$(eval $(call BuildPlugin,mdcollect,input_mdcollect))
$(eval $(call BuildPlugin,gps,input_gps))
$(eval $(call BuildPlugin,chilli,input_chilli))
$(eval $(call BuildPlugin,io,input_io))
$(eval $(call BuildPlugin,custom,format_custom))
$(eval $(call BuildPlugin,http,output_http))
$(eval $(call BuildPlugin,kinesis,output_kinesis))
$(eval $(call BuildPlugin,mqtt,output_mqtt))
$(eval $(call BuildPlugin,socket,output_socket))
$(eval $(call BuildPlugin,sms,output_sms))
$(eval $(call BuildPlugin,smtp,output_smtp))
$(eval $(call BuildPlugin,nflog,input_nflog))
$(eval $(call BuildPlugin,eventlog,input_eventlog))
$(eval $(call BuildPlugin,base,input_base))
$(eval $(call BuildPlugin,lua,input_lua))
$(eval $(call BuildPlugin,lua_out,output_lua))
$(eval $(call BuildPlugin,lua_format,format_lua))
$(eval $(call BuildPlugin,telegram,output_telegram))
$(eval $(call BuildPlugin,ftp,output_ftp))
$(eval $(call BuildPlugin,ubus,output_ubus))
$(eval $(call BuildPlugin,file,output_file))
$(eval $(call BuildPlugin,dlms,input_dlms))
$(eval $(call BuildPlugin,mqtt_in,input_mqtt))
