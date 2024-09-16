#!/usr/bin/env lua
-- Copyright (C) 2024 Teltonika

local fs = require "nixio.fs"
local nixio = require "nixio"
local util = require "vuci.util"
local opkg = require "vuci.opkg"
local OpkgPkg = require "vuci.opkg_pkg"

local PACKAGE_FILE = "/etc/package_restore.txt"
local BACKUP_PACKAGES = "/etc/backup_packages/"
local FAILED_PACKAGES = "/etc/failed_packages"
local THIRD_PARTY_FEEDS = "--force_feeds /etc/opkg/openwrt/distfeeds.conf -f /etc/tlt_opkg.conf"
local TLT_PACKAGES = "/var/opkg-lists/tlt_packages"
local TIME_OF_SLEEP = 10
local PKG_REBOOT = false
local PKG_NET_RESTART = false

local qt = util.shellquote

local app_names_with_tlt_names = {}
local function add_to_failed_packages(app_name)
	local f = io.open(FAILED_PACKAGES, "a+")
	if f then
		local line = app_names_with_tlt_names[app_name]
		if line then
			f:write(line .. "\n")
		end
		f:close()
	end
end

-- handles file locking when calling opkg to avoid opkg lock conflicts with api
local function opkg_cmd(cmd)
	while fs.access("/var/lock/opkg.lock") do
		nixio.nanosleep(TIME_OF_SLEEP)
	end
	local ok, lock_file = opkg.acquire_lock()
	assert(ok, "lock error")
	opkg.opkg_cmd(cmd, os.execute)
	lock_file:lock("ulock")
	lock_file:close()
end


--------------- BACKUP PACKAGE INSTALL -----------------------
local backup_pkgs = {}
for file in fs.glob(BACKUP_PACKAGES .. "*.ipk") do
	util.exec("tar x -zf %s -C %s ./control.tar.gz" % {qt(file), BACKUP_PACKAGES})
	util.exec("tar x -zf %s -C %s ./control" % {BACKUP_PACKAGES .. "control.tar.gz", BACKUP_PACKAGES})
	if (fs.readfile(BACKUP_PACKAGES .. "control") or ""):find("tlt_name", nil, true) then
		-- main pkg has tlt_name, it must be first in the list to install correctly
		table.insert(backup_pkgs, 1, qt(file))
	else
		table.insert(backup_pkgs, qt(file))
	end
end
if #backup_pkgs > 0 then
	opkg_cmd("opkg install %s" % table.concat(backup_pkgs, " "))
	util.exec("rm -rf %s 2> /dev/null" % BACKUP_PACKAGES)
	opkg._trigger_pkg_event()
	opkg._restart_services()
end


--------------- MAIN PACKAGE INSTALL -----------------------
local pkg_text = util.trim(fs.readfile(PACKAGE_FILE) or "")
if #pkg_text == 0 then os.exit(0) end

local third_party_pkg_names = {}
local app_names = {}
for _, line in ipairs(util.split(pkg_text)) do
	local pkg_name, tlt_name = line:match("([%w%.%-%+_]+)%s*%-+%s*(.*)")
	if pkg_name and tlt_name and not tlt_name:find("^%s*%-*%s*$") then
		app_names_with_tlt_names[pkg_name] = line
		table.insert(app_names, pkg_name)
	else
		table.insert(third_party_pkg_names, line)
	end
end
if #app_names == 0 and #third_party_pkg_names == 0 then os.exit(0) end

while true do
	opkg_cmd("opkg -f /etc/tlt_opkg.conf update")
	if fs.access(TLT_PACKAGES) then
		local tlt_packages_stat = fs.stat(TLT_PACKAGES)
		local size = tlt_packages_stat and tlt_packages_stat.size or 0
		if size > 0 then break end
	end
	nixio.nanosleep(TIME_OF_SLEEP)
end

local packages = {}
for _, app_name in ipairs(app_names) do
	local pkg = OpkgPkg.get_available_pkg(app_name)
	if pkg then
		if pkg:is_installed() then
			pkg:_remove_pkg_from_pkg_restore()
		else
			table.insert(packages, pkg)
		end
	else
		add_to_failed_packages(app_name)
	end
end

for _, pkg in ipairs(packages) do
	(function ()
		local ok, err_code = pkg:_validate_pkg_online_install()
		if not ok then
			add_to_failed_packages(pkg.app_name)
			return --continue
		end
		ok, err_code = pkg:_install_package_online()
		opkg._trigger_pkg_event()
		if not ok then
			add_to_failed_packages(pkg.app_name)
			return --continue
		end

		opkg._restart_services()
		if pkg:get("pkg_reboot") == "1" then PKG_REBOOT = true end
		if pkg:get("pkg_network_restart") == "1" then PKG_NET_RESTART = true end

		pkg:_remove_pkg_from_pkg_restore()
	end)()
end


--------------- 3RD PARTY PACKAGE INSTALL -----------------------
local uci = require "vuci.uci".cursor()
local third_party_pkg = uci:get("package_restore" , "package_restore", "3rd_party_pkg") == "1"
if third_party_pkg then
	while true do
		opkg_cmd("opkg %s update" % THIRD_PARTY_FEEDS)
		if fs.access("/var/opkg-lists/openwrt_packages") then break end
		nixio.nanosleep(TIME_OF_SLEEP)
	end
	for _, pkg_name in ipairs(third_party_pkg_names) do
		local is_installed = opkg.pkg_installed(pkg_name)
		if not is_installed then
			opkg_cmd("opkg %s install %s" % {THIRD_PARTY_FEEDS, qt(pkg_name)})
			if not opkg.pkg_installed(pkg_name) then
				util.perror("Failed to install package " .. pkg_name)
			end
		end
		opkg._remove_pkg_from_pkg_restore(pkg_name)
	end
end

os.remove(PACKAGE_FILE)
if PKG_NET_RESTART then util.exec("/etc/init.d/network restart") end
if PKG_REBOOT then util.exec("reboot") end