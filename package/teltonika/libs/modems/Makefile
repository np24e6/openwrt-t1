#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=modems
PKG_VERSION:=2024-08-29



PKG_LICENSE:=GPL-2.0-only

include $(INCLUDE_DIR)/package.mk

define Package/modems
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=3G/4G modem list
endef

define Build/Compile
endef

define Package/modems/install
	$(INSTALL_DIR) $(1)/lib/network/wwan/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/data/* $(1)/lib/network/wwan/
	#in order to keep the Lede GIT repo free of filenames with colons,
	#we name the files xxxx-yyyy
	# and rename here after copying to the build directory
	shopt -s nullglob ; \
	for filevar in $(1)/lib/network/wwan/*-* ; \
	do \
		FILENAME=$$$$(basename $$$$filevar) ; \
		NEWNAME=$$$${FILENAME//-/:} ; \
		mv "$(1)/lib/network/wwan/$$$$FILENAME" "$(1)/lib/network/wwan/$$$$NEWNAME" ; \
	done
endef

$(eval $(call BuildPackage,modems))
