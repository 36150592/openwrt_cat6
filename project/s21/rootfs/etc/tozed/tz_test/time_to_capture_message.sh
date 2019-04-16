#!/bin/sh


capture_message()
{
	dmesg > /tmp/capture.log
	echo "=================================" >> /tmp/capture.log
	echo "=================================" >> /tmp/capture.log
	echo "=================================" >> /tmp/capture.log
	
	route -n >> /tmp/capture.log
	echo "=================================" >> /tmp/capture.log
	echo "=================================" >> /tmp/capture.log
	echo "=================================" >> /tmp/capture.log
	
	logread >> /tmp/capture.log
	echo "=================================" >> /tmp/capture.log
	echo "=================================" >> /tmp/capture.log
	echo "=================================" >> /tmp/capture.log

	brctl show >> /tmp/capture.log
	echo "=================================" >> /tmp/capture.log
	echo "=================================" >> /tmp/capture.log
	echo "=================================" >> /tmp/capture.log

		
}



while [ 1 ]
do
	cur_time=`cat /proc/uptime | awk -F '.' '{print $1}'`
	echo ${cur_time}
	if [ ${cur_time} -gt 60 ]; then
		capture_message
		exit
	fi
	sleep 3
done