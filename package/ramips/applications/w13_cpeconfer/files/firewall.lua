require("uci")
require("io")
firewall_module = {}
local util=require("tz.util")
local x = uci.cursor()
local FIREWALL_CONFIG_FILE="firewall"
local NETWORK_CONFIG_FILE="network"
local NETWORK_UCI_INTERFACE="interface"
local FIREWALL_CUSTOM_CONFIG_FILE="/etc/firewall.user"
local TOZED_CONFIG_FILE="tozed"
local TOZED_UCI_SECTION="cfg"
local TOZED_REMOTE_CONTROL_SECTION="remote_control"
local TOZED_UCI_OPTION_STATIC_ROUTE="TZ_STATIC_ROUTE"

local FIREWALL_MAC_FILTER_PREX="MAC-FILTER"
local FIREWALL_URL_FILTER_PREX="URL-FILTER"
local FIREWALL_IP_FILTER_PREX="IP-FILTER"
local FIREWALL_PORT_FILTER_PREX="PORT-FILTER"
local FIREWALL_ACL_FILTER_PREX="ACL-FILTER"
local FIREWALL_IP_MAC_BIND_FILTER_PREX="IP-MAC-BIND-FILTER"
local FIREWALL_SPEED_LIMIT_FILTER_PREX="SPEED-LIMIT-FILTER"
local FIREWALL_PORT_REDIRECT_PREX="PORT-REDIRECT"
local FIREWALL_UCI_ZONE="zone"
local debug = util.debug
local split = util.split 
local sleep = util.sleep

firewall_module.mac_filter = {
		
	["mac"] = nil,
	["action"] = nil,--DROP:refused  ACCEPT: accept connect 
	["comment"] = nil, --user note
	["iswork"] = nil  -- is apply this rule  true:apply  false:not apply
}

function firewall_module.mac_filter:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
		
	self["mac"] = obj["mac"] or nil
	self["action"] = obj["action"] or nil 
	self["comment"] = obj["comment"] or nil
	self["iswork"] = obj["iswork"] or nil
end

firewall_module.speed_filter = {
		
	["ipaddr"] = nil,
	["speed"] = nil, -- number  in KB/s
	["comment"] = nil, --user note
	["iswork"] = nil  -- is apply this rule  true:apply  false:not apply
}

function firewall_module.speed_filter:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
		
	self["ipaddr"] = obj["ipaddr"] or nil
	self["speed"] = obj["speed"] or nil 
	self["comment"] = obj["comment"] or nil
	self["iswork"] = obj["iswork"] or nil
end


firewall_module.ipmac_bind_filter = {
		
	["mac"] = nil,
	["ipaddr"] = nil,
	["comment"] = nil, --user note
	["iswork"] = nil  -- is apply this rule  true:apply  false:not apply
}

function firewall_module.ipmac_bind_filter:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
		
	self["mac"] = obj["mac"] or nil
	self["ipaddr"] = obj["ipaddr"] or nil 
	self["comment"] = obj["comment"] or nil
	self["iswork"] = obj["iswork"] or nil
end

firewall_module.url_filter = {
		
	["url"] = nil,
	["action"] = nil, --DROP:refused  ACCEPT: accept connect 
	["comment"] = nil, -- user note
	["interface"] = nil, -- interface like lan lan1 or lan2  , nil if set to all interface
	["iswork"] = nil  -- is apply this rule  true:apply  false:not apply
}

function firewall_module.url_filter:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
		
	self["url"] = obj["url"] or nil
	self["action"] = obj["action"] or nil
	self["comment"] = obj["comment"] or nil
	self["iswork"] = obj["iswork"] or nil
	
end

firewall_module.ip_filter = {
		
	["ipaddr"] = nil,
	["action"] = nil,--DROP:refused  ACCEPT: access connect 
	["protocol"] = nil, --tcp  upd  all
	["comment"] = nil, -- user note
	["iswork"] = nil  -- is apply this rule  true:apply  false:not apply
}

function firewall_module.ip_filter:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
		
	self["ipaddr"] = obj["ipaddr"] or nil
	self["action"] = obj["action"] or nil
	self["protocol"] = obj["protocol"] or nil
	self["comment"] = obj["comment"] or nil
	self["iswork"] = obj["iswork"] or nil
	
end

firewall_module.acl_filter = {
		
	["src_ipaddr"] = nil,
	["dest_ipaddr"] = nil,
	["action"] = nil,--DROP:refused  ACCEPT: access connect 
	["protocol"] = nil, --tcp  upd  all
	["comment"] = nil, -- user note
	["iswork"] = nil  -- is apply this rule  true:apply  false:not apply
}

function firewall_module.acl_filter:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
		
	self["src_ipaddr"] = obj["src_ipaddr"] or nil
	self["dest_ipaddr"] = obj["dest_ipaddr"] or nil
	self["action"] = obj["action"] or nil
	self["protocol"] = obj["protocol"] or nil
	self["comment"] = obj["comment"] or nil
	self["iswork"] = obj["iswork"] or nil
	
end


firewall_module.port_filter = {
	
	["port"] = nil,
	["protocol"] = nil, --tcp  upd  all
	["action"] = nil, --DROP:refused  ACCEPT: accept connect 
	["comment"] = nil, --user note
	["iswork"] = nil  -- is apply this rule  true:apply  false:not apply
}

function firewall_module.port_filter:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
		
	self["port"] = obj["port"] or nil
	self["protocol"] = obj["protocol"] or nil
	self["action"] = obj["action"] or nil
	self["comment"] = obj["comment"] or nil
	self["iswork"] = obj["iswork"] or nil
	
end

firewall_module.port_redirect = {
	["protocol"] = nil, --tcp  upd  all
	["dest_addr"] = nil,--origin dest addr of ip  ,this can be nil if redirect all ip  
	["dest_port"] = nil,--origin dest port 
	["dest_port_end"] = nil, --the range of dest_port , this can be nil if dest_port is not a range
	["redirect_addr"] = nil,-- redirect ip address 
	["redirect_port"]  = nil,-- redirect port
	["comment"] = nil, --user note
	["iswork"] = nil  -- is apply this rule  true:apply  false:not apply

}

function firewall_module.port_redirect:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
		
	self["dest_port"] = obj["dest_port"] or nil
	self["protocol"] = obj["protocol"] or nil
	self["dest_addr"] = obj["dest_addr"] or nil
	self["redirect_addr"] = obj["redirect_addr"] or nil
	self["redirect_port"] = obj["redirect_port"] or nil
	self["comment"] = obj["comment"] or nil
	self["iswork"] = obj["iswork"] or nil
	
end

local function reserve_array(array)

	local new_array = {}

	for i = 1,table.maxn(array)
	do
		new_array[table.maxn(array) - i + 1] = array[i]
	end

	return new_array
