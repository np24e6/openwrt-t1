include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=libazure-iot-sdk-c
PKG_VERSION:=2024-04-29
PKG_RELEASE:=1

# Need this to not redownload source each time it is compiled

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/Azure/azure-iot-sdk-c.git
PKG_SOURCE_VERSION:=3fd808bee3c88f8578dcc30c0a6d1d396c172070
PKG_HASH:=e9af1ddae3942040c698e60678a6ad6f1051b5fe0ad51b24a4cd87686e1ffc67

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libazure-iot-sdk-c
	SECTION:=base
	CATEGORY:=Base system
	DEPENDS:=+libc +libopenssl +libcurl +libuuid
	TITLE:=Azure IoT C SDKs and Libraries
endef

CMAKE_OPTIONS += -Dbuild_as_dynamic=ON \
		 -Duse_mqtt=ON \
		 -Duse_amqp=OFF \
		 -Duse_http=OFF \
		 -Duse_prov_client=ON \
		 -Dhsm_type_x509=ON \
		 -DBUILD_TESTING=OFF \
		 -Dskip_samples=ON \
		 -Ddont_use_uploadtoblob=ON \
		 -DCMAKE_BUILD_TYPE="Release"

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
