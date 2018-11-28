#ifndef  _EC20_H_
#define _EC20_H_

#include "common.h"
//#include "boardtype_306A.h"
extern void ec20_init(int*  Dial_proc_state);
extern void ec20_dial(int* Dial_proc_state );
extern void ec20_sendat(int num);
extern void ec20_get_moduleinfo(MDI* p);
extern MDI ec20_moduleinfo;
extern void ec20_deamon(int* Dial_proc_state);
extern void ec20_report_handle(char* buffer);
extern DialProc process_ec20;
#endif