require("uci")
require("io")
sim_module = {}
local util=require("tz.util")
local x = uci.cursor()
local TOZED_CONFIG_FILE="tozed"
local UCI_SECTION_DIALTOOL2="modem"
local SIM_CONFIG_FILE="network"
local SIM_DYNAMIC_STATUS_PATH="/tmp/.system_info_dynamic"
local SIM_SEND_AT_CMD="sendat -d/dev/ttyUSB1 -f/tmp/at_send -o/tmp/at_recv"
local SIM_AT_SEND_FILE="/tmp/at_send"
local SIM_AT_RECV_FILE="/tmp/at_recv"
local SIM_PIN_ENABLE_FILE="/tmp/.sim_pin_enable"
local debug = util.debug
local split = util.split 
local sleep = util.sleep


sim_module.sim_status = {
	["iccid"]  = nil,      		--string
	["imsi"]  = nil,	--string
	["card_status"]  = nil,		--number 1:pin lock 2:puk lock 0:ready
	["is_sim_exist"]  = nil,			--string
	["pinlock_enable"]  = nil, 			--string 0:disable 1:enable
	["pin_left_times"]  = nil,			--string
	["puk_left_times"]  = nil 			--string
}


function sim_module.sim_status:new(o,obj)

	o = o or {}
	setmetatable(o, self)
	self.__index = self
	if obj == nil then
		return o
	end
	
	self["iccid"]  = obj["iccid"] or  nil
	self["imsi"]  = obj["imsi"] or  nil
	self["card_status"]  = obj["card_status"] or  nil
	self["is_sim_exist"]  = obj["is_sim_exist"] or  nil
	self["pinlock_enable"]  = obj["pinlock_enable"] or  nil
	self["pin_left_times"]  = obj["pin_left_times"] or  nil
	self["puk_left_times"]  = obj["puk_left_times"] or  nil
end

local function clear_file(path)
	return 0 == os.execute(string.format("> %s", path))
end

local function format_enable_pin_lock_at_cmd(pin_passwd)return string.format('AT+CLCK="SC",1,"%s"', pin_passwd) end
local function format_disable_pin_lock_at_cmd(pin_passwd)return string.format('AT+CLCK="SC",0,"%s"', pin_passwd) end
local function format_get_pin_lock_at_cmd()return 'AT+CLCK="SC",2,""' end
local function format_pin_unlock_at_cmd(pin_passwd)return string.format('AT+CPIN=%s', pin_passwd) end
local function format_pin_change_at_cmd(pin_passwd,new_pin)return string.format('AT+CPWD="SC","%s","%s"', pin_passwd, new_pin) end
local function format_puk_unlock_at_cmd(puk,new_pin)return string.format('AT+CPIN="%s","%s"', puk, new_pin) end
local function format_get_rev_cmd(file)return string.format('cat %s | grep OK | wc -l', file) end

local function send_at_cmd(cmd)
	debug("send_at_cmd cmd = ",cmd)
	local tzlib = require("luatzlib")
	local res = tzlib.send_at(cmd)
	debug("at result = ",res)
	return res
end

local function get_pin_lock_enable_status()

	local f = io.popen("cat ".. SIM_PIN_ENABLE_FILE)
	if nil ~= f
	then
		local res = f:read()
		io.close(f)
		if "1" == res or "0" == res
		then
			return res
		end
	end

	local ret = send_at_cmd(format_get_pin_lock_at_cmd())
	if nil ~= ret
	then
		local f = string.find(ret, "+CLCK: 0")
		if f ~= nil
		then
			os.execute("echo -n '0' > "..SIM_PIN_ENABLE_FILE)
			return "0"
		end
	end

	os.execute("echo -n '1' > "..SIM_PIN_ENABLE_FILE)
	return "1"
end

function sim_module.sim_reload()
	if send_at_cmd("AT+CFUN=0")
	then
		return send_at_cmd("AT+CFUN=1")
	end
	return false
end

local function  get_card_status()
	local res = send_at_cmd("AT+CPIN?")
	if nil ~= res
	then
	
			if string.find(res,"SIM PIN") ~= nil
			then
				return 1
			elseif string.find(res,"PUK") ~= nil
			then
				return 2
			elseif string.find(res,"READY") ~= nil
			then 
				return 0
			else
				return -1
			end
	end

	return -1

end

