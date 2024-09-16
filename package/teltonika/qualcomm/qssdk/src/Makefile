MODULE_DIR:=$(abspath $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST))))))
include $(MODULE_DIR)/config
include $(MODULE_DIR)/make/linux_opt.mk

SRCS := 

include $(MODULE_DIR)/hsl/sources.mk
include $(MODULE_DIR)/sal/sources.mk
include $(MODULE_DIR)/init/sources.mk
include $(MODULE_DIR)/util/sources.mk
include $(MODULE_DIR)/ref/sources.mk
include $(MODULE_DIR)/shell_lib/sources.mk
ifeq (TRUE, $(FAL))
 include $(MODULE_DIR)/fal/sources.mk
 include $(MODULE_DIR)/adpt/sources.mk
endif

include $(MODULE_DIR)/api/sources.mk

OBJS = $(subst .c,.o, $(SRCS))

EXTRA_CFLAGS := $(MODULE_CFLAG)
EXTRA_CFLAGS += $(MODULE_INC)

qssdk-objs := $(OBJS)
obj-m := qssdk.o

