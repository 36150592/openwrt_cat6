#!/usr/bin/lua

local wifi = require "tz.wifi"
local dhcp = require "tz.dhcp"
local modem = require "tz.modem"
local network = require "tz.network"
local sim = require "tz.sim"
local system = require "tz.system"
local device = require "tz.device"
local firewall = require "tz.firewall"


function firewall_set_default_action()

	firewall.firewall_set_default_action(arg[2])
end


function firewall_get_port_redirect()

	print("in firewall_get_port_redirect")
	local info = firewall.firewall_get_port_redirect_list()

	if nil == info 
	then
		print("info is nil")
		return
	end

	for k,v in pairs(info) do 
			for key,value in pairs(v) do
				print(key,"=",value)
			end
	end

end

function firewall_set_port_redirect()
		print("in firewall_set_port_redirect")
		local ports={
		{["dest_addr"]=nil, ["dest_port"]="90", ["protocol"]='tcp', ["redirect_addr"]='192.168.2.1',["redirect_port"]='8080', ["comment"]="test port redirect1"},
		{["dest_addr"]='192.168.2.131', ["dest_port"]="80", ["protocol"]='udp', ["redirect_addr"]='192.168.2.1',["redirect_port"]='8090', ["comment"]="test port redirect2"},
		{["dest_addr"]='192.168.2.131', ["dest_port"]="22", ["protocol"]='all', ["redirect_addr"]='192.168.2.1',["redirect_port"]='80', ["comment"]="test port redirect3"}
		}

		if firewall.firewall_set_port_redirect_list(ports)
		then
			print("set success")
		else
			print("set fail")
		end
end



function firewall_get_port_filter()

	print("in firewall_get_port_filter")
	local info = firewall.firewall_get_port_filter_list()

	if nil == info 
	then
		print("info is nil")
		return
	end

	for k,v in pairs(info) do 
			for key,value in pairs(v) do
				print(key,"=",value)
			end
	end
end

function firewall_set_port_filter()
		print("in firewall_set_port_filter")
		local ports={
		{["port"]='4545',["protocol"]='tcp', ["action"]='ACCEPT', ["comment"]="test port filter1"},
		{["port"]='1902',["protocol"]='udp', ["action"]='DROP', ["comment"]="test port filter2"},
		{["port"]='907',["protocol"]='all',["action"]='DROP', ["comment"]="test port filter3"}
		}

		if firewall.firewall_set_port_filter_list(ports)
		then
			print("set success")
		else
			print("set fail")
		end
end


function firewall_get_ip_filter()

	print("in firewall_get_ip_filter")
	local info = firewall.firewall_get_ip_filter_list()

	if nil == info 
	then
		print("info is nil")
		return
	end

	for k,v in pairs(info) do 
			for key,value in pairs(v) do
				print(key,"=",value)
			end
	end

end

function firewall_set_ip_filter()
		print("in firewall_set_ip_filter")
		local ips={
		{["ipaddr"]='192.168.3.6', ["action"]='ACCEPT', ["comment"]="test ip filter"},
		{["ipaddr"]='192.168.4.6', ["action"]='DROP', ["comment"]="test ip filter"}

		}

		if firewall.firewall_set_ip_filter_list(ips)
		then
			print("set success")
		else
			print("set fail")
		end
end


function firewall_get_url_filter()

	print("in firewall_get_url_filter")
	local info = firewall.firewall_get_url_filter_list()

	if nil == info 
	then
		print("info is nil")
		return
	end

	for k,v in pairs(info) do 
			for key,value in pairs(v) do
				print(key,"=",value)
			end
	end

end

function firewall_set_url_filter()
		print("in firewall_set_url_filter")
		local urls={
		{["url"]='sina.cn', ["action"]='ACCEPT', ["comment"]="test url filter"},
		{["url"]='baidu.com', ["action"]='DROP', ["comment"]="test url filter"}

		}

		if firewall.firewall_set_url_filter_list(urls)
		then
			print("set success")
		else
			print("set fail")
		end
end


function firewall_get_mac_filter()

	print("in firewall_get_mac_filter")
	local info = firewall.firewall_get_mac_filter_list()

	if nil == info 
	then
		print("info is nil")
		return
	end

	for k,v in pairs(info) do 
			for key,value in pairs(v) do
				print(key,"=",value)
			end
	end

end



