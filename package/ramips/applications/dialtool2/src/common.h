#ifndef _COMMON_H_
#define _COMMON_H_

#include "queue.h"

#define CMD_EXE_OK "\r\nOK\r\n"
#define CMD_EXE_ERROR "\r\nERROR\r\n"
#define CMD_RESULT_CME_ERROR "\r\n+CME ERROR:"
#define CMD_RESULT_CMS_ERROR "\r\n+CMS ERROR:"
//no carrier
#define CMD_RESULT_NO_CARRIER "\r\nNO CARRIER\r\n"

//CGDATA ^DPPPI CONNECT
#define CMD_RESULT_CONNECT "\r\nCONNECT\r\n"

#define CMD_RESULT_CPIN "\r\n+CPIN:"
#define CMD_RESULT_CPIN_READY "READY"
#define CMD_RESULT_CPIN_NEED_PIN "SIM PIN"
#define CMD_RESULT_CPIN_NEED_PUK "SIM PUK"

//+QCPIN: READY
#define CMD_RESULT_QCPIN "\r\n+QCPIN:"
#define CMD_RESULT_QCPIN_NEED_PIN "UIM PIN"
#define CMD_RESULT_QCPIN_NEED_PUK "UIM PUK"

#define PARMS_SIZE 128
#define CMD_EXE_STATUS_OK 0
#define CMD_EXE_STATUS_ERROR 1
#define CMD_EXE_STATUS_UNKNOWN -1
#define FALSE 0
#define TRUE 1

#define MODULE_ABNORMAL_FILE "/tmp/module_abnormal_file"
#define MODULE_ERROR_FILE "/mnt/module_error"
#define PLMN_SELECTION_MODE_AUTOMATIC 0
#define PLMN_SELECTION_MODE_MANUAL 1

#define TZ_VENDOR_P500_ID			0x2020
#define TZ_PRODUCT_P500V1			0x2031
#define TZ_PRODUCT_P500V2			0x2032
#define TZ_PRODUCT_P500V3			0x2033

#define TZ_VENDOR_EC20_ID			0x05c6
#define TZ_PRODUCT_EC20				0x9215

#define TZ_VENDOR_SIM7100_ID			0x05c6
#define TZ_PRODUCT_SIM7100			0x9001

#define TZ_VENDOR_SIM7000C          		0x1e0e
#define TZ_PRODUCT_SIM7000C          		0x9001

#define TZ_VENNOR_AR9500      			0x2df3
#define TZ_PRODUCT_AR9500     			0x6b3d

#define TZ_VENNOR_LT10      			0x1ab7
#define TZ_PRODUCT_LT10     			0x1761

#if 0

#ifdef P61_DIALTOOL_FLAG
	    #define VENDORID_9341_FILE	"/sys/bus/usb/devices/1-1.1/idVendor"
	    #define PRODUCTID_9341_FILE	"/sys/bus/usb/devices/1-1.1/idProduct"
#else
	    #define VENDORID_9341_FILE	"/sys/bus/usb/devices/1-1.4/idVendor"
	    #define PRODUCTID_9341_FILE	"/sys/bus/usb/devices/1-1.4/idProduct"
#endif
#endif
    #define VENDORID_MT7621_FILE	"/sys/devices/platform/xhci-hcd/usb1/1-1/idVendor"
    #define PRODUCTID_MT7621_FILE	"/sys/devices/platform/xhci-hcd/usb1/1-1/idProduct"

#define UDHCPC_PIDFILE_PATH "/tmp/.udhcpc.pid"
#define SYSTEM_INFO_STATIC		"/tmp/.system_info_static"
#define SYSTEM_INFO_DYNAMIC		"/tmp/.system_info_dynamic"
#define DIAL_INDICATOR			"/tmp/.dialtool_indicator"


#define DIAL_STATUS_INDICATOR	"/proc/tozed/ind"
#define USB_SERIAL_STATUS		"/proc/tozed/usbserialind"  //0:right,1:error
#define MANUAL_DISCONNECT		"/tmp/.disconnect_from_lte_network_immediately"
#define RSSI_DISPLAY				"/tmp/.rssi_value_ind"

#define GPS_IS_ENABLE 			"/tmp/.gps_is_enable"



#define ARRAY_MEMBER_COUNT(array_name)  (sizeof(array_name)/sizeof(array_name[0]))
#define A_CMP_B_ACCORDING_B( A,B ) ( strlen( B ) && !strncmp( A,B,strlen( B ) ) )


typedef struct                                                                                                                                                                                                 
{
	char Name[64];
	char Value[64];
}PARMS;

//this function can be used to init the function
//typedef void (*init_proc_func)(char* pin,char* puk,char* apn,char* newpin,char* recipient_number,char* ipstack,int lte_module_mode,int lte_module_priority);
typedef void (*proc_func)(int* proc_state);

typedef void (*reporter_handle_func)(char* buffer);

/*
 *TRUE:continue
 *FALSE	:exit
 */
typedef void  (*atsend)(int);

typedef void proc_boardtype(void);


typedef struct
{
	//different module have different init way
	proc_func init_proc_func;
	//different module need use different dial process,it will designed a state machine
	proc_func dial_proc_func;
	//different hardware may  have different deamon
	proc_func deamon_proc_func;
	
	//report data handle fucntion
//	report_handler report_porc_func;
	//different module have different at cmd,so we need different sendat func
	atsend sendat;
	reporter_handle_func reporter_handle_func;
	proc_func timer_proc_func;
}DialProc;


typedef struct Module_Process
{
	const char *module_name;
	const char *hardware_type;
	char flag_file[128];			//for web display	
	proc_boardtype *init_boardtype;
	proc_boardtype *update_boardtype;	
	DialProc *process_functions;
}MP;

typedef struct PVID
{
	int idVendor;
	int idProduct;
}IDPV;




typedef struct Module_Detail_Info
{
	IDPV  module_serial;
	char  dev_name[16];
	void (* get_moduleinfo)(struct Module_Detail_Info *  p) ;
	char manufacturer[128];
	char hardver[128];
	char softver[128];
	char imei[32];
	char driver_name[64];
	char network_card_name[16];
}MDI;


typedef void (* Get_ModuleInfo)(MDI* p);


typedef struct web_real_mode_comparision_table
{
	int web_mode;
	int real_mode;
}WRMCT;

typedef struct mode_comparision_table
{
	const char *country;
	const char *operator_str;
	const char *operator_num;
	WRMCT web_mode_table[5];//4g only,4g priority, 3g only,3g prioty,2g only
}MCT;



extern PARMS cfg[PARMS_SIZE];

#define m_free(X) {free(X); (X) = NULL;}


extern void* strip_head_tail_space(void *str);
extern int separate_config(char *str,char pointer,PARMS* ptr);
extern void write_str_file(char *filename,char *str,char *format);
extern int rm_file(const char *file_name);
extern void fill_config(char *filename ,PARMS* ptr);
extern void get_config_specified(PARMS*ptr,int len,char *name,char *value);
extern int util_send_cmd(int dev_handle,char *cmd,int* exception_dealwith);
extern void separate_str(char* str,char const *sep,char** ret);
extern void* m_malloc_two(int m,int n);
extern void free_two(char** ptr,int m,int n);
extern void * m_malloc(unsigned int  size) ;
extern int refresh_timer(int sec);
extern int  get_value(char* file_path,int base);
extern int getpid_by_pidfile(char* pidfile_path);
extern void num_asciistr_to_decimalstr(char * input,char * output);
extern int cmd_echo(char* str,const char *file_name);
extern int check_file_exist(char *file_path);
extern int cmd_touch(const char *file_name);
#endif
