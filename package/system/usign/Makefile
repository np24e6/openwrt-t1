include $(TOPDIR)/rules.mk

PKG_NAME:=usign
PKG_RELEASE:=1

PKG_SOURCE_DATE:=2020-05-23
CMAKE_INSTALL:=1
PKG_CHECK_FORMAT_SECURITY:=1
PKG_BUILD_FLAGS:=no-mips16 lto

PKG_LICENSE:=ISC
PKG_LICENSE_FILES:=main.c

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(PROJECT_GIT)/project/usign.git
PKG_SOURCE_VERSION:=f1f65026a94137c91b5466b149ef3ea3f20091e9

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/usign
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=+libubox
  TITLE:=OpenWrt signature verification utility
endef

CMAKE_OPTIONS += \
	-DUSE_LIBUBOX=on

define Package/usign/install
	$(INSTALL_DIR) $(1)/usr/bin

	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/usign $(1)/usr/bin
	ln -s usign $(1)/usr/bin/signify
endef

$(eval $(call BuildPackage,usign))
