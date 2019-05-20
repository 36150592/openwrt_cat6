require("uci")
require("io")
require("math")
system_module = {}
local util=require("tz.util")
local x = uci.cursor()
local debug = util.debug
local split = util.split 
local sleep = util.sleep
local SYSTEM_CONFIG_FILE="system"
local TOZED_CONFIG_FILE="tozed"
local LANRECORD_CURRENT_FILE="/tmp/.current_lan_list"
local LANRECORD_HISTORY_FILE="/etc/history_lan_list"
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
local TOZED_DDNS_SECTION="ddns"

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

local TMP_NETWORK_TOOL_LOG_FILE="/tmp/log_zip/network_tool_log"
local TMP_TCPDUMP_FILE="/tmp/web_upload/tcpdump"
local TMP_PKG_DOWNLOAD_FILE="/tz_www/html/manage/network_tool_log.tcpdump"
local TMP_LOG_DOWNLOAD_FILE="/tz_www/html/manage/network_tool_log.tar"
local LOG_FILE_LIMIT=40960000 -- log file limit 40M
function system_module.system_network_tool(tz_req)
	
	local tz_answer = {}
	if "ping_start" == tz_req["tool"]
	then
		os.execute("rm -rf /tmp/log_zip;mkdir /tmp/log_zip;")
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
		os.execute("rm -rf /tmp/log_zip;mkdir /tmp/log_zip;")
		if "" == tz_req["catchPackageIfname"]
		then
			tz_req["catchPackageIfname"] = x:get("network","wan","ifname")
		end

		if (util.is_file_exist(TMP_TCPDUMP_FILE) == true)
		then
			os.execute(string.format("chmod 755 %s", TMP_TCPDUMP_FILE))
			os.execute(string.format("softlimit -f %d %s -i %s -w %s 2>/dev/null > /dev/null &", LOG_FILE_LIMIT, TMP_TCPDUMP_FILE, tz_req["catchPackageIfname"],TMP_NETWORK_TOOL_LOG_FILE))
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
		os.execute("rm -rf /tmp/log_zip;mkdir /tmp/log_zip;")
		if "" ~= tz_req["traceUrl"] 
		then
				os.execute(string.format("softlimit -f %d traceroute %s %s > %s &",LOG_FILE_LIMIT, tz_req["traceUrl"], tz_req["tracePort"],TMP_NETWORK_TOOL_LOG_FILE))
		end
	elseif "trace_stop" == tz_req["tool"]
	then
		os.execute("ps | grep 'traceroute' | grep -v grep | awk '{print $1}' | xargs kill -9")
	elseif "log_start" == tz_req["tool"]
	then
		os.execute("rm -rf /tmp/log_zip;mkdir /tmp/log_zip;")
		os.execute(string.format("softlimit -f %d logread -f > %s &",LOG_FILE_LIMIT, TMP_NETWORK_TOOL_LOG_FILE))
	elseif "log_stop" == tz_req["tool"]
	then
		os.execute("ps | grep 'logread' | grep -v grep | awk '{print $1}' | xargs kill -9")
		os.execute("sh /usr/lib/lua/tz/pack_debug_info.sh")
		-- uncompress cmd :dd if=filename.des3 |openssl des3 -d -k password | tar zxf -
		os.execute("tar -zcf  - /tmp/log_zip/* |openssl des3 -salt -k tz18c6 | dd of=/tmp/log_zip/log.tar.tz")
		os.execute(string.format("ln -sf %s %s ", "/tmp/log_zip/log.tar.tz", TMP_LOG_DOWNLOAD_FILE))
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

--[[
ret: 
	0: success
	-1: file is not exist
	-2: file format is error
]]--
function system_module.update_config(file_path)
	if (util.is_file_exist(file_path) ~= true)
	then
		return -1
	end
	
	os.execute(string.format("/etc/tozed/config_update %s 0",file_path))
	
	if (util.is_file_exist("/tmp/.update_config_success") == true)
	then
		return 0
	end
	
	if (util.is_file_exist("/tmp/.update_config_fail") == true)
	then
		return -2
	end
	
	return -2
	

end

-- import config /etc/config
-- input:file_path :the absolute path of the import config package
-- return:true if success , false if fail
function system_module.system_import_config(file_path)
	if true ~= util.is_file_exist(file_path)
	then
		debug(file_path," does not exist")
		return false
	end

	local super_user = x:get(TOZED_CONFIG_FILE,"cfg","TZ_SUPER_USERNAME")
	local super_passwd = x:get(TOZED_CONFIG_FILE,"cfg","TZ_SUPER_PASSWD")
	local test_user = x:get(TOZED_CONFIG_FILE, "cfg", "TZ_TEST_USERNAME")
	local test_passwd = x:get(TOZED_CONFIG_FILE,"cfg","TZ_TEST_PASSWD")
	local pci = x:get(TOZED_CONFIG_FILE,"modem","TZ_DIALTOOL2_LTE_PCI_LOCK")
	local earfcn = x:get(TOZED_CONFIG_FILE,"modem","TZ_DIALTOOL2_LTE_EARFCN_LOCK")
	local ret = os.execute("dd if="..file_path.." |openssl des3 -d -k tz18c6 | tar zxf - -C /")
	x = uci.cursor()
	if nil ~= super_user
	then
		x:set(TOZED_CONFIG_FILE, "cfg","TZ_SUPER_USERNAME", super_user)
		x:set(TOZED_CONFIG_FILE, "cfg","TZ_SUPER_PASSWD", super_passwd)
	else
		x:delete(TOZED_CONFIG_FILE,"cfg","TZ_SUPER_USERNAME")	
		x:delete(TOZED_CONFIG_FILE,"cfg","TZ_SUPER_PASSWD")
	end

	if nil ~= test_user
	then
		x:set(TOZED_CONFIG_FILE, "cfg","TZ_TEST_USERNAME", test_user)
		x:set(TOZED_CONFIG_FILE, "cfg","TZ_TEST_PASSWD", test_passwd)
	else
		x:delete(TOZED_CONFIG_FILE,"cfg","TZ_TEST_USERNAME")	
		x:delete(TOZED_CONFIG_FILE,"cfg","TZ_TEST_PASSWD")
	end


	if nil ~= pci
	then
		x:set(TOZED_CONFIG_FILE, "modem","TZ_DIALTOOL2_LTE_PCI_LOCK", pci)
		x:set(TOZED_CONFIG_FILE, "modem","TZ_DIALTOOL2_LTE_EARFCN_LOCK", earfcn)
	else
		x:delete(TOZED_CONFIG_FILE,"modem","TZ_DIALTOOL2_LTE_PCI_LOCK")	
		x:delete(TOZED_CONFIG_FILE,"modem","TZ_DIALTOOL2_LTE_EARFCN_LOCK")
	end

	return x:commit(TOZED_CONFIG_FILE)
end


