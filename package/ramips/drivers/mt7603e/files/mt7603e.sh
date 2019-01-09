#!/bin/sh

append DRIVERS "mt7603e"

debug() {
 # echo "debug: ""$1"
  echo -n
}

#修复无法重置raX虚拟接口问题。
reload_mt7603e() {
	debug "reload_mt7603e"
	local vif
	for vif in ra0 ra1 ra2 ra3 ra4 ra5 ra6 ra7 wds0 wds1 wds2 wds3 apcli0; do
	ifconfig $vif down 2>/dev/null
	set_wifi_down $vif
	#debug "ifdown $vif"
	done
	rmmod mt7603e
	insmod mt7603e
}

scan_mt7603e() {
	debug "scan_mt7603e"
	debug "$1 $2 $3"
	local device="$1"
	local ifname="$1"

	mt7603e_phy_if=0
	
	#重置整个驱动
#	reload_mt7603e

	local i=-1
	
	while grep -qs "^ *ra$((++i)):" /proc/net/dev; do
	debug "found phy$i ra$i inteface"
	let mt7603e_phy_if+=1
	done

	mt7603e_ap_num=0
	mt7603e_wds_num=0
	mt7603e_sta_num=0
	mt7603e_sta_first=0
	
	config_get vifs "$device" vifs
	
	for vif in $vifs; do
		config_get_bool disabled "$vif" disabled 0
		[ $disabled -eq 0 ] || continue

		config_get mode "$vif" mode
		case "$mode" in
			adhoc)
				echo "mt7603e only support ap+wds+sta!"
			;;
			sta)
				[ ${mt7603e_sta_num:-0} -ge 1 ] && {
					echo "mt7603e only support 1 sta!"
				}
				mt7603e_sta_num=$(($mt7603e_sta_num + 1))
				debug "mt7603e_sta_num=$mt7603e_sta_num"
			;;
			ap)
				mt7603e_ap_num=$(($mt7603e_ap_num + 1))
				debug "mt7603e_ap_num=$mt7603e_ap_num"
				apmode=1
				mt7603e_ap_vif="${mt7603e_ap_vif:+$mt7603e_ap_vif }$vif"
			;;
			wds)
				config_get wds_addr "$vif" bssid
				[ -z "$wds_addr" ] || {
					wds_addr=$(echo "$wds_addr" | tr 'A-F' 'a-f')
					append mt7603e_wds "$wds_addr"
				}
			;;
			monitor)
			;;
			*) echo "$device($vif): Invalid mode";;
		esac
	done
}


