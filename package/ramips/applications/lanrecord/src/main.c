#include "include.h"


int main()
{
	LAN_LIST_T current_lan_list;
	MAC_FLOW_LIST history_lan_list;
	log_info("Lanrecord V0.0.2\n");
	restore_quota();

	while(1)
	{
		int sleepTime = 10;
		//1. get the current mac list
		get_current_lan_list(&current_lan_list);
		print_lan_list("current lan list", &current_lan_list);
		
		
		//2. change the quota rule
		refresh_quota_rule(&current_lan_list, &history_lan_list);
		
		print_lan_list("current lan list with quota", &current_lan_list);
		print_history_list("history lan list with quota", &history_lan_list);

		//3. write to file
		echo_lan_list_to_file(&current_lan_list, CURRENT_LAN_LIST);
		echo_history_list_to_file(&history_lan_list, HISTORY_LAN_LIST);

		//sleep
		while(sleepTime--)
		{
			sleep(1);
			if (cmd_file_exist(RESTORE_LAN_LIST_SIGN) )
			{
				system("rm -rf "RESTORE_LAN_LIST_SIGN);
				system("rm -rf "HISTORY_LAN_LIST);
				memset(&history_lan_list, 0, sizeof(LAN_LIST_T));
				restore_quota();
				break;
			}
		}
		
	}
	
	return 0;
}
