#ifndef  _BM906_H_
#define _BM906_H_

#include "common.h"
#include "boardtype_306A.h"

/*
typedef struct modepair
{
	int mode_web;
	int mode_module;
	char* band_name;
}MPAIR;
*/


extern void bm906_init(int*  Dial_proc_state);
extern void bm906_dial(int* Dial_proc_state );
extern void bm906_sendat(int num);
extern void bm906_get_moduleinfo(MDI* p);
extern const MDI bm906_moduleinfo;
extern void bm906_deamon(int* Dial_proc_state);
extern void bm906_report_handle(char* buffer);
extern DialProc process_bm906;
#endif