mt7603e_prepare_config() {

	debug "mt7603e_prepare_config"
	
	#config_get 变量名 Section名 Section参数名
	
	#获取参数 存储配置的变量 目标配置关键字
	local ssid_num=0 apcli_num=0 mode disabled
	local maxra0=0 maxra1=0	
#准备产生RaX的无线配置
	local device=$1

#获取当前的无线频道
	config_get channel $device channel

#获取当前的802.11无线模式
	config_get hwmode $device mode
	
#获取WMM支持
	config_get wmm $device wmm
	
#获取设备的传输功率
	config_get txpower $device txpower
	
#获取设备的HT（频宽）
	config_get ht $device ht

#获取国家代码	
	config_get country $device country
	
#是否有MAC过滤
	config_get macpolicy $device macpolicy
	config_get macpolicyra1 $device macpolicy
	
#MAC地址过滤列表
	config_get maclist $device maclist
	config_get maclistra1 $device maclist
#字符格式转义
	ra_maclist="${maclist// /;};"
	ra_maclistra1="${maclistra1// /;};"
#是否支持GREEN AP功能
	config_get_bool greenap $device greenap 0

	config_get_bool antdiv "$device" diversity
	
	config_get frag "$device" frag 2346
	
	config_get rts "$device" rts 2347
	
	config_get distance "$device" distance

	config_get hidessid "$device" hidden 0
	
#获取该Radio下面的虚拟接口	
	config_get vifs "$device" vifs

#大于11的时候支持11-14号频道
	[ "$channel" != "auto" ] && {
	[ ${channel:-0} -ge 1 -a ${channel:-0} -le 11 ] && countryregion=0
	[ ${channel:-0} -ge 12 -a ${channel:-0} -le 13 ] && countryregion=1
	[ ${channel:-0} -eq 14 ] && countryregion=31
	debug "channel=$channel countryregion=$countryregion"
	}

	countryregion=1

#获取虚拟接口的数量，并提前配置SSID
for vif in $vifs; do
	config_get_bool disabled "$vif" disabled 0
	config_get mode "$vif" mode 0
	
	#如果关闭，则略过该vif配置
	[ "$disabled" == "1" ]&& {
	set_wifi_down $vif
	continue
	}
	
	#如果某个SSID接口需要隐藏，则所有的接口都隐藏
	[ "$hidessid" == "0" ] && {
	config_get hidessid $vif hidden 0
	}
	
	
	[ "$mode" == "sta" ] || [ "$mode" == "wds" ] && {
	continue
	}

	let ssid_num+=1 #SSID Number
	case $ssid_num in
	1)
		config_get ssid1 "$vif" ssid
		;;
	2)
		config_get ssid2 "$vif" ssid
		;;
	3)
		config_get ssid3 "$vif" ssid
		;;
	4)
		config_get ssid4 "$vif" ssid
		;;
	*)
		echo "Only support 4 MSSID!"
		;;
	esac
done

#开始准备HT模式配置，注意HT模式仅在11N下有效。
	HT=1
	HT_CE=1

    if [ "$ht" = "20" ]; then
      HT=0 
    elif [ "$ht" = "20+40" ]; then
      HT=1 
      HT_CE=1
    elif [ "$ht" = "40" ] ; then
      HT=1 
      HT_CE=0
    else
    echo "ht config error!use default!!!"
      HT=0
      HT_CE=1
    fi


	# 在HT40模式下,需要另外的一个频道，如果EXTCHA=1,则当前第二频道为$channel + 4.
	# 如果EXTCHA=0,则当前的第二频道为$channel - 4.
	# 如果当前频道被限制在1-4,则是当前频道+ 4，若否，则为当前频道-4 
	
	EXTCHA=1
	
	[ "$channel" != "auto" ] && [ "$channel" -lt "5" ] && EXTCHA=0

#配置自动选择无线频道
    [ "$channel" == "auto" ] && {
        channel=11
        AutoChannelSelect=2 #增强型自动频道选择
    }

#开始判断WiFi的MAC过滤方式.
    case "$macpolicy" in
	allow|2)
	ra_macfilter=1;
	;;
	deny|1)
	ra_macfilter=2;
	;;
	*|disable|none|0)
	ra_macfilter=0;
	;;
    esac

    case "$macpolicyra1" in
	allow|2)
	ra_macfilterra1=1;
	;;
	deny|1)
	ra_macfilterra1=2;
	;;
	*|disable|none|0)
	ra_macfilterra1=0;
	;;
    esac
	#FIXME:单独的STA模式无法启动。
	[ "$mt7603e_ap_num" = "0" ] && [ "$mt7603e_sta_num" = "1" ] && {
	 ssid_num=1
	}
					
	cat > /tmp/RT2860.dat<<EOF
