#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
   文件名称:  message.c
   功能:      消息处理函数头文件
   编写人：   简亮                                    时间：2006年4月
------------------------------------------------------------------------------*/


#ifndef _MESSGAE_H_
#define _MESSAGE_H_

#ifdef MESSAGE_GLOBALS
#define MESSAGE_EXT
#else
#define MESSAGE_EXT extern
#endif


//TMESSAGE 结构
// msg 消息基本数据，发送消息必须有值
// msg_ext 消息附加数据，对特别的消息可能带有附加数据时使用
// msg_ptr 消息附加指针，和MSG_EXt类似，当附加数据为指针类型时使用
#define MAX_MESSAGES_SIZE 128
typedef struct
{
  INT32U msg;
  INT32U msg_ext;
  INT32U msg_ext2;
  void *ptr;
}TMessage;

MESSAGE_EXT void putMsg_detail(TMessage msg);
MESSAGE_EXT BOOLEAN getMsg(TMessage *m);
MESSAGE_EXT void putMsg(INT32U msg_detail);
MESSAGE_EXT void clearMsg(void);
MESSAGE_EXT void message_proc(void);


//**************BEGIN KEY DEFINE ********************
#define KEY_NULL 0

#define KEY_1   '1'
#define KEY_2   '2'
#define KEY_3   '3'
#define KEY_4   '4'
#define KEY_5   '5'
#define KEY_6   '6'
#define KEY_7   '7'
#define KEY_8   '8'
#define KEY_9   '9'
#define KEY_0   '0'

#define KEY_XING   '*'
#define KEY_JING   '#'

#define KEY_JIA     '+'
#define KEY_DOT '.'

#define KEY_SPACE ' '

#define KEY_QUIT    		129
#define KEY_DEL      		130
#define KEY_UP        		131
#define KEY_DOWN   			132
#define KEY_LEFT            133
#define KEY_RIGHT           134

#define KEY_HOOKOFF       	135
#define KEY_HOOKON         	136
#define KEY_HOOKOFF_HOST    137
#define KEY_HOOKON_HOST     138
#define KEY_HOOKFLASH    	139
#define KEY_FREEHAND      	140

#define KEY_MENU   			141
#define KEY_CONFIRM        	141
#define KEY_CHANGE 			143 	//effectively it is KEY EDIT

#ifdef CENTER_KEY_FUNC
#define KEY_CENTER        KEY_CHANGE
#endif

#define KEY_HOLDED         	144//使用putMsgEx发送的消息

#define KEY_CALL_RECORD     145
#define KEY_GSM_RING     	146
#define KEY_GSM_RING_STOP 	147

#define KEY_SMS_RING        148

#define KEY_MUTE 			149
#define KEY_VOLUP 			150
#define KEY_VOLDOWN 		151

#define KEY_START           152
#define KEY_REDIAL 			153

#define KEY_HOME 			154
#define KEY_CONTACT			155
#define KEY_SET				156

#define KEY_SHORTCUT_1		157
#define KEY_SHORTCUT_2		158
#define KEY_SHORTCUT_3		159
#define KEY_SHORTCUT_4		160
#define KEY_SHORTCUT_5		161
#define KEY_SHORTCUT_6		162
#define KEY_SHORTCUT_7		163
#define KEY_SHORTCUT_8		164


//**************END KEY DEFINE ********************

//#define MSG_CALL_CONNECTED    170
//#define MSG_CALL_DISCONNECTED 171
//#define MSG_NO_ANSWER         172
//#define MSG_UART_OK           173
//#define MSG_UART_ERROR        174 
#define MSG_HOOK_ON           175
#define MSG_HOOK_OFF          176
#define MSG_UART_UNKNOW_STR     177
#define MSG_UART_DIAL_NORUIM   178

