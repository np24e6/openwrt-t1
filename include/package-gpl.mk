#
# Copyright (C) 2024 Teltonika-Networks
#

PKG_VUCI_BUILD_DIR := $(if $(findstring feeds/vuci,$(shell pwd)),$(GPL_BUILD_DIR)/package/feeds/vuci/$(shell basename $$(pwd)),)
PKG_NORMAL_BUILD_DIR := $(GPL_BUILD_DIR)/$(subst $(TOPDIR)/,,$(shell pwd))
PKG_GPL_BUILD_DIR ?= $(if $(PKG_VUCI_BUILD_DIR),$(PKG_VUCI_BUILD_DIR),$(PKG_NORMAL_BUILD_DIR))

IS_TLT_LIC := $(findstring Teltonika-,$(PKG_LICENSE))
IS_NDA_SRC := $(findstring Teltonika-nda-source,$(PKG_LICENSE))

define gpl_clear_install
	[ -e "$(1)/Makefile" ] && \
		sed -i '/^define Build\/InstallGPL/,/^endef/d' "$(1)/Makefile" || true
	find $(1) -iname ".gitlab*" -delete || true
endef

define gpl_clear_vars
	( \
		sed -i "/cmake.mk/d" "$(1)/Makefile"; \
		sed -i '/define Build\/Prepare/,/endef/d' "$(1)/Makefile"; \
		sed -i '/define Build\/Compile/,/endef/d' "$(1)/Makefile"; \
		sed -i "/^PKG_MIRROR_HASH:=/d" "$(1)/Makefile"; \
		sed -i "/^PKG_SOURCE_PROTO:=/d" "$(1)/Makefile"; \
		sed -i "/^PKG_SOURCE_URL/d" "$(1)/Makefile"; \
	)
endef

define gpl_install_mixed
	( \
		mkdir -p "$(1)"; \
		cp -rf . "$(1)"; \
		$(if $(PKG_UPSTREAM_URL), \
			$(if $(PKG_SOURCE_URL), \
				printf "'$(PKG_SOURCE_URL)' '%s/Makefile'\0" "$$(a=$(CURDIR) && echo $${a#$(TOPDIR)/})" >>"$(TOPDIR)/tmp/SOURCE_URLs" && \
				sed -i -E \
					-e '/[\t ]*PKG_SOURCE.*[:?]?=.*$$/d' \
					-e "s#^[\t ]*PKG_UPSTREAM_(SOURCE_)?#PKG_SOURCE_#gm" \
					"$(1)/Makefile"; \
			, \
				sed -i -E \
					-e '/[\t ]*(PKG_SOURCE|PKG_UPSTREAM).*[:?]?=.*$$/d' \
					"$(1)/Makefile"; \
			) \
		, \
			$(if $(findstring $(TLT_GIT),$(PKG_SOURCE_URL)), \
				mkdir -p "$(1)/src"; \
				tar xf "$(DL_DIR)/$(PKG_SOURCE)" --strip-components=$(if $(STRIP_COMPONENTS),$(STRIP_COMPONENTS),1) -C "$(1)/src" ||\
					unzip -q -d "$(1)/src" $(DL_DIR)/$(PKG_SOURCE); \
				sed -i -E \
					-e "/^[\t ]*(PKG_SOURCE_PROTO|PKG_MIRROR_HASH|PKG_SOURCE_URL)[\t ]*[:?]?=/d" \
					"$(1)/Makefile"; \
			) \
		) \
	)
endef

define gpl_install_feeds
	(\
		$(if $(PKG_UPSTREAM_URL), \
			printf "'$(PKG_SOURCE_URL)' '%s/Makefile'\0" "$$(a=$(CURDIR) && echo $${a#$(TOPDIR)/})" >>"$(TOPDIR)/tmp/SOURCE_URLs";) \
		name=$$(basename "$(1)"); \
		rm -rf "$(1)"; \
		TOPDIR="$(GPL_BUILD_DIR)" "$(GPL_BUILD_DIR)/scripts/feeds" install "$${name}"; \
	)
endef

define gpl_install_deps
	(\
		dir=$$(find "$(TOPDIR)/package" -name "$(1)" | xargs -I {} find -L {} -name Makefile -maxdepth 1 -printf "%h\n" | \
			sed 's@$(TOPDIR)/@@g'); \
		[ -n "$${dir}" ] && $(MAKE) -C "$(TOPDIR)/$${dir}" gpl-install || true; \
	)
