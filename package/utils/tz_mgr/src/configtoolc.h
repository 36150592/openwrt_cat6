#ifndef _CONFIGTOOLC_H_
#define _CONFIGTOOLC_H_

//#define GEN_RANDOM_WEB_LOGIN_PWD

//send to server
#define ETHERNET_FRAME_TYPE_TO_SERVER 0x9886
//send to client
#define ETHERNET_FRAME_TYPE_TO_CLIENT 0x9887


#define RC_SYSTEM_CONFIG_FILE "/tmp/rc.system.conf"
#define UPLINK_CONNECTION_IS_OK_TMP_FILE "/tmp/.uplink_connection_is_ok_tmp_file"
#define UPLINK_CONNECTION_IS_OK_TMP_FILE "/tmp/.uplink_connection_is_ok_tmp_file"
#define NEED_TO_REBOOT_TEMP_FILE "/tmp/.need_to_reboot"
#define NEED_TO_RESTORE_SETTINGS_TEMP_FILE "/tmp/.need_to_restore_settings"
#define ALL_MACS_TMP_FILE "/tmp/.all_macs_tmp_file"

#define infostruct_set_field_defined( infostruct_pointer,field_magic_number ) { ( infostruct_pointer )->defined_fields[ field_magic_number>>5 ]=( infostruct_pointer )->defined_fields[ field_magic_number>>5 ]|( 1<<( field_magic_number&0x1f ) ); };

#endif
