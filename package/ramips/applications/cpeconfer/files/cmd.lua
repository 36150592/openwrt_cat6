#!/usr/bin/lua

local wifi = require "tz.wifi"
local dhcp = require "tz.dhcp"
local modem = require "tz.modem"
local network = require "tz.network"
local sim = require "tz.sim"
local system = require "tz.system"
local device = require "tz.device"
local firewall = require "tz.firewall"
local led = require "tz.led"

function led_set_signal()

	if led.led_set_signal(tonumber(arg[2]))
	then
		print("set signal success")
	else
		print("set signal fail")
	end

end

function led_set_normal()
	if led.led_set_normal(arg[2])
	then
		print("led_set_normal success")
	else
		print("led_set_normal fail")
	end
end

function led_set_exception()
	if led.led_set_exception(arg[2])
	then
		print("led_set_exception success")
	else
		print("led_set_exception fail")
	end
end

function led_set_wifi()
	if led.led_set_wifi(arg[2])
	then
		print("led_set_wifi success")
	else
		print("led_set_wifi fail")
	end
end

function led_set_wifi_5g()
	if led.led_set_wifi_5g(arg[2])
	then
		print("led_set_wifi_5g success")
	else
		print("led_set_wifi_5g fail")
	end
end


function led_set_wps()
	if led.led_set_wps(arg[2])
	then
		print("led_set_wps success")
	else
		print("led_set_wps fail")
	end
end

function led_set_phone()
	if led.led_set_phone(arg[2])
	then
		print("led_set_phone success")
	else
		print("led_set_phone fail")
	end
end

function firewall_get_static_route()
	print("firewall_get_static_route")
	local list = firewall.firewall_get_static_route()

	if nil == list
	then
		print("empty")
		return
	end

	for k,v in pairs(list)
	do
		print("target_ip = ", v["target_ip"])
		print("target_netmask = ", v["target_netmask"])
		print("next_ip = ", v["next_ip"])
		print("target_interface = ", v["target_interface"])
	end


end

function firewall_set_static_route()
	print("firewall_set_static_route")
	local route_list = {
		{["target_ip"]="192.168.2.0" ,["target_netmask"]="255.255.255.0",["next_ip"]="192.168.2.1", ["target_interface"]="br-lan"},
		{["target_ip"]="192.168.3.0" ,["target_netmask"]="255.255.255.0",["next_ip"]="192.168.3.1", ["target_interface"]="br-lan1"},
		{["target_ip"]="192.168.4.0" ,["target_netmask"]="255.255.255.0",["next_ip"]="192.168.4.1", ["target_interface"]="br-lan2"},
	}
	local ret = firewall.firewall_set_static_route(route_list)

	print(ret)

end


function firewall_get_lan_network()
	local list = firewall.firewall_get_lan_network()

	if nil == list 
	then
		print("nil list")
		return
	end

	for k,v in pairs(list)
	do
		print(v)
	end

end

function firewall_set_mutil_nat()
	print("in firewall_set_mutil_nat")
	local mutil_nat = {arg[2], arg[3],arg[4]}
	local ret = firewall.firewall_set_mutil_nat(mutil_nat)

	if ret
	then
		print("set success")
	else
		print("set fail")
	end

end

function firewall_get_mutil_nat()
	print("in firewall_get_mutil_nat")
	local ret = firewall.firewall_get_mutil_nat(arg[2])

	if 1 == ret
	then
		print(arg[2] .. " nat on")
	elseif 0 == ret
	then
		print(arg[2] .. " nat off")
	else
		print("get error")
	end
end

function firewall_remote_get_web_login()
	print("in firewall_remote_get_web_login")
	local ret = firewall.firewall_remote_get_web_login()
	print(ret)
end

function firewall_remote_set_web_login()
	print("in firewall_remote_set_web_login")
	local ret = firewall.firewall_remote_set_web_login(tonumber(arg[2]))
	if ret
	then
		print("set success")
	else
		print("set fail")
	end
end

function firewall_remote_get_default_list()
	print("in firewall_remote_get_default_list")
	local iplist= firewall.firewall_remote_get_default_list()
	for k,v in pairs(iplist)
	do
		print(k, " = ", v)
	end

end

function firewall_remote_set_default_list()
	print("in firewall_remote_set_default_list")
	local iplist = {"192.168.2.9", "192.168.2.10", "192.168.2.11", "192.168.2.12"}
	local ret = firewall.firewall_remote_set_default_list(iplist)
	if ret
	then
		print("set success")
	else
		print("set fail")
	end
end


function firewall_remote_get_web_login_list()
	print("in firewall_remote_get_web_login_list")
	local iplist= firewall.firewall_remote_get_web_login_list()
	for k,v in pairs(iplist)
	do
		print(k, " = ", v)
	end

end

function firewall_remote_set_web_login_list()
	print("in firewall_remote_set_web_login_list")
	local iplist = {"192.168.2.9", "192.168.2.10", "192.168.2.11", "192.168.2.12"}
	local ret = firewall.firewall_remote_set_web_login_list(iplist)
	if ret
	then
		print("set success")
	else
		print("set fail")
	end
end

function firewall_remote_get_ping()
	print("in firewall_remote_get_ping")
	local ret = firewall.firewall_remote_get_ping()
	print(ret)
end

function firewall_remote_set_ping()
	print("in firewall_remote_set_ping")
	local ret = firewall.firewall_remote_set_ping(tonumber(arg[2]))
	if ret
	then
		print("set success")
	else
		print("set fail")
	end
end

function firewall_remote_get_ping_list()
	print("in firewall_remote_get_ping_list")
	local iplist= firewall.firewall_remote_get_ping_list()
	for k,v in pairs(iplist)
	do
		print(k, " = ", v)
	end

end

function firewall_remote_set_ping_list()
	print("in firewall_remote_set_ping_list")
	local iplist = {"192.168.2.9", "192.168.2.10", "192.168.2.11", "192.168.2.12"}
	local ret = firewall.firewall_remote_set_ping_list(iplist)
	if ret
	then
		print("set success")
	else
		print("set fail")
	end
