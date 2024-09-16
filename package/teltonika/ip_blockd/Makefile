include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=ip_blockd
PKG_VERSION:=2024-05-23
PKG_RELEASE:=2

PKG_SOURCE_VERSION:=1.9.1

PKG_FIXUP:=autoreconf

PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/ip_blockd
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=IP Block Daemon
	DEPENDS:=+rpcd +ipset +iptables +libubus +libubox +libuci +liblog +libip4tc +libip6tc +libipset +libtlt-logger +libblobmsg-json +libubox +libboardjson +libtlt_utils +libsqlite3
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

ifdef CONFIG_IP_BLOCKD_MAC_SUPPORT
	CONFIGURE_ARGS += --enable-mac
endif


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
