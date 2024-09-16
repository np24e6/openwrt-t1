#
# Copyright (C) 2024 Teltonika-Networks
#

# ignore on prereq or tmp generation
ifeq ($(or $(findstring prereq,$(MAKECMDGOALS)),$(findstring tmp/info/.files,$(MAKECMDGOALS))),)
ifndef TLT_VERSION

TLT_VERSION:=$(shell $(TOPDIR)/scripts/get_tlt_version.sh)
TLT_VERSION_FILE:=$(shell $(TOPDIR)/scripts/get_tlt_version.sh --file)

export TLT_VERSION
export TLT_VERSION_FILE

endif # TLT_VERSION
endif # prereq, tmp/info/.files