endef

define gpl_scan_deps
	$(foreach dep,$(PKG_BUILD_DEPENDS), \
		$(if $(findstring :,$(dep)), \
			$(if $(findstring $(PKG_NAME),$(dep)),, \
				$(call gpl_install_deps,$(lastword $(subst :, ,$(dep)))); \
			) \
		, \
			$(if $(findstring /,$(dep)), \
				$(if $(findstring $(PKG_NAME),$(dep)),, \
					$(call gpl_install_deps,$(firstword $(subst /, ,$(dep)))); \
				) \
			, \
				$(call gpl_install_deps,$(dep)); \
			) \
		) \
	)

	$(foreach dep,$(HOST_BUILD_DEPENDS), \
		$(if $(findstring :,$(dep)), \
			$(if $(findstring $(PKG_NAME),$(dep)),, \
				$(call gpl_install_deps,$(lastword $(subst :, ,$(dep)))); \
			) \
		, \
			$(if $(findstring /,$(dep)), \
				$(if $(findstring $(PKG_NAME),$(dep)),, \
					$(call gpl_install_deps,$(firstword $(subst /, ,$(dep)))); \
				) \
			, \
				$(call gpl_install_deps,$(dep)); \
			) \
		) \
	)
endef

ifdef PKG_UPSTREAM_URL
include $(INCLUDE_DIR)/unpack-upstream.mk
endif

define gpl_install_orig_w_patch
	rm -fr "$(1)/src"

	# include upstream code into SDK if it's a subdir of another repo, or if it's hosted on Teltonika servers
	$(if $(PKG_UPSTREAM_SOURCE_SUBDIR)$(findstring $(TLT_GIT),$(PKG_UPSTREAM_URL)), \
		$(eval UPSTREAM_TO_LOCAL_SRC:=1) \
	)

	$(eval OLD_PKG_UPSTREAM_URL:=$(PKG_UPSTREAM_URL))
	$(eval PKG_UPSTREAM_URL:=)
	$(eval OLD_PKG_SOURCE_URL:=$(PKG_SOURCE_URL))
	$(if $(PKG_SOURCE_URL),$(eval PKG_SOURCE_URL:=$(PKG_SOURCE_URL) $(TLT_GIT)))
	$(call gpl_install_mixed,$(1))
	$(eval PKG_SOURCE_URL:=$(OLD_PKG_SOURCE_URL))
	$(eval PKG_UPSTREAM_URL:=$(OLD_PKG_UPSTREAM_URL))
	$(eval PKG_UPSTREAM_SOURCE_DIR:=$(1)$(if $(PKG_UPSTREAM_URL),/upstream))

	mkdir -p "$(PKG_UPSTREAM_SOURCE_DIR)"
	$(call UPSTREAM_UNPACK_CMD,$(PKG_UPSTREAM_SOURCE_DIR))

