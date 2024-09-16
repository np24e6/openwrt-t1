include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/version.mk

PKG_NAME:=ipq-wifi-firmware
PKG_RELEASE:=2
PKG_FLAGS:=nonshared

PKG_LICENSE:=GPL-2.0-or-later

include $(INCLUDE_DIR)/package.mk

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile
endef

# Use ath10k-fwencoder from https://github.com/qca/qca-swiss-army-knife.git
# to generate the firmware-*.bin files here.

ALLWIFIBOARDS:= \
	teltonika_rutx

ALLWIFIPACKAGES:=$(foreach BOARD,$(ALLWIFIBOARDS),ipq-wifi-firmware-$(BOARD))

define Package/ipq-wifi-firmware-default
  SUBMENU:=ath10k Firmware Overrides
  SECTION:=firmware
  CATEGORY:=Firmware
  DEPENDS:=@(TARGET_ipq40xx||TARGET_ipq806x)
  TITLE:=Custom Board
endef

define ipq-wifi-firmware-install-one-to
  $(INSTALL_DIR)  $(2)/lib/firmware/ath10k/$(3)/
  $(INSTALL_DATA) $(1) $(2)/lib/firmware/ath10k/$(3)/firmware-5.bin
endef

define ipq-wifi-firmware-install-one
  $(if $(filter $(suffix $(1)),.QCA4019 .qca4019),\
    $(call ipq-wifi-firmware-install-one-to,$(1),$(2),QCA4019/hw1.0),\
  $(if $(filter $(suffix $(1)),.QCA9888 .qca9888),\
    $(call ipq-wifi-firmware-install-one-to,$(1),$(2),QCA9888/hw2.0),\
  $(if $(filter $(suffix $(1)),.QCA9984 .qca9984),\
    $(call ipq-wifi-firmware-install-one-to,$(1),$(2),QCA9984/hw1.0),\
    $(error Unrecognized firmware-file suffix '$(suffix $(1))' for '$(1)')\
  )))

endef
# Blank line required at end of above define due to foreach context



define generate-ipq-wifi-firmware-package
  define Package/ipq-wifi-firmware-$(1)
    $(call Package/ipq-wifi-firmware-default)
    TITLE:=firmware-5.bin Overrides for $(2)
    CONFLICTS:=$(PREV_BOARD)
  endef

  define Package/ipq-wifi-firmware-$(1)/description
The $(2) requires most recent WLAN firmware
that is not yet present in the upstream wireless firmware distribution.

This package supplies firmware-5.bin file(s) that, in the interim,
overwrite those supplied by the ath10k-firmware-* packages.

This is package is only necessary for the $(2).

Do not install it for any other device!
  endef

  define Package/ipq-wifi-firmware-$(1)/install-overlay
    $$$$(foreach IPQ_WIFI_FIRMWARE_FILE,$$$$(wildcard firmware-$(1).*),\
      $$$$(call ipq-wifi-firmware-install-one,$$$$(IPQ_WIFI_FIRMWARE_FILE),$$(1)))
  endef


  PREV_BOARD+=ipq-wifi-firmware-$(1)
endef

# Add board name to ALLWIFIBOARDS
# Place files in this directory as board-<devicename>.<qca4019|qca9888|qca9984>
# Add $(eval $(call generate-ipq-wifi-package,<devicename>,<display name>))

$(eval $(call generate-ipq-wifi-firmware-package,teltonika_rutx,Teltonika RUTX))

$(foreach PACKAGE,$(ALLWIFIPACKAGES),$(eval $(call BuildPackage,$(PACKAGE))))