#The word of "Default" must not be removed
Default
CountryRegion=${countryregion:-1}
CountryRegionABand=0
CountryCode=${country:-US}
BssidNum=${ssid_num:-1}
SSID1=${ssid1:-RW2458_SSID1}
SSID2=${ssid2:-RW2458_SSID2}
SSID3=${ssid3:-RW2458_SSID3}
SSID4=${ssid4:-RW2458_SSID4}
SSID5=
SSID6=
SSID7=
SSID8=
WirelessMode=${hwmode:-9}
FixedTxMode=
TxRate=0
Channel=${channel:-11}
BasicRate=15
BeaconPeriod=100
DtimPeriod=1
TxPower=${txpower:-100}
DisableOLBC=0
BGProtection=0
TxAntenna=
RxAntenna=
TxPreamble=1
RTSThreshold=${rts:-2347}
FragThreshold=${frag:-2346}
TxBurst=1
PktAggregate=1
AutoProvisionEn=0
FreqDelta=0
TurboRate=0
WmmCapable=1
APAifsn=3;7;1;1
APCwmin=8;8;3;2
APCwmax=16;20;4;3
APTxop=8;8;94;47
APACM=0;0;0;0
BSSAifsn=3;7;2;2
BSSCwmin=8;8;3;2
BSSCwmax=16;20;4;3
BSSTxop=6;6;94;47
BSSACM=0;0;0;0
AckPolicy=0;0;0;0
APSDCapable=1
DLSCapable=0
NoForwarding=0
NoForwardingBTNBSSID=0
HideSSID=${hidessid:-0}
ShortSlot=1
AutoChannelSelect=${AutoChannelSelect:-0}
IEEE8021X=0
IEEE80211H=1
CarrierDetect=0
ITxBfEn=0
PreAntSwitch=
PhyRateLimit=0
DebugFlags=0
ETxBfEnCond=0
ITxBfTimeout=0
ETxBfTimeout=0
ETxBfNoncompress=0
ETxBfIncapable=1
FineAGC=0
StreamMode=0
StreamModeMac0=
StreamModeMac1=
StreamModeMac2=
StreamModeMac3=
CSPeriod=10
WirelessEvent=0
IdsEnable=0
AuthFloodThreshold=32
AssocReqFloodThreshold=32
ReassocReqFloodThreshold=32
ProbeReqFloodThreshold=32
DisassocFloodThreshold=32
DeauthFloodThreshold=32
EapReqFooldThreshold=32
RDRegion=
StationKeepAlive=0
DfsLowerLimit=0
DfsUpperLimit=0
DfsOutdoor=0
SymRoundFromCfg=0
BusyIdleFromCfg=0
DfsRssiHighFromCfg=0
DfsRssiLowFromCfg=0
DFSParamFromConfig=0
FCCParamCh0=
FCCParamCh1=
FCCParamCh2=
FCCParamCh3=
CEParamCh0=
CEParamCh1=
CEParamCh2=
CEParamCh3=
JAPParamCh0=
JAPParamCh1=
JAPParamCh2=
JAPParamCh3=
JAPW53ParamCh0=
JAPW53ParamCh1=
JAPW53ParamCh2=
JAPW53ParamCh3=
FixDfsLimit=0
LongPulseRadarTh=0
AvgRssiReq=0
DFS_R66=0
BlockCh=
GreenAP=0
PreAuth=0
AuthMode=OPEN
EncrypType=NONE
WapiPsk1=0.16.06789
WapiPsk2=
WapiPsk3=
WapiPsk4=
WapiPsk5=
WapiPsk6=
WapiPsk7=
WapiPsk8=
WapiPskType=0
Wapiifname=
WapiAsCertPath=
WapiUserCertPath=
WapiAsIpAddr=
WapiAsPort=
RekeyMethod=DISABLE
RekeyInterval=3600
PMKCachePeriod=10
MeshAutoLink=0
MeshAuthMode=
MeshEncrypType=
MeshDefaultkey=0
MeshWEPKEY=
MeshWPAKEY=
MeshId=
WPAPSK1=.16.0678
WPAPSK2=
WPAPSK3=
WPAPSK4=
WPAPSK5=
WPAPSK6=
WPAPSK7=
WPAPSK8=
DefaultKeyID=1
Key1Type=0
Key1Str1=
Key1Str2=
Key1Str3=
Key1Str4=
Key1Str5=
Key1Str6=
Key1Str7=
Key1Str8=
Key2Type=0
Key2Str1=
Key2Str2=
Key2Str3=
Key2Str4=
Key2Str5=
Key2Str6=
Key2Str7=
Key2Str8=
Key3Type=0
Key3Str1=
Key3Str2=
Key3Str3=
Key3Str4=
Key3Str5=
Key3Str6=
Key3Str7=
Key3Str8=
Key4Type=0
Key4Str1=
Key4Str2=
Key4Str3=
Key4Str4=
Key4Str5=
Key4Str6=
Key4Str7=
Key4Str8=
HSCounter=0
HT_HTC=0
HT_RDG=1
HT_LinkAdapt=0
HT_OpMode=0
HT_MpduDensity=5
HT_EXTCHA=${EXTCHA}
HT_BW=${HT:-0}
HT_AutoBA=1
HT_BADecline=0
HT_AMSDU=1
HT_BAWinSize=64
HT_GI=1
HT_STBC=1
HT_LDPC=0
HT_MCS=33
VHT_BW=0
VHT_SGI=1
VHT_STBC=0
VHT_BW_SIGNAL=0
VHT_DisallowNonVHT=0
VHT_LDPC=1
HT_TxStream=2
HT_RxStream=2
HT_PROTECT=1
HT_DisallowTKIP=1
HT_BSSCoexistence=0
GreenAP=${greenap:-0}
WscConfMode=0
WscConfStatus=1
WCNTest=0
AccessPolicy0=${ra_macfilter:-0}
AccessControlList0=${ra_maclist:-0}
AccessPolicy1=${ra_macfilterra1:-0}
AccessControlList1=${ra_maclistra1:-0}
AccessPolicy2=0
AccessControlList2=
AccessPolicy3=0
AccessControlList3=
AccessPolicy4=0
AccessControlList4=
AccessPolicy5=0
AccessControlList5=
AccessPolicy6=0
AccessControlList6=
AccessPolicy7=0
AccessControlList7=
WdsEnable=0
WdsPhyMode=
WdsEncrypType=NONE
WdsList=
Wds0Key=
Wds1Key=
Wds2Key=
Wds3Key=
RADIUS_Server=
RADIUS_Port=1812
RADIUS_Key1=
RADIUS_Key2=
RADIUS_Key3=
RADIUS_Key4=
RADIUS_Key5=
RADIUS_Key6=
RADIUS_Key7=
RADIUS_Key8=
RADIUS_Acct_Server=
RADIUS_Acct_Port=1813
RADIUS_Acct_Key=
own_ip_addr=
Ethifname=
EAPifname=
PreAuthifname=
session_timeout_interval=0
idle_timeout_interval=0
WiFiTest=0
TGnWifiTest=0
ApCliEnable=0
ApCliSsid=
ApCliBssid=
ApCliAuthMode=
ApCliEncrypType=
ApCliWPAPSK=
ApCliDefaultKeyID=0
ApCliKey1Type=0
ApCliKey1Str=
ApCliKey2Type=0
ApCliKey2Str=
ApCliKey3Type=0
ApCliKey3Str=
ApCliKey4Type=0
ApCliKey4Str=
RadioOn=1
SSID=
WPAPSK=0.16.06789
Key1Str=
Key2Str=
Key3Str=
Key4Str=
WscManufacturer=
WscModelName=
WscDeviceName=
WscModelNumber=
WscSerialNumber=
PMFMFPC=0
PMFMFPR=0
PMFSHA256=0
LoadCodeMethod=0
EOF

}
disable_mt7603e() {
	debug "disable_mt7603e"
	local vif
	for vif in ra0 ra1 ra2 ra3 ra4 ra5 ra6 ra7 wds0 wds1 wds2 wds3 apcli0; do
	ifconfig $vif down 2>/dev/null
	set_wifi_down $vif
	#debug "ifdown $vif"
	done
	
	true
}

