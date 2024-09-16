#!/bin/sh
. /usr/share/libubox/jshn.sh
DEBUG_LOG="1"
DEBUG_ECHO="0"
SSHFS_PATH="/usr/bin/sshfs"
DEFAULT_FILTER_FILE_PATH="/etc/modem_logger_default.cfg"
LOGGER_PATH="" # Logger selection in set_logger
LOGGERS="qlog diag_saver diag_mdlog"
HOSTNAME="https://modemfota.teltonika-networks.com"
START="0"

PLATFORM="$(cat /etc/board.json | jsonfilter -e "@.model.platform")" # TRB1, TRB5, RUTX....

MOUNT="0"
MOUNT_DIR=""
MOUNT_IP=""
MOUNT_USER=""
MOUNT_PASS=""
MOUNT_RMT_DIR=""

FORCE_LOGGER=""
FORCE_AT_TTY=""

FILTER_PATH=""

DIAG=""
MODEM_ID=""
#Needed if more than one modem. Set by IDtoTTY
LOG_DEV=""

ADD_QUECTEL_AT="" # Adds additional Quectel AT debug commands. See: QUECTEL_AT.
QUECTEL_AT="\
AT+QGMR \
AT+QCFG=\"nwscanmodeex\" \
AT+QCFG=\"nwscanmode\" \
AT+QCFG=\"nwscanseq\" \
AT+QCFG=\"dbgctl\" \
AT+QNVFR=\"nv/item_files/modem/mmode/ue_usage_setting\" \
AT+QCSQ? \
AT+QNWINFO \
AT+QENG=\"neighbourcell\" \
AT+QENG=\"servingcell\" \
AT+QCFG=\"gprsattach\" \
AT+QCFG=\"servicedomain\" \
AT+QCFG=\"band\" \
AT+QCFG=\"multi_ip_package\" \
AT+QCFG=\"ims\" \
AT+QMBNCFG=\"list\" \
"

BASE_AT="\
ATI \
AT+CSUB \
AT+CPIN? \
AT+COPS? \
AT+CREG? \
AT+CGREG? \
AT+CSQ \
AT+CGATT? \
AT+CGDCONT? \
AT+CGACT? \
AT+CGPADDR \
AT+CIMI \
AT+CCID \
AT+GSN \
"

# Executes forced at command by sending it to tty port
exec_forced_at(){
    local at="$1"
    local tty="$2"
    echo "CMD: echo -ne \"$at\r\n\" > $tty"
    echo -ne "$cmd\r\n" > "$tty"
}

# Executes at command by sending it to gsmctl
exec_at(){
    local at="$1"
    local modem="$2"
    echo "CMD: $at ${modem:+-O "$modem"}"
    gsmctl -A $at ${modem:+-O "$modem"}
}

execute_at_array() {
    for cmd in $array; do
        if [ -n "$FORCE_LOGGER" ]; then
            exec_forced_at $cmd $FORCE_AT_TTY
        else
            exec_at $cmd $MODEM_ID
        fi
        sleep 1
    done
}

get_pre_init_set() {
    #space separated array
    array="${ADD_QUECTEL_AT:+"AT+QCFG=\"dbgctl\",0 "} AT+CFUN=0"
}

get_post_init_set() {
    #space separated array
    array="$BASE_AT ${ADD_QUECTEL_AT:+"$QUECTEL_AT"}"
}

debug() {
    if [ "$DEBUG_LOG" = "1" ]; then
        logger -t "modem_logger" "$1"
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
    done
}

verify_modem() {
    [ -n "$FORCE_LOGGER" ] && {
        echo "[INFO] Skipping verify_modem, because of forced logging"
        return
    }

    local rsp="$(exec_at "AT" "$MODEM_ID")"
    case "$rsp" in
        *OK*)
            debug "[INFO] "$MODEM_ID" is responding to AT commands."
        ;;
        *)
            echo "[ERROR] "$MODEM_ID" is wrong or modem is not responding."
            helpFunction
            exit 0
    esac
}

