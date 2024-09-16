
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=pam-radius
PKG_VERSION:=2024-03-07
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=1.3
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)
PKG_LICENSE:=GPL-2.0

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz

PKG_FIXUP:=autoreconf
GPL_INCLUDE_SRC:=1

include $(INCLUDE_DIR)/package.mk

define Package/pam-radius
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=Radius server PAM module
  DEPENDS:=+libpam
endef

define Package/pam-radius/install
	$(INSTALL_DIR) $(1)/usr/lib/security/
	$(CP) $(PKG_BUILD_DIR)/pam_radius_auth.so $(1)/usr/lib/security/
endef

$(eval $(call BuildPackage,pam-radius))
