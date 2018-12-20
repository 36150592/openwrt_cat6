require("uci")
require("io")
sim_module = {}
local util=require("tz.util")
local x = uci.cursor()
local SIM_CONFIG_FILE="network"
local SIM_DYNAMIC_STATUS_PATH="/tmp/.system_info_dynamic"
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
		elseif "pinlock_enable_flag" == key
		then 
			status["pinlock_enable"] = value
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

	io.close(f)

	return status

end


return sim_module