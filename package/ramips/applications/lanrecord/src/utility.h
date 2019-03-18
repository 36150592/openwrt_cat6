#ifndef __UTILITY_H_
#define __UTILITY_H_

extern int execute_cmd(char *shellcmd, char *out, int size);
extern char *get_value_by_end(char *data, char *value, char *endString);
extern int ll_write_file_data(char *fl, void *data, int size);
extern unsigned short StringSplit(char *instr, char *tokken, LP_SPLIT_STRUCT lp_out_strs);
extern int format_mac(char *pSrc, char *pDes);
extern void util_strip_blank_of_string_end( char* str );
extern void print_lan_list(char* table_name, LAN_LIST_T* lan_list);
extern int cmd_file_exist(const char* file_name);


#endif

