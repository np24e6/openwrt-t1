#!/bin/bash

shopt -s globstar nullglob

# directory where to search for packages
TOP_PKG_DIR="${TOP_DIR:-./bin}"
# directory where to search for images
TOP_DIR="${TOP_DIR:-./bin/targets}"
# key to sign images
BUILD_KEY="${BUILD_KEY:-key-build}" # TODO unify naming?
# remove other signatures (added e.g.  by buildbot)
REMOVE_OTHER_SIGNATURES="${REMOVE_OTER_SIGNATURES:-1}"

# find all *_WEBUI.bin images in TOP_DIR
for image in "${TOP_DIR}"/**/*_WEBUI.bin; do
	# check if image actually support metadata
	fwtool -i /dev/null "$image" || continue

	# remove all previous signatures
	[ -z "$REMOVE_OTHER_SIGNATURES" ] || while true; do
		fwtool -t -s /dev/null "$image" || break
	done

	# run same operation as build root does for signing
	cp "$BUILD_KEY.ucert" "$image.ucert"
	usign -S -m "$image" -s "$BUILD_KEY" -x "$image.sig"
	ucert -A -c "$image.ucert" -x "$image.sig"
	fwtool -S "$image.ucert" "$image"

	/usr/bin/rm "$image.sig" "$image.ucert"
done

# find all Packages files in TOP_PKG_DIR
for pkg in "$TOP_PKG_DIR"/**/Packages; do
	usign -S -m "$pkg" -s "$BUILD_KEY"
done
