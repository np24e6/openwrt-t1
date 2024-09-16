#
# Copyright (C) 2021 Teltonika
#

include rules.mk

PKG_LIST ?= $(strip $(foreach m,$(shell jq "keys | .[]" --raw-output "$(TOPDIR)/ipk_packages.json"),$(if $(filter m,$(CONFIG_PACKAGE_$(m))),$(m),)))

ifeq ($(PKG_LIST),)

define PM/Build/Clean
	true
endef
define PM/Build/Prepare
	true
endef
define PM/Build/Sign_ipk
	true
endef
define PM/Build
	true
endef
define PM/Build/Sign_index
	true
endef

else # $(PKG_LIST)

PM_DIR:=$(OUTPUT_DIR)/packages/$(ARCH_PACKAGES)/pm_packages
PM_ZIPPED_DIR:=$(OUTPUT_DIR)/packages/$(ARCH_PACKAGES)/zipped_packages

SIGN_FILE_LIST?=$(TMP_DIR)/pm/.sign-file-list

define PM/Build/Clean
	rm -fr "$(PM_DIR)" "$(PM_ZIPPED_DIR)" "$(TMP_DIR)/pm"
endef

define PM/Build/Prepare
	mkdir -p "$(PM_DIR)" && \
	"$(SCRIPT_DIR)/prepare_tlt_pm_packages.sh" -u "$(SIGN_FILE_LIST)" "$(PKG_LIST)" "$(TOPDIR)" "$(ARCH_PACKAGES)" "$(PACKAGE_DIR)"
endef

define PM/Build/Sign_ipk
	"$(SCRIPT_DIR)/sign_packages.sh" "$(SIGN_FILE_LIST)"
endef

define PM/Build
	mkdir -p "$(PM_ZIPPED_DIR)" && \
	"$(SCRIPT_DIR)/prepare_tlt_pm_packages.sh" -p "$(PKG_LIST)" "$(TOPDIR)" "$(ARCH_PACKAGES)" "$(PACKAGE_DIR)" && \
	cd "$(PM_DIR)" && \
	{ "$(SCRIPT_DIR)/ipkg-make-index.sh" . >Packages.manifest; } 2>&1 && \
	grep -vE '^(Maintainer|LicenseFiles|Source|SourceName|Require)' Packages.manifest >Packages && \
	gzip -9nc Packages >Packages.gz
endef

define PM/Build/Sign_index
	"$(STAGING_DIR_HOST)/bin/usign" -S -m "$(PM_DIR)/Packages" -s "$(BUILD_KEY)" -x "$(PM_DIR)/Packages.sig"
endef

endif # $(PKG_LIST)
