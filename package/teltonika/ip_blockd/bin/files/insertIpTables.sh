#!/bin/ash
# shellcheck shell=dash

# This script inserts/removes all required iptables rules for ip_blockd

# consts
ipt="/usr/sbin/iptables"
ip6t="/usr/sbin/ip6tables"

# checks a rule, if it's not there, inserts
check_and_insert() {
    local ipt=$1 # for ipv6 support
    local rule=$2

    $ipt -C $rule >& /dev/null
    [ "$?" = "1" ] && {
        $ipt -I $rule
    }
}

check_and_remove() {
    local ipt=$1 # for ipv6 support
    local rule=$2

    $ipt -C "$rule"
    [ "$?" = "0" ] && {
        $ipt -D "$rule"
    }
}

# enacts the required static ruleset
enact_ruleset() {
  func=$1 # function must have args: $1 as ipt bin, $2 as rule

  # ipv4
  $func "$ipt" "INPUT -m set --match-set ipb_mac src -j DROP"
  $func "$ipt" "INPUT -m set --match-set ipb_port src,dst -j DROP"
  $func "$ipt" "INPUT -m set --match-set ipb_port_dest src,dst,dst -j DROP"
  $func "$ipt" "FORWARD -m set --match-set ipb_mac src -j DROP"
  $func "$ipt" "FORWARD -m set --match-set ipb_port src,dst -j DROP" 
  $func "$ipt" "FORWARD -m set --match-set ipb_port_dest src,dst,dst -j DROP"

  # ipv6
  $func "$ip6t" "INPUT -m set --match-set ipb_mac src -j DROP"
  $func "$ip6t" "INPUT -m set --match-set ipb_port_v6 src,dst -j DROP"
  $func "$ip6t" "INPUT -m set --match-set ipb_port_dest_v6 src,dst,dst -j DROP"
  $func "$ip6t" "FORWARD -m set --match-set ipb_mac src -j DROP"
  $func "$ip6t" "FORWARD -m set --match-set ipb_port_v6 src,dst -j DROP" 
  $func "$ip6t" "FORWARD -m set --match-set ipb_port_dest_v6 src,dst,dst -j DROP"

}

main() {
  [ "$1" = "insert" ] && {
    enact_ruleset check_and_insert
    exit 0
  }

  [ "$1" = "remove" ] && {
    enact_ruleset check_and_remove
    exit 0
  }
}

main "$1"
