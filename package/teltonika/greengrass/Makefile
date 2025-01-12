#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk

PKG_NAME:=greengrass
PKG_VERSION:=1.11.4
PKG_RELEASE:=2

STRIP_COMPONENTS:=0


PKG_LICENSE:=Teltonika-3p-closed


include $(INCLUDE_DIR)/target.mk
include $(INCLUDE_DIR)/kernel-version.mk
LINUX_FULL_VERSION:=$(KERNEL_PATCHVER)$(LINUX_VERSION-$(KERNEL_PATCHVER))

include $(INCLUDE_DIR)/package.mk

define Package/greengrass
	SECTION:=net
	CATEGORY:=Network
	DEPENDS:=+USE_GLIBC:libc +bash
	TITLE:=AWS Greengrass Core
endef

define Package/greengrass/description
	AWS Greengrass Core is a service meant for communication
	with Amazon Web Services Greengrass Core cloud software.
endef

define Package/greengrass/conffiles
/etc/config/greengrass
/greengrass/config/config.json
endef






define Package/greengrass/install
	$(INSTALL_DIR) $(1)/etc/config $(1)/etc/init.d $(1)/greengrass $(1)/boot
	$(CP) $(PKG_BUILD_DIR)/greengrass/. $(1)/greengrass/
	$(INSTALL_CONF) ./files/greengrass.conf $(1)/etc/config/greengrass
	$(INSTALL_BIN) ./files/greengrass.init $(1)/etc/init.d/greengrass

	$(CP) $(BUILD_DIR)/linux-$(BOARD)$(if $(SUBTARGET),_$(SUBTARGET))/linux-$(LINUX_FULL_VERSION)/.config $(1)/boot/config-$(LINUX_FULL_VERSION)

	$(if $(CONFIG_USE_GLIBC), \
		$(INSTALL_DIR) $(1)/lib; \
		$(CP) $(TOOLCHAIN_DIR)/lib/libc.so.6 $(1)/lib/libc.so \
	)
endef

group=ggc_group
user=ggc_user

define Package/greengrass/postinst
#!/bin/sh

[ -z "$${IPKG_INSTROOT}" ] || return

grep -q $(group) /etc/group || addgroup $(group)
grep -q $(user) /etc/shadow || adduser -DH -G $(group) $(user)
endef

define Package/greengrass/postrm
#!/bin/sh

[ -z "$${IPKG_INSTROOT}" ] || return

deluser $(user)
sed --in-place '/$(group)/d' /etc/group

rm -rf /greengrass
endef


$(eval $(call BuildPackage,greengrass))
