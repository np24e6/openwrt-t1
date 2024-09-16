#
# Copyright (C) 2021 Tetlonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=smppd
PKG_VERSION:=2024-08-13

PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=1.7
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/smpp
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=SMPP Transceiver
	DEPENDS:=+libsmpp34 +libubox +libtlt-logger +libgsm +liburc \
		+libubus +libboardjson
	#USERID:=smppd=454:smppd=454
endef

define Package/smpp/description
	SMPP package that provides SMPP protocol
	services for flexible short message data transfer.
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