(\
	check_size() { \
		[[ "$$(du -bs "$$1" | awk '{print $$1}')" -lt 512 ]] && printf 'Error: %s of $(1) is empty!\n' "$$1" >&2 && exit 3; \
	}; \
	cd "$(1)"; \
	check_size upstream; \
	check_size src; \
	diff_txt="$$(diff --recursive --unified --new-file --no-dereference upstream/ src/)"; \
	[ -z "$$diff_txt" ] || { \
		mkdir -p "$(1)/patches"; \
		echo "$$diff_txt" >"$(1)/patches/000_tlt.patch"; \
		rm -fr "$(1)/src"; \
		mv "$(PKG_UPSTREAM_SOURCE_DIR)" "$(1)/src"; \
	}; \
	rm -fr "$(PKG_UPSTREAM_SOURCE_DIR)" $(if $(UPSTREAM_TO_LOCAL_SRC),,"$(1)/src"); \
)
	$(if $(UPSTREAM_TO_LOCAL_SRC), \
		sed -i -E \
			-e '/[\t ]*(PKG_SOURCE|PKG_UPSTREAM|PKG_BUILD_DIR|HOST_BUILD_DIR).*[:?]?=.*$$/d' \
			"$(1)/Makefile"; \
	, \
		$(if $(PKG_SOURCE_URL), \
			printf "'$(PKG_SOURCE_URL)' '%s/Makefile'\0" "$$(a=$(CURDIR) && echo $${a#$(TOPDIR)/})" >>"$(TOPDIR)/tmp/SOURCE_URLs"; \
		) \
		sed -i -E \
			-e '/[\t ]*PKG_SOURCE_(VERSION|URL|PROTO|URL_FILE)[\t ]*[:?]?=.*$$/d' \
			$(if $(PKG_UPSTREAM_BUILD_DIR), \
				-e '/[\t ]*PKG_BUILD_DIR[\t ]*[:?]?=.*$$/d' \
				-e "s#PKG_UPSTREAM_BUILD_DIR#PKG_BUILD_DIR#gm" \
			) \
			$(if $(HOST_UPSTREAM_BUILD_DIR), \
				-e '/[\t ]*HOST_BUILD_DIR[\t ]*[:?]?=.*$$/d' \
				-e "s#HOST_UPSTREAM_BUILD_DIR#HOST_BUILD_DIR#gm" \
			) \
			$(if $(PKG_UPSTREAM_URL_FILE), \
				-e '/[\t ]*PKG_SOURCE[\t ]*[:?]?=.*$$/d' \
				-e "s#PKG_UPSTREAM_URL_FILE#PKG_SOURCE#gm" \
			) \
			$(if $(PKG_UPSTREAM_HASH), \
				-e '/[\t ]*PKG_HASH[\t ]*[:?]?=.*$$/d' \
				-e "s#PKG_UPSTREAM_HASH#PKG_HASH#gm" \
			) \
			-e "s#PKG_UPSTREAM_(SOURCE_)?#PKG_SOURCE_#gm" \
			"$(1)/Makefile"; \
	)
	$(if $(findstring feeds,$(2)), \
		TOPDIR="$(GPL_BUILD_DIR)" "$(GPL_BUILD_DIR)/scripts/feeds" install "$$(basename "$(1)")"; \
	)
endef

define gpl_install_closed
	rm -rf "$(PKG_GPL_BUILD_DIR)"; \
	mkdir -p "$(PKG_GPL_BUILD_DIR)/bin"; \
	cp Makefile "$(PKG_GPL_BUILD_DIR)"; \
	[ -e Config.in ] && cp Config.in "$(PKG_GPL_BUILD_DIR)" || true; \
	$(call gpl_clear_vars,$(PKG_GPL_BUILD_DIR)); \
	$(call Build/InstallGPL,$(PKG_GPL_BUILD_DIR)/bin)
endef

define Build/InstallGPL/Default
	$(eval current_dir:=$(shell pwd))
	$(if $(IS_TLT_LIC), \
		$(if $(and $(IS_NDA_SRC),$(CONFIG_GPL_INCLUDE_WEB_SOURCES)), \
			$(call gpl_install_mixed,$(1)) \
		, \
			$(error Build/InstallGPL section is not defined! Please fix the package.) \
		) \
	, \
		$(if $(strip $(PKG_UPSTREAM_URL)), \
			$(call gpl_install_orig_w_patch,$(1),$(current_dir)) \
		, \
			$(if $(findstring feeds,$(current_dir)), \
				$(if $(findstring feeds/vuci,$(current_dir)), \
					$(call gpl_install_mixed,$(1)) \
				, \
					$(call gpl_install_feeds,$(1)) \
				) \
			, \
				$(call gpl_install_mixed,$(1)) \
			) \
		) \
	)
endef

Build/InstallGPL=$(call Build/InstallGPL/Default,$(1))

$(GPL_BUILD_DIR)/package:
	rm -rf "$@"
	mkdir -p "$@"
	find "$(TOPDIR)/package" -maxdepth 1 -type f -exec cp "{}" "$@" \;
	cp -rf "$(TOPDIR)/feeds" "$(GPL_BUILD_DIR)"

gpl-install: $(GPL_BUILD_DIR)/package
ifdef PKG_UPSTREAM_URL
	UPSTREAM_FETCH=1 $(MAKE) download
endif
	$(if $(IS_TLT_LIC), \
		$(if $(and $(IS_NDA_SRC),$(CONFIG_GPL_INCLUDE_WEB_SOURCES)), \
			$(call Build/InstallGPL,$(PKG_GPL_BUILD_DIR)) \
		, \
			$(call gpl_install_closed,$(PKG_GPL_BUILD_DIR)) \
		) \
	, \
		$(call Build/InstallGPL,$(PKG_GPL_BUILD_DIR)) \
	)
	$(call gpl_scan_deps)
	$(call gpl_clear_install,$(PKG_GPL_BUILD_DIR))
