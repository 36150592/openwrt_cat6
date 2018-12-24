var AlertUtil = {
	alertMsg: function(msg) {
		alert(msg);
	},
	confirm: function(msg) {
		return confirm(msg);
	}
};
var ProgressTime = {
	REBOOT: 120,
	FIND_AP: 15,
	UPDATE_UBOOT: 10,
	UPDATE_PARTIAL: 210,
	UPLOAD_FILE: 60,
	SEARCH_PLMN: 120,
	UPGRADE_CHECK: 60,
	REMOTE_UPGRADE: 240
};

var RequestCmd = {
	SYS_INFO: 0,
	WIRELESS_INFO: 1,

	WIRELESS_CONFIG: 2,
	NETWORK_CONFIG: 3,
	USER_INFO: 4,
	SYS_UPDATE: 5,
	SYS_REBOOT: 6,
	PING_TEST: 7,
	WIRELESS_CONFIG_ADVANCED: 8,

	BASIC_CONFIG: 9,
	FIREWALL: 10,

	SET_DATETIME: 11,
	SMS_INFO: 12,
	SMS_SEND: 13,
	SMS_DELETE: 14,
	SMS_DELETE_ALL: 15,

	SYS_HELPER: 16,
	SYS_LOG: 17,
	FLOW_INFO: 18,
	MODULE_LOG: 19,

	APPLY_FILTER: 20,
	PORT_FILTER: 21,
	IP_FILTER: 22,
	MAC_FILTER: 23,
	IP_MAC_BINDING: 24,
	SPEED_LIMIT: 25,
	URL_FILTER: 26,
	OTHER_FILTER: 27,
	DEFAULT_FILTER: 28,
	URL_DEFAULT_FILTER: 29,
	MAC_DEFAULT_FILTER: 30,

	CLEAN_ALL_FILTER: 39,

	DM_CONFIG: 40,
	MTU_CONFIG: 41,
	WATCHDOG_CONFIG: 42,

	DEVICE_VERSION_INFO: 43,

	LTE_LOG_CONFIG: 44,

	LTE_AT: 45,

	EXE_CMD: 46,
	SYSLOG_FUNCTION_CONFIG: 47,

	NET_CONNECT: 48,
	NET_DISCONNECT: 49,
	GET_HTML: 50,
	MODEM_CMD: 51,
	GET_DHTML: 52,
	FAST_SET_WIFI: 53,
	
	INIT_PAGE: 80,
	SET_ANTENNA: 81,
	GET_LTE_STATUS: 82,

	CHANGE_LANGUAGE: 97,
	CHANGE_USERNAME: 98,
	REBOOT: 99,
	LOGIN: 100,
	LOGOUT: 101,
	CHANGE_PASSWD: 102,
	FIND_AP: 103,
	GET_DEVICE_NAME: 104,
	UPDATE_UBOOT: 105,
	UPDATE_PARTIAL: 106,

	BRIDGED_INFO: 107,
	NAT_INFO: 108,
	GET_SPEED_MODE: 109,

	GET_MTU: 110,
	GET_CFG_FILENAMES: 111,
	RESTORE_DEFAULT: 112,

	GET_SYS_STATUS: 113,
	GET_PLMN_NUMBERS: 114,
	STATIC_LEASE: 115,
	SEARCH_PLMN: 116,
	MAC_CONTROL_INFO: 117,
	IPV6_CONFIG: 118,
	DIAL_CONFIG: 119,

	FIND_CLIENT_LIST_WLAN: 120,
	FIND_CLIENT_LIST_DUM: 121,
	FIND_CLIENT_LIST_CLIENT: 122,

	GET_SMS_STATUS_REPORT: 125,
	SET_SMS_STATUS_REPORT: 126,
	
	APN_LIST: 130,
	IMSI_PREFIX_LIST: 131,
	WPS_CONFIG: 132,
	ROUTER_INFO: 133,
	QUERY_SIM_STATUS: 134,

	GET_SUPPORT_BANDS: 157,
	SYS_REBOOT_PAGE: 159,
	LOCK_PHY_CELL: 160,
	LOCK_BAND: 161,
	LOCK_ONE_CELL: 162,

	BACKUP_FIREWALL: 163,
	ROUTER_TABLE: 164,
	ROUTER_MODE: 165,
	TR069_REG: 166,
	PPTP_VPN: 167,
	NETWORK_TOOLS: 168,
	DDNS: 169,
	WRITE_TIME: 170,
	ART_CHECK: 171,
	NETWORK_SERVICE: 172,
	GET_SERVER_INFO: 173,
	LAN_SPEED_LIMIT: 174,
	DNS_CONFIG: 175,
	LNS_LIST: 176,
	GET_ART: 177,
	UPDATE_ART: 178,

	SYS_FILE_INFO: 179,
	CONFIG_EXPORT: 180,
	SIM_LOCKING: 181,

	CLOSE_MANAGE: 182,
	PPPOE_LOG: 183,
	CONFIG_UPDATE: 184,
	GET_PPPOE_LIST: 185,
	LTE_AT_ARRAY: 186,
	LTE_AT_P500: 187,
	FLOW_STATISTICS_SWITCH: 188,
	L2TP_DIAL_STATE_CHECK: 189,
	SYS_FILE_CHECK: 190,
	BACKUP_ART: 191,
	CHECK_BACKUP_ART: 192,
	IS_RUKU_VERSION: 193,
	UPLOAD_MODULE: 194,
	GET_UPLOAD_MODULE_RESULT_CODE: 195,

	ONLINE_UPGRADE_AUTO: 196,
	GET_UPGRADE_RESULT_CODE: 197,
	REMOTE_UPGRADE: 198,

	CONFIG_LOADER: 199,

	WRITE_FLASH: 200,
	TR069_CONFIG: 201,
	WAN_ACCESS_WEB_INFO: 202,
	SET_WAN_ACCESS_WEB_RULE: 203,
	VOLTE_CONFIG: 204,
	TZ_APN_CONFIG: 205,
	DIGITMAP_CONFIG: 206

};

var RENOOTTYPE = {
	NORMAL_REBOOT: 1,
	CONFIG_CHANGE: 2,
	RESTORE_SETTING: 3,
	RESTORE_REBOOT_CANCEL: 4
};

var Url = {
	LOGIN: '/login.html',
	INDEX: '/index.html',
	PASSWD: '/html/changePasswd.html',

	DEFAULT_CGI: '/cgi-bin/lua.cgi'
	//DEFAULT_CGI: 'xml_action.cgi?method=set&module=duster&file=http'
};

var SmsType = {
    RECEIVE: 0,
    SEND: 1,
    DRAFT: 2
};

var DialMode = {
	MANUALLY: "yes",
	AUTO: "no"
};

var ConnectStatus = {
	CONNECT: "connect",
	DISCONNECT: "disconnect"
};

function RootMenu(url, requireTitle, requireIndent, cmd) {
	this.url = url;
	this.isRoot = true;
	this.requireTitle = requireTitle;
	this.requireIndent = requireIndent;
	this.cmd = cmd || RequestCmd.SYS_INFO;
}

var MenuItem = {
	SYS_INFO: { cmd: RequestCmd.SYS_INFO, url: "html/info/sysInfo.html" },
	CLIENT_LIST: { cmd: RequestCmd.FIND_CLIENT_LIST_WLAN, url: "html/info/clientListInfo.html" },
	FLOW_INFO: { cmd: RequestCmd.FLOW_INFO, url: "html/info/flowInfo.html" },
	DEVICE_INFO: { cmd: RequestCmd.SYS_INFO, url: "html/info/deviceInfo.html" },
	USER_INFO: { cmd: RequestCmd.USER_INFO, url: "html/info/userInfo.html" },

	DEMO_CONFIG: { cmd: RequestCmd.BASIC_CONFIG, url: "html/demo/demoIndex.html" },
	QUICK_SETTINGS: { cmd: RequestCmd.BASIC_CONFIG, url: "html/config/quickSettings.html" },
	BASIC_CONFIG: { cmd: RequestCmd.BASIC_CONFIG, url: "html/basic/basicConfig.html" },
	WIRELESS_CONFIG: { cmd: RequestCmd.WIRELESS_CONFIG, url: "html/config/wirelessConfig.html" },
    WLAN_5G_CONFIG: { cmd: RequestCmd.WLAN_5G_CONFIG, url: "html/config/wlan5gConfig.html" },
	PHONE_CONFIG: { cmd: RequestCmd.CHANGE_PASSWD, url: "html/config/phoneConfig.html" },

	NETWORK_CONFIG: { cmd: RequestCmd.NETWORK_CONFIG, url: "html/config/networkConfig.html" },
	IPV6_CONFIG: { cmd: RequestCmd.IPV6_CONFIG, url: "html/config/ipv6Config.html" },
	DIAL_CONFIG: { cmd: RequestCmd.DIAL_CONFIG, url: "html/config/dialConfig.html" },
	LAN_SPEED_LIMIT: { cmd: RequestCmd.LAN_SPEED_LIMIT, url: "html/config/lanConfig.html" },
	NETWORK_SERVICE: { cmd: RequestCmd.NETWORK_SERVICE, url: "html/config/networkServiceConfig.html" },
	DDNS_CONFIG: { cmd: RequestCmd.NETWORK_SERVICE, url: "html/ddns/ddnsIndex.html" },
	PPTP_VPN: { cmd: RequestCmd.PPTP_VPN, url: "html/config/pptpVpn.html" },
	ADVANCED_CONFIG: { cmd: RequestCmd.BASIC_CONFIG, url: "html/advance/advancedConfig.html" },
	ROUTER_TABLE: { cmd: RequestCmd.ROUTER_TABLE, url: "html/route/routeIndex.html" },
	
	LTE_LOG_CONFIG: { cmd: RequestCmd.LTE_LOG_CONFIG, url: "html/config/lteLogConfig.html" },
	LTE_LOCK_FREQUENCY: { cmd: RequestCmd.LTE_AT, url: "html/config/lteLockFrequencyConfig.html" },
	LTE_LOCK_FREQUENCY_P500: { cmd: RequestCmd.LTE_AT, url: "html/config/lteLockFrequencyConfigP500.html" },
	LTE_LOCK_FREQUENCY_SIM5360: { cmd: RequestCmd.LTE_AT, url: "html/config/lteLockFrequencyConfigSim5360.html" },
	LTE_LOCK_FREQUENCY_ZTE: { cmd: RequestCmd.LTE_AT, url: "html/config/lteLockFrequencyConfigZTE.html" },
	LTE_AT: { cmd: RequestCmd.LTE_AT, url: "html/config/lteATConfig.html" },
	SYSTEM_MANAGE: { cmd: RequestCmd.EXE_CMD, url: "html/manage/systemManage.html" },

	FW_RULE: { cmd: RequestCmd.PORT_FILTER, url: "html/firewall/firewall.html" },
	FW_MAC_FILTER: { cmd: RequestCmd.MAC_FILTER, url: "html/firewall/firewall.html" },
	FW_IP_MAC_BINDING: { cmd: RequestCmd.IP_MAC_BINDING, url: "html/firewall/firewall.html" },
	FW_URL_FILTER: { cmd: RequestCmd.URL_FILTER, url: "html/firewall/firewall.html" },
	FW_PORT_MAPPING: { cmd: RequestCmd.OTHER_FILTER, url: "html/firewall/firewall.html" },
	FW_SPEED_LIMIT: { cmd: RequestCmd.SPEED_LIMIT, url: "html/firewall/firewall.html" },
	FW_DMZ: { cmd: RequestCmd.NETWORK_SERVICE, url: "html/firewall/dmz.html" },
	FW_ALG: { cmd: RequestCmd.NETWORK_SERVICE, url: "html/firewall/alg.html" },
	FW_BACKUP_FIREWALL: { cmd: RequestCmd.BACKUP_FIREWALL, url: "html/firewall/backup.html" },

	SMS_NEW: { cmd: RequestCmd.SMS_INFO, smsType: SmsType.SEND, title: MENU.sms.add, url: "html/sms/smsNew.html"},
	SMS_RECEIVE: { cmd: RequestCmd.SMS_INFO, smsType: SmsType.RECEIVE, title: MENU.sms.inbox, url: "html/sms/smsInfo.html" },
	SMS_SEND: { cmd: RequestCmd.SMS_INFO, smsType: SmsType.SEND, title: MENU.sms.outbox, url: "html/sms/smsInfo.html" },
	SMS_DRAFT: { cmd: RequestCmd.SMS_INFO, smsType: SmsType.DRAFT, title: MENU.sms.drafts, url: "html/sms/smsInfo.html" },

	SYS_SET: { cmd: RequestCmd.CHANGE_PASSWD, url: "html/sys/sysConfigIndex.html" },
	SYS_CHECK: { cmd: RequestCmd.SYS_INFO, url: "html/check/checkIndex.html" },
	SYS_HELPER: { cmd: RequestCmd.SYS_HELPER, url: "html/manage/sysHelper.html" },
	SYS_LOG: { cmd: RequestCmd.SYS_LOG, url: "html/manage/sysLog.html" },
	
	SYS_UPDATE:	{ cmd: RequestCmd.SYS_UPDATE, url: "html/update/sysUpdate.html" },
	CONFIG_UPDATE:	{ cmd: RequestCmd.CONFIG_UPDATE, url: "html/update/configUpdate.html" },
	
	CHECKING_STATUS: { cmd: RequestCmd.LTE_AT, url: "html/manage/checkingStatus.html" },
	SYS_ART: { cmd: RequestCmd.BACKUP_ART, url: "html/manage/sysArt.html" },
	SYS_FILE_CHECK: { cmd: RequestCmd.SYS_FILE_CHECK, url: "html/manage/sysFileCheck.html" },
	SIM_LOCKING: { cmd: RequestCmd.SIM_LOCKING, url: "html/isp/ispConfig.html" },
	MODULE_UPDATE:	{ cmd: RequestCmd.UPLOAD_MODULE, url: "html/manage/moduleUpdate.html" },
	MODULE_UPDATE_ML7810:	{ cmd: RequestCmd.UPLOAD_MODULE, url: "html/manage/moduleUpdateML7810.html" },
	SYS_FILE_INFO: { cmd: RequestCmd.SYS_FILE_INFO, url: "html/manage/sysFileInfo.html" },
	NETWORK_TOOLS: { cmd: RequestCmd.NETWORK_TOOLS, url: "html/tools/toolsIndex.html" },
	SYS_REBOOT: { cmd: RequestCmd.SYS_REBOOT, url: "" },
	SYS_REBOOT_PAGE: { cmd: RequestCmd.SYS_REBOOT_PAGE, url: "html/manage/reboot.html" },
	ROOT_LOCK_FREQ: new RootMenu("", true, false),
	ROOT_LOCK_CELL: new RootMenu("html/isp/lockCell.html", true, true),
	ROOT_LOCK_CARD: new RootMenu("html/isp/lockCard.html", true, true),
	ROOT_UNLOCK_CARD: new RootMenu("html/isp/unlockCard.html", true, true),
	ROOT_UNLOCK_IMSI: new RootMenu("html/isp/unlockImsi.html", true, true),

	FAIL_MESSSAGES:  { cmd: RequestCmd.SYS_INFO, url: "html/info/failInfo.html" },
	CLOSED_MANAGEMENT:  { cmd: RequestCmd.SYS_INFO, url: "html/manage/closeManage.html" }
};

