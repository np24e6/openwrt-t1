#!/usr/bin/env bash

# colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

OUT_FD=${OUT_FD:-1}
PKG_INFO_CACHE=tmp/.pkg-rebuilt.cached
PKG_INFO_FILE=${PKG_INFO_FILE:=tmp/.pkg-rebuilt.done}
BUILD_CONF_FILE=${BUILD_CONF_FILE:=.config}

if [ -f "$PKG_INFO_CACHE" ]; then
	cat "$PKG_INFO_CACHE" >&"$OUT_FD"
	exit 0
fi

if [ ! -f "$PKG_INFO_FILE" ] || [ ! -f "$BUILD_CONF_FILE" ]; then
	echo -ne "${RED}Unable to find configuration files:${NC}"
	[ ! -f "$PKG_INFO_FILE" ] && echo -ne " $PKG_INFO_FILE"
	[ ! -f "$BUILD_CONF_FILE" ] && echo -ne " $BUILD_CONF_FILE"
	echo -e "${NC}"
	exit 1
fi

get_user() {
	echo "$(whoami)@$(hostname)"
}

get_machine() {
	local distro=""

	if command -v lsb_release &>/dev/null; then
		distro="$(lsb_release -rd | awk -F':\t' '{print $2}' | paste -sd' ' -)"
	fi

	if [ -n "$distro" ]; then
		echo -n "$distro @"
	fi

	uname -rs
}

get_buildsys() {
	if [ -n "$INSIDE_DOCKER" ] && [ "$INSIDE_DOCKER" -eq 1 ]; then
		echo "Docker"
	else
		echo "Native"
	fi
}

get_target() {
	grep CONFIG_TARGET_PROFILE "$BUILD_CONF_FILE" | sed 's/.*="\(.*\)"/\1/'
}

PKG_NUM=$(wc -l <"$PKG_INFO_FILE")
PKG_MAX=$(grep -cE "^CONFIG_PACKAGE_.*=y" "$BUILD_CONF_FILE")

diff=$(($(stat -c %Y "$PKG_INFO_FILE") - $(stat -c %W "$PKG_INFO_FILE")))
PKG_BUILD_TIME=$(printf "%dm%ds\n" $((diff / 60)) $((diff % 60)))

diff=$(($(date +%s) - $(stat -c %W "$PKG_INFO_FILE")))
TOTAL_BUILD_TIME=$(printf "%dm%ds\n" $((diff / 60)) $((diff % 60)))

echo -ne "\
 ${CYAN}Build summary:${NC}
 \t${YELLOW}• User${NC}     ${GREEN}$(get_user)${NC}
 \t${YELLOW}• Machine${NC}  ${GREEN}$(get_machine)${NC}
 \t${YELLOW}• System${NC}   ${GREEN}$(get_buildsys)${NC}
 \t${YELLOW}• Target${NC}   ${GREEN}$(get_target)${NC}
 \t${YELLOW}• Built${NC}    ${GREEN}${PKG_NUM} out of ${PKG_MAX} packages${NC} in ${BLUE}${PKG_BUILD_TIME}${NC}
 \t${YELLOW}• Finished${NC} in ${BLUE}${TOTAL_BUILD_TIME}${NC}
\n" >"$PKG_INFO_CACHE"

cat "$PKG_INFO_CACHE" >&"$OUT_FD"
