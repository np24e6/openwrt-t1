LOC_DIR=adpt
LIB=ADPT

SRC_LIST := adpt.c

SRCS += $(addprefix $(LOC_DIR)/, $(SRC_LIST))

include $(MODULE_DIR)/$(LOC_DIR)/hppe/sources.mk

