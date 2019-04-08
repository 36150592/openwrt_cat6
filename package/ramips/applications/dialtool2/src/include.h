#ifndef _INCLUDE_H_
#define _INCLUDE_H_

#define RECV_BUFF_SIZE 1024
#define SEND_BUFF_SIZE 128
#define TIMER_DELAY 20

typedef enum 
{
	DIAL_INIT,
	DIAL_DIAL,
	DIAL_DEAMON,
}DV1;

typedef struct GLOBAL_DIALTOOL
{
	int dev_handle;			//fd for sendat
	char buffer_send[SEND_BUFF_SIZE];		//store send at cmd
	char buffer_at_sponse[RECV_BUFF_SIZE];	//store respond of at sended
	struct Queue at_recv;					//queue to store response of module
	MP *boardtype_module_combine;
	DialProc *board_func_set;
	DV1 Dial_Lvl_1;		//section dial,init should DIAL_INIT
	int Dial_proc_state;		//it is a ptr,so init it in module type file
	int exe_status;
	
	/*if module reset,we should kill thread and recycle them,
		reload ehci_hcd,if not /dev/ttyUSB which using will not work
	*/
	int pthread_moniter_flag;
	int refresh_timer_flag;
	char network_card_name[16];
	char web_display_flag[128];
}GD;

typedef struct GLOBAL_SYSINFO
{
	char device_name[128];
	char sys_softver[64];
	char sys_hardware[64];
	char sys_sha[128];
	char build_time[128];
	MDI  module_info;			//store module info
}GSI;

typedef struct GLOBAL_INIT_PARAMS
{
	char enable_pin[8];
	char pin[128];
	char puk[128];
	char apn[128];
	char newpin[128];
	char ipstack[128];
	int network_mode_web;
	char ppp_auth_type[64];
	char ppp_username[64];
	char ppp_password[64];
	char lte_band_choose[64];
	char gw_band_choose[64];
	char tds_band_choose[64];
	int plmn_mode;
	char plmn[64];
	int plmn_act;
	char plmn_lock[64];
	char network_card_mtu[8];
	char lte_pci_lock[64];
	char lte_earfcn_lock[64];
	int signal_rssi_lvl[5];
	int signal_rsrp_lvl[5];
	char enable_auto_dial[8];
}GIP;

typedef struct  GLOBAL_DIAL_VARIABLES
{
	char iccid[64];
	char imsi[64];
	int sim_real_lock_pin;            //sim's pin is open: 1: on open:0
	int device_lock_pin;		//PIN lock flag,lock:1,unlock:0
	int device_lock_puk;		//PUK lock flag,lock:1,unlock:0
	int pin_qpin_flag;		//use pin or qcpin flag,pin:0,qcpin:1
	int is_sim_exist;		//sim card exist or not
	int reg_status;			//indicator 2g
	int greg_status;			//indicator 3g
	int ereg_status;			//indicator 4g
	int act;
	int roam_status;		//indicator sim roam or not
	int pin_left_times;			//pin try times left
	int puk_left_times;			//puk try times left
	int pinlock_enable_flag;		//PIN lock enable or not
	int evdo_cdma_flag;		//make sure whether network mode is evdo or cdma  or not,if 0,evdo_cdma_sysinfo is real ,else not
	int network_mode_bmrat;		//detail register network mode
	char network_mode_detail[64];		//network mode display by str
	int ipstack_registered;		//sometime we can not register ipstack we want
	int signal_rssi_level;			//rssi lvl
	char signal_rssi_value[8];		//rssi value
	char signal_rsrp[8];
	char signal_rsrq[8];
	char signal_sinr[8];
	int network_link;			//data link status ,come from bmdatastatus
	int module_dormant;			//module dormant
	char cell_id[64];
	char pci[64];
	char lac_id[64];
	char active_band[16];
	char active_channel[64];
	char earfcn[64];
	char enodebid[64];
	char tac[64];
	char rrc_status[16];
	int service_status;
	int ps_cs_region;
	char cqi[8];
	char mcs[8];
	
	char gw_all_band[64];
	char lte_all_band[64];
	char tds_all_band[64];
	char gw_lock_band[64];
	char lte_lock_band[64];
	char tds_lock_band[64];
	char operator_current[64];
	char operator_plmn[6];
	char band[20];
	char moduleType[20];
}GDV;

typedef struct CONFIG_PARAMS
{
	PARMS enable_pin;
	PARMS pin;
	PARMS puk;
	PARMS apn;
	PARMS network_mode_web;
	PARMS ppp_auth_type;
	PARMS ppp_username;
	PARMS ppp_password;
	PARMS lte_band_choose;
	PARMS gw_band_choose;
	PARMS tds_band_choose;
	PARMS plmn_mode;
	PARMS plmn;
	PARMS plmn_act;
	PARMS plmn_lock;
	PARMS lte_pci_lock;
	PARMS lte_earfcn_lock;
	PARMS ipstack;
	PARMS network_card_mtu;
	PARMS enable_auto_dial;
}CPS;

typedef struct CONFIG_SCAN_ATT
{
	PARMS scan_att_check_enable;
	PARMS scan_att_mcc_mnc;
	PARMS scan_att_apn_name;
}CSA;

extern struct Queue at_recv_buff;  //define at recv queue
extern GD global_dialtool;
extern GSI global_system_info;
extern GIP global_init_parms;
extern GDV global_dial_vars;;
extern CPS configs;
//extern MP* project_info_summary;

#endif