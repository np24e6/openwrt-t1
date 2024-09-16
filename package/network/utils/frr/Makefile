#
# Copyright (C) 2019 Lucian Cristian <lucian.cristian@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
PKG_NAME:=frr
PKG_VERSION:=8.4.2
PKG_RELEASE:=1
PKG_SOURCE_DATE:=2023-02-17

PKG_SOURCE:=$(PKG_NAME)-$(PKG_SOURCE_DATE).tar.gz
PKG_SOURCE_VERSION:=410d78b7a4f2c8b4d4c3736c6d6cdb710287ca80
PKG_SOURCE_URL:=https://codeload.github.com/FRRouting/frr/tar.gz/$(PKG_SOURCE_VERSION)?

PKG_HASH:=1334b050ea3d4144efeb7bdfe9c6976ebc91744810c5d8a7381a53fac2877ea9
PKG_MAINTAINER:=Lucian Cristian <lucian.cristian@gmail.com>

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_SOURCE_VERSION)
HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_SOURCE_VERSION)

PKG_LICENSE:=GPL-2.0-only LGPL-2.1-only

PKG_DAEMON_AVAILABLE:= \
	babeld \
	bfdd \
	bgpd \
	eigrpd \
	fabricd \
	isisd \
	ldpd \
	nhrp \
	ospfd \
	ospf6d \
	pathd \
	pbrd \
	pimd \
	ripd \
	ripngd \
	staticd \
	vrrpd

PKG_CONFIG_DEPENDS:= \
	CONFIG_IPV6 \
	CONFIG_FRR_OPENSSL \
	CONFIG_FRR_INTERNAL \
	CONFIG_FRR_SNMP \
	CONFIG_FRR_NO_SNMP \
	CONFIG_PACKAGE_frr-libfrr \
	CONFIG_PACKAGE_frr-vtysh \
	CONFIG_PACKAGE_frr-watchfrr \
	CONFIG_PACKAGE_frr-zebra \
	$(patsubst %,CONFIG_PACKAGE_frr-%,$(PKG_DAEMON_AVAILABLE)) \

PKG_FIXUP:=autoreconf
PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

PKG_BUILD_DEPENDS:=frr/host
HOST_BUILD_DEPENDS:=python3/host

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk

define Package/frr-libfrr/config
  source "$(SOURCE)/Config.in"
endef

define Package/frr/Default
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Routing and Redirection
  URL:=https://www.frrouting.org/
  DEPENDS:=frr
endef

define Package/frr
  $(call Package/frr/Default)
  DEPENDS:=+librt
  TITLE:=The FRRouting (FRR) Software Routing Suite
  MENU:=1
endef

define Package/frr/description
  FRRouting is free software that implements and manages various IPv4 and IPv6
  routing protocols.

  Currently FRRouting supports BGP4, BGP4+, OSPFv2, OSPFv3, RIPv1, RIPv2, RIPng,
  IS-IS, PIM-SM/MSDP, LDP, VRRP and Babel as well as very early support for EIGRP and
  NHRP.
endef

define Package/frr-libfrr
  $(call Package/frr/Default)
  TITLE:=zebra library
  DEPENDS+=+librt +libatomic +libcap +libjson-c +libyang +FRR_OPENSSL:libopenssl +FRR_SNMP:libnetsnmp
  CONFLICTS:=quagga-libzebra
endef

define Package/frr-vtysh
  $(call Package/frr/Default)
  DEPENDS+=+frr-libfrr +libreadline +libncurses +more
  TITLE:=integrated shell for frr routing software
  CONFLICTS:=quagga-vtysh
  ifeq (m, $(CONFIG_PACKAGE_frr-vtysh))
    PKG_TLT_NAME:=FRR VTYSH
    PKG_ROUTER:=$(TLT_PLATFORM_NAME)
  endif
endef

define Package/frr-watchfrr
  $(call Package/frr/Default)
  TITLE:=frr watchdog
  DEPENDS+=+frr-libfrr
  DEFAULT:=n if PACKAGE_frr
endef

define Package/frr-zebra
  $(call Package/frr/Default)
  TITLE:=Zebra daemon
  DEPENDS+=+frr-libfrr
  DEFAULT:=y if PACKAGE_frr
  CONFLICTS:=quagga-zebra
endef

define Package/frr-pythontools
  $(call Package/frr/Default)
  TITLE:=Python reload tool
  DEPENDS+=+frr-libfrr +python3-base +python3-light +python3-logging
endef

##Migrate from quagga
##should test file exists and link in frr folder
#define Package/frr/postinst
##!/bin/sh
#if [ -f /etc/quagga/ospfd.conf ]; then
#    ls to /etc/frr
#    sed enable ospfd daemon
#fi
#exit 0
#endef

