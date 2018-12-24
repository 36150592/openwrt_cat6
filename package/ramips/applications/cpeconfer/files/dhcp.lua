require("uci")
dhcp_module = {}
local util = require("tz.util")
local x = uci.cursor()
local DHCP_CONFIG_FILE="dhcp"
local DHCP_CONFIG_TYPE="dnsmasq"
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

function dhcp_module.dhcp_enable()
	return os.execute("/etc/init.d/dnsmasq enable")
end

function dhcp_module.dhcp_disable()
	return os.execute("/etc/init.d/dnsmasq disable ")
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
		return true
	end

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

-- get server ip 
-- input:none
-- return:
--		string of server ip
function dhcp_module.dhcp_get_server_ip()
	debug("dhcp_get_server_ip")
	local section_name = get_section_name_by_type(DHCP_CONFIG_TYPE)
	if nil == section_name 
	then
		debug("section_name is nil")
		return nil
	end
	
	local domain = x:get(DHCP_CONFIG_FILE, section_name, "domain")
	debug("domain = ", domain)
	if nil ~= domain
	then
		local ifname =  x:get(DHCP_CONFIG_FILE,domain, "interface")
		local ipaddr =  x:get(NETWORK_CONFIG_FILE,ifname, "ipaddr")
		debug("ip addr = ", ipaddr)
		return ipaddr
	else
		debug("domain is nil")
		return nil
	end
end


-- set lan server ip
-- input:
--		ip(string)  the server ip 
-- return:
--		boolean  true if success false if fail
function dhcp_module.dhcp_set_server_ip(ip)

	debug("dhcp_set_server_ip")
	local start,endp = string.find(ip,"%d+%.%d+%.%d+%.%d+")
	debug("start = ",start,"endp = ", endp)
	if nil == ip or start == nil
	then
		debug("input error: must be string like xx.xx.xx.xx")
		return false
	end
	
	local section_name = get_section_name_by_type(DHCP_CONFIG_TYPE)
	if nil == section_name 
	then
		debug("section_name is nil")
		return false
	end
	
	local domain = x:get(DHCP_CONFIG_FILE, section_name, "domain")
	debug("domain = ", domain)
	if nil ~= domain
	then
		local ifname =  x:get(DHCP_CONFIG_FILE,domain, "interface")
		if x:set(NETWORK_CONFIG_FILE,ifname, "ipaddr",ip)
		then
			return x:commit(NETWORK_CONFIG_FILE)	
		end
	else
		debug("domain is nil")
	end
	
	return false

end

-- get server netmask
-- input:none
-- return:
--		string of server netmask
function dhcp_module.dhcp_get_server_mask()
	debug("dhcp_get_server_mask")
	local section_name = get_section_name_by_type(DHCP_CONFIG_TYPE)
	if nil == section_name 
	then
		debug("section_name is nil")
		return nil
	end
	
	local domain = x:get(DHCP_CONFIG_FILE, section_name, "domain")
	debug("domain = ", domain)
	if nil ~= domain
	then
		local ifname =  x:get(DHCP_CONFIG_FILE,domain, "interface")
		local ipaddr =  x:get(NETWORK_CONFIG_FILE,ifname, "netmask")
		debug("ip addr = ", ipaddr)
		return ipaddr
	else
		debug("domain is nil")
		return nil
	end
end

-- set lan server mask
-- input:
--		ip_mask(string)  the netmask
-- return:
--		boolean  true if success false if fail
function dhcp_module.dhcp_set_server_mask(ip_mask)
	debug("dhcp_set_server_mask")
	local start,endp = string.find(ip_mask,"%d+%.%d+%.%d+%.%d+")
	debug("start = ",start,"endp = ", endp)
	if nil == ip_mask or start == nil
	then
		debug("input error: must be string like xx.xx.xx.xx")
		return false
	end
	
	local section_name = get_section_name_by_type(DHCP_CONFIG_TYPE)
	if nil == section_name 
	then
		debug("section_name is nil")
		return false
	end
	
	local domain = x:get(DHCP_CONFIG_FILE, section_name, "domain")
	debug("domain = ", domain)
	if nil ~= domain
	then
		local ifname =  x:get(DHCP_CONFIG_FILE,domain, "interface")
		if x:set(NETWORK_CONFIG_FILE,ifname, "netmask",ip_mask)
		then
			return x:commit(NETWORK_CONFIG_FILE)	
		end
	else
		debug("domain is nil")
	end
	
	return false
