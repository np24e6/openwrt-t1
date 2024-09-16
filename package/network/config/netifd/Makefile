include $(TOPDIR)/rules.mk

PKG_NAME:=netifd
PKG_RELEASE:=2

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(PROJECT_GIT)/project/netifd.git
PKG_SOURCE_DATE:=2024-01-04
PKG_SOURCE_VERSION:=c18cc79d50002ab8529c21184aceb016c61ac61c
PKG_MIRROR_HASH:=0a1080ade51dc4a55249c8899d4d384f665e0d21075adab24ea23a2808165e05
PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=

PKG_BUILD_PARALLEL:=1
PKG_BUILD_FLAGS:=lto

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/netifd
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=+libuci +libnl-tiny +libubus +ubus +ubusd +jshn +libubox
  TITLE:=OpenWrt Network Interface Configuration Daemon
endef

define Package/netifd/conffiles
/etc/udhcpc.user
/etc/udhcpc.user.d/
endef

TARGET_CFLAGS += \
	-I$(STAGING_DIR)/usr/include/libnl-tiny \
	-I$(STAGING_DIR)/usr/include

CMAKE_OPTIONS += \
	-DLIBNL_LIBS=-lnl-tiny \
	-DDEBUG=1

define Package/netifd/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/netifd $(1)/sbin/
	$(CP) ./files/* $(1)/
	$(if $(CONFIG_USES_SOFT_PORT_MIRROR),, \
		$(RM) $(1)/lib/netifd/proto/mirror.sh)
	$(INSTALL_DIR) $(1)/etc/udhcpc.user.d/
	$(CP) $(PKG_BUILD_DIR)/scripts/* $(1)/lib/netifd/

	$(if $(CONFIG_SMP_SUPPORT),, \
		$(RM) $(1)/etc/hotplug.d/net/20-smp-packet-steering)

	$(if $(and $(CONFIG_MOBILE_SUPPORT),$(CONFIG_HAS_SINGLE_ETH_PORT)),,\
    	rm $(1)/etc/hotplug.d/iface/29-setup-mobile-gateway-passthrough-bridge)

	$(if $(CONFIG_ADVANCED_NETWORK),,\
		$(RM) $(1)/lib/netifd/proto/l2tpv3.sh)

	$(if $(CONFIG_PPP_MOBILE_SUPPORT),, \
		$(RM) $(1)/lib/netifd/proto/pppmobile.sh)


endef

$(eval $(call BuildPackage,netifd))
