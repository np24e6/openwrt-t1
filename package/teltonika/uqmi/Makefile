#
# Copyright (C) 2023 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=uqmi

PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=56cb2d4056fef132ccf78dfb6f3074ae5d109992
PKG_SOURCE_URL:=https://github.com/openwrt/uqmi.git

PKG_MAINTAINER:=Mindaugas Antanavicius <mindaugas.antanavicius@teltonika.lt>

PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=LICENSES

PKG_BUILD_PARALLEL:=1
PKG_BUILD_FLAGS:=gc-sections

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/uqmi
	SECTION:=net
	CATEGORY:=Network
	SUBMENU:=WWAN
	DEPENDS:=+libubox +libblobmsg-json +kmod-usb-net +kmod-usb-net-qmi-wwan +wwan +libtlt_uqmi
	TITLE:=Control utility for mobile broadband modems
endef

define Package/libtlt_uqmi
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=A library for uqmi API.
	DEPENDS:=
endef

define Package/uqmi/description
	uqmi is a command line tool for controlling mobile broadband modems using
	the QMI-protocol.
endef

define Package/libtlt_uqmi/description
	A simple lib with wds commands. Created by Teltonika
endef


TARGET_CFLAGS += \
	-I$(STAGING_DIR)/usr/include

CMAKE_OPTIONS += \
	-DDEBUG=1

define Build/Compile
	#cant add execute permission on patch so do this
	chmod +x $(PKG_BUILD_DIR)/data/gen-verbose-error-list.pl
	$(Build/Compile/Default)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libtlt_uqmi.so $(1)/usr/lib/
endef

define Package/uqmi/install
	$(INSTALL_DIR) $(1)/sbin $(1)/usr/bin $(1)/lib/netifd/proto
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/uqmi $(1)/sbin/
	$(INSTALL_BIN) ./files/dhcp_mobile.script $(1)/lib/netifd/
	$(INSTALL_BIN) ./files/uqmi_shared_functions.sh $(1)/lib/netifd/ ; \
	$(INSTALL_BIN) ./files/qmux_track.sh $(1)/usr/bin/qmuxtrack ; \

	$(if $(CONFIG_BASEBAND_SUPPORT), \
		$(INSTALL_BIN) ./files/connm.sh $(1)/lib/netifd/proto ; \
	, \
		$(INSTALL_BIN) ./files/qmi.sh $(1)/lib/netifd/proto ; \
		$(INSTALL_BIN) ./files/qmux.sh $(1)/lib/netifd/proto)

	$(if $(or $(CONFIG_RNDIS_SUPPORT),$(CONFIG_USB_NCM_SUPPORT)), \
		$(INSTALL_BIN) ./files/ncm.sh $(1)/lib/netifd/proto ; \
		$(INSTALL_BIN) ./files/ncm_conn.sh $(1)/usr/bin/ncm_conn.sh)
endef

define Package/libtlt_uqmi/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libtlt_uqmi.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,uqmi))
$(eval $(call BuildPackage,libtlt_uqmi))
