#!/bin/sh

. /usr/share/libubox/jshn.sh
. /lib/functions.sh

NAME=$(basename $0)
MAX_INSTANCE_RETRIES=180
MAX_WAIT_RETRIES=70
WAIT_SLEEP=2

# Error status
PING_SUCCESS=0
ERR_NETWORK_CONFIG_FAIL=100
ERR_SIM_IDLE_CONFIG_FAIL=101
ERR_PING_FAIL=102
ERR_CONN_FAIL=104
ERR_DISCONN_FAIL=105
MOBIFD_OBJ_NOT_FOUND=106
NO_SUITALBE_INTERFACES_FOUND=107
NO_SUITALBE_ACTIVE_INTERFACES_FOUND=108
NO_ACTIVE_SIM_FOUND=109
FLOCK_FAILED=110

SIM_ARG=0
MODEM_ARG=0
MODEM_OBJ=-1
SIM_CHANGE=-1

INICIAL_SIM=0
CURRENT_ACTIVE_SIM=0

# Interface name for pinging
MOBILE_INTERFACE_L3_DEVICE=0

usage() {
    echo "Usage: $0 [-s <sim1|sim2>] [-m <modem_id>]" 1>&2
    exit 1
}

logprint() {
    logger -t "($$) $NAME" "$1"
}

log() {
    ubus call log write_ext "{
        \"event\": \"$1\",
        \"sender\": \"Sim Idle Protection\",
        \"table\": 2,
        \"write_db\": 1
    }"
}

# Get mobifd/gsmd interface used on port
find_mobifd_object() {
    local arg="$(ubus list gsm.modem*)"
    for object in $arg; do
        json_init
        json_load "$(ubus call $object info)"
        json_get_var usb_id usb_id

        if [ "$usb_id" == "$MODEM_ARG" ]; then
            MODEM_OBJ=$(echo $object | tail -c 2)
            break
        fi
    done

    [ $MODEM_OBJ -lt 0 ] && finish $MOBIFD_OBJ_NOT_FOUND
}

# Wait for GSM to report connectivity
wait_for_connection() {
    local count=0
    local state=$(gsmctl -j -O $MODEM_ARG)
    while [ "$state" = "Disconnected" ]; do
        state=$(gsmctl -j -O $MODEM_ARG)
        count=$((count + 1))
        sleep $WAIT_SLEEP
        [ $count -eq $MAX_WAIT_RETRIES ] && finish $ERR_CONN_FAIL
    done
}

# Wait for GSM to report disconnect
wait_for_disconnect() {
    local count=0
    local state=$(gsmctl -j -O $MODEM_ARG)
    while [ "$state" != "Disconnected" ]; do
        state=$(gsmctl -j -O $MODEM_ARG)
        count=$((count + 1))
        sleep $WAIT_SLEEP
        [ $count -eq $MAX_WAIT_RETRIES ] && finish $ERR_DISCONN_FAIL
    done
}

wait_for_interface() {
    local count=0
    local status=1
    logprint "Finding suitable interface"
    while [ "$status" != "0" ]; do
        found_up_interface
        [ "$?" == "1" ] || status=0
        sleep $WAIT_SLEEP
        [ $count -eq $MAX_WAIT_RETRIES ] && finish $NO_SUITALBE_ACTIVE_INTERFACES_FOUND
    done
}

# Get current active sim card slot
get_active_sim_info() {
    json_init
    json_load "$(ubus call gsm.modem$MODEM_OBJ get_sim_slot)"
    json_get_var CURRENT_ACTIVE_SIM index
    [ -z "$CURRENT_ACTIVE_SIM" ] && finish $NO_ACTIVE_SIM_FOUND
}

# Reset sim info to its initial state
reset_sim() {
    if [ "$SIM_CHANGE" == "1" ]; then
        logprint "Switching back to sim$INICIAL_SIM"
        ubus call gsm.modem$MODEM_OBJ set_sim_slot "{\"index\":$INICIAL_SIM}"
        wait_for_disconnect
    fi
}

# Get l3_dev
get_l3_dev() {
    json_init
    json_load "$(ubus call network.interface.$1 status)"
    json_get_var MOBILE_INTERFACE_IS_UP up
    json_get_var MOBILE_INTERFACE_L3_DEVICE l3_device
}

# Load required options from sim_idle_protection config
sim_idle_config_load() {
    config_load "sim_idle_protection"

    [ -z "$CONFIG_SECTIONS" ] && finish $ERR_SIM_IDLE_CONFIG_FAIL

    for section in $CONFIG_SECTIONS; do
        config_get sim_ping_m $section modem
        config_get sim_ping_p $section position
        if [[ "$sim_ping_m" == "$MODEM_ARG" && "$sim_ping_p" == "$SIM_ARG" ]]; then
            config_get ping_ip $section host
            config_get ping_c $section count
            config_get ping_s $section packet_size
            config_get ping_t $section ip_type
            ping_t=$(echo $ping_t | tail -c 2)
        fi
    done
}

# Found suitable interfaces from network config
found_suitable_interfaces() {
    config_load "network"

    [ -z "$CONFIG_SECTIONS" ] && finish $ERR_NETWORK_CONFIG_FAIL

    for section in $CONFIG_SECTIONS; do
        config_get cfgtype "$section" TYPE
        [ "$cfgtype" != "interface" ] && continue

        config_get sim "$section" sim
        [ -z "$sim" ] && continue

        config_get modem_id "$section" modem
        config_get pdptype "$section" pdptype
        config_get disabled "$section" disabled
        [ "$sim" != "$SIM_ARG" ] || [ "$modem_id" != "$MODEM_ARG" ] || [ -n "$disabled" ] && continue

        [ "$ping_t" == "4" ] && [[ "$pdptype" == "ip" || "$pdptype" == "ipv4v6" ]] && suitable_interface_list="$suitable_interface_list $section"
        [ "$ping_t" == "6" ] && [[ "$pdptype" == "ipv6" || "$pdptype" == "ipv4v6" ]] && suitable_interface_list="$suitable_interface_list $section"

    done

    [ -z "$suitable_interface_list" ] && finish $NO_SUITALBE_INTERFACES_FOUND
}

