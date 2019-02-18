local module = {}

require("io")
local DEBUG=require("debug")

local tzlib = require("luatzlib")
tzlib.remove_stderr()
local socket = require "socket"
function module.test_time(f)
	--os.execute(string.format("logger test_time %s  %s", f, tostring(socket.gettime())))
end

function module.debug(...)
	--[[arg = { ... } 
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
	os.execute(string.format("logger %s", log_str))]]--
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

   --local t0 = os.clock()

   --while os.clock() - t0 <= n do end

   socket.select(nil,nil,n)

end

function module.is_file_exist(FileName) 
	local f = io.open(FileName,"r")
	if(f == nil )then
		return false
	end
	f:close()
	return true
end 

function module.web_log(content)
	if nil ~= content
	then
		local ff = io.open("/tmp/web_log","a")
		ff:write(content)
		io.close(ff)
	end
end

function module.get_env_cmdId(env)
	local lines = string.match(env,'QUERY_STRING=cmd=%d+')
	if nil ~= lines
	then
		local cmdId = string.match(lines,'%d+')
		if nil ~= cmdId 
		then
			return cmdId
		end
	end
	return nil
end

function module.get_env_boundary(env)
        local lines = string.match(env,'boundary=[-%d%u%l]*')
        if nil ~= lines
        then
                local boundary = string.sub(lines, 10, string.len(lines))
                if nil ~= boundary
                then
                        return boundary
                end
        end
end

function module.get_env_content_len(env)                                                                                  
        local lines = string.match(env,'CONTENT_LENGTH=%d+')
        if nil ~= lines
        then
                local len = string.match(lines,'%d+')
                if nil ~= len
                then
                        return len
                end
        end
end

function module.get_upload_file_name(lines)
	local p1 = string.match(lines,"filename=.*")
	if nil == p1
	then
		return nil
	end

	local p2 = string.match(p1,"\".*")
	if nil == p2
	then
		return nil
	end

	local p3 = string.sub(p2, 2, string.len(p2) -2)

	local strchar
	if nil ~= string.find(p3, "\\")
	then
		strchar = "\\"
	else
		if nil ~= string.find(p3,"/")
		then
			strchar = "/"
		else
			return p3
		end
	end

	local str1 = string.reverse(p3)
	local pa1, pa2 = string.find(str1, strchar)
	local str2 = string.sub(str1, 1, pa2-1)
	local str3 = string.reverse(str2)

	return str3

end

local function  to_binary(arg)
	local data64 = {}
	local bit={}
	for i=1,64
	do
    	data64[i]=2^(64-i)
	end

 	for i=1,64 
 	do 
 		if arg >= data64[i] 
 		then 
 			bit[i]=1 
 			arg=arg-data64[i] 
 		else 
 			bit[i]=0 
 		end 
 	end
 	return bit
end


function  module.to_hex_string(arg)  

	if arg > 10^15
	then
		debug("arg out of range")
		return arg
	end

	if arg == 0 
	then
		return '0'
	end

    local data64 = {}    
    local bit={}                                
    for i=1,64                                  
    do                                          
    	data64[i]=16^(64-i)                         
    end                                         
                                                
    for i=1,64                                  
    do                                          
            local j = 1                         
            local temp =0                       
            local tmp =0                        
            while j < 16                        
            do                                  
                    if arg >= j*data64[i]     
                    then                      
                            tmp = j           
                            if j == 10        
                            then              
                                    temp = 'a'
                            elseif j == 11    
                            then              
                                    temp = 'b'
                            elseif j == 12    
                            then              
                                    temp = 'c'
                            elseif j == 13       
                            then                 
                                    temp = 'd'   
                            elseif j == 14       
                            then                 
                                    temp = 'e'   
                            elseif j == 15       
                            then                 
                                    temp = 'f'   
                            else                 
                                    temp=j       
                            end                  
                    else                         
                            break    
                    end              
                    j = j + 1        
            end                      
                                     
            bit[i]=temp              
            arg = arg - tmp*data64[i]
    end 


    local hex = ""                                       
	local flag = false                                   
	for k,v in pairs(bit)   
	do                                       
        if v ~= 0                        
        then                             
                flag = true              
        end                             
        if flag == true               
        then                          
                hex = hex .. v        
        end                           
	end                  

    return hex                    
end               


local function to_digit(arg) 
		local dig=0 
		for i=1,64 
		do 
			if arg[i] ==1 
			then 
				dig=dig+2^(64-i) 
			end 
		end 

		return dig 
end

local function show_digit_array(dig)

	for i = 1,64
	do
		io.write(dig[i])
	end
	io.write("\n")
end

function module._and(a, b)
	local t1 = to_binary(a)
	local t2 = to_binary(b)

	local dig = {}

	--[[print("t1")
	show_digit_array(t1)
	print("t2")
	show_digit_array(t2)]]--

	for i = 1,64 do

		if t1[i] == 1 and t2[i] == 1 
		then
			dig[i] = 1
		else
			dig[i] = 0
		end
	end

	return to_digit(dig)


end


return module
