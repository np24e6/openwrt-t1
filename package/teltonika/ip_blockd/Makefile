include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=ip_blockd
PKG_VERSION:=2024-06-17
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=1.10

PKG_FIXUP:=autoreconf

PKG_LICENSE:=Teltonika-closed

PKG_CONFIG_DEPENDS := \
  CONFIG_IP_BLOCKD_MAC_SUPPORT \
  CONFIG_MOBILE_SUPPORT

include $(INCLUDE_DIR)/package.mk

define Package/ip_blockd
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=IP Block Daemon
	DEPENDS:=+rpcd +ipset +iptables +libubus +libubox +libuci +liblog +libip4tc +libip6tc +libipset +libtlt-logger +libblobmsg-json +libtlt_utils +libsqlite3
	MENU:=1
endef

define Package/ip_blockd/config
  source "$(SOURCE)/Config.in"
endef

define Package/ip_blockd/description
	IP block daemon
endef

define Package/ip_blockd/conffiles
/etc/config/ip_blockd
endef

CONFIGURE_ARGS += $(if $(CONFIG_IP_BLOCKD_MAC_SUPPORT),--enable-mac)
CONFIGURE_ARGS += $(if $(CONFIG_MOBILE_SUPPORT),--enable-phone)


define Package/ip_blockd/install
	$(INSTALL_DIR)  $(1)/etc/config/
	$(INSTALL_DIR)  $(1)/usr/bin/
	$(INSTALL_DIR)  $(1)/usr/sbin/
	$(INSTALL_DIR)  $(1)/etc/init.d/
	$(INSTALL_DIR)  $(1)/usr/share/ip_block/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/insertIpTables.sh $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/ip_blockd.init $(1)/etc/init.d/ip_blockd
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/ip_blockd.conf $(1)/etc/config/ip_blockd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ip_blockd $(1)/usr/bin/
endef

$(eval $(call BuildPackage,ip_blockd))
