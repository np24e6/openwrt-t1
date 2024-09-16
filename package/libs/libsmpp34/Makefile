
include $(TOPDIR)/rules.mk

PKG_NAME:=libsmpp34
PKG_VERSION:=1.14.1
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=09726c1cb1ce191cb2f5a848c3feda0f57755951
PKG_SOURCE_URL=https://gitea.osmocom.org/cellular-infrastructure/libsmpp34.git

PKG_SOURCE_DATE:=2021-02-23

PKG_FIXUP:=autoreconf

PKG_INSTALL:=1
PKG_LICENSE:=LGPL-2.1-or-later
PKG_LICENSE_FILES:=COPYING

include $(INCLUDE_DIR)/package.mk

define Package/libsmpp34
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=C library for SMPP 3.4
  DEPENDS:=+libpthread +librt +zlib +libxml2
  URL:=http://git.osmocom.org/libsmpp34
endef

define Package/libsmpp34/description
This package contains a C-language library implementing the SMPP Protocol version 3.4
as specified by the SMPP Developers Forum.
endef


CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--with-build-cc="$(HOSTCC)"

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libsmpp34.so* $(1)/usr/lib/
endef

define Package/libsmpp34/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libsmpp34.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libsmpp34))