-- export config 
-- input:file_path :the absolute path of the package which you want to export
-- return:true if success , false if fail
function system_module.system_export_config(file_path)
	return 0 == os.execute("tar -zcf  - /etc/config/* |openssl des3 -salt -k tz18c6 | dd of="..file_path)
end


function system_module.get_socket_at_switch()
	local app_switch = x:get("tozed","cfg","TZ_SOCKET_AT_ENABLE")
	if(nil ~= app_switch)
	then
		if ("1" == app_switch)
		then
			return true
		end

		return false
	end

	return false
end


function system_module.set_socket_at_app(isOpen)
	if(nil ~= isOpen)
	then
		if(true == isOpen)
		then
			os.execute("cfg -a TZ_SOCKET_AT_ENABLE=1")
			os.execute("cfg -c")
			os.execute("/etc/init.d/socket_at stop 1>/dev/null 2>&1")
			os.execute("/etc/init.d/socket_at start 1>/dev/null 2>&1 &")
		else
			os.execute("cfg -a TZ_SOCKET_AT_ENABLE=")
			os.execute("cfg -c")
			os.execute("/etc/init.d/socket_at stop 1>/dev/null 2>&1")
		end
		return 0
	end

	return -1
end


system_module.tr069_info = {
		
		["tr069_app_enable"] = nil,   --string
		["tr069_ServerURL"] = nil, --string
		["tr069_PeriodicInformEnable"] = nil,  --string
		["tr069_PeriodicInformInterval"] = nil, --string
		["tr069_ACS_auth"] = nil,  --string
		["tr069_ServerUsername"] = nil,  --string
		["tr069_ServerPassword"] = nil,  --string
		["tr069_CPE_auth"] = nil,  --string
		["tr069_ConnectionRequestUname"] = nil,  --string
		["tr069_ConnectionRequestPassword"] = nil  --string
}


function system_module.tr069_info:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
		
	self["tr069_app_enable"] = obj["tr069_app_enable"] or nil
	self["tr069_ServerURL"] = obj["tr069_ServerURL"] or nil
	self["tr069_PeriodicInformEnable"] = obj["tr069_PeriodicInformEnable"] or nil
	self["tr069_PeriodicInformInterval"] = obj["tr069_PeriodicInformInterval"] or nil
	self["tr069_ACS_auth"] = obj["tr069_ACS_auth"] or nil
	self["tr069_ServerUsername"] = obj["tr069_ServerUsername"] or nil
	self["tr069_ServerPassword"] = obj["tr069_ServerPassword"] or nil
	self["tr069_CPE_auth"] = obj["tr069_CPE_auth"] or nil
	self["tr069_ConnectionRequestUname"] = obj["tr069_ConnectionRequestUname"] or nil
	self["tr069_ConnectionRequestPassword"] = obj["tr069_ConnectionRequestPassword"] or nil
end

function system_module.system_get_tr069_info()

		local info = system_module.tr069_info:new(nil,nil)

		info["tr069_app_enable"] =  x:get("tozed","cfg","tr069_app_enable")
		info["tr069_ServerURL"] =  x:get("tozed","cfg","tr069_ServerURL") or ""
		info["tr069_PeriodicInformEnable"] =  x:get("tozed","cfg","tr069_PeriodicInformEnable")
		info["tr069_PeriodicInformInterval"] =  x:get("tozed","cfg","tr069_PeriodicInformInterval") or ""
		info["tr069_ACS_auth"] =  x:get("tozed","cfg","tr069_ACS_auth")
		info["tr069_ServerUsername"] =  x:get("tozed","cfg","tr069_ServerUsername") or ""
		info["tr069_ServerPassword"] =  x:get("tozed","cfg","tr069_ServerPassword") or ""
		info["tr069_CPE_auth"] =  x:get("tozed","cfg","tr069_CPE_auth")
		info["tr069_ConnectionRequestUname"] =  x:get("tozed","cfg","tr069_ConnectionRequestUname") or ""
		info["tr069_ConnectionRequestPassword"] =  x:get("tozed","cfg","tr069_ConnectionRequestPassword") or ""

		return info
end

function system_module.system_set_tr069_app_enable(app_switch)

	if(nil ~= app_switch)
	then
		if("y" == app_switch)
		then
			x:set("tozed","cfg","tr069_app_enable","y")
		else
			x:set("tozed","cfg","tr069_app_enable","n")
		end
		x:commit("tozed")
		return 0
	end

	return -1
end

function system_module.system_set_tr069_ServerURL(url)

	if(nil ~= url)
	then
		x:set("tozed","cfg","tr069_ServerURL",url)
		x:commit("tozed")
		return 0
	end

	return -1
end

function system_module.system_set_tr069_PeriodicInformEnable(isOpen)

	if(nil ~= isOpen)
	then
		if("y" == isOpen)
		then
			x:set("tozed","cfg","tr069_PeriodicInformEnable","y")
		else
			x:set("tozed","cfg","tr069_PeriodicInformEnable","n")
		end
		x:commit("tozed")
		return 0
	end

	return -1
end

function system_module.system_set_tr069_PeriodicInformInterval(seconds)

	if(nil ~= seconds)
	then
		x:set("tozed","cfg","tr069_PeriodicInformInterval",seconds)
		x:commit("tozed")
		return 0
	end

	return -1
end

function system_module.system_set_tr069_tr069_ACS_auth(isOpen)

	if(nil ~= isOpen)
	then
		if("y" == isOpen)
		then
			x:set("tozed","cfg","tr069_ACS_auth","y")
		else
			x:set("tozed","cfg","tr069_ACS_auth","n")
		end
		x:commit("tozed")
		return 0
	end

	return -1
end

function system_module.system_set_tr069_ServerUsername(name)

	if(nil ~= name)
	then
		x:set("tozed","cfg","tr069_ServerUsername",name)
		x:commit("tozed")
		return 0
	end

	return -1
end

function system_module.system_set_tr069_ServerPassword(pwd)

	if(nil ~= pwd)
	then
		x:set("tozed","cfg","tr069_ServerPassword",pwd)
		x:commit("tozed")
		return 0
	end

	return -1
end

function system_module.system_set_tr069_tr069_tr069_CPE_auth(isOpen)

	if(nil ~= isOpen)
	then
		if("y" == isOpen)
		then
			x:set("tozed","cfg","tr069_CPE_auth","y")
		else
			x:set("tozed","cfg","tr069_CPE_auth","n")
		end
		x:commit("tozed")
		return 0
	end

	return -1
end

function system_module.system_set_tr069_ConnectionRequestUname(name)

	if(nil ~= name)
	then
		x:set("tozed","cfg","tr069_ConnectionRequestUname",name)
		x:commit("tozed")
		return 0
	end

	return -1
end

function system_module.system_set_tr069_ConnectionRequestPassword(pwd)

	if(nil ~= pwd)
	then
		x:set("tozed","cfg","tr069_ConnectionRequestPassword",pwd)
		x:commit("tozed")
		return 0
	end

	return -1
end


