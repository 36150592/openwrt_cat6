#!/usr/bin/lua

local cjson = require "cjson"
local wifi = require "tz.wifi"
local dhcp = require "tz.dhcp"
local system = require "tz.system"
local sim = require "tz.sim"
local modem = require "tz.modem"
local device = require "tz.device"
local network = require "tz.network"
local firewall = require "tz.firewall"
local uti = require "tz.util"

local WEB_PATH = "/tz_www"
print("Contenttype:text/html\n")

local envf = io.popen("env")
local envv = envf:read("*a")
io.close(envf)

local tz_req

function login()

    local tz_answer = {}
    tz_answer["cmd"] = 100
    local bolename = 0
    local bolepassword = 0
    local userSign
    local nSplitArray = {}

    local username = "'" .. tz_req["username"] .. "'"
    local password = "'" .. tz_req["passwd"] .. "'"
    local sessionId = tz_req["sessionId"]
    
    local file = io.open("/etc/config/tozed", "r")
    io.input(file)
    for l in file:lines() do
        nSplitArray = uti.split(l, ' ')
        if nSplitArray[2] == 'TZ_USERNAME' then
            if nSplitArray[3] == username then
                bolename = 1
                userSign = 'TZ_USERNAME'
            end
        end

        if nSplitArray[2] == 'TZ_PASSWD' and userSign == 'TZ_USERNAME' then
            if nSplitArray[3] == password then
                bolepassword = 1
                break
            end
        end

        if nSplitArray[2] == 'TZ_SUPER_USERNAME' then
            if nSplitArray[3] == username then
                bolename = 1
                userSign = 'TZ_SUPER_USERNAME'
            end
        end

        if nSplitArray[2] == 'TZ_SUPER_PASSWD' and userSign ==
            'TZ_SUPER_USERNAME' then
            if nSplitArray[3] == password then
                bolepassword = 1
                break
            end
        end

        if nSplitArray[2] == 'TZ_TEST_USERNAME' then
            if nSplitArray[3] == username then
                bolename = 1
                userSign = 'TZ_TEST_USERNAME'
            end
        end

        if nSplitArray[2] == 'TZ_TEST_PASSWD' and userSign == 'TZ_TEST_USERNAME' then
            if nSplitArray[3] == password then
                bolepassword = 1
                break
            end
        end

    end

    io.close(file)

    if bolename == 1 and bolepassword == 1 then
        if (uti.is_file_exist("/tmp/sessionsave") ~= true) then
            os.execute("cd /tmp && mkdir  sessionsave")
        end

        local logintime = os.time()
        local fileDir = "/tmp/sessionsave/."
        local fileName = string.format("%s%s%s", fileDir, sessionId, logintime)
        local file1 = io.open(fileName, "w")
        local timeout = system.system_login_get_session_timeout()
        io.input(file1)
        file1:write("updateTime:" .. logintime)
        io.close(file1)

        tz_answer["success"] = true
        tz_answer["sessionId"] = sessionId .. logintime
        tz_answer["timeout"] = timeout

        local web_info = {}
        web_info = system.system_get_web_info()
        if (userSign == 'TZ_USERNAME') then
            tz_answer["auth"] = web_info["web_user_show_hide_pref"]
            tz_answer["level"] = "3"
        end

        if (userSign == 'TZ_SUPER_USERNAME') then
            tz_answer["auth"] = web_info["web_operator_show_hide_pref"]
            tz_answer["level"] = "2"
        end

        if (userSign == 'TZ_TEST_USERNAME') then
            tz_answer["auth"] = "FFFFFFFFFFFF"
            tz_answer["level"] = "1"
        end

        result_json = cjson.encode(tz_answer)
        print(result_json)
        return
    end

    tz_answer["success"] = false
    result_json = cjson.encode(tz_answer)
    print(result_json)
    return

end

function logout()

    local tz_answer = {}
    tz_answer["cmd"] = 99
    local fileName = string.format("rm /tmp/sessionsave/.%s",
                                   tz_req["sessionId"])
    os.execute(fileName)
    tz_answer["success"] = true
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function iniPage()
    local file = io.open("../config.json", "r")
    io.input(file)
    local t = io.read("*a")
    io.close(file)
    local Jsondata = cjson.decode(t)
    local Rdata = {}
    Rdata["language"] = Jsondata["Language"]
    Rdata["languageList"] = Jsondata["LanguageList"]
    local tz_answer = {}
    tz_answer["success"] = true
    tz_answer["cmd"] = 80
    tz_answer["data"] = Rdata
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function change_language()
    local oldLanguage = string.lower(tz_req["languageOld"])
    local newLanguage = string.lower(tz_req["languageSelect"])
    shellcmd = string.format(
                   "sed -i 's/%s.js?t=000000/%s.js?t=000000/g' %s/login.html",
                   oldLanguage, newLanguage, WEB_PATH)
    shellcmd1 = string.format(
                    "sed -i 's/%s.js?t=000000/%s.js?t=000000/g' %s/index.html",
                    oldLanguage, newLanguage, WEB_PATH)
    shellcmd2 = string.format(
                    "sed -i 's/\"Language\":\"%s\"/\"Language\":\"%s\"/g' %s/config.json",
                    oldLanguage, newLanguage, WEB_PATH)

    system.system_set_web_language(newLanguage)

    os.execute(shellcmd)
    os.execute(shellcmd1)
    os.execute(shellcmd2)

    local tz_answer = {}
    tz_answer["success"] = true
    tz_answer["cmd"] = 97
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_sysinfo()
    local data_array = {}

    data_array["wan"] = network.network_get_wan_info()
    data_array["4g"] = network.network_get_4g_net_info()
    data_array["4g1"] = network.network_get_4g1_net_info()
    data_array["4g2"] = network.network_get_4g2_net_info()

    local tz_answer = {}
    tz_answer["success"] = true
    tz_answer["cmd"] = 0
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_sysinfo_2()
    local data_array = {}

    data_array["system"] = system.system_get_status() or ''

    local tz_answer = {}
    tz_answer["success"] = true
    tz_answer["cmd"] = 0
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_sysinfo_3()
    local data_array = {}

    data_array["modem"] = modem.modem_get_status() or ''

    local tz_answer = {}
    tz_answer["success"] = true
    tz_answer["cmd"] = 0
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_sysinfo_4()
    local data_array = {}

    data_array["sim"] = sim.sim_get_status() or ''

    local tz_answer = {}
    tz_answer["success"] = true
    tz_answer["cmd"] = 0
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_sysinfo_5()
    local data_array = {}
    local array = wifi.wifi_get_dev()
    if next(array) ~= nil then
        local network = array[1]['network']
        data_array["lan"] = dhcp.dhcp_get_object_by_network(network) or ''
    else
        data_array["lan"] = ''
    end

    local tz_answer = {}
    tz_answer["success"] = true
    tz_answer["cmd"] = 0
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_diviceinfo()
    local data_array = {}
    data_array["sim"] = sim.sim_get_status() or ''
    data_array["modem"] = modem.modem_get_info() or ''
    data_array["factory"] = system.system_get_tozed_factory_info() or ''

    -- data_array["device"] = device.device_get_info() or ''
    data_array["version"] = system.get_divice_version() or ''
    data_array["system"] = system.system_get_status() or ''
    data_array["softVersion"] = system.system_get_tozed_system_info() or ''
    local tz_answer = {}
    tz_answer["success"] = true
    tz_answer["cmd"] = 43
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_wifi()
    local array = wifi.wifi_get_dev()
    local data_array
    if next(array) ~= nil then

        local id
        for k, v in pairs(array) do
            if (v["band"] == "2.4G") then
                data_array = v
                id = v['wifi_id']
            end
        end
        data_array['status'] = wifi.wifi_get_enable_status(id)
        data_array['hidden_ssid'] = wifi.wifi_get_hidden_ssid(id)
        data_array['wmm'] = wifi.wifi_get_wmm(id)
        -- data_array['ssid'] = wifi.wifi_get_ssid(id)
        -- data_array['maxNum'] = wifi.wifi_get_connect_sta_number(id)
        -- data_array['encryption'] = wifi.wifi_get_encryption(id)
        -- data_array['pwd'] = wifi.wifi_get_password(id)

    else
        data_array = ''
    end

    local tz_answer = {}
    tz_answer["success"] = true
    tz_answer["cmd"] = 101
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)
end

