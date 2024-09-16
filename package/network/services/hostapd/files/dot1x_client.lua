#!/usr/bin/env lua
require "ubus"
require "uloop"
local util = require("vuci.util")
local fs = require("nixio.fs")
local board = require("vuci.board")
local family_name = board:get_family_name()
local has_dsa = board:has_dsa()
local uci = require("vuci.uci").cursor()

local CONFIG_DIR = "/tmp/dot1x_client/"

local conn = ubus.connect()
uloop.init()
if not conn then
	error("Failed to connect to ubus")
end

local port_tables = {
	forward_table = {},
	reverse_table = {},
	vlan_mapping = {},
	bridged_ports = {}
}

local dot1x_clients = { }

local function in_table(table, value, key)
	for k, v in pairs(table) do
		if key and k == value then return true end
		if not key and v == value then return true end
	end
	return false
end

local function get_port_physical_iface(port)
	if string.match(family_name, "^TAP") then return "br-lan" end
	if has_dsa then port = port_tables.forward_table[port] end
	local result = port_tables.vlan_mapping[port] or (has_dsa and port or false)
	return result
end

local function reload_port_wpa_supplicant(port)
	port = string.lower(port)
	local iface = port_tables.reverse_table[port]
	local phy = get_port_physical_iface(iface)
	if not phy then return end
	if dot1x_clients[iface].enabled ~= "1" then return end
	local obj_name = "wpa_supplicant."..phy
	if not in_table(conn:objects(), obj_name) then return end
	local timer = uloop.timer(function()
		conn:call(obj_name, "reload", {})
	end)
	timer:set(1000) -- give time for port to negotiate speed and be usable
end

local function parse_config(client)
	if client.enabled ~= "1" then return false end
	local config = {key_mgmt="IEEE8021X", eapol_flags="0"}
	local function add_string(cfg, option, value, path)
		if not value then return end
		if path and not fs.access(value) then return end
		cfg[option] = string.format('"%s"', value)
	end

	config.eap = string.upper(client.auth_type)
	local features = {
		identity           = {cfg_name = "identity", used_in = {"md5", "tls", "pwd", "ttls", "peap"}},
		anonymous_identity = {cfg_name = "anonymous_identity", used_in = {"ttls", "peap"}},
		ca_cert            = {cfg_name = "ca_cert", used_in = {"tls", "ttls", "peap"}, file = true},
		private_key        = {cfg_name = "private_key", used_in = {"tls"}},
		private_key_passwd = {cfg_name = "private_key_pass", used_in = {"tls"}},
		client_cert        = {cfg_name = "client_cert", used_in = {"tls"}},
		password           = {cfg_name = "password", used_in = {"md5", "pwd", "ttls", "peap"}},
	}

	for key, info in pairs(features) do
		if in_table(info.used_in, client.auth_type) then
			add_string(config, key, client[info.cfg_name], info.file)
		end
	end

	local ttls_phase2_map = {
		["MSCHAPV2"]      = '"autheap=MSCHAPV2"',
		["MD5"]           = '"autheap=MD5"',
		["GTC"]           = '"autheap=GTC"',
		["MSCHAPV2NOEAP"] = '"auth=MSCHAPV2"',
		["PAP"]           = '"auth=PAP"',
		["MSCHAP"]        = '"auth=MSCHAP"',
		["CHAP"]          = '"auth=CHAP"'
	}

	-- too custom to add through the features table
	if client.auth_type == "ttls" then
		config.phase2 = ttls_phase2_map[client.inner_authentication]
	elseif client.auth_type == "peap" then
		config.phase2 = string.format('"auth=%s"', client.inner_authentication)
		config.phase1 = string.format('"peapver=%s"', client.peap_version)
	end

	return config
end

local old_config = {}

local function write_config_to_file(file, config)
	local config_string = "network={\n"
	for key, value in pairs(config) do
		if not string.match(key, "^_priv_") then
			config_string = config_string..string.format("	%s=%s\n", key, value)
		end
	end
	config_string = config_string.."}\n"
	fs.writefile(file, config_string)
end

local ERROR_CODES = {
	NO_DEVICE = 1,
	DEVICE_USED = 2,
	DEVICE_USED_SAME_CONFIG = 3,
	SUCCESS = 4
}

local statuses = {}

