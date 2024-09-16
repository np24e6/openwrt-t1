include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=sim_idle_protection

PKG_SOURCE_VERSION:=1.0

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=sim idle protection
	ifeq (m, $(CONFIG_PACKAGE_$(PKG_NAME)))
		PKG_TLT_NAME:=SIM Idle Protection
		PKG_ROUTER:=$(TLT_PLATFORM_NAME)
	endif
endef

define Package/$(PKG_NAME)/teltonika_rutx
	PKG_ROUTER:=RUTX09 RUTX11 RUTXR1 RUTX14 RUTX50
endef

define Package/$(PKG_NAME)/teltonika_rutm
	PKG_ROUTER:=RUTM09 RUTM11 RUTM14 RUTM50 RUTM51
endef

define Package/$(PKG_NAME)/description
	Keep your SIM card alive when is not in use
endef

define Package/$(PKG_NAME)/conffiles
/etc/config/sim_idle_protection
endef


define Package/sim_idle_protection/postinst
	#!/bin/sh
	. /usr/share/libubox/jshn.sh
	. /lib/functions.sh

	CFG=/etc/board.json

	[[ -s $$CFG ]] || /bin/board_detect || return 1

	[[ -s "/etc/config/sim_idle_protection" ]] && return 0

	touch /etc/config/sim_idle_protection

	add_sim_idle_protection_config() {
		local device="$$1"
		local position="$$2"

		[[ -z "$$position" ]] && return 0

		uci_add sim_idle_protection sim_idle_protection
		uci_set sim_idle_protection $$CONFIG_SECTION enable 0
		uci_set sim_idle_protection $$CONFIG_SECTION period month
		uci_set sim_idle_protection $$CONFIG_SECTION day 1
		uci_set sim_idle_protection $$CONFIG_SECTION hour 1
		uci_set sim_idle_protection $$CONFIG_SECTION min 0
		uci_set sim_idle_protection $$CONFIG_SECTION packet_size 56
		uci_set sim_idle_protection $$CONFIG_SECTION count 2
		uci_set sim_idle_protection $$CONFIG_SECTION host 8.8.8.8
		uci_set sim_idle_protection $$CONFIG_SECTION ip_type ipv4
		uci_set sim_idle_protection $$CONFIG_SECTION modem "$$device"
		uci_set sim_idle_protection $$CONFIG_SECTION position "$$position"

		uci_commit sim_idle_protection
	}

	json_init
	json_load_file "$${CFG}"
	json_get_var modems modems

	sim_count="0"

	if json_is_a modems array; then
		json_get_keys modems modems
		json_select modems

		for modem in $$modems; do
			json_select "$$modem"
			json_get_vars id simcount builtin
			while [[ $$sim_count != $$simcount ]]; do
				add_sim_idle_protection_config "$$id" "$$((sim_count + 1))"
				sim_count=$$((sim_count + 1))
			done
			json_select ..
		done

		[[ -x "/bin/trigger_vuci_routes_reload" ]] && /bin/trigger_vuci_routes_reload
	fi
endef

define Package/$(PKG_NAME)/prerm
    #!/bin/sh
	[[ -s "/etc/config/sim_idle_protection" ]] && rm -f /etc/config/sim_idle_protection
	[[ -x "/bin/trigger_vuci_routes_reload" ]] && /bin/trigger_vuci_routes_reload
endef


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_DIR) $(1)/sbin/
	$(INSTALL_DIR) $(1)/etc/config/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/sim_idle_protection.init $(1)/etc/init.d/sim_idle_protection
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/sim_idle_protection.sh $(1)/sbin/sim_idle_protection.sh
endef

$(eval $(call BuildPackage,sim_idle_protection))
