/**
 * Created by dell on 2019/1/8.
 */

var ProgressTime = {
    REBOOT: 120,
    FIND_AP: 15,
    UPDATE_UBOOT: 10,
    UPDATE_PARTIAL: 260,
    UPLOAD_FILE: 60,
    SEARCH_PLMN: 120,
    UPGRADE_CHECK: 60,
    REMOTE_UPGRADE: 240
};

var RequestCmd = {
    SYS_INFO: 0,
	GET_SYSTEM_TIME: 1,
    WIRELESS_CONFIG: 2,
    NETWORK_CONFIG: 3,
    SYS_UPDATE: 5,
	SYS_REBOOT: 6,
	
	BASIC_CONFIG: 9,
	GET_DATETIME:10,
    SET_DATETIME: 11,
	
    SYS_LOG: 17,
	SYS_AT:19,

    APPLY_FILTER: 20,
    PORT_FILTER: 21,
    IP_FILTER: 22,
    MAC_FILTER: 23,
    IP_MAC_BINDING: 24,
    SPEED_LIMIT: 25,
    URL_FILTER: 26,
    OTHER_FILTER: 27,
    DEFAULT_FILTER: 28,
    ACL_FILTER:29,
    CLEAN_ALL_FILTER: 39,


    DEVICE_VERSION_INFO: 43,
    INIT_PAGE: 80,
    CHANGE_LANGUAGE: 97,
	LOGOUT: 99,
    LOGIN: 100,

    WIFI_INFO: 101,
    DHCP_INFO:102,
    DHCPCLIENT_INFO:103,

	UPDATE_PARTIAL: 106,
    GET_SYS_STATUS: 113,

    WIFI_CONTROL_INFO: 117,
    WIFI5_CONTROL_INFO: 118,

    SSIDLIST_INFO:120,
    SSIDLIST_CONFIG:121,
	
	MAC_ACCESS_CONTROL:123,
	SET_MAC_CONTROL:124,
	
	WIFI5_MAC_CONTROL:125,
    SET_WIFI5_MAC_CONTROL:126,

    ROUTER_INFO: 133,
	QUERY_SIM_STATUS: 134,
	SET_PIN_STATUS:135,

    CHANGE_PASSWD: 144,
    NETWORK_TOOL: 145,
	SYS_TIME:146,

    LOCK_PHY_CELL: 160,	
	LOCK_ONE_CELL: 162,
	
	NETWORKSET_GET:163,
	NETWORKSET_SET:164,
	
	LOCK_BAND_GET: 165,
    LOCK_BAND_SET: 166,
	
	REMOTE_LOGIN_GET:167,
    REMOTE_LOGIN_SET:168,
	
	LOCK_PLMN_GET:169,
    LOCK_PLMN_SET:170,
	
	WPS_PIN_GET:171,
    WPS_PIN_SET:172,
	
	REMOTE_PIN_GET:173,
    REMOTE_PIN_SET:174,
	
	TR069_CONFIG: 175,
	
	WPS5_PIN_GET:176,
    WPS5_PIN_SET:177,
	
	CONFIG_UPDATE: 184,

    WIFI5_INFO: 201,
    WIRELESS5_CONFIG: 202,
	LAN_INFO: 203

};

var MenuItem = {
    SYS_INFO: { cmd: RequestCmd.SYS_INFO, url: "html/info/sysInfo.html" },
    DEVICE_INFO: { cmd: RequestCmd.DEVICE_VERSION_INFO, url: "html/info/deviceInfo.html" },

    NETWORK_CONFIG: { cmd: RequestCmd.NETWORK_CONFIG, url: "html/config/networkConfig.html" },
    WIRELESS_CONFIG: { cmd: RequestCmd.WIRELESS_CONFIG, url: "html/config/wirelessConfig.html" },
	
    SYS_SET: { cmd: RequestCmd.CHANGE_PASSWD, url: "html/sys/sysConfigIndex.html" },
    SYS_LOG: { cmd: RequestCmd.SYS_LOG, url: "html/manage/sysLog.html" },
    SYS_UPDATE:	{ cmd: RequestCmd.SYS_UPDATE, url: "html/update/sysUpdate.html" },
	CONFIG_UPDATE:	{ cmd: RequestCmd.CONFIG_UPDATE, url: "html/update/configUpdate.html" },
	SYS_TIME:{ cmd: RequestCmd.SYS_TIME, url: "html/sys/timeSetting.html" },
	SYS_REBOOT: { cmd: RequestCmd.SYS_REBOOT, url: "" },

    
};

var JSONMethod = {
    GET: "GET",
    POST: "POST"
};

var Url = {
    LOGIN: '/login.html',
    INDEX: '/index.html',
    PASSWD: '/html/changePasswd.html',

    DEFAULT_CGI: '/cgi-bin/lua.cgi'
    //DEFAULT_CGI: 'xml_action.cgi?method=set&module=duster&file=http'
};

