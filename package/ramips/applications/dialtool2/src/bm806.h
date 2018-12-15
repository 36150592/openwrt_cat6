#ifndef  _BM806_H_
#define _BM806_H_

#include "common.h"
#include "boardtype_306A.h"

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