end


function firewall_set_default_action()

	firewall.firewall_set_default_action(arg[2])
end


function firewall_get_default_action()
	print("get default firewall action")

	local ret = firewall.firewall_get_default_action()
	print(ret)

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
		{["dest_addr"]=nil, ["dest_port"]="90", ["protocol"]='tcp', ["redirect_addr"]='192.168.2.1',["redirect_port"]='8080', ["comment"]="test port redirect1",["iswork"]=true},
		{["dest_addr"]='192.168.2.131', ["dest_port"]="80", ["dest_port_end"]="89",["protocol"]='udp', ["redirect_addr"]='192.168.2.1',["redirect_port"]='8090', ["comment"]="test port redirect2",["iswork"]=true},
		{["dest_addr"]='192.168.2.131', ["dest_port"]="22", ["protocol"]='all', ["redirect_addr"]='192.168.2.1',["redirect_port"]='80', ["comment"]="test port redirect3",["iswork"]=true},
		{["dest_addr"]='192.168.2.131', ["dest_port"]="8899", ["protocol"]='all', ["redirect_addr"]='192.168.2.1',["redirect_port"]='8896', ["comment"]="test port redirect3",["iswork"]=false}
		
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
		{["port"]='4545',["protocol"]='tcp', ["action"]='ACCEPT', ["comment"]="test port filter1",["iswork"]=true},
		{["port"]='1902',["protocol"]='udp', ["action"]='DROP', ["comment"]="test port filter2",["iswork"]=true},
		{["port"]='907',["protocol"]='all',["action"]='DROP', ["comment"]="test port filter3",["iswork"]=true},
		{["port"]='523',["protocol"]='all',["action"]='DROP', ["comment"]="test port filter3",["iswork"]=false},
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
		{["protocol"] = 'tcp', ["ipaddr"]='192.168.3.6', ["action"]='ACCEPT', ["comment"]="test ip filter",["iswork"]=true},
		{["protocol"] = 'all', ["ipaddr"]='192.168.4.6', ["action"]='DROP', ["comment"]="test ip filter",["iswork"]=true},
		{["protocol"] = 'udp', ["ipaddr"]='192.168.4.8', ["action"]='DROP', ["comment"]="test ip filter",["iswork"]=true},
		{["protocol"] = 'tcp', ["ipaddr"]='192.168.4.9', ["action"]='DROP', ["comment"]="test ip filter",["iswork"]=false},
		}

		if firewall.firewall_set_ip_filter_list(ips)
		then
			print("set success")
		else
			print("set fail")
		end
end

function firewall_get_acl_filter()

	print("in firewall_get_acl_filter")
	local info = firewall.firewall_get_acl_filter_list()

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

function firewall_set_acl_filter()
		print("in firewall_set_ip_filter")
		local ips={
		{["protocol"] = 'tcp', ["src_ipaddr"]='192.168.3.6', ["dest_ipaddr"]='192.168.3.99', ["action"]='ACCEPT', ["comment"]="test acl filter",["iswork"]=true},
		{["protocol"] = 'all', ["src_ipaddr"]='192.168.3.6', ["dest_ipaddr"]='192.168.3.88', ["action"]='DROP', ["comment"]="test acl filter",["iswork"]=true},
		{["protocol"] = 'udp', ["src_ipaddr"]='192.168.3.6', ["dest_ipaddr"]='192.168.3.77', ["action"]='DROP', ["comment"]="test acl filter",["iswork"]=true},
		{["protocol"] = 'tcp', ["src_ipaddr"]='192.168.3.6', ["dest_ipaddr"]='192.168.3.66', ["action"]='DROP', ["comment"]="test acl filter",["iswork"]=false},
		}

		if firewall.firewall_set_acl_filter_list(ips)
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
		{["url"]='sina.cn', ["action"]='ACCEPT', ["comment"]=nil, ["interface"]=nil, ["iswork"]=true},
		{["url"]='baidu.com', ["action"]='DROP', ["comment"]="", ["interface"]="lan", ["iswork"]=true},
		{["url"]='cn.bing.com', ["action"]='DROP', ["comment"]="test url filter", ["interface"]="", ["iswork"]=false},
		}

		if firewall.firewall_set_url_filter_list(urls)
		then
			print("set success")
		else
			print("set fail")
		end
end

function firewall_get_speed_filter()

	print("in firewall_get_speed_filter")
	local info = firewall.firewall_get_speed_filter_list()

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



function firewall_set_speed_filter()
		print("in firewall_set_speed_filter")
		local macs={
		{["speed"]=11, ["ipaddr"]='192.168.2.158', ["comment"]="test speed filter",["iswork"]=true},
		{["speed"]=90, ["ipaddr"]='192.168.2.159', ["comment"]="test speed 2 filter",["iswork"]=true},
		{["speed"]=1200, ["ipaddr"]='192.168.2.150', ["comment"]="test spped 3 filter",["iswork"]=false},
		}

		if firewall.firewall_set_speed_filter_list(macs)
		then
			print("set success")
		else
			print("set fail")
		end
end


function firewall_get_ipmac_bind_filter()

	print("in firewall_get_ipmac_bind_filter")
	local info = firewall.firewall_get_ipmac_bind_filter_list()

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



function firewall_set_ipmac_bind_filter()
		print("in firewall_set_ipmac_bind_filter")
		local macs={
		{["mac"]='aa:bb:cc:11:22:33', ["ipaddr"]='192.168.2.158', ["comment"]="test ip mac bind filter",["iswork"]=true},
		{["mac"]='aa:bb:cc:11:22:34', ["ipaddr"]='192.168.2.159', ["comment"]="test ip mac bind filter filter",["iswork"]=true},
		{["mac"]='aa:bb:cc:11:22:44', ["ipaddr"]='192.168.2.150', ["comment"]="test ip mac bind filter",["iswork"]=false},
		}

		if firewall.firewall_set_ipmac_bind_filter_list(macs)
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
		{["mac"]='aa:bb:cc:11:22:33', ["action"]='DROP', ["comment"]="test mac filter",["iswork"]=true},
		{["mac"]='aa:bb:cc:11:22:34', ["action"]='DROP', ["comment"]="test mac filter",["iswork"]=true},
		{["mac"]='aa:bb:cc:11:22:44', ["action"]='DROP', ["comment"]="test mac filter",["iswork"]=false},
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

