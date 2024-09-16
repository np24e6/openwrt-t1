#!/bin/sh

. /lib/functions.sh

[ -e "/etc/config/frr" ] || exit 0

clean_bgp=0
clean_rip=0
clean_ospf=0
clean_eigrp=0
clean_nhrp=0

clean_svc() {
	eval "[ \"\$clean_$1\" = 0 ]" && [ -f "/etc/config/$1" ] || return
	rm "/etc/config/$1"
	eval "clean_$1=1"
}

config_cb() {
	local type="$1"
	local name="$2"
	case "$type" in
		bgp_general|bgp_instance)
			service="bgp"
			clean_svc "$service"
			;;
		peer|peer_group|access_list|route_maps|route_map_filters)
			type="bgp_$type"
			service="bgp"
			clean_svc "$service"
			;;
		rip_general|rip_access_list)
			service="rip"
			clean_svc "$service"
			;;
		interface)
			type="rip_$type"
			service="rip"
			clean_svc "$service"
			;;
		ospf)
			type="${type}_general"
			service="ospf"
			clean_svc "$service"
			;;
		ospf_interface|ospf_neighbor|ospf_area|ospf_network)
			service="ospf"
			clean_svc "$service"
			;;
		eigrp_general)
			service="eigrp"
			clean_svc "$service"
			;;
		nhrp_general|nhrp_instance|*"_map")
			service="nhrp"
			clean_svc "$service"
			;;
		*)
			service=""
			;;
	esac

	migrate_config $type $2 $service config
	option_cb() {
		migrate_config $1 $2 $service option
	}
	list_cb() {
		migrate_config $1 $2 $service list
	}
}

migrate_config(){
	local option="$1"
	local value="$2"
	local service="$3"
	local cfg_type="$4"

	[ -z "$service" ] && return 1

	case "$4" in
		config)
			echo "" >> "/etc/config/$3"
			if [ "${value#*cfg}" != "$value" ]; then
				echo "config $option" >> "/etc/config/$3"
			else
				echo "config $option '$value'" >> "/etc/config/$3"
			fi
			;;
		option)
			echo -e "\toption $option '$value'" >> "/etc/config/$3"
			;;
		list)
			echo -e "\tlist $option '$value'" >> "/etc/config/$3"
			;;
	esac
}

config_load frr
rm /etc/config/frr
