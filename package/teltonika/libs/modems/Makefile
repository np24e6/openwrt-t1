#
# Copyright (C) 2023 Teltonika-Networks
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=modems
PKG_SOURCE_VERSION:=3.3

include $(INCLUDE_DIR)/package.mk

define Package/modems
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=3G/4G modem list
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
