require("uci")
dhcp_module = {}
local util = require("tz.util")
local x = uci.cursor()
local DHCP_CONFIG_FILE="dhcp"
local DHCP_CONFIG_TYPE="dnsmasq"
local DHCP_STATIC_IP_SECTION="host"
local NETWORK_CONFIG_FILE="network"
local DNSMASQ_CONFIG_PATH="/var/etc/dnsmasq.conf"
local debug=util.debug
local split=util.split
local sleep=util.sleep



function dhcp_module.dhcp_start()
	return os.execute("/etc/init.d/dnsmasq start ")
end

function dhcp_module.dhcp_stop()
	return os.execute("/etc/init.d/dnsmasq stop ")
end

function dhcp_module.dhcp_restart()
	return os.execute("/etc/init.d/dnsmasq restart ")
end


-- enable dhcp server
function dhcp_module.dhcp_enable()
	return os.execute("/etc/init.d/dnsmasq enable")
end

-- disable dhcp server
function dhcp_module.dhcp_disable()
	return os.execute("/etc/init.d/dnsmasq disable ")
end

-- enable mutil dhcp config by name
function dhcp_module.dhcp_config_enable(name)
	if nil == name or  "" == name
	then
		debug("name is nil")
		return false
	end
	x:set(DHCP_CONFIG_FILE, name, "ignore", "0")
	return x:commit(DHCP_CONFIG_FILE)
end

-- disable mutil dhcp config by name
function dhcp_module.dhcp_config_disable(name)
	if nil == name or  "" == name
	then
		debug("name is nil")
		return false
	end
	x:set(DHCP_CONFIG_FILE, name, "ignore", "1")
	return x:commit(DHCP_CONFIG_FILE)
end

-- get the dhcp server if enable
-- input:none
-- return:
--		true if enable  false disable

function dhcp_module.dhcp_get_enable_status()
	local f = io.popen("ls /etc/rc.d/ | grep S*dnsmasq | wc -l")

	local temp = f:read()

	debug("dhcp server status = ", temp)

	if "1" == temp
	then
		io.close(f)
		return true
	end

	io.close(f)
	return false
end


function dhcp_module.dhcp_reload()
	os.execute("ubus call network reload ")
	os.execute("/etc/init.d/dnsmasq reload ")
	
end

local function get_section_name_by_type(s_type)
	local all_config = x:get_all(DHCP_CONFIG_FILE)

	if nil == all_config 
	then
		debug("all_config = nil")
		return nil
	end

	for k,v in pairs(all_config) do
		
			for key, value in pairs(v) do
				if ".type" == key and s_type == value
				then
					return v[".name"]
				end
			end 
	end


	return nil
end


dhcp_module.dhcp_object = {
	["name"]	 = 	nil,
	["interface"]	 = 	nil,
	["start_ip"]	 = 	nil,
	["end_ip"]	 = 	nil,
	["leasetime"]	 = 	nil,
	["server_ip"] = nil,
	["netmask"] = nil,
	["enable"] = nil, -- true or false
	["lan_mac"] = nil,
}


function dhcp_module.dhcp_object:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
	
	self["name"]	 = obj["name"] or nil
	self["interface"]	 = obj["interface"] or nil
	self["start_ip"]	 = 	obj["start_ip"] or nil
	self["end_ip"]	 = 	obj["end_ip"] or nil
	self["leasetime"]	 = 	obj["leasetime"] or nil
	self["server_ip"]	 = 	obj["server_ip"] or nil
	self["netmask"]	 = 	obj["netmask"] or nil
	self["enable"]	 = 	obj["enable"] or nil
	self["lan_mac"]	 = 	obj["lan_mac"] or nil
   return o
end

