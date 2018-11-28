#ifndef  _LT10_H_
#define _LT10_H_

#include "common.h"

typedef enum
{
	Lt10_State_initialized,			//send AT for test AT is ok
	Lt10_State_CMEE,			//enable result code +CME ERROR:<num>
	Lt10_State_DGSN_QUERY2,
	Lt10_State_DLTEM_QUERY,
	Lt10_State_DLTEM,
	Lt10_State_DLTEM_QUERY2,	//5
	Lt10_State_ROAMING_QUERY,
	Lt10_State_ROAMING_SET,
	Lt10_State_ROAMING_QUERY2,
	Lt10_State_BANDSETEX_TEST,
	Lt10_State_BANDSETEX_QUERY,	
	Lt10_State_DUSIMR_QUERY,
	Lt10_State_DUSIMR_REPORT,
	Lt10_State_ICCID,
	Lt10_State_CPIN,
	Lt10_State_CPIN_SET,		
	Lt10_State_CPNNUM,			
	Lt10_State_CIMI,
	Lt10_State_COPS,
	Lt10_State_DSTMEX,
	Lt10_State_CEMODE,			
	Lt10_State_DDPDN,			
	Lt10_State_CGDCONT,
	Lt10_State_CFUN_DISABLE,
	Lt10_State_CFUN_ENABLE,
	Lt10_State_CREG,				
	Lt10_State_CEREG,			
	Lt10_State_CGREG,
	Lt10_State_CREG_QUERY,
	Lt10_State_CEREG_QUERY,
	Lt10_State_CGREG_QUERY,
	Lt10_State_CGDATA,			
	Lt10_State_CGDATA_CHECK,
	Lt10_State_CSQ,
	Lt10_State_SYSINFO,
	Lt10_State_DQDATA_QUERY,
	Lt10_State_AT,
	
}DialState_lt10;

extern void lt10_init(int*  Dial_proc_state);
extern void lt10_dial(int* Dial_proc_state );
extern void lt10_sendat(int num);
extern void lt10_get_moduleinfo(MDI* p);
extern MDI lt10_moduleinfo;
extern void lt10_deamon(int* Dial_proc_state);
extern void lt10_report_handle(char* buffer);
extern DialProc process_lt10;
#endif
