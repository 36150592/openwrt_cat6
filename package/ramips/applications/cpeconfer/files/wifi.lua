require("uci")
require("io")
wifi_module = {}
local util=require("tz.util")
local x = uci.cursor()
local WIFI_CONFIG_FILE="wireless"
local WIFI_OPTION_DEVICE="wifi-device"
local WIFI_OPTION_INTERFACE="wifi-ifame"
local WIFI_DRIVE_CONFIG_DIR="/etc/wireless"
local debug = util.debug
local split = util.split 
local sleep = util.sleep

-- get section name (.name) by index 
-- input:
-- 		number:wifi_id --> wifi index get by wifi_get_dev
-- return:
--		string:device type or nil if get fail
local function common_get_section_name_by_index(wifi_id)

	assert(type(wifi_id) == "number", "wifi_id is not a number")

	--debug("wifi_id = ", wifi_id)
	
	local value = nil
	local dev_type = nil
	local all_config = x:get_all(WIFI_CONFIG_FILE)

	if nil == all_config 
	then
		debug("all_config = nil")
	end

	for k,v in pairs(all_config) do
		if wifi_id == v[".index"] then
			--debug("get index == wifi_id")
			if nil == v[".name"]
			then
				debug("error:device type is nil. ")
				return nil
			end
			return tostring(v[".name"])
		end
	end

	debug("not found")
	return nil
end

-- get ifame section name (.name) by index 
-- input:
-- 		number:wifi_id --> wifi index get by wifi_get_dev
-- return:
--		string:section name or nil if get fail
local function common_get_ifame_section_name_by_index(wifi_id)
	assert(type(wifi_id) == "number", "wifi_id is not a number")

	--debug("wifi_id = ", wifi_id)

	
	local value = nil
	local dev_type = nil
	local all_config = x:get_all(WIFI_CONFIG_FILE)

	if nil == all_config 
	then
		debug("all_config = nil")
	end

	for k,v in pairs(all_config) do
		if wifi_id == v[".index"] then
			--debug("get index == wifi_id")
			dev_type = v["type"]
			for key, value in pairs(all_config) do
				if dev_type == value["device"] then
					return tostring(value[".name"])
				end
			end
		end
	end

	return nil
end
-- set option value to /etc/config/ , if set fail,set it to driver dat file
-- input:
-- 		wifi_id --> wifi index get by wifi_get_dev  or section name 
--		option  --> config option in config file e.g /etc/config/wireless 
--		dat_opton --> option in wifi drive dat file  e.g /etc/wireless/mt7603e/mt7603e.dat
-- return:
--		boolean true if success false if fail

local function common_config_set(wifi, option, dat_option, value)
	local ret = nil
	local section_name = nil
	if type(wifi) == "number"
	then
			
		debug("wifi_id = ", wifi)

		section_name = common_get_section_name_by_index(wifi)
					
		ret = x:set(WIFI_CONFIG_FILE, section_name, option, value)
	elseif type(wifi) == "string"
	then
		debug("section name = ", wifi)
		section_name = wifi
		ret = x:set(WIFI_CONFIG_FILE, section_name, option, value)
	else
		debug("error:not wifi id or section name ", wifi)
	end

	debug("ret = ", tostring(ret))

	if ret
	then
		x:commit(WIFI_CONFIG_FILE)
	else
			local dev_type = x:get(WIFI_CONFIG_FILE, section_name, "type")
			if nil == dev_type
			then
				dev_type = x:get(WIFI_CONFIG_FILE, section_name, "device")
			end

			if nil == dev_type or nil == dat_option
			then
				debug("cannot get dev_type by section_name ", section_name)
				return false
			end
			debug("cannot set section.", option, " -->set the dat file ")
			local cmd = string.format("sed  s/%s=.*/%s=%s/g %s/%s/%s.dat", dat_option, dat_option, value,WIFI_DRIVE_CONFIG_DIR, dev_type, dev_type)
			debug("cmd = ", cmd)
			local t = io.popen(cmd)
			local res = t:read("*all")
			io.close(t)
			return true
		
	end

	return ret
end

