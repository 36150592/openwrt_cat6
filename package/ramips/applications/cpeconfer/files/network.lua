require("uci")
require("io")
network_module = {}
local util=require("tz.util")
local x = uci.cursor()
local NETWORK_CONFIG_FILE="network"
local NETWORK_DNS_MAIN_RESOLV_FILE="/etc/resolv.conf"
local NETWORK_DNS_SECOND_RESOLV_FILE="/tmp/resolv.conf.auto"
local debug = util.debug
local split = util.split 
local sleep = util.sleep
local tzlib = require("luatzlib")
network_module.net_info = {
		
		["mac"] = nil,
		["ipaddr"] = nil,
		["netmask"] = nil,
		["gateway"] = nil,
		["first_dns"] = nil,
		["second_dns"] = nil,
		["tx_package"] = nil,
		["rx_package"] = nil,
		["tx_byte"] = nil,
		["rx_byte"] = nil
}


function network_module.net_info:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
		
	self["mac"] = obj["mac"] or nil
	self["ipaddr"] = obj["ipaddr"] or nil
	self["netmask"] = obj["netmask"] or nil
	self["gateway"] = obj["gateway"] or nil
	self["first_dns"] = obj["first_dns"] or nil
	self["second_dns"] = obj["second_dns"] or nil
	self["tx_packets"] = obj["tx_package"] or nil
	self["rx_packets"] = obj["rx_package"] or nil
	self["tx_bytes"] = obj["tx_byte"] or nil
	self["rx_bytes"] = obj["rx_byte"] or nil
	
end

local function format_get_gateway_cmd(ifname) return string.format("ip route list table %s | grep default | awk '{print $3}'", ifname) end
local function format_get_mac(ifname) return string.format("cat /sys/class/net/%s/address", ifname) end

local function get_ip_bcast_mask_mac(ifname)

	local ip,bcast,mask,mac
	local tx_p,rx_p,tx_b,rx_b
	if nil == ifname
	then 
		debug("get ip fail: ifname nil")
		return nil,nil,nil
	end

	f = io.popen(string.format("ifconfig %s", ifname))
	local res = f:read()

	while nil ~= res
	do
		if string.match(res,"inet addr") ~= nil
		then
				local temp = string.gmatch(res,"%d+%.%d+%.%d+%.%d+")
				ip = temp()
				bcast = temp()
				mask = temp()
		elseif string.match(res,"HWaddr") ~= nil
		then
			local temp = string.gmatch(res,"%x%x:%x%x:%x%x:%x%x:%x%x:%x%x")
			mac = temp()
		elseif string.match(res,"RX packets:%d+") ~= nil
		then
			temp = string.match(res,"RX packets:%d+")
			arr = split(temp, ":")
			rx_p = arr[2]
		elseif string.match(res,"TX packets:%d+") ~= nil
		then
			temp = string.match(res,"TX packets:%d+")
			arr = split(temp, ":")
			tx_p = arr[2]
		elseif string.match(res,"RX bytes:%d+") ~= nil
		then
			temp = string.match(res,"RX bytes:%d+")
			arr = split(temp, ":")
			rx_b = arr[2]

			temp = string.match(res,"TX bytes:%d+")
			arr = split(temp, ":")
			tx_b = arr[2]
		end

		res = f:read()
	end
	io.close(f)

	if nil == ip or string.match(ip,"%d+%.%d+%.%d+%.%d+") == nil
	then
		debug("get ip error")
		return nil,nil,nil,nil,nil,nil,nil,nil
	end


	return ip,bcast,mask,mac,tx_p,rx_p,tx_b,rx_b
end

local function get_gateway(ifname)

	local gateway = nil
	if nil == ifname
	then 
		debug("get gateway fail")
		return nil
	end

	f = io.popen(format_get_gateway_cmd(ifname))
	gateway = f:read()
	io.close(f)


	if nil == gateway or string.match(gateway,"%d+%.%d+%.%d+%.%d+") == nil
	then
		debug("get gateway error")
		return nil
	end

	return gateway
end

local function get_mac(ifname)
	
	local mac = nil
	if nil == ifname
	then 
		debug("get mac fail,ifname nil")
		return nil
	end

	f = io.popen(format_get_mac(ifname))
	mac = f:read()
	io.close(f)

	if nil == mac or string.match(mac,"%x%x:%x%x:%x%x:%x%x:%x%x:%x%x") == nil
	then
		return nil
	end

	return mac

