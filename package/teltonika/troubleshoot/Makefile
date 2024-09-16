#
# Copyright (C) 2023 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=troubleshoot
PKG_VERSION:=1.1

include $(INCLUDE_DIR)/package.mk

define Package/troubleshoot
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Troubleshoot generator by Teltonika
endef



define Package/troubleshoot/install
	$(INSTALL_DIR) $(1)/sbin $(1)/lib/functions $(1)/lib/troubleshoot

	$(INSTALL_BIN) ./files/troubleshoot.sh $(1)/sbin
	$(CP) ./files/libtroubleshoot.sh $(1)/lib/functions
	$(CP) ./files/troubleshoot/kernel.sh $(1)/lib/troubleshoot
	$(CP) ./files/troubleshoot/logdb.sh $(1)/lib/troubleshoot
	$(CP) ./files/troubleshoot/network.sh $(1)/lib/troubleshoot

	$(if $(CONFIG_MOBILE_SUPPORT),\
		$(CP) ./files/troubleshoot/modem.sh $(1)/lib/troubleshoot)
endef

$(eval $(call BuildPackage,troubleshoot))
