#
# Copyright (C) 2022 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=post_get

PKG_SOURCE_VERSION:=1.2

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/post-get/Default
  	SECTION:=base
	CATEGORY:=Base system
endef

define Package/post-get
	$(call Package/post-get/Default)
	TITLE:=Call actions over HTTP POST/GET requests
	DEPENDS:=+libuci +libtlt_uci +libubus +libubox +libtlt-logger \
		+libcgi +PACKAGE_post-get-mobile:libgsm \
		+PACKAGE_post-get-io:iomand
endef

define Package/post-get/conffiles
/etc/config/post_get
endef

define Package/post-get-mobile
  $(call Package/post-get/Default)
  TITLE:=Mobile handlers
  DEPENDS:=post-get
endef

define Package/post-get-io
  $(call Package/post-get/Default)
  TITLE:=I/O handlers
  DEPENDS:=post-get
endef

CONFIGURE_ARGS += \
	$(if $(CONFIG_PACKAGE_post-get-mobile),--enable-mobile) \
	$(if $(CONFIG_PACKAGE_post-get-io),--enable-io)


define BuildHandler
  define Package/post-get-$(1)/install
	$(INSTALL_DIR) $$(1)/www/cgi-bin
	for m in $(2); do \
		$(LN) post_get $$(1)/www/cgi-bin/$$$$$$$${m} ; \
	done
  endef
  $$(eval $$(call BuildPackage,post-get-$(1)))
endef

define Package/post-get/install
	$(INSTALL_DIR) $(1)/www/cgi-bin $(1)/etc/config/
	$(INSTALL_DIR) $(1)/etc/uci-defaults/etc

	$(CP) ./files/99_sms_post_get $(1)/etc/uci-defaults/etc
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/post_get $(1)/www/cgi-bin/
	$(INSTALL_DATA) ./files/post_get.conf $(1)/etc/config/post_get
endef

MOBILE_HANDLERS:= \
	sms_send \
	sms_read \
	sms_delete \
	sms_total \
	sms_list \
	mobile_data

IO_HANDLERS:= \
	io_state \
	io_type \
	io_value

$(eval $(call BuildPackage,post-get))
$(eval $(call BuildHandler,mobile,$(MOBILE_HANDLERS)))
$(eval $(call BuildHandler,io,$(IO_HANDLERS)))