//#define MSG_GET_MODEN_ST_OK    179
//#define MSG_SEND_SMS_START    180
//#define MSG_SEND_SMS_OK       181
//#define MSG_SEND_SMS_FAIL     182
//#define MSG_DIALOG_CLOSE_OK     183
//#define MSG_DIALOG_CLOSE_CANCLE 184
#define MSG_PINYIN_INPUT_END    185
#define MSG_SIGN_INPUT_OK       186
#define MSG_SIGN_INPUT_CANCEL   187      //?
#define MSG_SELECT_YES          188
#define MSG_SELECT_NO	          189      //?
//#define MSG_FIND_PHONE_START    190
//#define MSG_FIND_PHONE_OK       191
//#define MSG_BENQ_INIT_END       193
//#define MSG_SMS_LIST_RESPOND    194
//#define MSG_SMS_READ_RESPOND    195
#define MSG_MYEDIT_OK         196
#define MSG_MYEDIT_CANCLE     197
#define MSG_ACCEPT_DTMF       198
//#define MSG_CHECK_PIN_RESPOND 198 
//#define MSG_HLOCK_OK          199
//#define MSG_HLOCK_FAIL        200
#define MSG_UPDATE_CONTROL    201   //基本上属于空消息,发送的频率很大，一般不应该响应
//#define MSG_1S                202
#define MSG_EXACT_1S          203
#define MSG_CSD_HAVE_CMD      204
#define MSG_EPH1200_REC_PACKET 205
#define MSG_TRANSLATE_IS_ENABLE    206
#define MSG_TRANSLATE_IS_DISABLE   207
#define MSG_10S                    208
#define MSG_1MIN                   209
#define KEY_KEY_SOUND_SET_CHANGED  210
#define KEY_SMS_PRIOD_CHANGED      211
#define MSG_INPUT_NEW_PASSWORD_OK  212
#define MSG_EPH1200_INKING         213
#define MSG_EPH1200_RECOGNIZED      214
#define MSG_PEN_INPUT_OK             215
#define MSG_PIN_INPUT_SELECT   216
#define KEY_HOOKON_CURRENT         217
#define MSG_GROUP_SELECTED  218
#define MSG_PHONE_LIST_SELECTED    219
#define MSG_EXEC_AT_CMD_OVER        220
#define MSG_CPI_CHANGED             221
#define KEY_GSM_DATA_RING           222  
#define MSG_TIMER_DELAY_ONCE_TIMEOUT  223
#define MSG_CUSD_RESPOND              224 
#define MSG_CALL_BERESERVED           225
#define MSG_CALL_BEACTIVED            226
#define MSG_CALL_IN_MEETING           227
#define KEY_MEETING                   228
#define MSG_CURSOR_FLASH              229
#define MSG_CSD_SHALL_OVER            230
#define MSG_ANTI_LOCK_SIM_OK          231
#define MSG_ANTI_LOCK_SIM_FAIL        232
#define MSG_BIHUA_INPUT_OK            233
#define MSG_KEY_UP                    234
#define MSG_SMS_SET_CHANGED           235
#define MSG_LONG_DISTANCE_CHANGED     236
#define MSG_ALARM_CYCLE_MENU_SET      237       
#define MSG_ALARM_SET_MAY_CHANGED     238
#define MSG_SERVICE_DIAL_RESPOND      239

#define MSG_STK_ITEM     240
#define MSG_STK_MENU     241

#define MSG_CALL_BARRING_ENABLED 242
#define MSG_CALL_BARRING_DISABLED 243
#define MSG_DIVERT_CALL_IN        244
#define MSG_PHONE_LOCK_SET_CHANGED        245
#define MSG_VOICE_FEE_SET_CHANGED        246
#define MSG_500MS        247
#define MSG_HOOK_ON_WAIT_SET_CHANGED        248
#define MSG_CHANGE_OK        249

#define MSG_KEY_QUIT 250

#define KEY_EXIT      255


#define MSG_LIST_PRINT 179
#define MSG_CHROMA_SET_CHANGED 180

#define MSG_PHONE_CID_SET_CHANGED        181
#define MSG_PAY_INFO_OK        182
#define MSG_PAY_INFO_CANCAL        183
#define MSG_USSD        184

#define MSG_PAY_TYPE_CHANGED        190
#define MSG_CREG_SEARCH        191
#define MSG_UNUSUAL_SIM_PASS_TYPE_CHANGED        192
#define MSG_AUTO_PRINT_SET_CHANGED        193

#define MSG_1S                256
#define MSG_GPRS_DATA         257
#define MSG_GPRS_CLOSE        258
#define MSG_ELECTRICITY_LOGIN        259
#define MSG_ELECTRICITY_USER_ID        260
#define MSG_ELECTRICITY_CHARGE        261
#define MSG_ELECTRICITY_INVOICE        262
#define MSG_ELECTRICITY_FLOW        263
#define MSG_PRINT_FINISH        264
#define MSG_ELECTRICITY_BACKOUT_CHARGE        265
#define MSG_ELECTRICITY_CANCEL_INVOICE        266
#define MSG_ELECTRICITY_DETAIL_ACCOUNT        267
#define MSG_ELECTRICITY_MONEY_SUMMARY        268
#define MSG_ELECTRICITY_QUERY_FLOW        269
#define MSG_ELECTRICITY_MODIFY_PASSWORD        270
#define MSG_ELECTRICITY_INQUIRE_USER_ID        271
#define MSG_ELECTRICITY_INQUIRE_USER_NAME        272

#define MSG_XFS3011_RIGHT_FRAME      273
#define MSG_XFS3011_ERROR_FRAME      274
#define MSG_XFS3011_RUN_OVER      275
#define MSG_READ_SMS_TEXT 276

