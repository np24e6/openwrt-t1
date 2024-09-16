include $(TOPDIR)/rules.mk

PKG_NAME:=libcgi
PKG_VERSION:=1.3.0
PKG_RELEASE:=1

CMAKE_INSTALL:=1

PKG_LICENSE:=LGPL-2.1-or-later
PKG_LICENSE_FILES:=LICENSES/LGPL-2.1.txt

PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=v1.3.0
PKG_SOURCE_URL=https://github.com/rafaelsteil/libcgi.git

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libcgi
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=C library to build CGI applications
endef

define Package/libcgi/description
	LibCGI is a simple to use yet powerful library written from scratch to \
	assist in the making of CGI appliactions in C
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include/libcgi
	$(CP) $(PKG_BUILD_DIR)/include/libcgi/* $(1)/usr/include/libcgi
	$(CP) $(PKG_BUILD_DIR)/src/libcgi.so* $(1)/usr/lib/
endef

define Package/libcgi/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/src/libcgi.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libcgi))
