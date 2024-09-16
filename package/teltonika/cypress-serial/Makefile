#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=cypress-serial

PKG_SOURCE_VERSION:=1.1.1

PKG_LICENSE:=GPL-2.0-only

GPL_INCLUDE_SRC:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/$(PKG_NAME)
	DEPENDS:=+kmod-usb-serial
	SUBMENU:=Other modules
	TITLE:=Cypress CY7C65213 USB to Serial chip driver with flow control support
	FILES:=$(PKG_BUILD_DIR)/$(PKG_NAME).ko
	AUTOLOAD:=$(call AutoLoad,99,$(PKG_NAME))
	MODPARAMS.$(PKG_NAME):=$(CONFIG_CYPRESS_SERIAL_MODPARAMS)
endef

MAKE_OPTS:= $(KERNEL_MAKE_FLAGS) M="$(PKG_BUILD_DIR)"

define KernelPackage/$(PKG_NAME)/config
	source "$(SOURCE)/Config.in"
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" $(MAKE_OPTS) modules
endef

$(eval $(call KernelPackage,$(PKG_NAME)))