end


local function format_delete_item_cmd(item_str)
	return string.format("sed -i /%s/d %s", item_str, FIREWALL_CUSTOM_CONFIG_FILE)
end

local function format_set_mac_filter_cmd(mac,action,comment) 
	return string.format("echo 'iptables -I FORWARD -m mac --mac-source %s -j %s ##%s##%s##%s##%s' >> %s", mac, action, FIREWALL_MAC_FILTER_PREX, mac, action, comment, FIREWALL_CUSTOM_CONFIG_FILE) 
end

local function format_get_mac_filter_cmd() 
	return string.format("cat %s | grep %s", FIREWALL_CUSTOM_CONFIG_FILE, FIREWALL_MAC_FILTER_PREX) 
end

local function format_set_speed_filter_cmd(ip ,speed, comment)
		
	local target_speed = math.ceil(speed/100) 
	if target_speed < 1
	then
		target_speed = 1
	end

	local cmd1 = string.format("echo 'iptables -I FORWARD -s %s -j DROP  ##%s##%s##%s##%s##1'  >> %s;",
									ip, FIREWALL_SPEED_LIMIT_FILTER_PREX, ip, speed, comment,FIREWALL_CUSTOM_CONFIG_FILE)

	local cmd2 = string.format("echo 'iptables -I FORWARD -m limit -s %s --limit %f/s --limit-burst %d -j ACCEPT  ##%s##%s##%s##%s##2'  >> %s;",
									ip,target_speed,target_speed, FIREWALL_SPEED_LIMIT_FILTER_PREX, ip, speed, comment,FIREWALL_CUSTOM_CONFIG_FILE)

	local cmd3 = string.format("echo 'iptables -I FORWARD -d %s -j DROP  ##%s##%s##%s##%s##3'  >> %s;",
									ip, FIREWALL_SPEED_LIMIT_FILTER_PREX, ip, speed, comment,FIREWALL_CUSTOM_CONFIG_FILE)

	local cmd4 = string.format("echo 'iptables -I FORWARD -m limit -d %s --limit %f/s --limit-burst %d -j ACCEPT  ##%s##%s##%s##%s##4'  >> %s;",
									ip,target_speed,target_speed, FIREWALL_SPEED_LIMIT_FILTER_PREX, ip, speed, comment,FIREWALL_CUSTOM_CONFIG_FILE)


	return string.format("%s%s%s%s", cmd1,cmd2,cmd3,cmd4)
end

local function format_get_speed_filter_cmd() 
	return string.format("cat %s | grep %s", FIREWALL_CUSTOM_CONFIG_FILE, FIREWALL_SPEED_LIMIT_FILTER_PREX) 
end


local function format_set_ipmac_bind_filter_cmd(protocol, ip ,mac, comment)
	
	local cmd1 = string.format("echo 'iptables -I FORWARD  -p %s -m mac --mac-source %s -j DROP ##%s##%s##%s##%s##1'  >> %s;",
									protocol, mac, FIREWALL_IP_MAC_BIND_FILTER_PREX, ip, mac, comment,FIREWALL_CUSTOM_CONFIG_FILE)

	local cmd2 = string.format("echo 'iptables -I FORWARD  -p %s -s %s -j DROP ##%s##%s##%s##%s##2'  >> %s;",
									protocol, ip, FIREWALL_IP_MAC_BIND_FILTER_PREX, ip, mac, comment, FIREWALL_CUSTOM_CONFIG_FILE)

	local cmd3 = string.format("echo 'iptables -I FORWARD -p %s -s %s -m mac --mac-source %s -j ACCEPT ##%s##%s##%s##%s##3'  >> %s;",
									protocol, ip, mac, FIREWALL_IP_MAC_BIND_FILTER_PREX, ip, mac, comment,FIREWALL_CUSTOM_CONFIG_FILE)

	return string.format("%s%s%s", cmd1,cmd2,cmd3)
end

local function format_get_ipmac_bind_filter_cmd() 
	return string.format("cat %s | grep %s", FIREWALL_CUSTOM_CONFIG_FILE, FIREWALL_IP_MAC_BIND_FILTER_PREX) 
end

local function format_set_url_filter_cmd(url,action,comment, interface)
	if nil == interface  or "" == interface
	then
		return string.format("echo 'iptables -I FORWARD -m string --string %s --algo bm -j %s ##%s##%s##%s##%s' >> %s", url, action, FIREWALL_URL_FILTER_PREX, url, action, comment, FIREWALL_CUSTOM_CONFIG_FILE)
	else
		return string.format("echo 'iptables -I FORWARD -i br-%s -m string --string %s --algo bm -j %s ##%s##%s##%s##%s##%s' >> %s",interface, url, action, FIREWALL_URL_FILTER_PREX, url, action, comment, interface, FIREWALL_CUSTOM_CONFIG_FILE)		
	end
end

local function format_get_url_filter_cmd() 
	return string.format("cat %s | grep %s", FIREWALL_CUSTOM_CONFIG_FILE, FIREWALL_URL_FILTER_PREX) 
end

local function format_set_ip_filter_cmd(ip,protocol,action,comment)

	local cmd1 = string.format("echo 'iptables  -I FORWARD -p %s -s %s  -j %s ##%s##%s##%s##%s##%s##1' >> %s ;",
								 protocol,ip, action, FIREWALL_IP_FILTER_PREX, protocol, ip, action, comment, FIREWALL_CUSTOM_CONFIG_FILE)

	local cmd2 = string.format("echo 'iptables  -I FORWARD -p %s -d %s  -j %s ##%s##%s##%s##%s##%s##2' >> %s ;",
								  protocol,ip, action, FIREWALL_IP_FILTER_PREX,  protocol,ip, action, comment, FIREWALL_CUSTOM_CONFIG_FILE)
	return string.format("%s%s",cmd1,cmd2)
end

local function format_get_ip_filter_cmd() 
	return string.format("cat %s | grep %s", FIREWALL_CUSTOM_CONFIG_FILE, FIREWALL_IP_FILTER_PREX) 
end


local function format_set_acl_filter_cmd(ip_src,ip_dest,protocol,action,comment)

	local cmd = string.format("echo 'iptables  -I FORWARD -p %s -s %s -d %s -j %s ##%s##%s##%s##%s##%s##%s##1' >> %s ;",
								 protocol,ip_src,ip_dest, action, FIREWALL_ACL_FILTER_PREX, protocol, ip_src,ip_dest, action, comment, FIREWALL_CUSTOM_CONFIG_FILE)

	return cmd
end

local function format_get_acl_filter_cmd() 
	return string.format("cat %s | grep %s", FIREWALL_CUSTOM_CONFIG_FILE, FIREWALL_ACL_FILTER_PREX)
