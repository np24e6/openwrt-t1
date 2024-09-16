include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=sim_switch
PKG_VERSION:=2023-09-15
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=3.1
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

include $(INCLUDE_DIR)/package.mk

define Package/sim_switch
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=SIM Switch management daemon
	DEPENDS:=+libubus +libubox +libblobmsg-json +libuci +libgsm +liblog \
			  +libboardjson
endef

define Package/sim_switch/description
	SIM Switch daemon for monitoring rules
endef

define Package/sim_switch/conffiles
/etc/config/sim_switch
endef


define Package/sim_switch/install
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/etc/config $(1)/usr/bin

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/sim_switch.init $(1)/etc/init.d/sim_switch
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/sim_switch.conf $(1)/etc/config/sim_switch
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sim_switch $(1)/usr/bin/sim_switch
endef

$(eval $(call BuildPackage,sim_switch))

