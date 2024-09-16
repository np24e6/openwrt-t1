#
# Copyright (C) 2006-2015 OpenWrt.org
# Copyright (C) 2016-2017 LEDE Project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=opkg
PKG_RELEASE:=1
PKG_FLAGS:=essential

PKG_SOURCE_VERSION:=1bf042dd06751b693a8544d2317e5b969d666b69
PKG_SOURCE_DATE:=2021-06-13

PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=COPYING

PKG_MAINTAINER:=Jo-Philipp Wich <jo@mein.io>

# Extend depends from version.mk
PKG_CONFIG_DEPENDS += \
	CONFIG_SIGNATURE_CHECK \
	CONFIG_TARGET_INIT_PATH

PKG_BUILD_PARALLEL:=1
HOST_BUILD_PARALLEL:=1
PKG_INSTALL:=1

HOST_BUILD_DEPENDS:=libubox/host

PKG_BUILD_FLAGS:=gc-sections

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.openwrt.org/project/opkg-lede.git
PKG_SOURCE_VERSION:=1bf042dd06751b693a8544d2317e5b969d666b69

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/opkg
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=opkg package manager
  DEPENDS:=+uclient-fetch +libpthread +libubox
  URL:=$(PKG_SOURCE_URL)
  MENU:=1
endef

define Package/opkg/description
  Lightweight package management system
  opkg is the opkg Package Management System, for handling
  installation and removal of packages on a system. It can
  recursively follow dependencies and download all packages
  necessary to install a particular package.

  opkg knows how to install both .ipk and .deb packages.
endef

define Package/opkg/conffiles
/etc/opkg.conf
/etc/opkg/keys/
/etc/opkg/customfeeds.conf
endef

EXTRA_CFLAGS += $(TARGET_CPPFLAGS)

CMAKE_OPTIONS += \
	-DBUILD_TESTS=OFF \
	-DHOST_CPU=$(PKGARCH) \
	-DPATH_SPEC="$(TARGET_INIT_PATH)" \
	-DVERSION="$(PKG_SOURCE_VERSION) ($(PKG_SOURCE_DATE))"

CMAKE_HOST_OPTIONS += \
	-DSTATIC_UBOX=ON \
	-DBUILD_TESTS=OFF \
	-DHOST_CPU=$(PKGARCH) \
	-DLOCK_FILE=/tmp/opkg.lock \
	-DVERSION="$(PKG_SOURCE_VERSION) ($(PKG_SOURCE_DATE))"

define Package/opkg/install
	$(INSTALL_DIR) $(1)/usr/lib/opkg
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_DIR) $(1)/etc/opkg
	$(INSTALL_DATA) ./files/customfeeds.conf $(1)/etc/opkg/customfeeds.conf
	$(INSTALL_DATA) ./files/opkg$(2).conf $(1)/etc/opkg.conf
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/opkg-cl $(1)/bin/opkg
  ifneq ($(CONFIG_SIGNATURE_CHECK),)
	echo "option check_signature" >> $(1)/etc/opkg.conf
  endif
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) ./files/opkg-key $(1)/usr/sbin/
	echo src/gz tlt_packages https://opkg.teltonika-networks.com/c013f03e8b82fdac41a8706fe3375cbf4f46e5703fb76ec01babbaea571f6fdb > $(1)/etc/opkg/teltonikafeeds.conf
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/src/opkg-cl $(STAGING_DIR_HOST)/bin/opkg
endef


OPKG_TARGET_UPPER:=$(shell echo $(CONFIG_TLT_VERSIONING_PREFIX) | tr '[:lower:]' '[:upper:]')
#								grep for '_R_' (release prefix) in FW version

$(eval $(call BuildPackage,opkg))
$(eval $(call HostBuild))
