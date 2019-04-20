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

local TMP_NETWORK_TOOL_LOG_FILE="/tmp/log_tar/network_tool_log"
local TMP_TCPDUMP_FILE="/tmp/web_upload/tcpdump"
local TMP_PKG_DOWNLOAD_FILE="/tz_www/html/manage/network_tool_log.tcpdump"
local TMP_LOG_DOWNLOAD_FILE="/tz_www/html/manage/network_tool_log.tar"
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
			os.execute(string.format("softlimit -f %d %s -i %s -w %s &", LOG_FILE_LIMIT, TMP_TCPDUMP_FILE, tz_req["catchPackageIfname"],TMP_NETWORK_TOOL_LOG_FILE))
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
		os.execute("rm -rf /tmp/log_tar;mkdir /tmp/log_tar ; iptables -S > /tmp/log_tar/iptables;iptables -S -t nat >> /tmp/log_tar/iptables;")
		os.execute("iwconfig  > /tmp/log_tar/iwconifg;ifconfig -a > /tmp/log_tar/ifconfig;ip route > /tmp/log_tar/ip_route ;ip rule list > /tmp/log_tar/ip_rule")
		os.execute("ps > /tmp/log_tar/ps ;cat /proc/meminfo  > /tmp/log_tar/meminfo;dmesg > /tmp/log_tar/dmesg")
		os.execute("tar cvf /tmp/log.tar /tmp/log_tar/* ")
		os.execute(string.format("ln -sf %s %s ", "/tmp/log.tar", TMP_LOG_DOWNLOAD_FILE))
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
	
	os.execute(string.format("/etc/tozed/config_update %s 0  1>/dev/null 2>/dev/null",file_path))
	
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
	return os.execute("/etc/init.d/system restart")
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
		local index = string.find(timezone, ">")
		timezone = string.sub(timezone, index+1, string.len(timezone))
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
			x:set(SYSTEM_CONFIG_FILE,s[".name"], "timezone", "<-0"..timezone .. ">+"..timezone)
		else
			x:set(SYSTEM_CONFIG_FILE,s[".name"], "timezone", "<+0"..math.abs(timezone) .. ">"..timezone)
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

	return os.execute(string.format("date -s '%s' > /dev/null", date_string))
end

return system_module