-- get option value from /etc/config/ , if not found get it from driver dat file
-- input:
-- 		wifi_id --> wifi index get by wifi_get_dev
--		option  --> config option in config file e.g /etc/config/wireless 
--		dat_opton --> option in wifi drive dat file  e.g /etc/wireless/mt7603e/mt7603e.dat
-- return 
--		string value of option or nil if not found


local function common_config_get(wifi_id, option, dat_option)
	assert(type(wifi_id) == "number", "wifi_id is not a number")

	--debug("wifi_id = ", wifi_id)

	if nil == option 
	then
		debug("option is nil")
		return nil
	end
	
	local value = nil
	local dev_type = nil
	local all_config = x:get_all(WIFI_CONFIG_FILE)

	if nil == all_config 
	then
		debug("all_config = nil")
	end

	for k,v in pairs(all_config) do
		if wifi_id == v[".index"] then
			--debug("get index == wifi_id")
			value = x:get(WIFI_CONFIG_FILE, k, option)
			dev_type = v["type"]
			if nil == value then
				debug("not found  in index section, find ifame section")
				for key, val in pairs(all_config) do
					if dev_type == val["device"] then
						debug("get dev_type ", val["device"])
						value = x:get(WIFI_CONFIG_FILE, key, option)
						break
					end

				end
			else
				break
			end
		end

	end

	if ( nil == value )
	then
		if   dev_type ~= nil and nil ~= dat_option and "" ~= dat_option
		then
		
			--debug(" is nil,get  from dat file ")
			local cmd = string.format("grep %s %s/%s/%s.dat  | cut -d'=' -f 2", dat_option, WIFI_DRIVE_CONFIG_DIR, dev_type, dev_type)
			debug("cmd = ", cmd)
			local t = io.popen(cmd)
			local res = t:read("*all")
			value = res
			io.close(t)
		else
			debug("dev_type = nil and dat_option = nil, can not get")
			return nil
		end
	end
	

	return string.gsub(value,"\n","")
end

--<------------	1	------>
--.name	 = 	mt7603e
--.anonymous	 = 	false
--ht	 = 	20
--band	 = 	2.4G
--.index	 = 	0
--country	 = 	CN
--channel	 = 	auto
--.type	 = 	wifi-device
--mode	 = 	7
--txpower	 = 	100
--type	 = 	mt7603e
--<------------	2	------>
--band	 = 	5G
--type	 = 	mt7612e
--country	 = 	CN
--vendor	 = 	ralink
--aregion	 = 	0
--autoch	 = 	0
--.index	 = 	2
--bw	 = 	2
--channel	 = 	149
--.name	 = 	mt7612e
--.type	 = 	wifi-device
--txpower	 = 	100
--.anonymous	 = 	false

wifi_module.device = {
	["band"]	 = 	nil,
	["txpower"]	 = 	nil,
	["type"]	 = 	nil,
	["ht"]	 = 	nil,
	["channel"]	 = 	nil,
	["mode"]	 = 	nil,
	["wifi_id"]	 = 	nil,
	["dev_name"]	 = 	nil,
	["ssid"]	 = 	nil,
	["encryption"] = nil,
	["interface_name"] = nil,
	["hidden_ssid"] = nil,
	["password"] = nil,
	["encry_algorithms"] = nil,
	["connect_sta_number"] = nil,
}

function wifi_module.device:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
	
	self["band"]	 = 	obj["band"] or nil
	self["txpower"]	 = 	obj["txpower"] or nil
	self["type"]	 = 	obj["type"] or nil
	self["ht"]	 = 	obj["ht"] or nil
	self["channel"]	 = 	obj["channel"] or nil
	self["mode"]	 = 	obj["mode"] or nil
	self["wifi_id"]	 = 	obj["wifi_id"] or nil
	self["dev_name"]	 = 	obj["dev_name"] or nil
	self["ssid"]	 = 	obj["ssid"] or nil
	self["encryption"] = obj["encryption"] or nil
	self["interface_name"] = obj["interface_name"] or nil	
	self["hidden_ssid"] = obj["hidden_ssid"] or nil
	self["password"] = obj["password"] or nil
	self["encry_algorithms"] = obj["encry_algorithms"] or nil
	self["connect_sta_number"] = obj["connect_sta_number"] or nil
   return o
end