# Found active suitable interfaces
found_up_interface() {
    local status=1

    local all_interfaces=$(ubus list network.interface.* | xargs)

    for interface in $suitable_interface_list; do
        interface_obj="$interface"_"$ping_t"
        echo $all_interfaces | grep -q $interface_obj || continue
        get_l3_dev $interface_obj
        [ "$MOBILE_INTERFACE_IS_UP" != "1" ] && continue
        inter_ping_from_list="$inter_ping_from_list $MOBILE_INTERFACE_L3_DEVICE"
        status=0
    done

    return $status
}

# Simple ping method
ping_ip() {
    local status=1
    # If source interface is set to wwan and pining to 127.0.0.1 ping fails
    [ "$1" == "127.0.0.1" ] && interface=0 || interface=$5

    if ping -$4 $1 -W 1 -c $2 -s $3 -I $interface &>/dev/null
    then
        status=0
    fi

    return "$status"
}

# Ping from all active suitable interfaces
ping_ip_all() {
    local status=1

    get_active_sim_info
    wait_for_interface

    for inter in $inter_ping_from_list; do
        ping_ip "$ping_ip" "$ping_c" "$ping_s" "$ping_t" "$inter"

        [ "$?" == "1" ] && logprint "Failed to ping from interface $MOBILE_INTERFACE_L3_DEVICE" || status=0 && break
    done

    [ "$status" != "0" ] && finish $ERR_PING_FAIL
}

# Print error messages and reset sim
finish() {
    local option="$1"

    case "$option" in
    "$ERR_NETWORK_CONFIG_FAIL")
        logprint "Failed to load network config file"
        log "Ping failed"
        reset_sim
        exit "$option"
        ;;
    "$ERR_SIM_IDLE_CONFIG_FAIL")
        logprint "Failed to load sim_idle_protection config file"
        log "Ping failed"
        reset_sim
        exit "$option"
        ;;
    "$ERR_PING_FAIL")
        logprint "Ping failed"
        log "Ping failed"
        reset_sim
        exit "$option"
        ;;
    "$ERR_CONN_FAIL")
        logprint "Failed to make mobile data connection"
        log "Ping failed"
        reset_sim
        exit "$option"
        ;;
    "$ERR_DISCONN_FAIL")
        logprint "Failed to disconnect from current SIM $INICIAL_SIM"
        log "Ping failed"
        reset_sim
        exit "$option"
        ;;
    "$MOBIFD_OBJ_NOT_FOUND")
        logprint "Could not find mobifd object"
        log "Ping failed"
        exit "$option"
        ;;
    "$NO_SUITALBE_INTERFACES_FOUND")
        log "There is no mobile IPv$ping_t interface found. Ping failed"
        logprint "There is no mobile IPv$ping_t interface found. Ping failed"
        reset_sim
        exit "$option"
        ;;
    "$NO_ACTIVE_SIM_FOUND")
        log "Ping failed"
        logprint "Could not get current active SIM slot"
        reset_sim
        exit "$option"
        ;;
    "$PING_SUCCESS")
        logprint "Successfully pinged from SIM $CURRENT_ACTIVE_SIM"
        log "Successfully pinged from SIM $CURRENT_ACTIVE_SIM"
        reset_sim
        exit "$option"
        ;;
    "$FLOCK_FAILED")
        log "Ping failed"
        logprint "sim_idle_protection flock failed"
        exit "$option"
        ;;
    *)
        logprint "Unknown error"
        reset_sim
        exit "$option"
        ;;
    esac
}

main() {

    # Find mobifd object
    find_mobifd_object

    # Get current sim slot
    get_active_sim_info
    INICIAL_SIM=$CURRENT_ACTIVE_SIM

    # Load sim_idle_protection config
    sim_idle_config_load

    # Check if there is suitable interface and it's enabled
    found_suitable_interfaces

    # If current SIM slot is not that need ping from
    if [ "$SIM_ARG" != "$INICIAL_SIM" ]; then
        logprint "Switching to sim$SIM_ARG"
        ubus call gsm.modem$MODEM_OBJ set_sim_slot "{\"index\":$SIM_ARG}"
        SIM_CHANGE=1
        wait_for_disconnect
    fi

    logprint "Waiting for sim$SIM_ARG connection"
    wait_for_connection

    ping_ip_all
    finish $PING_SUCCESS
}

sim_idle_lock() {
    local status=1
	flock -n 700 &> /dev/null
	if [ "$?" != "0" ]; then
	    exec 700>"$LOCK_FILE"
		flock 700
		if [ "$?" != "0" ]; then
			status=0
		fi
	fi
	return $status
}

while getopts ":s:m:" o; do
    case "${o}" in
    s)
        [ "$OPTARG" != "sim1" ] && [ "$OPTARG" != "sim2" ] && usage
        SIM_ARG=$(echo $OPTARG | tail -c 2)
        ;;
    m)
        MODEM_ARG=${OPTARG}
        ;;
    *)
        usage
        ;;
    esac
done

[ "$SIM_ARG" == "0" ] && usage
[ "$MODEM_ARG" == "0" ] && usage

LOCK_FILE=/tmp/lock/sim_idle_$MODEM_ARG.lock

# Check if another instance is running
sim_idle_lock && finish $FLOCK_FAILED

main

return 0