cleanup(){
    [ -e "/usr/bin/diag_mdlog" ] && [ -e "/dev/diag" ] && {
        # Gets current diag dev perms and reverts to previous
        [ "$(ls -l /dev/diag | awk '{print $1}')" == "crw----rw-" ] && {
            echo "[INFO] Reverting \"/dev/diag\" permissions to 600"
            chmod 600 "/dev/diag"
        }
    }
}

stop_logging(){
    echo "[INFO] Trying to stop loggers"
    local stopped=""
    for l in $LOGGERS; do
        [ "$(killall -2 "$l" 2>&1)" == "" ] && stopped="$stopped $l"
    done
    [ "$stopped" == "" ] && echo "[WARNING] Did not stop any logger" || echo "[INFO] Stopped:$stopped"
    cleanup
}

# Checks logger process for open ".qmdl*", ".sdl" and ".logel" log files
started_logging(){
    local PID="$(pidof "$LOGGER_PATH")"
    [ -z "$PID" ] && {
        echo "[ERROR] Logger $LOGGER_PATH process exited unexpectedly."
        echo "[INFO] Run logger directly with [LOGGER_START] to see more info. Exiting."
        exit 1
    }
    local OPEN_FILES=$(ls -l "/proc/$PID/fd" 2>/dev/null | grep -Ei ".*(\.qmdl.*|\.sdl|\.logel)$")
    [ "$OPEN_FILES" != "" ] && return 0
    return 1
}

#Checks ttyUSBx-1 and ttyUSBx-2(in rare cases I have seen this to happen where one tty port skipped)
find_modem_log_port()
{
    local tty_port="$1"
    tty_port_num="${tty_port: -1}"
    tty_port_num=$((tty_port_num - 1))
    LOG_DEV="/dev/ttyUSB$tty_port_num"
    [ -e "$LOG_DEV" ] && return
    tty_port_num=$((tty_port_num - 1))
    LOG_DEV="/dev/ttyUSB$tty_port_num"
    [ -e "$LOG_DEV" ] && return
    echo "[ERROR] Could not find logging port automatically. Exiting."
    exit 1
}

set_logger(){
    # TRB5 and TRB1 uses only diag_mdlog for logging
    [[ "$PLATFORM" =~ "TRB[51]" ]] && [ -e "/usr/bin/diag_mdlog" ] && {
        LOGGER_PATH="/usr/bin/diag_mdlog"
        ADD_QUECTEL_AT="true"
        return
    }

    local modems="$(ubus list gsm.modem* | tr "\n" " ")"
    for modem in $modems; do
        local info="$(ubus call "$modem" info)"
        local model="$(echo "$info" | jsonfilter -e "@.model")"
        local usb_id="$(echo "$info" | jsonfilter -e "@.usb_id")"
        local tty_port="$(echo "$info" | jsonfilter -e "@.tty_port")"
        local manuf="$(echo "$info" | jsonfilter -e "@.manuf")"
        ([ "$MODEM_ID" == "$usb_id" ] || [ "$MODEM_ID" == "" ]) && {
            [ "$manuf" == "Quectel" ] && [ -z "$ADD_QUECTEL_AT" ] && ADD_QUECTEL_AT="true"
            case "$model" in
                SLM770*)
                    LOGGER_PATH="/usr/bin/qlog" # Meig seems to ask for logs taken with qlog and using a filter.
                    #LOGGER_PATH="/usr/bin/diag_saver"
                    #[ -z "$LOG_DEV" ] && find_modem_log_port "$tty_port"
                    return
                ;;
                *)
                    LOGGER_PATH="/usr/bin/qlog"
                    return
                ;;
            esac
        }
    done
    echo "[ERROR] Could not auto-detect required logger. Is gsmd running? Exiting."
    exit 1
}

check_active_loggers(){
    for l in $LOGGERS; do
        local PID=$(pidof "$l")
        [ -n "$PID" ] && {
            echo "---------------------------------------------------------------------------------"
            echo "[WARNING] $l logger with $PID PID is already active and might interfere."
            echo "[INFO] If its not intentional stop it with \"kill ${PID}\" or \"modem_logger -x\""
            echo "---------------------------------------------------------------------------------"
        }
    done
}