local Device = wifi_module.device
function wifi_module.wifi_get_dev()

	dev_array = {}
	var = 1
	
    x:foreach(WIFI_CONFIG_FILE, WIFI_OPTION_DEVICE, function(s)
		
		temp = Device:new(nil,nil)
		for k,v in pairs(s) do
			repeat
				if (k == ".anonymous" 
					or k == ".type" 
					or k == "country" 
					or k == "vendor" 
					or k == "aregion" 
					or k == "autoch"
					or k == "bw")
				then
					break
				elseif (".name" == k )
				then
					temp["dev_name"] = v
				elseif (k == ".index" )
				then
					temp["wifi_id"] = v
				else
					temp[k] = v
				end
			until true
			
		end
		dev_array[var] = temp
		var = var+1
    end)

	x:foreach(WIFI_CONFIG_FILE, WIFI_OPTION_INTERFACE, function(s)


		p_dev = nil

		for j =1,table.maxn(dev_array) do
			temp1 = dev_array[j]

			
			--print("device = ", s["device"])
			--print("temp1_device = ", temp1["type"])
			if (s["device"] == temp1["type"]) 
			then
				p_dev =dev_array[j]
				break
			end

		end

		if (p_dev == nil) 
		then
			p_dev = Device:new(nil,nil)
			dev_array[var] = p_dev
			var = var +1
		end
		
		for k,v in pairs(s) do
			repeat
				if (k == ".anonymous" 
					or k == ".type" 
					or k == "mode" 
					or k == "network" 
					or k == "device" 
					or k == "autoch"
					or k == "bw"
					or k == ".name"
					or k == ".index")
				then
					break
				elseif ("ifname" == k )
				then
					p_dev["interface_name"] = v
				else
					p_dev[k] = v
				end
			until true
			
		end
    end)



    
	--show_dev(dev_array)
	return dev_array
	
end

-- wifi must be enable status when use this interface
function wifi_module.wifi_start(wifi_id)
	local section_name = common_get_section_name_by_index(wifi_id)
	local dev_type = x:get(WIFI_CONFIG_FILE, section_name,"type")
	if nil == dev_type
	then
		debug("start fail for dev_type unknown")
	end
    return os.execute("wifi start " .. dev_type)
end

-- wifi must be enable status when use this interface
function wifi_module.wifi_start_all()
     return os.execute("wifi  ")
end

-- wifi must be enable status when use this interface
function wifi_module.wifi_restart(wifi_id)
	local section_name = common_get_section_name_by_index(wifi_id)
	local dev_type = x:get(WIFI_CONFIG_FILE, section_name,"type")
	if nil == dev_type
	then
		debug("start fail for dev_type unknown")
	end
    os.execute("wifi down " .. dev_type)
    sleep(1)
    return os.execute("wifi start " .. dev_type)
end

function wifi_module.wifi_restart_all()
    os.execute("wifi  down")
    sleep(1)
    return os.execute("wifi")
end

function wifi_module.wifi_stop(wifi_id)
    local section_name = common_get_section_name_by_index(wifi_id)
	local dev_type = x:get(WIFI_CONFIG_FILE, section_name,"type")
	if nil == dev_type
	then
		debug("start fail for dev_type unknown")
	end
    return os.execute("wifi down " .. dev_type)
end

function wifi_module.wifi_stop_all()
   	return os.execute("wifi  down")
end

-- enable wifi self boot 
-- input:wifi id get by wifi_get_dev
-- return: true if success false if fail
function wifi_module.wifi_enable(wifi_id)
	debug("set wifi self start at boot")
    -- set the ifame section
	local section_name = common_get_ifame_section_name_by_index(wifi_id)
	return common_config_set(section_name, "disabled", "", 0)
end

-- is the wifi start of stop
-- input:wifi_id get by wifi_get_dev
-- return:true if wifi is started false if wifi is stoped
function wifi_module.wifi_is_start(wifi_id)

	--debug("wifi_is_start")
	local section_name = common_get_ifame_section_name_by_index(wifi_id)
    local ifname = x:get(WIFI_CONFIG_FILE, section_name, "ifname")
	local f = io.popen(string.format("iwconfig %s | grep ESSID | wc -l", ifname))
	local res = f:read()
	io.close(f)

	if "1" == res
	then
		return true
	end

	return false
end