function system_lanrecord_get_current_info()
	local list = system.system_lanrecord_get_current_info()

	if nil ~= list
	then
		for key,value in pairs(list)
		do
			for k,v in pairs(value)
			do
				print(k .. " = " .. v)

			end

			print("<------------------------------>")
		end
	end

end



function system_lanrecord_get_history_info()
	local list = system.system_lanrecord_get_history_info()

	if nil ~= list
	then
		for key,value in pairs(list)
		do
			for k,v in pairs(value)
			do
				print(k .. " = " .. v)

			end

			print("<------------------------------>")
		end
	end

end


function system_ntp_enable()
	local ret = system.system_ntp_enable()

	if ret
	then
		print("enable success")
	else
		print("enable fail")
	end

end

function system_ntp_disable()
	local ret = system.system_ntp_disable()

	if ret
	then
		print("disable success")
	else
		print("disable fail")
	end

end

function system_ntp_get_enable_status()
	local ret = system.system_ntp_get_enable_status()

	if ret == "1"
	then
		print("ntp is enable")
	else
		print("ntp is disable")
	end

end


function system_ntp_get_timezone()
	local timezone = system.system_ntp_get_timezone()

	print("time zone = ", timezone)
end

function system_ntp_set_timezone()
	local ret = system.system_ntp_set_timezone(tonumber(arg[2]))

	if ret
	then
		print("set success")
	else
		print("set fail")
	end
end

function system_ntp_get_server_address()
	local server_list = system.system_ntp_get_server_address()

	for key,value in pairs(server_list)
	do
		print(value)
	end
end

function system_ntp_set_server_address()
	local server_list =    {"0.openwrt.pool.ntp.org",
  "1.openwrt.pool.ntp.org",
  "2.openwrt.pool.ntp.org",
  "3.openwrt.pool.ntp.org"}

	local ret = system.system_ntp_set_server_address(server_list)

	if ret
	then
		print("set success")
	else
		print("set fail")
	end
end

function system_ntp_set_date()
	local ret = system.system_ntp_set_date(arg[2])

	if ret
	then
		print("set success")
	else
		print("set fail")
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

function system_network_tool()

	print("in system_network_tool")
	--local req = {["pingUrl"]="www.baidu.com",["pingNum"]="50",["tool"]="ping_start"}
	--local req = {["pingUrl"]="www.baidu.com",["pingNum"]="50",["tool"]="ping_stop"}
	local req = {["catchPackageIfname"]="eth0",["tool"]="catch_pkg_start"}
	--local req = {["catchPackageIfname"]="eth0",["tool"]="catch_pkg_stop"}
	--local req = {["catchPackageIfname"]="eth0",["tool"]="get_size"}
	local info = system.system_network_tool(req)

	if nil == info 
	then
		print("info is nil")
		return
	end


	for k,v in pairs(info) do 
			print(k,"=", v)
	end

end

function sim_reload()
	print("in sim_reload")
	local ret = sim.sim_reload()
	if ret 
	then	
		print("reload success")
	else
		print("reload fail")
	end
end

function sim_pin_set_remember()

	local ret = sim.sim_pin_set_remember(arg[2],arg[3])

	print(ret)
end

function sim_pin_get_remember()

	local enable,passwd = sim.sim_pin_get_remember()

	print("enable = ",enable)
	print("pin_password = ", passwd)
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

function sim_pin_lock_enable()
	print("in sim_pin_lock_enable")
	local ret = sim.sim_pin_lock_enable(arg[2])

	if ret
	then
		print("enable pin lock success")
		return 
	end

	print("enable pin lock fail")
end

function sim_pin_lock_disable()
	print("in sim_pin_lock_disable")
	local ret = sim.sim_pin_lock_disable(arg[2])

	if ret
	then
		print("disable pin lock success")
		return 
	end

	print("disable pin lock fail")
end

function sim_pin_unlock()
	print("in sim_pin_unlock")
	local ret = sim.sim_pin_unlock(tostring(arg[2]))

	if ret
	then
		print(" pin unlock success")
		return 
	end

	print("pin unlock fail")
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

function network_get_4g1_net_info()

	print("in network_get_4g1_net_info")
	local info = network.network_get_4g1_net_info()

	if nil == info 
	then
		print("info is nil")
		return
	end

	for k,v in pairs(info) do 
			print(k,"=", v)
	end

end

function network_get_4g2_net_info()

	print("in network_get_4g2_net_info")
	local info = network.network_get_4g2_net_info()

	if nil == info 
	then
		print("info is nil")
		return
	end

	for k,v in pairs(info) do 
			print(k,"=", v)
	end

end

function modem_reload_config()
	print("in modem_reload_config")
	local ret = modem.modem_reload_config()

	if ret 
	then
		print("reload success")
	else 
		print("reload fail")
	end
end

function modem_enable_auto_dial()
	local ret = modem.modem_enable_auto_dial()
	print(ret)
end

function modem_disable_auto_dial()
	local ret = modem.modem_disable_auto_dial()
	print(ret)
end

function modem_get_auto_dial()
	local ret = modem.modem_get_auto_dial()
	print(ret)
end

function modem_get_mutilapn_status()

	local ret = modem.modem_get_mutilapn_status()

	if nil == ret
	then
		print("error ,got nil")
	end

	for k,v in pairs(ret)
	do
		for key,value in pairs(v)
		do

			print(key,"=",value)
		end

		print("<------------------------>")
	end

end 

function modem_get_mutilapn_config()

	local ret = modem.modem_get_mutilapn_config()

	for k,v in pairs(ret)
	do
		for key,value in pairs(v)
		do

			print(key,"=",value)
		end

		print("<------------------------>")
	end

