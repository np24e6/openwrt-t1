include $(TOPDIR)/rules.mk

PKG_NAME:=ustream-ssl
PKG_RELEASE:=1

PKG_SOURCE_DATE:=2024-03-04
CMAKE_INSTALL:=1

PKG_LICENSE:=ISC
PKG_LICENSE_FILES:=ustream-ssl.h

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(PROJECT_GIT)/project/ustream-ssl.git
PKG_SOURCE_VERSION:=263b9a97cf7e1e2467319c23832b705fc01190b5

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libustream/default
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=ustream SSL Library
  DEPENDS:=+libubox
  ABI_VERSION:=20201210
endef

define Package/libustream-openssl
  $(Package/libustream/default)
  TITLE += (openssl)
  DEPENDS += +PACKAGE_libustream-openssl:libopenssl
  VARIANT:=openssl
endef

define Package/libustream/default/install
	$(INSTALL_DIR) $(1)/lib/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/libustream-ssl.so $(1)/lib/
endef

Package/libustream-openssl/install = $(Package/libustream/default/install)

$(eval $(call BuildPackage,libustream-openssl))
