include $(TOPDIR)/rules.mk

PKG_NAME:=dfota
PKG_VERSION:=2024-08-05
PKG_RELEASE:=1

PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/dfota
	SECTION:=utils
	CATEGORY:=Utilities
	MAINTAINER:=Titas Kutavičius
	TITLE:=Quectel DFOTA update script
	DEPENDS:=+curl +gsmctl +ubus +libubox
endef

define Package/dfota/conffiles
/etc/config/dfota
endef

define Package/dfota/description
 Quectel DFOTA update system
endef


define Build/Configure
endef


define Package/dfota/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d/ $(1)/etc/config
	$(INSTALL_BIN) ./files/dfota.init $(1)/etc/init.d/dfota
	$(INSTALL_BIN) ./files/dfota $(1)/usr/sbin/dfota
	$(INSTALL_CONF) ./files/dfota.config $(1)/etc/config/dfota
endef

$(eval $(call BuildPackage,dfota))
