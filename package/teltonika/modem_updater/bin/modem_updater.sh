#!/bin/ash
. /usr/share/libubox/jshn.sh

### Static defines

HOSTNAME="modemfota.teltonika-networks.com"
SSH_PASS="u3qQo99duKeaVWr7"
SSHFS_PATH="/usr/bin/sshfs"
DEVICE_PATH="/sys/bus/usb/devices/"

##############################################################################

### Flasher names
MEIG_FLASHER="meig_firehose"
MEIG_ASR_FLASHER="fbfdownloader"
QUECTEL_FLASHER="quectel_flash"
QUECTEL_ASR_FLASHER="QDLoader"

##############################################################################

### Dynamic variables

PRODUCT_NAME=$(mnf_info -n | cut -b 1-4)

DEBUG_LOG="1"
DEBUG_ECHO="0"

DEVICE=""
SKIP_VALIDATION="0"
FW_PATH="/tmp/firmware/"
FLASHER_PATH=""
LEGACY_MODE="0"
TTY_PORT=""
MODEM_N=""
MODEM_USB_ID=""
JUST_LIST="0"
USER_PATH="0"
VERSION=""
NEED_MODEM_RESTART="0"
NEED_SERVICE_RESTART="0"

##############################################################################

### General Utils

debug() {
    if [ "$DEBUG_LOG" = "1" ]; then
        logger -t "modem_updater" "$1"
    fi
    if [ "$DEBUG_ECHO" = "1" ]; then
        echo "$1"
    fi
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

validate_connection() {
    if ping -q -c 1 -W 5 8.8.8.8 >/dev/null; then
        debug "[INFO] internet connection is working"
        CONNECTION_STATUS="ACTIVE"
    else
        echo "[ERROR] internet connection is not active"
        CONNECTION_STATUS="INACTIVE"
    fi
}

restarted="0"
graceful_exit() {

    [ "$NEED_SERVICE_RESTART" = "1" ] && sleep 10 && start_services
    # If we don't need to restart we just exit here
    [ "$NEED_MODEM_RESTART" = "0" ] && exit 0
    # Skip validation was used, we are done here
    [ "$SKIP_VALIDATION" = "0" ] && exit 0

    # Wait for modem to turn ON, if it doesn't restart it
    # Most modems will power by themselves
    retries=10
    live=0
    echo "[INFO] Waiting for modem: $MODEM_N"
    while [ $retries != 0 ]; do
        echo "."
        retries=$((retries-1))
        is_modem_live
        [ "$live" = "1" ] && echo "Modem is responding to AT commands" && exit 0

        sleep 5
    done

    result=$(mctl -r -m "$MODEM_N")
    [ "$result" = "Unable to find specified modem" ] && {
        echo "[INFO] Restart failed. Trying to restart default modem"
        mctl -r

        [ "$restarted" = "0" ] && {
            restarted=1
            NEED_SERVICE_RESTART=0
            graceful_exit
        }
        echo "Looks like modem is not responsive. A system reboot may be required."
        exit 0
    }
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

##############################################################################

### Modem generic functions

is_modem_live() {
    result=$(gsmctl ${MODEM_N:+-N "$MODEM_N"} -A "AT")
    case "$result" in
    *OK*)
        live="1"
        return
        ;;
    esac
    live="0"
}

find_modem_edl_device() {
    sys_path=""
    for entry in "$DEVICE_PATH"/*
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

get_modems() {
    echo "Modem List:"
    modem_array="$(ubus list gsm.modem* | tr "\n" " ")"
    for s in $modem_array ; do
        MODEM_N="${s: -1}"
        modem_info="$(ubus call "$s" info)"
        builtin="$(echo "$modem_info" | grep "builtin" | tr -d ' ,\"\t')"
        exec_ubus_call "$MODEM_N" "get_firmware"
        fwver=$(parse_from_ubus_rsp "firmware")
        usbid="$(echo "$modem_info" | grep "usb_id" | cut -d'"' -f 4)"
        name="$(echo "$modem_info" | grep "name" | cut -d'"' -f 4)"
        echo "[$MODEM_N] $name | USB_ID: $usbid | Firmware: $fwver | $builtin"
        get_fw_list
    done
}

verify_MODEM_N() {
    if [ "$MODEM_N" = "" ]; then
        echo "[ERROR] MODEM_N is not set. Please use \"-i\" or \"-u\" option."
        helpFunction
        graceful_exit
    fi
    exec_ubus_call "$MODEM_N" "exec" "{\"command\":\"AT\"}"
    #AT_RESULT=$(ubus call gsm.modem"$MODEM_N" exec {\"command\":\"AT\"}) > /dev/null 2>&1
    case "$ubus_rsp" in
        *OK*)
            debug "[INFO] $MODEM_N is responding to AT commands."
        ;;
        *)
            echo "[ERROR] MODEM_N is wrong or modem is not responding."
            helpFunction
            graceful_exit
    esac
}

get_MODEM_USB_ID() {
    exec_ubus_call "$1" "info"
    MODEM_USB_ID=$(parse_from_ubus_rsp "usb_id")
    echo "[INFO] MODEM_USB_ID: $MODEM_USB_ID"
}

usb_id_to_MODEM_N() {
    modem_array="$(ubus list gsm.modem* | tr "\n" " ")"
    for s in $modem_array ; do
        modem_info="$(ubus call "$s" info)"
        usbid="$(echo "$modem_info" | grep "usb_id" | cut -d'"' -f 4)"
        [ "$usbid" != "$MODEM_USB_ID" ] && continue
        MODEM_N="${s: -1}"
        break
    done
    echo "[INFO]found MODEM_N: ""$MODEM_N"""
}

find_modem_n() {
    local get_any="$1"
    modem_array="$(ubus list gsm.modem* | tr "\n" " ")"
    if [ "$(echo "$modem_array" | wc -w)" = 1 ]; then
        MODEM_N="${modem_array: -2}"
        echo "[INFO] Detected MODEM_N: ""$MODEM_N"""
    elif [ "$get_any" != "" ]; then
        MODEM_N="${modem_array: -2}"
        echo "[INFO] Detected MODEM_N: ""$MODEM_N"""
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

##############################################################################

### SSHFS HELPERS

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

##############################################################################

### Meiglink specific functions

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

retry_backup=0
backupnvr() {
    debug "[INFO] Meiglink device lets backup NVRAM"
    NV_RESULT=$(gsmctl -N "$MODEM_N" -A AT+NVBURS=2)
    exec_ubus_call "$MODEM_N" "info"
    nvresultcheck
    #if no backup exists then we make one.
    if [ $NV_FAILED = 1 ]; then
        NV_RESULT=$(gsmctl -N "$MODEM_N" -A AT+NVBURS=0)
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

##############################################################################

### Flasher functions

setDevice() {
    if [ "$MODEM_N" = "" ]; then
        echo "[ERROR] Failed to find modem."
        exit 1
    fi

    exec_ubus_call "$MODEM_N" "info"
    DEVICE=$(parse_from_ubus_rsp "manuf")
    if [ "$DEVICE" = "N/A" ]; then
        echo "[ERROR] Device manufacturer not properly recognized. Exiting.."
        helpFunction
        graceful_exit
    fi

    # Change Quectel to QuectelASR here
    if [ "$DEVICE" = "Quectel" ]; then
        exec_ubus_call "$MODEM_N" "get_firmware"
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
        exec_ubus_call "$MODEM_N" "get_firmware"
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

    if [ "$MODEM_N" != "" ]; then
        verify_MODEM_N
    fi

    exec_ubus_call "$MODEM_N" "get_firmware"
    MODEM=$(parse_from_ubus_rsp "firmware")

    if [ "$DEVICE" = "Quectel" ]; then
        MODEM_SHORT=$(echo "$MODEM" | cut -b -8)
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
    elif [[ $MODEM =~ "RG520NEB" ]]; then
        get_compatible_fw_list "RG520NEB/fwlist.txt"
    else
        get_compatible_fw_list "fwlist.txt"
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

##############################################################################

### Common validation method (used for all devices)

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

    if [ "$MODEM_N" = "" ]; then
        #TODO: Get it automatically. If cant then or multiple then error.
        find_modem_n
        if [ "$MODEM_N" = "" ] && [ "$SKIP_VALIDATION" = "0" ]; then
            echo "[ERROR] Cannot find modem_n automatically. "
            helpFunction
            graceful_exit
        fi
    fi

    if [ "$SKIP_VALIDATION" = "0" ]; then
        verify_MODEM_N
    fi

    if [ "$MODEM_USB_ID" = "" ]; then
        get_MODEM_USB_ID "$MODEM_N"
        if [ "$MODEM_USB_ID" = "" ]; then
            echo "[ERROR] Could not get MODEM_USB_ID automatically."
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
        exec_ubus_call "$MODEM_N" "get_firmware"
        MODEM_FW=$(parse_from_ubus_rsp "firmware")
        case "$MODEM_FW" in
            *$VERSION* )
                echo "[ERROR] Specified firmware is already installed. Exiting.."
                graceful_exit
            ;;
        esac
        #Quectel
        if [ "$DEVICE" = "Quectel" ]; then
            exec_ubus_call "$MODEM_N" "get_firmware"
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
            exec_ubus_call "$MODEM_N" "get_firmware"
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

##############################################################################

### Generic methods (Mainly for Qualcomm devices)

generic_validation() {
    #User has specified tty port for non legacy mode (it will not be used)
    if [ "$TTY_PORT" != "" ] &&
    [ "$LEGACY_MODE" = "0" ]; then
        echo "[WARN] Warning you specified ttyUSB port but it will not be used for non legacy(fastboot) flash!"
    fi

    #Legacy mode is only for Quectel modems
    if [ "$LEGACY_MODE" = "1" ] && [ "$DEVICE" != "Quectel" ]; then
        echo "[ERROR] Legacy mode only supported for Quectel routers."
        graceful_exit
    fi

    #Legacy mode requires specified ttyUSB port
    if [ "$LEGACY_MODE" = "1" ] && [ "$DEVICE" = "Quectel" ] && [ "$TTY_PORT" = "" ]; then
        echo "[ERROR] ttyUSB port not specified. The detection will be done by QFlash."
    fi

    setFlasherPath

    if [ ! -f "$FLASHER_PATH" ]; then
        echo "[ERROR] Flasher not found. You need to use \"-r\" option to install missing dependencies."
        helpFunction
        graceful_exit
    fi
}

generic_prep() {
    /etc/init.d/modem_trackd stop
    NEED_SERVICE_RESTART="1"

    #backup NVRAM
    #it is done in the flasher now, but just in case..
    if [ "$DEVICE" = "Meiglink" ] && [ "$SKIP_VALIDATION" = "0" ]; then
        backupnvr
    fi

    #go into edl/disable mobile connection.
    if [ $LEGACY_MODE = "0" ]; then
        if [ "$DEVICE" = "Quectel" ]; then
            $FLASHER_PATH qfirehose -x ${MODEM_USB_ID:+-s /sys/bus/usb/devices/"$MODEM_USB_ID"}
        elif [ "$DEVICE" = "Meiglink" ]; then
            $FLASHER_PATH -x ${MODEM_USB_ID:+-s /sys/bus/usb/devices/"$MODEM_USB_ID"}
        fi
    else
        gsmctl ${MODEM_N:+-N "$MODEM_N"} -A "AT+CFUN=0"
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
        echo "[ERROR] No firehose folder found. Either the firmware is not right or you must use legacy mode (-l option)"
        if [ "$USER_PATH" = "0" ]; then
            umount "$FW_PATH"
        fi
        helpFunction
        graceful_exit
    fi

    stop_services
}

generic_flash() {
    if [ "$DEVICE" = "Meiglink" ]; then
        $FLASHER_PATH -d -f "$FW_PATH" ${MODEM_USB_ID:+-s /sys/bus/usb/devices/"$MODEM_USB_ID"}
    fi

    if [ "$DEVICE" = "Quectel" ]; then
        if [ $LEGACY_MODE = "1" ]; then
            #fastboot
            $FLASHER_PATH -v -m1 -f "$FW_PATH" ${TTY_PORT:+-p "$TTY_PORT"}
        else
            #firehose
            find_modem_edl_device
            [ "$sys_path" != "" ] && confirm_modem_usb_id "$sys_path" "$MODEM_USB_ID"
            if [ "$confirm_modem_usb_id_result" == "0" ]; then
                $FLASHER_PATH qfirehose -n -f "$FW_PATH/update/firehose/" ${sys_path:+-s $sys_path}
            else #"" and "1"
                $FLASHER_PATH qfirehose -n -f "$FW_PATH/update/firehose/" ${MODEM_USB_ID:+-s /sys/bus/usb/devices/"$MODEM_USB_ID"}
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

generic_start() {
    generic_validation
    generic_prep
    echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    echo " DO NOT TURN OFF YOUR DEVICE DURING THE UPDATE PROCESS!"
    echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    echo "Starting flasher..."
    generic_flash
}

##############################################################################

### TRB modem specific flashing functions

trb_validation() {
    if [ "$USER_PATH" = "1" ]; then
        echo "Please use modem_upgrade directly."
        graceful_exit
    fi
}

trb_prep() {
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

trb_flash() {
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

trb_start() {
    trb_validation
    trb_prep
    echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    echo " DO NOT TURN OFF YOUR DEVICE DURING THE UPDATE PROCESS!"
    echo "          YOUR DEVICE WILL RESTART ITSELF!             "
    echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    trb_flash
}

##############################################################################

### ASR modem flashing function

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
        gsmctl ${MODEM_N:+-N "$MODEM_N"} -A "AT+MEIGEDL"
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

##############################################################################

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
    printf "\t-n \t <MODEM_N> Modem ID(number). eg.: 0, 1, 2...\n"
    printf "\t-i \t <MODEM_N> Modem USB ID. eg.: 1-1 3-1 etc.\n"
    printf "\t-f \t Force upgrade start without extra validation. USE AT YOUR OWN RISK.\n"
    printf "\t-l \t Legacy mode for quectel modems(Fastboot).\n"
    printf "\t-d \t <name> Manually set device Vendor (Quectel, QuectelASR, QuectelUNISOC or Meiglink, MeiglinkASR).\n"
    printf "\t-t \t <ttyUSBx> ttyUSBx port(for legacy mode).\n"
    printf "\t-D \t debug\n"
}

### Entry point

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
                MODEM_N="$1"
            else
                echo "[ERROR] MODEM_N not specified."
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
                MODEM_USB_ID="$1"
                usb_id_to_MODEM_N
            else
                echo "[ERROR] MODEM_USB_ID option used but MODEM_USB_ID not specified?"
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

if [ "$MODEM_N" = "" ]; then
    [ "$SKIP_VALIDATION" = "0" ] && find_modem_n
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