var FrequencyMode = {
	ATX20: "7,000000E000000000",
	ATX26: "7,0000002000000000",
	ATX27: "7,0000004000000000",
	ATX28: "7,0000008000000000"
};

var SortDirection = {
	ASC: "asc",
	DESC: "desc"
};

var UpdateType = {
	CLIENT: "CLIENT",
	SERVER: "SERVER"
};

var Network = {
	LAN: "LAN",
	WAN: "WAN"
};
var DHCPServer = {
	OPENED: "1",
	CLOSED: "0"
};
var WiFi = {
	OPENED: "yes",
	CLOSED: "no"
};

var WATCHDOG = {
	OPENED: "no",
	CLOSED: "yes"
};
var NetMode = {
	NAT: "nat",
	BRIDGED: "bridged"
};

var DMNet = {
	CURRENT: "no",
	EXPERIMENT: "yes"
};

var DMRegState = {
	REGSUCCESS: "yes",
	REGFAIL: "no"
};

var DMSwitch = {
	OPENED: "yes",
	CLOSED: "no"
};
var STATE = {
	YES: "yes",
	NO: "no"
};

var EncryptionMode = {
	NONE: "None",
	WPA: "WPA",
	WPA1: "WPA1",
	WPA2: "WPA2",
	WEP: "WEP"
};

var SecurityMode = {
	NONE: "OPEN",
	WPA_PSK_TKIP: "WPA-PSK[TKIP]",
	WPA_PSK_AES: "WPA-PSK[AES]",
	WPA_PSK_TKIP_AES: "WPA-PSK[TKIP AES]",
	WPA2_PSK_TKIP: "WPA2-PSK[TKIP]",
	WPA2_PSK_AES: "WPA2-PSK[AES]",
	WPA2_PSK_TKIP_AES: "WPA2-PSK[TKIP AES]",
	WPA2_MIXED: "WPA2-MIXED",
	WEP: "WEP",
	WPA_EAP_TKIP: "WPA-EAP[TKIP]",
	WPA2_EAP_AES: "WPA2-EAP[AES]",
	WPA2_EAP_MIXED_TKIP: "WPA2-EAP-MIXED[TKIP]",
	WPA2_EAP_MIXED_AES: "WPA2-EAP-MIXED[AES]",
	WEP_EAP: "WEP-EAP"
};

var SecurityJSON = {
	NONE: {
		secMode: "None",
		secFile: "",
		cypher:  "",
		wpa:     "",
		debug:   "0",
		groupRekey: "600",
		gmkRekey: "86400"
	},
	WPA_PSK_TKIP:{
		secMode: "WPA",
		secFile: "PSK",
		cypher:  "TKIP",
		wpa:     "1",
		debug:   "0",
		groupRekey: "600",
		gmkRekey: "86400"
	},
	WPA_PSK_AES: {
		secMode: "WPA",
		secFile: "PSK",
		cypher:  "CCMP",
		wpa:     "1",
		debug:   "0",
		groupRekey: "600",
		gmkRekey: "86400"
	},
	WPA_PSK_TKIP_AES: {
		secMode: "WPA",
		secFile: "PSK",
		cypher:  "TKIP CCMP",
		wpa:     "1",
		debug:   "0",
		groupRekey: "600",
		gmkRekey: "86400"
	},
	WPA2_PSK_TKIP: {
		secMode: "WPA",
		secFile: "PSK",
		cypher:  "TKIP",
		wpa:     "2",
		debug:   "0",
		groupRekey: "600",
		gmkRekey: "86400"
	},
	WPA2_PSK_AES: {
		secMode: "WPA",
		secFile: "PSK",
		cypher:  "CCMP",
		wpa:     "2",
		debug:   "0",
		groupRekey: "600",
		gmkRekey: "86400"
	},
	WPA2_PSK_TKIP_AES: {
		secMode: "WPA",
		secFile: "PSK",
		cypher:  "TKIP CCMP",
		wpa:     "2",
		debug:   "0",
		groupRekey: "600",
		gmkRekey: "86400"
	},
	WPA2_MIXED_TKIP_AES: {
		secMode: "WPA",
		secFile: "PSK",
		cypher:  "TKIP CCMP",
		wpa:     "3",
		debug:   "0",
		groupRekey: "600",
		gmkRekey: "86400"
	},
	WPA2_MIXED_TKIP: {
		secMode: "WPA",
		secFile: "PSK",
		cypher:  "TKIP",
		wpa:     "3",
		debug:   "0",
		groupRekey: "600",
		gmkRekey: "86400"
	},
	WPA2_MIXED_AES: {
		secMode: "WPA",
		secFile: "PSK",
		cypher:  "CCMP",
		wpa:     "3",
		debug:   "0",
		groupRekey: "600",
		gmkRekey: "86400"
	},
	WEP: {
		secMode: "WEP",
		secFile: ""
	},
	WPA_EAP_TKIP:{
		secMode: "WPA",
		secFile: "EAP",
		cypher:  "TKIP",
		wpa:     "1",
		debug:   "0",
		groupRekey: "600",
		gmkRekey: "86400"
	},
	WPA2_EAP_AES:{
		secMode: "WPA",
		secFile: "EAP",
		cypher:  "CCMP",
		wpa:     "2",
		debug:   "0",
		groupRekey: "600",
		gmkRekey: "86400"
	},
	WPA2_EAP_MIXED_TKIP:{
		secMode: "WPA",
		secFile: "EAP",
		cypher:  "TKIP",
		wpa:     "3",
		debug:   "0",
		groupRekey: "600",
		gmkRekey: "86400"
	},
	WPA2_EAP_MIXED_AES:{
		secMode: "WPA",
		secFile: "EAP",
		cypher:  "CCMP",
		wpa:     "3",
		debug:   "0",
		groupRekey: "600",
		gmkRekey: "86400"
	},
	WEP_EAP:{
		secMode: "WPA",
		secFile: "EAP",
		wpa:     "0",
		debug:   "0"
	},
	getSecurityType: function(data) {
        var secMode = data.secMode || EncryptionMode.NONE,
			secFile = FormatUtil.formatField(data.secFile),
			cypher = FormatUtil.formatField(data.cypher),
			wpa = FormatUtil.formatField(data.wpa);
        
		var securityMode = '', securityType = '';
    	switch(cypher) {
        case "TKIP":
        	securityMode = "[TKIP]";
            break;
        case "CCMP":
        	securityMode = "[AES]";
            break;
        case "TKIP CCMP":
        	securityMode = "[TKIP AES]";
            break;
        default:
            break;
        }

    	if (secFile == "EAP") {
        	//802.1x
    		if (wpa == "1") {
	    		securityType = "WPA-EAP" + securityMode;
	    	} else if(wpa == "2") {
	            securityType = "WPA2-EAP" + securityMode;
	    	} else if(wpa == "3") {
	            securityType = "WPA2-EAP-MIXED" + securityMode;
	    	} else {
	            securityType = "WEP-EAP";
	    	}
        } else {
	    	if (secMode == EncryptionMode.NONE) {
	    		securityType = "None";
	    	} else if (secMode == EncryptionMode.WEP) {
	    		securityType = "WEP";
	    	} else if (wpa == "1") {
	    		securityType = "WPA-PSK" + securityMode;
	    	} else if(wpa == "2"){
	            securityType = "WPA2-PSK" + securityMode;
	    	} else {
	    		securityType = "WPA2-MIXED" + securityMode;
	    	}
        }

    	return securityType;
	}
};

var wifiWorkModeJSON={
	m11ng: {
		chMode: "11NGHT20",
		pureg: "0",
		puren: "0",
		rateCtl: "auto",
		manRate: "",
		manRetries: ""
	},
	m11b: {
		chMode: "11B",
		pureg: "0",
		puren: "0",
		rateCtl: "auto",
		manRate: "",
		manRetries: ""
	},
	m11g: {
		chMode: "11G",
		pureg: "1",
		puren: "0",
		rateCtl: "auto",
		manRate: "",
		manRetries: ""
	},
	m11bg: {
		chMode: "11G",
		pureg: "0",
		puren: "0",
		rateCtl: "auto",
		manRate: "",
		manRetries: ""
	},
	m11n: {
		chMode: "11NGHT20",
		pureg: "0",
		puren: "1",
		rateCtl: "auto",
		manRate: "",
		manRetries: ""
	},
	m11nSingleFixed: {
		chMode: "11NGHT20",
		pureg: "0",
		puren: "1",
		rateCtl: "manual",
		manRate: "0x84858687",
		manRetries: "0x04040404"
	},
	m11nSingleFixedMax: {
		chMode: "11NGHT20",
		pureg: "0",
		puren: "1",
		rateCtl: "manual",
		manRate: "0x87878787",
		manRetries: "0x04040404"
	},
	m11nDoubleFixed: {
		chMode: "11NGHT20",
		pureg: "0",
		puren: "1",
		rateCtl: "manual",
		manRate: "0x8c8d8e8f",
		manRetries: "0x04040404"
	},
	m11nDoubleFixedMax: {
		chMode: "11NGHT20",
		pureg: "0",
		puren: "1",
		rateCtl: "manual",
		manRate: "0x8f8f8f8f",
		manRetries: "0x04040404"
	}
};

var WepMode = {
	OPEN_SYSTEM: "1",
	SHARED_KEY: "2"
};

var WepPasswdType = {
	B64: "1",
	B128: "2"
};

var OperMode = {
	AUTO: "auto",
	MANUAL: "manual"
};

var LanMode = {
	DHCP: "auto",
	STATIC_IP: "manual"
};
var WanMode = {
	DHCP: "auto",
	STATIC_IP: "manual"
};
var TxPowerMode = {
	AUTO: "auto",
	BYDB: "bydb",
	MANUAL: "manual"
};

var RateControl = {
	AUTO: "auto",
	MANUAL: "manual"
};

var JSONMethod = {
	GET: "GET",
	POST: "POST"
};

var InputUtil = {
	setRadiosSelectedValue: function($radios, value) {
    	$radios.each(function(){
    		if($(this).val() == value.toString()){
    			$(this).attr("checked", "checked");
    		}
    	});
    },
    createOptions: function(selectedValue, values, texts) {
    	var sb = new StringBuilder();
        var optionFormat = '<option value="{0}">{1}</option>';
        var optionFormat2 = '<option value="{0}" selected="selected">{1}</option>';
        var length = Math.max(values.length, texts.length);
        for (var i = 0; i < length; i++) {
        	if (values[i] == selectedValue)
        		sb.append(String.format(optionFormat2, values[i], texts[i]));
        	else
        		sb.append(String.format(optionFormat, values[i], texts[i]));
        }

        return sb.toString();
    }
};

var ConvertUtil = {
	frequencyToChannel: function(frequency) {
		var frequencys = [ 2412, 2417, 2422, 2427,
	                       2432, 2437, 2442, 2447,
	                       2452, 2457, 2462, 2467, 2472,
	                       5745, 5765, 5785, 5805, 5825];
	    var channels = [ 1,   2,   3,   4,
	                     5,   6,   7,   8,
	                     9,   10,  11,  12, 13,
	                     149, 153, 157, 161, 165];

    	var channel = "-";
        frequency = parseInt(parseFloat(frequency) * 1000, 10);
        if (!isNaN(frequency)) {
        	var length = frequencys.length;
    	    for (var i = 0; i < length; i++) {
    	        if (frequency == frequencys[i]) {
    	        	channel = channels[i].toString();
    	            break;
    	        }
    	    }
        }

        return channel;
	},
	parseNet: function(ipStr, netMaskStr) {
		var ip = this.ip4ToBytes(ipStr);
		var netMask = this.ip4ToBytes(netMaskStr);
		if (ip == null || netMask == null) {
			return null;
		}
		var ipPool = [], ipBegin = [], ipEnd = [];
		for (var i = 0; i < 4; i++) {
			ipBegin[i] = ip[i] & netMask[i];
			ipEnd[i] = (ipBegin[i] + (255 ^ netMask[i])) % 256;
		}
		//ipBegin[3] = ipBegin[3] + 1;
		//ipEnd[3] = ipEnd[3] - 1;
		ipBegin[3] = 100;
		ipEnd[3] = 150;

		ipPool[0] = ipBegin.join(".");
		ipPool[1] = ipEnd.join(".");

		return ipPool;
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
    },
    bytesToIp4: function(ip) {
    	if (ip < 0) ip += 1 << 24;

    	var ip0 = ip % 256;
    	var ip1 = parseInt(ip / 256) % 256;
    	var ip2 = parseInt(ip / (256 * 256)) % 256;
    	var ip3 = parseInt(ip / (256 * 256 * 256)) % 256;

    	return String.format("{0}.{1}.{2}.{3}", ip3, ip2, ip1, ip0);
    },
    parseSingalLevel: function(singalLevel) {
		var singalStd = [-94, -80, -75, -70, -65];
		var singalDesc = ["无信号", "非常差", "差", "一般", "好", "非常好"];
        var singalStdLength = singalStd.length;
        var singalLevelNum = parseInt(singalLevel, 10);
        var singalFlag = 1;
        if (!isNaN(singalLevelNum)) {
        	if (singalLevelNum >= singalStd[singalStdLength - 1]) {
        		singalFlag = singalStdLength;
        	} else {
        		for(var j = 0; j < singalStdLength; j++) {
                	if (singalLevelNum < singalStd[j]) {
                		singalFlag = j;
                		break;
                	}
            	}
        	}
        }

        return { level: singalFlag, desc: singalDesc[singalFlag] };
	},
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
    getNetmaskBitsCount: function(netmask) {
    	//console.log(netmask);
    	var hex = "00000000" + this.ip4ToNum(netmask).toString(16);
    	var bits = this.parseHex(hex.substring(hex.length - 8));
    	//console.log(bits);
    	var count = 0;
    	for (var i = 0, len = bits.length; i < len; i++) {
    		if (bits.charAt(i) == "0") {
    			break;
    		}
    		count++;
    	}
    	return count;
    },
    parseFindAp: function(datas, arrName) {
	var indexFlag = '"' + arrName + '":["';
	var index = datas.indexOf(indexFlag);
	if (index > 0) {
		var theData = datas.substring(index + indexFlag.length);
		index = theData.indexOf('"]');
		if (index > 0) {
			var arr = theData.substring(0, index).split('","');
			for(var i = 0; i < arr.length; i++) {
				arr[i] = arr[i].replaceQuote();
			}
			return arr;
		}
	}

	return [];
    },
    parseUptime: function(uptime, unit) {
        var runTime = "", runStatus = "";
    	var uptimeReg = /^(.*)up(.*)\,\s*load\s*average\:(.*)$/;
        if (uptimeReg.test(uptime)) {
            runTime = RegExp.$2.replace('days', 'day').replace('day', unit.day).replace(':', unit.hour);
            if (runTime.indexOf('min') > 0) {
            	runTime = runTime.replace('min', unit.min);
            } else {
                runTime = runTime + unit.min;
            }
            runStatus = RegExp.$3;
        }
        return { runTime: runTime, runStatus: runStatus };
    },

    timeStamp:function(StatusMinute){
    var day=parseInt(StatusMinute/60/24);
    var hour=parseInt(StatusMinute/60%24);
    var min= parseInt(StatusMinute % 60);
    StatusMinute="";
     if (day > 0)
       {
        StatusMinute= day + "天 ";
       }
     if (hour>0)
       {
        StatusMinute += hour + "小时 ";
       }
      if (min>0)
      {
        StatusMinute += parseFloat(min) + "分钟 ";
      }
      return StatusMinute;
    }
};

