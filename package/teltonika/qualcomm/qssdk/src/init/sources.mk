LOC_DIR=init
LIB=INIT

SRC_LIST := ssdk_clk.c \
	    ssdk_init.c \
	    ssdk_interrupt.c \
	    ssdk_plat.c


SRC_SW_LIST=ssdk_uci.c

ifeq (TRUE, $(SWCONFIG))
	SRC_LIST += ssdk_uci.c
endif

SRCS += $(addprefix $(LOC_DIR)/, $(SRC_LIST))