end

local function get_dns()

	local cmd = nil
	local f = nil
	local dns1,dns2
	cmd = string.format("cat %s | awk '{print $2}' | tail -n 2",NETWORK_DNS_MAIN_RESOLV_FILE)

    f = io.popen(cmd)

    dns1 = f:read()

    if nil == dns1 or string.match(dns1,"%d+%.%d+%.%d+%.%d+") == nil or "127.0.0.1" == dns1
    then
    	io.close(f)
    	cmd = string.format("cat %s | awk '{print $2}' | tail -n 2",NETWORK_DNS_SECOND_RESOLV_FILE)
    	f = io.popen(cmd)

    	dns1 = f:read()
    	if nil == dns1 or string.match(dns1,"%d+%.%d+%.%d+%.%d+") == nil or "127.0.0.1" == dns1
    	then 
			io.close(f)
    		debug("no dns found")
    		return nil,nil
    	end
    end

    dns2 = f:read()
    io.close(f)
    return dns1,dns2
end

-- get wan net info
-- input:none
-- return:
-- 		the struct of net_info , nil if no ip on the net interface
function network_module.network_get_wan_info()
	local info = network_module.net_info:new(nil,nil)

	local ifname = x:get(NETWORK_CONFIG_FILE, "wan", "ifname")

	local temp
	info["ipaddr"],temp,info["netmask"], info["mac"],info["tx_packets"],info["rx_packets"],info["tx_bytes"],info["rx_bytes"]= get_ip_bcast_mask_mac(ifname)
	info["gateway"] = get_gateway(ifname)
	info["first_dns"],info["second_dns"] = get_dns()

	if nil == info["ipaddr"]
	then
		debug("no ip was assign to the wan interface,return nil")
		return nil
	end

	return info
end

-- get 4g net info
-- input:none
-- return:
-- 		the struct of net_info , nil if no ip on the net interface
function network_module.network_get_4g_net_info()

	local info = network_module.net_info:new(nil,nil)

	local ifname = x:get(NETWORK_CONFIG_FILE, "4g", "ifname")

	local temp
	info["ipaddr"],temp,info["netmask"], info["mac"],info["tx_packets"],info["rx_packets"],info["tx_bytes"],info["rx_bytes"]= get_ip_bcast_mask_mac(ifname)
	info["gateway"] = get_gateway(ifname)
	info["first_dns"],info["second_dns"] = get_dns()
	if nil == info["ipaddr"]
	then
		debug("no ip was assign to the 4g interface,return nil")
		return nil
	end

	return info
end

-- get 4g1 net info
-- input:none
-- return:
-- 		the struct of net_info , nil if no ip on the net interface
function network_module.network_get_4g1_net_info()

	local info = network_module.net_info:new(nil,nil)

	local ifname = x:get(NETWORK_CONFIG_FILE, "4g1", "ifname")

	local temp
	info["ipaddr"],temp,info["netmask"], info["mac"],info["tx_packets"],info["rx_packets"],info["tx_bytes"],info["rx_bytes"]= get_ip_bcast_mask_mac(ifname)
	info["gateway"] = get_gateway(ifname)
	info["first_dns"],info["second_dns"] = get_dns()
	if nil == info["ipaddr"]
	then
		debug("no ip was assign to the 4g interface,return nil")
		return nil
	end

	return info
end

-- get 4g2 net info
-- input:none
-- return:
-- 		the struct of net_info , nil if no ip on the net interface
function network_module.network_get_4g2_net_info()

	local info = network_module.net_info:new(nil,nil)

	local ifname = x:get(NETWORK_CONFIG_FILE, "4g2", "ifname")

	local temp
	info["ipaddr"],temp,info["netmask"], info["mac"],info["tx_packets"],info["rx_packets"],info["tx_bytes"],info["rx_bytes"]= get_ip_bcast_mask_mac(ifname)
	info["gateway"] = get_gateway(ifname)
	info["first_dns"],info["second_dns"] = get_dns()
	if nil == info["ipaddr"]
	then
		debug("no ip was assign to the 4g interface,return nil")
		return nil
	end

	return info
end

return network_module