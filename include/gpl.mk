#
# Copyright (C) 2023 Teltonika-Networks
#

define GPL/Build/Default
	cp -rf . "$(1)/"
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
	)
endef

GPL/Build=$(call GPL/Build/Default,$(1))

gpl:
	$(call GPL/Build,$(1))
