#
# Copyright (C) 2007-2021 OpenWrt.org
# Copyright (C) 2010 Vertical Communications
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/version.mk
include $(INCLUDE_DIR)/feeds.mk

PKG_NAME:=base-files
PKG_FLAGS:=nonshared
PKG_RELEASE:=$(COMMITCOUNT)

PKG_FILE_DEPENDS:=$(PLATFORM_DIR)/ $(GENERIC_PLATFORM_DIR)/base-files/
PKG_BUILD_DEPENDS:=ucert/host
PKG_LICENSE:=GPL-2.0

PKG_ORIGIN_URL:=https://github.com/openwrt/openwrt/blob/openwrt-21.02/package/base-files

# Extend depends from version.mk
PKG_CONFIG_DEPENDS += \
	CONFIG_SIGNED_PACKAGES \
	CONFIG_TARGET_INIT_PATH \
	CONFIG_NAND_SUPPORT \
	CONFIG_CLEAN_IPKG \
	CONFIG_PER_FEED_REPO \
	$(foreach feed,$(FEEDS_AVAILABLE),CONFIG_FEED_$(feed))

include $(INCLUDE_DIR)/package.mk

ifneq ($(DUMP),1)
  STAMP_CONFIGURED:=$(strip $(STAMP_CONFIGURED))_$(shell echo $(CONFIG_TARGET_INIT_PATH) | $(MKHASH) md5)
  TARGET:=-$(BOARD)
endif

define Package/base-files
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=+netifd +libc +jsonfilter +SIGNATURE_CHECK:usign +SIGNATURE_CHECK:openwrt-keyring +NAND_SUPPORT:ubi-utils +fstools +!BUILD_FACTORY_TEST_IMAGE:fwtool
  TITLE:=Base filesystem for OpenWrt
  URL:=http://openwrt.org/
  VERSION:=$(PKG_RELEASE)-$(REVISION)
endef

define Package/base-files/conffiles
/etc/config/
/etc/config/network
/etc/config/system
/etc/crontabs/
/etc/dropbear/
/etc/ethers
/etc/group
/etc/hosts
/etc/inittab
/etc/iproute2/rt_protos
/etc/iproute2/rt_tables
/etc/passwd
/etc/profile
/etc/profile.d
/etc/protocols
/etc/rc.local
/etc/services
/etc/shadow
/etc/shells
/etc/shinit
/etc/sysctl.conf
/etc/sysupgrade.conf
$(call $(TARGET)/conffiles)
endef

define Package/base-files/description
 This package contains a base filesystem and system scripts for OpenWrt.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile/Default

endef
Build/Compile = $(Build/Compile/Default)

ifeq ($(CONFIG_NAND_SUPPORT),)
  define Package/base-files/nand-support
	rm -f $(1)/lib/upgrade/nand.sh
  endef
endif

ifdef CONFIG_INSTALL_LOCAL_KEY
  define Package/base-files/install-local-key
	mkdir -p $(1)/etc/opkg/keys
	$(CP) $(BUILD_KEY).pub $(1)/etc/opkg/keys/`$(STAGING_DIR_HOST)/bin/usign \
		-F -p $(BUILD_KEY).pub`
  endef
endif

