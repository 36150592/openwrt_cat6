#include "include.h"

#define CURRENT_LAN_LIST "/tmp/.current_lan_list"
#define HISTORY_LAN_LIST "/tmp/.history_lan_list"
#define RESTORE_LAN_LIST_SIGN "/tmp/.restore_lan_list"

int main()
{
	LAN_LIST_T current_lan_list;
	LAN_LIST_T history_lan_list;

	restore_quota();

	while(1)
	{
		int sleepTime = 10;
		//1. get the current mac list
		get_current_lan_list(&current_lan_list);
		print_lan_list("current lan list", &current_lan_list);
		
		//2. get the history record mac list
		form_history_lan_list(&current_lan_list, &history_lan_list);
		print_lan_list("history lan list", &history_lan_list);

		//3. change the quota rule
		set_quota_rule(&current_lan_list);
		
		//4. get the quota value
		get_quota_value(&current_lan_list, &history_lan_list);
		print_lan_list("current lan list with quota", &current_lan_list);
		print_lan_list("history lan list with quota", &history_lan_list);

		//5. write to file
		echo_lan_list_to_file(&current_lan_list, CURRENT_LAN_LIST);
		echo_lan_list_to_file(&history_lan_list, HISTORY_LAN_LIST);

		//sleep
		while(sleepTime--)
		{
			sleep(1);
			if (cmd_file_exist(RESTORE_LAN_LIST_SIGN) )
			{
				system("rm -rf "RESTORE_LAN_LIST_SIGN);
				memset(&history_lan_list, 0, sizeof(LAN_LIST_T));
				restore_quota();
				break;
			}
		}
	}
	
	return 0;
}