function get_wifi5()
    local array = wifi.wifi_get_dev()
    local data_array
    if next(array) ~= nil then
        local id
        for k, v in pairs(array) do
            if (v["band"] == "5G") then
                data_array = v
                id = v['wifi_id']
            end
        end

        data_array['status'] = wifi.wifi_get_enable_status(id)
        data_array['hidden_ssid'] = wifi.wifi_get_hidden_ssid(id)
        data_array['wmm'] = wifi.wifi_get_wmm(id)
        -- data_array['ssid'] = wifi.wifi_get_ssid(id)
        -- data_array['mode'] = wifi.wifi_get_mode(id)
        data_array['bandwidth'] = wifi.wifi_get_bandwidth(id)
        -- data_array['encryption'] = wifi.wifi_get_encryption(id)
        -- data_array['enType'] = wifi.wifi_get_encryption_type(id)
        -- data_array['pwd'] = wifi.wifi_get_password(id)
    else
        data_array = ''
    end
    local tz_answer = {}
    tz_answer["success"] = true
    tz_answer["cmd"] = 201
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_ssidlist()

    local array = wifi.wifi_secondary_get_ssid_list()
    local data_array = {}
    local wifiId = tonumber(tz_req["wifiId"])
    local i = 2
    for k, v in pairs(array) do
        if (v["primary_id"] == wifiId) then
            data_array[i] = v
            data_array[i]['status'] = wifi.wifi_secondary_get_enable_status(
                                          v["secondary_id"])
            i = i + 1
        end
    end
    local tz_answer = {}
    tz_answer["success"] = true
    tz_answer["cmd"] = 120
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_ssidlist()

    local tz_answer = {}
    local ret

    local wifiId = tonumber(tz_req["wifiId"])
    local secondaryId = tonumber(tz_req["secondaryId"])
    local wifiOpen = tonumber(tz_req["wifiOpen"])
    local broadcast = tonumber(tz_req["broadcast"])
    local wmm = tonumber(tz_req["wmm"])
    local ssid = tz_req["ssid"]
    local maxStation = tonumber(tz_req["maxStation"])
    local authenticationType = tz_req["authenticationType"]
    local encryptAlgorithm = tz_req["encryptAlgorithm"]
    local key = tz_req["key"]

    if (nil ~= wifiOpen) then
        if (0 == wifiOpen) then
            ret = wifi.wifi_secondary_enable(secondaryId)
            if (not ret) then tz_answer["enablewifi"] = false end
        elseif (1 == wifiOpen) then
            ret = wifi.wifi_secondary_disable(secondaryId)
            if (not ret) then tz_answer["disablewifi"] = false end
        end
    end

    if (nil ~= broadcast) then
        if (0 == broadcast) then
            ret = wifi.wifi_secondary_disable_hidden(secondaryId)
            if (not ret) then tz_answer["disableSsid"] = false end
        elseif (1 == broadcast) then
            ret = wifi.wifi_secondary_enable_hidden(secondaryId)
            if (not ret) then tz_answer["enableSsid"] = false end
        end
    end

    if (nil ~= wmm) then
        if (0 == wmm) then
            ret = wifi.wifi_secondary_disable_wmm(secondaryId)
            if (not ret) then tz_answer["disableWmm"] = false end
        elseif (1 == wmm) then
            ret = wifi.wifi_secondary_enable_wmm(secondaryId)
            if (not ret) then tz_answer["enableWmm"] = false end
        end
    end

    if (nil ~= ssid) then
        ret = wifi.wifi_secondary_set_ssid(secondaryId, ssid)
        if (not ret) then tz_answer["setSsid"] = false end
    end

    if (nil ~= maxStation) then
        ret = wifi.wifi_secondary_set_connect_sta_num(secondaryId, maxStation)

        if (not ret) then tz_answer["setnumber"] = false end
    end

    if (nil ~= authenticationType) then
        ret =
            wifi.wifi_secondary_set_encryption(secondaryId, authenticationType)
        if (not ret) then tz_answer["setEncryption"] = false end
    end

    if (nil ~= encryptAlgorithm) then
        ret = wifi.wifi_secondary_set_encryption_type(secondaryId,
                                                      encryptAlgorithm)
        if (not ret) then tz_answer["setEncryptionType"] = false end
    end

    if (nil ~= key) then
        ret = wifi.wifi_secondary_set_password(secondaryId, key)
        if (not ret) then tz_answer["setPassword"] = false end
    end

    wifi.wifi_restart(wifiId)
    tz_answer["success"] = true
    tz_answer["cmd"] = 121
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_lan()

    local array = wifi.wifi_get_dev()
    local network = {}
    local wifi_id
    local i = 1
    network[i] = array[1]['network']
    i = i + 1
    wifiId = array[1]['wifi_id']
    local data_lan = {}

    local array2 = wifi.wifi_secondary_get_ssid_list()
    for k, v in pairs(array2) do
        if (v["primary_id"] == wifiId) then
            network[i] = v['network']
            i = i + 1
        end
    end

    for k, v in pairs(network) do
        data_lan[k] = dhcp.dhcp_get_object_by_network(v)['interface']
    end

    local tz_answer = {}
    tz_answer["success"] = true
    tz_answer["cmd"] = 203
    tz_answer["data"] = data_lan
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_dhcp()
    local data_array = {}
    -- data_array["status"] = dhcp.dhcp_get_enable_status()
    -- data_array["lanIp"] = dhcp.dhcp_get_server_ip()
    -- data_array["netMask"] = dhcp.dhcp_get_server_mask()
    -- data_array["ipBegin"],data_array["ipEnd"] = dhcp.dhcp_get_ip_range()
    -- data_array["expireTime"] = dhcp.dhcp_get_lease_time()
    data_array = dhcp.dhcp_get_object_list()
    local tz_answer = {}
    tz_answer["success"] = true
    tz_answer["cmd"] = 102
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)
end

