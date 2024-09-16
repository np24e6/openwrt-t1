#!/bin/sh

case "$1" in
	"openvpn-openssl")
		openvpn --version | grep "$2.*SSL (OpenSSL)"
		;;
esac