end

function modem_set_mutilapn_config()

	local list = {
{["profile_name"]="main_apn",["apn_name"]="",["username"]="aaa",["password"]="bbb",["ip_stack"]="IP",["auth_type"]=3,["mtu"]=1500,["enable"]=1},
{["profile_name"]="apn1",["apn_name"]="ctlte",["username"]="aaa",["password"]="bbb",["ip_stack"]="IP",["auth_type"]=3,["mtu"]=1500,["enable"]=1},
{["profile_name"]="apn2",["apn_name"]="ctnet",["username"]="aaa",["password"]="bbb",["ip_stack"]="IP",["auth_type"]=3,["mtu"]=1500,["enable"]=1},

}

	local ret =  modem.modem_set_mutilapn_config(list)

	print(ret)
end



function modem_get_supprot_band()
	local gw,lte,tds = modem.modem_get_support_band()

	print("GW:")
	for k,v in pairs(gw)
	do
		print(v)
	end

	print("LTE:")
	for k,v in pairs(lte)
	do
		print(v)
	end

	print("TDS:")
	for k,v in pairs(tds)
	do
		print(v)
	end

end


function modem_set_lte_lock_cell()

	print("in modem_set_lte_lock_cell")
	local ret = modem.modem_set_lte_lock_cell(tonumber(arg[2]), tonumber(arg[3]))

	if ret 
	then
		print("set success")
	else 
		print("set fail")
	end

end

function modem_get_lte_lock_cell()

	print("in modem_get_lte_lock_cell")
	local pci,earfcn = modem.modem_get_lte_lock_cell()

	if nil == pci or nil == earfcn
	then
		print("lte locl cell is disable")
	else
		print("lock pci = ", pci)
		print("lock earfcn = ", earfcn)
	end
end


function modem_set_lock_operator()

	print("in modem_set_lock_operator")
	local operator_list = {"46011","46002", "46003"}
	local ret = modem.modem_set_lock_operator(operator_list)

	if ret 
	then
		print("set success")
	else 
		print("set fail")
	end

end

function modem_get_lock_operator()

	print("in modem_get_lock_operator")
	local list = modem.modem_get_lock_operator()

	if nil == list
	then
		print("not lock any operator")
		return 
	end

	for k,v in pairs(list)
	do
		print(v)
	end

end

function modem_set_network_mode()

	print("in modem_set_network_mode")
	local ret = modem.modem_set_network_mode(tonumber(arg[2]))

	if ret 
	then
		print("set success")
	else 
		print("set fail")
	end

end

function modem_get_network_mode()

	print("in modem_get_network_mode")
	local ret = modem.modem_get_network_mode()

	print("network mode = ", ret)
end

function modem_set_lock_band()
	print("in modem_set_lock_band")

	local band_list = {"EUTRAN_BAND40", "EUTRAN_BAND41", "EUTRAN_BAND42"}
	local ret = modem.modem_set_lock_band(band_list)

	if ret 
	then
		print("set success")
	else 
		print("set fail")
	end
end

function modem_get_lock_band()
	print("in modem_get_lock_band")

	local band_list = modem.modem_get_lock_band()

	for k, v in pairs(band_list)
	do
		print(v)
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

function wifi_set_mac_access_control()

	--local macs = {"aa:bb:cc:dd:ee:11","aa:bb:cc:dd:ee:12","aa:bb:cc:dd:ee:13"}
	local macs = {}
	local ret = wifi.wifi_set_mac_access_control(tonumber(arg[2]), tonumber(arg[3]), macs)
	print(ret)
end


function wifi_get_mac_access_control()

	local policy, macs = wifi.wifi_get_mac_access_control(tonumber(arg[2]))

	print("policy = ", policy)

	for k,v  in pairs(macs)
	do
		print(v)
	end
end

function wifi_secondary_get_ssid_list()
	print("in wifi_secondary_get_ssid_list")
	local array = wifi.wifi_secondary_get_ssid_list()
	for i = 1,table.maxn(array) do 
		temp = array[i]
		
		print('<------------',i,'------>')
		for k,v in pairs(temp) do
			print(k," = ", v)
		end

	end

end

function wifi_secondary_get_ssid()
	print("wifi_get_ssid")
	local ret = wifi.wifi_secondary_get_ssid(tonumber(arg[2]))

	if  nil ~= ret 
	then
		print("wifi ssid = ", ret)
	else
		print("get ssid fail")
	end
end

function wifi_secondary_set_ssid()
	print("wifi_set_ssid")
	local ret = wifi.wifi_secondary_set_ssid(tonumber(arg[2]), arg[3])

	if  ret 
	then
		print("set wifi ssid success")
	else
		print("set ssid fail")
	end
end


function wifi_secondary_get_password()
	print("wifi_get_password")
	local ret = wifi.wifi_secondary_get_password(tonumber(arg[2]))

	if  nil ~= ret 
	then
		print("wifi password = ", ret)
	else
		print("get password  fail")
	end
end

function wifi_secondary_set_password()
	print("wifi_set_password")
	print(type(arg[3]))
	local ret = wifi.wifi_secondary_set_password(tonumber(arg[2]), arg[3])

	if  ret 
	then
		print("set wifi password success")
	else
		print("set password fail")
	end
end

function wifi_secondary_get_hidden()
	print("wifi_get_hidden_ssid")
	local hidden = wifi.wifi_secondary_get_hidden(tonumber(arg[2]))
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

function wifi_secondary_enable_hidden()
	print("wifi_enable_hidden_ssid")
	local ret = wifi.wifi_secondary_enable_hidden(tonumber(arg[2]))

	if  ret 
	then
		print("enable hidden ssid success")
	else
		print("enable hidden ssid fail")
	end
end

function wifi_secondary_disable_hidden()
	print("wifi_disable_hidden_ssid")
	local ret = wifi.wifi_secondary_disable_hidden(tonumber(arg[2]))

	if  ret 
	then
		print("disable hidden ssid success")
	else
		print("disable hidden ssid fail")
	end
