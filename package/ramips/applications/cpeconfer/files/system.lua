require("uci")
require("io")
system_module = {}
local util=require("tz.util")
local x = uci.cursor()
local debug = util.debug
local split = util.split 
local sleep = util.sleep
local GET_RUNTIME_CMD="cat /proc/uptime | awk '{print $1}'"
local GET_MEM_TOTAL_CMD="cat /proc/meminfo | awk '{print $2}' | sed -n  1p"
local GET_MEM_FREE_CMD="cat /proc/meminfo | awk '{print $2}' | sed -n  2p"
local GET_MEM_CACHE_CMD="cat /proc/meminfo | awk '{print $2}' | sed -n  5p"
local GET_CPU_AVG1_CMD="cat /proc/loadavg | awk '{print $1}'"
local GET_CPU_AVG5_CMD="cat /proc/loadavg | awk '{print $2}'"
local GET_CPU_AVG15_CMD="cat /proc/loadavg | awk '{print $3}'"

system_module.sys_info = {
		
		["runtime"] = nil,   -- number   system up time in second
		["mem_total"] = nil, -- number   total memerory in kb
		["mem_free"] = nil,  -- number   current free memerory in kb
		["mem_cache"] = nil, -- number   system memerory cache in kb
		["cpu_average1"] = nil,  --number cpu average load  in 1 min
		["cpu_average5"] = nil,  --number cpu average load in 5 min
		["cpu_average15"] = nil  --number cpu average load in 15 min
}


function system_module.sys_info:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
		
	self["runtime"] = obj["runtime"] or nil
	self["mem_total"] = obj["mem_total"] or nil
	self["mem_free"] = obj["mem_free"] or nil
	self["mem_cache"] = obj["mem_cache"] or nil
	self["cpu_average1"] = obj["cpu_average1"] or nil
	self["cpu_average5"] = obj["cpu_average5"] or nil
	self["cpu_average15"] = obj["cpu_average15"] or nil
end

local function execute_cmd(cmd)
	
	local temp = nil

	local f = io.popen(cmd)
	temp = f:read()
	io.close(f)

	return temp
end

-- get system run status
-- input:none
-- return:
--		the struct of sys_info 
function system_module.system_get_status()

		local status = system_module.sys_info:new(nil,nil)

		status["runtime"] =  tonumber(execute_cmd(GET_RUNTIME_CMD))
		status["mem_total"] = tonumber(execute_cmd(GET_MEM_TOTAL_CMD))
		status["mem_free"] = tonumber(execute_cmd(GET_MEM_FREE_CMD))
		status["mem_cache"] = tonumber(execute_cmd(GET_MEM_CACHE_CMD))
		status["cpu_average1"] = tonumber(execute_cmd(GET_CPU_AVG1_CMD))
		status["cpu_average5"] = tonumber(execute_cmd(GET_CPU_AVG5_CMD))
		status["cpu_average15"] = tonumber(execute_cmd(GET_CPU_AVG15_CMD))

		return status
end




return system_module