-- disable wifi self boot 
-- input:wifi id get by wifi_get_dev
-- return: true if success false if fail
function wifi_module.wifi_disable(wifi_id)
	--debug("disable wifi self start at boot")
    -- set the ifame section
	local section_name = common_get_ifame_section_name_by_index(wifi_id)
	return common_config_set(section_name, "disabled", "", 1)
end

-- get wifi self boot config
--input:wifi id get by wifi_get_dev
--return:
--		1:disable
--		0:enable
--		nil:get fail
function wifi_module.wifi_get_enable_status(wifi_id)
	local section_name = common_get_ifame_section_name_by_index(wifi_id)

	if nil == section_name 
	then
		debug("error:can not get section name ")
		return nil
	end

	local ret = x:get(WIFI_CONFIG_FILE, section_name, "disabled")

	if nil == ret
	then
		debug("ret is nil ,return default value")
		return "0"
	end

	return ret
end

--get ssid
-- input:
--		wifi_id(number) get by wifi_get_dev
-- return:string
--		the wifi ssid
function wifi_module.wifi_get_ssid(wifi_id)
    return  common_config_get(wifi_id, "ssid", "SSID")
end

--set ssid
-- input:
--		wifi_id(number) -->get by wifi_get_dev
--		ssid(string)
-- return:boolean
--		true if success  false if fail
function wifi_module.wifi_set_ssid(wifi_id,ssid)
     debug("set ssid")
    -- set the ifame section
	local section_name = common_get_ifame_section_name_by_index(wifi_id)
	return common_config_set(section_name, "ssid", "SSID", ssid)
end


--get password
-- input:
--		wifi_id(number) get by wifi_get_dev
-- return:string
--		the wifi password
function wifi_module.wifi_get_password(wifi_id)

	--when is wep encryption the dat_option is DefaultKeyID  ,and the key group(key1 key2 key3 key4) is also set 	
    return  common_config_get(wifi_id, "key", "WPAPSK")
end

--set password
-- input:
--		wifi_id(number) -->get by wifi_get_dev
--		password(string)
-- return:boolean
--		true if success  false if fail
function wifi_module.wifi_set_password(wifi_id,password)
   debug("set password")
    -- set the ifame section
	local section_name = common_get_ifame_section_name_by_index(wifi_id)
	--when is wep encryption the dat_option is DefaultKeyID  ,and the key group(key1 key2 key3 key4) is also set 
	return common_config_set(section_name, "key", "WPAPSK", password)
end

--channel
--txpower
-- input:
--		wifi_id(number) get by wifi_get_dev
-- return:string
--		the wifi channel
function wifi_module.wifi_get_channel(wifi_id)
    return  common_config_get(wifi_id, "channel", "Channel")
end

--channel
-- input:
--		wifi_id(number) -->get by wifi_get_dev
--		channel(string) 2.4G [auto 0-13] 5.8G [36 40 44 48 52 56 60 64 149 153 157 161 165]
-- return:boolean
--		true if success  false if fail
function wifi_module.wifi_set_channel(wifi_id,channel)

	local section_name = common_get_section_name_by_index(wifi_id)
	local band = x:get(WIFI_CONFIG_FILE, section_name, "band")
	if '2.4G' == band
	then 
	
		if "auto" == channel or "0" == channel or "1" == channel or "2" == channel or "3" == channel or
			"4" == channel or "5" == channel or "6" == channel or "7" == channel or "8" == channel or
			"9" == channel or "10" == channel or "11" == channel or "12" == channel or "13" == channel 
		then
			return common_config_set(wifi_id, "channel", "Channel", channel)
		end
	elseif '5G' == band
	then
		
		if
			"36" == channel or "40" == channel or "44" == channel or "48" == channel or "52" == channel or
			"56" == channel or "60" == channel or "64" == channel or "149" == channel or "153" == channel or
			"157" == channel or "161" == channel or "165" == channel 
		then
			return common_config_set(wifi_id, "channel", "Channel", channel)
		end
	end

    debug("input error: 2.4G [auto 0-13] 5.8G [36 40 44 48 52 56 60 64 149 153 157 161 165]")
	return false
end

