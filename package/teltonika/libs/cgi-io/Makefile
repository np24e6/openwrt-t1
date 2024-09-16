include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=cgi-io
PKG_VERSION:=2023-08-31
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=1.4
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

include $(INCLUDE_DIR)/package.mk

define Package/cgi-io
	SECTION:=vuci
	CATEGORY:=VuCI
	SUBMENU:=api-core
	DEPENDS:=+libubox +libubus +libuci
	TITLE:=CGI utility for handling up/downloading of files
endef

define Package/cgi-io/description
	This package contains an cgi utility that is useful for up/downloading files
endef


define Package/cgi-io/install
	$(INSTALL_DIR) $(1)/usr/libexec $(1)/www/cgi-bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/cgi-io $(1)/usr/libexec
	$(LN) ../../usr/libexec/cgi-io $(1)/www/cgi-bin/cgi-upload 
	$(LN) ../../usr/libexec/cgi-io $(1)/www/cgi-bin/cgi-backup 
	$(LN) ../../usr/libexec/cgi-io $(1)/www/cgi-bin/cgi-tcpdump 
	$(LN) ../../usr/libexec/cgi-io $(1)/www/cgi-bin/cgi-troubleshoot 
	$(LN) ../../usr/libexec/cgi-io $(1)/www/cgi-bin/cgi-mib
endef

$(eval $(call BuildPackage,cgi-io))