end


local function format_set_port_filter_cmd(port,protocol,action,comment)
	if "all" == protocol
	then
		local cmd1 = string.format("echo 'iptables -I FORWARD -p tcp -m state --state NEW,ESTABLISHED,RELATED -m tcp --dport %s -j %s ##%s##%s##%s##%s##%s##1' >> %s ;",
								 port, action, FIREWALL_PORT_FILTER_PREX, port,protocol, action, comment, FIREWALL_CUSTOM_CONFIG_FILE)

		local cmd2 = string.format("echo 'iptables -I FORWARD -p udp -m state --state NEW,ESTABLISHED,RELATED -m udp --dport %s -j %s ##%s##%s##%s##%s##%s##2' >> %s ;",
								 port, action, FIREWALL_PORT_FILTER_PREX, port,protocol, action, comment, FIREWALL_CUSTOM_CONFIG_FILE)
		return string.format("%s%s",cmd1,cmd2)
	else
		return string.format("echo 'iptables -I FORWARD -p %s -m state --state NEW,ESTABLISHED,RELATED -m %s --dport %s -j %s ##%s##%s##%s##%s##%s##1' >> %s ;",
								 protocol, protocol, port, action, FIREWALL_PORT_FILTER_PREX, port,protocol, action, comment, FIREWALL_CUSTOM_CONFIG_FILE)
	end
end

local function format_get_port_filter_cmd() 
	return string.format("cat %s | grep %s", FIREWALL_CUSTOM_CONFIG_FILE, FIREWALL_PORT_FILTER_PREX) 
end


local function format_set_port_redirect_cmd(protocol,dest_addr,dest_port,dest_port_end,redirect_addr,redirect_port,comment)
	if "all" == protocol
	then
		local cmd1 ,cmd2
		if nil == dest_addr
		then 
		cmd1 = string.format("echo 'iptables -t nat -I PREROUTING  -p tcp --dport %s:%s -j DNAT --to %s:%s ##%s##%s##%s##%s##%s##%s##%s##%s##1' >> %s ;",
								 dest_port, dest_port_end, redirect_addr, redirect_port, FIREWALL_PORT_REDIRECT_PREX, protocol, "*", dest_port,redirect_addr, redirect_port, comment, dest_port_end, FIREWALL_CUSTOM_CONFIG_FILE)

		cmd2 = string.format("echo 'iptables -t nat -I PREROUTING  -p udp --dport %s:%s -j DNAT --to %s:%s ##%s##%s##%s##%s##%s##%s##%s##%s##2' >> %s ;",
								 dest_port, dest_port_end, redirect_addr, redirect_port, FIREWALL_PORT_REDIRECT_PREX, protocol, "*", dest_port,redirect_addr, redirect_port, comment, dest_port_end, FIREWALL_CUSTOM_CONFIG_FILE)
		else
		cmd1 = string.format("echo 'iptables -t nat -I PREROUTING  -p tcp -d %s --dport %s:%s -j DNAT --to %s:%s ##%s##%s##%s##%s##%s##%s##%s##%s##1' >> %s ;",
								 dest_addr, dest_port, dest_port_end, redirect_addr, redirect_port, FIREWALL_PORT_REDIRECT_PREX, protocol, dest_addr, dest_port,redirect_addr, redirect_port, comment, dest_port_end, FIREWALL_CUSTOM_CONFIG_FILE)

		cmd2 = string.format("echo 'iptables -t nat -I PREROUTING  -p udp -d %s --dport %s:%s -j DNAT --to %s:%s ##%s##%s##%s##%s##%s##%s##%s##%s##2' >> %s ;",
								 dest_addr, dest_port, dest_port_end, redirect_addr, redirect_port, FIREWALL_PORT_REDIRECT_PREX, protocol, dest_addr, dest_port,redirect_addr, redirect_port, comment, dest_port_end, FIREWALL_CUSTOM_CONFIG_FILE)
		
		end


		return string.format("%s%s",cmd1,cmd2)
	else
		if nil == dest_addr
		then
			return string.format("echo 'iptables -t nat -I PREROUTING  -p %s --dport %s:%s -j DNAT --to %s:%s ##%s##%s##%s##%s##%s##%s##%s##%s##1' >> %s ;",
								 protocol, dest_port, dest_port_end, redirect_addr, redirect_port, FIREWALL_PORT_REDIRECT_PREX, protocol, "*", dest_port,redirect_addr, redirect_port, comment, dest_port_end,FIREWALL_CUSTOM_CONFIG_FILE)

		else

		return string.format("echo 'iptables -t nat -I PREROUTING  -p %s -d %s --dport %s:%s -j DNAT --to %s:%s ##%s##%s##%s##%s##%s##%s##%s##%s##1' >> %s ;",
								 protocol, dest_addr, dest_port, dest_port_end, redirect_addr, redirect_port, FIREWALL_PORT_REDIRECT_PREX, protocol, dest_addr, dest_port,redirect_addr, redirect_port, comment, dest_port_end,FIREWALL_CUSTOM_CONFIG_FILE)


		end

	end

end

local function format_get_port_redirect_cmd()
	return string.format("cat %s | grep %s", FIREWALL_CUSTOM_CONFIG_FILE, FIREWALL_PORT_REDIRECT_PREX)
end




local function execute_cmd(...)
	
	local temp = nil
	local cmd = nil
	if false == arg[2]
	then
		cmd = string.gsub(arg[1], 'echo \'','echo \'#')
	else
		cmd = arg[1]
	end


	local f = io.popen(cmd)

	if nil == f
	then
		debug("execute cmd ", cmd, "fail")
		return nil
	end

	temp = f:read()


	if nil == temp
	then
		io.close(f)
		return "success"
	end

	local data = {}
	local i = 1
	while nil ~= temp
	do
		data[i] = temp
		temp = f:read()
		i = i+1
	end

	io.close(f)
	return data
end

local function check_mac(mac)
	if string.match(mac,"%x%x:%x%x:%x%x:%x%x:%x%x:%x%x") == nil
	then
		return false
	end

	return true
end

local function check_ip(ip)

	if string.match(ip,"%d+%.%d+%.%d+%.%d+") == nil
	then
		debug("check ip fail")
		return false
	end
	debug("check_ip true")
	return true
end

local function check_action(action)

	if "ACCEPT" == action or "DROP" == action or "REJECT" == action
	then
		return true
	end

	debug("check action fail")
	return false
end

local function check_protocol(protocol)

	if "tcp" == protocol or "udp" == protocol or "all" == protocol
	then
		return true
	end

	debug("check protocol fail")
	return false
end

local function rule_is_work(str)
	local ret = string.find(str,"#iptables")

	if nil ~= ret
	then
		return false
	end

	return true

