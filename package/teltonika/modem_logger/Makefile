#
# Copyright (C) 2023 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=modem_logger

PKG_SOURCE_VERSION:=1.0

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Modem logging tool by Teltonika
	DEPENDS:=+libusb-1.0 +libstdcpp +libpthread
	
	PKG_TLT_NAME:=Modem Logger
	PKG_ROUTER:=$(TLT_PLATFORM_NAME)
	PKG_HIDDEN:=1
endef

define Package/$(PKG_NAME)/description
	Modem firmware logging tool by Teltonika
endef


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/diag_saver $(1)/usr/bin/diag_saver
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/qlog $(1)/usr/bin/qlog
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/modem_logger.sh $(1)/usr/bin/modem_logger
	$(if $(CONFIG_BASEBAND_SUPPORT), \
		$(INSTALL_DIR) $(1)/etc; \
		$(CP) $(PKG_BUILD_DIR)/src/qlog/conf/default.cfg $(1)/etc/modem_logger_default.cfg)
endef

$(eval $(call BuildPackage,$(PKG_NAME)))