system_module.web_info = {
		
		["web_language_select_enable"] = nil,   --string
		["web_language"] = nil,   --string
		["web_user_show_hide_pref"] = nil,   --string
		["web_operator_show_hide_pref"] = nil
}


function system_module.web_info:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
		
	self["web_language_select_enable"] = obj["web_language_select_enable"] or nil
	self["web_language"] = obj["web_language"] or nil
	self["web_user_show_hide_pref"] = obj["web_user_show_hide_pref"] or nil
	self["web_operator_show_hide_pref"] = obj["web_operator_show_hide_pref"] or nil

end


function system_module.system_get_web_info()

		local info = system_module.web_info:new(nil,nil)

		info["web_language_select_enable"] =  x:get("tozed","web","TZ_WEB_SHOW_LANGUAGE_SELECT")
		info["web_language"] =  x:get("tozed","web","TZ_WEB_LANGUAGE")
		info["web_user_show_hide_pref"] =  x:get("tozed","web","TZ_WEB_USER_SHOW_HIDE_PREF")
		info["web_operator_show_hide_pref"] =  x:get("tozed","web","TZ_WEB_OPERATOR_SHOW_HIDE_PREF")
		

		return info
end

system_module.tozed_system_info = {
		
		["config_version"] = nil,   --string
		["software_version"] = nil,   --string
}


function system_module.tozed_system_info:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
		
	self["config_version"] = obj["config_version"] or nil
	self["software_version"] = obj["software_version"] or nil

end

function system_module.system_get_tozed_system_info()

		local info = system_module.tozed_system_info:new(nil,nil)

		info["config_version"] =  x:get("tozed","system","TZ_SYSTEM_CONFIG_VERSION")
		info["software_version"] =  x:get("tozed","system","TZ_SYSTEM_SOFTWARE_VERSION")
		
		if nil == info["software_version"]
		then
			local f=io.popen("cat /version  | grep software_version | cut -d'=' -f 2")
			info["software_version"] = f:read()
			io.close(f)
		end

		return info
end


system_module.lte_ant_status = {
		
		["main_ant"] = nil,   --string
		["sub_ant"] = nil,   --string
}


function system_module.lte_ant_status:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
		
	self["main_ant"] = obj["main_ant"] or nil
	self["sub_ant"] = obj["sub_ant"] or nil

end
--if true ~= util.is_file_exist(file_path)
function system_module.system_get_4g_ant()
	local info = system_module.lte_ant_status:new(nil,nil)
	if true ~= util.is_file_exist("/sys/class/gpio/gpio4") then
		os.execute("echo 3 > /sys/class/gpio/export");
		os.execute("echo 4 > /sys/class/gpio/export");
		os.execute("echo in > /sys/class/gpio/gpio3/direction");
		os.execute("echo in > /sys/class/gpio/gpio4/direction");
	end

	info["main_ant"] = execute_cmd("cat /sys/class/gpio/gpio4/value")
	info["sub_ant"] = execute_cmd("cat /sys/class/gpio/gpio3/value")
	return info
end


