#
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ulogd
PKG_VERSION:=2.0.7
PKG_RELEASE:=6

PKG_SOURCE_URL:=https://netfilter.org/projects/ulogd/files/ \
	ftp://ftp.netfilter.org/pub/ulogd/
PKG_SOURCE_VERSION:=2.0.7
PKG_SOURCE:=$(PKG_NAME)-$(PKG_SOURCE_VERSION).tar.bz2
PKG_HASH:=990a05494d9c16029ba0a83f3b7294fc05c756546b8d60d1c1572dc25249a92b

PKG_MAINTAINER:=Alexandru Ardelean <ardeleanalex@gmail.com>
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=COPYING

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

PKG_BUILD_DEPENDS:=libnetfilter-acct libnetfilter-conntrack libnetfilter-log
PKG_CONFIG_DEPENDS:= \
	CONFIG_PACKAGE_ulogd-mod-sqlite

include $(INCLUDE_DIR)/package.mk

define Package/ulogd/Default
  SECTION:=net
  CATEGORY:=Network
  URL:=https://www.netfilter.org/projects/ulogd/index.html
endef

define Package/ulogd
  $(call Package/ulogd/Default)
  DEPENDS:=+libmnl +libnfnetlink +libpthread
  TITLE:=Netfilter userspace logging daemon
  MENU:=1
endef

define Package/ulogd/conffiles
/etc/ulogd.conf
/etc/config/ulogd
endef

define Package/ulogd-mod-nfacct
  $(call Package/ulogd/Default)
  DEPENDS:=ulogd +PACKAGE_ulogd-mod-nfacct:libnetfilter-acct
  TITLE:=Input plugin for flow-based logging (accounting)
endef

define Package/ulogd-mod-nfct
  $(call Package/ulogd/Default)
  DEPENDS:=ulogd +PACKAGE_ulogd-mod-nfct:libnetfilter-conntrack
  TITLE:=Input plugin for flow-based logging (conntracking)
endef

define Package/ulogd-mod-nflog
  $(call Package/ulogd/Default)
  DEPENDS:=ulogd +PACKAGE_ulogd-mod-nflog:libnetfilter-log
  TITLE:=Input plugin using NFLOG
endef

define Package/ulogd-mod-pcap
  $(call Package/ulogd/Default)
  DEPENDS:=ulogd +PACKAGE_ulogd-mod-pcap:libpcap
  TITLE:=Output plugin for logging in pcap format
endef

define Package/ulogd-mod-sqlite
  $(call Package/ulogd/Default)
  DEPENDS:=ulogd +PACKAGE_ulogd-mod-sqlite:libsqlite3
  TITLE:=Output plugin for logging to an SQLite database
endef

define Package/ulogd-mod-syslog
  $(call Package/ulogd/Default)
  DEPENDS:=ulogd
  TITLE:=Syslog output plugin
endef

define Package/ulogd-mod-xml
  $(call Package/ulogd/Default)
  DEPENDS:=ulogd +PACKAGE_ulogd-mod-xml:libnetfilter-acct +PACKAGE_ulogd-mod-xml:libnetfilter-conntrack +PACKAGE_ulogd-mod-xml:libnetfilter-log
  TITLE:=XML output plugin
endef

define Package/ulogd-mod-extra
  $(call Package/ulogd/Default)
  DEPENDS:=ulogd
  TITLE:=Extra plugins
endef

define Package/tlt-ulogd
  $(call Package/ulogd/Default)
  DEPENDS:=+ulogd +ulogd-mod-extra +ulogd-mod-nflog
  TITLE:=Netfilter userspace logging daemon
  MENU:=1
endef

define Package/tlt-ulogd/description
  Traffic logger is a service that provides the possibility
  to set up periodic transfers of the selected traffic log
  to the FTP server.
endef

CONFIGURE_ARGS += \
	--enable-nfacct \
	--enable-nfct \
	--enable-nflog \
	--without-dbi \
	--without-pgsql \
	--without-mysql

ifneq ($(SDK)$(CONFIG_PACKAGE_ulogd-mod-sqlite),)
	CONFIGURE_ARGS += --with-sqlite="$(STAGING_DIR)/usr"
else
	CONFIGURE_ARGS += --without-sqlite
endif

define Package/ulogd/install
	$(INSTALL_DIR) $(1)/etc
	$(CP) $(PKG_BUILD_DIR)/ulogd.conf $(1)/etc/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/ulogd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib/ulogd
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/ulogd/ulogd_raw2packet_BASE.so $(1)/usr/lib/ulogd/
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/etc/config $(1)/sbin
	$(INSTALL_BIN) ./files/ulogd.init $(1)/etc/init.d/ulogd
	$(INSTALL_BIN) ./files/ulogd.conf $(1)/etc/config/ulogd
	$(INSTALL_BIN) ./files/ftp_upload.sh $(1)/sbin/
endef

define BuildPlugin
  define Package/ulogd-mod-$(1)/install
	$(INSTALL_DIR) $$(1)/usr/lib/ulogd
	for m in $(2); do \
		$(CP) $(PKG_INSTALL_DIR)/usr/lib/ulogd/ulogd_$$$$$$$${m}.so $$(1)/usr/lib/ulogd/ ; \
	done
  endef
  $$(eval $$(call BuildPackage,ulogd-mod-$(1)))
endef

define Package/tlt-ulogd/install
	true
endef

ULOGD_EXTRA_PLUGINS:= \
	filter_HWHDR \
	filter_IFINDEX \
	filter_IP2BIN \
	filter_IP2HBIN \
	filter_IP2STR \
	filter_MARK \
	filter_PRINTFLOW \
	filter_PRINTPKT \
	filter_PWSNIFF \
	inppkt_UNIXSOCK \
	output_GPRINT \
	output_GRAPHITE \
	output_IPFIX \
	output_LOGEMU \
	output_OPRINT \

$(eval $(call BuildPackage,ulogd))
$(eval $(call BuildPlugin,nfacct,inpflow_NFACCT))
$(eval $(call BuildPlugin,nfct,inpflow_NFCT))
$(eval $(call BuildPlugin,nflog,inppkt_NFLOG))
$(eval $(call BuildPlugin,pcap,output_PCAP))
$(eval $(call BuildPlugin,sqlite,output_SQLITE3))
$(eval $(call BuildPlugin,syslog,output_SYSLOG))
$(eval $(call BuildPlugin,xml,output_XML))
$(eval $(call BuildPlugin,extra,$(ULOGD_EXTRA_PLUGINS)))
$(eval $(call BuildPackage,tlt-ulogd))
