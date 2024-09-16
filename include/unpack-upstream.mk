# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2006-2020 OpenWrt.org

UPSTREAM_HOST_TAR:=$(TAR)
UPSTREAM_TAR_CMD=$(UPSTREAM_HOST_TAR) -C $(1)/ --strip-components=$(if $(STRIP_COMPONENTS),$(STRIP_COMPONENTS),1) $(TAR_OPTIONS)
UPSTREAM_UNZIP_CMD=unzip -q -d $(1)/ $(DL_DIR)/upstream/$(PKG_UPSTREAM_FILE)

ifeq ($(PKG_UPSTREAM_FILE),)
  UPSTREAM_PKG_UNPACK ?= true
else

ifeq ($(strip $(UPSTREAM_UNPACK_CMD)),)
  ifeq ($(strip $(PKG_CAT)),)
    # try to autodetect file type
    EXT:=$(call ext,$(PKG_UPSTREAM_FILE))
    EXT1:=$(EXT)

    ifeq ($(filter gz tgz,$(EXT)),$(EXT))
      EXT:=$(call ext,$(PKG_UPSTREAM_FILE:.$(EXT)=))
      UPSTREAM_DECOMPRESS_CMD:=gzip -dc $(DL_DIR)/upstream/$(PKG_UPSTREAM_FILE) |
    endif
    ifeq ($(filter bzip2 bz2 bz tbz2 tbz,$(EXT)),$(EXT))
      EXT:=$(call ext,$(PKG_UPSTREAM_FILE:.$(EXT)=))
      UPSTREAM_DECOMPRESS_CMD:=bzcat $(DL_DIR)/upstream/$(PKG_UPSTREAM_FILE) |
    endif
    ifeq ($(filter xz txz,$(EXT)),$(EXT))
      EXT:=$(call ext,$(PKG_UPSTREAM_FILE:.$(EXT)=))
      UPSTREAM_DECOMPRESS_CMD:=xzcat $(DL_DIR)/upstream/$(PKG_UPSTREAM_FILE) |
    endif
    ifeq (zst,$(EXT))
      EXT:=$(call ext,$(PKG_UPSTREAM_FILE:.$(EXT)=))
      UPSTREAM_DECOMPRESS_CMD:=zstdcat $(DL_DIR)/upstream/$(PKG_UPSTREAM_FILE) |
    endif
    ifeq ($(filter tgz tbz tbz2 txz,$(EXT1)),$(EXT1))
      EXT:=tar
    endif
    UPSTREAM_DECOMPRESS_CMD ?= cat $(DL_DIR)/upstream/$(PKG_UPSTREAM_FILE) |
    ifeq ($(EXT),tar)
      UPSTREAM_UNPACK_CMD=$(UPSTREAM_DECOMPRESS_CMD) $(UPSTREAM_TAR_CMD)
    endif
    ifeq ($(EXT),cpio)
      UPSTREAM_UNPACK_CMD=$(UPSTREAM_DECOMPRESS_CMD) (cd $(1)/..; cpio -i -d)
    endif
    ifeq ($(EXT),zip)
      UPSTREAM_UNPACK_CMD=$(UPSTREAM_UNZIP_CMD)
    endif
  endif

  # compatibility code for packages that set PKG_CAT
  ifeq ($(strip $(UPSTREAM_UNPACK_CMD)),)
    # use existing PKG_CAT
    UPSTREAM_UNPACK_CMD=$(PKG_CAT) $(DL_DIR)/upstream/$(PKG_UPSTREAM_FILE) | $(UPSTREAM_TAR_CMD)
    ifeq ($(PKG_CAT),unzip)
      UPSTREAM_UNPACK_CMD=$(UPSTREAM_UNZIP_CMD)
    endif
    # replace zcat with $(ZCAT), because some system don't support it properly
    ifeq ($(PKG_CAT),zcat)
      UPSTREAM_UNPACK_CMD=gzip -dc $(DL_DIR)/upstream/$(PKG_UPSTREAM_FILE) | $(UPSTREAM_TAR_CMD)
    endif
  endif
endif

endif # PKG_UPSTREAM_FILE
