include $(TOPDIR)/rules.mk

PKG_NAME:=libe4c
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/guillermocalvo/exceptions4c.git
PKG_SOURCE_DATE:=2021-01-28
PKG_SOURCE_VERSION:=cce539956be894a7f3f3ac947846400e96ed10fb
PKG_MIRROR_HASH:=72da9063c934b769c0307f82efcdbbeaaf58fd3004d6d383354b2c223fe2a17d

include $(INCLUDE_DIR)/package.mk

define Package/libe4c
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=Exceptions framework for C language
endef

define Package/libe4c/description
	This library provides the power of exceptions to your C applications.
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) ./files/Makefile $(PKG_BUILD_DIR)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/libe4c.so $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/src/e4c.h $(1)/usr/include
endef

define Package/libe4c/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libe4c.so $(1)/usr/lib
endef

$(eval $(call BuildPackage,libe4c))