wait_till_start(){
    local max_wait=60

    for current_wait in $(seq 1 $max_wait)
    do
        started_logging && return

        # Triggers some log because modem might not respond with anything if in manual mode
        [ -n "$FORCE_LOGGER" ] && {
            exec_forced_at "AT" $FORCE_AT_TTY
        }

        echo "[INFO] Waiting for logger to start ${current_wait}s/${max_wait}s"
        sleep 1
    done

    echo "[ERROR] Failed to start logging in ${max_wait}s. Log file was not created. Exiting."
    killall "$LOGGER_PATH" 2> /dev/null
    cleanup
    exit 1
}

start_logger(){
    case "$LOGGER_PATH" in
        /usr/bin/qlog)
            local QLOG_DEV=""
            [ -n "$MODEM_ID" ] && {
                QLOG_DEV="/sys/bus/usb/devices/$MODEM_ID"
                [ ! -e "$QLOG_DEV" ] && {
                    echo "[ERROR] Modem $MODEM_ID not found in $QLOG_DEV . Please check modem id and try again. Exiting."
                    exit 1
                }
            }

            echo "[LOGGER_START]: $LOGGER_PATH -Dqmdl ${QLOG_DEV:+-p $QLOG_DEV} ${FILTER_PATH:+-f "$FILTER_PATH"} -s $LOG_DIR"
            "$LOGGER_PATH" -Dqmdl ${QLOG_DEV:+-p "$QLOG_DEV"} ${FILTER_PATH:+-f "$FILTER_PATH"} -s "$LOG_DIR" &>/dev/null &
        ;;
        /usr/bin/diag_saver)
            echo "[LOGGER_START]: $LOGGER_PATH" -p "$LOG_DEV" -s "$LOG_DIR"
            "$LOGGER_PATH" -p "$LOG_DEV" -s "$LOG_DIR" &>/dev/null &
        ;;
        /usr/bin/diag_mdlog)
            echo "[INFO]: Creating /sdcard/diag_logs/ folder and setting chmod 757 to it."
            echo "[INFO]: Setting chmod 606 to \"/dev/diag\"."
            echo "[INFO]: Setting chmod 757 to \"$LOG_DIR\"."
            echo "[INFO]: Revert \"/dev/diag\" permissions when logging is finished with \"modem_logger -x\""
            # diag_mdlog needs /sdcard/diag_logs folder to store its pid file and other stuff...
            mkdir -p "/sdcard/diag_logs"
            chmod 757 -R "/sdcard"
            chmod 606 "/dev/diag"
            chmod 757 "$LOG_DIR"
            [[ "$PLATFORM" =~ "TRB[51]" ]] && [ -z "$FILTER_PATH" ] && [ -e "$DEFAULT_FILTER_FILE_PATH" ] && {
                # Sets default.cfg filter file to use if it exists and not already set
                FILTER_PATH="$DEFAULT_FILTER_FILE_PATH"
            }
            echo "[LOGGER_START]: $LOGGER_PATH" ${FILTER_PATH:+-f "$FILTER_PATH"} -o "$LOG_DIR"
            "$LOGGER_PATH" ${FILTER_PATH:+-f "$FILTER_PATH"} -o "$LOG_DIR" &>/dev/null &
        ;;
        *) ;;
    esac

    wait_till_start
}

available_loggers(){
    local err=""
    echo "[INFO] Available loggers:"
    for l in $LOGGERS; do
        [ -f "/usr/bin/$l" ] && err="$err $l"
    done
    [ "$err" == "" ] && echo "[WARNING] No loggers found in /usr/bin/*" || echo "[INFO] Loggers: $err"
}

