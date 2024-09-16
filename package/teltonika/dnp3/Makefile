include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=dnp3

PKG_SOURCE_VERSION:=7.5

include $(INCLUDE_DIR)/package.mk

define Package/dnp3
	SECTION:=net
	CATEGORY:=Network
	TITLE:=dnp3 stack
	DEPENDS:=+libstdcpp +libuci +libubus +libopenssl +libsqlite3 +libgsm +GPS_SUPPORT:libgps \
		+libtlt_uci +libmdcollect +libjson-c +opendnp3 +vuci-app-dnp3-ui +libubox \
		+libblobmsg-json +libmnfinfo +data-sender-mod-dnp3
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

$(eval $(call BuildPackage,dnp3))
