LOC_DIR=ref
LIB=REF

SRC_LIST := ref_fdb.c \
	    ref_mib.c \
	    ref_port_ctrl.c \
	    ref_vlan.c \
	    ref_vsi.c 

ifeq (TRUE, $(SWCONFIG))
        SRC_LIST += ref_misc.c ref_uci.c
endif

SRCS += $(addprefix $(LOC_DIR)/, $(SRC_LIST))
