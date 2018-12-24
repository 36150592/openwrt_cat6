#!/usr/bin/lua

local cjson = require "cjson"
local wifi = require "tz.wifi"
local dhcp = require "tz.dhcp"
local system = require "tz.system"
local sim = require "tz.sim"
local modem = require "tz.modem"
local device = require "tz.device"
local network = require "tz.network"

print("Contenttype:text/html\n")

data1=io.read();
tz_req = cjson.decode(data1);



function login()

	local tz_answer = {};
	tz_answer["cmd"] = 100;

    local username = tz_req["username"]
	local password = tz_req["passwd"]
	
	local file = io.open("../config.json", "r")
	io.input(file)
	local t =io.read("*a")
	io.close(file)
	local Jsondata = cjson.decode(t)
	local Login = Jsondata["Login"]
	
	for k,v in pairs(Login) do
	  if(v["UserName"] == username) then
	      if(v["PassWord"] == password) then
		     tz_answer["success"] = true;
			 result_json = cjson.encode(tz_answer);
			 print(result_json);
			 return 
		  else
		     tz_answer["success"] = false;
			 result_json = cjson.encode(tz_answer);
			 print(result_json);
			 return
		  end   
	  
	  end
	 
	end
	
	tz_answer["success"] = false;
	result_json = cjson.encode(tz_answer);
	print(result_json);
	return
   
	
	
end

function iniPage()
    local file = io.open("../config.json", "r")
	io.input(file)
	local t =io.read("*a")
	io.close(file)
	local Jsondata = cjson.decode(t);
	local Rdata ={};
	Rdata["language"] = Jsondata["Language"]
	Rdata["languageList"] = Jsondata["LanguageList"]
	local tz_answer = {};
	tz_answer["success"] = true;
	tz_answer["cmd"] = 80;
	tz_answer["data"] = Rdata;
	result_json = cjson.encode(tz_answer);
	print(result_json);
	
	
end

function change_language()
	local oldLanguage = string.lower(tz_req["languageOld"])
    local newLanguage = string.lower(tz_req["languageSelect"])
	shellcmd = string.format("sed -i 's/%s.js?t=000000/%s.js?t=000000/g' /usr/tzwww/login.html",oldLanguage,newLanguage)
	shellcmd1 = string.format("sed -i 's/%s.js?t=000000/%s.js?t=000000/g' /usr/tzwww/index.html",oldLanguage,newLanguage)
	shellcmd2 = string.format("sed -i 's/\"Language\":\"%s\"/\"Language\":\"%s\"/g' /usr/tzwww/config.json",oldLanguage,newLanguage)
	
	os.execute(shellcmd)
	os.execute(shellcmd1)
	os.execute(shellcmd2)
	
    local tz_answer = {};
	tz_answer["success"] = true;
	tz_answer["cmd"] = 97;
	result_json = cjson.encode(tz_answer);
	print(result_json);
   
end


function get_sysinfo()
	local data_array = {}
	local data_lan = {}
    data_array["system"]  = system.system_get_status()
	data_array["sim"] = sim.sim_get_status()
	data_array["network"] = network.network_get_wan_info()
	data_lan["lanIp"] = dhcp.dhcp_get_server_ip()
	data_lan["netMask"] = dhcp.dhcp_get_server_mask()
    data_array["lan"] = data_lan
	local tz_answer = {};
    tz_answer["success"] = true;
	tz_answer["cmd"] = 0;
	tz_answer["data"] = data_array;
	result_json = cjson.encode(tz_answer);
	print(result_json); 

end

function get_diviceinfo()
	local data_array = {}
	data_array["sim"] = sim.sim_get_status()
	data_array["modem"] = modem.modem_get_info()
	data_array["device"] = device.device_get_info()
    local tz_answer = {};
    tz_answer["success"] = true;
	tz_answer["cmd"] = 43;
	tz_answer["data"] = data_array;
	result_json = cjson.encode(tz_answer);
	print(result_json); 
	
end

function get_wifi()
	local array = wifi.wifi_get_dev()
	local data_array = array[1]
	local tz_answer = {};
    tz_answer["success"] = true;
	tz_answer["cmd"] = 101;
	tz_answer["data"] = data_array;
	result_json = cjson.encode(tz_answer);
	print(result_json);
end

function get_dhcp()
    local data_array = {}
	data_array["lanIp"] = dhcp.dhcp_get_server_ip()
	data_array["netMask"] = dhcp.dhcp_get_server_mask()
	data_array["ipBegin"],data_array["ipEnd"] = dhcp.dhcp_get_ip_range()
	data_array["expireTime"] = dhcp.dhcp_get_lease_time()
	local tz_answer = {};
    tz_answer["success"] = true;
	tz_answer["cmd"] = 102;
	tz_answer["data"] = data_array;
	result_json = cjson.encode(tz_answer);
	print(result_json);
