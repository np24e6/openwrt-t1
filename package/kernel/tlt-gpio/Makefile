#
# Copyright (C) 2024 Teltonika-Networks
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=tlt-gpio
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=LICENSE/GPL-2.0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/tlt-gpio
	SUBMENU:=Other modules
	TITLE:=Teltonika gpiochip parameter modification module
	FILES:=$(PKG_BUILD_DIR)/tlt_gpio.ko
	AUTOLOAD:=$(call AutoLoad,50,tlt-gpio)
	KCONFIG:=
endef

MAKE_OPTS:= $(KERNEL_MAKE_FLAGS) M="$(PKG_BUILD_DIR)"

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" $(MAKE_OPTS) modules
endef

$(eval $(call KernelPackage,tlt-gpio))
