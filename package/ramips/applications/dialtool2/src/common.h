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

#define TZ_VERDOR_S21_ID			0x05c6
#define TZ_PRODUCT_S21V1			0x9025

#define TZ_VERDOR_BM916_ID			0x2020
#define TZ_PRODUCT_BM916_ID			0x2063


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

typedef enum
{
	Dial_State_Stop,		
	Dial_State_initialized,			//send AT for test AT is ok
	Dial_State_CMEE,			//enable result code +CME ERROR:<num>
	Dial_State_PLMN_LOCK,		//lock plmn 
	Dial_State_PLMN_LOCK_QUERY,
	Dial_State_PCI_LOCK,
	Dial_State_PCI_LOCK_QUERY,
	Dial_State_ICCID,			//query iccid
	Dial_State_BMIMEI,			//query module imei
	Dial_State_CPIN_QUERY,			//query card exit or not
	Dial_State_QCPIN_QUERY,			//query card exit or not  in evda or 1x mode	
	Dial_State_CGMM,			//query module hardware type
	Dial_State_QCGMM,		//query module hardware type
	Dial_State_CGMR,			//query module software version
	Dial_State_BMSWVER,		//query detail module softver info
	Dial_State_ATI,			//display info about module
	Dial_State_BMSIMTYPE,		//query SIM type
	Dial_State_CPIN_SET,		//set pin
	Dial_State_QCPIN_SET,		//set pin in 3gpp2 mode
	Dial_State_CIMI,			//query register IMSI
	Dial_State_QCIMI,			//query register IMSI in 3gpp2
	Dial_State_BMCPNCNT,	//query pin and puk try times
	Dial_State_CPUK_SET,
	Dial_State_CLCK_QUERY,
	Dial_State_QCLCK_QUERY,
	Dial_State_CLCK_ENABLE,
	Dial_State_CPWD,
	Dial_State_BMDATASTATUSEN,
	Dial_State_PSDIALIND,
	Dial_State_BMTCELLINFO,
	Dial_State_CFUN_ENABLE,
	Dial_State_CFUN_DISABLE,
	Dial_State_CFUN_QUERY,
	Dial_State_BMMODODR,
	Dial_State_BMMODODR_QUERY,
	Dial_State_CGDCONT,			//set 3gpp PDP context
	Dial_State_CGDCONT2,			//set 3gpp2 PDP context
	Dial_State_QCPDPP,			//ppp dial cmd
	Dial_State_BM3GPP2AUTHINFO,	//set 3gpp2 dial username and password
	Dial_State_BMBANDPREF_SUPPORT_BAND_QUERY,
	Dial_State_BMBANDPREF_QUERY,
	Dial_State_RECOVERY_BAND,
	Dial_State_BMBANDPREF,
	Dial_State_BMBANDPREF_RESTORE_DEFAULT,
	Dial_State_BMRAT,
	Dial_State_COPS_SET,
	Dial_State_COPS_OPER_NUM_SET,
	Dial_State_CPMS_SM,
	Dial_State_CMGL_SM,
	Dial_State_CPMS_ME,
	Dial_State_CMGL_ME,
	Dial_State_CMGD,				//delete SM
	Dial_State_CMGL,				//query SM 
	Dial_State_SYSINFO,
	Dial_State_COPS_QUERY,
	Dial_State_CSCS,
	Dial_State_CSCS_QUERY,
	Dial_State_CNMI,
	Dial_State_CGSMS,	
	Dial_State_CSCA_QUERY,
	Dial_State_CSCA_SET,
	Dial_State_CMGF,
	Dial_State_CMGF_DM,
	Dial_State_CMGS,	
	Dial_State_CMGS_DM,
	Dial_State_CGEQREQ,
	Dial_State_CGACT_ENABLE,
	Dial_State_CGCONTRDP,
	Dial_State_CGACT_QUERY,
	Dial_State_SIGNALIND,
	Dial_State_QCRMCALL,
	Dial_State_QCRMCALL_QUERY,
	Dial_State_QCRMCALL_DISCONNECT,
	Dial_State_QCRMCALL_V6,
	Dial_State_QCRMCALL_QUERY_V6,
	Dial_State_QCRMCALL_DISCONNECT_V6,
	Dial_State_QCRMCALL_V4,
	Dial_State_QCRMCALL_QUERY_V4,
	Dial_State_QCRMCALL_DISCONNECT_V4,
	Dial_State_QCRMCALL_V4V6,
	Dial_State_CSQ,
	Dial_State_CSQ_SET,
	Dial_State_HDRCSQ,
	Dial_State_CEREG,
	Dial_State_CEREG_QUERY,
	Dial_State_BMDATASTATUS,
	Dial_State_BMBAND,
	Dial_State_GPSINFO,
	Dial_State_GPSSTART,
}DialState_bm;



#if 0

#ifdef P61_DIALTOOL_FLAG
	    #define VENDORID_9341_FILE	"/sys/bus/usb/devices/1-1.1/idVendor"
	    #define PRODUCTID_9341_FILE	"/sys/bus/usb/devices/1-1.1/idProduct"
#else
	    #define VENDORID_9341_FILE	"/sys/bus/usb/devices/1-1.4/idVendor"
	    #define PRODUCTID_9341_FILE	"/sys/bus/usb/devices/1-1.4/idProduct"
#endif
#endif
#define VENDORID_MT7621_FILE_3_0	"/sys/devices/platform/xhci-hcd/usb2/2-1/idVendor"
#define PRODUCTID_MT7621_FILE_3_0	"/sys/devices/platform/xhci-hcd/usb2/2-1/idProduct"


#define VENDORID_MT7621_FILE_2_0	"/sys/devices/platform/xhci-hcd/usb1/1-2/idVendor"
#define PRODUCTID_MT7621_FILE_2_0	"/sys/devices/platform/xhci-hcd/usb1/1-2/idProduct"



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
extern int stop_timer();
extern int  get_value(char* file_path,int base);
extern int getpid_by_pidfile(char* pidfile_path);
extern void num_asciistr_to_decimalstr(char * input,char * output);
extern int cmd_echo(char* str,const char *file_name);
extern int check_file_exist(char *file_path);
extern int cmd_touch(const char *file_name);
extern int read_memory(char *shellcmd, char *out, int size);
#endif
