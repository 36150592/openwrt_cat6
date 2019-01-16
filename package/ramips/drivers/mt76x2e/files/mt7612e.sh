#!/bin/sh
append DRIVERS "mt7612e"

. /lib/wifi/ralink_common.sh

prepare_mt7612e() {
	prepare_ralink_wifi mt7612e
}

scan_mt7612e() {
	scan_ralink_wifi mt7612e mt76x2e
}

disable_mt7612e() {
	disable_ralink_wifi mt7612e
}

enable_mt7612e() {
	local ifn
	for ifn in rai0 rai1 rai2 rai3; do
		ifconfig $ifn down 2>/dev/null
		set_wifi_down $ifn
	done

	enable_ralink_wifi mt7612e mt76x2e
}

detect_mt7612e() {
#	detect_ralink_wifi mt7612e mt76x2e
	ssid=tozed-5g-`ifconfig eth0 | grep HWaddr | cut -c 51- | sed 's/://g' | sed s/' '//g`
	cd /sys/module/
	[ -d $module ] || return
	[ -e /etc/config/wireless ] && return
	 cat <<EOF
config wifi-device      mt7612e
 	option type     mt7612e
 	option vendor   ralink
	option band     5G
	option txpower	100
	option aregion	0
	option bw	2
	option aregion  0
	option country	CN
	option channel  149
	option autoch   0
    option mode     14
    
config wifi-iface
    option device   mt7612e
    option ifname   rai0
    option network  lan
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
        option device 'mt7612e'      
        option network 'lan1'         
        option mode 'ap'             
        option ssid ${ssid}-second   
        option encryption 'psk2+ccmp'
        option key '12345678'        
        option maxassoc '20'         
        option ifname 'rai1'
        option disabled '1'
                                     
config wifi-iface                    
        option device 'mt7612e'      
        option network 'lan2'         
        option mode 'ap'             
        option ssid '${ssid}-third'  
        option encryption 'psk2+ccmp'
        option key '12345678'        
        option maxassoc '20' 
        option ifname 'rai2'
        option disabled '1'
                           
EOF

}


