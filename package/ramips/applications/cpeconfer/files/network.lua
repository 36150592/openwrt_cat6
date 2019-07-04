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
		["interface"] = nil,
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
	self["interface"] = obj["interface"] or nil
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
				if nil == mask
				then
					mask = bcast
				end

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

local function get_dns(section)

	local cmd = nil
	local f = nil
	local dns1,dns2

	if nil == section
	then
		cmd = string.format("cat %s | awk '{print $2}' | tail -n 2",NETWORK_DNS_MAIN_RESOLV_FILE)

	    f = io.popen(cmd)

	    dns1 = f:read()
	    dns2 = f:read()
		if nil == dns1 or string.match(dns1,"%d+%.%d+%.%d+%.%d+") == nil or "127.0.0.1" == dns1
		then
			dns1 = nil
		end

		if nil == dns2 or string.match(dns2,"%d+%.%d+%.%d+%.%d+") == nil or "127.0.0.1" == dns2
		then
			dns2 = nil
		end

	    io.close(f)
	    return dns1,dns2
	end


	cmd = string.format("cat %s ",NETWORK_DNS_SECOND_RESOLV_FILE)
	f = io.popen(cmd)

	local res = f:read()

	while nil ~= res
	do
		if "# Interface "..section == res
		then
			res = f:read()
			if nil ~= res
			then
				local temp = string.gmatch(res,"%d+%.%d+%.%d+%.%d+")
				dns1 = temp()
			end

			res = f:read()
			if nil ~= res
			then
				temp = string.gmatch(res,"%d+%.%d+%.%d+%.%d+")
				dns2 = temp()
			end
			break
		end

		res=f:read()
	end

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
	info["first_dns"],info["second_dns"] = get_dns("wan")
	info["interface"] = ifname
	--[[if nil == info["ipaddr"]
	then
		debug("no ip was assign to the wan interface,return nil")
		return nil
	end]]--

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
	info["first_dns"],info["second_dns"] = get_dns("4g")
	info["interface"] = ifname
	--[[if nil == info["ipaddr"]
	then
		debug("no ip was assign to the 4g interface,return nil")
		return nil
	end]]--

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
	info["first_dns"],info["second_dns"] = get_dns("4g1")
	info["interface"] = ifname
	--[[if nil == info["ipaddr"]
	then
		debug("no ip was assign to the 4g interface,return nil")
		return nil
	end]]--

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
	info["first_dns"],info["second_dns"] = get_dns("4g2")
	info["interface"] = ifname
	--[[if nil == info["ipaddr"]
	then
		debug("no ip was assign to the 4g interface,return nil")
		return nil
	end]]--

	return info
end

-- get the Ethernet interface up down status
-- input:none
-- return:an array of the the status
-- 			element 1 respresend port 0 
--			element 2 respresend port 1 
--			element 3 respresend port 2
--			element 4 respresend port 4(wan interface) 
function network_module.network_get_interface_up_down_status()

	local f = io.popen("swconfig dev mt7530 show | grep port")
	local res = f:read()
	local ar = {}
	while nil ~= res
	do
		local index = 0
	
		if string.find(res,"link: port:0")	~= nil
		then
			index = 1
		elseif string.find(res,"link: port:1") ~= nil
		then 
			index = 2
		elseif string.find(res,"link: port:2") ~= nil
		then 
			index = 3
		elseif string.find(res,"link: port:4") ~= nil
		then 
			index = 4
		end

		if index > 0
		then
			if string.find(res,"link:up") ~= nil
			then
				ar[index] = 1
			else
				ar[index] = 0
			end
		end

		res = f:read()

	end

	io.close(f)
	return ar
end



-- set the wired lan interface mode ,restart the system to take effect
-- input:
--mode
--		lan-lan-lan-lan : all the eth ports are set to be lan 
--		lan-lan-lan-wan :  port4 is set to wan
--		lan-lan-iptv-lan : port3 is set to iptv
--      lan-lan-iptv-wan : port3 is set to iptv, port4 is set to wan

--iptv_use_apn_num
--0: main apn
--1: apn1
--2: apn2

