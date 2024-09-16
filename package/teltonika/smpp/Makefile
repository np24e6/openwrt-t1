#
# Copyright (C) 2021 Tetlonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=smppd
PKG_VERSION:=2023-09-01

PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=1.4
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

include $(INCLUDE_DIR)/package.mk

define Package/smpp
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=SMPP Transceiver
	DEPENDS:=+libsmpp34 +libubox +libtlt-logger +libgsm +liburc \
		+libubus +libboardjson +vuci-app-smpp-ui
	#USERID:=smppd=454:smppd=454
	PKG_TLT_NAME:=SMPP
	PKG_ROUTER:=$(TLT_PLATFORM_NAME)
endef

define Package/smpp/teltonika_rutx
  PKG_ROUTER:=RUTX09 RUTX11 RUTX12 RUTX14 RUTXR1 RUTX50
endef

define Package/smpp/teltonika_rutm
	PKG_ROUTER:=RUTM09 RUTM11 RUTM12 RUTM14 RUTM50 RUTM51
endef

define Package/smpp/description
	SMPP Transceiver
endef

define Package/smpp/conffiles
/etc/config/smpp
endef


define Package/smpp/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/config

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/smppd $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/smpp.init $(1)/etc/init.d/smpp
	$(CP) $(PKG_BUILD_DIR)/files/smpp.conf $(1)/etc/config/smpp
endef

$(eval $(call BuildPackage,smpp))
