#!/bin/sh

BOOTSTRAP="./scripts/docker-bootstrap"

if [ "${FORCE_NATIVE_BUILD:-1}" = "1" ] || [ -n "$INSIDE_DOCKER" ] || ! "$BOOTSTRAP" true; then
	$*
else
	#shellcheck disable=2086
	"$BOOTSTRAP" $*
fi
