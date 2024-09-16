include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=qssdk
PKG_RELEASE:=1

PKG_LICENSE:=Custom-Permissive-License

GPL_INCLUDE_SRC:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/qssdk
  SECTION:=QCA
  CATEGORY:=QCA Proprietary software
  TITLE:=QCA switch SDK
  FILES:=$(PKG_BUILD_DIR)/qssdk.ko
  DEPENDS:=@(TARGET_ipq40xx||TARGET_ipq806x) +kmod-ppp +kmod-ipt-conntrack +kmod-nft-arp
  AUTOLOAD:=$(call AutoLoad,81,qssdk)
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) -r ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

MAKE_OPTS:= \
        $(KERNEL_MAKE_FLAGS) \
	HNAT_FEATURE=enable \
	RFS_FEATURE=disable \
	HK_CHIP=enable \
	SUPPORT_CHIP="DESS ISISC SHIVA"\
	CHIP_TYPE=ALL_CHIP \
	PRJ_PATH=$(PKG_BUILD_DIR)/src \
	MODULE_TYPE=KSLIB \
        M="$(PKG_BUILD_DIR)"

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS)
endef

$(eval $(call KernelPackage,qssdk))

