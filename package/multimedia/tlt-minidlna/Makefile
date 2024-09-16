#
# Copyright (C) 2010-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tlt-minidlna
PKG_VERSION:=2022-08-29
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.code.sf.net/p/minidlna/git
PKG_SOURCE_DATE:=2022-08-29
PKG_SOURCE_VERSION:=0a6e10e821e920e0fedfc86850cdf90f131b1e5b

PKG_MAINTAINER:=
PKG_LICENSE:=GPL-2.0-or-later BSD-3-Clause
PKG_LICENSE_FILES:=COPYING LICENCE.miniupnpd

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1
PKG_BUILD_DEPENDS:=util-linux

GPL_INCLUDE_SRC:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/tlt-minidlna
  SECTION:=multimedia
  CATEGORY:=Multimedia
  TITLE:=UPnP A/V & DLNA Media Server
  URL:=http://minidlna.sourceforge.net/
  DEPENDS:= +libpthread +libexif +libjpeg +libsqlite3 +libffmpeg-audio-dec \
  	+libid3tag +libflac +libvorbis +libuuid +vuci-app-minidlna-ui +libubus \
	+libubox $(ICONV_DEPENDS) $(INTL_DEPENDS)
  USERID:=minidlna:minidlna
  PKG_TLT_NAME:=DLNA
  PKG_ROUTER:=$(TLT_PLATFORM_NAME)
endef

define Package/tlt-minidlna/teltonika_rut9xx
  PKG_ROUTER:=RUT955
endef

define Package/tlt-minidlna/teltonika_rut9m
  PKG_ROUTER:=RUT956 RUT906
endef

define Package/tlt-minidlna/description
	MiniDLNA (aka ReadyDLNA) is server software with the aim of
	being fully compliant with DLNA/UPnP-AV clients.
endef

define Package/tlt-minidlna/conffiles
/etc/config/minidlna
endef

CONFIGURE_ARGS += \
	--with-libiconv-prefix="$(ICONV_PREFIX)" \
	--with-libintl-prefix="$(INTL_PREFIX)" \
	--with-os-name="RutOS" \
	--with-os-version="$(LINUX_VERSION)" \
	--with-os-url="https://teltonika-networks.com/" \
	--with-db-path="/var/run/minidlna" \
	--with-log-path="/var/log"

TARGET_CFLAGS += -DHAVE_CLOCK_GETTIME

define Package/tlt-minidlna/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/minidlnad $(1)/usr/bin/minidlna
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/minidlna.init $(1)/etc/init.d/minidlna
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/minidlna.config $(1)/etc/config/minidlna
	$(INSTALL_DIR) $(1)/etc/sysctl.d
	$(INSTALL_CONF) ./files/minidlna.sysctl $(1)/etc/sysctl.d/30-minidlna.conf
	$(INSTALL_DIR) $(1)/etc/hotplug.d/block
	$(INSTALL_CONF) ./files/minidlna.hotplug $(1)/etc/hotplug.d/block/20-minidlna-rescan
endef

$(eval $(call BuildPackage,tlt-minidlna))
