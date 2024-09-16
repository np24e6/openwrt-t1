#!/bin/bash

print_help() {
	printf "Usage: %s TARGET_IP TARGET_PORT PACKAGES TEST_FILES [FD]
	
	PACKAGES \t List of packages to test, separated by space
	TEST_FILES \t List of test files for each package to run, separated by space. Separate multiple tests for the same package by comma. Can be an empty string
	FD \t\t (optional) File descriptor to redirect output of this script to. Default: %d
" "$0" "$1" >&"$1"
}

# Relevant only when building in GitLab
device_ip_map() {
	device=$(grep -om1 "DEVICE_t.*=y$" .config)
	device=${device##D*_}
	device=${device%=y}

	case "$device" in
	rutx)
		TARGET_IP="192.168.1.1"
		;;
	rut9xx)
		TARGET_IP="192.168.1.1"
		;;
	*)
		echo "Unsupported device: $device" >&"$FD"
		exit 1
		;;
	esac
}

TARGET_IP=${1}
TARGET_PORT=${2:-22}
PACKAGES=${3}
TEST_FILES=${4}
FD="${5:-1}"

FAIL=0

declare -A available_tests
declare -A tests_to_run

SSH_PRIVATE_KEY="${TEST_SSH_PRIVATE_KEY:-$(cat "$HOME/.ssh/id_rsa")}"

# GitLab
[ -n "$TEST_SSH_PRIVATE_KEY" ] && {
	device_ip_map

	eval "$(ssh-agent -s)"
	trap "ssh-agent -k" EXIT

	ssh-add <(echo "${SSH_PRIVATE_KEY}")
	mkdir -p "$HOME/.ssh"
	[ -n "$SSH_HOST_KEY" ] && echo "${SSH_HOST_KEY}" >>"$HOME/.ssh/known_hosts"
}

[ -z "$TARGET_IP" ] && {
	echo "TARGET_IP must be exported to the environment!" >&"$FD"
	print_help "$FD"
	exit 1
}

[ -z "$PACKAGES" ] && {
	# Fallback to packages' Makefiles that differ from develop branch
	# shellcheck disable=SC2046 # Word splitting of '$(dirname...)' is intended here
	PACKAGES=$(basename -a $(dirname $(git diff --name-only "origin/develop..$(git rev-parse HEAD)" | grep Makefile | tr '\n' ' ')))
}

[ "$PACKAGES" = "all" ] && {
	# shellcheck disable=SC2046 # Word splitting of '$(find...' is intended here
	PACKAGES="$(basename -a $(find package/ -name "Makefile" -type f -exec dirname "{}" \;))"
}

readarray -t requested_packages <<<"$PACKAGES"
readarray -t requested_test_files <<<"$TEST_FILES"

# If TEST_FILES is specified, its length must be equal to PACKAGES length
[ -n "$TEST_FILES" ] && [ ${#requested_packages[@]} -ne ${#requested_test_files[@]} ] && {
	echo "Error: Must specify tests to run (or 'all') for each package" >&"$FD"
	print_help "$FD"
	exit 1
}

for i in "${!requested_packages[@]}"; do
	if [ -n "${requested_test_files[$i]}" ]; then
		tests_to_run[${requested_packages[$i]}]=$(tr ',' ' ' <<<"${requested_test_files[$i]}")
	else
		tests_to_run[${requested_packages[$i]}]='all'
	fi
done

for pkg in "${!tests_to_run[@]}"; do
	pkg_Makefile="$(find package/ -name "${pkg}" -type d)/Makefile"
	[ -f "$pkg_Makefile" ] || continue

	printf "checking tests for %-30s " "${pkg}..." >&"$FD"

	tests=''
	for test in $(tr '\n' ' ' <"${pkg_Makefile}" | grep -Po "(?<=define Build\/Test).*?(?=endef)"); do
		tests="$tests $test"
	done

	[ -z "$tests" ] && {
		echo "Package '${pkg}' does not have any tests written. Skipping." >&"$FD"
		continue
	}

	echo "OK: $tests" >&"$FD"

	available_tests[$pkg]=$tests
done

test_count="${#available_tests[@]}"
# If checking, don't run tests, just exit
[ "$((CHECK))" -eq 1 ] && {
	[ "$test_count" -le 0 ] && echo "No packages to test" >&"$FD"
	[ "$test_count" -gt 0 ]
	exit "$?"
}

# No packages to test - exit with success
[ "$test_count" -le 0 ] && {
	echo "No packages to test" >&"$FD"
	exit 0
}

# Compile toolchain
make prepare "-j${RUNNER_JOB_THREADS:-1}" V="$V"

for pkg in "${!available_tests[@]}"; do
	make "package/${pkg}/compile" V="$V" || {
		FAIL=1
		continue
	}

	for test_file in ${tests_to_run[$pkg]}; do
		[ "$test_file" == "all" ] || grep -q "$test_file" <<<"${available_tests[$pkg]}" || {
			echo "'$test_file' is not a valid test for $pkg package. Skipping." >&"$FD"
			continue
		}
		make "package/${pkg}/unit_test" TEST_FILE="$test_file" TARGET_IP="$TARGET_IP" TARGET_PORT="$TARGET_PORT" V="$V" || {
			FAIL=1
			continue
		}
	done
done

exit $FAIL
