#ifndef __QUOTA_H_
#define __QUOTA_H_

extern void restore_quota();
extern void set_quota_rule(LAN_LIST_T* current_lan_list);
extern void get_quota_value(LAN_LIST_T* current_lan_list, LAN_LIST_T* history_lan_list);

#endif

