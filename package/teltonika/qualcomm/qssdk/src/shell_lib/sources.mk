LOC_DIR=shell_lib
LIB=SHELIB

SRC_LIST := shell.c \
	    shell_config.c \
	    shell_io.c \
	    shell_sw.c

SRCS += $(addprefix $(LOC_DIR)/, $(SRC_LIST))