function get_dhcp_list()
    local array = wifi.wifi_get_dev()
    local network = {}
    local wifi_id
    local i = 1
    network[i] = array[1]['network']
    i = i + 1
    wifiId = array[1]['wifi_id']
    local data_array = {}

    local array2 = wifi.wifi_secondary_get_ssid_list()
    for k, v in pairs(array2) do
        if (v["primary_id"] == wifiId) then
            network[i] = v['network']
            i = i + 1
        end
    end

    for k, v in pairs(network) do
        data_array[k] = dhcp.dhcp_get_object_by_network(v)
    end

    local lists = dhcp.dhcp_get_reserve_ip()

    local tz_answer = {}
    tz_answer["success"] = true
    tz_answer["cmd"] = 102
    tz_answer["data"] = data_array
    tz_answer["reserveList"] = lists
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_wifi()
    local tz_answer = {}
    tz_answer["cmd"] = 2
    local ret
    local array = wifi.wifi_get_dev()

    local id
    for k, v in pairs(array) do
        if (v["band"] == "2.4G") then id = v['wifi_id'] end
    end

    local wifiOpen = tonumber(tz_req["wifiOpen"])
    local broadcast = tonumber(tz_req["broadcast"])
    local wmm = tonumber(tz_req["wmm"])
    local ssid = tz_req["ssid"]
    local txPower = tonumber(tz_req["txPower"])
    local maxStation = tonumber(tz_req["maxStation"])
    local channel = tz_req["channel"]
    local mode = tonumber(tz_req["wifiWorkMode"])
    local ht = tz_req["ht"]
    local authenticationType = tz_req["authenticationType"]
    local encryptAlgorithm = tz_req["encryptAlgorithm"]
    local key = tz_req["key"]

    if (nil ~= wifiOpen) then
        if (0 == wifiOpen) then
            ret = wifi.wifi_enable(id)
            if (not ret) then tz_answer["enablewifi"] = false end
        elseif (1 == wifiOpen) then
            ret = wifi.wifi_disable(id)
            if (not ret) then tz_answer["disablewifi"] = false end
        end
    end

    if (nil ~= broadcast) then
        if (0 == broadcast) then
            ret = wifi.wifi_disable_hidden_ssid(id)
            if (not ret) then tz_answer["disableSsid"] = false end
        elseif (1 == broadcast) then
            ret = wifi.wifi_enable_hidden_ssid(id)
            if (not ret) then tz_answer["enableSsid"] = false end
        end
    end

    if (nil ~= wmm) then
        if (0 == wmm) then
            ret = wifi.wifi_disable_wmm(id)
            if (not ret) then tz_answer["disableWmm"] = false end
        elseif (1 == wmm) then
            ret = wifi.wifi_enable_wmm(id)
            if (not ret) then tz_answer["enableWmm"] = false end
        end
    end

    if (nil ~= ssid) then
        ret = wifi.wifi_set_ssid(id, ssid)
        if (not ret) then tz_answer["setSsid"] = false end
    end

    if (nil ~= txPower) then
        ret = wifi.wifi_set_txpower(id, txPower)
        if (not ret) then tz_answer["setTxpower"] = false end
    end

    if (nil ~= maxStation) then
        ret = wifi.wifi_set_connect_sta_number(id, maxStation)
        if (not ret) then tz_answer["setnumber"] = false end
    end

    if (nil ~= channel) then
        ret = wifi.wifi_set_channel(id, channel)
        if (not ret) then tz_answer["setChannel"] = false end
    end

    if (nil ~= mode) then
        ret = wifi.wifi_set_mode(id, mode)
        if (not ret) then tz_answer["setMode"] = false end
    end

    if (nil ~= ht) then
        ret = wifi.wifi_set_bandwidth(id, ht)
        if (not ret) then tz_answer["setHtMode"] = false end
    end

    if (nil ~= authenticationType) then
        ret = wifi.wifi_set_encryption(id, authenticationType)
        if (not ret) then tz_answer["setEncryption"] = false end
    end

    if (nil ~= encryptAlgorithm) then
        ret = wifi.wifi_set_encryption_type(id, encryptAlgorithm)
        if (not ret) then tz_answer["setEncryptionType"] = false end
    end

    if (nil ~= key) then
        ret = wifi.wifi_set_password(id, key)
        if (not ret) then tz_answer["setPassword"] = false end
    end

    wifi.wifi_restart(id)
    tz_answer["success"] = true
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_wifi5()

    local tz_answer = {}
    tz_answer["cmd"] = 202
    local ret
    local array = wifi.wifi_get_dev()

    local id
    for k, v in pairs(array) do
        if (v["band"] == "5G") then id = v['wifi_id'] end
    end

    local wifiOpen = tonumber(tz_req["wifiOpen"])
    local broadcast = tonumber(tz_req["broadcast"])
    local wmm = tonumber(tz_req["wmm"])
    local ssid = tz_req["ssid"]
    local channel = tz_req["channel"]
    local mode = tonumber(tz_req["wifiWorkMode"])
    local bandWidth = tz_req["bandWidth"]
    local maxStation = tonumber(tz_req["maxStation"])
    local authenticationType = tz_req["authenticationType"]
    local encryptAlgorithm = tz_req["encryptAlgorithm"]
    local key = tz_req["key"]

    if (nil ~= wifiOpen) then
        if (0 == wifiOpen) then
            ret = wifi.wifi_enable(id)
            if (not ret) then tz_answer["enablewifi"] = false end
        elseif (1 == wifiOpen) then
            ret = wifi.wifi_disable(id)
            if (not ret) then tz_answer["disablewifi"] = false end
        end
    end

    if (nil ~= broadcast) then
        if (0 == broadcast) then
            ret = wifi.wifi_disable_hidden_ssid(id)
            if (not ret) then tz_answer["disableSsid"] = false end
        elseif (1 == broadcast) then
            ret = wifi.wifi_enable_hidden_ssid(id)
            if (not ret) then tz_answer["enableSsid"] = false end
        end
    end

    if (nil ~= wmm) then
        if (0 == wmm) then
            ret = wifi.wifi_disable_wmm(id)
            if (not ret) then tz_answer["disableWmm"] = false end
        elseif (1 == wmm) then
            ret = wifi.wifi_enable_wmm(id)
            if (not ret) then tz_answer["enableWmm"] = false end
        end
    end

    if (nil ~= ssid) then
        ret = wifi.wifi_set_ssid(id, ssid)
        if (not ret) then tz_answer["setSsid"] = false end
    end

    if (nil ~= channel) then
        ret = wifi.wifi_set_channel(id, channel)
        if (not ret) then tz_answer["setChannel"] = false end
    end

    if (nil ~= mode) then
        ret = wifi.wifi_set_mode(id, mode)
        if (not ret) then tz_answer["setMode"] = false end
    end

    if (nil ~= bandWidth) then
        ret = wifi.wifi_set_bandwidth(id, bandWidth)
        if (not ret) then tz_answer["setBwMode"] = false end
    end

    if (nil ~= maxStation) then
        ret = wifi.wifi_set_connect_sta_number(id, maxStation)
        if (not ret) then tz_answer["setnumber"] = false end
    end

    if (nil ~= authenticationType) then
        ret = wifi.wifi_set_encryption(id, authenticationType)
        if (not ret) then tz_answer["setEncryption"] = false end
    end

    if (nil ~= encryptAlgorithm) then
        ret = wifi.wifi_set_encryption_type(id, encryptAlgorithm)
        if (not ret) then tz_answer["setEncryptionType"] = false end
    end

    if (nil ~= key) then
        ret = wifi.wifi_set_password(id, key)
        if (not ret) then tz_answer["setPassword"] = false end
    end

    wifi.wifi_restart(id)
    tz_answer["success"] = true
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_dhcp()

    local tz_answer = {}
    tz_answer["cmd"] = 3
    local ret

    local lanName = tz_req["lanName"]
    local lanIp = tz_req["lanIp"]
    local netMask = tz_req["netMask"]
    local dhcpServer = tonumber(tz_req["dhcpServer"])
    local ipBegin = tz_req["ipBegin"]
    local ipEnd = tz_req["ipEnd"]
    local limitNum = tz_req["limitNum"]
    local expireTime = tonumber(tz_req["expireTime"])
    local main_Dns = tz_req["main_dns"]
    local vice_Dns = tz_req["vice_dns"]

    if (nil ~= lanIp) then
        ret = dhcp.dhcp_set_server_ip(lanName, lanIp)
        if (ret) then
            tz_answer["setIp"] = true
        else
            tz_answer["setIp"] = false
        end
    end

    if (nil ~= netMask) then
        ret = dhcp.dhcp_set_server_mask(lanName, netMask)
        if (not ret) then tz_answer["setMask"] = false end
    end

    if (nil ~= expireTime) then
        ret = dhcp.dhcp_set_lease_time(lanName, expireTime)
        if (not ret) then tz_answer["setLeaseTime"] = false end
    end

    if (nil ~= dhcpServer) then
        if (0 == dhcpServer) then
            ret = dhcp.dhcp_config_disable(lanName)
            if (not ret) then tz_answer["disableDhcp"] = false end
        elseif (1 == dhcpServer) then
            ret = dhcp.dhcp_config_enable(lanName)
            if (not ret) then tz_answer["enableDhcp"] = false end
        end

    end

    if (nil ~= ipBegin) then
        ret = dhcp.dhcp_set_ip_range(lanName, ipBegin, limitNum)
        if (not ret) then tz_answer["setIpRange"] = false end
    end

    if (nil ~= main_Dns) then
        ret = dhcp.dhcp_set_main_dns(lanName, main_Dns)

        if (not ret) then tz_answer["mainDns"] = false end

    end

    if (nil ~= vice_Dns) then
        ret = dhcp.dhcp_set_vice_dns(lanName, vice_Dns)

        if (not ret) then tz_answer["secondDns"] = false end

    end

    dhcp.dhcp_restart()
    tz_answer["success"] = true
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_dhcpClient()
    local tz_answer = {}
    tz_answer["cmd"] = 103
    local data_array = dhcp.dhcp_get_client_list()
    tz_answer["success"] = true
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_wifiClient()

    local tz_answer = {}
    tz_answer["cmd"] = 117

    local data_array = dhcp.dhcp_get_client_list()

    local data_array2 = wifi.wifi_get_connect_sta_by_dev("2.4G")
    tz_answer["success"] = true
    tz_answer["userlist"] = data_array
    tz_answer["wifilist"] = data_array2
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_wifi5Client()

    local tz_answer = {}
    tz_answer["cmd"] = 118
    local data_array = dhcp.dhcp_get_client_list()

    local data_array2 = wifi.wifi_get_connect_sta_by_dev("5G")
    tz_answer["success"] = true
    tz_answer["userlist"] = data_array
    tz_answer["wifilist"] = data_array2
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_sysstatus()
    local data_array = {}
    local data_wifi = {}
    local sim_status = sim.sim_get_status() or {}
    local modem_status = modem.modem_get_status() or {}

    data_array["is_sim_exist"] = sim_status["is_sim_exist"] or ''
    data_array["card_status"] = sim_status["card_status"] or ''
    data_array["2g_register_status"] = modem_status["2g_register_status"] or ''
    data_array["3g_register_status"] = modem_status["3g_register_status"] or ''
    data_array["4g_register_status"] = modem_status["4g_register_status"] or ''
    data_array["rsrp"] = modem_status["rsrp"]
    data_array["signal_lvl"] = modem_status["signal_lvl"]
    data_array["network_link_stauts"] = modem_status["network_link_stauts"]

    data_array["ethInter"] = network.network_get_interface_up_down_status() or
                                 ''
    local array = wifi.wifi_get_dev()
    for k, v in pairs(array) do
        if (v["band"] == "2.4G") then
            data_wifi['status'] = wifi.wifi_get_enable_status(v['wifi_id'])
        end
        if (v["band"] == "5G") then
            data_wifi['status5'] = wifi.wifi_get_enable_status(v['wifi_id'])
        end
    end

    data_array["wifi"] = data_wifi
    local tz_answer = {}
    tz_answer["cmd"] = 113
    tz_answer["success"] = true
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_routerinfo()
    local tz_answer = {}
    tz_answer["cmd"] = 133
    local data_array = {}
    data_array["status"] = modem.modem_get_status() or ''
    data_array["divice"] = modem.modem_get_info() or ''
    data_array["system"] = system.system_get_status() or ''
    data_array["network"] = network.network_get_wan_info()
    data_array["softVersion"] = system.system_get_tozed_system_info() or ''
    if nil == data_array["network"] then
        data_array["network"] = network.network_get_4g_net_info() or ''
    end
    local array = wifi.wifi_get_dev()
    local data_wifi = array[1]
    local id = array[1]['wifi_id']
    data_wifi['status'] = wifi.wifi_get_enable_status(id)
    data_wifi['ssid'] = wifi.wifi_get_ssid(id)
    data_array["wifi"] = data_wifi

    tz_answer["success"] = true
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)
end

function firewall_restart()

    local tz_answer = {}
    tz_answer["cmd"] = 20
    tz_answer["success"] = firewall.firewall_restart()
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function clear_allrule()

    local tz_answer = {}
    tz_answer["cmd"] = 39
    local datas = tz_req["datas"]

    if nil ~= datas then

        if "portFilter" == datas["param"] then
            tz_answer["success"] = firewall.firewall_set_port_filter_list({})
        elseif "ipFilter" == datas["param"] then
            tz_answer["success"] = firewall.firewall_set_ip_filter_list({})
        elseif "macFilter" == datas["param"] then
            tz_answer["success"] = firewall.firewall_set_mac_filter_list({})
        elseif "portMapping" == datas["param"] then
            tz_answer["success"] = firewall.firewall_set_port_redirect_list({})
        elseif "urlFilter" == datas["param"] then
            tz_answer["success"] = firewall.firewall_set_url_filter_list({})
        elseif "ipMacBinding" == datas["param"] then
            tz_answer["success"] = firewall.firewall_set_ipmac_bind_filter_list(
                                       {})
        elseif "speedLimit" == datas["param"] then
            tz_answer["success"] = firewall.firewall_set_speed_filter_list({})
        elseif "aclFilter" == datas["param"] then
            tz_answer["success"] = firewall.firewall_set_acl_filter_list({})
        end
    else
        tz_answer["success"] = firewall.firewall_clear_all_user_rule()
    end
    result_json = cjson.encode(tz_answer)
    print(result_json)
    os.execute("/etc/init.d/firewall restart > /dev/null 2>&1 &")

end