var SysUtil = {
	deviceName: null,

	rebootMessage: PROMPT.tips.rebootMessage,
	defaultRebootSettings: {
		rebootType: RENOOTTYPE.CONFIG_CHANGE,
		msg: PROMPT.saving.success,
		callback: null,
		hideConfirm: false
	},
	reboot: function(options) {
		var opts = $.extend({}, SysUtil.defaultRebootSettings, options);
		var msg = opts.msg.trim();
		var lastChar = msg.charAt(msg.length - 1);
		if (".,?!:;。？：！，；".indexOf(lastChar) < 0) {
			msg = msg + ', ';
		}
		SysUtil.rebootDevice(opts.rebootType, msg + SysUtil.rebootMessage, opts.callback, opts.hideConfirm);
	},
	rebootDevice: function(rebootType, msg, callback, hideConfirm) {

		if(!rebootType){
			rebootType = RENOOTTYPE.CONFIG_CHANGE;
		}

		if (!msg) msg = /*PROMPT.saving.success + */this.rebootMessage;

		if (!hideConfirm) {
			if (!AlertUtil.confirm(msg)) {
				if ($.isFunction(callback)) {
					callback(true);
				}
				return;
			}
		}
		var isOk = false;

		var timeoutCtl = setTimeout(function(){
			isOk = true;
		}, 1000 * 90);
		SysUtil.showProgress(ProgressTime.REBOOT, PROMPT.status.rebooting,
			function() {
				return isOk;
			},
			function() {
				clearTimeout(timeoutCtl);
				if ($.isFunction(callback)) {
					callback(false);
				} else{
					location.reload();
				}
			}
		);

		Page.postJSON({
	        json: { cmd: RequestCmd.SYS_INFO },
	        success: function(data) {
	            Page.lanIp = FormatUtil.formatField(data.lanIP);
	        },
	        complete: function() {
	        	if (Page.lanIp == location.host) {
					// 重启前发送AT
					Page.postJSON({
						json: {
							method: JSONMethod.POST,
							cmd: RequestCmd.MODEM_CMD,
							subcmd: 0
						},
						success: function(data) {
							if(data.message.indexOf("OK") >= 0) {
							}
						},
						complete: function(){
						}
					});
	        	}
	        }
	    });

		setTimeout(reboot, 3000);

		function reboot() {
		// 发送重启命令
		Page.postJSON({
			json: {
				cmd: RequestCmd.SYS_REBOOT,
				rebootType: rebootType,
				method: JSONMethod.POST
			},
			success: function() {
				setTimeout(loop, 30000);
			},
			fail: function() {
				isOk = true;
			}
    		});
		}
		function loop() {
			// 重启命令执行成功，循环检测设备是否已重启
			Page.getDeviceName(function() {
				isOk = true;
			}, function() {
				isOk = true;
			}, function() {
				setTimeout(loop, 5000);
			});
		}
		//Page.getPageByMenuItem(MenuItems.sysReboot);
	},
	restoreRebootCancel: function() {
		Page.postJSON({
			json: {
				cmd: RequestCmd.SYS_REBOOT,
				rebootType: RENOOTTYPE.RESTORE_REBOOT_CANCEL,
				method: JSONMethod.POST
			},
			success: function() {
			}
    	});
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
				else if (ratio > 1 && maxCount - count > delayCount) count++;
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
	showProgress_2: function(message, checkStatus, callback) {
		var $mask = $('#mask'), $box = $('#progress_box');
		var $progress = $('#progress_status'), $info = $('#progress_info');
		var h = document.documentElement.clientHeight;
		var w = document.documentElement.clientWidth;
		$mask.height(h);
		$mask.show();
		$box.show();
		SysUtil.setBoxStyle($box, w, h);

		var count = 0;
		var width = $('#progress_bar').width();
		function loop() {
			count = checkStatus();
			$info.text(message + DOC.comma + PROMPT.status.progress+" " + count + "%");
			$progress.width(parseInt((width * count) / 100));
			if (count < 100) {
				setTimeout(loop, 500);
			} else {
				callback();
			}
		}
		loop();
	},
	setBoxStyle: function($box, w, h) {
		$box.css({
			"left": parseInt(((w || document.documentElement.clientWidth) - $box.width()) / 2, 10) + "px",
			"top": parseInt(((h || document.documentElement.clientHeight) - $box.height()) / 2, 10) + "px"
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
	getModule: function() {
		if (Page.imei && Page.modelVersion != "" && Page.modelVersion != "NULL") return true;

		var count = 0;
		function loop() {
			Page.postJSON({
	            json: { cmd: RequestCmd.DEVICE_VERSION_INFO },
	            success: function(data) {
	            	Page.module = data.module;
	            	Page.modelVersion = data.modversion;
	            	Page.imei = data.imei;
	            	if (data.modversion == "" || data.modversion == "NULL" || count++ < 20) {
	                	setTimeout(loop, 10000);
	            	}
	            }
			});
		}
		loop();
		
		return false;
	},
	saveGuideState: function(requiredSave) {
		if (!requiredSave) return;
		
		Page.postJSON({
	        json: {
	        	cmd: RequestCmd.IS_RUKU_VERSION,
	        	method: 'SAVE',
	        	configGuide: 'yes'
	        },
	        success: function(data) {
	        }
	    });
	},
	processMsg: function(message) {
		if (message == "NO_AUTH") {
			AlertUtil.alertMsg(PROMPT.status.noAuth);
            location.href = Page.getUrl(Url.LOGIN);
        } else {
            AlertUtil.alertMsg(message);
        }
	},
	upload: function($form, $file, command, callback) {
		var url = String.format("{0}?cmd={1}&method=POST&sessionId={2}&language={3}", Url.DEFAULT_CGI, RequestCmd.SYS_UPDATE, Page.sessionId, Page.language);
			
		//var url = "xml_action.cgi?Action=Upload&file=upgrade&command=" + command;
		
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

                if(!confirm(PROMPT.confirm.uploadFile + updateFileName)){
                	return false;
                }

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
            },
            success: function(data, statusText) {
            	datas = data;
            },
            error: function(responseText, statusText) {
                datas = { success: false, message: responseText };
            }
        });
	}
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
    formatNetState: function (value) {
        if (!value || value.trim() == "" || value == "NULL") {
            return "-";
        }
        return this.formatField(value);
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
    	return '?';
    }
};

var ItemHideTable = {
	LOCK_CELL: { index: 0, hideFlag: "1", emptyHide: false },
	LOCK_CARD: { index: 1, hideFlag: "1", emptyHide: false },
	WAN_LIMIT_SPEED: { index: 2, hideFlag: "1", emptyHide: false },
	MTU: { index: 3, hideFlag: "1", emptyHide: false },
	MAX_ACCESS_USER_NUM: { index: 4, hideFlag: "0", emptyHide: true },
	LOCK_FREQ: { index: 5, hideFlag: "1", emptyHide: false },
	LOCK_PHY_CELL: { index: 6, hideFlag: "0", emptyHide: true },
	QUICK_SET: { index: 7, hideFlag: "0", emptyHide: true },
	LOCAL_MANAGE: { index: 8, hideFlag: "0", emptyHide: true },
	ISP: { index: 9, hideFlag: "1", emptyHide: false },
	PLMN_MODE: { index: 10, hideFlag: "1", emptyHide: false },
	LOCK_PHY_CELL_REMOTE: { index: 11, hideFlag: "0", emptyHide: true },
	ADVANCED_SETTING: { index: 12, hideFlag: "1", emptyHide: false },
	TR069: { index: 13, hideFlag: "0", emptyHide: true },
	REMOTE_SERVER: { index: 14, hideFlag: "0", emptyHide: true },
	GSM_MODE: { index: 15, hideFlag: "1", emptyHide: false },
	SNMP: { index: 16, hideFlag: "0", emptyHide: true },
	DTU: { index: 17, hideFlag: "0", emptyHide: true },
	SWDTU: { index: 18, hideFlag: "0", emptyHide: true },
	DDNS: { index: 19, hideFlag: "1", emptyHide: false },
	PPTP_VPN: { index: 20, hideFlag: "0", emptyHide: true },
	TR069_REG: { index: 21, hideFlag: "0", emptyHide: true },
	ROUTER_MODE: { index: 22, hideFlag: "0", emptyHide: true },
	UNLOCK_CARD: { index: 23, hideFlag: "1", emptyHide: false },
	UNLOCK_IMSI: { index: 24, hideFlag: "1", emptyHide: false },
	REMOTE_LOGIN: { index: 25, hideFlag: "1", emptyHide: false },
	BRIDGE_MODE: { index: 26, hideFlag: "1", emptyHide: false },
	LANGUAGE: { index: 27, hideFlag: "1", emptyHide: false },
	MCC_MNC: { index: 28, hideFlag: "1", emptyHide: false },
	WIFI: { index: 29, hideFlag: "1", emptyHide: false },
	CONFIG_UPDATE: { index: 30, hideFlag: "1", emptyHide: false },
	SYSTEM_UPDATE: { index: 31, hideFlag: "1", emptyHide: false },
	ROUTE_SETTINGS: { index: 32, hideFlag: "1", emptyHide: false },
	NETWORK_TOOLS: { index: 33, hideFlag: "1", emptyHide: false },
	SYSTEM_CHECK: { index: 34, hideFlag: "1", emptyHide: false },
	TIME_SETTINGS: { index: 35, hideFlag: "1", emptyHide: false },
	MODULE_UPDATE: { index: 36, hideFlag: "1", emptyHide: false },
	APN: { index: 37, hideFlag: "1", emptyHide: false }
	,TIMER_REBOOT: { index: 39, hideFlag: "0", emptyHide: true }
	,SMS: { index: 40, hideFlag: "1", emptyHide: false }
	,ROAMING: { index: 41, hideFlag: "1", emptyHide: false }
	,VOIP: { index: 42, hideFlag: "1", emptyHide: false }
	,LOCK_IMSI_PREFIX: { index: 43, hideFlag: "1", emptyHide: false }
	,ANTENNA: { index: 44, hideFlag: "0", emptyHide: true }
	,IP_STACK: { index: 45, hideFlag: "1", emptyHide: false }
	,IPV6: { index: 46, hideFlag: "1", emptyHide: false }
	,FIREWALL: { index: 47, hideFlag: "1", emptyHide: false }
	,WPS: { index: 48, hideFlag: "1", emptyHide: false }
	,NETWORK_SERVICE: { index: 49, hideFlag: "1", emptyHide: false }
	,WIFI_INDOOR: { index: 50, hideFlag: "1", emptyHide: false }
	,WAN_MODE: { index: 51, hideFlag: "1", emptyHide: false }
	,LTE_PHYCELL_INFO: { index: 52, hideFlag: "1", emptyHide: false }
	,LTE_AREA_INFO: { index: 53, hideFlag: "1", emptyHide: false }
	,WIFI_WMM: { index: 54, hideFlag: "1", emptyHide: false }
	,AT_CMD: { index: 55, hideFlag: "0", emptyHide: true }
	,LTE_STATUS_INFO: { index: 56, hideFlag: "1", emptyHide: false }
	,SYS_LOG: { index: 57, hideFlag: "1", emptyHide: false }
	,FOTA: { index: 58, hideFlag: "1", emptyHide: false }
	,DHCP_RESERVED: { index: 59, hideFlag: "1", emptyHide: false }
	,PIN_SETTING: { index: 60, hideFlag: "1", emptyHide: false }
	,NETWORK_SIM_INFO: { index: 63, hideFlag: "1", emptyHide: false }
	,DNS: { index: 64, hideFlag: "1", emptyHide: false }
	,ALG_SERVER: { index: 65, hideFlag: "1", emptyHide: false }
	,DDOS: { index: 66, hideFlag: "0", emptyHide: true }
	,SERVICES: { index: 67, hideFlag: "1", emptyHide: false }
	,ENCRYPT_CARD: { index: 68, hideFlag: "1", emptyHide: false }
	,NETWORK_MODE: { index: 69, hideFlag: "1", emptyHide: false }
	,CHANGE_USERNAME: { index: 70, hideFlag: "1", emptyHide: false }
	,CHANGE_PASSWD: { index: 71, hideFlag: "1", emptyHide: false }
	,OFFICAL_SITE: { index: 72, hideFlag: "1", emptyHide: false }
	,HELP: { index: 73, hideFlag: "1", emptyHide: false }
};

var ItemDisableTable = {
	LOCK_CELL: 0,
	LOCK_CARD: 1,
	WAN_LIMIT_SPEED: 2,
	EXPORT_CONFIG: 3,
	NET_MODE: 4,
	ONLY_4G: 5,
	IMSI: 6,
	LAN_LIMIT_SPEED: 7,
	APN: 8,
	DNS: 9,
	DHCP: 10,
	LOCAL_MANAGE: 11,
	IP_STACK: 12,
	TR069: 13,
	REMOTE_SERVER: 14,
	COMMON_USER_LOGIN: 15,
	MAX_STATION: 16,
	DIAL_MODE: 17,
	LOCK_NETWORK: 18,
	ALWAYS_GUIDE: 19,
	APN_GUIDE: 20,
	APN_NOT_USER_INFO: 21
};

var ItemSupportedTable = {
//Devices Settings->WAN Settings->Dial Number 
DISPLAY_Dial_Number: (0),
//Devices Settings->Universal Frequency Locking
DISPLAY_UNIVERSAL_FREQUENCY_LOCK: (1),
//Devices Settings->Advanced Settings->Roaming Settings
DISPLAY_ROAMING_SETTINGS: (2),
DISPLAY_ML7810_MODULE_UPGRADE: (3)
,DISPLAY_REMOTE_IP_TO_CAPTURE_MODEM_LOG: (4)
,DISPLAY_MORE_WIFI_OPERATIONS_HELP: (10)
,DISPLAY_WIRELESS_ACCESS_CONTROL: (11)
,SUPPORT_TO_REBOOT_AFTER_N_HOURS: (12)
,DISPLAY_LTE_STATUS: (13)
,LOCK_ONE_CELL: (14)
,PROMPT_TO_REBOOT_FOR_LOCK_BAND: (15)
,LOCK_IMSI_PREFIX: (16)
,SUPPORT_TO_REDIAL_AFTER_N_HOURS: (17)
,SUPPORT_VOIP: (18)
,SUPPORT_WAN_LOGIN_BLACKLIST_AND_PORT: (21)
,SUPPORT_LTE_WIFI_COEXIST_SETTINGS: (22)
,SUPPORT_WIFI_20M_40M_CONFIG: (23)
,SUPPORT_BRIDGE_MTU_CONFIG: (24)
,SUPPORT_BRIDGE_IP_STACK_CONFIG: (25)
,SUPPORT_REBOOT_AFTER_CHANGE_LAN_IP: (28)
,WEB_PAGE_SUPPORT_SSH_ACCESS_CONTROL: (29)
,WEB_PAGE_SUPPORT_DISPLAY_DEVICE_CONNECTION_MEDIUM: (31)
}

var ItemUnsupportedTable = {
//System Status->Version Information->LTE VERSION
//LTE Build Date/Time:
DISPLAY_LTE_Build_Date: (0),

//Physical Layer Version:
DISPLAY_Physical_Layer_Version: (1),

//High-level Protocol Version:
DISPLAY_High_level_Protocol_Version: (2),

DISPLAY_ICCID: (10),

//Devices Settings->WAN Settings->Network Mode
//Dialing Mode:
DISPLAY_Dialing_Mode: (3),

//Search Preferred:
DISPLAY_Search_Preferred: (4),

//CS/(E)PS Preferred:
DISPLAY_CS_PS_Preferred: (5),

//Working Mode:
DISPLAY_Working_Mode: (6),

//PLMN Mode:
DISPLAY_PLMN_Mode: (7),

//Management->Module Upgrade
DISPLAY_Module_Upgrade: (8),

//Management->Log Catching
DISPLAY_Log_Catching: (9),

//Device Settings->Advanced Settings->Frequency Lockin
DISPLAY_Frequency_Locking: (11),

//Device Settings->Network Service
DISPLAY_Network_Service: (12),

//Device Settings->PPTP VPN->Mode
DISPLAY_PPTP_VPN_Mode: (13),

//Device Settings->PPTP VPN
DISPLAY_PPTP_VPN: (14),

//System Status->Version Information->Module Information->Module Version
DISPLAY_Module_Version: (15),

DISPLAY_SMS: (16),

DISPLAY_Network_Mode_GSM_Mode: (17),
DISPLAY_Network_Mode_Preferred_Mode: (18),

DISPLAY_WIFI: (19),
DISPLAY_Route_Settings: (20),
DISPLAY_Time_Settings: (21),
DISPLAY_System_Check: (22),
DISPLAY_Network_Tools: (23),
DISPLAY_FIREWALL_DMZ: (24),
DISPLAY_FIREWALL_ALG: (25),
DISPLAY_FIREWALL_Backup: (26),
DISPLAY_FIREWALL_URL: (27),
DISPLAY_USB_SERIAL_PORT: (28),
NOT_SUPPORT_LOCK_MCC_MNC: (29),
NOT_SUPPORT_DDNS: (30),
NOT_SUPPORT_WMM: (31),
NOT_SUPPORT_ISP_SETTINGS: (32),
NOT_SUPPORT_REMOTE_LOGIN: (33),
NOT_SUPPORT_AREA_INFO: (34),
NOT_SUPPORT_STATELESS_DHCPV6: (35),
NOT_SUPPORT_PNAT_464XLAT: (36),
NOT_SUPPORT_L2TP_PLUS_PPPOE: (37),
NOT_SUPPORT_CALIBRATION_STATUS: (38),
NOT_SUPPORT_MTU_SETTINGS: (39),
NOT_SUPPORT_DHCP_RESERVED_IP: (40),
NOT_SUPPORT_INDOOR_DEVICE_LIST: (41),
NOT_SUPPORT_REBOOT_PROMPT_AFTER_RIGHT_PIN_INPUT: (42),
NOT_SUPPORT_WPS_PIN_VERIFICATION: (43),
NOT_SUPPORT_PROMPT_REBOOT_WHEN_RESTORE_DEFAULT_CONFIGS: (44),
NOT_SUPPORT_TELNET_SETTINGS: (45)
,NOT_SUPPORT_PROMPT_TO_REBOOT_AFTER_LOCK_ONE_CELL: (47)
};

var Page = {
	isTest: false,
	isNULLToSpace: false,
	connectStatus: true,
	isRuKuVersion: false,
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
            // complete: function() {
            // 	var href = location.href;
            // 	if(href.indexOf('#') > -1){
            // 		href = href.substring(0, href.indexOf('#'));
            // 	}
            // 	if(href.indexOf('?') > -1){
            // 		href = href.substring(0, href.indexOf('?'));
            // 	}
            // 	location.href = Page.getUrl(href);
            // }
        });
    },
    requireChangeLang: function(language) {
    	if (!Page.allLanguage) {
    		Page.getLanguageList('CN');
    	}
    	return language.length > 0 && Page.allLanguage.indexOf(language) >=0 && language != Page.language;
    },
    isChinese: function() {
    	return Page.language == "CN";
    },
    alertTimes: 0,
    defaultAlertTimes: 100,
    $iframe: null,
    menuItem: null,
    module: "",
    itemHide: "0000",
    itemDisable: "0000",
    setItemHide: function(itemHide) {
    	if (!itemHide || itemHide.length < 4) {
        	itemHide = "0000";
        }
    	var index = itemHide.indexOf("1");
    	if (index > 0) {
    		// delete pre zero
    		itemHide = itemHide.substring(index);
    	} else if (index < 0) {
    		itemHide = "";
    	}
    	// expand 5 bits
    	if (itemHide.length < 5) {
    		itemHide = ("00000" + itemHide);
    		itemHide = itemHide.substring(itemHide.length - 5);
    	}
    	Page.itemHide = itemHide;
    },
    setItemDisable: function(itemDisable) {
    	if (!itemDisable || itemDisable.length < 4) {
    		itemDisable = "0000";
        }
    	var index = itemDisable.indexOf("1");
    	if (index > 0) {
    		// delete pre zero
    		itemDisable = itemDisable.substring(index);
    	} else if (index < 0) {
    		itemDisable = "";
    	}
    	// expand 7 bits
    	if (itemDisable.length < 7) {
    		itemDisable = ("0000000" + itemDisable);
    		itemDisable = itemDisable.substring(itemDisable.length - 7);
    	}
    	Page.itemDisable = itemDisable;
    },
    isItemHide: function(itemHideTable) {
    	var itemHide = Page.itemHide;
    	var lastIndex = itemHideTable.index;
    	if (lastIndex >= itemHide.length) return itemHideTable.emptyHide;

    	if (itemHide.charAt(itemHide.length - 1 - lastIndex) == itemHideTable.hideFlag)
    		return true;
    	else
    		return false;
    },
    isItemDisable: function(lastIndex) {
    	var itemDisable = Page.itemDisable;
    	if (lastIndex >= itemDisable.length) return false;

    	if (itemDisable.charAt(itemDisable.length - 1 - lastIndex) == "1")
    		return true;
    	else
    		return false;
    },
    setSupported: function(supported) {
    	Page.supported = supported || "0000";
    },
    setUnsupported: function(unsupported) {
    	Page.unsupported = unsupported || "0000";
    },
    isSupported: function(lastIndex, supportFlag) {
    	if (supportFlag) {
    		var supported = Page.supported || "0000";
    		if (lastIndex >= supported.length) return false;
    		return supported.charAt(supported.length - 1 - lastIndex) == "1" ? true : false;
    	} else {
    		var unsupported = Page.unsupported || "0000";
    		if (lastIndex >= unsupported.length) return true;
    		return unsupported.charAt(unsupported.length - 1 - lastIndex) == "0" ? true : false;
    	}
    },
    
    setMenu: function(id, itemHideTable) {
    	if (Page.isItemHide(itemHideTable)) {
    		$(id).detach();
    	} else {
    		$(id).show();
    	}
    },
    setSupportMenu: function(id, itemSupportTable, supportFlag) {
    	if (supportFlag) {
        	if (Page.isSupported(itemSupportTable, true)) {
        		$(id).show();
        	} else {
        		$(id).detach();
        	}
    	} else {
        	if (Page.isSupported(itemSupportTable, false)) {
        		$(id).show();
        	} else {
        		$(id).detach();
        	}
    	}
    },
    setHideAndSupportMenu: function(id, itemHideTable, itemSupportTable, supportFlag) {
    	if (Page.isSupported(itemSupportTable, supportFlag)) {
        	Page.setMenu(id, itemHideTable);
        } else {
            Page.setSupportMenu(id, itemSupportTable, supportFlag);
        }
    },
    isModulP500: function() {
    	return Page.module == "P500";
    },
    isModulL1761: function() {
    	return Page.module == "L1761";
    },
    isModulZTE7510: function() {
    	return Page.module == "ZTE7510";
    },
    isModulMC2716: function() {
    	return Page.module == "MC2716";
    },
    isModulSIM5360: function() {
    	return Page.module == "SIM5360";
    },
    isModuleHOJY9X07: function() {
    	return Page.module == "HOJY9X07";
    },
    isModuleZTE: function() {
    	return Page.module.startsWith("ML7810");
    },
    isModuleZTEB78: function() {
    	return Page.module == "ML7810_B78";
    },
    isModuleZTEB53: function() {
    	return Page.module == "ML7810_B53";
    },
    isModuleC5300V: function() {
    	return Page.module == "C5300V";
    },
    isModuleU8300: function() {
    	return Page.module == "U8300";
    },
    isModuleSLM630B: function() {
    	return Page.module == "SLM630B";
    },
    isModuleBLS001: function() {
    	return Page.module == "BLS001";
    },
    isModuleLM06: function() {
    	return Page.module == "LM06";
    },

	initPage: function(isLogin) {
		var $header = $('#header');
		// if (!Page.hasGetLogo) {
		// 	Page.postJSON({
		// 		json: { cmd: RequestCmd.INIT_PAGE },
		// 		success: function(data) {
		// 			Page.hasGetLogo = true;
		// 			Page.enableChannelOneToFour = (data.channelOneToFour == "yes");
		// 			Page.disableChannelEnds = (data.hideChannel12And13 == "yes");
		// 			var logoPath = (data.logoPath || "").trim();
		// 			if (logoPath != "NULL" && logoPath.length > 0) {
		// 				var background = String.format("url(/images/{0})", logoPath);
		// 				$header.css({
		// 					"background-image": background,
		// 					"background-repeat": "no-repeat"
		// 				});
		// 			} else {
		// 				if (data.isLogoExists == "1") {
		// 					$header.addClass("header_logo");
		// 				} else {
		// 					$header.addClass("header_default");
		// 				}
		// 			}
		// 			if (data.deviceType == 'NULL') {
		// 				data.deviceType = '';
		// 			}
		// 			Page.displayedVersion = data.displayedVersion;
		// 			if (Page.displayedVersion == 'NULL') {
		// 				Page.displayedVersion = '';
		// 			}
		// 			//Page.setTitle(data.displayedTitle, data.deviceType);
		// 			document.title = "Phone Router";
		// 		}
		// 	});
		// 	//if (!isLogin) {
		// 	//	Page.killSearchPlmn();
		// 	//}
		// }
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
	setStripeTable: function(id) {
		var $tab = $(id || 'table.detail');
		$('tr:odd', $tab).addClass("odd");
		$('tr:even', $tab).addClass("even");
		$('tr:first', $tab).removeClass("even");
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
		// Page.getHtml(url, Page.menuItem.cmd, function(data) {
        	// $helper.html(data);
        	// Page.setHelperStyle($helper);
		// });
        $helper.load(url);
        Page.setHelperStyle($helper);
	},
	setHelperInfo: function(_this, name) {
		var $desc = $('#helper_' + (name || _this.name));
        if ($desc.length > 0) {
        	$desc.show().siblings().hide();
        }
	},
	get: function(url, title) {
		if (Page.disableLoad) return;

		Page.disableLoad = true;
		Page.destory();
		
    	var menu = Page.menuItem;
		Page.postJSON({
			returnHtml: true,
			json: {
	        	cmd: RequestCmd.GET_HTML,
	            url: url,
	            subcmd: menu.cmd
	        },
	        success: function(data) {
	        	if (data.indexOf('"message":"NO_AUTH"') > 0) {
					location.href = Page.getUrl(Url.LOGIN);
	        	} else {
		        	Page.$iframe.html(data);
		        	
					if (menu.isRoot) {
						if (menu.requireTitle) {
							$('#sitepath').text(title).show();
						}
						if (menu.requireIndent) {
							$('#child_container').addClass('detail_box');
						}
					}
	        	}
	        },
	        complete: function() {
	        	setTimeout(function() {
	        		Page.disableLoad = false;
	        	}, 100);
	        }
		});
	},
	getHtml: function(url, subcmd, callback) {
		Page.postJSON({
			returnHtml: true,
			json: {
	        	cmd: RequestCmd.GET_HTML,
	            url: url,
	            subcmd: subcmd || 0
	        },
	        success: function(data) {
	        	if (data.indexOf('"message":"NO_AUTH"') > 0) {
					location.href = Page.getUrl(Url.LOGIN);
	        	} else {
		        	callback(data);
	        	}
	        }
		});
	},
	writeTime: function(subcmd) {
		Page.postJSON({
	        json: {
	            cmd: RequestCmd.WRITE_TIME,
	            method: JSONMethod.POST,
	            subcmd: subcmd,
	            time: new Date().format('yyyy-mm-dd HH:MM')
	        },
	        success: function(data) {
	        }
	    });
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

    	// if (!(json.cmd == RequestCmd.LOGIN || json.cmd == RequestCmd.GET_DEVICE_NAME)) {
        	// json.sessionId = Page.sessionId; //CookieUtil.getCookie("username");
    	// }

        	// var $id = settings.$id;
        	// if ($id) {
        	// 	$id.disable();
        	// }

        	$.ajax({
                async:false,
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
        				// if (json.cmd == RequestCmd.FIND_AP && datas.indexOf("ssids") > 0) {
        				// 	// 单独解析FIND_AP
        				// 	data = {};
        				// 	data.success = true;
        				// 	data.cmd = RequestCmd.FIND_AP;
        				// 	data.macs = ConvertUtil.parseFindAp(datas, "macs");
        				// 	data.ssids = ConvertUtil.parseFindAp(datas, "ssids");
        				// 	data.frequencys = ConvertUtil.parseFindAp(datas, "frequencys");
        				// 	data.singalLevels = ConvertUtil.parseFindAp(datas, "singalLevels");
        				// 	data.encryptionKeys = ConvertUtil.parseFindAp(datas, "encryptionKeys");
        				// 	data.encryptionModes = ConvertUtil.parseFindAp(datas, "encryptionModes");
        				// 	data.groupCiphers = ConvertUtil.parseFindAp(datas, "groupCiphers");
        				// 	data.pairwiseCiphers = ConvertUtil.parseFindAp(datas, "pairwiseCiphers");
        				// 	data.authenticationSuites = ConvertUtil.parseFindAp(datas, "authenticationSuites");
        				// } else {
        				// 	if (!settings.returnHtml) {
                    		// 	data = SysUtil.parseJSON(datas);
        				// 	} else {
                			// 	// 返回HTML不用解析
        				// 		data = datas;
        				// 	}
        				// }

    					//if (!settings.returnHtml) {
	        				if (data.cmd == undefined) return;

	            			if (data.success) {
	            				settings.success(data);
	            			} else {
	            				// if (data.message == "NO_AUTH") {
	            			   	//     location.href = Page.getUrl(Url.LOGIN);
	            				// 	return;
	            				// }
	            				if ($.isFunction(settings.fail)) {
	            					settings.fail(data);
	            				} else {
	            					if (data.message)
	            						AlertUtil.alertMsg(data.message);
	            				}
	            			}
    					//} else {
    					//	settings.success(data);
    				//	}
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

        			// if ($id) {
    					// $id.enable();
        			// }
        		}
        	});


	},
    defaultSetting: {
        url: Url.DEFAULT_CGI,
        timeout: 0,
        returnHtml: false,
        success: function(){},
        complete: function(){}
    },
    killSearchPlmn: function() {
    	Page.postJSON({
            json: { 
    			cmd: RequestCmd.SEARCH_PLMN,
    			method: 'KILL'
    		},
            success: function(data) {
            }
        });
    },
    destory: function(){
        Page.currentId++;
        if (Page.isSearchingPlmn) {
        	Page.isSearchingPlmn = false;
        	Page.killSearchPlmn();
        }
    },
    createTable: function(title, names, values, count, columnNum, css) {
    	var sb = new StringBuilder();
    	sb.append(Page.createTableHead(title, css));

        if (count <= 0 || columnNum <= 0) return sb.toString();

    	Page.createTableBody(sb, names, values, count, columnNum, css);

    	return sb.toString();
    },
    createFieldset: function(title, names, values, count, columnNum, css) {
    	var sb = new StringBuilder();
    	sb.append("<fieldset>");
    	sb.append(String.format("<legend>{0}</legend>", title));
    	Page.createTableBody(sb, names, values, count, columnNum, css);
    	sb.append("</fieldset>");

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
    sortAp: function(data, field, sortDirection) {
    	var count = data.ssids.length;
    	var ssids = data.ssids, macs = data.macs, frequencys = data.frequencys,
    		singalLevels = data.singalLevels,
    		encryptionKeys = data.encryptionKeys,
    		encryptionModes = data.encryptionModes,
    		groupCiphers = data.groupCiphers,
    		pairwiseCiphers = data.pairwiseCiphers,
    		authenticationSuites = data.authenticationSuites;

    	for (var i = 0; i < count - 1; i++) {
    		singalLevels[i] = parseInt(singalLevels[i], 10);
    	}

    	var fields = eval(field);
    	var isAsc = (sortDirection == SortDirection.ASC);
        for (var i = 0; i < count - 1; i++) {
        	for (var j = i + 1; j < count; j++) {
        		if (isAsc) {
            		if (fields[j] < fields[i]) {
            			swapAll(i, j);
            		}
        		} else {
            		if (fields[j] > fields[i]) {
            			swapAll(i, j);
            		}
        		}
        	}
        }

        var min;
    	function swap(arr, i, j) {
    		min = arr[j];
    		arr[j] = arr[i];
    		arr[i] = min;
    	}

    	function swapAll(i, j) {
    		swap(ssids, i, j);
			swap(macs, i, j);
			swap(frequencys, i, j);
			swap(singalLevels, i, j);

			swap(encryptionKeys, i, j);
			swap(encryptionModes, i, j);
			swap(groupCiphers, i, j);
			swap(pairwiseCiphers, i, j);
			swap(authenticationSuites, i, j);
    	}
    },
    createApTableHtml: function(data) {
		var count = data.ssids.length, freq;
		var ssids = data.ssids, macs = data.macs, frequencys = data.frequencys,
			singalLevels = data.singalLevels,
			encryptionKeys = data.encryptionKeys,
    		encryptionModes = data.encryptionModes,
    		groupCiphers = data.groupCiphers,
    		pairwiseCiphers = data.pairwiseCiphers,
    		authenticationSuites = data.authenticationSuites;

		var sbTab = new StringBuilder();
		//sbTab.append("<tr class=\"fixed_header\"><th>序号</th><th>接入点名称(SSID)</th><th>MAC地址</th><th>信道</th><th>信号强度</th><th>选择</th></tr>");
		var encryptionFlag = (!!encryptionKeys);
		var singalLevel;
		for (var i = 0; i < count; i++) {
            singalLevel = FormatUtil.formatSingalLevel(singalLevels[i], encryptionFlag ? encryptionKeys[i] : null);

            sbTab.append(String.format("<tr title=\"{0}\">", singalLevel.title));

            sbTab.append(String.format("<td id=\"index{0}\" class=\"ap_index\">", i));
            sbTab.append(i + 1);
            sbTab.append("</td>");

            sbTab.append(String.format("<td id=\"ssid{0}\">", i));
            sbTab.append(ssids[i]);
            sbTab.append("</td>");

            sbTab.append(String.format("<td id=\"mac{0}\" class=\"ap_mac\">", i));
            sbTab.append(macs[i]);
            sbTab.append("</td>");

            sbTab.append(String.format("<td id=\"freq{0}\" class=\"ap_freq\">", i));
            freq = frequencys[i];
            sbTab.append(freq.substring(freq.indexOf("Channel") + 7, freq.lastIndexOf(")")));
            sbTab.append("</td>");

            sbTab.append(String.format("<td id=\"singal{0}\">", i));
            sbTab.append(singalLevel.text);

            sbTab.append("</td><td>");

            sbTab.append(String.format("<a id=\"choose{0}\" href=\"javascript:;\">选择</a>", i));
            sbTab.append(String.format("<input type=\"hidden\" id=\"encrypted{0}\" value=\"{1}\" />", i, singalLevel.isEncrypted));
            sbTab.append(String.format("<input type=\"hidden\" id=\"encryptionMode{0}\" value=\"{1}\" />", i, encryptionModes[i]));
            sbTab.append(String.format("<input type=\"hidden\" id=\"groupCipher{0}\" value=\"{1}\" />", i, groupCiphers[i]));
            sbTab.append(String.format("<input type=\"hidden\" id=\"pairwiseCipher{0}\" value=\"{1}\" />", i, pairwiseCiphers[i]));
            sbTab.append(String.format("<input type=\"hidden\" id=\"authenticationSuite{0}\" value=\"{1}\" />", i, authenticationSuites[i]));

            sbTab.append("</td></tr>");
        }

		return sbTab.toString();
	},
    createApTable: function(data, callback) {

        $('#progress_box').hide();

    	var $box = $('#info_box'), $header = $('#info_head'),
    	$tab_header = $('#info_tab_head'), $info = $('#info_main');
    	$box.show();
    	SysUtil.setBoxStyle($box);

    	if (data == null) {
    		$info.html("<div class=\"info\">查询数据超时，请重试</div>");
    	} else if (data.ssids.length == 0) {
    		$info.html("<div class=\"info\">对不起，没有查询到数据</div>");
    	} else {
    		var sb = new StringBuilder();
        	var sortField = "singalLevels", sortDirection = SortDirection.DESC;

        	function setHtml() {
    			Page.sortAp(data, sortField, sortDirection);

            	$('#tab_ap').html(Page.createApTableHtml(data));
            	Page.setStripeTable('#tab_ap');

            	setChoose();
        	}

        	function setChoose() {
        		$('a[id^=choose]').click(function() {
            		var index = $(this).attr('id').substring(6);
            		callback({
            			ssid: $('#ssid' + index).text(),
            			mac: $('#mac' + index).text(),
            			encrypted: $('#encrypted' + index).val().trim() == "true",
            			encryptionMode: $('#encryptionMode' + index).val().trim(),
            			groupCipher: $('#groupCipher' + index).val().trim(),
            			pairwiseCipher: $('#pairwiseCipher' + index).val().trim(),
            			authenticationSuite: $('#authenticationSuite' + index).val().trim()
            		});
            	});
        	}

        	$header.html("搜索结果如下：");

        	sb.append("<table cellspacing=\"0\">");
        	sb.append("<tr>");
        	sb.append("<th id=\"index\">序号</th>");
        	sb.append("<th id=\"ssids\" class=\"sort_field\">接入点名称(SSID)</th>");
        	sb.append("<th id=\"macs\" class=\"sort_field\">MAC地址</th>");
        	sb.append("<th id=\"frequencys\" class=\"sort_field\">信道</th>");
        	sb.append("<th id=\"singalLevels\" class=\"sort_field sort_current desc\">信号强度</th>");
        	sb.append("<th>选择</th>");
        	sb.append("</tr>");
        	sb.append("</table>");
        	$tab_header.html(sb.toString());
        	sb.clear();

        	sb.append("<table id=\"tab_ap\" class=\"detail\" cellspacing=\"0\">");

            Page.sortAp(data, sortField, sortDirection);
            sb.append(Page.createApTableHtml(data));
        	sb.append("</table>");

        	$info.html(sb.toString());
        	Page.setStripeTable('#tab_ap');
        	// 设置宽度
        	$('#index').css({"width": $('#index0').width() + "px"});
        	$('#ssids').css({"width": $('#ssid0').width() + "px"});
        	$('#macs').css({"width": $('#mac0').width() + "px"});
        	$('#frequencys').css({"width": $('#freq0').width() + "px"});
        	$('#singalLevels').css({"width": $('#singal0').width() + "px"});

        	setChoose();

        	$('th.sort_field').click(function() {
        		var field = this.id;
        		var $lastSortField = $('#' + sortField);
        		var $currentSortField = $('#' + field);
        		if (field != sortField) {
        			$lastSortField.removeClass('sort_current');
        			if (sortDirection == SortDirection.DESC) {
        				$lastSortField.removeClass(SortDirection.DESC);
        			} else {
        				$lastSortField.removeClass(SortDirection.ASC);
        			}
        			$currentSortField.addClass('sort_current');

        			sortField = field;
        			if (field == "singalLevels") {
        				sortDirection = SortDirection.DESC;
        			} else {
        				sortDirection = SortDirection.ASC;
        			}
        		} else {
        			if (sortDirection == SortDirection.DESC) {
        				sortDirection = SortDirection.ASC;
        				$lastSortField.removeClass(SortDirection.DESC);
        			} else {
        				sortDirection = SortDirection.DESC;
        				$lastSortField.removeClass(SortDirection.ASC);
        			}
        		}
        		$currentSortField.addClass(sortDirection);
    			setHtml();
        	});
    	}
    },
    getDeviceName: function(success, fail, error) {
    	Page.postJSON({
            json: {
            	cmd: RequestCmd.GET_DEVICE_NAME,
                method: JSONMethod.GET,
                sessionId: ""
            },
            success: function(data) {
            	if ($.isFunction(success)) {
            		//if(data.name === "tozedap-p11_4g"){
            			success(data);
            		//}
            	}
            },
            fail: function(data) {
            	if ($.isFunction(fail)) {
            		fail(data);
            	}
            },
            error: function() {
            	if ($.isFunction(error)) {
            		error();
            	}
            }
        });
    },
    getDeviceVersion: function(name, version) {
    	if (!version) return '';
    	
    	var deviceName = Page.deviceType || name;
    	var begin = version.indexOf('R');
    	var end = version.indexOf('C');
    	var dotIndex = version.indexOf('.') + 1;
    	if (end > begin && begin >= 0) {
    		return String.format('{0}_{1}_V{2}', deviceName.replace(' ', '_'), version.substring(begin, end), version.slice(dotIndex));
    	}
    	return version;
    },
    setTitle: function(displayedTitle, deviceType) {
		Page.deviceType = FormatUtil.formatField(deviceType || '');
		if (Page.deviceType == 'NULL') Page.deviceType = '';
		
    	if (displayedTitle != 'NULL' && displayedTitle != '') {
        	document.title = displayedTitle;
    	} else {
    		if (Page.deviceType) {
    			if (DOC.head.indexOf('LTE') >= 0) {
                	document.title = DOC.head.replace('LTE', Page.deviceType);
    			} else {
                	document.title = Page.deviceType + ' ' + DOC.head;
    			}
    		} else {
            	document.title = DOC.head;
    		}
    	}
    },
    setFormFieldValue: function(eleId, value){
    	// value值可能为0，不能用!value判断
    	if(typeof(value) == undefined || !eleId){
			return;
		}
		var $id = "#" + eleId;
		$($id).val(value);
    },
    setFormFieldValue2:function (eleId, value) {
        if(typeof(value) == undefined || !eleId){
            return;
        }
        var $id = "#" + eleId;
        $($id).val(value);
        $($id).attr('old',value);
    },
	
    getDifferentJson: function(oldJson, newJson) {
    	var mustKeys = ",cmd,subcmd,method,sessionId,language";
    	var json = {};
    	$.each(newJson, function(key, value) {
    		if (mustKeys.indexOf(key) > 0 || value != oldJson[key]) {
    			json[key] = value;
    		}
    	});
    	return json;
    },
    render: function(containerId, templateId, datas) {
    	if (!containerId) containerId = '#child_container';
    	if (!templateId) templateId = '#child_template';
    	if (!datas) datas = DOC;
    	
		$(containerId).html(_.template($(templateId).html(), datas));
    },
    renderData: function(datas) {
    	Page.render('#child_container', '#child_template', datas);
    	
		$(containerId).html(_.template($(templateId).html(), datas));
    },
    renderHelper: function(containerId, templateId, datas) {
    	if (!containerId) containerId = '#helper_container';
    	if (!templateId) templateId = '#helper_template';
    	if (!datas) datas = DOC;
    	
		$(containerId).html(_.template($(templateId).html(), datas));
    },
    getPreHostname: function() {
    	var hostname = FormatUtil.formatField(Page.lanIp || location.hostname);
    	var lastIndex = hostname.lastIndexOf('.');
    	if (lastIndex > 0) {
    		hostname = hostname.substring(0, lastIndex + 1);
    	} else {
    		hostname = "192.168.0.";
    	}
    	return hostname;
    }
};

