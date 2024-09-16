#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=port_eventsd

PKG_SOURCE_VERSION:=1.7.1

include $(INCLUDE_DIR)/package.mk

define Package/port_eventsd
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Port Events Daemon
	DEPENDS:=+libubox +libubus +libnl-tiny +liblog +libmnfinfo +libboardjson
endef

define Package/port_events/description
	Switch info parser and switch event reporter
endef

TARGET_CPPFLAGS += -D_GNU_SOURCE -I$(STAGING_DIR)/usr/include/libnl-tiny


define Package/port_eventsd/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/etc/init.d/

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/port_eventsd $(1)/usr/bin/port_eventsd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/port_eventsd.init $(1)/etc/init.d/port_eventsd
endef

$(eval $(call BuildPackage,port_eventsd))
