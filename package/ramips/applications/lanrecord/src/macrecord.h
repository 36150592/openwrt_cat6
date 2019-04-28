#ifndef __MACRECORD_H_
#define __MACRECORD_H_

#include "type.h"
/*
	function: get_current_lan_list
	target: to get the current lan list
	
	param1: current_lan_list
	
*/
void get_current_lan_list(LAN_LIST_T* current_lan_list );

void echo_lan_list_to_file(LAN_LIST_T* lan_list, char* filename);

void echo_history_list_to_file(LAN_LIST_T* lan_list, char* filename);

#endif