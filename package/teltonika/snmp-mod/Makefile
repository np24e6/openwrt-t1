include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=snmp-mod
PKG_VERSION:=2023-09-01
PKG_RELEASE:=2

PKG_SOURCE_VERSION:=3.4
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

include $(INCLUDE_DIR)/package.mk

define Package/snmp-mod/Default
	SUBMENU:=SNMP modules
	SECTION:=base
	CATEGORY:=Base system
	DEPENDS:=+libnetsnmp +libubus +libubox
endef

define Package/snmp
	$(call Package/snmp-mod/Default)
	DEPENDS+=+snmpd +vuci-app-snmp-ui +snmp-mib +WIFI_SUPPORT:snmp-mod-chilli +snmp-mod-device \
			+GPS_SUPPORT:snmp-mod-gps +MOBILE_SUPPORT:snmp-mod-gsm +IO_SUPPORT:snmp-mod-io \
			+snmp-mod-sqm +snmp-mod-vlan +WIFI_SUPPORT:snmp-mod-wireless +snmp-mod-port +snmptrap
	TITLE:=SNMP Meta package

	ifeq (m, $(CONFIG_PACKAGE_snmp))
		PKG_TLT_NAME:=SNMP
		PKG_ROUTER:=$(TLT_PLATFORM_NAME)
	endif
endef

define Package/snmp-mod-chilli
	$(call Package/snmp-mod/Default)
	DEPENDS+=+libuci +libtltchilli
	TITLE:=SNMP coovachilli module
endef

TARGET_CFLAGS += $(if $(CONFIG_PACKAGE_snmp-mod-chilli),-DCHILLI_SUPPORT)

define Package/snmp-mod-device
	$(call Package/snmp-mod/Default)
	DEPENDS+=+libuci +libmnfinfo
	TITLE:=SNMP mnf info module
endef

define Package/snmp-mod-gps
	$(call Package/snmp-mod/Default)
	DEPENDS+=+libgps +libgsm
	TITLE:=SNMP GPS module
endef

define Package/snmp-mod-gsm
	$(call Package/snmp-mod/Default)
	DEPENDS+=+libgsm +libmdcollect
	TITLE:=SNMP GSM module
endef

define Package/snmp-mod-io
	$(call Package/snmp-mod/Default)
	DEPENDS+=+iomand
	TITLE:=SNMP I/O module
endef

define Package/snmp-mod-wireless
	$(call Package/snmp-mod/Default)
	DEPENDS+=+libuci +libubus
	TITLE:=SNMP wireless module
endef

define Package/snmp-mod-vlan
	$(call Package/snmp-mod/Default)
	DEPENDS+=+libuci
	TITLE:=SNMP VLAN module
endef

define Package/snmp-mod-sqm
	$(call Package/snmp-mod/Default)
	DEPENDS+=+libuci
	TITLE:=SNMP traffic shaping/SQM module
endef

define Package/snmp-mod-port
	$(call Package/snmp-mod/Default)
	TITLE:=SNMP Port module
endef

define Package/snmp-mib
	SUBMENU:=SNMP modules
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=SNMP teltonika MIBs
endef

define Package/snmp/install
	true
endef


define Package/snmp-mib/install
	$(INSTALL_DIR) $(1)/etc/snmp
	$(INSTALL_DIR) $(1)/etc/snmp/modules
	$(INSTALL_DIR) $(1)/etc/uci-defaults

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/generate_MIBs.sh $(1)/etc/snmp/
	$(CP) $(PKG_BUILD_DIR)/files/modules/* $(1)/etc/snmp/modules/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/scripts/* $(1)/etc/snmp/modules/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/snmpd.defaults $(1)/etc/uci-defaults/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/launch_generate_MIBs.sh $(1)/etc/uci-defaults/
endef

define BuildPlugin
  define Package/snmp-mod-$(1)/install
	$(INSTALL_DIR) $$(1)/usr/lib/snmpd-mod
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(1).so $$(1)/usr/lib/snmpd-mod/
  endef
  $$(eval $$(call BuildPackage,snmp-mod-$(1)))
endef

$(eval $(call BuildPackage,snmp))
$(eval $(call BuildPackage,snmp-mib))
$(eval $(call BuildPlugin,chilli))
$(eval $(call BuildPlugin,device))
$(eval $(call BuildPlugin,gps))
$(eval $(call BuildPlugin,gsm))
$(eval $(call BuildPlugin,io))
$(eval $(call BuildPlugin,wireless))
$(eval $(call BuildPlugin,vlan))
$(eval $(call BuildPlugin,sqm))
$(eval $(call BuildPlugin,port))
