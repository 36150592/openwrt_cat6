#!/bin/bash


UPDATE_FILE_PATH=./
#echo $UPDATE_FILE_PATH
UPDATE_FILE_NAME="s21_v0.05.bin"
FILE_UPDATE_VERSION='33308931d21e0e07ad9aad6daaf15a0b2fbc0484'
UPDATE_FILE_NAME_TEST1="s21_v0.05-1.bin"
UPDATE_FILE_NAME_TEST2="s21_v0.05-2.bin"
FILE_UPDATE_VERSION_1='33308931d21e0e07ad9aad6daaf15a0b2fbc0484'
FILE_UPDATE_VERSION_2='dd3784f1cb98a77a281e4e5bf415239402c7840b'
#echo $filename
TARGET_IP=192.168.2.1
count=1

session_id="eca8df00588997f4d8b04c60270140b15550569951555057118"
password="21232f297a57a5a743894a0e4a801fc3"
login_name="admin"
method="POST"
echo "cat /version" > ./cat_cmd
while [ 1 ]
	do
	echo "-------- cpe upgrade test count:$count --------"

	if [ $UPDATE_FILE_NAME"" == $UPDATE_FILE_NAME_TEST1"" ];then
		UPDATE_FILE_NAME=$UPDATE_FILE_NAME_TEST2
		FILE_UPDATE_VERSION=$FILE_UPDATE_VERSION_2
	else
		UPDATE_FILE_NAME=$UPDATE_FILE_NAME_TEST1
		FILE_UPDATE_VERSION=$FILE_UPDATE_VERSION_1
	fi

	echo "FILE NAME:$UPDATE_FILE_NAME"
	echo "FILE VERSION:$FILE_UPDATE_VERSION"

	cmd="100"
	#登录
	echo "@@@@@@  start login"
	login_result1=`curl -H "Content-Type:application/json" -X POST --data "{\"cmd\":$cmd,\"username\":\"$login_name\",\"passwd\":\"$password\",\"sessionId\":\"${session_id}\"}" http://192.168.2.1/cgi-bin/lua.cgi 2>/dev/null`
    echo $login_result1	
    session_id=`echo $login_result1 | awk -F ',' '{print $3}' | sed s/\"//g  | cut -d':' -f 2`
    echo "session_id = $session_id"
	login_result2=`echo $login_result1 | grep "success" | wc -l`

	login_result3=`echo $login_result1 | grep "true" | wc -l`

	if [ $login_result2 == 1  -a $login_result3 == 1 ] ;
	then
		echo ">>>>>>>>  login success  <<<<<<<<"
	else
		echo "login extract fail"
		#exit $count
		sleep 180
		continue
	fi


	#上传文件
	cmd="5"
	echo "@@@@@@  start upload"
	#upload_result1=`curl -F "fp=@${UPDATE_FILE_PATH}/${UPDATE_FILE_NAME}" "http://${TARGET_IP}/cgi-bin/lua.cgi?cmd=${cmd}&method=${method}&language=EN&username=${login_name}&passwd=${password}&sessionId=${session_id}" 2>/dev/null`
    upload_result1=`curl -F "fp=@${UPDATE_FILE_PATH}/${UPDATE_FILE_NAME}" "http://${TARGET_IP}/cgi-bin/lua.cgi?cmd=${cmd}&method=${method}&language=EN&username=${login_name}&passwd=${password}"`
    
    echo $upload_result1
	result2=`echo $upload_result1 | grep "success" | wc -l`

	result3=`echo $upload_result1 | grep "true" | wc -l`

	if [  $result2 == 1  -a $result3 == 1 ] ;
	then
		echo ">>>>>>>>  upload success  <<<<<<<<"
	else
		echo "upload fail"
		exit $count
	fi

	#升级写文件
	cmd="106"
	echo "@@@@@@  start upgrade"
	upgrade_result1=`curl -m 2000 -H "Content-Type:application/json" -X POST --data "{\"cmd\":$cmd,\"fileName\":\"$UPDATE_FILE_NAME\",\"sessionId\":\"${session_id}\"}" http://192.168.2.1/cgi-bin/lua.cgi 2>/dev/null`
	
    echo $upgrade_result1
    
    upgrade_result2=`echo $upgrade_result1 | grep "md5sum success" | wc -l `

    if [ $upgrade_result2 == 1 ] ;
    then
        ping -c 3 -w 100 192.168.2.1
        if [ $? != 0 ];
        then
           echo "ping fail upgrade fail"
           echo "run upgrade $count"
           exit 3
        fi
    fi 

    temp_ver=""
    temp_ver=`sshpass -p 123456 ssh test@192.168.2.1 -p 10210  <>  ./cat_cmd | grep sha1 | awk -F'=' '{print $2}'`

    if [ $FILE_UPDATE_VERSION"" != $temp_ver"" ];then
    	echo "version check fail. upgrade fail"
    	exit 4
    fi
     
    [ "$1" = "1" ] && exit 0

	let "count++"
	#sleep 180
done
