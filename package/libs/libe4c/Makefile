include $(TOPDIR)/rules.mk

PKG_NAME:=libe4c
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/guillermocalvo/exceptions4c.git
PKG_SOURCE_DATE:=2021-01-28
PKG_SOURCE_VERSION:=cce539956be894a7f3f3ac947846400e96ed10fb
PKG_MIRROR_HASH:=72da9063c934b769c0307f82efcdbbeaaf58fd3004d6d383354b2c223fe2a17d

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libe4c
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=Exceptions framework for C language
endef

define Package/libe4c/description
	This library provides the power of exceptions to your C applications.
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libe4c*.a $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/*.h $(1)/usr/include/
endef

define Package/libe4c/install
	true
endef

$(eval $(call BuildPackage,libe4c))
