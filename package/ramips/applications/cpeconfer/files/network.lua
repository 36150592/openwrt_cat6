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


local function format_get_ip_cmd(ifname) return string.format("ifconfig %s | grep 'inet addr' | cut -d':' -f 2 | cut -d' ' -f 1", ifname) end
local function format_get_bcast_cmd(ifname) return string.format("ifconfig %s | grep 'inet addr' | cut -d':' -f 3 | cut -d' ' -f 1", ifname) end
local function format_get_mask_cmd(ifname) return string.format("ifconfig %s | grep 'inet addr' | cut -d':' -f 4 | cut -d' ' -f 1", ifname) end
local function format_get_gateway_cmd(ifname) return string.format("route | grep %s  | awk '{print $2}' |  grep -E -o '([0-9]{1,3}[\.]){3}[0-9]{1,3}' ", ifname) end
local function format_get_mac(ifname) return string.format("cat /sys/class/net/%s/address", ifname) end
local function format_get_tx_packets(ifname) return string.format("cat /sys/class/net/%s/statistics/tx_packets", ifname) end
local function format_get_rx_packets(ifname) return string.format("cat /sys/class/net/%s/statistics/rx_packets", ifname) end
local function format_get_tx_bytes(ifname) return string.format("cat /sys/class/net/%s/statistics/tx_bytes", ifname) end
local function format_get_rx_bytes(ifname) return string.format("cat /sys/class/net/%s/statistics/rx_bytes", ifname) end

local function get_ip_bcast_mask(ifname)

	local ip,bcast,mask
	if nil == ifname
	then 
		debug("get ip fail: ifname nil")
		return nil,nil,nil
	end

	f = io.popen(format_get_ip_cmd(ifname))
	ip = f:read("*all")
	io.close(f)


	f = io.popen(format_get_bcast_cmd(ifname))
	bcast = f:read("*all")
	io.close(f)


	f = io.popen(format_get_mask_cmd(ifname))
	mask = f:read("*all")
	io.close(f)

	if nil == ip or string.match(ip,"%d+%.%d+%.%d+%.%d+") == nil
	then
		debug("get ip error")
		return nil,nil,nil
	end


	return ip,bcast,mask
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


local function get_statistics(ifname)
	local tx_p,rx_p,tx_b,rx_b
	local f

	f = io.popen(format_get_tx_packets(ifname))
	tx_p = f:read()
	io.close(f)


	f = io.popen(format_get_rx_packets(ifname))
	rx_p = f:read()
	io.close(f)

	f = io.popen(format_get_tx_bytes(ifname))
	tx_b = f:read()
	io.close(f)

	f = io.popen(format_get_rx_bytes(ifname))
	rx_b = f:read()
	io.close(f)

	return tx_p,rx_p,tx_b,rx_b
end

-- get wan net info
-- input:none
-- return:
-- 		the struct of net_info , nil if no ip on the net interface
function network_module.network_get_wan_info()
	local info = network_module.net_info:new(nil,nil)

	local ifname = x:get(NETWORK_CONFIG_FILE, "wan", "ifname")

	local temp
	info["ipaddr"],temp,info["netmask"] = get_ip_bcast_mask(ifname)
	info["gateway"] = get_gateway(ifname)
	info["mac"] = get_mac(ifname)
	info["first_dns"],info["second_dns"] = get_dns()
	info["tx_packets"],info["rx_packets"],info["tx_bytes"],info["rx_bytes"] = get_statistics(ifname)

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
	info["ipaddr"],temp,info["netmask"] = get_ip_bcast_mask(ifname)
	info["gateway"] = get_gateway(ifname)
	info["mac"] = get_mac(ifname)
	info["first_dns"],info["second_dns"] = get_dns()
	info["tx_packets"],info["rx_packets"],info["tx_bytes"],info["rx_bytes"] = get_statistics(ifname)

	if nil == info["ipaddr"]
	then
		debug("no ip was assign to the 4g interface,return nil")
		return nil
	end

	return info
end


return network_module