local module = {}

require("io")
local DEBUG=require("debug")

local tzlib = require("luatzlib")
tzlib.remove_stderr()

function module.debug(...)
	arg = { ... } 
	local log_str = ""
	local info = DEBUG.getinfo( 2, "nSl")
	for k,v in pairs(info) do
		if k == "name" or k == "source"
		then
			log_str = log_str .. tostring(v)
		elseif k == "currentline"
		then
			log_str = log_str .. ":" .. tostring(v) .. " "
		end
		
	end

	for k,v in pairs(arg) do
		log_str = log_str .. tostring(v) .. " "
	end
	os.execute(string.format("logger %s", log_str))
end

function module.split(szFullString, szSeparator)  
	local nFindStartIndex = 1  
	local nSplitIndex = 1  
	local nSplitArray = {}  
	while true do  
	   local nFindLastIndex = string.find(szFullString, szSeparator, nFindStartIndex)  
	   if not nFindLastIndex then  
	    nSplitArray[nSplitIndex] = string.sub(szFullString, nFindStartIndex, string.len(szFullString))  
	    break  
	   end  
	   nSplitArray[nSplitIndex] = string.sub(szFullString, nFindStartIndex, nFindLastIndex - 1)  
	   nFindStartIndex = nFindLastIndex + string.len(szSeparator)  
	   nSplitIndex = nSplitIndex + 1  
	end  
	return nSplitArray  
end 
function module.sleep(n)

   local t0 = os.clock()

   while os.clock() - t0 <= n do end

end

function module.is_file_exist(FileName) 
	local f = io.open(FileName,"r")
	if(f == nil )then
		return false
	end
	f:close()
	return true
end 

return module
