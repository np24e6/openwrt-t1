#!/bin/ash

# This script inserts/removes all required ipt rules for ip_blockd

# consts
ipt="iptables"
ip6t="ip6tables"

rule_input="INPUT -m set --match-set"
rule_forward="FORWARD -m set --match-set"
target="-j DROP"

# checks a rule, if it's not there, inserts
check_and_insert() {
	local ipt=$1 # for ipv6 support
	local rule=$2

	$ipt -C $rule 2>/dev/null || {
		$ipt -I $rule 2>/dev/null
	}
}

check_and_remove() {
	local ipt=$1 # for ipv6 support
	local rule=$2

	$ipt -C $rule 2>/dev/null && {
		$ipt -D $rule 2>/dev/null
	}
}

# executes an ipt rule in both the input and forward tables
rule_infw() {
	func=$1
	iptb=$2
	table=$3
	io=$4

	$func "$iptb" "${rule_input} ${table} ${io} ${target}"
	$func "$iptb" "${rule_forward} ${table} ${io} ${target}"
}

# executes an ipt rule in both ipv4 and ipv6 iptables,
# while calling the function above, so that it would insert
# into both INPUT and FORWARD tables as well
rule_ip() {
	func=$1
	table=$2
	io=$3

	rule_infw $func $ipt $table $io
	rule_infw $func $ip6t "${table}_v6" $io
}

# enacts the required static ruleset
enact_ruleset() {
	func=$1 # function must have args: $1 as ipt bin, $2 as rule

	rule_ip $func "ipb_only_ip" "src"
	rule_ip $func "ipb_port" "src,dst"
	rule_ip $func "ipb_port_dest" "src,dst,dst"
}

main() {
	case $1 in
	"insert")
		enact_ruleset check_and_insert
		exit 0
		;;
	"remove")
		enact_ruleset check_and_remove
		exit 0
		;;
	esac
}

main $1
