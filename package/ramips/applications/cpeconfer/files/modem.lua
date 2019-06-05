require("uci")
require("io")
modem_module = {}
local util=require("tz.util")
local tzlib = require("luatzlib")
local x = uci.cursor()
local MODEM_CONFIG_FILE="network"
local TOZED_CONFIG_FILE="tozed"
local UCI_SECTION_DIALTOOL2="modem"
local FIREWALL_CONFIG_FILE="firewall"
local MODEM_DYNAMIC_STATUS_PATH="/tmp/.system_info_dynamic"
local MODEM_STATIC_INFO_PATH="/tmp/.system_info_static"
local DIALTOOL2_SOCKET_FILE="/tmp/dialtool2.socket"
local MUTILAPN_INFO_FILE="/tmp/.mutilapn_info"
local GENERATE_DIALTOOL_CONFIG_CMD="cfg -e  --section modem |grep -v 'export value' |  awk '{print $2}' | grep 'TZ_DIALTOOL2' > /tmp/dialtool2_config"
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
	["plmn"]=nil,   				--string like 46001
	["cqi"]=nil,					--string
	["mcs"]=nil                     --string
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
	self["cqi"] = obj["cqi"] or  nil                                      
	self["mcs"] = obj["mcs"] or  nil
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
	"4g_register_status",                                      
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
	"cqi",
	"mcs",
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

local function send_dialtool_socket_cmd(cmd)
	local fd = tzlib.unix_connect(DIALTOOL2_SOCKET_FILE)
	if nil == fd
	then
		debug("open socket error")
		return false
	end
	
	local n = tzlib.unix_write(fd, cmd,string.len(cmd));
	if n ~= string.len(cmd)
	then
		debug("write cmd fail")
		return false
	end

	local str = tzlib.unix_read(fd)
	tzlib.unix_close(fd)

	if "OK" == str
	then
		return true;
	end
	return false
end

function modem_module.modem_reload_config()
	return 0 == os.execute("/etc/init.d/dialtool2 restart")
end



-- lock band
-- input:mode(number):
--1-4g only 5
--2-4g pro 2(TDS) or 11(CDMA) or 17(WCDMA)
--3-3g only 7 or 10(EVDO) or 1:WCDMA ONLY
--4-3g pro 4(GSM) or 12(EVDO) or 16(LTE) 18(GSM) or 19(LTE)
--0-2g 3g 21:WCDMA+GSM 14:CDMA+EVDO 8:TDS+GSM 
-- return:
--		true if success  false if fail
function modem_module.modem_set_network_mode(mode)
	if type(mode) ~= "number"
	then
		debug("input error")
		return false
	end

	if mode >= 5 and mode < 0
	then
		debug("mode out of range")
		return false
	end

	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_LTE_MODULE_MODE", mode)
	return x:commit(TOZED_CONFIG_FILE)
end

