#!/bin/sh

status_24g_wifi=
status_58g_wifi=
status_br_wifi=


restore_all_status()
{
	status_24g_wifi=fail
	status_58g_wifi=fail
	status_br_wifi=fail
}



check_wifi_24g()
{
	SSID_24G_EXIST=`iwconfig ra0 | grep ESSID`
	if [ "${SSID_24G_EXIST}" = "" ]; then
		status_24g_wifi=fail
		echo "SSID_24G_EXIST fail"
		return
	fi

	ACCESS_POINT_24G_EXIST=`iwconfig ra0 | grep "Access Point" | awk '{print $5}'`
	if [ "${ACCESS_POINT_24G_EXIST}" = "" ]; then
                status_24g_wifi=fail
		echo "ACCESS_POINT_24G_EXIST fail"
		return
        fi

	status_24g_wifi=ok
}


check_wifi_58g()
{
	SSID_58G_EXIST=`iwconfig rai0 | grep ESSID`
	if [ "${SSID_58G_EXIST}" = "" ]; then
		status_58g_wifi=fail
		echo "SSID_58G_EXIST fail"
		return
	fi

	ACCESS_POINT_58G_EXIST=`iwconfig rai0 | grep "Access Point" | awk '{print $5}'`
	if [ "${ACCESS_POINT_58G_EXIST}" = "" ]; then
                status_58g_wifi=fail
		echo "ACCESS_POINT_58G_EXIST fail"
		return
        fi

	status_58g_wifi=ok

}


check_br()
{
	ra0_exist=`brctl show | grep ra0`
	rai0_exist=`brctl show | grep rai0`
	ra1_exist=`brctl show | grep ra1`
	rai1_exist=`brctl show | grep rai1`
	ra2_exist=`brctl show | grep ra2`
	rai2_exist=`brctl show | grep rai2`

	if [ "${ra0_exist}" != "" -a "${rai0_exist}" != "" -a $ra1_exist"" != "" -a $ra2_exist"" != "" -a $rai1_exist"" != "" -a $rai2_exist"" != "" ]; then
		status_br_wifi=ok
	else
		status_br_wifi=fail
	fi
}



sleep 60

if [ ! -f /root/reboot_count ]; then
        echo 0 > /root/reboot_count
fi

reboot_num=`cat /root/reboot_count`

wait_num=0

while [ 1 ]; 
do
	restore_all_status

        check_wifi_24g
        check_wifi_58g
        check_br

    ping -I bmwan0 -w 5 114.114.114.114
    status_4g=$?

    ping -I bmwan1 -w 5 114.114.114.114
    status_4g_1=$?

    ping -I bmwan2 -w 5 114.114.114.114
    status_4g_2=$?

    ping6 -w 5 2400:da00::6666
    status_4g_v6=$?

	if [ "${status_58g_wifi}" = "ok" -a "${status_24g_wifi}" = "ok" -a "$status_br_wifi" = "ok"  -a $status_4g"" == "0"  -a $status_4g_1"" == "0"  -a $status_4g_2"" == "0" -a $status_4g_v6"" == "0" ]; then
		last_reboot_num=`expr ${reboot_num} + 1`
		echo ${last_reboot_num} > /root/reboot_count

		if [ $wait_num -gt 3 ];then
			echo "TIMEOUT" >> /root/reboot_timeout
		fi

		sleep 10
		reboot
	fi

	echo "wait_num=" ${wait_num}

	if [ "${wait_num}" = "3" ]; then
		iwconfig > /root/record.${reboot_num}.log
		brctl show >> /root/record.${reboot_num}.log
		logread >> /root/record.${reboot_num}.log

		echo "status_58g_wifi=" ${status_58g_wifi} >> /root/record.${reboot_num}.log
		echo "status_24g_wifi=" ${status_24g_wifi} >> /root/record.${reboot_num}.log
		echo "status_br_wifi=" ${status_br_wifi} >> /root/record.${reboot_num}.log
		echo "status_4g=" ${status_4g} >> /root/record.${reboot_num}.log
		echo "status_4g_1=" ${status_4g_1} >> /root/record.${reboot_num}.log
		echo "status_4g_2=" ${status_4g_2} >> /root/record.${reboot_num}.log
		echo "status_4g_v6=" ${status_4g_v6} >> /root/record.${reboot_num}.log
	fi

	wait_num=`expr ${wait_num} + 1`
	sleep 10
done