end

function wifi_secondary_get_encryption()
	print("wifi_secondary_get_encryption")
	local ret = wifi.wifi_secondary_get_encryption(tonumber(arg[2]))

	if  nil ~= ret 
	then
		print("encryption = ", ret)
	else
		print("get encryption fail")
	end
end

function wifi_secondary_set_encryption()
	print("wifi_secondary_set_encryption")
	local ret = wifi.wifi_secondary_set_encryption(tonumber(arg[2]), tostring(arg[3]))

	if  ret 
	then
		print("set encryption success")
	else
		print("set encryption fail")
		print("usepage: wifi_set_encryption wifi_id  wep|psk|psk2|miexed|none")
	end
end

function wifi_secondary_get_encryption_type()
	print("wifi_secondary_get_encryption_type")
	local ret = wifi.wifi_secondary_get_encryption_type(tonumber(arg[2]))

	if  nil ~= ret 
	then
		print("encryption type = ", ret)
	else
		print("get encryption type fail")
		print("usepage:wifi_get_encryption_type wifi_id")
	end
end

function wifi_secondary_set_encryption_type()
	print("wifi_secondary_set_encryption_type")
	local ret = wifi.wifi_secondary_set_encryption_type(tonumber(arg[2]), tostring(arg[3]))

	if  ret 
	then
		print("set encryption type success")
	else
		print("set encryption type fail")
		print("usepage: wifi_set_encryption_type wifi_id ccmp+tkip|ccmp|tkip|none")
	end
end

function wifi_secondary_get_connect_sta_number()
	print("wifi_secondary_get_connect_sta_number")
	local ret = wifi.wifi_secondary_get_connect_sta_num(tonumber(arg[2]))

	if  ret 
	then
		print("the max connect sta number  = ",ret)
	else
		print("get fail")
		print("usepage: wifi_secondary_get_connect_sta_number wifi_secondary_id")
	end

end

function wifi_secondary_set_connect_sta_number()
	print("wifi_secondary_set_connect_sta_number")
	local ret = wifi.wifi_secondary_set_connect_sta_num(tonumber(arg[2]), tonumber(arg[3]))

	if  ret 
	then
		print("set success ")
	else
		print("get fail")
		print("usepage: wifi_secondary_set_connect_sta_number wifi_secondary_id num")
	end

end

function wifi_secondary_get_wmm()
	print("wifi_secondary_get_wmm")
	local ret = wifi.wifi_secondary_get_wmm(tonumber(arg[2]))

	if  nil ~= ret 
	then
		print("wmm = ", ret)
	else
		print("get encryption type fail")
		print("usepage:wifi_get_wmm wifi_id")
	end
end

function wifi_secondary_enable_wmm()
	print("wifi_secondary_enable_wmm")
	local ret = wifi.wifi_secondary_enable_wmm(tonumber(arg[2]))

	if  ret 
	then
		print("enable wmm success")
	else
		print("set wmm fail")
		print("usepage: wifi_enable_wmm wifi_id ")
	end
end

function wifi_secondary_disable_wmm()
	print("wifi_secondary_disable_wmm")
	local ret = wifi.wifi_secondary_disable_wmm(tonumber(arg[2]))

	if  ret 
	then
		print("disable wmm success")
	else
		print("set wmm fail")
		print("usepage: wifi_disable_wmm wifi_id ")
	end
end		

function wifi_secondary_enable()
	print("wifi_secondary_enable")
	local ret = wifi.wifi_secondary_enable(tonumber(arg[2]))

	if  ret 
	then
		print("wifi enable succes")
	else
		print("wifi enable fail")
	end
end

function wifi_secondary_disable()
	print("wifi_secondary_disable")
	local ret = wifi.wifi_secondary_disable(tonumber(arg[2]))

	if  ret 
	then
		print("wifi_disable succes")
	else
		print("wifi_disable fail")
	end
end

function wifi_secondary_get_enable_status()
	print("wifi_secondary_get_enable_status")
	local ret = wifi.wifi_secondary_get_enable_status(tonumber(arg[2]))

	if  "0" == ret
	then
		print("wifi is  enable")
	else
		print("wifi is disable")
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


function wifi_get_bandwidth()
	print("wifi_get_bandwidth")
	local ret = wifi.wifi_get_bandwidth(tonumber(arg[2]))

	if  nil ~= ret 
	then
		print("bandwidth = ", ret)
	else
		print("wifi_get_bandwidth fail")
	end
end

function wifi_set_bandwidth()
	print("wifi_set_bandwidth")
	local ret = wifi.wifi_set_bandwidth(tonumber(arg[2]), tostring(arg[3]))

	if  ret 
	then
		print("wifi_set_bandwidth success")
	else
		print("wifi_set_bandwidth fail")
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

function wifi_is_start()
	print("wifi_is_start")
	local ret = wifi.wifi_is_start(tonumber(arg[2]))

	if  ret 
	then
		print("wifi is started")
	else
		print("wifi is stoped")
	end
end

function wifi_enable()
	print("wifi_enable")
	local ret = wifi.wifi_enable(tonumber(arg[2]))

	if  ret 
	then
		print("wifi enable succes")
	else
		print("wifi enable fail")
	end
end

function wifi_disable()
	print("wifi_disable")
	local ret = wifi.wifi_disable(tonumber(arg[2]))

	if  ret 
	then
		print("wifi_disable succes")
	else
		print("wifi_disable fail")
	end
end

function wifi_get_enable_status()
	print("wifi_get_enable_status")
	local ret = wifi.wifi_get_enable_status(tonumber(arg[2]))

	if  "0" == ret
	then
		print("wifi is  enable")
	else
		print("wifi is disable")
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
		print("<-------------------------------->")
	end
end

