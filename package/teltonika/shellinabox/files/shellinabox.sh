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
			if grep -q "[^[:print:][:blank:]]" "$uhttpd_cert"; then
				openssl x509 -inform DER -in "$uhttpd_cert" -outform PEM | cat "$uhttpd_key" - > /tmp/shellinabox.tmp
			else
				cat "$uhttpd_key" "$uhttpd_cert" > /tmp/shellinabox.tmp
			fi
			mv /tmp/shellinabox.tmp "$shell_cert"
			[ "$key_type" = "ec" ] && sed -i 's/\([^C]\) PRIVATE/\1 EC PRIVATE/g' "$shell_cert"
		if [ -n "$HTTPS" ]; then
			/usr/sbin/shellinaboxd --disable-ssl-menu --cgi="${port}" -u 0 -g 0 -c /tmp
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