local function get_dhcp_config(name)
	if nil == name or  "" == name
	then
		debug("name is nil")
		return nil
	end
	local temp = dhcp_module.dhcp_object:new(nil,nil)


	temp["name"] = name
	temp["interface"] = x:get(DHCP_CONFIG_FILE, name, "interface")
	if nil == temp["interface"]
	then
		debug("cannot get this config")
		return nil
	end
	
	temp["server_ip"] = x:get(NETWORK_CONFIG_FILE, temp["interface"], "ipaddr")
	temp["netmask"] = x:get(NETWORK_CONFIG_FILE, temp["interface"], "netmask")
	temp["lan_mac"] = x:get(NETWORK_CONFIG_FILE, temp["interface"], "macaddr")
	temp["leasetime"] = x:get(DHCP_CONFIG_FILE, name, "leasetime")
	if nil == temp["leasetime"]
	then
		return nil
	end
	local l_array = split(temp["leasetime"], 'h')
	temp["leasetime"] = l_array[1]
	local ignore = x:get(DHCP_CONFIG_FILE, name, "ignore")
	temp["enable"] = ignore == "0" or ignore == nil or false
	if nil ~= temp["server_ip"] and nil ~= temp["netmask"] and nil ~= temp["name"]
	then
		temp["start"] = x:get(DHCP_CONFIG_FILE, name, "start")
		temp["limit"] = x:get(DHCP_CONFIG_FILE, name, "limit")
		local cmd = string.format("/bin/ipcalc.sh %s %s %s %d", temp["server_ip"], temp["netmask"], temp["start"], tonumber(temp["limit"])-1)
		local f = io.popen(cmd)
		local res = f:read()

		while nil ~= res
		do
			if nil ~= string.find(res,"START")
			then
				local ar = split(res, "=")
				temp["start_ip"] = ar[2]
			elseif nil ~= string.find(res,"END")
			then
				local ar = split(res, "=")
				temp["end_ip"] = ar[2]
			end

			res = f:read()
		end
		io.close(f)
		return temp 

	end

	return nil
end

-- get dchp config by dhcp interface 
-- input:network(string):the network of wifi config(the interface dhcp config)
-- return:the struct of dhcp_object or nil if fail
function dhcp_module.dhcp_get_object_by_network(network)

	local all_config = x:get_all(DHCP_CONFIG_FILE)

	if nil == all_config  or nil == network
	then
		debug("all_config = nil or network = nil")
	end

	local i = 1
	for k,v in pairs(all_config) do
		
		if "dhcp" == v[".type"] and v["interface"] ==  network
		then
			local temp = get_dhcp_config(v[".name"])
			return temp
		end
	end

	return nil
end

-- get all dhcp configs 
-- input:none
-- return:the array of dhcp_object
function dhcp_module.dhcp_get_object_list()

	local dhcp_list = {}
	local all_config = x:get_all(DHCP_CONFIG_FILE)

	if nil == all_config 
	then
		debug("all_config = nil")
	end

	local i = 1
	for k,v in pairs(all_config) do
		
		if "dhcp" == v[".type"]
		then
			local temp = get_dhcp_config(v[".name"])
			if nil ~= temp
			then
				dhcp_list[i] = temp
				i = i + 1
			end
			
		end
	end

	return dhcp_list
end


-- get server ip 
-- input:
--		name(string):the dhcp_object name
-- return:
--		string of server ip
function dhcp_module.dhcp_get_server_ip(name)
	debug("dhcp_get_server_ip")
	if nil == name 
	then
		debug("name is nil")
		return nil
	end
	
	debug("name = ", name)
	local interface = x:get(DHCP_CONFIG_FILE, name, "interface")

	if nil == interface 
	then
		debug("interface is nil")
		return nil
	end

	local ipaddr =  x:get(NETWORK_CONFIG_FILE,interface, "ipaddr")
	debug("ip addr = ", ipaddr)
	return ipaddr

end


-- set lan server ip
-- input:
--		name(string):the dhcp_object name
--		ip(string) :the server ip 
-- return:
--		boolean  true if success false if fail
function dhcp_module.dhcp_set_server_ip(name,ip)

	debug("dhcp_set_server_ip")
	local start,endp = string.find(ip,"%d+%.%d+%.%d+%.%d+")
	debug("start = ",start,"endp = ", endp)
	if nil == ip or start == nil
	then
		debug("input error: must be string like xx.xx.xx.xx")
		return false
	end
	
	if nil == name
	then
		debug("name is nil")
		return nil
	end

	local interface = x:get(DHCP_CONFIG_FILE, name, "interface")
	if nil == interface 
	then
		debug("interface is nil")
		return false
	end
	
	debug("interface = ", interface)

	local origin_ip = x:get(NETWORK_CONFIG_FILE,interface, "ipaddr")
	local dhcp_option = x:get(DHCP_CONFIG_FILE, name,"dhcp_option")

	for k,v in pairs(dhcp_option)
	do
		if 'option:dns-server,'..origin_ip == v
		then
			dhcp_option[k] = 'option:dns-server,'..ip
		end
	end
	x:set(DHCP_CONFIG_FILE, name, "dhcp_option", dhcp_option)
	x:commit(DHCP_CONFIG_FILE)


	if x:set(NETWORK_CONFIG_FILE,interface, "ipaddr",ip)
	then
		return x:commit(NETWORK_CONFIG_FILE)	
	end

	return false