var DHCPServer = {
    OPENED: "1",
    CLOSED: "0"
};

var AlertUtil = {
    alertMsg: function(msg) {
        alert(msg);
    },
    confirm: function(msg) {
        return confirm(msg);
    }
};


var CookieUtil = {
    getCookie: function(name) {
        var str = document.cookie;
        if(!str || str.indexOf(name + '=') < 0) {
            return;
        }
        var cookies = str.split(';');
        for(var i = 0; i < cookies.length; i++) {
            var cookie = cookies[i].trim();
            if(cookie.indexOf(name + '=') == 0) {
                return decodeURI(cookie.substring(name.length + 1));
            }
        }
    },
    setCookie: function(name, value, option) {
        var str = String.format("{0}={1}", name, encodeURI(value));

        if(option){
            if(option.expireHours){
                var date = new Date();
                date.setTime(date.getTime() + option.expireHours * 3600 * 1000);
                str += String.format("; expires={0}", date.toGMTString());
            }
            if(option.path) {
                str += String.format("; path={0}", option.path);
            }
            if(option.domain) {
                str += String.format("; domain={0}", option.domain);
            }
            if(option.secure) {
                str += '; true';
            }
        }

        document.cookie = str;
    },
    deleteCookie: function(name, option) {
        this.setCookie(name, '', option);
    }
};

var InputUtil = {
    setRadiosSelectedValue: function($radios, value) {
        $radios.each(function(){
            if($(this).val() == value.toString()){
                $(this).attr("checked", "checked");
            }
        });
    },
};

var FormatUtil = {
    formatValue: function (value) {
        if (!value || value == "NULL") {
            return '';
        }
        return value;
    },
    formatField: function (value, unit) {
        if (unit) {
            unit = "&nbsp;" + unit;
        } else {
            unit = "";
        }
        if (value == "NULL") {
            //isNULLToSpace设置为false 此模式可以取消，注意在各页面使用完后要恢复默认值
            if(Page.isNULLToSpace){
                return '';
            }
            return String.format("<span class=\"fail\">{0}</span>", DOC.status.getFailed);
        } else {
            return String.format("{0}{1}" , value, unit).replaceQuote();
        }
    },
    /*此函数为搜网时调用，不是显示系统状态时调用*/
    formatSingalLevel: function(singalLevel, encryptionKey, requiredTitle) {
        var isEncrypted = (!!encryptionKey && encryptionKey == "on");
        var singal = ConvertUtil.parseSingalLevel(singalLevel);
        var title = String.format("信号强度：{0} &nbsp; {1}", singal.desc, singal.level > 0 ? (isEncrypted ? "是否加密：已加密" : "是否加密：未加密") : "");
        return {
            isEncrypted: isEncrypted,
            text: String.format("<span class=\"singal singal{0}{1}\"{2}>{3}&nbsp;dBm</span>",
                singal.level, isEncrypted ? "_lock" : "",
                (requiredTitle || false) ? String.format(" title=\"{0}\"", title) : "",
                singalLevel),
            title: title
        };
    },
    KB: 1024,
    MB: 1024 * 1024,
    GB: 1024 * 1024 * 1024,
    formatByteSize: function(size) {
        if (size < this.KB) {
            return size + ' B';
        } else if (size < this.MB) {
            return (size / this.KB).toFixed(2) + ' KB';
        } else if (size < this.GB) {
            return (size / this.MB).toFixed(2) + ' MB';
        } else {
            return (size / this.GB).toFixed(2) + ' GB';
        }
    },
    getPLMNList: function() {
        var brands = DOC.brand;
        return {
            PLMN26801: 'VODAFONE',
            PLMN26803: 'NOS (OPTIMUS)',
            PLMN26806: 'MEO (TMN)',
            PLMN46000: brands.mobile,
            PLMN46001: brands.unicom,
            PLMN46002: brands.mobile,
            PLMN46003: brands.telecom,
            PLMN46007: brands.mobile,
            PLMN46011: brands.telecom,
            PLMN42701: 'Ooredoo',
            PLMN41311: 'Dialog',
            PLMN41317: 'Dialog',
            PLMN41302: 'Dialog'
        }
    },
    formatPLMN: function(plmn) {
        var plmns = FormatUtil.getPLMNList();
        var ret = plmns['PLMN' + plmn];
        if (ret) {
            return String.format('{0} / {1}', plmn, ret);
        } else if (plmn && plmn.length > 0) {
            return plmn;
        }
        return '-';
    }
};

