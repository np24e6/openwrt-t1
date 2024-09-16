
. /lib/functions.sh

PLUGIN_FOUND=0

plugin_find_cb() {
    local sec="$1"
    local name="$2"
    local plugin

    config_get plugin "$sec" plugin
    [ -z "$plugin" ] && return

    [ "$plugin" = "$name" ] && PLUGIN_FOUND=1
}

ds_find_plugin() {
    local name="${1}"

    [ -z "$name" ] && return

	config_load data_sender
	config_foreach plugin_find_cb input "$name"

    [ "$PLUGIN_FOUND" -eq 1 ] && {
        PLUGIN_FOUND=0
        
        return 0
    }

    return 1
}