# define Package/frr-zebra/conffiles
# /etc/frr/zebra.conf
# endef

# define Package/frr-babeld/conffiles
# /etc/frr/babeld.conf
# endef

define Package/frr-bgpd/conffiles
/etc/config/bgp
endef

define Package/frr-eigrpd/conffiles
/etc/config/eigrp
endef

# define Package/frr-isisd/conffiles
# /etc/frr/isisd.conf
# endef

define Package/frr-ldpd/conffiles
/etc/config/mpls
endef

define Package/frr-nhrp/conffiles
/etc/config/nhrp
endef

define Package/frr-ospfd/conffiles
/etc/config/ospf
endef

# define Package/frr-ospf6d/conffiles
# /etc/frr/ospf6d.conf
# endef

# define Package/frr-pbrd/conffiles
# /etc/frr/pbrd.conf
# endef

# define Package/frr-pimd/conffiles
# /etc/frr/pimd.conf
# endef

define Package/frr-ripd/conffiles
/etc/config/rip
endef

# define Package/frr-staticd/conffiles
# /etc/config/static
# endef

# define Package/frr-ripngd/conffiles
# /etc/frr/ripngd.conf
# endef

# define Package/frr-libfrr/conffiles
# /etc/frr/vtysh.conf
# endef

define BuildDaemon
    define Package/frr-$(1)
	$$(call Package/frr/Default)
	TITLE:= $(1) routing engine
	DEPENDS+=frr-libfrr frr-zebra $(2)

	ifeq ($(1),bgpd)
		DEPENDS+=+kmod-tcpmd5sig
		DEPENDS+=+vuci-app-dynamic-routes-bgp-ui
	endif
	ifeq ($(1),eigrpd)
		DEPENDS+=+vuci-app-dynamic-routes-eigrp-ui
	endif
	ifeq ($(1),ospfd)
		DEPENDS+=+vuci-app-dynamic-routes-ospf-ui
	endif
	ifeq ($(1),ripd)
		DEPENDS+=+vuci-app-dynamic-routes-rip-ui
	endif
	ifeq ($(1),nhrp)
		DEPENDS+=+vuci-app-dynamic-routes-nhrp-ui
	endif

#	if [ "$(1)" == "bfdd" ]; then \
#	export HAVE_BFDD == 1 ; fi

		ifeq (m, $(CONFIG_PACKAGE_frr-$(1)))
			ifeq (bgpd, $(1))
				PKG_TLT_NAME:= BGP daemon
			endif
			ifeq (eigrpd, $(1))
				PKG_TLT_NAME:= EIGRP daemon
			endif
			ifeq (ospfd, $(1))
				PKG_TLT_NAME:= OSPF daemon
			endif
			ifeq (ripd, $(1))
				PKG_TLT_NAME:= RIP daemon
			endif
			ifeq (ldpd, $(1))
				PKG_TLT_NAME:= MPLS daemon
			endif
			ifeq (staticd, $(1))
				PKG_TLT_NAME:= STATIC daemon
			endif
			ifeq (nhrp, $(1))
				PKG_TLT_NAME:= NHRP daemon
			endif
			PKG_ROUTER:=$(TLT_PLATFORM_NAME)
		endif
    endef

    define Package/frr-$(1)/install
		$(INSTALL_DIR) $$(1)/usr/sbin
		$(INSTALL_DIR) $$(1)/etc/config
		$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/$(1) $$(1)/usr/sbin/; \
		if [ "$(1)" == "nhrp" ]; then \
		$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/nhrpd $$(1)/usr/sbin/; \
		$(INSTALL_DIR) $$(1)/usr/lib; \
		$(CP) $(PKG_INSTALL_DIR)/usr/lib/libfrrcares.so* $$(1)/usr/lib/; fi
		config_name="$(1)"; \
		config_name="$$$$$$$${config_name::-1}"; \
		config_name="$$$$$$$${config_name/nhr/nhrp}"; \
		config_name="$$$$$$$${config_name/ldp/mpls}"; \
		$(INSTALL_CONF) ./files/configs/"$$$$$$$${config_name}".conf $$(1)/etc/config/"$$$$$$$${config_name}"
    endef

    $$(eval $$(call BuildPackage,frr-$(1)))
endef

TARGET_LDFLAGS += -Wl,--gc-sections,--as-needed -latomic
TARGET_CFLAGS += -flto

define Host/Configure
	$(Host/Configure/Default)
	$(SED) 's/$$$$(MAKE) $$$$(AM_MAKEFLAGS) install-am/# $$$$(MAKE) $$$$(AM_MAKEFLAGS) install-am/' $(HOST_BUILD_DIR)/Makefile.in
endef