end

function firewall_module.firewall_clear_all_user_rule()
	return execute_cmd(string.format("> %s", FIREWALL_CUSTOM_CONFIG_FILE))
end

function firewall_module.firewall_restart()
	return execute_cmd("/etc/init.d/firewall restart")
end

function firewall_module.firewall_start()
	return execute_cmd("/etc/init.d/firewall start")
end

function firewall_module.firewall_stop()
	return execute_cmd("/etc/init.d/firewall stop")
end

-- get mac filter list
-- input:none
-- return:the array of struct mac_filter
function firewall_module.firewall_get_mac_filter_list()

	local data = execute_cmd(format_get_mac_filter_cmd())
	local mac_filter_list = {}
	if nil == data
	then
		debug("execute get  mac filter cmd fail")
		return nil
	elseif "success" == data
	then
		debug("the mac filter list is empty")
		return nil
	end



	for i = 1, table.maxn(data)
	do
		debug(data[i])
		local array = split(data[i], "##")
		local temp = firewall_module.mac_filter:new(nil,nil)
		temp["mac"] = array[3]
		temp["action"] = array[4]
		temp["comment"] = array[5]
		temp["iswork"] = rule_is_work(data[i])
		mac_filter_list[i] = temp

	end

	return reserve_array(mac_filter_list)
end

-- set mac filter
-- input:the array of struct mac_filter
-- 		input example
--[[
local macs={
		{["mac"]='aa:bb:cc:11:22:33', ["action"]='DROP', ["comment"]="test mac filter"},
		{["mac"]='aa:bb:cc:11:22:34', ["action"]='DROP', ["comment"]="test mac filter"}

		}
]]--
-- return true if success false if fail
function firewall_module.firewall_set_mac_filter_list(filter_list)

	if type(filter_list) ~= 'table' 
	then
		debug("filter list is empty or not a table")
		return nil
	end

	local array = reserve_array(filter_list)


	if nil == execute_cmd(format_delete_item_cmd(FIREWALL_MAC_FILTER_PREX))
	then
		debug("execute clear cmd fail")
		return false
	end

	if table.maxn(filter_list) <= 0
	then
		return true
	end

	for i=1,table.maxn(array)
	do
		local temp = array[i]

		if check_action(temp["action"]) and check_mac(temp["mac"])
		then
			execute_cmd(format_set_mac_filter_cmd(temp["mac"], temp["action"], temp["comment"]), temp["iswork"])
		else
			debug("check fail ", temp["action"]," ", temp["mac"], " ", temp["comment"])
			return false
		end
	end

	
	return true
end

-- get speed limit filter list
-- input:none
-- return:the array of struct speed_filter
function firewall_module.firewall_get_speed_filter_list()

	local data = execute_cmd(format_get_speed_filter_cmd())
	local speed_filter_list = {}
	local j = 1
	if nil == data
	then
		debug("execute get  mac filter cmd fail")
		return nil
	elseif "success" == data
	then
		debug("the mac filter list is empty")
		return nil
	end

	for i = 1, table.maxn(data)
	do
		
		local array = split(data[i], "##")
		local temp = firewall_module.speed_filter:new(nil,nil)
		temp["ip"] = array[3]
		temp["speed"] = array[4]
		temp["comment"] = array[5]
		temp["iswork"] = rule_is_work(data[i])
		if "1" == array[6]
		then
			speed_filter_list[j] = temp
			j = j+1
		end
		

	end

	return reserve_array(speed_filter_list)
end

-- set speed filter list
-- input:the array of struct speed_filter
-- 		input example
--[[
local macs={
		{["speed"]=100, ["ipaddr"]='192.168.2.1', ["comment"]="test speed filter", ["iswork"]=true},
		{["speed"]=2000, ["ipaddr"]='192.168.2.2', ["comment"]="test speed filter",["iswork"]=false}

		}
]]--
-- return true if success false if fail
function firewall_module.firewall_set_speed_filter_list(filter_list)

	if type(filter_list) ~= 'table' 
	then
		debug("filter list is empty or not a table")
		return nil
	end

	local array = reserve_array(filter_list)


	if nil == execute_cmd(format_delete_item_cmd(FIREWALL_SPEED_LIMIT_FILTER_PREX))
	then
		debug("execute clear cmd fail")
		return false
	end

	if table.maxn(filter_list) <= 0
	then
		return true
	end

	for i=1,table.maxn(array)
	do
		local temp = array[i]
		if check_ip(temp["ipaddr"]) and type(temp["speed"]) == "number" and temp["speed"] > 0 
		then
			execute_cmd(format_set_speed_filter_cmd(temp["ipaddr"], temp["speed"], temp["comment"]), temp["iswork"])
		else
			debug("check fail ", temp["ipaddr"]," ", temp["speed"], " ", temp["comment"])
			return false
		end
	end

	return true
end



-- get ip mac bind filter list
-- input:none
-- return:the array of struct ip_mac_bind_filter
function firewall_module.firewall_get_ipmac_bind_filter_list()

	local data = execute_cmd(format_get_ipmac_bind_filter_cmd())
	local ipmac_filter_list = {}
	local j = 1
	if nil == data
	then
		debug("execute get  mac filter cmd fail")
		return nil
	elseif "success" == data
	then
		debug("the mac filter list is empty")
		return nil
	end



	for i = 1, table.maxn(data)
	do
		
		local array = split(data[i], "##")
		local temp = firewall_module.ipmac_bind_filter:new(nil,nil)
		temp["ip"] = array[3]
		temp["mac"] = array[4]
		temp["comment"] = array[5]
		temp["iswork"] = rule_is_work(data[i])
		if "1" == array[6]
		then
			ipmac_filter_list[j] = temp
			j = j+1
		end
		

	end

	return reserve_array(ipmac_filter_list)
end

-- set ip mac bind filter list
-- input:the array of struct ip_mac_bind_filter
-- 		input example
--[[
local macs={
		{["mac"]='aa:bb:cc:11:22:33', ["ipaddr"]='192.168.2.1', ["comment"]="test ip mac bind filter", ["iswork"]=true},
		{["mac"]='aa:bb:cc:11:22:34', ["ipaddr"]='192.168.2.2', ["comment"]="test ip mac bind filter",["iswork"]=false}

		}
]]--
-- return true if success false if fail
function firewall_module.firewall_set_ipmac_bind_filter_list(filter_list)

	if type(filter_list) ~= 'table' 
	then
		debug("filter list is empty or not a table")
		return nil
	end

	local array = reserve_array(filter_list)


	if nil == execute_cmd(format_delete_item_cmd(FIREWALL_IP_MAC_BIND_FILTER_PREX))
	then
		debug("execute clear cmd fail")
		return false
	end

	if table.maxn(filter_list) <= 0
	then
		return true
	end

	for i=1,table.maxn(array)
	do
		local temp = array[i]
		if check_ip(temp["ipaddr"]) and check_mac(temp["mac"]) 
		then
			execute_cmd(format_set_ipmac_bind_filter_cmd("all", temp["ipaddr"], temp["mac"], temp["comment"]), temp["iswork"])
		else
			debug("check fail ", temp["ipaddr"]," ", temp["mac"], " ", temp["comment"])
			return false
		end
	end

	
	return true
