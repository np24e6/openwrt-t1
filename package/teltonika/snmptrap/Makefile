#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=snmptrap
PKG_VERSION:=2023-09-01
PKG_RELEASE:=2

PKG_SOURCE_VERSION:=1.4
PKG_MAINTAINER:=Darius Joksas <darius.joksas@teltonika.lt>
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/snmptrap
	MENU:=1
	SECTION:=net
	CATEGORY:=Network
	TITLE:=Teltonika SNMP trap sender
	DEPENDS:=+libuci +libtlt-logger +libubox +libubus +libnetsnmp \
		+SNMPTRAP_GSM:libgsm +SNMPTRAP_GSM:liburc +SNMPTRAP_IO:iomand
endef

define Package/snmptrap/config
	source "$(SOURCE)/Config.in"
endef

define Package/snmptrap/description
	Teltonika SNMP trap implementation (notification sender)
endef

define Package/snmptrap/conffiles
/etc/config/snmptrap
endef

ifdef CONFIG_SNMPTRAP_GSM
	CONFIGURE_ARGS += --enable-gsm
endif

ifdef CONFIG_SNMPTRAP_IO
	CONFIGURE_ARGS += --enable-io
endif

ifdef CONFIG_SNMPTRAP_CHILLI
	CONFIGURE_ARGS += --enable-chilli
endif


define Package/snmptrap/install
	$(INSTALL_DIR) $(1)/etc/config $(1)/usr/sbin $(1)/etc/init.d
	$(INSTALL_BIN) ./files/snmptrap.init $(1)/etc/init.d/snmptrap
	$(INSTALL_BIN) ./files/snmptrap.conf $(1)/etc/config/snmptrap
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/snmptraps $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,snmptrap))