--txpower
-- input:
--		number wifi_id get by wifi_get_dev
-- return:
-- 		number
--		the number of txpower percent range from 0 to 100
function wifi_module.wifi_get_txpower(wifi_id)
    local txpower =  common_config_get(wifi_id, "txpower", "TxPower")
    return tonumber(txpower)
end

--txpower
-- input:
--		wifi_id(number) -->get by wifi_get_dev
--		txpower(number) [0 ,100] wifi tx power in percent
-- return:boolean
--		true if success  false if fail
function wifi_module.wifi_set_txpower(wifi_id,txpower)
	if type(txpower) ~= "number" 
	then
		debug("input error: txpower not number")
		return false
	end

	if txpower < 0 or txpower > 100
	then
		debug("input error: txpower must  range [0,100]")
		return false
	end
	
    return common_config_set(wifi_id, "txpower", "TxPower", txpower)
end

--hidden ssid
-- input:
--		number wifi_id get by wifi_get_dev
-- return:
-- 		boolean
--		true if success  false if fail
function wifi_module.wifi_enable_hidden_ssid(wifi_id)
	debug("enable hidden ssid")
	-- hidden ssid , set the ifame section
	local section_name = common_get_ifame_section_name_by_index(wifi_id)
	return common_config_set(section_name, "hidden", "HideSSID", 1)
end

--hidden ssid
-- input:
--		number wifi_id get by wifi_get_dev
-- return:
-- 		boolean
--		true if success  false if fail
function wifi_module.wifi_disable_hidden_ssid(wifi_id)
	debug("disable hidden ssid")
    -- hidden ssid , set the ifame section
	local section_name = common_get_ifame_section_name_by_index(wifi_id)
	return common_config_set(section_name, "hidden", "HideSSID", 0)
end



-- get hidden ssid config 
-- input:
-- 		wifi_id  get by wifi_get_dev
-- return: string value as follow
-- 		0:disable
-- 		1:enable
function wifi_module.wifi_get_hidden_ssid(wifi_id)
	return common_config_get(wifi_id, "hidden", "HideSSID")
end




--get wireless mode 802.11 n/g/bs
-- input: wifi_id get by wifi_get_dev
-- return:
--		number as follow:
--0: legacy 11b/g mixed
--1: legacy 11b only
--2: legacy 11a only
--3: legacy 11a/b/g mixed
--4: legacy 11g only
--5: 11abgn mixed
--6: 11n only in 2.4g band
--7: 11gn mixed
--8: 11an mixed
--9: 11bgn mixed
--10: 11AGN mixed
--11: 11n only in 5g band
--14: 11A/AN/AC mixed 5G band only (Only 11AC chipset support)
--15: 11 AN/AC mixed 5G band only (Only 11AC chipset support)
function wifi_module.wifi_get_mode(wifi_id)
    return common_config_get(wifi_id, "mode", "WirelessMode")
end

--set wireless mode 802.11 n/g/bs
-- input: 
--		wifi_id get by wifi_get_dev
--		mode number as above
-- return:
--		boolean true if success false if fail
function wifi_module.wifi_set_mode(wifi_id,mode)
   if type(mode) ~= "number"
   then
		debug("input error: not a number")
   end

   if mode > 15 or mode < 0
   then 
		debug("input error: mode must [0,15]")
   end
   return common_config_set(wifi_id, "mode", "WirelessMode", mode)
end

--get HT mode
-- input:
--		wifi_id get by wifi_get_dev
-- return:
--		string value as follow:
-- 20 -->20M
-- 40 -->40M
-- 20+40 -->20/40M
function wifi_module.wifi_get_ht_mode(wifi_id)
   return common_config_get(wifi_id, "ht", "HT_BW")
end

--set HT mode
-- input:
--		wifi_id get by wifi_get_dev
--		mode string value as above
-- return:
--		boolean true if success false if fail
function wifi_module.wifi_set_ht_mode(wifi_id,mode)
	if "20" == mode or "40" == mode or "20+40" ==mode
	then
	    return common_config_set(wifi_id, "ht", "HT_BW", mode)
	else
		debug("input error: must be [20 40 20+40]")
		return false
	end
end