var ConvertUtil = {

    parseHex: function(hex) {
        if (!hex) return "0000";

        var bits = ['0000', '0001', '0010', '0011',
            '0100', '0101', '0110', '0111',
            '1000', '1001', '1010', '1011',
            '1100', '1101', '1110', '1111'];
        var sb = new StringBuilder();
        var length = hex.length;
        for (var i = 0, len = length; i < len; i++) {
            sb.append(bits[parseInt(hex.charAt(i), 16)]);
        }
        return sb.toString();
    },
    timeStamp:function(StatusMinute){
        var day=parseInt(StatusMinute/3600/24);
        var hour=parseInt(StatusMinute/3600%24);
        var min= parseInt(StatusMinute/60% 60);
        StatusMinute="";
        if (day > 0)
        {
            StatusMinute= day + DOC.unit.day;
        }
        if (hour>0)
        {
            StatusMinute += hour + DOC.unit.hour;
        }
        if (min>0)
        {
            StatusMinute += parseFloat(min) + DOC.unit.min;
        }
        return StatusMinute;
    },
    ip4ToNum: function(ipStr){
        var ips = this.ip4ToBytes(ipStr);
        var ip = "0x";
        var value, i;
        for(i = 0; i < 4; i++) {
            value = ips[i].toString(16);
            if(value.length == 1){
                value = "0" + value;
            }
            ip += value;
        }

        return parseInt(ip, 16);
    },
    ip4ToBytes: function(ipStr) {
        var ips = ipStr.split(".");
        var length = ips.length;

        if (length != 4) {
            return null;
        }

        var temp, ip = [];
        for(var i = 0; i < length; i++) {
            temp = parseInt(ips[i], 10);
            if (isNaN(temp) || temp < 0 || temp > 255) {
                return null;
            }
            ip[i] = temp;
        }
        return ip;
    }

};

var SysUtil = {
    setBoxStyle: function($box, w, h) {
        $box.css({
            "left": parseInt(((w || document.documentElement.clientWidth) - $box.width()) / 2, 10) + "px",
            "top": parseInt(((h || document.documentElement.clientHeight) - $box.height()) / 2, 10) + "px"
        });
    },
    setBoxStyle1: function($box, w, h) {
        $box.css({
            "left": parseInt(((w || document.documentElement.clientWidth -600) - $box.width()) / 2, 10) + "px",
            "top": parseInt(((h || document.documentElement.clientHeight -600) - $box.height()) / 2, 10) + "px"
        });
    },
	rebootDevice: function(msg) {
        if (!AlertUtil.confirm(msg)) {
            return;
        }
        Page.postJSON({
            json: {
                cmd: RequestCmd.SYS_REBOOT,
                method: JSONMethod.POST
            },
            success: function() {
                var isOk = false;
                var timeoutCtl = setTimeout(function() {
                    isOk = true;
                }, 1000 * 90);
                SysUtil.showProgress(ProgressTime.REBOOT, PROMPT.status.rebooting,
                    function() {
                        return isOk;
                    },
                    function() {
                        clearTimeout(timeoutCtl);
                        location.reload();


                    }
                );
            },
            fail: function() {

            }
        });
    },
    parseJSON: function(data) {
        var index = data.indexOf("{");
        if (index < 0) return {};

        // 从大括号开始计算json起始位置
        if (index > 0) {
            data = data.substring(data.indexOf("{"));
        }
        return JSON.parse(data);
    },
    showProgress: function(seconds, message, checkStatus, callback) {
        var $mask = $('#mask'), $box = $('#progress_box');
        var $progress = $('#progress_status'), $info = $('#progress_info');
        var h = document.documentElement.clientHeight;
        var w = document.documentElement.clientWidth;
        $mask.height(h);
        $mask.show();
        $box.show();
        SysUtil.setBoxStyle($box, w, h);

        var count = 1, delayCount = 2, maxCount = seconds * 10, ratio;
        var width = $('#progress_bar').width();
        function loop() {
            if (checkStatus()) {
                count += parseInt((maxCount - count) / delayCount) + 1;
            } else {
                ratio = maxCount / count;
                if (ratio >= 3) count += 3;
                else if (ratio >= 2) count += 2;
                //else if (ratio > 1 && maxCount - count > delayCount) count++;
				else  count++;
            }
            if (count <= maxCount) {
                $info.text(message + DOC.comma + PROMPT.status.progress+" " + parseInt((100 * count) / maxCount) + "%");
                $progress.width(parseInt((width * count) / maxCount));
                setTimeout(loop, 100);
            } else {
                callback();
            }
        }

        loop();
    },
    upload: function($form, $file, command, callback) {
        var url = String.format("{0}?cmd={1}&method=POST",  '/cgi-bin/lua.cgi', RequestCmd.SYS_UPDATE);
        
		var datas = null;
        $form.ajaxSubmit({
            url: url,
            type: 'POST',
            dataType: 'json',
            beforeSubmit: function() {
                var updateFileName = $file.val();
                if (updateFileName.length == 0) {
                    AlertUtil.alertMsg(CHECK.required.uploadFile);
                    return false;
                }

                if(/[\\\/]/.test(updateFileName)) {
                    var matchs = updateFileName.match(/(.*)?[\\\/](.*)/);
                    updateFileName = matchs[2];
                }

                if(confirm(PROMPT.confirm.uploadFile + updateFileName)){
                    SysUtil.showProgress(ProgressTime.UPLOAD_FILE, PROMPT.status.uploading,
                        function() {
                            return datas != null;
                        },
                        function() {
					      if (datas.success) {
                            AlertUtil.alertMsg(PROMPT.status.uploadSuccess);
                          } else {
                    	    SysUtil.processMsg(datas.message);
                          }
                          if ($.isFunction(callback)) {
                                callback(updateFileName);
                           }
                        }
                    );

                    return true;

                }else {
                    return false;
                }

            },
            success: function(data) {
               datas = data;
            },
            error: function(responseText) {
               datas = { success: false, message: responseText };
            }
        });
    }
};

