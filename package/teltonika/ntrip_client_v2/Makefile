include $(TOPDIR)/rules.mk

PKG_NAME:=ntrip_client_v2
PKG_VERSION:=2024-06-25

PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=647cae59ee687216180a475d38fdf32ba90a1a25
PKG_SOURCE_URL:=https://github.com/hail-linda/pi_ublox-ntrip.git


PKG_LICENSE:=Apache-2.0
PKG_LICENSE_FILES:=LICENSE NOTICE

CMAKE_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

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
