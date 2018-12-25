require("uci")
require("io")

update_module = {}

local util=require("tz.util")
local debug = util.debug
local split = util.split
local sleep = util.sleep

-- update system api
-- input:
--		string: package_path --> the upgrade bin
-- return:
-- 		number:
-- 			-1: the package_path is empty
-- 			-2: the package_path is not exist
-- 			-3: the package_path's content is error.
-- 			-4: upgrade system fail


function update_module.tz_update_system(package_path)

	if type(package_path) ~= "string"
	then
		debug("the package_path is nil")
		return -1
	end
	
	if util.is_file_exist(package_path) ~= true
	then
		debug("the package_path is not exist")
		return -2
	end
	
	os.execute("rm /tmp/updateit -f");

	local cmd = tostring("cd /tmp && unzip -P tz18c6 ")..tostring(package_path)..tostring(" updateit")
	os.execute(cmd)

	if util.is_file_exist("/tmp/updateit") ~= true
	then
		debug("the package_path's content is error.")
		return -3
	end

	os.execute("chmod 777 /tmp/updateit");

	cmd = tostring("/tmp/updateit ")..tostring(package_path)
	os.execute(cmd);

	if util.is_file_exist("/tmp/.update_system_fail") ~= true
        then
                debug("upgrade system fail")
                return -4
        end
end

return update_module