function port_filter()

    local tz_answer = {}
    tz_answer["cmd"] = 21

    local datas = tz_req["datas"]
    local getfun = tz_req["getfun"]

    if (getfun) then
        local data_array = firewall.firewall_get_port_filter_list() or ''
        tz_answer["data"] = data_array
        tz_answer["success"] = true
    else
        tz_answer["success"] = firewall.firewall_set_port_filter_list(datas)

    end

    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function ip_filter()

    local tz_answer = {}
    tz_answer["cmd"] = 22

    local datas = tz_req["datas"]
    local getfun = tz_req["getfun"]

    if (getfun) then
        local data_array = firewall.firewall_get_ip_filter_list() or ''
        tz_answer["data"] = data_array
        tz_answer["success"] = true
    else
        tz_answer["success"] = firewall.firewall_set_ip_filter_list(datas)

    end

    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function mac_filter()

    local tz_answer = {}
    tz_answer["cmd"] = 23

    local datas = tz_req["datas"]
    local getfun = tz_req["getfun"]

    if (getfun) then
        local data_array = firewall.firewall_get_mac_filter_list() or ''
        tz_answer["data"] = data_array
        tz_answer["success"] = true
    else
        tz_answer["success"] = firewall.firewall_set_mac_filter_list(datas)

    end

    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function ipmac_binding()

    local tz_answer = {}
    tz_answer["cmd"] = 24

    local datas = tz_req["datas"]
    local getfun = tz_req["getfun"]

    if (getfun) then
        local data_array = firewall.firewall_get_ipmac_bind_filter_list() or ''
        tz_answer["data"] = data_array
        tz_answer["success"] = true
    else
        tz_answer["success"] = firewall.firewall_set_ipmac_bind_filter_list(
                                   datas)

    end

    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function speed_limit()

    local tz_answer = {}
    tz_answer["cmd"] = 25

    local datas = tz_req["datas"]
    local getfun = tz_req["getfun"]

    if (getfun) then
        local data_array = firewall.firewall_get_speed_filter_list() or ''
        tz_answer["data"] = data_array
        tz_answer["success"] = true
    else
        tz_answer["success"] = firewall.firewall_set_speed_filter_list(datas)

    end

    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function url_filtet()

    local tz_answer = {}
    tz_answer["cmd"] = 26

    local datas = tz_req["datas"]
    local getfun = tz_req["getfun"]

    if (getfun) then
        local data_array = firewall.firewall_get_url_filter_list() or ''
        tz_answer["data"] = data_array
        tz_answer["success"] = true
    else
        tz_answer["success"] = firewall.firewall_set_url_filter_list(datas)

    end

    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function port_redirect()

    local tz_answer = {}
    tz_answer["cmd"] = 27

    local datas = tz_req["datas"]
    local getfun = tz_req["getfun"]

    if (getfun) then
        local data_array = firewall.firewall_get_port_redirect_list() or ''
        tz_answer["data"] = data_array
        tz_answer["success"] = true
    else
        tz_answer["success"] = firewall.firewall_set_port_redirect_list(datas)

    end

    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function default_action()

    local tz_answer = {}
    tz_answer["cmd"] = 28

    local action = tz_req["action"]
    local getfun = tz_req["getfun"]

    if (getfun) then
        local data = firewall.firewall_get_default_action() or ''
        tz_answer["action"] = data
        tz_answer["success"] = true
    else
        tz_answer["success"] = firewall.firewall_set_default_action(action)

    end

    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function acl_filter()

    local tz_answer = {}
    tz_answer["cmd"] = 29

    local datas = tz_req["datas"]
    local getfun = tz_req["getfun"]

    if (getfun) then
        local data_array = firewall.firewall_get_acl_filter_list() or ''
        tz_answer["data"] = data_array
        tz_answer["success"] = true
    else
        tz_answer["success"] = firewall.firewall_set_acl_filter_list(datas)

    end

    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function network_tool()

    local tz_answer = system.system_network_tool(tz_req)

    tz_answer["cmd"] = 145
    tz_answer["success"] = true
    result_json = cjson.encode(tz_answer)
    print(result_json)
end

function upload_file()
    local tz_answer = {}
    tz_answer["cmd"] = 5
    local UploadDir = "/tmp/web_upload/"

    local boundary = uti.get_env_boundary(envv)
    local content_len = uti.get_env_content_len(envv)

    local a1 = io.read()
    local a2 = io.read()
    local a3 = io.read()
    local a4 = io.read()

    local file_len = content_len - string.len(a2) - string.len(a3) -
                         string.len(boundary) * 2 - 10 - 7 + 1

    local a5 = io.read(file_len)
    if nil ~= a5 then
        local file_name = uti.get_upload_file_name(a2)
        local file_path = string.format("%s%s", UploadDir, file_name)

        os.execute(string.format("mkdir -p %s", UploadDir))
        local ff = io.open(file_path, "w")
        ff:write(a5)
        io.close(ff)
    end

    tz_answer["success"] = true
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function update_partial()
    local UploadDir = "/tmp/web_upload/"
    local tz_answer = {}
    tz_answer["cmd"] = 106
    local fileName = tz_req["fileName"]
    tz_answer["success"] = true
    tz_answer["data"] = system.update_system(UploadDir .. fileName)
    result_json = cjson.encode(tz_answer)
    print(result_json)
end

function config_update()
    local UploadDir = "/tmp/web_upload/"
    local tz_answer = {}
    tz_answer["cmd"] = 184

    local fileName = tz_req["fileName"]
    tz_answer["success"] = true
    tz_answer["data"] = system.update_config(UploadDir .. fileName)
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function reboot_sys()

    local tz_answer = {}
    tz_answer["cmd"] = 6
    local ret = os.execute("reboot ")
    if (ret == 0) then
        tz_answer["success"] = true
    else
        tz_answer["success"] = false
    end
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function deal_at()
    local tz_answer = {}
    tz_answer["cmd"] = 19

    local f = io.popen(string.format("sendat -d/dev/ttyUSB1 -e  %s",
                                     tz_req["atCmd"]))
    tz_answer["data"] = f:read("*a")
    io.close(f)
    tz_answer["success"] = true
    result_json = cjson.encode(tz_answer)
    print(result_json)
end

