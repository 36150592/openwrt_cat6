require("uci")
require("io")
device_module = {}
local util=require("tz.util")
local x = uci.cursor()
local debug = util.debug
local split = util.split 
local sleep = util.sleep

local GET_KEY_CMD="cat /etc/dialogtool2_version  | cut -d':' -f 1"
local GET_VALUE_CMD="cat /etc/dialogtool2_version  | cut -d':' -f 2"

device_module.dev_info = {
		
		["type"] = nil,   -- string tz config version
		["version"] = nil, -- string   software version
		["hwversion"] = nil,  -- string   hardware version
		["device"] = nil, -- string   device name
		["build"] = nil,  --stirng  build time
		["branch"] = nil,  --string code branch
		["os"] = nil,  --string compile os
		["sha1"] = nil,  --string device uniq code
		["compile_option"] = nil  --string 
}


function device_module.dev_info:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
		
	self["type"] = obj["type"] or nil
	self["version"] = obj["version"] or nil
	self["device"] = obj["device"] or nil
	self["build"] = obj["build"] or nil
	self["branch"] = obj["branch"] or nil
	self["os"] = obj["os"] or nil
	self["sha1"] = obj["sha1"] or nil
	self["compile_option"] = obj["compile_option"] or nil
end


-- get device info
-- input:none
-- return:
--		the struct of dev_info 
function device_module.device_get_info()

		local info = device_module.dev_info:new(nil,nil)

		local f_key = io.popen(GET_KEY_CMD)
		local f_value = io.popen(GET_VALUE_CMD)

		if nil == f_key or nil == f_value
		then
			debug("error open")
			return nil
		end


		local key = f_key:read()
		local value = f_value:read()

		while nil ~= key
		do
			if nil ~= value
			then
				info[key] = value
			end

			key = f_key:read()
			value = f_value:read()
		end

		return info
end


return device_module