define Package/base-files/install
	$(CP) ./files/* $(1)/
	$(Package/base-files/install-local-key)
	$(Package/base-files/nand-support)
	if [ -d $(GENERIC_PLATFORM_DIR)/base-files/. ]; then \
		$(CP) $(GENERIC_PLATFORM_DIR)/base-files/* $(1)/; \
	fi
	if [ -d $(PLATFORM_DIR)/base-files/. ]; then \
		$(CP) $(PLATFORM_DIR)/base-files/* $(1)/; \
	fi
	$(if $(filter-out $(PLATFORM_DIR),$(PLATFORM_SUBDIR)), \
		if [ -d $(PLATFORM_SUBDIR)/base-files/. ]; then \
			$(CP) $(PLATFORM_SUBDIR)/base-files/* $(1)/; \
		fi; \
	)

	$(VERSION_SED_SCRIPT) \
		$(1)/etc/openwrt_version \
		$(1)/usr/lib/os-release

	$(VERSION_SED_SCRIPT) \
		$(1)/etc/openwrt_release \
		$(1)/usr/lib/os-release

	$(SED) "s#%PATH%#$(TARGET_INIT_PATH)#g" \
		$(1)/sbin/hotplug-call \
		$(1)/etc/preinit \
		$(1)/etc/profile

	mkdir -p $(1)/CONTROL
	mkdir -p $(1)/dev
	mkdir -p $(1)/etc/config
	mkdir -p $(1)/etc/crontabs
	mkdir -p $(1)/etc/rc.d
	mkdir -p $(1)/overlay
	mkdir -p $(1)/lib/firmware
	$(if $(LIB_SUFFIX),-$(LN) lib $(1)/lib$(LIB_SUFFIX))
	mkdir -p $(1)/mnt
	mkdir -p $(1)/proc
	mkdir -p $(1)/tmp
	mkdir -p $(1)/usr/lib
	$(if $(LIB_SUFFIX),-$(LN) lib $(1)/usr/lib$(LIB_SUFFIX))
	mkdir -p $(1)/usr/bin
	mkdir -p $(1)/sys
	mkdir -p $(1)/www
	mkdir -p $(1)/root
	$(LN) /proc/mounts $(1)/etc/mtab
	rm -f $(1)/var
	$(LN) tmp $(1)/var
	mkdir -p $(1)/etc
	$(LN) /tmp/resolv.conf /tmp/TZ /tmp/localtime $(1)/etc/

	chmod 0600 $(1)/etc/shadow
	chmod 1777 $(1)/tmp

	$(call Package/base-files/install-target,$(1))
	for conffile in $(1)/etc/config/*; do \
		if [ -f "$$$$conffile" ]; then \
			grep "$$$${conffile##$(1)}" $(1)/CONTROL/conffiles || \
				echo "$$$${conffile##$(1)}" >> $(1)/CONTROL/conffiles; \
		fi \
	done

	$(if $(CONFIG_INCLUDE_CONFIG), \
		echo -e "# Build configuration for board $(BOARD)/$(SUBTARGET)/$(PROFILE)\n" >$(1)/etc/build.config; \
		cat $(BIN_DIR)/config.buildinfo >>$(1)/etc/build.config; \
		cat $(BIN_DIR)/feeds.buildinfo >>$(1)/etc/build.feeds; \
		cat $(BIN_DIR)/version.buildinfo >>$(1)/etc/build.version)

	$(if $(CONFIG_CLEAN_IPKG),, \
		mkdir -p $(1)/etc/opkg/openwrt; \
		$(call FeedSourcesAppend,$(1)/etc/opkg/openwrt/distfeeds.conf); \
		$(VERSION_SED_SCRIPT) $(1)/etc/opkg/openwrt/distfeeds.conf)
	$(if $(CONFIG_IPK_FILES_CHECKSUMS),, \
		rm -f $(1)/sbin/pkg_check)

	# set target name
	$(SED) "s#%%NAME%%#$(TLT_PLATFORM_NAME)#g" \
		$(1)/bin/config_generate \
		$(1)/etc/init.d/system

	# set lan ip
	$(SED) "s#%%LAN_IP%%#$(CONFIG_DEVICE_DEFAULT_IP)#g" $(1)/bin/config_generate \
		$(1)/etc/board.d/1-board_json

	# add compile time timestamp
	date +%s > $(1)/etc/firmware-date

	# remove rfkill script if not supported
	$(if $(CONFIG_RFKILL_SUPPORT),,\
		$(RM) $(1)/etc/rc.button/rfkill)

	$(if $(CONFIG_MOBILE_SUPPORT),,\
		$(RM) $(1)/bin/board_modem)

#		--prod flag
	$(if $(CONFIG_VUCI_COMPILE_LUA),\
		$(RM) $(1)/sbin/memleak_watcher $(1)/etc/init.d/memleak_watcher)

	if [ ! $(CONFIG_MOBILE_SUPPORT) ] || [ $(CONFIG_BASEBAND_SUPPORT) ]; then \
		$(RM) $(1)/lib/preinit/82_modem_power; \
	fi;

	if [ ! $(CONFIG_MOBILE_SUPPORT) ] || [ ! $(CONFIG_SERIAL_SUPPORT) ]; then \
		$(RM) $(1)/etc/init.d/rs_modem $(1)/etc/config/rs_modem; \
	fi;

	if [ ! $(CONFIG_WIFI_SUPPORT) ] && [ ! $(CONFIG_BUILD_FACTORY_TEST_IMAGE) ]; then \
		$(RM) $(1)/sbin/wifi; \
	fi;

	if [ ! $(CONFIG_SERIAL_SUPPORT) ]; then \
		$(RM) $(1)/etc/hotplug.d/tty/01-serial-symlink.sh; \
		$(RM) $(1)/etc/init.d/rs_console $(1)/etc/config/rs_console; \
		$(RM) $(1)/sbin/usb_serial_adapters.sh; \
	fi;

	if [ ! $(CONFIG_NAND_SUPPORT) ]; then \
		$(RM) $(1)/etc/init.d/mount_nand; \
	fi;

	if [ ! $(CONFIG_BUSYBOX_CONFIG_NTPD) ]; then \
		$(RM) $(1)/etc/init.d/ntpserver; \
	fi;

	$(RM) $(1)/etc/config/rutx_console;

	if [ $(CONFIG_BUILD_FACTORY_TEST_IMAGE) ]; then \
		$(RM) $(1)/etc/rc.local $(1)/usr/libexec/login.sh; \
	fi;

	if [ ! $(CONFIG_SMP_SUPPORT) ]; then \
		$(RM) $(1)/lib/functions/smp.sh; \
	fi;
endef

ifneq ($(DUMP),1)
  -include $(PLATFORM_DIR)/base-files.mk
  -include $(PLATFORM_SUBDIR)/base-files.mk
endif

$(eval $(call BuildPackage,base-files))
