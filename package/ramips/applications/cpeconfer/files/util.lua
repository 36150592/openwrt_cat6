local module = {}

local DEBUG=require("debug")
function module.debug(...)
	arg = { ... } 
 	local info = DEBUG.getinfo( 2, "nSl")
	for k,v in pairs(info) do
		if k == "name" or k == "source"
		then
			io.write(v)
		elseif k == "currentline"
		then
			io.write(":")
			io.write(v)
			io.write(" ")
		end
		
	end

	for k,v in pairs(arg) do
		io.write(v)
		io.write(" ")
	end
	io.write("\n")
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

return module