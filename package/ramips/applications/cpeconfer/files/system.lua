require("uci")
require("io")
system_module = {}
local util=require("tz.util")
local x = uci.cursor()
local debug = util.debug
local split = util.split 
local sleep = util.sleep
local MEM_INFO_FILE="/proc/meminfo"
local CPU_AVG_FILE="/proc/loadavg"
local DEVICE_VERSION="/version"
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

local function get_mem_info()
	local res = nil
	local mem_total = nil
	local mem_free = nil
	local mem_cache = nil
	local f = io.open(MEM_INFO_FILE)
	res = f:read()

	while nil ~= res
	do
		local temp = nil
		local arr = nil
		if string.match(res,"MemTotal:%s+%d+") ~= nil
		then 
			temp = string.match(res,"MemTotal:%s+%d+")
			arr = split(temp, ":")
			mem_total = tonumber(arr[2])
		elseif string.match(res,"MemFree:%s+%d+") ~= nil
		then 
			temp = string.match(res,"MemFree:%s+%d+")
			arr = split(temp, ":")
			mem_free = tonumber(arr[2])
		elseif string.match(res,"Cached:%s+%d+") ~= nil
		then 
			temp = string.match(res,"Cached:%s+%d+")
			arr = split(temp, ":")
			mem_cache = tonumber(arr[2])
		end
		if nil ~= mem_total and nil ~= mem_free and nil ~= mem_cache
		then 
			break
		end


		res = f:read()
	end

	io.close(f)

	return mem_total,mem_free,mem_cache
end


local function get_cpu_average()
	local f = io.open(CPU_AVG_FILE)
	local arr = nil
	res = f:read()
	io.close(f)


	if nil == res
	then
		return nil,nil,nil
	end

	arr = split(res,' ')

	if nil == arr
	then
		return nil,nil,nil
	end
	return arr[1],arr[2],arr[3]
end

function system_module.get_divice_version()
     local f = io.open(DEVICE_VERSION)
	 local arr = nil
	 local info = {}
	 res = f:read()
	 
	 while nil ~= res
		do
		arr = split(res,'=')
		info[arr[1]] = arr[2]
        res = f:read()
	end
	 io.close(f)
	 return info

end

-- get system run status
-- input:none
-- return:
--		the struct of sys_info 
function system_module.system_get_status()

		local status = system_module.sys_info:new(nil,nil)

		status["runtime"] =  tonumber(execute_cmd(GET_RUNTIME_CMD))
		status["mem_total"],status["mem_free"] ,status["mem_cache"] = get_mem_info()
		status["cpu_average1"],status["cpu_average5"],status["cpu_average15"] = get_cpu_average()

		return status
end

local TMP_NETWORK_TOOL_LOG_FILE="/tmp/.network_tool_log"
local TMP_TCPDUMP_FILE="/tmp/web_upload/tcpdump"
local TMP_PKG_DOWNLOAD_FILE="/tz_www/html/manage/network_tool_log.tcpdump"
local TMP_LOG_DOWNLOAD_FILE="/tz_www/html/manage/network_tool_log.log"
local LOG_FILE_LIMIT=40960000 -- log file limit 40M
function system_module.system_network_tool(tz_req)
	
	local tz_answer = {}
	if "ping_start" == tz_req["tool"]
	then
		if "" ~= tz_req["pingUrl"]  
		then
			if "1" == tz_req["pingLoop"]
			then
				os.execute(string.format("softlimit -f %d ping %s > %s &",LOG_FILE_LIMIT, tz_req["pingUrl"],TMP_NETWORK_TOOL_LOG_FILE))
			else
				if "" == tz_req["pingNum"]
				then
					tz_req["pingNum"] = "50"
				end
				os.execute(string.format("softlimit -f %d ping -c %s %s > %s &",LOG_FILE_LIMIT, tz_req["pingNum"], tz_req["pingUrl"], TMP_NETWORK_TOOL_LOG_FILE))
			end
		end
	elseif "ping_stop" == tz_req["tool"]
	then
		os.execute("ps | grep 'ping' | grep -v grep | awk '{print $1}' | xargs kill -9")
	elseif "get_log" == tz_req["tool"]
	then
		local f = io.open(TMP_NETWORK_TOOL_LOG_FILE)
		local res = f:read("*a")
		
		tz_answer["data"] = res
		io.close(f)
	elseif "catch_pkg_start" == tz_req["tool"]
	then
		if "" == tz_req["catchPackageIfname"]
		then
			tz_req["catchPackageIfname"] = x:get("network","wan","ifname")
		end

		if (util.is_file_exist(TMP_TCPDUMP_FILE) == true)
		then
			os.execute(string.format("chmod 755 %s", TMP_TCPDUMP_FILE))
			os.execute(string.format("softlimit -f %d %s -i %s > %s &", LOG_FILE_LIMIT, TMP_TCPDUMP_FILE, tz_req["catchPackageIfname"],TMP_NETWORK_TOOL_LOG_FILE))
		else
			tz_answer["data"] = "NOTCPDUMP"
		end

	elseif "catch_pkg_stop" == tz_req["tool"]
	then
		os.execute("ps | grep 'tcpdump' | grep -v grep | awk '{print $1}' | xargs kill -9")
		os.execute(string.format("ln -sf %s %s ", TMP_NETWORK_TOOL_LOG_FILE, TMP_PKG_DOWNLOAD_FILE))
	elseif "get_size" == tz_req["tool"]
	then
		local f = io.popen(string.format("ls -l %s | awk '{print $5}'", TMP_NETWORK_TOOL_LOG_FILE))
		tz_answer["data"] = f:read()
		io.close(f)
	elseif "trace_start" == tz_req["tool"]
	then
		if "" ~= tz_req["traceUrl"] 
		then
				os.execute(string.format("softlimit -f %d traceroute %s %s > %s &",LOG_FILE_LIMIT, tz_req["traceUrl"], tz_req["tracePort"],TMP_NETWORK_TOOL_LOG_FILE))
		end
	elseif "trace_stop" == tz_req["tool"]
	then
		os.execute("ps | grep 'traceroute' | grep -v grep | awk '{print $1}' | xargs kill -9")
	elseif "log_start" == tz_req["tool"]
	then
		os.execute(string.format("softlimit -f %d logread -f > %s &",LOG_FILE_LIMIT, TMP_NETWORK_TOOL_LOG_FILE))
	elseif "log_stop" == tz_req["tool"]
	then
		os.execute("ps | grep 'logread' | grep -v grep | awk '{print $1}' | xargs kill -9")
		os.execute(string.format("ln -sf %s %s ", TMP_NETWORK_TOOL_LOG_FILE, TMP_LOG_DOWNLOAD_FILE))
	end

	return tz_answer
end

--[[
ret: 
	0: success
	-1: file is not exist
	-2: file format is error
]]--
function system_module.update_system(file_path)
	if (util.is_file_exist(file_path) ~= true)
	then
		return -1
	end

	os.execute("rm -rf /tmp/updateit")
	os.execute(string.format("cd /tmp && unzip -P tz18c6 %s updateit", file_path))

	if (util.is_file_exist("/tmp/updateit") ~= true)
	then
		return -2
	end

	os.execute("chmod 777 /tmp/updateit")
	os.execute(string.format("/tmp/updateit %s",file_path))

	return 0

end

return system_module