function wifi_get_connect_sta_by_dev()
	print("wifi_get_connect_sta_by_dev")
	local ret = wifi.wifi_get_connect_sta_by_dev(tostring(arg[2]))
	print("wifi_get_connect_sta_by_dev")
	
	for key, value in pairs(ret)
	do
		
		for k,v in pairs(value)
		do
			print(k,"=",v)
		end
		print("<-------------------------------->")
	end
end

function wifi_get_connect_sta_number()
	print("wifi_get_connect_sta_number")
	local ret = wifi.wifi_get_connect_sta_number(tonumber(arg[2]))

	if  nil ~= ret 
	then
		print("connect sta number = ", ret)
	else
		print("get connect sta number fail")
		print("usepage:wifi_get_connect_sta_number wifi_id")
	end

end

function wifi_set_connect_sta_number()
	print("wifi_set_connect_sta_number")
	local ret = wifi.wifi_set_connect_sta_number(tonumber(arg[2]),tonumber(arg[3]))

	if   ret 
	then
		print("set connect sta number success")
	else
		print("set connect sta number fail")
		print("usepage:wifi_set_connect_sta_number wifi_id  number")
	end

end

function wifi_set_wps_pin()
	print("in wifi_set_wps_pin")
	local ret = wifi.wifi_set_wps_pin(tonumber(arg[2]), tonumber(arg[3]))

	print(ret)
end

function wifi_get_wps_pin()
	print("in wifi_get_wps_pin")
	local ret = wifi.wifi_get_wps_pin(tonumber(arg[2]))

	print(ret)
end

function wifi_enable_wps_pbc()

	local ret = wifi.wifi_enable_wps_pbc(tonumber(arg[2]))
	print(ret)
end

function wifi_disable_wps_pbc()

	local ret = wifi.wifi_disable_wps_pbc(tonumber(arg[2]))
	print(ret)
end

function wifi_get_wps_pbc()
	print("in wifi_get_wps_pbc")
	local ret = wifi.wifi_get_wps_pbc_enable_status(tonumber(arg[2]))
	if 1 == ret 
	then
		print("enable")
	else
		print("disable")
	end
	
end

function dhcp_get_reserve_ip()
	print("in dhcp_get_reserve_ip")
	local list = dhcp.dhcp_get_reserve_ip()

	print(table.maxn(list))
	for k,v in pairs(list)
	do
		print("ip = ", v["ip"])
		print("mac = ", v["mac"])
		--print("leasetime = ", v["leasetime"])

		print("-------------------")
	end

end

function dhcp_set_reserve_ip()
	print("in dhcp_set_reserve_ip")

	local list = {
		{["ip"]="192.168.2.6",["mac"]="11:aa:99:22:88:bb"},
		{["ip"]="192.168.2.8",["mac"]="11:aa:99:22:88:bc"},
		{["ip"]="192.168.2.9",["mac"]="11:aa:99:22:88:bd"},
		{["ip"]="192.168.4.6",["mac"]="11:aa:99:22:81:bb"},
		{["ip"]="192.168.4.8",["mac"]="11:aa:99:22:82:bc"},
		{["ip"]="192.168.4.9",["mac"]="11:aa:99:22:83:bd"},
	}

	local ret = dhcp.dhcp_set_reserve_ip(list)

	print(ret)
end

function dhcp_config_enable()
	local ret = dhcp.dhcp_config_enable(arg[2])
	print(ret)
end

function dhcp_config_disable()
	local ret = dhcp.dhcp_config_disable(arg[2])
	print(ret)
end

function dhcp_get_object()

	local temp = dhcp.dhcp_get_object_by_network(arg[2])


	for key,value in pairs(temp)
	do
		print("key = ", key, "value = ", value)

	end

end

function dhcp_get_list()

	local list = dhcp.dhcp_get_object_list()

	for k, v in pairs(list)
	do

		for key,value in pairs(v)
		do
			print("key = ", key, "value = ", value)

		end

		print("-------------")
	end


end

function dhcp_get_enable_status()

	print("dhcp_get_enable_status")
	local ret = dhcp.dhcp_get_enable_status(arg[2])

	if  ret 
	then
		print("dhcp server is enable")
	else
		print("dhcp server is disable")
	end
end


function dhcp_get_server_ip()

	print("dhcp_get_server_ip")
	local ret = dhcp.dhcp_get_server_ip(arg[2])

	if  nil ~= ret 
	then
		print("dhcp server ip = ", ret)
	else
		print("get  dhcp server ip fail")
		print("usepage:dhcp_get_server_ip dhcp_name")
	end
end


function dhcp_set_server_ip()
	local ret = dhcp.dhcp_set_server_ip(arg[2],tostring(arg[3]))
	
	if ret
	then
		print("set server ip success")
	else
		print("set server ip fail")
		print("usepage:dhcp_set_server_ip dhcp_name server_ip")
	end
	
end


function dhcp_get_server_mask()

	print("dhcp_get_server_mask")
	local ret = dhcp.dhcp_get_server_mask(arg[2])

	if  nil ~= ret 
	then
		print("dhcp server mask = ", ret)
	else
		print("get  dhcp server mask fail")
		print("usepage:dhcp_get_server_mask dhcp_name")
	end
end


function dhcp_set_server_mask()
	local ret = dhcp.dhcp_set_server_mask(arg[2],tostring(arg[3]))
	
	if ret
	then
		print("set server mask success")
	else
		print("set server mask fail")
		print("usepage:dhcp_set_server_mask dhcp_name netmask")
	end
	
end


function dhcp_get_ip_range()

	print("dhcp_get_ip_range")
	local start,endp = dhcp.dhcp_get_ip_range(arg[2])

	if  nil ~= start 
	then
		print("dhcp ip range = ", start, "-", endp)
	else
		print("get  dhcp ip range fail")
		print("usepage:dhcp_get_ip_range dhcp_name")
	end
end


function dhcp_set_ip_range()
	local ret = dhcp.dhcp_set_ip_range(arg[2], tostring(arg[3]), tonumber(arg[4]))
	
	if ret
	then
		print("set set ip range success")
	else
		print("set set ip range fail")
		print("usepage:dhcp_set_ip_range dhcp_name start_ip limit")
	end