function firewall_set_mac_filter()
		print("in firewall_set_mac_filter")
		local macs={
		{["mac"]='aa:bb:cc:11:22:33', ["action"]='DROP', ["comment"]="test mac filter"},
		{["mac"]='aa:bb:cc:11:22:34', ["action"]='DROP', ["comment"]="test mac filter"}

		}

		if firewall.firewall_set_mac_filter_list(macs)
		then
			print("set success")
		else
			print("set fail")
		end
end


function device_get_info()
	
	print("in device_get_info")
	local info = device.device_get_info()

	if nil == info 
	then
		print("info is nil")
		return
	end


	for k,v in pairs(info) do 
			print(k,"=", v)
	end

end

function system_get_status()

	print("in system_get_status")
	local info = system.system_get_status()

	if nil == info 
	then
		print("info is nil")
		return
	end


	for k,v in pairs(info) do 
			print(k,"=", v)
	end

end

function sim_get_status()

	print("in sim_get_status")
	local info = sim.sim_get_status()

	if nil == info 
	then
		print("info is nil")
		return
	end

	for k,v in pairs(info) do 
			print(k,"=", v)
	end

end


function network_get_wan_info()

	print("in network_get_wan_info")
	local info = network.network_get_wan_info()

	if nil == info 
	then
		print("info is nil")
		return
	end

	for k,v in pairs(info) do 
			print(k,"=", v)
	end

end


function network_get_4g_net_info()

	print("in network_get_4g_net_info")
	local info = network.network_get_4g_net_info()

	if nil == info 
	then
		print("info is nil")
		return
	end

	for k,v in pairs(info) do 
			print(k,"=", v)
	end

end


function modem_get_status()

	print("in modem_get_status")
	local status = modem.modem_get_status()

	if nil == status 
	then
		print("status is nil")
		return
	end

	for k,v in pairs(status) do 
			print(k,"=", v)
	end

end

function modem_get_info()

	print("in modem_get_info")
	local info = modem.modem_get_info()

	if nil == info 
	then
		print("info is nil")
		return
	end

	for k,v in pairs(info) do 
			print(k,"=", v)
	end

end

function wifi_get_dev()
	print("in wifi_get_dev")
	local array = wifi.wifi_get_dev()
	for i = 1,table.maxn(array) do 
		temp = array[i]
		
		print('<------------',i,'------>')
		for k,v in pairs(temp) do
			print(k," = ", v)
		end

	end
end

function wifi_get_hidden_ssid()
	print("wifi_get_hidden_ssid")
	local hidden = wifi.wifi_get_hidden_ssid(tonumber(arg[2]))
	print(hidden)
	if "0" == hidden 
	then
		print("ssid hidden is disable")
	elseif "1" == hidden
	then
		print("ssid hidden is enable")
	else
		print("get ssid hidden config fail")
	end
end

function wifi_enable_hidden_ssid()
	print("wifi_enable_hidden_ssid")
	local ret = wifi.wifi_enable_hidden_ssid(tonumber(arg[2]))

	if  ret 
	then
		print("enable hidden ssid success")
	else
		print("enable hidden ssid fail")
	end
end

function wifi_disable_hidden_ssid()
	print("wifi_disable_hidden_ssid")
	local ret = wifi.wifi_disable_hidden_ssid(tonumber(arg[2]))

	if  ret 
	then
		print("disable hidden ssid success")
	else
		print("disable hidden ssid fail")
	end
end


function wifi_get_mode()
	print("wifi_get_mode")
	local ret = wifi.wifi_get_mode(tonumber(arg[2]))

	if  nil ~= ret 
	then
		print("wifi mode = ", ret)
	else
		print("get wifi mode fail")
	end
end

function wifi_set_mode()
	print("wifi_set_mode")
	local ret = wifi.wifi_set_mode(tonumber(arg[2]), tonumber(arg[3]))

	if  ret 
	then
		print("set wifi mode success")
	else
		print("set wifi fail")
	end
end

function wifi_get_ssid()
	print("wifi_get_ssid")
	local ret = wifi.wifi_get_ssid(tonumber(arg[2]))

	if  nil ~= ret 
	then
		print("wifi ssid = ", ret)
	else
		print("get ssid fail")
	end
end

function wifi_set_ssid()
	print("wifi_set_ssid")
	local ret = wifi.wifi_set_ssid(tonumber(arg[2]), arg[3])

	if  ret 
	then
		print("set wifi ssid success")
	else
		print("set ssid fail")
	end
end


function wifi_get_password()
	print("wifi_get_password")
	local ret = wifi.wifi_get_password(tonumber(arg[2]))

	if  nil ~= ret 
	then
		print("wifi password = ", ret)
	else
		print("get password  fail")
	end
