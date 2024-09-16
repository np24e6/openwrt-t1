#!/bin/sh
BUILD_DIR="$(pwd)/build_dir"
TC_BUILD_VER_FILE="$BUILD_DIR/.toolchain_build_ver"

mkdir -p "$BUILD_DIR"
eval "$(grep CONFIG_GCC_VERSION .config)"
CONFIG_TOOLCHAIN_BUILD_VER="$CONFIG_GCC_VERSION-$(cat toolchain/build_version)"
touch "$TC_BUILD_VER_FILE"
[ "$CONFIG_TOOLCHAIN_BUILD_VER" = "$(cat "$TC_BUILD_VER_FILE")" ] && exit 0
echo "Toolchain build version changed, running make targetclean"
make targetclean
echo "$CONFIG_TOOLCHAIN_BUILD_VER" > "$TC_BUILD_VER_FILE"
exit 0