--get encryption mode
-- input:
--		wifi_id get by wifi_get_dev
-- return:
--		string value as follow:
--		none   --> none or open
-- 		wep -->wep wep-open wep-shared
-- 		psk -->wap-psk wap psk
-- 		psk2 -->wap2-psk wap2 psk2
-- 		mixed --> psk+psk2/wap-psk/wap2-psk mixed
-- e.g
--	option encryption 'wep-open'
--	option key '1'
--	option key1 '1234567890'
--	option key2 '1234567890'
--	option key3 '1234567890'
--	option key4 '1234567890'
function wifi_module.wifi_get_encryption(wifi_id)
	local encry_all = common_config_get(wifi_id, "encryption", "AuthMode")

	--debug("encry_all = ", encry_all)
	local start, endp = string.find(encry_all, "+")

    --debug("start = ", start, "endp = ", endp)
    -- no encryption type return directly
    if nil == start 
    then
		return encry_all
    else

		local encry = string.sub(encry_all, 1, start-1)

		debug("encry = ", encry)

		return encry
   	end
end

--set encryption
-- input:
--		wifi_id get by wifi_get_dev
--		encryption string value as above
-- return:
--		boolean true if success false if fail
function wifi_module.wifi_set_encryption(wifi_id,encryption)
	if type(encryption) ~= "string" 
	then 
		debug("encryption not string,input error")
		return false
	end

	if "none" == encryption or "wep" == encryption or "psk" == encryption or
		"psk2" == encryption or "mixed" == encryption 
	then

	debug("set encryption")
    -- set the ifame section
	local section_name = common_get_ifame_section_name_by_index(wifi_id)
	return common_config_set(section_name, "encryption", "AuthMode", encryption)

	else
		debug("input error:must be [none wep psk psk2 mixed]")
		return false
	end
end

-- get encrytion algorithms
-- input:number
--		wifi_id get by wifi_get_dev 
-- return:string
--		ccmp+tkip  -->TKIPAES
--		ccmp	-->  AES
--		tkip -->TKIP
--		none --> AUTO
function wifi_module.wifi_get_encryption_type(wifi_id)
	
	local encry_all = common_config_get(wifi_id, "encryption", "EncrypType")

	local start, endp = string.find(encry_all, "+")

    --debug("start = ", start, "endp = ", endp)

	if nil == start
	then 
		return "none"
	end

	local encry_type = string.sub(encry_all, start+1)

	--debug("encry_type = ", encry_type)

	return encry_type
end

-- set encrytion algorithms
-- input:string
--		ccmp+tkip  -->TKIPAES
--		ccmp	-->  AES
--		tkip -->TKIP
--		none --> AUTO
-- return:
--		boolean true if success false if fail
function wifi_module.wifi_set_encryption_type(wifi_id,encry_algorithms)
  	debug("set encryption")
  	if type(encry_algorithms) ~= "string" 
	then 
		debug("encry_algorithms not string,input error")
		return false
	end
	
	if "none" == encry_algorithms or "ccmp+tkip" == encry_algorithms 
		or "ccmp" == encry_algorithms or "tkip" == encry_algorithms 
	then
	    --  set the ifame section
		local encry = wifi_module.wifi_get_encryption(wifi_id)
		encry_algorithms = encry .. "+" .. encry_algorithms 
	    
		local section_name = common_get_ifame_section_name_by_index(wifi_id)

		return common_config_set(section_name, "encryption", "EncrypType", encry_algorithms)
	else
		debug("input error:must be [none ccmp+tkip ccmp tkip]")
		return false
	end
end



--
-- get connect sta number
-- input:number
--		wifi_id get by wifi_get_dev 
-- return:number 
function wifi_module.wifi_get_connect_sta_number(wifi_id)
   local sta_number = common_config_get(wifi_id, "maxassoc", "MaxStaNum")
   return tonumber(sta_number)
end

-- set connect sta number
-- input:number
--		wifi_id --> get by wifi_get_dev
--		number --> the number of sta limit number
-- return:
--		boolean true if success false if fail
function wifi_module.wifi_set_connect_sta_number(wifi_id,number)
    debug("set connect sta number")
  	if type(number) ~= "number" 
	then 
		debug(" not number,input error")
		return false
	end
	
    --  set the ifame section
    
	local section_name = common_get_ifame_section_name_by_index(wifi_id)

	return common_config_set(section_name, "maxassoc", "MaxStaNum", number)

