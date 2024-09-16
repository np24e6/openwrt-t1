#
# Copyright (C) 2020 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=libnetifd
PKG_VERSION:=2024-06-18
PKG_RELEASE:=1
PKG_LICENSE:=Teltonika-closed

PKG_SOURCE_VERSION:=7.8
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz

include $(INCLUDE_DIR)/package.mk

define Package/libnetifd
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=A library meant for handling interfaces via netifd ubus object
	DEPENDS:=+libubus +libuci +libblobmsg-json +libjson-c +netifd
endef

define Package/libnetifd/description
	Library meant for handling interface information from netifd ubus object.
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/*.so $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/include/*.h $(1)/usr/include/
endef


define Package/libnetifd/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libnetifd.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libnetifd))
