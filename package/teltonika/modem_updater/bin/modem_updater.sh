#!/bin/sh
. /usr/share/libubox/jshn.sh
DEBUG_LOG="1"
DEBUG_ECHO="0"
HOSTNAME="modemfota.teltonika-networks.com"
SSH_PASS="u3qQo99duKeaVWr7"
DEVICE=""
SKIP_VALIDATION="0"
FW_PATH="/tmp/firmware/"
FLASHER_PATH=""
SSHFS_PATH="/usr/bin/sshfs"
LEGACY_MODE="0"
TTY_PORT=""
modem_n=""
modem_usb_id=""
MEIG_FLASHER="meig_firehose"
MEIG_ASR_FLASHER="fbfdownloader"
QUECTEL_FLASHER="quectel_flash"
QUECTEL_ASR_FLASHER="QDLoader"
JUST_LIST="0"
PRODUCT_NAME=$(mnf_info -n | cut -b 1-4)
CPU_NAME=$( < /proc/cpuinfo grep -e model)
KERNEL_VERSION=$(uname -a | awk -F ' ' '{print $3}')
current_version=$( < /etc/version awk -F '.' '{print $2 "." $3}')
USER_PATH="0"
VERSION=""
NEED_MODEM_RESTART="0"
NEED_SERVICE_RESTART="0"
device_path="/sys/bus/usb/devices/"

