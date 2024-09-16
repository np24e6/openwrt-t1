include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

GPL_INCLUDE_SRC:=1

PKG_NAME:=libazure-iot-sdk-c
PKG_VERSION:=2023-01-05
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/Azure/azure-iot-sdk-c.git
PKG_SOURCE_VERSION:=3fd808bee3c88f8578dcc30c0a6d1d396c172070
# Need this to not redownload source each time it is compiled
PKG_MIRROR_HASH:=a39263d0de257d188cc26fa86510b1ac

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libazure-iot-sdk-c
	SECTION:=base
	CATEGORY:=Base system
	DEPENDS:=+libc +libopenssl +libcurl +libuuid
	TITLE:=Azure IoT C SDKs and Libraries
endef

CMAKE_OPTIONS += -DCMAKE_BUILD_TYPE="Release" \
				 -Dbuild_as_dynamic=ON \
				 -Duse_mqtt=ON \
				 -Duse_amqp=OFF \
				 -Duse_http=OFF \
				 -Duse_prov_client=ON \
				 -Dhsm_type_x509=ON \
				 -DBUILD_TESTING=OFF \
				 -Dskip_samples=ON

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR)/usr/include/
	$(INSTALL_DIR) $(STAGING_DIR)/usr/include/azure_macro_utils/
	$(INSTALL_DIR) $(STAGING_DIR)/usr/include/umock_c/
	$(INSTALL_DIR) $(STAGING_DIR)/usr/include/azure_prov_client/
	$(INSTALL_DIR) $(STAGING_DIR)/usr/include/azureiot/
	$(INSTALL_DIR) $(STAGING_DIR)/usr/include/azure_c_shared_utility/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/*.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/azure_macro_utils/*.h $(STAGING_DIR)/usr/include/azure_macro_utils/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/umock_c/*.h $(STAGING_DIR)/usr/include/umock_c/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/azure_prov_client/*.h $(STAGING_DIR)/usr/include/azure_prov_client/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/azureiot/*.h $(STAGING_DIR)/usr/include/azureiot/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/azure_c_shared_utility/*.h $(STAGING_DIR)/usr/include/azure_c_shared_utility/

	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libprov_mqtt_transport.so $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libiothub_client.so* $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libprov_device_client.so* $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libprov_auth_client.so $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libhsm_security_client.so $(STAGING_DIR)/usr/lib/
endef


define Package/libazure-iot-sdk-c/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libprov_mqtt_transport.so $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libiothub_client.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libprov_device_client.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libprov_auth_client.so $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libhsm_security_client.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libazure-iot-sdk-c))
