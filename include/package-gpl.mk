#
# Copyright (C) 2023 Teltonika-Networks
#

PKG_VUCI_BUILD_DIR := $(if $(findstring feeds/vuci,$(shell pwd)),$(GPL_BUILD_DIR)/package/feeds/vuci/$(shell basename $$(pwd)),)
PKG_NORMAL_BUILD_DIR := $(GPL_BUILD_DIR)/$(subst $(TOPDIR)/,,$(shell pwd))
PKG_GPL_BUILD_DIR ?= $(if $(PKG_VUCI_BUILD_DIR),$(PKG_VUCI_BUILD_DIR),$(PKG_NORMAL_BUILD_DIR))

define gpl_clear_install
	[ -e "$(1)/Makefile" ] && \
		sed -i '/^define Build\/InstallGPL/,/^endef/d' "$(1)/Makefile" || true
endef

define gpl_clear_vars
	( \
		sed -i "/cmake.mk/d" "$(1)/Makefile"; \
		sed -i '/define Build\/Prepare/,/endef/d' "$(1)/Makefile"; \
		sed -i '/define Build\/Compile/,/endef/d' "$(1)/Makefile"; \
		sed -i "/^PKG_MIRROR_HASH:=/d" "$(1)/Makefile"; \
		sed -i "/^PKG_SOURCE_PROTO:=/d" "$(1)/Makefile"; \
		sed -i "/^PKG_SOURCE_URL/d" "$(1)/Makefile"; \
		sed -i "/^PKG_MIRROR_HASH:=/d" "$(1)/Makefile"; \
	)
endef

define gpl_install_mixed
	( \
		mkdir -p "$(1)"; \
		cp -rf . "$(1)"; \
		$(if $(PKG_UPSTREAM_URL), \
			printf "'$(PKG_SOURCE_URL)' '%s/Makefile'\0" "$$(a=$(CURDIR) && echo $${a#$(TOPDIR)/})" >>"$(TOPDIR)/tmp/SOURCE_URLs" && \
			sed -i'' -E \
				-e '/ *PKG_SOURCE_URL *[:?]?=.*$$/d' \
				-e "s#PKG_UPSTREAM_URL( *[:?]?=.*$$)#PKG_SOURCE_URL\1#" \
				"$(1)/Makefile" \
		, \
			$(if $(findstring $(TLT_GIT),$(PKG_SOURCE_URL)), \
				mkdir -p "$(1)/src"; \
				tar xf "$(TOPDIR)/dl/$(PKG_SOURCE)" --strip-components=1 -C "$(1)/src"; \
				sed --in-place='' --regexp-extended \
					--expression="/^ *PKG_SOURCE_PROTO *[:?]?=/d" \
					--expression="/^ *PKG_MIRROR_HASH *[:?]?=/d" \
					--expression="/^ *PKG_SOURCE_URL *[:?]?=/d" "$(1)/Makefile"; \
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

define gpl_install_orig_w_patch
	$(eval URL_FILE:=$(PKG_UPSTREAM_URL))
	$(eval HASH:=skip)
	$(eval FILE:=$(PKG_UPSTREAM_FILE))
	rm -fr "$(1)/src"

	$(eval OLD_TLT_GIT:=$(TLT_GIT))
	$(eval TLT_GIT:=)
	$(call DownloadMethod/$(call dl_method,$(PKG_UPSTREAM_URL),))
	$(eval TLT_GIT:=$(OLD_TLT_GIT))

	$(eval OLD_PKG_UPSTREAM_URL:=$(PKG_UPSTREAM_URL))
	$(eval PKG_UPSTREAM_URL:=)
	$(call gpl_install_mixed,$(1))
	$(eval PKG_UPSTREAM_URL:=$(OLD_PKG_UPSTREAM_URL))
(\
	mkdir -p "$(1)/orig"; \
	cd "$(1)/orig"; \
	case $(PKG_UPSTREAM_FILE) in \
	*.tar.bz2|*.tbz2)	tar xjf "$(DL_DIR)/$(PKG_UPSTREAM_FILE)" --strip-components=1	;; \
	*.tar.gz|*.tgz)		tar xzf "$(DL_DIR)/$(PKG_UPSTREAM_FILE)" --strip-components=1	;; \
	*.bz2)			bunzip2 "$(DL_DIR)/$(PKG_UPSTREAM_FILE)"	;; \
	*.rar)			unrar x "$(DL_DIR)/$(PKG_UPSTREAM_FILE)"	;; \
	*.gz)			gunzip "$(DL_DIR)/$(PKG_UPSTREAM_FILE)"		;; \
	*.tar)			tar xf "$(DL_DIR)/$(PKG_UPSTREAM_FILE)"		;; \
	*.zip)			unzip "$(DL_DIR)/$(PKG_UPSTREAM_FILE)"		;; \
	*.Z)			uncompress "$(DL_DIR)/$(PKG_UPSTREAM_FILE)"	;; \
	*.7z)			7z x "$(DL_DIR)/$(PKG_UPSTREAM_FILE)"		;; \
	esac; \
	cd "$(1)"; \
	diff_txt="$$(diff --recursive --unified --new-file --no-dereference orig/ src/)"; \
	[ -n "$$diff_txt" ] && { \
		mkdir -p "$(1)/patches"; \
		echo "$$diff_txt" >"$(1)/patches/000_tlt.patch"; \
		rm -fr "$(1)/src" && mv "$(1)/orig" "$(1)/src"; \
	}; \
	rm -fr "$(1)/orig"; \
)
endef

define gpl_install_def
	$(if $(findstring $(GPL_INCLUDE_SRC),1), \
		$(call gpl_install_mixed,$(1)) \
	, \
		$(error Build/InstallGPL section is not defined! Please fix the package) \
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
	$(if $(findstring package/teltonika,$(current_dir)), \
		$(if $(and $(findstring $(GPL_INCLUDE_SRC),1),$(strip $(PKG_UPSTREAM_URL))), \
			$(call gpl_install_orig_w_patch,$(1)) \
		, \
			$(call gpl_install_def,$(1)) \
		) \
	, \
		$(if $(findstring feeds/vuci,$(current_dir)), \
			$(if $(CONFIG_GPL_INCLUDE_WEB_SOURCES), \
				$(call gpl_install_mixed,$(1)) \
			, \
				$(call gpl_install_def,$(1)) \
			) \
		, \
			$(if $(findstring feeds,$(current_dir)), \
				$(call gpl_install_feeds,$(1)) \
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
	$(if $(findstring $(GPL_INCLUDE_SRC),1), \
		$(call Build/InstallGPL,$(PKG_GPL_BUILD_DIR)) \
	, \
		$(if $(findstring package/teltonika,$(shell pwd)), \
			$(call gpl_install_closed,$(PKG_GPL_BUILD_DIR)) \
		, \
			$(if $(findstring feeds/vuci,$(shell pwd)), \
				$(if $(CONFIG_GPL_INCLUDE_WEB_SOURCES), \
					$(call Build/InstallGPL,$(PKG_GPL_BUILD_DIR)) \
				, \
					$(call gpl_install_closed,$(PKG_GPL_BUILD_DIR)) \
				) \
			, \
				$(call Build/InstallGPL,$(PKG_GPL_BUILD_DIR)) \
			) \
		) \
	)
	$(call gpl_scan_deps)
	$(call gpl_clear_install,$(PKG_GPL_BUILD_DIR))
