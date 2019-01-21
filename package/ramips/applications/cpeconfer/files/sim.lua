require("uci")
require("io")
sim_module = {}
local util=require("tz.util")
local x = uci.cursor()
local SIM_CONFIG_FILE="network"
local SIM_DYNAMIC_STATUS_PATH="/tmp/.system_info_dynamic"
local SIM_SEND_AT_CMD="sendat -d/dev/ttyUSB1 -f/tmp/at_send -o/tmp/at_recv"
local SIM_AT_SEND_FILE="/tmp/at_send"
local SIM_AT_RECV_FILE="/tmp/at_recv"
local debug = util.debug
local split = util.split 
local sleep = util.sleep


sim_module.sim_status = {
	["iccid"]  = nil,      		--string
	["imsi"]  = nil,	--string
	["card_status"]  = nil,		--number 1:pin lock 2:puk lock 0:ready
	["is_sim_exist"]  = nil,			--string
	["pinlock_enable"]  = nil, 			--string 0:disable 1:enable
	["pin_left_times"]  = nil,			--string
	["puk_left_times"]  = nil 			--string
}


function sim_module.sim_status:new(o,obj)

	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
	
	self["iccid"]  = obj["iccid"] or  nil
	self["imsi"]  = obj["imsi"] or  nil
	self["card_status"]  = obj["card_status"] or  nil
	self["is_sim_exist"]  = obj["is_sim_exist"] or  nil
	self["pinlock_enable"]  = obj["pinlock_enable"] or  nil
	self["pin_left_times"]  = obj["pin_left_times"] or  nil
	self["puk_left_times"]  = obj["puk_left_times"] or  nil
end

local function clear_file(path)
	return os.execute(string.format("> %s", path))
end

local function format_enable_pin_lock_at_cmd(pin_passwd)return string.format('AT+CLCK="SC",1,"%s"\n', pin_passwd) end
local function format_disable_pin_lock_at_cmd(pin_passwd)return string.format('AT+CLCK="SC",0,"%s"\n', pin_passwd) end
local function format_get_pin_lock_at_cmd()return 'AT+CLCK="SC",2,""\n' end
local function format_pin_unlock_at_cmd(pin_passwd)return string.format('AT+CPIN=%s\n', pin_passwd) end
local function format_get_rev_cmd(file)return string.format('cat %s | grep OK | wc -l', file) end

local function send_at_cmd(cmd)
	clear_file(SIM_AT_SEND_FILE)
	clear_file(SIM_AT_RECV_FILE)
	local f_send = io.open(SIM_AT_SEND_FILE, "w+")
	local p = nil
	if nil == f_send
	then
		debug("open send file fail")
		return false
	end

	if nil == cmd
	then
		debug("error:cmd nil")
		return false
	end

	f_send:write(cmd)

	p = io.popen(SIM_SEND_AT_CMD)
	if p == nil
	then
		debug("execute at cmd fail")
		return false
	end

	io.close(p)

	p = io.popen(format_get_rev_cmd(SIM_AT_RECV_FILE))

	if nil == p
	then
		debug("open recv file fail")
		return false
	end

	if "1" == p:read()
	then
		debug("at fail")
		return true
	end

	io.close(p)
	io.close(f_send)

	return false

end

local function get_pin_lock_enable_status()
	local ret = send_at_cmd(format_get_pin_lock_at_cmd())
	if true == ret
	then
		local f = io.popen(string.format("cat %s | grep 'CLCK' | cut -d':' -f 2", SIM_AT_RECV_FILE))
		if nil ~= f
		then 
			local res = f:read()
			io.close(f)
			return res
		end
	end

	return nil
end

function sim_module.sim_reload()
	if send_at_cmd("AT+CFUN=0\n")
	then
		return send_at_cmd("AT+CFUN=1\n")
	end
	return false
end

function sim_module.sim_get_status()
	local f = nil
	local status = nil
	local res = nil

	f = io.open(SIM_DYNAMIC_STATUS_PATH,"r")

	if nil == f
	then
		debug("open ",SIM_DYNAMIC_STATUS_PATH, " fail")
		return nil
	end

	res = f:read()

	while( res ~= nil)
	do
		if nil == status
		then
			status = sim_module.sim_status:new(nil,nil)
			status["card_status"] = 0
		end

		local array = split(res,":")
		--print(array[1],"=", array[2])
		local key = string.gsub(array[1],"\t","")
		local value = string.gsub(array[2],"\t","")
		key = string.lower(key)
		
		if  "iccid" == key
		then
			status["iccid"] = value
		elseif "imsi" == key
		then
			status["imsi"] = value
		elseif "lock_pin_flag" == key
		then
			if value == "1"
			then 
				status["card_status"] = 1
			end
		elseif "lock_puk_flag" == key
		then
			if value == "1"
			then 
				status["card_status"] = 2
			end
		elseif "is_sim_exist" == key
		then
			status["is_sim_exist"] = value
		elseif "pin_left_times" == key
		then
			status["pin_left_times"] = value
		elseif "puk_left_times" == key
		then
			status["puk_left_times"] = value
		end

		res = f:read()
	end

	status["pinlock_enable"] = get_pin_lock_enable_status()


	io.close(f)

	return status

end




function sim_module.sim_pin_lock_enable(pin_passwd)

	if nil == pin_passwd
	then
		debug("error:pin_passwd nil")
		return false
	end

	return send_at_cmd(format_enable_pin_lock_at_cmd(pin_passwd))
end

function sim_module.sim_pin_lock_disable(pin_passwd)
	
	if nil == pin_passwd
	then
		debug("error:pin_passwd nil")
		return false
	end

	return send_at_cmd(format_disable_pin_lock_at_cmd(pin_passwd))
end

function sim_module.sim_pin_unlock(pin_passwd)
	
	if nil == pin_passwd
	then
		debug("error:pin_passwd nil")
		return false
	end

	return send_at_cmd(format_pin_unlock_at_cmd(pin_passwd))

end

return sim_module
