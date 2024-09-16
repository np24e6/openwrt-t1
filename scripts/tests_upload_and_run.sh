#!/bin/bash

declare -A executables_dirs

print_help() {
	printf "Usage: %s IP PORT EXECUTABLE [EXECUTABLE...]

Arguments:
	IP		IP address of the device to transfer the EXECUTABLE to
	PORT		SSH port of the device (default: 22)
	EXECUTABLE...	List of executables to transfer to and execute on the device

Uses sshpass to authenticate if it's installed and TARGET_PASS variable is set in the environment.
" "$0"
}

ssh_pass() {
	local prog=$1
	local port_opt=$2
	shift 2

	# prepend SSH options to $@
	set -- -o LogLevel=ERROR -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null "${port_opt}" "${TARGET_PORT}" "$@"

	if [ -n "$TARGET_PASS" ]; then
		SSHPASS=${TARGET_PASS} sshpass -e "$prog" "$@"
	else
		"$prog" "$@"
	fi
}

TARGET_IP=$1
TARGET_PORT=$2
shift 2

while [ -n "$1" ]; do
	arg=$1

	case "$arg" in
	-*)
		printf "Unknown option '%s'\n" "$arg"
		print_help
		exit 1
		;;
	*)
		executables_dirs["$(basename "$arg")"]="$(dirname "$arg")"
		;;
	esac
	shift
done

[ -z "$TARGET_IP" ] || [ -z "$TARGET_PORT" ] && {
	echo "Error: missing TARGET_IP and/or TARGET_PORT" >&2
	print_help
	exit 1
}

which sshpass >/dev/null || TARGET_PASS=''

for exe in "${!executables_dirs[@]}"; do
	ssh_pass "scp" "-P" "${executables_dirs[$exe]}/${exe}" "root@${TARGET_IP}:/tmp/${exe}"
	ssh_pass "ssh" "-p" "root@${TARGET_IP}" "chmod +x /tmp/${exe}; GCOV_PREFIX=${GCOV_PREFIX} \
		GCOV_PREFIX_STRIP=${GCOV_PREFIX_STRIP} /tmp/${exe}"
done

if [ "$GCOV_RUN" = "1" ];
then
	ssh_pass "scp" "-P" "-r" "root@${TARGET_IP}:${GCOV_PREFIX}*" "./build/gcov/out"
fi
