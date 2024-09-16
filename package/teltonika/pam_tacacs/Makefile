
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=pam-tacacs
PKG_VERSION:=2023-09-01
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=1.0
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/pam-tacacs
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=TACACS+ PAM module
  DEPENDS:=+libpam +libtacacs +libopenssl
endef

define Package/libtacacs
  SECTION:=lib
  CATEGORY:=Base system
  TITLE:=TACACS+ PAM module
  DEPENDS:=+libpam +libopenssl
  PROVIDES:=libtacacs
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libtac/include/libtac.h $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/.libs/libtac.so* $(1)/usr/lib/
endef


define Package/pam-tacacs/install
	$(INSTALL_DIR) $(1)/usr/lib/security/
	$(CP) $(PKG_BUILD_DIR)/.libs/pam_tacplus.so $(1)/usr/lib/security/
endef

define Package/libtacacs/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/.libs/libtac.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,pam-tacacs))
$(eval $(call BuildPackage,libtacacs))