validate_forced_logger_params() {
    [ -z "$MODEM_ID" ] && [ "$FORCE_LOGGER" == "qlog" ] && {
        echo "[ERROR] Modem id must be provided for forced qlog logging. Exiting."
        echo "[INFO] Example: modem_logger -i <modem_id> -f qlog -t <tty_path> -s <dir>"
        exit 1
    }

    [ -n "$MODEM_ID" ] && (
        [ "$FORCE_LOGGER" == "diag_saver" ] || [ "$FORCE_LOGGER" == "diag_mdlog" ]
    ) && {
        echo "[ERROR] Modem id is not supported on diag_saver and diag_mdlog. Only modem tty port should be provided. Exiting."
        echo "[INFO] Example: modem_logger -f <logger> -t <tty_path> -s <dir>"
        exit 1
    }

    [ -z "$FORCE_AT_TTY" ] && {
        echo "[ERROR] Modem at tty must be provided for forced logging. Exiting."
        exit 1
    }

    [ ! -e "$FORCE_AT_TTY" ] && {
        echo "[ERROR] Modem at tty file not found in $FORCE_AT_TTY . Exiting."
        exit 1
    }
}

start_logging(){
    check_active_loggers
    [ -z "$FORCE_LOGGER" ] && set_logger || {
        validate_forced_logger_params
        LOGGER_PATH="/usr/bin/$FORCE_LOGGER"
    }

    if [ -z "$LOGGER_PATH" ]; then
        echo "[ERROR] Failed to set logger. Exiting."
        available_loggers
        exit 1
    fi

    if [ ! -f "$LOGGER_PATH" ]; then
        echo "[ERROR] Logger not found in $LOGGER_PATH. Exiting."
        available_loggers
        exit 1
    fi

    if [ -n "$FILTER_PATH" ] && [ ! -f "$FILTER_PATH" ]; then
        echo "[ERROR] Filter file not found in $FILTER_PATH. Exiting."
        exit 1
    fi

    [ -n "$MODEM_ID" ] && verify_modem

    [ -n "$FORCE_TTY" ] && [ -z "$LOG_DEV" ] && LOG_DEV="$FORCE_TTY"

    if [ -z "$LOG_DIR" ]; then
        echo "[ERROR] Logging directory was not specified"
        helpFunction
        exit 0
    fi

    [ ! -f "$LOG_DIR" ] && mkdir -p "$LOG_DIR"

    get_pre_init_set
    execute_at_array

    start_logger

    #Primitive check for if log is getting gathered.
    if started_logging; then
        echo "[INFO] Logging started successfully."
    else
        echo "[ERROR] Logging failed to start. .qmdl* or .sdl file not found in $LOG_DIR"
        killall "$LOGGER_PATH" # Kills logger if its failed.
        exit 1
    fi

    array="AT+CFUN=1"
    execute_at_array

    #split into meig and qtel commands later?
    if [ -n "$DIAG" ]; then
        get_post_init_set
        execute_at_array
    fi

    echo "[INFO] Logger will work in the background now. Stop it with \"modem_logger -x\"."
}

exec_mount(){
    SSHFS_RESULT=$(echo "$MOUNT_PASS" | "$SSHFS_PATH" -ossh_command='ssh -y' "$MOUNT_USER"@"$MOUNT_IP":"$MOUNT_RMT_DIR" "$MOUNT_DIR" -o password_stdin)
    debug "[INFO] $SSHFS_RESULT"
    case "$SSHFS_RESULT" in
        *Timeout*)
            echo "[ERROR] Timeout while mounting with sshfs."
            exit 1
        ;;
    esac
}