find_modem_edl_device() {
    sys_path=""
    for entry in "$device_path"/*
    do
        #echo "$entry"
        idVendor=$(cat "$entry/idVendor") > /dev/null 2>&1
        [ "$idVendor" = "05c6" ] || continue
        idProduct=$(cat "$entry/idProduct") > /dev/null 2>&1
        [ "$idProduct" = "9008" ] || continue
        [ "$sys_path" != "" ] && {
            echo "[WARN] Found multiple devices in EDL mode!"
            sys_path=""
            return
        }
        echo "Found EDL device: $entry"
        sys_path="$entry"
    done
}

exec_ubus_call() {
    local m_id="$1"
    local cmd="$2"
    local opt="$3"
    ubus_rsp=$(ubus call gsm.modem$m_id "$cmd" ${opt:+"$opt"})
}

parse_from_ubus_rsp() {
    echo "$ubus_rsp" | grep "$1" |  cut -d'"' -f 4
}

validate_connection()  {
    if ping -q -c 1 -W 5 8.8.8.8 >/dev/null; then
        debug "[INFO] internet connection is working"
        CONNECTION_STATUS="ACTIVE"
    else
        echo "[ERROR] internet connection is not active"
        CONNECTION_STATUS="INACTIVE"
    fi
}

get_modems() {
    echo "Modem List:"
    modem_array="$(ubus list gsm.modem* | tr "\n" " ")"
    for s in $modem_array ; do
        modem_n="${s: -1}"
        modem_info="$(ubus call "$s" info)"
        builtin="$(echo "$modem_info" | grep "builtin" | tr -d ' ,\"\t')"
        exec_ubus_call "$modem_n" "get_firmware"
        fwver=$(parse_from_ubus_rsp "firmware")
        usbid="$(echo "$modem_info" | grep "usb_id" | cut -d'"' -f 4)"
        name="$(echo "$modem_info" | grep "name" | cut -d'"' -f 4)"
        echo "[$modem_n] $name | USB_ID: $usbid | Firmware: $fwver | $builtin"
        get_fw_list
    done
}

verify_modem_n() {
    if [ "$modem_n" = "" ]; then
        echo "[ERROR] modem_n is not set. Please use \"-i\" or \"-u\" option."
        helpFunction
        graceful_exit
    fi
    exec_ubus_call "$modem_n" "exec" "{\"command\":\"AT\"}"
    #AT_RESULT=$(ubus call gsm.modem"$modem_n" exec {\"command\":\"AT\"}) > /dev/null 2>&1
    case "$ubus_rsp" in
        *OK*)
            debug "[INFO] $modem_n is responding to AT commands."
        ;;
        *)
            echo "[ERROR] modem_n is wrong or modem is not responding."
            helpFunction
            graceful_exit
    esac
}

exec_sshfs() {
    SSHFS_RESULT=$(echo $SSH_PASS | $SSHFS_PATH -p 21 rut@$HOSTNAME:/"$1" "$2" -o password_stdin 2>&1)
    debug "[INFO] $SSHFS_RESULT"
    case "$SSHFS_RESULT" in
        *Timeout*)
            echo "[ERROR] Timeout while mounting with sshfs. Your ISP might be blocking ssh connections."
            graceful_exit
        ;;
    esac
}

setup_ssh() {
    if [ ! -f $SSHFS_PATH ]; then
        echo "[ERROR] SSHFS not found."
        echo "You can install SSHFS using this command: \"opkg update && opkg install sshfs\""
        echo "Or alternatively using \"-r\" option."
        graceful_exit
    fi
    
    SSH_PATH="/root/.ssh"
    TMP_SSH_PATH="/tmp/known_hosts"
    
    chmod 755 $SSHFS_PATH
    
    key=$(cat $SSH_PATH/known_hosts | grep -c $HOSTNAME) > /dev/null 2>&1
    if [ "$key" = "0" ]; then
        debug "[INFO] Downloading key"
        
        curl -Ss --ssl-reqd https://$HOSTNAME/download/$TMP_SSH_PATH \
        --output $TMP_SSH_PATH --connect-timeout 90
        [ -f $SSH_PATH ] || mkdir -p $SSH_PATH
        cat $TMP_SSH_PATH >>$SSH_PATH/known_hosts
    fi
}

mount_sshfs() {
    [ -f $FW_PATH ] || mkdir -p $FW_PATH
    
    debug "[INFO] Checking old mount"
    
    if [ -n "$(ls -A $FW_PATH)" ]; then
        echo "[ERROR] $FW_PATH Not Empty. Unmounting"
        umount "$FW_PATH"
        if [ -n "$(ls -A $FW_PATH)" ]; then
            echo "[ERROR] Unmounting fail. Exiting..."
            graceful_exit
        fi
    else
        debug "[INFO] Old mount: Empty"
    fi
    
    # Mounting remote partition
    if [ "$DEVICE" = "QuectelUNISOC" ]; then
        exec_sshfs "RG500U//$VERSION" "$FW_PATH"
    else
        exec_sshfs "$VERSION" "$FW_PATH"
    fi
}

debug() {
    if [ "$DEBUG_LOG" = "1" ]; then
        logger -t "modem_updater" "$1"
    fi
    if [ "$DEBUG_ECHO" = "1" ]; then
        echo "$1"
    fi
}

is_modem_live() {
    result=$(gsmctl ${modem_n:+-N "$modem_n"} -A "AT")
    case "$result" in 
    *OK*)
        live="1"
        return
        ;;
    esac
    live="0"
}

restarted="0"
graceful_exit() {

    [ "$NEED_SERVICE_RESTART" = "1" ] && sleep 10 && start_services
    # If we don't need to restart we just exit here
    [ "$NEED_MODEM_RESTART" = "0" ] && exit 0

    # Wait for modem to turn ON, if it doesn't restart it
    # Most modems will power by themselves
    retries=10
    live=0
    echo "[INFO] Waiting for modem: $modem_n"
    while [ $retries != 0 ]; do
        echo "."
        retries=$((retries-1))
        is_modem_live
        [ "$live" = "1" ] && exit 0

        sleep 5
    done

    result=$(mctl -r -m "$modem_n")
    [ "$result" = "Unable to find specified modem" ] && {
        echo "[INFO] Restart failed. Trying to restart default modem"
        mctl -r

        [ "$restarted" = "0" ] && {
            restarted=1
            NEED_SERVICE_RESTART=0
            graceful_exit
        }
        exit 0
    }
}

helpFunction() {
    echo ""
    echo "Usage: $0 -v <version> <options>."
    printf "\t-g \t List all available modems and versions available for them.\n"
    printf "\t-v \t <version> Modem firmware version to install.\n"
    printf "\t-p \t <path> Specify a custom firmware path. Remote mounting with sshfs will not be used by the script.\n"
    printf "\t-r \t <fwver> Install missing dependencies into tmp folder. (Deprecated)\n"
    printf "\t-s \t Search for EDL devices.\n"
    printf "\t \t Use \"-r show\" to show available versions on the server.\n"
    printf "\t-h \t Print help.\n"

    echo "auxiliary options:"
    printf "\t-n \t <modem_n> Modem ID(number). eg.: 0, 1, 2...\n"
    printf "\t-i \t <modem_n> Modem USB ID. eg.: 1-1 3-1 etc.\n"
    printf "\t-f \t Force upgrade start without extra validation. USE AT YOUR OWN RISK.\n"
    printf "\t-l \t Legacy mode for quectel modems(Fastboot).\n"
    printf "\t-d \t <name> Manually set device Vendor (Quectel, QuectelASR, QuectelUNISOC or Meiglink, MeiglinkASR).\n"
    printf "\t-t \t <ttyUSBx> ttyUSBx port(for legacy mode).\n"
    printf "\t-D \t debug\n"
}

nvresultcheck() {
    case "$NV_RESULT" in
        *"nvburs: 0"* | *"NVBURS: 0"* | *"OK"*)
        ;;
        *)
            echo "[ERROR] NVBURS failed!"
            echo "$NV_RESULT"
            NV_FAILED=1
            return
        ;;
    esac
    debug "[INFO] NVBURS:0"
    NV_FAILED=0
}

setDevice() {
    if [ "$modem_n" = "" ]; then
        echo "[ERROR] Failed to find modem."
        exit 1
    fi

    exec_ubus_call "$modem_n" "info"
    DEVICE=$(parse_from_ubus_rsp "manuf")
    if [ "$DEVICE" = "N/A" ]; then
        echo "[ERROR] Device manufacturer not properly recognized. Exiting.."
        helpFunction
        graceful_exit
    fi

    # Change Quectel to QuectelASR here
    if [ "$DEVICE" = "Quectel" ]; then
        exec_ubus_call "$modem_n" "get_firmware"
        MODEM=$(parse_from_ubus_rsp "firmware")
        case $MODEM in
        EC200*) 
            DEVICE="QuectelASR"
            ;;
        RG500U*) 
            DEVICE="QuectelUNISOC"
            ;;
        *) 
            ;;
        esac
    fi
    # Change Meiglink to MeiglinkASR here
    if [ "$DEVICE" = "Meiglink" ]; then
        exec_ubus_call "$modem_n" "get_firmware"
        MODEM=$(parse_from_ubus_rsp "firmware")
        echo "fw: $MODEM"
        case $MODEM in
        SLM770*) 
            DEVICE="MeiglinkASR"
            ;;
        *) 
            ;;
        esac
    fi
}

setFlasherPath() {
    #setdevice
    if [ "$DEVICE" = "Quectel" ]; then
        FLASHER_PATH="/usr/bin/$QUECTEL_FLASHER"
    elif [ "$DEVICE" = "QuectelASR" ] || [ "$DEVICE" = "QuectelUNISOC" ]; then
        FLASHER_PATH="/usr/bin/$QUECTEL_ASR_FLASHER"
    elif [ "$DEVICE" = "Meiglink" ]; then
        FLASHER_PATH="/usr/bin/$MEIG_FLASHER"
    elif [ "$DEVICE" = "MeiglinkASR" ]; then
        FLASHER_PATH="/usr/bin/$MEIG_ASR_FLASHER"
    fi
}

retry_backup=0
backupnvr() {
    debug "[INFO] Meiglink device lets backup NVRAM"
    NV_RESULT=$(gsmctl -N "$modem_n" -A AT+NVBURS=2)
    exec_ubus_call "$modem_n" "info" 
    nvresultcheck
    #if no backup exists then we make one.
    if [ $NV_FAILED = 1 ]; then
        NV_RESULT=$(gsmctl -N "$modem_n" -A AT+NVBURS=0)
        nvresultcheck
        if [ $NV_FAILED = 1 ]; then
            #failed to make a backup.
            echo "[ERROR] Failed to make NV ram backup..."
            [ "$retry_backup" = 0 ] && {
                retry_backup=1
                sleep 5
                backupnvr
            }
            graceful_exit
        else
            echo "[INFO] NVRAM backup successful"
        fi
    fi
}

get_compatible_fw_list() {
    validate_connection
    if [ "$CONNECTION_STATUS" = "INACTIVE" ]; then
        echo "[ERROR] Cannot get firmware list from the server."
    else
        FW_LIST=$(curl -Ss --ssl-reqd https://$HOSTNAME/"$1")
        FOUND_FW=$(echo "$FW_LIST" | grep ^"$MODEM_SHORT" | sort -Vr)
        echo "Available versions (highest version number first):"
        echo "$FOUND_FW"
    fi
}

get_fw_list() {
    setDevice

    if [ "$modem_n" != "" ]; then
        verify_modem_n
    fi

    exec_ubus_call "$modem_n" "get_firmware"
    MODEM=$(parse_from_ubus_rsp "firmware")

    if [ "$DEVICE" = "Quectel" ]; then
        MODEM_SHORT=$(echo "$MODEM" | cut -b -7)
    elif [ "$DEVICE" = "QuectelASR" ] || [ "$DEVICE" = "QuectelUNISOC" ]; then
        MODEM_SHORT=$(echo "$MODEM" | cut -b -8)
    elif [ "$DEVICE" = "Meiglink" ]; then
        #Currently meiglink uses only 3 letter device names so this should be fine for now.
        MODEM_SHORT=$(echo "$MODEM" | cut -b -6)
    elif [ "$DEVICE" = "MeiglinkASR" ]; then
        MODEM_SHORT=$(echo "$MODEM" | cut -b -6)
    fi

    if [ "$PRODUCT_NAME" = "TRB1" ]; then
        get_compatible_fw_list "TRB1/fwlist.txt"
    elif [ "$PRODUCT_NAME" = "TRB5" ]; then
        get_compatible_fw_list "TRB5/fwlist.txt"
    elif [ "$DEVICE" = "QuectelASR" ]; then
        get_compatible_fw_list "EC200/fwlist.txt"
    elif [ "$DEVICE" = "MeiglinkASR" ]; then
        get_compatible_fw_list "SLM770/fwlist.txt"
    elif [ "$DEVICE" = "QuectelUNISOC" ]; then
        get_compatible_fw_list "RG500U/fwlist.txt"
    else
        get_compatible_fw_list "fwlist.txt"
    fi
}

get_modem_usb_id() {
    exec_ubus_call "$1" "info"
    modem_usb_id=$(parse_from_ubus_rsp "usb_id")
    echo "[INFO] modem_usb_id: $modem_usb_id"
}

usb_id_to_modem_n() {
    modem_array="$(ubus list gsm.modem* | tr "\n" " ")"
    for s in $modem_array ; do
        modem_info="$(ubus call "$s" info)"
        usbid="$(echo "$modem_info" | grep "usb_id" | cut -d'"' -f 4)"
        [ "$usbid" != "$modem_usb_id" ] && continue
        modem_n="${s: -1}"
        break
    done
    echo "[INFO]found modem_n: ""$modem_n"""
}

find_modem_n() {
    local get_any="$1"
    modem_array="$(ubus list gsm.modem* | tr "\n" " ")"
    if [ "$(echo "$modem_array" | wc -w)" = 1 ]; then
        modem_n="${modem_array: -2}"
        echo "[INFO] Detected modem_n: ""$modem_n"""
    elif [ "$get_any" != "" ]; then
        modem_n="${modem_array: -2}"
        echo "[INFO] Detected modem_n: ""$modem_n"""
    fi
}

# Returns 0 if firmware can not be upgraded due to embargo
check_blocked() {
    local current_firmware="$1"
    local new_firmware="$2"

    local R1="R[0-9]{2}A2[0-9]"
    local R2="_[0-9]{2}.2[0-9]{2}.[0-9]{2}.2[0-9]{2}$"

    ( [ "$(echo "$current_firmware" | grep -oEi "$R1")" != "" ] ||
    [ "$(echo "$current_firmware" | grep -oEi "$R2")" != "" ] ) &&
    ( [ "$(echo "$new_firmware" | grep -oEi "$R1")" == "" ] &&
    [ "$(echo "$new_firmware" | grep -oEi "$R2")" == "" ] ) && return 0

    return 1
}

common_validation() {
    if [ "$VERSION" = "" ] &&
    [ "$USER_PATH" = "0" ]; then
        echo "[ERROR] Modem version and firmware path not specified. Please use either ""-p"" or ""-v""."
        helpFunction
        graceful_exit
    fi

    if [ "$VERSION" != "" ] &&
    [ "$USER_PATH" != "0" ]; then
        echo "[ERROR] Modem version and firmware path are both specified. Please use either ""-p"" or ""-v""."
        helpFunction
        graceful_exit
    fi

    if [ "$modem_n" = "" ]; then
        #TODO: Get it automatically. If cant then or multiple then error.
        find_modem_n
        if [ "$modem_n" = "" ]; then
            echo "[ERROR] Cannot find modem_n automatically. "
            helpFunction
            graceful_exit
        fi
    fi

    if [ "$SKIP_VALIDATION" = "0" ]; then
        verify_modem_n
    fi

    if [ "$modem_usb_id" = "" ]; then
        get_modem_usb_id "$modem_n"
        if [ "$modem_usb_id" = "" ]; then
            echo "[ERROR] Could not get modem_usb_id automatically."
            helpFunction
            graceful_exit
        fi
    fi

    if [ "$DEVICE" = "" ]; then
        setDevice
    fi

    if [ "$DEVICE" = "Quectel" ] || [ "$DEVICE" = "Meiglink" ] || [ "$DEVICE" = "QuectelASR" ] ||
    [ "$DEVICE" = "MeiglinkASR" ] || [ "$DEVICE" = "QuectelUNISOC" ]; then
        debug "[INFO] DEVICE is compatible."
    else
        if [ "$SKIP_VALIDATION" = "0" ]; then
            echo "[ERROR] Not supported or unknown modem. Exiting.."
            graceful_exit
        fi
    fi

    if [ "$SKIP_VALIDATION" = "0" ] &&
    [ "$VERSION" != "" ]; then
        exec_ubus_call "$modem_n" "get_firmware"
        MODEM_FW=$(parse_from_ubus_rsp "firmware")
        case "$MODEM_FW" in
            *$VERSION* )
                echo "[ERROR] Specified firmware is already installed. Exiting.."
                graceful_exit
            ;;
        esac
        #Quectel
        if [ "$DEVICE" = "Quectel" ]; then
            exec_ubus_call "$modem_n" "get_firmware"
            DEV_MOD=$(parse_from_ubus_rsp "firmware" | cut -c 1-8)
            if [ "$DEV_MODULE" != "$DEV_MOD" ]; then
                [ "$(echo "$DEV_MODULE" | cut -c 1-4)" != "$(echo "$DEV_MOD" | cut -c 1-4)" ] ||
                ( ! ( [[ "$DEV_MODULE" =~ "AFF[AD]" ]] && [[ "$DEV_MOD" =~ "AFF[AD]" ]] )) && {
                    echo "$DEV_MODULE != $DEV_MOD"
                    echo "[ERROR] Specified firmware is not intended for this module. Exiting.."
                    graceful_exit
                }
            fi
            if check_blocked "$MODEM_FW" "$VERSION"; then
                echo "[ERROR] Modem firmware upgrade is disabled for this modem version. Exiting.."
                graceful_exit
            fi
        fi
        #Meiglink
        if [ "$DEVICE" = "Meiglink" ]; then
            exec_ubus_call "$modem_n" "get_firmware"
            DEV_MOD=$(parse_from_ubus_rsp "firmware" | head -c 6)
            DEV_MODULE=$(echo "$VERSION" | cut -c 1-6 | tr -d _)
            if [ "$DEV_MODULE" != "$DEV_MOD" ]; then
                echo "$DEV_MODULE != $DEV_MOD"
                echo "[ERROR] Specified firmware is not intended for this module. Exiting.."
                graceful_exit
            fi
        fi
    fi

}

confirm_modem_usb_id() {
    local match_in_path="$1"
    local match_id="$2"
    case "$match_in_path" in
        *"$match_id"*)
            confirm_modem_usb_id_result="1"
            ;;
        *)
            confirm_modem_usb_id_result="0"
            ;;
    esac
    
}

generic_validation()  {
    if [ "$TTY_PORT" != "" ] &&
    [ "$LEGACY_MODE" = "0" ]; then
        echo "[WARN] Warning you specified ttyUSB port but it will not be used for non legacy(fastboot) flash!"
    fi
      
    if [ "$LEGACY_MODE" = "1" ] && [ "$DEVICE" != "Quectel" ]; then
        echo "[ERROR] Legacy mode only supported for Quectel routers."
        graceful_exit
    fi
    
    setFlasherPath
    
    if [ ! -f "$FLASHER_PATH" ]; then
        echo "[ERROR] Flasher not found. You need to use \"-r\" option to install missing dependencies."
        helpFunction
        graceful_exit
    fi
}

start_services() {
    echo "[INFO] Starting services:"
    /etc/init.d/gsmd start
    /etc/init.d/modem_trackd start
    /etc/init.d/ledman start
}

stop_services() {
    echo "[INFO] Stopping services.."
    /etc/init.d/gsmd stop
    /etc/init.d/modem_trackd stop
    /etc/init.d/ledman stop
    NEED_SERVICE_RESTART="1"
}

generic_prep()  {
    /etc/init.d/modem_trackd stop
    NEED_SERVICE_RESTART="1"
    
    #backup NVRAM
    #its in the flasher now, but just in case..
    if [ "$DEVICE" = "Meiglink" ] && [ "$SKIP_VALIDATION" = "0" ]; then
        backupnvr
    fi
    
    #go into edl/disable mobile connection.
    if [ $LEGACY_MODE = "0" ]; then
        if [ "$DEVICE" = "Quectel" ]; then
            $FLASHER_PATH qfirehose -x ${modem_usb_id:+-s /sys/bus/usb/devices/"$modem_usb_id"}
        elif [ "$DEVICE" = "Meiglink" ]; then
            $FLASHER_PATH -x ${modem_usb_id:+-s /sys/bus/usb/devices/"$modem_usb_id"}
        fi
    else
        gsmctl ${modem_n:+-N "$modem_n"} -A "AT+CFUN=0"
    fi
    NEED_MODEM_RESTART="1"

    #Wait for backup connection to kick in.
    sleep 10
    
    #check if connection is working
    if [ "$USER_PATH" = "0" ]; then
        validate_connection
    fi
    
    if [ $USER_PATH = "0" ]; then
        setup_ssh
        mount_sshfs
    fi
    debug "[INFO] Checking firmware path"
    
    if [ -z "$(ls -A $FW_PATH)" ]; then
        echo "[ERROR] firmware directory is empty. Mount failed? Exiting..."
        graceful_exit
    fi
    
    #some firmware sanity checks
    if [ "$DEVICE" = "Quectel" ] &&
    [ $LEGACY_MODE = "0" ] &&
    [ -z "$(ls -A $FW_PATH/update/firehose/)" ]; then
        echo "[ERROR] No firehose folder found. Either the firmware is not right or you must use legacy(fastboot) mode"  
        if [ "$USER_PATH" = "0" ]; then
            umount "$FW_PATH"
        fi
        helpFunction
        graceful_exit
    fi

    stop_services
}

generic_flash()  {
    if [ "$DEVICE" = "Meiglink" ]; then
        $FLASHER_PATH -d -f "$FW_PATH" ${modem_usb_id:+-s /sys/bus/usb/devices/"$modem_usb_id"}
    fi
    
    if [ "$DEVICE" = "Quectel" ]; then
        if [ $LEGACY_MODE = "1" ]; then
            #fastboot
            $FLASHER_PATH -v -m2 -f "$FW_PATH" ${TTY_PORT:+-p "$TTY_PORT"}
        else
            #firehose
            find_modem_edl_device
            [ "$sys_path" != "" ] && confirm_modem_usb_id "$sys_path" "$modem_usb_id"            
            if [ "$confirm_modem_usb_id_result" == "0" ]; then
                $FLASHER_PATH qfirehose -n -f "$FW_PATH/update/firehose/" ${sys_path:+-s $sys_path}
            else #"" and "1"
                $FLASHER_PATH qfirehose -n -f "$FW_PATH/update/firehose/" ${modem_usb_id:+-s /sys/bus/usb/devices/"$modem_usb_id"}
            fi
        fi
    fi
    if [ "$DEVICE" = "QuectelUNISOC" ]; then
        "$FLASHER_PATH" -f "$FW_PATH/update.pac"
    fi
    
    sleep 10
        
    if [ "$USER_PATH" = "0" ]; then
        umount "$FW_PATH"
    fi
}

generic_start()  {
    generic_validation
    generic_prep
    echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    echo " DO NOT TURN OFF YOUR DEVICE DURING THE UPDATE PROCESS!"
    echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    echo "Starting flasher..."
    generic_flash
}

trb_validation()  {
    if [ "$USER_PATH" = "1" ]; then
        echo "Please use modem_upgrade directly."
        graceful_exit
    fi
}

trb_prep()  {
    UPDATE_BIN="/modem_update.bin"
    echo "[INFO] Downloading firmware"

    if [ -f "$UPDATE_BIN" ]; then
        echo "[INFO] Overwriting $UPDATE_BIN"
    fi

    if [ "$PRODUCT_NAME" = "TRB1" ]; then
        curl -Ss --ssl-reqd https://$HOSTNAME/TRB1/"$VERSION" \
        --output "$UPDATE_BIN" --connect-timeout 300
    elif [ "$PRODUCT_NAME" = "TRB5" ]; then
        curl -Ss --ssl-reqd https://$HOSTNAME/TRB5/"$VERSION" \
        --output "$UPDATE_BIN" --connect-timeout 300
    fi
}

trb_flash()  {
    check_result=$(modem_upgrade --check  --file "$UPDATE_BIN")
    
    case "$check_result" in
        *"Validation successful"*)
            modem_upgrade --file "$UPDATE_BIN"
        ;;
        *)
            echo "[ERROR] Image check failed!"
            graceful_exit
        ;;
    esac
    rm "$UPDATE_BIN"
    echo "Rebooting now!"
    reboot
}

trb_start()  {
    trb_validation
    trb_prep
    echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    echo " DO NOT TURN OFF YOUR DEVICE DURING THE UPDATE PROCESS!"
    echo "          YOUR DEVICE WILL RESTART ITSELF!             "
    echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    trb_flash
}

ASR_validation() {
    if [ "$TTY_PORT" != "" ] &&
    [ "$LEGACY_MODE" = "0" ]; then
        echo "[WARN] Warning! You specified ttyUSB port but it will not be used for non legacy(fastboot) flash!"
    fi
      
    if [ "$LEGACY_MODE" = "1" ]; then
        echo "[ERROR] ASR modems don't support legacy mode."
        graceful_exit
    fi
    
    setFlasherPath
    
    if [ ! -f "$FLASHER_PATH" ]; then
        echo "[ERROR] Flasher not found. You need to use \"-r\" option to install missing dependencies."
        helpFunction
        graceful_exit
    fi
}

ASR_prep() {
    setFlasherPath
    if [ "$DEVICE" = "MeiglinkASR" ] && [ "$SKIP_VALIDATION" = "0" ]; then
        backupnvr
    fi
    
    if [ "$USER_PATH" = "0" ]; then
        UPDATE_BIN="/tmp/modem_update.bin"
        echo "[INFO] Downloading firmware"

        if [ -f "$UPDATE_BIN" ]; then
            echo "[INFO] Overwriting $UPDATE_BIN"
        fi

        if [ "$DEVICE" = "QuectelASR" ]; then
            curl -Ss --ssl-reqd https://$HOSTNAME/EC200/"$VERSION" \
            --output "$UPDATE_BIN" --connect-timeout 300
        elif [ "$DEVICE" = "MeiglinkASR" ]; then
            curl -Ss --ssl-reqd https://$HOSTNAME/SLM770/"$VERSION" \
            --output "$UPDATE_BIN" --connect-timeout 300    
        else
            echo "[ERROR] Unknown device($DEVICE)."
            graceful_exit
        fi

        if grep -Fq '<head><title>404 Not Found</title></head>' "$UPDATE_BIN"; then
            echo "[ERROR] firmware download failed."
            graceful_exit
        fi
    else
        UPDATE_BIN="$FW_PATH"
        [ "$DEVICE" = "MeiglinkASR" ] && [ ! -f "$FW_PATH" ] && {
            echo "[ERROR] MeiglinkASR modems require path to be a firmware file instead of a directory."
            echo "[INFO] Eg. $0 -p update/firmware.bin"
            graceful_exit
        }
    fi

    #We need to turn on EDL mode via AT command
    if [ "$DEVICE" = "MeiglinkASR" ]; then
        gsmctl ${modem_n:+-N "$modem_n"} -A "AT+MEIGEDL"
        NEED_MODEM_RESTART="1" 
    fi

    stop_services
}

ASR_flash() {
    "$FLASHER_PATH" -f "$UPDATE_BIN"
}

ASR_start() {
    ASR_validation
    ASR_prep
    echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    echo " DO NOT TURN OFF YOUR DEVICE DURING THE UPDATE PROCESS!"
    echo "          YOUR DEVICE WILL RESTART ITSELF!             "
    echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    ASR_flash
}

while [ -n "$1" ]; do
	case "$1" in
		-h) helpFunction
            graceful_exit
        ;;
		-v) shift
            if [ "$1" != "" ]; then
                VERSION="$1"
                DEV_MODULE=$(echo "$1" | cut -c 1-8)
                #DEV_VERSION=$(echo "$VERSION" | tail -c 6 | cut -c1-2)
                debug "[INFO] $1 firmware version selected."
            else
                echo "[ERROR] No firmware version specified! Exiting."
                helpFunction
                graceful_exit
            fi
        ;;
		-n) shift
            if [ "$1" != "" ]; then
                modem_n="$1"
            else
                echo "[ERROR] modem_n not specified."
                helpFunction
                graceful_exit
            fi
        ;;
		-t) shift
            if [ "$1" != "" ]; then
                TTY_PORT="$1"
            else
                echo "[ERROR] tty Port option used but port not specified? This option is needed for legacy mode only."
                helpFunction
                graceful_exit
            fi
        ;;
		-p) shift
			if [ "$1" != "" ]; then
                USER_PATH="1"
                FW_PATH="$1"         
            else
                echo "[ERROR] path not specified."
                helpFunction
                graceful_exit
            fi
        ;;
        -s)
            echo "[INFO] Looking for EDL devices."
            find_modem_edl_device
            echo "[INFO] Done."
            exit 0
        ;;
        -f)
            SKIP_VALIDATION="1"
        ;;
        -D)
            DEBUG_LOG="1"
            DEBUG_ECHO="1"
        ;;
        -g)
            JUST_LIST="1"
        ;;
        -d) shift
            if [ "$1" != "" ]; then
                DEVICE="$1"
            else
                echo "[ERROR] device option used but device not specified?"
                helpFunction
                graceful_exit
            fi
        ;;
        -i) shift
            if [ "$1" != "" ]; then
                modem_usb_id="$1"
                usb_id_to_modem_n
            else
                echo "[ERROR] modem_usb_id option used but modem_usb_id not specified?"
                helpFunction
                graceful_exit
            fi
        ;;
        -l)
            LEGACY_MODE="1"
        ;;
        -r)
            echo "[INFO] This option is longer available as the package is distrubuted via OPKG package manager"
            echo "[INFO] To install this package use: \"opkg update && opkg install sshfs modem_updater\" command"
            graceful_exit
        ;;
		-*)
			helpFunction
            graceful_exit
		;;
		*) break;;
	esac
	shift;
done

if [ "$modem_n" = "" ]; then
    find_modem_n
fi

if [ "$JUST_REQUIREMENTS" = "1" ]; then
    if [ "$PRODUCT_NAME" = "TRB1" ] || [ "$PRODUCT_NAME" = "TRB5" ] &&
    [ "$SKIP_VALIDATION" = "0" ]; then
        echo "[INFO] Not required for $PRODUCT_NAME modems."
    else
        get_requirements
    fi
    graceful_exit
fi

if [ "$JUST_LIST" = "1" ]; then
    get_modems
    graceful_exit
fi

common_validation

if [ "$PRODUCT_NAME" = "TRB1" ] || [ "$PRODUCT_NAME" = "TRB5" ]; then
    trb_start
elif [ "$DEVICE" = "QuectelASR" ] || [ "$DEVICE" = "MeiglinkASR" ]; then
    ASR_start
else
    generic_start
fi

echo "[INFO] Script finished"
graceful_exit