enable_mt7603e_wps_pbc() {
#注册 代理 中继
iwpriv $1 set WscConfMode=7 
#WPS AP未配置
iwpriv $1 set WscConfStatus=2
#WPS PBC模式
iwpriv $1 set WscMode=2
#WPS WSC V2支持
#iwpriv $1 set WscV2Support=1
}

mt7603e_start_vif() {
	local vif="$1"
	local ifname="$2"

	local net_cfg
	net_cfg="$(find_net_config "$vif")"
	[ -z "$net_cfg" ] || start_net "$ifname" "$net_cfg"

	set_wifi_up "$vif" "$ifname"
}

enable_mt7603e() {
	
	#传参过来的第一个参数是device
	local device="$1" dmode if_num=-1;
	debug "enable_mt7603e"
	
	#检查并创建WiFi驱动配置链接
	[ -f /etc/wireless/mt7603e/mt7603e.dat ] || {
	ln -s /tmp/RT2860.dat /etc/wireless/mt7603e//mt7603e.dat 2>/dev/null
#	ln -s /tmp/RT2860.dat /etc/Wireless/RT2860/RT2860AP.dat 2>/dev/null
	}

        #[ -f /etc/wireless/mt7628/mt7628.dat ] || {                             
        #mkdir -p /etc/wireless/mt7628/ 2>/dev/null                              
        #}                                                                       
                                                                                
       # rm /etc/wireless/mt7628/mt7628.dat                                     
       # ln -s /tmp/RT2860.dat /etc/wireless/mt7628/mt7628.dat 2>/dev/null       
           	
	
	#重置整个驱动
	reload_mt7603e
	debug "mt7603e_ap_num=$mt7603e_ap_num"
	config_get_bool disabled "$device" disabled 0	
	if [ "$disabled" = "1" ] ;then
	return
	fi
	
	#开始准备该设备的无线配置参数
	mt7603e_prepare_config $device
	
	config_get dmode $device mode
	config_get vifs "$device" vifs

	#config_get maxassoc $device maxassoc 0
	
	local mt7603e_vifs mt7603e_ap_vifs  mt7603e_sta_vifs mt7603e_wds_vifs
	
	for vif in $vifs; do
		debug "vifs vif=$vif"
		config_get mode "$vif" mode
		[ "$mode" == "ap" ] && {
			mt7603e_ap_vifs="$mt7603e_ap_vifs $vif"
		}
		[ "$mode" == "sta" ] && {
			mt7603e_sta_vifs="$mt7603e_sta_vifs $vif"
		}
		[ "$mode" == "wds" ] && {
			mt7603e_wds_vifs="$mt7603e_ap_vifs $vif"
		}
	done
	
	mt7603e_vifs="$mt7603e_vifs$mt7603e_ap_vifs $mt7603e_sta_vifs $mt7603e_wds_vifs "
	
#	for vif in $vifs; do
	for vif in $mt7603e_vifs; do
		config_get_bool disabled $vif disabled 0
		#如果关闭，则略过该vif配置
		[ "$disabled" == "1" ] && {
			continue
		}
		
		local ifname encryption key ssid mode
		
#		config_get ifname $vif device			
		config_get encryption $vif encryption
		config_get key $vif key
		config_get ssid $vif ssid
		config_get mode $vif mode
		config_get maxassoc $vif maxassoc
#		echo $maxassoc >>/root/test	
		#获取WPS配置信息
		local wps pin
		config_get wps $vif wps 
		config_get pin $vif pin
			
		#是否隔离客户端
		config_get isolate $vif isolate 0
		
		#802.11h
		config_get doth $vif doth 0

#		config_get hidessid $vif hidden 0	

		#排除如果设置为sta wds
		[ "$mode" != "sta" ] && [ "$mode" != "wds" ] && {
		let if_num+=1
		debug "if_num=$if_num"
		
		#根据ifname数量配置多SSID
		ifname="ra$if_num"
		}
		
		#STA APClient配置
		[ "$mode" == "sta" ] && {

					[ "$mt7603e_ap_num" = "0" ] && [ "$mt7603e_sta_num" = "1" ] || [ "$if_num" == "-1" ]&& {
						#初始化phy
						ifconfig ra0 up
					}
					
					#如果为apcli模式，指定接口名称为apcli0
					ifname="apcli0"
					
					echo "#Encryption" >/tmp/wifi_encryption_${ifname}.dat
					
					ifconfig $ifname down
					iwpriv $ifname set ApCliEnable=0 
					iwpriv $ifname set ApCliSsid=$ssid
					config_get bssid $vif bssid 0
					[ -z "$mode" ] && {
					iwpriv $ifname set ApCliBssid=$bssid
					echo "APCli use bssid connect."
					}
			case "$encryption" in
				none)
					echo "NONE" >>/tmp/wifi_encryption_${ifname}.dat
					iwpriv $ifname set ApCliAuthMode=OPEN 
					iwpriv $ifname set ApCliEncrypType=NONE 
					;;
				WEP|wep|wep-open)
					echo "WEP" >>/tmp/wifi_encryption_${ifname}.dat
					iwpriv $ifname set AuthMode=WEPAUTO
					iwpriv $ifname set ApCliEncrypType=WEP
					iwpriv $ifname set Key0=${key}
					;;
				WEP-SHARE|wep-shared)
					echo "WEP" >>/tmp/wifi_encryption_${ifname}.dat
					iwpriv $ifname set AuthMode=WEPAUTO
					iwpriv $ifname set ApCliEncrypType=WEP
					iwpriv $ifname set Key0=${key}
					;;
				WPA*|wpa*|WPA2-PSK|psk*)
					echo "WPA2" >>/tmp/wifi_encryption_${ifname}.dat
					iwpriv $ifname set ApCliAuthMode=WPAPSKWPA2PSK
					iwpriv $ifname set ApCliEncrypType=AES
					iwpriv $ifname set ApCliWPAPSK=$key
					echo "WPAPSKWPA2PSK" >>/tmp/wifi_encryption_${ifname}.dat
					echo "TKIPAES" >>/tmp/wifi_encryption_${ifname}.dat
					;;
			esac
					iwpriv $ifname set ApCliEnable=1
					ifconfig $ifname up
					#FIXME:单独STA模式
					[ "$mt7603e_ap_num" = "0" ] && [ "$mt7603e_sta_num" = "1" ] && {
						ifconfig ra0 down
					}
		}
		#AP模式配置
		[ "$mode" == "ap" ] && {
			[ "$key" = "" -a "$vif" = "private" ] && {
				logger "no key set serial"
				key="AAAAAAAAAA"
			}
			[ "$dmode" == "6" ] && wpa_crypto="aes"
			ifconfig $ifname up
			#判断当前加密模式
			echo "#Encryption" >/tmp/wifi_encryption_${ifname}.dat
			iwpriv $ifname set "SSID=${ssid}"
			case "$encryption" in
				#找到WPA/WPA2加密
				wpa*|psk*|WPA*|Mixed|mixed)
					echo "WPA" >>/tmp/wifi_encryption_${ifname}.dat
					local enc
					case "$encryption" in
						Mixed|mixed|psk+psk2)
							enc=WPAPSKWPA2PSK
							;;
						WPA2*|wpa2*|psk2*)
							enc=WPA2PSK
							;;
						WPA*|WPA1*|wpa*|wpa1*|psk*)
							enc=WPAPSK
							;;
					esac
					local crypto="AES"
					case "$encryption" in
					*tkip+aes*|*tkip+ccmp*|*aes+tkip*|*ccmp+tkip*)
						crypto="TKIPAES"
						;;
					*aes*|*ccmp*)
						crypto="AES"
						;;
					*tkip*) 
						crypto="TKIP"
						echo Warring!!! TKIP not support in 802.11n 40Mhz!!!
					;;
					esac
					echo "$enc" >>/tmp/wifi_encryption_${ifname}.dat
					echo "$crypto" >>/tmp/wifi_encryption_${ifname}.dat
					iwpriv $ifname set AuthMode=$enc
					iwpriv $ifname set EncrypType=$crypto
					iwpriv $ifname set IEEE8021X=0
					iwpriv $ifname set "SSID=${ssid}"
					iwpriv $ifname set "WPAPSK=${key}"
					iwpriv $ifname set DefaultKeyID=2
					#iwpriv $ifname set "SSID=${ssid}"
					
					#配置WPS	
					if [ "$wps" == "pbc" ]  && [ "$encryption" != "none" ]; then {
					enable_mt7603e_wps_pbc $ifname
					touch /tmp/"$ifname"_wps_pbc.lock 2>/dev/null
					}
					else
					rm -rf /tmp/"$ifname"_wps_pbc.lock 2>/dev/null
					fi;
					
					;;
					
				WEP|wep|wep-open|wep-shared)
					echo "WEP" >>/tmp/wifi_encryption_${ifname}.dat
					iwpriv $ifname set AuthMode=WEPAUTO
					iwpriv $ifname set EncrypType=WEP
					iwpriv $ifname set IEEE8021X=0
					for idx in 1 2 3 4; do
						config_get keyn $vif key${idx}
						[ -n "$keyn" ] && iwpriv $ifname set "Key${idx}=${keyn}"
					done
					iwpriv $ifname set DefaultKeyID=${key}
					iwpriv $ifname set "SSID=${ssid}"
					echo 
					iwpriv $ifname set WscConfMode=0
					;;
				none|open)
					echo "NONE" >>/tmp/wifi_encryption_${ifname}.dat
					iwpriv $ifname set AuthMode=OPEN
					iwpriv $ifname set EncrypType=NONE
					iwpriv $ifname set WscConfMode=0
					;;
			esac
		}
		
		#如果关闭了WIFI，则关闭RF
		if [ $disabled == 1 ]; then
		 iwpriv $ifname set RadioOn=0
		 set_wifi_down $ifname
		else
		 iwpriv $ifname set RadioOn=1
		fi
	
		if [ $hidessid ==  1 ];then                                     
                iwpriv $ifname set HideSSID=1                                   
                #echo $ifname                                                   
                else                                                            
                iwpriv $ifname set HideSSID=0                                   
                fi 
	
		#隔离客户端连接。
		[ $isolate == "1" ]&&{
			iwpriv $ifname set NoForwarding=1
		}
		
