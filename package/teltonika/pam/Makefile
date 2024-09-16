
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=pam
PKG_VERSION:=2022-01-03
PKG_RELEASE:=2

PKG_SOURCE_VERSION:=1.0
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

include $(INCLUDE_DIR)/package.mk

define Package/pam
	SECTION:=utils
	CATEGORY:=Base system
	DEPENDS:=+rpcd +libubus +libubox +libpam +pam-radius +pam-tacacs +libpam-mod-deny +libpam-mod-permit +libpam-mod-umask +libpam-mod-unix +vuci-app-pamd-ui
	TITLE:=Teltonika PAM support
	ifeq (m, $(CONFIG_PACKAGE_pam))
  		PKG_TLT_NAME:=PAM
  		PKG_ROUTER:=$(TLT_PLATFORM_NAME)
  	endif
endef

define Package/pam/description
	This application provides PAM methods through ubus
endef

define Package/pam/conffiles
/etc/config/pam
endef


define Package/pam/install
	$(INSTALL_DIR) $(1)/etc $(1)/usr/sbin/
	$(CP) ./files/* $(1)/etc/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pam $(1)/usr/sbin/pamd
endef

define Package/pam/prerm
	#!/bin/sh
	. /lib/functions.sh
	
	clean_rpcd() {
		uci_set "rpcd" "$$1" "auth_type" "shadow"
	}
	
	clean_dropbear() {
		uci_remove "dropbear" "$$1" "pam"
	}
	
	config_load "rpcd"
	config_foreach clean_rpcd "login"
	uci_commit "rpcd"
	
	config_load "dropbear"
	config_foreach clean_dropbear "dropbear"
	uci_commit "dropbear"
	/etc/init.d/dropbear reload
endef

$(eval $(call BuildPackage,pam))
