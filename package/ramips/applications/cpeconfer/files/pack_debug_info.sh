#!/bin/sh

if [ ! -f  /tmp/log_zip ];then
	mkdir /tmp/log_zip
fi

logread > /tmp/log_zip/logread
dmesg > /tmp/log_zip/dmesg
ps > /tmp/log_zip/ps
cp /proc/meminfo /tmp/log_zip/meminfo
cp /proc/uptime /tmp/log_zip/uptime
lsof > /tmp/log_zip/lsof
lsmod > /tmp/log_zip/lsmod
lsusb > /tmp/log_zip/lsusb
cp -r /etc/config  /tmp/log_zip/
uname -a > /tmp/log_zip/uname
cp /version > /tmp/log_zip/version
ip route >> /tmp/log_zip/ip_route
ip rule list >> /tmp/log_zip/ip_route
for temp in `cat /etc/iproute2/rt_tables  | grep -v ^# | awk '{print $2 }'`
do
	echo "table $temp:" >> /tmp/log_zip/ip_route
	ip route list table $temp >> /tmp/log_zip/ip_route
	echo "end"
done

iptables -S >> /tmp/log_zip/iptables 
iptables -t nat -S >> /tmp/log_zip/iptables
cp /tmp/dhcp.lease /tmp/log_zip/dhcp.lease
iwconfig > /tmp/log_zip/wifi_info
iwpriv ra0 get_on_cli >> /tmp/log_zip/wifi_info
iwpriv rai0 get_on_cli >> /tmp/log_zip/wifi_info

ifconfig -a >> /tmp/log_zip/ifconfig
ip neigh >> /tmp/log_zip/ip_neigh
cp /proc/net/nf_conntrack /tmp/log_zip/nf_conntrack
netstat -an >> /tmp/log_zip/netstat
cp /tmp/.system_info_dynamic /tmp/.system_info_static /tmp/.mutilapn_info /tmp/.current_lan_list /tmp/log_zip/

