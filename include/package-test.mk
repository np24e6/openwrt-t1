#
# Copyright (C) 2022 Teltonika
#

# When verbose is turned off, FD 8 is open and printed to console, redirect to it instead of stdout
_FD := $(if $(shell 2>&1 >&8),1,8)
_v := $(if $(shell 2>&1 >&8),4,3)

define Test/unit_test
	$(eval TEST_FILE ?= all)
	$(eval TARGET_PORT ?= 22)
	set -ae;									\
	grep "='" "$(TMP_DIR)/.tc" || sed -r -i "s|=([^;]*)|='\1'|g" "$(TMP_DIR)/.tc";	\
	. "$(TMP_DIR)/.tc";								\
	cd "$(PKG_BUILD_DIR)" && {							\
		TARGET_IP="$(TARGET_IP)"						\
		TARGET_PORT="$(TARGET_PORT)"						\
		ceedling verbosity[$(_v)] "gcov:$(TEST_FILE)" "utils:gcov" >&$(_FD) 2>&1; \
		cd -;									\
	}
endef

define Test/unit_test_clean
	set -ae;									\
	grep "='" "$(TMP_DIR)/.tc" || sed -r -i "s|=([^;]*)|='\1'|g" "$(TMP_DIR)/.tc";	\
	. "$(TMP_DIR)/.tc";								\
	cd "$(PKG_BUILD_DIR)" && { 							\
		ceedling verbosity[$(_v)] clean >&$(_FD) 2>&1;				\
		rm -r ./build/test/{cache,preprocess} 2>/dev/null || true;		\
		cd -;									\
	}
endef
