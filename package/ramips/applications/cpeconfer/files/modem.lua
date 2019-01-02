require("uci")
require("io")
modem_module = {}
local util=require("tz.util")
local x = uci.cursor()
local MODEM_CONFIG_FILE="network"
local MODEM_DYNAMIC_STATUS_PATH="/tmp/.system_info_dynamic"
local MODEM_STATIC_INFO_PATH="/tmp/.system_info_static"
local debug = util.debug
local split = util.split 
local sleep = util.sleep

modem_module.modem_status= {
	["band"]=nil,					 --string current register band 
	["act"]=nil,					 --number 7:4g 2:3g 0:2g -1:none 
	["service_status"]=nil,			 --number <srv_status> 0 无服务 1 有限制服务 2 服务有效 3 有限制的区域服务 number                                          
	["ps_cs_status"]=nil,			 --number <srv_domain>0 无服务 1 仅CS 服务 2 仅PS 服务 3 PS+CS 服务 4 CS 、PS 均未注册,并处于搜索状态                                            
	["2g_register_status"]=nil,		 --number 1:registed 0:not registed                                      
	["3g_register_status"]=nil, 	 --number 1:registed 0:not registed                                     
	["4g_register_stauts"]=nil,		 --number 1:registed 0:not registed                                      
	["roam_status"]=nil,			 --number 0:not roam 1:roam                                             
	["network_type_num"]=nil,		 --number 
	--[[
				+NWTYPEIND:31 //"no service",
				+NWTYPEIND:32 //"network:gsm",
				+NWTYPEIND:33 //"network:gprs",
				+NWTYPEIND:34 //"network:edge",
				+NWTYPEIND:35 //"network:wcdma",
				+NWTYPEIND:36 //"network:hsdpa",
				+NWTYPEIND:37 //"network:hsupa"
				+NWTYPEIND:38 //"network (hsdpa_plus)"
				+NWTYPEIND:39 //"network (td_scdma)"
				+NWTYPEIND:40 //"network (lte_fdd)"
				+NWTYPEIND:41 //"network (lte_tdd)"
				+NWTYPEIND:42 //"network (EVDO)"
				+NWTYPEIND:43 //CDMA
				+NWTYPEIND:44 //CDMA&EVDO
				+NWTYPEIND:45 //EVDO
				]]--                                        
	["network_type_str"]=nil,  --string
	--[[
				LTE
				TDS
				HSPA+
				HSUPA
				HSDPA
				HSPA
				HSDPA+
				DC HSDPA+
				UMTS
				HDR RevA
				HDR RevB
				HDR Rev0
				GPRS
				EDGE
				GSM
				1x
				NONE
		]]--                                      
	["ipstack_current"]=nil,		--number <IP type> 1:Ipv4 2:Ipv6 3:Ipv4v6                                          
	["signal_lvl"]=nil,				--number 0~5                                               
	["rssi"]=nil,					--string   
	["network_link_stauts"]=nil,	--number 0 无数据链路 1 有数据链路                                     
	["cell_id"]=nil,				--string
	["pci"]=nil,					--string    
	["rsrp"]=nil,					--string   
	["rsrq"]=nil,					--string   
	["sinr"]=nil,					--string   
	["lac_id"]=nil,					--string 
	["active_band"]=nil,			--string                                             
	["active_channel"]=nil,			--string                                          
	["earfcn"]=nil,					--string 
	["enodebid"]=nil,				--string                                                
	["tac"]=nil,					--string    
	["rrc_status"]=nil,				--string                                              
	["gw_all_band"]=nil,			--number in hex                                             
	["lte_all_band"]=nil,			--number in hex                                            
	["tds_all_band"]=nil,			--number in hex                                            
	["gw_lock_band"]=nil,			--number in hex                                            
	["lte_lock_band"]=nil,			--number in hex                                           
	["tds_lock_band"]=nil,			--number in hex                                           
	["operator"]=nil, 				--string                                               
	["plmn"]=nil   				--string like 46001
}

modem_module.modem_device = {
	["type"]  = nil,      		--string
	["manufacturer"]  = nil,	--string
	["hardware"]  = nil,		--string
	["softver"]  = nil,			--string
	["imei"]  = nil 			--string
}