end


-- enable wmm
-- input:
--		wifi_id(number) get by wifi_get_dev
-- return:boolean
--		 true if success false if fail
function wifi_module.wifi_enable_wmm(wifi_id)
    debug("enable wmm")
	-- set the ifame section
	local section_name = common_get_ifame_section_name_by_index(wifi_id)
	return common_config_set(section_name, "wmm", "WmmCapable", 1)
end

-- disable wmm
-- input:
--		wifi_id(number) get by wifi_get_dev
-- return:boolean
--		 true if success false if fail
function wifi_module.wifi_disable_wmm(wifi_id)
     debug("disable wmm")
	-- set the ifame section
	local section_name = common_get_ifame_section_name_by_index(wifi_id)
	return common_config_set(section_name, "wmm", "WmmCapable", 0)
end

-- get wmm setting
-- input:
--		wifi_id(number) get by wifi_get_dev
-- return:number
--		wmm 0 disable 1 enable
function wifi_module.wifi_get_wmm(wifi_id)
   local wmm = common_config_get(wifi_id, "wmm", "WmmCapable")
   return tonumber(wmm)
end



wifi_module.station = {
	["mac"]	 = 	nil,
	["dev_name"]	 = 	nil,
	["interface_name"]	 = 	nil,
	["wifi_id"]	 = 	nil,
	["bandwidth"]	 = 	nil,
	["rate"]	 = 	nil,
	["ssid"]	 = 	nil,
	["tx_packets"] = nil,
	["rx_packets"] = nil,
	["tx_bytes"] = nil,
	["rx_bytes"] = nil
}

function wifi_module.station:new(o,obj)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
	
	self["mac"]	 = obj["mac"] or nil
	self["dev_name"]	 = 	obj["dev_name"] or nil
	self["interface_name"]	 = 	obj["interface_name"] or nil
	self["wifi_id"]	 = 	obj["wifi_id"] or nil
	self["bandwidth"]	 = 	obj["bandwidth"] or nil
	self["rate"]	 = 	obj["rate"] or nil
	self["ssid"]	 = 	obj["ssid"] or nil
	self["tx_packets"] = obj["tx_packets"] or nil
	self["rx_packets"] = obj["rx_packets"] or nil
	self["tx_bytes"] = obj["tx_bytes"] or nil
	self["rx_bytes"] = obj["rx_bytes"] or nil
   return o
end

local Station=wifi_module.station


local function get_sta_mac_table(ifname,sta_list)

	local cmd = nil
	local t = nil
	cmd = string.format("iwpriv %s show stainfo ", ifname)
	--debug("cmd1 = ", cmd)
	
	t = io.popen(cmd)
	io.close(t)
	sleep(1)
	cmd = string.format("dmesg | tail -n 100")
	--debug("cmd2 = ", cmd)
	t = io.popen(cmd)
	local res = nil

	res = t:read()
	while( res ~= nil)
	do
		local start,endp =string.find(res,"MAC.+Rate   QosMap")
		if nil ~= start
		then
			
			--debug("get sta mac table at ", start)
			
			while(res ~= nil)
			do
				res = t:read()
				if nil == res
				then
					break
				end

				res = string.sub(res,start)
				if string.match(res,"%x%x:%x%x:%x%x:%x%x:%x%x:%x%x") == nil
				then
					break
				end
				
				res = string.gsub(res, '                ','#')
				res = string.gsub(res, '         ', '#')
				res = string.gsub(res, '        ', '#')
				res = string.gsub(res, '       ', '#')
				res = string.gsub(res, '      ', '#')
				res = string.gsub(res, '     ', '#')
				res = string.gsub(res, '    ', '#')
				res = string.gsub(res, '   ', '#')
				res = string.gsub(res, '  ', '#')
				res = string.gsub(res, ' ', '#')
				print(res)
				local s_array = split(res,'#')

				--[[for v,k in pairs(s_array)
				do
					print(v,"=",k)
				end]]--
				
				--debug("MAC = ", s_array[1])
				--debug("RSSI = ", s_array[7])
				--debug("RATE = ", s_array[14])

				if string.len(s_array[1]) == 17 and string.find(s_array[1], ':') > 0
				then
					local flag = false
					for key,value in pairs(sta_list)
					do
						if value["mac"] == s_array[1]
						then
							value["rate"] = s_array[14]
							flag = true
						end
						
					end
					if flag == false
					then
						--debug("flag = false ,new Station")
						local temp_sta = Station:new(nil,nil)
						temp_sta["mac"] = s_array[1]
						temp_sta["rate"] = s_array[14]
						sta_list[table.maxn(sta_list)+1] = temp_sta
					end
				end
			
			end

		end
		res =t:read()
	end

	io.close(t)
