# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2006-2020 OpenWrt.org

PKG_DEFAULT_DEPENDS = +libc

ifneq ($(PKG_NAME),toolchain)
  PKG_FIXUP_DEPENDS = $(if $(filter kmod-%,$(1)),$(2),$(PKG_DEFAULT_DEPENDS) $(filter-out $(PKG_DEFAULT_DEPENDS),$(2)))
else
  PKG_FIXUP_DEPENDS = $(2)
endif

PKG_ORIGIN_URL ?= $(PKG_SOURCE_URL)

define Package/Default
  CONFIGFILE:=
  SECTION:=opt
  CATEGORY:=Extra packages
  DEPENDS:=
  MDEPENDS:=
  CONFLICTS:=
  PROVIDES:=
  EXTRA_DEPENDS:=
  MAINTAINER:=$(PKG_MAINTAINER)
  PKG_TLT_NAME:=
  PKG_APP_NAME:=
  PKG_HW_INFO:=
  PKG_ROUTER:=
  PKG_REBOOT:=
  SOURCE:=$(patsubst $(TOPDIR)/%,%,$(CURDIR))
  ifneq ($(PKG_VERSION),)
    ifneq ($(PKG_RELEASE),)
      VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
    else
      VERSION:=$(PKG_VERSION)
    endif
  else
    VERSION:=$(PKG_RELEASE)
  endif
  ABI_VERSION:=
  ifneq ($(PKG_FLAGS),)
    PKGFLAGS:=$(PKG_FLAGS)
  else
    PKGFLAGS:=
  endif
  ifneq ($(ARCH_PACKAGES),)
    PKGARCH:=$(ARCH_PACKAGES)
  else
    PKGARCH:=$(BOARD)
  endif
  DEFAULT:=
  MENU:=
  SUBMENU:=
  SUBMENUDEP:=
  TITLE:=
  KCONFIG:=
  BUILDONLY:=
  HIDDEN:=
  PKG_HIDDEN:=
  URL:=
  VARIANT:=
  DEFAULT_VARIANT:=
  USERID:=
  ALTERNATIVES:=
  LICENSE:=$(PKG_LICENSE)
  LICENSE_FILES:=$(PKG_LICENSE_FILES)
  FILE_MODES:=$(PKG_FILE_MODES)
endef

Build/Patch:=$(Build/Patch/Default)
ifneq ($(strip $(PKG_UNPACK)),)
  define Build/Prepare/Default
	@if [ "$(PKG_SOURCE_PROTO)" == "git" ] && [ -d "$(CURDIR)/git" ]; then \
		$(CP) -r $(CURDIR)/git/* $(PKG_BUILD_DIR); \
	else \
		$(PKG_UNPACK); \
		[ ! -d ./src/ ] || $(CP) ./src/. $(PKG_BUILD_DIR); \
	fi

	$(Build/Patch)
  endef
endif

EXTRA_CXXFLAGS = $(EXTRA_CFLAGS)
ifeq ($(CONFIG_BUILD_NLS),y)
    DISABLE_NLS:=
else
    DISABLE_NLS:=--disable-nls
endif

CONFIGURE_PREFIX:=/usr
CONFIGURE_ARGS = \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--program-prefix="" \
		--program-suffix="" \
		--prefix=$(CONFIGURE_PREFIX) \
		--exec-prefix=$(CONFIGURE_PREFIX) \
		--bindir=$(CONFIGURE_PREFIX)/bin \
		--sbindir=$(CONFIGURE_PREFIX)/sbin \
		--libexecdir=$(CONFIGURE_PREFIX)/lib \
		--sysconfdir=/etc \
		--datadir=$(CONFIGURE_PREFIX)/share \
		--localstatedir=/var \
		--mandir=$(CONFIGURE_PREFIX)/man \
		--infodir=$(CONFIGURE_PREFIX)/info \
		$(DISABLE_NLS) \
		$(DISABLE_IPV6)

CONFIGURE_VARS = \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(EXTRA_CXXFLAGS)" \
		CPPFLAGS="$(TARGET_CPPFLAGS) $(EXTRA_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(EXTRA_LDFLAGS)" \

CONFIGURE_PATH = .
CONFIGURE_CMD = ./configure

replace_script=$(FIND) $(1) -name $(2) | $(XARGS) chmod u+w; \
	       $(FIND) $(1) -name $(2) | $(XARGS) -n1 cp --remove-destination \
	       $(SCRIPT_DIR)/$(2);

define Build/Configure/Default
	(cd $(PKG_BUILD_DIR)/$(CONFIGURE_PATH)/$(strip $(3)); \
	if [ -x $(CONFIGURE_CMD) ]; then \
		$(call replace_script,$(PKG_BUILD_DIR)/$(3),config.guess) \
		$(call replace_script,$(PKG_BUILD_DIR)/$(3),config.sub) \
		$(CONFIGURE_VARS) \
		$(2) \
		$(CONFIGURE_CMD) \
		$(CONFIGURE_ARGS) \
		$(1); \
	fi; \
	)
endef

MAKE_VARS = \
	CFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS) $(TARGET_CPPFLAGS) $(EXTRA_CPPFLAGS)" \
	CXXFLAGS="$(TARGET_CXXFLAGS) $(EXTRA_CXXFLAGS) $(TARGET_CPPFLAGS) $(EXTRA_CPPFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS) $(EXTRA_LDFLAGS)"

MAKE_FLAGS = \
	$(TARGET_CONFIGURE_OPTS) \
	CROSS="$(TARGET_CROSS)" \
	ARCH="$(ARCH)"

MAKE_INSTALL_FLAGS = \
	$(MAKE_FLAGS) \
	DESTDIR="$(PKG_INSTALL_DIR)"

MAKE_PATH ?= .

define Build/Compile/Default
	$(if $(shell [ -d ./bin ] && echo "1"), \
		$(CP) ./bin/. $(PKG_BUILD_DIR), \
		$(MAKE_VARS) \
		$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR)/$(MAKE_PATH) \
			$(MAKE_FLAGS) \
			$(1); \
	)
endef

define Build/Install/Default
	$(MAKE_VARS) \
	$(MAKE) -C $(PKG_BUILD_DIR)/$(MAKE_PATH) \
		$(MAKE_INSTALL_FLAGS) \
		$(if $(1), $(1), install);
endef

define Build/Dist/Default
	$(call Build/Compile/Default, DESTDIR="$(PKG_BUILD_DIR)/tmp" CC="$(TARGET_CC)" dist)
endef

define Build/DistCheck/Default
	$(call Build/Compile/Default, DESTDIR="$(PKG_BUILD_DIR)/tmp" CC="$(TARGET_CC)" distcheck)
endef