end


function dhcp_get_lease_time()

	print("dhcp_get_lease_time")
	local lease_time = dhcp.dhcp_get_lease_time(arg[2])

	if  nil ~= lease_time 
	then
		print("dhcp lease_time = ", lease_time, "hour")
	else
		print("get  lease_time fail")
		print("usepage:dhcp_get_lease_time dhcp_name")
	end
end


function dhcp_set_lease_time()
	local ret = dhcp.dhcp_set_lease_time(arg[2], tonumber(arg[3]))
	
	if ret
	then
		print("set leasetime success")
	else
		print("set leasetime fail")
		print("usepage:dhcp_set_lease_time dhcp_name leasetime")
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

function system_import_config()

	local ret = system.system_import_config(arg[2])

	print(ret)
end

function system_export_config()
	local ret = system.system_export_config(arg[2])

	print(ret)

end

function tozed_get_tr069_config()
	print("tozed_get_tr069_config")
	local ret = system.system_get_tr069_info()

	for key, value in pairs(ret)
	do
		print(key,"=",value)
	end
end

function tozed_set_tr069_config()
	print("tozed_get_tr069_config")

	system.system_set_tr069_app_enable("n")
	system.system_set_tr069_ServerURL("http://113.98.195.202:7547/")
	system.system_set_tr069_PeriodicInformEnable("y")
	system.system_set_tr069_PeriodicInformInterval("1800")
	system.system_set_tr069_tr069_ACS_auth("n")
	system.system_set_tr069_ServerUsername("acs_user")
	system.system_set_tr069_ServerPassword("acs_pwd")
	system.system_set_tr069_tr069_tr069_CPE_auth("n")
	system.system_set_tr069_ConnectionRequestUname("cpe_user")
	system.system_set_tr069_ConnectionRequestPassword("cpe_pwd")

end