end

function wifi_set_password()
	print("wifi_set_password")
	print(type(arg[3]))
	local ret = wifi.wifi_set_password(tonumber(arg[2]), arg[3])

	if  ret 
	then
		print("set wifi password success")
	else
		print("set password fail")
	end
end

function wifi_get_txpower()
	print("wifi_get_txpower")
	local ret = wifi.wifi_get_txpower(tonumber(arg[2]))

	if  nil ~= ret 
	then
		print("wifi txpower = ", ret)
	else
		print("get txpower  fail")
	end
end

function wifi_set_txpower()
	print("wifi_set_txpower")
	local ret = wifi.wifi_set_txpower(tonumber(arg[2]), tonumber(arg[3]))

	if  ret 
	then
		print("set wifi txpower success")
	else
		print("set txpower fail")
	end
end

function wifi_get_channel()
	print("wifi_get_channel")
	local ret = wifi.wifi_get_channel(tonumber(arg[2]))

	if  nil ~= ret 
	then
		print("wifi channel = ", ret)
	else
		print("get channel  fail")
	end
end

function wifi_set_channel()
	print("wifi_set_channel")
	local ret = wifi.wifi_set_channel(tonumber(arg[2]), arg[3])

	if  ret 
	then
		print("set wifi channel success")
	else
		print("set channel fail")
	end
end


function wifi_get_ht_mode()
	print("wifi_get_ht_mode")
	local ret = wifi.wifi_get_ht_mode(tonumber(arg[2]))

	if  nil ~= ret 
	then
		print("ht mode = ", ret)
	else
		print("get ht mode fail")
	end
end

function wifi_set_ht_mode()
	print("wifi_set_ht_mode")
	local ret = wifi.wifi_set_ht_mode(tonumber(arg[2]), tostring(arg[3]))

	if  ret 
	then
		print("set ht mode success")
	else
		print("set ht fail")
	end
end


function wifi_get_encryption()
	print("wifi_get_encryption")
	local ret = wifi.wifi_get_encryption(tonumber(arg[2]))

	if  nil ~= ret 
	then
		print("encryption = ", ret)
	else
		print("get encryption fail")
	end
end

function wifi_set_encryption()
	print("wifi_set_encryption")
	local ret = wifi.wifi_set_encryption(tonumber(arg[2]), tostring(arg[3]))

	if  ret 
	then
		print("set encryption success")
	else
		print("set encryption fail")
		print("usepage: wifi_set_encryption wifi_id  wep|psk|psk2|miexed|none")
	end
end


function wifi_get_encryption_type()
	print("wifi_get_encryption_type")
	local ret = wifi.wifi_get_encryption_type(tonumber(arg[2]))

	if  nil ~= ret 
	then
		print("encryption type = ", ret)
	else
		print("get encryption type fail")
		print("usepage:wifi_get_encryption_type wifi_id")
	end
end

function wifi_set_encryption_type()
	print("wifi_set_encryption_type")
	local ret = wifi.wifi_set_encryption_type(tonumber(arg[2]), tostring(arg[3]))

	if  ret 
	then
		print("set encryption type success")
	else
		print("set encryption type fail")
		print("usepage: wifi_set_encryption_type wifi_id ccmp+tkip|ccmp|tkip|none")
	end
end



function wifi_get_wmm()
	print("wifi_get_wmm")
	local ret = wifi.wifi_get_wmm(tonumber(arg[2]))

	if  nil ~= ret 
	then
		print("wmm = ", ret)
	else
		print("get encryption type fail")
		print("usepage:wifi_get_wmm wifi_id")
	end
end

function wifi_enable_wmm()
	print("wifi_enable_wmm")
	local ret = wifi.wifi_enable_wmm(tonumber(arg[2]))

	if  ret 
	then
		print("enable wmm success")
	else
		print("set wmm fail")
		print("usepage: wifi_enable_wmm wifi_id ")
	end
end

function wifi_disable_wmm()
	print("wifi_disable_wmm")
	local ret = wifi.wifi_disable_wmm(tonumber(arg[2]))

	if  ret 
	then
		print("disable wmm success")
	else
		print("set wmm fail")
		print("usepage: wifi_disable_wmm wifi_id ")
	end
end		


function wifi_start()
	print("wifi_start_wifi")
	local ret = wifi.wifi_start(tonumber(arg[2]))

	if  ret 
	then
		print("start wifi success")
	else
		print("starat wifi fail")
		print("usepage: wifi_start_wifi ")
	end
