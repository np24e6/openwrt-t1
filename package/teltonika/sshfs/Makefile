#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sshfs
PKG_VERSION:=3.7.1

PKG_LICENSE:=GPL-2.0-only
PKG_MAINTAINER:=Zoltan HERPAI <wigyori@uid0.hu>

PKG_SOURCE_VERSION:=3.7.1
PKG_SOURCE_FILE:=$(PKG_NAME)-$(PKG_SOURCE_VERSION).tar.xz
PKG_SOURCE_URL:=https://github.com/libfuse/sshfs/releases/download/$(PKG_NAME)-$(PKG_SOURCE_VERSION)

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_HASH:=fe5d3436d61b46974889e0c4515899c21a9d67851e3793c209989f72353d7750

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk
include ../../devel/meson/meson.mk

define Package/sshfs
	TITLE:=SSHFS service for Teltonika package manager
	DEPENDS:=+fuse3-utils +glib2 +libpthread
	SECTION:=net
	CATEGORY:=Network
	SUBMENU:=Filesystem
	URL:=https://github.com/libfuse/sshfs
endef

define Package/sshfs/description
	Simple application to mount and interact with directories and files located on a remote server or workstation over SFTP.
endef

define Package/sshfs/conffiles
/etc/config/sshfs
endef

define Package/sshfs/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/usr/sbin $(1)/etc/config $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/sshfs $(1)/usr/bin/
	$(INSTALL_BIN) ./files/sshfs.config $(1)/etc/config/sshfs
	$(INSTALL_BIN) ./files/sshfs.init $(1)/etc/init.d/sshfs
	$(INSTALL_BIN) ./files/run_sshfs_application.sh $(1)/usr/sbin/run_sshfs_application.sh
endef

$(eval $(call BuildPackage,sshfs))
