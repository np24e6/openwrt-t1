#!/bin/sh
title=""
paragraph=""
shell_cert="/tmp/certificate.pem"
uhttpd_cert=$(uci -q get uhttpd.main.cert)
uhttpd_key=$(uci -q get uhttpd.main.key)
key_type=$(uci get uhttpd.defaults.key_type)
enable=$(uci -q get cli.status.enable)
wan_deny=0
echo "$SERVER_ADDR" | grep -q -E '^192\.168\.|^10\.' || {
	wan_access=$(uci -q get cli.status._cliWanAccess)
	[ "$wan_access" -eq "1" ] || wan_deny=1
}

if [ "$enable" -eq "1" ] && [ "$wan_deny" -eq "0" ]; then
	port=$(uci -q get cli.status.port)
	if [ -z "$port" ]; then
		port="4200-4220"
		uci -q set cli.status.port="$port"
		uci -q commit cli
	fi
	shell_limit=$(uci -q get cli.status.shell_limit)
	if [ -z "$shell_limit" ]; then
		shell_limit="5"
		uci -q set cli.status.shell_limit="$shell_limit"
		uci -q commit cli
	fi
	shells=$(ps | grep -v grep | grep -c shellinaboxd)
	if [ "$shells" -lt "$shell_limit" ]; then
		if [ -n "$HTTPS" ]; then
			tmp_cert_file=$(mktemp)

			if openssl rsa -in "$uhttpd_key" -check 2>/dev/null > /dev/null; then
				cp "$uhttpd_key" "$tmp_cert_file"
			elif openssl ec -in "$uhttpd_key" -check 2>/dev/null > /dev/null; then
				openssl ec -in "$uhttpd_key" -outform PEM 2>/dev/null > "$tmp_cert_file"
			else
				openssl dsa -in "$uhttpd_key" -outform PEM 2>/dev/null > "$tmp_cert_file"
			fi

			if grep -q "[^[:print:][:blank:]]" "$uhttpd_cert"; then
				openssl x509 -inform DER -in "$uhttpd_cert" -outform PEM >> "$tmp_cert_file"
			else
				cat "$uhttpd_cert" >> "$tmp_cert_file"
			fi

			mv "$tmp_cert_file" "$shell_cert"
			exec 3<"$shell_cert"
			/usr/sbin/shellinaboxd --disable-ssl-menu --cgi="${port}" -u 0 -g 0 --cert-fd=3
		else
			/usr/sbin/shellinaboxd -t --cgi="${port}" -u 0 -g 0
		fi
	else
		title="Too many active shell instances!"
		paragraph="Too many active shell instances! Close some shell instances and try again."
	fi
else
	title="CLI not enabled!"
	paragraph="CLI not enabled! Enable CLI and try again."
fi

if [ -n "$title" ] && [ -n "$paragraph" ]; then
echo "Content-type: text/html"
echo ""
cat <<EOT
<!DOCTYPE html>
<html>
<head>
        <title>${title}</title>
</head>
<body>
        <p>${paragraph}</p>
</body>
</html>
EOT
fi