end	

function wifi_start_all()
	print("wifi_start_all")
	local ret = wifi.wifi_start_all()

	if  ret 
	then
		print("start wifi all success")
	else
		print("starat wifi all fail")
		print("usepage: wifi_start_all wifi_id ")
	end
end	

function wifi_restart(wifi_id)
	print("wifi_restart_wifi")
	local ret = wifi.wifi_restart(tonumber(arg[2]))

	if  ret 
	then
		print("restart wifi success")
	else
		print("restarat wifi  fail")
		print("usepage: wifi_restart wifi_id  ")
	end
end	

function wifi_restart_all()
	print("wifi_restart_all")
	local ret = wifi.wifi_restart_all()

	if  ret 
	then
		print("restart wifi all success")
	else
		print("restarat wifi all fail")
		print("usepage: wifi_restart_all  ")
	end
end	


function wifi_stop()
	print("wifi_stop")
	local ret = wifi.wifi_stop(tonumber(arg[2]))

	if  ret 
	then
		print("stop wifi success")
	else
		print("stop wifi fail")
		print("usepage: wifi_stop ")
	end
end	

function wifi_stop_all()
	print("wifi_stop_all")
	local ret = wifi.wifi_stop_all()

	if  ret 
	then
		print("stop wifi all success")
	else
		print("stop wifi all fail")
		print("usepage: wifi_stop_all ")
	end
end	


function wifi_get_connect_sta_list()
	print("wifi_get_connect_sta_list")
	local ret = wifi.wifi_get_connect_sta_list(tonumber(arg[2]))

	
	for key, value in pairs(ret)
	do
		
		for k,v in pairs(value)
		do
			print(k,"=",v)
		end
		
	end
end




function dhcp_get_server_ip()

	print("dhcp_get_server_ip")
	local ret = dhcp.dhcp_get_server_ip()

	if  nil ~= ret 
	then
		print("dhcp server ip = ", ret)
	else
		print("get  dhcp server ip fail")
		print("usepage:dhcp_get_server_ip")
	end
end


function dhcp_set_server_ip()
	local ret = dhcp.dhcp_set_server_ip(tostring(arg[2]))
	
	if ret
	then
		print("set server ip success")
	else
		print("set server ip fail")
	end
	
end


function dhcp_get_server_mask()

	print("dhcp_get_server_mask")
	local ret = dhcp.dhcp_get_server_mask()

	if  nil ~= ret 
	then
		print("dhcp server mask = ", ret)
	else
		print("get  dhcp server mask fail")
		print("usepage:dhcp_get_server_mask")
	end
end


function dhcp_set_server_mask()
	local ret = dhcp.dhcp_set_server_mask(tostring(arg[2]))
	
	if ret
	then
		print("set server mask success")
	else
		print("set server mask fail")
	end
	
end


function dhcp_get_ip_range()

	print("dhcp_get_ip_range")
	local start,endp = dhcp.dhcp_get_ip_range()

	if  nil ~= start 
	then
		print("dhcp ip range = ", start, "-", endp)
	else
		print("get  dhcp ip range fail")
		print("usepage:dhcp_get_ip_range")
	end
end


function dhcp_set_ip_range()
	local ret = dhcp.dhcp_set_ip_range(tostring(arg[2]), tonumber(arg[3]))
	
	if ret
	then
		print("set set ip range success")
	else
		print("set set ip range fail")
	end
end


function dhcp_get_lease_time()

	print("dhcp_get_lease_time")
	local lease_time = dhcp.dhcp_get_lease_time()

	if  nil ~= lease_time 
	then
		print("dhcp lease_time = ", lease_time, "hour")
	else
		print("get  lease_time fail")
		print("usepage:dhcp_get_lease_time")
	end
end


function dhcp_set_lease_time()
	local ret = dhcp.dhcp_set_lease_time(tonumber(arg[2]))
	
	if ret
	then
		print("set leasetime success")
	else
		print("set leasetime fail")
	end
end

function dhcp_get_client_list()
	print("dhcp_get_client_list")
	local ret = dhcp.dhcp_get_client_list()

	
	for key, value in pairs(ret)
	do
		
		for k,v in pairs(value)
		do
			print(k,"=",v)
		end
		
	end
end

