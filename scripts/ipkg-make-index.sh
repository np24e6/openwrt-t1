#!/usr/bin/env bash
set -e

pkg_dir=$1

if [[ -z $pkg_dir || ! -d $pkg_dir ]]; then
	echo "Usage: ipkg-make-index <package_directory>" >&2
	exit 1
fi

for pkg in $(find "$pkg_dir" -name '*.ipk' | sort); do
	name="${pkg##*/}"
	name="${name%%_*}"
	[[ "$name" = "kernel" ]] && continue
	[[ "$name" = "libc" ]] && continue
	echo "Generating index for package $pkg" >&2
	file_size=$(stat -L -c%s "$pkg")
	sha256sum=$(openssl dgst -sha256 "$pkg" 2>/dev/null | awk '{print $2}')
	# Take pains to make variable value sed-safe
	sed_safe_pkg=$(echo "$pkg" | sed -e 's/^\.\///g' -e 's/\//\\\//g')
	tar -xzOf "$pkg" ./control.tar.gz | tar xzOf - ./control | sed -e "s/^Description:/Filename: $sed_safe_pkg\\
Size: $file_size\\
SHA256sum: $sha256sum\\
Description:/"
	echo ""
done
echo
