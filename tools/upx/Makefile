include $(TOPDIR)/rules.mk

PKG_NAME:=upx
PKG_VERSION:=4.2.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-src.tar.xz
PKG_SOURCE_URL:=https://github.com/upx/upx/releases/download/v$(PKG_VERSION)
PKG_HASH:=42ee0455eea610ef7ee732aa1f657b34a351ebcfa64a24c1e2a7aaec74c1e038

PKG_LICENSE:=GPL-2.0-or-later

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION)-src

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/gpl.mk

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/build/release/upx $(STAGING_DIR_HOST)/bin
endef

$(eval $(call HostBuild))
