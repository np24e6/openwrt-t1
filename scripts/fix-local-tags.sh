#!/bin/bash

[[ $CI == "true" ]] && exit 0

parse_tags() {
	awk '{print $2 " " $1}' | sort
}

diff <(git show-ref --tags --dereference | parse_tags) <(git ls-remote --tags origin | parse_tags) &>/dev/null && exit 0

git fetch --tags --force &>/dev/null || echo -e "\x1B[35mWarning: failed to fetch remote tags! There is a chance FW version might be wrong\x1B[0m" >&2 &