-- must start the dialtool_new
function sim_module.sim_get_status()
	local f = nil
	local status = nil
	local res = nil

	f = io.open(SIM_DYNAMIC_STATUS_PATH,"r")

	if nil == f
	then
		debug("open ",SIM_DYNAMIC_STATUS_PATH, " fail")
		return nil
	end

	res = f:read()

	while( res ~= nil)
	do
		if nil == status
		then
			status = sim_module.sim_status:new(nil,nil)
			status["card_status"] = 0
		end

		local array = split(res,":")
		--print(array[1],"=", array[2])
		if nil ~= array[1] and nil ~= array[2]
		then
	 		local key = string.gsub(array[1],"\t","")
			local value = string.gsub(array[2],"\t","")
			key = string.lower(key)
			
			if  "iccid" == key
			then
				status["iccid"] = value
			elseif "imsi" == key
			then
				status["imsi"] = value
			elseif "lock_pin_flag" == key
	           then
	               if value == "1"
	               then 
	                       status["card_status"] = 1
	               end
	        elseif "lock_puk_flag" == key
	        then
	               if value == "1"
	               then 
	                       status["card_status"] = 2
	               end
			elseif "is_sim_exist" == key
			then
				status["is_sim_exist"] = value
			elseif "pin_left_times" == key
			then
				status["pin_left_times"] = value
			elseif "puk_left_times" == key
			then
				status["puk_left_times"] = value
			end
		end
		
		res = f:read()
	end

	--status["card_status"] = get_card_status()
	status["pinlock_enable"] = get_pin_lock_enable_status()


	io.close(f)

	return status

end


-- only when the usim card is ready status can this interface be invoked
function sim_module.sim_pin_lock_enable(pin_passwd)

	if nil == pin_passwd
	then
		debug("error:pin_passwd nil")
		return false
	end

	local res =  send_at_cmd(format_enable_pin_lock_at_cmd(pin_passwd))

	if res ~= nil and string.find(res,"OK")
	then
		os.execute("echo -n '1' > "..SIM_PIN_ENABLE_FILE)
		return true
	else
		return false
	end

end


-- only when the usim card is ready status can this interface be invoked
function sim_module.sim_pin_lock_disable(pin_passwd)
	
	if nil == pin_passwd
	then
		debug("error:pin_passwd nil")
		return false
	end

	local res = send_at_cmd(format_disable_pin_lock_at_cmd(pin_passwd))

	if res ~= nil and string.find(res,"OK")
	then
		os.execute("echo -n '0' > "..SIM_PIN_ENABLE_FILE)
		return true
	else
		return false
	end
end

function sim_module.sim_pin_unlock(pin_passwd)
	
	if nil == pin_passwd
	then
		debug("error:pin_passwd nil")
		return false
	end

	local res =  send_at_cmd(format_pin_unlock_at_cmd(pin_passwd))
	if res ~= nil and string.find(res,"OK")
	then
		return true
	else
		return false
	end

end

--change pin code, restart cfun to take effecct
--input:
--		pin_passwd:string ,the usim pin code like 1234
--		new_pin:string , the new pin you want to set like 1234
--return:true if success OR false if fail
function sim_module.sim_pin_change(pin_passwd,new_pin)
	
	if nil == pin_passwd or nil == new_pin
	then
		debug("error:pin_passwd nil")
		return false
	end

	local res =  send_at_cmd(format_pin_change_at_cmd(pin_passwd, new_pin))

	if res ~= nil and string.find(res,"OK")
	then
		return true
	else
		return false
	end

end


--unlock puk setting, restart cfun to take effecct
--input:
--		puk:string ,the usim puk code
--		new_pin:string , the new pin you want to set like 1234
--return:true if success OR false if fail
function sim_module.sim_puk_unlock(puk,new_pin)
	
	if nil == puk or nil == new_pin
	then
		debug("error:puk nil")
		return false
	end

	local res =  send_at_cmd(format_puk_unlock_at_cmd(puk, new_pin))
	if res ~= nil and string.find(res,"OK")
	then
		return true
	else
		return false
	end

end

--get pin remember setting, restart the dialtool_new to take effecct
--input:
--		enable:string 1:remember the pin and auto unlock pin 0:don't auto unlock the pin
--		pin_passwd:string pin password
--return:true if success OR false if fail
function sim_module.sim_pin_set_remember(enable,pin_passwd)

	if "0" == enable 
	then
		x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_ENABLE_PIN",0)
		x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_PIN_CODE", pin_passwd)
	elseif "1" == enable
	then
		x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_ENABLE_PIN",1)
		x:set(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_PIN_CODE", pin_passwd)
	else
		return false
	end

	return x:commit(TOZED_CONFIG_FILE)
end

-- get pin remember setting
-- input:none
-- return,enable(0 or 1) ,pin_code
function sim_module.sim_pin_get_remember()
	local enable = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_ENABLE_PIN")
	local pin_code = x:get(TOZED_CONFIG_FILE, UCI_SECTION_DIALTOOL2, "TZ_DIALTOOL2_PIN_CODE")
	return enable,pin_code
end

return sim_module