end

-- get ip range
-- input:none
-- return:
--		two var are return: the start ip and the end ip  of string
function dhcp_module.dhcp_get_ip_range()
	debug("dhcp_get_ip_range")
	local cmd = string.format("cat %s | grep dhcp-range | cut -d',' -f 2",DNSMASQ_CONFIG_PATH)
	local f = io.popen(cmd)
	local start_ip = f:read()
	io.close(f)
	cmd = string.format("cat %s | grep dhcp-range | cut -d',' -f 3",DNSMASQ_CONFIG_PATH)
	f = io.popen(cmd)
	local endp_ip = f:read()
	io.close(f)

	debug("start_ip = ",start_ip)
	debug("endp_ip = ", endp_ip)

	if string.find(start_ip, "%d+%.%d+%.%d+%.%d+") == nil or
		string.find(endp_ip, "%d+%.%d+%.%d+%.%d+") == nil
	then
		debug("get wrong start_ip or endp_ip ")
		return nil
	end

	return start_ip,endp_ip
	
end

-- set ip range
-- input:
--		ip_start(string)  start ip address
--		limit_number(number)  the number of ip range
-- return:
--		boolean  true if success false if fail
function dhcp_module.dhcp_set_ip_range(ip_start, limit_number)
	debug("dhcp_set_ip_range")
	local start,endp = string.find(ip_start,"%d+%.%d+%.%d+%.%d+")
	debug("start = ",start,"endp = ", endp)
	if nil == ip_start or start == nil
	then
		debug("input error: ip_start must be string like xx.xx.xx.xx")
		return false
	end

	if type(limit_number) ~= "number"
	then
		debug("input error: limit_number not a number")
		return false
	end
	
	local section_name = get_section_name_by_type(DHCP_CONFIG_TYPE)
	if nil == section_name 
	then
		debug("section_name is nil")
		return false
	end

	ip_start = string.gsub(ip_start, '%.', '#')
	debug(ip_start)
	local s_array = split(ip_start, '#')

	debug("s_array[4] = ", s_array[4])
	local domain = x:get(DHCP_CONFIG_FILE, section_name, "domain")
	debug("domain = ", domain)
	if nil ~= domain
	then
		
		if  x:set(DHCP_CONFIG_FILE,domain, "start", s_array[4]) and
			x:set(DHCP_CONFIG_FILE,domain, "limit", limit_number)
		then
			return x:commit(DHCP_CONFIG_FILE)	
		end
	else
		debug("domain is nil")
	end
	
	return false
end

-- get lease time
-- input:none
-- return:
-- 		the number of lease time in hour
function dhcp_module.dhcp_get_lease_time()
	debug("dhcp_get_lease_time")
	local cmd = string.format("cat %s | grep dhcp-range | cut -d',' -f 5",DNSMASQ_CONFIG_PATH)
	local f = io.popen(cmd)
	local leasetime = f:read()
	io.close(f)
	
	local l_array = split(leasetime, 'h')
	debug("leasetime = ", l_array[1])
	return tonumber(l_array[1])

end

-- set lease time
-- input:
--		hour(number) 
-- return:
--		boolean  true if success false if fail
function dhcp_module.dhcp_set_lease_time(hour)
	debug("dhcp_set_lease_time")
	if type(hour) ~= "number"
	then
		debug("input error: must be number")
		return false
	end
	
	local section_name = get_section_name_by_type(DHCP_CONFIG_TYPE)
	if nil == section_name 
	then
		debug("section_name is nil")
		return false
	end
	
	local domain = x:get(DHCP_CONFIG_FILE, section_name, "domain")
	debug("domain = ", domain)
	if nil ~= domain
	then
		local temp_h = string.format("%dh", hour)
		if x:set(DHCP_CONFIG_FILE,domain, "leasetime",temp_h)
		then
			return x:commit(DHCP_CONFIG_FILE)	
		end
	else
		debug("domain is nil")
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
	self["name"]	 = 	obj["mac"] or nil
	self["ipaddr"]	 = 	obj["mac"] or nil
	self["connect_time"]	 = 	obj["mac"] or nil
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
	
	return client_list
end


 
return dhcp_module
