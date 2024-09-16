include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=ntrip_client_v2

PKG_SOURCE_VERSION:=7.5

CMAKE_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/ntrip_client_v2
	SECTION:=net
	CATEGORY:=Network
	PKG_TLT_NAME=NTRIP
	PKG_ROUTER:=$(TLT_PLATFORM_NAME)
	TITLE:=ntrip_client_v2
	DEPENDS:=+libstdcpp +libuci +libgsm +libubus +libtlt_uci +libjson-c +libblobmsg-json +vuci-app-ntrip-ui +GPS_SUPPORT:libgps
endef

define Package/ntrip_client_v2/teltonika_rut9xx
  PKG_ROUTER:=RUT955
endef

define Package/ntrip_client_v2/teltonika_rut9m
  PKG_ROUTER:=RUT956 RUT906
endef

define Package/ntrip_client_v2/mdm9x07
  PKG_ROUTER:=TRB142 TRB145
endef

define Package/ntrip_client_v2/teltonika_trb2xx
  PKG_ROUTER:=TRB245 TRB255
endef

define Package/ntrip_client_v2/teltonika_rutx
  PKG_ROUTER:=RUTX09 RUTX11 RUTX12 RUTX14 RUTXR1 RUTX50 RUTX10 RUTX08
endef

define Package/ntrip_client_v2/description
	Ntrip client application
endef

ifeq ($(CONFIG_USE_GLIBC),y)
TARGET_CFLAGS += -Wno-write-strings -Wno-unused-result
TARGET_LDFLAGS += -lpthread
endif

define Package/$(PKG_NAME)/conffiles
/etc/config/rs_ntrip
endef


define Package/ntrip_client_v2/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ntrip_client $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/rs_ntrip.init $(1)/etc/init.d/rs_ntrip
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/rs_ntrip.conf $(1)/etc/config/rs_ntrip
endef

$(eval $(call BuildPackage,ntrip_client_v2))
