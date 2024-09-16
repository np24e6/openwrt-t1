#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=DMVPN
PKG_VERSION:=2021-02-11
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/dmvpn
	SUBMENU:=VPN
  	SECTION:=net
  	CATEGORY:=Network
	TITLE:=Dynamic multipoint VPN using NHRP GRE and IPsec
	DEPENDS:=+kmod-gre +IPV6:kmod-gre6 +resolveip +libpthread +ip \
		+kmod-crypto-authenc +kmod-ipsec +kmod-ipsec4 +IPV6:kmod-ipsec6 \
		+kmod-ipt-ipsec +iptables-mod-ipsec +frr +frr-libfrr +libcares \
		+frr-zebra +vuci-app-dmvpn-ui +frr-nhrp +vuci-app-dynamic-routes-nhrp-ui +iptables-mod-hashlimit
	ifeq (m, $(CONFIG_PACKAGE_dmvpn))
		PKG_TLT_NAME:=DMVPN
		PKG_ROUTER:=$(TLT_PLATFORM_NAME)
	endif
endef



define Package/dmvpn/conffiles
/etc/config/dmvpn
endef

define Package/dmvpn/install
	$(INSTALL_DIR) \
		$(1)/etc/config
	$(INSTALL_CONF) files/etc/config/dmvpn \
		$(1)/etc/config/dmvpn
endef

define Package/dmvpn/prerm
	#!/bin/sh
	. /lib/functions.sh
	delete_dmvpn() {
		section="$${1}"
		config="$${2}"
		config_get service "$${section}" service
		[ "$${service}" = "dmvpn" ] && uci_remove "$${config}" "$${section}"
	}
	clean_dmvpn() {
		config="$${1}"
		config_load "$${config}" || return
		config_foreach delete_dmvpn "" "$${config}"
		uci_commit "$${config}"
		/etc/init.d/"$${config}" reload
	}
	clean_dmvpn network
	clean_dmvpn ipsec
	clean_dmvpn frr
	exit 0
endef

$(eval $(call BuildPackage,dmvpn))