var StatusUtil = {

    formatSingalLevel: function(signalLevel, preCss, content) {
        var ddl = DOC.ddl;
        var signals = [ddl.signalLevel0, ddl.signalLevel1, ddl.signalLevel2, ddl.signalLevel3, ddl.signalLevel4, ddl.signalLevel5];
        var theSignalLevel = parseInt(signalLevel, 10);
        if(isNaN(theSignalLevel) || theSignalLevel > 5 || theSignalLevel < 0){
            theSignalLevel = 0;
        }

        return String.format("<div class=\"singal singal{0}\" title=\"{1}\">{2}</div>", (preCss || "") + theSignalLevel, title, content || "");
    },
    formatWiFiInfo: function(state) {
        var title, css;
        //alet(state);
        if(state == "1") {
            css = "unnormal";
            title = DOC.status.disabled;
        } else if(state == "part") {
            css = "partnormal";
            title = DOC.status.wifiPartEnabled;
        } else {
            css = "normal";
            title = DOC.status.enabled;
        }


        return String.format("<div class=\"{0}\" title=\"{1}\">Wi-Fi</div>", css, title);
    },
    getSysStatus: function() {
        var sysStatus = null;
        var $lan1 = $('#lan1'), $lan2 = $('#lan2'),
            $lan3 = $('#lan3'), $lan4 = $('#lan4'),
            $wan1 = $('#wan1');

        function loop() {
            Page.postJSON({
                json: {
                    cmd: RequestCmd.GET_SYS_STATUS,
                    method: JSONMethod.GET,
                    sessionId: ""
                },
                success: function(datas) {
                    sysStatus = datas.data;
                    console.log(data);
                },
                complete: function() {

                    var wifiStatus = sysStatus.wifi;
                    var modemStatus = sysStatus.modem;
                    $('#wifiInfo').html(StatusUtil.formatWiFiInfo(wifiStatus.status));

                }
            });
        }

        loop();
    }
};

var TempUtil = {
    render: function(templateId, containerId, data){
        var $templateId = $('#' + templateId),
            $containerId = $('#' + containerId);
        $containerId.html(_.template($templateId.html(), data));
    },
    renderData: function(data) {
        data.data = Page.isChinese() ? data.cn : data.en;
        $(data.containerId).html(_.template($(data.templateId).html(), data.data));
    }
};