HOST_CPPFLAGS += -I$(STAGING_DIR_HOST)/include/libelf
HOST_CONFIGURE_ARGS+= \
	--enable-clippy-only

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOSTPKG)/bin
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/lib/clippy $(STAGING_DIR_HOSTPKG)/bin/
endef

CONFIGURE_ARGS+= \
	--with-clippy=$(STAGING_DIR_HOSTPKG)/bin/clippy \
	--prefix=/usr \
	--enable-shared \
	--disable-static \
	--enable-user=root \
	--enable-group=root \
	--disable-ospfclient \
	--disable-doc \
	--disable-backtrace \
	--disable-watchfrr \
	--disable-bgp-vnc \
	--disable-bgp-bmp \
	--sysconfdir=/etc/frr \
	--localstatedir=/var/run/frr \
	$(if $(CONFIG_FRR_OPENSSL),--with-crypto=openssl,) \
	$(if $(CONFIG_FRR_SNMP),--enable-snmp,) \
	$(foreach m,$(PKG_DAEMON_AVAILABLE), \
	$(call autoconf_bool,CONFIG_PACKAGE_frr-$(m),$(m)) ) \
	$(call autoconf_bool,CONFIG_PACKAGE_frr-vtysh,vtysh) \
	$(call autoconf_bool,CONFIG_PACKAGE_frr-libfrr,zebra)

define Package/frr/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) ./files/frr $(1)/usr/sbin/frr.init
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/frr.init $(1)/etc/init.d/frr
	$(INSTALL_BIN) ./files/frrcommon.sh $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/uci-defaults/7.5
	$(CP) ./files/defaults/97_frr_indexing $(1)/etc/uci-defaults/7.5/97_frr_indexing
	$(CP) ./files/defaults/98_modify-frr.sh $(1)/etc/uci-defaults/7.5/98_modify-frr.sh
	$(CP) ./files/defaults/99_migrate-frr.sh $(1)/etc/uci-defaults/7.5/99_migrate-frr.sh
endef

define Package/frr-watchfrr/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) ./files/watchfrr.sh $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/watchfrr $(1)/usr/sbin/
endef

define Package/frr-zebra/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/zebra $(1)/usr/sbin/
endef

define Package/frr-libfrr/install
	$(INSTALL_DIR) $(1)/usr/lib $(1)/etc/frr
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libfrr.so* $(1)/usr/lib/
	$(if $(CONFIG_FRR_SNMP),$(CP) $(PKG_INSTALL_DIR)/usr/lib/libfrrsnmp.so* $(1)/usr/lib/,)
endef

define Package/frr-pythontools/install
	$(INSTALL_DIR) $(1)/usr/lib/frr $(1)/usr/sbin $(1)/etc/frr
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/*.py $(1)/usr/lib/frr/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/frr-reload $(1)/usr/sbin/
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/tools/etc/frr/support_bundle_commands.conf $(1)/etc/frr/
endef

define Package/frr-vtysh/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/etc/frr
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/vtysh $(1)/usr/bin/
	$(INSTALL_CONF) ./files/vtysh.conf $(1)/etc/frr/
endef

define generate_post_template
define Package/frr-$(1)/postinst
	/etc/init.d/frr reload > /dev/null 2>&1
	exit 0
endef
define Package/frr-$(1)/postrm
	/etc/init.d/frr reload > /dev/null 2>&1
	exit 0
endef
endef

$(foreach pkg,$(PKG_DAEMON_AVAILABLE),$(eval $(call generate_post_template,$(pkg))))
$(eval $(call HostBuild))
$(eval $(call BuildPackage,frr))
$(eval $(call BuildPackage,frr-libfrr))
$(eval $(call BuildPackage,frr-pythontools))
$(eval $(call BuildPackage,frr-watchfrr))
$(eval $(call BuildPackage,frr-zebra))
$(eval $(call BuildPackage,frr-vtysh))
$(eval $(call BuildDaemon,babeld,))
$(eval $(call BuildDaemon,bfdd,))
$(eval $(call BuildDaemon,bgpd,))
$(eval $(call BuildDaemon,eigrpd,))
$(eval $(call BuildDaemon,fabricd,))
$(eval $(call BuildDaemon,isisd,))
$(eval $(call BuildDaemon,ldpd,))
$(eval $(call BuildDaemon,nhrp,+libcares))
$(eval $(call BuildDaemon,ospfd,))
$(eval $(call BuildDaemon,ospf6d,@IPV6))
$(eval $(call BuildDaemon,pathd,))
$(eval $(call BuildDaemon,pbrd,))
$(eval $(call BuildDaemon,pimd,))
$(eval $(call BuildDaemon,ripd,))
$(eval $(call BuildDaemon,ripngd,@IPV6))
$(eval $(call BuildDaemon,staticd,))
$(eval $(call BuildDaemon,vrrpd,))
