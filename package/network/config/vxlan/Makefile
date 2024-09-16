include $(TOPDIR)/rules.mk

PKG_NAME:=vxlan
PKG_RELEASE:=7
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/vxlan
  SECTION:=net
  CATEGORY:=Network
  MAINTAINER:=Matthias Schiffer <mschiffer@universe-factory.net>
  TITLE:=Virtual eXtensible LAN config support
  DEPENDS:=+kmod-vxlan
  PKGARCH:=all
  ifeq (m, $(CONFIG_PACKAGE_vxlan))
    PKG_TLT_NAME:=VXLAN
    PKG_ROUTER:=$(TLT_PLATFORM_NAME)
  endif

endef

define Package/vxlan/description
 Virtual eXtensible LAN config support in /etc/config/network.
endef

define Build/Compile
endef

define Build/Configure
endef

define Package/vxlan/install
	true
endef

$(eval $(call BuildPackage,vxlan))
