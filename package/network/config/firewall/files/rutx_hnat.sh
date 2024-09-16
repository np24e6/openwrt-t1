#!/bin/sh
. /lib/functions.sh

offload_status=$(uci get firewall.1.flow_offloading_hw)

if [[ ! -e /usr/bin/qssdk-shell ]]; then
    echo "qssdk-shell was not found."
    return
fi

if [[ $offload_status == 1 ]]; then
    qssdk-shell nat global set enable disable
elif [[ $offload_status == 0 ]]; then
    qssdk-shell nat global set disable disable
else
    echo "UCI configuration was not found for HNAT."
fi