end

-- get server netmask
-- input:
--		name(string):the dhcp_object name
-- return:
--		string of server netmask
function dhcp_module.dhcp_get_server_mask(name)
	debug("dhcp_get_server_mask")

	if nil == name
	then
		debug("name is nil")
		return nil
	end

	local interface = x:get(DHCP_CONFIG_FILE, name, "interface")

	if nil == interface 
	then
		debug("interface is nil")
		return nil
	end
	
	debug("domain = ", interface)
	local netmask =  x:get(NETWORK_CONFIG_FILE,interface, "netmask")
	debug("netmask = ", netmask)
	return netmask	
end

-- set lan server mask
-- input:
--		name(string):the dhcp_object name
--		ip_mask(string)  the netmask
-- return:
--		boolean  true if success false if fail
function dhcp_module.dhcp_set_server_mask(name, ip_mask)
	debug("dhcp_set_server_mask")
	local start,endp = string.find(ip_mask,"%d+%.%d+%.%d+%.%d+")
	debug("start = ",start,"endp = ", endp)
	if nil == ip_mask or start == nil
	then
		debug("input error: must be string like xx.xx.xx.xx")
		return false
	end
	
	if nil == name
	then
		debug("name is nil")
		return nil
	end

	local interface = x:get(DHCP_CONFIG_FILE, name, "interface")

	if nil == interface 
	then
		debug("interface is nil")
		return false
	end
	
	if x:set(NETWORK_CONFIG_FILE,interface, "netmask",ip_mask)
	then
		return x:commit(NETWORK_CONFIG_FILE)	
	end
	
	return false
end

-- get ip range
-- input:
--		name(string):the dhcp_object name
-- return:
--		two var are return: the start ip and the end ip  of string
function dhcp_module.dhcp_get_ip_range(name)
	debug("dhcp_get_ip_range")
	local temp = get_dhcp_config(name)
	if nil ~= temp
	then
		return temp["start_ip"],temp["end_ip"]
	end
	return nil,nil
end

-- set ip range
-- input:
--		name(string):the dhcp_object name
--		ip_start(string)  start ip address
--		limit_number(number)  the number of ip range
-- return:
--		boolean  true if success false if fail
function dhcp_module.dhcp_set_ip_range(name, ip_start, limit_number)
	debug("dhcp_set_ip_range")
	local start,endp = string.find(ip_start,"%d+%.%d+%.%d+%.%d+")
	debug("start = ",start,"endp = ", endp)
	if nil == ip_start or nil == start
	then
		debug("input error: ip_start must be string like xx.xx.xx.xx")
		return false
	end

	if type(limit_number) ~= "number"
	then
		debug("input error: limit_number not a number")
		return false
	end
	
	if nil == name 
	then
		debug("name is nil")
		return false
	end

	ip_start = string.gsub(ip_start, '%.', '#')
	debug(ip_start)
	local s_array = split(ip_start, '#')

	debug("s_array[4] = ", s_array[4])
	
		
	if  x:set(DHCP_CONFIG_FILE, name, "start", s_array[4]) and
		x:set(DHCP_CONFIG_FILE, name, "limit", limit_number)
	then
		return x:commit(DHCP_CONFIG_FILE)	
	end

	return false
end

-- get lease time
-- input:
--		name(string):the dhcp_object name
-- return:
-- 		the number of lease time in hour
function dhcp_module.dhcp_get_lease_time(name)
	debug("dhcp_get_lease_time")

	local leasetime = x:get(DHCP_CONFIG_FILE, name, "leasetime")
	if nil == leasetime
	then
		return nil
	end

	local l_array = split(leasetime, 'h')
	debug("leasetime = ", l_array[1])
	return tonumber(l_array[1])

end

-- set lease time
-- input:
--		name(string):the dhcp_object name
--		hour(number) 
-- return:
--		boolean  true if success false if fail
function dhcp_module.dhcp_set_lease_time(name, hour)
	debug("dhcp_set_lease_time")
	if type(hour) ~= "number"
	then
		debug("input error: must be number")
		return false
	end
	
	if nil == name 
	then
		debug("name is nil")
		return false
	end
	
	local temp_h = string.format("%dh", hour)
	if x:set(DHCP_CONFIG_FILE, name, "leasetime",temp_h)
	then
		return x:commit(DHCP_CONFIG_FILE)	
	end
	
	
	return false