local timezone_location = {
	["Africa/Abidjan"]= "GMT0",
	["Africa/Accra"]= "GMT0",
	["Africa/AddisAbaba"]= "EAT-3",
	["Africa/Algiers"]= "CET-1",
	["Africa/Asmara"]= "EAT-3",
	["Africa/Bamako"]= "GMT0",
	["Africa/Bangui"]= "WAT-1",
	["Africa/Banjul"]= "GMT0",
	["Africa/Bissau"]= "GMT0",
	["Africa/Blantyre"]= "CAT-2",
	["Africa/Brazzaville"]= "WAT-1",
	["Africa/Bujumbura"]= "CAT-2",
	["Africa/Casablanca"]= "WET0",
	["Africa/Ceuta"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	["Africa/Conakry"]= "GMT0",
	["Africa/Dakar"]= "GMT0",
	["Africa/DaresSalaam"]= "EAT-3",
	["Africa/Djibouti"]= "EAT-3",
	["Africa/Douala"]= "WAT-1",
	["Africa/ElAaiun"]= "WET0",
	["Africa/Freetown"]= "GMT0",
	["Africa/Gaborone"]= "CAT-2",
	["Africa/Harare"]= "CAT-2",
	["Africa/Johannesburg"]= "SAST-2",
	["Africa/Kampala"]= "EAT-3",
	["Africa/Khartoum"]= "EAT-3",
	["Africa/Kigali"]= "CAT-2",
	["Africa/Kinshasa"]= "WAT-1",
	["Africa/Lagos"]= "WAT-1",
	["Africa/Libreville"]= "WAT-1",
	["Africa/Lome"]= "GMT0",
	["Africa/Luanda"]= "WAT-1",
	["Africa/Lubumbashi"]= "CAT-2",
	["Africa/Lusaka"]= "CAT-2",
	["Africa/Malabo"]= "WAT-1",
	["Africa/Maputo"]= "CAT-2",
	["Africa/Maseru"]= "SAST-2",
	["Africa/Mbabane"]= "SAST-2",
	["Africa/Mogadishu"]= "EAT-3",
	["Africa/Monrovia"]= "GMT0",
	["Africa/Nairobi"]= "EAT-3",
	["Africa/Ndjamena"]= "WAT-1",
	["Africa/Niamey"]= "WAT-1",
	["Africa/Nouakchott"]= "GMT0",
	["Africa/Ouagadougou"]= "GMT0",
	["Africa/Porto-Novo"]= "WAT-1",
	["Africa/SaoTome"]= "GMT0",
	["Africa/Tripoli"]= "EET-2",
	["Africa/Tunis"]= "CET-1",
	["Africa/Windhoek"]= "WAT-1WAST,M9.1.0,M4.1.0",
	["America/Adak"]= "HAST10HADT,M3.2.0,M11.1.0",
	["America/Anchorage"]= "AKST9AKDT,M3.2.0,M11.1.0",
	["America/Anguilla"]= "AST4",
	["America/Antigua"]= "AST4",
	["America/Araguaina"]= "BRT3",
	["America/Argentina/Buenos Aires"]= "ART3",
	["America/Argentina/Catamarca"]= "ART3",
	["America/Argentina/Cordoba"]= "ART3",
	["America/Argentina/Jujuy"]= "ART3",
	["America/Argentina/LaRioja"]= "ART3",
	["America/Argentina/Mendoza"]= "ART3",
	["America/Argentina/Rio Gallegos"]= "ART3",
	["America/Argentina/Salta"]= "ART3",
	["America/Argentina/SanJuan"]= "ART3",
	["America/Argentina/Tucuman"]= "ART3",
	["America/Argentina/Ushuaia"]= "ART3",
	["America/Aruba"]= "AST4",
	["America/Asuncion"]= "PYT4PYST,M10.1.0/0,M4.2.0/0",
	["America/Atikokan"]= "EST5",
	["America/Bahia"]= "BRT3",
	["America/Barbados"]= "AST4",
	["America/Belem"]= "BRT3",
	["America/Belize"]= "CST6",
	["America/Blanc-Sablon"]= "AST4",
	["America/BoaVista"]= "AMT4",
	["America/Bogota"]= "COT5",
	["America/Boise"]= "MST7MDT,M3.2.0,M11.1.0",
	["America/CambridgeBay"]= "MST7MDT,M3.2.0,M11.1.0",
	["America/CampoGrande"]= "AMT4AMST,M10.3.0/0,M2.3.0/0",
	["America/Cancun"]= "CST6CDT,M4.1.0,M10.5.0",
	["America/Caracas"]= "VET4:30",
	["America/Cayenne"]= "GFT3",
	["America/Cayman"]= "EST5",
	["America/Chicago"]= "CST6CDT,M3.2.0,M11.1.0",
	["America/Chihuahua"]= "MST7MDT,M4.1.0,M10.5.0",
	["America/CostaRica"]= "CST6",
	["America/Cuiaba"]= "AMT4AMST,M10.3.0/0,M2.3.0/0",
	["America/Curacao"]= "AST4",
	["America/Danmarkshavn"]= "GMT0",
	["America/Dawson"]= "PST8PDT,M3.2.0,M11.1.0",
	["America/DawsonCreek"]= "MST7",
	["America/Denver"]= "MST7MDT,M3.2.0,M11.1.0",
	["America/Detroit"]= "EST5EDT,M3.2.0,M11.1.0",
	["America/Dominica"]= "AST4",
	["America/Edmonton"]= "MST7MDT,M3.2.0,M11.1.0",
	["America/Eirunepe"]= "AMT4",
	["America/ElSalvador"]= "CST6",
	["America/Fortaleza"]= "BRT3",
	["America/GlaceBay"]= "AST4ADT,M3.2.0,M11.1.0",
	 ["America/GooseBay"]= "AST4ADT,M3.2.0/0:01,M11.1.0/0:01",
	 ["America/GrandTurk"]= "EST5EDT,M3.2.0,M11.1.0",
	 ["America/Grenada"]= "AST4",
	 ["America/Guadeloupe"]= "AST4",
	 ["America/Guatemala"]= "CST6",
	 ["America/Guayaquil"]= "ECT5",
	 ["America/Guyana"]= "GYT4",
	 ["America/Halifax"]= "AST4ADT,M3.2.0,M11.1.0",
	 ["America/Havana"]= "CST5CDT,M3.2.0/0,M10.5.0/1",
	 ["America/Hermosillo"]= "MST7",
	 ["America/Indiana/Indianapolis"]= "EST5EDT,M3.2.0,M11.1.0",
	 ["America/Indiana/Knox"]= "CST6CDT,M3.2.0,M11.1.0",
	 ["America/Indiana/Marengo"]= "EST5EDT,M3.2.0,M11.1.0",
	 ["America/Indiana/Petersburg"]= "EST5EDT,M3.2.0,M11.1.0",
	 ["America/Indiana/TellCity"]= "CST6CDT,M3.2.0,M11.1.0",
	 ["America/Indiana/Vevay"]= "EST5EDT,M3.2.0,M11.1.0",
	 ["America/Indiana/Vincennes"]= "EST5EDT,M3.2.0,M11.1.0",
	 ["America/Indiana/Winamac"]= "EST5EDT,M3.2.0,M11.1.0",
	 ["America/Inuvik"]= "MST7MDT,M3.2.0,M11.1.0",
	 ["America/Iqaluit"]= "EST5EDT,M3.2.0,M11.1.0",
	 ["America/Jamaica"]= "EST5",
	 ["America/Juneau"]= "AKST9AKDT,M3.2.0,M11.1.0",
	 ["America/Kentucky/Louisville"]= "EST5EDT,M3.2.0,M11.1.0",
	 ["America/Kentucky/Monticello"]= "EST5EDT,M3.2.0,M11.1.0",
	 ["America/LaPaz"]= "BOT4",
	 ["America/Lima"]= "PET5",
	 ["America/LosAngeles"]= "PST8PDT,M3.2.0,M11.1.0",
	 ["America/Maceio"]= "BRT3",
	 ["America/Managua"]= "CST6",
	 ["America/Manaus"]= "AMT4",
	 ["America/Marigot"]= "AST4",
	 ["America/Martinique"]= "AST4",
	 ["America/Matamoros"]= "CST6CDT,M3.2.0,M11.1.0",
	 ["America/Mazatlan"]= "MST7MDT,M4.1.0,M10.5.0",
	 ["America/Menominee"]= "CST6CDT,M3.2.0,M11.1.0",
	 ["America/Merida"]= "CST6CDT,M4.1.0,M10.5.0",
	 ["America/MexicoCity"]= "CST6CDT,M4.1.0,M10.5.0",
	 ["America/Miquelon"]= "PMST3PMDT,M3.2.0,M11.1.0",
	 ["America/Moncton"]= "AST4ADT,M3.2.0,M11.1.0",
	 ["America/Monterrey"]= "CST6CDT,M4.1.0,M10.5.0",
	 ["America/Montevideo"]= "UYT3UYST,M10.1.0,M3.2.0",
	 ["America/Montreal"]= "EST5EDT,M3.2.0,M11.1.0",
	 ["America/Montserrat"]= "AST4",
	 ["America/Nassau"]= "EST5EDT,M3.2.0,M11.1.0",
	 ["America/NewYork"]= "EST5EDT,M3.2.0,M11.1.0",
	 ["America/Nipigon"]= "EST5EDT,M3.2.0,M11.1.0",
	 ["America/Nome"]= "AKST9AKDT,M3.2.0,M11.1.0",
	 ["America/Noronha"]= "FNT2",
	 ["America/NorthDakota/Center"]= "CST6CDT,M3.2.0,M11.1.0",
	 ["America/North Dakota/New Salem"]= "CST6CDT,M3.2.0,M11.1.0",
	 ["America/Ojinaga"]= "MST7MDT,M3.2.0,M11.1.0",
	 ["America/Panama"]= "EST5",
	 ["America/Pangnirtung"]= "EST5EDT,M3.2.0,M11.1.0",
	 ["America/Paramaribo"]= "SRT3",
	 ["America/Phoenix"]= "MST7",
	 ["America/PortofSpain"]= "AST4",
	 ["America/Port-au-Prince"]= "EST5",
	 ["America/PortoVelho"]= "AMT4",
	 ["America/PuertoRico"]= "AST4",
	 ["America/RainyRiver"]= "CST6CDT,M3.2.0,M11.1.0",
	 ["America/RankinInlet"]= "CST6CDT,M3.2.0,M11.1.0",
	 ["America/Recife"]= "BRT3",
	 ["America/Regina"]= "CST6",
	 ["America/RioBranco"]= "AMT4",
	 ["America/SantaIsabel"]= "PST8PDT,M4.1.0,M10.5.0",
	 ["America/Santarem"]= "BRT3",
	 ["America/SantoDomingo"]= "AST4",
	 ["America/SaoPaulo"]= "BRT3BRST,M10.3.0/0,M2.3.0/0",
	 ["America/Scoresbysund"]= "EGT1EGST,M3.5.0/0,M10.5.0/1",
	 ["America/Shiprock"]= "MST7MDT,M3.2.0,M11.1.0",
	 ["America/StBarthelemy"]= "AST4",
	 ["America/StJohns"]= "NST3:30NDT,M3.2.0/0:01,M11.1.0/0:01",
	 ["America/StKitts"]= "AST4",
	 ["America/StLucia"]= "AST4",
	 ["America/StThomas"]= "AST4",
	 ["America/StVincent"]= "AST4",
	 ["America/SwiftCurrent"]= "CST6",
	 ["America/Tegucigalpa"]= "CST6",
	 ["America/Thule"]= "AST4ADT,M3.2.0,M11.1.0",
	 ["America/ThunderBay"]= "EST5EDT,M3.2.0,M11.1.0",
	 ["America/Tijuana"]= "PST8PDT,M3.2.0,M11.1.0",
	 ["America/Toronto"]= "EST5EDT,M3.2.0,M11.1.0",
	 ["America/Tortola"]= "AST4",
	 ["America/Vancouver"]= "PST8PDT,M3.2.0,M11.1.0",
	 ["America/Whitehorse"]= "PST8PDT,M3.2.0,M11.1.0",
	 ["America/Winnipeg"]= "CST6CDT,M3.2.0,M11.1.0",
	 ["America/Yakutat"]= "AKST9AKDT,M3.2.0,M11.1.0",
	 ["America/Yellowknife"]= "MST7MDT,M3.2.0,M11.1.0",
	 ["Antarctica/Casey"]= "WST-8",
	 ["Antarctica/Davis"]= "DAVT-7",
	 ["Antarctica/DumontDUrville"]= "DDUT-10",
	 ["Antarctica/Macquarie"]= "MIST-11",
	 ["Antarctica/Mawson"]= "MAWT-5",
	 ["Antarctica/McMurdo"]= "NZST-12NZDT,M9.5.0,M4.1.0/3",
	 ["Antarctica/Rothera"]= "ROTT3",
	 ["Antarctica/SouthPole"]= "NZST-12NZDT,M9.5.0,M4.1.0/3",
	 ["Antarctica/Syowa"]= "SYOT-3",
	 ["Antarctica/Vostok"]= "VOST-6",
	 ["Arctic/Longyearbyen"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Asia/Aden"]= "AST-3",
	 ["Asia/Almaty"]= "ALMT-6",
	 ["Asia/Anadyr"]= "ANAT-11ANAST,M3.5.0,M10.5.0/3",
	 ["Asia/Aqtau"]= "AQTT-5",
	 ["Asia/Aqtobe"]= "AQTT-5",
	 ["Asia/Ashgabat"]= "TMT-5",
	 ["Asia/Baghdad"]= "AST-3",
	 ["Asia/Bahrain"]= "AST-3",
	 ["Asia/Baku"]= "AZT-4AZST,M3.5.0/4,M10.5.0/5",
	 ["Asia/Bangkok"]= "ICT-7",
	 ["Asia/Beirut"]= "EET-2EEST,M3.5.0/0,M10.5.0/0",
	 ["Asia/Bishkek"]= "KGT-6",
	 ["Asia/Brunei"]= "BNT-8",
	 ["Asia/Choibalsan"]= "CHOT-8",
	 ["Asia/Chongqing"]= "CST-8",
	 ["Asia/Colombo"]= "IST-5:30",
	 ["Asia/Damascus"]= "EET-2EEST,M4.1.5/0,M10.5.5/0",
	 ["Asia/Dhaka"]= "BDT-6",
	 ["Asia/Dili"]= "TLT-9",
	 ["Asia/Dubai"]= "GST-4",
	 ["Asia/Dushanbe"]= "TJT-5",
	 ["Asia/Gaza"]= "EET-2EEST,M3.5.6/0:01,M9.1.5",
	 ["Asia/Harbin"]= "CST-8",
	 ["Asia/HoChiMinh"]= "ICT-7",
	 ["Asia/HongKong"]= "HKT-8",
	 ["Asia/Hovd"]= "HOVT-7",
	 ["Asia/Irkutsk"]= "IRKT-8IRKST,M3.5.0,M10.5.0/3",
	 ["Asia/Jakarta"]= "WIT-7",
	 ["Asia/Jayapura"]= "EIT-9",
	 ["Asia/Kabul"]= "AFT-4:30",
	 ["Asia/Kamchatka"]= "PETT-11PETST,M3.5.0,M10.5.0/3",
	 ["Asia/Karachi"]= "PKT-5",
	 ["Asia/Kashgar"]= "CST-8",
	 ["Asia/Kathmandu"]= "NPT-5:45",
	 ["Asia/Kolkata"]= "IST-5:30",
	 ["Asia/Krasnoyarsk"]= "KRAT-7KRAST,M3.5.0,M10.5.0/3",
	 ["Asia/KualaLumpur"]= "MYT-8",
	 ["Asia/Kuching"]= "MYT-8",
	 ["Asia/Kuwait"]= "AST-3",
	 ["Asia/Macau"]= "CST-8",
	 ["Asia/Magadan"]= "MAGT-11MAGST,M3.5.0,M10.5.0/3",
	 ["Asia/Makassar"]= "CIT-8",
	 ["Asia/Manila"]= "PHT-8",
	 ["Asia/Muscat"]= "GST-4",
	 ["Asia/Nicosia"]= "EET-2EEST,M3.5.0/3,M10.5.0/4",
	 ["Asia/Novokuznetsk"]= "NOVT-6NOVST,M3.5.0,M10.5.0/3",
	 ["Asia/Novosibirsk"]= "NOVT-6NOVST,M3.5.0,M10.5.0/3",
	 ["Asia/Omsk"]= "OMST-7",
	 ["Asia/Oral"]= "ORAT-5",
	 ["Asia/PhnomPenh"]= "ICT-7",
	 ["Asia/Pontianak"]= "WIT-7",
	 ["Asia/Pyongyang"]= "KST-9",
	 ["Asia/Qatar"]= "AST-3",
	 ["Asia/Qyzylorda"]= "QYZT-6",
	 ["Asia/Rangoon"]= "MMT-6:30",
	 ["Asia/Riyadh"]= "AST-3",
	 ["Asia/Sakhalin"]= "SAKT-10SAKST,M3.5.0,M10.5.0/3",
	 ["Asia/Samarkand"]= "UZT-5",
	 ["Asia/Seoul"]= "KST-9",
	 ["Asia/Shanghai"]= "CST-8",
	 ["Asia/Singapore"]= "SGT-8",
	 ["Asia/Taipei"]= "CST-8",
	 ["Asia/Tashkent"]= "UZT-5",
	 ["Asia/Tbilisi"]= "GET-4",
	 ["Asia/Tehran"]= "IRST-3:30IRDT,80/0,264/0",
	 ["Asia/Thimphu"]= "BTT-6",
	 ["Asia/Tokyo"]= "JST-9",
	 ["Asia/Ulaanbaatar"]= "ULAT-8",
	 ["Asia/Urumqi"]= "CST-8",
	 ["Asia/Vientiane"]= "ICT-7",
	 ["Asia/Vladivostok"]= "VLAT-10VLAST,M3.5.0,M10.5.0/3",
	 ["Asia/Yakutsk"]= "YAKT-9YAKST,M3.5.0,M10.5.0/3",
	 ["Asia/Yekaterinburg"]= "YEKT-5YEKST,M3.5.0,M10.5.0/3",
	 ["Asia/Yerevan"]= "AMT-4AMST,M3.5.0,M10.5.0/3",
	 ["Atlantic/Azores"]= "AZOT1AZOST,M3.5.0/0,M10.5.0/1",
	 ["Atlantic/Bermuda"]= "AST4ADT,M3.2.0,M11.1.0",
	 ["Atlantic/Canary"]= "WET0WEST,M3.5.0/1,M10.5.0",
	 ["Atlantic/CapeVerde"]= "CVT1",
	 ["Atlantic/Faroe"]= "WET0WEST,M3.5.0/1,M10.5.0",
	 ["Atlantic/Madeira"]= "WET0WEST,M3.5.0/1,M10.5.0",
	 ["Atlantic/Reykjavik"]= "GMT0",
	 ["Atlantic/SouthGeorgia"]= "GST2",
	 ["Atlantic/StHelena"]= "GMT0",
	 ["Atlantic/Stanley"]= "FKT4FKST,M9.1.0,M4.3.0",
	 ["Australia/Adelaide"]= "CST-9:30CST,M10.1.0,M4.1.0/3",
	 ["Australia/Brisbane"]= "EST-10",
	 ["Australia/BrokenHill"]= "CST-9:30CST,M10.1.0,M4.1.0/3",
	 ["Australia/Currie"]= "EST-10EST,M10.1.0,M4.1.0/3",
	 ["Australia/Darwin"]= "CST-9:30",
	 ["Australia/Eucla"]= "CWST-8:45",
	 ["Australia/Hobart"]= "EST-10EST,M10.1.0,M4.1.0/3",
	 ["Australia/Lindeman"]= "EST-10",
	 ["Australia/LordHowe"]= "LHST-10:30LHST-11,M10.1.0,M4.1.0",
	 ["Australia/Melbourne"]= "EST-10EST,M10.1.0,M4.1.0/3",
	 ["Australia/Perth"]= "WST-8",
	 ["Australia/Sydney"]= "EST-10EST,M10.1.0,M4.1.0/3",
	 ["Europe/Amsterdam"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Andorra"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Athens"]= "EET-2EEST,M3.5.0/3,M10.5.0/4",
	 ["Europe/Belgrade"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Berlin"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Bratislava"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Brussels"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Bucharest"]= "EET-2EEST,M3.5.0/3,M10.5.0/4",
	 ["Europe/Budapest"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Chisinau"]= "EET-2EEST,M3.5.0/3,M10.5.0/4",
	 ["Europe/Copenhagen"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Dublin"]= "GMT0IST,M3.5.0/1,M10.5.0",
	 ["Europe/Gibraltar"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Guernsey"]= "GMT0BST,M3.5.0/1,M10.5.0",
	 ["Europe/Helsinki"]= "EET-2EEST,M3.5.0/3,M10.5.0/4",
	 ["Europe/IsleofMan"]= "GMT0BST,M3.5.0/1,M10.5.0",
	 ["Europe/Istanbul"]= "EET-2EEST,M3.5.0/3,M10.5.0/4",
	 ["Europe/Jersey"]= "GMT0BST,M3.5.0/1,M10.5.0",
	 ["Europe/Kaliningrad"]= "EET-2EEST,M3.5.0,M10.5.0/3",
	 ["Europe/Kiev"]= "EET-2EEST,M3.5.0/3,M10.5.0/4",
	 ["Europe/Lisbon"]= "WET0WEST,M3.5.0/1,M10.5.0",
	 ["Europe/Ljubljana"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/London"]= "GMT0BST,M3.5.0/1,M10.5.0",
	 ["Europe/Luxembourg"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Madrid"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Malta"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Mariehamn"]= "EET-2EEST,M3.5.0/3,M10.5.0/4",
	 ["Europe/Minsk"]= "EET-2EEST,M3.5.0,M10.5.0/3",
	 ["Europe/Monaco"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Moscow"]= "MSK-3",
	 ["Europe/Oslo"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Paris"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Podgorica"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Prague"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Riga"]= "EET-2EEST,M3.5.0/3,M10.5.0/4",
	 ["Europe/Rome"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Samara"]= "SAMT-3SAMST,M3.5.0,M10.5.0/3",
	 ["Europe/SanMarino"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Sarajevo"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Simferopol"]= "EET-2EEST,M3.5.0/3,M10.5.0/4",
	 ["Europe/Skopje"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Sofia"]= "EET-2EEST,M3.5.0/3,M10.5.0/4",
	 ["Europe/Stockholm"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Tallinn"]= "EET-2EEST,M3.5.0/3,M10.5.0/4",
	 ["Europe/Tirane"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Uzhgorod"]= "EET-2EEST,M3.5.0/3,M10.5.0/4",
	 ["Europe/Vaduz"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Vatican"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Vienna"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Vilnius"]= "EET-2EEST,M3.5.0/3,M10.5.0/4",
	 ["Europe/Volgograd"]= "VOLT-3VOLST,M3.5.0,M10.5.0/3",
	 ["Europe/Warsaw"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Zagreb"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Europe/Zaporozhye"]= "EET-2EEST,M3.5.0/3,M10.5.0/4",
	 ["Europe/Zurich"]= "CET-1CEST,M3.5.0,M10.5.0/3",
	 ["Indian/Antananarivo"]= "EAT-3",
	 ["Indian/Chagos"]= "IOT-6",
	 ["Indian/Christmas"]= "CXT-7",
	 ["Indian/Cocos"]= "CCT-6:30",
	 ["Indian/Comoro"]= "EAT-3",
	 ["Indian/Kerguelen"]= "TFT-5",
	 ["Indian/Mahe"]= "SCT-4",
	 ["Indian/Maldives"]= "MVT-5",
	 ["Indian/Mauritius"]= "MUT-4",
	 ["Indian/Mayotte"]= "EAT-3",
	 ["Indian/Reunion"]= "RET-4",
	 ["Pacific/Apia"]= "WST11",
	 ["Pacific/Auckland"]= "NZST-12NZDT,M9.5.0,M4.1.0/3",
	 ["Pacific/Chatham"]= "CHAST-12:45CHADT,M9.5.0/2:45,M4.1.0/3:45",
	 ["Pacific/Efate"]= "VUT-11",
	 ["Pacific/Enderbury"]= "PHOT-13",
	 ["Pacific/Fakaofo"]= "TKT10",
	 ["Pacific/Fiji"]= "FJT-12",
	 ["Pacific/Funafuti"]= "TVT-12",
	 ["Pacific/Galapagos"]= "GALT6",
	 ["Pacific/Gambier"]= "GAMT9",
	 ["Pacific/Guadalcanal"]= "SBT-11",
	 ["Pacific/Guam"]= "ChST-10",
	 ["Pacific/Honolulu"]= "HST10",
	 ["Pacific/Johnston"]= "HST10",
	 ["Pacific/Kiritimati"]= "LINT-14",
	 ["Pacific/Kosrae"]= "KOST-11",
	 ["Pacific/Kwajalein"]= "MHT-12",
	 ["Pacific/Majuro"]= "MHT-12",
	 ["Pacific/Marquesas"]= "MART9:30",
	 ["Pacific/Midway"]= "SST11",
	 ["Pacific/Nauru"]= "NRT-12",
	 ["Pacific/Niue"]= "NUT11",
	 ["Pacific/Norfolk"]= "NFT-11:30",
	 ["Pacific/Noumea"]= "NCT-11",
	 ["Pacific/PagoPago"]= "SST11",
	 ["Pacific/Palau"]= "PWT-9",
	 ["Pacific/Pitcairn"]= "PST8",
	 ["Pacific/Ponape"]= "PONT-11",
	 ["Pacific/PortMoresby"]= "PGT-10",
	 ["Pacific/Rarotonga"]= "CKT10",
	 ["Pacific/Saipan"]= "ChST-10",
	 ["Pacific/Tahiti"]= "TAHT10",
	 ["Pacific/Tarawa"]= "GILT-12",
	 ["Pacific/Tongatapu"]= "TOT-13",
	 ["Pacific/Truk"]= "TRUT-10",
	 ["Pacific/Wake"]= "WAKT-12",
	 ["Pacific/Wallis"]= "WFT-12",
 }
local function reload_config()
	return 0 == os.execute("/etc/init.d/system restart")
end

-- get ntp time sync enable or disable
-- input:none
-- return:
--		1:enable
--		0:disable
function system_module.system_ntp_get_enable_status()
	return x:get(SYSTEM_CONFIG_FILE,"ntp", "enabled")
end

--enable ntp
function system_module.system_ntp_enable()
	x:set(SYSTEM_CONFIG_FILE, "ntp", "enabled", "1")

	local ret =  x:commit(SYSTEM_CONFIG_FILE)
	return ret and reload_config()
end

--disenable ntp
function system_module.system_ntp_disable()
	x:set(SYSTEM_CONFIG_FILE, "ntp", "enabled", "0")


	local ret =  x:commit(SYSTEM_CONFIG_FILE)
	return ret and reload_config()
end

--get time zone name
--input:none
--return:the time zone name which in the key string of timezone_location above
function system_module.system_ntp_get_zonename()
	local timezone
	x:foreach(SYSTEM_CONFIG_FILE, "system", function(s)
		timezone = s["zonename"]
	end)

	return timezone
end

--set time zone by zone name
--input:zonename(string):the key string of array timezone_location above
--return:true if success  false if fail
function system_module.system_ntp_set_zonename(zonename)

	for key,value in pairs(timezone_location)
	do
		
		local ret = false
		if key == zonename
		then
			x:foreach(SYSTEM_CONFIG_FILE, "system", function(s)
				x:set(SYSTEM_CONFIG_FILE,s[".name"], "zonename", key)
				x:set(SYSTEM_CONFIG_FILE,s[".name"], "timezone", value)

			end)
			ret = x:commit(SYSTEM_CONFIG_FILE)
			return ret and reload_config()
		end

	end
	debug("error:please check input value.")
	return false
end

--get time time zone 
--input:none
--return:the number represent the time zone like +8 = "west 8"  -8="east 8"
function system_module.system_ntp_get_timezone()
	local timezone
	x:foreach(SYSTEM_CONFIG_FILE, "system", function(s)
		timezone = s["timezone"]
		local index = string.find(timezone, "CST")
		if nil == index
		then
			index = string.find(timezone, ">")
			return tonumber(string.sub(timezone, index+1, string.len(timezone)))
		end
		timezone = string.sub(timezone, index+3, string.len(timezone))
	end)

	return tonumber(timezone)
end

--set time zone by time zone
--input:zonename(string):the number represent the time zone like +8 = "west 8"  -8="east 8"
--return:true if success  false if fail
function system_module.system_ntp_set_timezone(timezone)

	if type(timezone) ~= "number"
	then
		debug("timezone error")
		return false
	end

	x:foreach(SYSTEM_CONFIG_FILE, "system", function(s)
		if timezone > 0
		then
			x:set(SYSTEM_CONFIG_FILE,s[".name"], "timezone", "CST+"..timezone)
		else
			x:set(SYSTEM_CONFIG_FILE,s[".name"], "timezone", "CST"..timezone)
		end

	end)

	return  x:commit(SYSTEM_CONFIG_FILE) and reload_config()

end

--get ntp server address list
--input:none
--return:the array list of server address
function system_module.system_ntp_get_server_address()
	local server_list = x:get(SYSTEM_CONFIG_FILE,"ntp","server")

	return server_list
end

--set ntp server address list
--input:server_list(string list):the array list of server address
--return:true if success  false if fail
function system_module.system_ntp_set_server_address(server_list)
	x:set(SYSTEM_CONFIG_FILE,"ntp","server", server_list)

	return x:commit(SYSTEM_CONFIG_FILE) and reload_config()
end

--set date
--input:date_string(string):the date time in YYYY-MM-DD HH:MM:SS format
--return:true if success  false if fail
function system_module.system_ntp_set_date(date_string)
	
	if string.match(date_string, "%d+-%d+-%d+ %d+:%d+:%d+") == nil
	then
		debug("date_string format error")
		return false
	end

	return 0 == os.execute(string.format("date -s '%s' > /dev/null", date_string))
end

system_module.lanrecord_current_info = {
		
		["mac"] = nil,   -- string  the user mac
		["flow"] = nil, -- number   the total flow which this mac use  in byte
		["ipaddr"] = nil, -- string the ip address which the user is assign now
		["host_name"] = nil, -- string device name
		["interface"] = nil, -- string the access interface
		["ssid"] = nil , -- string the access ssid , '*'' if access by land(eth0.1)
		["lease_time"] = nil, -- number the dhcp lease time in second(s)

}


function system_module.lanrecord_current_info:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
		
	self["mac"] = obj["mac"] or nil
	self["flow"] = obj["flow"] or nil
	self["ipaddr"] = obj["ipaddr"] or nil
	self["host_name"] = obj["host_name"] or nil
	self["interface"] = obj["interface"] or nil
	self["ssid"] = obj["ssid"] or nil
	self["lease_time"] = obj["lease_time"] or nil
end


-- get the current client flow  detail
-- input:none
-- return:the array of the struct of lanrecord_current_info
function system_module.system_lanrecord_get_current_info()
	 local f = io.open(LANRECORD_CURRENT_FILE)
	 local list = {}
	 local count = 1
	 local arr = nil
	 local info = {}
	 res = f:read()
	 
	 while nil ~= res
	do
		local info = system_module.lanrecord_current_info:new(nil,nil)
		arr = split(res,' |')
		info["mac"] = arr[1]
		info["ipaddr"] = arr[2]
		info["host_name"] = arr[3]
		info["interface"] = arr[4]
		info["ssid"] = arr[5]
		info["lease_time"] = tonumber(arr[6])
		info["flow"] = tonumber(arr[7])
		list[count] = info
		count = count + 1
 		res = f:read()
	end
	

	io.close(f)
	return list
end

-- get the histroy login client flow  detail
-- input:none
-- return:the array of the struct of lanrecord_history_info
function system_module.system_lanrecord_get_history_info()
	 local f = io.open(LANRECORD_HISTORY_FILE)
	 local list = {}
	 local count = 1
	 local arr = nil
	 local info = {}
	 res = f:read()
	 
	 while nil ~= res
	do
		local info = system_module.lanrecord_current_info:new(nil,nil)
		arr = split(res,' |')
		info["mac"] = arr[1]
		info["ipaddr"] = arr[2]
		info["host_name"] = arr[3]
		info["interface"] = arr[4]
		info["ssid"] = arr[5]
		info["lease_time"] = arr[6]
		info["flow"] = tonumber(arr[7])
		list[count] = info
		count = count + 1
 		res = f:read()
	end
	

	io.close(f)
	return list
end

local ddns_services = {
["oray"]="phddns", -- https://hsk.oray.com/ ygl921 123456 phddns60.oray.net
["pubyun"]="ez-ipupdate", -- http://www.pubyun.com ygl921 123456 sztozedtest.f3322.net
["noip"]="yaddns" --https://www.noip.com/  sztozedtest 123456 sztozed.ddns.net
}

local function check_ddns_service(name)

	for k,v in pairs(ddns_services)
	do
		if k == name
		then
			return true
		end

	end

	return false
end
local function get_ddns_application_name(name)
	for k,v in pairs(ddns_services)
	do
		if k == name
		then
			return v
		end

	end

	return nil
end

local function get_ddns_service_name(name)
	for k,v in pairs(ddns_services)
	do
		if v == name
		then
			return k
		end

	end

	return nil
end
-- get ddns service offers which our device support
-- input:none
-- return:an array of ddns service offers' name
function system_module.system_ddns_get_services()
	
	local service_name = {}
	local i =  1
	for k,v in pairs(ddns_services)
	do
		service_name[i] = k
		i = i + 1
	end

	return service_name
end

system_module.ddns_config = {
		
	["enable"] = nil,  		-- ddns enable status value 0:disable 1:enable
	["service"] = nil,	 	-- ddns service offer,get by system_ddns_get_service
	["username"] = nil, 	-- service uesrname id
	["password"] = nil,  	-- service password
	["hostname"] = nil,     -- the hostname which the ddns need to translate
}

function system_module.ddns_config:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
		
	self["enable"] = obj["enable"] or nil
	self["service"] = obj["service"] or nil 
	self["username"] = obj["username"] or nil
	self["password"] = obj["password"] or nil
	self["hostname"] = obj["hostname"] or nil
end

-- config ddns 
-- input:the struct of ddns_config 
-- return:true if success  false if fail
function system_module.system_ddns_set_config(config)

	if nil == config or nil == config["enable"]
	then
		debug("error input")
		return false
	end

	if 0 == config["enable"]
	then
		x:set(TOZED_CONFIG_FILE,TOZED_DDNS_SECTION,"value")
		x:set(TOZED_CONFIG_FILE,TOZED_DDNS_SECTION, "enable", 0)
		return x:commit(TOZED_CONFIG_FILE)
	end

	if not check_ddns_service(config["service"]) or 1 ~= config["enable"]
	then
		debug("input:error service or error enable ")
		return false
	end

	x:set(TOZED_CONFIG_FILE,TOZED_DDNS_SECTION,"value")
	x:set(TOZED_CONFIG_FILE,TOZED_DDNS_SECTION,"enable", 1)
	x:set(TOZED_CONFIG_FILE,TOZED_DDNS_SECTION,"service",get_ddns_application_name(config["service"]))
	x:set(TOZED_CONFIG_FILE,TOZED_DDNS_SECTION,"username",config["username"])
	x:set(TOZED_CONFIG_FILE,TOZED_DDNS_SECTION,"password",config["password"])
	x:set(TOZED_CONFIG_FILE,TOZED_DDNS_SECTION,"hostname",config["hostname"])

	return x:commit(TOZED_CONFIG_FILE)
end

-- get ddns config  
-- input:none
-- return:the struct of ddns_config 
function system_module.system_ddns_get_config()
	local config = system_module.ddns_config:new(nil,nil)
	config["enable"] = x:get(TOZED_CONFIG_FILE,TOZED_DDNS_SECTION,"enable")
	config["service"] = x:get(TOZED_CONFIG_FILE,TOZED_DDNS_SECTION,"service")
	config["username"] = x:get(TOZED_CONFIG_FILE,TOZED_DDNS_SECTION,"username")
	config["password"] = x:get(TOZED_CONFIG_FILE,TOZED_DDNS_SECTION,"password")
	config["hostname"] = x:get(TOZED_CONFIG_FILE,TOZED_DDNS_SECTION,"hostname")

	if nil ~= config["service"]
	then
		config["service"] = get_ddns_service_name(config["service"])
	end

	if nil == config["enable"]
	then
		config["enable"] = 0
	end
	return config
end


-- get the ddns service status
-- input:none
-- return: one of the following string:
--okConnected 			
--okAuthpassed 			
--okDomainListed,		
--okDomainsRegistered
--okKeepAliveRecved
--okConnecting
--okRetrievingMisc
--okRedirecting
--errorConnectFailed
--errorSocketInitialFailed
--errorDomainListFailed
--errorDomainRegisterFailed
--errorUpdateTimeout
--errorKeepAliveError
--errorRetrying,
--errorAuthBusy
--errorStatDetailInfoFailed
--okNormal
--okNoData				
--okServerER	
--errorOccupyReconnect
--errorNull
--errorAuthFailed
--errorOccupy
--errorNotYourDomain
--errorServerBlock
--ipNotUpdate
--noNeedUpdate
function system_module.system_ddns_get_status()

	local f = io.popen("cat /tmp/.ddns_status")

	if nil == f
	then
		debug("no status ")
		return "errorNull"
	end

	local res = f:read()

	if nil == res
	then
		debug("no status")
		return "errorNull"
	end

	io.close(f)
	return res
end
return system_module

