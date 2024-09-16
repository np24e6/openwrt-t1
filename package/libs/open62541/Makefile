include $(TOPDIR)/rules.mk

PKG_NAME:=open62541
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/$(PKG_NAME)/$(PKG_NAME).git
PKG_SOURCE_VERSION:=v1.3.4
PKG_MIRROR_HASH:=463ae8ff6b91d52a5ee33cfa36a381f3
CMAKE_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

# Disabling encryption, logging and minimizing built-in information model size (breaks certification) yields 222K; otherwise 400K
CMAKE_OPTIONS += \
	-DOPEN62541_VERSION=v$(PKG_VERSION) \
	-DBUILD_SHARED_LIBS=ON \
	-DUA_ENABLE_ENCRYPTION=ON \
	-DUA_ENABLE_ENCRYPTION_OPENSSL=ON \
	-DUA_BUILD_EXAMPLES=ON \
	-DUA_ENABLE_JSON_ENCODING=ON \
	-DUA_LOGLEVEL=0 \
	-DUA_ENABLE_HARDENING=OFF

define Package/open62541
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=implementation of IEC 62541
	DEPENDS=+libopenssl
endef

define Package/open62541/description
	Open source implementation of OPC UA (OPC Unified Architecture) aka IEC 62541 licensed under Mozilla Public License v2.0
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include/open62541
	$(CP) $(PKG_BUILD_DIR)/bin/*.so* $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/ipkg-install/usr/include/open62541/* $(1)/usr/include/open62541
endef

define Package/open62541/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/bin/*.so* $(1)/usr/lib
endef

$(eval $(call BuildPackage,open62541))