end

function set_wifi()

	local tz_answer = {}
	tz_answer["cmd"] = 2
    local ret 
	local array = wifi.wifi_get_dev()
	local id = array[1]['wifi_id']
	
	local broadcast = tonumber(tz_req["broadcast"])
	local wmm = tonumber(tz_req["wmm"])
    local ssid = tz_req["ssid"]
	local txPower = tonumber(tz_req["txPower"])
	local channel = tz_req["channel"]
	local mode = tonumber(tz_req["wifiWorkMode"])
	
	if(nil ~= broadcast)
	then
		if(0 == broadcast)
		 then
	       ret = wifi.wifi_disable_hidden_ssid(id)
		    if(not ret)
			  then
			  tz_answer["disableSsid"] = false
		    end
		elseif(1 == broadcast)
		  then
			ret = wifi.wifi_enable_hidden_ssid(id)
			  if(not ret)
		        then
			     tz_answer["enableSsid"] = false
              end
	   end
	end
	
	if(nil ~= wmm)
	then
		if(0 == wmm)
		  then
	        ret = wifi.wifi_disable_wmm(id)
		     if(not ret)
		       then
			   tz_answer["disableWmm"] = false
			  end
	    elseif(1 == wmm)
	      then
	      ret = wifi.wifi_enable_wmm(id)
	        if(not ret)
		      then
			  tz_answer["enableWmm"] = false
            end
	    end
	end
	
	if(nil ~= ssid)
	 then
		ret = wifi.wifi_set_ssid(id, ssid)
			if(not ret)
				then
				tz_answer["setSsid"] = false
			end
	end
		
	if(nil ~= txPower)
	 then
		ret = wifi.wifi_set_txpower(id, txPower)	
			if(not ret)
				then
				tz_answer["setTxpower"] = false
			end
	end  
	
	if(nil ~= channel)
	 then
		ret = wifi.wifi_set_channel(id, channel)	
			if(not ret)
				then
				tz_answer["setChannel"] = false
			end
	end  
	
	if(nil ~= mode)
	 then
		ret = wifi.wifi_set_mode(id, mode)	
			if(not ret)
				then
				tz_answer["setMOde"] = false
			end
	end  
	
	

	--wifi.wifi_restart(id)
	tz_answer["success"] = true
	result_json = cjson.encode(tz_answer)
	print(result_json);
	
end


function set_dhcp()

	local tz_answer = {}
	tz_answer["cmd"] = 3
    local ret 
	local setipflg = false
	local array = wifi.wifi_get_dev()
	local id = array[1]['wifi_id']
	
	local lanIp = tz_req["lanIp"]
	local netMask = tz_req["netMask"]
	local ipBegin = tz_req["ipBegin"]
	local ipEnd = tz_req["ipEnd"]
	local limitNum = tz_req["limitNum"]
	local expireTime = tonumber(tz_req["expireTime"])
   
	if(nil ~= lanIp)
	  then
		ret = dhcp.dhcp_set_server_ip(lanIp)
			if(ret)
				then
				setipflg = true
				tz_answer["setIp"] = true	
			else
                tz_answer["setIp"] = false			
			end
	end
	
	if(nil ~= netMask)
	  then
		ret = dhcp.dhcp_set_server_mask(netMask)
			if(not ret)
				then
				tz_answer["setMask"] = false
			end
	end
	
	if(nil ~= expireTime)
	  then
		ret = dhcp.dhcp_set_lease_time(expireTime)
			if(not ret)
				then
				tz_answer["setLeaseTime"] = false
			end
	end
	
	if(nil ~= ipBegin)
	  then
		ret = dhcp.dhcp_set_ip_range(ipBegin, limitNum)
			if(not ret)
				then
				tz_answer["setIpRange"] = false
			end
	end	
	
	if(setipflg)
	   then
	   dhcp.dhcp_reload()
	else
		dhcp.dhcp_restart()
    end	
	
	tz_answer["success"] = true
	result_json = cjson.encode(tz_answer)
	print(result_json)
	
end

function get_dhcpClient()
    local tz_answer = {}
	tz_answer["cmd"] = 103   
	local data_array = dhcp.dhcp_get_client_list()
	tz_answer["success"] = true
	tz_answer["data"] = data_array
	result_json = cjson.encode(tz_answer)
	print(result_json)

end

local switch = {
     [0] = get_sysinfo,
     [2] = set_wifi,
	 [3] = set_dhcp,
	 [43] = get_diviceinfo,
     [80] = iniPage,
	 [97] = change_language,
	 [100] = login,
	 [101] = get_wifi,
	 [102] = get_dhcp,
	 [103] = get_dhcpClient
 }
 
 local f = switch[tz_req["cmd"]]
 if(f) then
     f()
 end
 
 









