#ifndef  _BM916_H_
#define _BM916_H_

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


extern void bm916_init(int*  Dial_proc_state);
extern void bm916_dial(int* Dial_proc_state );
extern void bm916_sendat(int num);
extern void bm916_get_moduleinfo(MDI* p);
extern const MDI bm916_moduleinfo;
extern void bm916_deamon(int* Dial_proc_state);
extern void bm916_report_handle(char* buffer);
extern DialProc process_bm916;
#endif
