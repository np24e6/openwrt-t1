#!/usr/bin/lua

local pop3 = require "pop3"
local util = require("vuci.util")

MAX_MODEM_CNT = 12
MSG_SIZE = 1024
MAX_SIZE = 20480
MAX_MESSAGE_TO_READ = 32

local function trim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end

local function read_output(command)
	local f = io.popen(command)
	local l = f:read("*a")
	f:close()
	return trim(l)
end

local function get_modem_query(modem)

	local modem_count = 0
	local modem_name

	modem_count = util.ubus("gsm", "info", {})
	if modem_count == nil then
			do return nil end
	end

	modem_count = modem_count["mdm_stats"]["num_modems"]

	if modem_count == nil or modem_count == 0 then
			do return nil end
	end

	if modem == nil or modem == '' and modem_count == 1 then
			do return "gsm.modem0" end
	end

	for i=0, MAX_MODEM_CNT do
			modem_name = util.ubus("gsm.modem"..i, "info", {})

			if modem_name ~= nil and modem_name["usb_id"] == modem then
					do return "gsm.modem"..i end
			end
	end

	return nil

end

local function send_big_sms(phone_number, text, limit, modem_query)
	local max_sms_size = 120
	local i = 1
	local t = {}

	if #text == 0 then
		print("message is empty")
		return false
	end

	if modem_query == nil then
		print("No modem was found")
		return false
	end

	while #text > 1 and i < (limit + 1) do
		t[i] = text:sub(1, max_sms_size)
		text = text:sub(max_sms_size + 1)
		i = i + 1
	end

	for k , v in pairs(t) do
		util.ubus(modem_query, "send_sms", {number=phone_number, text=v})
	end

	return true
end

local enabled = read_output("uci -q get email_to_sms.pop3.enabled")
local some_mail = {
	host	 = read_output("uci -q get email_to_sms.pop3.host");
	username = read_output("uci -q get email_to_sms.pop3.username");
	password = read_output("uci -q get email_to_sms.pop3.password");
	port = read_output("uci -q get email_to_sms.pop3.port");
	limit = tonumber(read_output("uci -q get email_to_sms.pop3.limit")) or 0;
	modem = read_output("uci -q get email_to_sms.pop3.modem_id");
}
local modem_query = get_modem_query(some_mail.modem)

local function test_header(mbox, id)
	local txt, err = mbox:top(id, 1)

	if err or not txt then
		print("Failed to get SMS message")
		return false
	end

	local msg = pop3.message(txt)
	if msg == nil then
		print("Failed to parse SMS message")
		return false
	end

	local number=msg:subject()
	if number == nil or number == "" or number:match('^[0-9+]+$') == nil then
		return false
	end

	return true
end

local function handle_message(mbox, i)
	local id, size = mbox:list(i)
	if size > MAX_SIZE or test_header(mbox, i) == false then
		return
	end

	local msg = mbox:message(i)
	if not msg then
		return
	end

	print('- Message -', i, msg:id(), msg:to(), msg:subject())

	local anytext = nil
	local plaintext = nil
	for n,v in ipairs(msg:full_content()) do
		if v.text then
			if v.type == "text/plain" then
				plaintext = v.text
			else
				anytext = v.text
			end
		end
	end

	local mytext = (plaintext ~= nil) and plaintext or anytext
	if mytext ~= nil and send_big_sms(msg:subject(), string.sub(mytext, 1, MSG_SIZE), some_mail.limit, modem_query) then
		print('- Delete -', i)
		mbox:dele(i)
	end
end

local function main()
	if tonumber(enabled) ~= 1 then
		return
	end

	local ssl = tonumber(read_output("uci -q get email_to_sms.pop3.ssl")) == 1
	local ssl_verify = tonumber(read_output("uci -q get email_to_sms.pop3.ssl_verify")) == 1
	local mbox = pop3.new()
	if ssl then
		mbox:open_tls(some_mail.host, some_mail.port, nil, ssl_verify)
	else
		mbox:open(some_mail.host, some_mail.port)
	end
	print('open   :', mbox:is_open())
	if not mbox:is_open() then
		return
	end

	mbox:auth(some_mail.username, some_mail.password)
	if not mbox:is_auth() then
		mbox:close()
		return
	end
	print('auth   :', mbox:is_auth())

	local cnt, size = mbox:stat()
	for i = cnt > MAX_MESSAGE_TO_READ and cnt - MAX_MESSAGE_TO_READ + 1 or 1, cnt do
		handle_message(mbox, i)
	end
	mbox:close()
end

local function start()
	local reboot=0
	local find = read_output("grep -q /usr/bin/email_to_sms /etc/crontabs/root; echo $?")
	if tonumber(find) == 0 then
		os.execute("sed -i '\\/usr\\/bin\\/email_to_sms/d' /etc/crontabs/root")
		reboot=1
	end
	if tonumber(enabled) == 1 then
		local command=""
		local time_format = read_output("uci -q get email_to_sms.pop3.time")
		if time_format == "min" then
			local min_number = read_output("uci -q get email_to_sms.pop3.min")
			command = 'echo "*/'..tonumber(min_number)..' * * * * lua /usr/bin/email_to_sms read" >>/etc/crontabs/root'
		elseif time_format == "hour" then
			local hour_number = read_output("uci -q get email_to_sms.pop3.hour")
			command = 'echo "0 */'..tonumber(hour_number)..' * * * lua /usr/bin/email_to_sms read" >>/etc/crontabs/root'
		elseif time_format == "day" then
			local day_number = read_output("uci -q get email_to_sms.pop3.day")
			command = 'echo "0 0 */'..tonumber(day_number)..' * * lua /usr/bin/email_to_sms read" >>/etc/crontabs/root'
		end
		reboot=1
		print(command)
		os.execute(command)
	end
	if tonumber(reboot) == 1 then
		os.execute("/etc/init.d/cron restart")
	end
end

local function stop()
	local find = read_output("grep -q /usr/bin/email_to_sms /etc/crontabs/root; echo $?")
	if tonumber(find) == 0 then
		os.execute("sed -i '\\/usr\\/bin\\/email_to_sms/d' /etc/crontabs/root")
		os.execute("/etc/init.d/cron restart")
	end
end


local out =
[[unknown command line argument.

usage:
  email_to_sms read
  email_to_sms start
]]
--
-- Program execution
--
if #arg > 0 and #arg < 2 then
	if arg[1] == "read" then main()
	elseif arg[1] == "start" then start()
	elseif arg[1] == "stop" then stop()
	else
		print(out)
	end
else
	print(out)
end
