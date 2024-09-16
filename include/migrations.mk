#
# Copyright (C) 2023 Teltonika-Networks
#

# copy device-related directory with files
# 1 => source directory
# 2 => destination directory
define install_device_files
[ -d "$(1)" ] && $(CP) $(1)/* $(2) || true
endef

# install files based on initial supported version
# if device does not have this option, fallback "0.0" version is used instead
# 1 => source directory
# 2 => destination directory
define install_version_files
(\
	imajor="$$$$(echo $(CONFIG_INITIAL_SUPPORT_VERSION) | awk -F . '{print $$$$1 }')"; \
	iminor="$$$$(echo $(CONFIG_INITIAL_SUPPORT_VERSION) | awk -F . '{print $$$$2 }')"; \
	ipatch="$$$$(echo $(CONFIG_INITIAL_SUPPORT_VERSION) | awk -F . '{print $$$$3 }')"; \
	imajor="$$$${imajor:-0}"; \
	iminor="$$$${iminor:-0}"; \
	ipatch="$$$${ipatch:-0}"; \
	dirs="$$$$(find $(1) -mindepth 1 -maxdepth 1 -type d -printf "%f\n")"; \
	for d in $$$$dirs; do \
		dmajor="$$$$(echo $$$$d | awk -F . '{print $$$$1 }')"; \
		dminor="$$$$(echo $$$$d | awk -F . '{print $$$$2 }')"; \
		dpatch="$$$$(echo $$$$d | awk -F . '{print $$$$3 }')"; \
		dpatch="$$$${dpatch:-0}"; \
		[ "$$$$dmajor" -eq "$$$$dmajor" ] 2>/dev/null || continue; \
		[ "$$$$dminor" -eq "$$$$dminor" ] 2>/dev/null || continue; \
		[ "$$$$dmajor" -lt "$$$$imajor" ] && continue; \
		[ "$$$$dmajor" -eq "$$$$imajor" ] && \
			[ "$$$$dminor" -lt "$$$$iminor" ] && continue; \
		[ "$$$$dmajor" -eq "$$$$imajor" ] && \
			[ "$$$$dminor" -eq "$$$$iminor" ] && \
			[ "$$$$dpatch" -lt "$$$$ipatch" ] && continue; \
		$(INSTALL_DIR) "$(2)/etc/uci-defaults/$$$$d"; \
		find "$(1)/$$$$d" -maxdepth 1 -type f -exec $(CP) {} "$(2)/etc/uci-defaults/$$$$d" \;; \
		$(call install_device_files,files/$$$$d/$(call device_shortname),$(2)/etc/uci-defaults/$$$$d); \
	done; \
)
endef

# install migration scripts based on initial supported version, supported
#  devices and unspecified (always installed) scripts
# 1 => source directory
# 2 => destination directory
define install_migrations
	$(call install_version_files,$(1),$(2))

	$(if $(CONFIG_INITIAL_SUPPORT_VERSION),, \
		$(INSTALL_DIR) $(2)/etc/uci-defaults/old; \
		find $(1)/old -maxdepth 1 -type f -exec $(CP) {} $(2)/etc/uci-defaults/old \;; \
		$(call install_device_files,$(1)/old/$(call device_shortname),$(2)/etc/uci-defaults/old); \
		$(if $(or $(if $(CONFIG_MOBILE_SUPPORT),,1),$(CONFIG_BASEBAND_SUPPORT)), \
			$(RM) $(2)/etc/uci-defaults/old/05_system))

	$(INSTALL_DIR) $(2)/etc/uci-defaults/etc
	find $(1)/etc -maxdepth 1 -type f -exec $(CP) {} $(2)/etc/uci-defaults/etc \;
	$(call install_device_files,$(1)/etc/$(call device_shortname),$(2)/etc/uci-defaults/etc)
endef
