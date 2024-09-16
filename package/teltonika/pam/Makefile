
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=pam
PKG_VERSION:=2024-06-20
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=1.1
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)
PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

RCOMPR:=:

define Package/pam
	SECTION:=utils
	CATEGORY:=Base system
	DEPENDS:=+rpcd +libubus +libubox +libpam +pam-radius +pam-tacacs +libpam-mod-deny +libpam-mod-permit +libpam-mod-umask +libpam-mod-unix
	TITLE:=Teltonika PAM support
endef

define Package/pam/description
	Privileged access management (PAM) allows to manage access
	for different services through TACACS+ or Radius server.
endef

define Package/pam/conffiles
/etc/config/pam
endef


define Package/pam/install
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/usr/sbin $(1)/etc/config
	$(INSTALL_CONF) ./files/pam.config $(1)/etc/config/pam
	$(INSTALL_BIN) ./files/pam.init $(1)/etc/init.d/pam
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pam $(1)/usr/sbin/pamd
endef

define Package/pam/prerm
	#!/bin/sh
	[ -z "$${IPKG_INSTROOT}" ] || exit 0
	. /lib/functions.sh
	
	clean_rpcd_login() {
		uci_set "rpcd" "$$1" "auth_type" "shadow"
	}

	clean_rpcd() {
		uci_remove "rpcd" "$$1" "pam_all_users" 2> /dev/null
		uci_remove "rpcd" "$$1" "pam_default_group" 2> /dev/null
	}
	
	clean_dropbear() {
		uci_remove "dropbear" "$$1" "pam" 2> /dev/null
		uci_remove "dropbear" "$$1" "pam_privilege_lvl" 2> /dev/null
	}
	
	config_load "rpcd"
	config_foreach clean_rpcd_login "login"
	config_foreach clean_rpcd "rpcd"
	uci_commit "rpcd"
	
	config_load "dropbear"
	config_foreach clean_dropbear "dropbear"
	uci_commit "dropbear"
	/etc/init.d/dropbear reload
endef

$(eval $(call BuildPackage,pam))