end

-- get url filter list
-- input:none
-- return:the array of struct url_filter 
function firewall_module.firewall_get_url_filter_list()

	local data = execute_cmd(format_get_url_filter_cmd())
	local url_filter_list = {}
	if nil == data
	then
		debug("execute get  url filter cmd fail")
		return nil
	elseif "success" == data
	then
		debug("the url filter list is empty")
		return nil
	end



	for i = 1, table.maxn(data)
	do
		debug(data[i])
		local array = split(data[i], "##")
		local temp = firewall_module.url_filter:new(nil,nil)
		temp["url"] = array[3]
		temp["action"] = array[4]
		temp["comment"] = array[5]
		temp["interface"] = array[6]
		temp["iswork"] = rule_is_work(data[i])
		url_filter_list[i] = temp

	end

	return reserve_array(url_filter_list)
end

-- set url filter
-- input:the array of struct url_filter
-- 		input example
--[[
local urls={
		{["url"]='sina.cn', ["action"]='ACCEPT', ["comment"]="test url filter", ["interface"]= nil},
		{["url"]='baidu.com', ["action"]='DROP', ["comment"]="test url filter", ["interface"]="lan"}

		}
]]--
-- return:true if success false if fail
function firewall_module.firewall_set_url_filter_list(filter_list)

	if type(filter_list) ~= 'table'
	then
		debug("filter list is empty or not a table")
		return nil
	end

	local array = reserve_array(filter_list)

	if nil == execute_cmd(format_delete_item_cmd(FIREWALL_URL_FILTER_PREX))
	then
		debug("execute clear cmd fail")
		return false
	end

	if table.maxn(filter_list) <= 0
	then
		return true
	end

	for i=1,table.maxn(array)
	do
		local temp = array[i]

		if check_action(temp["action"]) 
		then
			execute_cmd(format_set_url_filter_cmd(temp["url"], temp["action"], temp["comment"] or "", temp["interface"]), temp["iswork"])
		else
			debug("check fail ", temp["action"]," ", temp["url"], " ", temp["comment"])
			return false
		end
	end

	
	return true
end

-- get ip filter list
-- input:none
-- return:the array of struct ip_filter
function firewall_module.firewall_get_ip_filter_list()

	local data = execute_cmd(format_get_ip_filter_cmd())
	local ip_filter_list = {}
	local j = 1

	if nil == data
	then
		debug("execute get  ip filter cmd fail")
		return nil
	elseif "success" == data
	then
		debug("the ip filter list is empty")
		return nil
	end



	for i = 1, table.maxn(data)
	do
		
		local array = split(data[i], "##")
		local temp = firewall_module.ip_filter:new(nil,nil)

		temp["protocol"] = array[3]
		temp["ipaddr"] = array[4]
		temp["action"] = array[5]
		temp["comment"] = array[6]
		temp["iswork"] = rule_is_work(data[i])
		if '1' == array[7]
		then
			ip_filter_list[j] = temp
			j = j+1
		end

	end

	return reserve_array(ip_filter_list)
end

-- set ip filter
-- input:the array of struct ip_filter
--		input example
--[[
local ips={
		{["protocol"] = "all",["ipaddr"]='192.168.3.6', ["action"]='ACCEPT', ["comment"]="test ip filter"},
		{["protocol"] = "tcp",["ipaddr"]='192.168.4.6', ["action"]='DROP', ["comment"]="test ip filter"}

		}
]]--
-- return:true if success false if fail
function firewall_module.firewall_set_ip_filter_list(filter_list)

	if type(filter_list) ~= 'table'
	then
		debug("filter list is empty or not a table")
		return nil
	end

	local array = reserve_array(filter_list)

	if nil == execute_cmd(format_delete_item_cmd(FIREWALL_IP_FILTER_PREX))
	then
		debug("execute clear cmd fail")
		return false
	end

	if table.maxn(filter_list) <= 0
	then
		return true
	end

	for i=1,table.maxn(array)
	do
		local temp = array[i]

		if check_action(temp["action"]) and check_ip(temp["ipaddr"]) and check_protocol(temp["protocol"])
		then
			execute_cmd(format_set_ip_filter_cmd(temp["ipaddr"],temp["protocol"], temp["action"], temp["comment"]), temp["iswork"])
		else
			debug("check fail ", temp["action"]," ", temp["ipaddr"], " ", temp["comment"], " ", temp["protocol"])
			return false
		end
	end

	
	return true
end


-- get acl filter list
-- input:none
-- return:the array of struct acl_filter
function firewall_module.firewall_get_acl_filter_list()

	local data = execute_cmd(format_get_acl_filter_cmd())
	local acl_filter_list = {}
	local j = 1

	if nil == data
	then
		debug("execute get  ip filter cmd fail")
		return nil
	elseif "success" == data
	then
		debug("the ip filter list is empty")
		return nil
	end


	--print("data = ", data[1])
	for i = 1, table.maxn(data)
	do
		
		local array = split(data[i], "##")
		local temp = firewall_module.acl_filter:new(nil,nil)

		temp["protocol"] = array[3]
		temp["src_ipaddr"] = array[4]
		temp["dest_ipaddr"] = array[5]
		temp["action"] = array[6]
		temp["comment"] = array[7]
		temp["iswork"] = rule_is_work(data[i])
		acl_filter_list[i] = temp

	end

	return reserve_array(acl_filter_list)
end

