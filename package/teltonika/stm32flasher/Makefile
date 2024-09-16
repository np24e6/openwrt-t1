#
# Copyright (C) 2022 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=stm32flasher

PKG_SOURCE_VERSION:=27623f730f1c0f4b1f31179d03d610e19897147b

include $(INCLUDE_DIR)/package.mk

define Package/stm32flasher
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Flasher utility for STM32
	DEPENDS:=+libubus +libubox +libblobmsg-json
endef


define Package/stm32flasher/install
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/usr/bin $(1)/usr/share/stm32
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/autoflash.script $(1)/usr/bin/autoflash
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/stm32flasher $(1)/usr/bin/stm32flasher
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/firmware.bin $(1)/usr/share/stm32/
endef

$(eval $(call BuildPackage,stm32flasher))
