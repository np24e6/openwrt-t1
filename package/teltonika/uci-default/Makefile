#
# Copyright (C) 2023 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=uci-default
PKG_VERSION:=2023-04-25
PKG_RELEASE:=1

GPL_INCLUDE_SRC:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/migrations.mk

define Package/uci-default
	SECTION:=utils
	CATEGORY:=Base system
	TITLE:=Upgrade UCI configurations by firmware version
endef

define Build/Compile
	true
endef

define Package/uci-default/install
	$(call install_migrations,files,$(1))
endef

$(eval $(call BuildPackage,uci-default))