local function generate_configuration()
	statuses = {}
	os.execute("rm -fr "..CONFIG_DIR)
	fs.mkdirr(CONFIG_DIR)
	local configs = {}
	local locks = {}
	local wpa_supplicants = {}
	uci:foreach("dot1x", "client", function(client)
		local config = parse_config(client)
		local name = client[".name"]
		local phy = get_port_physical_iface(name)
		if not phy then
			statuses[name] = {code = ERROR_CODES.NO_DEVICE}
			configs[name] = nil
			return
		end
		if client.enabled ~= "1" then
			if not locks[phy] or locks[phy] == name then
				locks[phy] = false
			end
			return
		end
		config._priv_br = in_table(port_tables.bridged_ports, phy)
		config._priv_phy = phy
		configs[name] = config
		local old_cfg = old_config[name]
		if not locks[phy] or locks[phy] == name then
			locks[phy] = name
			statuses[name] = {code = ERROR_CODES.SUCCESS, owner = name, phy = phy}
		else
			local owner = configs[locks[phy]]
			if not owner or not util.deep_compare(owner, config) then
				statuses[name] = {code = ERROR_CODES.DEVICE_USED, owner = locks[phy]}
			else
				statuses[name] = {code = ERROR_CODES.DEVICE_USED_SAME_CONFIG, owner = locks[phy]}
			end
			return
		end
		local cfg_path = CONFIG_DIR..phy
		write_config_to_file(cfg_path, config)
		wpa_supplicants[phy] = { driver = "wired", iface = phy, config = cfg_path, bridge = config._priv_br and phy or nil, reload = not old_cfg or not util.deep_compare(old_cfg, config) }
	end)

	for _, cfg in pairs(old_config or {}) do
		if not in_table(locks, cfg._priv_phy, true) then
			wpa_supplicants[cfg._priv_phy] = false
		end
	end
	for phy, owner in pairs(locks) do
		local iface_state = conn:call("file", "exec", {command = "ip", params = {"link", "show", "dev", phy}})
		if not owner or iface_state.code ~= 0 then
			if configs[owner] then
				configs[owner].mark_old = true -- random data to force restart client on reload
				statuses[owner] = { code = ERROR_CODES.NO_DEVICE }
			end
			wpa_supplicants[phy] = false
		end
	end
	for phy, info in pairs(wpa_supplicants) do
		if not info or info.reload then
			conn:call("wpa_supplicant", "config_remove", {iface = phy})
		end
		if info and info.reload then
			info.reload = nil
			conn:call("wpa_supplicant", "config_add", info)
		end
	end
	old_config = configs
end

local function refresh_tables()
	uci:foreach("dot1x", "client", function(client)
		port_tables.reverse_table[client.iface] = client[".name"]
		port_tables.forward_table[client[".name"]] = client.iface
		dot1x_clients[client[".name"]] = client
	end)
	if has_dsa then
		port_tables.bridged_ports = {}
		uci:foreach("network", "device", function(br)
			if br.type ~= "bridge" then return end
			for _, v in ipairs(br.ports or {}) do
				port_tables.bridged_ports[#port_tables.bridged_ports+1] = v
			end
		end)
	end
	if family_name ~= "RUTX" then return end

	-- port number to name
	local port_nums = {}
	uci:foreach("network", "port", function(port)
		port_nums[port.port_num] = { name = port[".name"], role = port.role }
	end)

	-- assign vlans to each number
	uci:foreach("network", "switch_vlan", function(vlan)
		local ports = util.split(vlan.ports or "", " ")
		for _, port in ipairs(ports) do
			if port_nums[port] then
				port_nums[port].vlan = vlan.vid
			end
		end
	end)

	-- devices to interfaces
	local bridges = {}
	uci:foreach("network", "device", function(dev)
		if dev.type ~= "bridge" then return end
		for _, dev_port in ipairs(dev.ports or {}) do
			bridges[dev_port] = dev.name
		end
	end)
	uci:foreach("network", "interface", function(dev)
		if not dev.device then return end
		if string.match(dev.device, "^br-") then return end
		if dev.device == "lo" then return end
		bridges[dev.device] = dev.device
	end)

	-- ports to interfaces
	for _, port in pairs(port_nums) do
		local iface = nil
		local prefix = port.role == "wan" and "eth1" or "eth0"
		if port.vlan == "1" then
			iface = prefix
		elseif port.vlan == "2" then
			iface = prefix
		elseif port.vlan then
			iface = prefix.."."..port.vlan
		end
		port.bridge = bridges[iface]
		port_tables.vlan_mapping[port.name] = port.bridge
	end
end

local sub = {
	notify = function( msg, name )
		if name ~= "link_update" then return end
		if not msg.state or msg.state ~= "UP" then return end
		reload_port_wpa_supplicant(msg.port)
	end,
}

local method = {
	dot1x_client = {
		reload = {
			function()
				port_tables.forward_table = {}
				port_tables.reverse_table = {}
				port_tables.vlan_mapping = {}
				port_tables.bridged_ports = {}
				dot1x_clients = {}
				refresh_tables()
				generate_configuration()
			end, { }
		},
		status = {
			function(req)
				conn:reply(req, statuses)
			end, {}
		},
		kill_clients = {
			function()
				for k, v in pairs(statuses) do
					if v.owner == k then
						conn:call("wpa_supplicant", "config_remove", {iface = v.phy})
					end
				end
				old_config = {}
			end, {}
		}
	}
}

conn:add(method)
conn:subscribe("port_events", sub)
refresh_tables()
generate_configuration()
uloop.run()
