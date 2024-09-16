#!/bin/sh

. /lib/functions.sh

CRONTAB_FILE=/etc/crontabs/root

check_rules() {
    config_get enabled "$1" "enable" "0"
    [ "$enabled" -ne 1 ] && return

    config_set "$1" "current_try" "0"
    config_get time "$1" "time" "0"

    case "${time}" in
    "30")
        echo "0,30 * * * * /usr/sbin/ping_reboot.sh $1 " >>${CRONTAB_FILE}
        ;;
    "60")
        echo "0 */1 * * * /usr/sbin/ping_reboot.sh $1 " >>${CRONTAB_FILE}
        ;;
    "120")
        echo "0 */2 * * * /usr/sbin/ping_reboot.sh $1 " >>${CRONTAB_FILE}
        ;;
    *)
        echo "*/$time * * * * /usr/sbin/ping_reboot.sh $1 " >>${CRONTAB_FILE}
        ;;
    esac
}

config_load 'ping_reboot'
config_foreach check_rules 'ping_reboot'
/etc/init.d/cron restart
