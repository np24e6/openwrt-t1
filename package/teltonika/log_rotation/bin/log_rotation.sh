#!/bin/sh

EMPTY=1
NO_FILE=2
NO_SYSTEM=3
OUT_OF_BOUNDS=4
MISSING_PARAM=5

SYS_SIZE_MIN=100000 
SYS_SIZE_MAX=500000
LOG_SIZE_MIN=10000
LOG_SIZE_MAX=512000
TIME_MIN=1
TIME_MAX=168

COMPRESS=0

print_help () {
        cat <<EOF
Usage: $0 [options] {-s N | -t N | -s N -t N} [additional]

Manage log file rotation by maximum allowed active log file's size and optionally by defined
time interval. By default rotation is performed when active log file's size reaches 200kB.
When rotation is performed, active log's content is copied to a new file <log-name>.<number>,
where <number> is next in line old log's number, while active log's content is deleted.
If defined, old log is compressed as a GZ file.

It is mandatory to define a mount point in which logs will be stored. When on mount point
there is less than 200kB or (if provided) defined free space left, old logs will be deleted
until enough space is available.

Options:
        -l | --log LOG          Name of a log file
        -m | --mount SYSTEM     Name of a mount point in which logs will be stored
Actions:
        -s | --log-size N       Maximum allowed size N of a log file (in bytes) until log
                                rotation is performed. Must be at least 10kB and at most 512kB
        -t | --time N           Time interval N (in hours) after which log rotation is performed.
                                Must be at least 1h and at most 168h (one week)
Additional:
        -S | --system-size N    Minimum size N of free space (in bytes) to be left for a system.
                                Must be at least 100kB and at most 500kB. Default value is 200kB
        -c | --compress         Compress old logs using gzip. By default old logs are not compressed
EOF
}

rotate_log() {
        log_dir=$1
        log_name=$2

        touch "${log_dir}/${log_name}.lock"

        newest_ver=$(ls -v "$log_dir" | grep -E -i "${log_name}.[0-9]+($|.gz)" \
                     | tail -1 | awk '{split($0,array,"."); print array[2]}')
        newest_ver=$((newest_ver+1))

        cp "${log_dir}/${log_name}" "/tmp/${log_name}.${newest_ver}"
        > "${log_dir}/${log_name}"
        mv "/tmp/${log_name}.${newest_ver}" "$log_dir"

        rm "${log_dir}/${log_name}.lock"

        [ "$COMPRESS" -eq 1 ] && {
                gzip "${log_dir}/${log_name}.${newest_ver}"
        }
}

check_file_size() {
        log_dir=$1
        log_name=$2
        log_size_limit=$3

        log_size=$(ls -l "${log_dir}/${log_name}" | awk '{print $5}')

        [ "$log_size" -ge "$log_size_limit" ] && {
                while [ -f "${log_dir}/${log_name}.lock" ]; do
                        sleep 1
                done
                rotate_log "$log_dir" "$log_name"
        }
}

check_system_size() {
        log_dir=$1
        log_name=$2
        system=$3
        system_size_limit=$4
        counted_limit=0

        system_size=$(df | grep -E "()${system}$" | awk '{print $4}')
        system_size=$((system_size * 1000))

        [ "$system_size" -le "$system_size_limit" ] && {
                counted_limit=$((system_size_limit - system_size + 50000))
                while [ "$counted_limit" -gt 0 ]; do
                        oldest=$(ls -v "$log_dir" | grep -E -i "${log_name}.[0-9]+($|.gz)" | head -1)
                        oldest_size=$(ls -l "$log_dir" | grep -E "()$oldest$" | awk '{print $5}')
                        rm "${log_dir}/${oldest}" 2> /dev/null
                        counted_limit=$((counted_limit - oldest_size))
                done
        }
}

run_size_monitoring() {
        log_dir=$1
        log_name=$2
        log_size_limit=$3
        system=$4
        system_size_limit=$5

        while true; do
                sleep 1
                check_file_size "$log_dir" "$log_name" "$log_size_limit"
                check_system_size "$log_dir" "$log_name" "$system" "$system_size_limit"
        done
}

run_timer() {
        log_dir=$1
        log_name=$2
        time=$3

        while true; do
                sleep "${time}h"
                while [ -f "${log_dir}/${log_name}.lock" ]; do
                        sleep 1
                done
                rotate_log "$log_dir" "$log_name"
        done
}

validate_input() {
        option=$1
        input=$2

        case "$option" in
                "$EMPTY")
                        [ -z "$input" ] && {
                                echo "'${3}' value is empty"
                                exit "$EMPTY"
                        }
                        ;;
                "$NO_FILE")
                        [ ! -f "$input" ] && {
                                echo "File '${input}' does not exist"
                                exit "$NO_FILE"
                        }
                        ;;
                "$NO_SYSTEM")
                        system=$(df | grep -E "()${input}$")
                        [ -z "$system" ] && {
                                echo "Mount point '${input}' does not exist"
                                exit "$NO_SYSTEM"
                        }
                        ;;
                "$OUT_OF_BOUNDS")
                        echo "$input" | grep -E "^[0-9]+$" > /dev/null
                        [ "$?" -ne 0 ] && {
                                echo "'${input}' is not a positive integer"
                                exit "$OUT_OF_BOUNDS"
                        }
                        min=$3
                        max=$4
                        [ "$input" -lt "$min" ] || [ "$input" -gt "$max" ] && {
                                echo "'${input}' out of bounds - must be between ${min} and ${max}"
                                exit "$OUT_OF_BOUNDS"
                        }
        esac
}

[ -z "$1" ] && {
        print_help
        exit 0
}

log_dir=
log_name=
system=
system_size_limit=200000
log_size_limit=200000
time=

while [ "$1" ]; do
        case "$1" in
                -l|--log)
                        validate_input "$EMPTY" "$2" "log"
                        validate_input "$NO_FILE" "$2"
                        log_dir=$(dirname "$2")
                        log_name=$(basename "$2")
                        shift 2
                        ;;
                -m|--mount)
                        validate_input "$EMPTY" "$2" "mount"
                        validate_input "$NO_SYSTEM" "$2"
                        system=$2
                        shift 2
                        ;;
                -s|--log-size)
                        validate_input "$EMPTY" "$2" "log-size"
                        validate_input "$OUT_OF_BOUNDS" "$2" "$LOG_SIZE_MIN" "$LOG_SIZE_MAX"
                        log_size_limit=$2
                        shift 2
                        ;;
                -t|--time)
                        validate_input "$EMPTY" "$2" "time"
                        validate_input "$OUT_OF_BOUNDS" "$2" "$TIME_MIN" "$TIME_MAX"
                        time=$2
                        shift 2
                        ;;
                -S|--system-size)
                        validate_input "$EMPTY" "$2" "system-size"
                        validate_input "$OUT_OF_BOUNDS" "$2" "$SYS_SIZE_MIN" "$SYS_SIZE_MAX"
                        system_size_limit=$2
                        shift 2
                        ;;
                -c|--compress)
                        COMPRESS=1
                        shift
                        ;;
                --help|-h|*)
                        print_help
                        exit 0
                        ;;
        esac
done

[ -z "$log_name" ] || [ -z "$system" ] && {
        echo "Log name or mount point not provided"
        exit "$MISSING_PARAM"
}

[ -n "$time" ] && {
        run_timer "$log_dir" "$log_name" "$time" &
}

run_size_monitoring "$log_dir" "$log_name" "$log_size_limit" "$system" "$system_size_limit"