local api_func = {
	["wifi_get_dev"]=wifi_get_dev,
	["wifi_get_hidden_ssid"]=wifi_get_hidden_ssid,
	["wifi_enable_hidden_ssid"]=wifi_enable_hidden_ssid,
	["wifi_disable_hidden_ssid"]=wifi_disable_hidden_ssid,
	["wifi_get_mode"]=wifi_get_mode,
	["wifi_set_mode"]=wifi_set_mode,
	["wifi_get_ht_mode"]=wifi_get_ht_mode,
	["wifi_set_ht_mode"]=wifi_set_ht_mode,
	["wifi_get_encryption"]=wifi_get_encryption,
	["wifi_set_encryption"]=wifi_set_encryption,
	["wifi_get_encryption_type"]=wifi_get_encryption_type,
	["wifi_set_encryption_type"]=wifi_set_encryption_type,
	["wifi_get_wmm"]=wifi_get_wmm,
	["wifi_enable_wmm"]=wifi_enable_wmm,
	["wifi_disable_wmm"]=wifi_disable_wmm,
	["wifi_get_ssid"]=wifi_get_ssid,
	["wifi_set_ssid"]=wifi_set_ssid,
	["wifi_get_password"]=wifi_get_password,
	["wifi_set_password"]=wifi_set_password,
	["wifi_get_txpower"]=wifi_get_txpower,
	["wifi_set_txpower"]=wifi_set_txpower,
	
	["wifi_get_channel"]=wifi_get_channel,
	["wifi_set_channel"]=wifi_set_channel,

	["wifi_start"]=wifi_start,
	["wifi_start_all"]=wifi_start_all,

	["wifi_restart"]=wifi_restart,
	["wifi_restart_all"]=wifi_restart_all,

	["wifi_stop"]=wifi_stop,
	["wifi_stop_all"]=wifi_stop_all,
	
	
	["wifi_get_connect_sta_list"]=wifi_get_connect_sta_list,

	--dhcp
	["dhcp_get_server_ip"]=dhcp_get_server_ip,
	["dhcp_set_server_ip"]=dhcp_set_server_ip,
	["dhcp_get_server_mask"]=dhcp_get_server_mask,
	["dhcp_set_server_mask"]=dhcp_set_server_mask,
	["dhcp_get_ip_range"]=dhcp_get_ip_range,
	["dhcp_set_ip_range"]=dhcp_set_ip_range,
	["dhcp_get_lease_time"]=dhcp_get_lease_time,
	["dhcp_set_lease_time"]=dhcp_set_lease_time,
	["dhcp_get_client_list"]=dhcp_get_client_list,
	["dhcp_start"]=dhcp.dhcp_start,
	["dhcp_stop"]=dhcp.dhcp_stop,
	["dhcp_restart"]=dhcp.dhcp_restart,
	["dhcp_enable"]=dhcp.dhcp_enable,
	["dhcp_disable"]=dhcp.dhcp_disable,
	["dhcp_reload"]=dhcp.dhcp_reload,

	--modem
	["modem_get_status"] = modem_get_status,
	["modem_get_info"] = modem_get_info,

	--network
	["network_get_wan_info"] = network_get_wan_info,
	["network_get_4g_net_info"] = network_get_4g_net_info,

	--sim
	["sim_get_status"] = sim_get_status,

	--system
	["system_get_status"] = system_get_status,

	--device
	["device_get_info"] = device_get_info,

	--firewall
	["firewall_set_mac_filter"] = firewall_set_mac_filter,
	["firewall_get_mac_filter"] = firewall_get_mac_filter,
	["firewall_set_url_filter"] = firewall_set_url_filter,
	["firewall_get_url_filter"] = firewall_get_url_filter,
	["firewall_set_ip_filter"] = firewall_set_ip_filter,
	["firewall_get_ip_filter"] = firewall_get_ip_filter,
	["firewall_get_port_filter"] = firewall_get_port_filter,
	["firewall_set_port_filter"] = firewall_set_port_filter,
	["firewall_get_port_redirect"] = firewall_get_port_redirect,
	["firewall_set_port_redirect"] = firewall_set_port_redirect,

	["firewall_get_default_action"] = firewall.firewall_get_default_action,
	["firewall_set_default_action"] = firewall_set_default_action,
	["firewall_start"] = firewall.firewall_start,
	["firewall_stop"] = firewall.firewall_stop,
	["firewall_restart"] = firewall.firewall_restart,
	["firewall_clear_all_rules"] = firewall.firewall_clear_all_user_rule 
}




for i,v in pairs(api_func)  do

        if type(v) == "function"  and arg[1] == i then
         	v()
         	return 0
        end
end


for i,v in pairs(api_func) do
	print(i)
end