-- output:
--		true if success , false if fail
function network_module.network_set_wired_lan_mode(mode,iptv_use_apn_num)
	if "lan-lan-lan-lan" ~= mode and "lan-lan-iptv-lan" ~= mode and "lan-lan-iptv-wan" ~= mode and "lan-lan-lan-wan" ~= mode 
	then
		debug("input mode error,it should be: lan-lan-lan-lan, lan-lan-lan-wan, lan-lan-iptv-lan, lan-lan-iptv-wan")
		return false
	end

	if iptv_use_apn_num == nil
	then
		iptv_use_apn_num=0
	end

	x:foreach(NETWORK_CONFIG_FILE, "switch_vlan", function(s)

		if "1" == s["vlan"]
		then
			if "lan-lan-lan-lan" == mode
			then
				x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","0 1 2 3 4 6t")
			elseif "lan-lan-lan-wan" == mode
			then
 				x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","0 1 2 3 6t")
 			elseif "lan-lan-iptv-lan" == mode
			then
				if iptv_use_apn_num == 0
				then
					x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","0 1 2 3 4 6t")
				else
					x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","0 1 4 6t")
				end
 			elseif "lan-lan-iptv-wan" == mode
			then
				if iptv_use_apn_num == 0
				then
					x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","0 1 2 3 6t")
				else
					x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","0 1 6t")
				end
 			end
 		elseif "2" == s["vlan"]
 		then
 			if "lan-lan-lan-lan" == mode
			then
				x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","6t")
			elseif "lan-lan-lan-wan" == mode
			then
 				x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","4 6t")
 			elseif "lan-lan-iptv-lan" == mode
			then
 				x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","6t")
 			elseif "lan-lan-iptv-wan" == mode
			then
 				x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","4 6t")
 			end
		elseif "3" == s["vlan"]
 		then
			if "lan-lan-lan-lan" == mode
			then
				x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","6t")
			elseif "lan-lan-lan-wan" == mode
			then
 				x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","6t")
 			elseif "lan-lan-iptv-lan" == mode
			then
				if iptv_use_apn_num == 1
				then
					x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","2 6t")
				else
					x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","6t")
				end
 			elseif "lan-lan-iptv-wan" == mode
			then
 				if iptv_use_apn_num == 1
				then
					x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","2 6t")
				else
					x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","6t")
				end
 			end
		elseif "4" == s["vlan"]
 		then
 			if "lan-lan-lan-lan" == mode
			then
				x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","6t")
			elseif "lan-lan-lan-wan" == mode
			then
 				x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","6t")
 			elseif "lan-lan-iptv-lan" == mode
			then
				if iptv_use_apn_num == 2
				then
					x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","2 6t")
				else
					x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","6t")
				end
 			elseif "lan-lan-iptv-wan" == mode
			then
 				if iptv_use_apn_num == 2
				then
					x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","2 6t")
				else
					x:set(NETWORK_CONFIG_FILE,s[".name"],"ports","6t")
				end
 			end
		end
	end)

	return x:commit(NETWORK_CONFIG_FILE)
end



-- set the lan/wan mode ,restart the system to take effect
-- input:string
--		lan:set to lan mode--> all the eth ports are set to be lan 
--		wan:set to wan mode--> port 4 set to wan  and the other set to lan
-- output:
--		true if success , false if fail
function network_module.network_set_lan_wan_mode(mode)
	if "lan" ~= mode and "wan" ~= mode
	then
		debug("input mode error,must be lan/wan")
		return false
	end

	local iptv_use_apn_num = 0

	local apn1_action = x:get("tozed", "system", "TZ_APN1_ACTION")
	local apn2_action = x:get("tozed", "system", "TZ_APN2_ACTION")

	if "3" == apn1_action
	then
		iptv_use_apn_num = 1
	elseif "3" == apn2_action
	then
		iptv_use_apn_num = 2
	end

	if mode == "lan"
	then
		if iptv_use_apn_num == 0
		then
			network_module.network_set_wired_lan_mode("lan-lan-lan-lan", 0)
		else
			network_module.network_set_wired_lan_mode("lan-lan-iptv-lan", iptv_use_apn_num)
		end
	else
		if iptv_use_apn_num == 0
		then
			network_module.network_set_wired_lan_mode("lan-lan-lan-wan", 0)
		else
			network_module.network_set_wired_lan_mode("lan-lan-iptv-wan", iptv_use_apn_num)
		end
	end

	return true

end

-- get the lan/wan mode ,restart the system to take effect
-- input:none
-- output:string
--		lan:set to lan mode--> all the eth ports are set to be lan 
--		wan:set to wan mode--> port 4 set to wan  and the other set to lan
function network_module.network_get_lan_wan_mode()

	local mode = "lan"
	x:foreach(NETWORK_CONFIG_FILE, "switch_vlan", function(s)

		if "2" == s["vlan"] and "6t" == s["ports"]
		then
			mode = "lan"
		elseif "2" == s["vlan"] and "4 6t" == s["ports"]
		then
			mode = "wan"
		end
	end)

	return mode
end

local function check_mac(mac)
	if string.match(mac,"%x%x:%x%x:%x%x:%x%x:%x%x:%x%x") == nil
	then
		return false
	end

	return true
end

-- set wan mac addr, restart system to take effect
-- input:string
--	 mac addr or nil to set the default mac in the factory section
-- return:
--		true if success , false if fail
function network_module.network_set_wan_mac(mac_addr)

	if nil == mac_addr or "" == mac_addr
	then
		local f = io.popen("eth_mac  r wan")
		if nil ~= f
		then
			local mac = f:read()
			if nil ~= mac
			then
				x:set(NETWORK_CONFIG_FILE,"wan","macaddr",mac)
				return x:commit(NETWORK_CONFIG_FILE)
			end
			io.close(f)
		end
	else
		if check_mac(mac_addr)
		then
			x:set(NETWORK_CONFIG_FILE,"wan","macaddr",mac_addr)
			return x:commit(NETWORK_CONFIG_FILE)
		end
	end

	return false
end

-- get wan mac addr
-- input:none
-- return:
--		mac addr
function network_module.network_get_wan_mac()
	return x:get(NETWORK_CONFIG_FILE,"wan","macaddr")
end


return network_module