var CheckUtil = {
	isEmpty: function(value) {
		return !value || value.length == 0 || value == 'NULL';
	},
	checkIp: function(ip, isIpv6) {
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
	checkIpPort: function(ipPort) {
		var reg = /^(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\:(\d+)$/;
		if (!reg.test(ipPort)) return false;
		var port = parseInt(RegExp.$5, 10);
		if (isNaN(port) || port < 0 || port > 65535) return false;
		return true;
	},
	checkNumber: function(num) {
		var reg = /^\d+$/;
		return reg.test(num);
	},
	checkHex: function(hex) {
		var reg = /^[0-9A-F]*$/gi;
		return reg.test(hex);
	},
	checkASCII: function(ascii) {
		var reg = /[\x00-\xff]/g;
		return reg.test(ascii);
	},
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
	checkPwd2: function(pwd) {
		var asciiReg = /^[0-9A-Za-z_\-\!#\^\@\,\.]{0,64}$/;
		return asciiReg.test(pwd);
	},
	checkeWepPwd68: function(pwd){
		var asciiReg = /^[\x00-\xff]{5}$/;
		var hexReg = /^[0-9A-F]{10}$/;
		return asciiReg.test(pwd) || hexReg.test(pwd);
	},
	checkeWepPwd128: function(pwd){
		var asciiReg = /^[\x00-\xff]{13}$/;
		var hexReg = /^[0-9A-F]{26}$/;
		return asciiReg.test(pwd) || hexReg.test(pwd);
	},
	checkNetSegment: function(lanip, netMask, ip){
		var lanips = ConvertUtil.ip4ToNum(lanip);
		var netMasks = ConvertUtil.ip4ToNum(netMask);
		var ips = ConvertUtil.ip4ToNum(ip);

		return (lanips & netMasks) == (ips & netMasks);
	},
	checkExeCmd: function(value){
		var reg = /.*/;
		return reg.test(value);
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

var DivUtil = {
    showDiv: function($id) {
        $id.width(document.documentElement.clientWidth);
        $id.height(document.documentElement.clientHeight);
        $id.show();
    },
    clearDiv: function($id) {
        $id.html("");
        $id.hide();
    },
    moveEvent: function(e, $id) {
        var isIE = (document.all)? true: false;
        var drag = true;
        var xPoint = isIE? event.x: e.pageX;
        var yPoint = isIE? event.y: e.pageY;
        offLeft = $id.offset().left;
        offTop = $id.offset().top;

        $(document).mousemove(function(e) {
            if (drag) {
                var x = isIE? event.x: e.pageX;
                if(x < 200) {
                    x = 200;
                }
                if(x > document.documentElement.clientWidth) {
                    x = document.documentElement.clientWidth;
                }
                var y = isIE? event.y: e.pageY;
                if(y < 100) {
                    y = 100;
                }
                if(y > document.documentElement.clientHeight) {
                    y = document.documentElement.clientHeight;
                }
                $id.css({
                    'top': offTop - yPoint + y - 100 + 'px',
                    'left': offLeft - xPoint + x - 200 + 'px'
                });
            }
        });
        $(document).mouseup(function(){
            drag = false;
        });
    }
};

var SmsUtil = {
	createTable: function(options) {
		var datas = options.datas, smsType = options.smsType;
		var beginIndex = options.pageIndex * options.pageSize;
		var lang = DOC.sms;

		var sb = new StringBuilder();
		sb.append('<table class="sms" cellspacing="0" cellpadding="0">');
		sb.append(String.format('<tr><th class="indexNo">{0}</th>', DOC.table.rowNo));
		sb.append(String.format('<th>{0}<input type="checkbox" id="chkAllSms" /></th>', lang.tab));

		var editText;
		if (smsType == SmsType.RECEIVE) {
			editText = lang.transmit;
			sb.append(String.format('<th>{0}</th>', lang.state));
			sb.append(String.format('<th>{0}</th>', lang.sender));
		} else {
			editText = DOC.table.edit;
			sb.append(String.format('<th>{0}</th>', lang.receiver));
		}
		sb.append(String.format('<th class="content">{0}</th><th>{1}</th>', lang.content, lang.time));
		if (smsType == SmsType.SEND) {
			sb.append(String.format('<th>{0}</th>', lang.sendState));
		}

		if (smsType == SmsType.RECEIVE) {
			sb.append(String.format('<th>{0}</th>', lang.reply));
		}

		sb.append(String.format('<th>{0}</th><th>{1}</th></tr>', editText, DOC.table.del));
		if (datas != null) {
			var length = datas.length;
			for (var i = 0; i < length; i++) {
				var item = datas[i];
				var id = beginIndex + i + 1;
				sb.append('<tr>');
				sb.append(String.format('<td class="indexNo">{0}', id));
				sb.append(String.format('<input id="hddSmsId{0}" type="hidden" value="{1}" /></td>', id, item.smsId));
				sb.append(String.format('<input id="hddSmsIndex{0}" type="hidden" value="{1}" /></td>', id, item.smsIndex));

				sb.append(String.format('<td><input id="chkSms{0}" type="checkbox" /></td>', id));
				if(smsType == SmsType.RECEIVE){
					sb.append(String.format('<td>{0}</td>', item.status == 1 ? lang.read : ("<span style=\"color:red;\">" + lang.unread + "</span>")));
				}
				sb.append(String.format('<td id="phoneNo{0}">{1}</td>', id, item.phoneNo));
				sb.append(String.format('<td id="content{0}" class="content"><table class="content"><tr style="height:20px;display:none;"><tr><td>{1}</td></tr></table></td>', id, item.content));
				sb.append(String.format('<td>{0}</td>', item.datetime));
				if (smsType == SmsType.SEND) {
					if (item.status == 1)
						sb.append(String.format('<td class="ok">{0}</td>', lang.sendSuccess));
					else
						sb.append(String.format('<td class="fail">{0}</td>', lang.sendFailed));
				}
				if(smsType == SmsType.RECEIVE){
					sb.append(String.format('<td><a id="reply{0}" class="edit" href="javascript:;">{1}</a></td>', id, lang.reply));
				}
				sb.append(String.format('<td><a id="edit{0}" class="edit" href="javascript:;">{1}</a></td>', id, editText));
				sb.append(String.format('<td><a id="del{0}" class="edit" href="javascript:;">{1}</a></td>', id, DOC.table.del));
				sb.append('</tr>');
			}
		}
		sb.append('</table>');

		$(options.id).html(sb.toString());
        Page.setStripeTable('table.sms');

        $('#chkAllSms').click(function() {
        	var checked = $(this).prop('checked');
        	$('input[id^=chkSms]').prop('checked', checked);
        });

        $('a[id^=edit]').click(function() {
        	var id = this.id.slice(4);
        	var phoneNo = (smsType == SmsType.RECEIVE) ? "" : $('#phoneNo' + id).text().trim();

        	options.edit($('#hddSmsId' + id).val(), phoneNo, $('#content' + id).text().trim());
        });

        $('a[id^=reply]').click(function() {
        	var id = this.id.slice(5);
        	//var phoneNo = (smsType == SmsType.RECEIVE) ? "" : $('#phoneNo' + id).text().trim();
        	var phoneNo = $('#phoneNo' + id).text().trim();
        	options.edit("-1", phoneNo, "");
        });

        $('a[id^=del]').click(function() {
        	var id = this.id.slice(3);
        	options.del($('#hddSmsId' + id).val(), $('#hddSmsIndex' + id).val());
        });
	}
};

var StatusUtil = {
	getNetState: function(signal, eps, netMode, netDataMode) {
	    var theSignal = parseInt(signal, 10);
		var theNetMode = parseInt(netMode, 10);
	    var theNetDataMode = parseInt(netDataMode, 10);
	    var theEps = parseInt(eps, 10);

	    var stateComment = "?";

	    if(isNaN(theSignal) || theSignal <= 0 || theSignal > 5){
	    	return stateComment;
	    }
	    switch(theNetDataMode){
	        case 0:
	            if (theNetMode == 7 || theEps == 1 || theEps == 5)
	                stateComment = "4G";
	            else if (theNetMode == 2)
		            stateComment = "3G";
	            else if (theNetMode == 0)
		            stateComment = "2G";
	            else if (theNetMode == 1)
		            stateComment = "2G";
	            break;
	        case 1:
	            stateComment = "2G";
	            break;
	        case 2:
	            stateComment = "2G";
	            break;
	        case 3:
	        case 4:
	        case 5:
	        case 6:
	            stateComment = "3G";
	            break;
	        default:
	            break;
	    }

	    return stateComment;
	},
	getNetSys: function(netMode, netModeEx){
    	if (netModeEx) {
    		return netModeEx;
    	}
		 var theNetMode = parseInt(netMode, 10);
		 var stateComment = "?";
		    switch(theNetMode){
		        case 0:
		        	stateComment = "GSM";
		            break;
		        case 1:
		            stateComment = "GSM Compact";
		            break;
		        case 2:
		            stateComment = "TD";
		            break;
		        case 7:
		            stateComment = "LTE";
		            break;
		        default:
		            break;
		    }

		    return stateComment;
	},
	formatSingalLevel: function(signalLevel, preCss, content) {
		var ddl = DOC.ddl;
		var signals = [ddl.signalLevel0, ddl.signalLevel1, ddl.signalLevel2, ddl.signalLevel3, ddl.signalLevel4, ddl.signalLevel5];
	    var theSignalLevel = parseInt(signalLevel, 10);
	    if(isNaN(theSignalLevel) || theSignalLevel > 5 || theSignalLevel < 0){
	    	theSignalLevel = 0;
	    }
	    var title = DOC.lte.signalLevel + DOC.colon + signals[theSignalLevel];

	    return String.format("<div class=\"singal singal{0}\" title=\"{1}\">{2}</div>", (preCss || "") + theSignalLevel, title, content || "");
	},
	formatSimInfo: function(state) {
	    var theState = parseInt(state, 10);
		var title, css;
	    if(theState == 0){
	    	css = "invalid";
	    	title = DOC.status.noSim;
	    } else if(theState >= 1) {
	        css = "normal";
	    	title = DOC.status.existSim;
	    } else {
	        css = "invalid";
	        title = DOC.status.invalid;
	    }

	    return String.format("<div class=\"{0}\" title=\"{1}\">SIM</div>", css, title);
	},
	formatLteUsbInfo: function(state) {
	    var theState = parseInt(state, 10);
	    var title, css;
	    if(theState == 1) {
	        css = "invalid";
	        title = DOC.status.noUsb;
	    } else if(theState == 0) {
	        css = "normal";
	        title = DOC.status.normalUsb;
	    } else {
	        css = "invalid";
	        title = DOC.status.invalidUsb;
	    }

	    return String.format("<div class=\"{0}\" title=\"{1}\">AT</div>", css, title);
	},
	formatWiFiInfo: function(state) {
	    var title, css;
	    //alet(state);
		    if(state == "0") {
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
	formatLanWanInfo: function($id, state, lbl) {
	    if(state == "NOTSUPPORT"){
	    	$id.hide();
	    	if (lbl == "WAN") {
		    	$('#siginfo').width(450);
	    	} else {
		    	$('#siginfo').width(380);
	    	}
	    	return "";
	    } else {
	    	var title, css;

	    	$id.show();
	    	if ($.trim(state) == "1") {
	    		css = "link";
		        title = DOC.status.connected;
	    	} else {
		        css = "unlink";
		        title = DOC.status.disconnected;
	    	}

		    return String.format("<div class=\"{0}\" title=\"{1}\">{2}</div>", css, title, lbl);
	    }
	},
	hasNewSmsPrompt: false,
	smsIntervalId: null,
	setSmsInfo: function(state) {
		var theState = parseInt(state, 10);
		var title, css;
	    if(theState == 1){
	        css = "newsms";
	    	title = DOC.status.newMessage;
	    } else {
	    	css = "normal";
	    	title = DOC.status.message;
	    }

	    function setHtml() {
	        $('#smsInfo').html(String.format("<div class=\"{0}\" title=\"{1}\">{1}</div>", css, title));
	    }

	    function clearSmsInterval() {
	       if (StatusUtil.smsIntervalId != null)
	           clearInterval(StatusUtil.smsIntervalId);
	    }

	    if (theState == 1) {
	        // 新消息是否已经提示
	        if (!StatusUtil.hasNewSmsPrompt) {
	        	StatusUtil.hasNewSmsPrompt = true;

	            var count = 0;
	            StatusUtil.smsIntervalId = setInterval(function() {
	                css = (count % 2 == 0) ? "newsms" : "newsms2";
	                setHtml();
	                if (count > 10) {
	                	clearSmsInterval();
	                }
	                count++;
	            }, 500);
	        }
	    } else {
	        StatusUtil.hasNewSmsPrompt = false;
	        clearSmsInterval();
	    	setHtml();
	    }
	},
    isAnyRegSuccess: function(state, ps, eps) {
    	return StatusUtil.isRegSuccess(state) ||
	    	StatusUtil.isRegSuccess(ps) ||
	    	StatusUtil.isRegSuccess(eps);
    },
    isRegSuccess: function(regStateCode) {
    	return regStateCode == 1 || regStateCode == 5;
    },
    getRegState: function(state, eps){
    	var theState = parseInt(state, 10);
        if (typeof(eps) != 'undefined') {
            var theEps = parseInt(eps, 10);
            // 注册状态为未注册，但EPS为已注册时，使用EPS的注册状态
            if ((theState != 1 || theState != 5 ) && (theEps == 1 || theEps == 5)) {
                theState = theEps;
            }
        }
        
        var registerStatus = DOC.registerStatus;
        var items = [registerStatus.item0, registerStatus.item1, registerStatus.item2,
                     String.format('<span class="error strong">{0}</span>', registerStatus.item3),
                     registerStatus.item4, registerStatus.item5, registerStatus.item6];
        var length = items.length;
        if (theState >=0 && theState < length) {
        	return items[theState];
        }
        return items[length - 1];
    },
    parseInds: function(tozedinds) {
    	var defaultInds = "-1 -1 -1 -1 -1 -1 -1 -1 -1 -1";
    	var inds = (tozedinds || defaultInds).split(" ");
        if (inds.length < 9) return defaultInds.split(" ");
        
        return {
        	simCardState: inds[0],
        	pinState: inds[1],
        	pukState: inds[2],
        	singalLevel: inds[3],
        	noname: inds[4],
        	state: inds[5],
        	ps: inds[6],
        	eps: inds[7],
        	netMode: inds[8],
        	netDataMode: inds[9]
        };
    },
    getSysStatus: function() {
    	var sysStatus = null;
    	var $lan1 = $('#lan1'), $lan2 = $('#lan2'),
    		$lan3 = $('#lan3'), $lan4 = $('#lan4'),
    		$wan1 = $('#wan1');

        function loop() {
         //   Page.postJSON({
         //        json: {
         //            cmd: RequestCmd.GET_SYS_STATUS,
         //            method: JSONMethod.GET,
         //            sessionId: ""
         //        },
         //        success: function(data) {
         //        	sysStatus = data;
         //        },
         //       complete: function() {
         //           setTimeout(loop, 5000);
         //           if (sysStatus == null ) return ;//|| !sysStatus.tozedinds) return;
                    var sysStatus = {"wifiOpen":"0","cardType":"0","web_signal":"","tozedinds":""};
                    Page.enableWiFi = sysStatus.wifiOpen ;
                    /*
                    if (!Page.isItemHide(ItemHideTable.WIFI) && Page.isSupported(ItemUnsupportedTable.DISPLAY_WIFI)) {
                    	$('#wifiInfo').html(StatusUtil.formatWiFiInfo(sysStatus.wifi, sysStatus.wifiOpen)).show();
                    } else {
                    	$('#wifiInfo').hide();
                    }
                    */
                    $('#wifiInfo').html(StatusUtil.formatWiFiInfo(sysStatus.wifiOpen));
                    $('#simInfo').html(StatusUtil.formatSimInfo(sysStatus.cardType));
                    if(sysStatus.cardType >= 1)                    {
                    	var singalLevel = sysStatus.web_signal;
                    	var netInfo = StatusUtil.formatSingalLevel(singalLevel);
                    	$('#netInfo').html(netInfo);
                	}else{
                		var netInfo = StatusUtil.formatSingalLevel(0);
                    	$('#netInfo').html(netInfo);
                	}
                    return;

                    var tozedinds = sysStatus.tozedinds.split(" ");
                    if (tozedinds.length < 9) return;

                    var singalLevel = tozedinds[3];
                    var netStatus = StatusUtil.getNetState(tozedinds[3], tozedinds[7], tozedinds[8], tozedinds[9]);
                    if (!StatusUtil.isAnyRegSuccess(tozedinds[5], tozedinds[6], tozedinds[7])) {
                    	// 都注册失败时，信号强度直接显示为无信号
                    	singalLevel = 0;
                    	netStatus = '?';
                    }
                    var netInfo = StatusUtil.formatSingalLevel(singalLevel, "2_", netStatus);
                    $('#netInfo').html(netInfo);
                    
                    if (sysStatus.lteUsb == "0" && Page.isSupported(ItemUnsupportedTable.DISPLAY_USB_SERIAL_PORT)) {
                        $('#lteUsbInfo').html(StatusUtil.formatLteUsbInfo(sysStatus.lteUsb)).show();
                    } else {
                    	$('#lteUsbInfo').hide();
                    }

                    StatusUtil.setSmsInfo(sysStatus.smsind);

                    if(sysStatus.dialMode == DialMode.MANUALLY && sysStatus.connectStatus == ConnectStatus.DISCONNECT){
                    	Page.connectStatus = false;
                    } else {
                    	Page.connectStatus = true;
                    }
                    Page.isSimLocked = ((sysStatus.simLockingStatus || '').trim() == "1");
                    Page.requiredPIN = (tozedinds[1] == "1");
                    Page.requiredPUK = (tozedinds[2] == "1");

                    if (sysStatus.closeInternet == "1") {
                    	Page.isCloseInternet = true;
                    	$('#closeInternet').html(PROMPT.tips.closeInternet1).addClass('close_internet');
                    } else {
                    	Page.isCloseInternet = false;
                    	$('#closeInternet').html("").removeClass('close_internet');
                    }
                    if (sysStatus.useSpecifiedSimCard == "1") {
                    	Page.useSpecifiedSimCard = true;
                    	$('#closeInternet').html(PROMPT.tips.closeInternet2).addClass('close_internet');
                    } else {
                    	Page.useSpecifiedSimCard = false;
                    }
                    if (sysStatus.remoteCloseInternet == "1") {
                    	Page.remoteCloseInternet = true;
                    	$('#closeInternet').html(PROMPT.tips.closeInternet3).addClass('close_internet');
                    } else {
                    	Page.remoteCloseInternet = false;
                    }
                    
                    // hide LAN
                    if (sysStatus.macCount > 0) {
                    	sysStatus.lan1 = 0;
                    	sysStatus.lan2 = 0;
                    	sysStatus.lan3 = 0;
                    	sysStatus.lan4 = 0;
                    }
                    $lan1.html(StatusUtil.formatLanWanInfo($lan1, sysStatus.lan1, "LAN1"));
                    $lan2.html(StatusUtil.formatLanWanInfo($lan2, sysStatus.lan2, "LAN2"));
                    $lan3.html(StatusUtil.formatLanWanInfo($lan3, sysStatus.lan3, "LAN3"));
                    $lan4.html(StatusUtil.formatLanWanInfo($lan4, sysStatus.lan4, "LAN4"));
                    $wan1.html(StatusUtil.formatLanWanInfo($wan1, sysStatus.wan1, "WAN"));
                    
                    var systime = sysStatus.systime;
                    if (systime.length > 3) {
                        systime = systime.substring(0, systime.length - 3);
                    }
                    
                    var lteConnectStatus = DOC.status.searchingNetwork;
                    // eps register
                    var eps = parseInt(tozedinds[7], 10);
                    if (eps == 2) {
                    	lteConnectStatus = DOC.status.connecting;
                    } else {
                    	if (StatusUtil.isRegSuccess(eps) && CheckUtil.checkIp(sysStatus.wanIp)) {
                        	lteConnectStatus = DOC.status.connected;
                    	}
                    }
                    var connectStatusHtml = '';
                    if (Page.isSupported(ItemSupportedTable.DISPLAY_LTE_STATUS, true) && !Page.isItemHide(ItemHideTable.LTE_STATUS_INFO)) {
                    	connectStatusHtml = String.format('<span class="connect_status">{0}</span>&nbsp;', lteConnectStatus);
                    }
                    $('#timeinfo').html(String.format('{0}{1}', connectStatusHtml, systime));
           //     }
         //   });
        }

        loop();
    }
};


var ATUtil = {
    params: {
		SEPARATOR: '$',
		FIELD_SEPARATOR: '@',
		SYSTEM_MODE: 'System Mode',
		OPERATION_MODE: 'Operation Mode',
		PLMN: 'MCC MNC',
		LAC_TAC: 'TAC/LAC',
		SERVING_CELLID: 'Serving CellID',
		PHYSICAL_CELLID: 'Physical CellID',
		CURRENT_BAND: 'Frequency Band',
		EARFCN: 'EARFCN/ARFCN',
		DOWNLINK_BANDWIDTH: 'Downlink Bandwidth',
		UPLINK_BANDWIDTH: 'Uplink Bandwidth',
		RSRQ: 'RSRQ',
		RSRP: 'RSRP',
		RSRP2: 'RSRP2',
		RSSI: 'RSSI',
		RSSI2: 'RSSI2',
		SINR: 'SINR',
		SINR2: 'SINR2',
		TZTRANSMODE: 'TZTRANSMODE',
		TZTA: 'TZTA',
		TZTXPOWER: 'TZTXPOWER'
	},
	sendAT: function(atcmd, callback) {
		Page.postJSON({
	        json: {
	        	method: JSONMethod.POST,
	        	cmd: RequestCmd.LTE_AT,
	        	atcmd: atcmd
	        },
	        success: function(data) {
	        	callback(data);
	        }
    	});
	},
	getEmciInfo: function(callback) {
		var success = false;
		var enodeId = '', freqPoint = '', phyCellId = '';
		function parseEmciInfo(message) {
			if (message.indexOf('ERROR') < 0) {
				var index = message.indexOf('+ZEMCI:');
				if (index >= 0) {
					var items = message.substring(index + 7).split(',');
					if (items.length >= 14) {
						success = true;
						var cellId = '00000000' + parseInt(items[0].trim(), 10).toString(16);
						cellId = cellId.substring(cellId.length - 8);
						enodeId = parseInt(cellId.substring(0, 6), 16);

						var scell = '00000000' + parseInt(items[14].trim(), 16).toString(16);
						scell = scell.substring(scell.length - 8);
						freqPoint = parseInt(scell.substring(4, 8), 16);
						phyCellId = parseInt(scell.substring(0, 4), 16);
					}
				}
			}
			
			callback({
				success: success,
				enodeId: enodeId,
				freqPoint: freqPoint,
				phyCellId: phyCellId
			});
		}
		
		Page.postJSON({
			returnHtml: true,
			json: { cmd: RequestCmd.GET_EMCI_INFO },
			success: function(data) {
				parseEmciInfo(data);
			}
		});
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
		debugger;
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

var SysCheckUtil = {
	lockedCellStatus: "-",
	getAtLockedStatus: function(callback) {
		if (SysCheckUtil.lockedCellStatus != "-") {
			if ($.isFunction(callback)) {
        		callback();
        	}
			return;
		}
		Page.postJSON({
	        json: {
	        	method: JSONMethod.POST,
	        	cmd: MODEM_CMD,
	        	subcmd: 1
	        },
	        success: function(data) {
	        	if (data.message.indexOf('ERROR') >= 0) {
	    			return;
	        	}
	        	var reg = /.*\DPCILK\: (\d+(\,\d+\,\d+)?)/;
        		var matches = data.message.match(reg);
        		var items = [];
	        	if (matches != null) {
	        		items = matches[1].split(',');
	        	}
	        	if (items.length > 0 && items[0] == "1") {
	        		SysCheckUtil.lockedCellStatus = DOC.status.lockedLocal;
	        	} else {
	        		SysCheckUtil.lockedCellStatus = DOC.status.unlocked;
	        	}
	        },
	        complete: function() {
	        	if ($.isFunction(callback)) {
	        		callback();
	        	}
	        }
    	});
	},
	getLockedStatus: function() {
		Page.postJSON({
	        json: {
	    		method: JSONMethod.GET,
	    		cmd: RequestCmd.LOCK_PHY_CELL
	    	},
	        success: function(data) {
	        	var locked = (data.enable == "yes");
	        	if (!locked) {
	        		SysCheckUtil.getAtLockedStatus();
	        	} else {
	        		SysCheckUtil.lockedCellStatus = DOC.status.lockedRemote;
	        	}
	        }
		});
	}
};


function getSysCheck(callback) {
	var DEFAULT_TOZEDIND = "-1 -1 -1 -1 -1 -1 -1 -1 -1 -1";
    function formatField(success, failMsg) {
    	if (!failMsg) {
    		failMsg = "";
    	} else {
    		failMsg = String.format('({0})', failMsg);
    	}
    	var css = "fail", msg = DOC.status.fail;
    	if (success) {
    		css = "ok";
    		msg = DOC.status.success;
    	}
		return String.format('<span class="{0}">{1}{2}</span>', css, msg, failMsg);
    }
    
    var data = null;
    function createTable() {
    	if (data == null) return;
    	
    	var tozedind = data.tozedind ? data.tozedind : DEFAULT_TOZEDIND;
        var tozedinds = tozedind.split(' ');
        var names = [], values = [];
        var lteNames = DOC.lte,
	    	netNames = DOC.net,
	    	wlanNames = DOC.wlan;

        names.push(DOC.lbl.runtime);
        names.push(DOC.lbl.internetAccessRights);
        //names.push(DOC.lte.signalQuality);
        //if (Page.isChinese()) {
        //    names.push(DOC.lbl.regState.replace('CS', ''));
        //} else {
        //    names.push(DOC.lbl.netRegisterState);
        //}
        //names.push(DOC.lbl.usim);
        //names.push(DOC.lbl.pinCode);
        //names.push(DOC.lbl.pukCode);
        //names.push(DOC.lbl.wanDev);
        names.push(DOC.lbl.wan + " " + DOC.net.ip);
        //names.push(DOC.net.mask);
        names.push(DOC.net.gateway);
        names.push(DOC.lbl.dns);
        
        if (Page.isModulL1761()) {
            names.push(DOC.lte.phyCellStatus);
        }
        
        var runInfo = ConvertUtil.parseUptime(data.uptime.trim(), DOC.unit);
        values.push(runInfo.runTime);
        
        if (Page.remoteCloseInternet) {
            values.push(formatField(false, PROMPT.tips.closeInternet3));
        } else if (Page.useSpecifiedSimCard) {
            values.push(formatField(false, PROMPT.tips.closeInternet2));
        } else if (Page.isCloseInternet) {
            values.push(formatField(false, PROMPT.tips.closeInternet1));
        } else {
        	values.push(formatField(true));
        }
        
        //var signal = parseInt(tozedinds[3], 10);
        //values.push(formatField(signal > 0));

        //values.push(formatField(StatusUtil.isAnyRegSuccess(tozedinds[5], tozedinds[6], tozedinds[7])));

        //var usim = parseInt(tozedinds[0], 10);
        //values.push(formatField(usim == 1));

        //var pinCode = parseInt(tozedinds[1], 10);
        //values.push(formatField(pinCode == 0));

        //var pukCode = parseInt(tozedinds[2], 10);
        //values.push(formatField(pukCode == 0));

        //values.push(formatField(data.netDevStatus == "1"));
        values.push(formatField(CheckUtil.checkIp(data.wanIp)));
        //values.push(formatField(CheckUtil.checkIp(data.wanMaskIp)));
        values.push(formatField(CheckUtil.checkIp(data.wanGateway)));
        values.push(formatField(CheckUtil.checkIp(data.wanDNS) || CheckUtil.checkIp(data.wanDNS_2)));

        if (Page.isModulL1761()) {
        	values.push(SysCheckUtil.lockedCellStatus);
        }
        
        var sys_html = Page.createTable(DOC.title.selfChecking, names, values, names.length, 1);
        callback(sys_html);
    }

	Page.postJSON({
        json: { cmd: RequestCmd.SYS_INFO },
        success: function(datas) {
        	data = datas;

            if (!Page.isModulL1761()) {
        		createTable();
            } else {
            	var locked = (datas.lockOneCell == "yes");
            	if (!locked) {
            		SysCheckUtil.getAtLockedStatus(createTable);
            	} else {
            		SysCheckUtil.lockedCellStatus = DOC.status.lockedRemote;
            		createTable();
            	}
            }
        }
    });
}


function getOpenInfo() {
	var loading = '-';
	var theRouterInfo = {}, theLteInfo = {};
	Page.isNULLToSpace = true;
	
 	function loop() {
 		getOpenInfo(function(html, noRefresh) {
 			$('#device_check').html(html).show();
            Page.setStripeTable('#device_check');
     		
            if (!noRefresh) {
            	setTimeout(loopForCheck, 5000);
            }
 		});
 	}
	
    function createTable(routerInfo, lteInfo, noRefresh) {
    	//var supportedLte = Page.isSupported(ItemSupportedTable.DISPLAY_LTE_STATUS, true) && !Page.isItemHide(ItemHideTable.LTE_STATUS_INFO);
        var supportedLte = true;
    	var lte_info_id = '#lte_info';
    	var lte_info_advanced_id = '#lte_info_advanced';
    	var wan_info_id = supportedLte ? '#wan_info' : '#wan_info2';
    	var wlan_info_id = supportedLte ? '#wlan_info' : '#wlan_info2';

    	//lteInfo = $.extend({}, lteInfo, theLteInfo);
        //var tozedinds = StatusUtil.parseInds(sysInfo.tozedind);
        var names = [], values = [];
        var names_advanced = [], values_advanced = [];
        var lteNames = DOC.lte,
        	netNames = DOC.net,
        	wlanNames = DOC.wlan;

        var html;
	var vsinr,vrsrp,vrssi;
    	if (supportedLte) {
	        names.push(DOC.device.imsi);
	        names.push(DOC.lte.plmn);
	        names.push(lteNames.phyCellId);
	        names.push(lteNames.sinr);
	        names.push(lteNames.rsrp);
			names.push(lteNames.rssi);

	        //names_advanced.push(lteNames.tm);
	        names_advanced.push(lteNames.freqPoint);
	        names_advanced.push(lteNames.rsrq);
	        names_advanced.push(lteNames.globalCellId);
	        names_advanced.push(lteNames.enodeB);
	        names_advanced.push(lteNames.volteRegisterStatus);
	        names_advanced.push(TAB.advance.roaming);
	        names_advanced.push(DOC.device.imei);

	        values.push(FormatUtil.formatField(routerInfo.imsi || loading));
	        values.push(FormatUtil.formatField(lteInfo.lte_plmn || loading));
	        values.push(FormatUtil.formatField(lteInfo.phyCellId || loading));
		if(lteInfo.sinr2!=null)
		{
			vsinr=(parseInt(lteInfo.sinr,10)+parseInt(lteInfo.sinr2,10))/2;
		}
		else if(lteInfo.sinr!=null)
		{
			vsinr=lteInfo.sinr;
		}
		else
		{
			vsinr=0;
		}
		if(lteInfo.rsrp2!=null)
		{
			vrsrp=(parseInt(lteInfo.rsrp,10)+parseInt(lteInfo.rsrp2,10))/2;
		}
		else if(lteInfo.rsrp!=null)
		{
			vrsrp=lteInfo.rsrp;
		}
		else
		{
			vrsrp=0;
		}
		if(lteInfo.rssi2!=null)
		{
			vrssi=(parseInt(lteInfo.rssi,10)+parseInt(lteInfo.rssi2,10))/2;
		}
		else if(lteInfo.rssi!=null)
		{
			vrssi=lteInfo.rssi;
		}
		else
		{
			vrssi=0;
		}

		values.push(lteInfo.sinr ? FormatUtil.formatField(vsinr, 'dB') : loading);
	        values.push(lteInfo.rsrp ? FormatUtil.formatField(vrsrp, 'dBm') : loading);
		values.push(lteInfo.rssi ? FormatUtil.formatField(vrssi, 'dBm') : loading);
	        //values.push(lteInfo.sinr ? FormatUtil.formatField(lteInfo.sinr, 'dB') : loading);
	        //values.push(lteInfo.rsrp ? FormatUtil.formatField(lteInfo.rsrp, 'dBm') : loading);
		//values.push(lteInfo.rssi ? FormatUtil.formatField(lteInfo.rssi, 'dBm') : loading);
	        //values.push(lteInfo.txpower ? FormatUtil.formatField(lteInfo.txpower, 'dBm') : loading);

	        //values_advanced.push(lteInfo.tm ? FormatUtil.formatField('tm ' + lteInfo.tm) : loading);
	        values_advanced.push(FormatUtil.formatField(lteInfo.freq || loading));
	        values_advanced.push(lteInfo.rsrq ? FormatUtil.formatField(lteInfo.rsrq, 'dB') : loading);
	        values_advanced.push(FormatUtil.formatField(lteInfo.globalCellId || loading));
	        values_advanced.push(FormatUtil.formatField(lteInfo.lte_enodebid || loading));
	        values_advanced.push(FormatUtil.formatField(lteInfo.volteRegister || loading));
	        values_advanced.push(FormatUtil.formatField(lteInfo.simcard_roam || loading));
	        values_advanced.push(FormatUtil.formatField(routerInfo.imei || loading));
	        
	        html = Page.createTable(DOC.title.lteInfoBasic, names, values, names.length, 1);
	        $('#device_check').show();
	        $(lte_info_id).html(html);
	        //$(lte_info_id).css({ 'color': '#F18906' });
	        Page.setStripeTable(lte_info_id);
	        
	        html = Page.createTable(DOC.title.lteInfoAdvanced, names_advanced, values_advanced, names_advanced.length, 1);
	        $(lte_info_advanced_id).html(html);
	        Page.setStripeTable(lte_info_advanced_id);
    	}
        
        names = [];
        names.push(DOC.lbl.runtime);
        names.push(DOC.device.firmwareVersion);
        names.push(DOC.device.modemVersion);
        
        values = [];
        var runInfo = ConvertUtil.parseUptime((routerInfo.uptime || '').trim(), DOC.unit);
        values.push(runInfo.runTime);
        //values.push(FormatUtil.formatField(Page.getDeviceVersion(routerInfo.name, routerInfo.version) || loading));certificationVer
        values.push(FormatUtil.formatField( routerInfo.certificationVer|| loading));
        values.push(FormatUtil.formatField(routerInfo.modversion || loading));
        
        html = Page.createTable(DOC.title.router, names, values, names.length, 1);
        $('#router_info').html(html);
        Page.setStripeTable('#router_info');
        
        names = [];
        names.push(netNames.ip);
        //names.push(netNames.mask);
        names.push(netNames.dns1);
        names.push(netNames.dns2);
        //names.push(netNames.gateway);
        names.push(netNames.ipv6);
        //names.push(netNames.gatewayv6);
        //names.push(netNames.dns3);
        //names.push(netNames.dns4);
        
        values = [];
        values.push(FormatUtil.formatField(routerInfo.wanIp || loading));
        //values.push(FormatUtil.formatField(routerInfo.wanMaskIp || loading));
        values.push(FormatUtil.formatField(routerInfo.wanDNS || loading));
        values.push(FormatUtil.formatField(routerInfo.wanDNS_2 || loading));
        //values.push(FormatUtil.formatField(routerInfo.wanGateway || loading));
        values.push(FormatUtil.formatField(routerInfo.wanIpv6 || loading));
        //values.push(FormatUtil.formatField(routerInfo.wanGatewayIpv6 || loading));
        //values.push(FormatUtil.formatField(routerInfo.wanDNS_3 || loading));
        //values.push(FormatUtil.formatField(routerInfo.wanDNS_4 || loading));

        html = Page.createTable(DOC.title.wan, names, values, names.length, 1);
        $(wan_info_id).html(html);
        Page.setStripeTable(wan_info_id);

        names = [];
        names.push(netNames.ip);
        names.push(netNames.mask);
        names.push(netNames.ipBegin);
        names.push(netNames.ipEnd);
        
        values = [];
        values.push(FormatUtil.formatField(routerInfo.lanIp || loading));
        values.push(FormatUtil.formatField(routerInfo.lanMaskIp || loading));
        values.push(FormatUtil.formatField(routerInfo.ipBegin || loading));
        values.push(FormatUtil.formatField(routerInfo.ipEnd || loading));
        
        //html = Page.createTable(DOC.title.lan, names, values, names.length, 1);
        //$(lan_info_id).html(html);
        //Page.setStripeTable(lan_info_id);
        
        if (1) {
	        names = [];
	        names.push(wlanNames.ssid);
	        names.push(wlanNames.channel);
	        names.push(wlanNames.wifi);
	        names.push(DOC.lbl.connectedDevicesViaWiFi);
	        //names.push(wlanNames.secMode);
	        
	        values = [];
	        values.push(FormatUtil.formatField(routerInfo.ssid || loading));
	        values.push(FormatUtil.formatField(routerInfo.channel || loading));
	        //values.push(FormatUtil.formatField(ConvertUtil.frequencyToChannel(routerInfo.frequency) || loading));
	        
	        var wifiOpen = routerInfo.wifiOpen;
	        if(wifiOpen == WiFi.CLOSED) {
	        	values.push(DOC.status.closed);
	        } else if(wifiOpen == WiFi.OPENED) {
	        	values.push(DOC.status.opened);
	        } else {
	        	values.push(loading);
	        }
	        //values.push(String.format('<span id="spanWiFiCount">{0}</span>', Page.wifiUserCount == null ? '' : Page.wifiUserCount.toString()));
	        values.push(FormatUtil.formatField(routerInfo.sta_count || loading))
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
 		
        if (!noRefresh) {
        	setTimeout(getRouterInfo, 10000);
        }
    }
    
    var SEPARATOR = '$', FIELD_SEPARATOR = '@',
		SYSTEM_MODE = 'System Mode',
		OPERATION_MODE = 'Operation Mode',
		PLMN = 'MCC MNC',
		LAC_TAC = 'TAC/LAC',
		SERVING_CELLID = 'Serving CellID',
		PHYSICAL_CELLID = 'Physical CellID',
		CURRENT_BAND = 'Frequency Band',
		EARFCN = 'EARFCN/ARFCN',
		DOWNLINK_BANDWIDTH = 'Downlink Bandwidth',
		UPLINK_BANDWIDTH = 'Uplink Bandwidth',
		RSRQ = 'RSRQ', RSRP = 'RSRP',RSRP2 = 'RSRP2',
		RSSI = 'RSSI',RSSI2 = 'RSSI2', SINR = 'SINR',SINR2 = 'SINR2',
		TZTRANSMODE = 'TZTRANSMODE',
		TZTA = 'TZTA',
		TZTXPOWER = 'TZTXPOWER';
    
    function getLteInfo(routerInfo) {
    	Page.isOpenPage = true;
    	/*
    	//为什么要在首页中得到这个网页?
    	Page.getHtml(MenuItem.CLIENT_LIST.url, MenuItem.CLIENT_LIST.cmd, function(data) {
    		$('#client_info_hidden').html(data);
    	});
    	*/
    	if (!Page.isSupported(ItemSupportedTable.DISPLAY_LTE_STATUS, true) || Page.isItemHide(ItemHideTable.LTE_STATUS_INFO)) {
        	createTable(routerInfo, {}, true);
        	return;
    	}
    	
    	Page.postJSON({
	        json: {
	    		method: JSONMethod.POST,
	    		cmd: RequestCmd.GET_LTE_STATUS
	    	},
	        success: function(data) {
	        	/*
	    		var lteInfo = {};
	        	var phyCellId = '';
        		var items = data.message.split(SEPARATOR);
		        for (var i = 0; i < items.length; i++) {
			        var item = items[i].split(FIELD_SEPARATOR);
			        if (item.length == 2) {
				        switch(item[0]) {
				        case EARFCN: lteInfo.freq = item[1]; break;
				        case CURRENT_BAND: lteInfo.band = item[1]; break;
				        case RSRP: lteInfo.rsrp = item[1]; break;
					case RSRP2: lteInfo.rsrp2 = item[1]; break;
				        case RSRQ: lteInfo.rsrq = item[1]; break;
				        case SINR: lteInfo.sinr = item[1]; break;
					case SINR2: lteInfo.sinr2 = item[1]; break;
					case RSSI: lteInfo.rssi = item[1]; break;
					case RSSI2: lteInfo.rssi2 = item[1]; break;
				        
				        case SERVING_CELLID:
					        phyCellId = parseInt(item[1], 10);
							if (!isNaN(phyCellId)) {
                                lteInfo.globalCellId = phyCellId.toString( 16 ).toUpperCase();
								Page.cellId = phyCellId % 256;
				        		Page.enodeId = (phyCellId - Page.cellId) / 256;
				    			
				        		lteInfo.enodeBId = String.format('{0}/{1}', Page.enodeId, Page.cellId);
				    		} else {
                                lteInfo.globalCellId = phyCellId;
				    			lteInfo.enodeBId = phyCellId;
				    		}
					        break;
				        
				        case PHYSICAL_CELLID: lteInfo.phyCellId = item[1]; break;
					        
				        case DOWNLINK_BANDWIDTH: lteInfo.bandWidth = item[1]; break;
				        case TZTRANSMODE: lteInfo.tm = item[1]; break;
				        case TZTA: lteInfo.tzta = item[1]; break;
				        case TZTXPOWER: lteInfo.txpower = item[1]; break;
				        
				        default: break;
				        }
				    }
			    }
				*/
		        theLteInfo = data;
            	createTable(routerInfo, theLteInfo);
	        },
            fail: function() {
            	createTable(routerInfo, theLteInfo);
            },
            error: function() {
            	createTable(routerInfo, theLteInfo);
            }
    	});
    }

    function getRouterInfo() {
    	Page.postJSON({
            json: { cmd: RequestCmd.ROUTER_INFO },
            success: function(routerInfo) {
            	// save
            	theRouterInfo = routerInfo;
            	
            	getLteInfo(routerInfo);
            	createTable(routerInfo, theLteInfo, true);
            },
            fail: function() {
                getLteInfo(theRouterInfo);
            },
            error: function() {
                getLteInfo(theRouterInfo);
            }
        });
    }
    
    //getRouterInfo();
	createTable(theRouterInfo, theLteInfo, true);
};

function MenuHead(css, text, bodys) {
	this.css = css;
	this.text = text;
	this.bodys = [];
}

MenuHead.prototype.push = function(body, itemHide, itemSupported, supportFlag) {
	if (itemHide && Page.isItemHide(itemHide)) return;
	if (itemSupported && !Page.isSupported(itemSupported, supportFlag)) return;
	
	this.bodys.push(body);
}

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


