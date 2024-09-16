LOC_DIR=sal/sd/linux/uk_interface
LIB=SAL

SRC_LIST :=

ifeq (TRUE, $(UK_IF))
ifeq (KSLIB, $(MODULE_TYPE))
  ifeq (TRUE, $(UK_NETLINK)) 
    SRC_LIST += sw_api_ks_netlink.c
  endif

  ifeq (TRUE, $(UK_IOCTL)) 
    SRC_LIST += sw_api_ks_ioctl.c
  endif
endif

ifeq (USLIB, $(MODULE_TYPE))
  ifeq (TRUE, $(UK_NETLINK)) 
    SRC_LIST += sw_api_us_netlink.c
  endif

  ifeq (TRUE, $(UK_IOCTL)) 
    SRC_LIST += sw_api_us_ioctl.c
  endif
endif
endif

SRCS += $(addprefix $(LOC_DIR)/, $(SRC_LIST))

