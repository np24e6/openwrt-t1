#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=qssdk-shell
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/qssdk-shell
  SECTION:=QCA
  CATEGORY:=QCA Proprietary software
  TITLE:=QCA switch SDK shell
  DEPENDS:=@(TARGET_ipq40xx||TARGET_ipq806x) +kmod-qssdk
  #MENU:=1
endef



define Build/Configure
endef

MAKE_OPTS:= \
	ARCH="$(ARCH)" \
	CC="$(TARGET_CROSS)gcc" \
	HNAT_FEATURE=enable \
	RFS_FEATURE=disable \
	HK_CHIP=enable \
	SUPPORT_CHIP=DESS \
	CHIP_TYPE=ALL_CHIP \
	PRJ_PATH=$(PKG_BUILD_DIR) \
	MODULE_TYPE=USLIB
	



define Package/qssdk-shell/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/qssdk-shell $(1)/usr/bin
endef

define Package/qssdk-hnat
  TITLE:=QCA SSDK HNAT
  CATEGORY:=QCA Proprietary software
  SUBMENU:=QSSDK features
  DEPENDS:=@(TARGET_ipq40xx||TARGET_ipq806x) +qssdk-shell
endef

define Package/qssdk-hnat/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/qssdk-hnat.init $(1)/etc/init.d/qssdk-hnat
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/qssdk-hnat.defaults $(1)/etc/uci-defaults/99_qssdk_hnat
endef

$(eval $(call BuildPackage,qssdk-shell))
$(eval $(call BuildPackage,qssdk-hnat))


