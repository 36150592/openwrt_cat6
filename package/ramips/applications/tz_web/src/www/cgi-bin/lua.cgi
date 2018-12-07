#!/usr/bin/lua

local cjson = require "cjson"

print("Contenttype:text/html\n")

data1=io.read();
local tz_req = cjson.decode(data1);

if( tz_req["cmd"] == 80) then
	local tz_answer = {};
	tz_answer["success"] = true;
	tz_answer["cmd"] = 80;
	result_json = cjson.encode(tz_answer);
	print(result_json);
else
	print("hello");
end