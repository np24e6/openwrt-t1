include $(TOPDIR)/rules.mk

PKG_NAME:=emsdk
PKG_VERSION:=3.1.31

PKG_SOURCE_URL:=https://github.com/emscripten-core/emsdk/archive/refs/tags/
PKG_SOURCE_URL_FILE:=$(PKG_VERSION).tar.gz
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_HASH:=1d38b7375e12e85197165a4c51d76d90e1d9db8c2c593b64cfaec4338af54750

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/gpl.mk

define Host/Compile
	true
endef

define Host/Install
	$(HOST_BUILD_DIR)/emsdk install latest 
	$(HOST_BUILD_DIR)/emsdk activate latest
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/opt/emsdk
	$(CP) $(HOST_BUILD_DIR) $(STAGING_DIR_HOST)/opt/emsdk
endef

$(eval $(call HostBuild))