-- set acl filter
-- input:the array of struct acl_filter
--		input example
--[[
local ips={
		{["protocol"] = "all",["src_ipaddr"]='192.168.3.6', ["dest_ipaddr"]='192.168.3.99',["action"]='ACCEPT', ["comment"]="test acl filter"},
		{["protocol"] = "tcp",["src_ipaddr"]='192.168.3.6', ["dest_ipaddr"]='192.168.3.88',["action"]='DROP', ["comment"]="test acl filter"},
		{["protocol"] = "udp",["src_ipaddr"]='192.168.3.6', ["dest_ipaddr"]='192.168.3.77',["action"]='DROP', ["comment"]="test acl filter"},
		}
]]--
-- return:true if success false if fail
function firewall_module.firewall_set_acl_filter_list(acl_list)

	if type(acl_list) ~= 'table'
	then
		debug("filter list is empty or not a table")
		return nil
	end

	local array = reserve_array(acl_list)

	if nil == execute_cmd(format_delete_item_cmd(FIREWALL_ACL_FILTER_PREX))
	then
		debug("execute clear cmd fail")
		return false
	end

	if table.maxn(acl_list) <= 0
	then
		return true
	end

	for i=1,table.maxn(array)
	do
		local temp = array[i]

		if check_action(temp["action"]) and check_ip(temp["src_ipaddr"]) and check_ip(temp["dest_ipaddr"]) and check_protocol(temp["protocol"])
		then
			execute_cmd(format_set_acl_filter_cmd(temp["src_ipaddr"],temp["dest_ipaddr"],temp["protocol"], temp["action"], temp["comment"]), temp["iswork"])
		else
			debug("check fail ", temp["action"]," ", temp["src_ipaddr"]," ", temp["dest_ipaddr"], " ", temp["comment"], " ", temp["protocol"])
			return false
		end
	end

	
	return true
end


-- get port redirect list
-- input:none
-- return:the array of struct port_filter
function firewall_module.firewall_get_port_filter_list()

	local data = execute_cmd(format_get_port_filter_cmd())
	local port_filter_list = {}
	local j = 1

	if nil == data
	then
		debug("execute get  port filter cmd fail")
		return nil
	elseif "success" == data
	then
		debug("the port filter list is empty")
		return nil
	end

	for i = 1, table.maxn(data)
	do
		
		local array = split(data[i], "##")
		local temp = firewall_module.port_filter:new(nil,nil)

		temp["port"] = array[3]
		temp["protocol"] = array[4]
		temp["action"] = array[5]
		temp["comment"] = array[6]
		temp["iswork"] = rule_is_work(data[i])
		if '1' == array[7] or nil == array[7]
		then
			port_filter_list[j] = temp
			j = j+1
		end

	end

	return reserve_array(port_filter_list)
end

-- set port filter
-- input:the array of struct port_filter
--		input example
--[[
local ports={
		{["port"]='4545',["protocol"]='tcp', ["action"]='ACCEPT', ["comment"]="test port filter1"},
		{["port"]='1902',["protocol"]='udp', ["action"]='DROP', ["comment"]="test port filter2"},
		{["port"]='907',["protocol"]='all',["action"]='DROP', ["comment"]="test port filter3"}
		}
]]--
-- return:true if success false if fail
function firewall_module.firewall_set_port_filter_list(filter_list)

	if type(filter_list) ~= 'table'
	then
		debug("filter list is empty or not a table")
		return nil
	end

	local array = reserve_array(filter_list)

	if nil == execute_cmd(format_delete_item_cmd(FIREWALL_PORT_FILTER_PREX))
	then
		debug("execute clear cmd fail")
		return false
	end

	if table.maxn(filter_list) <= 0
	then
		return true
	end

	for i=1,table.maxn(array)
	do
		local temp = array[i]

		if check_action(temp["action"]) and check_protocol(temp["protocol"])
		then
			execute_cmd(format_set_port_filter_cmd(temp["port"],temp["protocol"], temp["action"], temp["comment"]), temp["iswork"])
		else
			debug("check fail ", temp["action"]," ", temp["protocol"], " ", temp["port"], " ", temp["comment"])
			return false
		end
	end

	
	return true
end


-- get port redirect list
-- input:none
-- return:the array of struct port_redirect
function firewall_module.firewall_get_port_redirect_list()

	local data = execute_cmd(format_get_port_redirect_cmd())
	local port_redirect_list = {}
	local j = 1

	if nil == data
	then
		debug("execute get  port redirect cmd fail")
		return nil
	elseif "success" == data
	then
		debug("the port redirect list is empty")
		return nil
	end

	for i = 1, table.maxn(data)
	do
		
		local array = split(data[i], "##")
		local temp = firewall_module.port_redirect:new(nil,nil)

		temp["protocol"] = array[3]
		temp["dest_addr"] = array[4]
		temp["dest_port"] = array[5]
		temp["redirect_addr"] = array[6]
		temp["redirect_port"] = array[7]
		temp["comment"] = array[8]
		temp["dest_port_end"] = array[9]
		temp["iswork"] = rule_is_work(data[i])
		if '1' == array[10] or nil == array[10]
		then
			port_redirect_list[j] = temp
			j = j+1
		end

	end

	return reserve_array(port_redirect_list)
end

-- set port redirect 
-- input: the array of struct port_redirect
--      input example 
--[[
	local ports={
		{["dest_addr"]=nil, ["dest_port"]="90", ["protocol"]='tcp', ["redirect_addr"]='192.168.2.1',["redirect_port"]='8080', ["comment"]="test port redirect1"},
		{["dest_addr"]='192.168.2.131', ["dest_port"]="80",["dest_port_end"]="89", ["protocol"]='udp', ["redirect_addr"]='192.168.2.1',["redirect_port"]='8090', ["comment"]="test port redirect2"},
		{["dest_addr"]='192.168.2.131', ["dest_port"]="22", ["protocol"]='all', ["redirect_addr"]='192.168.2.1',["redirect_port"]='80', ["comment"]="test port redirect3"}
		}
]]--
-- return:true if success false if fail
function firewall_module.firewall_set_port_redirect_list(redirect_list)

	if type(redirect_list) ~= 'table'
	then
		debug("redirect list is empty or not a table")
		return nil
	end

	local array = reserve_array(redirect_list)

	if nil == execute_cmd(format_delete_item_cmd(FIREWALL_PORT_REDIRECT_PREX))
	then
		debug("execute clear cmd fail")
		return false
	end

	if table.maxn(redirect_list) <= 0
	then
		return true
	end

	for i=1,table.maxn(array)
	do
		local temp = array[i]

		if  check_protocol(temp["protocol"]) and check_ip(temp["redirect_addr"])
		then
			if nil ~= temp["dest_addr"] and check_ip(temp["dest_addr"]) == false
			then
				debug("dest_addr check error")
				return false
			end

			execute_cmd(format_set_port_redirect_cmd(temp["protocol"], temp["dest_addr"], temp["dest_port"], temp["dest_port_end"] or temp["dest_port"], temp["redirect_addr"], temp["redirect_port"], temp["comment"]), temp["iswork"])
		else
			debug(temp["protocol"], " ", temp["dest_addr"]," ",  temp["dest_port"]," ",  temp["redirect_addr"]," ",  temp["redirect_port"]," ", temp["comment"])
			return false
		end
	end	
	return true
end


local function get_section_name_by_type(type_name)

	if nil == type_name
	then
		debug("type_name nil")
		return nil
	end

	debug("type_name = ", type_name)

	local all_config = x:get_all(FIREWALL_CONFIG_FILE)

	if nil == all_config 
	then
		debug("all_config = nil")
	end

	for k,v in pairs(all_config) do
