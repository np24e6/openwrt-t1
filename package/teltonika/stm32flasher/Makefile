#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=stm32flasher

PKG_SOURCE_VERSION:=2.7

PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/stm32flasher
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Flasher utility for STM32
	DEPENDS:=+libgpiod
endef


define Package/stm32flasher/install
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/usr/bin $(1)/usr/share/stm32
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/autoflash.script $(1)/usr/bin/autoflash
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/stm32flasher $(1)/usr/bin/stm32flasher
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/firmware.bin $(1)/usr/share/stm32/
endef

$(eval $(call BuildPackage,stm32flasher))