local api_func = {
	["wifi_secondary_get_ssid_list"] = wifi_secondary_get_ssid_list,
	["wifi_secondary_get_hidden"]=wifi_secondary_get_hidden,
	["wifi_secondary_enable_hidden"]=wifi_secondary_enable_hidden,
	["wifi_secondary_disable_hidden"]=wifi_secondary_disable_hidden,
	["wifi_secondary_get_encryption"]=wifi_secondary_get_encryption,
	["wifi_secondary_set_encryption"]=wifi_secondary_set_encryption,
	["wifi_secondary_get_encryption_type"]=wifi_secondary_get_encryption_type,
	["wifi_secondary_set_encryption_type"]=wifi_secondary_set_encryption_type,
	["wifi_secondary_get_wmm"]=wifi_secondary_get_wmm,
	["wifi_secondary_enable_wmm"]=wifi_secondary_enable_wmm,
	["wifi_secondary_disable_wmm"]=wifi_secondary_disable_wmm,
	["wifi_secondary_get_ssid"]=wifi_secondary_get_ssid,
	["wifi_secondary_set_ssid"]=wifi_secondary_set_ssid,
	["wifi_secondary_get_password"]=wifi_secondary_get_password,
	["wifi_secondary_set_password"]=wifi_secondary_set_password,
	["wifi_secondary_get_connect_sta_number"] = wifi_secondary_get_connect_sta_number,
	["wifi_secondary_set_connect_sta_number"] = wifi_secondary_set_connect_sta_number,
	["wifi_secondary_enable"]=wifi_secondary_enable,
	["wifi_secondary_disable"]=wifi_secondary_disable,
	["wifi_secondary_get_enable_status"]=wifi_secondary_get_enable_status,


	["wifi_get_dev"]=wifi_get_dev,
	["wifi_get_hidden_ssid"]=wifi_get_hidden_ssid,
	["wifi_enable_hidden_ssid"]=wifi_enable_hidden_ssid,
	["wifi_disable_hidden_ssid"]=wifi_disable_hidden_ssid,
	["wifi_get_mode"]=wifi_get_mode,
	["wifi_set_mode"]=wifi_set_mode,
	["wifi_get_bandwidth"]=wifi_get_bandwidth,
	["wifi_set_bandwidth"]=wifi_set_bandwidth,
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
	["wifi_is_start"]=wifi_is_start,
	["wifi_enable"]=wifi_enable,
	["wifi_disable"]=wifi_disable,
	["wifi_get_enable_status"]=wifi_get_enable_status,
	["wifi_get_connect_sta_number"] = wifi_get_connect_sta_number,
	["wifi_set_connect_sta_number"] = wifi_set_connect_sta_number,
	["wifi_get_connect_sta_list"]=wifi_get_connect_sta_list,
	["wifi_get_connect_sta_by_dev"]=wifi_get_connect_sta_by_dev,
	["wifi_get_mac_access_control"] = wifi_get_mac_access_control,
	["wifi_set_mac_access_control"] = wifi_set_mac_access_control,
	["wifi_get_wps_pin"] = wifi_get_wps_pin,
	["wifi_set_wps_pin"] = wifi_set_wps_pin,
	["wifi_get_wps_pbc"] = wifi_get_wps_pbc,
	["wifi_enable_wps_pbc"] = wifi_enable_wps_pbc,
	["wifi_disable_wps_pbc"] = wifi_disable_wps_pbc,

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
	["dhcp_get_enable_status"]=dhcp_get_enable_status,
	["dhcp_get_list"] = dhcp_get_list,
	["dhcp_get_object"] = dhcp_get_object,
	["dhcp_config_disable"] = dhcp_config_disable,
	["dhcp_config_enable"] = dhcp_config_enable,
	["dhcp_set_reserve_ip"] = dhcp_set_reserve_ip,
	["dhcp_get_reserve_ip"] = dhcp_get_reserve_ip,

	--modem
	["modem_get_status"] = modem_get_status,
	["modem_get_info"] = modem_get_info,
	["modem_get_network_mode"] = modem_get_network_mode,
	["modem_set_network_mode"] = modem_set_network_mode,
	["modem_get_lock_band"] = modem_get_lock_band,
	["modem_set_lock_band"] = modem_set_lock_band,
	["modem_set_lock_operator"] = modem_set_lock_operator,
	["modem_get_lock_operator"] = modem_get_lock_operator,
	["modem_reload_config"] = modem_reload_config,
	["modem_set_lte_lock_cell"] = modem_set_lte_lock_cell,
	["modem_get_lte_lock_cell"] = modem_get_lte_lock_cell,
	["modem_get_support_band"] = modem_get_supprot_band,
	["modem_get_mutilapn_config"] = modem_get_mutilapn_config,
	["modem_set_mutilapn_config"] = modem_set_mutilapn_config,
	["modem_get_mutilapn_status"] = modem_get_mutilapn_status,
	["modem_enable_auto_dial"] = modem_enable_auto_dial,
	["modem_disable_auto_dial"] = modem_disable_auto_dial,
	["modem_get_auto_dial"] = modem_get_auto_dial,

	--network
	["network_get_wan_info"] = network_get_wan_info,
	["network_get_4g_net_info"] = network_get_4g_net_info,
	["network_get_4g1_net_info"] = network_get_4g1_net_info,
	["network_get_4g2_net_info"] = network_get_4g2_net_info,
	--sim
	["sim_get_status"] = sim_get_status,
	["sim_pin_lock_enable"] = sim_pin_lock_enable,
	["sim_pin_lock_disable"] = sim_pin_lock_disable,
	["sim_pin_unlock"] = sim_pin_unlock,
	["sim_pin_set_remember"] = sim_pin_set_remember,
	["sim_pin_get_remember"] = sim_pin_get_remember,
	["sim_reload"] = sim_reload,
	
	--system
	["system_get_status"] = system_get_status,
	["system_network_tool"] = system_network_tool,
	["system_ntp_enable"] = system_ntp_enable,
	["system_ntp_disable"] = system_ntp_disable,
	["system_ntp_get_enable_status"] = system_ntp_get_enable_status,
	["system_ntp_get_timezone"] = system_ntp_get_timezone,
	["system_ntp_set_timezone"] = system_ntp_set_timezone,
	["system_ntp_get_server_address"] = system_ntp_get_server_address,
	["system_ntp_set_server_address"] = system_ntp_set_server_address,
	["system_ntp_set_date"] = system_ntp_set_date,
	["system_lanrecord_get_history_info"] = system_lanrecord_get_history_info,
	["system_lanrecord_get_current_info"] = system_lanrecord_get_current_info,
	["system_export_config"] = system_export_config,
	["system_import_config"] = system_import_config,

 	--device
	["device_get_info"] = device_get_info,

	--firewall
	["firewall_set_mac_filter"] = firewall_set_mac_filter,
	["firewall_get_mac_filter"] = firewall_get_mac_filter,
	["firewall_get_ipmac_bind_filter"] = firewall_get_ipmac_bind_filter,
	["firewall_set_ipmac_bind_filter"] = firewall_set_ipmac_bind_filter,
	["firewall_set_url_filter"] = firewall_set_url_filter,
	["firewall_get_url_filter"] = firewall_get_url_filter,
	["firewall_set_ip_filter"] = firewall_set_ip_filter,
	["firewall_get_ip_filter"] = firewall_get_ip_filter,
	["firewall_set_acl_filter"] = firewall_set_acl_filter,
	["firewall_get_acl_filter"] = firewall_get_acl_filter,
	["firewall_get_port_filter"] = firewall_get_port_filter,
	["firewall_set_port_filter"] = firewall_set_port_filter,
	["firewall_get_port_redirect"] = firewall_get_port_redirect,
	["firewall_set_port_redirect"] = firewall_set_port_redirect,
	["firewall_get_speed_filter"] = firewall_get_speed_filter,
	["firewall_set_speed_filter"] = firewall_set_speed_filter,
	["firewall_get_default_action"] = firewall_get_default_action,
	["firewall_set_default_action"] = firewall_set_default_action,
	["firewall_start"] = firewall.firewall_start,
	["firewall_stop"] = firewall.firewall_stop,
	["firewall_restart"] = firewall.firewall_restart,
	["firewall_clear_all_rules"] = firewall.firewall_clear_all_user_rule,
	["firewall_remote_get_web_login"] = firewall_remote_get_web_login,
	["firewall_remote_set_web_login"] = firewall_remote_set_web_login,
	["firewall_remote_get_default_list"] = firewall_remote_get_default_list,
	["firewall_remote_set_default_list"] = firewall_remote_set_default_list,
	["firewall_remote_get_web_login_list"] = firewall_remote_get_web_login_list,
	["firewall_remote_set_web_login_list"] = firewall_remote_set_web_login_list,
	["firewall_remote_set_ping"] = firewall_remote_set_ping,
	["firewall_remote_get_ping"] = firewall_remote_get_ping,
	["firewall_remote_get_ping_list"] = firewall_remote_get_ping_list,
	["firewall_remote_set_ping_list"] = firewall_remote_set_ping_list,
	["firewall_set_mutil_nat"] = firewall_set_mutil_nat,
	["firewall_get_mutil_nat"] = firewall_get_mutil_nat,
	["firewall_get_lan_network"] = firewall_get_lan_network,
	["firewall_set_static_route"] = firewall_set_static_route,
	["firewall_get_static_route"] = firewall_get_static_route,

	--led
	["led_set_normal"] = led_set_normal,
	["led_set_exception"] = led_set_exception,
	["led_set_wifi"] = led_set_wifi,
	["led_set_wifi_5g"] = led_set_wifi_5g,
	["led_set_phone"] = led_set_phone,
	["led_set_wps"] = led_set_wps,
	["led_set_signal"] = led_set_signal,
	["led_all_off"] = led.led_all_off,
	["led_all_on"] = led.led_all_on,

	--tr069
	["tozed_get_tr069_config"] = tozed_get_tr069_config,
	["tozed_set_tr069_config"] = tozed_set_tr069_config
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

