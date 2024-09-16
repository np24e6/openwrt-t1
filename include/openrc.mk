#
# Copyright (C) 2024 Teltonika-Networks
#

ifeq ($(CONFIG_USE_OPENRC),y)
OPENRC_RUNLEVELS:=sysinit boot default shutdown

OPENRC_INSTALL:= \
	OPENRC_RUNLEVELS="$(OPENRC_RUNLEVELS)" \
	INSTALL_BIN="$(INSTALL_BIN)" \
	INSTALL_DIR="$(INSTALL_DIR)" \
	LN="$(LN)" \
	/bin/sh -x $(TOPDIR)/scripts/openrc-install
endif
