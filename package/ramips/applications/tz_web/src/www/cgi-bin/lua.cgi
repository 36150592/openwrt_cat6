#!/usr/bin/lua

local cjson = require "cjson"

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
	tz_answer["cmd"] = 100;
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

local switch = {
     [80] = iniPage,
	 [97] = change_language,
	 [100] = login
 }
 
 local f = switch[tz_req["cmd"]]
 if(f) then
     f()
 end