function modem_module.modem_status:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
	
	self["band"] = obj["band"] or  nil 
	self["act"] = obj["act"] or  nil
	self["service_status"] = obj["service_status"] or  nil                                         
	self["ps_cs_status"] = obj["ps_cs_status"] or  nil                                            
	self["2g_register_status"] = obj["2g_register_status"] or  nil                                      
	self["3g_register_status"] = obj["3g_register_status"] or  nil                                      
	self["4g_register_stauts"] = obj["4g_register_stauts"] or  nil                                     
	self["roam_status"] = obj["roam_status"] or  nil                                             
	self["network_type_num"] = obj["network_type_num"] or  nil                                        
	self["network_type_str"] = obj["network_type_str"] or  nil                                        
	self["ipstack_current"] = obj["ipstack_current"] or  nil                                        
	self["signal_lvl"] = obj["signal_lvl"] or  nil                                            
	self["rssi"] = obj["rssi"] or  nil
	self["network_link_stauts"] = obj["network_link_stauts"] or  nil
	self["cell_id"] = obj["cell_id"] or  nil
	self["pci"] = obj["pci"] or  nil
	self["rsrp"] = obj["rsrp"] or  nil
	self["rsrq"] = obj["rsrq"] or  nil
	self["sinr"] = obj["sinr"] or  nil
	self["lac_id"] = obj["lac_id"] or  nil
	self["active_band"] = obj["active_band"] or  nil                                         
	self["active_channel"] = obj["active_channel"] or  nil                                       
	self["earfcn"] = obj["earfcn"] or  nil
	self["enodebid"] = obj["enodebid"] or  nil                                          
	self["tac"] = obj["tac"] or  nil
	self["rrc_status"] = obj["rrc_status"] or  nil                                            
	self["gw_all_band"] = obj["gw_all_band"] or  nil                                          
	self["lte_all_band"] = obj["lte_all_band"] or  nil                                           
	self["tds_all_band"] = obj["tds_all_band"] or  nil                                        
	self["gw_lock_band"] = obj["gw_lock_band"] or  nil                                         
	self["lte_lock_band"] = obj["lte_lock_band"] or  nil                                        
	self["tds_lock_band"] = obj["tds_lock_band"] or  nil                                        
	self["operator"] = obj["operator"] or  nil                                      
	self["plmn"] = obj["plmn"] or  nil
   return o
end

function modem_module.modem_device:new(o,obj)

	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
	
	self["type"]  = obj["type"] or  nil
	self["manufacturer"]  = obj["manufacturer"] or  nil
	self["hardware"]  = obj["hardware"] or  nil
	self["softver"]  = obj["softver"] or  nil
	self["imei"]  = obj["imei"] or  nil
end

local status_keys = {
	"band",   
	"act",    
	"service_status",                                          
	"ps_cs_status",                                            
	"2g_register_status",                                      
	"3g_register_status",                                      
	"4g_register_stauts",                                      
	"roam_status",                                             
	"network_type_num",                                        
	"network_type_str",                                        
	"ipstack_current",                                         
	"signal_lvl",                                              
	"rssi",   
	"network_link_stauts",                                     
	"cell_id",
	"pci",    
	"rsrp",   
	"rsrq",   
	"sinr",   
	"lac_id", 
	"active_band",                                             
	"active_channel",                                          
	"earfcn", 
	"enodebid",                                                
	"tac",    
	"rrc_status",                                              
	"gw_all_band",                                             
	"lte_all_band",                                            
	"tds_all_band",                                            
	"gw_lock_band",                                            
	"lte_lock_band",                                           
	"tds_lock_band",                                           
	"operator",                                                
	"plmn",   
}

local device_keys = {"type","manufacturer","hardware","softver","imei"}

local function match_status_key(key)
	for k,v in pairs(status_keys) do
		if v == key
		then
			return true
		end

	end

	return false
end

local function match_device_key(key)
	for k,v in pairs(device_keys) do
		if v == key
		then
			return true
		end

	end

	return false
end

-- get modem status information
-- input:none
-- return:
--		the struct of modem_status 
function modem_module.modem_get_status()
	local f = nil
	local status = nil
	local res = nil

	f = io.open(MODEM_DYNAMIC_STATUS_PATH,"r")

	if nil == f
	then
		debug("open ",MODEM_DYNAMIC_STATUS_PATH, " fail")
		return nil
	end

	res = f:read()

	while( res ~= nil)
	do
		if nil == status
		then
			status = modem_module.modem_status:new(nil,nil)
		end

		local array = split(res,":")
		--print(array[1],"=", array[2])
		local key = string.gsub(array[1],"\t","")
		local value = string.gsub(array[2],"\t","")
		key = string.lower(key)
		
		if  match_status_key(key)
		then
			status[key] = value
		end

		res = f:read()
	end

	io.close(f)

	return status

end


local function get_device_type()

	local f = io.open(MODEM_DYNAMIC_STATUS_PATH, "r")

	if nil == f
	then
		debug("open ", MODEM_DYNAMIC_STATUS_PATH, " fail")
		return nil
	end

	local res = f:read()

	while (nil ~= res)
	do
		local strs = split(res, ":")

		if "ModuleType" == strs[1]
		then
		    io.close(f)
			return strs[2]
		end
		res = f:read()
	end

	io.close(f)
	return nil
end

-- get 4g module information
-- input:none
-- return:
--		the struct of modem_device
function modem_module.modem_get_info()
	local f = nil
	local info = nil
	local res = nil

	f = io.open(MODEM_STATIC_INFO_PATH,"r")

	if nil == f
	then
		debug("open ",MODEM_STATIC_INFO_PATH, " fail")
		return nil
	end

	res = f:read()

	while( res ~= nil)
	do
		if nil == info
		then
			info = modem_module.modem_device:new(nil,nil)
			info["type"] = get_device_type()
		end

		local array = split(res,":")
		--print(array[1],"=", array[2])
		local key = string.gsub(array[1],"\t","")
		local value = string.gsub(array[2],"\t","")
		key = string.lower(key)
		
		local keys = split(key,"_")
		if keys[1] == "module" and match_device_key(keys[2])
		then
			info[keys[2]] = value
		end

		res = f:read()
	end

	io.close(f)

	return info

end


return modem_module
