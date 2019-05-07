#!/usr/bin/lua

local led = require "tz.led"
local util=require("tz.util")
local sleep = util.sleep



local function all_led_status_1()

	led.led_set_signal(5)
	led.led_set_exception("on")

	led.led_set_phone("off")
	led.led_set_wps("off")
	led.led_set_wifi("off")
	led.led_set_wifi_5g("off")
	led.led_set_normal("off")
	

end

local function all_led_status_2()

	led.led_set_signal(0)
	led.led_set_exception("off")

	led.led_set_phone("on")
	led.led_set_wps("on")
	led.led_set_wifi("on")
	led.led_set_wifi_5g("on")
	led.led_set_normal("on")

end


while true
do
	sleep(1)
	all_led_status_1()
	sleep(1)
	all_led_status_2()
end
