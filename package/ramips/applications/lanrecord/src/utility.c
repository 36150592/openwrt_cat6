#include "include.h"


int read_memory(char *shellcmd, char *out, int size) 
{

	FILE *stream;
	char buffer[size];
    memset(buffer, 0, sizeof(buffer));

	stream = popen(shellcmd, "r");
	if(stream != NULL){
        fread(buffer, sizeof(char), sizeof(buffer), stream);
        pclose(stream);

		memcpy(out, buffer, strlen(buffer) + 1);

		return 0;
	} else {
		out[0] = '\0';
	}

	return -1;
}


int execute_cmd_with_length(char *shellcmd, char *out, int size)
{
	char cmd[512] = {0};
	sprintf(cmd, "%s%s", shellcmd, " 2>/tmp/error.log");

	return read_memory(cmd, out, size);
}


/*return: 0, success; -1, fail*/
int execute_cmd(char *shellcmd, char *out, int size) 
{
	return execute_cmd_with_length(shellcmd, out, size);
}

char *get_value_by_end(char *data, char *value, char *endString)
{
	char *pIndex = NULL;
	char *pTail = NULL;
	int i = 0;

	// 初始化为空字符串
	value[0] = '\0';
	if (data == NULL) {
		return NULL;
	}
	pIndex = data;
	while (*pIndex == ' ') {
		pIndex++;
		i++;
		if(i > 19){
			return pIndex;
		}
	}

	pTail = strstr(pIndex, endString);
	if (pTail == NULL) {
		return pIndex;
	}

	memcpy(value, pIndex, pTail - pIndex);
	value[pTail - pIndex] = '\0';

	return pTail;
}

int ll_write_file_data(char *fl, void *data, int size)
{
	T_FFS_FD fd;

	if(fl == NULL || data == NULL || size <= 0)
		return FALSE;
		
	fd = open(fl, O_CREAT|O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO);
	if (fd > 0)
	{
		write(fd, data, size);
		close(fd);
	}
	else
	{
		log_info("[lan_list_read] %s File Open Error %d!\n", fl, fd);
	}  

	system("sync");

	return TRUE;
}

unsigned short StringSplit(char *instr, char *tokken, LP_SPLIT_STRUCT lp_out_strs)
{
	char *st = instr;
	char *ed = NULL;
	int ind = 0;

	if(!instr || !instr[0])
		return 0;

	ed = strstr(st, tokken);
	while(ed != 0)
	{
		lp_out_strs[ind].lp_string = st;
		lp_out_strs[ind].length = ed - st;

		ind ++;
		st = ed + strlen(tokken);
		
		ed = strstr(st, tokken);
	}
	lp_out_strs[ind].lp_string = st;
	lp_out_strs[ind].length = strlen(st);

	return ind + 1;
}


int format_mac(char *pSrc, char *pDes)
{
	unsigned int i = 0, j = 0;
 	
	if (pSrc == NULL || pDes == NULL)
		return -1;

	if (strchr(pSrc, ':') != NULL)
	{
		strcpy(pDes, pSrc);
		return 0;
	}
	
	for (i = 0; i < strlen(pSrc); i++)
	{
		if (i > 0 && i%2 == 0)
			pDes[j++] = ':';
		pDes[j++] = pSrc[i];
	}

	return 0;
}


void util_strip_blank_of_string_end( char* str )
{
        int length;
        char* tmp_str;

        if( str == NULL )
        {
                return;
        }

        length=strlen( str );

        tmp_str=str+length-1;
        while( tmp_str >= str )
        {
                if( ( *tmp_str == ' ' )
                        || ( *tmp_str == '\t' )
                        || ( *tmp_str == '\r' )
                        || ( *tmp_str == '\n' )
                        )
                {
                        *tmp_str=0;
                        length-=1;
                        tmp_str=str+length-1;
                }
                else
                {
                        break;
                }
        }
}


void print_lan_list(char* table_name, LAN_LIST_T* lan_list)
{
#ifdef  DEBUG
	log_info("\n******************************  %s  ******************************\n", table_name);
	int i = 0;
	for(i = 0; i < lan_list->cnt; i++)
	{
		log_info("%s\t|\t", lan_list->list[i].mac);
		log_info("%s\t|\t", lan_list->list[i].ipaddr);
		log_info("%s\t|\t", lan_list->list[i].interface);
		log_info("%s\t|\t", lan_list->list[i].hostname);
		log_info("%s\t|\t", lan_list->list[i].expires);
		log_info("%s\t|\t", lan_list->list[i].ssid);
		log_info("%d\t|\t", lan_list->list[i].flow);
		log_info("\n");
	}
#endif
}

void print_history_list(char* table_name, MAC_FLOW_LIST* lan_list)
{
#ifdef  DEBUG
	log_info("\n******************************  %s  ******************************\n", table_name);
	int i = 0;
	for(i = 0; i < lan_list->cnt; i++)
	{
		log_info("%s\t|\t", lan_list->mac_flow_item[i].mac);
		log_info("%d\t|\t", lan_list->mac_flow_item[i].flow);
		log_info("\n");
	}
#endif
}


int cmd_file_exist(const char* file_name)
{
#ifdef WIN32
	{
		WIN32_FIND_DATA wfd;
		HANDLE hFind=FindFirstFile( file_name,&wfd );
		//need create directory
		if( hFind == INVALID_HANDLE_VALUE )
		{
			return FALSE;
		}
	}
#else
	if( access( file_name,R_OK ) )
	{
		return FALSE;
	}
#endif
	return TRUE;
}