mount(){
    #Check if sshfs is present
    if [ ! -f $SSHFS_PATH ]; then
        echo "[ERROR] SSHFS not found."
        echo "You can install SSHFS using this command: \"opkg update && opkg install sshfs\""
        echo "Or alternatively using \"-r\" option."
        exit 1
    fi

    #check if all parameters have values
    if [ "$MOUNT_DIR" = "" ] ||
    [ "$MOUNT_IP" = "" ] ||
    [ "$MOUNT_USER" = "" ] ||
    [ "$MOUNT_PASS" = "" ] ||
    [ "$MOUNT_RMT_DIR" = "" ]; then
        echo "Mount parameters were incorrect."
        echo "DIR: $MOUNT_DIR | IP: $MOUNT_IP | USER: $MOUNT_USER | PASS: $MOUNT_PASS | RMT_DIR: $MOUNT_RMT_DIR"
        helpFunction
        exit 1
    fi
    [ -f "$MOUNT_DIR" ] || mkdir -p "$MOUNT_DIR"

    if [ -n "$(ls -A $MOUNT_DIR)" ]; then
        echo "[ERROR] $MOUNT_DIR Not Empty. Unmounting"
        umount "$MOUNT_DIR"
        if [ -n "$(ls -A $MOUNT_DIR)" ]; then
            echo "[ERROR] Unmounting fail. Exiting..."
            exit 1
        fi
    else
        debug "[INFO] Old mount: Empty"
    fi
    exec_mount

}

unmount(){
    echo "unmounting $MOUNT_DIR"
    umount "$MOUNT_DIR"
}

helpFunction() {
    echo "$1 commands:"
    echo -e "\t-r --requirements \t <fwver> Install missing dependencies into tmp folder. (Deprecated)"
    echo -e "\t Use \"-r show\" to show available versions on the server."
    echo -e "\t-m --mount \t <local_dir> <ip> <user> <password> <remote_dir> mount with sshfs."
    echo -e "\t-u --unmount \t <dir> unmount the directory."
    echo -e "\t-s --start \t <dir> Start logging in the specified dir. Deletes existing logs from the directory."
    echo -e "\t-x --stop \t stop logging."
    echo -e "Helpers:"
    echo -e "\t-h --help \t Print help."
    echo -e "\t-g --get_modems\t List available modems."
    echo -e "Parameters:"
    echo -e "\t-d --diag \t Enable diagnostic AT commands when logging starts."
    echo -e "\t-i --id \t <modem_id> Specify modem id, useful if there are multiple modems available."
    echo -e "\t-c --config \t <cfg_path> filter cfg or mask file for logger."
    echo -e "\t-f --force_logger \t <modem_logger> Forces this modem logger to use. ex.: $LOGGERS."
    echo -e "\t-t --at_port \t <tty_path> at tty to send commands to. Required when using forced logger."
    #echo -e "\t-p --tty \t <diag port> Specify modem ttyport, a port from which the logger will collect the logs from (SLM770A only)."
}

while [ -n "$1" ]; do
	case "$1" in
        -s|--start) 
            shift
            LOG_DIR="$1"
            START="1"
        ;;
        -x|--stop)
            stop_logging
            exit 1
        ;;
        -m|--mount)
            MOUNT="1"
            shift
            MOUNT_DIR="$1"
            shift
            MOUNT_IP="$1"
            shift
            MOUNT_USER="$1"
            shift
            MOUNT_PASS="$1"
            shift
            MOUNT_RMT_DIR="$1"
        ;;
        -u|--unmount)
            shift
            MOUNT_DIR="$1"
            unmount
            exit 1
        ;;
        -d|--diag)
            shift
            DIAG="1"
        ;;
        -i|--id)
            shift
            MODEM_ID="$1"
        ;;
        -c|--config)
            shift
            FILTER_PATH="$1"
        ;;
        -f|--force_logger)
            shift
            FORCE_LOGGER="$1"
        ;;
        -t|--at_port)
            shift
            FORCE_AT_TTY="$1"
        ;;
        #-p|--tty)
        #    shift
        #    LOG_DEV="$1"
        #;;
        -g|--get_modems) 
            get_modems
            exit 1
        ;;
        -r|--requirements) 
            echo "Command is no longer supported. Please use the following command to install this package:"
            echo "opkg update && opkg install modem_logger sshfs"
            exit 1
        ;;
		-*)
			helpFunction
			exit 1
		;;
		*) break;;
	esac
	shift;
done

if [ "$START" = "0" ] &&
    [ "$MOUNT" = "0" ]; then
        echo "[ERROR] Not found: ""-start"" or ""-m"". Exiting.."
        helpFunction
        exit 1
fi

[ "$START" = "1" ] && start_logging

[ "$MOUNT" = "1" ] && mount