--		print(v[".type"])
		if type_name == v[".type"] then
			return tostring(v[".name"])
		end
	end

	debug("not found")
	return nil

end

local function check_section_is_exist(section_name)

	if nil == section_name
	then
		debug("section_name nil")
		return nil
	end

	debug("section_name = ", section_name)

	local all_config = x:get_all(FIREWALL_CONFIG_FILE)

	if nil == all_config 
	then
		debug("all_config = nil")
	end

	for k,v in pairs(all_config) do
--		print(v[".type"])
		if section_name == v[".name"] then
			return true
		end
	end

	return false
end

-- get default policy for firewall
-- input:none
-- return:
--		ACCEPT: default pass forward packets
--		DROP:  default drop forward packets
--		nil:get fail
function firewall_module.firewall_get_default_action()

	if check_section_is_exist("default_forward")
	then
		local src = x:get(FIREWALL_CONFIG_FILE, "default_forward", "src")
		local dest = x:get(FIREWALL_CONFIG_FILE, "default_forward", "dest")

		if "lan" == src and "wan" == dest
		then
			return "ACCEPT"
		else
			return "DROP"
		end
	end

	debug("firewall get defaults action fail")
	return nil

end

-- set default pocily for firewall
-- input:
--		ACCEPT: default pass forward packets
--		DROP:  default drop forward packets
--		REJECT: default reject forward packets
-- return:
--		true if success false if fail
function firewall_module.firewall_set_default_action(action)

	if check_action(action) == false
	then
		debug("check_action fail")
		return false
	end

	if check_section_is_exist("default_forward")
	then
		if "ACCEPT" ~= action
		then
			x:delete(FIREWALL_CONFIG_FILE, "default_forward", "src")
			x:delete(FIREWALL_CONFIG_FILE, "default_forward", "dest")
		else 
			x:set(FIREWALL_CONFIG_FILE,"default_forward", "src", "lan")
			x:set(FIREWALL_CONFIG_FILE,"default_forward", "dest", "wan")
		end
	else
		debug("no default_forward section")
		return false
	end
	
	if x:commit(FIREWALL_CONFIG_FILE)
	then
		debug("set firewall default action success")
		return true
	end

	debug("set firewall default action fail")
	return false
end


-- input:none
-- return:true for allow remote web login 
--		  false for not allow remote web login
function firewall_module.firewall_remote_get_web_login()
		local ret = x:get(TOZED_CONFIG_FILE,TOZED_REMOTE_CONTROL_SECTION, "TZ_REMOTE_LOGIN")

		if "1" ==  ret
		then
			return true
		else
			return false
		end
end

-- input:onoff(number):1 for turn on the remote web login, 0 for turn off the remote web login
-- return:true for set success ,false for set fail
function firewall_module.firewall_remote_set_web_login(onoff)

	if 1 == onoff
	then
		x:set(TOZED_CONFIG_FILE,TOZED_REMOTE_CONTROL_SECTION, "TZ_REMOTE_LOGIN", 1)
	else
		x:set(TOZED_CONFIG_FILE,TOZED_REMOTE_CONTROL_SECTION, "TZ_REMOTE_LOGIN", 0)
	end

	return x:commit(TOZED_CONFIG_FILE)
end

-- input:none
-- output:the array of default access ip list
function firewall_module.firewall_remote_get_default_list()
	local ret = x:get(TOZED_CONFIG_FILE,TOZED_REMOTE_CONTROL_SECTION, "TZ_REMOTE_DEFAULT_LIST")
	return ret
end

-- input:
--		list:the array of default access ip list
-- return:true for set success ,false for set fail
function firewall_module.firewall_remote_set_default_list(list)
	if nil == list
	then
		x:set(TOZED_CONFIG_FILE,TOZED_REMOTE_CONTROL_SECTION, "TZ_REMOTE_DEFAULT_LIST", "")
	else
		x:set(TOZED_CONFIG_FILE,TOZED_REMOTE_CONTROL_SECTION, "TZ_REMOTE_DEFAULT_LIST", list)
	end
	return x:commit(TOZED_CONFIG_FILE)
end

-- input:none
-- output:the array of user access ip list
function firewall_module.firewall_remote_get_web_login_list()
	local ret = x:get(TOZED_CONFIG_FILE,TOZED_REMOTE_CONTROL_SECTION, "TZ_REMOTE_LOGIN_LIST")
	return ret
end

-- input:
--		list:the array of user access ip list
-- return:true for set success ,false for set fail
function firewall_module.firewall_remote_set_web_login_list(list)
	if nil == list
	then
		x:set(TOZED_CONFIG_FILE,TOZED_REMOTE_CONTROL_SECTION, "TZ_REMOTE_LOGIN_LIST", "")
	else
		x:set(TOZED_CONFIG_FILE,TOZED_REMOTE_CONTROL_SECTION, "TZ_REMOTE_LOGIN_LIST", list)
	end
	return x:commit(TOZED_CONFIG_FILE)
end


-- input:none
-- return:true for turn on the remote ping , false for turn off
function firewall_module.firewall_remote_get_ping()
	local ret = x:get(TOZED_CONFIG_FILE,TOZED_REMOTE_CONTROL_SECTION, "TZ_REMOTE_PING")

	if "1" ==  ret
	then
		return true
	else
		return false
	end
end

-- input:onoff(number):1 for turn on remote ping , 0 for turn off
-- return:true for set success ,false for set fail
function firewall_module.firewall_remote_set_ping(onoff)
	if 1 == onoff
	then
		x:set(TOZED_CONFIG_FILE,TOZED_REMOTE_CONTROL_SECTION, "TZ_REMOTE_PING", 1)
	else
		x:set(TOZED_CONFIG_FILE,TOZED_REMOTE_CONTROL_SECTION, "TZ_REMOTE_PING", 0)
	end

	return x:commit(TOZED_CONFIG_FILE)
end


-- input:none
-- output:the array of user ping access list
function firewall_module.firewall_remote_get_ping_list()
	local ret = x:get(TOZED_CONFIG_FILE,TOZED_REMOTE_CONTROL_SECTION, "TZ_REMOTE_PING_LIST")
	return ret
end

-- input:
--		list:the array of user ping access list
-- return:true for set success ,false for set fail
function firewall_module.firewall_remote_set_ping_list(list)
	if nil == list
	then
		x:set(TOZED_CONFIG_FILE,TOZED_REMOTE_CONTROL_SECTION, "TZ_REMOTE_PING_LIST", "")
	else
		x:set(TOZED_CONFIG_FILE,TOZED_REMOTE_CONTROL_SECTION, "TZ_REMOTE_PING_LIST", list)
	end
	return x:commit(TOZED_CONFIG_FILE)
