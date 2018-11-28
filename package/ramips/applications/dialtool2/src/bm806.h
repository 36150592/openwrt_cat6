#ifndef  _BM806_H_
#define _BM806_H_

#include "common.h"
#include "boardtype_306A.h"


typedef enum
{
	Dial_State_initialized,			//send AT for test AT is ok
	Dial_State_CMEE,			//enable result code +CME ERROR:<num>
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
	Dial_State_CSQ,
	Dial_State_CSQ_SET,
	Dial_State_HDRCSQ,
	Dial_State_CEREG,
	Dial_State_CEREG_QUERY,
	Dial_State_BMDATASTATUS,
	Dial_State_BMBAND,
	Dial_State_GPSINFO,
	Dial_State_GPSSTART,
}DialState_bm806;

typedef struct modepair
{
	int mode_web;
	int mode_module;
	char* band_name;
}MPAIR;


//extern struct ASBM806 AT_SET_BM806;
extern void bm806_init(int*  Dial_proc_state);
extern void bm806_dial(int* Dial_proc_state );
extern void bm806_sendat(int num);
extern void bm806_get_moduleinfo(MDI* p);
extern const MDI bm806_moduleinfo;
extern void bm806_deamon(int* Dial_proc_state);
extern void bm806_report_handle(char* buffer);
extern DialProc process_bm806;
#endif