#define MSG_MMS_startResponse 277
#define MSG_MMS_terminateResponse 278
#define MSG_MMS_notificationReceived 279
#define MSG_MMS_httpSessionStart 280
#define MSG_MMS_retrieveMessageResponse 281
#define MSG_MMS_getMessageResponse 282
#define MSG_MMS_getBodyPartResponse 283
#define MSG_MMS_httpSessionStop 284

#define MSG_IC_SIM_RECEIVED_9F 285
#define MSG_IC_SIM_RECEIVED_90 286
#define MSG_IC_SIM_RECEIVED_ERROR 287

#define MSG_CMS_ERROR_RECEIVED 288
#define MSG_CMGS_RECEIVED 289

#define KEY_IC_CARD_OUTSIDE 290
#define KEY_IC_CARD_INSERT 291

#define MSG_SIM_CARD_RIGHT 292
#define MSG_SIM_CARD_WRONG 293

#define MSG_KEY_LONG_PRESS 294


#define MSG_HTTP_GPRS_RECV_ERROR		295
#define MSG_HTTP_GPRS_RECV_NEW_URL		296
#define MSG_HTTP_GPRS_CONNECTED			297
#define MSG_HTTP_GPRS_RECV_PART_DATA	298
#define MSG_HTTP_GPRS_RECV_END			299
#define MSG_HTTP_GPRS_REQUEST_NEXT_PART	300

#define MSG_STK_QUIT	301
#define MSG_SET_STK_OK	302
#define MSG_GROUP_CHANGE_SELECTED	303
#define MSG_GROUP_CHANGE_FINISH	304
#define MSG_GROUP_NULL	305
#define MSG_SIM_CARD_DTM_FINISH	306
#define MSG_REC_IRDA_DMT	307

#define MSG_IC_SIM_ERROR 308
#define MSG_IC_SIM_OK 309
#define MSG_APDU_RESET_OK 310

#define MSG_WATCHDATA_SIM_OTA_OK 311
#define MSG_WATCHDATA_SIM_OTA_ERROR 312

#define MSG_STRING       313

#define MSG_GPRS_CONNECTED 			314
#define MSG_GPRS_HTTP_DATA_RECV 	315
#define MSG_GPRS_HTTP_DATA_OVERFLOW 316

#define MSG_IDENTITY_CARD_OK			319
#define MSG_IDENTITY_CARD_ERROR		320

#define MSG_HTTP_GPRS_SEND_ERROR		321
#define MSG_UPLOAD_YES          322
#define MSG_UPLOAD_NO	        323
#define MSG_UPDATE_PROC    		324  

//#define MSG_UPDATE_TIME         326

typedef enum{
	MSG_VOIP_RESPOND = 325,
	MSG_UPDATE_TIME,
	MSG_CALLWAIT_NEW_INCOME,
	MSG_CALLWAIT_CPI_CHANGE,
	MSG_CALLWAIT_CALL_OUT_REJECTED,
	MSG_CALLWAIT_CALL_OUT_CONNECTED,
	MSG_CALLWAIT_FOURTH_INCOME,
	MSG_DCPI,
	MSG_UNKNOWN_ERR,
	MSG_250MS,
	MSG_IILS,				// 335
	MSG_SLIC_TONE_START,	// format: continuous_en() on(ms) off(ms)
	MSG_SLIC_TONE_STOP,
	MSG_SLIC_RING_START,	
	MSG_SLIC_RING_STOP,
	MSG_SLIC_INCOMING_CALLWAIT_TONE,	
	MSG_SLIC_REVERSE_ACTIVE,
	MSG_SLIC_TZPHONE_IDLE,
//	MSG_SLIC_SET_AUDIO_GAIN, // format: rx, tx ( -30 ~ 9 )
	MSG_MAX
}eEXT_MSG;

#define send_hookon_msg() do{volte_dbg_out("putMsg hookon at %s %d\n", __FILE__, __LINE__);putMsg(KEY_HOOKON);}while(0)
typedef enum
{
	UPDATE_FOR_REMOTE = 0,
	UPDATE_FOR_SYSTEM,
	UPDATE_FOR_OTHER_END
}TZ_UPDATE_CONFIG;
#define TZ_UPDATE_SYSTEM_CONFIG "/amt/tzupdate.zip"

extern char lmi40_apn_ip[16];

MESSAGE_EXT BOOLEAN is_keypad(INT32U msg);
MESSAGE_EXT void StartSMSRun(void);
MESSAGE_EXT void StopSMSRun(void);
MESSAGE_EXT int CheckUserFileModified(void);
MESSAGE_EXT void get_lmi40_apnip(void);
#endif



#ifdef __cplusplus
}
#endif