end



local function get_sub_net_by_ip(ip, netmask)
		local cmd = string.format("/bin/ipcalc.sh %s %s", ip, netmask)
		local f = io.popen(cmd)
		local res = f:read()
		local sub_net = nil

		while nil ~= res
		do
			if nil ~= string.find(res,"NETWORK")
			then
				local ar = split(res, "=")
				sub_net = ar[2]
				break
			end

			res = f:read()
		end
		io.close(f)

		return sub_net
end


function  firewall_module.firewall_get_lan_network()
	local list = {}
	local i = 1
	x:foreach(NETWORK_CONFIG_FILE, NETWORK_UCI_INTERFACE, function(s)
		if "bridge" == s["type"] 
		then
			list[i] = s[".name"]
			i = i+1
		end
	end)

	return list
end

-- set mutil nat 
-- input:
--	  network_list:the list of lan network name which want to turn on the  nat , the lan network name can get by firewall_get_lan_network
--					if the network_list == nil or  have no member it will turn off all nat 
-- return:
--		true if success  false if fail
function firewall_module.firewall_set_mutil_nat(network_list)

		local ret = true
		x:foreach(FIREWALL_CONFIG_FILE, FIREWALL_UCI_ZONE, function(s)
			if "wan" == s["name"]
			then
			
				local temp = {}
				local i = 1

				if nil ~= network_list
				then
					for k,v in pairs(network_list)
					do
						local ipaddr = x:get(NETWORK_CONFIG_FILE,v,"ipaddr")
						local netmask = x:get(NETWORK_CONFIG_FILE,v,"netmask")
						if nil == ipaddr or nil == netmask
						then
							debug("unknown network,input error")
							ret = false
							return 
						end

						local sub_net = get_sub_net_by_ip(ipaddr, netmask)
						if nil == sub_net
						then
							debug("error subnet")
							ret = false
							return 
						end

						temp[i] = sub_net .. "/" .. netmask
						i = i + 1

					end
				end


				if 	table.maxn(temp) == 0
				then
					x:set(FIREWALL_CONFIG_FILE,s[".name"],"masq_src", '')
					x:set(FIREWALL_CONFIG_FILE,s[".name"], "masq", "0")
				else
					x:set(FIREWALL_CONFIG_FILE,s[".name"],"masq_src", temp)
					x:set(FIREWALL_CONFIG_FILE,s[".name"], "masq", "1")
				end

			end

		end)

		if false == ret 
		then
			return ret
		end

		return x:commit(FIREWALL_CONFIG_FILE)
end

-- get mutil onoff
-- input(string):
--		network: the lan network name which get by firewall_get_lan_network
-- return(number):
--		1: on
--		0:off
--		nil:get error
function firewall_module.firewall_get_mutil_nat(network)

		local flag = 0
		x:foreach(FIREWALL_CONFIG_FILE, FIREWALL_UCI_ZONE, function(s)
			if "wan" == s["name"]
			then
				if "0" == s["masq"]
				then
					flag = 0
					return
				end

				if "1" == s["masq"] and nil == s["masq_src"] 
				then
					flag = 1
					return
				end

				if "1" == s["masq"] and "" == s["masq_src"][1]
				then
					flag = 1
					return
				end

				local ipaddr = x:get(NETWORK_CONFIG_FILE,network,"ipaddr")
				local netmask = x:get(NETWORK_CONFIG_FILE,network,"netmask")
				if nil == ipaddr or nil == network
				then
					debug("unknown network,input error")
					return nil
				end

				local sub_net = get_sub_net_by_ip(ipaddr, netmask)
				if nil == sub_net
				then
					debug("error subnet")
					return nil
				end

				if nil ~= s["masq_src"]
				then
					for k,v in pairs(s["masq_src"])
					do
						if string.find(v, sub_net .. "/" .. netmask) ~= nil
						then
							flag = 1
						end
					end
				end


				
			end

		end)

		return flag
end

firewall_module.static_route = {
		
	["target_ip"] = nil,	 	-- target ip
	["target_netmask"] = nil,  	-- target netmask
	["next_ip"] = nil, 			-- next ip
	["target_inferface"] = nil  -- target interface
}

function firewall_module.static_route:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
		
	self["target_ip"] = obj["target_ip"] or nil
	self["target_netmask"] = obj["target_netmask"] or nil 
	self["next_ip"] = obj["next_ip"] or nil
	self["target_inferface"] = obj["target_inferface"] or nil
end


local function check_net_interface(inter)
	if "" == inter or nil == inter
	then
		return false
	end

	local cmd = string.format("ls /sys/class/net/ | grep '^%s$' | wc -l", inter)
	local f = io.popen(cmd)

	if nil == f
	then
		return false
	end

	local res = f:read()
	debug("res = ", res)
	if "1" == res
	then
		return true
	end

	return false

end

-- set the user static route
-- input:
--		route_list: the array of struct static_route
-- return: true if success or false if fail
function firewall_module.firewall_set_static_route(route_list)

	local list = {}
	local i = 1

	x:delete(TOZED_CONFIG_FILE,TOZED_CONFIG_FILE,TOZED_UCI_OPTION_STATIC_ROUTE)
	for k,temp in pairs(route_list)
	do

		debug(temp["target_ip"])
		debug(temp["next_ip"])
		debug(temp["target_netmask"])
		debug(temp["target_interface"])

		if check_ip(temp["target_ip"]) and check_ip(temp["next_ip"]) and check_ip(temp["target_netmask"]) and check_net_interface(temp["target_interface"])
		then
			list[i] = temp["target_interface"] .. " " .. temp["target_ip"] .. " " .. temp["target_netmask"] .. " " .. temp["next_ip"]
			debug(list[i])
			i = i + 1
		end

	end

	x:set(TOZED_CONFIG_FILE, TOZED_UCI_SECTION,TOZED_UCI_OPTION_STATIC_ROUTE, list)

	return x:commit(TOZED_CONFIG_FILE)

end

-- get the user static route
-- input:none
-- return: the array of struct static_route
function firewall_module.firewall_get_static_route()

	local list =  x:get(TOZED_CONFIG_FILE, TOZED_UCI_SECTION, TOZED_UCI_OPTION_STATIC_ROUTE)
	local route_list = {}
	local i = 1

	for k,v in pairs(list)
	do
		v = string.gsub(v," ", "#")
		local ar = split(v,"#")
		local temp = firewall_module.static_route:new(nil, nil)
		temp["target_interface"] = ar[1]
		temp["target_ip"] = ar[2]
		temp["target_netmask"] = ar[3]
		temp["next_ip"] = ar[4]

		route_list[i] = temp
		i = i + 1
	end


	return route_list


end

return firewall_module