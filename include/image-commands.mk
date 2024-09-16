# Build commands that can be called from Device/* templates

IMAGE_KERNEL = $(word 1,$^)
IMAGE_ROOTFS = $(word 2,$^)

define Build/append-dtb
	cat $(KDIR)/image-$(firstword $(DEVICE_DTS)).dtb >> $@
endef

define Build/append-kernel
	dd if=$(IMAGE_KERNEL) >> $@
endef

compat_version=$(if $(DEVICE_COMPAT_VERSION),$(DEVICE_COMPAT_VERSION),1.0)
json_quote=$(subst ','\'',$(subst ",\",$(1)))
#")')

legacy_supported_message=$(SUPPORTED_DEVICES) - Image version mismatch: image $(compat_version), \
	device 1.0. Please wipe config during upgrade (force required) or reinstall. \
	$(if $(DEVICE_COMPAT_MESSAGE),Reason: $(DEVICE_COMPAT_MESSAGE),Please check documentation ...)

metadata_devices=$(if $(1),$(subst "$(space)","$(comma)",$(strip $(foreach v,$(1),"$(call json_quote,$(v))"))))
metadata_json = \
	'{ $(if $(IMAGE_METADATA),$(IMAGE_METADATA)$(comma)) \
		"metadata_version": "1.1", \
		"compat_version": "$(call json_quote,$(compat_version))", \
		"version":"$(call json_quote,$(TLT_VERSION))", \
		"device_code": [$(DEVICE_COMPAT_CODE)], \
		"hwver": [".*"], \
		"batch": [".*"], \
		"serial": [".*"], \
		$(if $(DEVICE_COMPAT_MESSAGE),"compat_message": "$(call json_quote,$(DEVICE_COMPAT_MESSAGE))"$(comma)) \
		$(if $(filter-out 1.0,$(compat_version)),"new_supported_devices": \
			[$(call metadata_devices,$(SUPPORTED_DEVICES))]$(comma) \
			"supported_devices": ["$(call json_quote,$(legacy_supported_message))"]$(comma)) \
		$(if $(filter 1.0,$(compat_version)),"supported_devices":[$(call metadata_devices,$(SUPPORTED_DEVICES))]$(comma)) \
		"version_wrt": { \
			"dist": "$(call json_quote,$(VERSION_DIST))", \
			"version": "$(call json_quote,$(VERSION_NUMBER))", \
			"revision": "$(call json_quote,$(REVISION))", \
			"target": "$(call json_quote,$(TARGETID))", \
			"board": "$(call json_quote,$(if $(BOARD_NAME),$(BOARD_NAME),$(DEVICE_NAME)))" \
		}, \
		$(subst $(comma)},},"hw_support": { \
			$(foreach data, \
				$(HW_SUPPORT), \
					"$(firstword $(subst %,": ,$(data))) \
					["$(subst :," $(comma)",$(lastword $(subst %,": ,$(data))))"],)},) \
		$(subst $(comma)},},"hw_mods": { \
			$(foreach data, \
				$(HW_MODS), \
					"$(firstword $(subst %,": ,$(data))) \
					"$(subst :," $(comma)",$(lastword $(subst %,": ,$(data))))",)}) \
	}'

version_json = '{"version":"$(call json_quote,$(TLT_VERSION))"}'

define Build/append-metadata
	$(if $(SUPPORTED_DEVICES),-echo $(call metadata_json) | fwtool -I - $@)
	[ -z "$(CONFIG_SIGNED_IMAGES)" -o ! -s "$(BUILD_KEY)" -o ! -s "$(BUILD_KEY).ucert" -o ! -s "$@" ] || { \
		cp "$(BUILD_KEY).ucert" "$@.ucert" ;\
		usign -S -m "$@" -s "$(BUILD_KEY)" -x "$@.sig" ;\
		ucert -A -c "$@.ucert" -x "$@.sig" ;\
		fwtool -S "$@.ucert" "$@" ;\
	}
endef

define Build/append-version
	echo $(call version_json) | fwtool -I - $@
endef

define Build/append-rootfs
	dd if=$(IMAGE_ROOTFS) >> $@
endef

define Build/append-ubi
	sh $(TOPDIR)/scripts/ubinize-image.sh \
		$(if $(UBOOTENV_IN_UBI),--uboot-env) \
		$(if $(KERNEL_IN_UBI),--kernel $(IMAGE_KERNEL)) \
		$(foreach part,$(UBINIZE_PARTS),--part $(part)) \
		$(IMAGE_ROOTFS) \
		$@.tmp \
		-p $(BLOCKSIZE:%k=%KiB) -m $(PAGESIZE) \
		$(if $(SUBPAGESIZE),-s $(SUBPAGESIZE)) \
		$(if $(VID_HDR_OFFSET),-O $(VID_HDR_OFFSET)) \
		$(UBINIZE_OPTS)
	cat $@.tmp >> $@
	rm $@.tmp
endef

define Build/append-uboot
	dd if=$(UBOOT_PATH) >> $@
endef

define Build/check-size
	@imagesize="$$(stat -c%s $@)"; \
	limitsize="$$(($(subst k,* 1024,$(subst m, * 1024k,$(if $(1),$(1),$(IMAGE_SIZE))))))"; \
	[ $$limitsize -ge $$imagesize ] || { \
		echo "WARNING: Image file $@ is too big: $$imagesize > $$limitsize" >&2; \
		rm -f $@; \
	}
endef

define Build/fit
	$(TOPDIR)/scripts/mkits.sh \
		-D $(DEVICE_NAME) -o $@.its -k $@ \
		$(if $(word 2,$(1)),-d $(word 2,$(1))) -C $(word 1,$(1)) \
		-a $(KERNEL_LOADADDR) -e $(if $(KERNEL_ENTRY),$(KERNEL_ENTRY),$(KERNEL_LOADADDR)) \
		$(if $(DEVICE_FDT_NUM),-n $(DEVICE_FDT_NUM)) \
		-c $(if $(DEVICE_DTS_CONFIG),$(DEVICE_DTS_CONFIG),"config@1") \
		-A $(LINUX_KARCH) -v $(LINUX_VERSION)
	PATH=$(LINUX_DIR)/scripts/dtc:$(PATH) mkimage -f $@.its $@.new
	@mv $@.new $@
endef

define Build/gzip
	gzip -f -9n -c $@ $(1) > $@.new
	@mv $@.new $@
endef

define Build/finalize-tlt-custom
	[ -d $(BIN_DIR)/tltFws ] || mkdir -p $(BIN_DIR)/tltFws
	$(CP) $@ $(BIN_DIR)/tltFws/$(TLT_VERSION_FILE)$(if $(1),_$(word 1,$(1)))$(if $(findstring 1,$(FAKE_RELEASE_BUILD)),_FAKE).$(if $(word 2,$(1)),$(word 2,$(1)),bin)
	echo "Copying $@ to tltFws"
	echo  $(BIN_DIR)/tltFws/$(TLT_VERSION_FILE)$(if $(1),_$(word 1,$(1)))$(if $(findstring 1,$(FAKE_RELEASE_BUILD)),_FAKE).$(if $(word 2,$(1)),$(word 2,$(1)),bin) > /tmp/last_built.fw
endef

define Build/finalize-tlt-webui
	$(call Build/finalize-tlt-custom,WEBUI bin)
endef

define Build/finalize-tlt-master-stendui
	[ -d $(BIN_DIR)/tltFws ] || mkdir -p $(BIN_DIR)/tltFws

	$(eval UBOOT_INSERTION=$(shell cat ${BIN_DIR}/u-boot_version))
	$(if $(UBOOT_INSERTION), $(eval UBOOT_INSERTION=_UBOOT_$(UBOOT_INSERTION)))
	$(CP) $@ $(BIN_DIR)/tltFws/$(TLT_VERSION_FILE)$(UBOOT_INSERTION)_MASTER_STENDUI$(word 1,$(1))$(if $(findstring 1,$(FAKE_RELEASE_BUILD)),_FAKE).bin
endef

define Build/kernel-bin
	rm -f $@
	cp $< $@
endef

define Build/lzma
	$(call Build/lzma-no-dict,-lc1 -lp2 -pb2 $(1))
endef

define Build/lzma-no-dict
	$(STAGING_DIR_HOST)/bin/lzma e $@ $(1) $@.new
	@mv $@.new $@
endef

define Build/pad-extra
	dd if=/dev/zero bs=$(1) count=1 >> $@
	echo "padding $@ with $(1) zeros"
endef

define Build/pad-offset
	let \
		size="$$(stat -c%s $@)" \
		pad="$(subst k,* 1024,$(word 1, $(1)))" \
		offset="$(subst k,* 1024,$(word 2, $(1)))" \
		pad="(pad - ((size + offset) % pad)) % pad" \
		newsize='size + pad'; \
		dd if=$@ of=$@.new bs=$$newsize count=1 conv=sync
	mv $@.new $@
endef

define Build/pad-rootfs
	$(STAGING_DIR_HOST)/bin/padjffs2 $@ $(1) \
		$(if $(BLOCKSIZE),$(BLOCKSIZE:%k=%),4 8 16 64 128 256)
endef

define Build/pad-to
	$(call Image/pad-to,$@,$(1))
endef

# Convert a raw image into a $1 type image.
# E.g. | qemu-image vdi
define Build/qemu-image
	if command -v qemu-img; then \
		qemu-img convert -f raw -O $1 $@ $@.new; \
		mv $@.new $@; \
	else \
		echo "WARNING: Install qemu-img to create VDI/VMDK images" >&2; exit 1; \
	fi
endef

define Build/sysupgrade-tar
	sh $(TOPDIR)/scripts/sysupgrade-tar.sh \
		--board $(if $(BOARD_NAME),$(BOARD_NAME),$(DEVICE_NAME)) \
		--kernel $(call param_get_default,kernel,$(1),$(IMAGE_KERNEL)) \
		--rootfs $(call param_get_default,rootfs,$(1),$(IMAGE_ROOTFS)) \
		$@
endef

define Build/uImage
	mkimage \
		-A $(LINUX_KARCH) \
		-O linux \
		-T kernel \
		-C $(word 1,$(1)) \
		-a $(KERNEL_LOADADDR) \
		-e $(if $(KERNEL_ENTRY),$(KERNEL_ENTRY),$(KERNEL_LOADADDR)) \
		-n '$(if $(UIMAGE_NAME),$(UIMAGE_NAME),$(call toupper,$(LINUX_KARCH)) $(VERSION_DIST) Linux-$(LINUX_VERSION))' \
		$(if $(UIMAGE_MAGIC),-M $(UIMAGE_MAGIC)) \
		$(wordlist 2,$(words $(1)),$(1)) \
		-d $@ $@.new
	mv $@.new $@
endef
