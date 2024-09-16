#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk

PKG_NAME:=greengrass
PKG_VERSION:=1.11.4
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-openwrt-armv7l-$(PKG_VERSION).tar.gz
PKG_HASH:=fce721a94d8d5607bf0fecad693eea604034ebfc35e948fea79bfb2b9d6d75bb

PKG_UPSTREAM_VERSION:=1.11.4
PKG_UPSTREAM_URL:=https://d1onfpft10uf5o.cloudfront.net/greengrass-core/downloads/$(PKG_UPSTREAM_VERSION)
PKG_UPSTREAM_URL_FILE:=greengrass-openwrt-armv7l-$(PKG_UPSTREAM_VERSION).tar.gz

PKG_LICENSE:=Teltonika-3p-closed
PKG_LICENSE_FILES:=ggc/core/LICENSE ggc/packages/LICENSE
PKG_LICENSE_FILES:=ota/ota_agent/LICENSE/LICENSE ota/ota_agent_v.1.3.0/LICENSE/LICENSE

PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

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
