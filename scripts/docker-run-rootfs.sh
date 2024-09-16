#!/bin/sh

#set -xxe

SELF="$0"
ROOTFS_PATH="$(pwd)/bin/targets/x86/64-glibc/openwrt-x86-64-teltonika_x86_64-rootfs.tar.gz"
NETWORK_ENABLE="${NETWORK_ENABLE:-0}"
NETWORK_PREFIX="${NETWORK_PREFIX:-192.168.33}"
IMAGE_NAME="openwrt-rootfs:$NETWORK_PREFIX"
NETWORK_NAME="none"

die() {
	echo "$1"
	exit 1
}

usage() {
	cat >&2 <<EOF
Usage: $SELF [-h|--help]
       $SELF
         [--rootfs <rootfs>]
         [-n|--network]
         [-p|--prebuild]
         [-m|--nat]

<rootfs> allows to specifiy a different path for the rootfs.
<network> enables network access based on <NETWORK_PREFIX>
<prebuild> uses the official docker images openwrtorg/rootfs:latest
	-> changes to <NETWORK_PREFIX> are ignored
<nat> masquerades traffic into the container, so that it can be accessed from outside the host
EOF
}

parse_args() {
	while [ "$#" -gt 0 ]; do
		case "$1" in
			--rootfs) ROOTFS_PATH="$2"; shift 2 ;;
			--network|-n) NETWORK_ENABLE=1; shift ;;
			--prebuild|-p) PREBUILD=1; shift ;;
			--nat|-m) NAT_MASQUERADE=1; shift ;;
			--help|-h)
				usage
				exit 0
				;;
			*)
				DOCKER_EXTRA="$DOCKER_EXTRA $1"
				shift
				;;
		esac
	done
}

parse_args "$@"

[ -f "$ROOTFS_PATH" ] || die "Couldn't find rootfs at $ROOTFS_PATH"

if [ -z "$PREBUILD" ]; then
	DOCKERFILE="$(mktemp -p $(dirname $ROOTFS_PATH))"
	cat <<EOT > "$DOCKERFILE"
	FROM scratch
	ADD $(basename $ROOTFS_PATH) /
	RUN echo "console::askfirst:/usr/libexec/login.sh" >> /etc/inittab; \
		echo "::askconsole:/usr/libexec/login.sh" >> /etc/inittab
	EXPOSE 22 80 443
	USER root
	CMD ["/sbin/init"]
EOT
	docker build -t "$IMAGE_NAME" -f "$DOCKERFILE" "$(dirname $ROOTFS_PATH)"
	rm "$DOCKERFILE"
else
	IMAGE_NAME="openwrtorg/rootfs:latest"
	docker pull "$IMAGE_NAME"
fi

echo "[*] Build: $ROOTFS_PATH"

if [ "$NETWORK_ENABLE" = 1 ]; then
	NETWORK_NAME="rutos-lan-$NETWORK_PREFIX"
	LAN_IP="$NETWORK_PREFIX.1"
	if [ -z "$(docker network ls | grep $NETWORK_NAME)" ]; then
		docker network create \
		  -o "com.docker.network.bridge.name"="br-rutos-docker" \
		  --driver=bridge \
		  --subnet="$NETWORK_PREFIX.0/24" \
		  --ip-range="$NETWORK_PREFIX.0/24" \
		  --gateway="$NETWORK_PREFIX.2" \
		  "$NETWORK_NAME"
		echo "[*] Created $NETWORK_NAME network "

		cleanup() {
			while docker network inspect "$NETWORK_NAME" &>/dev/null; do
				docker network rm "$NETWORK_NAME" && break
				sleep 1
			done
			echo '[*] Cleaned up network'
		}
		trap cleanup EXIT
	fi

	if [ "$NAT_MASQUERADE" = 1 ]; then
		NAT_MASQUERADE_RULE_FORWARD='FORWARD -j ACCEPT'
		NAT_MASQUERADE_RULE_POSTROUTING='POSTROUTING -o br-rutos-docker -j MASQUERADE -tnat'

		scf="$(sysctl net.ipv4.ip_forward)"
		scf="${scf##*= }"

		sudo sh -c "
			sysctl -w net.ipv4.ip_forward=1
			iptables -I $NAT_MASQUERADE_RULE_FORWARD
			iptables -I $NAT_MASQUERADE_RULE_POSTROUTING
			{
				exec </dev/null
				wait $$
				iptables -D $NAT_MASQUERADE_RULE_FORWARD
				iptables -D $NAT_MASQUERADE_RULE_POSTROUTING
				sysctl -w net.ipv4.ip_forward=$scf
			} &
		"
	fi
fi

docker run --privileged --interactive --tty --rm --network="$NETWORK_NAME" --ip="$LAN_IP" \
	--name openwrt-docker $DOCKER_EXTRA "$IMAGE_NAME"