end

local function get_sta_count_info(ifname,sta_list)

	
	local cmd = nil
	local t = nil
	cmd = string.format("iwpriv %s show stacountinfo ", ifname)
	--debug("cmd1 = ", cmd)
	t = io.popen(cmd)
	io.close(t)
	sleep(1)
	
	cmd = string.format("dmesg | tail -n 100")
	--debug("cmd2 = ", cmd)
	t = io.popen(cmd)
	local res = nil

	res = t:read()
	while( res ~= nil)
	do
		local start,endp =string.find(res,"MAC.+TxPackets   RxPackets   TxBytes     RxBytes")
		
		if nil ~= start
		then
			
			--debug("get sta count table at", start)
			while(res ~= nil)
			do
				res = t:read()
				if nil == res
				then
					break
				end

				res = string.sub(res,start)
				if string.match(res,"%x%x:%x%x:%x%x:%x%x:%x%x:%x%x") == nil
				then
					break
				end
				
				res = string.gsub(res, '                ','#')
				res = string.gsub(res, '         ', '#')
				res = string.gsub(res, '        ', '#')
				res = string.gsub(res, '       ', '#')
				res = string.gsub(res, '      ', '#')
				res = string.gsub(res, '     ', '#')
				res = string.gsub(res, '    ', '#')
				res = string.gsub(res, '   ', '#')
				res = string.gsub(res, '  ', '#')
				res = string.gsub(res, ' ', '#')

				local s_array = split(res,'#')
				--debug(res)
				--[[for k,v in pairs(s_array)
				do
					print(k,"=",v)
				end]]--
				
				--debug("tx packets = ", s_array[3])
				--debug("rx packets = ", s_array[4])
				--debug("tx bytes= ", s_array[5])
				--debug("rx bytes= ", s_array[6])

				if string.len(s_array[1]) == 17
				then
					
					local flag = false
					for key,value in pairs(sta_list)
					do
						if value["mac"] == s_array[1]
						then
							value["tx_packets"]=s_array[3]
							value["rx_packets"]=s_array[4]
							value["tx_bytes"]=s_array[5]
							value["rx_bytes"]=s_array[6]
							flag = true
							break
						end
					end
					
					if flag == false
					then
						local temp_sta = Station:new(nil,nil)
						temp_sta["mac"] = s_array[1]
						temp_sta["tx_packets"] = s_array[3]
						temp_sta["rx_packets"] = s_array[4]
						temp_sta["tx_bytes"] = s_array[5]
						temp_sta["rx_bytes"] = s_array[6]
						sta_list[table.maxn(sta_list)] = temp_sta
					end
				end
			
			end
			
			
		

		end
		res = t:read()
	end
	io.close(t)
end

--connect sta list
function wifi_module.wifi_get_connect_sta_list(wifi_id)
	--debug("wifi_get_connect_sta_list")
    local section_name = common_get_ifame_section_name_by_index(wifi_id)
    local ifname = x:get(WIFI_CONFIG_FILE, section_name, "ifname")
	local dev_name = x:get(WIFI_CONFIG_FILE, section_name, "device")
	local ssid = x:get(WIFI_CONFIG_FILE, section_name, "ssid")
	
	local redo_count = 3
	if nil == ifname
	then 
		debug("cannot get ifname")
		return nil
	end
	
  	station_list = {}
  	get_sta_mac_table(ifname, station_list)
	
	print(table.maxn(station_list))

	get_sta_count_info(ifname, station_list)

	print(table.maxn(station_list))

	for k,v in pairs(station_list)
	do
		v["interface_name"] = ifname
		v["dev_name"] = dev_name
		v["ssid"] = ssid
	end

	return station_list
	
end
 
return wifi_module