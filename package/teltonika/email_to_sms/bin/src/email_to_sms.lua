#!/usr/bin/lua

local pop3 = require "pop3"
local util = require("vuci.util")
local uci = require("vuci.uci").cursor()

local MAX_MODEM_CNT = 12
local MSG_SIZE = 1024
local MAX_SIZE = 61440
local MAX_MESSAGE_TO_READ = 32

local function get_cfg(opt)
	return uci:get("email_to_sms", "pop3", opt)
end

local function perror(msg)
	util.exec("logger -p daemon.err -t email_to_sms %s" % util.shellquote(tostring(msg)))
end

local function get_modem_query(modem)
	local modem_name

	local modem_count = util.ubus("gsm", "info", {})
	if modem_count == nil then
		return nil
	end

	modem_count = modem_count["mdm_stats"]["num_modems"]

	if modem_count == nil or modem_count == 0 then
		return nil
	end

	if modem == nil or modem == '' and modem_count == 1 then
		return "gsm.modem0"
	end

	for i = 0, MAX_MODEM_CNT do
		modem_name = util.ubus("gsm.modem" .. i, "info", {})

		if modem_name ~= nil and modem_name["usb_id"] == modem then
			return "gsm.modem" .. i
		end
	end

	return nil
end

local function send_big_sms(phone_number, text, limit, modem_query)
	if #text == 0 then
		perror("message is empty")
		return false
	end

	if modem_query == nil then
		perror("No modem was found")
		return false
	end

	local res = util.ubus(modem_query, "send_sms", { number = phone_number, text = text, validate = true }) or {}
	if not res.valid then
		perror("SMS is invalid")
		return false
	end
	if res.sms_used > limit then
		perror("Max SMS count is %s messages" % limit)
		return false
	end

	res = util.ubus(modem_query, "send_sms", { number = phone_number, text = text }) or {}
	if res.errno then
		perror("Failed to send SMS (errno=%s)" % res.errno)
		return false
	end

	return true
end

local enabled = get_cfg("enabled")
local some_mail = {
	host     = get_cfg("host");
	username = get_cfg("username");
	password = get_cfg("password");
	port = get_cfg("port");
	limit = tonumber(get_cfg("limit")) or 0;
	modem = get_cfg("modem_id");
}
local modem_query = get_modem_query(some_mail.modem)
if not modem_query then
	perror("Modem not found")
	return
end

local function test_header(mbox, id)
	local txt, err = mbox:top(id, 1)

	if err or not txt then
		perror("Failed to get email message")
		perror(err)
		return false
	end

	local msg, err = pop3.message(txt)
	if msg == nil then
		perror("Failed to parse email message")
		perror(err)
		return false
	end

	local number = msg:subject()
	if number == nil or number == "" or number:match('^[0-9+]+$') == nil then
		-- skip email if subject is not tel number
		return false
	end

	return true
end

local function handle_message(mbox, i)
	local id, size = mbox:list(i)
	if not test_header(mbox, i) then
		return
	end

	if size > MAX_SIZE then
		perror(string.format("Message too big (%s bytes). Max allowed size is %d bytes", size, MAX_SIZE))
		return
	end

	local msg, err = mbox:message(i)
	if not msg then
		perror("Message retrieve error")
		perror(err)
		return
	end

	print('- Message -', i, msg:id(), msg:to(), msg:subject())

	local anytext = nil
	local plaintext = nil
	for n, v in ipairs(msg:full_content()) do
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

	local ssl = tonumber(get_cfg("ssl")) == 1
	local ssl_verify = tonumber(get_cfg("ssl_verify")) == 1
	local mbox = pop3.new()
	if ssl then
		local ok, err = mbox:open_tls(some_mail.host, some_mail.port, nil, ssl_verify)
		if not ok then
			perror("TLS connection error")
			perror(err)
			return
		end
	else
		local ok, err = mbox:open(some_mail.host, some_mail.port)
		if not ok then
			perror("Connection error")
			perror(err)
			return
		end
	end
	print('open   :', mbox:is_open())
	if not mbox:is_open() then
		return
	end

	local ok, err = mbox:auth(some_mail.username, some_mail.password)
	if not mbox:is_auth() then
		mbox:close()
		perror("Authentication error")
		perror(err)
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
	local reboot = 0
	local find = util.exec("grep -q /usr/bin/email_to_sms /etc/crontabs/root; echo $?")
	if tonumber(find) == 0 then
		os.execute("sed -i '\\/usr\\/bin\\/email_to_sms/d' /etc/crontabs/root")
		reboot = 1
	end
	if tonumber(enabled) == 1 then
		local command = ""
		local time_format = get_cfg("time")
		if time_format == "min" then
			local min_number = get_cfg("min")
			command = 'echo "*/' .. tonumber(min_number) .. ' * * * * lua /usr/bin/email_to_sms read" >>/etc/crontabs/root'
		elseif time_format == "hour" then
			local hour_number = get_cfg("hour")
			command = 'echo "0 */' ..tonumber(hour_number) .. ' * * * lua /usr/bin/email_to_sms read" >>/etc/crontabs/root'
		elseif time_format == "day" then
			local day_number = get_cfg("day")
			command = 'echo "0 0 */' .. tonumber(day_number) .. ' * * lua /usr/bin/email_to_sms read" >>/etc/crontabs/root'
		end
		reboot = 1
		print(command)
		os.execute(command)
	end
	if tonumber(reboot) == 1 then
		os.execute("/etc/init.d/cron restart")
	end
end

local function stop()
	local find = util.exec("grep -q /usr/bin/email_to_sms /etc/crontabs/root; echo $?")
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
	if arg[1] == "read" then
		main()
	elseif arg[1] == "start" then
		start()
	elseif arg[1] == "stop" then
		stop()
	else
		print(out)
	end
else
	print(out)
end
