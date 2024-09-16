LOC_DIR=sal/sd
LIB=SAL

SRC_LIST := sd.c
SRCS += $(addprefix $(LOC_DIR)/, $(SRC_LIST))

include $(MODULE_DIR)/$(LOC_DIR)/linux/sources.mk