#		[ $maxassoc -gt 0 ]&&{
			iwpriv $ifname  set MaxStaNum=$maxassoc
#		}		
		
		#802.11h 支持
		[ $doth == "1" ]&&{
			iwpriv $ifname set IEEE80211H=1
		}	
		
		ifconfig "$ifname" up
		if [ "$mode" == "sta" ];then {
			net_cfg="$(find_net_config "$vif")"
			[ -z "$net_cfg" ] || {
					mt7603e_start_vif "$vif" "$ifname"

			}
		}
		else
		{
			count=0
			local net_cfg bridge
			net_cfg="$(find_net_config "$vif")"
			[ -z "$net_cfg" ]||{
				
				bridge="$(bridge_interface "$net_cfg")"
				while [ $count -lt 3 ]
				do
					[ -z "$bridge" ] || break
					count=$(($count+1))
					echo "count="$count
					sleep 1
				done		
				if [ $count -lt 3 ];then
			
				config_set "$vif" bridge "$bridge"
				mt7603e_start_vif "$vif" "$ifname"
				#Fix bridge problem
				[ -z `brctl show |grep $ifname` ] && {
				brctl addif $(bridge_interface "$net_cfg") $ifname
				}
				fi
			}



		}
		fi;
		
		set_wifi_up "$vif" "$ifname"
		debug "set vif=$vif $ifname up"
	done
	
	#配置无线最大连接数
	#iwpriv $device set MaxStaNum=100
    local tmprssi1=`/sbin/uci  get wireless.ra0.KickStaRssiLow 2> /dev/null`
   # local tmprssi2=`/sbin/uci  get wireless.ra0.AuthRssiThres 2> /dev/null`
    local tmprssi3=`/sbin/uci  get wireless.ra0.AssocReqRssiThres 2> /dev/null`
    if [ -z "$tmprssi1" ]; then
         tmprssi1="100"                                                          
    fi                            
    echo $tmprssi1     
   iwpriv $device set KickStaRssiLow=-$tmprssi1	
    
    if [ -z "$tmprssi2" ]; then
         tmprssi2="100"                                                          
    fi                            
    echo $tmprssi2     
