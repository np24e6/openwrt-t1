include $(TOPDIR)/rules.mk

PKG_NAME:=ntrip_client_v2
PKG_VERSION:=2024-04-02

PKG_SOURCE_VERSION:=7.7

PKG_LICENSE:=Apache-2.0

CMAKE_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=net
	CATEGORY:=Network
	TITLE:=$(PKG_NAME)
	DEPENDS:=+libstdcpp +libuci +libubus +libtlt_uci +libtlt_termios +libjson-c +libblobmsg-json +GPS_SUPPORT:libgps
endef

define Package/$(PKG_NAME)/description
	NTRIP client designed for providing differential correction data acquired via Internet Protocol to RTK receiver or rover connected via serial connection.
	It is not meant to correct the GPS coordinates of the Teltonika device itself.
endef

ifeq ($(CONFIG_USE_GLIBC),y)
TARGET_CFLAGS += -Wno-write-strings -Wno-unused-result
TARGET_LDFLAGS += -lpthread
endif

define Package/$(PKG_NAME)/conffiles
/etc/config/rs_ntrip
endef


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ntrip_client $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/rs_ntrip.init $(1)/etc/init.d/rs_ntrip
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/rs_ntrip.conf $(1)/etc/config/rs_ntrip
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
