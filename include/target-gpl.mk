#
# Copyright (C) 2021 Teltonika
#

define GPL/Target/Build/Default
	cp -rf . "$(1)/"

	subtargets=$$(grep "SUBTARGETS:=" "$(1)/Makefile" | cut -d '=' -f2-); \
	for sub in $$subtargets; do \
		rm "$(1)/image/$${sub}.mk"; \
		[ -d "$(1)/$${sub}" ] && rm -rf "$(1)/$${sub}" || true; \
	done; \
	sub=$(CONFIG_TARGET_SUBTARGET); \
	sed -i '/^SUBTARGETS:=.*/c\SUBTARGETS:='"$${sub}" "$(1)/Makefile"; \
	cp "./image/$${sub}.mk" "$(1)/image/$${sub}.mk"; \
	[ -d "$${sub}" ] && cp -r "$${sub}" "$(1)" || true; \
	sed -i '/^TARGET_DEVICES +=/d' "$(1)/image/$${sub}.mk"; \
	echo "TARGET_DEVICES += teltonika_$(call device_shortname)" >> "$(1)/image/$${sub}.mk"

	sed -i '/target-gpl.mk/d' "$(1)/Makefile"
	sed -i '/define GPL\/Target\/Build/,/endef/d' "$(1)/Makefile"
endef

GPL/Target/Build=$(call GPL/Target/Build/Default,$(1))

$(BOARD)/gpl:
	$(call GPL/Target/Build,$(1))
