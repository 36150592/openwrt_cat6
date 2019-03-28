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

if [ ! -f /etc/reboot_count ]; then
        echo 0 > /etc/reboot_count
fi

reboot_num=`cat /etc/reboot_count`

wait_num=0

while [ 1 ]; 
do
	restore_all_status

        check_wifi_24g
        check_wifi_58g
        check_br

    ping -w 5 www.baidu.com
    status_4g=$?

	echo "status_58g_wifi=" ${status_58g_wifi}
	echo "status_24g_wifi=" ${status_24g_wifi}
	echo "status_br_wifi=" ${status_br_wifi}
	echo "status_4g=" ${status_4g}

	if [ "${status_58g_wifi}" = "ok" -a "${status_24g_wifi}" = "ok" -a "$status_br_wifi" = "ok"  -a $status_4g"" == "0" ]; then
		last_reboot_num=`expr ${reboot_num} + 1`
		echo ${last_reboot_num} > /etc/reboot_count

		if [ $wait_num -gt 3 ];then
			echo "TIMEOUT" >> /etc/reboot_timeout
		fi

		sleep 10
		reboot
	fi

	echo "wait_num=" ${wait_num}

	if [ "${wait_num}" = "3" ]; then
		iwconfig > /etc/record.${reboot_num}.log
		brctl show >> /etc/record.${reboot_num}.log
		logread >> /etc/record.${reboot_num}.log
	fi

	wait_num=`expr ${wait_num} + 1`
	sleep 10
done


