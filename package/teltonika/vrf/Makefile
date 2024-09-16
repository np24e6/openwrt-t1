#
# Copyright (C) 2023 Teltonika
#

include $(TOPDIR)/rules.mk

PKG_NAME:=vrf
PKG_VERSION:=2023-10-20
PKG_RELEASE:=1
PKG_LICENSE:=Teltonika-closed

GPL_INCLUDE_SRC:=1

PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

include $(INCLUDE_DIR)/package.mk

define Package/vrf
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=vrf protocol handler
	DEPENDS:=+kmod-vrf
endef

define Build/Compile
endef

define Package/vrf/install
	$(INSTALL_DIR) $(1)/lib/netifd/proto
	$(INSTALL_BIN) ./files/vrf.sh $(1)/lib/netifd/proto
endef

$(eval $(call BuildPackage,vrf))
