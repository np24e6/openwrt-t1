include $(TOPDIR)/rules.mk

PKG_NAME:=7zip
PKG_VERSION:=22.0
PKG_RELEASE:=1

PKG_HASH:=40969f601e86aff49aaa0ba0df5ce6fd397cf7e2683a84b591b0081e461ef675

PKG_SOURCE_URL:=https://github.com/ip7z/7zip/releases/download/$(PKG_SOURCE_VERSION)
PKG_SOURCE_VERSION:=22.00
PKG_SOURCE:=7z$(subst .,,$(PKG_SOURCE_VERSION))-src.tar.xz

PKG_LICENSE:=LGPL-2.1-or-later
PKG_LICENSE_FILES:=DOC/License.txt

include $(INCLUDE_DIR)/package.mk

UNPACK_CMD=unxz --stdout $(DL_DIR)/$(PKG_SOURCE) | tar -C $(PKG_BUILD_DIR) -x

define Package/7zip
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=7-Zip archiver (Teltonika version)
	URL:=http://http://www.7-zip.org
	DEPENDS:=+libstdcpp
	PKG_TLT_NAME:=7-zip
	PKG_ROUTER:=$(TLT_PLATFORM_NAME)
endef

define Package/7zip/description
	Minified 7-Zip archiver version with only 7z
	archive format and AES-256 encryption support.
endef

define Build/Compile
	cd $(PKG_BUILD_DIR) && { \
		$(MAKE) 7zr \
		CC="$(TARGET_CC)" \
		CXX="$(TARGET_CXX)" \
		;\
	}
endef

define Package/7zip/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/7zr $(1)/usr/bin
endef

$(eval $(call BuildPackage,7zip))