end


dhcp_module.client = {
	["mac"]	 = 	nil,
	["name"]	 = 	nil,
	["ipaddr"]	 = 	nil,
	["connect_time"]	 = 	nil,
}


function dhcp_module.client:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
	
	self["mac"]	 = obj["mac"] or nil
	self["name"]	 = 	obj["name"] or nil
	self["ipaddr"]	 = 	obj["ipaddr"] or nil
	self["connect_time"]	 = 	obj["connect_time"] or nil
   return o
end

local Client = dhcp_module.client

-- get connect client info list
-- input:none
-- return:
--   a array of client
function dhcp_module.dhcp_get_client_list()
	debug("dhcp_get_client_list")
	local section_name = get_section_name_by_type(DHCP_CONFIG_TYPE)
	local lease_file = x:get(DHCP_CONFIG_FILE, section_name, 'leasefile')

	local f = io.open(lease_file)

	local res = f:read()
	local client_list = {}
	local i = 1
	while nil ~= res
	do

		local temp_array = split(res, ' ')
		local temp_client = Client:new(nil,nil)

		temp_client["connect_time"] = os.date("%Y-%m-%d %X",tonumber(temp_array[1]))
		temp_client["mac"] = temp_array[2]
		temp_client["ipaddr"] = temp_array[3]
		temp_client["name"] = temp_array[4]
		
		client_list[i] = temp_client
		i = i + 1
	
		res = f:read()
	end

    io.close(f)
	return client_list
end


dhcp_module.reserve_ip = {
	["ip"]	 = 	nil,
	["mac"]	 = 	nil,
	["leasetime"]	 = 	nil,
}


function dhcp_module.reserve_ip:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
	
	self["ip"]	 = obj["ip"] or nil
	self["mac"]	 = 	obj["mac"] or nil
	self["leasetime"]	 = 	obj["leasetime"] or nil
   return o
end

local function check_mac(mac)
	if string.match(mac,"%x%x:%x%x:%x%x:%x%x:%x%x:%x%x") == nil
	then
		return false
	end

	return true
end

local function check_ip(ip)

	local str = ip
	str = string.gsub(str,'%.','#')
	local ar = split(str,'#')

	if table.maxn(ar) ~= 4
	then
		return false
	end

	if tonumber(ar[1]) > 255 or tonumber(ar[2]) > 255 or tonumber(ar[3]) > 255 or tonumber(ar[4]) > 255
	then
		return false
	end

	if tonumber(ar[1]) < 0 or tonumber(ar[2]) < 0 or tonumber(ar[3]) < 0 or tonumber(ar[4]) < 0
	then
		return false
	end


	if string.match(ip,"%d+%.%d+%.%d+%.%d+") == nil
	then
		debug("check ip fail")
		return false
	end

	return true
end

function dhcp_module.dhcp_set_reserve_ip(reserve_list)

	
	x:foreach(DHCP_CONFIG_FILE,DHCP_STATIC_IP_SECTION,function(s)
		x:delete(DHCP_CONFIG_FILE,s[".name"])
	end)

	for k,v in pairs(reserve_list)
	do
		if check_ip(v["ip"]) and check_mac(v["mac"])
		then
			local section = x:add(DHCP_CONFIG_FILE,DHCP_STATIC_IP_SECTION)
			x:set(DHCP_CONFIG_FILE,section, "ip", v["ip"])
			x:set(DHCP_CONFIG_FILE,section, "mac", v["mac"])
			x:set(DHCP_CONFIG_FILE,section, "leasetime", v["leasetime"] or "")
		end
	end

	return x:commit(DHCP_CONFIG_FILE)
end


function dhcp_module.dhcp_get_reserve_ip()

	local list = {}
	local i = 1

	x:foreach(DHCP_CONFIG_FILE,DHCP_STATIC_IP_SECTION,function(s)

		local temp = dhcp_module.reserve_ip:new(nil,nil)

		temp["ip"] = s["ip"]
		temp["mac"] = s["mac"]
		--temp["leasetime"] = s["leasetime"]

		list[i] = temp
		i = i + 1
	end)


	return list
end
 
return dhcp_module
