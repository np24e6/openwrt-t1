#!/usr/bin/env bash

days=''
PACKAGES_ROOT=/home/admin/opkg_packages

help_and_exit() {
	printf "Usage: upload_opkg.sh [-c [MONTHS]] [ -d | -p ] DEVICE

Options:
	-c | --cleanup MONTHS	Cleanup packages that are older than MONTHS (minimum and default: 1).
					DEVICE argument is not required, all devices will be cleaned up.
	-d | --demo		Upload to OPKG test server
	-p | --production	Upload to OPKG production server

Arguments:
	DEVICE			Target name, e.g. RUT36

SSH credentials that are needed in the environment:
	{PRODUCTION | TEST}_SSH_PRIVATE_KEY
	{PRODUCTION | TEST}_SSH_HOST_KEY
	{PRODUCTION | TEST}_SSH_USER_HOST
	{PRODUCTION | TEST}_SSH_PORT
"
	exit 1
}

check_missing() {
	for var in "${SSH_PRIVATE_KEY}" "${SSH_HOST_KEY}" "${SSH_USER_HOST}" "${SSH_PORT}"; do
		[[ -z ${!var} ]] && echo "$var"
	done
}

cleanup() {
	local path=${1}/
	local mtime=+${2:-30}

	local df_cmd="df --block-size=1 --output=used '$path' | tail -n1"
	local fmt='%7d MB'

	ssh -p "${!SSH_PORT}" "${!SSH_USER_HOST}" "\
		b=\"\$($df_cmd)\" ;\
		d=1048576 ;\
		find '$path' -maxdepth 1 -type d -mtime '$mtime' -exec rm -r {} ';' ;\
		a=\"\$($df_cmd)\" ;\
		printf \"Before:  $fmt\\nAfter:   $fmt\\nCleaned: $fmt\\n\" \"\$((b / d))\" \"\$((a / d))\" \"\$(((b - a) / d))\"
	"
}

while [ $# -gt 0 ]; do
	case $1 in
	--cleanup | -c)
		days=$(($2 * 30))
		[ $days -lt 30 ] && days=30
		shift 1
		;;
	--production | -p)
		prefix=PRODUCTION
		;;
	--demo | -d)
		prefix=TEST
		;;
	-*)
		help_and_exit
		;;
	*)
		PLATFORM="${1^^}"
		;;
	esac
	shift 1
done

[[ -z $prefix ]] && help_and_exit

SSH_PRIVATE_KEY="${prefix}_SSH_PRIVATE_KEY"
SSH_HOST_KEY="${prefix}_SSH_HOST_KEY"
SSH_USER_HOST="${prefix}_SSH_USER_HOST"
SSH_PORT="${prefix}_SSH_PORT"

missing=$(check_missing)
[[ -n $missing ]] && {
	#shellcheck disable=2086 # Splitting of 'missing' is intended here
	printf "Environment variable not found: %s\n" $missing
	help_and_exit
}

eval "$(ssh-agent -s)"
trap "ssh-agent -k" exit
ssh-add <(echo "${!SSH_PRIVATE_KEY}")
mkdir -p ~/.ssh
echo "${!SSH_HOST_KEY}" >~/.ssh/known_hosts

[[ -n $days ]] && {
	[[ $prefix == PRODUCTION ]] && {
		printf "%s cleanup is not allowed!\n" "$prefix"
		exit 1
	}
	cleanup "$PACKAGES_ROOT" "$days"
	exit 0
}

TOPDIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." >/dev/null && pwd)
ARCH=$(ls "${TOPDIR}/bin/packages/")
PACKAGEDIR="${TOPDIR}/bin/packages/${ARCH}/pm_packages"
ZIPPEDDIR="${TOPDIR}/bin/packages/${ARCH}/zipped_packages"
TAG=$(git describe | awk -F "-pm" '{print $1}')
HASH=$(echo -n "00/${TAG}/${PLATFORM}" | sha256sum | awk '{print $1}')
FOLDER="$PACKAGES_ROOT/${HASH}"
LINK="$PACKAGES_ROOT/packages/00/${TAG}"

echo "UPLOADING TO ${!SSH_USER_HOST#*@}:"
ssh -p "${!SSH_PORT}" "${!SSH_USER_HOST}" "rm -fr ${FOLDER:?}"
ssh -p "${!SSH_PORT}" "${!SSH_USER_HOST}" "mkdir -p ${FOLDER}/wiki ${LINK} && ln -fs ${FOLDER} ${LINK}/${PLATFORM}" || exit 1
scp -P "${!SSH_PORT}" "${PACKAGEDIR}/"* "${!SSH_USER_HOST}:/${FOLDER}/"
scp -P "${!SSH_PORT}" "${ZIPPEDDIR}/"* "${!SSH_USER_HOST}:/${FOLDER}/wiki/"

echo "OPKG URL: http://$([[ $prefix == TEST ]] && echo 'test.')opkg.teltonika-networks.com/${HASH}"

exit 0
