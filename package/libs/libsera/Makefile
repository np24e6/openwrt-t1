#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libsera
PKG_VERSION:=1.2.2
PKG_RELEASE:=1
PKG_REV:=e2bcbf012b35cf45f9450b8821fa11178e486753

PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=e2bcbf012b35cf45f9450b8821fa11178e486753
PKG_SOURCE_URL:=https://github.com/SoftwareAG/cumulocity-sdk-c.git
PKG_HASH:=5c761910d8f811b7a6312314c01c3e7306aa8d09d75dfd6d473bff0c17807369


PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_REV).tar.gz
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=COPYRIGHT

include $(INCLUDE_DIR)/package.mk

TARGET:=linux

define Package/libsera
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=Cumulocity IoT C SDK Library.
	DEPENDS:= +libc +libcurl +libpthread +libstdcpp +liblua
endef

define Package/libsera/description
	A simple library to enable Cumulocity IoT Platform on the device.
endef

define Build/Prepare
	$(PKG_UNPACK)
	#$(CP) $(PKG_BUILD_DIR)/Makefile.template $(PKG_BUILD_DIR)/Makefile
	$(Build/Patch)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/include/libsera
	$(INSTALL_DIR) $(1)/usr/include/ext
	$(CP) $(PKG_BUILD_DIR)/include/* $(1)/usr/include/libsera
	$(CP) $(PKG_BUILD_DIR)/ext/* $(1)/usr/include/ext
	$(CP) $(PKG_BUILD_DIR)/lib/linux/release/libsera.so* $(1)/usr/lib/
endef

define Package/libsera/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/lib/linux/release/libsera.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libsera))
