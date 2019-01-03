#ifndef _UPDATE_CONFIG_H
#define _UPDATE_CONFIG_H

#define CONFIG_SIZE 40960

enum {
	CONFIG_SUCCESS = 0,
	CONFIG_ENCRY_CONFIG_NO_EXIST = 1,//encry config no exist fail
	CONFIG_OPEN_ENCRY_CONFIG_FAIL = 2,//encry config fail
	CONFIG_READ_ENCRY_CONFIG_FAIL = 3,// read encry config fail
	CONFIG_CHECK_SUM_FAIL = 4, // check sum fail
	CONFIG_CREATE_CONFIG_FILE_FAIL = 5, //create config file fail
	CONFIG_WRITE_CONFIG_FILE_FAIL = 6, //write config file fail
	CONFIG_OPEN_EXPORT_FILE_FAIL = 7, // open export file fail
	CONFIG_OPEN_PARAM_FILE_FAIL = 8, // open param file fail
};

extern int decry_tzupdate_config_file(char* conf_file, char* export_file);

extern int export_file_to_param_file(char* export_file, char* target_file);

#endif