function get_datetime()
    local tz_answer = {}
    tz_answer["cmd"] = 10

    local data_array = {}
    data_array["status"] = system.system_ntp_get_enable_status()
    data_array["timezone"] = system.system_ntp_get_timezone() or ''
    data_array["server"] = system.system_ntp_get_server_address() or ''

    local lbltime = os.date("%Y-%m-%d %H:%M:%S %A")
    data_array["lbltime"] = lbltime

    tz_answer["success"] = true
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_datetime()
    local tz_answer = {}
    tz_answer["cmd"] = 11
    local ret

    local ntpOpen = tonumber(tz_req["ntpOpen"])
    local timezone = tonumber(tz_req["timezone"])
    local timeServer = tz_req["timeServer"]
    local datetime = tz_req["datetime"]

    if (nil ~= ntpOpen) then
        if (0 == ntpOpen) then
            ret = system.system_ntp_disable(id)
            if (not ret) then tz_answer["disablentp"] = false end
        elseif (1 == ntpOpen) then
            ret = system.system_ntp_enable(id)
            if (not ret) then tz_answer["enablentp"] = false end
        end
    end

    if (nil ~= timezone) then
        ret = system.system_ntp_set_timezone(timezone)
        if (not ret) then tz_answer["setTimezone"] = false end
    end

    if (nil ~= timeServer) then
        local serverArr = {
            timeServer, tz_req["timeServer2"], tz_req["timeServer3"],
            tz_req["timeServer4"]
        }

        ret = system.system_ntp_set_server_address(serverArr)
        if (not ret) then tz_answer["setTimeserver"] = false end

    end

    if (nil ~= datetime) then
        ret = system.system_ntp_set_date(datetime)
        if (not ret) then tz_answer["setDatetime"] = false end
    end

    tz_answer["success"] = true
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_systime()
    local tz_answer = {}
    tz_answer["cmd"] = 1

    -- local timezone = system.system_ntp_get_timezone()
    local systime = os.time()

    tz_answer["success"] = true
    tz_answer["systime"] = systime
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_lockceel()
    local tz_answer = {}
    tz_answer["cmd"] = 160

    local ret
    local earfcn = tonumber(tz_req["txtFreqPoint"])
    local pci = tonumber(tz_req["txtPhyCellId"])

    ret = modem.modem_set_lte_lock_cell(pci, earfcn)
    if (not ret) then tz_answer["setLockcell"] = false end

    tz_answer["success"] = true
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_lockceel()
    local tz_answer = {}
    tz_answer["cmd"] = 162

    local pci, earfcn = modem.modem_get_lte_lock_cell()
    local modem = modem.modem_get_status() or ''

    tz_answer["success"] = true
    tz_answer["pci"] = pci
    tz_answer["earfcn"] = earfcn
    tz_answer["modem"] = modem
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_networkSet()
    local tz_answer = {}
    tz_answer["cmd"] = 163
    local data_array = {}

    data_array["act"] = modem.modem_get_network_mode()

    tz_answer["success"] = true
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_networkSet()
    local tz_answer = {}
    tz_answer["cmd"] = 164

    local ret
    local act = tonumber(tz_req["networkMode"])

    if (nil ~= act) then
        ret = modem.modem_set_network_mode(act)
        if (not ret) then tz_answer["setNetwork"] = false end
    end

    tz_answer["success"] = true
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_plmnSet()
    local tz_answer = {}
    tz_answer["cmd"] = 169
    local data_array = {}

    data_array["plmn"] = modem.modem_get_lock_operator() or ''

    tz_answer["success"] = true
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_plmnSet()
    local tz_answer = {}
    tz_answer["cmd"] = 170

    local ret
    local plmn = tz_req["lockPlmn"]

    if (nil ~= plmn) then
        ret = modem.modem_set_lock_operator(plmn)
        if (not ret) then tz_answer["setOperator"] = false end
    end

    tz_answer["success"] = true
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_simstatus()
    local tz_answer = {}
    tz_answer["cmd"] = 134

    local sim = sim.sim_get_status() or ''

    tz_answer["success"] = true
    tz_answer["sim"] = sim
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_supprotband()
    local tz_answer = {}
    tz_answer["cmd"] = 165

    local data_array = {}
    local gw, lte, tds = modem.modem_get_support_band()
    local lockband = modem.modem_get_lock_band()

    data_array["gw"] = gw
    data_array["lte"] = lte
    data_array["tds"] = tds

    tz_answer["success"] = true
    tz_answer["band"] = data_array
    tz_answer["lockband"] = lockband
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_supprotband()
    local tz_answer = {}
    tz_answer["cmd"] = 166

    local ret
    local band = tz_req["band"]

    if (nil ~= band) then
        ret = modem.modem_set_lock_band(band)
        if (not ret) then tz_answer["setBand"] = false end
    end

    tz_answer["success"] = true
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_remoteLogin()
    local tz_answer = {}
    tz_answer["cmd"] = 167

    local data_array = {}
    data_array["enremote"] = firewall.firewall_remote_get_web_login()

    tz_answer["success"] = true
    tz_answer["remote"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_remotePin()
    local tz_answer = {}
    tz_answer["cmd"] = 173

    local data_array = {}
    data_array["enremote"] = firewall.firewall_remote_get_web_login()

    tz_answer["success"] = true
    tz_answer["remote"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_pinstatus()
    local tz_answer = {}
    tz_answer["cmd"] = 135

    local ret 
    local type = tonumber(tz_req["type"])
    local passwd = tonumber(tz_req["passwd"])
    local pingType = tonumber(tz_req["pingType"])
    local new_pin = tonumber(tz_req["new_pin"])
    local pukPassword = tonumber(tz_req["pukPassword"])
    local pukPinPasswordConfirm = tonumber(tz_req["pukPinPasswordConfirm"])
    tz_answer["success"] = true
    if(pingType == 1)
        then
          ret = sim.sim_pin_unlock(passwd)
            if(not ret)
               then
               tz_answer["success"] = false
               tz_answer["enablePin"] = false
            end
    end
    if(1 == type)
       then
          ret = sim.sim_pin_lock_enable(passwd)
            if(not ret)
               then
               tz_answer["success"] = false
               tz_answer["enablePin"] = false
            end
    elseif(2 == type)
       then
          ret = sim.sim_pin_lock_disable(passwd)
            if(not ret)
              then
              tz_answer["success"] = false
              tz_answer["unlockPin"] = false
            end
     elseif(3 == type)
       then
          ret = sim.sim_pin_change(passwd,new_pin)
          print("ret = ",ret)
            if(not ret)
              then
              tz_answer["success"] = false
            end
     
    end
    if(pukPassword ~= nil)
        then
            ret = sim.sim_puk_unlock(pukPassword,pukPinPasswordConfirm)
            if(not ret)
               then
               tz_answer["success"] = false
            end

    end     


    result_json = cjson.encode(tz_answer)
    print(result_json)
end

function get_macAccess()
    local tz_answer = {}
    tz_answer["cmd"] = 123

    local data_array = {}
    local array = wifi.wifi_get_dev()
    if next(array) ~= nil then

        local id
        for k, v in pairs(array) do
            if (v["band"] == "2.4G") then id = v['wifi_id'] end
        end

        data_array["policy"], data_array["macs"] =
            wifi.wifi_get_mac_access_control(id)

    else
        data_array = ''
    end

    tz_answer["success"] = true
    tz_answer["maclist"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_macAccess()
    local tz_answer = {}
    tz_answer["cmd"] = 124

    local ret
    local array = wifi.wifi_get_dev()

    local id
    for k, v in pairs(array) do
        if (v["band"] == "2.4G") then id = v['wifi_id'] end
    end

    local policy = tonumber(tz_req["policy"])
    local maclist = tz_req["maclist"]

    if (nil ~= policy) then
        ret = wifi.wifi_set_mac_access_control(id, policy, maclist)
        if (not ret) then tz_answer["setMacAccess"] = false end
    end

    wifi.wifi_restart(id)
    tz_answer["success"] = true
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_wifi5macAccess()

    local tz_answer = {}
    tz_answer["cmd"] = 125

    local data_array = {}
    local array = wifi.wifi_get_dev()
    if next(array) ~= nil then

        local id
        for k, v in pairs(array) do
            if (v["band"] == "5G") then id = v['wifi_id'] end
        end

        data_array["policy"], data_array["macs"] =
            wifi.wifi_get_mac_access_control(id)

    else
        data_array = ''
    end

    tz_answer["success"] = true
    tz_answer["maclist"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_wifi5macAccess()

    local tz_answer = {}
    tz_answer["cmd"] = 126

    local ret
    local array = wifi.wifi_get_dev()

    local id
    for k, v in pairs(array) do
        if (v["band"] == "5G") then id = v['wifi_id'] end
    end

    local policy = tonumber(tz_req["policy"])
    local maclist = tz_req["maclist"]

    if (nil ~= policy) then
        ret = wifi.wifi_set_mac_access_control(id, policy, maclist)
        if (not ret) then tz_answer["setMacAccess"] = false end
    end

    wifi.wifi_restart(id)
    tz_answer["success"] = true
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_tr069()
    local tz_answer = {}
    tz_answer["cmd"] = 175
    local dataArr = system.system_get_tr069_info()

    tz_answer["success"] = true
    tz_answer["tr069"] = dataArr
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_wpsSet()
    local tz_answer = {}
    tz_answer["cmd"] = 171

    local array = wifi.wifi_get_dev()
    local id
    local data_array = {}
    for k, v in pairs(array) do
        if (v["band"] == "2.4G") then id = v['wifi_id'] end
    end
    data_array["switchWps"] = wifi.wifi_get_wps_pbc_enable_status(id) or {}
    data_array["getRandomPin"] = wifi.wifi_get_wps_random_pin_code(id) or {}

    tz_answer["success"] = true
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_wpsSet()
    local tz_answer = {}
    tz_answer["cmd"] = 172

    local ret
    local array = wifi.wifi_get_dev()

    local id
    for k, v in pairs(array) do
        if (v["band"] == "2.4G") then id = v['wifi_id'] end
    end

    local wpsEnble = tonumber(tz_req["wpsEnble"])
    local wpsPin = tonumber(tz_req["wpsPin"])



    if (nil ~= wpsPin) then
        if (0 == wpsPin) then
            ret = wifi.wifi_set_wps_pin(id, nil)
            if (not ret) then tz_answer["setwpsPin"] = false end
        else
            ret = wifi.wifi_set_wps_pin(id, wpsPin)
            if (not ret) then tz_answer["setwpsPin"] = false end
        end
    end

    if (nil ~= wpsEnble) then
        if(1 == wpsEnble) then
            ret = wifi.wifi_enable_wps_pbc(id)
            if (not ret) then tz_answer["enablewps"] = false end
        else
            ret = wifi.wifi_disable_wps_pbc(id)
            if (not ret) then tz_answer["disablewps"] = false end
        end
        
    end

    tz_answer["success"] = true
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_wps5Set()
    local tz_answer = {}
    tz_answer["cmd"] = 176

    local array = wifi.wifi_get_dev()
    local id
    local data_array = {}
    for k, v in pairs(array) do
        if (v["band"] == "5G") then id = v['wifi_id'] end
    end

    data_array["switchWps"] = wifi.wifi_get_wps_pbc_enable_status(id) or {}
    data_array["getRandomPin"] = wifi.wifi_get_wps_random_pin_code(id) or {}

    tz_answer["success"] = true
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_wps5Set()
    local tz_answer = {}
    tz_answer["cmd"] = 177

    local ret
    local array = wifi.wifi_get_dev()

    local id
    for k, v in pairs(array) do
        if (v["band"] == "5G") then id = v['wifi_id'] end
    end

    local wpsEnble = tonumber(tz_req["wpsEnble"])
    local wpsPin = tonumber(tz_req["wpsPin"])

    if (nil ~= wpsPin) then
        if (0 == wpsPin) then
            ret = wifi.wifi_set_wps_pin(id, nil)
            if (not ret) then tz_answer["setwpsPin"] = false end
        else
            ret = wifi.wifi_set_wps_pin(id, wpsPin)
            if (not ret) then tz_answer["setwpsPin"] = false end
        end
    end
    if (nil ~= wpsEnble) then
        if(1 == wpsEnble) then
            ret = wifi.wifi_enable_wps_pbc(id)
            if (not ret) then tz_answer["enablewps"] = false end
        else
            ret = wifi.wifi_disable_wps_pbc(id)
            if (not ret) then tz_answer["disablewps"] = false end
        end
        
    end

    tz_answer["success"] = true
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_socket_at_switch()
    local tz_answer = {}
    tz_answer["cmd"] = 204
    tz_answer["success"] = true
    tz_answer["swtich"] = system.get_socket_at_switch()
    result_json = cjson.encode(tz_answer)
    print(result_json)
end

function set_socket_at_app()

    local appEnable = tz_req["appEnable"]

    if (nil ~= appEnable) then
        if (true == appEnable) then
            system.set_socket_at_app(true)
        else
            system.set_socket_at_app(false)
        end
    end
    local tz_answer = {}
    tz_answer["cmd"] = 205
    tz_answer["success"] = true
    result_json = cjson.encode(tz_answer)
    print(result_json)
end

function set_dhcp_ip_mac()
    local ret
    local tz_answer = {}
    local dataList = tz_req["dataList"]

    if (nil ~= dataList) then
        ret = dhcp.dhcp_set_reserve_ip(dataList)

        if (not ret) then tz_answer["success"] = false end
    end

    tz_answer["cmd"] = 206
    tz_answer["success"] = true
    print(dataList)
    result_json = cjson.encode(tz_answer)

    print(result_json)
end

function route_list_get()

    local tz_answer = {}
    tz_answer["cmd"] = 211
    local data_array = {}

    data_array["routeList"] = firewall.firewall_get_static_route()
    tz_answer["success"] = true
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function route_list_set()

    local ret
    local tz_answer = {}
    local routeList = tz_req["routeList"]
    firewall.firewall_set_static_route(routeList)

    tz_answer["success"] = true
    tz_answer["cmd"] = 212
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_apn_data()

    local tz_answer = {}
    tz_answer["cmd"] = 213
    local data_array = {}

    tz_answer["success"] = true
    tz_answer["data"] = modem.modem_get_mutilapn_config()
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_apn_data()

    local ret
    local tz_answer = {}
    local apnList = tz_req["apnList"]
    if (nil ~= apnList) then
        ret = modem.modem_set_mutilapn_config(apnList)
        if (not ret) then tz_answer["success"] = false end
    end
    tz_answer["success"] = true
    tz_answer["cmd"] = 214
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_ping_list()

    local ret
    local tz_answer = {}
    local dataList = tz_req["dataList"]
    local pingFlag = tonumber(tz_req["pingFlag"])
    if (nil ~= dataList) then
        ret = firewall.firewall_remote_set_ping_list(dataList)
        if (not ret) then tz_answer["success"] = false end
    end
    if (nil ~= pingFlag) then
        ret = firewall.firewall_remote_set_ping(pingFlag)
        if (not ret) then tz_answer["success"] = false end
    end

    tz_answer["success"] = true
    tz_answer["cmd"] = 215
   
    result_json = cjson.encode(tz_answer)
    print(result_json)
    os.execute("/etc/init.d/firewall restart > /dev/null 2>&1 &")

end

function get_ping_list()

    local tz_answer = {}
    tz_answer["cmd"] = 216
    local data_array = {}
    data_array["pingList"] = firewall.firewall_remote_get_ping_list()
    data_array["dataFlag"] = firewall.firewall_remote_get_ping()

    tz_answer["success"] = true
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_web_list()

    local tz_answer = {}
    tz_answer["cmd"] = 217
    local data_array = {}
    data_array["remoteLoginPort"] = firewall.firewall_remote_get_web_login_port()
    data_array["defultList"] = firewall.firewall_remote_get_default_list()
    data_array["webList"] = firewall.firewall_remote_get_web_login_list()
    data_array["webFlag"] = firewall.firewall_remote_get_web_login()

    tz_answer["success"] = true
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_web_list()
    local ret
    local tz_answer = {}
    local remoteLoginPort = tz_req["remoteLoginPort"]
    local dataList1 = tz_req["dataList1"]
    local dataList2 = tz_req["dataList2"]
    local pingFlag = tonumber(tz_req["pingFlag"])
    if (nil ~= remoteLoginPort) then
        ret = firewall.firewall_remote_set_web_login_port(remoteLoginPort)
        if (not ret) then tz_answer["success"] = false end
    end
    if (nil ~= dataList1) then
        ret = firewall.firewall_remote_set_web_login_list(dataList1)
        if (not ret) then tz_answer["success"] = false end
    end
    if (nil ~= pingFlag) then
        ret = firewall.firewall_remote_set_web_login(pingFlag)
        if (not ret) then tz_answer["success"] = false end
    end
    if (nil ~= dataList2) then
        ret = firewall.firewall_remote_set_default_list(dataList2)
        if (not ret) then tz_answer["success"] = false end
    end

    tz_answer["success"] = true
    tz_answer["cmd"] = 218
    
    result_json = cjson.encode(tz_answer)
    print(result_json)
    os.execute("/etc/init.d/firewall restart > /dev/null 2>&1 &")
end

function set_tr069_config()
    local ret
    local tz_answer = {}
    local enabled = tz_req["enabled"]
    local url = tz_req["url"]
    local passwd = tz_req["passwd"]
    local userName = tz_req["userName"]
    local linkUserName = tz_req["linkUserName"]
    local linkPasswd = tz_req["linkPasswd"]
    local noticeEnabled = tz_req["noticeEnabled"]
    local acsAuthEnabled = tz_req["acsAuthEnabled"]
    local cpeAuthEnabled = tz_req["cpeAuthEnabled"]
    local noticeInterval = tz_req["noticeInterval"]

    ret = system.system_set_tr069_app_enable(enabled)

    ret = system.system_set_tr069_ServerURL(url)
    if (not ret) then tz_answer["success"] = false end

    if (nil ~= noticeEnabled) then
        ret = system.system_set_tr069_PeriodicInformEnable(noticeEnabled)
        if (not ret) then tz_answer["success"] = false end
    end
    ret = system.system_set_tr069_PeriodicInformInterval(noticeInterval)
    if (not ret) then tz_answer["success"] = false end

    if (nil ~= acsAuthEnabled) then
        ret = system.system_set_tr069_tr069_ACS_auth(acsAuthEnabled)
        if (not ret) then tz_answer["success"] = false end
    end
    ret = system.system_set_tr069_ServerUsername(userName)
    if (not ret) then tz_answer["success"] = false end

    ret = system.system_set_tr069_ServerPassword(passwd)
    if (not ret) then tz_answer["success"] = false end

    if (nil ~= cpeAuthEnabled) then
        ret = system.system_set_tr069_tr069_tr069_CPE_auth(cpeAuthEnabled)
        if (not ret) then tz_answer["success"] = false end
    end

    ret = system.system_set_tr069_ConnectionRequestUname(linkUserName)
    if (not ret) then tz_answer["success"] = false end

    ret = system.system_set_tr069_ConnectionRequestPassword(linkPasswd)
    if (not ret) then tz_answer["success"] = false end

    tz_answer["success"] = true
    tz_answer["cmd"] = 219

    result_json = cjson.encode(tz_answer)
    print(result_json)
    os.execute("/etc/init.d/netcwmp restart > /dev/null 2>&1 &")
    
end

function get_web_hide_config()
    local tz_answer = {}
    tz_answer["cmd"] = 220
    local data_array = {}

    tz_answer["success"] = true
    tz_answer["data"] = system.system_get_web_info()
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_auto_dial()
    local tz_answer = {}
    tz_answer["cmd"] = 221
    local data_array = {}

    tz_answer["success"] = true
    tz_answer["data"] = modem.modem_get_auto_dial()
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_open_auto_dial()
    local ret
    local tz_answer = {}
    local enable = tonumber(tz_req["enable"])
    if (nil ~= enable) then
        ret = modem.modem_enable_auto_dial(enable)
        if (not ret) then tz_answer["success"] = false end
    end

    tz_answer["success"] = true
    tz_answer["cmd"] = 222
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_close_auto_dial()
    local ret
    local tz_answer = {}
    local enable = tonumber(tz_req["enable"])
    if (nil ~= enable) then
        ret = modem.modem_disable_auto_dial(enable)
        if (not ret) then tz_answer["success"] = false end
    end
    tz_answer["success"] = true
    tz_answer["cmd"] = 223
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function restore_factory()
    local tz_answer = {}
    tz_answer["cmd"] = 224
    local data_array = {}

    tz_answer["success"] = true
    os.execute("/etc/tozed/tz_restore_factory")
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_dmz_data()
    local tz_answer = {}
    tz_answer["cmd"] = 226
    local data_array = {}

    tz_answer["success"] = true
    tz_answer["enable"], tz_answer["ip"] = firewall.firewall_get_dmz()
    result_json = cjson.encode(tz_answer)
    print(result_json)

end
function set_dmz_data()
    local ret
    local tz_answer = {}
    local enable = tz_req["enable"]
    local ip = tz_req["ip"]
    if (nil ~= enable) then
        ret = firewall.firewall_set_dmz(enable, ip)
        if (not ret) then tz_answer["success"] = false end
    end
    tz_answer["success"] = true
    tz_answer["cmd"] = 225
    result_json = cjson.encode(tz_answer)
    print(result_json)
    os.execute("/etc/init.d/firewall restart > /dev/null 2>&1 &")
end

function get_arp_data()
    local tz_answer = {}
    tz_answer["cmd"] = 228
    local data_array = {}

    tz_answer["success"] = true
    tz_answer["data"] = firewall.firewall_get_arp_bind_list()
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_arp_data()

    local ret
    local tz_answer = {}
    local routeList = tz_req["routeList"]
    if (nil ~= routeList) then
        ret = firewall.firewall_set_arp_bind_list(routeList)
        if (not ret) then tz_answer["success"] = false end
    end

    tz_answer["success"] = true
    tz_answer["cmd"] = 229
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function system_export_config()

    local tz_answer = {}
    tz_answer["cmd"] = 230
    local data_array = {}
    local data_array = tz_req[""]
    tz_answer["success"] = true
    tz_answer["data"] =
        system.system_export_config("/tz_www/html/export_config")
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function system_import_config()
    local ret
    local tz_answer = {}
    tz_answer["cmd"] = 231
    local configFile = tz_req["configFile"]
    if (nil ~= configFile) then
        ret = system.system_import_config(configFile)
        if (not ret) then
            tz_answer["success"] = false
        else
            tz_answer["success"] = true
        end
    end

    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_ddns_data()
    local tz_answer = {}
    tz_answer["cmd"] = 232
    local data_array = {}

    tz_answer["success"] = true
    tz_answer["data"] = system.system_ddns_get_config()
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_ddns_data()
    local ret
    local tz_answer = {}
    local enabled = tonumber(tz_req["enable"])
    local username = tz_req["username"]
    local hostname = tz_req["hostname"]
    local password = tz_req["password"]
    local service = tz_req["service"]
    tz_answer["cmd"] = 233
    local config = {
        ["enable"] = enabled,
        ["service"] = service,
        ["username"] = username,
        ["password"] = password,
        ["hostname"] = hostname
    }

    if (nil ~= config) then
        ret = system.system_ddns_set_config(config)
        if (not ret) then tz_answer["success"] = false end
    end

    tz_answer["success"] = true

    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function firewall_upnp_get_data()
    local tz_answer = {}
    tz_answer["cmd"] = 236
    local data_array = {}
    local data_data = {}

    tz_answer["success"] = true
    data_data["status"] = firewall.firewall_upnp_get_enable_status()
    data_data["upnuList"] = firewall.firewall_upnp_get_port_mapping_list()
    tz_answer['data'] = data_data
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function set_firewall_upnp_enable()
    local ret
    local tz_answer = {}
    ret = firewall.firewall_upnp_enable()
    if (not ret) then tz_answer["success"] = false end

    tz_answer["success"] = true
    tz_answer["cmd"] = 234
    result_json = cjson.encode(tz_answer)
    print(result_json)

end
function set_firewall_upnp_disable()
    local ret
    local tz_answer = {}
    ret = firewall.firewall_upnp_disable()
    if (not ret) then tz_answer["success"] = false end

    tz_answer["success"] = true
    tz_answer["cmd"] = 235
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_info()
    local data_array = {}

    data_array["wan_mac"] = network.network_get_wan_info()['mac']

        data_array["sim_imsi"] = sim.sim_get_status()['imsi'] or ''
        data_array["sim_iccid"] = sim.sim_get_status()['iccid'] or ''
        data_array["modem_imei"] = modem.modem_get_info()['imei'] or ''
        data_array["modem_type"] = modem.modem_get_info()['type'] or ''
        data_array["modem_softver"] = modem.modem_get_info()['softver'] or ''
    data_array["factory_imei"] = system.system_get_tozed_factory_info()['imei'] or ''
        data_array["type"] = system.get_divice_version()['type'] or ''
        data_array["real_software_version"] = system.get_divice_version()['software_version'] or ''
        data_array["build"] = system.get_divice_version()['build'] or ''
    data_array["runtime"] = system.system_get_status()['runtime'] or ''
        data_array["config_software"] = system.system_get_tozed_system_info()['software_version'] or ''
        data_array["config_version"] = system.system_get_tozed_system_info()['config_version'] or ''
        data_array["device_sn"] = system.system_get_tozed_system_info()['device_sn'] or ''

    local array = wifi.wifi_get_dev()
    if next(array) ~= nil then
        for k, v in pairs(array) do
            if (v["band"] == "2.4G") then
                data_array['twopointfour_g_mac'] = v['mac']
                data_array['twopointfour_g_ssid'] = v['ssid']
            end
        end
    else
        data_array = ''
    end

        local array1 = dhcp.dhcp_get_object_list()
        data_array['lan_mac'] = array1[1]['lan_mac']

    local array3 = wifi.wifi_get_dev()
    if next(array3) ~= nil then
        for k, v in pairs(array3) do
            if (v["band"] == "5G") then
                                data_array['fivepointeight_g_mac'] = v['mac']
                                data_array['fivepointeight_g_ssid'] = v['ssid']
            end
        end
    else
        data_array = ''
    end

    local tz_answer = {}
    tz_answer["success"] = true
    tz_answer["cmd"] = 237
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_login_right_info()
    local data_array = {}

    data_array["factory_imei"] = system.system_get_tozed_factory_info()['imei'] 
    data_array["software_version"] = system.system_get_tozed_system_info()['software_version'] 
    data_array["config_version"] = system.system_get_tozed_system_info()['config_version']
    data_array["device_sn"] = system.system_get_tozed_system_info()['device_sn'] 
    data_array["version_type"] = system.get_divice_version()['type']
    data_array["modem_act"] = modem.modem_get_status()['act'] 
    data_array["modem_rsrp"] = modem.modem_get_status()['rsrp']
    data_array["modem_rssi"] = modem.modem_get_status()['rssi'] 
    data_array["modem_rsrq"] = modem.modem_get_status()['rsrq'] 
    data_array["modem_sinr"] = modem.modem_get_status()['sinr'] 
    data_array["wan_ipaddr"] = network.network_get_wan_info()['ipaddr'] 
    data_array["wan_netmask"] = network.network_get_wan_info()['netmask'] 
    data_array["wan_gateway"] = network.network_get_wan_info()['gateway'] 
    data_array["wan_first_dns"] = network.network_get_wan_info()['first_dns']
    data_array["wan_second_dns"] = network.network_get_wan_info()['second_dns'] 
    data_array["fourg_ipaddr"] = network.network_get_4g_net_info()['ipaddr'] 
    data_array["fourg_netmask"] = network.network_get_4g_net_info()['netmask'] 
    data_array["fourg_gateway"] = network.network_get_4g_net_info()['gateway'] 
    data_array["fourg_dns1"] = network.network_get_4g_net_info()['first_dns'] 
    data_array["fourg_dns2"] = network.network_get_4g_net_info()['second_dns'] 

    local tz_answer = {}
    tz_answer["success"] = true
    tz_answer["cmd"] = 238
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function open_random_pin()
        local data_array  = {}
        local ret
        local tz_answer = {}

    local array = wifi.wifi_get_dev()
    local id
    local data_array = {}
    for k, v in pairs(array) do
        if (v["band"] == "2.4G") then id = v['wifi_id'] end
    end
        ret = wifi.wifi_create_wps_random_pin_code(id)
        if (not ret) 
            then 
                tz_answer["success"] = false 
        end

        tz_answer["success"] = true
        tz_answer["cmd"] = 239
        result_json = cjson.encode(tz_answer)
        print(result_json)

end

function open_5g_random_pin()
        local data_array  = {}
        local ret
        local tz_answer = {}

    local array = wifi.wifi_get_dev()
    local id
    local data_array = {}
    for k, v in pairs(array) do
        if (v["band"] == "5G") then id = v['wifi_id'] end
    end
        ret = wifi.wifi_create_wps_random_pin_code(id)
        if (not ret) 
            then 
                tz_answer["success"] = false 
        end

        tz_answer["success"] = true
        tz_answer["cmd"] = 240
        result_json = cjson.encode(tz_answer)
        print(result_json)

end

function get_web_log() 
    local data_array = {}

    data_array["logs"] = uti.get_web_log()

    local tz_answer = {}
    tz_answer["success"] = true
    tz_answer["cmd"] = 241
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function clear_web_log()
    local data_array = {}

    uti.clear_web_log()

    local tz_answer = {}
    tz_answer["success"] = true
    tz_answer["cmd"] = 242
    -- tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function wifi_advanced_config()

    local tz_answer = {}
    tz_answer["cmd"] = 243
    local ret
    local array = wifi.wifi_get_dev()

    local id
    for k, v in pairs(array) do
        if (v["band"] == "2.4G") then id = v['wifi_id'] end
    end

    
    local txPower = tonumber(tz_req["txPower"])
    local maxStation = tonumber(tz_req["maxStation"])
    local channel = tz_req["channel"]
    local mode = tonumber(tz_req["wifiWorkMode"])
    local ht = tz_req["ht"]

    if (nil ~= txPower) then
        ret = wifi.wifi_set_txpower(id, txPower)
        if (not ret) then tz_answer["setTxpower"] = false end
    end

    if (nil ~= maxStation) then
        ret = wifi.wifi_set_connect_sta_number(id, maxStation)
        if (not ret) then tz_answer["setnumber"] = false end
    end

    if (nil ~= channel) then
        ret = wifi.wifi_set_channel(id, channel)
        if (not ret) then tz_answer["setChannel"] = false end
    end

    if (nil ~= mode) then
        ret = wifi.wifi_set_mode(id, mode)
        if (not ret) then tz_answer["setMode"] = false end
    end

    if (nil ~= ht) then
        ret = wifi.wifi_set_bandwidth(id, ht)
        if (not ret) then tz_answer["setHtMode"] = false end
    end

    wifi.wifi_restart(id)
    tz_answer["success"] = true
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function wifi5_advanced_config()

    local tz_answer = {}
    tz_answer["cmd"] = 244
    local ret
    local array = wifi.wifi_get_dev()

    local id
    for k, v in pairs(array) do
        if (v["band"] == "5G") then id = v['wifi_id'] end
    end

    local channel = tz_req["channel"]
    local mode = tonumber(tz_req["wifiWorkMode"])
    local bandWidth = tz_req["bandWidth"]
    local maxStation = tonumber(tz_req["maxStation"])

    if (nil ~= channel) then
        ret = wifi.wifi_set_channel(id, channel)
        if (not ret) then tz_answer["setChannel"] = false end
    end

    if (nil ~= mode) then
        ret = wifi.wifi_set_mode(id, mode)
        if (not ret) then tz_answer["setMode"] = false end
    end

    if (nil ~= bandWidth) then
        ret = wifi.wifi_set_bandwidth(id, bandWidth)
        if (not ret) then tz_answer["setBwMode"] = false end
    end

    if (nil ~= maxStation) then
        ret = wifi.wifi_set_connect_sta_number(id, maxStation)
        if (not ret) then tz_answer["setnumber"] = false end
    end

    wifi.wifi_restart(id)
    tz_answer["success"] = true
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function wan_mac_get()
    local tz_answer={}
    local data_array={}

    data_array["model"] = network.network_get_lan_wan_mode() or {}
    data_array["mac"] = network.network_get_wan_mac() or {}

    tz_answer["success"] = true
    tz_answer["cmd"] = 245
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)
end

function wan_mac_settings()
    local tz_answer = {}
    local mac = {}
    local model = {}
    local ret
    
    model = tz_req["model"] 
    mac = tz_req["mac"]
    tz_answer["cmd"] = 246
    ret = network.network_set_wan_mac(mac)

    if (not ret) 
        then 
        tz_answer["setMac"] = false 
    end

    if(model ~= nil)
        then
        ret = network.network_set_lan_wan_mode(model)
        if(not ret)
            then
            tz_answer["setModel"] = false
        end    
    end

    tz_answer["success"] = true
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function firewall_url_default_get()
    local tz_answer = {}
    local data_array = {}
    tz_answer["cmd"] = 247
    data_array = firewall.firewall_get_url_default_action() or {}
    tz_answer["success"] = true
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function firewall_url_default_set()
    local tz_answer = {}
    local data_array = {}
    local ret
    local url_default = {}
    tz_answer["cmd"] = 248
    url_default = tz_req["url_default"]
    ret = firewall.firewall_set_url_default_action(url_default)
    if(not ret)
        then
        tz_answer["url_default"] = false
     end   
    tz_answer["success"] = true
    result_json = cjson.encode(tz_answer)
    print(result_json)
    os.execute("/etc/init.d/firewall restart > /dev/null 2>&1 &")

end

function get_home_info()
    local data_array = {}

    -- data_array["wan"] = network.network_get_wan_info()
    -- data_array["4g"] = network.network_get_4g_net_info()
    -- data_array["4g1"] = network.network_get_4g1_net_info()
    -- data_array["4g2"] = network.network_get_4g2_net_info()

    data_array['systime'] = os.time() 

    local sys_status = {}
    data_array['sys_status'] = sys_status
    local data_wifi = {}
    local sim_status = sim.sim_get_status() or {}
    local modem_status = modem.modem_get_status() or {}
    sys_status["is_sim_exist"] = sim_status["is_sim_exist"] or ''
    sys_status["card_status"] = sim_status["card_status"] or ''
    sys_status["2g_register_status"] = modem_status["2g_register_status"] or ''
    sys_status["3g_register_status"] = modem_status["3g_register_status"] or ''
    sys_status["4g_register_status"] = modem_status["4g_register_status"] or ''
    sys_status["rsrp"] = modem_status["rsrp"]
    sys_status["signal_lvl"] = modem_status["signal_lvl"]
    sys_status["network_link_stauts"] = modem_status["network_link_stauts"]
    sys_status["ethInter"] = network.network_get_interface_up_down_status() or ''
    local array = wifi.wifi_get_dev()
    for k, v in pairs(array) do
        if (v["band"] == "2.4G") then
            data_wifi['status'] = wifi.wifi_get_enable_status(v['wifi_id'])
        end
        if (v["band"] == "5G") then
            data_wifi['status5'] = wifi.wifi_get_enable_status(v['wifi_id'])
        end
    end
    sys_status["wifi"] = data_wifi

    data_array['sim_card_status'] = sim.sim_get_status()['card_status'] or ''

    data_array['web_language_select_enable'] = system.system_get_web_info()['web_language_select_enable']

    local tz_answer = {}
    tz_answer["success"] = true
    tz_answer["cmd"] = 249
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

function get_login_info()
    local data_array = {}

    local sys_status = {}
    data_array['sys_status'] = sys_status
    local data_wifi = {}
    local sim_status = sim.sim_get_status() or {}
    local modem_status = modem.modem_get_status() or {}
    sys_status["is_sim_exist"] = sim_status["is_sim_exist"] or ''
    sys_status["card_status"] = sim_status["card_status"] or ''
    sys_status["2g_register_status"] = modem_status["2g_register_status"] or ''
    sys_status["3g_register_status"] = modem_status["3g_register_status"] or ''
    sys_status["4g_register_status"] = modem_status["4g_register_status"] or ''
    sys_status["rsrp"] = modem_status["rsrp"]
    sys_status["signal_lvl"] = modem_status["signal_lvl"]
    sys_status["network_link_stauts"] = modem_status["network_link_stauts"]
    sys_status["ethInter"] = network.network_get_interface_up_down_status() or ''
    local array = wifi.wifi_get_dev()
    for k, v in pairs(array) do
        if (v["band"] == "2.4G") then
            data_wifi['status'] = wifi.wifi_get_enable_status(v['wifi_id'])
        end
        if (v["band"] == "5G") then
            data_wifi['status5'] = wifi.wifi_get_enable_status(v['wifi_id'])
        end
    end
    sys_status["wifi"] = data_wifi

    data_array['sim_card_status'] = sim.sim_get_status()['card_status'] or ''

    local language = {}
    data_array['language'] = language
    language['web_language_select_enable'] = system.system_get_web_info()['web_language_select_enable']
    language['web_language'] = system.system_get_web_info()['web_language']
    
    local main_info = {}
    data_array['main_info'] = main_info
    main_info["factory_imei"] = system.system_get_tozed_factory_info()['imei'] 
    main_info["software_version"] = system.system_get_tozed_system_info()['software_version'] 
    main_info["config_version"] = system.system_get_tozed_system_info()['config_version']
    main_info["device_sn"] = system.system_get_tozed_system_info()['device_sn'] 
    main_info["version_type"] = system.get_divice_version()['type']
    main_info["modem_act"] = modem.modem_get_status()['act'] 
    main_info["modem_rsrp"] = modem.modem_get_status()['rsrp']
    main_info["modem_rssi"] = modem.modem_get_status()['rssi'] 
    main_info["modem_rsrq"] = modem.modem_get_status()['rsrq'] 
    main_info["modem_sinr"] = modem.modem_get_status()['sinr'] 
    main_info["wan_ipaddr"] = network.network_get_wan_info()['ipaddr'] 
    main_info["wan_netmask"] = network.network_get_wan_info()['netmask'] 
    main_info["wan_gateway"] = network.network_get_wan_info()['gateway'] 
    main_info["wan_first_dns"] = network.network_get_wan_info()['first_dns']
    main_info["wan_second_dns"] = network.network_get_wan_info()['second_dns'] 
    main_info["fourg_ipaddr"] = network.network_get_4g_net_info()['ipaddr'] 
    main_info["fourg_netmask"] = network.network_get_4g_net_info()['netmask'] 
    main_info["fourg_gateway"] = network.network_get_4g_net_info()['gateway'] 
    main_info["fourg_dns1"] = network.network_get_4g_net_info()['first_dns'] 
    main_info["fourg_dns2"] = network.network_get_4g_net_info()['second_dns'] 

    local tz_answer = {}
    tz_answer["success"] = true
    tz_answer["cmd"] = 250
    tz_answer["data"] = data_array
    result_json = cjson.encode(tz_answer)
    print(result_json)

end

local switch = {
    [0] = get_sysinfo,
    [1] = get_systime,
    [2] = set_wifi,
    [3] = set_dhcp,
    [6] = reboot_sys,
    [10] = get_datetime,
    [11] = set_datetime,
    [19] = deal_at,
    [20] = firewall_restart,
    [21] = port_filter,
    [22] = ip_filter,
    [23] = mac_filter,
    [24] = ipmac_binding,
    [25] = speed_limit,
    [26] = url_filtet,
    [27] = port_redirect,
    [28] = default_action,
    [29] = acl_filter,
    [39] = clear_allrule,
    [43] = get_diviceinfo,
    [80] = iniPage,
    [97] = change_language,
    [99] = logout,
    [100] = login,
    [101] = get_wifi,
    [102] = get_dhcp_list,
    [103] = get_dhcpClient,
    [106] = update_partial,
    [113] = get_sysstatus,
    [117] = get_wifiClient,
    [118] = get_wifi5Client,
    [120] = get_ssidlist,
    [121] = set_ssidlist,
    [123] = get_macAccess,
    [124] = set_macAccess,
    [125] = get_wifi5macAccess,
    [126] = set_wifi5macAccess,
    [133] = get_routerinfo,
    [134] = get_simstatus,
    [135] = set_pinstatus,
    [145] = network_tool,
    [160] = set_lockceel,
    [162] = get_lockceel,
    [163] = get_networkSet,
    [164] = set_networkSet,
    [165] = get_supprotband,
    [166] = set_supprotband,
    [167] = get_remoteLogin,
    [169] = get_plmnSet,
    [170] = set_plmnSet,
    [171] = get_wpsSet,
    [172] = set_wpsSet,
    [173] = get_remotePin,
    [175] = get_tr069,
    [176] = get_wps5Set,
    [177] = set_wps5Set,
    [184] = config_update,
    [201] = get_wifi5,
    [202] = set_wifi5,
    [203] = get_lan,
    [204] = get_socket_at_switch,
    [205] = set_socket_at_app,
    [206] = set_dhcp_ip_mac,
    [207] = get_sysinfo_2,
    [208] = get_sysinfo_3,
    [209] = get_sysinfo_4,
    [210] = get_sysinfo_5,
    [211] = route_list_get,
    [212] = route_list_set,
    [213] = get_apn_data,
    [214] = set_apn_data,
    [215] = set_ping_list,
    [216] = get_ping_list,
    [217] = get_web_list,
    [218] = set_web_list,
    [219] = set_tr069_config,
    [220] = get_web_hide_config,
    [221] = get_auto_dial,
    [222] = set_open_auto_dial,
    [223] = set_close_auto_dial,
    [224] = restore_factory,
    [225] = set_dmz_data,
    [226] = get_dmz_data,
    [228] = get_arp_data,
    [229] = set_arp_data,
    [230] = system_export_config,
    [231] = system_import_config,
    [232] = get_ddns_data,
    [233] = set_ddns_data,
    [234] = set_firewall_upnp_enable,
    [235] = set_firewall_upnp_disable,
    [236] = firewall_upnp_get_data,
    [237] = get_info,
    [238] = get_login_right_info,
    [239] = open_random_pin,
    [240] = open_5g_random_pin,
    [241] = get_web_log,
    [242] = clear_web_log,
    [243] = wifi_advanced_config,
    [244] = wifi5_advanced_config,
    [245] = wan_mac_get,
    [246] = wan_mac_settings,
    [247] = firewall_url_default_get,
    [248] = firewall_url_default_set,
    [249] = get_home_info,
    [250] = get_login_info
}

cmdid = uti.get_env_cmdId(envv)
if cmdid ~= nil then
    uti.web_log(cmdid)
    if "5" == cmdid then upload_file() end
else
  --data1=io.read();
    local content_len = uti.get_env_content_len(envv)
    data1 = uti.read_stdin(content_len)
    tz_req = cjson.decode(data1)
    local cmd = tz_req["cmd"]
    if (cmd ~= 100 and cmd ~= 80 and cmd ~= 133 and cmd ~= 97 and cmd ~= 113 and
        cmd ~= 220 and cmd ~= 43 and cmd ~= 0 and cmd ~= 208 and cmd ~= 101 and cmd ~= 238 and cmd ~= 134 and cmd ~= 250)  then
        local fileName = string.format("/tmp/sessionsave/.%s",
                                       tz_req["sessionId"])
        if (uti.is_file_exist(fileName) ~= true) then
            local tz_answer = {}
            tz_answer["cmd"] = tz_req["cmd"]
            tz_answer["success"] = false
            result_json = cjson.encode(tz_answer)
            print(result_json)
            return

        else

            local f = switch[tz_req["cmd"]]
            if (f) then f() end
            if (tz_req["cmd"] ~= 99) then
                local logintime = os.time()
                local file = io.open(fileName, "w")
                io.input(file)
                file:write("updateTime:" .. logintime)
                io.close(file)
            end
        end

    else

        local f = switch[tz_req["cmd"]]
        if (f) then f() end
    end

end