-- get network mode 
-- input(none)
-- return 
--		network mode in number as above
function modem_module.modem_get_network_mode()
	return tonumber(x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_LTE_MODULE_MODE"))
end

local lte_band_bit = {
	["EUTRAN_BAND1"] = 0x1           ,
	["EUTRAN_BAND2"] = 0x2           ,
	["EUTRAN_BAND3"] = 0x4           ,
	["EUTRAN_BAND4"] = 0x8           ,
	["EUTRAN_BAND5"] = 0x10          ,
	["EUTRAN_BAND6"] = 0x20          ,
	["EUTRAN_BAND7"] = 0x40          ,
	["EUTRAN_BAND8"] = 0x80          ,
	["EUTRAN_BAND9"] = 0x100         ,
	["EUTRAN_BAND10"]= 0x200         ,
	["EUTRAN_BAND11"]= 0x400         ,
	["EUTRAN_BAND12"]= 0x800         ,
	["EUTRAN_BAND13"]= 0x1000        ,
	["EUTRAN_BAND14"]= 0x2000        ,
	["EUTRAN_BAND15"]= 0x4000        ,
	["EUTRAN_BAND16"]= 0x8000        ,
	["EUTRAN_BAND17"]= 0x10000       ,
	["EUTRAN_BAND18"]= 0x20000       ,
	["EUTRAN_BAND19"]= 0x40000       ,
	["EUTRAN_BAND20"]= 0x80000       ,
	["EUTRAN_BAND21"]= 0x100000      ,
	["EUTRAN_BAND22"]= 0x200000      ,
	["EUTRAN_BAND23"]= 0x400000      ,
	["EUTRAN_BAND24"]= 0x800000      ,
	["EUTRAN_BAND25"]= 0x1000000     ,
	["EUTRAN_BAND26"]= 0x2000000     ,
	["EUTRAN_BAND27"]= 0x4000000     ,
	["EUTRAN_BAND28"]= 0x8000000     ,
	["EUTRAN_BAND29"]= 0x10000000    ,
	["EUTRAN_BAND30"]= 0x20000000    ,
	["EUTRAN_BAND31"]= 0x40000000    ,
	["EUTRAN_BAND32"]= 0x80000000    ,
	["EUTRAN_BAND33"]= 0x100000000   ,
	["EUTRAN_BAND34"]= 0x200000000   ,
	["EUTRAN_BAND35"]= 0x400000000   ,
	["EUTRAN_BAND36"]= 0x800000000   ,
	["EUTRAN_BAND37"]= 0x1000000000  ,
	["EUTRAN_BAND38"]= 0x2000000000  ,
	["EUTRAN_BAND39"]= 0x4000000000  ,
	["EUTRAN_BAND40"]= 0x8000000000  ,
	["EUTRAN_BAND41"]= 0x10000000000 ,
	["EUTRAN_BAND42"]= 0x20000000000 ,
	["EUTRAN_BAND43"]= 0x40000000000 ,
}

local gw_band_bit = {
	["BC0 A"]  		 	 	=	0x1            ,
	["BC0 B"]  		 		=	0x2            ,
	["BC1"] 		 		=	0x4            ,
	["GSM_450"] 	 		=	0x10000        ,
	["GSM_480"]	 			=	0x20000        ,
	["GSM_750"]	 			=	0x00040000     ,
	["GSM_850"]	 			= 	0x00080000     ,
	["EGSM_900"]	 		=	0x00000100     ,
	["RGSM_900"]	 		=	0x00100000     ,
	["PGSM_900"]	 		=	0x00000200     ,
	["DCS_1800"]	 		=	0x00000080     ,
	["PCS_1900"]	 		=	0x00200000     ,
	["WCDMA_I_IMT_2000"]  	=	0x00400000     ,
	["WCDMA_II_PCS_1900"] 	=	0x00800000     ,
	["WCDMA_III_1700"]		=	0x01000000     ,
	["WCDMA_IV_1700"]		=	0x02000000     ,
	["WCDMA_V_850"]			=	0x04000000     ,
	["WCDMA_VI_800"] 		=	0x08000000     ,
	["WCDMA_VII_2600"] 		=	0x1000000000000,
	["WCDMA_VIII_900"] 		=	0x2000000000000,
	["WCDMA_IX_1700"] 		=	0x4000000000000,	
}

local tds_band_bit = {
	["TDS_BAND A"]=0x1 ,
	["TDS_BAND B"]=0x2 ,
	["TDS_BAND C"]=0x4 ,
	["TDS_BAND D"]=0x8 ,
	["TDS_BAND E"]=0x10,
	["TDS_BAND F"]=0x20,
}



local function get_support_band()
	
	local cmd = string.format("cat %s | grep ALL_BAND", MODEM_DYNAMIC_STATUS_PATH)
	local f = io.popen(cmd)
	local gw_all_band, tds_all_band, lte_all_band
	if nil ~= f
	then
		res = f:read()
		while nil ~= res
		do
			local ar 
			if string.find(res,"GW_ALL_BAND") ~= nil
			then	
				ar = util.split(res,":")
				gw_all_band = ar[2]
			elseif string.find(res,"LTE_ALL_BAND") ~= nil
			then
				ar = util.split(res,":")
				lte_all_band = ar[2]
			elseif string.find(res,"TDS_ALL_BAND") ~= nil
			then
				ar = util.split(res, ":")
				tds_all_band = ar[2]
			end

			res = f:read()
		end

		io.close(f)
		return tonumber(gw_all_band), tonumber(lte_all_band), tonumber(tds_all_band)
	end

	debug("get support band error")
	return nil, nil, nil
end

local function get_lock_band()

	local gw_lock_band ,tds_lock_band, lte_lock_band
	local gw_support_band, lte_support_band,tds_support_band  = get_support_band()

	gw_lock_band = x:get(TOZED_CONFIG_FILE,UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_GW_BAND_PREF")
	tds_lock_band = x:get(TOZED_CONFIG_FILE,UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_TDS_BAND_PREF")
	lte_lock_band = x:get(TOZED_CONFIG_FILE,UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_LTE_BAND_PREF")

	if nil == gw_lock_band or "" == gw_lock_band
	then
		gw_lock_band = gw_support_band
	else
		gw_lock_band = "0x" .. gw_lock_band 
	end

	if nil == tds_lock_band or "" == tds_lock_band
	then
		tds_lock_band = tds_support_band
	else
		tds_lock_band = "0x" .. tds_lock_band
	end

	if nil == lte_lock_band or "" == lte_lock_band
	then
		lte_lock_band = lte_support_band
	else
		lte_lock_band = "0x" .. lte_lock_band
	end

	return tonumber(gw_lock_band), tonumber(lte_lock_band), tonumber(tds_lock_band)
end

local function band_hex_to_band_list(type,hex)
	
	local band_name = {}
	local i = 1

	if "gw" == type
	then
		for key, value in pairs(gw_band_bit)
		do
			if util._and(value, hex) > 0
			then
				band_name[i]=key
				i = i +1
			end
		end
	elseif "tds" == type
	then
		for key, value in pairs(tds_band_bit)
		do
			
			if util._and(value, hex) > 0
			then
				
				band_name[i]=key
				i = i +1
			end
		end
	elseif "lte" == type
	then
		for key, value in pairs(lte_band_bit)
		do
			if util._and(value, hex) > 0 
			then
				band_name[i]=key
				i = i +1
			end
		end
	else
		debug("type error:must be [gw , tds, lte]")
		return nil
	end

	return band_name
end

-- get modem support band name 
-- input:none
-- return:the list of support band names in gw , lte, tds, nil if  not support any band
function modem_module.modem_get_support_band()
	local gw_all_band, lte_all_band, tds_all_band
	local gw_names, lte_names,tds_names
	gw_all_band, lte_all_band, tds_all_band = get_support_band()
	debug("gw_all_band = ", gw_all_band)
	debug("lte_all_band = ", lte_all_band)
	debug("tds_all_band = ", tds_all_band)
	
	if nil ~= gw_all_band
	then
		gw_names = band_hex_to_band_list("gw", gw_all_band)
	end

	if nil ~= lte_all_band
	then
		lte_names = band_hex_to_band_list("lte", lte_all_band)
	end

	if nil ~= tds_all_band
	then
		tds_names = band_hex_to_band_list("tds", tds_all_band)
	end

	return gw_names,lte_names,tds_names
end

-- set lock band
-- input:the list of band name as tds_band_bit gw_band_bit and lte_band_bit 's key name
-- return:true if success false if fail
function modem_module.modem_set_lock_band(band_list)
		
	local lte_band = 0x0
	local tds_band = 0x0
	local gw_band = 0x0
	local gw_all_band, lte_all_band, tds_all_band
	local gw_ret = true
	local lte_ret = true
	local tds_ret = true

	gw_all_band, lte_all_band, tds_all_band = get_support_band()

	debug("gw_all_band = ", gw_all_band)
	debug("lte_all_band = ", lte_all_band)
	debug("tds_all_band = ", tds_all_band)

	for key,value in pairs(band_list)
	do
		if nil ~= lte_band_bit[value] 
		then
			lte_band = lte_band + lte_band_bit[value]
		elseif nil ~= gw_band_bit[value]
		then
			gw_band = gw_band + gw_band_bit[value]
		elseif nil ~= tds_band_bit[value]
		then
			tds_band = tds_band + tds_band_bit[value]
		end
	end

	debug("lte_band = ", lte_band)
	debug("gw_band = ", gw_band)
	debug("tds_band = ", tds_band)

	if gw_all_band ~= nil and gw_band ~= nil and util._and(gw_all_band, gw_band) == gw_band 
	then
		gw_ret = x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_GW_BAND_PREF", util.to_hex_string(gw_band))
		debug("gw_ret ", gw_ret)
	end

	if lte_all_band ~= nil and lte_band ~= nil and util._and(lte_all_band, lte_band) == lte_band 
	then
		lte_ret = x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_LTE_BAND_PREF", util.to_hex_string(lte_band))
		debug("lte_ret ", lte_ret)
	end

	if tds_all_band ~= nil and tds_band ~= nil and util._and(tds_all_band, tds_band) == tds_band 
	then
		tds_ret = x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_TDS_BAND_PREF", util.to_hex_string(tds_band))
		debug("tds_ret ", tds_ret)
	end

	x:commit(TOZED_CONFIG_FILE)
	return gw_ret and lte_ret and tds_ret 

end

-- get lock band
-- input:none
-- return:the list of band name as tds_band_bit gw_band_bit and lte_band_bit 's key name
function modem_module.modem_get_lock_band()
	local gw_lock_band, lte_lock_band, tds_lock_band = get_lock_band()
	local gw_ar, lte_ar, tds_ar
	local ret_ar = {}
	local i = 1
	
	debug("gw_lock_band = ", gw_lock_band)
	debug("lte_lock_band = ", lte_lock_band)
	debug("tds_lock_band = ", tds_lock_band)

	if nil ~= gw_lock_band
	then
		gw_ar = band_hex_to_band_list("gw", gw_lock_band)

		for k,v in pairs(gw_ar)
		do
			debug("gw_ar[", k,"] = ", v)
			ret_ar[i] = v
			i = i+1
		end
	end

	if nil ~= lte_lock_band
	then
		lte_ar = band_hex_to_band_list("lte", lte_lock_band)
		for k,v in pairs(lte_ar)
		do
			debug("lte_ar[", k,"] = ", v)
			ret_ar[i] = v
			i = i+1
		end
	end

	if nil ~= tds_lock_band
	then
		tds_ar = band_hex_to_band_list("tds", tds_lock_band)
		for k,v in pairs(tds_ar)
		do
			debug("tds_ar[", k,"] = ", v)
			ret_ar[i] = v
			i = i+1
		end
	end

	return ret_ar
end

-- lock operator 
-- input: 
--		the list of operator mcc mnc
--		nil if do not lock any operator / disable lock operator
-- return:true if success false if fail
function modem_module.modem_set_lock_operator(operator_list)

	if nil == operator_list
	then
		debug("operator_list is nil disable lock operator")
		operator_list = {""}
	end


	local operator_str = ""
	for k,v in pairs(operator_list)
	do
		operator_str = operator_str .. "," .. v

	end

	operator_str = string.sub(operator_str, 2, string.len(operator_str))

	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_PLMN_LOCK", operator_str)
	return x:commit(TOZED_CONFIG_FILE) 
end

-- get the operator lock list 
-- input:none
-- return:
-- 		the list of operator mcc mnc
--		nil if not lock any mcc mnc
function modem_module.modem_get_lock_operator()

	local operator_list = {}

	local operator_str = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_PLMN_LOCK")
	if nil == operator_str or "" == operator_str
	then
		return nil
	end

	operator_list = util.split(operator_str,',')

	return operator_list

end

-- lock LTE CELL 
-- input: 
--		pci:LTE physical cell id range from 0 ~503 , nil or '' mean disable this function
--		earfcn:LTE earfcn range from 0 ~ 65535 , nil or '' mean disable this function
-- return:true if success false if fail
function modem_module.modem_set_lte_lock_cell(pci, earfcn)

	if '' == pci or '' == earfcn or nil == pci or nil == earfcn
	then
		x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_LTE_PCI_LOCK", "")
		x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_LTE_EARFCN_LOCK", "")
		os.execute("sendat -e at+bmpcilock=0,2")
	elseif	pci >=0 and pci <= 503 and earfcn <=65535 and earfcn >=0
	then
		x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_LTE_PCI_LOCK", pci)
		x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_LTE_EARFCN_LOCK", earfcn)
		os.execute("sendat -e at+bmpcilock=1,2,"..earfcn..","..pci)
	else
		debug("input error")
		return false
	end
	return x:commit(TOZED_CONFIG_FILE)

end

-- get lock LTE CELL
-- input:none
-- return:
-- 		pci,earfcn 
--		or  nil --> that this fucntion is disable
function modem_module.modem_get_lte_lock_cell()

	local pci = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_LTE_PCI_LOCK")
	local earfcn = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_LTE_EARFCN_LOCK")

	if nil == pci or nil == earfcn 
	then
		debug("lte lock cell is disable")
		return nil,nil
	end


	return pci,earfcn
end

modem_module.modem_mutilapn_config = {
	["profile_name"]  = nil,      		--string
	["apn_name"]  = nil,	--string
	["username"]  = nil,		--string
	["password"]  = nil,			--string
	["ip_stack"]  = nil,			--string IP IPV6 IPV4V6
	["auth_type"] = nil,			--number 0:NONE 1:PAP 2:CHAP 3:PAP&CHAP
	["mtu"] = nil,					--number 
	["nat"] = nil,				--string enable/disable   mutil nat config
	["nat_lookback"] = nil,     --string nat lookback
	["enable"] = nil,				--number 0:disable 1:enable
}

function modem_module.modem_mutilapn_config:new(o,obj)

	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
	
	self["profile_name"]  = obj["profile_name"] or  nil
	self["apn_name"]  = obj["apn_name"] or  nil
	self["username"]  = obj["username"] or  nil
	self["password"]  = obj["password"] or  nil
	self["ip_stack"]  = obj["ip_stack"] or  nil
	self["auth_type"]  = obj["auth_type"] or  nil
	self["mtu"]  = obj["mtu"] or  nil
	self["nat"]  = obj["nat"] or  nil
	self["nat_lookback"] = obj["nat_lookback"] or nil
	self["enable"]  = obj["enable"] or  nil
end

-- get mutil apn config
-- input:NONE
-- return:the array of modem_mutilapn_config . the No.0 is the primary apn
function modem_module.modem_get_mutilapn_config()
	local ar = {}
	local primary_apn = modem_module.modem_mutilapn_config:new(nil,nil)
	local primary_apn1 = modem_module.modem_mutilapn_config:new(nil,nil)
	local primary_apn2 = modem_module.modem_mutilapn_config:new(nil,nil)

	primary_apn["profile_name"] = "main_apn"
	primary_apn["apn_name"] = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_APN_NAME")
	primary_apn["username"] = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_PPP_USERNAME")
	primary_apn["password"] = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_PPP_PASSWORD")
	primary_apn["ip_stack"] = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_IP_STACK_MODE") or "IP"
	primary_apn["auth_type"] = tonumber(x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_PPP_AUTH_TYPE") or 3)
	primary_apn["mtu"] = tonumber(x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_MTU_USB0") or 1500)
	primary_apn["nat"] = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_NAT") or "enable"
	primary_apn["nat_lookback"] = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_NAT_LOOKBACK") or "enable"
	primary_apn["enable"] = 1

	ar[1] = primary_apn

	primary_apn1["profile_name"] = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN1_PROFILE_NAME")
	primary_apn1["apn_name"] = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN1_APN_NAME")
	primary_apn1["username"] = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN1_USERNAME")
	primary_apn1["password"] = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN1_PASSWORD")
	primary_apn1["ip_stack"] = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN1_IP_STACK_MODE") or "IP"
	primary_apn1["auth_type"] = tonumber(x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN1_AUTH_TYPE") or 3)
	primary_apn1["mtu"] = tonumber(x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN1_MTU") or 1500)
	primary_apn1["enable"] = tonumber(x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN1_ENABLE"))
	primary_apn1["nat"] = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN1_NAT") or "enable"
	primary_apn1["nat_lookback"] = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN1_NAT_LOOKBACK") or "enable"

	ar[2] = primary_apn1

	primary_apn2["profile_name"] = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN2_PROFILE_NAME")
	primary_apn2["apn_name"] = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN2_APN_NAME")
	primary_apn2["username"] = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN2_USERNAME")
	primary_apn2["password"] = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN2_PASSWORD")
	primary_apn2["ip_stack"] = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN2_IP_STACK_MODE") or "IP"
	primary_apn2["auth_type"] = tonumber(x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN2_AUTH_TYPE") or 3)
	primary_apn2["mtu"] = tonumber(x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN2_MTU") or 1500)
	primary_apn2["enable"] = tonumber(x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN2_ENABLE"))
	primary_apn2["nat"] = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN2_NAT") or "enable"
	primary_apn2["nat_lookback"] = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN2_NAT_LOOKBACK") or "enable"
	ar[3] = primary_apn2

	return ar
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


-- set mutil apn config
-- input:the array of modem_mutilapn_config . the No.0 is the primary apntrue 
-- return:true if success OR false if fail
-- note:firewall restart needed
function modem_module.modem_set_mutilapn_config(apn_list)

	local apns = apn_list[1]
	local temp = {}
	local i = 1
	local sub_net=nil
	local ipaddr = nil
	local netmask = nil

	--apns["profile_name"] = "main_apn"
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_APN_NAME",apns["apn_name"] or "")
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_PPP_USERNAME", apns["username"] or "")
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_PPP_PASSWORD",apns["password"] or "")
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_IP_STACK_MODE",apns["ip_stack"] or "IP")
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_PPP_AUTH_TYPE",apns["auth_type"] or 3)
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_MTU_USB0",apns["mtu"] or 1500)
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_NAT",apns["nat"] or "enable")
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_NAT_LOOKBACK",apns["nat_lookback"] or "enable")
	--apns["enable"] = 1

	if "disable" ~= apns["nat"]
	then
		ipaddr =  x:get(MODEM_CONFIG_FILE,"lan","ipaddr")
		netmask =  x:get(MODEM_CONFIG_FILE,"lan","netmask")
		if nil ~= ipaddr and nil ~= netmask
		then
			sub_net = get_sub_net_by_ip(ipaddr, netmask)
			if nil ~= sub_net
			then
				temp[i] = sub_net .. "/" .. netmask
				i = i+1
			end
		end
	end



	apns = apn_list[2]
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN1_PROFILE_NAME",apns["profile_name"] or "apn1")
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN1_APN_NAME",apns["apn_name"]  or "")
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN1_USERNAME",apns["username"] or "")
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN1_PASSWORD",apns["password"] or "")
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN1_IP_STACK_MODE",apns["ip_stack"] or "IP")
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN1_AUTH_TYPE",apns["auth_type"] or 3)
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN1_MTU",apns["mtu"] or 1500)
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN1_ENABLE",apns["enable"] or 0)
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN1_NAT",apns["nat"] or "enable")
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN1_NAT_LOOKBACK",apns["nat_lookback"] or "enable")

	if "disable" ~= apns["nat"]
	then
		ipaddr =  x:get(MODEM_CONFIG_FILE,"lan1","ipaddr")
		netmask =  x:get(MODEM_CONFIG_FILE,"lan1","netmask")
		if nil ~= ipaddr and nil ~= netmask
		then
			sub_net = get_sub_net_by_ip(ipaddr, netmask)
			if nil ~= sub_net
			then
				temp[i] = sub_net .. "/" .. netmask
				i = i+1
			end
		end
	end

	apns = apn_list[3]
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN2_PROFILE_NAME",apns["profile_name"] or "apn1")
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN2_APN_NAME",apns["apn_name"]  or "")
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN2_USERNAME",apns["username"] or "")
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN2_PASSWORD",apns["password"] or "")
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN2_IP_STACK_MODE",apns["ip_stack"] or "IP")
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN2_AUTH_TYPE",apns["auth_type"] or 3)
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN2_MTU",apns["mtu"] or 1500)
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN2_ENABLE",apns["enable"] or 0)
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN2_NAT",apns["nat"] or "enable")
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_MUTILAPN2_NAT_LOOKBACK",apns["nat_lookback"] or "enable")

	if "disable" ~= apns["nat"]
	then
		ipaddr =  x:get(MODEM_CONFIG_FILE,"lan2","ipaddr")
		netmask =  x:get(MODEM_CONFIG_FILE,"lan2","netmask")
		if nil ~= ipaddr and nil ~= netmask
		then
			sub_net = get_sub_net_by_ip(ipaddr, netmask)
			if nil ~= sub_net
			then
				temp[i] = sub_net .. "/" .. netmask
				i = i+1
			end
		end
	end


	local ret1 =  x:commit(TOZED_CONFIG_FILE)

	x:foreach(FIREWALL_CONFIG_FILE, "zone", function(s)
		if 	table.maxn(temp) == 0
		then
			x:set(FIREWALL_CONFIG_FILE,s[".name"],"masq_src", '')
			x:set(FIREWALL_CONFIG_FILE,s[".name"], "masq", "0")
		else
			x:set(FIREWALL_CONFIG_FILE,s[".name"],"masq_src", temp)
			x:set(FIREWALL_CONFIG_FILE,s[".name"], "masq", "1")
		end
	end)

	local ret2 = x:commit(FIREWALL_CONFIG_FILE)

	return ret1 and ret2
end

modem_module.modem_mutilapn_status = {
	["ipaddr"]  = nil,      		--string
	["type"] = nil, -- wan section like wan,4g,4g1,4g2
	["netmask"]  = nil,	--string
	["ifname"]  = nil,		--string
	["lan_gate_ip"]  = nil,			--string
	["lan_gate_netmask"]  = nil,			--string I
	["ssid"] = nil,			--string 
	["dial_status"] = nil   -- number 0:disconnect 1:connect
}

function modem_module.modem_mutilapn_status:new(o,obj)

	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
	
	self["ipaddr"]  = obj["ipaddr"] or  nil
	self["netmask"]  = obj["netmask"] or  nil
	self["ifname"]  = obj["ifname"] or  nil
	self["lan_gate_ip"]  = obj["lan_gate_ip"] or  nil
	self["lan_gate_netmask"]  = obj["lan_gate_netmask"] or  nil
	self["ssid"]  = obj["ssid"] or  nil
	self["dial_status"]  = obj["dial_status"] or  nil
end

-- get mutilapn status
-- input:none
-- return:then array of mutilapn_status or nil if error 
function modem_module.modem_get_mutilapn_status()
	local f = nil
	local primary_apn = modem_module.modem_mutilapn_status:new(nil,nil)
	local apn1 = modem_module.modem_mutilapn_status:new(nil,nil)
	local apn2 = modem_module.modem_mutilapn_status:new(nil,nil)
	local eth = modem_module.modem_mutilapn_status:new(nil,nil)
	local res = nil
	local pointer = nil

	f = io.open(MUTILAPN_INFO_FILE,"r")
	if nil == f
	then
		debug("open ",MUTILAPN_INFO_FILE, " fail")
		return nil
	end

	res = f:read()
	while( res ~= nil)
	do


		local array = split(res,":")
		--print(array[1],"=", array[2])
		local key = string.gsub(array[1],"\t","")
		local value = string.gsub(array[2],"\t","")
		local ar = split(value,",")

		if string.find(key,"APN1") ~= nil
		then
			pointer = primary_apn
		elseif string.find(key,"APN2") ~= nil
		then
			pointer = apn1
		elseif string.find(key,"APN3") ~= nil
		then
			pointer = apn2
		elseif string.find(key,"ETH") ~= nil
		then
			pointer = eth
		end

		pointer["type"] = ar[1]
		pointer["ipaddr"]  = ar[2]
		pointer["netmask"]  = ar[3]
		pointer["ifname"]  = ar[4]
		pointer["lan_gate_ip"]  = ar[5]
		pointer["lan_gate_netmask"]  = ar[6]
		pointer["ssid"] = ar[7]
		if nil == pointer["ipaddr"] or "" == pointer["ipaddr"]
		then
			pointer["dial_status"] =  0
		else
			pointer["dial_status"] =  1
		end

		res = f:read()
	end

	io.close(f)

	return {primary_apn,apn1,apn2,eth}


end

function modem_module.modem_enable_auto_dial()
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_ENABLE_AUTO_DIAL",1)
	local ret1 = x:commit(TOZED_CONFIG_FILE)
	os.execute("/etc/init.d/dialtool2 restart")
	return ret1 
end

function modem_module.modem_disable_auto_dial()
	x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_ENABLE_AUTO_DIAL",0)
	local ret1 = x:commit(TOZED_CONFIG_FILE)
	os.execute("/etc/init.d/dialtool2 restart")
	return ret1
end

function modem_module.modem_get_auto_dial()
	local ret = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_ENABLE_AUTO_DIAL")

	if "1" == ret 
	then 
		return 1
	else
		return 0
	end
end

return modem_module
