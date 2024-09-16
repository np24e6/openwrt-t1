#
# Copyright (C) 2024 Teltonika-Networks
#

# ignore on prereq or tmp generation
ifeq ($(or $(findstring prereq,$(MAKECMDGOALS)),$(findstring tmp/info/.files,$(MAKECMDGOALS))),)
ifndef TLT_VERSION

ifdef __TLT_VERSION
TLT_VERSION:=$(__TLT_VERSION)
else
TLT_VERSION:=$(shell $(TOPDIR)/scripts/get_tlt_version.sh)
endif

ifdef __TLT_VERSION_FILE
TLT_VERSION_FILE:=$(__TLT_VERSION_FILE)
else
TLT_VERSION_FILE:=$(shell $(TOPDIR)/scripts/get_tlt_version.sh --file)
endif

export TLT_VERSION
export TLT_VERSION_FILE

endif # TLT_VERSION
endif # prereq, tmp/info/.files