#    iwpriv $device set AuthRssiThres=-$tmprssi2
    
    if [ -z "$tmprssi3" ]; then
         tmprssi3="100"                                                          
    fi                            
    echo $tmprssi3     
    iwpriv $device set AssocReqRssiThres=-$tmprssi3

   qos-start
	
   #ifconfig br-lan:1 192.168.1.220	  
 
}

#获取MAC地址
mt7603e_get_mac() {
#	/lib/functions.sh
	factory_part=$(find_mtd_part $1)
	dd bs=1 skip=4 count=6 if=$factory_part 2>/dev/null | /usr/sbin/maccalc bin2mac	
}

#detect_mt7603e函数用于检测是否存在驱动
detect_mt7603e() {
	local i=-1
#判断系统是否存在mt7603e_ap，不存在则退出
	cd /sys/module/
	[ -d mt7603e ] || return

#	[ -f /etc/config/wireless ] && return 
	
#检测系统存在多少个wifi接口
	while grep -qs "^ *ra$((++i)):" /proc/net/dev; do
		config_get type mt7603e type
#		echo "test"$type
		[ "$type" = mt7603e ] && continue
		
		[ "$type" = mt7612e ] && continue  

        [ -f /etc/wireless/mt7603e/mt7603e.dat ] || {
        ln -s /tmp/RT2860.dat /etc/wireless/mt7603e//mt7603e.dat 2>/dev/null
        }






	
	
	local pre_wpa2_key
	pre_wpa2_key=$(hexdump -n 8 /dev/urandom |awk '{print $2$3$4$5;}')
#	mt7603e_mac=$(mt7603e_get_mac Factory)	
	
	ssid=tozed-`ifconfig eth0 | grep HWaddr | cut -c 48- | sed 's/://g' | sed s/' '//g`

	
		cat <<EOF
config wifi-device  mt7603e
	option type     mt7603e
	option mode 	7
	option channel  auto
	option txpower 100
	option ht 	20
	option country CN
	option band 2.4G	
	
config wifi-iface
	option device   mt7603e
	option network	lan
	option mode     ap
	option ssid     $ssid
	option encryption psk2+ccmp
	option key      12345678
    option maxassoc 20
EOF

	

	[ -f /etc/config/mutilssid ] || {
		cat > /etc/config/mutilssid <<EOF
#this four temp config must not be removed,if remove uci  will apaer id repeat bug
config temp                                                                       
config temp                                       
config temp                                       
config temp                                       
             
EOF

	}

	cat >> /etc/config/mutilssid <<EOF
config wifi-iface                                 
        option device 'mt7603e'      
        option network 'lan'         
        option mode 'ap'             
        option ssid ${ssid}-second   
        option encryption 'psk2+ccmp'
        option key '12345678'        
        option maxassoc '20'         
        option ifname 'ra1'
        option disabled '1'          
                                     
config wifi-iface                    
        option device 'mt7603e'      
        option network 'lan'         
        option mode 'ap'             
        option ssid '${ssid}-third'  
        option encryption 'psk2+ccmp'
        option key '12345678'        
        option maxassoc '20' 
        option ifname 'ra2'
        option disabled '1' 
                           
EOF
done
}