var CheckUtil = {
    checkForm: function($form, rules, messages){
        var validate = $form.validate({
            ignore: ".ignore",
            rules: rules,
            messages: messages
        });

        return validate.form();
    },
	checkPwd: function(pwd) {
        var asciiReg = /^[\x00-\xff]{8,63}$/;
        var hexReg = /^[0-9A-F]{64}$/;
        return asciiReg.test(pwd) || hexReg.test(pwd);
    },
	checkIp: function(ip, isIpv6) {
		//console.log(ip);
        if(isIpv6 != "IPV6"){
            var reg = /^(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/;
            return reg.test(ip);
        } else {
            return  ( /::/g.test(ip) && ip.match(/::/g).length == 1
                && (/^([\da-f]{1,4}(:|::)){1,6}[\da-f]{1,4}$/i.test(ip)
                || /^([\da-f]{1,4}(:|::)){1,6}[\da-f]{1,4}\/\d{1,3}$/i.test(ip)
                || /^::[\da-f]{1,4}$/i.test(ip)
                || /^::[\da-f]{1,4}\/\d{1,3}$/i.test(ip)))
                || /^([\da-f]{1,4}:){7}[\da-f]{1,4}$/i.test(ip)
                || /^([\da-f]{1,4}:){7}[\da-f]{1,4}\/\d{1,3}$/i.test(ip);
        }

    },
	checkMac: function(mac) {
        var reg = /^([0-9a-f]{2}[\:|\-]){5}[0-9a-f]{2}$/i;
        return reg.test(mac);
    },
	checkPort: function(port) {
        var thePort = parseInt(port, 10);
        if (isNaN(thePort) || thePort < 0 || thePort > 65535) return { isValid : false };
        return { isValid : true, port: thePort};
    },
    checkNetSegment: function(lanip, netMask, ip){
        var lanips = ConvertUtil.ip4ToNum(lanip);
        var netMasks = ConvertUtil.ip4ToNum(netMask);
        var ips = ConvertUtil.ip4ToNum(ip);

        return (lanips & netMasks) == (ips & netMasks);
    }
};

var Page = {
    AUTH:"",
	Level:"",
	menuItem: null,
    isNULLToSpace: false,
    currentId: 0,
    sessionId: "",
    language: DOC.language || "CN",
    allLanguage: "",
    languageList: "110",
    getLanguageList: function(language) {
        var theLanguageList = Page.languageList;
        var length = theLanguageList.length;
        if (length <= 0) return '';

        var requiredDefault = (theLanguageList.charAt(length - 1) == "1");
        var sb = new StringBuilder();
        var langs = all_supported_language_info;
        var defaultLang = language.toLowerCase();
        var allLang = '';
        $.each(langs, function(name, value) {
            allLang += name.toUpperCase() + ',';
            var offset = parseInt(value.bit_offset, 10);
            if (offset < length && theLanguageList.charAt(length - offset - 1) == "1") {
                if (name === defaultLang) {
                    sb.append(String.format('<option value="{0}" selected="selected">{1}</option>', name.toUpperCase(), LANG[name]));
                } else {
                    if (requiredDefault) {
                        sb.append(String.format('<option value="{0}">{1}({2})</option>', name.toUpperCase(), LANG[name], LANG[name + '_' + defaultLang]));
                    } else {
                        sb.append(String.format('<option value="{0}">{1}</option>', name.toUpperCase(), LANG[name]));
                    }
                }
            }
        });
        Page.allLanguage = allLang;

        return sb.toString();
    },
    changeLanguage: function($languageSelect) {
        var languageSelect = $languageSelect.val();

        Page.postJSON({
            json: {
                cmd: RequestCmd.CHANGE_LANGUAGE,
                method: JSONMethod.POST,
                sessionId: "",
                languageOld: Page.language,
                languageSelect: languageSelect
            },
            success: function(data) {
                var href = location.href;
                if(href.indexOf('#') > -1){
                    href = href.substring(0, href.indexOf('#'));
                }
                if(href.indexOf('?') > -1){
                    href = href.substring(0, href.indexOf('?'));
                }
                location.href = Page.getUrl(href);
            }

        });
    },
    isChinese: function() {
        return Page.language == "CN";
    },
    initPage: function(isLogin) {
        var $header = $('#header');
        document.title = "Phone Router";
        var headerH = 68, footerH = 30;
        var w = Math.max(document.documentElement.clientWidth, 1000);
        var h = Math.max(document.documentElement.clientHeight - headerH - footerH, 450);
        $('#main').height(h);
        $('#left').height(h);
        $('#left_m').height(h - 30);

        var $right = $('#right');
        $right.css({
            "padding-top": "15px"
        });
        var left = $('#left').width() + 1;
        var right = w - left;
        $right.width(right);
        $right.height(h - 15);

        if (isLogin) {
            var left = (((w - 402) / 2) - 400);
            left = Math.max(left, 0);
            $('#login').css({
                "left": left + "px",
                "top": ((h - 302) / 2 - 50) + "px"
            });
            $('#check_box_left .check_info').css({
                "width": (right / 2 - 40) + "px"
            });
            $('#check_box_right .check_info').css({
                "width": (right / 2 - 30) + "px"
            });
            $('#device_check').css({
                "left": "20px",
                "width": (right - 40) + "px",
                "height": (h - 20) + "px"
            });
        } else {
        }

        $header.width(w);
        $('#footer').width(w);

        var $mask = $('#mask');
        if ($mask.is(":visible")) {
            $mask.width(document.documentElement.clientWidth);
            $mask.height(document.documentElement.clientHeight);
        }

        var $box = $('#info_box');
        if ($box.is(":visible")) {
            SysUtil.setBoxStyle($box, w, h);
        }

        $box = $('#progress_box');
        if ($box.is(":visible")) {
            SysUtil.setBoxStyle($box, w, h);
        }
    },
    getRandomParam: function() {
        return "_t=" + Math.floor(Math.random() * 10000000);
    },
    getUrl: function(url) {
        return String.format("{0}?{1}", url, Page.getRandomParam());
    },
    setHelperStyle: function($helper) {
        $helper.height(Page.$iframe.height() - 65);
    },
    getHelperPage: function(url) {
        if (!url) {
            url = Page.menuItem.url;
        }
        url = url.replace(".html", "Helper.html");

        var $helper = $('#helper');
        $helper.load(url);
        Page.setHelperStyle($helper);
    },
    setFormFieldValue2:function (eleId, value) {
        if(typeof(value) == undefined || !eleId){
            return;
        }
        var $id = "#" + eleId;
        $($id).val(value);
        $($id).attr('old',value);
    },

    render: function(containerId, templateId, datas) {
        if (!containerId) containerId = '#child_container';
        if (!templateId) templateId = '#child_template';
        if (!datas) datas = DOC;

        $(containerId).html(_.template($(templateId).html(), datas));
    },
	getPreHostname: function() {
    	var hostname = FormatUtil.formatField(location.hostname);
    	var lastIndex = hostname.lastIndexOf('.');
    	if (lastIndex > 0) {
    		hostname = hostname.substring(0, lastIndex + 1);
    	} else {
    		hostname = "192.168.0.";
    	}
    	return hostname;
    },
    setStripeTable: function(id) {
        var $tab = $(id || 'table.detail');
        $('tr:odd', $tab).addClass("odd");
        $('tr:even', $tab).addClass("even");
        $('tr:first', $tab).removeClass("even");
    },
    createTable: function(title, names, values, count, columnNum, css) {
        var sb = new StringBuilder();
        sb.append(Page.createTableHead(title, css));

        if (count <= 0 || columnNum <= 0) return sb.toString();

        Page.createTableBody(sb, names, values, count, columnNum, css);

        return sb.toString();
    },
    createTableHead: function(title, css) {
        if (!css) {
            css = "detail";
        } else {
            css = "detail " + css;
        }

        return String.format("<div class=\"{0}\">{1}</div>", css, title);
    },
    createTableBody: function(sb, names, values, count, columnNum, css) {
        if (!css) {
            css = "detail";
        } else {
            css = "detail " + css;
        }
        sb.append(String.format("<table class=\"{0}\" cellspacing=\"0\">", css));

        var columnCount = 0;
        var colon = (Page.isChinese() ? "：" : ":");
        for (var i = 0; i < count; i++) {
            if (i % columnNum == 0) {
                sb.append("<tr>");
            }

            // 输出th
            sb.append("<th>");
            if (!names[i]) {
                sb.append("&nbsp;</th>");
            } else {
                sb.append(names[i]);
                sb.append(colon);
                sb.append("</th>");
            }

            // 输出td
            var colspan = false;
            if (columnNum == 2 && (i + 1) < count && !names[i + 1]) {
                // colspan
                colspan = true;
                sb.append('<td colspan="3">');
            } else {
                sb.append("<td>");
            }
            if (values[i].length == 0) {
                sb.append("&nbsp;</td>");
            } else {
                sb.append(values[i]);
                sb.append("</td>");
            }

            columnCount++;
            if (colspan || columnCount == columnNum) {
                sb.append("</tr>");
                columnCount = 0;
            }
            if (colspan) {
                i++;
            }
        }
        if (columnCount != 0) {
            // 补充输出th,td
            for (var i = columnCount; i < columnNum; i++) {
                sb.append("<th>&nbsp;</th><td>&nbsp;</td>");
            }
            sb.append("</tr>");
        }
        sb.append("</table>");
    },
    postJSON: function(options) {
        var settings = $.extend({}, Page.defaultSetting, options);
        var json = settings.json;
        if (!json.method) {
            json.method = JSONMethod.GET;
        }

        if(!json.language){
            json.language = Page.language;
        }
		
		if(!json.sessionId){
            json.sessionId = Page.sessionId;
        }
		
		var asyns;
        if(isIE()){
            asyns = false;
        }else {
            asyns = true;
        }

        $.ajax({
            async:asyns,
            url: settings.url,
            type: 'POST',
            timeout: settings.timeout,
            contentType: 'application/json; charset=UTF-8',
            data: JSON.stringify(json),
            dataType: 'text',
            beforeSend: function (xhr) {
                //xhr.setRequestHeader("Authorization", 'Digest username="admin", realm="Highwmg", nonce="1000", uri="/cgi/xml_action.cgi", response="c4a96d6622ef9a7ec485afcdb82c4459", qop=auth, nc=00000015, cnonce="e7dace847a70f733"');
            },
            success: function(datas) {
                try {
                    var data = SysUtil.parseJSON(datas);
                    if (data.cmd == undefined) return;

                    if (data.success) {
                        settings.success(data);
                    } else {
                        if ($.isFunction(settings.fail)) {
                            settings.fail(data);
                        } else {
                            if (data.message)
                                AlertUtil.alertMsg(data.message);
                        }
                    }
                } catch(ex) {
                    //AlertUtil.alertMsg("[EXCEPTION]" + ex.message);
                }
            },
            error: function(xhr, textStatus, error) {
                if ($.isFunction(settings.error)) {
                    settings.error(xhr, textStatus, error);
                }
            },
            complete: function() {
                if($.isFunction(settings.complete))
                {
                    settings.complete();
                }
            }
        });


    },
    defaultSetting: {
        url: Url.DEFAULT_CGI,
        timeout: 0,
        returnHtml: false,
        success: function(){},
		fail: function() {
		  $('#logout').click();
          location.href = Page.getUrl(Url.LOGIN);
        },
        complete: function(){}
    }

};



function isIE() {
    if (!!window.ActiveXObject || "ActiveXObject" in window)
        return true;
    else
        return false;
}

function getOpenInfo() {
    var loading = '-';
    var theRouterInfo = {};
    Page.isNULLToSpace = true;

    function createTable(routerInfo) {
        var supportedLte = false;
        var wan_info_id = supportedLte ? '#wan_info' : '#wan_info2';
        var wlan_info_id = supportedLte ? '#wlan_info' : '#wlan_info2';

        var names = [], values = [];
        var names_advanced = [], values_advanced = [];
        var lteNames = DOC.lte,
            netNames = DOC.net,
            wlanNames = DOC.wlan;

        var html;
        if (supportedLte) {
            var status = routerInfo.status;
            var divice = routerInfo.divice;
            var system = routerInfo.system;
            var wifidate = routerInfo.wifi;
            var network = routerInfo.network;

            //names.push(DOC.device.imsi);
            names.push(DOC.lte.plmn);
            names.push(lteNames.phyCellId);
            names.push(lteNames.sinr);
            names.push(lteNames.rsrp);
            names.push(lteNames.rssi);


            //values.push(status.imsi || loading);
            values.push(status.plmn || loading);
            values.push(status.pci || loading);

            values.push(status.sinr ? FormatUtil.formatField(status.sinr, 'dB') : loading);
            values.push(status.rsrp ? FormatUtil.formatField(status.rsrp, 'dBm') : loading);
            values.push(status.rssi ? FormatUtil.formatField(status.rssi, 'dBm') : loading);
            //values.push(lteInfo.txpower ? FormatUtil.formatField(lteInfo.txpower, 'dBm') : loading);

            //names_advanced.push(lteNames.tm);
            names_advanced.push(lteNames.freqPoint);
            names_advanced.push(lteNames.rsrq);
            names_advanced.push(lteNames.globalCellId);
            names_advanced.push(lteNames.enodeB);
           // names_advanced.push(lteNames.volteRegisterStatus);
            names_advanced.push(TAB.advance.roaming);
            names_advanced.push(DOC.device.imei);

            //values_advanced.push(lteInfo.tm ? FormatUtil.formatField('tm ' + lteInfo.tm) : loading);
            values_advanced.push(status.earfcn ||loading);
            values_advanced.push(status.rsrq ? FormatUtil.formatField(status.rsrq, 'dB') : loading);
            values_advanced.push(status.cell_id ||loading);
            values_advanced.push(status.enodebid || loading);
         //   values_advanced.push(loading);
            var roam = loading;
            if(status.roam_status){
                if(status.roam_status == 0){
                    roam = DOC.status.noroam;
                }else if(status.roam_status == 1){
                    roam = DOC.status.roam;
                }
            }
            values_advanced.push(roam);
            values_advanced.push(divice.imei || loading);

            html = Page.createTable(DOC.title.lteInfoBasic, names, values, names.length, 1);
            $('#device_check').show();

            html = Page.createTable(DOC.title.lteInfoAdvanced, names_advanced, values_advanced, names_advanced.length, 1);

        }

        names = [];
        names.push(DOC.lbl.runtime);
        names.push(DOC.device.firmwareVersion);
        names.push(DOC.device.modemVersion);

        values = [];
        var runtime = ConvertUtil.timeStamp(system.runtime || 0);
        values.push(runtime);
        //values.push(FormatUtil.formatField(Page.getDeviceVersion(routerInfo.name, routerInfo.version) || loading));certificationVer
        values.push(divice.hardware|| loading);
        values.push(divice.type || loading);

        html = Page.createTable(DOC.title.router, names, values, names.length, 1);
        $('#router_info').html(html);
        Page.setStripeTable('#router_info');

        names = [];
        names.push(netNames.ip);
        names.push(netNames.mask);
        names.push(netNames.dns1);
        names.push(netNames.dns2);
        //names.push(netNames.gateway);
        //names.push(netNames.ipv6);
        //names.push(netNames.gatewayv6);
        //names.push(netNames.dns3);
        //names.push(netNames.dns4);

        values = [];
        values.push(network.ipaddr || loading);
        values.push(network.netmask || loading);
        values.push(network.first_dns || loading);
        values.push(network.second_dns || loading);
        //values.push(FormatUtil.formatField(routerInfo.wanGateway || loading));
        //values.push(FormatUtil.formatField(network.wanIpv6 || loading));
        //values.push(FormatUtil.formatField(routerInfo.wanGatewayIpv6 || loading));
        //values.push(FormatUtil.formatField(routerInfo.wanDNS_3 || loading));
        //values.push(FormatUtil.formatField(routerInfo.wanDNS_4 || loading));

        html = Page.createTable(DOC.title.wan, names, values, names.length, 1);
        $(wan_info_id).html(html);
        Page.setStripeTable(wan_info_id);

        // names = [];
        // names.push(netNames.ip);
        // names.push(netNames.mask);
        // names.push(netNames.ipBegin);
        // names.push(netNames.ipEnd);
        //
        // values = [];
        // values.push(FormatUtil.formatField(routerInfo.lanIp || loading));
        // values.push(FormatUtil.formatField(routerInfo.lanMaskIp || loading));
        // values.push(FormatUtil.formatField(routerInfo.ipBegin || loading));
        // values.push(FormatUtil.formatField(routerInfo.ipEnd || loading));

        //html = Page.createTable(DOC.title.lan, names, values, names.length, 1);
        //$(lan_info_id).html(html);
        //Page.setStripeTable(lan_info_id);

        if (1) {
            names = [];
            names.push(wlanNames.ssid);
            names.push(wlanNames.channel);
            names.push(wlanNames.wifi);
            // names.push(DOC.lbl.connectedDevicesViaWiFi);
            //names.push(wlanNames.secMode);

            values = [];
            values.push(wifidate.ssid || loading);
            values.push(wifidate.channel || loading);
            //values.push(FormatUtil.formatField(ConvertUtil.frequencyToChannel(routerInfo.frequency) || loading));

            var wifiOpen = wifidate.status;
            if(wifiOpen == "0") {
                values.push(DOC.status.opened);
            } else if(wifiOpen == "1") {
                values.push(DOC.status.closed);
            } else {
                values.push(loading);
            }
            //values.push(String.format('<span id="spanWiFiCount">{0}</span>', Page.wifiUserCount == null ? '' : Page.wifiUserCount.toString()));
            //values.push(FormatUtil.formatField(routerInfo.sta_count || loading));
            /*var securityType = SecurityJSON.getSecurityType(routerInfo);
             if (routerInfo.secMode) {
             values.push(securityType);
             } else {
             values.push(loading);
             }*/

            html = Page.createTable(DOC.title.wlan, names, values, names.length, 1);
            $(wlan_info_id).html(html);
            Page.setStripeTable(wlan_info_id);
        }
    }

    function getRouterInfo() {
        Page.postJSON({
            json: { cmd: RequestCmd.ROUTER_INFO },
            success: function(datas) {
                console.log(datas);
                theRouterInfo = datas.data;

                //	getLteInfo(routerInfo);
                createTable(theRouterInfo);
            },
            fail: function() {
                createTable(theRouterInfo);
            },
            error: function() {
                createTable(theRouterInfo);
            }
        });
    }

    getRouterInfo();

}

var MasterPage = {
    defaultSettings: {
        containerId: '#child_container',
        templateId: '#child_template',
        requiredTemplate: true,
        lang: {
            rowNo: DOC.table.rowNo,
            enabled: DOC.status.enabled,
            add: DOC.table.add,
            edit: DOC.table.edit,
            del: DOC.table.del,
            noEmpty: CHECK.required.noEmpty,
            inputNumber: CHECK.format.inputNumber,
            colon: DOC.colon,
            loading: String.format('<span class="cmt">{0}</span>', DOC.loading),
            btnSetting: DOC.btn.set,
            btnSave: DOC.btn.save,
            btnClose: DOC.btn.close,
            success: PROMPT.saving.success,
            requestTimeout: PROMPT.status.requestTimeout
        },
        init: function() {}
    },
    init: function(options) {
        var opts = $.extend({}, this.defaultSettings, options);
        var lang = $.extend({}, this.defaultSettings.lang, Page.isChinese() ? options.cn : options.en);
        if (opts.global) {
            lang = $.extend({}, opts.global, lang);
        }
      
        if (opts.requiredTemplate) {
            var html = _.template($(opts.templateId).html(), lang);
            options.$id = $(opts.containerId).html(html);
        } else {
            options.$id = $(opts.containerId);
        }

        opts.init();

        return lang;
    },
    initEnd: function($id) {
    }
};

function MenuHead(css, text, bodys) {
    this.css = css;
    this.text = text;
    this.bodys = [];
}

MenuHead.prototype.push = function(body) {
    this.bodys.push(body);
};

function MenuBody(id, text) {
    this.id = id;
    this.text = text;
}

var MenuUtil = {
    create: function(menus) {
        var sb = new StringBuilder();
        var menu, body, bodys;
        for (var i = 0, length = menus.length; i < length; i++) {
            menu = menus[i];
            sb.append(String.format('<h3 class="{0}">{1}</h3>', menu.css, menu.text));
            sb.append(String.format('<ul class="{0}">', menu.css));

            bodys = menu.bodys;
            for (var j = 0, max = bodys.length; j < max; j++) {
                body = bodys[j];
                sb.append(String.format('<li><a id="{0}" href="javascript:;">{1}</a></li>', body.id, body.text));
            }
            sb.append('</ul>');
        }
        return sb.toString();
    }
};