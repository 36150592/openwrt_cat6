require("os")
led_module = {}
local util=require("tz.util")
local LED_SIGNAL="/sys/class/leds/blue:signal"
local LED_SIGNAL1="/sys/class/leds/blue:signal1"
local LED_SIGNAL2="/sys/class/leds/blue:signal2"
local LED_SIGNAL3="/sys/class/leds/blue:signal3"
local LED_SIGNAL4="/sys/class/leds/blue:signal4"
local LED_SIGNAL5="/sys/class/leds/blue:signal5"
local LED_NORMAL_STATUS="/sys/class/leds/blue:status"
local LED_EXCEPTION_STATUS="/sys/class/leds/red:status"
local LED_PHONE="/sys/class/leds/blue:phone"
local LED_WIFI="/sys/class/leds/blue:wifi"
local LED_WIFI_5G="/sys/class/leds/blue:wifi-5g"
local LED_WPS="/sys/class/leds/blue:wps"
local LED_MAX_SIGNAL=5
local debug = util.debug
local split = util.split 
local sleep = util.sleep

local LEDS_ARRAY = {
LED_SIGNAL1,
LED_SIGNAL2,
LED_SIGNAL3,
LED_SIGNAL4,
LED_SIGNAL5,
LED_NORMAL_STATUS,
LED_EXCEPTION_STATUS,
LED_PHONE,
LED_WIFI,
ED_WIFI_5G,
LED_WPS}

local function led_on(path)
	local ret1 = os.execute(string.format("echo none > %s/trigger", path))
	local ret2 = os.execute(string.format("echo 1 > %s/brightness", path))

	return 0 == ret1 and 0 == ret2
end

local function led_off(path)
	local ret1 = os.execute(string.format("echo none > %s/trigger", path))
	local ret2 = os.execute(string.format("echo 0 > %s/brightness", path))

	return 0 == ret1 and 0 == ret2
end

local function led_blink_slow(path)

	local ret = os.execute(string.format("echo timer > %s/trigger", path))

	return 0 == ret
end

local function led_blink_fast(path)

	local ret1 = os.execute(string.format("echo timer > %s/trigger", path))
	local ret2 = os.execute(string.format("echo 200 > %s/delay_on", path))
	local ret3 = os.execute(string.format("echo 200 > %s/delay_off", path))
	return 0 == ret1 and 0 == ret2 and 0 == ret3
end


function led_module.led_set_signal(number)

	if type(number) ~= "number"  or number > LED_MAX_SIGNAL  or number < 0
	then
		debug("input error:must be number range from 1 to ", LED_MAX_SIGNAL)
		return false
	end

	for i = 1,LED_MAX_SIGNAL 
	do
		local ret = false
		if i <= number
		then
			ret = led_on(string.format("%s%d",LED_SIGNAL, i))
		
		else
			ret = led_off(string.format("%s%d",LED_SIGNAL, i))
		end

		if false == ret 
		then 
			return false
		end
	
	end

	return true
end

--normal
function led_module.led_set_normal(action)

	if "on" == action
	then
		return led_on(LED_NORMAL_STATUS)
	elseif "off" == action
	then
		return led_off(LED_NORMAL_STATUS)
	elseif "blink_slow" ==  action
	then
		return led_blink_slow(LED_NORMAL_STATUS)
	elseif "blink_fast" == action
	then
		return led_blink_fast(LED_NORMAL_STATUS)
	else
		debug("input error: action must be one of  on off blink_slow blink_fast")
		return false
	end
end


--exception
function led_module.led_set_exception(action)
	if "on" == action
	then
		return led_on(LED_EXCEPTION_STATUS)
	elseif "off" == action
	then
		return led_off(LED_EXCEPTION_STATUS)
	elseif "blink_slow" ==  action
	then
		return led_blink_slow(LED_EXCEPTION_STATUS)
	elseif "blink_fast" == action
	then
		return led_blink_fast(LED_EXCEPTION_STATUS)
	else
		debug("input error: action must be one of  on off blink_slow blink_fast")
		return false
	end
end



--phone
function led_module.led_set_phone(action)
	if "on" == action
	then
		return led_on(LED_PHONE)
	elseif "off" == action
	then
		return led_off(LED_PHONE)
	elseif "blink_slow" ==  action
	then
		return led_blink_slow(LED_PHONE)
	elseif "blink_fast" == action
	then
		return led_blink_fast(LED_PHONE)
	else
		debug("input error: action must be one of  on off blink_slow blink_fast")
		return false
	end
end


--wifi
function led_module.led_set_wifi(action)
	if "on" == action
	then
		return led_on(LED_WIFI)
	elseif "off" == action
	then
		return led_off(LED_WIFI)
	elseif "blink_slow" ==  action
	then
		return led_blink_slow(LED_WIFI)
	elseif "blink_fast" == action
	then
		return led_blink_fast(LED_WIFI)
	else
		debug("input error: action must be one of  on off blink_slow blink_fast")
		return false
	end
end



--wifi 5g
function led_module.led_set_wifi_5g(action)
	if "on" == action
	then
		return led_on(LED_WIFI_5G)
	elseif "off" == action
	then
		return led_off(LED_WIFI_5G)
	elseif "blink_slow" ==  action
	then
		return led_blink_slow(LED_WIFI_5G)
	elseif "blink_fast" == action
	then
		return led_blink_fast(LED_WIFI_5G)
	else
		debug("input error: action must be one of  on off blink_slow blink_fast")
		return false
	end
end

--wps
function led_module.led_set_wps(action)
	if "on" == action
	then
		return led_on(LED_WPS)
	elseif "off" == action
	then
		return led_off(LED_WPS)
	elseif "blink_slow" ==  action
	then
		return led_blink_slow(LED_WPS)
	elseif "blink_fast" == action
	then
		return led_blink_fast(LED_WPS)
	else
		debug("input error: action must be one of  on off blink_slow blink_fast")
		return false
	end
end


function led_module.led_all_on()

	for k,v in pairs(LEDS_ARRAY)
	do
		local ret = led_on(v)
		if false == ret
		then
			break
		end

		if k == table.maxn(LEDS_ARRAY)
		then
			return true
		end
	end

	return false

end


function led_module.led_all_off()

	for k,v in pairs(LEDS_ARRAY)
	do

		local ret = led_off(v)
		if false == ret
		then
			break
		end

		if k == table.maxn(LEDS_ARRAY)
		then
			return true
		end
	end

	return false

end


return led_module