#!/usr/bin/env bash
#
# Copyright (C) 2024 Teltonika-Networks
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
SELF=${0##*/}

[ -z "$COMPRESSOR" ] && {
  echo "$SELF: compressor command not defined (COMPRESSOR variable not set)"
  exit 1
}

TARGETS=$*

[ -z "$TARGETS" ] && {
  echo "$SELF: no directories / files specified"
  echo "usage: $SELF [PATH...]"
  exit 1
}

find $TARGETS -type f -a -exec file {} \; | \
  sed -n -e "s/^\(.*\):.*ELF.*\(executable\).*,.*/\1:\2/p" | \
(
  IFS=":"
  while read F S; do
	echo "$SELF: $F: $S"
	b=$(stat -c '%a' $F)
	eval "$COMPRESSOR $COMPRESS_OPTIONS $F"
	a=$(stat -c '%a' $F)
	[ "$a" = "$b" ] || chmod $b $F
  done
  true
)
