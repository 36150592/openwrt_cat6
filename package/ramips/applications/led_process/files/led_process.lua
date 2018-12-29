#!/usr/bin/lua

local led = require "tz.led"
local util=require("tz.util")
local wifi=require("tz.wifi")
local modem=require("tz.modem")
local debug = util.debug
local split = util.split 
local sleep = util.sleep



local function modem_led()

	local  status =  modem.modem_get_status()	

	if nil ~= status 
	then
		led.led_set_signal(tonumber(status["signal_lvl"]))

		if "1" == status["network_link_stauts"]
		then
			led.led_set_normal("on")
			led.led_set_exception("off")
		else 
			if "2" == status["service_status"]
			then
				led.led_set_normal("blink_slow")
				led.led_set_exception("off")
			else
				led.led_set_normal("off")
				led.led_set_exception("on")
			end

		end
	else
		led.led_set_signal(0)
		led.led_set_normal("off")
		led.led_set_exception("on")
	end


end

local function wifi_led()

	local wifi_devs = wifi.wifi_get_dev()

	for k,v in pairs(wifi_devs)
	do
		local ret = wifi.wifi_is_start(v["wifi_id"])

		if "2.4G" == v["band"]
		then
			if ret
			then 
				led.led_set_wifi("on")
			else
				led.led_set_wifi("off")
			end

		elseif "5G" == v["band"]
		then
			if ret
			then 
				led.led_set_wifi_5g("on")
			else
				led.led_set_wifi_5g("off")
			end
		end


	end

end



led.led_all_off()
while true
do
	sleep(3)
	modem_led()
	wifi_led()
end