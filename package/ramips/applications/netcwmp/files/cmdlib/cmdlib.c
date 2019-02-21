#include "cmdlib.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

//抽象的频段名称和对应的值的对应关系
typedef struct
{
	int band_value;
	char* band_name;
}BandNamePair;

//检查指定的模块是否被使用
typedef int (* is_one_module_is_used)( void );
//用于对当前所在的频段的返回值进行解析的函数
typedef int (* parse_current_used_band)( char* current_used_band_result,char* dest_bit_map );
//用于对查询所有支持的频段的AT命令的返回值进行解析的函数
typedef int (* parse_all_supported_band)( char* all_supported_band_result,char* dest_bit_map );
//用于对查询模块当前支持的频段的AT命令的返回值进行解析的函数
typedef int (* parse_current_supported_band)( char* current_supported_band_result,char* dest_bit_map );
//获取锁频的AT命令字符串
typedef int (* at_to_lock_specified_band)( char* all_supported_bands,char* current_supported_band_result,char* dest_at_cmd );
//查询指定的频段名称对应的值,失败的情况下返回-1
static int util_get_band_value_by_name_ex( char* band_name,BandNamePair pairs[],int pairs_count );
//查询频段对应的名称
static char* util_get_band_name_by_value_ex( int band_value,BandNamePair all_band_name_pairs[],int pairs_count );
//将指定的位转换为标准的位信息
static int util_convert_bit_defintion_to_standard_definition( char* original_bit_map,BandNamePair pairs[],int pairs_count,BigValue* dest_big_value );
//将标准的位定义转换为模块特定的位定义
static int util_convert_standard_bit_defintion_to_module_definition( char* original_bit_map,BandNamePair pairs[],int pairs_count,BigValue* dest_big_value );
//查询指定的频段名称对应的值,失败的情况下返回-1
static int util_get_band_value_by_name( char* band_name );
//查询频段对应的名称
static char* util_get_band_name_by_value( int band_value );


typedef struct
{
	//检查指定的模块是否被使用
	is_one_module_is_used is_one_module_is_used_function;
	//用于查询当前所在的频段的AT命令
	char* at_to_query_current_used_band;
	//用于查询所有支持的频段的AT命令
	char* at_to_query_all_supported_band;
	//用于查询模块当前支持的频段的AT命令,这个可能和所有支持的频段不同
	char* at_to_query_current_supported_band;
	//用于对当前所在的频段的返回值进行解析的函数
	parse_current_used_band parse_current_used_band_function;
	//用于对查询所有支持的频段的AT命令的返回值进行解析的函数
	parse_all_supported_band parse_all_supported_band_function;
	//用于对查询模块当前支持的频段的AT命令的返回值进行解析的函数
	parse_current_supported_band parse_current_supported_band_function;
	//用于进行锁频的函数
	at_to_lock_specified_band at_to_lock_specified_band_function;
	//是否支持锁频
	int support_lock_band;
}LTEBandControlInfo;


//其中的band_value表示第几位的值为1,从0开始计算,band_name为对应的频段名称
static BandNamePair all_band_name_pairs[]=
{
	{ 1-1,"BAND1" },
	{ 2-1,"BAND2" },
	{ 3-1,"BAND3" },
	{ 4-1,"BAND4" },
	{ 5-1,"BAND5" },
	{ 6-1,"BAND6" },
	{ 7-1,"BAND7" },
	{ 8-1,"BAND8" },
	{ 9-1,"BAND9" },
	{ 10-1,"BAND10" },
	{ 11-1,"BAND11" },
	{ 12-1,"BAND12" },
	{ 13-1,"BAND13" },
	{ 14-1,"BAND14" },
	{ 15-1,"BAND15" },
	{ 16-1,"BAND16" },
	{ 17-1,"BAND17" },
	{ 18-1,"BAND18" },
	{ 19-1,"BAND19" },
	{ 20-1,"BAND20" },
	{ 21-1,"BAND21" },
	{ 22-1,"BAND22" },
	{ 23-1,"BAND23" },
	{ 24-1,"BAND24" },
	{ 25-1,"BAND25" },
	{ 26-1,"BAND26" },
	{ 27-1,"BAND27" },
	{ 28-1,"BAND28" },
	{ 29-1,"BAND29" },
	{ 30-1,"BAND30" },
	{ 31-1,"BAND31" },
	{ 32-1,"BAND32" },
	{ 33-1,"BAND33" },
	{ 34-1,"BAND34" },
	{ 35-1,"BAND35" },
	{ 36-1,"BAND36" },
	{ 37-1,"BAND37" },
	{ 38-1,"BAND38" },
	{ 39-1,"BAND39" },
	{ 40-1,"BAND40" },
	{ 41-1,"BAND41" },
	{ 42-1,"BAND42" },
	{ 43-1,"BAND43" },

	{ 56+0,"TDS_BAND A" },
	{ 56+1,"TDS_BAND B" },
	{ 56+2,"TDS_BAND C" },
	{ 56+3,"TDS_BAND D" },
	{ 56+4,"TDS_BAND E" },
	{ 56+5,"TDS_BAND F" },

	{ 64+0,"WCDMA_I_IMT_2000" },
	{ 64+1,"WCDMA_II_PCS_1900" },
	{ 64+2,"WCDMA_III_1700" },
	{ 64+3,"WCDMA_IV_1700" },
	{ 64+4,"WCDMA_V_850" },
	{ 64+5,"WCDMA_VI_800" },
	{ 64+6,"WCDMA_VII_2600" },
	{ 64+7,"WCDMA_VIII_900" },
	{ 64+8,"WCDMA_IX_1700" },

	{ 64+14,"WCDMA/GSM Unknown 1" },
	{ 64+15,"WCDMA/GSM Unknown 2" },

	{ 80+0,"GSM_450" },
	{ 80+1,"GSM_480" },
	{ 80+2,"GSM_750" },
	{ 80+3,"GSM_850" },
	{ 80+4,"GSM_EGSM_900" },
	{ 80+5,"GSM_RGSM_900" },
	{ 80+6,"GSM_PGSM_900" },
	{ 80+7,"GSM_DCS_1800" },
	{ 80+8,"GSM_PCS_1900" },
};

//用于对当前所在的频段的返回值进行解析的函数
//+TZBAND: 38
static int util_l1761_parse_current_used_band( char* current_used_band_result,char* dest_bit_map )
{
	char* band_info=strstr( current_used_band_result,"+TZBAND:" );
	int used_band;

	if( band_info != NULL )
	{
		char buffer[ 64 ];
		band_info+=strlen( "+TZBAND:" );
		band_info=util_skip_all_spaces( band_info );
		used_band=atoi( band_info );

		snprintf(
				buffer
				,sizeof(buffer)-1
				,"%s%sBAND%d"
				,CURRENT_NETWORK_INFO_Frequency_Band
				,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
				,used_band
				);

		if( dest_bit_map != NULL )
		{
			strcpy( dest_bit_map,buffer );
		}

		return 0;
	}

	return -1;
}

int util_char2hex(char c);

char *util_get_next_arg_cmdlib(char  * argp,int is_first_parameter)
{
	int is_in_double_quote=FALSE;

	while(
			( (*argp > ' ') && (*argp != ',') )
			||( (*argp == ' ') && is_in_double_quote )
			||( is_first_parameter && ( (*argp == '\r') || (*argp == '\n') ) )
		)
	{
		if( *argp == '\"' )
		{
			is_in_double_quote=TRUE;
		}
		argp++;   // scan past current arg
	}
	while( (*argp == ' ') || (*argp == ',') )
	{
		if(*argp == ',')
		{
			argp++;   // scan past spaces
			break;
		}
		else
		{
			argp++;   // scan past spaces
		}
	}
	return argp;
}


void util_get_arg_from_string_cmdlib( char *bDest,char *cp,int bMaxLen )
{
	char *bPtr;
	int i=0;
	bPtr=(char *)cp;
	while((*bPtr!='\"') && (*bPtr!='\r') && (*bPtr!='\n') && (*bPtr!=',') && (i<bMaxLen) && (*bPtr!=0))
	{
		*(bDest+i)=*bPtr;
		i++;
		bPtr++;
	}
	*(bDest+i)=0;
}

//获取当前位信息的信息,成功返回0,失败返回-1
static int util_get_big_value_from_bits( char* bit_string,BigValue* big_value )
{
	int start_byte=0;
	int last_byte;
	int value;
	int bit_index=0;

	if( bit_string == NULL )
	{
		return -1;
	}

	if( big_value == NULL )
	{
		return -1;
	}

	memset( big_value,0,sizeof( BigValue ) );

	//将所有的字母转换为大写字母
	util_convert_all_chars_to_upper_case( bit_string );

	if( !strncmp( bit_string,"0X",strlen("0X") ) )
	{
		start_byte=2;
	}

	last_byte=strlen( bit_string )-1;

	while( last_byte >= start_byte )
	{
		int index;
		value=util_char2hex( bit_string[ last_byte ] );

		index=0;
		while( index < 4 )
		{
			if( value&( 1<<index ) )
			{
				big_value->all_bits_info[ bit_index/32 ]+=( 1<<( bit_index%32 ) );
			}
			bit_index++;
			index++;
		}

		last_byte--;
	}

	return 0;
}

//将BigValue类型转换为字符串
static int util_convert_BigValue_to_string( BigValue* big_value,char* bit_string )
{
	int index=MAX_BYTES_COUNT_FOR_BIT-1;

	if( bit_string == NULL )
	{
		return -1;
	}

	if( big_value == NULL )
	{
		return -1;
	}

	while( index >= 0  )
	{
		if( big_value->all_bits_info[ index ] )
		{
			break;
		}
		index--;
	}

	bit_string[0]=0;
	while( index >= 0  )
	{
		sprintf(
				bit_string+strlen( bit_string )
				,"%08x"
				,big_value->all_bits_info[ index ]
				);
		index--;
	}
	return 0;
}

//重置当前的LTE位定义
static void util_reset_lte_bits_for_BigValue( BigValue* big_value )
{
	int index=0;

	while( index < MAX_BYTES_COUNT_FOR_BIT )
	{
		int bit_index=0;
		while( bit_index < 32 )
		{
			if( big_value->all_bits_info[ index ] & ( 1<<bit_index ) )
			{
				int band_value=index*32+bit_index;
				char* band_name=util_get_band_name_by_value( band_value );
				if( band_name != NULL )
				{
					//为LTE频段
					if( IS_STRING_B_PREFIX_OF_A( band_name,"BAND" ) )
					{
						big_value->all_bits_info[ index ]=big_value->all_bits_info[ index ]&( ~( 1<<bit_index ) );
					}
				}

			}
			bit_index++;
		}
		index++;
	}
}

//合并LTE位的定义
static void util_merge_lte_bits_for_BigValue( BigValue* dest_big_value,BigValue* original_big_value )
{
	int index=0;

	while( index < MAX_BYTES_COUNT_FOR_BIT )
	{
		int bit_index=0;
		while( bit_index < 32 )
		{
			if( original_big_value->all_bits_info[ index ] & ( 1<<bit_index ) )
			{
				int band_value=index*32+bit_index;
				char* band_name=util_get_band_name_by_value( band_value );
				if( band_name != NULL )
				{
					//为LTE频段
					if( IS_STRING_B_PREFIX_OF_A( band_name,"BAND" ) )
					{
						dest_big_value->all_bits_info[ index ]=dest_big_value->all_bits_info[ index ]&( ~( 1<<bit_index ) );
						dest_big_value->all_bits_info[ index ]+=( 1<<bit_index );
					}
				}
			}
			bit_index++;
		}
		index++;
	}
}

//用于对查询所有支持的频段的AT命令的返回值进行解析的函数
//\r\n^BANDSETEX: 0,9A,2,0000000000000000,7,000000E000000000
//<act>,<band_bitmap>[,<act>,<band_bitmap>[,...<act>,<band_bitmap>]]
static int util_l1761_parse_all_supported_band( char* all_supported_band_result,char* dest_bit_map )
{
	static char tmp_buffer[ 64 ];
	char* next_arg=strstr( all_supported_band_result,"^BANDSETEX:" );
	int act;
	int index=0;

	if( next_arg == NULL )
	{
		return -1;
	}

	next_arg+=strlen( "^BANDSETEX:" );
	next_arg=util_skip_all_spaces( next_arg );

	while( TRUE )
	{
		//避免可能出现错误的数据引起的bug
		index+=1;
		if( index >= 5 )
		{
			goto END_PROC_BANDSETEX;
		}

		//act
		if( *next_arg == '\r')
		{
			goto END_PROC_BANDSETEX;
		}
		act=atoi( next_arg );
		next_arg=util_get_next_arg_cmdlib(next_arg,FALSE);
		if( *next_arg == '\r')
		{
			goto END_PROC_BANDSETEX;
		}
		//当前为LTE网络的锁频信息
		if( act == 7 )
		{
			//保存LTE锁频信息
			util_get_arg_from_string_cmdlib( tmp_buffer,next_arg,( int )sizeof( tmp_buffer )-1 );

			//将位信息转换为支持的字符串信息,以便于更直观的查看支持的锁频的频段
			util_get_all_supported_band_names( tmp_buffer,dest_bit_map );

			return 0;
		}

		//获取下一个act的信息
		next_arg=util_get_next_arg_cmdlib(next_arg,FALSE);
	}

	END_PROC_BANDSETEX:
		;

	return -1;
}

//获取锁频的AT命令字符串
static int util_l1761_at_to_lock_specified_band( char* all_supported_bands,char* current_supported_band_result,char* dest_at_cmd )
{
	BigValue big_value;

	util_get_big_value_from_bits( current_supported_band_result,&big_value );

	if( dest_at_cmd != NULL )
	{
		sprintf(
				dest_at_cmd
				,"AT^BANDSETEX=7,%08X%08X"
				,big_value.all_bits_info[1]
				,big_value.all_bits_info[0]
				);

		return 0;
	}

	return -1;
}

//用于对当前所在的频段的返回值进行解析的函数
//+BMBAND:1
static int util_p520a_parse_current_used_band( char* current_used_band_result,char* dest_bit_map )
{
	char* band_info=strstr( current_used_band_result,"+BMBAND:" );
	int used_band;

	if( band_info != NULL )
	{
		char buffer[ 64 ];
		band_info+=strlen( "+BMBAND:" );
		band_info=util_skip_all_spaces( band_info );
		used_band=atoi( band_info );

		snprintf(
				buffer
				,sizeof(buffer)-1
				,"%s%sBAND%d"
				,CURRENT_NETWORK_INFO_Frequency_Band
				,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
				,used_band
				);

		if( dest_bit_map != NULL )
		{
			strcpy( dest_bit_map,buffer );
		}

		return 0;
	}

	return -1;

}

static BandNamePair p520a_band_name_pairs_lte[]=
{
	{ 1-1,"BAND1" },
	{ 2-1,"BAND2" },
	{ 3-1,"BAND3" },
	{ 4-1,"BAND4" },
	{ 5-1,"BAND5" },
	{ 6-1,"BAND6" },
	{ 7-1,"BAND7" },
	{ 8-1,"BAND8" },
	{ 9-1,"BAND9" },
	{ 10-1,"BAND10" },
	{ 11-1,"BAND11" },
	{ 12-1,"BAND12" },
	{ 13-1,"BAND13" },
	{ 14-1,"BAND14" },
	{ 15-1,"BAND15" },
	{ 16-1,"BAND16" },
	{ 17-1,"BAND17" },
	{ 18-1,"BAND18" },
	{ 19-1,"BAND19" },
	{ 20-1,"BAND20" },
	{ 21-1,"BAND21" },
	{ 22-1,"BAND22" },
	{ 23-1,"BAND23" },
	{ 24-1,"BAND24" },
	{ 25-1,"BAND25" },
	{ 26-1,"BAND26" },
	{ 27-1,"BAND27" },
	{ 28-1,"BAND28" },
	{ 29-1,"BAND29" },
	{ 30-1,"BAND30" },
	{ 31-1,"BAND31" },
	{ 32-1,"BAND32" },
	{ 33-1,"BAND33" },
	{ 34-1,"BAND34" },
	{ 35-1,"BAND35" },
	{ 36-1,"BAND36" },
	{ 37-1,"BAND37" },
	{ 38-1,"BAND38" },
	{ 39-1,"BAND39" },
	{ 40-1,"BAND40" },
	{ 41-1,"BAND41" },
	{ 42-1,"BAND42" },
	{ 43-1,"BAND43" },
};

//p520a位和标准位之间的对照表,只列出不同的位对应的频段信息
static BandNamePair p520a_band_name_pairs_tdscdma[]=
{
	{ 0,"TDS_BAND A" },
	{ 1,"TDS_BAND B" },
	{ 2,"TDS_BAND C" },
	{ 3,"TDS_BAND D" },
	{ 4,"TDS_BAND E" },
	{ 5,"TDS_BAND F" },
};

static BandNamePair p520a_band_name_pairs_wcdma_gsm[]=
{
	{ 22+0,"WCDMA_I_IMT_2000" },
	{ 22+1,"WCDMA_II_PCS_1900" },
	{ 22+2,"WCDMA_III_1700" },
	{ 22+3,"WCDMA_IV_1700" },
	{ 22+4,"WCDMA_V_850" },
	{ 22+5,"WCDMA_VI_800" },
	{ 42+6,"WCDMA_VII_2600" },
	{ 42+7,"WCDMA_VIII_900" },
	{ 42+8,"WCDMA_IX_1700" },

	{ 16+0,"GSM_450" },
	{ 16+1,"GSM_480" },
	{ 16+2,"GSM_750" },
	{ 16+3,"GSM_850" },
	{ 8,"GSM_EGSM_900" },
	{ 20,"GSM_RGSM_900" },
	{ 9,"GSM_PGSM_900" },
	{ 7,"GSM_DCS_1800" },
	{ 21,"GSM_PCS_1900" },

	{ 0,"WCDMA/GSM Unknown 1" },
	{ 1,"WCDMA/GSM Unknown 2" },
};

//用于对查询所有支持的频段的AT命令的返回值进行解析的函数
//+BMBANDPREF: support band: <gw_bd>,<lte_bd>,<tds_bd>
//+BMBANDPREF: support band: 0x2000005E80380,0x1E0000800D7, 0x21
static int util_p520a_parse_all_supported_band( char* all_supported_band_result,char* dest_bit_map )
{
	static char tmp_buffer[ 64 ];
	char* next_arg=strstr( all_supported_band_result,"+BMBANDPREF: support band:" );
	int index=0;
	//最终生成的位信息
	BigValue dest_big_value;

	if( next_arg == NULL )
	{
		return -1;
	}

	memset( &dest_big_value,0,sizeof( BigValue ) );

	next_arg+=strlen( "+BMBANDPREF: support band:" );
	next_arg=util_skip_all_spaces( next_arg );

	while( TRUE )
	{
		//避免可能出现错误的数据引起的bug
		index+=1;
		if( index >= 5 )
		{
			goto END_PROC_BANDSETEX;
		}

		//<gw_bd> GSM,WCDMA,CDMA,EVDO相关的频段(16进制数,设置时不用加“0x”前缀)
		if( index == 1 )
		{
			//保存LTE锁频信息
			util_get_arg_from_string_cmdlib( tmp_buffer,next_arg,( int )sizeof( tmp_buffer )-1 );
			util_strip_blank_of_string_end( tmp_buffer );

			util_convert_bit_defintion_to_standard_definition( tmp_buffer
																,p520a_band_name_pairs_wcdma_gsm
																,ARRAY_ELEMENTS_COUNT( p520a_band_name_pairs_wcdma_gsm )
																,&dest_big_value
																);
		}
		else if( index == 2 )
		{
			//保存LTE锁频信息
			util_get_arg_from_string_cmdlib( tmp_buffer,next_arg,( int )sizeof( tmp_buffer )-1 );
			util_strip_blank_of_string_end( tmp_buffer );

			util_convert_bit_defintion_to_standard_definition( tmp_buffer
																,NULL
																,0
																,&dest_big_value
																);
		}
		else if( index == 3 )
		{
			//保存LTE锁频信息
			util_get_arg_from_string_cmdlib( tmp_buffer,next_arg,( int )sizeof( tmp_buffer )-1 );
			util_strip_blank_of_string_end( tmp_buffer );

			util_convert_bit_defintion_to_standard_definition( tmp_buffer
																,p520a_band_name_pairs_tdscdma
																,ARRAY_ELEMENTS_COUNT( p520a_band_name_pairs_tdscdma )
																,&dest_big_value
																);
		}

		//将位信息转换为支持的字符串信息,以便于更直观的查看支持的锁频的频段
		util_convert_BigValue_to_string( &dest_big_value,tmp_buffer );
		util_get_all_supported_band_names( tmp_buffer,dest_bit_map );

		if( index == 3 )
		{
			return 0;
		}

		//获取下一个act的信息
		next_arg=util_get_next_arg_cmdlib(next_arg,FALSE);
		next_arg=util_skip_all_spaces( next_arg );
	}

	END_PROC_BANDSETEX:
		;

	return -1;
}

//用于对查询所有支持的频段的AT命令的返回值进行解析的函数
//+BMBANDPREF: support band: <gw_bd>,<lte_bd>,<tds_bd>
//+BMBANDPREF: support band: 0x2000005E80380,0x1E0000800D7, 0x21
static int util_p520a_parse_current_supported_band( char* all_supported_band_result,char* dest_bit_map )
{
	static char tmp_buffer[ 64 ];
	char* next_arg=strstr( all_supported_band_result,"+BMBANDPREF:" );
	int index=0;
	//最终生成的位信息
	BigValue dest_big_value;

	if( next_arg == NULL )
	{
		return -1;
	}

	if( strstr( all_supported_band_result,"+BMBANDPREF: support band:" ) )
	{
		return -1;
	}

	memset( &dest_big_value,0,sizeof( BigValue ) );

	next_arg+=strlen( "+BMBANDPREF:" );
	next_arg=util_skip_all_spaces( next_arg );

	while( TRUE )
	{
		//避免可能出现错误的数据引起的bug
		index+=1;
		if( index >= 5 )
		{
			goto END_PROC_BANDSETEX;
		}

		//<gw_bd> GSM,WCDMA,CDMA,EVDO相关的频段(16进制数,设置时不用加“0x”前缀)
		if( index == 1 )
		{
			//保存LTE锁频信息
			util_get_arg_from_string_cmdlib( tmp_buffer,next_arg,( int )sizeof( tmp_buffer )-1 );
			util_strip_blank_of_string_end( tmp_buffer );

			util_convert_bit_defintion_to_standard_definition( tmp_buffer
																,p520a_band_name_pairs_wcdma_gsm
																,ARRAY_ELEMENTS_COUNT( p520a_band_name_pairs_wcdma_gsm )
																,&dest_big_value
																);
		}
		else if( index == 2 )
		{
			//保存LTE锁频信息
			util_get_arg_from_string_cmdlib( tmp_buffer,next_arg,( int )sizeof( tmp_buffer )-1 );
			util_strip_blank_of_string_end( tmp_buffer );

			util_convert_bit_defintion_to_standard_definition( tmp_buffer
																,NULL
																,0
																,&dest_big_value
																);
		}
		else if( index == 3 )
		{
			//保存LTE锁频信息
			util_get_arg_from_string_cmdlib( tmp_buffer,next_arg,( int )sizeof( tmp_buffer )-1 );
			util_strip_blank_of_string_end( tmp_buffer );

			util_convert_bit_defintion_to_standard_definition( tmp_buffer
																,p520a_band_name_pairs_tdscdma
																,ARRAY_ELEMENTS_COUNT( p520a_band_name_pairs_tdscdma )
																,&dest_big_value
																);
		}

		//将位信息转换为支持的字符串信息,以便于更直观的查看支持的锁频的频段
		util_convert_BigValue_to_string( &dest_big_value,tmp_buffer );
		util_get_all_supported_band_names( tmp_buffer,dest_bit_map );

		if( index == 3 )
		{
			return 0;
		}

		//获取下一个act的信息
		next_arg=util_get_next_arg_cmdlib(next_arg,FALSE);
		next_arg=util_skip_all_spaces( next_arg );
	}

	END_PROC_BANDSETEX:
		;

	return -1;
}

//获取锁频的AT命令字符串
static int util_p520a_at_to_lock_specified_band( char* all_supported_bands,char* current_supported_band_result,char* dest_at_cmd )
{
	BigValue big_value;
	BigValue big_value_lte;
	BigValue big_value_lte_for_module;
	BigValue big_value_wcdma_gsm_for_module;
	BigValue big_value_tdscdma_for_module;
	char tmp_buffer[ 128 ];

	util_get_big_value_from_bits( all_supported_bands,&big_value );
	util_get_big_value_from_bits( current_supported_band_result,&big_value_lte );
	util_reset_lte_bits_for_BigValue( &big_value );
	util_merge_lte_bits_for_BigValue( &big_value,&big_value_lte );
	//将位定义转换为字符串
	util_convert_BigValue_to_string( &big_value,tmp_buffer );

	//获取LTE相关的位定义
	util_convert_standard_bit_defintion_to_module_definition( tmp_buffer
																,p520a_band_name_pairs_lte
																,ARRAY_ELEMENTS_COUNT( p520a_band_name_pairs_lte )
																,&big_value_lte_for_module
																);
	//获取WCDMA，GSM相关的位定义
	util_convert_standard_bit_defintion_to_module_definition( tmp_buffer
																,p520a_band_name_pairs_wcdma_gsm
																,ARRAY_ELEMENTS_COUNT( p520a_band_name_pairs_wcdma_gsm )
																,&big_value_wcdma_gsm_for_module
																);

	//获取TDSCDMA相关的位定义
	util_convert_standard_bit_defintion_to_module_definition( tmp_buffer
																,p520a_band_name_pairs_tdscdma
																,ARRAY_ELEMENTS_COUNT( p520a_band_name_pairs_tdscdma )
																,&big_value_tdscdma_for_module
																);

	if( dest_at_cmd != NULL )
	{
		char lte_bitmap[ 64 ];
		char wcdma_gsm_bitmap[ 64 ];
		char tdscdma_bitmap[ 64 ];

		//将BigValue值转换为位定义
		util_convert_BigValue_to_string( &big_value_lte_for_module,lte_bitmap );
		util_convert_BigValue_to_string( &big_value_wcdma_gsm_for_module,wcdma_gsm_bitmap );
		util_convert_BigValue_to_string( &big_value_tdscdma_for_module,tdscdma_bitmap );

		sprintf(
				dest_at_cmd
				,"AT+BMBANDPREF=%s,%s,%s"
				,wcdma_gsm_bitmap
				,lte_bitmap
				,tdscdma_bitmap
				);

		return 0;
	}

	return -1;
}

//用于对当前所在的频段的返回值进行解析的函数
/*If camping on a lte cell:
+CPSI: <System Mode>,<Operation Mode>,<MCC>-<MNC>,<
TAC>,<SCellID>,<PCellID>,<Frequency Band>,<earfcn>,<dlb
w>,<ulbw>,<RSRQ>,<RSRP>,<RSSI>,<RSSNR>

<System Mode>
System mode, values: “NO SERVICE”, “GSM”, “WCDMA”, “LTE”, “TDS”...
If module in LIMITED SERVICE state and +CNLSA command is set to 1, the system mode will
display as “GSM-LIMITED”, “WCDMA-LIMITED”...

<Operation Mode>
UE operation mode, values: “Online”,“Offline”,“Factory Test Mode”,“Reset”, “Low Power Mode”.

<MCC>
Mobile Country Code (first part of the PLMN code)

<MNC>
Mobile Network Code (second part of the PLMN code)

<TAC>
Tracing Area Code

<Cell ID>
Service-cell ID.

<PCellID>
Physical Cell ID

<Frequency Band>
Frequency Band of active set

<earfcn>
E-UTRA absolute radio frequency channel number for searching LTE cells

<dlbw>
Transmission bandwidth configuration of the serving cell on the downlink

<ulbw>
Transmission bandwidth configuration of the serving cell on the uplink

<RSRQ>
Current reference signal receive quality as measured by L1.

<RSRP>
Current reference signal received power in -1/10 dBm. Available for LTE

<RSSNR>
Average reference signal signal-to-noise ratio of the serving cell
*/
//\r\n+CPSI: LTE,Online,460-00,0x27BE,46709250,208,EUTRAN-BAND40,38950,5,5,-37,-835,-602,8\r\n\r\nOK\r\n
static int util_sim7100_parse_current_used_band( char* current_used_band_result,char* dest_bit_map )
{
	char tmp_buffer[ 64 ];
	char system_mode[ 64 ];
	char* next_arg=strstr( current_used_band_result,"+CPSI:" );

	if( next_arg != NULL )
	{

		next_arg+=strlen( "+CPSI:" );
		next_arg=util_skip_all_spaces( next_arg );

		//去掉\r\n后边的部分
		{
			char* cr_lf=strstr( next_arg,"\r\n" );
			if( cr_lf != NULL )
			{
				*cr_lf=0;
			}
		}

		//<System Mode>
		//获取当前的工作模式
		util_get_arg_from_string_cmdlib( system_mode,next_arg,( int )sizeof( system_mode )-1 );
		util_strip_blank_of_string_end( system_mode );
		if( !IS_STRING_B_PREFIX_OF_A( system_mode,"NO SERVICE" ) )
		{
			if( strlen( dest_bit_map ) )
			{
				STRCAT_S( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			sprintf(
					dest_bit_map+strlen( dest_bit_map )
					,"%s%s%s"
					,CURRENT_NETWORK_INFO_System_Mode
					,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
					,system_mode
					);

			//<Operation Mode>
			next_arg=util_get_next_arg_cmdlib(next_arg,FALSE);
			if( *next_arg == '\r' )
			{
				return 0;
			}
			next_arg=util_skip_all_spaces( next_arg );
			util_get_arg_from_string_cmdlib( tmp_buffer,next_arg,( int )sizeof( tmp_buffer )-1 );
			util_strip_blank_of_string_end( tmp_buffer );
			if( strlen( dest_bit_map ) )
			{
				STRCAT_S( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			sprintf(
					dest_bit_map+strlen( dest_bit_map )
					,"%s%s%s"
					,CURRENT_NETWORK_INFO_Operation_Mode
					,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
					,tmp_buffer
					);

			//<MCC>-<MNC>,
			next_arg=util_get_next_arg_cmdlib(next_arg,FALSE);
			if( *next_arg == '\r' )
			{
				return 0;
			}
			next_arg=util_skip_all_spaces( next_arg );
			util_get_arg_from_string_cmdlib( tmp_buffer,next_arg,( int )sizeof( tmp_buffer )-1 );
			util_strip_blank_of_string_end( tmp_buffer );
			//去掉-
			util_remove_special_char( tmp_buffer,'-' );
			if( strlen( dest_bit_map ) )
			{
				STRCAT_S( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			sprintf(
					dest_bit_map+strlen( dest_bit_map )
					,"%s%s%s"
					,CURRENT_NETWORK_INFO_MCC_MNC
					,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
					,tmp_buffer
					);

			//<TAC>,
			next_arg=util_get_next_arg_cmdlib(next_arg,FALSE);
			if( *next_arg == '\r' )
			{
				return 0;
			}
			next_arg=util_skip_all_spaces( next_arg );
			util_get_arg_from_string_cmdlib( tmp_buffer,next_arg,( int )sizeof( tmp_buffer )-1 );
			util_strip_blank_of_string_end( tmp_buffer );
			util_convert_all_chars_to_upper_case( tmp_buffer );
			if( IS_STRING_B_PREFIX_OF_A( tmp_buffer,"0X" ) )
			{
				memmove( tmp_buffer,tmp_buffer+2,strlen( tmp_buffer+2 )+1 );
			}
			if( strlen( dest_bit_map ) )
			{
				STRCAT_S( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			sprintf(
					dest_bit_map+strlen( dest_bit_map )
					,"%s%s%s"
					,CURRENT_NETWORK_INFO_TAC_LAC
					,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
					,tmp_buffer
					);

			//<SCellID>
			next_arg=util_get_next_arg_cmdlib(next_arg,FALSE);
			if( *next_arg == '\r' )
			{
				return 0;
			}
			next_arg=util_skip_all_spaces( next_arg );
			util_get_arg_from_string_cmdlib( tmp_buffer,next_arg,( int )sizeof( tmp_buffer )-1 );
			util_strip_blank_of_string_end( tmp_buffer );
			util_convert_all_chars_to_upper_case( tmp_buffer );
			{
				unsigned int cellid=atoi( tmp_buffer );
				sprintf(
						tmp_buffer
						,"%X"
						,cellid
						);
			}
			if( strlen( dest_bit_map ) )
			{
				STRCAT_S( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			sprintf(
					dest_bit_map+strlen( dest_bit_map )
					,"%s%s%s"
					,CURRENT_NETWORK_INFO_Serving_CellID
					,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
					,tmp_buffer
					);

			//<Absolute RF Ch Num>
			//AFRCN for service-cell.
			if(
					IS_STRING_B_PREFIX_OF_A( system_mode,"GSM" )
					)
			{
				next_arg=util_get_next_arg_cmdlib(next_arg,FALSE);
				if( *next_arg == '\r' )
				{
					return 0;
				}
				next_arg=util_skip_all_spaces( next_arg );
				util_get_arg_from_string_cmdlib( tmp_buffer,next_arg,( int )sizeof( tmp_buffer )-1 );
				util_strip_blank_of_string_end( tmp_buffer );
				util_convert_all_chars_to_upper_case( tmp_buffer );
				if( strlen( dest_bit_map ) )
				{
					STRCAT_S( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
				}
				sprintf(
						dest_bit_map+strlen( dest_bit_map )
						,"%s%s%s"
						,CURRENT_NETWORK_INFO_EARFCN
						,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
						,tmp_buffer
						);
			}
			//<PCellID>
			else if(
				IS_STRING_B_PREFIX_OF_A( system_mode,"LTE" )
				)
			{
				next_arg=util_get_next_arg_cmdlib(next_arg,FALSE);
				if( *next_arg == '\r' )
				{
					return 0;
				}
				next_arg=util_skip_all_spaces( next_arg );
				util_get_arg_from_string_cmdlib( tmp_buffer,next_arg,( int )sizeof( tmp_buffer )-1 );
				util_strip_blank_of_string_end( tmp_buffer );
				util_convert_all_chars_to_upper_case( tmp_buffer );
				if( strlen( dest_bit_map ) )
				{
					STRCAT_S( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
				}
				sprintf(
						dest_bit_map+strlen( dest_bit_map )
						,"%s%s%s"
						,CURRENT_NETWORK_INFO_Physical_CellID
						,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
						,tmp_buffer
						);
			}


			if(
				!IS_STRING_B_PREFIX_OF_A( system_mode,"GSM" )
				)
			{
				//<Frequency Band>
				next_arg=util_get_next_arg_cmdlib(next_arg,FALSE);
				if( *next_arg == '\r' )
				{
					return 0;
				}
				next_arg=util_skip_all_spaces( next_arg );
				util_get_arg_from_string_cmdlib( tmp_buffer,next_arg,( int )sizeof( tmp_buffer )-1 );
				util_strip_blank_of_string_end( tmp_buffer );
				util_convert_all_chars_to_upper_case( tmp_buffer );
				if( IS_STRING_B_PREFIX_OF_A( tmp_buffer,"EUTRAN-" ) )
				{
					memmove( tmp_buffer,tmp_buffer+strlen( "EUTRAN-" ),strlen( tmp_buffer+strlen( "EUTRAN-" ) )+1 );
				}
				if( strlen( dest_bit_map ) )
				{
					STRCAT_S( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
				}
				sprintf(
						dest_bit_map+strlen( dest_bit_map )
						,"%s%s%s"
						,CURRENT_NETWORK_INFO_Frequency_Band
						,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
						,tmp_buffer
						);
			}

			if(
				IS_STRING_B_PREFIX_OF_A( system_mode,"LTE" )
				)
			{
				//<earfcn>
				next_arg=util_get_next_arg_cmdlib(next_arg,FALSE);
				if( *next_arg == '\r' )
				{
					return 0;
				}
				next_arg=util_skip_all_spaces( next_arg );
				util_get_arg_from_string_cmdlib( tmp_buffer,next_arg,( int )sizeof( tmp_buffer )-1 );
				util_strip_blank_of_string_end( tmp_buffer );
				util_convert_all_chars_to_upper_case( tmp_buffer );
				if( strlen( dest_bit_map ) )
				{
					STRCAT_S( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
				}
				sprintf(
						dest_bit_map+strlen( dest_bit_map )
						,"%s%s%s"
						,CURRENT_NETWORK_INFO_EARFCN
						,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
						,tmp_buffer
						);
			}
		}

		return 0;
	}

	return -1;

}

/*
罗马数字
基本字符				I	V	X	L	C	D	M
相应的阿拉伯数字表示为	1	5	10	50	100	500	1000

相同的数字连写、所表示的数等于这些数字相加得到的数、如：Ⅲ=3；
小的数字在大的数字的右边、所表示的数等于这些数字相加得到的数、 如：Ⅷ=8、Ⅻ=12；
小的数字、（限于 Ⅰ、X 和 C）在大的数字的左边、所表示的数等于大数减小数得到的数、如：Ⅳ=4、Ⅸ=9；
正常使用时、连写的数字重复不得超过三次；
在一个数的上面画一条横线、表示这个数扩大 1000 倍。
*/
static BandNamePair b78_band_name_pairs_wcdma[]=
{
	{ 1,"WCDMA_I_IMT_2000" },
	{ 2,"WCDMA_II_PCS_1900" },
	{ 3,"WCDMA_III_1700" },
	{ 4,"WCDMA_VI_800" },
	{ 5,"WCDMA_IV_1700" },
	{ 6,"WCDMA_V_850" },
	{ 7,"WCDMA_VII_2600" },
	{ 9,"WCDMA_VIII_900" },
	{ 10,"WCDMA_IX_1700" },
};

static BandNamePair b78_band_name_pairs_wcdma_amt[]=
{
	{ 1,"WCDMA_I_IMT_2000" },
	{ 2,"WCDMA_II_PCS_1900" },
	{ 3,"WCDMA_III_1700" },
	{ 4,"WCDMA_VI_800" },
	{ 5,"WCDMA_IV_1700" },
	{ 6,"WCDMA_V_850" },
	{ 7,"WCDMA_VII_2600" },
	{ 8,"WCDMA_VIII_900" },
	{ 9,"WCDMA_IX_1700" },
};

//获取ML7810/ML7820/B78/B53/B90当前所在的网络模式
/*
AT+ZBAND? +ZBAND: 3,41 OK
+ZBAND?

未驻留或驻留在 GSM 制式网络时
+ZBAND:<Act>
驻留在 TDSCDMA 制式网络时
+ZBAND:<Act>,<BAND1>,<BAND2>
驻留在 LTE 制式网络时
+ZBAND:<Act>,<BAND1>
驻留在 WCDMA 制式网络时
+ZBAND:<Act>,<BAND1>

命令描述
查询命令,用来获取当前网络制式信息和频段信息。
备注:
通过沟通,我们可以仅提供LTE、TD以及W制式下的频段信息。其中TD制式下需要上报主
频点和辅频点对于频段信息。对于中间态情况,协议栈可以根据自己的内部实现,进行频
段信息的上报,对于中间态上报信息没有明确要求,但要保证协议栈在稳态下上报的频段
信息准确就可以了。该查询AT命令仅提供给上层用于WIFI抗干扰处理。

取值说明
<Act>:
取值		含义
0		未驻留
1		驻留 GSM 网络
2		驻留 TDSCDMA 网络
3		驻留 LTE 网络
4		驻留 WCDMA 网络

<BAND1>:主频段信息,255 为无效值
<BAND2>:辅频段信息,255 为无效值

<BAND1>和<BAND2>参数的取值及含义如下:
若服务小区为 TDSCDMA 小区:
取值		含义
11		TddA
12		TddB
13		TddC
14		TddD
15		TddE
16		TddF

若服务小区为 LTE 小区:
取值		含义
1		LTEBand1
2		LTEBand2
...		...
64		LTEBand64

若服务小区为 WCDMA 小区:
取值		含义
1 FDDI
2 FDDII
3 FDDIII
4 FDDVI
5 FDDIV
6 FDDV
7 FDDVII
8 FDDExtInd1
9 FDDVIII
10 FDDIX
11 FDDX
12 FDDXI
13 FDDXII
14 FDDXIII
15 FDDXIV
16 FDDXV
17 FDDXVI
18 FDDXVII
19 FDDXVIII
20 FDDXIX
21 FDDXX
22 FDDXXI
23 FDDXXII
24 FDDExtInd2
典型示例
请求:AT+ZBAND?<CR>
响应:<CR><LF>+ZBAND: 0<CR><LF>OK<CR><LF>
*/
static int util_b78_parse_current_used_band( char* current_used_band_result,char* dest_bit_map )
{
	int act;
	int band;
	char* band_name;
	char* next_arg=strstr( current_used_band_result,"+ZBAND:" );

	if( next_arg != NULL )
	{
		next_arg+=strlen( "+ZBAND:" );
		next_arg=util_skip_all_spaces( next_arg );

		//<Act>
		act=atoi( next_arg );
		//0		未驻留
		if( act == 0 )
		{
			sprintf(
					dest_bit_map+strlen( dest_bit_map )
					,"%s%sNot Registered"
					,CURRENT_NETWORK_INFO_Frequency_Band
					,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
					);
			return 0;
		}
		//3		驻留 LTE 网络
		else if( act == 3 )
		{
			next_arg=util_get_next_arg_cmdlib(next_arg,FALSE);
			if( *next_arg == '\r' )
			{
				return 0;
			}
			next_arg=util_skip_all_spaces( next_arg );

			//驻留在 LTE 制式网络时
			//+ZBAND:<Act>,<BAND1>
			band=atoi( next_arg );
			sprintf(
					dest_bit_map+strlen( dest_bit_map )
					,"%s%sBAND%d"
					,CURRENT_NETWORK_INFO_Frequency_Band
					,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
					,band
					);
			return 0;
		}
		//2		驻留 TDSCDMA 网络
		else if( act == 2 )
		{
			next_arg=util_get_next_arg_cmdlib(next_arg,FALSE);
			if( *next_arg == '\r' )
			{
				return 0;
			}
			next_arg=util_skip_all_spaces( next_arg );

			//驻留在 LTE 制式网络时
			//+ZBAND:<Act>,<BAND1>
			band=atoi( next_arg );
			band=band-11;
			band_name=util_get_band_name_by_value_ex(
													band
													,p520a_band_name_pairs_tdscdma
													,ARRAY_ELEMENTS_COUNT( p520a_band_name_pairs_tdscdma )
													);
			if( band_name != NULL )
			{
				//获取对应的频段名称
				sprintf(
						dest_bit_map+strlen( dest_bit_map )
						,"%s%s%s"
						,CURRENT_NETWORK_INFO_Frequency_Band
						,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
						,band_name
						);
			}


			next_arg=util_get_next_arg_cmdlib(next_arg,FALSE);
			if( *next_arg == '\r' )
			{
				return 0;
			}
			next_arg=util_skip_all_spaces( next_arg );
			//驻留在 LTE 制式网络时
			//+ZBAND:<Act>,<BAND1>
			band=atoi( next_arg );
			band=band-11;
			band_name=util_get_band_name_by_value_ex(
													band
													,p520a_band_name_pairs_tdscdma
													,ARRAY_ELEMENTS_COUNT( p520a_band_name_pairs_tdscdma )
													);


			if( band_name != NULL )
			{
				if( strlen( dest_bit_map ) )
				{
					strcat( dest_bit_map,"," );
					strcat( dest_bit_map,band_name );
				}
			}
			return 0;
		}
		//4		驻留 WCDMA 网络
		else if( act == 4 )
		{
			next_arg=util_get_next_arg_cmdlib(next_arg,FALSE);
			if( *next_arg == '\r' )
			{
				return 0;
			}
			next_arg=util_skip_all_spaces( next_arg );

			//驻留在 WCDMA 制式网络时
			//+ZBAND:<Act>,<BAND1>
			band=atoi( next_arg );
			band_name=util_get_band_name_by_value_ex(
													band
													,b78_band_name_pairs_wcdma
													,ARRAY_ELEMENTS_COUNT( b78_band_name_pairs_wcdma )
													);
			if( band_name != NULL )
			{
				//获取对应的频段名称
				sprintf(
						dest_bit_map+strlen( dest_bit_map )
						,"%s%s%s"
						,CURRENT_NETWORK_INFO_Frequency_Band
						,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
						,band_name
						);
			}
			return 0;
		}
		//1		驻留 GSM 网络
		else if( act == 1 )
		{
			sprintf(
					dest_bit_map+strlen( dest_bit_map )
					,"%s%s%s"
					,CURRENT_NETWORK_INFO_Frequency_Band
					,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
					,"2G"
					);
			return 0;
		}
	}
	return -1;
}

//查询B78模块所有支持的频段
static int util_b78_parse_all_supported_band( char* all_supported_band_result,char* dest_bit_map )
{
	BigValue dest_big_value;
	char tmp_buffer[256];
	char tmp_buffer2[256];
	int band;
	int band_value;
	char* band_name;

	/*
	校准 LTE 频段信息+ZLTEAMTBAND
	+ZLTEAMTBAND?
	+ZLTEAMTBAND:<band1-8>,<band9-16>,<band17-24>,<band25-32>,<band33-40>,<band41-48>,<band49-56>,<band57-64>

	命令描述
	用于检测校准的 LTE 频段信息,即模块硬件版本

	取值说明
	命令的 8 个参数取值范围均为 0-255 的字节类型整数。以<band1-8>为例,其各 bit 与对
	应频段映射关系如下:

	<band1-8>:
	bit8	bit7	bit6	bit5	bit4	bit3	bit2	bit1
	band8	band7	band6	band5	band4	band3	band2	band1

	当 Bit 值为 1 时表示支持其对应的频段,为 0 则表示不支持。参数 band9-16 的
	bit1-bit8 对应的频段为 band9-band16,以此类推,参数 band57-64 的 bit1-bit8 对应的
	频段为 band57-band64。

	典型示例
	请求:AT+ZLTEAMTBAND?
	响应:+ZLTEAMTBAND: 133,0,0,0,224,1,0,0
	*/
	//+ZLTEAMTBAND: 0,0,0,0,224,1,0,0 OK
	char* next_arg=strstr( all_supported_band_result,"+ZLTEAMTBAND:" );

	if( next_arg != NULL )
	{
		if( util_is_lte_mode_not_supported() )
		{
			goto END_PROC_ZLTEAMTBAND;
		}

		next_arg+=strlen( "+ZLTEAMTBAND:" );
		next_arg=util_skip_all_spaces( next_arg );

		memset( tmp_buffer,0,sizeof( tmp_buffer ) );
		util_get_band_pref_from_ml8710_set( tmp_buffer,next_arg );

		//将位定义转换为频段定义
		memset( tmp_buffer2,0,sizeof( tmp_buffer2 ) );
		util_get_all_supported_band_names( tmp_buffer,tmp_buffer2 );
		if( strlen( tmp_buffer2 ) )
		{
			if( strlen( dest_bit_map ) )
			{
				strcat( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			strcat( dest_bit_map,tmp_buffer2 );
		}
	}
	END_PROC_ZLTEAMTBAND:

	/*
	校准 GSM 频段信息+ZGSMAMTBAND
	+ZGSMAMTBAND?
	+ZGSMAMTBAND: < GSM_X >

	用于检测校准的 GSM 频段信息,即模块硬件版本
	取值说明
	< Gsm_x>:0-255 的字节类型整数,其各 bit 与对应频段映射关系如下:
	bit8		Bit7		Bit6-bit5	Bit4	bit3	bit2	bit1
	GSM1900		GSM1800		说明 1)		GSM850	GSM750	GSM480	GSM450

	当 Bit 值为 1 时表示支持其对应的频段,为 0 则表示不支持,Bit6-bit5 除外。
	说明 1)bit6~ bit5
	0 0 - 不支持 GSM900
	0 1 - 表示 GSM P900
	1 0 - 表示 GSM E900
	1 1 - 表示 GSM R900
	*/
	//+ZGSMAMTBAND: 224 OK
	next_arg=strstr( all_supported_band_result,"+ZGSMAMTBAND:" );

	if( next_arg != NULL )
	{
		if( util_is_gsm_mode_not_supported() )
		{
			goto END_PROC_ZGSMAMTBAND;
		}

		next_arg+=strlen( "+ZGSMAMTBAND:" );
		next_arg=util_skip_all_spaces( next_arg );

		//驻留在 LTE 制式网络时
		//+ZBAND:<Act>,<BAND1>
		band_value=atoi( next_arg );
		if( IS_ONE_BIT_SET( band_value,0 ) )
		{
			band_name="GSM_450";
			band=util_get_band_value_by_name( band_name );
			if( band != -1 )
			{
				if( strlen( dest_bit_map ) )
				{
					strcat( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
				}

				sprintf(
						dest_bit_map+strlen( dest_bit_map )
						,"%d%s%s"
						,band
						,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
						,band_name
						);
			}
		}

		if( IS_ONE_BIT_SET( band_value,1 ) )
		{
			band_name="GSM_480";
			band=util_get_band_value_by_name( band_name );
			if( band != -1 )
			{
				if( strlen( dest_bit_map ) )
				{
					strcat( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
				}

				sprintf(
						dest_bit_map+strlen( dest_bit_map )
						,"%d%s%s"
						,band
						,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
						,band_name
						);
			}
		}

		if( IS_ONE_BIT_SET( band_value,2 ) )
		{
			band_name="GSM_750";
			band=util_get_band_value_by_name( band_name );
			if( band != -1 )
			{
				if( strlen( dest_bit_map ) )
				{
					strcat( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
				}

				sprintf(
						dest_bit_map+strlen( dest_bit_map )
						,"%d%s%s"
						,band
						,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
						,band_name
						);
			}
		}

		if( IS_ONE_BIT_SET( band_value,3 ) )
		{
			band_name="GSM_850";
			band=util_get_band_value_by_name( band_name );
			if( band != -1 )
			{
				if( strlen( dest_bit_map ) )
				{
					strcat( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
				}

				sprintf(
						dest_bit_map+strlen( dest_bit_map )
						,"%d%s%s"
						,band
						,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
						,band_name
						);
			}
		}

		if( GET_2_BITS_VALUE( band_value,4 ) == 3 )
		{
			band_name="GSM_RGSM_900";
			band=util_get_band_value_by_name( band_name );
			if( band != -1 )
			{
				if( strlen( dest_bit_map ) )
				{
					strcat( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
				}

				sprintf(
						dest_bit_map+strlen( dest_bit_map )
						,"%d%s%s"
						,band
						,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
						,band_name
						);
			}
		}
		else if( GET_2_BITS_VALUE( band_value,4 ) == 1 )
		{
			band_name="GSM_PGSM_900";
			band=util_get_band_value_by_name( band_name );
			if( band != -1 )
			{
				if( strlen( dest_bit_map ) )
				{
					strcat( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
				}

				sprintf(
						dest_bit_map+strlen( dest_bit_map )
						,"%d%s%s"
						,band
						,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
						,band_name
						);
			}
		}
		else if( GET_2_BITS_VALUE( band_value,4 ) == 2 )
		{
			band_name="GSM_EGSM_900";
			band=util_get_band_value_by_name( band_name );
			if( band != -1 )
			{
				if( strlen( dest_bit_map ) )
				{
					strcat( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
				}

				sprintf(
						dest_bit_map+strlen( dest_bit_map )
						,"%d%s%s"
						,band
						,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
						,band_name
						);
			}
		}

		if( IS_ONE_BIT_SET( band_value,6 ) )
		{
			band_name="GSM_DCS_1800";
			band=util_get_band_value_by_name( band_name );
			if( band != -1 )
			{
				if( strlen( dest_bit_map ) )
				{
					strcat( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
				}

				sprintf(
						dest_bit_map+strlen( dest_bit_map )
						,"%d%s%s"
						,band
						,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
						,band_name
						);
			}
		}

		if( IS_ONE_BIT_SET( band_value,7 ) )
		{
			band_name="GSM_PCS_1900";
			band=util_get_band_value_by_name( band_name );
			if( band != -1 )
			{
				if( strlen( dest_bit_map ) )
				{
					strcat( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
				}

				sprintf(
						dest_bit_map+strlen( dest_bit_map )
						,"%d%s%s"
						,band
						,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
						,band_name
						);
			}
		}
	}
	END_PROC_ZGSMAMTBAND:

	/*
	校准 TDD 频段信息+ZTDDAMTBAND
	+ZTDDAMTBAND?
	+ZTDDAMTBAND: <TddA_H>,< TddI-P >
	命令描述
	用于检测校准的 TDS 频段信息,即模块硬件版本

	取值说明
	<TddA_H>:
	bit8		bit7		bit6		bit5		bit4		bit3		bit2		bit1
	<bTddH>		<bTddG>		<bTddF>		<bTddE>		<bTddD>		<bTddC>		<bTddB>		<bTddA>

	<TddI_P>:
	bit8		bit7		bit6		bit5		bit4		bit3		bit2		bit1
	<bTddP>		<bTddO>		<bTddN>		<bTddM>		<bTddL>		<bTddK>		<bTddJ>		<bTddI>

	当 Bit 值为 1 时表示支持其对应的频段,为 0 则表示不支持。

	典型示例
	请求:AT+ZTDDAMTBAND?
	响应:+ZTDDAMTBAND: 33,0
	*/
	//+ZTDDAMTBAND: 33,0 OK
	next_arg=strstr( all_supported_band_result,"+ZTDDAMTBAND:" );

	if( next_arg != NULL )
	{
		if( util_is_tdscdma_mode_not_supported() )
		{
			goto END_PROC_ZTDDAMTBAND;
		}

		next_arg+=strlen( "+ZTDDAMTBAND:" );
		next_arg=util_skip_all_spaces( next_arg );

		memset( tmp_buffer,0,sizeof( tmp_buffer ) );
		memset( &dest_big_value,0,sizeof( BigValue ) );
		util_get_arg_from_string_cmdlib( tmp_buffer,next_arg,( int )sizeof( tmp_buffer )-1 );
		util_strip_blank_of_string_end( tmp_buffer );
		band=atoi( tmp_buffer );
		sprintf(tmp_buffer,"%x",band);

		util_convert_bit_defintion_to_standard_definition( tmp_buffer
															,p520a_band_name_pairs_tdscdma
															,ARRAY_ELEMENTS_COUNT( p520a_band_name_pairs_tdscdma )
															,&dest_big_value
															);

		memset( tmp_buffer2,0,sizeof( tmp_buffer2 ) );
		//将位信息转换为支持的字符串信息,以便于更直观的查看支持的锁频的频段
		util_convert_BigValue_to_string( &dest_big_value,tmp_buffer );
		util_get_all_supported_band_names( tmp_buffer,tmp_buffer2 );
		if( strlen( tmp_buffer2 ) )
		{
			if( strlen( dest_bit_map ) )
			{
				strcat( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			strcat( dest_bit_map,tmp_buffer2 );
		}
	}
	END_PROC_ZTDDAMTBAND:

	next_arg=strstr( all_supported_band_result,"+ZFDDAMTBAND:" );

	if( next_arg != NULL )
	{
		if( util_is_wcdma_mode_not_supported() )
		{
			goto END_PROC_ZFDDAMTBAND;
		}

		next_arg+=strlen( "+ZFDDAMTBAND:" );
		next_arg=util_skip_all_spaces( next_arg );

		band=atoi( next_arg );

		//获取下一个参数
		next_arg=util_get_next_arg_cmdlib(next_arg,FALSE);
		if( *next_arg == '\r')
		{
			goto END_PROC_ZFDDAMTBAND;
		}
		next_arg=util_skip_all_spaces( next_arg );
		band=band+( atoi( next_arg )<<8 );
		band=band<<1;

		memset( tmp_buffer,0,sizeof( tmp_buffer ) );
		sprintf(tmp_buffer,"%x",band);

		memset( &dest_big_value,0,sizeof( BigValue ) );
		util_convert_bit_defintion_to_standard_definition( tmp_buffer
															,b78_band_name_pairs_wcdma_amt
															,ARRAY_ELEMENTS_COUNT( b78_band_name_pairs_wcdma_amt )
															,&dest_big_value
															);

		memset( tmp_buffer2,0,sizeof( tmp_buffer2 ) );
		//将位信息转换为支持的字符串信息,以便于更直观的查看支持的锁频的频段
		util_convert_BigValue_to_string( &dest_big_value,tmp_buffer );
		util_get_all_supported_band_names( tmp_buffer,tmp_buffer2 );
		if( strlen( tmp_buffer2 ) )
		{
			if( strlen( dest_bit_map ) )
			{
				strcat( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			strcat( dest_bit_map,tmp_buffer2 );
		}
	}
	END_PROC_ZFDDAMTBAND:

	if( strlen( dest_bit_map ) )
	{
		return 0;
	}

	return -1;
}

//查询B78模块当前支持的频段
static int util_b78_parse_current_supported_band( char* all_supported_band_result,char* dest_bit_map )
{
	char tmp_buffer[256];
	char tmp_buffer2[256];
	char* next_arg=strstr( all_supported_band_result,"+ZLTEBAND:" );

	if( next_arg != NULL )
	{
		next_arg+=strlen( "+ZLTEBAND:" );
		next_arg=util_skip_all_spaces( next_arg );

		memset( tmp_buffer,0,sizeof( tmp_buffer ) );
		util_get_band_pref_from_ml8710_set( tmp_buffer,next_arg );

		//将位定义转换为频段定义
		memset( tmp_buffer2,0,sizeof( tmp_buffer2 ) );
		util_get_all_supported_band_names( tmp_buffer,tmp_buffer2 );
		if( strlen( tmp_buffer2 ) )
		{
			if( strlen( dest_bit_map ) )
			{
				strcat( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			strcat( dest_bit_map,tmp_buffer2 );
		}
		return 0;
	}
	return -1;
}

//获取锁频的AT命令字符串
static int util_b78_at_to_lock_specified_band( char* all_supported_bands,char* current_supported_band_result,char* dest_at_cmd )
{
	if( strlen( current_supported_band_result ) )
	{
		char tmp_buffer[ 128 ];
		memset( tmp_buffer,0,sizeof(tmp_buffer) );
		util_convert_band_pref_to_ml8710_set( current_supported_band_result,tmp_buffer );
		sprintf(dest_at_cmd
				,"AT+ZLTEBAND=%s"
				,tmp_buffer
				);
		return 0;
	}

	return -1;
}


//查询ML7810/ML7820模块所有支持的频段
static int util_ml7810_parse_all_supported_band( char* all_supported_band_result,char* dest_bit_map )
{
	char tmp_buffer[256];
	char tmp_buffer2[256];
	char* next_arg=strstr( all_supported_band_result,"+BANDINFO:" );

	if( next_arg != NULL )
	{
		next_arg+=strlen( "+BANDINFO:" );
		next_arg=util_skip_all_spaces( next_arg );

		memset( tmp_buffer,0,sizeof( tmp_buffer ) );
		util_parse_bands_info( next_arg,FALSE,NULL,tmp_buffer );

		//将位定义转换为频段定义
		memset( tmp_buffer2,0,sizeof( tmp_buffer2 ) );
		util_get_all_supported_band_names( tmp_buffer,tmp_buffer2 );
		if( strlen( tmp_buffer2 ) )
		{
			if( strlen( dest_bit_map ) )
			{
				strcat( dest_bit_map,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			strcat( dest_bit_map,tmp_buffer2 );
		}
		return 0;
	}

	return -1;
}

static LTEBandControlInfo all_modules_support_band_lock[]=
{
	{
		is_module_l1761_used
		,"AT+TZBAND?"
		,"AT^BANDSETEX=?"
		,"AT^BANDSETEX?"
		,util_l1761_parse_current_used_band
		,util_l1761_parse_all_supported_band
		,util_l1761_parse_all_supported_band
		,util_l1761_at_to_lock_specified_band
		,TRUE
	},
	{
		is_module_bm806_used
		,"AT+BMBAND"
		,"AT+BMBANDPREF=?"
		,"AT+BMBANDPREF?"
		,util_p520a_parse_current_used_band
		,util_p520a_parse_all_supported_band
		,util_p520a_parse_current_supported_band
		,util_p520a_at_to_lock_specified_band
		,TRUE
	},
	{
		is_module_sim7100_used
		,"AT+CPSI?\r"
		,NULL
		,NULL
		,util_sim7100_parse_current_used_band
		,NULL
		,NULL
		,NULL
		,FALSE
	},
	{
		is_module_b78_used
		,"AT+ZBAND?"
		,"AT+ZLTEAMTBAND?\nAT+ZGSMAMTBAND?\nAT+ZTDDAMTBAND?\nAT+ZFDDAMTBAND?"
		,"AT+ZLTEBAND?"
		,util_b78_parse_current_used_band
		,util_b78_parse_all_supported_band
		,util_b78_parse_current_supported_band
		,util_b78_at_to_lock_specified_band
		,TRUE
	},
	{
		is_module_ml7810_used
		,"AT+ZBAND?"
		,"AT+BANDINFO?"
		,"AT+ZLTEBAND?"
		,util_b78_parse_current_used_band
		,util_ml7810_parse_all_supported_band
		,util_b78_parse_current_supported_band
		,util_b78_at_to_lock_specified_band
		,TRUE
	},
};

//获取查询当前所在的频段的AT命令
char* util_get_at_to_query_current_used_band( void )
{
	int index=0;
	while( index < ARRAY_ELEMENTS_COUNT( all_modules_support_band_lock ) )
	{
		if( all_modules_support_band_lock[index].is_one_module_is_used_function() )
		{
			return all_modules_support_band_lock[index].at_to_query_current_used_band;
		}
		index++;
	}

	return NULL;
}

//获取查询所有支持的频段的AT命令
char* util_get_at_to_query_all_supported_band( void )
{
	int index=0;
	while( index < ARRAY_ELEMENTS_COUNT( all_modules_support_band_lock ) )
	{
		if( all_modules_support_band_lock[index].is_one_module_is_used_function() )
		{
			return all_modules_support_band_lock[index].at_to_query_all_supported_band;
		}
		index++;
	}

	return NULL;
}

//获取查询模块当前支持的频段的AT命令,这个可能和所有支持的频段不同
char* util_get_at_to_query_current_supported_band( void )
{
	int index=0;
	while( index < ARRAY_ELEMENTS_COUNT( all_modules_support_band_lock ) )
	{
		if( all_modules_support_band_lock[index].is_one_module_is_used_function() )
		{
			return all_modules_support_band_lock[index].at_to_query_current_supported_band;
		}
		index++;
	}

	return NULL;
}

//用于对当前所在的频段的返回值进行解析的函数
int util_parse_current_used_band( char* current_used_band_result,char* dest_bit_map )
{
	int index=0;

	if( dest_bit_map != NULL )
	{
		*dest_bit_map=0;
	}

	while( index < ARRAY_ELEMENTS_COUNT( all_modules_support_band_lock ) )
	{
		if( all_modules_support_band_lock[index].is_one_module_is_used_function() )
		{
			if( all_modules_support_band_lock[index].parse_current_used_band_function != NULL )
			{
				return all_modules_support_band_lock[index].parse_current_used_band_function( current_used_band_result,dest_bit_map );
			}
		}
		index++;
	}
	return -1;
}

#ifndef BOOL_TRUE
#define BOOL_TRUE 1
#endif

#ifndef BOOL_FALSE
#define BOOL_FALSE 0
#endif

//检查是否所有的字符都是0
int util_all_char_be_zero_or_zero_length( char* one_string )
{
	if( one_string == NULL )
	{
		return BOOL_FALSE;
	}

	while( *one_string )
	{
		if( *one_string != '0' )
		{
			return BOOL_FALSE;
		}
		one_string++;
	}

	return BOOL_TRUE;
}

//用于对查询所有支持的频段的AT命令的返回值进行解析的函数
int util_parse_all_supported_band( char* all_supported_band_result,char* dest_bit_map )
{
	int index=0;

	if( dest_bit_map != NULL )
	{
		*dest_bit_map=0;
	}

	while( index < ARRAY_ELEMENTS_COUNT( all_modules_support_band_lock ) )
	{
		if( all_modules_support_band_lock[index].is_one_module_is_used_function() )
		{
			if( all_modules_support_band_lock[index].parse_all_supported_band_function != NULL )
			{
				return all_modules_support_band_lock[index].parse_all_supported_band_function( all_supported_band_result,dest_bit_map );
			}
		}
		index++;
	}
	return -1;
}
//用于对查询模块当前支持的频段的AT命令的返回值进行解析的函数
int util_parse_current_supported_band( char* current_supported_band_result,char* dest_bit_map )
{
	int index=0;

	if( dest_bit_map != NULL )
	{
		*dest_bit_map=0;
	}

	while( index < ARRAY_ELEMENTS_COUNT( all_modules_support_band_lock ) )
	{
		if( all_modules_support_band_lock[index].is_one_module_is_used_function() )
		{
			if( all_modules_support_band_lock[index].parse_current_supported_band_function != NULL )
			{
				return all_modules_support_band_lock[index].parse_current_supported_band_function( current_supported_band_result,dest_bit_map );
			}
		}
		index++;
	}
	return -1;
}

//用于进行锁频的函数
int util_get_at_to_lock_specified_band( char* all_supported_bands,char* current_supported_band_result,char* dest_at_cmd )
{
	int index=0;

	if( dest_at_cmd != NULL )
	{
		*dest_at_cmd=0;
	}

	if( strstr( all_supported_bands,"\r\n" ) )
	{
		if( dest_at_cmd != NULL )
		{
			*dest_at_cmd=0;
		}
		return -1;
	}

	if( strstr( current_supported_band_result,"\r\n" ) )
	{
		if( dest_at_cmd != NULL )
		{
			*dest_at_cmd=0;
		}
		return -1;
	}

	while( index < ARRAY_ELEMENTS_COUNT( all_modules_support_band_lock ) )
	{
		if( all_modules_support_band_lock[index].is_one_module_is_used_function() )
		{
			if( all_modules_support_band_lock[index].at_to_lock_specified_band_function != NULL )
			{
				return all_modules_support_band_lock[index].at_to_lock_specified_band_function( all_supported_bands,current_supported_band_result,dest_at_cmd );
			}
		}
		index++;
	}
	return -1;
}

//是否支持进行锁频,支持返回TRUE,否则返回FALSE,支持的情况下使用checkbox进行选择,不支持的情况下,指示列出当前所有支持的频段及当前支持的频段
int util_is_universal_lock_band_supported( void )
{
	int index=0;

	while( index < ARRAY_ELEMENTS_COUNT( all_modules_support_band_lock ) )
	{
		if( all_modules_support_band_lock[index].is_one_module_is_used_function() )
		{
			return all_modules_support_band_lock[index].support_lock_band;
		}
		index++;
	}
	return -1;
}

//查询频段对应的名称
char* util_get_band_name_by_value_ex( int band_value,BandNamePair all_band_name_pairs[],int pairs_count )
{
	int index=0;

	while( index < pairs_count )
	{
		if( all_band_name_pairs[ index ].band_value == band_value )
		{
			debug( "\r\nutil_get_band_name_by_value_ex:%d found,%s,%s\r\n",band_value,all_band_name_pairs[ index ].band_name,all_band_name_pairs[ 0 ].band_name );
			return all_band_name_pairs[ index ].band_name;
		}

		index++;
	}

	debug( "\r\nutil_get_band_name_by_value_ex:%d  not found,%s\r\n",band_value,all_band_name_pairs[ 0 ].band_name );

	return NULL;
}

//查询频段对应的名称
char* util_get_band_name_by_value( int band_value )
{
	return util_get_band_name_by_value_ex( band_value,all_band_name_pairs,ARRAY_ELEMENTS_COUNT( all_band_name_pairs ) );
}

//查询指定的频段名称对应的值,失败的情况下返回-1
int util_get_band_value_by_name_ex( char* band_name,BandNamePair pairs[],int pairs_count )
{
	int index=0;

	while( index < pairs_count )
	{
		if( !strcmp( pairs[ index ].band_name,band_name ) )
		{
			debug( "\r\nutil_get_band_value_by_name_ex:%s found,%d，%s\r\n",band_name,pairs[ index ].band_value,pairs[ 0 ].band_name );
			return pairs[ index ].band_value;
		}

		index++;
	}

	debug( "\r\nutil_get_band_value_by_name_ex:%s not found,%s\r\n",band_name,pairs[ 0 ].band_name );

	return -1;
}

//查询指定的频段名称对应的值,失败的情况下返回-1
int util_get_band_value_by_name( char* band_name )
{
	int band_value;

	band_value=util_get_band_value_by_name_ex( band_name,all_band_name_pairs,ARRAY_ELEMENTS_COUNT( all_band_name_pairs ) );
	if( band_value != -1 )
	{
		return band_value;
	}


	return -1;
}

//获取所有支持的频段的名称
int util_get_all_supported_band_names( char* all_band_map,char* band_names )
{
	BigValue big_value;

	if( all_band_map == NULL )
	{
		return -1;
	}

	if( band_names == NULL )
	{
		return -1;
	}

	band_names[0]=0;

	util_get_big_value_from_bits( all_band_map,&big_value );

	if( band_names != NULL )
	{
		int index=0;

		while( index < MAX_BYTES_COUNT_FOR_BIT )
		{
			int bit_index=0;
			while( bit_index < 32 )
			{
				if( big_value.all_bits_info[ index ] & ( 1<<bit_index ) )
				{
					int band_value=index*32+bit_index;
					char* band_name=util_get_band_name_by_value( band_value );
					if( band_name != NULL )
					{
						if( strlen( band_names ) )
						{
							strcat( band_names,CURRENT_NETWORK_INFO_SEPARATOR );
						}
						sprintf(
								band_names+strlen(band_names)
								,"%d@%s"
								,band_value
								,band_name
								);
					}

				}
				bit_index++;
			}
			index++;
		}

		return 0;
	}

	return -1;
}

//将指定的位转换为标准的位信息
int util_convert_bit_defintion_to_standard_definition( char* original_bit_map,BandNamePair pairs[],int pairs_count,BigValue* dest_big_value )
{
	BigValue big_value;
	int index=0;

	if( dest_big_value == NULL )
	{
		return -1;
	}

	util_get_big_value_from_bits( original_bit_map,&big_value );

	while( index < MAX_BYTES_COUNT_FOR_BIT )
	{
		int bit_index=0;
		while( bit_index < 32 )
		{
			if( big_value.all_bits_info[ index ] & ( 1<<bit_index ) )
			{
				int band_value=index*32+bit_index;
				char* band_name;
				if( pairs != NULL )
				{
					band_name=util_get_band_name_by_value_ex( band_value,pairs,pairs_count );
				}
				else
				{
					band_name=util_get_band_name_by_value( band_value );
				}

				if( band_name != NULL )
				{
					//将指定模块的位信息转换为标准的位信息
					int bit_value=util_get_band_value_by_name( band_name );
					dest_big_value->all_bits_info[ bit_value/32 ]+=( 1<<( bit_value%32 ) );
				}

			}
			bit_index++;
		}
		index++;
	}

	return 0;
}

//将标准的位定义转换为模块特定的位定义
int util_convert_standard_bit_defintion_to_module_definition( char* original_bit_map,BandNamePair pairs[],int pairs_count,BigValue* dest_big_value )
{
	BigValue big_value;
	int index=0;

	if( dest_big_value == NULL )
	{
		return -1;
	}

	memset( dest_big_value,0,sizeof( BigValue ) );

	util_get_big_value_from_bits( original_bit_map,&big_value );

	while( index < MAX_BYTES_COUNT_FOR_BIT )
	{
		int bit_index=0;
		while( bit_index < 32 )
		{
			if( big_value.all_bits_info[ index ] & ( 1<<bit_index ) )
			{
				int band_value=index*32+bit_index;
				char* band_name;
				band_name=util_get_band_name_by_value( band_value );

				//在指定的映射中查找指定的BAND NAME对应的位值
				if( band_name != NULL )
				{
					//将指定模块的位信息转换为标准的位信息
					int bit_value=util_get_band_value_by_name_ex( band_name,pairs,pairs_count );
					if( bit_value != -1 )
					{
						dest_big_value->all_bits_info[ bit_value/32 ]+=( 1<<( bit_value%32 ) );
					}
				}
			}
			bit_index++;
		}
		index++;
	}

	return 0;
}

//检查指定的模块是否被使用
typedef int (* get_real_supported_network_mode_for_this_module)( BandNamePair** supported_mode_info,int* mode_count );

typedef struct
{
	//检查指定的模块是否被使用
	is_one_module_is_used is_one_module_is_used_function;
	//特定模块支持的网络模式
	BandNamePair* supported_mode_info;
	//支持的网络模式的数量
	int mode_count;
	//这个是模块特定的网络模式
	int default_network_mode;
	//是否需要根据真正的模块类型来获取支持的网络模式
	int need_to_check_real_module_type;
	//使用如下的函数获取真正支持的网络模式
	get_real_supported_network_mode_for_this_module get_real_supported_network_mode_func;
}ModuleNetworkModeInfo;

//支持的所有网络优选模式
static BandNamePair all_preferred_network_mode_name_pairs[]=
{
	//0:在 4G/3G/2G 多模式下工作,4G模式优先
	{ 0,"4G/3G/2G Multi-mode" },
	//1:在 4G/3G 模式下工作(ZLT MF210 使用 ZM8620 情况下,该模式无效)
	{ 1,"Only 4G/3G" },
	//2:在 3G/2G 模式下工作
	{ 2,"Only 3G/2G" },
	//3:只在 4G 模式下工作
	{ 3,"Only 4G" },
	//4:只在 3G 模式下工作(ZLT MF210 使用 ZM8620 情况下,该模式有效)
	{ 4,"Only 3G" },
	{ 5,"Automatic(LTE>TDS>Other)" },
	{ 6,"Automatic(LTE>UMTS>Other)" },
	{ 7,"Automatic(LTE>TDS>UMTS>Other)" },
	{ 8,"Automatic(LTE>EVDO>CDMA>TDS>UMTS>GSM)" },
	{ 9,"Automatic(LTE>UMTS>TDS>Other)" },
	{ 10,"LTE+UMTS Only(4G+3G Only)" },
	{ 11,"LTE+EVDO Only(4G+3G Only)" },
	{ 12,"LTE+TDS Only(4G+3G Only)" },
	{ 13,"WCDMA/TDS-CDMA(3G Only)" },
	{ 14,"WCDMA Only(3G Only)" },
	{ 15,"EVDO Only(3G Only)" },
	{ 16,"TDS-CDMA Only(3G Only)" },
	{ 17,"UMTS+GSM Only(3G+2G Only)" },
	{ 18,"TDS+GSM Only(3G+2G Only)" },
	{ 19,"EVDO+CDMA Only(3G+2G Only)" },
	{ 20,"GSM+CDMA Only" },
	{ 21,"GSM Only" },
	{ 22,"CDMA Only" },
};

//SIM7100E支持的网络模式
static BandNamePair sim7100e_preferred_network_mode_name_pairs[]=
{
	{ 2,"4G/3G/2G Multi-mode" },
	{ 38,"Only 4G" },
	{ 14,"WCDMA Only(3G Only)" },
	//{ 59,"TDS-CDMA Only(3G Only)" },//只有SIM7100C支持TDSCDMA
	{ 13,"GSM Only" },
};

//L1761模块支持的网络模式
static BandNamePair l1761_preferred_network_mode_name_pairs[]=
{
	//0:在 4G/3G/2G 多模式下工作,4G模式优先
	{ 0,"4G/3G/2G Multi-mode" },
	//1:在 4G/3G 模式下工作(ZLT MF210 使用 ZM8620 情况下,该模式无效)
	{ 1,"Only 4G/3G" },
	//2:在 3G/2G 模式下工作
	{ 2,"Only 3G/2G" },
	//3:只在 4G 模式下工作
	{ 3,"Only 4G" },
};

//BM806U支持的网络模式
static BandNamePair bm806u_preferred_network_mode_name_pairs[]=
{
	{ 2,"4G/3G/2G Multi-mode" },
	{ 5,"Only 4G" },

	{ 17,"Automatic(LTE>UMTS>TDS>Other)" },

	{ 20,"LTE+UMTS Only(4G+3G Only)" },
	{ 9,"LTE+TDS Only(4G+3G Only)" },

	{ 7,"WCDMA/TDS-CDMA(3G Only)" },
	{ 1,"WCDMA Only(3G Only)" },
	{ 6,"TDS-CDMA Only(3G Only)" },


	{ 21,"UMTS+GSM Only(3G+2G Only)" },
	{ 8,"TDS+GSM Only(3G+2G Only)" },

	{ 14,"EVDO+CDMA Only(3G+2G Only)" },


};

//BM806C支持的网络模式
static BandNamePair bm806c_preferred_network_mode_name_pairs[]=
{
	{ 2,"4G/3G/2G Multi-mode" },
	{ 5,"Only 4G" },

	{ 17,"Automatic(LTE>UMTS>TDS>Other)" },
	{ 11,"Automatic(LTE>EVDO>CDMA>TDS>UMTS>GSM)" },

	{ 20,"LTE+UMTS Only(4G+3G Only)" },
	{ 13,"LTE+EVDO Only(4G+3G Only)" },
	{ 9,"LTE+TDS Only(4G+3G Only)" },

	{ 7,"WCDMA/TDS-CDMA(3G Only)" },
	{ 1,"WCDMA Only(3G Only)" },
	{ 6,"TDS-CDMA Only(3G Only)" },
	{ 10,"EVDO Only(3G Only)" },


	{ 21,"UMTS+GSM Only(3G+2G Only)" },
	{ 14,"EVDO+CDMA Only(3G+2G Only)" },
	{ 8,"TDS+GSM Only(3G+2G Only)" },

	{ 3,"GSM+CDMA Only" },
	{ 15,"CDMA Only" },
};

//BLS001模块支持的网络模式
static BandNamePair bls001_preferred_network_mode_name_pairs[]=
{
	//0:在 4G/3G/2G 多模式下工作,4G模式优先
	{ 0,"4G/3G/2G Multi-mode" },
	//1:在 4G/3G 模式下工作(ZLT MF210 使用 ZM8620 情况下,该模式无效)
	{ 1,"Only 4G/3G" },
	//2:在 3G/2G 模式下工作
	{ 2,"Only 3G/2G" },
	//3:只在 4G 模式下工作
	{ 3,"Only 4G" },
	//4:只在 3G 模式下工作(ZLT MF210 使用 ZM8620 情况下,该模式有效)
	{ 4,"Only 3G" },
};

static char read_module_type_buffer[1024];
#define BM806C_IS_USED "/tmp/.module_bm806c_is_used"
#define BM806U_IS_USED "/tmp/.module_bm806u_is_used"
#define MODULE_TYPE_AT_RESPONSE "/tmp/.check_module_type_at_cmd_response"

//返回获取LTE状态信息需要发送的AT命令
typedef char* (* at_to_query_lte_status)( void );
//对获取的LTE状态信息进行解析
typedef int (* parse_lte_status)( char* lte_status_response,char* lte_status_result );
//获取查询锁物理小区状态的AT命令
typedef char* (* at_to_query_lock_one_cell_status)( void );
//对获取到的锁物理小区查询的结果进行解析
typedef int (* parse_lock_one_cell_status)( char* lock_one_cell_response,int* locked_status,char* earfcn,char* physical_cell_id  );
//执行锁物理小区操作锁需要的AT命令
typedef int (* at_to_lock_one_cell)( int lock_or_unlock,char* earfcn,char* physical_cell_id,char* dest_at_command );

typedef struct
{
	//检查指定的模块是否被使用
	is_one_module_is_used is_one_module_is_used_function;
	//返回获取LTE状态信息需要发送的AT命令
	char* at_to_query_lte_status;
	//获取查询锁物理小区状态的AT命令
	char* at_to_query_lock_one_cell_status;
	//查询当前所在的频点和物理小区ID的AT命令
	char* at_to_query_earfcn_physical_cell_id;
	//对获取的LTE状态信息进行解析
	parse_lte_status parse_lte_status_function;
	//对获取到的锁物理小区查询的结果进行解析
	parse_lock_one_cell_status parse_lock_one_cell_status_function;
	//执行锁物理小区操作锁需要的AT命令
	at_to_lock_one_cell at_to_lock_one_cell_function;
	//对获取的EARFCN和物理小区ID信息进行解析
	parse_lte_status parse_earfcn_physical_function;
}LTEQueryStatusAndLockOneCell;

//根据模块类型判定是否支持的网络模式列表
int p520a_get_real_supported_network_mode( BandNamePair** supported_mode_info,int* mode_count )
{
	if( is_module_bm806_used() )
	{
		RECHECK_REAL_USED_MODULE_TYPE:
		if( cmd_file_exist( BM806C_IS_USED ) )
		{
			*supported_mode_info=bm806c_preferred_network_mode_name_pairs;
			*mode_count=ARRAY_ELEMENTS_COUNT( bm806c_preferred_network_mode_name_pairs );
			return 0;
		}
		else if( cmd_file_exist( BM806U_IS_USED ) )
		{
			*supported_mode_info=bm806u_preferred_network_mode_name_pairs;
			*mode_count=ARRAY_ELEMENTS_COUNT( bm806u_preferred_network_mode_name_pairs );
			return 0;
		}
		else if( cmd_file_exist( MODULE_TYPE_AT_RESPONSE ) )
		{
			cmd_cat( MODULE_TYPE_AT_RESPONSE,read_module_type_buffer,sizeof(read_module_type_buffer)-1 );
			if( strstr( read_module_type_buffer,"BM806C" ) )
			{
				cmd_touch( BM806C_IS_USED );
				goto RECHECK_REAL_USED_MODULE_TYPE;
			}
			else if( strstr( read_module_type_buffer,"BM806U" ) )
			{
				cmd_touch( BM806U_IS_USED );
				goto RECHECK_REAL_USED_MODULE_TYPE;
			}
			else
			{
				*supported_mode_info=bm806u_preferred_network_mode_name_pairs;
				*mode_count=ARRAY_ELEMENTS_COUNT( bm806u_preferred_network_mode_name_pairs );
				return 0;
			}
		}
		else
		{
			*supported_mode_info=bm806u_preferred_network_mode_name_pairs;
			*mode_count=ARRAY_ELEMENTS_COUNT( bm806u_preferred_network_mode_name_pairs );
			return 0;
		}
	}

	return -1;
}

static ModuleNetworkModeInfo network_mode_info_for_all_modules[]=
{
	{
		is_module_l1761_used,
		l1761_preferred_network_mode_name_pairs,
		ARRAY_ELEMENTS_COUNT( l1761_preferred_network_mode_name_pairs ),
		0
	},
	{
		is_module_sim7100_used,
		sim7100e_preferred_network_mode_name_pairs,
		ARRAY_ELEMENTS_COUNT( sim7100e_preferred_network_mode_name_pairs ),
		2
	},
	{
		is_module_bm806_used,
		bm806u_preferred_network_mode_name_pairs,
		ARRAY_ELEMENTS_COUNT( bm806u_preferred_network_mode_name_pairs ),
		2,
		TRUE,
		p520a_get_real_supported_network_mode
	},
	{
		is_module_bls001_used,
		bls001_preferred_network_mode_name_pairs,
		ARRAY_ELEMENTS_COUNT( bls001_preferred_network_mode_name_pairs ),
		0
	},
};

//获取当前模块支持的网络模式信息,成功返回0,失败返回-1,失败或者返回的模式为空的情况下使用默认模式
int util_get_supported_network_mode_for_this_module( char* supported_network_mode )
{
	int index;
	BandNamePair* supported_mode_info=NULL;
	int mode_count=0;

	if( supported_network_mode == NULL )
	{
		return -1;
	}
	else
	{
		*supported_network_mode=0;
	}

	index=0;
	while( index < ARRAY_ELEMENTS_COUNT( network_mode_info_for_all_modules ) )
	{
		ModuleNetworkModeInfo* mode_info=network_mode_info_for_all_modules+index;
		if( mode_info->is_one_module_is_used_function() )
		{
			if( mode_info->need_to_check_real_module_type && mode_info->get_real_supported_network_mode_func )
			{
				mode_info->get_real_supported_network_mode_func( &supported_mode_info,&mode_count );
			}
			else
			{
				supported_mode_info=mode_info->supported_mode_info;
				mode_count=mode_info->mode_count;
			}
			break;
		}
		index++;
	}

	if( supported_mode_info == NULL )
	{
		return 0;
	}

	if( !mode_count )
	{
		return 0;
	}

	index=0;
	while( index < mode_count )
	{
		char* mode_name=supported_mode_info[index].band_name;
		int mode_value=util_get_band_value_by_name_ex( mode_name
														,all_preferred_network_mode_name_pairs
														,ARRAY_ELEMENTS_COUNT( all_preferred_network_mode_name_pairs )
														);
		if( mode_value != -1 )
		{
			if( strlen( supported_network_mode ) )
			{
				strcat( supported_network_mode,CURRENT_NETWORK_INFO_SEPARATOR );
			}

			sprintf(
					supported_network_mode+strlen(supported_network_mode)
					,"%d%s%s"
					,mode_value
					,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
					,mode_name
					);
		}
		index++;
	}

	return 0;
}

//根据配置中设置的值,获取对应的实际应该设置的网络模式,这个值保存在TZ_LTE_MODULE_MODE中
int util_get_real_network_mode_from_settings( int network_mode )
{
	int index;
	BandNamePair* supported_mode_info=NULL;
	int mode_count=0;
	int default_value=-1;

	index=0;
	while( index < ARRAY_ELEMENTS_COUNT( network_mode_info_for_all_modules ) )
	{
		ModuleNetworkModeInfo* mode_info=network_mode_info_for_all_modules+index;
		if( mode_info->is_one_module_is_used_function() )
		{
			if( mode_info->need_to_check_real_module_type && mode_info->get_real_supported_network_mode_func )
			{
				mode_info->get_real_supported_network_mode_func( &supported_mode_info,&mode_count );
			}
			else
			{
				supported_mode_info=mode_info->supported_mode_info;
				mode_count=mode_info->mode_count;
			}
			default_value=mode_info->default_network_mode;
			break;
		}
		index++;
	}

	if( supported_mode_info == NULL )
	{
		return -1;
	}

	if( !mode_count )
	{
		return -1;
	}

	index=0;
	while( index < mode_count )
	{
		int module_mode_value=-1;
		char* mode_name=util_get_band_name_by_value_ex(
														network_mode
														,all_preferred_network_mode_name_pairs
														,ARRAY_ELEMENTS_COUNT( all_preferred_network_mode_name_pairs )
														);

		if( mode_name == NULL )
		{
			return -1;
		}

		//模块相关的模式值
		module_mode_value=util_get_band_value_by_name_ex( mode_name
														,supported_mode_info
														,mode_count
														);
		if( module_mode_value != -1 )
		{
			return module_mode_value;
		}
		else
		{
			return default_value;
		}
		index++;
	}

	return -1;
}

void standardise_str_for_module(char * input)
{
	char *p=strstr(input,"+BMTCELLINFO:")+strlen("+BMTCELLINFO:");
	while(1)
	{
		if(!strncmp(p,"\nOK",3) || !strncmp(p,"\r\nOK",4) )
		{
			break;
		}
		else
		{
			if(!strncmp(p,"\r\n",2))
			{
				*p=' ';
				p++;
				*p=',';
				p++;
			}
			else if(!strncmp(p,"\n",1))
			{
				*p=',';
				p++;
			}
			else
			{
				p++;
			}
			cmd_echo_append("========","/tmp/p/aaa");
			cmd_echo_append(input,"/tmp/p/aaa");
		}
	}
}

static InfoStruct l1761_lte_status_resonse_result;
//对获取的LTE状态信息进行解析
int util_parse_l1761_lte_status( char* lte_status_response,char* lte_status_result )
{
	char dest_file_name[ 512 ];
	//获取进程ID
	int pid=getpid();

	//将响应信息写入到指定的文件中
	sprintf(
			dest_file_name
			,"/tmp/.util_parse_l1761_lte_status-%d-response-file"
			,pid
			);

	cmd_rm( dest_file_name );

	//首先将内容写入到临时文件中
	//if( is_module_based_on_zte7510_used() )
	if( 0 )
	{
		int file_size;
		int counter=0;
		while( TRUE )
		{
			file_size=util_get_file_size_by_file_name( TMP_EMCI_INFO_FILE );
			if( file_size >= 6 )
			{
				break;
			}
			counter++;
			if( counter >= 3 )
			{
				break;
			}
			sleep( 1 );
		}
		cmd_cp( TMP_EMCI_INFO_FILE,dest_file_name );
	}
	cmd_touch( dest_file_name );
	cmd_echo_append_without_new_line( "\r\n\r\n",dest_file_name );
	//if( is_module_bm806_used() )
	//{
	//	standardise_str_for_module(lte_status_response);
	//}
	cmd_echo_append_without_new_line( lte_status_response,dest_file_name );

	memset( &l1761_lte_status_resonse_result,0,sizeof(l1761_lte_status_resonse_result) );
	util_read_module_status( dest_file_name,&l1761_lte_status_resonse_result );
	//删除临时生成的文件
	cmd_rm( dest_file_name );

	if( lte_status_result == NULL )
	{
		return -1;
	}

	//将所有处理的结果使用固定的格式返回
	if( strlen( l1761_lte_status_resonse_result.TZEARFCN ) )
	{
		if( lte_status_result != NULL )
		{
			if( strlen( lte_status_result ) )
			{
				STRCAT_S( lte_status_result,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			sprintf(
					lte_status_result+strlen( lte_status_result )
					,"%s%s%s"
					,CURRENT_NETWORK_INFO_EARFCN
					,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
					,l1761_lte_status_resonse_result.TZEARFCN
					);
		}
	}

	if( strlen( l1761_lte_status_resonse_result.TZBAND ) )
	{
		if( lte_status_result != NULL )
		{
			if( strlen( lte_status_result ) )
			{
				STRCAT_S( lte_status_result,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			sprintf(
					lte_status_result+strlen( lte_status_result )
					,"%s%s%s"
					,CURRENT_NETWORK_INFO_Frequency_Band
					,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
					,l1761_lte_status_resonse_result.TZBAND
					);
		}
	}

	if( strlen( l1761_lte_status_resonse_result.TZBANDWIDTH ) )
	{
		if( lte_status_result != NULL )
		{
			if( strlen( lte_status_result ) )
			{
				STRCAT_S( lte_status_result,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			sprintf(
					lte_status_result+strlen( lte_status_result )
					,"%s%s%s"
					,CURRENT_NETWORK_INFO_Downlink_Bandwidth
					,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
					,l1761_lte_status_resonse_result.TZBANDWIDTH
					);
		}
	}

	if( strlen( l1761_lte_status_resonse_result.TZTRANSMODE ) )
	{
		if( lte_status_result != NULL )
		{
			if( strlen( lte_status_result ) )
			{
				STRCAT_S( lte_status_result,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			sprintf(
					lte_status_result+strlen( lte_status_result )
					,"%s%s%s"
					,CURRENT_NETWORK_INFO_TZTRANSMODE
					,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
					,l1761_lte_status_resonse_result.TZTRANSMODE
					);
		}
	}

	if( strlen( l1761_lte_status_resonse_result.TZRSRP ) )
	{
		if( lte_status_result != NULL )
		{
			if( strlen( lte_status_result ) )
			{
				STRCAT_S( lte_status_result,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			sprintf(
					lte_status_result+strlen( lte_status_result )
					,"%s%s%s"
					,CURRENT_NETWORK_INFO_RSRP
					,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
					,l1761_lte_status_resonse_result.TZRSRP
					);
		}
	}

	if( strlen( l1761_lte_status_resonse_result.TZRSRQ ) )
	{
		if( lte_status_result != NULL )
		{
			if( strlen( lte_status_result ) )
			{
				STRCAT_S( lte_status_result,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			sprintf(
					lte_status_result+strlen( lte_status_result )
					,"%s%s%s"
					,CURRENT_NETWORK_INFO_RSRQ
					,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
					,l1761_lte_status_resonse_result.TZRSRQ
					);
		}
	}

	if( strlen( l1761_lte_status_resonse_result.TZSINR ) )
	{
		if( lte_status_result != NULL )
		{
			if( strlen( lte_status_result ) )
			{
				STRCAT_S( lte_status_result,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			sprintf(
					lte_status_result+strlen( lte_status_result )
					,"%s%s%s"
					,CURRENT_NETWORK_INFO_SINR
					,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
					,l1761_lte_status_resonse_result.TZSINR
					);
		}
	}

	if( strlen( l1761_lte_status_resonse_result.TZTA ) )
	{
		if( lte_status_result != NULL )
		{
			if( strlen( lte_status_result ) )
			{
				STRCAT_S( lte_status_result,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			sprintf(
					lte_status_result+strlen( lte_status_result )
					,"%s%s%s"
					,CURRENT_NETWORK_INFO_TZTA
					,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
					,l1761_lte_status_resonse_result.TZTA
					);
		}
	}

	if( strlen( l1761_lte_status_resonse_result.TZTXPOWER ) )
	{
		if( lte_status_result != NULL )
		{
			if( strlen( lte_status_result ) )
			{
				STRCAT_S( lte_status_result,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			sprintf(
					lte_status_result+strlen( lte_status_result )
					,"%s%s%s"
					,CURRENT_NETWORK_INFO_TZTXPOWER
					,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
					,l1761_lte_status_resonse_result.TZTXPOWER
					);
		}
	}

	if( strlen( l1761_lte_status_resonse_result.TZGLBCELLID ) )
	{
		if( lte_status_result != NULL )
		{
			if( strlen( lte_status_result ) )
			{
				STRCAT_S( lte_status_result,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			sprintf(
					lte_status_result+strlen( lte_status_result )
					,"%s%s%s"
					,CURRENT_NETWORK_INFO_Serving_CellID
					,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
					,l1761_lte_status_resonse_result.TZGLBCELLID
					);
		}
	}

	if( strlen( l1761_lte_status_resonse_result.TZPHYCELLID ) )
	{
		if( lte_status_result != NULL )
		{
			if( strlen( lte_status_result ) )
			{
				STRCAT_S( lte_status_result,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			sprintf(
					lte_status_result+strlen( lte_status_result )
					,"%s%s%s"
					,CURRENT_NETWORK_INFO_Physical_CellID
					,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
					,l1761_lte_status_resonse_result.TZPHYCELLID
					);
		}
	}

	if( strlen( l1761_lte_status_resonse_result.TZPHYCELLID ) )
	{
		if( lte_status_result != NULL )
		{
			if( strlen( lte_status_result ) )
			{
				STRCAT_S( lte_status_result,CURRENT_NETWORK_INFO_SEPARATOR );
			}
			sprintf(
					lte_status_result+strlen( lte_status_result )
					,"%s%s%s"
					,CURRENT_NETWORK_INFO_Physical_CellID
					,CURRENT_NETWORK_INFO_FIELD_SEPARATOR
					,l1761_lte_status_resonse_result.TZPHYCELLID
					);
		}
	}

	debug( "\r\n lte_status_result:%s \r\n",lte_status_result );

	return 0;
}
//查询L1761模块锁物理小区的状态
int util_parse_l1761_lock_one_cell_status( char* lock_one_cell_response,int* locked_status,char* earfcn,char* physical_cell_id  )
{
	//^DPCILK:
	char* next_arg=strstr( lock_one_cell_response,"^DPCILK:" );

	if( next_arg != NULL )
	{
		int lock_status;
		next_arg+=strlen( "^DPCILK:" );
		next_arg=util_skip_all_spaces( next_arg );

		lock_status=atoi( next_arg );
		if( lock_status == 1 )
		{
			char used_earfcn[ 64 ];
			char used_physical_cell_id[ 64 ];

			//获取EARFCN
			next_arg=util_get_next_arg_cmdlib(next_arg,FALSE);
			if( *next_arg == '\r')
			{
				goto END_PROC_DPCILK;
			}
			next_arg=util_skip_all_spaces( next_arg );

			memset( used_earfcn,0,sizeof( used_earfcn ) );
			util_get_arg_from_string_cmdlib( used_earfcn,next_arg,( int )sizeof( used_earfcn )-1 );

			//获取物理小区ID
			next_arg=util_get_next_arg_cmdlib(next_arg,FALSE);
			if( *next_arg == '\r')
			{
				goto END_PROC_DPCILK;
			}
			next_arg=util_skip_all_spaces( next_arg );

			memset( used_physical_cell_id,0,sizeof( used_physical_cell_id ) );
			util_get_arg_from_string_cmdlib( used_physical_cell_id,next_arg,( int )sizeof( used_physical_cell_id )-1 );

			if( locked_status != NULL )
			{
				*locked_status=1;
			}

			if( earfcn != NULL )
			{
				strcpy( earfcn,used_earfcn );
			}

			if( physical_cell_id != NULL )
			{
				strcpy( physical_cell_id,used_physical_cell_id );
			}
		}
		else
		{
			if( locked_status != NULL )
			{
				*locked_status=0;
			}

			if( earfcn != NULL )
			{
				*earfcn=0;
			}

			if( physical_cell_id != NULL )
			{
				*physical_cell_id=0;
			}
		}

		return 0;
	}

	END_PROC_DPCILK:
	return -1;
}
//check if one file exist
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
//^(.+)$
//"\1\\n"
//L1761执行锁物理小区操作锁需要的AT命令
int util_l1761_get_at_to_lock_one_cell( int lock_or_unlock,char* earfcn,char* physical_cell_id,char* dest_at_command )
{
	if( dest_at_command != NULL )
	{
		if( lock_or_unlock )
		{
			sprintf(dest_at_command
					,"AT^DPCILK=1,%s,%s"
					,earfcn
					,physical_cell_id
					);
		}
		else
		{
			sprintf(
					dest_at_command
					,"AT^DPCILK=0"
					);
		}
		return 0;
	}
	return -1;
}
static LTEQueryStatusAndLockOneCell all_modules_query_lte_status_and_lock_one_cell[]=
{
	{
		is_module_l1761_used
		,"AT+TZEARFCN?\n"
		"AT+TZBAND?\n"
		"AT+TZBANDWIDTH?\n"
		"AT+TZTRANSMODE?\n"
		"AT+TZRSRP?\n"
		"AT+TZRSRQ?\n"
		"AT+TZSINR?\n"
		"AT+TZTA?\n"
		"AT+TZTXPOWER?\n"
		"AT+TZGLBCELLID?\n"
		"AT+TZPHYCELLID?\n"
		,"AT^DPCILK?"
		,"AT+TZEARFCN?\n"
		"AT+TZPHYCELLID?\n"
		,util_parse_l1761_lte_status
		,util_parse_l1761_lock_one_cell_status
		,util_l1761_get_at_to_lock_one_cell
		,util_parse_l1761_lte_status
	},
	{
		is_module_ml7810_used
		,"AT+ZSINR"
		,"AT+ZLTELC?"
		,"AT"
	},
	{
		is_module_bm806_used
		,"AT+BMTCELLINFO\n"
		,"AT\n"
		,"AT"
		,util_parse_l1761_lte_status
	},

};
//返回获取LTE状态信息需要发送的AT命令
char* util_get_at_to_query_lte_status( void )
{
	int index=0;
	while( index < ARRAY_ELEMENTS_COUNT( all_modules_query_lte_status_and_lock_one_cell ) )
	{
		if( all_modules_query_lte_status_and_lock_one_cell[index].is_one_module_is_used_function() )
		{
			return all_modules_query_lte_status_and_lock_one_cell[index].at_to_query_lte_status;
		}
		index++;
	}

	return NULL;
}

//对获取的LTE状态信息进行解析
int util_parse_lte_status( char* lte_status_response,char* lte_status_result )
{
	int index=0;

	if( lte_status_result != NULL )
	{
		*lte_status_result=0;
	}

	if( lte_status_response == NULL )
	{
		return -1;
	}

	if( !strlen( lte_status_response ) )
	{
		return -1;
	}

	while( index < ARRAY_ELEMENTS_COUNT( all_modules_query_lte_status_and_lock_one_cell ) )
	{
		if( all_modules_query_lte_status_and_lock_one_cell[index].is_one_module_is_used_function() )
		{
			if( all_modules_query_lte_status_and_lock_one_cell[index].parse_lte_status_function != NULL )
			{
				return all_modules_query_lte_status_and_lock_one_cell[index].parse_lte_status_function( lte_status_response,lte_status_result );
			}
		}
		index++;
	}
	return -1;
}
//执行锁物理小区操作锁需要的AT命令
//lock_or_unlock,1表示被锁定,否则表示没有被锁定
//earfcn,physical_cell_id使用10进制
int util_get_at_to_lock_one_cell( int lock_or_unlock,char* earfcn,char* physical_cell_id,char* dest_at_command )
{
	int index=0;

	if( dest_at_command != NULL )
	{
		*dest_at_command=0;
	}

	while( index < ARRAY_ELEMENTS_COUNT( all_modules_query_lte_status_and_lock_one_cell ) )
	{
		if( all_modules_query_lte_status_and_lock_one_cell[index].is_one_module_is_used_function() )
		{
			if( all_modules_query_lte_status_and_lock_one_cell[index].at_to_lock_one_cell_function != NULL )
			{
				return all_modules_query_lte_status_and_lock_one_cell[index].at_to_lock_one_cell_function( lock_or_unlock,earfcn,physical_cell_id,dest_at_command );
			}
		}
		index++;
	}
	return -1;
}
//获取查询锁物理小区的AT命令
char* util_get_at_to_query_lock_one_cell_status( void )
{
	int index=0;
	while( index < ARRAY_ELEMENTS_COUNT( all_modules_query_lte_status_and_lock_one_cell ) )
	{
		if( all_modules_query_lte_status_and_lock_one_cell[index].is_one_module_is_used_function() )
		{
			return all_modules_query_lte_status_and_lock_one_cell[index].at_to_query_lock_one_cell_status;
		}
		index++;
	}

	return NULL;
}

//对获取到的锁物理小区查询的结果进行解析
//locked_status,1表示被锁定,否则表示没有被锁定
//earfcn,physical_cell_id使用10进制
int util_parse_lock_one_cell_status( char* lock_one_cell_response,int* locked_status,char* earfcn,char* physical_cell_id  )
{
	int index=0;

	while( index < ARRAY_ELEMENTS_COUNT( all_modules_query_lte_status_and_lock_one_cell ) )
	{
		if( all_modules_query_lte_status_and_lock_one_cell[index].is_one_module_is_used_function() )
		{
			if( all_modules_query_lte_status_and_lock_one_cell[index].parse_lock_one_cell_status_function != NULL )
			{
				return all_modules_query_lte_status_and_lock_one_cell[index].parse_lock_one_cell_status_function( lock_one_cell_response,locked_status,earfcn,physical_cell_id );
			}
		}
		index++;
	}
	return -1;
}

//获取查询当前所在的频点和物理小区ID的AT命令
char* util_get_at_to_query_earfcn_physical_cell_id( void )
{
	int index=0;
	while( index < ARRAY_ELEMENTS_COUNT( all_modules_query_lte_status_and_lock_one_cell ) )
	{
		if( all_modules_query_lte_status_and_lock_one_cell[index].is_one_module_is_used_function() )
		{
			return all_modules_query_lte_status_and_lock_one_cell[index].at_to_query_earfcn_physical_cell_id;
		}
		index++;
	}

	return NULL;
}


//对当前所在的频段和物理小区ID进行解析
int util_parse_query_earfcn_physical_cell_id_response( char* lte_status_response,char* lte_status_result )
{
	return util_parse_lte_status( lte_status_response,lte_status_result );
}
//write string to one specified file
int cmd_echo(char* str,const char *file_name)
{
	//open file handle to write
	FILE* file_handle=fopen(file_name,"wb");

	if( file_handle == NULL )
	{
		return -1;
	}

	fwrite(str,strlen( str ),1,file_handle);
	fwrite("\n",strlen( "\n" ),1,file_handle);
	//close file handle
	fclose(file_handle);

	return 0;
}

//append string to one specified file
int cmd_echo_append(char* str,const char *file_name)
{
	//open file handle to write
	FILE* file_handle=fopen(file_name,"ab");

	if( file_handle == NULL )
	{
		return -1;
	}

	fwrite(str,strlen( str ),1,file_handle);
	fwrite("\n",strlen( "\n" ),1,file_handle);
	//close file handle
	fclose(file_handle);

	return 0;
}


//append string to one specified file
int cmd_echo_append_without_new_line(char* str,const char *file_name)
{
	//open file handle to write
	FILE* file_handle=fopen(file_name,"ab");

	if( file_handle == NULL )
	{
		return -1;
	}

	fwrite(str,strlen( str ),1,file_handle);
	//close file handle
	fclose(file_handle);

	return 0;
}

int cmd_echo_without_new_line(char* str,const char *file_name)
{
	//open file handle to write
	FILE* file_handle=fopen(file_name,"wb");

	if( file_handle == NULL )
	{
		return -1;
	}

	fwrite(str,strlen( str ),1,file_handle);
	//close file handle
	fclose(file_handle);

	return 0;
}

//去掉字符串中的特殊字符
void util_remove_special_char( char* one_string,char special_char )
{
	char* last_string;

	if( one_string == NULL )
	{
		return;
	}

	last_string=one_string+strlen(one_string)-1;

	while( last_string >= one_string )
	{
		if( *last_string == special_char )
		{
			memmove( last_string,last_string+1,strlen( last_string+1 )+1 );
		}
		last_string--;
	}
}


//是否应该禁止漫游
int util_should_forbid_roaming( InfoStruct* info_struct )
{
	return !strcmp( info_struct->TZ_FORBID_ROAMING,"yes" );
}

//write string to one specified file
int cmd_touch(const char *file_name)
{
	//open file handle to write
	FILE* file_handle=fopen(file_name,"wb");

	if( file_handle == NULL )
	{
		return -1;
	}

	//close file handle
	fclose(file_handle);

	return 0;
}

//设置文件的权限为可执行
int cmd_chmodx(const char* file_path)
{
	//the file exist
	if( cmd_file_exist(file_path) )
	{
		//now the file can be read/write and execute
		#ifndef WIN32
		chmod(file_path,S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH);
		#endif
		return 0;
	}

	return -1;
}

//设置文件的读写权限的可进行读写
int cmd_chmodrw(const char* file_path)
{
	//the file exist
	if( cmd_file_exist(file_path) )
	{
		//now the file can be read/write and execute
		#ifndef WIN32
		chmod( file_path,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH );
		#endif
		return 0;
	}

	return -1;
}

//当前使用的是CDMA卡
int util_is_cdma_card_used(void)
{
	return cmd_file_exist( CDMA_UIM_RUIM_CARD_USED_FILE );
}

//rm one file
int cmd_rm(const char *file_name)
{
	//open file handle to write
#ifndef WIN32
	if( !access( file_name,R_OK ) )
	{
		unlink( file_name );
	}
#endif

	return 0;
}

//print file content
int cmd_cat(const char* file_name,char* buffer,int buffer_size)
{
	//open file handle to write
	FILE* file_handle=fopen(file_name,"rb");

	if( file_handle == NULL )
	{
		return -1;
	}

	if( buffer != NULL )
	{
		memset(buffer,0,buffer_size);
		fread(buffer,buffer_size-1,1,file_handle);
	}
	else
	{
		char tmp_buffer[64];
		memset(tmp_buffer,0,sizeof(tmp_buffer));
		fread(tmp_buffer,sizeof(tmp_buffer)-1,1,file_handle);
	}


	//close file handle
	fclose(file_handle);

	return 0;
}

//check if sim card exist
int cmd_sim_exist(void)
{
	char buffer[64];
	cmd_cat("/proc/tozed/ind",buffer,sizeof(buffer));
	return atoi( buffer );
}

char *util_get_next_arg2(char  * argp,int is_first_parameter)
{
	int is_in_double_quote=FALSE;

	while(
			( (*argp > ' ') && (*argp != ',') )
			||( (*argp == ' ') && is_in_double_quote )
			||( is_first_parameter && ( (*argp == '\r') || (*argp == '\n') ) )
		)
	{
		if( *argp == '\"' )
		{
			is_in_double_quote=TRUE;
		}
		argp++;   // scan past current arg
	}
	while( (*argp == ' ') || (*argp == ',') )
	{
		argp++;   // scan past spaces
	}
	return argp;
}

//是否需要输入PIN码
//<SIM/USIM 卡是否存在> <当前卡是否需要输入 PIN 码> <当前卡是否需要输入 PUK> <信号质量> <短
//消息状态> <网络注册状态> <PS 数据业务注册状态> <EPS 数据业务注册状态> <当前网络模式> <当
//前网络数据模式>
//<当前卡是否需要输入 PIN 码>:
//0:不需要 PIN 码
//1:需要输入 PIN 码
//其他数值均为无效状态
int cmd_sim_need_pin(void)
{
	char* tmp_ptr;
	char buffer[64];
	cmd_cat("/proc/tozed/ind",buffer,sizeof(buffer));

	tmp_ptr=util_get_next_arg2( buffer,TRUE );

	return ( atoi( tmp_ptr ) == 1 );
}

/*
<当前卡是否需要输入 PUK>:
0:不需要 PUK 码
1:需要输入 PUK 码
其他数值均为无效状态
*/
int cmd_sim_need_puk(void)
{
	char* tmp_ptr;
	char* tmp_ptr2;
	char buffer[64];
	cmd_cat("/proc/tozed/ind",buffer,sizeof(buffer));

	tmp_ptr=util_get_next_arg2( buffer,TRUE );
	tmp_ptr2=util_get_next_arg2( tmp_ptr,FALSE );

	return ( atoi( tmp_ptr2 ) == 1 );
}

//P500模块网络连接是否完好
int util_p500_link_connected(void)
{
	char buffer[64];
	buffer[0]=0;
	cmd_cat( P500_NETWORK_LINK_CONNECTED_FILE,buffer,sizeof(buffer) );

#ifdef __i386__
	return 1;
#else
#ifdef USE_CDC_ETHER_TO_DIAL_NETWORK
	if( is_module_slm630b_used() )
	{
		return 1;
	}
	else
#endif
	{
		return ( atoi( buffer ) == 1 );
	}
#endif
}

int util_get_all_arguments(char* buffer,char* argv[])
{
	char* ptr=buffer;
	int argc=0;
	int is_last_argument_end=FALSE;

	//no arguments
	if( !buffer )
	{
		return 0;
	}

	//skip spaces at the beginning of the buffer
	while( ( *ptr == ' ' ) || ( *ptr == '\t' ) || ( *ptr == '\r' ) || ( *ptr == '\n' ) )
	{
		ptr++;
	}

	if( ptr && *ptr )
	{
		argv[argc++]=ptr;
	}

	while( ptr && *ptr )
	{
		if( ( *ptr == ' ' ) || ( *ptr == '\t' ) || ( *ptr == '\r' ) || ( *ptr == '\n' ) )
		{
			*ptr='\0';
			is_last_argument_end=TRUE;
		}
		else if( is_last_argument_end )
		{
			argv[argc++]=ptr;
			is_last_argument_end=FALSE;
		}

		ptr++;
	}

	return argc;
}


//可以通过 cat /proc/tozed/ind 查看系统的状态,返回的信息如下:
//<SIM/USIM 卡是否存在> <当前卡是否需要输入 PIN 码> <当前卡是否需要输入 PUK> <信号质量> <短消息状态>
//<网络注册状态> <PS 数据业务注册状态> <EPS 数据业务注册状态> <当前网络模式> <当前网络数据模式>
//get the dial status and signal quality
int cmd_get_dial_status_and_signal_quality( int* dial_status,int* signal_quality )
{
	char buffer[128];
	char* argv[10];
	int argc;

	cmd_cat("/proc/tozed/ind",buffer,sizeof(buffer));
	argc=util_get_all_arguments( buffer,argv );

	if( argc < 8 )
	{
		return -1;
	}

	if( dial_status )
	{
		*dial_status=-1;
		*dial_status=( atoi( argv[5] ) == 1 )||( atoi( argv[6] ) == 1 )||( atoi( argv[7] ) == 1 );
	}

	if( signal_quality )
	{
		*signal_quality=-1;
		*signal_quality=atoi( argv[3] );
	}

	return 0;
}

//copy one file's content to the other file
int cmd_cp(const char *src_file_name,const char *dest_file_name)
{
	//open file handle to write
#ifndef WIN32
	if( !access( src_file_name,R_OK ) )
	{
		char buffer[1024];
		int read_bytes_counter;
		int total_byte_counter=0;
		FILE* src_file_handle=fopen(src_file_name,"rb");
		FILE* dest_file_handle=NULL;
		struct stat file_status;

		if( src_file_handle == NULL )
		{
			return -1;
		}

		dest_file_handle=fopen(dest_file_name,"wb");
		if( dest_file_handle == NULL )
		{
			fclose(src_file_handle);
			return -1;
		}

		//get file size
		stat(src_file_name, &file_status);

		while( TRUE )
		{
			memset(buffer,0,sizeof(buffer));
			read_bytes_counter=fread(buffer,1,sizeof(buffer),src_file_handle);
			if( read_bytes_counter )
			{
				fwrite(buffer,read_bytes_counter,1,dest_file_handle);
				total_byte_counter+=read_bytes_counter;
			}
			else
			{
				//fread() and fwrite() return the number of items successfully read or written (i.e., not the number of characters).  If an error occurs, or the end-of-file is reached, the
				//return value is a short item count (or zero).
				//fread() does not distinguish between end-of-file and error, and callers must use feof(3) and ferror(3) to determine which occurred.
				if( file_status.st_size > total_byte_counter )
				{
					fwrite(buffer,file_status.st_size-total_byte_counter,1,dest_file_handle);
				}
				break;
			}
		}

		fclose(src_file_handle);
		fclose(dest_file_handle);
	}
#endif

	return 0;
}

int cmd_init_signal_ind_led( int is_running_in_p11 )
{
	if( is_this_device_p13() )
	{
		cmd_echo("412","/proc/tozed_gpio/led");
		cmd_echo("413","/proc/tozed_gpio/led");
		cmd_echo("414","/proc/tozed_gpio/led");
		cmd_echo("312","/proc/tozed_gpio/led");
		cmd_echo("313","/proc/tozed_gpio/led");
		cmd_echo("314","/proc/tozed_gpio/led");
		cmd_echo("117","/proc/tozed_gpio/led");
		cmd_echo("317","/proc/tozed_gpio/led");
		cmd_echo("118","/proc/tozed_gpio/led");
		cmd_echo("318","/proc/tozed_gpio/led");
	}
	else
	//运行在ZLT P11设备中
	if( is_running_in_p11 )
	{
		cmd_echo("412","/proc/tozed_gpio/led");
		cmd_echo("413","/proc/tozed_gpio/led");
		cmd_echo("414","/proc/tozed_gpio/led");
		cmd_echo("312","/proc/tozed_gpio/led");
		cmd_echo("313","/proc/tozed_gpio/led");
		cmd_echo("314","/proc/tozed_gpio/led");
	}
	else
	{
		//config the gpio 14 and gpio 17 as output mode
		//system( "echo \"414\">/proc/tozed_gpio/led" );
		cmd_echo("414","/proc/tozed_gpio/led");
		//system( "echo \"117\">/proc/tozed_gpio/led" );
		cmd_echo("117","/proc/tozed_gpio/led");
		cmd_echo("314","/proc/tozed_gpio/led");
		cmd_echo("317","/proc/tozed_gpio/led");
	}
	return 0;
}

//初始化LTE指示灯为输出模式
void util_init_lte_led( void )
{
	//红
	cmd_echo("519","/proc/tozed_gpio/led");
	//蓝
	cmd_echo("520","/proc/tozed_gpio/led");
	//绿
	cmd_echo("521","/proc/tozed_gpio/led");
}

//for tozedap-router_4g_industry
void util_turn_on_net_led( void )
{


}
void util_turn_off_net_led( void )
{


}
void util_turn_on_wifi_led( void )
{

}
void util_turn_off_wifi_led(void)
{

}

//开启LTE指示灯
void util_turn_on_lte_led( void )
{
	//GPIO17 控制“DATA”灯，表示  LTE网络连接正常，GPIO17为低点亮DS15，为高DS15灭掉
	cmd_echo("217","/proc/tozed_gpio/led");
}

//关掉LTE指示灯
void util_turn_off_lte_led( void )
{
	//GPIO17 控制“DATA”灯，表示  LTE网络连接正常，GPIO17为低点亮DS15，为高DS15灭掉
	cmd_echo("317","/proc/tozed_gpio/led");
}

//update signal ind
int cmd_update_signal_ind( int signal_quality,int is_running_in_p11 )
{
	//no signal
	if(signal_quality <= 0)
	{
		if( is_this_device_p13() )
		{
			cmd_echo("317","/proc/tozed_gpio/led");
			cmd_echo("313","/proc/tozed_gpio/led");
			cmd_echo("314","/proc/tozed_gpio/led");
			cmd_echo("312","/proc/tozed_gpio/led");
		}
		else
		if( is_running_in_p11 && !is_this_device_p21_voip() )
		{
			cmd_echo("312","/proc/tozed_gpio/led");
			cmd_echo("313","/proc/tozed_gpio/led");
			cmd_echo("314","/proc/tozed_gpio/led");
		}
		else
		if( is_this_device_p21_voip() )
		{
			cmd_echo("101","/proc/tozed_gpio/i2c");
		}
		else
		{
			//system( "echo \"314\">/proc/tozed_gpio/led" );
			cmd_echo("314","/proc/tozed_gpio/led");
			//system( "echo \"317\">/proc/tozed_gpio/led" );
			cmd_echo("317","/proc/tozed_gpio/led");
		}
	}
	//weak signal,red
	else if(signal_quality == 1)
	{
		if( is_this_device_p13() )
		{
			cmd_echo("217","/proc/tozed_gpio/led");
			cmd_echo("313","/proc/tozed_gpio/led");
			cmd_echo("314","/proc/tozed_gpio/led");
			cmd_echo("312","/proc/tozed_gpio/led");

		}
		else
		if( is_running_in_p11 && !is_this_device_p21_voip() )
		{
			cmd_echo("212","/proc/tozed_gpio/led");
			cmd_echo("313","/proc/tozed_gpio/led");
			cmd_echo("314","/proc/tozed_gpio/led");
		}
		else
		if( is_this_device_p21_voip() )
		{
			cmd_echo("201","/proc/tozed_gpio/i2c");
		}
		else
		{
			//system( "echo \"214\">/proc/tozed_gpio/led" );
			cmd_echo("214","/proc/tozed_gpio/led");
			//system( "echo \"317\">/proc/tozed_gpio/led" );
			cmd_echo("317","/proc/tozed_gpio/led");
		}
	}
	//middle signal,orange
	else if(signal_quality == 2)
	{
		if( is_this_device_p13() )
		{
			cmd_echo("217","/proc/tozed_gpio/led");
			cmd_echo("213","/proc/tozed_gpio/led");
			cmd_echo("314","/proc/tozed_gpio/led");
			cmd_echo("312","/proc/tozed_gpio/led");
		}
		else
		if( is_running_in_p11 && !is_this_device_p21_voip() )
		{
			cmd_echo("312","/proc/tozed_gpio/led");
			cmd_echo("213","/proc/tozed_gpio/led");
			cmd_echo("314","/proc/tozed_gpio/led");
		}
		else
		if( is_this_device_p21_voip() )
		{
			cmd_echo("301","/proc/tozed_gpio/i2c");
		}
		else
		{
			//system( "echo \"214\">/proc/tozed_gpio/led" );
			cmd_echo("214","/proc/tozed_gpio/led");
			//system( "echo \"217\">/proc/tozed_gpio/led" );
			cmd_echo("217","/proc/tozed_gpio/led");
		}

	}
	else if( (signal_quality == 3) && is_this_device_p13() )
	{
		cmd_echo("217","/proc/tozed_gpio/led");
		cmd_echo("213","/proc/tozed_gpio/led");
		cmd_echo("214","/proc/tozed_gpio/led");
		cmd_echo("312","/proc/tozed_gpio/led");
	}
	else if( (signal_quality == 3) && is_this_device_p21_voip() )
	{
		cmd_echo("401","/proc/tozed_gpio/i2c");
	}
	else if( (signal_quality == 4) && is_this_device_p21_voip() )
	{
		cmd_echo("501","/proc/tozed_gpio/i2c");
	}
	//good signal,green
	else
	{
		if( is_this_device_p13() )
		{
			cmd_echo("217","/proc/tozed_gpio/led");
			cmd_echo("213","/proc/tozed_gpio/led");
			cmd_echo("214","/proc/tozed_gpio/led");
			cmd_echo("212","/proc/tozed_gpio/led");
		}
		else
		if( is_running_in_p11 && !is_this_device_p21_voip() )
		{
			cmd_echo("312","/proc/tozed_gpio/led");
			cmd_echo("313","/proc/tozed_gpio/led");
			cmd_echo("214","/proc/tozed_gpio/led");
		}
		else
		if( is_this_device_p21_voip() )
		{
			cmd_echo("551","/proc/tozed_gpio/i2c");
		}
		else
		{
			//system( "echo \"314\">/proc/tozed_gpio/led" );
			cmd_echo("314","/proc/tozed_gpio/led");
			//system( "echo \"217\">/proc/tozed_gpio/led" );
			cmd_echo("217","/proc/tozed_gpio/led");
		}
	}

	return 0;
}

//指示SIM卡存在
void util_turn_on_sim_led( void )
{
	//GPIO17 控制“DATA”灯，表示  LTE网络连接正常，GPIO17为低点亮DS15，为高DS15灭掉
	cmd_echo("218","/proc/tozed_gpio/led");
}

//指示SIM卡不存在
void util_turn_off_sim_led( void )
{
	//GPIO17 控制“DATA”灯，表示  LTE网络连接正常，GPIO17为低点亮DS15，为高DS15灭掉
	cmd_echo("318","/proc/tozed_gpio/led");
}

//move one file to the other
int cmd_mv(const char* src_file,const char* dest_file)
{
	//copy source file's content to the dest file
	if( !cmd_cp( src_file,dest_file ) )
	{
		//remove the source file
		return cmd_rm(src_file);
	}

	return -1;
}



//network card exist or not
int cmd_netdev_exist(const char* dev_name)
{
	char buffer[256],*p1,*p2;

	FILE* file_handle=fopen("/proc/net/dev","r");
	if( file_handle == NULL )
	{
		return FALSE;
	}

	while( fgets( buffer,sizeof(buffer)-1,file_handle ) != NULL )
	{
		for(p1=buffer;p1&&( *p1 != 0 );p1++)
		{
			if( ( *p1!=' ' ) && ( *p1!='\t' ) )
			{
				break;
			}
		}

		for( p2=p1;p2&&( *p2!=0 );p2++ )
		{
			if( ( *p2==' ' ) || ( *p2=='\t' ) )
			{
				*p2=0;
				break;
			}
			else if(*p2==':')
			{
				*p2=0;
				if( strcmp(p1,dev_name)==0 )
				{
					fclose( file_handle );
					return TRUE;
				}

				break;
			}
		}
	}

	fclose( file_handle );
	return FALSE;
}

//获取所有的参数信息,参数使用空格或\t进行分割,delimiter为0表示无效
int util_get_all_args( char* buffer,char delimiter1,char delimiter2,char* args[],int max_args_count )
{
	int args_count=0;
	int buffer_length=strlen( buffer );
	char *p1;

	if( buffer == NULL )
	{
		return args_count;
	}

	if( args == NULL )
	{
		return args_count;
	}

	p1=buffer;

	while( ( p1-buffer ) < buffer_length )
	{
		while( ( ( *p1 == delimiter1 ) && ( delimiter1 != 0 ) && ( ( p1-buffer ) < buffer_length ) )
				|| ( ( *p1 == delimiter2 ) && ( delimiter2 != 0 ) && ( ( p1-buffer ) < buffer_length ) )
			)
		{
			p1++;
		}

		if( ( p1-buffer ) >= buffer_length )
		{
			break;
		}

		args[ args_count ]=p1;
		args_count++;

		while( ( ( ( *p1 != delimiter1 ) && ( delimiter1 != 0 ) && ( ( p1-buffer ) < buffer_length ) ) || ( delimiter1 == 0 ) )
				&& ( ( ( *p1 != delimiter2 ) && ( delimiter2 != 0 ) && ( ( p1-buffer ) < buffer_length ) ) || ( delimiter2 == 0 ) )
			)
		{
			p1++;
		}
		*p1=0;
		p1++;

		if( args_count >= max_args_count )
		{
			break;
		}

		if( ( p1-buffer ) >= buffer_length )
		{
			break;
		}
	}

	return args_count;
}

/*
user_00@xxxx64:~> cat /proc/net/dev
Inter-    | Receive                                                              |  Transmit
 face     | bytes    packets  errs  drop   fifo   frame   compressed  multicast  |  bytes    packets   errs    drop    fifo    colls    carrier    compressed
  eth0:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
  eth1:   67437819880 552776948    0    0    0     0          0         6 63179118041 486899714    0    0    0     0       0          0
    lo:      2338316682 34154992    0    0    0     0          0         0 2338316682 34154992    0    0    0     0       0          0
 tunl0:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
  sit0:        0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0

ip6tnl0:    0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0

最左边的表示接口的名字，Receive表示收包，Transmit表示收包；
bytes表示收发的字节数；
packets表示收发正确的包量；
errs表示收发错误的包量；
drop表示收发丢弃的包量；
*/
//get the network card flow statistics
int cmd_netdev_flowstat_ex(const char* dev_name,char* rx_bytes_buffer,char* tx_bytes_buffer,char* rx_packages_buffer,char* tx_packages_buffer)
{
	char buffer[512];
	char tmp_buffer[64];
	char* args[18];
	int args_count;

	FILE* file_handle=fopen("/proc/net/dev","r");
	if( file_handle == NULL )
	{
		return -1;
	}

	while( fgets( buffer,sizeof(buffer)-1,file_handle ) != NULL )
	{
		args_count=util_get_all_args( buffer,' ',':',args,18 );

		if( args_count < 11 )
		{
			continue;
		}

		snprintf( tmp_buffer
				,sizeof( tmp_buffer )
				,"%s"
				,dev_name
				);

		if( strcmp( args[ 0 ],tmp_buffer ) )
		{
			continue;
		}

		if( rx_bytes_buffer != NULL )
		{
			strcpy( rx_bytes_buffer,args[ 1 ] );
		}

		if( rx_packages_buffer != NULL )
		{
			strcpy( rx_packages_buffer,args[ 2 ] );
		}

		if( tx_bytes_buffer != NULL )
		{
			strcpy( tx_bytes_buffer,args[ 9 ] );
		}

		if( tx_packages_buffer != NULL )
		{
			strcpy( tx_packages_buffer,args[ 10 ] );
		}

		fclose( file_handle );
		return 0;
	}

	fclose( file_handle );
	return -1;
}

int cmd_netdev_flowstat(const char* dev_name,char* rx_bytes_buffer,char* tx_bytes_buffer)
{
	return cmd_netdev_flowstat_ex( dev_name,rx_bytes_buffer,tx_bytes_buffer,NULL,NULL );
}

#ifndef WIN32


//check if ip addr exist
int cmd_get_ip(const char* dev_name,struct in_addr* address)
{
	struct ifreq ifr;
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);


	if (sock < 0)
	{
		return -1;
	}

	strcpy(ifr.ifr_name, dev_name);

	if( ioctl( sock,SIOCGIFADDR,&ifr ) < 0 )
	{
		close( sock );
		return -1;
	}
#ifdef CMDLIB_TEST
	else
	{
		unsigned long ip = *(unsigned long*)&ifr.ifr_broadaddr.sa_data[2];
		printf("ip:%s\n", inet_ntoa(*(struct in_addr*)&ip));
	}
#endif

	if( address )
	{
		unsigned long ip = *(unsigned long*)&ifr.ifr_broadaddr.sa_data[2];
		memcpy( address,(struct in_addr*)&ip,sizeof(struct in_addr) );
	}

	close( sock );
	return 0;
}

//配置IP地址
int util_config_ipv4_addr(const char *net_dev, const char* ipaddr)
{
    struct ifreq ifr;
    int fd = 0;
    struct sockaddr_in *pAddr;

    if( (NULL == net_dev) || (NULL == ipaddr) )
    {
        //dbg_log_print(LOG_ERR, "illegal call function SetGeneralIP!");
        return -1;
    }

    if ( ( fd = socket(AF_INET,SOCK_DGRAM,0) ) < 0 )
    {
        //dbg_log_print(LOG_ERR,"socket....setip..false!!!");
        return -1;
    }

    strcpy(ifr.ifr_name, net_dev);

    pAddr = (struct sockaddr_in *)&(ifr.ifr_addr);
    bzero(pAddr, sizeof(struct sockaddr_in));
    pAddr->sin_addr.s_addr = inet_addr(ipaddr);
    pAddr->sin_family = AF_INET;
    if (ioctl(fd, SIOCSIFADDR, &ifr) < 0)
    {
        close(fd);
        //dbg_log_print(LOG_ERR,"ioctl..set_ipaddr..false!!!");
        return -1;
    }
    close(fd);
    return 0;
}


//check if ip addr exist
int cmd_ip_exist(const char* dev_name)
{
	if( !cmd_get_ip(dev_name,NULL) )
	{
		return TRUE;
	}

	return FALSE;
}

//check if ipv6 addr exist
int cmd_ipv6_exist(const char* dev_name)
{
    int if_idx, plen, scope, dad_status;
    unsigned int temp[16];
    char ifname[32];
	FILE* file_handle=fopen("/proc/net/if_inet6","r");

	//no ipv6 support
	if( file_handle == NULL )
	{
		return FALSE;
	}

	 while (fscanf(file_handle, "%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x %02x %02x %02x %02x %20s\n",
				  &(temp[0]), &(temp[1]), &(temp[2]), &(temp[3]),
				  &(temp[4]), &(temp[5]), &(temp[6]), &(temp[7]),
				  &(temp[8]), &(temp[9]), &(temp[10]), &(temp[11]),
				  &(temp[12]), &(temp[13]), &(temp[14]), &(temp[15]),
				  &if_idx, &plen, &scope, &dad_status, ifname) != EOF)
	{
		//network card name is ok and ipv6 scope is global
		if ( !strcmp(dev_name, ifname) && ( scope == 0 ) )
		{
			fclose(file_handle);
			return TRUE;
		}
	}

	//close file handle
	fclose(file_handle);

	return FALSE;
}

//get one network card's mac addr
int cmd_netdev_get_mac_addr(const char* net_dev_name,MacAddr* mac_addr)
{
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	struct ifreq ifr;

	//create socket fail
	if(sock < 0)
	{
		return -1;
	}

	strcpy(ifr.ifr_name, net_dev_name);
	if( ioctl(sock, SIOCGIFHWADDR, &ifr) < 0 )
	{
		close(sock);
		return -1;
	}

	memcpy( mac_addr->value, ifr.ifr_hwaddr.sa_data, sizeof(MacAddr) );
	close(sock);

	return 0;
}

void util_replace_all_zero_to_space( char* buffer,int size )
{
	int index=0;

	while( index < size )
	{
		if( buffer[ index ] == '\0' )
		{
			buffer[ index ]=' ';
		}
		index+=1;
	}
}

//check if one program is running
int cmd_ps_is_one_program_running_ex(char* cmd_name,char* match_string[],int match_string_count,char*pid_value )
{
	DIR* dir_handle;
	struct dirent* dir_entry;
	struct stat stat_buffer;
	char buffer[256];
	char cmdline_path[128];
	char cmdline_output[128];

	dir_handle=opendir("/proc");
	if( dir_handle == NULL )
	{
		return FALSE;
	}

	memset( buffer,0,sizeof(buffer) );
	getcwd( buffer,sizeof(buffer)-1 );
	chdir("/proc");

	while( ( dir_entry=readdir( dir_handle ) ) != NULL )
	{
		if( ( strcmp(dir_entry->d_name,".") == 0 )
			||( strcmp(dir_entry->d_name,"..") == 0 )
			)
		{
			continue;
		}

		//get the file attributes
		lstat(dir_entry->d_name,&stat_buffer);

		if( S_ISDIR(stat_buffer.st_mode) )
		{
			snprintf(cmdline_path,sizeof(cmdline_path),"./%s/cmdline",dir_entry->d_name);
			if( !access(cmdline_path,R_OK) )
			{
				cmd_cat(cmdline_path,cmdline_output,sizeof(cmdline_output));
				//将所有的\0替换为空格
				util_replace_all_zero_to_space( cmdline_output,sizeof(cmdline_output)-1 );
				if( strncmp( cmdline_output,cmd_name,strlen(cmd_name) ) == 0 )
				{
					int index;
					int match_fail=FALSE;

					for( index=0;( index < match_string_count ) && ( match_string != NULL );index+=1 )
					{
						if( strstr( cmdline_output,match_string[ index ] ) == NULL )
						{
							match_fail=TRUE;
						}
					}

					if( match_fail )
					{
						continue;
					}

					if( pid_value != NULL )
					{
						//保存进程的id
						strcpy( pid_value,dir_entry->d_name );
					}

					//restore the previous dir
					chdir( buffer );
					closedir(dir_handle);
					//return the result and the program pid
					return ( TRUE+( atoi(dir_entry->d_name)<<8 ) );
				}
			}
		}
	}

	//restore the previous dir
	chdir( buffer );
	closedir(dir_handle);

	return FALSE;
}

int cmd_ps_is_one_program_running(char* cmd_name)
{
	return cmd_ps_is_one_program_running_ex( cmd_name,NULL,0,NULL );
}
#else
int cmd_ps_is_one_program_running(char* cmd_name)
{
	return FALSE;
}
#endif

//calc eui64 by mac address
int util_calc_eui64(MacAddr* mac_addr,EUI64* eui64_addr)
{
	eui64_addr->value[0]=mac_addr->value[0]^0x02;
	eui64_addr->value[1]=mac_addr->value[1];
	eui64_addr->value[2]=mac_addr->value[2];
	eui64_addr->value[3]=0xff;
	eui64_addr->value[4]=0xfe;
	eui64_addr->value[5]=mac_addr->value[3];
	eui64_addr->value[6]=mac_addr->value[4];
	eui64_addr->value[7]=mac_addr->value[5];
	return 0;
}

int util_get_cmd_result(char *shellcmd, char *buffer, int size)
{

#ifndef WIN32
	FILE *stream;
	memset(buffer, 0, size);

	stream = popen(shellcmd, "r");
	if(stream != NULL)
	{
        fread(buffer, sizeof(char), size-1, stream);
        pclose(stream);

		return TRUE;
	}
	else
	{
		buffer[0] = '\0';
	}

	return FALSE;
#else
	memset(buffer, 0, size);
	return TRUE;
#endif

}

//check if get date and time success
int util_is_getting_date_and_time_ok(void)
{
	time_t time_value=time( NULL );
	/*
	 * The  localtime() function converts the calendar time timep to broken-time representation, expressed relative to the user’s specified timezone.  The function acts as if it
       called tzset(3) and sets the external variables tzname with information about the current timezone, timezone with the difference between Coordinated Universal Time  (UTC)
       and  local  standard  time  in  seconds, and daylight to a non-zero value if daylight savings time rules apply during some part of the year.  The return value points to a
       statically allocated struct which might be overwritten by subsequent calls to any of the date and time functions.  The localtime_r() function does the  same,  but  stores
       the data in a user-supplied struct.  It need not set tzname, timezone, and daylight.
	 * */
	struct tm * time_p=localtime( &time_value );

	#ifdef CMDLIB_TEST
	printf("current date time:%d,%d,%d,%d,%d,%d\n"
			,time_p->tm_year+1900
			,time_p->tm_mon+1
			,time_p->tm_mday
			,time_p->tm_hour
			,time_p->tm_min
			,time_p->tm_sec
			);
	#endif
	//tm_year   The number of years since 1900.
	//use 80 but not 70 to avoid the  12/31/1970 23:59:59 problem
	if( time_p->tm_year+1900 > 1980 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

#ifndef WIN32

//resolve domain name to ipv4 or ipv6 addr
int util_resolv_domain_name(const char* domain_name,char* ipv4_addr,char* ipv6_addr,int* result_ind)
{
	struct addrinfo *answer,hints,*addr_info_p;
	int ret;
	struct sockaddr_in *sinp4;
	struct sockaddr_in6 *sinp6;
	char* addr;

	if( result_ind )
	{
		*result_ind=0;
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	/*
	 * The getaddrinfo() function allocates and initializes a linked list of addrinfo structures, one for each network address that matches node  and  service,  subject  to  any
       restrictions imposed by hints, and returns a pointer to the start of the list in res.  The items in the linked list are linked by the ai_next field.
	 * */
	ret = getaddrinfo(domain_name, NULL, &hints, &answer);
	if ( !ret )
	{
		for (addr_info_p = answer; addr_info_p != NULL; addr_info_p = addr_info_p->ai_next)
		{
			if(addr_info_p->ai_family == AF_INET)
			{
				sinp4 = (struct sockaddr_in *)addr_info_p->ai_addr;
				/*
				 * The  inet_ntoa()  function  converts  the Internet host address in, given in network byte order, to a string in IPv4 dotted-decimal notation.  The string is returned in a
       	   	   	   statically allocated buffer, which subsequent calls will overwrite.
				 * */
				addr = inet_ntoa( sinp4->sin_addr);
				#ifdef CMDLIB_TEST
				printf("ipv4 addr = %s\n", addr?addr:"unknow ");
				#endif

				if( ipv4_addr )
				{
					strcpy(ipv4_addr,addr);
				}

				if( result_ind )
				{
					*result_ind=(*result_ind)|IPv4_EXIST_FLAG;
				}
			}
			else if(addr_info_p->ai_family == AF_INET6)
			{
				sinp6 = (struct sockaddr_in6 *)addr_info_p->ai_addr;
				#ifdef CMDLIB_TEST
				int i;
				printf("ipv6 addr = ");
				for(i = 0; i < 16; i++)
				{
					if( ( !( i&0x01 ) ) && i )
					{
						printf(":");
					}
					printf("%02x",sinp6->sin6_addr.s6_addr[i]);
				}
				printf(" \n");
				#else
				if( ipv6_addr )
				{
					sprintf(ipv6_addr
							,"%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x"
							,sinp6->sin6_addr.s6_addr[0]
							,sinp6->sin6_addr.s6_addr[1]
							,sinp6->sin6_addr.s6_addr[2]
							,sinp6->sin6_addr.s6_addr[3]
							,sinp6->sin6_addr.s6_addr[4]
							,sinp6->sin6_addr.s6_addr[5]
							,sinp6->sin6_addr.s6_addr[6]
							,sinp6->sin6_addr.s6_addr[7]
							,sinp6->sin6_addr.s6_addr[8]
							,sinp6->sin6_addr.s6_addr[9]
							,sinp6->sin6_addr.s6_addr[10]
							,sinp6->sin6_addr.s6_addr[11]
							,sinp6->sin6_addr.s6_addr[12]
							,sinp6->sin6_addr.s6_addr[13]
							,sinp6->sin6_addr.s6_addr[14]
							,sinp6->sin6_addr.s6_addr[15]
							);
				}
				#endif

				if( result_ind )
				{
					*result_ind=(*result_ind)|IPv6_EXIST_FLAG;
				}
			}

			#ifdef CMDLIB_TEST
			printf(" \n");
			#endif

		}

		freeaddrinfo(answer);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

//fe80::202:3ff:fe04:506 dev eth0 lladdr 00:02:03:04:05:06 router STALE
//fe80::203:7fff:fe11:274f dev eth0 lladdr 00:03:7f:11:27:4f router STALE
//fe80::baac:6fff:febb:7e3f dev br0 lladdr b8:ac:6f:bb:7e:3f STALE
//2001:db8:8888:1::2 dev br0 lladdr b8:ac:6f:bb:7e:3f STALE
//parse the neigh info
int util_parse_neigh_info(const char* file_name,Neighbour_Info* neigh_info,int only_link_local_addr,int only_global_addr)
{
	FILE* file_handle;
	char temp_buffer[256];

	file_handle=fopen(file_name,"r");

	if( file_handle == NULL )
	{
		return -1;
	}

	neigh_info->neighbour_count=0;

	memset( temp_buffer,0,sizeof(temp_buffer) );
	while( fgets( temp_buffer,sizeof(temp_buffer)-1,file_handle ) != NULL )
	{
		char* p1;
		char* p2;
		int current_neigh_info_index=neigh_info->neighbour_count;

		//dest ipv6 addr is unreachable
		if( strstr(temp_buffer,"FAILED") )
		{
			memset( temp_buffer,0,sizeof(temp_buffer) );
			continue;
		}

		if( strstr(temp_buffer,"INCOMPLETE") )
		{
			memset( temp_buffer,0,sizeof(temp_buffer) );
			continue;
		}

		//no link local addr
		if( only_link_local_addr && ( strstr(temp_buffer,"fe80:") == NULL ) )
		{
			memset( temp_buffer,0,sizeof(temp_buffer) );
			continue;
		}
		else if( only_global_addr )
		{
			if( strstr(temp_buffer,"fe80:") )
			{
				memset( temp_buffer,0,sizeof(temp_buffer) );
				continue;
			}
		}

		if( strstr(temp_buffer,"REACHABLE") )
		{
			neigh_info->neighbours[ current_neigh_info_index ].status=IPV6_NEIGH_STATUS_REACHABLE;
		}
		else if( strstr(temp_buffer,"STALE") )
		{
			neigh_info->neighbours[ current_neigh_info_index ].status=IPV6_NEIGH_STATUS_STALE;
		}
		else if( strstr(temp_buffer,"DELAY") )
		{
			neigh_info->neighbours[ current_neigh_info_index ].status=IPV6_NEIGH_STATUS_DELAY;
		}
		else if( strstr(temp_buffer,"PROBE") )
		{
			neigh_info->neighbours[ current_neigh_info_index ].status=IPV6_NEIGH_STATUS_PROBE;
		}
		else
		{
			neigh_info->neighbours[ current_neigh_info_index ].status=IPV6_NEIGH_STATUS_FAILED;
		}

		if( strstr(temp_buffer,"router") )
		{
			neigh_info->neighbours[ current_neigh_info_index ].is_router=TRUE;
		}
		else
		{
			neigh_info->neighbours[ current_neigh_info_index ].is_router=FALSE;
		}

		for(p1=temp_buffer;p1&&( *p1 != 0 );p1++)
		{
			if( ( *p1!=' ' ) && ( *p1!='\t' ) )
			{
				break;
			}
		}

		for( p2=p1;p2&&( *p2!=0 );p2++ )
		{
			if( ( *p2==' ' ) || ( *p2=='\t' ) || ( *p2=='\n' ) || ( *p2=='/' ) )
			{
				*p2=0;
				break;
			}
		}

		if( p1 && strlen( p1 ) )
		{
			strcpy(neigh_info->neighbours[ current_neigh_info_index ].ipv6_addr,p1);
		}
		else
		{
			neigh_info->neighbours[ current_neigh_info_index ].ipv6_addr[0]='\0';
		}

		p1=strstr(p2+1,"lladdr");
		for( p2=p1;p2&&( *p2!=0 );p2++ )
		{
			if( ( *p2==' ' ) || ( *p2=='\t' ) || ( *p2=='\n' ) || ( *p2=='/' ) )
			{
				break;
			}
		}

		for(p1=p2;p1&&( *p1 != 0 );p1++)
		{
			if( ( *p1!=' ' ) && ( *p1!='\t' ) )
			{
				break;
			}
		}

		for( p2=p1;p2&&( *p2!=0 );p2++ )
		{
			if( ( *p2==' ' ) || ( *p2=='\t' ) || ( *p2=='\n' ) || ( *p2=='/' ) )
			{
				*p2=0;
				break;
			}
		}

		if( p1 && strlen( p1 ) )
		{
			strcpy(neigh_info->neighbours[ current_neigh_info_index ].mac_addr,p1);
		}
		else
		{
			neigh_info->neighbours[ current_neigh_info_index ].mac_addr[0]='\0';
		}


		neigh_info->neighbour_count+=1;

		if( neigh_info->neighbour_count >= 10 )
		{
			break;
		}
	}
	fclose( file_handle );

	return 0;
}



/*
 *3GPP mobile cellular networks such as GSM, UMTS, and LTE have
architectural support for IPv6 [RFC6459], but only 3GPP Release-10
and onwards of the 3GPP specification supports DHCPv6 Prefix
Delegation [RFC3633] for delegating IPv6 prefixes to a single LAN
link. To facilitate the use of IPv6 in a LAN prior to the deployment
of DHCPv6 Prefix Delegation in 3GPP networks and in User Equipment
(UE), this document describes how the 3GPP UE radio interface
assigned global /64 prefix may be extended from the 3GPP radio
interface to a LAN link. This is achieved by receiving the Router
Advertisement (RA) [RFC4861] announced globally unique /64 IPv6
prefix from the 3GPP radio interface and then advertising the same
IPv6 prefix to the LAN link with RA. For all of the cases in the
scope of this document, the UE may be any device that functions as an
IPv6 router between the 3GPP network and a LAN.

The 3GPP UE maintains the same consistent 128 bit global scope
IPv6 anycast address [RFC4291] on the 3GPP radio interface and the
LAN link. The LAN link is configured as a /64 and the 3GPP radio
interface is configured as a /128.

2. The Challenge of Providing IPv6 Addresses to a LAN link via a 3GPP UE
As described in [RFC6459], 3GPP networks assign a /64 global scope
prefix to each UE using RA. DHCPv6 Prefix Delegation is an optional
part of 3GPP Release-10 and is not covered by any earlier releases.
Neighbor Discovery Proxy (ND Proxy) [RFC4389] functionality has been
suggested as an option for extending the assigned /64 from the 3GPP
radio interface to the LAN link, but ND Proxy is an experimental
protocol and has some limitations with loop-avoidance.
DHCPv6 is the best way to delegate a prefix to a LAN link. The
methods described in this document should only be applied when
deploying DHCPv6 Prefix Delegation is not achievable in the 3GPP
network and the UE. The methods described in this document are at
various stages of implementation and deployment planning. The goal
of this memo is to document the available methods which may be used
prior to DHCPv6 deployment.


3.1 General Behavior for All Scenarios
As [RFC6459] describes, the 3GPP network assigned /64 is completely
dedicated to the UE and the gateway does not consume any of the /64
addresses. The gateway routes the entire /64 to the UE and does not
perform ND or Network Unreachability Detection (NUD) [RFC4861].
Communication between the UE and the gateway is only done using link-
local addresses and the link is point-to-point. This allows for the
UE to reliably manipulate the /64 from the 3GPP radio interface
without negatively impacting the point-to-point 3GPP radio link
interface. The LAN link RA configuration must be tightly coupled
with the 3GPP link state. If the 3GPP link goes down or changes the
IPv6 prefix, that state should be reflected in the LAN link IPv6
configuration. Just as in a standard IPv6 router, the packet TTL
will be decremented when passing packets between IPv6 links across
the UE. The UE is employing the weak host model. The RA function on
the UE is exclusively run on the LAN link.
The LAN link originated RA message carries a copy of the following
3GPP radio link received RA message option fields:


MTU (if not provided by the 3GPP network, the UE will provide its
3GPP link MTU size)
Prefix Information

3.3 Scenario 2: A Single Global Address Assigned to 3GPP Radio and LAN
link
In this method, the UE assigns itself one address from the 3GPP
network RA announced /64. This one address is configured as anycast
[RFC4291] on both the 3GPP radio link as a /128 and on the LAN link
as a /64. This allows the UE to maintain long lived data connections
since the 3GPP radio interface address does not change when the
router function is activated. This method may cause complications
for certain software that may not support multiple interfaces with
the same anycast IPv6 address or are sensitive to prefix length
changes. This method also creates complications for ensuring
uniqueness for Privacy Extensions [RFC4941]. When Privacy Extensions
are in use all temporary addresses will be copied from the 3GPP radio
interface to the LAN link and the preferred and valid lifetimes will

be synchronized, such that the temporary anycast addresses on both
interfaces expire simultaneously.
There might also be more complex scenarios in which the prefix length
is not changed and privacy extensions are supported by having the
subnet span multiple interfaces, as ND Proxy does [RFC4389]. Further
elaboration is out of scope of the present document.

Below is the general procedure for this scenario:
1. The user activates router functionality for a LAN on the UE.
2. The UE checks to make sure the 3GPP interface is active and has
  an IPv6 address. If the interface does not have an IPv6 address,
 an attempt will be made to acquire one, or else the procedure
will terminate.
3. In this example, the UE finds the 3GPP interface has the IPv6
  address 2001:db8:ac10:f002:1234:4567:0:9 assigned and active.

4. The UE moves the address 2001:db8:ac10:f002:1234:4567:0:9 as an
  anycast /64 from the 3GPP interface to the LAN interface and
 begins announcing the prefix 2001:db8:ac10:f002::/64 via RA to
the LAN. The 3GPP interface maintains the same IPv6 anycast
address with a /128. For this example, the LAN has
2001:db8:ac10:f002:1234:4567:0:9/64 and the 3GPP radio interface
has 2001:db8:ac10:f002:1234:4567:0:9/128.
5. The UE directly processes all packets destined to itself at
  2001:db8:ac10:f002:1234:4567:0:9.
6. On the LAN interface, there is no chance of address conflict
  since the address is defended using DAD. The 3GPP radio
 interface only has a /128 and no other systems on the 3GPP radio
point-to-point link may use the global /64.


 * */
void config_enable_sharing_64prefix_to_a_lan(const char* ipv6_addr,const char* network_dev_name)
{
	char buffer[256];
	//config global ipv6 addr
	snprintf(buffer
			,sizeof( buffer )
			,"ip -f inet6 addr add %s/128 dev %s scope global"
			,ipv6_addr
			,network_dev_name
			);
	system(buffer);

	snprintf(buffer
			,sizeof( buffer )
			,"ip -f inet6 addr add %s/64 dev br0 scope global"
			,ipv6_addr
			);
	system(buffer);
}

void config_ipv6_addr(const char*prefix,EUI64* eui64_addr,const char* network_dev_name,int modem_get_interface)
{
	char buffer[256];
	char ipv6[128];
	struct in6_addr local_addr6;
	struct in6_addr local_addr6_2;

	//delete global ipv6 addr
	snprintf(buffer
			,sizeof( buffer )
			,"ip -f inet6 addr flush dev %s scope global"
			,network_dev_name
			);
	system(buffer);
	//reconfig br0's ipv6 address
	system("ip -f inet6 addr flush dev br0 scope global");

	if ( !inet_pton(AF_INET6,prefix,&local_addr6) )
	{
		return;
	}

	if( modem_get_interface )
	{
		if( !util_read_interface_id(&local_addr6_2) )
		{
			local_addr6.s6_addr[8]=local_addr6_2.s6_addr[8];
			local_addr6.s6_addr[9]=local_addr6_2.s6_addr[9];
			local_addr6.s6_addr[10]=local_addr6_2.s6_addr[10];
			local_addr6.s6_addr[11]=local_addr6_2.s6_addr[11];
			local_addr6.s6_addr[12]=local_addr6_2.s6_addr[12];
			local_addr6.s6_addr[13]=local_addr6_2.s6_addr[13];
			local_addr6.s6_addr[14]=local_addr6_2.s6_addr[14];
			local_addr6.s6_addr[15]=local_addr6_2.s6_addr[15];
		}
	}
	else if( eui64_addr )
	{
		local_addr6.s6_addr[8]=eui64_addr->value[0];
		local_addr6.s6_addr[9]=eui64_addr->value[1];
		local_addr6.s6_addr[10]=eui64_addr->value[2];
		local_addr6.s6_addr[11]=eui64_addr->value[3];
		local_addr6.s6_addr[12]=eui64_addr->value[4];
		local_addr6.s6_addr[13]=eui64_addr->value[5];
		local_addr6.s6_addr[14]=eui64_addr->value[6];
		local_addr6.s6_addr[15]=eui64_addr->value[7];
	}

	memset( ipv6,0,sizeof(ipv6) );
	snprintf(ipv6
			,sizeof( ipv6 )
			,"%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x"
			,local_addr6.s6_addr[0]
			,local_addr6.s6_addr[1]
			,local_addr6.s6_addr[2]
			,local_addr6.s6_addr[3]
			,local_addr6.s6_addr[4]
			,local_addr6.s6_addr[5]
			,local_addr6.s6_addr[6]
			,local_addr6.s6_addr[7]
			,local_addr6.s6_addr[8]
			,local_addr6.s6_addr[9]
			,local_addr6.s6_addr[10]
			,local_addr6.s6_addr[11]
			,local_addr6.s6_addr[12]
			,local_addr6.s6_addr[13]
			,local_addr6.s6_addr[14]
			,local_addr6.s6_addr[15]
			);

	//enable share 64bit prefix function
	config_enable_sharing_64prefix_to_a_lan(ipv6,network_dev_name);
}


void config_default_ipv6_gateway(const char* ipv6_gateway,const char* network_dev_name)
{
	char buffer[256];

	//delete default ipv6 gateway
	system("ip -6 route del ::1/0 1>/dev/null 2>&1");
	system("ip -6 route del ::1/0 1>/dev/null 2>&1");

	//config default ipv6 gateway
	snprintf(buffer
			,sizeof( buffer )
			,"route -A inet6 add ::1/0 gw %s dev %s"
			,ipv6_gateway
			,network_dev_name
			);
	system(buffer);
}

//config dns server addr
void config_ipv6_dns_server_address(const char* global_dns_server1,const char* global_dns_server2,int dns_server_count,int nat64_mode)
{
	FILE* file_handle;
	FILE* tmp_file_handle;
	char temp_buffer[256];

	while( TRUE )
	{
		//udhcpc is setting the dns config file
		if( !access(UDHCP_DNS_SYNC_FILE,R_OK) )
		{
			sleep(1);
			continue;
		}

		//dhclient is setting the dns config file
		if( !access(DHCLIENT_DNS_SYNC_FILE,R_OK) )
		{
			sleep(1);
			continue;
		}

		file_handle=fopen(DNS_CONFIG_FILE,"r");
		tmp_file_handle=fopen(DNS_CONFIG_TEMP_FILE,"wb");
		memset( temp_buffer,0,sizeof(temp_buffer) );
		while( fgets( temp_buffer,sizeof(temp_buffer)-1,file_handle ) != NULL )
		{
			//this is ipv6 dns server config
			if( strchr(temp_buffer,':') == NULL )
			{
				fwrite(temp_buffer,strlen(temp_buffer),1,tmp_file_handle);
			}

			memset( temp_buffer,0,sizeof(temp_buffer) );
		}

		snprintf(temp_buffer
				,sizeof( temp_buffer )
				,"nameserver %s\n"
				,global_dns_server1
				);
		fwrite(temp_buffer,strlen(temp_buffer),1,tmp_file_handle);

		if( dns_server_count > 1 )
		{
			snprintf(temp_buffer
					,sizeof( temp_buffer )
					,"nameserver %s\n"
					,global_dns_server2
					);
			fwrite(temp_buffer,strlen(temp_buffer),1,tmp_file_handle);
		}

		//close the file handle
		fclose(file_handle);
		fclose(tmp_file_handle);

		//delete the old file
		cmd_rm(DNS_CONFIG_FILE);
		//rename the file
		rename(DNS_CONFIG_TEMP_FILE,DNS_CONFIG_FILE);
		cmd_chmodrw( DNS_CONFIG_FILE );

		if( nat64_mode )
		{
			system("killall rc.ipv6.totd");
			system("/etc/rc.d/rc.ipv6.totd");
		}

		//将IPv6 DNS服务器列表写入文件
		{
			if( dns_server_count > 1 )
			{
				snprintf(temp_buffer
						,sizeof( temp_buffer )
						,"%s %s"
						,global_dns_server1
						,global_dns_server2
						);
			}
			else
			{
				snprintf(temp_buffer
						,sizeof( temp_buffer )
						,"%s"
						,global_dns_server1
						);
			}

			cmd_echo_without_new_line( temp_buffer,IPV6_DNS_SERVER_LIST );


			if( dns_server_count > 1 )
			{
				snprintf(temp_buffer
						,sizeof( temp_buffer )
						,"%s,%s"
						,global_dns_server1
						,global_dns_server2
						);
			}
			else
			{
				snprintf(temp_buffer
						,sizeof( temp_buffer )
						,"%s"
						,global_dns_server1
						);
			}

			cmd_echo_without_new_line( temp_buffer,IPV6_DNS_SERVER_LIST_WITH_COMMA );
		}

		//下发从服务器获取的IPv6 DNS服务器信息,这个是移动测试规范规定的
		//if( is_acquired_ipv6_dns_info_used() )
		{
			//reenable radvd
			system("killall rc.ipv6.radvd");
			system("killall rc.ipv6.dhcp6s");
			//start advertise the router info
			system("/etc/rc.d/rc.ipv6.radvd &");
			//start dhcp6s
			system("/etc/rc.d/rc.ipv6.dhcp6s &");
		}
		break;
	}
}

//write interface info to file;
int util_write_interface_id(struct in6_addr* local_addr6,const char* network_dev_name )
{
	char buffer[256];
	struct in6_addr last_ipv6_addr;

	//config usb0's link local address
	if( !util_read_interface_id( &last_ipv6_addr ) )
	{
		if(	( local_addr6->s6_addr[8] != last_ipv6_addr.s6_addr[8] )
			||( local_addr6->s6_addr[9] != last_ipv6_addr.s6_addr[9] )
			||( local_addr6->s6_addr[10] != last_ipv6_addr.s6_addr[10] )
			||( local_addr6->s6_addr[11] != last_ipv6_addr.s6_addr[11] )
			||( local_addr6->s6_addr[12] != last_ipv6_addr.s6_addr[12] )
			||( local_addr6->s6_addr[13] != last_ipv6_addr.s6_addr[13] )
			||( local_addr6->s6_addr[14] != last_ipv6_addr.s6_addr[14] )
			||( local_addr6->s6_addr[15] != last_ipv6_addr.s6_addr[15] )
			)
		{
			//config link local address
			snprintf(buffer
					,sizeof( buffer )
					,"ip -f inet6 addr flush dev %s scope link"
					,network_dev_name
					);
			system( buffer );
			//config link local address
			snprintf(buffer
					,sizeof( buffer )
						,"ip -f inet6 addr add fe80::%02x%02x:%02x%02x:%02x%02x:%02x%02x/64 dev %s scope link"
						,local_addr6->s6_addr[8]
						,local_addr6->s6_addr[9]
						,local_addr6->s6_addr[10]
						,local_addr6->s6_addr[11]
						,local_addr6->s6_addr[12]
						,local_addr6->s6_addr[13]
						,local_addr6->s6_addr[14]
						,local_addr6->s6_addr[15]
						,network_dev_name
						);
			system( buffer );
		}
	}
	else
	{
		//config link local address
		snprintf(buffer
				,sizeof( buffer )
				,"ip -f inet6 addr flush dev %s scope link"
				,network_dev_name
				);
		system( buffer );
		//config link local address
		snprintf(buffer
				,sizeof( buffer )
					,"ip -f inet6 addr add fe80::%02x%02x:%02x%02x:%02x%02x:%02x%02x/64 dev %s scope link"
					,local_addr6->s6_addr[8]
					,local_addr6->s6_addr[9]
					,local_addr6->s6_addr[10]
					,local_addr6->s6_addr[11]
					,local_addr6->s6_addr[12]
					,local_addr6->s6_addr[13]
					,local_addr6->s6_addr[14]
					,local_addr6->s6_addr[15]
					,network_dev_name
					);
		system( buffer );
	}

	snprintf(buffer
			,sizeof( buffer )
			,"::%02x%02x:%02x%02x:%02x%02x:%02x%02x"
			,local_addr6->s6_addr[8]
			,local_addr6->s6_addr[9]
			,local_addr6->s6_addr[10]
			,local_addr6->s6_addr[11]
			,local_addr6->s6_addr[12]
			,local_addr6->s6_addr[13]
			,local_addr6->s6_addr[14]
			,local_addr6->s6_addr[15]
			);
	return cmd_echo(buffer,INTERFACE_ID_TEMP_FILE);
}

int util_read_interface_id(struct in6_addr* ipv6_addr)
{
	char buffer[256];


	int ret=cmd_cat(INTERFACE_ID_TEMP_FILE,buffer,sizeof(buffer));
	if( !ret )
	{
		char* ptr=strstr(buffer,"\n");
		if( ptr )
		{
			*ptr='\0';
		}

		ret=!inet_pton(AF_INET6,buffer,ipv6_addr);
	}
	return ret;
}


/* function: ip_checksum_add
 * adds data to a checksum
 * current_sum - the current checksum (or 0 to start a new checksum)
 * data        - the data to add to the checksum
 * len         - length of data
 */
unsigned short ip_checksum(const void *data, int len)
{
	unsigned short checksum = 0;
	int left = len;
	const unsigned char *data_8 = data;

	while(left > 1)
	{
		checksum += ( ( *data_8 )<<8 )+( *(data_8+1) );
		data_8 += 2;
		left -= 2;
	}

	if(left)
	{
		checksum += ( ( *( unsigned char* )data_8 )<<8 );
	}

	while(checksum > 0xffff)
	{
		checksum = (checksum >> 16) + (checksum & 0xFFFF);
	}

	checksum = (~checksum) & 0xffff;

	return htons(checksum);
}
#endif

#ifndef WIN32
static unsigned int random_generator_array[]={0x89abcdef,0x9abcdef8,0xabcdef89,0xbcdef89a,0xcdef89ab,0xdef89abc,0xef89abcd,0xf89abcde};
unsigned int util_clac_random_response(unsigned int random_value)
{
	unsigned int verify_value = random_value;
	unsigned int tmp_value;

	verify_value = verify_value ^ random_generator_array[ verify_value&0x07 ];
	tmp_value = verify_value >> 24;
	verify_value = ( verify_value&0x00ffffff )+(( verify_value&0xff )<<24);
	verify_value = ( verify_value&0xffffff00 )+tmp_value;
	return verify_value;
}
#endif

char* util_get_field_value_seperated_by_equal(char* tmp_buffer)
{
	char* p1;
	char* p2;
	int have_quote=FALSE;

	p1=strchr(tmp_buffer,'=');
	p1++;
	for(;p1&&( *p1 != 0 );p1++)
	{
		if( *p1 == '\"' )
		{
			have_quote=TRUE;
		}

		if( ( *p1!=' ' ) && ( *p1!='\t' )&& ( *p1!='\"' ) )
		{
			break;
		}
	}

	for( p2=p1;p2&&( *p2!=0 );p2++ )
	{
		if( ( ( ( *p2==' ' ) || ( *p2=='\t' ) ) && !have_quote )
				|| ( *p2=='\r' )
				|| ( *p2=='\n' )
				|| ( *p2=='\"' )
			)
		{
			*p2=0;
			break;
		}
	}

	return p1;
}


static const FieldConfigInfo all_need_parsed_fields[]=
{
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_FORBID_ROAMING ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_SAVE_LOCK_CELL_AFTER_TIMEOUT ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_NETWORK_MODE_NOT_SUPPORT ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_URGENT_HEARTBEAT_INTERVAL ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_PPP_AUTH_TYPE ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_4G_FDD_PREFERRED ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_CHECK_IF_LNS_ALIVE ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_NTP_SERVER2 ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_NTP_SERVER3 ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_NTP_SERVER4 ),

	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_ENABLE_LOCK_NETWORK ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_LOCK_NETWORK_PLMN ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_LOCK_NETWORK_PLMN2 ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_LOCK_NETWORK_PLMN3 ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_LOCK_NETWORK_PLMN4 ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_LOCK_NETWORK_PLMN5 ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_LOCK_NETWORK_PLMN6 ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_LOCK_NETWORK_PLMN7 ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_LOCK_NETWORK_PLMN8 ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_LOCK_NETWORK_PLMN9 ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_LOCK_NETWORK_PLMN10 ),

	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_ADMIN_RANDOM_PASSWORD ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_SPECIAL_SSID_PREFIX ),

	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_CS_PS_PREFERRED_MODE ),

	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_ENABLE_SET_REBOOT ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_SET_REBOOT_HOURS ),

	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_SIP_IP ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_SIP_PORT ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_SIP_DOMAIN ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_SIP_DOMAIN_PORT ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_SIP_DOMAIN_RANGE ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_SIP_PROXY_ENABLED ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_SIP_PROXY_IP ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_SIP_PROXY_PORT ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_SIP_DISPLAY_NAME ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_SIP_USERNAME ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_SIP_AUTH_NAME ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_SIP_AUTH_PASSWD ),

	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_ANTENNA_MODE ),

	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_WPS_IS_ENABLE ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_WPS_MODE ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_CLIENT_PIN ),

	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_ENABLE_12_13_AUTO_CHANNEL ),

	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_ENABLE_DDOS ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_ENABLE_DEFENCE_SYN_FLOOD ),
	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_ENABLE_DEFENCE_PING_OF_DEATH ),

	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_TELNET_SERVER_ENABLE ),

	DEFINE_ONE_FIELDCONFIGINFO_STRUCT( TZ_IP_PASS_THROUGH_ENABLE ),

};

#define ALL_NEED_PARSED_FIELDS_COUNT ( sizeof( all_need_parsed_fields )/sizeof( all_need_parsed_fields[0] ) )

//sublime text replace
//(if\(\s*!strncmp\(\s*p1,"([^\"]*)",p2-p1\s*\)\s*\)\n\t\t\{)
//\1\n\t\t\tinfostruct_is_field_defined( server_wifi_info,\2_DEFINED )
//read wifi info
int util_server_read_wifi_info_ex( const char* config_file,InfoStruct* server_wifi_info )
{
	char* p1;
	char* p2;
	int size;
	char* nvram_config_field;
	char temp_buffer[769];
	FILE* file_handle=fopen( config_file,"r" );

	if( file_handle == NULL )
	{
		return -1;
	}

	memset( temp_buffer,0,sizeof(temp_buffer) );
	//export AP_SEC_DEBUG=0
	//export TZ_DM_DEVICE_MANUFACTURER=SZTOZED
	//export TZ_MODEM_GET_IPV6="-nmipv6"
	//export TZ_MODEM_CGCONTRDP_INTERFACE="-nintf"
	//export AP_SECMODE=WPA
	//export AP_SECFILE=PSK
	while( fgets( temp_buffer,sizeof(temp_buffer)-1,file_handle ) != NULL )
	{
		p1=strstr(temp_buffer,"export");
		if( p1 == NULL )
		{
			continue;
		}
		p2=temp_buffer+strlen("export");

		for(p1=p2;p1&&( *p1 != 0 );p1++)
		{
			if( ( *p1!=' ' ) && ( *p1!='\t' ) )
			{
				break;
			}
		}

		for( p2=p1;p2&&( *p2!=0 );p2++ )
		{
			if( ( *p2==' ' ) || ( *p2=='\t' ) || ( *p2==':' ) || ( *p2=='=' )|| ( *p2=='\n' ) )
			{
				break;
			}
		}

		//export AP_SSID="tozed-11274F"
		if( !strncmp( p1,"AP_SSID",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,AP_SSID_DEFINED )
			size=sizeof( server_wifi_info->AP_SSID );
			nvram_config_field=server_wifi_info->AP_SSID;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export TZ_ENABLE_WIFI=yes
		else if( !strncmp( p1,"TZ_ENABLE_WIFI",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENABLE_WIFI_DEFINED )
			size=sizeof( server_wifi_info->TZ_ENABLE_WIFI );
			nvram_config_field=server_wifi_info->TZ_ENABLE_WIFI;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export PWD=6A7FCADA
		else if( !strncmp( p1,"PWD",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,PSK_KEY_DEFINED )
			size=sizeof( server_wifi_info->PSK_KEY );
			nvram_config_field=server_wifi_info->PSK_KEY;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export AP_CYPHER=CCMP
		else if( !strncmp( p1,"AP_CYPHER",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,AP_CYPHER_DEFINED )
			size=sizeof( server_wifi_info->AP_CYPHER );
			nvram_config_field=server_wifi_info->AP_CYPHER;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export AP_WPA=2
		else if( !strncmp( p1,"AP_WPA",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,AP_WPA_DEFINED )
			size=sizeof( server_wifi_info->AP_WPA );
			nvram_config_field=server_wifi_info->AP_WPA;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export PUREG=0
		else if( !strncmp( p1,"PUREG",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,PUREG_DEFINED )
			size=sizeof( server_wifi_info->PUREG );
			nvram_config_field=server_wifi_info->PUREG;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export RATECTL=manual
		else if( !strncmp( p1,"RATECTL",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,RATECTL_DEFINED )
			size=sizeof( server_wifi_info->RATECTL );
			nvram_config_field=server_wifi_info->RATECTL;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export MANRATE=0x87868584
		else if( !strncmp( p1,"MANRATE",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,MANRATE_DEFINED )
			size=sizeof( server_wifi_info->MANRATE );
			nvram_config_field=server_wifi_info->MANRATE;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export MANRETRIES=0x04040404
		else if( !strncmp( p1,"MANRETRIES",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,MANRETRIES_DEFINED )
			size=sizeof( server_wifi_info->MANRETRIES );
			nvram_config_field=server_wifi_info->MANRETRIES;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export WIFIWORKMODE=m11b
		else if( !strncmp( p1,"WIFIWORKMODE",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,WIFIWORKMODE_DEFINED )
			size=sizeof( server_wifi_info->WIFIWORKMODE );
			nvram_config_field=server_wifi_info->WIFIWORKMODE;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export PUREN=0
		else if( !strncmp( p1,"PUREN",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,PUREN_DEFINED )
			size=sizeof( server_wifi_info->PUREN );
			nvram_config_field=server_wifi_info->PUREN;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export AP_SECMODE=WPA
		else if( !strncmp( p1,"AP_SECMODE",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,AP_SECMODE_DEFINED )
			size=sizeof( server_wifi_info->AP_SECMODE );
			nvram_config_field=server_wifi_info->AP_SECMODE;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export AP_SECFILE=PSK
		else if( !strncmp( p1,"AP_SECFILE",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,AP_SECFILE_DEFINED )
			size=sizeof( server_wifi_info->AP_SECFILE );
			nvram_config_field=server_wifi_info->AP_SECFILE;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export AP_WPA_GROUP_REKEY=600
		else if( !strncmp( p1,"AP_WPA_GROUP_REKEY",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,AP_WPA_GROUP_REKEY_DEFINED )
			size=sizeof( server_wifi_info->AP_WPA_GROUP_REKEY );
			nvram_config_field=server_wifi_info->AP_WPA_GROUP_REKEY;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export AP_WPA_GMK_REKEY=86400
		else if( !strncmp( p1,"AP_WPA_GMK_REKEY",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,AP_WPA_GMK_REKEY_DEFINED )
			size=sizeof( server_wifi_info->AP_WPA_GMK_REKEY );
			nvram_config_field=server_wifi_info->AP_WPA_GMK_REKEY;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export AP_WEP_MODE_0=2
		else if( !strncmp( p1,"AP_WEP_MODE_0",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,AP_WEP_MODE_0_DEFINED )
			size=sizeof( server_wifi_info->AP_WEP_MODE_0 );
			nvram_config_field=server_wifi_info->AP_WEP_MODE_0;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export AP_PRIMARY_KEY_0=1
		else if( !strncmp( p1,"AP_PRIMARY_KEY_0",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,AP_PRIMARY_KEY_0_DEFINED )
			size=sizeof( server_wifi_info->AP_PRIMARY_KEY_0 );
			nvram_config_field=server_wifi_info->AP_PRIMARY_KEY_0;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export WEP_RADIO_NUM0_KEY_1="abcdefghijklm"
		else if( !strncmp( p1,"WEP_RADIO_NUM0_KEY_1",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,WEP_RADIO_NUM0_KEY_1_DEFINED )
			size=sizeof( server_wifi_info->WEP_RADIO_NUM0_KEY_1 );
			nvram_config_field=server_wifi_info->WEP_RADIO_NUM0_KEY_1;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export WEP_RADIO_NUM0_KEY_2="abcdefghijklm"
		else if( !strncmp( p1,"WEP_RADIO_NUM0_KEY_2",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,WEP_RADIO_NUM0_KEY_2_DEFINED )
			size=sizeof( server_wifi_info->WEP_RADIO_NUM0_KEY_2 );
			nvram_config_field=server_wifi_info->WEP_RADIO_NUM0_KEY_2;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export WEP_RADIO_NUM0_KEY_3="abcdefghijklm"
		else if( !strncmp( p1,"WEP_RADIO_NUM0_KEY_3",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,WEP_RADIO_NUM0_KEY_3_DEFINED )
			size=sizeof( server_wifi_info->WEP_RADIO_NUM0_KEY_3 );
			nvram_config_field=server_wifi_info->WEP_RADIO_NUM0_KEY_3;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export WEP_RADIO_NUM0_KEY_4="abcdefghijklm"
		else if( !strncmp( p1,"WEP_RADIO_NUM0_KEY_4",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,WEP_RADIO_NUM0_KEY_4_DEFINED )
			size=sizeof( server_wifi_info->WEP_RADIO_NUM0_KEY_4 );
			nvram_config_field=server_wifi_info->WEP_RADIO_NUM0_KEY_4;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export AP_HIDESSID=0
		else if( !strncmp( p1,"AP_HIDESSID",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,AP_HIDESSID_DEFINED )
			size=sizeof( server_wifi_info->AP_HIDESSID );
			nvram_config_field=server_wifi_info->AP_HIDESSID;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export AP_IPADDR="192.168.0.1"
		else if( !strncmp( p1,"AP_IPADDR",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,AP_IPADDR_DEFINED )
			size=sizeof( server_wifi_info->AP_IPADDR );
			nvram_config_field=server_wifi_info->AP_IPADDR;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export AP_NETMASK="255.255.255.0"
		else if( !strncmp( p1,"AP_NETMASK",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,AP_NETMASK_DEFINED )
			size=sizeof( server_wifi_info->AP_NETMASK );
			nvram_config_field=server_wifi_info->AP_NETMASK;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export TZ_DHCP_IP_BEGIN="192.168.0.100"
		else if( !strncmp( p1,"TZ_DHCP_IP_BEGIN",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_DHCP_IP_BEGIN_DEFINED )
			size=sizeof( server_wifi_info->TZ_DHCP_IP_BEGIN );
			nvram_config_field=server_wifi_info->TZ_DHCP_IP_BEGIN;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export TZ_DHCP_IP_END="192.168.0.250"
		else if( !strncmp( p1,"TZ_DHCP_IP_END",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_DHCP_IP_END_DEFINED )
			size=sizeof( server_wifi_info->TZ_DHCP_IP_END );
			nvram_config_field=server_wifi_info->TZ_DHCP_IP_END;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export TZ_DHCP_EXPIRE_TIME=86400
		else if( !strncmp( p1,"TZ_DHCP_EXPIRE_TIME",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_DHCP_EXPIRE_TIME_DEFINED )
			size=sizeof( server_wifi_info->TZ_DHCP_EXPIRE_TIME );
			nvram_config_field=server_wifi_info->TZ_DHCP_EXPIRE_TIME;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export AP_PRIMARY_CH=11
		else if( !strncmp( p1,"AP_PRIMARY_CH",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,AP_PRIMARY_CH_DEFINED )
			size=sizeof( server_wifi_info->AP_PRIMARY_CH );
			nvram_config_field=server_wifi_info->AP_PRIMARY_CH;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export AP_CHMODE=11NGHT20
		else if( !strncmp( p1,"AP_CHMODE",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,AP_CHMODE_DEFINED )
			size=sizeof( server_wifi_info->AP_CHMODE );
			nvram_config_field=server_wifi_info->AP_CHMODE;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export TXPOWER=18
		else if( !strncmp( p1,"TXPOWER",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TXPOWER_DEFINED )
			size=sizeof( server_wifi_info->TXPOWER );
			nvram_config_field=server_wifi_info->TXPOWER;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		//export TZ_ENABLE_WATCHDOG=no
		else if( !strncmp( p1,"TZ_ENABLE_WATCHDOG",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENABLE_WATCHDOG_DEFINED )
			size=sizeof( server_wifi_info->TZ_ENABLE_WATCHDOG );
			nvram_config_field=server_wifi_info->TZ_ENABLE_WATCHDOG;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		else if( !strncmp( p1,"TZ_SPEED_PREFERRED",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_SPEED_PREFERRED_DEFINED )
			STRCPY_S( server_wifi_info->TZ_SPEED_PREFERRED,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_LOCK_CARD",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LOCK_CARD_DEFINED )
			size=sizeof( server_wifi_info->TZ_LOCK_CARD );
			nvram_config_field=server_wifi_info->TZ_LOCK_CARD;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		else if( !strncmp( p1,"TZ_LOCK_CARD_TYPE",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LOCK_CARD_TYPE_DEFINED )
			size=sizeof( server_wifi_info->TZ_LOCK_CARD_TYPE );
			nvram_config_field=server_wifi_info->TZ_LOCK_CARD_TYPE;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		else if( !strncmp( p1,"TZ_INITIAL_PIN_CODE",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_INITIAL_PIN_CODE_DEFINED )
			size=sizeof( server_wifi_info->TZ_INITIAL_PIN_CODE );
			nvram_config_field=server_wifi_info->TZ_INITIAL_PIN_CODE;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		else if( !strncmp( p1,"TZ_MODIFIED_PIN_CODE",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_MODIFIED_PIN_CODE_DEFINED )
			size=sizeof( server_wifi_info->TZ_MODIFIED_PIN_CODE );
			nvram_config_field=server_wifi_info->TZ_MODIFIED_PIN_CODE;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		else if( !strncmp( p1,"TZ_LOCK_CELL_ID",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LOCK_CELL_ID_DEFINED )
			size=sizeof( server_wifi_info->TZ_LOCK_CELL_ID );
			nvram_config_field=server_wifi_info->TZ_LOCK_CELL_ID;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		else if( !strncmp( p1,"TZ_LOCK_CELL_ID_COUNT",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LOCK_CELL_ID_COUNT_DEFINED )
			size=sizeof( server_wifi_info->TZ_LOCK_CELL_ID_COUNT );
			nvram_config_field=server_wifi_info->TZ_LOCK_CELL_ID_COUNT;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		else if( !strncmp( p1,"TZ_LIMIT_SPEED_ENABLED",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LIMIT_SPEED_ENABLED_DEFINED )
			size=sizeof( server_wifi_info->TZ_LIMIT_SPEED_ENABLED );
			nvram_config_field=server_wifi_info->TZ_LIMIT_SPEED_ENABLED;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		else if( !strncmp( p1,"TZ_LIMIT_SPEED_VALUE",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LIMIT_SPEED_VALUE_DEFINED )
			size=sizeof( server_wifi_info->TZ_LIMIT_SPEED_VALUE );
			nvram_config_field=server_wifi_info->TZ_LIMIT_SPEED_VALUE;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		else if( !strncmp( p1,"TZ_L2TP_MODE",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_L2TP_MODE_DEFINED )
			size=sizeof( server_wifi_info->TZ_L2TP_MODE );
			nvram_config_field=server_wifi_info->TZ_L2TP_MODE;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		else if( !strncmp( p1,"TZ_LNS_SERVER",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LNS_SERVER_DEFINED )
			size=sizeof( server_wifi_info->TZ_LNS_SERVER );
			nvram_config_field=server_wifi_info->TZ_LNS_SERVER;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		else if( !strncmp( p1,"TZ_TUNNEL_NAME",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_TUNNEL_NAME_DEFINED )
			size=sizeof( server_wifi_info->TZ_TUNNEL_NAME );
			nvram_config_field=server_wifi_info->TZ_TUNNEL_NAME;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		else if( !strncmp( p1,"TZ_TUNNEL_PASS",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_TUNNEL_PASS_DEFINED )
			size=sizeof( server_wifi_info->TZ_TUNNEL_PASS );
			nvram_config_field=server_wifi_info->TZ_TUNNEL_PASS;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		else if( !strncmp( p1,"TZ_AUTH_NAME",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_AUTH_NAME_DEFINED )
			size=sizeof( server_wifi_info->TZ_AUTH_NAME );
			nvram_config_field=server_wifi_info->TZ_AUTH_NAME;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		else if( !strncmp( p1,"TZ_AUTH_PASS",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_AUTH_PASS_DEFINED )
			size=sizeof( server_wifi_info->TZ_AUTH_PASS );
			nvram_config_field=server_wifi_info->TZ_AUTH_PASS;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		else if( !strncmp( p1,"TZ_CONFIG_FILE_VERSION",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_CONFIG_FILE_VERSION_DEFINED )
			size=sizeof( server_wifi_info->TZ_CONFIG_FILE_VERSION );
			nvram_config_field=server_wifi_info->TZ_CONFIG_FILE_VERSION;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		else if( !strncmp( p1,"TZ_CONFIG_FILE_DESCRIPTION",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_CONFIG_FILE_DESCRIPTION_DEFINED )
			size=sizeof( server_wifi_info->TZ_CONFIG_FILE_DESCRIPTION );
			nvram_config_field=server_wifi_info->TZ_CONFIG_FILE_DESCRIPTION;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		else if( !strncmp( p1,"TZ_ENABLE_PPP_FORWARD",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENABLE_PPP_FORWARD_DEFINED )
			size=sizeof( server_wifi_info->TZ_ENABLE_PPP_FORWARD );
			nvram_config_field=server_wifi_info->TZ_ENABLE_PPP_FORWARD;
			nvram_config_field[ size-1 ]=0;
			strncpy( nvram_config_field, util_get_field_value_seperated_by_equal(temp_buffer),size-1 );
		}
		else if( !strncmp( p1,"LAN_LED_AS_LTE_STATUS",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,LAN_LED_AS_LTE_STATUS_DEFINED )
			STRCPY_S( server_wifi_info->LAN_LED_AS_LTE_STATUS,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_DDNS_HSK_USERNAME",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_DDNS_HSK_USERNAME_DEFINED )
			STRCPY_S( server_wifi_info->TZ_DDNS_HSK_USERNAME,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_DDNS_HSK_PASSWORD",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_DDNS_HSK_PASSWORD_DEFINED )
			STRCPY_S( server_wifi_info->TZ_DDNS_HSK_PASSWORD,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_DDNS_INTERFACE",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_DDNS_INTERFACE_DEFINED )
			STRCPY_S( server_wifi_info->TZ_DDNS_INTERFACE,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENABLE_HSK_DDNS_SERVER",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENABLE_HSK_DDNS_SERVER_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENABLE_HSK_DDNS_SERVER,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENABLE_3322_DDNS_SERVER",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENABLE_3322_DDNS_SERVER_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENABLE_3322_DDNS_SERVER,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_DDNS_3322_USERNAME",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_DDNS_3322_USERNAME_DEFINED )
			STRCPY_S( server_wifi_info->TZ_DDNS_3322_USERNAME,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_DDNS_3322_PASSWORD",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_DDNS_3322_PASSWORD_DEFINED )
			STRCPY_S( server_wifi_info->TZ_DDNS_3322_PASSWORD,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_3322_DOMAIN",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_3322_DOMAIN_DEFINED )
			STRCPY_S( server_wifi_info->TZ_3322_DOMAIN,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENABLE_NOIP_DDNS_SERVER",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENABLE_NOIP_DDNS_SERVER_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENABLE_NOIP_DDNS_SERVER,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_DDNS_NOIP_USERNAME",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_DDNS_NOIP_USERNAME_DEFINED )
			STRCPY_S( server_wifi_info->TZ_DDNS_NOIP_USERNAME,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_DDNS_NOIP_PASSWORD",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_DDNS_NOIP_PASSWORD_DEFINED )
			STRCPY_S( server_wifi_info->TZ_DDNS_NOIP_PASSWORD,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_NOIP_DOMAIN",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_NOIP_DOMAIN_DEFINED )
			STRCPY_S( server_wifi_info->TZ_NOIP_DOMAIN,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_PPP_USERNAME",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_PPP_USERNAME_DEFINED )
			STRCPY_S( server_wifi_info->TZ_PPP_USERNAME,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_PPP_PASSWORD",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_PPP_PASSWORD_DEFINED )
			STRCPY_S( server_wifi_info->TZ_PPP_PASSWORD,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_GET_DNS_AUTO",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_GET_DNS_AUTO_DEFINED )
			STRCPY_S( server_wifi_info->TZ_GET_DNS_AUTO,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_DNS_PRIMARY",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_DNS_PRIMARY_DEFINED )
			STRCPY_S( server_wifi_info->TZ_DNS_PRIMARY,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_DNS_SECONDARY",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_DNS_SECONDARY_DEFINED )
			STRCPY_S( server_wifi_info->TZ_DNS_SECONDARY,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_BAND_PREF",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_BAND_PREF_DEFINED )
			STRCPY_S( server_wifi_info->TZ_BAND_PREF,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_LTE_MODULE_MODE",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LTE_MODULE_MODE_DEFINED )
			STRCPY_S( server_wifi_info->TZ_LTE_MODULE_MODE,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_LTE_MODULE_PRIORITY",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LTE_MODULE_PRIORITY_DEFINED )
			STRCPY_S( server_wifi_info->TZ_LTE_MODULE_PRIORITY,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_HEARTBEAT_INTERVAL",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_HEARTBEAT_INTERVAL_DEFINED )
			STRCPY_S( server_wifi_info->TZ_HEARTBEAT_INTERVAL,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_SPECIAL_DEVICE",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_SPECIAL_DEVICE_DEFINED )
			STRCPY_S( server_wifi_info->TZ_SPECIAL_DEVICE,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_SERVICE_NUMBER",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_SERVICE_NUMBER_DEFINED )
			STRCPY_S( server_wifi_info->TZ_SERVICE_NUMBER,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_NTP_SERVER",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_NTP_SERVER_DEFINED )
			STRCPY_S( server_wifi_info->TZ_NTP_SERVER,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_SYS_UPDATE_SERVER",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_SYS_UPDATE_SERVER_DEFINED )
			STRCPY_S( server_wifi_info->TZ_SYS_UPDATE_SERVER,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_LNS_SERVER2",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LNS_SERVER2_DEFINED )
			STRCPY_S( server_wifi_info->TZ_LNS_SERVER2,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_TUNNEL_NAME2",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_TUNNEL_NAME2_DEFINED )
			STRCPY_S( server_wifi_info->TZ_TUNNEL_NAME2,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_TUNNEL_PASS2",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_TUNNEL_PASS2_DEFINED )
			STRCPY_S( server_wifi_info->TZ_TUNNEL_PASS2,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_LNS_SERVER3",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LNS_SERVER3_DEFINED )
			STRCPY_S( server_wifi_info->TZ_LNS_SERVER3,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_TUNNEL_NAME3",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_TUNNEL_NAME3_DEFINED )
			STRCPY_S( server_wifi_info->TZ_TUNNEL_NAME3,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_TUNNEL_PASS3",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_TUNNEL_PASS3_DEFINED )
			STRCPY_S( server_wifi_info->TZ_TUNNEL_PASS3,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_LNS_SERVER4",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LNS_SERVER4_DEFINED )
			STRCPY_S( server_wifi_info->TZ_LNS_SERVER4,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_TUNNEL_NAME4",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_TUNNEL_NAME4_DEFINED )
			STRCPY_S( server_wifi_info->TZ_TUNNEL_NAME4,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_TUNNEL_PASS4",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_TUNNEL_PASS4_DEFINED )
			STRCPY_S( server_wifi_info->TZ_TUNNEL_PASS4,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_LNS_SERVER5",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LNS_SERVER5_DEFINED )
			STRCPY_S( server_wifi_info->TZ_LNS_SERVER5,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_TUNNEL_NAME5",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_TUNNEL_NAME5_DEFINED )
			STRCPY_S( server_wifi_info->TZ_TUNNEL_NAME5,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_TUNNEL_PASS5",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_TUNNEL_PASS5_DEFINED )
			STRCPY_S( server_wifi_info->TZ_TUNNEL_PASS5,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_LNS_SERVER6",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LNS_SERVER6_DEFINED )
			STRCPY_S( server_wifi_info->TZ_LNS_SERVER6,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_TUNNEL_NAME6",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_TUNNEL_NAME6_DEFINED )
			STRCPY_S( server_wifi_info->TZ_TUNNEL_NAME6,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_TUNNEL_PASS6",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_TUNNEL_PASS6_DEFINED )
			STRCPY_S( server_wifi_info->TZ_TUNNEL_PASS6,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_LNS_SERVER7",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LNS_SERVER7_DEFINED )
			STRCPY_S( server_wifi_info->TZ_LNS_SERVER7,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_TUNNEL_NAME7",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_TUNNEL_NAME7_DEFINED )
			STRCPY_S( server_wifi_info->TZ_TUNNEL_NAME7,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_TUNNEL_PASS7",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_TUNNEL_PASS7_DEFINED )
			STRCPY_S( server_wifi_info->TZ_TUNNEL_PASS7,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_LNS_SERVER8",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LNS_SERVER8_DEFINED )
			STRCPY_S( server_wifi_info->TZ_LNS_SERVER8,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_TUNNEL_NAME8",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_TUNNEL_NAME8_DEFINED )
			STRCPY_S( server_wifi_info->TZ_TUNNEL_NAME8,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_TUNNEL_PASS8",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_TUNNEL_PASS8_DEFINED )
			STRCPY_S( server_wifi_info->TZ_TUNNEL_PASS8,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_LNS_SERVER9",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LNS_SERVER9_DEFINED )
			STRCPY_S( server_wifi_info->TZ_LNS_SERVER9,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_TUNNEL_NAME9",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_TUNNEL_NAME9_DEFINED )
			STRCPY_S( server_wifi_info->TZ_TUNNEL_NAME9,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_TUNNEL_PASS9",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_TUNNEL_PASS9_DEFINED )
			STRCPY_S( server_wifi_info->TZ_TUNNEL_PASS9,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_LNS_SERVER10",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LNS_SERVER10_DEFINED )
			STRCPY_S( server_wifi_info->TZ_LNS_SERVER10,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_TUNNEL_NAME10",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_TUNNEL_NAME10_DEFINED )
			STRCPY_S( server_wifi_info->TZ_TUNNEL_NAME10,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_TUNNEL_PASS10",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_TUNNEL_PASS10_DEFINED )
			STRCPY_S( server_wifi_info->TZ_TUNNEL_PASS10,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_LIMIT_UPLINK_SPEED_VALUE",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LIMIT_UPLINK_SPEED_VALUE_DEFINED )
			STRCPY_S( server_wifi_info->TZ_LIMIT_UPLINK_SPEED_VALUE,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_IP_STACK_MODE",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_IP_STACK_MODE_DEFINED )
			STRCPY_S( server_wifi_info->TZ_IP_STACK_MODE,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_PRESTORE_FREQUENCY_FDD",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_PRESTORE_FREQUENCY_FDD_DEFINED )
			STRCPY_S( server_wifi_info->TZ_PRESTORE_FREQUENCY_FDD,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_PRESTORE_FREQUENCY_TDD",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_PRESTORE_FREQUENCY_TDD_DEFINED )
			STRCPY_S( server_wifi_info->TZ_PRESTORE_FREQUENCY_TDD,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_LOCK_IMSI",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LOCK_IMSI_DEFINED )
			STRCPY_S( server_wifi_info->TZ_LOCK_IMSI,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_DEVICE_TYPE",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_DEVICE_TYPE_DEFINED )
			STRCPY_S( server_wifi_info->TZ_DEVICE_TYPE,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_MAX_ACCESS_USER_NUM",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_MAX_ACCESS_USER_NUM_DEFINED )
			STRCPY_S( server_wifi_info->TZ_MAX_ACCESS_USER_NUM,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		/*
		(.+)
		else if( !strncmp( p1,"\1",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,\1_DEFINED )
			STRCPY_S( server_wifi_info->\1,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		*/
		else if( !strncmp( p1,"TZ_SUPPORT_SIM_ENCRYPT",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_SUPPORT_SIM_ENCRYPT_DEFINED )
			STRCPY_S( server_wifi_info->TZ_SUPPORT_SIM_ENCRYPT,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENCRYPT_CARD_VENDOR_ID",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENCRYPT_CARD_VENDOR_ID_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENCRYPT_CARD_VENDOR_ID,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENCRYPT_KEY_VERSION",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENCRYPT_KEY_VERSION_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENCRYPT_KEY_VERSION,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENCRYPT_CARD_TYPE",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENCRYPT_CARD_TYPE_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENCRYPT_CARD_TYPE,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENCRYPT_KEY_1",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENCRYPT_KEY_1_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENCRYPT_KEY_1,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENCRYPT_KEY_2",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENCRYPT_KEY_2_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENCRYPT_KEY_2,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENCRYPT_KEY_3",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENCRYPT_KEY_3_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENCRYPT_KEY_3,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENCRYPT_KEY_4",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENCRYPT_KEY_4_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENCRYPT_KEY_4,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENCRYPT_KEY_5",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENCRYPT_KEY_5_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENCRYPT_KEY_5,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENCRYPT_KEY_6",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENCRYPT_KEY_6_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENCRYPT_KEY_6,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENCRYPT_KEY_7",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENCRYPT_KEY_7_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENCRYPT_KEY_7,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENCRYPT_KEY_8",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENCRYPT_KEY_8_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENCRYPT_KEY_8,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENCRYPT_KEY_9",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENCRYPT_KEY_9_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENCRYPT_KEY_9,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENCRYPT_KEY_10",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENCRYPT_KEY_10_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENCRYPT_KEY_10,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENCRYPT_KEY_11",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENCRYPT_KEY_11_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENCRYPT_KEY_11,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENCRYPT_KEY_12",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENCRYPT_KEY_12_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENCRYPT_KEY_12,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENCRYPT_KEY_13",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENCRYPT_KEY_13_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENCRYPT_KEY_13,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENCRYPT_KEY_14",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENCRYPT_KEY_14_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENCRYPT_KEY_14,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENCRYPT_KEY_15",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENCRYPT_KEY_15_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENCRYPT_KEY_15,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ENCRYPT_KEY_16",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ENCRYPT_KEY_16_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ENCRYPT_KEY_16,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_LOCK_ONE_CELL",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LOCK_ONE_CELL_DEFINED )
			STRCPY_S( server_wifi_info->TZ_LOCK_ONE_CELL,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_LOCKED_EARFCN",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LOCKED_EARFCN_DEFINED )
			STRCPY_S( server_wifi_info->TZ_LOCKED_EARFCN,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_LOCKED_PHYCELLID",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LOCKED_PHYCELLID_DEFINED )
			STRCPY_S( server_wifi_info->TZ_LOCKED_PHYCELLID,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_LOCKED_Enode_ID",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_LOCKED_Enode_ID_DEFINED )
			STRCPY_S( server_wifi_info->TZ_LOCKED_Enode_ID,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_FLOW_THRESHOLD",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_FLOW_THRESHOLD_DEFINED )
			STRCPY_S( server_wifi_info->TZ_FLOW_THRESHOLD,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_FLOW_WARNING",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_FLOW_WARNING_DEFINED )
			STRCPY_S( server_wifi_info->TZ_FLOW_WARNING,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_ISOLATE_WLAN_CLIENTS",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_ISOLATE_WLAN_CLIENTS_DEFINED )
			STRCPY_S( server_wifi_info->TZ_ISOLATE_WLAN_CLIENTS,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_L2TP_MTU",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_L2TP_MTU_DEFINED )
			STRCPY_S( server_wifi_info->TZ_L2TP_MTU,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_CPE_ROUTER_MODE",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_CPE_ROUTER_MODE_DEFINED )
			STRCPY_S( server_wifi_info->TZ_CPE_ROUTER_MODE,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_PPTP_ENABLED",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_PPTP_ENABLED_DEFINED )
			STRCPY_S( server_wifi_info->TZ_PPTP_ENABLED,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_PPTP_MODE",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_PPTP_MODE_DEFINED )
			STRCPY_S( server_wifi_info->TZ_PPTP_MODE,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_PNS_SERVER",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_PNS_SERVER_DEFINED )
			STRCPY_S( server_wifi_info->TZ_PNS_SERVER,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_PPTP_AUTH_NAME",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_PPTP_AUTH_NAME_DEFINED )
			STRCPY_S( server_wifi_info->TZ_PPTP_AUTH_NAME,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_PPTP_AUTH_PASS",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_PPTP_AUTH_PASS_DEFINED )
			STRCPY_S( server_wifi_info->TZ_PPTP_AUTH_PASS,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_PPTP_ENABLE_MPPE",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_PPTP_ENABLE_MPPE_DEFINED )
			STRCPY_S( server_wifi_info->TZ_PPTP_ENABLE_MPPE,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_PPTP_MTU",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_PPTP_MTU_DEFINED )
			STRCPY_S( server_wifi_info->TZ_PPTP_MTU,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else if( !strncmp( p1,"TZ_VPN_AS_DEFAULT_ROUTE",p2-p1 ) )
		{
			infostruct_set_field_defined( server_wifi_info,TZ_VPN_AS_DEFAULT_ROUTE_DEFINED )
			STRCPY_S( server_wifi_info->TZ_VPN_AS_DEFAULT_ROUTE,util_get_field_value_seperated_by_equal(temp_buffer) );
		}
		else
		{
			int index;
			for( index=0;index < ALL_NEED_PARSED_FIELDS_COUNT;index++ )
			{
				if( !strncmp( p1,all_need_parsed_fields[ index ].field_name,p2-p1 ) )
				{
					infostruct_set_field_defined( server_wifi_info,all_need_parsed_fields[ index ].field_flag )
					snprintf((char*)server_wifi_info+all_need_parsed_fields[ index ].field_offset
							,all_need_parsed_fields[ index ].field_size-1
							,"%s"
							,util_get_field_value_seperated_by_equal(temp_buffer)
							);
					break;
				}
			}
		}


	}

	fclose( file_handle );
	return 0;
}

int util_server_read_wifi_info( InfoStruct* server_wifi_info )
{
	return util_server_read_wifi_info_ex( RC_SYSTEM_CONFIG_FILE,server_wifi_info );
}

//sublime text replace
//if\(\s*strlen\(\s*sys_config_info->([^\s]*)\s*\)\s*\)
//if( infostruct_is_field_defined( sys_config_info,\1_DEFINED ) )
//size=sizeof\(\s*nvram_config_info->([^\s]*)\s*\);\n\tif\(\s*strlen\(\s*sys_config_field\s*\)\s*\)
//size=sizeof( nvram_config_info->\1 );\n\tif( infostruct_is_field_defined( sys_config_info,\1_DEFINED ) )
//(if\(\s*infostruct_is_field_defined\(\s*sys_config_info,([^\s]+)\s*\)\s*\)\n\t\{)
//\1\n\t\tinfostruct_set_field_defined( nvram_config_info,\2 );
//使用系统系统配置信息覆盖掉客户配置的信息
void util_overwrite_config_info( InfoStruct* sys_config_info,InfoStruct* nvram_config_info )
{
	//char* sys_config_field;
	//char* nvram_config_field;
	//int size;

/*
	if( infostruct_is_field_defined( sys_config_info,AP_IPADDR_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,AP_IPADDR_DEFINED );
		STRCPY_S( nvram_config_info->AP_IPADDR,sys_config_info->AP_IPADDR );
	}
	if( infostruct_is_field_defined( sys_config_info,AP_NETMASK_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,AP_NETMASK_DEFINED );
		STRCPY_S( nvram_config_info->AP_NETMASK,sys_config_info->AP_NETMASK );
	}
	if( infostruct_is_field_defined( sys_config_info,TZ_DHCP_IP_BEGIN_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_DHCP_IP_BEGIN_DEFINED );
		STRCPY_S( nvram_config_info->TZ_DHCP_IP_BEGIN,sys_config_info->TZ_DHCP_IP_BEGIN );
	}
	if( infostruct_is_field_defined( sys_config_info,TZ_DHCP_IP_END_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_DHCP_IP_END_DEFINED );
		STRCPY_S( nvram_config_info->TZ_DHCP_IP_END,sys_config_info->TZ_DHCP_IP_END );
	}
	if( infostruct_is_field_defined( sys_config_info,TZ_DHCP_EXPIRE_TIME_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_DHCP_EXPIRE_TIME_DEFINED );
		STRCPY_S( nvram_config_info->TZ_DHCP_EXPIRE_TIME,sys_config_info->TZ_DHCP_EXPIRE_TIME );
	}
	if( infostruct_is_field_defined( sys_config_info,AP_CHMODE_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,AP_CHMODE_DEFINED );
		STRCPY_S( nvram_config_info->AP_CHMODE,sys_config_info->AP_CHMODE );
	}
	if( infostruct_is_field_defined( sys_config_info,PUREG_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,PUREG_DEFINED );
		STRCPY_S( nvram_config_info->PUREG,sys_config_info->PUREG );
	}
	if( infostruct_is_field_defined( sys_config_info,PUREN_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,PUREN_DEFINED );
		STRCPY_S( nvram_config_info->PUREN,sys_config_info->PUREN );
	}
	if( infostruct_is_field_defined( sys_config_info,AP_WPA_GROUP_REKEY_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,AP_WPA_GROUP_REKEY_DEFINED );
		STRCPY_S( nvram_config_info->AP_WPA_GROUP_REKEY,sys_config_info->AP_WPA_GROUP_REKEY );
	}
	if( infostruct_is_field_defined( sys_config_info,AP_WPA_GMK_REKEY_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,AP_WPA_GMK_REKEY_DEFINED );
		STRCPY_S( nvram_config_info->AP_WPA_GMK_REKEY,sys_config_info->AP_WPA_GMK_REKEY );
	}
	if( infostruct_is_field_defined( sys_config_info,RTS_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,RTS_DEFINED );
		STRCPY_S( nvram_config_info->RTS,sys_config_info->RTS );
	}
	if( infostruct_is_field_defined( sys_config_info,RATECTL_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,RATECTL_DEFINED );
		STRCPY_S( nvram_config_info->RATECTL,sys_config_info->RATECTL );
	}
	if( infostruct_is_field_defined( sys_config_info,MANRATE_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,MANRATE_DEFINED );
		STRCPY_S( nvram_config_info->MANRATE,sys_config_info->MANRATE );
	}
	if( infostruct_is_field_defined( sys_config_info,MANRETRIES_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,MANRETRIES_DEFINED );
		STRCPY_S( nvram_config_info->MANRETRIES,sys_config_info->MANRETRIES );
	}
	if( infostruct_is_field_defined( sys_config_info,WIFIWORKMODE_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,WIFIWORKMODE_DEFINED );
		STRCPY_S( nvram_config_info->WIFIWORKMODE,sys_config_info->WIFIWORKMODE );
	}
	if( infostruct_is_field_defined( sys_config_info,TZ_ENABLE_WATCHDOG_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_ENABLE_WATCHDOG_DEFINED );
		STRCPY_S( nvram_config_info->TZ_ENABLE_WATCHDOG,sys_config_info->TZ_ENABLE_WATCHDOG );
	}
	if( infostruct_is_field_defined( sys_config_info,TZ_ENABLE_WIFI_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_ENABLE_WIFI_DEFINED );
		STRCPY_S( nvram_config_info->TZ_ENABLE_WIFI,sys_config_info->TZ_ENABLE_WIFI );
	}
	if( infostruct_is_field_defined( sys_config_info,AP_SSID_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,AP_SSID_DEFINED );
		STRCPY_S( nvram_config_info->AP_SSID,sys_config_info->AP_SSID );
	}
	if( infostruct_is_field_defined( sys_config_info,AP_HIDESSID_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,AP_HIDESSID_DEFINED );
		STRCPY_S( nvram_config_info->AP_HIDESSID,sys_config_info->AP_HIDESSID );
	}
	if( infostruct_is_field_defined( sys_config_info,AP_PRIMARY_CH_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,AP_PRIMARY_CH_DEFINED );
		STRCPY_S( nvram_config_info->AP_PRIMARY_CH,sys_config_info->AP_PRIMARY_CH );
	}
	if( infostruct_is_field_defined( sys_config_info,TXPOWER_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TXPOWER_DEFINED );
		STRCPY_S( nvram_config_info->TXPOWER,sys_config_info->TXPOWER );
	}
	if( infostruct_is_field_defined( sys_config_info,AP_SECMODE_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,AP_SECMODE_DEFINED );
		STRCPY_S( nvram_config_info->AP_SECMODE,sys_config_info->AP_SECMODE );
	}
	if( infostruct_is_field_defined( sys_config_info,AP_WPA_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,AP_WPA_DEFINED );
		STRCPY_S( nvram_config_info->AP_WPA,sys_config_info->AP_WPA );
	}
	if( infostruct_is_field_defined( sys_config_info,AP_SECFILE_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,AP_SECFILE_DEFINED );
		STRCPY_S( nvram_config_info->AP_SECFILE,sys_config_info->AP_SECFILE );
	}
	if( infostruct_is_field_defined( sys_config_info,PSK_KEY_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,PSK_KEY_DEFINED );
		STRCPY_S( nvram_config_info->PSK_KEY,sys_config_info->PSK_KEY );
	}
	if( infostruct_is_field_defined( sys_config_info,AP_CYPHER_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,AP_CYPHER_DEFINED );
		STRCPY_S( nvram_config_info->AP_CYPHER,sys_config_info->AP_CYPHER );
	}
	if( infostruct_is_field_defined( sys_config_info,AP_WEP_MODE_0_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,AP_WEP_MODE_0_DEFINED );
		STRCPY_S( nvram_config_info->AP_WEP_MODE_0,sys_config_info->AP_WEP_MODE_0 );
	}
	if( infostruct_is_field_defined( sys_config_info,AP_PRIMARY_KEY_0_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,AP_PRIMARY_KEY_0_DEFINED );
		STRCPY_S( nvram_config_info->AP_PRIMARY_KEY_0,sys_config_info->AP_PRIMARY_KEY_0 );
	}
	if( infostruct_is_field_defined( sys_config_info,AP_PRIMARY_KEY_0_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,AP_PRIMARY_KEY_0_DEFINED );
		STRCPY_S( nvram_config_info->AP_PRIMARY_KEY_0,sys_config_info->AP_PRIMARY_KEY_0 );
	}
	if( infostruct_is_field_defined( sys_config_info,WEP_RADIO_NUM0_KEY_1_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,WEP_RADIO_NUM0_KEY_1_DEFINED );
		STRCPY_S( nvram_config_info->WEP_RADIO_NUM0_KEY_1,sys_config_info->WEP_RADIO_NUM0_KEY_1 );
	}
	if( infostruct_is_field_defined( sys_config_info,WEP_RADIO_NUM0_KEY_2_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,WEP_RADIO_NUM0_KEY_2_DEFINED );
		STRCPY_S( nvram_config_info->WEP_RADIO_NUM0_KEY_2,sys_config_info->WEP_RADIO_NUM0_KEY_2 );
	}
	if( infostruct_is_field_defined( sys_config_info,WEP_RADIO_NUM0_KEY_3_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,WEP_RADIO_NUM0_KEY_3_DEFINED );
		STRCPY_S( nvram_config_info->WEP_RADIO_NUM0_KEY_3,sys_config_info->WEP_RADIO_NUM0_KEY_3 );
	}
	if( infostruct_is_field_defined( sys_config_info,WEP_RADIO_NUM0_KEY_4_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,WEP_RADIO_NUM0_KEY_4_DEFINED );
		STRCPY_S( nvram_config_info->WEP_RADIO_NUM0_KEY_4,sys_config_info->WEP_RADIO_NUM0_KEY_4 );
	}


	sys_config_field=sys_config_info->TZ_LOCK_CARD;
	nvram_config_field=nvram_config_info->TZ_LOCK_CARD;
	size=sizeof( nvram_config_info->TZ_LOCK_CARD );
	if( infostruct_is_field_defined( sys_config_info,TZ_LOCK_CARD_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_LOCK_CARD_DEFINED );
		nvram_config_field[ size-1 ]=0;
		strncpy( nvram_config_field,sys_config_field,size-1 );
	}

	sys_config_field=sys_config_info->TZ_LOCK_CARD_TYPE;
	nvram_config_field=nvram_config_info->TZ_LOCK_CARD_TYPE;
	size=sizeof( nvram_config_info->TZ_LOCK_CARD_TYPE );
	if( infostruct_is_field_defined( sys_config_info,TZ_LOCK_CARD_TYPE_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_LOCK_CARD_TYPE_DEFINED );
		nvram_config_field[ size-1 ]=0;
		strncpy( nvram_config_field,sys_config_field,size-1 );
	}

	sys_config_field=sys_config_info->TZ_INITIAL_PIN_CODE;
	nvram_config_field=nvram_config_info->TZ_INITIAL_PIN_CODE;
	size=sizeof( nvram_config_info->TZ_INITIAL_PIN_CODE );
	if( infostruct_is_field_defined( sys_config_info,TZ_INITIAL_PIN_CODE_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_INITIAL_PIN_CODE_DEFINED );
		nvram_config_field[ size-1 ]=0;
		strncpy( nvram_config_field,sys_config_field,size-1 );
	}

	sys_config_field=sys_config_info->TZ_MODIFIED_PIN_CODE;
	nvram_config_field=nvram_config_info->TZ_MODIFIED_PIN_CODE;
	size=sizeof( nvram_config_info->TZ_MODIFIED_PIN_CODE );
	if( infostruct_is_field_defined( sys_config_info,TZ_MODIFIED_PIN_CODE_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_MODIFIED_PIN_CODE_DEFINED );
		nvram_config_field[ size-1 ]=0;
		strncpy( nvram_config_field,sys_config_field,size-1 );
	}

	sys_config_field=sys_config_info->TZ_LOCK_CELL_ID;
	nvram_config_field=nvram_config_info->TZ_LOCK_CELL_ID;
	size=sizeof( nvram_config_info->TZ_LOCK_CELL_ID );
	if( infostruct_is_field_defined( sys_config_info,TZ_LOCK_CELL_ID_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_LOCK_CELL_ID_DEFINED );
		nvram_config_field[ size-1 ]=0;
		strncpy( nvram_config_field,sys_config_field,size-1 );
	}

	sys_config_field=sys_config_info->TZ_LOCK_CELL_ID_COUNT;
	nvram_config_field=nvram_config_info->TZ_LOCK_CELL_ID_COUNT;
	size=sizeof( nvram_config_info->TZ_LOCK_CELL_ID_COUNT );
	if( infostruct_is_field_defined( sys_config_info,TZ_LOCK_CELL_ID_COUNT_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_LOCK_CELL_ID_COUNT_DEFINED );
		nvram_config_field[ size-1 ]=0;
		strncpy( nvram_config_field,sys_config_field,size-1 );
	}

	sys_config_field=sys_config_info->TZ_LIMIT_SPEED_ENABLED;
	nvram_config_field=nvram_config_info->TZ_LIMIT_SPEED_ENABLED;
	size=sizeof( nvram_config_info->TZ_LIMIT_SPEED_ENABLED );
	if( infostruct_is_field_defined( sys_config_info,TZ_LIMIT_SPEED_ENABLED_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_LIMIT_SPEED_ENABLED_DEFINED );
		nvram_config_field[ size-1 ]=0;
		strncpy( nvram_config_field,sys_config_field,size-1 );
	}

	sys_config_field=sys_config_info->TZ_LIMIT_SPEED_VALUE;
	nvram_config_field=nvram_config_info->TZ_LIMIT_SPEED_VALUE;
	size=sizeof( nvram_config_info->TZ_LIMIT_SPEED_VALUE );
	if( infostruct_is_field_defined( sys_config_info,TZ_LIMIT_SPEED_VALUE_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_LIMIT_SPEED_VALUE_DEFINED );
		nvram_config_field[ size-1 ]=0;
		strncpy( nvram_config_field,sys_config_field,size-1 );
	}


	sys_config_field=sys_config_info->TZ_L2TP_MODE;
	nvram_config_field=nvram_config_info->TZ_L2TP_MODE;
	size=sizeof( nvram_config_info->TZ_L2TP_MODE );
	if( infostruct_is_field_defined( sys_config_info,TZ_L2TP_MODE_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_L2TP_MODE_DEFINED );
		nvram_config_field[ size-1 ]=0;
		strncpy( nvram_config_field,sys_config_field,size-1 );
	}

	sys_config_field=sys_config_info->TZ_LNS_SERVER;
	nvram_config_field=nvram_config_info->TZ_LNS_SERVER;
	size=sizeof( nvram_config_info->TZ_LNS_SERVER );
	if( infostruct_is_field_defined( sys_config_info,TZ_LNS_SERVER_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_LNS_SERVER_DEFINED );
		nvram_config_field[ size-1 ]=0;
		strncpy( nvram_config_field,sys_config_field,size-1 );
	}

	sys_config_field=sys_config_info->TZ_TUNNEL_NAME;
	nvram_config_field=nvram_config_info->TZ_TUNNEL_NAME;
	size=sizeof( nvram_config_info->TZ_TUNNEL_NAME );
	if( infostruct_is_field_defined( sys_config_info,TZ_TUNNEL_NAME_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_TUNNEL_NAME_DEFINED );
		nvram_config_field[ size-1 ]=0;
		strncpy( nvram_config_field,sys_config_field,size-1 );
	}

	sys_config_field=sys_config_info->TZ_TUNNEL_PASS;
	nvram_config_field=nvram_config_info->TZ_TUNNEL_PASS;
	size=sizeof( nvram_config_info->TZ_TUNNEL_PASS );
	if( infostruct_is_field_defined( sys_config_info,TZ_TUNNEL_PASS_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_TUNNEL_PASS_DEFINED );
		nvram_config_field[ size-1 ]=0;
		strncpy( nvram_config_field,sys_config_field,size-1 );
	}

	sys_config_field=sys_config_info->TZ_AUTH_NAME;
	nvram_config_field=nvram_config_info->TZ_AUTH_NAME;
	size=sizeof( nvram_config_info->TZ_AUTH_NAME );
	if( infostruct_is_field_defined( sys_config_info,TZ_AUTH_NAME_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_AUTH_NAME_DEFINED );
		nvram_config_field[ size-1 ]=0;
		strncpy( nvram_config_field,sys_config_field,size-1 );
	}

	sys_config_field=sys_config_info->TZ_AUTH_PASS;
	nvram_config_field=nvram_config_info->TZ_AUTH_PASS;
	size=sizeof( nvram_config_info->TZ_AUTH_PASS );
	if( infostruct_is_field_defined( sys_config_info,TZ_AUTH_PASS_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_AUTH_PASS_DEFINED );
		nvram_config_field[ size-1 ]=0;
		strncpy( nvram_config_field,sys_config_field,size-1 );
	}

	sys_config_field=sys_config_info->TZ_CONFIG_FILE_VERSION;
	nvram_config_field=nvram_config_info->TZ_CONFIG_FILE_VERSION;
	size=sizeof( nvram_config_info->TZ_CONFIG_FILE_VERSION );
	if( infostruct_is_field_defined( sys_config_info,TZ_CONFIG_FILE_VERSION_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_CONFIG_FILE_VERSION_DEFINED );
		nvram_config_field[ size-1 ]=0;
		strncpy( nvram_config_field,sys_config_field,size-1 );
	}

	sys_config_field=sys_config_info->TZ_CONFIG_FILE_DESCRIPTION;
	nvram_config_field=nvram_config_info->TZ_CONFIG_FILE_DESCRIPTION;
	size=sizeof( nvram_config_info->TZ_CONFIG_FILE_DESCRIPTION );
	if( infostruct_is_field_defined( sys_config_info,TZ_CONFIG_FILE_DESCRIPTION_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_CONFIG_FILE_DESCRIPTION_DEFINED );
		nvram_config_field[ size-1 ]=0;
		strncpy( nvram_config_field,sys_config_field,size-1 );
	}
	if( infostruct_is_field_defined( sys_config_info,TZ_PPP_USERNAME_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_PPP_USERNAME_DEFINED );
		STRCPY_S( nvram_config_info->TZ_PPP_USERNAME,sys_config_info->TZ_PPP_USERNAME );
	}
	if( infostruct_is_field_defined( sys_config_info,TZ_PPP_PASSWORD_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_PPP_PASSWORD_DEFINED );
		STRCPY_S( nvram_config_info->TZ_PPP_PASSWORD,sys_config_info->TZ_PPP_PASSWORD );
	}
	if( infostruct_is_field_defined( sys_config_info,TZ_GET_DNS_AUTO_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_GET_DNS_AUTO_DEFINED );
		STRCPY_S( nvram_config_info->TZ_GET_DNS_AUTO,sys_config_info->TZ_GET_DNS_AUTO );
	}
	if( infostruct_is_field_defined( sys_config_info,TZ_DNS_PRIMARY_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_DNS_PRIMARY_DEFINED );
		STRCPY_S( nvram_config_info->TZ_DNS_PRIMARY,sys_config_info->TZ_DNS_PRIMARY );
	}
	if( infostruct_is_field_defined( sys_config_info,TZ_DNS_SECONDARY_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_DNS_SECONDARY_DEFINED );
		STRCPY_S( nvram_config_info->TZ_DNS_SECONDARY,sys_config_info->TZ_DNS_SECONDARY );
	}
	if( infostruct_is_field_defined( sys_config_info,TZ_BAND_PREF_DEFINED ) )
	{
		infostruct_set_field_defined( nvram_config_info,TZ_BAND_PREF_DEFINED );
		STRCPY_S( nvram_config_info->TZ_BAND_PREF,sys_config_info->TZ_BAND_PREF );
	}
*/

}

char* util_get_config_field_value( InfoStruct *nvram_config_info,char* config_field_name )
{
	char* nvram_config_field=NULL;

	//export AP_SSID="tozed-11274F"
	if( !strcmp( config_field_name,"AP_SSID" ) )
	{
		nvram_config_field=nvram_config_info->AP_SSID;
	}
	//export TZ_ENABLE_WIFI=yes
	else if( !strcmp( config_field_name,"TZ_ENABLE_WIFI" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENABLE_WIFI;
	}
	//export PWD=6A7FCADA
	else if( !strcmp( config_field_name,"PWD" ) )
	{
		nvram_config_field=nvram_config_info->PSK_KEY;
	}
	else if( !strcmp( config_field_name,"PSK_KEY" ) )
	{
		nvram_config_field=nvram_config_info->PSK_KEY;
	}
	//export AP_CYPHER=CCMP
	else if( !strcmp( config_field_name,"AP_CYPHER" ) )
	{
		nvram_config_field=nvram_config_info->AP_CYPHER;
	}
	//export AP_WPA=2
	else if( !strcmp( config_field_name,"AP_WPA" ) )
	{
		nvram_config_field=nvram_config_info->AP_WPA;
	}
	//export PUREG=0
	else if( !strcmp( config_field_name,"PUREG" ) )
	{
		nvram_config_field=nvram_config_info->PUREG;
	}
	//export RATECTL=manual
	else if( !strcmp( config_field_name,"RATECTL" ) )
	{
		nvram_config_field=nvram_config_info->RATECTL;
	}
	//export MANRATE=0x87868584
	else if( !strcmp( config_field_name,"MANRATE" ) )
	{
		nvram_config_field=nvram_config_info->MANRATE;
	}
	//export MANRETRIES=0x04040404
	else if( !strcmp( config_field_name,"MANRETRIES" ) )
	{
		nvram_config_field=nvram_config_info->MANRETRIES;
	}
	//export WIFIWORKMODE=m11b
	else if( !strcmp( config_field_name,"WIFIWORKMODE" ) )
	{
		nvram_config_field=nvram_config_info->WIFIWORKMODE;
	}
	//export PUREN=0
	else if( !strcmp( config_field_name,"PUREN" ) )
	{
		nvram_config_field=nvram_config_info->PUREN;
	}
	//export AP_SECMODE=WPA
	else if( !strcmp( config_field_name,"AP_SECMODE" ) )
	{
		nvram_config_field=nvram_config_info->AP_SECMODE;
	}
	//export AP_SECFILE=PSK
	else if( !strcmp( config_field_name,"AP_SECFILE" ) )
	{
		nvram_config_field=nvram_config_info->AP_SECFILE;
	}
	//export AP_WPA_GROUP_REKEY=600
	else if( !strcmp( config_field_name,"AP_WPA_GROUP_REKEY" ) )
	{
		nvram_config_field=nvram_config_info->AP_WPA_GROUP_REKEY;
	}
	//export AP_WPA_GMK_REKEY=86400
	else if( !strcmp( config_field_name,"AP_WPA_GMK_REKEY" ) )
	{
		nvram_config_field=nvram_config_info->AP_WPA_GMK_REKEY;
	}
	//export AP_WEP_MODE_0=2
	else if( !strcmp( config_field_name,"AP_WEP_MODE_0" ) )
	{
		nvram_config_field=nvram_config_info->AP_WEP_MODE_0;
	}
	//export AP_PRIMARY_KEY_0=1
	else if( !strcmp( config_field_name,"AP_PRIMARY_KEY_0" ) )
	{
		nvram_config_field=nvram_config_info->AP_PRIMARY_KEY_0;
	}
	//export WEP_RADIO_NUM0_KEY_1="abcdefghijklm"
	else if( !strcmp( config_field_name,"WEP_RADIO_NUM0_KEY_1" ) )
	{
		nvram_config_field=nvram_config_info->WEP_RADIO_NUM0_KEY_1;
	}
	//export WEP_RADIO_NUM0_KEY_2="abcdefghijklm"
	else if( !strcmp( config_field_name,"WEP_RADIO_NUM0_KEY_2" ) )
	{
		nvram_config_field=nvram_config_info->WEP_RADIO_NUM0_KEY_2;
	}
	//export WEP_RADIO_NUM0_KEY_3="abcdefghijklm"
	else if( !strcmp( config_field_name,"WEP_RADIO_NUM0_KEY_3" ) )
	{
		nvram_config_field=nvram_config_info->WEP_RADIO_NUM0_KEY_3;
	}
	//export WEP_RADIO_NUM0_KEY_4="abcdefghijklm"
	else if( !strcmp( config_field_name,"WEP_RADIO_NUM0_KEY_4" ) )
	{
		nvram_config_field=nvram_config_info->WEP_RADIO_NUM0_KEY_4;
	}
	//export AP_HIDESSID=0
	else if( !strcmp( config_field_name,"AP_HIDESSID" ) )
	{
		nvram_config_field=nvram_config_info->AP_HIDESSID;
	}
	//export AP_IPADDR="192.168.0.1"
	else if( !strcmp( config_field_name,"AP_IPADDR" ) )
	{
		nvram_config_field=nvram_config_info->AP_IPADDR;
	}
	//export AP_NETMASK="255.255.255.0"
	else if( !strcmp( config_field_name,"AP_NETMASK" ) )
	{
		nvram_config_field=nvram_config_info->AP_NETMASK;
	}
	//export TZ_DHCP_IP_BEGIN="192.168.0.100"
	else if( !strcmp( config_field_name,"TZ_DHCP_IP_BEGIN" ) )
	{
		nvram_config_field=nvram_config_info->TZ_DHCP_IP_BEGIN;
	}
	//export TZ_DHCP_IP_END="192.168.0.250"
	else if( !strcmp( config_field_name,"TZ_DHCP_IP_END" ) )
	{
		nvram_config_field=nvram_config_info->TZ_DHCP_IP_END;
	}
	//export TZ_DHCP_EXPIRE_TIME=86400
	else if( !strcmp( config_field_name,"TZ_DHCP_EXPIRE_TIME" ) )
	{
		nvram_config_field=nvram_config_info->TZ_DHCP_EXPIRE_TIME;
	}
	//export AP_PRIMARY_CH=11
	else if( !strcmp( config_field_name,"AP_PRIMARY_CH" ) )
	{
		nvram_config_field=nvram_config_info->AP_PRIMARY_CH;
	}
	//export AP_CHMODE=11NGHT20
	else if( !strcmp( config_field_name,"AP_CHMODE" ) )
	{
		nvram_config_field=nvram_config_info->AP_CHMODE;
	}
	//export TXPOWER=18
	else if( !strcmp( config_field_name,"TXPOWER" ) )
	{
		nvram_config_field=nvram_config_info->TXPOWER;
	}
	//export TZ_ENABLE_WATCHDOG=no
	else if( !strcmp( config_field_name,"TZ_ENABLE_WATCHDOG" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENABLE_WATCHDOG;
	}
	else if( !strcmp( config_field_name,"TZ_SPEED_PREFERRED" ) )
	{
		nvram_config_field=nvram_config_info->TZ_SPEED_PREFERRED;
	}
	else if( !strcmp( config_field_name,"TZ_LOCK_CARD" ) )
	{
		nvram_config_field=nvram_config_info->TZ_LOCK_CARD;
	}
	else if( !strcmp( config_field_name,"TZ_LOCK_CARD_TYPE" ) )
	{
		nvram_config_field=nvram_config_info->TZ_LOCK_CARD_TYPE;
	}
	else if( !strcmp( config_field_name,"TZ_INITIAL_PIN_CODE" ) )
	{
		nvram_config_field=nvram_config_info->TZ_INITIAL_PIN_CODE;
	}
	else if( !strcmp( config_field_name,"TZ_MODIFIED_PIN_CODE" ) )
	{
		nvram_config_field=nvram_config_info->TZ_MODIFIED_PIN_CODE;
	}
	else if( !strcmp( config_field_name,"TZ_LOCK_CELL_ID" ) )
	{
		nvram_config_field=nvram_config_info->TZ_LOCK_CELL_ID;
	}
	else if( !strcmp( config_field_name,"TZ_LOCK_CELL_ID_COUNT" ) )
	{
		nvram_config_field=nvram_config_info->TZ_LOCK_CELL_ID_COUNT;
	}
	else if( !strcmp( config_field_name,"TZ_LIMIT_SPEED_ENABLED" ) )
	{
		nvram_config_field=nvram_config_info->TZ_LIMIT_SPEED_ENABLED;
	}
	else if( !strcmp( config_field_name,"TZ_LIMIT_SPEED_VALUE" ) )
	{
		nvram_config_field=nvram_config_info->TZ_LIMIT_SPEED_VALUE;
	}
	else if( !strcmp( config_field_name,"TZ_L2TP_MODE" ) )
	{
		nvram_config_field=nvram_config_info->TZ_L2TP_MODE;
	}
	else if( !strcmp( config_field_name,"TZ_LNS_SERVER" ) )
	{
		nvram_config_field=nvram_config_info->TZ_LNS_SERVER;
	}
	else if( !strcmp( config_field_name,"TZ_TUNNEL_NAME" ) )
	{
		nvram_config_field=nvram_config_info->TZ_TUNNEL_NAME;
	}
	else if( !strcmp( config_field_name,"TZ_TUNNEL_PASS" ) )
	{
		nvram_config_field=nvram_config_info->TZ_TUNNEL_PASS;
	}
	else if( !strcmp( config_field_name,"TZ_AUTH_NAME" ) )
	{
		nvram_config_field=nvram_config_info->TZ_AUTH_NAME;
	}
	else if( !strcmp( config_field_name,"TZ_AUTH_PASS" ) )
	{
		nvram_config_field=nvram_config_info->TZ_AUTH_PASS;
	}
	else if( !strcmp( config_field_name,"TZ_CONFIG_FILE_VERSION" ) )
	{
		nvram_config_field=nvram_config_info->TZ_CONFIG_FILE_VERSION;
	}
	else if( !strcmp( config_field_name,"TZ_CONFIG_FILE_DESCRIPTION" ) )
	{
		nvram_config_field=nvram_config_info->TZ_CONFIG_FILE_DESCRIPTION;
	}
	else if( !strcmp( config_field_name,"TZ_ENABLE_PPP_FORWARD" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENABLE_PPP_FORWARD;
	}
	else if( !strcmp( config_field_name,"LAN_LED_AS_LTE_STATUS" ) )
	{
		nvram_config_field=nvram_config_info->LAN_LED_AS_LTE_STATUS;
	}
	else if( !strcmp( config_field_name,"TZ_DDNS_HSK_USERNAME" ) )
	{
		nvram_config_field=nvram_config_info->TZ_DDNS_HSK_USERNAME;
	}
	else if( !strcmp( config_field_name,"TZ_DDNS_HSK_PASSWORD" ) )
	{
		nvram_config_field=nvram_config_info->TZ_DDNS_HSK_PASSWORD;
	}
	else if( !strcmp( config_field_name,"TZ_DDNS_INTERFACE" ) )
	{
		nvram_config_field=nvram_config_info->TZ_DDNS_INTERFACE;
	}
	else if( !strcmp( config_field_name,"TZ_ENABLE_HSK_DDNS_SERVER" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENABLE_HSK_DDNS_SERVER;
	}
	else if( !strcmp( config_field_name,"TZ_ENABLE_3322_DDNS_SERVER" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENABLE_3322_DDNS_SERVER;
	}
	else if( !strcmp( config_field_name,"TZ_DDNS_3322_USERNAME" ) )
	{
		nvram_config_field=nvram_config_info->TZ_DDNS_3322_USERNAME;
	}
	else if( !strcmp( config_field_name,"TZ_DDNS_3322_PASSWORD" ) )
	{
		nvram_config_field=nvram_config_info->TZ_DDNS_3322_PASSWORD;
	}
	else if( !strcmp( config_field_name,"TZ_3322_DOMAIN" ) )
	{
		nvram_config_field=nvram_config_info->TZ_3322_DOMAIN;
	}
	else if( !strcmp( config_field_name,"TZ_ENABLE_NOIP_DDNS_SERVER" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENABLE_NOIP_DDNS_SERVER;
	}
	else if( !strcmp( config_field_name,"TZ_DDNS_NOIP_USERNAME" ) )
	{
		nvram_config_field=nvram_config_info->TZ_DDNS_NOIP_USERNAME;
	}
	else if( !strcmp( config_field_name,"TZ_DDNS_NOIP_PASSWORD" ) )
	{
		nvram_config_field=nvram_config_info->TZ_DDNS_NOIP_PASSWORD;
	}
	else if( !strcmp( config_field_name,"TZ_NOIP_DOMAIN" ) )
	{
		nvram_config_field=nvram_config_info->TZ_NOIP_DOMAIN;
	}
	else if( !strcmp( config_field_name,"TZ_PPP_USERNAME" ) )
	{
		nvram_config_field=nvram_config_info->TZ_PPP_USERNAME;
	}
	else if( !strcmp( config_field_name,"TZ_PPP_PASSWORD" ) )
	{
		nvram_config_field=nvram_config_info->TZ_PPP_PASSWORD;
	}
	else if( !strcmp( config_field_name,"TZ_GET_DNS_AUTO" ) )
	{
		nvram_config_field=nvram_config_info->TZ_GET_DNS_AUTO;
	}
	else if( !strcmp( config_field_name,"TZ_DNS_PRIMARY" ) )
	{
		nvram_config_field=nvram_config_info->TZ_DNS_PRIMARY;
	}
	else if( !strcmp( config_field_name,"TZ_DNS_SECONDARY" ) )
	{
		nvram_config_field=nvram_config_info->TZ_DNS_SECONDARY;
	}
	else if( !strcmp( config_field_name,"TZ_BAND_PREF" ) )
	{
		nvram_config_field=nvram_config_info->TZ_BAND_PREF;
	}
	else if( !strcmp( config_field_name,"TZ_LTE_MODULE_MODE" ) )
	{
		nvram_config_field=nvram_config_info->TZ_LTE_MODULE_MODE;
	}
	else if( !strcmp( config_field_name,"TZ_LTE_MODULE_PRIORITY" ) )
	{
		nvram_config_field=nvram_config_info->TZ_LTE_MODULE_PRIORITY;
	}
	else if( !strcmp( config_field_name,"TZ_HEARTBEAT_INTERVAL" ) )
	{
		nvram_config_field=nvram_config_info->TZ_HEARTBEAT_INTERVAL;
	}
	else if( !strcmp( config_field_name,"TZ_SPECIAL_DEVICE" ) )
	{
		nvram_config_field=nvram_config_info->TZ_SPECIAL_DEVICE;
	}
	else if( !strcmp( config_field_name,"TZ_SERVICE_NUMBER" ) )
	{
		nvram_config_field=nvram_config_info->TZ_SERVICE_NUMBER;
	}
	else if( !strcmp( config_field_name,"TZ_NTP_SERVER" ) )
	{
		nvram_config_field=nvram_config_info->TZ_NTP_SERVER;
	}
	else if( !strcmp( config_field_name,"TZ_SYS_UPDATE_SERVER" ) )
	{
		nvram_config_field=nvram_config_info->TZ_SYS_UPDATE_SERVER;
	}
	else if( !strcmp( config_field_name,"TZ_PRESTORE_FREQUENCY_FDD" ) )
	{
		nvram_config_field=nvram_config_info->TZ_PRESTORE_FREQUENCY_FDD;
	}
	else if( !strcmp( config_field_name,"TZ_PRESTORE_FREQUENCY_TDD" ) )
	{
		nvram_config_field=nvram_config_info->TZ_PRESTORE_FREQUENCY_TDD;
	}
	else if( !strcmp( config_field_name,"TZ_LOCK_IMSI" ) )
	{
		nvram_config_field=nvram_config_info->TZ_LOCK_IMSI;
	}
	else if( !strcmp( config_field_name,"TZ_DEVICE_TYPE" ) )
	{
		nvram_config_field=nvram_config_info->TZ_DEVICE_TYPE;
	}
	else if( !strcmp( config_field_name,"TZ_MAX_ACCESS_USER_NUM" ) )
	{
		nvram_config_field=nvram_config_info->TZ_MAX_ACCESS_USER_NUM;
	}
	/*
	(.+)
	else if( !strcmp( config_field_name,"\1" ) )
	{
		nvram_config_field=nvram_config_info->\1;
	}
	*/
	else if( !strcmp( config_field_name,"TZ_SUPPORT_SIM_ENCRYPT" ) )
	{
		nvram_config_field=nvram_config_info->TZ_SUPPORT_SIM_ENCRYPT;
	}
	else if( !strcmp( config_field_name,"TZ_ENCRYPT_CARD_VENDOR_ID" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENCRYPT_CARD_VENDOR_ID;
	}
	else if( !strcmp( config_field_name,"TZ_ENCRYPT_KEY_VERSION" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENCRYPT_KEY_VERSION;
	}
	else if( !strcmp( config_field_name,"TZ_ENCRYPT_CARD_TYPE" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENCRYPT_CARD_TYPE;
	}
	else if( !strcmp( config_field_name,"TZ_ENCRYPT_KEY_1" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENCRYPT_KEY_1;
	}
	else if( !strcmp( config_field_name,"TZ_ENCRYPT_KEY_2" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENCRYPT_KEY_2;
	}
	else if( !strcmp( config_field_name,"TZ_ENCRYPT_KEY_3" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENCRYPT_KEY_3;
	}
	else if( !strcmp( config_field_name,"TZ_ENCRYPT_KEY_4" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENCRYPT_KEY_4;
	}
	else if( !strcmp( config_field_name,"TZ_ENCRYPT_KEY_5" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENCRYPT_KEY_5;
	}
	else if( !strcmp( config_field_name,"TZ_ENCRYPT_KEY_6" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENCRYPT_KEY_6;
	}
	else if( !strcmp( config_field_name,"TZ_ENCRYPT_KEY_7" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENCRYPT_KEY_7;
	}
	else if( !strcmp( config_field_name,"TZ_ENCRYPT_KEY_8" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENCRYPT_KEY_8;
	}
	else if( !strcmp( config_field_name,"TZ_ENCRYPT_KEY_9" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENCRYPT_KEY_9;
	}
	else if( !strcmp( config_field_name,"TZ_ENCRYPT_KEY_10" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENCRYPT_KEY_10;
	}
	else if( !strcmp( config_field_name,"TZ_ENCRYPT_KEY_11" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENCRYPT_KEY_11;
	}
	else if( !strcmp( config_field_name,"TZ_ENCRYPT_KEY_12" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENCRYPT_KEY_12;
	}
	else if( !strcmp( config_field_name,"TZ_ENCRYPT_KEY_13" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENCRYPT_KEY_13;
	}
	else if( !strcmp( config_field_name,"TZ_ENCRYPT_KEY_14" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENCRYPT_KEY_14;
	}
	else if( !strcmp( config_field_name,"TZ_ENCRYPT_KEY_15" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENCRYPT_KEY_15;
	}
	else if( !strcmp( config_field_name,"TZ_ENCRYPT_KEY_16" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ENCRYPT_KEY_16;
	}
	else if( !strcmp( config_field_name,"TZ_LOCK_ONE_CELL" ) )
	{
		nvram_config_field=nvram_config_info->TZ_LOCK_ONE_CELL;
	}
	else if( !strcmp( config_field_name,"TZ_LOCKED_EARFCN" ) )
	{
		nvram_config_field=nvram_config_info->TZ_LOCKED_EARFCN;
	}
	else if( !strcmp( config_field_name,"TZ_LOCKED_PHYCELLID" ) )
	{
		nvram_config_field=nvram_config_info->TZ_LOCKED_PHYCELLID;
	}
	else if( !strcmp( config_field_name,"TZ_LOCKED_Enode_ID" ) )
	{
		nvram_config_field=nvram_config_info->TZ_LOCKED_Enode_ID;
	}
	else if( !strcmp( config_field_name,"TZ_FLOW_THRESHOLD" ) )
	{
		nvram_config_field=nvram_config_info->TZ_FLOW_THRESHOLD;
	}
	else if( !strcmp( config_field_name,"TZ_FLOW_WARNING" ) )
	{
		nvram_config_field=nvram_config_info->TZ_FLOW_WARNING;
	}
	else if( !strcmp( config_field_name,"TZ_ISOLATE_WLAN_CLIENTS" ) )
	{
		nvram_config_field=nvram_config_info->TZ_ISOLATE_WLAN_CLIENTS;
	}
	else if( !strcmp( config_field_name,"TZ_L2TP_MTU" ) )
	{
		nvram_config_field=nvram_config_info->TZ_L2TP_MTU;
	}
	else if( !strcmp( config_field_name,"TZ_CPE_ROUTER_MODE" ) )
	{
		nvram_config_field=nvram_config_info->TZ_CPE_ROUTER_MODE;
	}
	else if( !strcmp( config_field_name,"TZ_PPTP_ENABLED" ) )
	{
		nvram_config_field=nvram_config_info->TZ_PPTP_ENABLED;
	}
	else if( !strcmp( config_field_name,"TZ_PPTP_MODE" ) )
	{
		nvram_config_field=nvram_config_info->TZ_PPTP_MODE;
	}
	else if( !strcmp( config_field_name,"TZ_PNS_SERVER" ) )
	{
		nvram_config_field=nvram_config_info->TZ_PNS_SERVER;
	}
	else if( !strcmp( config_field_name,"TZ_PPTP_AUTH_NAME" ) )
	{
		nvram_config_field=nvram_config_info->TZ_PPTP_AUTH_NAME;
	}
	else if( !strcmp( config_field_name,"TZ_PPTP_AUTH_PASS" ) )
	{
		nvram_config_field=nvram_config_info->TZ_PPTP_AUTH_PASS;
	}
	else if( !strcmp( config_field_name,"TZ_PPTP_ENABLE_MPPE" ) )
	{
		nvram_config_field=nvram_config_info->TZ_PPTP_ENABLE_MPPE;
	}
	else if( !strcmp( config_field_name,"TZ_PPTP_MTU" ) )
	{
		nvram_config_field=nvram_config_info->TZ_PPTP_MTU;
	}
	else if( !strcmp( config_field_name,"TZ_VPN_AS_DEFAULT_ROUTE" ) )
	{
		nvram_config_field=nvram_config_info->TZ_VPN_AS_DEFAULT_ROUTE;
	}
	else
	{
		int index;
		for( index=0;index < ALL_NEED_PARSED_FIELDS_COUNT;index++ )
		{
			if( !strcmp( config_field_name,all_need_parsed_fields[ index ].field_name ) )
			{
				return ( (char*)nvram_config_info+all_need_parsed_fields[ index ].field_offset );
			}
		}
	}



	return nvram_config_field;
}

//sublime text replace
//(else )?if\( !strcmp\( config_field_name,"([^\"]*)" \) \)
//if( strlen( nvram_config_info->\2 ) )
//nvram_config_field=(nvram_config_info->([^\;]*));
//printf("\\nexport \2=\\\"%s\\\"",\1);
//if\(\s*strlen\(\s*nvram_config_info->([^\s]*)\s*\)\s*\)
//if( infostruct_is_field_defined( nvram_config_info,\1_DEFINED ) )
void util_export_config_info( InfoStruct *nvram_config_info )
{
	//export AP_SSID="tozed-11274F"
	if( infostruct_is_field_defined( nvram_config_info,AP_SSID_DEFINED ) )
	{
		printf("\nexport AP_SSID=\"%s\"",nvram_config_info->AP_SSID);
	}
	//export TZ_ENABLE_WIFI=yes
	if( infostruct_is_field_defined( nvram_config_info,TZ_ENABLE_WIFI_DEFINED ) )
	{
		printf("\nexport TZ_ENABLE_WIFI=\"%s\"",nvram_config_info->TZ_ENABLE_WIFI);
	}
	//export PWD=6A7FCADA
	if( infostruct_is_field_defined( nvram_config_info,PSK_KEY_DEFINED ) )
	{
		printf("\nexport PWD=\"%s\"",nvram_config_info->PSK_KEY);
		printf("\nexport PSK_KEY=\"%s\"",nvram_config_info->PSK_KEY);
	}
	//export AP_CYPHER=CCMP
	if( infostruct_is_field_defined( nvram_config_info,AP_CYPHER_DEFINED ) )
	{
		printf("\nexport AP_CYPHER=\"%s\"",nvram_config_info->AP_CYPHER);
	}
	//export AP_WPA=2
	if( infostruct_is_field_defined( nvram_config_info,AP_WPA_DEFINED ) )
	{
		printf("\nexport AP_WPA=\"%s\"",nvram_config_info->AP_WPA);
	}
	//export PUREG=0
	if( infostruct_is_field_defined( nvram_config_info,PUREG_DEFINED ) )
	{
		printf("\nexport PUREG=\"%s\"",nvram_config_info->PUREG);
	}
	//export RATECTL=manual
	if( infostruct_is_field_defined( nvram_config_info,RATECTL_DEFINED ) )
	{
		printf("\nexport RATECTL=\"%s\"",nvram_config_info->RATECTL);
	}
	//export MANRATE=0x87868584
	if( infostruct_is_field_defined( nvram_config_info,MANRATE_DEFINED ) )
	{
		printf("\nexport MANRATE=\"%s\"",nvram_config_info->MANRATE);
	}
	//export MANRETRIES=0x04040404
	if( infostruct_is_field_defined( nvram_config_info,MANRETRIES_DEFINED ) )
	{
		printf("\nexport MANRETRIES=\"%s\"",nvram_config_info->MANRETRIES);
	}
	//export WIFIWORKMODE=m11b
	if( infostruct_is_field_defined( nvram_config_info,WIFIWORKMODE_DEFINED ) )
	{
		printf("\nexport WIFIWORKMODE=\"%s\"",nvram_config_info->WIFIWORKMODE);
	}
	//export PUREN=0
	if( infostruct_is_field_defined( nvram_config_info,PUREN_DEFINED ) )
	{
		printf("\nexport PUREN=\"%s\"",nvram_config_info->PUREN);
	}
	//export AP_SECMODE=WPA
	if( infostruct_is_field_defined( nvram_config_info,AP_SECMODE_DEFINED ) )
	{
		printf("\nexport AP_SECMODE=\"%s\"",nvram_config_info->AP_SECMODE);
	}
	//export AP_SECFILE=PSK
	if( infostruct_is_field_defined( nvram_config_info,AP_SECFILE_DEFINED ) )
	{
		printf("\nexport AP_SECFILE=\"%s\"",nvram_config_info->AP_SECFILE);
	}
	//export AP_WPA_GROUP_REKEY=600
	if( infostruct_is_field_defined( nvram_config_info,AP_WPA_GROUP_REKEY_DEFINED ) )
	{
		printf("\nexport AP_WPA_GROUP_REKEY=\"%s\"",nvram_config_info->AP_WPA_GROUP_REKEY);
	}
	//export AP_WPA_GMK_REKEY=86400
	if( infostruct_is_field_defined( nvram_config_info,AP_WPA_GMK_REKEY_DEFINED ) )
	{
		printf("\nexport AP_WPA_GMK_REKEY=\"%s\"",nvram_config_info->AP_WPA_GMK_REKEY);
	}
	//export AP_WEP_MODE_0=2
	if( infostruct_is_field_defined( nvram_config_info,AP_WEP_MODE_0_DEFINED ) )
	{
		printf("\nexport AP_WEP_MODE_0=\"%s\"",nvram_config_info->AP_WEP_MODE_0);
	}
	//export AP_PRIMARY_KEY_0=1
	if( infostruct_is_field_defined( nvram_config_info,AP_PRIMARY_KEY_0_DEFINED ) )
	{
		printf("\nexport AP_PRIMARY_KEY_0=\"%s\"",nvram_config_info->AP_PRIMARY_KEY_0);
	}
	//export WEP_RADIO_NUM0_KEY_1="abcdefghijklm"
	if( infostruct_is_field_defined( nvram_config_info,WEP_RADIO_NUM0_KEY_1_DEFINED ) )
	{
		printf("\nexport WEP_RADIO_NUM0_KEY_1=\"%s\"",nvram_config_info->WEP_RADIO_NUM0_KEY_1);
	}
	//export WEP_RADIO_NUM0_KEY_2="abcdefghijklm"
	if( infostruct_is_field_defined( nvram_config_info,WEP_RADIO_NUM0_KEY_2_DEFINED ) )
	{
		printf("\nexport WEP_RADIO_NUM0_KEY_2=\"%s\"",nvram_config_info->WEP_RADIO_NUM0_KEY_2);
	}
	//export WEP_RADIO_NUM0_KEY_3="abcdefghijklm"
	if( infostruct_is_field_defined( nvram_config_info,WEP_RADIO_NUM0_KEY_3_DEFINED ) )
	{
		printf("\nexport WEP_RADIO_NUM0_KEY_3=\"%s\"",nvram_config_info->WEP_RADIO_NUM0_KEY_3);
	}
	//export WEP_RADIO_NUM0_KEY_4="abcdefghijklm"
	if( infostruct_is_field_defined( nvram_config_info,WEP_RADIO_NUM0_KEY_4_DEFINED ) )
	{
		printf("\nexport WEP_RADIO_NUM0_KEY_4=\"%s\"",nvram_config_info->WEP_RADIO_NUM0_KEY_4);
	}
	//export AP_HIDESSID=0
	if( infostruct_is_field_defined( nvram_config_info,AP_HIDESSID_DEFINED ) )
	{
		printf("\nexport AP_HIDESSID=\"%s\"",nvram_config_info->AP_HIDESSID);
	}
	//export AP_IPADDR="192.168.0.1"
	if( infostruct_is_field_defined( nvram_config_info,AP_IPADDR_DEFINED ) )
	{
		printf("\nexport AP_IPADDR=\"%s\"",nvram_config_info->AP_IPADDR);
	}
	//export AP_NETMASK="255.255.255.0"
	if( infostruct_is_field_defined( nvram_config_info,AP_NETMASK_DEFINED ) )
	{
		printf("\nexport AP_NETMASK=\"%s\"",nvram_config_info->AP_NETMASK);
	}
	//export TZ_DHCP_IP_BEGIN="192.168.0.100"
	if( infostruct_is_field_defined( nvram_config_info,TZ_DHCP_IP_BEGIN_DEFINED ) )
	{
		printf("\nexport TZ_DHCP_IP_BEGIN=\"%s\"",nvram_config_info->TZ_DHCP_IP_BEGIN);
	}
	//export TZ_DHCP_IP_END="192.168.0.250"
	if( infostruct_is_field_defined( nvram_config_info,TZ_DHCP_IP_END_DEFINED ) )
	{
		printf("\nexport TZ_DHCP_IP_END=\"%s\"",nvram_config_info->TZ_DHCP_IP_END);
	}
	//export TZ_DHCP_EXPIRE_TIME=86400
	if( infostruct_is_field_defined( nvram_config_info,TZ_DHCP_EXPIRE_TIME_DEFINED ) )
	{
		printf("\nexport TZ_DHCP_EXPIRE_TIME=\"%s\"",nvram_config_info->TZ_DHCP_EXPIRE_TIME);
	}
	//export AP_PRIMARY_CH=11
	if( infostruct_is_field_defined( nvram_config_info,AP_PRIMARY_CH_DEFINED ) )
	{
		printf("\nexport AP_PRIMARY_CH=\"%s\"",nvram_config_info->AP_PRIMARY_CH);
	}
	//export AP_CHMODE=11NGHT20
	if( infostruct_is_field_defined( nvram_config_info,AP_CHMODE_DEFINED ) )
	{
		printf("\nexport AP_CHMODE=\"%s\"",nvram_config_info->AP_CHMODE);
	}
	//export TXPOWER=18
	if( infostruct_is_field_defined( nvram_config_info,TXPOWER_DEFINED ) )
	{
		printf("\nexport TXPOWER=\"%s\"",nvram_config_info->TXPOWER);
	}
	//export TZ_ENABLE_WATCHDOG=no
	if( infostruct_is_field_defined( nvram_config_info,TZ_ENABLE_WATCHDOG_DEFINED ) )
	{
		printf("\nexport TZ_ENABLE_WATCHDOG=\"%s\"",nvram_config_info->TZ_ENABLE_WATCHDOG);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_LOCK_CARD_DEFINED ) )
	{
		printf("\nexport TZ_LOCK_CARD=\"%s\"",nvram_config_info->TZ_LOCK_CARD);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_LOCK_CARD_TYPE_DEFINED ) )
	{
		printf("\nexport TZ_LOCK_CARD_TYPE=\"%s\"",nvram_config_info->TZ_LOCK_CARD_TYPE);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_INITIAL_PIN_CODE_DEFINED ) )
	{
		printf("\nexport TZ_INITIAL_PIN_CODE=\"%s\"",nvram_config_info->TZ_INITIAL_PIN_CODE);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_MODIFIED_PIN_CODE_DEFINED ) )
	{
		printf("\nexport TZ_MODIFIED_PIN_CODE=\"%s\"",nvram_config_info->TZ_MODIFIED_PIN_CODE);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_LOCK_CELL_ID_DEFINED ) )
	{
		printf("\nexport TZ_LOCK_CELL_ID=\"%s\"",nvram_config_info->TZ_LOCK_CELL_ID);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_LOCK_CELL_ID_COUNT_DEFINED ) )
	{
		printf("\nexport TZ_LOCK_CELL_ID_COUNT=\"%s\"",nvram_config_info->TZ_LOCK_CELL_ID_COUNT);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_LIMIT_SPEED_ENABLED_DEFINED ) )
	{
		printf("\nexport TZ_LIMIT_SPEED_ENABLED=\"%s\"",nvram_config_info->TZ_LIMIT_SPEED_ENABLED);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_LIMIT_SPEED_VALUE_DEFINED ) )
	{
		printf("\nexport TZ_LIMIT_SPEED_VALUE=\"%s\"",nvram_config_info->TZ_LIMIT_SPEED_VALUE);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_L2TP_MODE_DEFINED ) )
	{
		printf("\nexport TZ_L2TP_MODE=\"%s\"",nvram_config_info->TZ_L2TP_MODE);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_LNS_SERVER_DEFINED ) )
	{
		printf("\nexport TZ_LNS_SERVER=\"%s\"",nvram_config_info->TZ_LNS_SERVER);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_TUNNEL_NAME_DEFINED ) )
	{
		printf("\nexport TZ_TUNNEL_NAME=\"%s\"",nvram_config_info->TZ_TUNNEL_NAME);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_TUNNEL_PASS_DEFINED ) )
	{
		printf("\nexport TZ_TUNNEL_PASS=\"%s\"",nvram_config_info->TZ_TUNNEL_PASS);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_AUTH_NAME_DEFINED ) )
	{
		printf("\nexport TZ_AUTH_NAME=\"%s\"",nvram_config_info->TZ_AUTH_NAME);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_AUTH_PASS_DEFINED ) )
	{
		printf("\nexport TZ_AUTH_PASS=\"%s\"",nvram_config_info->TZ_AUTH_PASS);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_CONFIG_FILE_VERSION_DEFINED ) )
	{
		printf("\nexport TZ_CONFIG_FILE_VERSION=\"%s\"",nvram_config_info->TZ_CONFIG_FILE_VERSION);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_CONFIG_FILE_DESCRIPTION_DEFINED ) )
	{
		printf("\nexport TZ_CONFIG_FILE_DESCRIPTION=\"%s\"",nvram_config_info->TZ_CONFIG_FILE_DESCRIPTION);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_ENABLE_PPP_FORWARD_DEFINED ) )
	{
		printf("\nexport TZ_ENABLE_PPP_FORWARD=\"%s\"",nvram_config_info->TZ_ENABLE_PPP_FORWARD);
	}
	if( infostruct_is_field_defined( nvram_config_info,LAN_LED_AS_LTE_STATUS_DEFINED ) )
	{
		printf("\nexport LAN_LED_AS_LTE_STATUS=\"%s\"",nvram_config_info->LAN_LED_AS_LTE_STATUS);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_PPP_USERNAME_DEFINED ) )
	{
		printf("\nexport TZ_PPP_USERNAME=\"%s\"",nvram_config_info->TZ_PPP_USERNAME);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_PPP_PASSWORD_DEFINED ) )
	{
		printf("\nexport TZ_PPP_PASSWORD=\"%s\"",nvram_config_info->TZ_PPP_PASSWORD);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_ENABLE_HSK_DDNS_SERVER_DEFINED ) )
	{
		printf("\nexport TZ_ENABLE_HSK_DDNS_SERVER=\"%s\"",nvram_config_info->TZ_ENABLE_HSK_DDNS_SERVER);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_DDNS_HSK_USERNAME_DEFINED ) )
	{
		printf("\nexport TZ_DDNS_HSK_USERNAME=\"%s\"",nvram_config_info->TZ_DDNS_HSK_USERNAME);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_DDNS_HSK_PASSWORD_DEFINED ) )
	{
		printf("\nexport TZ_DDNS_HSK_PASSWORD=\"%s\"",nvram_config_info->TZ_DDNS_HSK_PASSWORD);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_DDNS_INTERFACE_DEFINED ) )
	{
		printf("\nexport TZ_DDNS_INTERFACE=\"%s\"",nvram_config_info->TZ_DDNS_INTERFACE);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_ENABLE_3322_DDNS_SERVER_DEFINED ) )
	{
		printf("\nexport TZ_ENABLE_3322_DDNS_SERVER=\"%s\"",nvram_config_info->TZ_ENABLE_3322_DDNS_SERVER);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_DDNS_3322_USERNAME_DEFINED ) )
	{
		printf("\nexport TZ_DDNS_3322_USERNAME=\"%s\"",nvram_config_info->TZ_DDNS_3322_USERNAME);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_DDNS_3322_PASSWORD_DEFINED ) )
	{
		printf("\nexport TZ_DDNS_3322_PASSWORD=\"%s\"",nvram_config_info->TZ_DDNS_3322_PASSWORD);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_3322_DOMAIN_DEFINED ) )
	{
		printf("\nexport TZ_3322_DOMAIN=\"%s\"",nvram_config_info->TZ_3322_DOMAIN);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_ENABLE_NOIP_DDNS_SERVER_DEFINED ) )
	{
		printf("\nexport TZ_ENABLE_NOIP_DDNS_SERVER=\"%s\"",nvram_config_info->TZ_ENABLE_NOIP_DDNS_SERVER);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_DDNS_NOIP_USERNAME_DEFINED ) )
	{
		printf("\nexport TZ_DDNS_NOIP_USERNAME=\"%s\"",nvram_config_info->TZ_DDNS_NOIP_USERNAME);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_DDNS_NOIP_PASSWORD_DEFINED ) )
	{
		printf("\nexport TZ_DDNS_NOIP_PASSWORD=\"%s\"",nvram_config_info->TZ_DDNS_NOIP_PASSWORD);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_NOIP_DOMAIN_DEFINED ) )
	{
		printf("\nexport TZ_NOIP_DOMAIN=\"%s\"",nvram_config_info->TZ_NOIP_DOMAIN);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_GET_DNS_AUTO_DEFINED ) )
	{
		printf("\nexport TZ_GET_DNS_AUTO=\"%s\"",nvram_config_info->TZ_GET_DNS_AUTO);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_DNS_PRIMARY_DEFINED ) )
	{
		printf("\nexport TZ_DNS_PRIMARY=\"%s\"",nvram_config_info->TZ_DNS_PRIMARY);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_DNS_SECONDARY_DEFINED ) )
	{
		printf("\nexport TZ_DNS_SECONDARY=\"%s\"",nvram_config_info->TZ_DNS_SECONDARY);
	}
	if( infostruct_is_field_defined( nvram_config_info,TZ_BAND_PREF_DEFINED ) )
	{
		printf("\nexport TZ_BAND_PREF=\"%s\"",nvram_config_info->TZ_BAND_PREF);
	}
}

//获取拨号方式
int util_get_TZ_L2TP_MODE( InfoStruct* nvram_config_info )
{
	return atoi( nvram_config_info->TZ_L2TP_MODE );
}

//是否开启了PPP转发来实现用户PPPOE拨号上网
int util_get_TZ_ENABLE_PPP_FORWARD( InfoStruct* nvram_config_info )
{
	return atoi( nvram_config_info->TZ_ENABLE_PPP_FORWARD );
}

//是否需要设置LTE灯的状态
int util_get_LAN_LED_AS_LTE_STATUS( InfoStruct* nvram_config_info )
{
	if( is_this_device_p13() )
	{
		return FALSE;
	}

	return ( is_this_device_p11plus() && !cmd_file_exist( LAN_LED_AS_WLAN_STATUS ) );
}


//读取版本相关的信息
int util_read_version_info( const char* config_file,InfoStruct* server_wifi_info )
{
	char* p1;
	char* field_name;
	char* field_value;
	char temp_buffer[256];
	FILE* file_handle=fopen( config_file,"r" );

	if( file_handle == NULL )
	{
		return -1;
	}

	memset( temp_buffer,0,sizeof(temp_buffer) );
	/*
	~ # cat /version
	type:tozedap-3g-wcdma-router
	version:3
	hwversion:TZ7.821.151
	device:ZLT MF210
	build:2014-08-10_15:01
	branch:master
	os:Debian
	customer:HEADELE
	*/
	while( fgets( temp_buffer,sizeof(temp_buffer)-1,file_handle ) != NULL )
	{
		p1=strstr(temp_buffer,":");
		if( p1 == NULL )
		{
			continue;
		}

		if( p1 == temp_buffer )
		{
			continue;
		}

		field_name=temp_buffer;
		while( ( *field_name == ' ' ) || ( *field_name == '\t' ) )
		{
			field_name++;
		}

		*p1=0;
		field_value=p1+1;
		while( ( *field_value == ' ' ) || ( *field_value == '\t' ) )
		{
			field_value++;
		}

		for(	p1=field_value+strlen( field_value )-1;
				( ( *p1 == '\r' ) || ( *p1 == '\n' ) || ( *p1 == ' ' ) || ( *p1 == '\t' ) )&&( p1 >= field_value );

			)
		{
			*p1=0;
			p1--;
		}

		if( !strcmp( field_name,"type" ) )
		{
			STRCPY_S( server_wifi_info->type,field_value );
		}
		else if( !strcmp( field_name,"version" ) )
		{
			STRCPY_S( server_wifi_info->version,field_value );
		}
		else if( !strcmp( field_name,"hwversion" ) )
		{
			STRCPY_S( server_wifi_info->hwversion,field_value );
		}
		else if( !strcmp( field_name,"device" ) )
		{
			STRCPY_S( server_wifi_info->device,field_value );
		}
		else if( !strcmp( field_name,"build" ) )
		{
			STRCPY_S( server_wifi_info->build,field_value );
		}
		else if( !strcmp( field_name,"branch" ) )
		{
			STRCPY_S( server_wifi_info->branch,field_value );
		}
		else if( !strcmp( field_name,"os" ) )
		{
			STRCPY_S( server_wifi_info->os,field_value );
		}
		else if( !strcmp( field_name,"sha1" ) )
		{
			STRCPY_S( server_wifi_info->sha1,field_value );
		}
		else if( !strcmp( field_name,"customer" ) )
		{
			STRCPY_S( server_wifi_info->customer,field_value );
		}
	}

	//关掉文件句柄
	fclose( file_handle );

	return 0;
}

//获取文件大小,文件当前的读取位置不会改变
int util_get_file_size_by_file_handle( FILE* file_handle )
{
	unsigned int current_read_position=ftell( file_handle );
	int file_size;
	fseek( file_handle,0,SEEK_END );
	//获取文件的大小
	file_size=ftell( file_handle );
	//恢复文件原来读取的位置
	fseek( file_handle,current_read_position,SEEK_SET );

	return file_size;
}

//获取文件大小,如果文件不存在则返回-1
int util_get_file_size_by_file_name( const char* file_name )
{
	FILE* file_handle;
	int file_size;
	//文件不存在
	if( !cmd_file_exist( file_name ) )
	{
		return -1;
	}

	//打开文件进行读取
	file_handle=fopen( file_name,"rb" );
	//打开文件失败
	if( file_handle == NULL )
	{
		return -1;
	}
	file_size=util_get_file_size_by_file_handle( file_handle );
	//关掉文件句柄
	fclose( file_handle );

	//返回文件大小
	return file_size;
}

//清空mac控制信息
void util_reset_mac_control_info( MacControlInfo* mac_control_info )
{
	//首先删除文件MAC_CONTROL_INFO_FILE
	cmd_rm( MAC_CONTROL_INFO_FILE );

	//对信道信息进行重置
	memset( mac_control_info,0,sizeof( MacControlInfo ) );
}

//读取MAC控制信息
void util_read_mac_control_info( MacControlInfo* mac_control_info,int is_used_by_web_page )
{
	//文件存在
	if( cmd_file_exist( MAC_CONTROL_INFO_FILE ) )
	{
		//打开文件进行读取
		FILE* file_handle=fopen( MAC_CONTROL_INFO_FILE,"rb" );

		//打开文件失败
		if( file_handle == NULL )
		{
			return ;
		}
		else
		{
			unsigned int file_size=util_get_file_size_by_file_handle( file_handle );
			//文件大小存在问题
			if( file_size != sizeof( MacControlInfo ) )
			{
				//关掉文件句柄
				fclose( file_handle );
				return ;
			}

			//读取文件的内容
			fread( mac_control_info,sizeof( MacControlInfo ),1,file_handle );
		}

		//关掉文件句柄
		fclose( file_handle );
	}
}

//保存mac控制信息
void util_save_mac_control_info( MacControlInfo* mac_control_info,int is_used_by_web_page )
{
	//打开文件进行写入
	FILE* file_handle=fopen( MAC_CONTROL_INFO_FILE,"wb" );

	//打开文件失败
	if( file_handle == NULL )
	{
		return;
	}

	//将数据写入文件
	fwrite( mac_control_info,sizeof( MacControlInfo ),1,file_handle );

	//关掉文件句柄
	fclose( file_handle );

	//通知配置服务器信息已经发生了改变
	if( is_used_by_web_page )
	{
		cmd_touch( MAC_CONTROL_INFO_FILE_CHANGED_TMP_FILE );
	}
}

//保存服务器的mac地址
void util_save_server_mac( unsigned char* remote_mac_addr )
{
	static MacAddr mac_addr;
	//打开文件进行写入
	FILE* file_handle;
	int file_size_error=FALSE;

	//首先检测文件的大小是否正确
	if( cmd_file_exist( ALL_MACS_TMP_FILE ) )
	{
		int file_size=util_get_file_size_by_file_name( ALL_MACS_TMP_FILE );
		//文件大小有问题，删除
		if( file_size != sizeof( MacAddr ) )
		{
			cmd_rm( ALL_MACS_TMP_FILE );
			file_size_error=TRUE;
		}
	}

	//如果文件大小没有问题的情况下,要写入的mac地址和保存到mac地址相同,则不重新进行保存
	if( ( !file_size_error ) && ( !memcmp( mac_addr.value,remote_mac_addr,sizeof(mac_addr.value) ) ) )
	{
		return;
	}

	file_handle=fopen( ALL_MACS_TMP_FILE,"wb" );

	//清空内容
	memset( &mac_addr,0,sizeof(mac_addr) );
	//copy值信息
	memcpy( mac_addr.value,remote_mac_addr,sizeof(mac_addr.value) );

	//打开文件失败
	if( file_handle == NULL )
	{
		return;
	}

	//将数据写入文件
	fwrite( &mac_addr,sizeof( MacAddr ),1,file_handle );

	//关掉文件句柄
	fclose( file_handle );
}

//保存客户端的mac地址
void util_save_client_macs( MacBuffer* all_client_macs )
{
	//打开文件进行写入
	FILE* file_handle;
	int file_size_error=FALSE;
	static MacBuffer macs_buffer;

	//首先检测文件的大小是否正确
	if( cmd_file_exist( ALL_MACS_TMP_FILE ) )
	{
		int file_size=util_get_file_size_by_file_name( ALL_MACS_TMP_FILE );
		//文件大小有问题，删除
		if( file_size != sizeof( MacBuffer ) )
		{
			cmd_rm( ALL_MACS_TMP_FILE );
			file_size_error=TRUE;
		}
	}

	//如果文件大小没有问题的情况下,要写入的mac地址和保存到mac地址相同,则不重新进行保存
	if( ( !file_size_error ) && ( !memcmp( &macs_buffer,all_client_macs,sizeof(macs_buffer) ) ) )
	{
		return;
	}

	memcpy( &macs_buffer,all_client_macs,sizeof(macs_buffer) );

	file_handle=fopen( ALL_MACS_TMP_FILE,"wb" );

	//打开文件失败
	if( file_handle == NULL )
	{
		return;
	}

	//将数据写入文件
	fwrite( all_client_macs,sizeof( MacBuffer ),1,file_handle );

	//关掉文件句柄
	fclose( file_handle );
}

//装载所有的MAC地址,成功返回0,失败返回-1
int util_load_saved_macs( MacBuffer* mac_struct )
{
	if( cmd_file_exist( ALL_MACS_TMP_FILE ) )
	{
		FILE* file_handle;
		//获取文件大小
		int file_size=util_get_file_size_by_file_name( ALL_MACS_TMP_FILE );

		file_handle=fopen( ALL_MACS_TMP_FILE,"rb" );

		//打开文件失败
		if( file_handle == NULL )
		{
			return -1;
		}
		//保存的是服务器MAC地址
		if( file_size == sizeof( MacAddr ) )
		{
			fread( mac_struct,sizeof( MacAddr ),1,file_handle );
			mac_struct->mac_count=1;
		}
		//保存的是客户端MAC地址
		else if( file_size == sizeof( MacBuffer ) )
		{
			fread( mac_struct,sizeof( MacBuffer ),1,file_handle );
		}
		else
		{
			//关掉文件句柄
			fclose( file_handle );
			//删除文件
			cmd_rm( ALL_MACS_TMP_FILE );
			return -1;
		}

		//关掉文件句柄
		fclose( file_handle );
		return 0;
	}
	else
	{
		return -1;
	}
}

//获取指定mac对应的信息,成功获取信息返回0，否则返回-1
int util_get_mac_info( MacControlInfo* mac_control_info,unsigned char* mac_addr,MacIPPair* ip_pair_info )
{
	int index=0;
	while( ( index < mac_control_info->mac_count ) && ( index < MAX_MAC_COUNT ) )
	{
		//mac地址匹配
		if( !memcmp( mac_control_info->ip_pair_info[ index ].mac.value,mac_addr,sizeof( mac_control_info->ip_pair_info[ index ].mac.value ) ) )
		{
			memcpy( ip_pair_info,&( mac_control_info->ip_pair_info[ index ] ),sizeof( MacIPPair ) );
			return 0;
		}

		index+=1;
	}

	return -1;
}


//写入指定mac对应的信息,该函数不会直接进行写入，需要调用函数util_save_mac_control_info进行保存,成功获取信息返回0，否则返回-1
int util_set_mac_info_ex( MacControlInfo* mac_control_info,unsigned char* mac_addr,char* ip_addr,char* ssid,char* primary_ch,char* txpower )
{
	int index=0;
	while( ( index < mac_control_info->mac_count ) && ( index < MAX_MAC_COUNT ) )
	{
		//mac地址匹配
		if( !memcmp( mac_control_info->ip_pair_info[ index ].mac.value,mac_addr,sizeof( mac_control_info->ip_pair_info[ index ].mac.value ) ) )
		{
			strcpy( mac_control_info->ip_pair_info[ index ].AP_IPADDR,ip_addr );
			strcpy( mac_control_info->ip_pair_info[ index ].AP_SSID,ssid );
			strcpy( mac_control_info->ip_pair_info[ index ].AP_PRIMARY_CH,primary_ch );
			strcpy( mac_control_info->ip_pair_info[ index ].TXPOWER,txpower );
			return 0;
		}

		index+=1;
	}

	//没有匹配的信息
	if( mac_control_info->mac_count < MAX_MAC_COUNT )
	{
		memcpy( mac_control_info->ip_pair_info[ mac_control_info->mac_count ].mac.value,mac_addr,sizeof( mac_control_info->ip_pair_info[ mac_control_info->mac_count ].mac.value ) );
		strcpy( mac_control_info->ip_pair_info[ mac_control_info->mac_count ].AP_IPADDR,ip_addr );
		strcpy( mac_control_info->ip_pair_info[ mac_control_info->mac_count ].AP_SSID,ssid );
		strcpy( mac_control_info->ip_pair_info[ mac_control_info->mac_count ].AP_PRIMARY_CH,primary_ch );
		strcpy( mac_control_info->ip_pair_info[ mac_control_info->mac_count ].TXPOWER,txpower );
		//保存设置
		mac_control_info->mac_count+=1;
		return 0;
	}


	return -1;
}

//保存信息
int util_set_mac_info2( MacControlInfo* mac_control_info,MacIPPair* mac_ip_pair )
{
	int index=0;
	while( ( index < mac_control_info->mac_count ) && ( index < MAX_MAC_COUNT ) )
	{
		//mac地址匹配
		if( !memcmp( mac_control_info->ip_pair_info[ index ].mac.value,mac_ip_pair->mac.value,sizeof( mac_control_info->ip_pair_info[ index ].mac.value ) ) )
		{
			memcpy( mac_control_info->ip_pair_info+index,mac_ip_pair,sizeof( MacIPPair ) );
			return 0;
		}

		index+=1;
	}

	//没有匹配的信息
	if( mac_control_info->mac_count < MAX_MAC_COUNT )
	{
		memcpy( mac_control_info->ip_pair_info+mac_control_info->mac_count,mac_ip_pair,sizeof( MacIPPair ) );
		//保存设置
		mac_control_info->mac_count+=1;
		return 0;
	}

	return -1;
}

int util_set_mac_info( MacControlInfo* mac_control_info,unsigned char* mac_addr,char* ip_addr,char* ssid,char* primary_ch )
{
	char* txpower="";
	return util_set_mac_info_ex( mac_control_info,mac_addr,ip_addr,ssid,primary_ch,txpower );
}

int util_char2hex(char c)
{
	unsigned char tmpData;
	if ((c >= '0') && (c <= '9'))
	{
		tmpData = c - '0';
	}
	else if ((c >= 'A') && (c <= 'F')) //A....F
	{
		tmpData = c - 'A'+10;
	}
	else if((c >= 'a') && (c <= 'f')) //a....f
	{
		tmpData = c - 'a'+10;
	}
	else
	{
		tmpData = 0;
	}

	return tmpData;
}

void util_get_mac(char *buf, unsigned char *mac)
{
	int i ;
	unsigned char v;
	char ptr[5];
	for(i=0; i<6; i++)
	{
		ptr[0] = buf[i*3];
		ptr[1] = buf[i*3+1];
		ptr[2] = 0;
                //printf(ptr);

		//sscanf(ptr, "%x", &v);
		v = (util_char2hex(ptr[0])<<4) + util_char2hex(ptr[1]);
		//printf(" %x \r\n", v);
		*mac++ = v;
	}
}

//删除指定mac地址相关的信息,成功获取信息返回0，否则返回-1
void util_delete_mac_info( MacControlInfo* mac_control_info,unsigned char* mac_addr )
{
	int index=0;
	while( ( index < mac_control_info->mac_count ) && ( index < MAX_MAC_COUNT ) )
	{
		//mac地址匹配
		if( !memcmp( mac_control_info->ip_pair_info[ index ].mac.value,mac_addr,sizeof( mac_control_info->ip_pair_info[ index ].mac.value ) ) )
		{
			int tmp_index=index;
			while( tmp_index < mac_control_info->mac_count-1 )
			{
				memcpy( &( mac_control_info->ip_pair_info[ tmp_index ] ),&( mac_control_info->ip_pair_info[ tmp_index+1 ] ),sizeof( mac_control_info->ip_pair_info[ tmp_index ] ) );
				tmp_index+=1;
			}

			mac_control_info->mac_count-=1;
			return;
		}

		index+=1;
	}
}

static const unsigned char file_encrypt_codes[]={ 0x78,0x89,0x9a,0xab,0xbc,0xcd,0xde,0xef };
//根据字节序数的最低三位,依次和0x78,0x89,0x9a,0xab,0xbc,0xcd,0xde,0xef相异或,最后将生成的配置文件保存为一个二进制文件;
//对文件内容进行加密
void util_encrypt_file_content( unsigned char* file_content,int file_size )
{
	int index=0;
	while( index < file_size )
	{
		file_content[ index ]=file_content[ index ]^file_encrypt_codes[ index&0x07 ];
		index+=1;
	}
}

//更新邻居ARP缓存
void util_update_neighbour_arp_cache( const char* network_dev_name,const char* ip_addr )
{
	char cmd_buffer[128];
	snprintf(cmd_buffer
			,sizeof( cmd_buffer )
			,"arping -A -c 1 -I %s -s %s 255.255.255.255"
			,network_dev_name
			,ip_addr
			);
	system(cmd_buffer);
}

//是否需要用户手动进行拨号
int util_should_dial_lte_manually( void )
{
	return cmd_file_exist( LTE_DIAL_MANUALLY );
}

//是否需要马上断开lte拨号连接
int util_should_disconnect_from_lte_network_immediately( void )
{
	return cmd_file_exist( LTE_DISCONNECT_FROM_NETWORK_IMMEDIATELY );
}

//执行大数字比较
int util_big_number_compare(BigNum* number1,BigNum* number2)
{
	int result;

	result=( number1->high > number2->high )-( number1->high < number2->high );
	if( result )
	{
		return result;
	}

	result=( number1->low > number2->low )-( number1->low < number2->low );
	return result;
}

//执行大数字加法
void util_big_number_add(BigNum* number1,BigNum* number2,BigNum* result)
{
	BigNum temp;
	temp.low=number1->low+number2->low;
	temp.high=number1->high+number2->high;

	if(
		( temp.low < number1->low )
		&&( temp.low < number2->low )
	)
	{
		temp.high++;
	}

	memcpy(result,&temp,sizeof(temp));
}

//执行大数字减法
void util_big_number_sub(BigNum* number1,BigNum* number2,BigNum* result)
{
	BigNum temp;
	if(number1->low < number2->low)
	{
		temp.low=( ~number2->low )+1+number1->low;
		number1->high--;
	}
	else
	{
		temp.low=number1->low-number2->low;
	}

	temp.high=number1->high-number2->high;

	memcpy(result,&temp,sizeof(temp));
}

/*
~ # cat /proc/net/dev
Inter-|   Receive                                                |  Transmit
 face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed
    lo:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
  eth0:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
  eth1: 9665855  131252  175  295    0   175          0       160 322029473  219411    0    0    0     0       0          0
   br0: 7557005  133941    0    0    0     0          0       486 322396874  221243    0    0    0     0       0          0
 wifi0:       0   15456    0    0    0     0          0         0        0   16253 1392    0    0     0       0          0
  ath0:  468089    2781    0    0    0     0          0         0  1603072    3746    0    2    0     0       0          0
  usb0:639234252  241979    0    0    0     0          0         0
  136266    0 127536    0     0       0          0
~ #
 * */

//将char转换为数字形式
void util_decimal_number_convert(char* number_str,BigNum* result)
{
	int length=strlen( number_str );
	char number[32];
	unsigned int temp;

	strcpy(number,number_str);

	result->high=0;
	result->low=0;
	//2147483647
	if( length <= 9 )
	{
		result->low=atoi(number);
	}
	else
	{
		unsigned int high_value;
		result->low=atoi(number+length-9);
		number[length-9]=0;
		high_value=atoi(number);
		result->high=0;

		//adjust the number low
		while(  high_value )
		{
			temp=result->low+1000000000;
			if(
				( temp < result->low )
				&&( temp < 1000000000u )
			)
			{
				result->high++;
			}

			result->low=temp;
			high_value--;
		}
	}
}

//MF210模块被使用
int is_module_mf210_used(void)
{
	return cmd_file_exist( MF210_IS_USED );
}

//SIM5360模块被使用
int is_module_sim5360_used(void)
{
	return cmd_file_exist( SIM5360_IS_USED );
}

//P500模块被使用
int is_module_p500_used(void)
{
	if( cmd_file_exist( BM806_IS_USED ) )
	{
		return TRUE;
	}

	return cmd_file_exist( P500_IS_USED );
}

//检查使用的是否为BM806模块
int is_module_bm806_used(void)
{
	return cmd_file_exist( BM806_IS_USED );
}

//L1761模块被使用
int is_module_l1761_used(void)
{
	return cmd_file_exist( L1761_IS_USED );
}
//LT10模块被使用
int is_module_lt10_used(void)
{
	return cmd_file_exist( LT10_IS_USED );
}
//ZM8620模块被使用
int is_module_zm8620_used(void)
{
	return cmd_file_exist( ZM8620_IS_USED );
}

//使用的是否为ZTE 7510模块
int is_module_zte7510_used(void)
{
	return cmd_file_exist( ZTE7510_IS_USED );
}

//使用的是否为ZTE MC2716模块
int is_module_mc2716_used(void)
{
	return cmd_file_exist( MC2716_IS_USED );
}

//使用的是否为龙尚C5300V模块
int is_module_c5300v_used(void)
{
	return cmd_file_exist( C5300V_IS_USED );
}

//使用的是否为龙尚U8300W模块
int is_module_u8300w_used(void)
{
	return cmd_file_exist( U8300W_IS_USED );
}

//是否只有3个LAN端口
int is_there_only_3_lan_ports(void)
{
	return cmd_file_exist( ONLY_3_LAN_PORTS );
}

//使用的是否为SIM7100C模块
int is_module_sim7100_used(void)
{
	if( cmd_file_exist( SIM7230_IS_USED ) )
	{
		return TRUE;
	}

	return cmd_file_exist( SIM7100_IS_USED );
}

int is_module_ar9500_used(void)
{
	if( cmd_file_exist( AR9500_IS_USED ) )
	{
		return TRUE;
	}

	return cmd_file_exist( AR9500_IS_USED );
}

//使用的是否为SIM7230_IS_USED模块
int is_module_sim7230_used(void)
{
	return cmd_file_exist( SIM7230_IS_USED );
}


//使用的是否为SLM630B模块
int is_module_slm630b_used(void)
{
	return cmd_file_exist( SLM630B_IS_USED );
}

//使用的是否为BLS001模块
int is_module_bls001_used(void)
{
	return cmd_file_exist( BLS001_IS_USED );
}

//使用的是否为ML7810模块
int is_module_ml7810_used(void)
{
	if( is_module_b78_used() )
	{
		return TRUE;
	}

	if( is_module_b53_used() )
	{
		return TRUE;
	}

	if( is_module_ml7820_used() )
	{
		return TRUE;
	}

	if( is_module_b90_used() )
	{
		return TRUE;
	}

	return cmd_file_exist( ML7810_IS_USED );
}

//使用的是否为ML7820模块
int is_module_ml7820_used(void)
{
	if( is_module_b90_used() )
	{
		return TRUE;
	}

	return cmd_file_exist( ML7820_IS_USED );
}

//使用的是否为B78模块
int is_module_b78_used(void)
{
	if( is_module_b53_used() )
	{
		return TRUE;
	}

	if( is_module_b90_used() )
	{
		return TRUE;
	}

	return cmd_file_exist( B78_IS_USED );
}

//使用的是否为B53模块
int is_module_b53_used(void)
{
	return cmd_file_exist( B53_IS_USED );
}

//使用的是否为B90模块
int is_module_b90_used(void)
{
	return cmd_file_exist( B90_IS_USED );
}

//使用的是否为移远EC20模块
int is_module_ec20_used(void)
{
	return cmd_file_exist( EC20_IS_USED );
}

//当前设备是否为P11+
int is_this_device_p11plus(void)
{
	return cmd_file_exist( CURRENT_DEVICE_IS_P11PLUS );
}

//当前设备是否为P13
int is_this_device_p13(void)
{
	return cmd_file_exist( CURRENT_DEVICE_IS_P13 );
}

int is_this_device_p51(void)
{
	return cmd_file_exist( CURRENT_DEVICE_IS_P51);
}

//当前设备是否为P21
int is_this_device_p21_voip(void)
{
	return cmd_file_exist( CURRENT_DEVICE_IS_P21_VOIP );
}

//是否应该设置WIFI设置界面
int util_should_display_wifi_settings_page(void)
{
	return cmd_file_exist( DISPLAY_WIFI_SETTINGS_PAGE );
}

//是否需要单独重启WIFI模块
int util_is_should_reboot_wifi_module(void)
{
	return cmd_file_exist( REBOOT_WIFI_MODULE );
}

//串口ttyUSB1是否被使用
int is_ttyusb1_used(void)
{
	return cmd_file_exist( TTYUSB1_IS_USED );
}

//使用获取的IPv6 DNS信息
int is_acquired_ipv6_dns_info_used(void)
{
	return cmd_file_exist( CONFIG_USE_ACQUIRED_IPV6_DNS_INFO );
}

//是否关掉了PNAT功能
int is_pnat_disabled(void)
{
	return cmd_file_exist( CONFIG_DISABLE_PNAT );
}


//是否关掉GSM功能
int is_gsm_disabled(void)
{
	return cmd_file_exist( CONFIG_DISABLE_GSM );
}

//wifi是否被关掉了
int is_wifi_disabled(void)
{
	return cmd_file_exist( CONFIG_DISABLE_WIFI );
}


//是否应该处理针对下位机的路由表
int should_check_downlink_router(void)
{
	return cmd_file_exist( CHECK_DOWNLINK_ROUTER );
}

//不支持GSM网络模式
int util_is_gsm_mode_not_supported( void )
{
	return cmd_file_exist( TZ_GSM_NOT_SUPPORTED_BIT_FLAG_FILE );
}

//不支持WCDMA
int util_is_wcdma_mode_not_supported( void )
{
	return cmd_file_exist( TZ_WCDMA_NOT_SUPPORTED_BIT_FLAG_FILE );
}

//不支持TDS-CDMA
int util_is_tdscdma_mode_not_supported( void )
{
	return cmd_file_exist( TZ_TDSCDMA_NOT_SUPPORTED_BIT_FLAG_FILE );
}

//不支持LTE
int util_is_lte_mode_not_supported( void )
{
	return cmd_file_exist( TZ_LTE_NOT_SUPPORTED_BIT_FLAG_FILE );
}

//优先选择WCDMA网络
int util_is_wcdma_preferred( void )
{
	return cmd_file_exist( TZ_WCDMA_PREFERRED_BIT_FLAG_FILE );
}

//是否为入库版本
int is_ruku_special_version(void)
{
	return cmd_file_exist( RUKU_SPECIAL_VERSION );
}

//检查usb设备是否存在,如果usb设备不存在,则进行等待,直到存在相应的usb设备
int check_if_usb_device_exist( const UsbPair* matched_usb_pairs,int usb_pairs_count,int* matched_device_index )
{
	char temp_buffer[256];
	char match_buffer[64];
	FILE* file_handle;
	const char* devices_file="/proc/bus/usb/devices";
	int index;
	char* match_str;

	//指定的文件不存在,则继续进行等待
	if( !cmd_file_exist( devices_file ) )
	{
#ifndef __i386__
		return FALSE;
#else
		return TRUE;
#endif
	}

	//读取文件的内容,判定设备是否存在
	file_handle=fopen( devices_file,"r" );

	memset( temp_buffer,0,sizeof(temp_buffer) );
	while( fgets( temp_buffer,sizeof(temp_buffer)-1,file_handle ) != NULL )
	{
		index=0;

		while( index < usb_pairs_count )
		{
			snprintf(match_buffer
					,sizeof( match_buffer )
					,"Vendor=%04x"
					,matched_usb_pairs[index].vendor_id
					);

			match_str=strstr( temp_buffer,match_buffer );
			if( match_str == NULL )
			{
				index+=1;
				continue;
			}

			snprintf(match_buffer
					,sizeof( match_buffer )
					,"ProdID=%04x"
					,matched_usb_pairs[index].product_id
					);

			match_str=strstr( temp_buffer,match_buffer );
			if( match_str == NULL )
			{
				index+=1;
				continue;
			}

			//usb设备匹配成功
			//关掉文件句柄
			fclose( file_handle );

			if( matched_device_index != NULL )
			{
				*matched_device_index=index;
			}

			return TRUE;
		}
	}
	//关掉文件句柄
	fclose( file_handle );

	return FALSE;
}

static const UsbPair matched_usb_pairs[]=
{
	{0x1ab7,0x1761},
	{0x19d2,0x0117},
	{0x19d2,0x0396},
	{0x05C6,0X9000},
	{0x2020,0x2031},
	{0x2020,0x2032},
	{0x2020,0x2033},
	{0x19d2,0x0199},
	{0x19d2,0xffed},
	{0x1c9e,0x9e00},
	{0x1c9e,0x9b05},
	{0x1e0e,0x9001},
	{0x05c6,0x5042},
	{0x05c6,0x9025},
};

#define USB_PAIRS_COUNT ( sizeof( matched_usb_pairs )/sizeof( matched_usb_pairs[0] ) )

//等待USB设备就绪
void util_wait_until_usb_ready( int wait_for_usb_serial )
{
	//一直进行检查和等待,知道USB设备就绪为止
	while( !check_if_usb_device_exist( matched_usb_pairs,USB_PAIRS_COUNT,NULL ) )
	{
		//如果usb设备不存在,则等待5秒
		#ifdef WIN32
		Sleep(5000);
		#else
		sleep( 2 );
		#endif
	}

	//usb设备匹配成功,等待5秒
	if( wait_for_usb_serial )
	{
		#ifdef WIN32
		Sleep(15000);
		#else
		sleep(5);
		#endif
	}
}


static unsigned char assist_char2hex(char c)
{
	unsigned char tmpData;
	if ((c >= '0') && (c <= '9'))
	{
		tmpData = (unsigned char)c - '0';
	}
	else if ((c >= 'A') && (c <= 'F')) //A....F
	{
		tmpData = (unsigned char)c - 0x37;
	}
	else if((c >= 'a') && (c <= 'f')) //a....f
	{
		tmpData = (unsigned char)c - 0x57;
	}
	else
	{
		tmpData = 0;
	}

	return tmpData;
}

static const char hex_map[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
int util_hex_dump( unsigned char *p_hex,char *p_dump,int len )
{
	char *tmp_p = p_dump;
	int i, pos;
	pos = 0;
	for(i = 0; i < len; i++)
	{
		tmp_p[pos++] = hex_map[ ( p_hex[i]>>4 )&0xf ];
		tmp_p[pos++] = hex_map[ p_hex[i]&0x0f ];
	}
	tmp_p[pos]=0;

	return pos;
}

int util_hex_recovery( char *p_dump,unsigned char *p_hex,int len )
{
	int i,j;
	unsigned char tmpData1, tmpData2, tmpData;
	char *ascii_buf = p_dump;
	unsigned char *hex_buf = p_hex;
	j=0;
	for(i = 0; i < len; i++)
	{
		if( ( ascii_buf[2*i] == 0 )
			||( ascii_buf[2*i+1] == 0 )
			)
		{
			break;
		}

		tmpData1 = assist_char2hex( ascii_buf[2*i] );
		tmpData2= assist_char2hex( ascii_buf[2*i+1] );

		tmpData = (unsigned char)( tmpData1<<4 )+( tmpData2 );

		hex_buf[j++]=tmpData;
	}

	return j;
}

//将数字写入文件中
void write_digit_to_file( const char* file_name,int value )
{
	char tmp_buffer[ 32 ];
	//根据当前文件的大小输出文件下载的进度
	FILE* file_handle=fopen(file_name,"wb");
	//写入当前的进度
	snprintf( tmp_buffer,sizeof( tmp_buffer ),"%d\n",value );

	//写入当前的下载进度
	fwrite( tmp_buffer,strlen(tmp_buffer),1,file_handle );
	//关掉文件句柄
	fclose( file_handle );
}

//将模块重启事件写入log文件中,写入的内容的格式:
//<进程>,<系统运行的时间>,<事件>
//使用系统运行的时间的目的是避免系统的时间错误所带的困扰,文件最大1M字节,不进行删除
void util_write_exception_log(const char*process_name,const char* event_description)
{
	char uptime_buffer[64];
	FILE* file_handle;
	int file_size=util_get_file_size_by_file_name( REBOOT_MODULE_LOG_FILE );
	int tmp_value;

	//通过syslog输出调试信息
	debug( "%s:%s",process_name,event_description );

	if( file_size >= 1048576 )
	{
		return;
	}

	//开始进行文件的写入
	if( cmd_file_exist( REBOOT_MODULE_LOG_FILE ) )
	{
		file_handle=fopen( REBOOT_MODULE_LOG_FILE,"a" );
	}
	else
	{
		file_handle=fopen( REBOOT_MODULE_LOG_FILE,"wb" );
	}

	//写入进程名称
	fwrite( process_name,strlen( process_name ),1,file_handle );
	fwrite( "\t\t",strlen( "\t\t" ),1,file_handle );
	//进入当前为止系统运行的时间
	//cat /proc/uptime
	//输出: 5113396.94 575949.85
	//第一数字即是系统已运行的时间5113396.94 秒，运用系统工具date即可算出系统启动时间
	cmd_cat( "/proc/uptime",uptime_buffer,sizeof(uptime_buffer) );
	tmp_value=atoi( uptime_buffer );
	//存在整数的天数
	if( tmp_value >= 24*60*60 )
	{
		snprintf(uptime_buffer
				,sizeof( uptime_buffer )
				,"%d day "
				,tmp_value/( 24*60*60 )
				);
		fwrite( uptime_buffer,strlen( uptime_buffer ),1,file_handle );
		tmp_value=tmp_value-tmp_value/( 24*60*60 )*( 24*60*60 );
	}

	if( tmp_value >= 60*60 )
	{
		snprintf(uptime_buffer
				,sizeof( uptime_buffer )
				,"%d hour "
				,tmp_value/( 60*60 )
				);
		fwrite( uptime_buffer,strlen( uptime_buffer ),1,file_handle );
		tmp_value=tmp_value-tmp_value/( 60*60 )*( 60*60 );
	}

	if( tmp_value >= 60 )
	{
		snprintf(uptime_buffer
				,sizeof( uptime_buffer )
				,"%d min "
				,tmp_value/60
				);
		fwrite( uptime_buffer,strlen( uptime_buffer ),1,file_handle );
		tmp_value=tmp_value-tmp_value/60*60;
	}

	snprintf(uptime_buffer
			,sizeof( uptime_buffer )
			,"%d sec"
			,tmp_value
			);
	fwrite( uptime_buffer,strlen( uptime_buffer ),1,file_handle );
	fwrite( "\t\t",strlen( "\t\t" ),1,file_handle );
	//写入事件
	fwrite( event_description,strlen( event_description ),1,file_handle );
	fwrite( "\n",strlen( "\n" ),1,file_handle );

	//关掉文件句柄
	fclose( file_handle );
}

static int data_forward_enabled=FALSE;

void util_disable_data_forward( void )
{
	system( "CLOSE_INTERNET_TEMPORARILY=1 /etc/rc.d/rc.disable-wan-communication" );
	data_forward_enabled=FALSE;
}

//关掉IPv4/IPv6数据转发功能
void util_disable_data_forward_and_output( void )
{
	util_disable_data_forward();

	system("iptables -I OUTPUT -o usb0 -j DROP");
	system("ip6tables -I OUTPUT -o usb0 -j DROP");
	#ifdef WIN32
	Sleep(10000);
	#else
	sleep(10);
	#endif
}

void util_enable_data_forward( void )
{
	if( !data_forward_enabled )
	{
		if( !cmd_file_exist( CLOSE_INTERNET ) )
		{
			system( "CLOSE_INTERNET_TEMPORARILY=1 /etc/rc.d/rc.enable-wan-communication" );
			data_forward_enabled=TRUE;
		}
	}
}

//开启IPv4/IPv6数据转发功能
void util_enable_data_forward_and_output( void )
{
	util_enable_data_forward();
	system("iptables -D OUTPUT -o usb0 -j DROP");
	system("ip6tables -D OUTPUT -o usb0 -j DROP");
}


void save_debug_file_content(const char* file_name,const char* file_content)
{
	//a      Open for appending (writing at end of file).  The file is created if it does not exist.  The stream is positioned at the end of the file.
	FILE* tmp_file_handle=NULL;
	int file_size=util_get_file_size_by_file_name( file_name );

	//10M字节
	if( file_size >= 10485760 )
	{
		return;
	}


	tmp_file_handle=fopen(file_name,"a+");
	//将内容保存到文件中
	fwrite(file_content,strlen(file_content),1,tmp_file_handle);
	fwrite("\n",strlen("\n"),1,tmp_file_handle);
	//关掉文件句柄
	fclose( tmp_file_handle );
}

#ifdef CONFIG_LOCK_CARD_AND_CELL

//由于测试发现util_calculate_pin_by_iccid这个函数的计算不太安全,因此需要使用一个更加随机的方法来计算随机PIN码
void util_calculate_pin_by_imei( char* original_imei,char* pin,int only_output_digit )
{
	int string_len=strlen( original_imei );
	unsigned int random_value;
	int index;
	int start_index;

	for( index=0;index < 8;index++ )
	{
		random_value=1;
		for( start_index=0;start_index < string_len;start_index++ )
		{
			if( random_value > 0xffffff )
			{
				random_value=~random_value;
				random_value=random_value&0xffffff;
			}
			random_value=random_value+(unsigned int)( *( original_imei+( start_index+index )%string_len ) )*( ( (index+1)*(start_index+1) )&0xff );
		}

		if( random_value > 0xffffff )
		{
			random_value=~random_value;
			random_value=random_value&0xffffff;
		}

		//只输出数字
		if( only_output_digit )
		{
			pin[index] = ( random_value%10 ) + '0';
		}
		else
		{
			random_value=random_value%52;
			if( random_value >= ( 10+26 ) )
			{
				pin[index] = ( random_value-( 10+26 ) ) + 'a';
			}
			else if( random_value >= ( 10 ) )
			{
				pin[index] = ( random_value-( 10 ) ) + 'A';
			}
			else
			{
				pin[index] = random_value + '0';
			}
		}
	}
}

//根据码,自动计算对应的PIN码
void util_calculate_pin_by_iccid( char* iccid,char* pin )
{
	int i;
	unsigned char* tmp_pin=( unsigned char* )pin;

	//将后三个字节(17,18,19)与前两个字节进行
	//或运算，增加生成的pin码的随机性
	tmp_pin[0]=( ( ( iccid[0]&0x0f )<<4 )|( iccid[1]&0x0f ) )|( ( ( iccid[16]&0x0f )<<4 )|( iccid[17]&0x0f ) );
	tmp_pin[1]=( ( ( iccid[2]&0x0f )<<4 )|( iccid[3]&0x0f ) )|( ( iccid[18]&0x0f )<<4 );

	//将后三个字节(17,18,19)与从第2个字节
	//开始的两个字节进行
	//或运算，增加生成的pin码的随机性
	tmp_pin[2]=( ( ( iccid[4]&0x0f )<<4 )|( iccid[5]&0x0f ) )|( ( ( iccid[16]&0x0f )<<4 )|( iccid[17]&0x0f ) );
	tmp_pin[3]=( ( ( iccid[6]&0x0f )<<4 )|( iccid[7]&0x0f ) )|( ( iccid[18]&0x0f )<<4 );


	//将后三个字节(17,18,19)与从第4个字节
	//开始的两个字节进行
	//或运算，增加生成的pin码的随机性
	tmp_pin[4]=( ( ( iccid[8]&0x0f )<<4 )|( iccid[9]&0x0f ) )|( ( ( iccid[16]&0x0f )<<4 )|( iccid[17]&0x0f ) );
	tmp_pin[5]=( ( ( iccid[10]&0x0f )<<4 )|( iccid[11]&0x0f ) )|( ( iccid[18]&0x0f )<<4 );

	//将后三个字节(17,18,19)与从第6个字节
	//开始的两个字节进行
	//或运算，增加生成的pin码的随机性
	tmp_pin[6]=( ( ( iccid[12]&0x0f )<<4 )|( iccid[13]&0x0f ) )|( ( ( iccid[16]&0x0f )<<4 )|( iccid[17]&0x0f ) );
	tmp_pin[7]=( ( ( iccid[14]&0x0f )<<4 )|( iccid[15]&0x0f ) )|( ( iccid[18]&0x0f )<<4 );

	//根据iccid数据产生pin码(8个字节)
	//应注意协议的格式0x00->0xff
	//注意有些卡的iccid和卡背面印刷的有所不同
	//这种情况应注意
	for(i = 0; i < 8; i++)
	{
		pin[i] = ( tmp_pin[i]%10 ) + '0';
	}
	pin[8]=0;
}

//是否锁固定PIN码
int util_is_lock_pin_fixed( InfoStruct* server_wifi_info )
{
	int lock_card_type=atoi( server_wifi_info->TZ_LOCK_CARD_TYPE );

	return ( lock_card_type == LOCK_SIM_TYPE_FIXED );
}

//是否锁随机PIN码
int util_is_lock_pin_random( InfoStruct* server_wifi_info )
{
	int lock_card_type=atoi( server_wifi_info->TZ_LOCK_CARD_TYPE );

	return ( lock_card_type == LOCK_SIM_TYPE_ICCID );
}

//是否应该进行锁卡
int util_should_lock_card( InfoStruct* server_wifi_info )
{
	int lock_card_type;
	//没有设置参数
	if( !strlen( server_wifi_info->TZ_LOCK_CARD )
		|| !strlen( server_wifi_info->TZ_LOCK_CARD_TYPE )
		|| !strlen( server_wifi_info->TZ_INITIAL_PIN_CODE )
		)
	{
		return FALSE;
	}

	//如果为锁固定PIN码,则需要设置锁卡之后的PIN码
	lock_card_type=atoi( server_wifi_info->TZ_LOCK_CARD_TYPE );
	if( ( lock_card_type < LOCK_SIM_TYPE_MIN ) || ( lock_card_type >= LOCK_SIM_TYPE_MAX ) )
	{
		return FALSE;
	}

	//锁固定PIN码的情况下,修改后的PIN码没有被设定
	if( util_is_lock_pin_fixed( server_wifi_info ) && !strlen( server_wifi_info->TZ_MODIFIED_PIN_CODE ) )
	{
		return FALSE;
	}

	return atoi( server_wifi_info->TZ_LOCK_CARD );
}

//锁卡开启情况下,获取修改后的PIN码
char* util_get_modified_pin_code( InfoStruct* server_wifi_info,char* iccid )
{
	//锁随机PIN码
	if( util_is_lock_pin_random( server_wifi_info ) )
	{
		//根据ICCID生成对应的PIN码
		util_calculate_pin_by_iccid( iccid,server_wifi_info->TZ_MODIFIED_PIN_CODE );
	}

	return server_wifi_info->TZ_MODIFIED_PIN_CODE;
}

//是否开启锁小区功能
int util_should_lock_cellid( InfoStruct* server_wifi_info )
{
	int lock_cell_function_number;
	int lock_cell_count;

	//没有设置参数
	if( !strlen( server_wifi_info->TZ_LOCK_CELL_ID )
		|| !strlen( server_wifi_info->TZ_LOCK_CELL_ID_COUNT )
		)
	{
		return FALSE;
	}

	//锁小区功能号
	lock_cell_function_number=atoi( server_wifi_info->TZ_LOCK_CELL_ID );
	//锁小区的数量
	lock_cell_count=atoi( server_wifi_info->TZ_LOCK_CELL_ID_COUNT );

	if( ( ( lock_cell_function_number&0xff )&&( lock_cell_count&0xff ) )
		||( ( lock_cell_function_number&0xff00 )&&( lock_cell_count&0xff00 ) )
		||( ( lock_cell_function_number&0xff0000 )&&( lock_cell_count&0xff0000 ) )
		)
	{
		return TRUE;
	}

	return FALSE;
}

//获取4G锁小区的数量
//是否锁小区,这个是掩码类型:|暂时不用|<锁4G小区>|<锁3G小区>|<锁2G小区>|
//锁小区的数量,这个也是掩码类型:|暂时不用|<锁4G小区数量>|<锁3G小区数量>|<锁2G小区数量>|
int util_get_lock_4g_cellid_count( InfoStruct* server_wifi_info )
{
	//锁小区功能号
	int lock_cell_function_number=atoi( server_wifi_info->TZ_LOCK_CELL_ID );
	//锁小区的数量
	int lock_cell_count=atoi( server_wifi_info->TZ_LOCK_CELL_ID_COUNT );

	if( ( lock_cell_function_number&0xff0000 )&&( lock_cell_count&0xff0000 ) )
	{
		return ( ( lock_cell_count&0xff0000 )>>16 );
	}
	return 0;
}

//获取3G锁小区的数量
//是否锁小区,这个是掩码类型:|暂时不用|<锁4G小区>|<锁3G小区>|<锁2G小区>|
//锁小区的数量,这个也是掩码类型:|暂时不用|<锁4G小区数量>|<锁3G小区数量>|<锁2G小区数量>|
int util_get_lock_3g_cellid_count( InfoStruct* server_wifi_info )
{
	//锁小区功能号
	int lock_cell_function_number=atoi( server_wifi_info->TZ_LOCK_CELL_ID );
	//锁小区的数量
	int lock_cell_count=atoi( server_wifi_info->TZ_LOCK_CELL_ID_COUNT );

	if( ( lock_cell_function_number&0xff00 )&&( lock_cell_count&0xff00 ) )
	{
		return ( ( lock_cell_count&0xff00 )>>8 );
	}
	return 0;
}

//获取2G锁小区的数量
//是否锁小区,这个是掩码类型:|暂时不用|<锁4G小区>|<锁3G小区>|<锁2G小区>|
//锁小区的数量,这个也是掩码类型:|暂时不用|<锁4G小区数量>|<锁3G小区数量>|<锁2G小区数量>|
int util_get_lock_2g_cellid_count( InfoStruct* server_wifi_info )
{
	//锁小区功能号
	int lock_cell_function_number=atoi( server_wifi_info->TZ_LOCK_CELL_ID );
	//锁小区的数量
	int lock_cell_count=atoi( server_wifi_info->TZ_LOCK_CELL_ID_COUNT );

	if( ( lock_cell_function_number&0xff )&&( lock_cell_count&0xff ) )
	{
		return ( lock_cell_count&0xff );
	}
	return 0;
}

//是否应该进行限速
int util_should_limit_speed( InfoStruct* server_wifi_info )
{
	int speed_limit_enabled;

	//没有设置参数
	if( !strlen( server_wifi_info->TZ_LIMIT_SPEED_ENABLED ) )
	{
		return FALSE;
	}

	//是否开启限速功能
	speed_limit_enabled=atoi( server_wifi_info->TZ_LIMIT_SPEED_ENABLED );

	return speed_limit_enabled;
}

//获取限速的数值
int util_should_limit_speed_value( InfoStruct* server_wifi_info )
{
	return atoi( server_wifi_info->TZ_LIMIT_SPEED_VALUE );
}

//获取上行限速的数值
int util_should_limit_uplink_speed_value( InfoStruct* server_wifi_info )
{
	return atoi( server_wifi_info->TZ_LIMIT_UPLINK_SPEED_VALUE );
}

//看门狗是否被开启了
int util_is_watchdog_enabled( InfoStruct* server_wifi_info )
{
	return strcmp( server_wifi_info->TZ_ENABLE_WATCHDOG,"no" );
}

//速度优先模式
int util_is_speed_preferred_mode( InfoStruct* server_wifi_info )
{
	return !strcmp( server_wifi_info->TZ_SPEED_PREFERRED,"yes" );
}

#endif

//将IP地址保存到文件中,在另外一个进程中将文件中保存下来
void util_read_ip_address( const char* ip_file,char* buffer,int buffer_size )
{
	buffer[0]=0;

	//存在默认网关信息
	if( cmd_file_exist( ip_file ) )
	{
		int index;

		cmd_cat( ip_file,buffer,buffer_size );
		index=strlen( buffer )-1;
		while( index >= 0 )
		{
			if( ( buffer[ index ] == '\r' ) || ( buffer[ index ] == '\n' ) )
			{
				buffer[ index ]=0;
			}
			else
			{
				break;
			}
			index-=1;
		}
	}
}

static char string_buffer[512];
static char write_buffer[512];
//同步模块信息
int util_sync_module_info( char* network_card_name )
{
	int sync_ok=TRUE;
	char* tmp_ptr;
	char* p1;
	char rx_bytes_counter[64];
	char tx_bytes_counter[64];
	char rx_packages_counter[64];
	char tx_packages_counter[64];

	cmd_cp( MODULE_INFO_FILE,MODULE_INFO_FILE2 );
	//将其余的信息写入指定的文件中
	/*
	uptime = 08:04:28 up 4 min
	loadaverage: 0.00, 0.05, 0.02
	wanRxBytes = 1217625121
	wanRxPackets = 121271
	wanTxBytes = 2121736726
	wanTxPackets = 211217
	PLMN = 50501
	LAC = 33872
	CellID = 16102
	RSCP = -53
	RSSI = -49
	BER = 0
	KCELL = 5,74,33,32f210,157e,8101,50,0,79,3,32f210,157e,80f7,37,64,5,32f210,157e,810b,37,55,32,3
	*/
	//写入uptime信息
	//uptime = 08:04:28 up 4 min
	//~ # uptime
	// 17:56:49 up 56 min,  load average: 0.12, 0.07, 0.01
	util_get_cmd_result( "uptime",string_buffer,( int )sizeof( string_buffer ) );
	tmp_ptr=strstr( string_buffer,"," );
	if( tmp_ptr != NULL )
	{
		*tmp_ptr=0;
		tmp_ptr++;
		while( ( *tmp_ptr == ' ' ) || ( *tmp_ptr == '\t' ) )
		{
			tmp_ptr++;
		}

		for(	p1=tmp_ptr+strlen( tmp_ptr )-1;
				( ( *p1 == '\r' ) || ( *p1 == '\n' ) || ( *p1 == ' ' ) || ( *p1 == '\t' ) )&&( p1 >= tmp_ptr );

			)
		{
			*p1=0;
			p1--;
		}

		snprintf(write_buffer
				,sizeof( write_buffer )
				,"uptime:%s\n%s"
				,string_buffer
				,tmp_ptr
				);
		cmd_echo_append( write_buffer,MODULE_INFO_FILE2 );
	}

	//读取WAN端口相关的信息
	/*
	wanRxBytes = 1217625121
	wanRxPackets = 121271
	wanTxBytes = 2121736726
	wanTxPackets = 211217
	*/
	if( !cmd_netdev_flowstat_ex(network_card_name,rx_bytes_counter,tx_bytes_counter,rx_packages_counter,tx_packages_counter) )
	{
		snprintf(write_buffer
				,sizeof( write_buffer )
				,"wanRxBytes:%s\nwanRxPackets:%s\nwanTxBytes:%s\nwanTxPackets:%s\n"
				,rx_bytes_counter
				,rx_packages_counter
				,tx_bytes_counter
				,tx_packages_counter
				);
		cmd_echo_append( write_buffer,MODULE_INFO_FILE2 );
	}
	else
	{
		sync_ok=FALSE;
	}

	//如果导出过配置文件,则自动计算其MD5,并保存到信息中
	if( cmd_file_exist( GENERATED_TMP_CONFIG_FILE ) )
	{
		//生成配置文件对应的md5文件
		cmd_rm( GENERATED_TMP_CONFIG_FILE_MD5_SUM );
		snprintf(write_buffer
				,sizeof( write_buffer )
				,"md5sum %s|cut -f1 -d\" \" > %s"
				,GENERATED_TMP_CONFIG_FILE
				,GENERATED_TMP_CONFIG_FILE_MD5_SUM
				);
		//生成配置文件对应的md5文件
		system( write_buffer );

		//如果成功生成对应的md5校验和文件,则保存到MODULE_INFO_FILE2中
		if( cmd_file_exist( GENERATED_TMP_CONFIG_FILE_MD5_SUM ) )
		{
			cmd_cat( GENERATED_TMP_CONFIG_FILE_MD5_SUM,string_buffer,sizeof( string_buffer ) );
			snprintf(write_buffer
								,sizeof( write_buffer )
								,"ConfigMD5:%s\n"
								,string_buffer
								);
			cmd_echo_append( write_buffer,MODULE_INFO_FILE2 );
		}
	}

	return sync_ok;
}

//去掉字符串末尾的空白
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

//0表示获取ARP信息成功,否则表示获取ARP信息失败
int util_get_arp_table_info( ARPTable* arp_table,char* network_card )
{
	const char* proc_net_arp="/proc/net/arp";
	char temp_buffer[256];
	int line_index=0;
	char* args[18];
	int args_count;
	FILE* file_handle;

	if( !cmd_file_exist( proc_net_arp ) )
	{
		return -1;
	}

	memset( arp_table,0,sizeof( ARPTable ) );

	//读取ARP表
	file_handle=fopen( proc_net_arp,"r" );

	memset( temp_buffer,0,sizeof(temp_buffer) );

	/*
	~ # cat  /proc/net/arp
	IP address       HW type     Flags       HW address            Mask     Device
	10.185.77.28     0x1         0x2         fe:fc:fc:00:11:22     *        usb0
	192.168.0.100    0x1         0x2         b8:ac:6f:bb:7e:3f     *        br0
	*/
	while( fgets( temp_buffer,sizeof(temp_buffer)-1,file_handle ) != NULL )
	{
		if( !line_index )
		{
			line_index+=1;
			continue;
		}

		//对获取的内容进行分割
		args_count=util_get_all_args( temp_buffer,' ','\t',args,18 );
		if( args_count >= 6 )
		{
			util_strip_blank_of_string_end( args[5] );
			if( ( network_card == NULL ) || !strlen( network_card ) || !strcmp( network_card,args[5] ) )
			{
				int arp_flags=0;

				sscanf( args[ 2 ]+2,"%x",&arp_flags );
				/*
				flags的对应值：
				C  完整记录，在/proc/net/arp显示的值为2
				M  永久性记录，在/proc/net/arp显示的值为4
				P  Proxy记录，在/proc/net/arp显示的值为8
				incomplete  被arp -d命令删除的记录，在/proc/net/arp显示的值为0
				*/
				if( arp_flags )
				{
					STRCPY_S( arp_table->table[ arp_table->count ].ip,args[ 0 ] );
					STRCPY_S( arp_table->table[ arp_table->count ].mac,args[ 3 ] );
					arp_table->count++;
				}

				if( arp_table->count >= sizeof( arp_table->table )/sizeof( arp_table->table[0] ) )
				{
					break;
				}
			}
		}

		line_index+=1;
	}
	//关掉文件句柄
	fclose( file_handle );

	return 0;
}

//0表示获取通过PPPOE连接的客户端信息成功,否则表示获取通过PPPOE连接的客户端信息失败
int util_get_pppoe_table_info( ARPTable* arp_table,char* network_card )
{
	const char* proc_net_pppoe="/proc/net/pppoe";
	char temp_buffer[256];
	int line_index=0;
	char* args[18];
	int args_count;
	FILE* file_handle;

	if( !cmd_file_exist( proc_net_pppoe ) )
	{
		return -1;
	}

	memset( arp_table,0,sizeof( ARPTable ) );

	//读取ARP表
	file_handle=fopen( proc_net_pppoe,"r" );

	memset( temp_buffer,0,sizeof(temp_buffer) );

	/*
	~ # cat /proc/net/pppoe
	Id       Address              Device
	00000007 b8:ac:6f:bb:7e:3f      br0
	*/
	while( fgets( temp_buffer,sizeof(temp_buffer)-1,file_handle ) != NULL )
	{
		if( !line_index )
		{
			line_index+=1;
			continue;
		}

		//对获取的内容进行分割
		args_count=util_get_all_args( temp_buffer,' ','\t',args,18 );
		if( args_count >= 3 )
		{
			util_strip_blank_of_string_end( args[2] );
			if( ( network_card == NULL ) || !strlen( network_card ) || !strcmp( network_card,args[2] ) )
			{
				//数量已经超过
				if( arp_table->count >= sizeof( arp_table->table )/sizeof( arp_table->table[0] ) )
				{
					break;
				}

				STRCPY_S( arp_table->table[ arp_table->count ].ip,args[ 0 ] );
				STRCPY_S( arp_table->table[ arp_table->count ].mac,args[ 1 ] );

				//获取当前拨号的用户名
				if( args_count >= 4 )
				{
					char user_buffer[ 100 ];
					int index=0;
					int dest_index=0;
					char* tmp_ptr;
					util_strip_blank_of_string_end( args[ 3 ] );

					memset( user_buffer,0,sizeof(user_buffer) );
					tmp_ptr=args[ 3 ];

					debug( "\r\noriginal pppoe user name args[ 3 ]:%s\r\n",tmp_ptr );
					//跳过读取的特殊字符,避免json解析失败,有部分CPE发上来的状态包里面的PPPOE帐号里面有： "、）、\、等字符。
					for( index=0;index < strlen( args[ 3 ] );index+=1 )
					{
						//跳过"字符
						if( *tmp_ptr == '"' )
						{
							tmp_ptr++;
							continue;
						}

						//跳过\字符
						if( *tmp_ptr == '\\' )
						{
							tmp_ptr++;
							continue;
						}

						if( dest_index >= sizeof( user_buffer )-1 )
						{
							break;
						}

						user_buffer[ dest_index ]=*tmp_ptr;
						dest_index+=1;
						tmp_ptr++;
					}

					debug( "\r\nreport pppoe user name user_buffer:%s\r\n",user_buffer );
					STRCPY_S( arp_table->table[ arp_table->count ].user,user_buffer );
				}

				arp_table->count++;

				if( arp_table->count >= sizeof( arp_table->table )/sizeof( arp_table->table[0] ) )
				{
					break;
				}
			}
		}

		line_index+=1;
	}
	//关掉文件句柄
	fclose( file_handle );

	return 0;
}

void get_used_lan(void)
{
	char lan1[8];
	char lan2[8];
	char lan3[8];
	char lan4[8];
	int lan_is_exist=FALSE;
	cmd_cat("/proc/phys26/lan5",lan1,sizeof(lan1));
	if( atoi( lan1 ) == 1 )
	{
		cmd_echo("801","/proc/tozed_gpio/i2c");
		lan_is_exist=TRUE;
	}
	if( atoi( lan1 ) == 0 )
	{
		cmd_echo("811","/proc/tozed_gpio/i2c");
		lan_is_exist=TRUE;
	}
	cmd_cat("/proc/phys26/lan2",lan2,sizeof(lan2));
	if( atoi( lan2 ) == 1 )
	{
		cmd_echo("751","/proc/tozed_gpio/i2c");
		lan_is_exist=TRUE;
	}
	if( atoi( lan2 ) == 0 )
	{
		cmd_echo("761","/proc/tozed_gpio/i2c");
		lan_is_exist=TRUE;
	}
	cmd_cat("/proc/phys26/lan3",lan3,sizeof(lan3));
	if( atoi( lan3 ) == 1 )
	{
		cmd_echo("701","/proc/tozed_gpio/i2c");
		lan_is_exist=TRUE;
	}
	if( atoi( lan3 ) == 0 )
	{
		cmd_echo("711","/proc/tozed_gpio/i2c");
		lan_is_exist=TRUE;
	}
	cmd_cat("/proc/phys26/lan4",lan4,sizeof(lan4));
	if( atoi( lan4 ) == 1 )
	{
		cmd_echo("601","/proc/tozed_gpio/i2c");
		lan_is_exist=TRUE;
	}
	if( atoi( lan4 ) == 0 )
	{
		cmd_echo("611","/proc/tozed_gpio/i2c");
		lan_is_exist=TRUE;
	}
	if( lan_is_exist == FALSE )
	{
		cmd_echo("821","/proc/tozed_gpio/i2c");
	}
}

//P11+的以太网端口映射表,ethernet_port_map_table_p11plus[ x-'0' ]
const char* const ethernet_port_map_table_p11plus[]={ "0","0","1","4","3","2","0","0", };
//其他设备的以太网端口映射表,ethernet_port_map_table[ x-'0' ]
const char* const ethernet_port_map_table[]={ "0","0","4","1","2","3","0","0", };

//获取指定mac地址和端口之间的对应关系,使用已经通过util_get_pppoe_table_info
//获取的信息来进一步获取指定MAC对应的端口信息
int util_get_port_number_for_one_mac( ARPTable* system_arp_table )
{
	const char* proc_phys26_addresstable="/proc/phys26/addresstable";
	char temp_buffer[256];
	char* args[18];
	int args_count;
	FILE* file_handle;

	if( !cmd_file_exist( proc_phys26_addresstable ) )
	{
		return -1;
	}

	//读取MAC地址对应的端口号的表
	file_handle=fopen( proc_phys26_addresstable,"r" );

	memset( temp_buffer,0,sizeof(temp_buffer) );

	/*
	~ # cat /proc/net/pppoe
	Id       Address              Device
	00000007 b8:ac:6f:bb:7e:3f      br0
	*/
	while( fgets( temp_buffer,sizeof(temp_buffer)-1,file_handle ) != NULL )
	{
		//对获取的内容进行分割
		/*
		00:03:7f:ff:ff:fe 1
		00:1d:7d:b0:41:62 5
		b8:ac:6f:bb:7e:1e 5
		*/
		args_count=util_get_all_args( temp_buffer,' ','\t',args,18 );
		if( args_count >= 2 )
		{
			int index=0;

			util_strip_blank_of_string_end( args[1] );
			//查找指定的MAC地址对应的端口号
			while( index < system_arp_table->count )
			{
				if( !strcmp( system_arp_table->table[ index ].mac,args[0] ) )
				{
					int port=atoi( args[1] );

					if( port < sizeof( ethernet_port_map_table_p11plus ) )
					{
						//设备为P11+
						if( is_this_device_p11plus() )
						{
							STRCPY_S( system_arp_table->table[ index ].port,ethernet_port_map_table_p11plus[ port ] );
						}
						else
						{
							STRCPY_S( system_arp_table->table[ index ].port,ethernet_port_map_table[ port ] );
						}
					}

					//查找到指定的MAC地址
					break;
				}

				index+=1;
			}
		}
	}
	//关掉文件句柄
	fclose( file_handle );

	return 0;


}

//P11+的以太网端口映射表,ethernet_port_map_table_p11plus[ x-'0' ]
const int ethernet_port_map_table_p11plus_from_label[]={ 0,2,5,4,3,0,0,0, };
//其他设备的以太网端口映射表,ethernet_port_map_table[ x-'0' ]
const int ethernet_port_map_table_from_label[]={ 0,3,4,5,2,0,0,0, };

//使用CPE标贴纸上的端口号,获取指定端口对应的实际端口
int util_get_cpe_phy_port_from_label_number( int label_number )
{
	//设备为P11+
	if( is_this_device_p11plus() )
	{
		return ethernet_port_map_table_p11plus_from_label[ label_number ];
	}
	else
	{
		return ethernet_port_map_table_from_label[ label_number ];
	}
}

//获取当前拨号的用户,使用逗号进行分割
static char all_pppoe_users_list[ 512 ];
char* util_get_all_pppoe_users( void )
{
	ARPTable system_arp_table;
	char tmp_user_info[ 100 ];
	int index=0;

	memset( &system_arp_table,0,sizeof( system_arp_table ) );
	memset( all_pppoe_users_list,0,sizeof( all_pppoe_users_list ) );
	util_get_pppoe_table_info( &system_arp_table,"br0" );
	//获取指定MAC地址对应的端口号
	util_get_port_number_for_one_mac( &system_arp_table );
	while( index < system_arp_table.count )
	{
		if( strlen( all_pppoe_users_list ) )
		{
			STRCAT_S( all_pppoe_users_list,"," );
		}

		memset( tmp_user_info,0,sizeof( tmp_user_info ) );
		snprintf(
				tmp_user_info,
				sizeof( tmp_user_info )-1,
				"%s/%s/%s",
				system_arp_table.table[ index ].user,
				system_arp_table.table[ index ].mac,
				system_arp_table.table[ index ].port
				);

		STRCAT_S( all_pppoe_users_list,tmp_user_info );
		index+=1;
	}

	return all_pppoe_users_list;
}

//获取所有连接的wifi客户端设备
/*
~ # wlanconfig ath0 list sta
ADDR               AID CHAN RATE RSSI IDLE  TXSEQ  RXSEQ CAPS ACAPS ERP    STATE HTCAPS
00:1e:65:50:b7:c0    1   11  19M   39    0     10   1776 EPSs         0        b PGM    RSN WME
 */
int util_get_all_wlan_clients( ARPTable* arp_table )
{
	//读取所有的客户端信息
	const char* wlan_client_info_file=ALL_WLAN_CLIENT_DEVICES_INFO;
	char temp_buffer[256];
	int line_index=0;
	char* args[18];
	int args_count;
	FILE* file_handle;

	//首先删除要生成的文件
	cmd_rm( ALL_WLAN_CLIENT_DEVICES_INFO );

	//将所有的wlan客户端信息写入到文件中
	system( "wlanconfig ath0 list sta > "ALL_WLAN_CLIENT_DEVICES_INFO );

	if( !cmd_file_exist( wlan_client_info_file ) )
	{
		return -1;
	}

	memset( arp_table,0,sizeof( ARPTable ) );

	//读取ARP表
	file_handle=fopen( wlan_client_info_file,"r" );

	memset( temp_buffer,0,sizeof(temp_buffer) );

	/*
	~ # wlanconfig ath0 list sta
	ADDR               AID CHAN RATE RSSI IDLE  TXSEQ  RXSEQ CAPS ACAPS ERP    STATE HTCAPS
	00:1e:65:50:b7:c0    1   11  19M   39    0     10   1776 EPSs         0        b PGM    RSN WME
	*/
	while( fgets( temp_buffer,sizeof(temp_buffer)-1,file_handle ) != NULL )
	{
		if( !line_index )
		{
			line_index+=1;
			continue;
		}

		//对获取的内容进行分割
		args_count=util_get_all_args( temp_buffer,' ','\t',args,18 );
		if( args_count >= 3 )
		{
			util_strip_blank_of_string_end( args[0] );
			//数量已经超过
			if( arp_table->count >= sizeof( arp_table->table )/sizeof( arp_table->table[0] ) )
			{
				break;
			}

			STRCPY_S( arp_table->table[ arp_table->count ].mac,args[ 0 ] );
			arp_table->count++;

			if( arp_table->count >= sizeof( arp_table->table )/sizeof( arp_table->table[0] ) )
			{
				break;
			}
		}

		line_index+=1;
	}
	//关掉文件句柄
	fclose( file_handle );

	return 0;
}

//去除结尾的空白字符
void util_strip_traling_spaces( char* one_string )
{
	char* tmp=one_string;
	int length=strlen(tmp);
	while(
			length
			&&(
				( tmp[ length-1 ] == '\r' )
				|| ( tmp[ length-1 ] == '\n' )
				|| ( tmp[ length-1 ] == '\t' )
				|| ( tmp[ length-1 ] == ' ' )
			)
		)
	{
		tmp[ length-1 ]=0;
		length--;
	}
}

//装载模块相关的信息
int util_load_module_info( InfoStruct* module_info )
{
	//all_http_params
	const char* version_file=MODULE_INFO_FILE;
	FILE* file_handle;
	char buffer[256];
	char*p1;
	/*
	~ # cat /tmp/.moduleinfo
	phy:PHYFA.0.60_LTEMV3.20.02.P02.R10_20131010
	hlp:3.20.02.P02.R10,2013-10-12
	imei:358600000000005
	modversion:LT10_HYNIX-4+2_V1.01
	modbuilddate:Dec 16 2013 16:49:57
	*/
	file_handle=fopen(version_file,"r");
	//读取文件每一行,并进行分析
	if( file_handle == NULL )
	{
		return FALSE;
	}

	//读取文件的每一行,并进行处理
	while( fgets( buffer,sizeof(buffer)-1,file_handle ) != NULL )
	{
		p1=strstr(buffer,":");
		if( p1 != NULL )
		{
			//模块物理层版本
			if( !strncmp(buffer,"phy",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				strcpy(module_info->module_phy_version,p1+1);
			}
			//模块高层协议版本
			else if( !strncmp(buffer,"hlp",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				strcpy(module_info->module_hlp_version,p1+1);
			}
			//获取模块IMEI
			else if( !strncmp(buffer,"imei",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				strcpy(module_info->imei,p1+1);
			}
			//获取模块IMSI
			else if( !strncmp(buffer,"imsi",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				strcpy(module_info->imsi,p1+1);
			}
			//模块版本
			else if( !strncmp(buffer,"modversion",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				strcpy(module_info->module_version,p1+1);
			}
			//模块编译的时间日期
			else if( !strncmp(buffer,"modbuilddate",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				strcpy(module_info->module_build_date,p1+1);
			}
			//RSSI:-59,接收信号强度
			else if( !strncmp(buffer,"RSSI",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				strcpy(module_info->RSSI,p1+1);
			}
			//LAC:65534,大区ID,10进制
			else if( !strncmp(buffer,"LAC",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				strcpy(module_info->LAC,p1+1);
			}
			//CellID,小区ID,10进制
			else if( !strncmp(buffer,"CellID",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				strcpy(module_info->CellID,p1+1);
			}
			//iccid,卡的ICCID
			else if( !strncmp(buffer,"iccid",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				strcpy(module_info->iccid,p1+1);
			}
			//当前所注册的网络的MCC+MNC
			else if( !strncmp(buffer,"PLMN",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				strcpy(module_info->PLMN,p1+1);
			}
			//当前所在的网络模式
			else if( !strncmp(buffer,"AcT",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				strcpy(module_info->AcT,p1+1);
			}
			//锁频相关的信息
			else if( !strncmp(buffer,"band_pref",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				strcpy(module_info->band_pref,p1+1);
			}
			//当前所使用的模块类型
			else if( !strncmp(buffer,"Module3G",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				strcpy(module_info->module_type,p1+1);
			}
		}
	}

	//关掉文件句柄
	fclose( file_handle );

	return TRUE;
}

//跳过所有的空白字符
char* util_skip_all_spaces( char* one_string )
{
	int index=0;
	while(
			( one_string[ index ] == '\r' )
			|| ( one_string[ index ] == '\n' )
			|| ( one_string[ index ] == '\t' )
			|| ( one_string[ index ] == ' ' )
		)
	{
		index+=1;
	}

	return (one_string+index);
}

//+BANDINFO:GSM900,GSM1800;TDSCDMA BAND34,BAND39;TDD BAND38,BAND39,BAND40
//+BANDINFO:GSM900,GSM1800;TDSCDMA BAND34,BAND39;TDD BAND38,BAND39,BAND40;FDD BAND3
//读取模块支持的LTE频段信息
void util_parse_bands_info( char* bands_info,int write_to_file,char* file_name,char* module_info )
{
	unsigned int bands_available_high_32bit=0;
	unsigned int bands_available_low_32bit=0;
	char* tmp_ptr=bands_info;
	char* cmd_result_end=strstr( bands_info,"\r" );

	if( cmd_result_end == NULL )
	{
		cmd_result_end=bands_info+strlen( bands_info );
	}

	while( tmp_ptr != NULL )
	{
		if( !strncmp( tmp_ptr,"TDD",strlen( "TDD" ) )
			|| !strncmp( tmp_ptr,"FDD",strlen( "FDD" ) )
				)
		{
			char* next_band=tmp_ptr+strlen( "TDD" )+1;
			char* bands_config_end=strstr( tmp_ptr,";" );
			if( bands_config_end == NULL )
			{
				bands_config_end=cmd_result_end;
			}
			else if( bands_config_end >= cmd_result_end )
			{
				bands_config_end=cmd_result_end;
			}

			while( next_band <= bands_config_end )
			{
				if( !strncmp( next_band,"BAND",strlen( "BAND" ) ) )
				{
					int band_value=atoi( next_band+strlen( "BAND" ) );
					if( band_value > 32 )
					{
						bands_available_high_32bit|=( 1<<( band_value-32-1 ) );
					}
					else
					{
						bands_available_low_32bit|=( 1<<( band_value-1 ) );
					}
				}

				next_band=strstr( next_band+1,"BAND" );
				if( next_band == NULL )
				{
					break;
				}
			}
		}

		tmp_ptr=strstr( tmp_ptr+1,";" );
		if( tmp_ptr != NULL )
		{
			tmp_ptr++;
		}


		if( tmp_ptr >= cmd_result_end )
		{
			break;
		}
	}

	//频段被设置
	if( bands_available_high_32bit || bands_available_low_32bit )
	{
		//将支持的频段写入buffer中
		char buffer[ 128 ];
		sprintf(
				buffer
				,"%08X%08X"
				,bands_available_high_32bit
				,bands_available_low_32bit
				);

		if( write_to_file && ( file_name != NULL ) && strlen( file_name ) )
		{
			cmd_echo_append_without_new_line( "+SupportedBands: ",file_name );
			cmd_echo_append( buffer,file_name );
		}

		if( module_info != NULL )
		{
			strcpy( module_info,buffer );
		}
	}
}

//读取发送给模块的AT命令查询到的状态
int util_read_module_status( char* result_file,InfoStruct* module_info )
{
	FILE* file_handle;
	char buffer[1024];
	char*p1;
	/*
	AT+TZEARFCN?

	+TZEARFCN: 37900

	OK

	AT+TZBAND?

	+TZBAND: 38

	OK

	AT+TZBANDWIDTH?

	+TZBANDWIDTH: 100

	OK

	AT+TZTRANSMODE?

	+TZTRANSMODE: 3

	OK

	AT+TZRSRP?

	+TZRSRP: -93

	OK

	AT+TZRSRQ?

	+TZRSRQ: -70

	OK

	AT+TZSINR?

	+TZSINR: 16

	OK

	AT+TZTA?

	+TZTA: 656

	OK

	AT+TZTXPOWER?

	+TZTXPOWER: 7

	OK

	AT+CEREG?

	+CEREG: 2,1,"27bd","0ac17d03",7

	OK

	AT+TZGLBCELLID?

	+TZGLBCELLID: 180452611

	OK

	AT+TZPHYCELLID?

	+TZPHYCELLID: 356

	OK
	*/
	file_handle=fopen(result_file,"r");
	//读取文件每一行,并进行分析
	if( file_handle == NULL )
	{
		return FALSE;
	}

	//读取文件的每一行,并进行处理
	while( fgets( buffer,sizeof(buffer)-1,file_handle ) != NULL )
	{
		p1=strstr(buffer,":");
		if( p1 != NULL )
		{
			//模块物理层版本
			if( !strncmp(buffer,"+TZEARFCN",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				strcpy(module_info->TZEARFCN,p1);
			}
			//模块高层协议版本
			else if( !strncmp(buffer,"+TZBAND",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				strcpy(module_info->TZBAND,p1);
			}
			//获取模块IMEI
			else if( !strncmp(buffer,"+TZBANDWIDTH",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				strcpy(module_info->TZBANDWIDTH,p1);
			}
			//模块版本
			else if( !strncmp(buffer,"+TZTRANSMODE",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				strcpy(module_info->TZTRANSMODE,p1);
			}
			//模块编译的时间日期
			else if( !strncmp(buffer,"+TZRSRP",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				strcpy(module_info->TZRSRP,p1);
			}
			//RSSI:-59,接收信号强度
			else if( !strncmp(buffer,"+TZRSRQ",p1-buffer) )
			{
				int value;
				char buffer[ 32 ];
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );

				value=atoi( p1 );
				value=value/10;
				sprintf( buffer,"%d",value );
				p1=buffer;
				strcpy(module_info->TZRSRQ,p1);
			}
			//LAC:65534,大区ID,10进制
			else if( !strncmp(buffer,"+TZSINR",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				strcpy(module_info->TZSINR,p1);
			}
			//CellID,小区ID,10进制
			else if( !strncmp(buffer,"+TZTA",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				strcpy(module_info->TZTA,p1);
			}
			//iccid,卡的ICCID
			else if( !strncmp(buffer,"+TZTXPOWER",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				strcpy(module_info->TZTXPOWER,p1);
			}
			//iccid,卡的ICCID
			else if( !strncmp(buffer,"+TZGLBCELLID",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				strcpy(module_info->TZGLBCELLID,p1);
			}
			else if( !strncmp(buffer,"+TZPHYCELLID",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				strcpy(module_info->TZPHYCELLID,p1);
			}
			else if( !strncmp(buffer,"+ZEMCI",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
//				util_parse_emci_info( p1,FALSE,NULL,module_info );
			}
			else if( !strncmp(buffer,"+ZEMCIQ",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
//				util_parse_emci_info( p1,FALSE,NULL,module_info );
			}
			else if( !strncmp(buffer,"+ZSINR",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
//				util_parse_sinr_info( p1,FALSE,NULL,module_info );
			}
			else if( !strncmp(buffer,"+ZEMSCIQ",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
//				util_parse_emsci_info( p1,FALSE,NULL,module_info );
			}
/*			else if( !strncmp(buffer,"+BMTCELLINFO",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				util_parse_bmtcell_info( p1,FALSE,NULL,module_info );
			}
//			else if( !strncmp(buffer,"+BMCELLINFO",p1-buffer) )
//			{
				//去除最后的空白
//				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白

//				p1=util_skip_all_spaces( p1+1 );
//				util_parse_bmcell_info( p1,FALSE,NULL,module_info );
//			}

				p1=util_skip_all_spaces( p1+1 );
				util_parse_bmcell_info( p1,FALSE,NULL,module_info );
			}
*/
			else if( !strncmp(buffer,"CELL_ID",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				strcpy(module_info->TZGLBCELLID,p1);
			}
			else if( !strncmp(buffer,"PCI",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				strcpy(module_info->TZPHYCELLID,p1);
			}
			else if( !strncmp(buffer,"LAC_ID",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				strcpy(module_info->LAC,p1);
			}
			else if( !strncmp(buffer,"RSRP",p1-buffer) )
			{
				int rsrp;
				char buf[32];
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				rsrp=atoi(p1)+3;
				sprintf(buf,"%d",rsrp);
				p1=buf;
				strcpy(module_info->TZRSRP,p1);
				cmd_echo( module_info->TZRSRP,"/tmp/.signal_info1" );
			}
			else if( !strncmp(buffer,"RSRQ",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				strcpy(module_info->TZRSRQ,p1);
			}
			else if( !strncmp(buffer,"SINR",p1-buffer) )
			{
				//去除最后的空白
				int sinr;
				char tmp_buf[32];
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				sinr=atoi(p1)+1;                            //sinr value add 1db in order to test
				sprintf(tmp_buf,"%d",sinr);
				p1=tmp_buf;
				strcpy(module_info->TZSINR,p1);
				cmd_echo( module_info->TZSINR,"/tmp/.signal_info2" );
			}
			else if( !strncmp(buffer,"ACTIVE BAND",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				if( atoi(p1) < 64 )
				{
					strcpy(module_info->TZBAND,p1);
				}
			}
			else if( !strncmp(buffer,"EARFCN",p1-buffer) )
			{
				//去除最后的空白
				util_strip_traling_spaces(p1+1);
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				strcpy(module_info->TZEARFCN,p1);
			}
		}
	}

	//关掉文件句柄
	fclose( file_handle );

	return TRUE;
}
//对BMTCELLINFO进行解析
void util_parse_bmtcell_info( char* emci,int write_to_file,char* file_name,InfoStruct* module_info )
{
	char tmp_buffer[1024];
	char* args[24];
	int rsrp;
	char buf[32];
	int args_count;
	srand( (unsigned) time(NULL) );
	STRCPY_S( tmp_buffer,emci );
	args_count=util_get_all_args( tmp_buffer,':',',',args,16 );

	if( strcmp(args[4],"LAC_ID") == 0 )
	{

		strcpy( module_info->TZGLBCELLID,args[1] );

		strcpy( module_info->TZPHYCELLID,args[3] );

		strcpy( module_info->LAC,args[5] );

		strcpy( module_info->RSSI,args[7] );

		rsrp=atoi(args[9])+3;

		sprintf(buf,"%d",rsrp);

		strcpy( module_info->TZRSRP,buf );

		strcpy( module_info->TZRSRQ,args[11] );

		strcpy(module_info->TZSINR,args[13] );

		strcpy(module_info->TZBAND,args[15] );

		cmd_echo( module_info->TZRSRP,"/tmp/.signal_info1" );
		cmd_echo( module_info->TZSINR,"/tmp/.signal_info2" );

	}

	else
	{

		strcpy( module_info->TZGLBCELLID,"0" );

		strcpy( module_info->TZPHYCELLID,"0" );

		strcpy( module_info->LAC,"0" );

		strcpy( module_info->RSSI,"-" );

		strcpy( module_info->TZRSRP,"-" );

		strcpy( module_info->TZRSRQ,"-" );

		strcpy(module_info->TZSINR,"0" );

		strcpy(module_info->TZBAND,"-" );
	}



}
//对BMTCELLINFO进行解析
void util_parse_bmcell_info( char* emci,int write_to_file,char* file_name,InfoStruct* module_info )
{
	char tmp_buffer[256];
	char* args[16];
	STRCPY_S( tmp_buffer,emci );
	util_get_all_args( tmp_buffer,':',',',args,16 );
	strcpy( module_info->TZPHYCELLID,args[1] );
}

/*
** Translation Table as described in RFC1113
*/
static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
** encodeblock
**
** encode 3 8-bit binary bytes as 4 '6-bit' characters
*/
int base64_encode( unsigned char *in, unsigned char *out, int src_len )
{
	int source_index=0;
	int dest_index=0;

	while( source_index < src_len )
	{
	    out[dest_index+0] = (unsigned char) cb64[ (int)(in[source_index+0] >> 2) ];
	    out[dest_index+1] = (unsigned char) cb64[ (int)(((in[source_index+0] & 0x03) << 4) | ((in[source_index+1] & 0xf0) >> 4)) ];
	    out[dest_index+2] = (unsigned char) (src_len-source_index > 1 ? cb64[ (int)(((in[source_index+1] & 0x0f) << 2) | ((in[source_index+2] & 0xc0) >> 6)) ] : '=');
	    out[dest_index+3] = (unsigned char) (src_len-source_index > 2 ? cb64[ (int)(in[source_index+2] & 0x3f) ] : '=');

		source_index+=3;
		dest_index+=4;
	}

	out[ dest_index ]=0;

	return dest_index;
}

/*
** decodeblock
**
** decode 4 '6-bit' characters into 3 8-bit binary bytes
*/
int base64_decode( unsigned char *in, unsigned char *out,int src_len )
{
	int source_index=0;
	int dest_index=0;
	int index=src_len;

	index=0;
	while( index < src_len )
	{
		if( ( in[ index ] >= 'A' ) && ( in[ index ] <= 'Z' ) )
		{
			in[ index ]=in[ index ]-'A';
		}
		else if( ( in[ index ] >= 'a' ) && ( in[ index ] <= 'z' ) )
		{
			in[ index ]=in[ index ]-'a'+26;
		}
		else if( ( in[ index ] >= '0' ) && ( in[ index ] <= '9' ) )
		{
			in[ index ]=in[ index ]-'0'+52;
		}
		else if( in[ index ] == '+' )
		{
			in[ index ]=62;
		}
		else if( in[ index ] == '/' )
		{
			in[ index ]=63;
		}
		else if( in[ index ] == '=' )
		{
			in[ index ]=0;
		}


		index+=1;
	}

	while( source_index < src_len )
	{
	    out[dest_index+ 0 ] = (unsigned char ) (in[source_index+0] << 2 | in[source_index+1] >> 4);
	    out[dest_index+ 1 ] = (unsigned char ) (in[source_index+1] << 4 | in[source_index+2] >> 2);
	    out[dest_index+ 2 ] = (unsigned char ) (((in[source_index+2] << 6) & 0xc0) | in[source_index+3]);

		source_index+=4;
		dest_index+=3;
	}

	out[ dest_index ]=0;

	return dest_index;
}

//检查是否所有的字符均为base64编码生成的字符
int util_is_base64_string( char *in,int src_len )
{
	int index=0;

	while( index < src_len )
	{
		if( ( in[ index ] >= 'A' ) && ( in[ index ] <= 'Z' ) )
		{
			;
		}
		else if( ( in[ index ] >= 'a' ) && ( in[ index ] <= 'z' ) )
		{
			;
		}
		else if( ( in[ index ] >= '0' ) && ( in[ index ] <= '9' ) )
		{
			;
		}
		else if( in[ index ] == '+' )
		{
			;
		}
		else if( in[ index ] == '/' )
		{
			;
		}
		else if( in[ index ] == '=' )
		{
			;
		}
		else
		{
			return FALSE;
		}


		index+=1;
	}

	return TRUE;
}

//获取根据AT+COPS?/AT+CGREG?/AT+CEREG?/AT+CREG?返回的网络模式信息返回对应的网络信息
//返回映射后的网络模式,并将网络模式实际代表的2G/3G/4G信息返回
int util_get_remapped_network_act( int act,char* network_mode )
{
	int remapped_network_act=act;

	if( network_mode != NULL )
	{
		char* dest_network_mode=util_convert_act_to_network_mode_string( act );
		*network_mode=0;
		if( dest_network_mode != NULL )
		{
			strcpy( network_mode,dest_network_mode );
		}
	}

	/*
	<AcT>:
	0: GSM
	1: GSM Compact
	2: UTRAN
	3: GSM w/EGPRS
	4: UTRAN w/HSDPA
	5: UTRAN w/HSUPA
	6: UTRAN w/HSDPA and HSUPA
	7: E-UTRAN
	8: UTRAN HSPA+
	*/
	if( act == 7 )
	{
		//4G网络
		;
	}
	else if( ( act >= 4 ) || ( act == 2 ) )
	{
		//3G网络
		remapped_network_act=2;
	}
	else
	{
		//2G网络
		remapped_network_act=0;
	}

	return remapped_network_act;
}

typedef struct
{
	int value;
	char* string_for_value;
}ValueStrPair;

static const ValueStrPair network_mode_act_pairs[]=
{
	{ 0,"2G" },
	{ 1,"2G" },
	{ 2,"3G" },
	{ 3,"2G/EDGE" },
	{ 4,"3G/HSDPA" },
	{ 5,"3G/HSUPA" },
	{ 6,"3G/HSDPA HSUPA" },
	{ 7,"4G" },
	{ 8,"3G/HSPA+" },
};

//convert act to network mode;
char* util_convert_act_to_network_mode_string( int act )
{
	int index=0;
	while( index < ARRAY_ELEMENTS_COUNT( network_mode_act_pairs ) )
	{
		if( network_mode_act_pairs[index].value == act )
		{
			return network_mode_act_pairs[index].string_for_value;
		}
		index++;
	}

	return NULL;
}

//上报当前系统运行的时间
void util_read_system_uptime( char* uptime_value )
{
	char uptime_string[128];
	char* args[18];
	int args_count;

	if( uptime_value != NULL )
	{
		uptime_value[ 0 ]=0;
	}

	//清空字符串
	memset( uptime_string,0,sizeof( uptime_string ) );
	//读取当前运行的时间
	cmd_cat( "/proc/uptime",uptime_string,sizeof( uptime_string ) );
	//对获取的字段进行分割
	args_count=util_get_all_args( uptime_string,' ','\t',args,18 );
	if( args_count >= 1 )
	{
		strcpy( uptime_value,args[0] );
	}
}

#ifndef WIN32
//获取MAC地址,成功返回0,否则返回-1
int util_load_mac_addr( char*net_dev_name,char* mac_addr )
{
	MacAddr mac;

	if( mac_addr == NULL )
	{
		return -1;
	}

	//成功获取MAC地址
	if( !cmd_netdev_get_mac_addr( net_dev_name,&mac ) )
	{
		sprintf(mac_addr
				,"%02X:%02X:%02X:%02X:%02X:%02X"
				,mac.value[0]
				,mac.value[1]
				,mac.value[2]
				,mac.value[3]
				,mac.value[4]
				,mac.value[5]
				);
		//获取MAC地址成功
		return 0;
	}

	//获取MAC地址失败
	return -1;
}
#endif

//获取WAN端口的名称
char* util_get_wan_interface_name( void )
{
	char* network_dev_name;

#ifndef __i386__
	//进行文件的同步
	if( is_module_mf210_used() )
	{
		network_dev_name= "ppp0" ;
	}
	else if( is_module_sim5360_used() )
	{
		network_dev_name= "ppp0" ;
	}
	else if( is_module_mc2716_used() )
	{
		network_dev_name= "ppp0" ;
	}
	else if( is_module_c5300v_used() )
	{
		network_dev_name= "ppp0" ;
	}
	else
	{
		//P500,B57_1/ZTE 7510,ZM8620,L1761
		char recvbuf[100]="";
		if(util_get_cmd_result("route -n | grep UG | awk \'{print$8}\'",recvbuf,sizeof(recvbuf)))
		{
			//printf("jiangyibo aaaaaa %s\n",recvbuf);
			if( !strncmp(recvbuf,"eth0.2",sizeof("eth0.2")-1))
				network_dev_name= "eth0.2";	

			else if( !strncmp(recvbuf,"ath0",sizeof("ath0")-1))
				network_dev_name= "ath0";
			else if( !strncmp(recvbuf,"ath1",sizeof("ath1")-1))
				network_dev_name= "ath1";
			else 	if(!strncmp(recvbuf,"eth0",sizeof("eth0")-1))
				network_dev_name= "eth0";		
			else
				network_dev_name= "usb0";
		}
		else
			network_dev_name= "usb0" ;
	}
#else
	network_dev_name= "eth0.2" ;
#endif

	return network_dev_name;
}

//获取使用的模块的名称
char* util_get_used_module_name( void )
{
	char* used_module_name=NULL;

	//进行文件的同步
	if( is_module_mf210_used() )
	{
		used_module_name= "MF210" ;
	}
	else if( is_module_zm8620_used() )
	{
		used_module_name= "ZM8620" ;
	}
	else if( is_module_sim5360_used() )
	{
		used_module_name= "SIM5360" ;
	}
	else if( is_module_mc2716_used() )
	{
		used_module_name= "MC2716" ;
	}
	else if( is_module_c5300v_used() )
	{
		used_module_name= "C5300V" ;
	}
	else if( is_module_l1761_used() )
	{
		used_module_name= "L1761" ;
	}
	else if( is_module_p500_used() )
	{
		used_module_name= "P500" ;
	}
	else if( is_module_zte7510_used() )
	{
		used_module_name= "ZTE7510" ;
	}
	else if( is_module_u8300w_used() )
	{
		used_module_name= "U8300C/W" ;
	}
	else if( is_module_sim7230_used() )
	{
		used_module_name= "SIM7230E" ;
	}
	else if( is_module_sim7100_used() )
	{
		used_module_name= "SIM7100" ;
	}
	else if( is_module_slm630b_used() )
	{
		used_module_name= "SLM630B" ;
	}
	else if( is_module_ml7810_used() )
	{
		used_module_name= "ML7810" ;
	}
	else if(is_module_lt10_used())
	{
		used_module_name= "LT10" ;
	}
	else
	{
		used_module_name= "" ;
	}

	return used_module_name;
}

#ifndef WIN32
//创建sokcet连接,成功返回0,失败返回其他值
int util_create_socket( char* host,int port,int* socket_handle )
{
	struct sockaddr_in servaddr;
	int socket_fd;
	struct timeval timeout;
	unsigned long ul=1;
	fd_set set;
	int error=-1,len;

	//创建socket描述符
	socket_fd = socket(AF_INET,SOCK_STREAM,0);
	//创建sokcet失败
	if( socket_fd < 0)
	{
		debug("sysmanagetool:create socket error: %s(errno: %d)\n", strerror(errno),errno);
		return -1;
	}

	//设置为非阻塞模式
	ioctl(socket_fd, FIONBIO, &ul);

	timeout.tv_sec=1;
	timeout.tv_usec=0;
	//设置接收超时
	if( setsockopt( socket_fd,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout.tv_sec,sizeof(struct timeval) ) < 0 )
	{
		debug("sysmanagetool:setsockopt receive timeout fail!");
		return -1;
	}

	//设置发送超时
	timeout.tv_sec=10;
	timeout.tv_usec=0;
	if( setsockopt( socket_fd,SOL_SOCKET,SO_SNDTIMEO,(char *)&timeout.tv_sec,sizeof(struct timeval) ) < 0 )
	{
		debug("sysmanagetool:setsockopt send timeout fail!");
		return -1;
	}

	memset( &servaddr,0,sizeof(servaddr) );
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	//将字符串ip地址转换为ip地址字符串
	if( inet_pton(AF_INET,host, &servaddr.sin_addr) <= 0)
	{
		debug("sysmanagetool:inet_pton error for %s\n",host);
		return -1;
	}
	/*
	connect - initiate a connection on a socket
	int connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
	RETURN VALUE
		   If the connection or binding succeeds, zero is returned.  On error, -1 is returned, and errno is set appropriately.
	*/
	//连接服务器
	if( connect( socket_fd,(struct sockaddr*)&servaddr, sizeof(servaddr) ) == -1 )
	{
		//等待10s，如果10s还没有连接,说明要么网络太差,要么服务器根本不存在
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
		/*
		Four  macros  are  provided  to  manipulate  the  sets.   FD_ZERO()  clears  a set.  FD_SET() and FD_CLR() respectively add and remove a given file descriptor from a set.
		FD_ISSET() tests to see if a file descriptor is part of the set; this is useful after select() returns.
		*/
		FD_ZERO(&set);
		FD_SET(socket_fd, &set);
		/*
		select, pselect, FD_CLR, FD_ISSET, FD_SET, FD_ZERO - synchronous I/O multiplexing
		int select(int nfds, fd_set *readfds, fd_set *writefds,
						  fd_set *exceptfds, struct timeval *timeout);
		The timeout
			   The time structures involved are defined in <sys/time.h> and look like
				   struct timeval {
					   long    tv_sec;         / * seconds * /
					   long    tv_usec;        / * microseconds * /
				   };
			   and
				   struct timespec {
					   long    tv_sec;         / * seconds * /
					   long    tv_nsec;        / * nanoseconds * /
				   };
			   (However, see below on the POSIX.1-2001 versions.)

		Three independent sets of file descriptors are watched.  Those listed in readfds will be watched to see if characters become available for reading (more precisely, to see
			   if a read will not block; in particular, a file descriptor is also ready on end-of-file), those in writefds will be watched to see if a write will not block, and those in
			   exceptfds will be watched for exceptions.  On exit, the sets are modified in place to indicate which file descriptors actually changed status.  Each  of  the  three  file
			   descriptor sets may be specified as NULL if no file descriptors are to be watched for the corresponding class of events.

		nfds is the highest-numbered file descriptor in any of the three sets, plus 1.
		timeout is an upper bound on the amount of time elapsed before select() returns.  If both fields of the timeval stucture are  zero,  then  select()  returns  immediately.
		(This is useful for polling.)  If timeout is NULL (no timeout), select() can block indefinitely.
		*/
		//等待
		if( select(socket_fd+1, NULL, &set, NULL, &timeout) > 0)
		{
		    getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
		    if(error != 0)
		    {
		    	debug("sysmanagetool:connect error: %s(errno: %d)\n",strerror(errno),errno);
		    	return -1;
		    }
		    else
		    {
		    	if( ( errno != EINPROGRESS ) && ( errno != 0 ) )
		    	{
		    		debug("sysmanagetool:connect error: %s(errno: %d)\n",strerror(errno),errno);
		    		return -1;
		    	}
		    }
		}
		else
		{
			debug("sysmanagetool:connect error: %s(errno: %d)\n",strerror(errno),errno);
			return -1;
		}
	}

	//设置socket为阻塞模式
	ul=0;
	ioctl( socket_fd,FIONBIO,&ul ); //设置为阻塞模式

	*socket_handle=socket_fd;

	return 0;
}
#endif

//读取当前的内存使用信息
int util_read_meminfo( char* mem_total,char* mem_free )
{
	const char* meminfo_file="/proc/meminfo";
	FILE* file_handle;
	char buffer[256];
	char*p1;
	/*
	[root@mls-dell-centos6 Desktop]# cat /proc/meminfo
	MemTotal:        1937876 kB
	MemFree:          122784 kB
	Buffers:          168752 kB
	Cached:           450792 kB
	SwapCached:            0 kB
	*/
	file_handle=fopen(meminfo_file,"r");
	//读取文件每一行,并进行分析
	if( file_handle == NULL )
	{
		return FALSE;
	}

	//读取文件的每一行,并进行处理
	while( fgets( buffer,sizeof(buffer)-1,file_handle ) != NULL )
	{
		p1=strstr(buffer,":");
		if( p1 != NULL )
		{
			//模块物理层版本
			if( !strncmp(buffer,"MemTotal",p1-buffer) )
			{
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				//去除最后的空白
				util_strip_traling_spaces(p1);
				if( mem_total != NULL )
				{
					strcpy( mem_total,p1);
				}
			}
			//模块高层协议版本
			else if( !strncmp(buffer,"MemFree",p1-buffer) )
			{
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				//去除最后的空白
				util_strip_traling_spaces(p1);
				if( mem_free != NULL )
				{
					strcpy( mem_free,p1);
				}
			}
		}
	}

	//关掉文件句柄
	fclose( file_handle );

	return TRUE;
}

//读取当前所使用的lns服务器信息
int util_read_lns_info( char* lns,char* name,char* pass )
{
	const char* meminfo_file=SELECTED_LNS_SERVER_INFO;
	FILE* file_handle;
	char buffer[256];
	char*p1;
	/*
	[root@mls-dell-centos6 Desktop]# cat /proc/meminfo
	MemTotal:        1937876 kB
	MemFree:          122784 kB
	Buffers:          168752 kB
	Cached:           450792 kB
	SwapCached:            0 kB
	*/
	file_handle=fopen(meminfo_file,"r");
	//读取文件每一行,并进行分析
	if( file_handle == NULL )
	{
		return FALSE;
	}

	//读取文件的每一行,并进行处理
	while( fgets( buffer,sizeof(buffer)-1,file_handle ) != NULL )
	{
		p1=strstr(buffer,":");
		if( p1 != NULL )
		{
			if( !strncmp(buffer,"lns",p1-buffer) )
			{
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				//去除最后的空白
				util_strip_traling_spaces(p1);
				if( lns != NULL )
				{
					strcpy( lns,p1);
				}
			}
			else if( !strncmp(buffer,"name",p1-buffer) )
			{
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				//去除最后的空白
				util_strip_traling_spaces(p1);
				if( name != NULL )
				{
					strcpy( name,p1);
				}
			}
			else if( !strncmp(buffer,"pass",p1-buffer) )
			{
				//跳过所有开头的空白
				p1=util_skip_all_spaces( p1+1 );
				//去除最后的空白
				util_strip_traling_spaces(p1);
				if( pass != NULL )
				{
					strcpy( pass,p1);
				}
			}
		}
	}

	//关掉文件句柄
	fclose( file_handle );

	return TRUE;
}

//读取文件/proc/tozed/ind的内容,获取当前系统的状态信息,成功返回0,失败返回-1
int util_read_status_ind(
							int* is_sim_exist
							,int* need_pin
							,int* need_puk
							,int* signal_quality
							,int* sms_status
							,int* reg_status
							,int* greg_status
							,int* ereg_status
							,int* act
							,int* network_data_service
						)
{
#ifndef __i386__
	const char* file_name="/proc/tozed/ind";
#else
	const char* file_name="./ind";
#endif
	char string_buffer[128];
	char* args[18];
	int args_count;

	//清空字符串
	memset( string_buffer,0,sizeof( string_buffer ) );
	//读取当前运行的时间
	cmd_cat( file_name,string_buffer,sizeof( string_buffer ) );
	//对获取的字段进行分割
	args_count=util_get_all_args( string_buffer,' ','\t',args,18 );
	if( args_count >= 10 )
	{
		if( is_sim_exist != NULL )
		{
			*is_sim_exist=atoi( args[ 0 ] );
		}

		if( need_pin != NULL )
		{
			*need_pin=atoi( args[ 1 ] );
		}

		if( need_puk != NULL )
		{
			*need_puk=atoi( args[ 2 ] );
		}

		if( signal_quality != NULL )
		{
			*signal_quality=atoi( args[ 3 ] );
		}

		if( sms_status != NULL )
		{
			*sms_status=atoi( args[ 4 ] );
		}

		if( reg_status != NULL )
		{
			*reg_status=atoi( args[ 5 ] );
		}

		if( greg_status != NULL )
		{
			*greg_status=atoi( args[ 6 ] );
		}

		if( ereg_status != NULL )
		{
			*ereg_status=atoi( args[ 7 ] );
		}

		if( act != NULL )
		{
			*act=atoi( args[ 8 ] );
		}

		if( network_data_service != NULL )
		{
			*network_data_service=atoi( args[ 9 ] );
		}

		//成功获取需要的数据
		return 0;
	}

	//获取数据失败
	return -1;
}

//l2tp sccrq请求发送的请求数据包
static const unsigned char l2tp_sccrq_data[]={
	0XC8,0X02,0X00,0X7C,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X80,0X08,0X00,0X00,
	0X00,0X00,0X00,0X01,0X80,0X08,0X00,0X00,0X00,0X02,0X01,0X00,0X80,0X0A,0X00,0X00,
	0X00,0X03,0X00,0X00,0X00,0X03,0X80,0X0A,0X00,0X00,0X00,0X04,0X00,0X00,0X00,0X00,
	0X00,0X08,0X00,0X00,0X00,0X06,0X06,0X90,0X80,0X0B,0X00,0X00,0X00,0X07,0X6C,0X74,
	0X65,0X6B,0X64,0X00,0X13,0X00,0X00,0X00,0X08,0X78,0X65,0X6C,0X65,0X72,0X61,0X6E,
	0X63,0X65,0X2E,0X63,0X6F,0X6D,0X80,0X08,0X00,0X00,0X00,0X09,0X53,0XF0,0X80,0X08,
	0X00,0X00,0X00,0X0A,0X00,0X04,0X80,0X16,0X00,0X00,0X00,0X0B,0X04,0X6F,0XC1,0X0F,
	0X96,0XBB,0X28,0XF4,0X9F,0XC2,0X88,0X20,0X81,0X2D,0XA8,0XA6,
};

static unsigned char l2tp_sccrq_data_new[ sizeof( l2tp_sccrq_data )+20 ];

#ifndef WIN32
//检查一个UDP端口是否被打开,用于检测L2TP端口是否被打开,成功返回1,否则返回0
int util_check_if_one_udp_port_is_open( char* ip_addr,int port,char* hostname )
{
	int socket_descriptor; //套接口描述字
	char message[256];
	struct sockaddr_in server_addr;//处理网络通信的地址
	int sin_len;
	int byte_read;
	struct timeval tv_out;
	int original_send_data_length=( l2tp_sccrq_data[2]<<8 )+l2tp_sccrq_data[3];
	int send_data_length=original_send_data_length;
	int hostname_type_length_offset=0x39;
	int hostname_offset=0x3E;
	int original_hostname_length=strlen( "ltekd" );
	int hostname_length=strlen( hostname );
	int challenge_offset=0x66;
	int challenge_length=0;

	if(	( hostname == NULL )
		||( !strlen( hostname ) )
		)
	{
		hostname="l2tp-client";
		hostname_length=strlen( hostname );
		//不发送challenge字段
		challenge_length=original_send_data_length-challenge_offset;
	}

	//重新计算要发送的数据长度
	send_data_length=original_send_data_length+hostname_length-original_hostname_length-challenge_length;
	//将内容复制到l2tp_sccrq_data_new中
	memcpy( l2tp_sccrq_data_new,l2tp_sccrq_data,hostname_offset );
	memcpy( l2tp_sccrq_data_new+hostname_offset,hostname,hostname_length );
	memcpy( l2tp_sccrq_data_new+hostname_offset+hostname_length
			,l2tp_sccrq_data+hostname_offset+original_hostname_length
			,original_send_data_length-hostname_offset-original_hostname_length
			);
	//修改总的长度字段
	l2tp_sccrq_data_new[2]=( send_data_length&0xff00 )>>8;
	l2tp_sccrq_data_new[3]=send_data_length&0xff;
	//修改主机名称的长度字段
	hostname_length=11+hostname_length-original_hostname_length;
	l2tp_sccrq_data_new[ hostname_type_length_offset ]=hostname_length&0xff;



	bzero( &server_addr,sizeof( server_addr ) );
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=inet_addr( ip_addr );//这里不一样
	server_addr.sin_port=htons( port );

	//检查IP地址是否有效
	if( inet_pton( AF_INET,ip_addr, &server_addr.sin_addr ) <= 0 )
	{
		debug("ip address:%s is not valid!\n",ip_addr);
		return FALSE;
	}

	//创建一个 UDP socket
	socket_descriptor=socket(AF_INET,SOCK_DGRAM,0);//IPV4  SOCK_DGRAM 数据报套接字（UDP协议）
	//创建socket失败
	if( socket_descriptor == -1 )
	{
		debug("( socket_descriptor == -1 )\n");
		return 0;
	}

	//设置等待超时
	tv_out.tv_sec = 4;//等待10秒
	tv_out.tv_usec = 0;
	setsockopt( socket_descriptor,SOL_SOCKET,SO_RCVTIMEO,&tv_out,sizeof( tv_out ) );

	if( connect( socket_descriptor,(struct sockaddr *)&server_addr,sizeof( server_addr ) ) ==-1 )
	{
		close(socket_descriptor);
		debug("udp connect....");
		return FALSE;
	}

	/*int PASCAL FAR sendto( SOCKET s, const char FAR* buf, int len, int flags,const struct sockaddr FAR* to, int tolen);
	* s：一个标识套接口的描述字。
	* buf：包含待发送数据的缓冲区。
	* len：buf缓冲区中数据的长度。
	* flags：调用方式标志位。
	* to：（可选）指针，指向目的套接口的地址。
	* tolen：to所指地址的长度。
	*/
	//向对方发送数据SCCRQ数据包,检查是否有响应
	sendto( socket_descriptor,l2tp_sccrq_data_new,send_data_length,0,(struct sockaddr *)&server_addr,sizeof(server_addr) );//发送stop 命令
	//从socket_descriptor接收数据
	sin_len=sizeof( server_addr );
	byte_read=recvfrom( socket_descriptor,message,sizeof( message ),0,(struct sockaddr *)&server_addr,(socklen_t *)&sin_len );
	if( byte_read == -1 )
	{
		debug( "lns server %s not exist!",ip_addr );
		close(socket_descriptor);
		return FALSE;
	}

	//关掉socket描述符
	close(socket_descriptor);

	//连接成功
	return TRUE;
}
#endif


//检查FDD预置频点配置是否为空
int util_is_fdd_prestore_frequency_empty( InfoStruct* info_struct )
{
	if( info_struct == NULL )
	{
		return TRUE;
	}
	util_strip_blank_of_string_end( info_struct->TZ_PRESTORE_FREQUENCY_FDD );
	return !strlen( info_struct->TZ_PRESTORE_FREQUENCY_FDD );
}

//检查TDD预置频点配置是否为空
int util_is_tdd_prestore_frequency_empty( InfoStruct* info_struct )
{
	if( info_struct == NULL )
	{
		return TRUE;
	}
	util_strip_blank_of_string_end( info_struct->TZ_PRESTORE_FREQUENCY_TDD );
	return !strlen( info_struct->TZ_PRESTORE_FREQUENCY_TDD );
}

//用于保存临时的预置频点列表
static char tmp_buffered_prestore_frequency_list[ 256 ];
static char tmp_buffered_at_response_result[ 256 ];

//获取没有在预置频点列表中的频点信息
char* util_get_next_prestore_frequency_not_in_list( char* tmp_at_response_result,InfoStruct* info_struct,PRESTORE_FREQUENCY_LIST_TYPE list_type  )
{
	char* response_args[32];
	int response_args_count;
	int response_args_index;
	char* list_args[32];
	int list_args_count;
	int list_args_index;
	char* at_response_result=tmp_buffered_at_response_result;

	STRCPY_S( tmp_buffered_at_response_result,tmp_at_response_result );
	//将需要预置的信息保存到列表中
	if( list_type == PRESTORE_FREQUENCY_LIST_TYPE_FDD )
	{
		//预置频点列表为空
		if( util_is_fdd_prestore_frequency_empty( info_struct ) )
		{
			return NULL;
		}
		STRCPY_S( tmp_buffered_prestore_frequency_list,info_struct->TZ_PRESTORE_FREQUENCY_FDD );
	}
	else
	{
		//预置频点列表为空
		if( util_is_tdd_prestore_frequency_empty( info_struct ) )
		{
			return NULL;
		}
		STRCPY_S( tmp_buffered_prestore_frequency_list,info_struct->TZ_PRESTORE_FREQUENCY_TDD );
	}

	//获取预置频点列表
	list_args_count=util_get_all_args( tmp_buffered_prestore_frequency_list,' ',',',list_args,sizeof( list_args )/sizeof( list_args[0] ) );
	//没有预置频点
	if( !list_args_count )
	{
		return NULL;
	}

	//获取已经保存的频点列表
	response_args_count=util_get_all_args( at_response_result,' ',',',response_args,sizeof( response_args )/sizeof( response_args[0] ) );
	for( list_args_index=0;list_args_index < list_args_count;list_args_index+=1 )
	{
		char* list_arg_value=list_args[ list_args_index ];
		int value_in_list=FALSE;
		for( response_args_index=0;response_args_index < response_args_count;response_args_index+=1 )
		{
			if( !strcmp( response_args[ response_args_index ],list_arg_value ) )
			{
				value_in_list=TRUE;
				break;
			}
		}

		//存在值不在列表中的情况
		if( !value_in_list )
		{
			return list_arg_value;
		}
	}

	return NULL;
}


char* util_get_next_fdd_prestore_frequency_not_in_list( char* at_response_result,InfoStruct* info_struct )
{
	return util_get_next_prestore_frequency_not_in_list( at_response_result,info_struct,PRESTORE_FREQUENCY_LIST_TYPE_FDD );
}

//检查TDD预置频点已经在列表中
char* util_get_next_tdd_prestore_frequency_not_in_list( char* at_response_result,InfoStruct* info_struct  )
{
	return util_get_next_prestore_frequency_not_in_list( at_response_result,info_struct,PRESTORE_FREQUENCY_LIST_TYPE_TDD );
}

//检查FDD预置频点已经在列表中
//^DEFS: 10,1650,75,1750,1800,1551,1651,2850,3375,1675,1625
int util_is_all_fdd_prestore_frequency_in_list( char* at_response_result,InfoStruct* info_struct  )
{
	if( util_get_next_fdd_prestore_frequency_not_in_list( at_response_result,info_struct ) != NULL )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

//检查TDD预置频点已经在列表中
int util_is_all_tdd_prestore_frequency_in_list( char* at_response_result,InfoStruct* info_struct  )
{
	if( util_get_next_tdd_prestore_frequency_not_in_list( at_response_result,info_struct ) != NULL )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

//获取锁IMSI的策略
int util_get_TZ_LOCK_IMSI( InfoStruct* nvram_config_info )
{
	int value=atoi( nvram_config_info->TZ_LOCK_IMSI );
	
	if( value < 0 )
	{
		value=LOCK_IMSI_STRATEGY_NONE;
	}
	
	if( value >= LOCK_IMSI_STRATEGY_LOCK_MAX )
	{
		value=LOCK_IMSI_STRATEGY_NONE;
	}
	
	return value;
}

//为网页提供用户发送AT命令的端口
char* util_get_at_port( void )
{
	char* at_command_port=NULL;

	if( is_module_c5300v_used() )
	{
		at_command_port="/dev/ttyUSB3";
	}
	else if( is_module_sim5360_used() )
	{
		at_command_port="/dev/ttyUSB2";
	}
	else if( is_module_sim7100_used() )
	{
		at_command_port="/dev/ttyUSB3";
	}
	else if( is_module_ar9500_used() )
	{
		at_command_port="/dev/ttyUSB3";
	}
	else
	{
		at_command_port="/dev/ttyUSB1";
	}

	return at_command_port;
}

//获取模块LOG的文件名称,返回不为空,则说明抓取的模块LOG文件在CPE内部,停止抓取LOG之后,需使用超链接进行下载
//否则不需要使用超链接进行下载
//例如返回的值为/tmp/sim5360-diag/sim5360-diag-log.tar.lzma,则需要在/tzwww目录下创建超链接文件进行下载
//ln -sf /tmp/sim5360-diag/sim5360-diag-log.tar.lzma $(INSTALL_ROOT)/tzwww/sim5360-diag-log.tar.lzma
char* util_get_modem_log_file_name( void )
{
	if( is_module_sim7100_used() )
	{
		return "/tmp/sim5360-diag/sim5360-diag-log.tar.lzma";
	}
	else if( is_module_sim5360_used() )
	{
		return "/tmp/sim5360-diag/sim5360-diag-log.tar.lzma";
	}

	return "";
}

//是否支持加密卡
/*
(2)AT^DAUTYPE=<authen-type>
设置加密卡类型。
<authen-type>:加密卡类型,支持(0-非加密卡;2-CMCC-加密卡;4-JIANGSU-加密卡;
8-VOQI-V3 加密卡)。
*/
int util_encrypt_card_supported( InfoStruct* info_struct )
{
	if( !strlen( info_struct->TZ_ENCRYPT_CARD_TYPE ) )
	{
		return FALSE;
	}

	return strcmp( info_struct->TZ_ENCRYPT_CARD_TYPE,"0" );
}

//加密卡是否使用了16组密钥
/*
目前有两种加密卡种类
一种是全国加密卡,参数<authen-type>设置为 2- - -CMCC-加密卡;
一种是江苏加密卡,参数<authen-type>设置为 4- - -JIANGSU-加密卡
*/
int util_encrypt_card_has_16_keys( InfoStruct* info_struct )
{
	return !strcmp( info_struct->TZ_ENCRYPT_CARD_TYPE,"4" );
}

//是否应该锁定一个小区
int util_should_lock_one_cell( InfoStruct* info_struct )
{
	//这几个参数只有远程锁物理小区的时候才会被使用
	if( !strcmp( info_struct->TZ_LOCK_ONE_CELL,"yes" )
		&& strlen( info_struct->TZ_LOCKED_EARFCN )
		&& strlen( info_struct->TZ_LOCKED_PHYCELLID )
		&& strlen( info_struct->TZ_LOCKED_Enode_ID )
		)
	{
		return TRUE;
	}

	return FALSE;
}

//获取移动数据连接默认使用的cid的值
int util_get_default_cid_value( InfoStruct* info_struct )
{
	if( strlen( info_struct->TZ_PPP_USERNAME ) || strlen( info_struct->TZ_PPP_PASSWORD ) )
	{
		if( is_module_l1761_used() )
		{
			return 2;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return 1;
	}
}

//将TZ_BAND_PREF保存的锁频字段band_pref转换为ml7810的锁频字段ml7810_band_pref,成功返回0,否则返回其他值
int util_convert_band_pref_to_ml8710_set( char* band_pref,char* ml7810_band_pref )
{
	char tmp_buffer[ 128 ];
	int index=0;
	int len;

	if(
		( band_pref == NULL )
		|| ( ml7810_band_pref == NULL )
		)
	{
		return -1;
	}

	memset( tmp_buffer,0,sizeof(tmp_buffer) );

	if( strlen( band_pref ) < 16 )
	{
		int zero_count=16-strlen( band_pref );
		index=0;
		while( index < zero_count )
		{
			strcat( tmp_buffer,"0" );
			index+=1;
		}
	}
	strcat( tmp_buffer,band_pref );

	//将锁频的设置转换为ml7810的设置
	len=strlen( tmp_buffer );

	//将TZ_BAND_PREF中的设置保存为ml7810的频段设置
	ml7810_band_pref[ 0 ]='\0';
	index=0;
	while( index < 8 )
	{
		if( index == 0 )
		{
			sprintf(
					ml7810_band_pref+strlen( ml7810_band_pref )
					,"%d"
					,util_char2hex( tmp_buffer[len-2-index*2] )*16+util_char2hex( tmp_buffer[len-1-index*2] )
					);
		}
		else
		{
			sprintf(
					ml7810_band_pref+strlen( ml7810_band_pref )
					,",%d"
					,util_char2hex( tmp_buffer[len-2-index*2] )*16+util_char2hex( tmp_buffer[len-1-index*2] )
					);
		}
		index+=1;
	}

	return 0;
}

//将ml7810的锁频字段ml7810_band_pref转换为TZ_BAND_PREF保存的锁频字段band_pref,成功返回0,否则返回其他值
int util_get_band_pref_from_ml8710_set( char* band_pref,char* ml7810_band_pref )
{
	unsigned int bands[ 8 ];
	char* next_arg=ml7810_band_pref;
	int index=0;

	if(
		( band_pref == NULL )
		|| ( ml7810_band_pref == NULL )
		)
	{
		return -1;
	}

	if( *next_arg == '\r')
	{
		return -1;
	}

	memset( bands,0,sizeof(bands) );
	//69,0,0,0,224,1,0,0
	while( index < sizeof(bands)/sizeof(bands[0]) )
	{
		bands[ index ]=atoi( next_arg );
		index++;
		//获取下一个参数
		next_arg=util_get_next_arg_cmdlib(next_arg,FALSE);
		if( *next_arg == '\r')
		{
			break;
		}
	}

	//将所有的频段信息写入字符串中
	band_pref[ 0 ]='\0';
	index=0;
	while( index < sizeof(bands)/sizeof(bands[0]) )
	{
		sprintf(
				band_pref+strlen( band_pref )
				,"%02X"
				,bands[ sizeof(bands)/sizeof(bands[0])-1-index ]
				);
		index++;
	}

	return 0;
}

//获取存在的ppp网卡名称,成功返回0,否则返回其他
int util_get_ppp_net_dev( char* ppp_dev )
{
	int index=0;
	char tmp_used_buffer[ 16 ];

	while( index <= 8 )
	{
		snprintf(tmp_used_buffer
				,sizeof( tmp_used_buffer )
				,"ppp%d"
				,index
				);

		if( cmd_netdev_exist( tmp_used_buffer ) )
		{
			if( ppp_dev != NULL )
			{
				strcpy( ppp_dev,tmp_used_buffer );
			}
			return 0;
		}

		index+=1;
	}

	if( ppp_dev != NULL )
	{
		*ppp_dev=0;
	}
	return -1;
}


//路由模式是否被开启
int util_is_router_mode_enabled( InfoStruct* info_struct )
{
	return !strcmp( info_struct->TZ_CPE_ROUTER_MODE,"yes" );
}

//L3Bridge模式是否被开启
int util_is_l3bridge_mode_enabled( void )
{
	return cmd_file_exist( L3BRIDGE_IS_ENABLE );
}
//LTE网络搜索策略
int util_get_lte_search_network_strategy( InfoStruct* info_struct )
{
	int strategy=atoi( info_struct->TZ_4G_FDD_PREFERRED );

	if( !strlen( info_struct->TZ_4G_FDD_PREFERRED ) )
	{
		strategy=LTE_SEARCH_NETWORK_NOT_CHANGE;
	}

	if( strategy < LTE_SEARCH_NETWORK_STRATEGY_MIN )
	{
		strategy=LTE_SEARCH_NETWORK_NOT_CHANGE;
	}

	if( strategy >= LTE_SEARCH_NETWORK_STRATEGY_MAX )
	{
		strategy=LTE_SEARCH_NETWORK_NOT_CHANGE;
	}

	return strategy;
}

/*
去掉cfg-e输出的字符串中包含如下形式的情况
export TZ_WMAC_GATEWAY="192.168.1.2"
export TZ_WMAC_DNS="202.96.134.133"
export TZ_SUPER_USERNAME=rootaccount
export TZ_SUPER_PASSWD=6b63f2cf2a0933f0a4131ced5d7f476b
export aaaa="\"cndistgt"
*/
void util_strip_quote_string_from_cfg_e( char* processed_quoted_string )
{
	char* escaped_quoted_string;

	//字符串为空
	if( processed_quoted_string == NULL )
	{
		return;
	}

	//去掉开头和结尾的双引号
	if( ( *processed_quoted_string == '\"' )
		&& strlen( processed_quoted_string )
		&& ( processed_quoted_string[ strlen( processed_quoted_string )-1 ] == '\"' )
		)
	{
		processed_quoted_string[ strlen( processed_quoted_string )-1 ]='\0';
		memmove( processed_quoted_string,processed_quoted_string+1,strlen( processed_quoted_string ) );
	}

	//去掉字符串中如下形式的\"
	escaped_quoted_string=strstr( processed_quoted_string,"\\\"" );
	while( escaped_quoted_string != NULL )
	{
		memmove( escaped_quoted_string,escaped_quoted_string+1,strlen( escaped_quoted_string ) );
		escaped_quoted_string=strstr( processed_quoted_string,"\\\"" );
	}
}

//当前模块是否支持L3 bridging模式
int util_is_this_module_support_l3_bridge_mode( void )
{
	if( is_module_zm8620_used() )
	{
		return FALSE;
	}
	else if( is_module_l1761_used() )
	{
		return FALSE;
	}
	else if( is_module_p500_used() )
	{
		return FALSE;
	}
	else if( is_module_zte7510_used() )
	{
		return TRUE;
	}
	else if( is_module_u8300w_used() )
	{
		return FALSE;
	}
	else if( is_module_sim7100_used() )
	{
		return FALSE;
	}
	else
	{
		return FALSE;
	}

	return FALSE;
}

//是否对不存在的LNS服务器进行重新拨号检查
int util_should_check_if_lns_alive( InfoStruct* info_struct )
{
	return !!strcmp( info_struct->TZ_CHECK_IF_LNS_ALIVE,"no" );
}

//检查是否允许注册到当前的网络
int util_is_register_to_curent_plmn_alowed( InfoStruct* info_struct,char* imsi )
{
	int valid_plmn_count=0;

	if( strcmp( info_struct->TZ_ENABLE_LOCK_NETWORK,"yes" ) )
	{
		return TRUE;
	}

	//没有提供IMSI信息,允许上网
	if( imsi == NULL )
	{
		return TRUE;
	}

	//没有IMSI信息
	if( !strlen( imsi ) )
	{
		return TRUE;
	}

	//检查锁定的IMSI信息是否和要锁定的网络信息相同
	if( IS_STRING_A_PREFIX_OF_B( info_struct->TZ_LOCK_NETWORK_PLMN,imsi ) )
	{
		return TRUE;
	}

	if( IS_STRING_A_PREFIX_OF_B( info_struct->TZ_LOCK_NETWORK_PLMN2,imsi ) )
	{
		return TRUE;
	}

	if( IS_STRING_A_PREFIX_OF_B( info_struct->TZ_LOCK_NETWORK_PLMN3,imsi ) )
	{
		return TRUE;
	}

	if( IS_STRING_A_PREFIX_OF_B( info_struct->TZ_LOCK_NETWORK_PLMN4,imsi ) )
	{
		return TRUE;
	}

	if( IS_STRING_A_PREFIX_OF_B( info_struct->TZ_LOCK_NETWORK_PLMN5,imsi ) )
	{
		return TRUE;
	}

	if( IS_STRING_A_PREFIX_OF_B( info_struct->TZ_LOCK_NETWORK_PLMN6,imsi ) )
	{
		return TRUE;
	}

	if( IS_STRING_A_PREFIX_OF_B( info_struct->TZ_LOCK_NETWORK_PLMN7,imsi ) )
	{
		return TRUE;
	}

	if( IS_STRING_A_PREFIX_OF_B( info_struct->TZ_LOCK_NETWORK_PLMN8,imsi ) )
	{
		return TRUE;
	}

	if( IS_STRING_A_PREFIX_OF_B( info_struct->TZ_LOCK_NETWORK_PLMN9,imsi ) )
	{
		return TRUE;
	}

	if( IS_STRING_A_PREFIX_OF_B( info_struct->TZ_LOCK_NETWORK_PLMN10,imsi ) )
	{
		return TRUE;
	}

	if( strlen( info_struct->TZ_LOCK_NETWORK_PLMN ) )
	{
		valid_plmn_count++;
	}

	if( strlen( info_struct->TZ_LOCK_NETWORK_PLMN2 ) )
	{
		valid_plmn_count++;
	}

	if( strlen( info_struct->TZ_LOCK_NETWORK_PLMN3 ) )
	{
		valid_plmn_count++;
	}

	if( strlen( info_struct->TZ_LOCK_NETWORK_PLMN4 ) )
	{
		valid_plmn_count++;
	}

	if( strlen( info_struct->TZ_LOCK_NETWORK_PLMN5 ) )
	{
		valid_plmn_count++;
	}

	if( strlen( info_struct->TZ_LOCK_NETWORK_PLMN6 ) )
	{
		valid_plmn_count++;
	}

	if( strlen( info_struct->TZ_LOCK_NETWORK_PLMN7 ) )
	{
		valid_plmn_count++;
	}

	if( strlen( info_struct->TZ_LOCK_NETWORK_PLMN8 ) )
	{
		valid_plmn_count++;
	}

	if( strlen( info_struct->TZ_LOCK_NETWORK_PLMN9 ) )
	{
		valid_plmn_count++;
	}

	if( strlen( info_struct->TZ_LOCK_NETWORK_PLMN10 ) )
	{
		valid_plmn_count++;
	}

	//没有进行PLMN配置
	if( !valid_plmn_count )
	{
		return TRUE;
	}

	return FALSE;
}

//是否需要对admin的帐号进行随机化
int util_should_admin_password_randomized( InfoStruct* info_struct )
{
	return !strcmp( info_struct->TZ_ADMIN_RANDOM_PASSWORD,"yes" );
}

//统计指定目录下的文件数量
int util_get_file_count_under_one_directory( char* directory_name )
{
	DIR* dir_handle;
	struct dirent* dir_entry;
	int file_count=0;

	if( directory_name == NULL )
	{
		return 0;
	}

	//无效的目录
	if( !strlen( directory_name ) )
	{
		return 0;
	}

	dir_handle=opendir( directory_name );

	if( dir_handle == NULL )
	{
		return 0;
	}

	while( ( dir_entry=readdir( dir_handle ) ) != NULL )
	{
		//当前工作目录
		if( !strcmp( dir_entry->d_name,"." ) )
		{
			continue;
		}

		//上级目录
		if( !strcmp( dir_entry->d_name,".." ) )
		{
			continue;
		}

		file_count++;
	}

	closedir(dir_handle);

	return file_count;
}

//将所有的小写字母都转换为大写字母
void util_convert_all_chars_to_upper_case( char* original_string )
{
	if( original_string == NULL )
	{
		return;
	}

	while( *original_string )
	{
		if( ( *original_string >= 'a' ) && ( *original_string <= 'z' ) )
		{
			*original_string=*original_string-'a'+'A';
		}
		original_string++;
	}
}

//将所有的小写字母都转换为大写字母
void util_convert_all_chars_to_lower_case( char* original_string )
{
	if( original_string == NULL )
	{
		return;
	}

	while( *original_string )
	{
		if( ( *original_string >= 'A' ) && ( *original_string <= 'Z' ) )
		{
			*original_string=*original_string-'A'+'a';
		}
		original_string++;
	}
}

//获取当前设置的cs/ps业务模式
int util_get_cs_ps_service_mode( InfoStruct* info_struct )
{
	unsigned int cs_ps_mode=CS_PS_WORK_MODE_MAX;
	if( !strlen( info_struct->TZ_CS_PS_PREFERRED_MODE ) )
	{
		return CS_PS_WORK_MODE_MAX;
	}

	cs_ps_mode=atoi( info_struct->TZ_CS_PS_PREFERRED_MODE );

	if( cs_ps_mode >= CS_PS_WORK_MODE_MAX )
	{
		cs_ps_mode=CS_PS_WORK_MODE_MAX;
	}

	return cs_ps_mode;
}

/*
根据驱动识别到的串口进行判定
~ # ls /sys/bus/usb/drivers
GobiNet            cdc_ncm            hub                usb                usbserial
cdc_ether          ftdi_sio           option             usbfs              usbserial_generic

~ # ls /sys/bus/usb/drivers/option
module  uevent

~ # ls /sys/bus/usb/drivers/ftdi_sio
module  uevent

~ # ls /sys/bus/usb/drivers/usbserial
module  uevent

~ # ls /sys/bus/usb/drivers/usbserial_generic
1-1.4:1.1  1-1.4:1.2  1-1.4:1.3  1-1.4:1.4  module     uevent


~ # ls /sys/bus/usb/drivers/usbserial_generic/1-1.4:1.1
bAlternateSetting      bInterfaceProtocol     driver                 iad_bFirstInterface    iad_bFunctionSubClass  power                  ttyUSB0
bInterfaceClass        bInterfaceSubClass     ep_08                  iad_bFunctionClass     iad_bInterfaceCount    subsystem              uevent
bInterfaceNumber       bNumEndpoints          ep_82                  iad_bFunctionProtocol  modalias               supports_autosuspend

~ # ls /sys/bus/usb/drivers/usb
1-1     1-1.4   module  uevent  usb1
~ #
~ #
~ # ls /sys/bus/usb/drivers/usb/1-1
1-1.4                bDeviceClass         bMaxPower            bmAttributes         dev                  idProduct            product              uevent
1-1:1.0              bDeviceProtocol      bNumConfigurations   busnum               devnum               idVendor             quirks               urbnum
authorized           bDeviceSubClass      bNumInterfaces       configuration        driver               maxchild             speed                usb_device
bConfigurationValue  bMaxPacketSize0      bcdDevice            descriptors          ep_00                power                subsystem            version

~ # ls /sys/bus/usb/drivers/usb/1-1/1-1.4
1-1.4:1.0            1-1.4:1.5            bDeviceProtocol      bNumInterfaces       descriptors          idProduct            product              urbnum
1-1.4:1.1            1-1.4:1.6            bDeviceSubClass      bcdDevice            dev                  idVendor             quirks               usb_device
1-1.4:1.2            authorized           bMaxPacketSize0      bmAttributes         devnum               manufacturer         speed                version
1-1.4:1.3            bConfigurationValue  bMaxPower            busnum               driver               maxchild             subsystem
1-1.4:1.4            bDeviceClass         bNumConfigurations   configuration        ep_00                power                uevent

~ # ls /sys/bus/usb/drivers/usb/1-1/1-1.4/1-1.4:1.0
bAlternateSetting      bInterfaceProtocol     ep_81                  iad_bFunctionProtocol  modalias               supports_autosuspend
bInterfaceClass        bInterfaceSubClass     iad_bFirstInterface    iad_bFunctionSubClass  power                  uevent
bInterfaceNumber       bNumEndpoints          iad_bFunctionClass     iad_bInterfaceCount    subsystem

~ # cat  /sys/bus/usb/drivers/usb/1-1/1-1.4/idVendor
1ab7
~ # cat  /sys/bus/usb/drivers/usb/1-1/1-1.4/idProduct
1761
*/

//find /sys/bus/usb/drivers/usb
//([\w\/\-\.\:]+)
//,"\1"
const char* const used_usb_devices_directories[]={
#ifdef __i386__
	"/sys/bus/usb/drivers/usb/3-1"
	,"/sys/bus/usb/drivers/usb/3-2"
	,"/sys/bus/usb/drivers/usb/5-1"
	,"/sys/bus/usb/drivers/usb/1-8"
#else
	"/sys/bus/usb/drivers/usb/1-1.4"
	,"/sys/bus/usb/drivers/usb/1-1.3"
	,"/sys/bus/usb/drivers/usb/1-1.2"
#endif
};

#define USB_DEVICE_SEARCH_DIRECTORY_COUNT ( sizeof(used_usb_devices_directories)/sizeof(used_usb_devices_directories[0]) )


//根据USB的idVendor/idProduct/interface_number信息获取对应的串口号,0表示成功,否则表示失败
int util_get_serial_name_by_usb_interface( int idVendor,int idProduct,int interface_number,char* serial_name )
{
	int index;
	DIR* dir_handle;
	struct dirent* dir_entry;
	char tmp_value[ 64 ];
	char tmp_buffer[ 128 ];
	int have_usb_serial_exist=FALSE;
	int directory_index;

	for( directory_index=0;directory_index < USB_DEVICE_SEARCH_DIRECTORY_COUNT;directory_index++ )
	{
		//首先检查指定目录下的idVendor/idProduct是否和设定的相同
		snprintf(tmp_buffer
				,sizeof( tmp_buffer )
				,"%s/idVendor"
				,used_usb_devices_directories[ directory_index ]
				);
		//文件idVendor不存在
		if( !cmd_file_exist( tmp_buffer ) )
		{
			continue;
		}
		//检查idVendor是否相同
		cmd_cat( tmp_buffer,tmp_value,sizeof(tmp_value)-1 );
		if( idVendor != ( int )strtol(tmp_value, (char **) NULL, 16) )
		{
			continue;
		}

		snprintf(tmp_buffer
				,sizeof( tmp_buffer )
				,"%s/idProduct"
				,used_usb_devices_directories[ directory_index ]
				);
		//文件idProduct不存在
		if( !cmd_file_exist( tmp_buffer ) )
		{
			continue;
		}
		//检查idProduct是否相同
		cmd_cat( tmp_buffer,tmp_value,sizeof(tmp_value)-1 );
		if( idProduct != ( int )strtol(tmp_value, (char **) NULL, 16) )
		{
			continue;
		}

		//检查数字开头的目录下是否存在指定接口的设备
		dir_handle=opendir( used_usb_devices_directories[ directory_index ] );
		if( dir_handle == NULL )
		{
			continue;
		}

		while( ( dir_entry=readdir( dir_handle ) ) != NULL )
		{
			//数字开头的目录
			if( isdigit( dir_entry->d_name[0] ) )
			{
				//检查接口属性是否存在
				snprintf(tmp_buffer
							,sizeof( tmp_buffer )
							,"%s/%s/bInterfaceNumber"
							,used_usb_devices_directories[ directory_index ]
							,dir_entry->d_name
							);

				if( !cmd_file_exist( tmp_buffer ) )
				{
					continue;
				}

				//检查接口号和设定的是否相同
				cmd_cat( tmp_buffer,tmp_value,sizeof(tmp_value)-1 );
				if( interface_number != atoi( tmp_value ) )
				{
					continue;
				}

				//检查串口设备的名称
				for( index=0;index < 20;index++ )
				{
					snprintf(tmp_buffer
							,sizeof( tmp_buffer )
							,"%s/%s/ttyUSB%d"
							,used_usb_devices_directories[ directory_index ]
							,dir_entry->d_name
							,index
							);

					//存在USB串口设备
					if( cmd_file_exist( tmp_buffer ) )
					{
						snprintf(tmp_buffer
									,sizeof( tmp_buffer )
									,"/dev/ttyUSB%d"
									,index
									);
						debug( "\r\nutil_get_serial_name_by_usb_interface:%x,%x,%d,%s\r\n",idVendor,idProduct,interface_number,tmp_buffer );
						strcpy( serial_name,tmp_buffer );
						have_usb_serial_exist=TRUE;
						goto END_PROCESS_ONE_DIRECTORY;
					}
				}
			}
		}

		END_PROCESS_ONE_DIRECTORY:

		closedir(dir_handle);
	}

	//根本不存在USB串口设备
	if( !have_usb_serial_exist )
	{
		return -1;
	}

	return 0;
}

//查找所有可用的USB串口,0表示成功,否则表示失败
int util_get_available_usb_ports( UsbSerialTable* usb_serial_table )
{
	int matched_device_index;
	int index;
	char serial_name[ 32 ];

	memset( usb_serial_table,0,sizeof( UsbSerialTable ) );
	//存在制定的串口
	if( check_if_usb_device_exist( matched_usb_pairs,USB_PAIRS_COUNT,&matched_device_index ) )
	{
		for( index=0;index < 20;index++ )
		{
			memset( serial_name,0,sizeof( serial_name ) );
			if( !util_get_serial_name_by_usb_interface( matched_usb_pairs[ matched_device_index ].vendor_id
			                                            ,matched_usb_pairs[ matched_device_index ].product_id
			                                            ,index
			                                            ,serial_name
			                                            )
				)
			{
				strcpy( usb_serial_table->table[ usb_serial_table->count ].port_name,serial_name );
				usb_serial_table->count++;
			}
		}

		//没有查找到对应的串口
		if( !( usb_serial_table->count ) )
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}

	return 0;
}

//判定模块 LOG 是否正在进行抓取的,调用如下函数判定是否正在抓取 LOG:
int util_is_log_tools_running(void)
{
	if( cmd_ps_is_one_program_running( "ComProxy" ) )
	{
		return TRUE;
	}

	if( cmd_ps_is_one_program_running( "remserial" ) )
	{
		return TRUE;
	}

	if( cmd_ps_is_one_program_running( "./diaggrab" ) )
	{
		return TRUE;
	}

	return FALSE;
}

//为高通使用gobinet驱动的设备查找对应的字符设备/dev/qcqmi*,成功返回0,失败返回非0
//将设备名称/设备的主设备号/次设备号保存到char_device_name/major_num/minor_number中
int util_get_gobinet_char_device_name( char* char_device_name,int* major_num,int* minor_number )
{
	int index;
	char tmp_buffer[ 256 ];

	//首先检查/sys/class/GobiQMI目录下是否存在文件qcqmi*
	//检查串口设备的名称
	for( index=0;index < 20;index++ )
	{
		snprintf(tmp_buffer
				,sizeof( tmp_buffer )
				,"/sys/class/GobiQMI/qcqmi%d"
				,index
				);

		//存在USB串口设备
		if( cmd_file_exist( tmp_buffer ) )
		{
			snprintf(tmp_buffer
						,sizeof( tmp_buffer )
						,"/dev/qcqmi%d"
						,index
						);
			debug( "\r\nqcqmi device name:%s\r\n",tmp_buffer );
			strcpy( char_device_name,tmp_buffer );

			//检查/dev/qcqmi*对应的主设备号和次设备号
			snprintf(	tmp_buffer
						,sizeof( tmp_buffer )
						,"/sys/class/GobiQMI/qcqmi%d/dev"
						,index
						);

			/*
			[root@mls-centos6-dell dialtool]# cat  /sys/class/GobiQMI/qcqmi1/dev
			246:0
			*/
			if( cmd_file_exist( tmp_buffer ) )
			{
				//读取主设备号和次设备号
				char device_number[ 64 ];
				if( !cmd_cat( tmp_buffer,device_number,sizeof(device_number)-1 ) )
				{
					char* minor_device_string=strstr( device_number,":" );
					*major_num=atoi( device_number );

					if( minor_device_string != NULL )
					{
						*minor_number=atoi( minor_device_string+1 );
						return 0;
					}
				}
			}

			break;
		}
	}

	return -1;
}

//检查是否需要重新创建设备的设备号,1表示需要,否则表示不需要
//char_device_name为设备名称,major_num为主设备号,minor_number为次设备号
int util_should_recreate_dev_file( char* char_device_name,int major_num,int minor_number )
{
	struct stat file_stat;

	//设备文件不存在,需要创建设备文件
	if( !cmd_file_exist( char_device_name ) )
	{
		return TRUE;
	}

	//获取文件的设备号信息
	if( stat( char_device_name,&file_stat ) == -1 )
	{
		return TRUE;
	}

	//检查设备号是否相同
	if( S_ISCHR( file_stat.st_mode ) || S_ISBLK( file_stat.st_mode ) )
	{
		if( major( file_stat.st_rdev ) != major_num )
		{
			return TRUE;
		}

		if( minor( file_stat.st_rdev ) != minor_number )
		{
			return TRUE;
		}
	}

	return FALSE;
}

//获取不支持的网络模式
int util_get_unsupported_network_mode( InfoStruct* info_struct )
{
	unsigned int unsupported_network_mode=( int )strtol(info_struct->TZ_NETWORK_MODE_NOT_SUPPORT, (char **) NULL, 16);

	return unsupported_network_mode;
}

//获取当前模块在网页上不能显示的页面项
unsigned int util_get_unsupported_web_page_features( void )
{
	unsigned int unsupported_web_page_features=0;
/*
	if( is_module_sim7100_used() )
	{
		unsupported_web_page_features=
				WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_LTE_Build_Date
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Physical_Layer_Version
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_High_level_Protocol_Version
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Search_Preferred
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_CS_PS_Preferred
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_PLMN_Mode
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Module_Upgrade
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Network_Mode_GSM_Mode
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Network_Mode_Preferred_Mode
				;
	}
	else if( is_module_mf210_used() )
	{
		unsupported_web_page_features=
				WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_LTE_Build_Date
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Physical_Layer_Version
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_High_level_Protocol_Version
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Search_Preferred
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_CS_PS_Preferred
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Working_Mode
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_PLMN_Mode
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Module_Upgrade
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Log_Catching
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Frequency_Locking
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Network_Service
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_PPTP_VPN
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_ICCID
				;
	}
	else if( is_module_sim5360_used() )
	{
		unsupported_web_page_features=
				WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_LTE_Build_Date
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Physical_Layer_Version
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_High_level_Protocol_Version
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Search_Preferred
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_CS_PS_Preferred
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Working_Mode
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_PLMN_Mode
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Module_Upgrade
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Frequency_Locking
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Network_Service
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_PPTP_VPN
				;
	}
	else if( is_module_mc2716_used() )
	{
		unsupported_web_page_features=
				WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_LTE_Build_Date
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Physical_Layer_Version
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_High_level_Protocol_Version
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Search_Preferred
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_CS_PS_Preferred
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Working_Mode
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_PLMN_Mode
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Module_Upgrade
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Log_Catching
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Frequency_Locking
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Network_Service
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_PPTP_VPN
				;
	}
	else if( is_module_c5300v_used() )
	{
		unsupported_web_page_features=
				WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_LTE_Build_Date
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Physical_Layer_Version
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_High_level_Protocol_Version
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Search_Preferred
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_CS_PS_Preferred
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Working_Mode
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_PLMN_Mode
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Module_Upgrade
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Log_Catching
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Frequency_Locking
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Network_Service
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_PPTP_VPN
				;
	}
	else if( is_module_bm806_used() )
	{
		unsupported_web_page_features=
				WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_LTE_Build_Date
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Physical_Layer_Version
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_High_level_Protocol_Version
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Search_Preferred
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_CS_PS_Preferred
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_PLMN_Mode
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Module_Upgrade
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Log_Catching
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Network_Mode_GSM_Mode
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Network_Mode_Preferred_Mode
				;
	}
	else if( is_module_p500_used() )
	{
		unsupported_web_page_features=
				WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_LTE_Build_Date
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Physical_Layer_Version
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_High_level_Protocol_Version
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Search_Preferred
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_CS_PS_Preferred
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Working_Mode
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Module_Upgrade
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Log_Catching
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Frequency_Locking
				;
	}
	else if( is_module_slm630b_used() || is_module_u8300w_used() )
	{
		unsupported_web_page_features=
				WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_LTE_Build_Date
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Physical_Layer_Version
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_High_level_Protocol_Version
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Search_Preferred
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_CS_PS_Preferred
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Working_Mode
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Module_Upgrade
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Log_Catching
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Frequency_Locking
				;
	}
	else if( is_module_l1761_used() )
	{
		unsupported_web_page_features=
				WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_LTE_Build_Date
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Physical_Layer_Version
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_High_level_Protocol_Version
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Module_Version
				;
	}
	else
	{
		unsupported_web_page_features=0;
	}


	if( !cmd_file_exist( TMP_CONFIG_FILE ) )
	{
		unsupported_web_page_features|=
				WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_LTE_Build_Date
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Physical_Layer_Version
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_High_level_Protocol_Version
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Search_Preferred
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_CS_PS_Preferred
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_PLMN_Mode
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Module_Upgrade
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Log_Catching
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Frequency_Locking
				// |WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Network_Service
				// |WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_PPTP_VPN
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Network_Mode_GSM_Mode
				|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_Network_Mode_Preferred_Mode
				;
	}

	unsupported_web_page_features=unsupported_web_page_features|WEB_PAGE_NOT_SUPPORT_TO_DISPLAY_PPTP_VPN_Mode;
*/
	return unsupported_web_page_features;
}

//获取当前模块在网页上需要显示的页面
unsigned int util_get_supported_web_page_features( void )
{
	unsigned int supported_web_page_features=0;
/*
	//进行文件的同步
	if( is_module_mf210_used() )
	{
		supported_web_page_features=supported_web_page_features
									|WEB_PAGE_SUPPORT_TO_DISPLAY_Dial_NUmber
									;
	}
	else if( is_module_sim5360_used() )
	{
		supported_web_page_features=supported_web_page_features
									|WEB_PAGE_SUPPORT_TO_DISPLAY_Dial_NUmber
									;
	}
	else if( is_module_mc2716_used() )
	{
		supported_web_page_features=supported_web_page_features
									|WEB_PAGE_SUPPORT_TO_DISPLAY_Dial_NUmber
									;
	}
	else if( is_module_c5300v_used() )
	{
		supported_web_page_features=supported_web_page_features
									|WEB_PAGE_SUPPORT_TO_DISPLAY_Dial_NUmber
									;
	}
	else if( is_module_bm806_used() )
	{
		supported_web_page_features=supported_web_page_features
									|WEB_PAGE_SUPPORT_TO_DISPLAY_ROAMING_SETTINGS
									|WEB_PAGE_SUPPORT_TO_VOIP_SETTINGS
									|WEB_PAGE_SUPPORT_WPS_SETTINGS
									|WEB_PAGE_SUPPORT_TO_DISPLAY_LTE_STATUS
									|WEB_PAGE_SUPPORT_TO_DISPLAY_TR069_SETTINGS
									;
	}
	else if( is_module_l1761_used() )
	{
		supported_web_page_features=supported_web_page_features
									|WEB_PAGE_SUPPORT_TO_DISPLAY_LTE_STATUS
									|WEB_PAGE_SUPPORT_TO_LOCK_ONE_CELL
									|WEB_PAGE_SUPPORT_TO_VOIP_SETTINGS
									|WEB_PAGE_SUPPORT_WPS_SETTINGS
									;
	}

	supported_web_page_features=supported_web_page_features
								|WEB_PAGE_SUPPORT_TO_REBOOT_AFTER_N_HOURS
								;
*/
	return supported_web_page_features;
}

//获取不支持的APN验证类型,如果不支持,则不显示对应的选项
//#define APN_AUTH_TYPE_NOT_SUPPORT_NONE (1<<0)
//#define APN_AUTH_TYPE_NOT_SUPPORT_PAP (1<<1)
//#define APN_AUTH_TYPE_NOT_SUPPORT_CHAP (1<<2)
//#define APN_AUTH_TYPE_NOT_SUPPORT_PAP_CHAP (1<<3)
unsigned int util_get_unsupported_apn_auth_type( void )
{
	unsigned int unsupported_apn_auth_type=0;

	if( is_module_l1761_used() )
	{
		unsupported_apn_auth_type=
									APN_AUTH_TYPE_NOT_SUPPORT_PAP_CHAP
									;
	}

	return unsupported_apn_auth_type;
}

//检查当前pptp客户端或服务器是否被开启,为yes表示被开启,否则表示没有被开启
//开启,返回TRUE,否则返回FALSE
int util_is_pptp_function_enabled( InfoStruct* nvram_config_info )
{
	return !strcmp( nvram_config_info->TZ_PPTP_ENABLED,"yes" );
}

//获取当前PPTP功能所处的模式
int util_get_pptp_mode( InfoStruct* nvram_config_info )
{
	return atoi( nvram_config_info->TZ_PPTP_MODE );
}

//pptp是否支持MPPE加密
int util_is_pptp_mppe_enabled( InfoStruct* nvram_config_info )
{
	return !strcmp( nvram_config_info->TZ_PPTP_ENABLE_MPPE,"yes" );
}

//使用连接上的pptp vpn作为默认路由
int util_is_pptp_vpn_used_as_default_route( InfoStruct* nvram_config_info )
{
	return !strcmp( nvram_config_info->TZ_VPN_AS_DEFAULT_ROUTE,"yes" );
}


#ifdef CMDLIB_TEST
//this file is used to test cmdlib

int main(int argc,char* argv[])
{
	char rx_buffer[64];
	char tx_buffer[64];
	Neighbour_Info neigh_info;
	InfoStruct server_wifi_info;
	char rx_bytes_counter[64];
	char tx_bytes_counter[64];
	char rx_packages_counter[64];
	char tx_packages_counter[64];
	const char* network_card_name="ppp0";
	ARPTable system_arp_table;

	{
		char processed_quoted_string[]="\"192.168.1.2\"";
		char processed_quoted_string2[]="rootaccount";
		char processed_quoted_string3[]="\"\\\"cndistgt\"";

		util_strip_quote_string_from_cfg_e(processed_quoted_string);
		printf( "\r\nprocessed_quoted_string:%s!!!!!\r\n",processed_quoted_string );

		util_strip_quote_string_from_cfg_e(processed_quoted_string2);
		printf( "\r\nprocessed_quoted_string2:%s!!!!!\r\n",processed_quoted_string2 );

		util_strip_quote_string_from_cfg_e(processed_quoted_string3);
		printf( "\r\nprocessed_quoted_string3:%s!!!!!\r\n",processed_quoted_string3 );

	}

	{
		char supported_bands_info[ 128 ];
		util_parse_bands_info( "GSM900,GSM1800;TDSCDMA BAND34,BAND39;TDD BAND38,BAND39,BAND40",FALSE,NULL,supported_bands_info );
		util_parse_bands_info( "GSM900,GSM1800;TDSCDMA BAND34,BAND39;TDD BAND38,BAND39,BAND40\r\n",FALSE,NULL,supported_bands_info );
		util_parse_bands_info( "GSM900,GSM1800;TDSCDMA BAND34,BAND39;TDD BAND38,BAND39,BAND40;TDD BAND38,BAND39,BAND40;\r\n",FALSE,NULL,supported_bands_info );
	}

	{
		UsbSerialTable usb_serial_table;

		if( !util_get_available_usb_ports( &usb_serial_table ) )
		{
			int index;
			for( index=0;index < usb_serial_table.count;index++ )
			{
				printf("\r\nutil_get_available_usb_ports:%d,%s\r\n"
						,index
						,usb_serial_table.table[ index ].port_name
						);


			}
		}
	}

	{
		int get_serial_name_ok;
		char serial_port_name[ 64 ];

		get_serial_name_ok=util_get_serial_name_by_usb_interface( 0x05c6,0x9025,1,serial_port_name );
		printf( "\r\nget_serial_name_ok 1:%d,%s\r\n",get_serial_name_ok,serial_port_name );

		get_serial_name_ok=util_get_serial_name_by_usb_interface( 0x05c6,0x9025,2,serial_port_name );
		printf( "\r\nget_serial_name_ok 2:%d,%s\r\n",get_serial_name_ok,serial_port_name );


		get_serial_name_ok=util_get_serial_name_by_usb_interface( 0x05c6,0x9025,3,serial_port_name );
		printf( "\r\nget_serial_name_ok 3:%d,%s\r\n",get_serial_name_ok,serial_port_name );

		get_serial_name_ok=util_get_serial_name_by_usb_interface( 0x05c6,0x9025,4,serial_port_name );
		printf( "\r\nget_serial_name_ok 4:%d,%s\r\n",get_serial_name_ok,serial_port_name );

		get_serial_name_ok=util_get_serial_name_by_usb_interface( 0x05c6,0x9025,5,serial_port_name );
		printf( "\r\nget_serial_name_ok 5:%d,%s\r\n",get_serial_name_ok,serial_port_name );
	}

	{
		char fdd_frequency_list[]="10,1650,75,1750,1800,1551,1651,2850,3375,1675,1625";
		memset( &server_wifi_info,0,sizeof( server_wifi_info ) );
		strcpy( server_wifi_info.TZ_PRESTORE_FREQUENCY_FDD,"123,456,789" );
		util_is_all_fdd_prestore_frequency_in_list( fdd_frequency_list,&server_wifi_info );
	}

	{
		char tdd_frequency_list[]="10,1650,75,1750,1800,1551,1651,2850,3375,1675,1625";
		memset( &server_wifi_info,0,sizeof( server_wifi_info ) );
		strcpy( server_wifi_info.TZ_PRESTORE_FREQUENCY_TDD,"1651,456,10" );
		util_is_all_tdd_prestore_frequency_in_list( tdd_frequency_list,&server_wifi_info );
	}

	//检查指定的UDP端口是否被打开
	{
		int result=util_check_if_one_udp_port_is_open( "120.210.236.221",1701,"ltekd" );
		printf("\nutil_check_if_one_udp_port_is_open:%d\n",result);

		result=util_check_if_one_udp_port_is_open( "120.210.236.3",1701,"ltekd10" );
		printf("\nutil_check_if_one_udp_port_is_open 2:%d\n",result);
	}

	{
		int is_sim_exist;
		int need_pin;
		int need_puk;
		int signal_quality;
		int sms_status;
		int reg_status;
		int greg_status;
		int ereg_status;
		int act;
		int network_data_service;
		util_read_status_ind(
								&is_sim_exist,
								&need_pin,
								&need_puk,
								&signal_quality,
								&sms_status,
								&reg_status,
								&greg_status,
								&ereg_status,
								&act,
								&network_data_service
								);
		printf(
				"%d %d %d %d %d %d %d %d %d %d"
				,is_sim_exist
				,need_pin
				,need_puk
				,signal_quality
				,sms_status
				,reg_status
				,greg_status
				,ereg_status
				,act
				,network_data_service
				);
	}

	{
		char uptime_value[ 128 ];
		uptime_value[ 0 ]=0;
		util_read_system_uptime( uptime_value );
		//将当前运行的时间写入文件中
		cmd_echo( uptime_value,LAST_TIME_CONNECT_TO_SERVER );
	}

	{
		char base64_source[256];
		char base64_dest[256];

		strcpy( base64_source,"a" );
		base64_encode( ( unsigned char* )base64_source,( unsigned char* )base64_dest,strlen( base64_source ) );
		printf( "\n base64_source:%s,base64_dest:%s",base64_source,base64_dest );

		strcpy( base64_source,"ab" );
		base64_encode( ( unsigned char* )base64_source,( unsigned char* )base64_dest,strlen( base64_source ) );
		printf( "\n base64_source:%s,base64_dest:%s",base64_source,base64_dest );

		strcpy( base64_source,"abc" );
		base64_encode( ( unsigned char* )base64_source,( unsigned char* )base64_dest,strlen( base64_source ) );
		printf( "\n base64_source:%s,base64_dest:%s",base64_source,base64_dest );

		strcpy( base64_source,"abcdefghi" );
		base64_encode( ( unsigned char* )base64_source,( unsigned char* )base64_dest,strlen( base64_source ) );
		printf( "\n base64_source:%s,base64_dest:%s",base64_source,base64_dest );

		strcpy( base64_source,"abcdefgh" );
		base64_encode( ( unsigned char* )base64_source,( unsigned char* )base64_dest,strlen( base64_source ) );
		printf( "\n base64_source:%s,base64_dest:%s",base64_source,base64_dest );


		strcpy( base64_source,"abcdefg" );
		base64_encode( ( unsigned char* )base64_source,( unsigned char* )base64_dest,strlen( base64_source ) );
		printf( "\n base64_source:%s,base64_dest:%s",base64_source,base64_dest );

		strcpy( base64_source,"YWJjZGVmZ2hp" );
		printf( "\n base64_source:%s",base64_source );
		base64_decode( ( unsigned char* )base64_source,( unsigned char* )base64_dest,strlen( base64_source ) );
		printf( "\n base64_dest:%s",base64_dest );

		strcpy( base64_source,"YWJjZGVmZ2g=" );
		printf( "\n base64_source:%s",base64_source );
		base64_decode( ( unsigned char* )base64_source,( unsigned char* )base64_dest,strlen( base64_source ) );
		printf( "\n base64_dest:%s",base64_dest );

		strcpy( base64_source,"YWJjZGVmZw==" );
		printf( "\n base64_source:%s",base64_source );
		base64_decode( ( unsigned char* )base64_source,( unsigned char* )base64_dest,strlen( base64_source ) );
		printf( "\n base64_dest:%s\n",base64_dest );
	}

	//读取模块状态
	util_read_module_status( "l1761-network-at-commands",&server_wifi_info );

	{
		system( "touch cmd_chmodx-test-file" );
		cmd_chmodx( "cmd_chmodx-test-file" );

		system( "touch cmd_chmodrw-test-file" );
		cmd_chmodrw( "cmd_chmodrw-test-file" );
	}

	{
		if( !util_get_arp_table_info( &system_arp_table,"" ) )
		{
			int index;
			for( index=0;index<system_arp_table.count;index++ )
			{
				printf(
						"arp mac:%s,ip:%s\r\n"
						,system_arp_table.table[ index ].mac
						,system_arp_table.table[ index ].ip
						);
			}
		}
	}

	{
		if( !util_get_pppoe_table_info( &system_arp_table,"" ) )
		{
			int index;
			for( index=0;index<system_arp_table.count;index++ )
			{
				printf(
						"pppoe mac:%s,id:%s\r\n"
						,system_arp_table.table[ index ].mac
						,system_arp_table.table[ index ].ip
						);
			}
		}
	}

	util_read_version_info( VERSION_INFO_FILE,&server_wifi_info );

	util_write_exception_log("cmdlib","reboot the system!");

	cmd_netdev_flowstat_ex(network_card_name,rx_bytes_counter,tx_bytes_counter,rx_packages_counter,tx_packages_counter);
	printf( "\nnetwork_card_name:%s\n",network_card_name );
	printf( "\nrx_bytes_counter:%s\n",rx_bytes_counter );
	printf( "\nrx_packages_counter:%s\n",rx_packages_counter );
	printf( "\ntx_bytes_counter:%s\n",tx_bytes_counter );
	printf( "\ntx_packages_counter:%s\n",tx_packages_counter );

	{
		char buffer[256];
		char* args[18];
		int args_count;
		int index;

		STRCPY_S( buffer,"  eth0: 96777365  234316    0   30    0     0          0      5695 16918910  129091    0    0    0     0       0          0" );
		printf( "\nbuffer:%s",buffer );

		args_count=util_get_all_args( buffer,' ','\t',args,18 );

		index=0;

		while( index < args_count )
		{
			printf(
					"\nindex:%d==>%s"
					,index
					,args[ index ]
					);
			index+=1;
		}
	}


	{
		BigNum rx_bytes_counter,tx_bytes_counter,usb0_rx_bytes_counter,usb0_tx_bytes_counter;
		BigNum rx_diff_counter,tx_diff_counter;
		char rx_bytes_buffer[64];
		char tx_bytes_buffer[64];

		if( !cmd_netdev_flowstat("usb0",rx_bytes_buffer,tx_bytes_buffer) )
		{
			usb0_rx_bytes_counter.high=100;
			usb0_rx_bytes_counter.low=100;
			usb0_tx_bytes_counter.high=100;
			usb0_tx_bytes_counter.low=100;

			util_decimal_number_convert(rx_bytes_buffer,&rx_bytes_counter);
			util_decimal_number_convert(tx_bytes_buffer,&tx_bytes_counter);
			if( util_big_number_compare( &rx_bytes_counter,&usb0_rx_bytes_counter ) >= 0 )
			{
				util_big_number_sub(&rx_bytes_counter,&usb0_rx_bytes_counter,&rx_diff_counter);
			}
			else
			{
				util_big_number_sub(&usb0_rx_bytes_counter,&rx_bytes_counter,&rx_diff_counter);
			}


			if( util_big_number_compare( &tx_bytes_counter,&usb0_tx_bytes_counter ) >= 0 )
			{
				util_big_number_sub(&tx_bytes_counter,&usb0_tx_bytes_counter,&tx_diff_counter);
			}
			else
			{
				util_big_number_sub(&usb0_tx_bytes_counter,&tx_bytes_counter,&tx_diff_counter);
			}

			if( ( tx_diff_counter.low >= 4000 )
				&&( ( !rx_diff_counter.high ) && ( !rx_diff_counter.low ) )
				)
			{
				;
			}


			memcpy( &usb0_rx_bytes_counter,&rx_bytes_counter,sizeof( BigNum ) );
			memcpy( &usb0_tx_bytes_counter,&tx_bytes_counter,sizeof( BigNum ) );
		}

		return 0;
	}

	//test cmd echo
	cmd_echo("hello world","./good");
	//test cmd touch
	cmd_touch("./byebye");

	//test copy function
	cmd_cp("./good","./good2");

	printf( "cmd_netdev_exist:%d\n",cmd_netdev_exist("eth0") );
	printf( "cmd_netdev_exist:%d\n",cmd_netdev_exist("usb0") );

	if( !cmd_netdev_flowstat("eth0",rx_buffer,tx_buffer) )
	{
		printf("rx_buffer:%s,tx_buffer:%s",rx_buffer,tx_buffer);
	}

	printf("cmd_ip_exist:%d",cmd_ip_exist("virbr0") );

	printf("cmd_ps_is_one_program_running:%d\n",cmd_ps_is_one_program_running( "dnsmasq" ));

	util_resolv_domain_name("www.baidu.com",NULL,NULL,NULL);

	util_is_getting_date_and_time_ok();

	system("ip -6 neigh show dev eth0 >./ipv6_nrigh_test 2>/dev/null");
	if( !util_parse_neigh_info("./ipv6_nrigh_test",&neigh_info,FALSE,FALSE) )
	{
		int index;

		for( index=0;index<neigh_info.neighbour_count;index++ )
		{
			printf("[FALSE,FALSE]:%s %s %d %d\n"
					,neigh_info.neighbours[ index ].ipv6_addr
					,neigh_info.neighbours[ index ].mac_addr
					,neigh_info.neighbours[ index ].is_router
					,neigh_info.neighbours[ index ].status
					);
		}
	}
	if( !util_parse_neigh_info("./ipv6_nrigh_test",&neigh_info,TRUE,FALSE) )
	{
		int index;

		for( index=0;index<neigh_info.neighbour_count;index++ )
		{
			printf("[TRUE,FALSE]:%s %s %d %d\n"
					,neigh_info.neighbours[ index ].ipv6_addr
					,neigh_info.neighbours[ index ].mac_addr
					,neigh_info.neighbours[ index ].is_router
					,neigh_info.neighbours[ index ].status
					);
		}
	}
	if( !util_parse_neigh_info("./ipv6_nrigh_test",&neigh_info,FALSE,TRUE) )
	{
		int index;

		for( index=0;index<neigh_info.neighbour_count;index++ )
		{
			printf("[FALSE,TRUE]:%s %s %d %d\n"
					,neigh_info.neighbours[ index ].ipv6_addr
					,neigh_info.neighbours[ index ].mac_addr
					,neigh_info.neighbours[ index ].is_router
					,neigh_info.neighbours[ index ].status
					);
		}
	}

	{
		struct in6_addr local_addr6;
		local_addr6.s6_addr[0]=0x20;
		local_addr6.s6_addr[1]=0x01;
		local_addr6.s6_addr[2]=0x0d;
		local_addr6.s6_addr[3]=0xb8;
		local_addr6.s6_addr[4]=0x11;
		local_addr6.s6_addr[5]=0x22;
		local_addr6.s6_addr[6]=0x33;
		local_addr6.s6_addr[7]=0x44;
		local_addr6.s6_addr[8]=0x55;
		local_addr6.s6_addr[9]=0x66;
		local_addr6.s6_addr[10]=0x77;
		local_addr6.s6_addr[11]=0x88;
		local_addr6.s6_addr[12]=0x99;
		local_addr6.s6_addr[13]=0xAA;
		local_addr6.s6_addr[14]=0xBB;
		local_addr6.s6_addr[15]=0xCC;
		util_write_interface_id( &local_addr6,"eth0" );
		util_read_interface_id( &local_addr6 );
	}

	{
		unsigned char frame[]={0xff,0xff,0xff,0xff,0xff,0xff,0xb8,0xac,0x6f,0xbb,0x7e,0x3f,0x98,0x86,0x00,0x04,0x01,0x00,0xbf,0xd2};
		//check if the checksum is ok
		int data_len=ntohs( *( unsigned short* )(frame+14) );
		int checksum=ip_checksum(frame,data_len+6+6+2+2);
		//invalid checksum
		if( checksum )
		{
			return -1;
		}

	}

	printf( "random result is:%08x",util_clac_random_response( 0x63f98898 ) );


	//检查usb设备是否存在
	{
		check_if_usb_device_exist( matched_usb_pairs,USB_PAIRS_COUNT,NULL );
	}

	return 0;
}

#endif
