
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=pam-radius
PKG_VERSION:=2024-04-15
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=ffcaf3a4462639f12b860377671836c28e124dfd
PKG_SOURCE_URL:=https://github.com/FreeRADIUS/pam_radius.git

PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

PKG_LICENSE:=GPL-2.0-or-later
PKG_LICENSE_FILES:=LICENSE

PKG_FIXUP:=autoreconf

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
