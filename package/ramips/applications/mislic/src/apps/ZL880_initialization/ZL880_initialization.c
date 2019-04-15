/** \file ZL880_initialisation.c
 * ZL880_initialisation.c
 *
 * This file is a Quick-Start application for a ZL880 ABS or Tracker device.
 *
 * This Quick-Start is tested on the Microsemi ZTAP host. The quick start can
 * be eaisly modifed to run on a customers platform if need be. The main purpose
 * of this Quick-Start is to provide an example of how to:
 *
 *  - Make device and line objects
 *  - Initialize all devices and lines
 *  - Change the line's feed state
 *  - Perform line calibration
 *  - Detect/Respond to line hook events
 *
 * Copyright (c) 2012, Microsemi Corporation.
 */

/*
 * These headers are for ZTAP/Linux purposes only.  The user will not find them
 * in the API
 */
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
/*
 * These files are required by all applications.  "vp_api.h" includes all of the
 * API libraries required by the application.  "profiles.h" includes the
 * profiles used by the application.  At a minimum, there must be a Device
 * Profile for the device to initialize.
 */
#include "vp_api.h"

/* Pick the supply type: */
#define FLY_BACK_SUPPLY         0 /* Fly Back */
#define INVERTING_BOOST_SUPPLY  1 /* Inverting Boost */
#define FBABS_SUPPLY            2 /* Fly Back ABS */
#define BBABS_SUPPLY            3 /* Buck Boost ABS */
#define BUCK_BOOST_SUPPLY       4 /* Buck Boost */

#define SUPPLY_CONFIGURATION BUCK_BOOST_SUPPLY

#if (SUPPLY_CONFIGURATION == FLY_BACK_SUPPLY)
    #include "FLY_BACK_profiles.h"

#elif (SUPPLY_CONFIGURATION == INVERTING_BOOST_SUPPLY) 
    #include "INVERTING_BOOST_profiles.h"

#elif (SUPPLY_CONFIGURATION == FBABS_SUPPLY)
    #include "FBABS_profiles.h"
    
#elif (SUPPLY_CONFIGURATION == BBABS_SUPPLY)
    #include "BBABS_profiles.h"
    
#elif (SUPPLY_CONFIGURATION == BUCK_BOOST_SUPPLY)
    #include "BUCK_BOOST_profiles.h"
    #include "ZLR964122_SM2_LV_BB_profiles.h"
#else 
    #error "SUPPLY_CONFIGURATION is incorrectly set"
#endif


/* ZTAP specific initialization header */
#include "sdk_qs_board.h"

/* Common utility functions */
#include "sdk_qs_utils.h"

/* ZTAP specific values for device ID depending on the MPI Mode */
#if 0	// by lizd
#define MPI_3WIRE       0x0000 /* 3 wire SPI w/ 2.5us interbyte chipselect off-time */
#define MPI_4WIRE       0x0100 /* 4 wire SPI w/ 2.5us interbyte chipselect off-time */
#define SPI8_3WIRE      0x0200 /* 3 wire SPI w/ one clock chipselect off-time */
#define SPI8_4WIRE      0x0400 /* 4 wire SPI w/ one clock chipselect off-time */
#define SPI8_3WIRE_CFM  0x0300 /* 3 wire SPI w/o chipselect off-time per VpMpiCmd() transation */
#define SPI8_4WIRE_CFM  0x0500 /* 4 wire SPI w/o chipselect off-time per VpMpiCmd() transation */
#define ZSI_4WIRE       0x0E00 /* 4 wire ZSI transation*/
#endif

#define SM              0x0010
#define DIN             0x0000

/* Number of lines supported by the quick start */
#define NUM_LINES       1

/* Internal Helper functions */
static void alarmHandle(int val);
static void InitSetMasks(void);

void init_tz_env_when_get_cal_cmp(void);
extern int write_file_data(char *filepath, char *data, int data_size, char *attr);
extern int read_file_data(char *filename, char *out_buf, int buf_len);
static int encode_cid_format (const char *phone_num, char *out_buff);
void check_dump_mode(void);



/*
 * Application memory space for the API required Device/Line contexts and
 * objects. This application uses the simplest memory scheme -- global, but the
 * API will work with any memory management scheme.
 */

VpDevCtxType devCtx;
VpLineCtxType lineCtx[NUM_LINES];
Vp886DeviceObjectType devObj;
Vp886LineObjectType lineObj[NUM_LINES];
VpStatusType status;

/*
 * Create a deviceId. Generally this is just the chipselect number of the device on the SPI bus.
 * However, this is a customer defined type in the vp_api_types.h file and can be whatever
 * the customer chooses.
 *
 * The following example is for the Microsemi ZTAP platform and combines the
 * location of the SLAC on the platform and the SPI interface type used by the SLAC.
 */
VpDeviceIdType deviceId = DIN; //(DIN | MPI_4WIRE);	by lizd
//-------------------------------------------------------------
//-- C_DEMO        set cad   on/off  ontime, offtime and cycle
//-- set           on/off    C_DEMO[9]/C_DEMO[13]   bit[0] set 1 on, set 0 off  source volume A 

//-- set           ontime    C_DEMO[10] if    ontime*0.2 >255  get (ontime*0.2/256) hight  bit[3:0]
//-- set           ontime    C_DEMO[11] else  ontime*0.2       get (ontime*0.2%256) low    bit[7:0]

//-- set           offtime   C_DEMO[14] if    offtime*0.2 >255 get (ontime*0.2/256) hight  bit[3:0]
//-- set           offtime   C_DEMO[15] else  offtime*0.2      get (ontime*0.2%256) low    bit[7:0]

//-- set           timeonce  C_DEMO[17] 
//-- set           timeonce  C_DEMO[17] if(timeonce==0) cycle  else cycle timeonce

//-- T_US_SIGNEL   set tone  Freq,   and volume db
//  set Freq      Freq*2.75  T_US_SIGNEL[10] set Hight byte
//  set Freq      Freq*2.75  T_US_SIGNEL[11] set low   byte 
//  set volume db set 16db   T_US_SIGNEL[12] set Hight byte
//  set volume db set 16db   T_US_SIGNEL[13] set low   byte 



static unsigned char Tone_Buff[19]={0};
static unsigned char Cad_Buff[18]={0};


VpStatusType 
VpSetlineToneMix(
//VpLineCtxType *pLineCtx,
uint16		   Freq    ,
uint16 		   timeonce,
uint16 		   ontime  ,
uint16         offtime)
{

    VpEventType event;
    //uint8 i;
	uint16 data= (Freq*275)/100;   // Freq *2.75
    VpStatusType status;
	memcpy(Tone_Buff, T_US_SIGNEL, 19);
    Tone_Buff[10]=data/256;
    Tone_Buff[11]=data%256;
    //Tone_Buff[12]=0x0e;   // -16db
    //for(i=0;i<19;i++)
    // printf("Tone_Buff=%x\n", Tone_Buff[i]);
	memcpy(Cad_Buff, C_DEMO, 18);
   
    if((ontime==0)&&(offtime=!0))
		Cad_Buff[9]=0x80;
	else if((ontime!=0)&&(offtime==0))
		Cad_Buff[13]=0x81;

	
    Cad_Buff[10]= (ontime/5)>255?(0x20+(ontime/5)/256):0x20;
    Cad_Buff[11]= (ontime/5)%256;

    Cad_Buff[14]= (offtime/5)>255?(0x20+(ontime/5)/256):0x20;
    Cad_Buff[15]= (offtime/5)%256;
    Cad_Buff[17]=timeonce;
	
    //for(i=0;i<18;i++)
    // printf("Cad_Buff[%d]=%x\n", i,Cad_Buff[i]);

	
	VpGetEvent(&devCtx, &event);
	status=VpSetLineTone(event.pLineCtx,Tone_Buff,Cad_Buff,VP_NULL);
	//VpSetLineTone(event.pLineCtx,T_US_SIGNEL,C_CALL,VP_NULL);
    return status;

}




/**
 * Function:  main()
 *
 * Description: This function initializes the API and Devices/Lines.
 * It also starts the "alarmHandle" function to be called at a periodic rate.
 */

#define TM_SYS_INTERVAL 				10  					// 10MS
#define SYS_TICK_OF_1S 					(1000/TM_SYS_INTERVAL)
#define MAX_CHECK_FEEDBACK_INTERVAL 	(SYS_TICK_OF_1S*5)		// 5S	

#define TAG_GET_LOOP_COND_RESOULT 0x8358

extern int tzVp886RegisterDumpInt(void);

int main(void)
{
    struct itimerval timeStruct, timeOldVal;
    uint8 lineNum = 0;

    tz_dbg(LOG_LEVEL_VP_INFO, "Ver %x.%02x.%02x(0x%02X) Built at %s %s",
    						VP_API_VERSION_MAJOR_NUM,
    						VP_API_VERSION_MINOR_NUM,
    						VP_API_VERSION_MINI_NUM,
    						VP_API_VERSION_TAG,
    						__DATE__, __TIME__);
    tz_dbg(LOG_LEVEL_VP_INFO, "Sys Tick Per Second is %d, Check Tick is %d", 
    						SYS_TICK_OF_1S, 
    						MAX_CHECK_FEEDBACK_INTERVAL);

	/*QS_DEBUG("-------------- init dumping ---------------\n");
    tzVp886RegisterDumpInt();						
	QS_DEBUG("-------------- dump end ---------------\n"); */
	
    /* ZTAP board initialization (platform specific) */
    BoardInit(); /* See "../common/board.c" */
    VpSysInit();

    /*
     * The following structure is specific to Linux.  It sets up a timed call
     * to a user specified function.  For the purpose of this demo, the timing
     * is 20mS and the function that will be called is "alarmHandle".  The timer
     * and function call does not start until "setitimer()" and "signal()" are
     * called (below).
     */
    timeStruct.it_interval.tv_sec = 0L;
    timeStruct.it_interval.tv_usec = TM_SYS_INTERVAL*1000L;	// 10MS
    timeStruct.it_value.tv_sec = 0L;
    timeStruct.it_value.tv_usec = TM_SYS_INTERVAL*1000L;

    timeOldVal.it_interval.tv_sec = 0L;
    timeOldVal.it_interval.tv_usec = 0L;
    timeOldVal.it_value.tv_sec = 0L;
    timeOldVal.it_value.tv_usec = 0L;

    /* Make the objects (1 device, 2 lines) */
#if (SUPPLY_CONFIGURATION == FBABS_SUPPLY) || (SUPPLY_CONFIGURATION == BBABS_SUPPLY)
    status = VpMakeDeviceObject(VP_DEV_886_SERIES, deviceId, &devCtx, &devObj);
#else
    status = VpMakeDeviceObject(VP_DEV_887_SERIES, deviceId, &devCtx, &devObj);
#endif
    if (status != VP_STATUS_SUCCESS) {
        QS_DEBUG("Error making the device object: %s\n", MapStatus(status));
        exit(-1);
    }

    for (; lineNum < NUM_LINES; lineNum++) {
        status = VpMakeLineObject(VP_TERM_FXS_LOW_PWR, lineNum, &lineCtx[lineNum], &lineObj[lineNum], &devCtx);
        if (status != VP_STATUS_SUCCESS) {
            QS_DEBUG("Error calling VpMakeLineObject on lineNum %i: %s\n", lineNum, MapStatus(status));
            exit(-1);
        }
        QS_DEBUG("--- Created lineNum %i---\n", lineNum);
    }

    /* Start VpInitDevice and wait for the event VP_DEV_EVID_DEV_INIT_CMP */
#if (SUPPLY_CONFIGURATION == FLY_BACK_SUPPLY)
    status = VpInitDevice(&devCtx, FB_DEV_PROFILE, FB_AC_600_FXS, FB_DC_25MA_CC, FB_RING_25HZ_SINE, VP_PTABLE_NULL, VP_PTABLE_NULL);
#elif (SUPPLY_CONFIGURATION == INVERTING_BOOST_SUPPLY) 
    status = VpInitDevice(&devCtx, IB_DEV_PROFILE, IB_AC_600_FXS, IB_DC_25MA_CC, IB_RING_25HZ_SINE, VP_PTABLE_NULL, VP_PTABLE_NULL);
#elif (SUPPLY_CONFIGURATION == FBABS_SUPPLY)
    status = VpInitDevice(&devCtx, FBABS_DEV_PROFILE, FBABS_AC_600_FXS, FBABS_DC_23MA_CC, FBABS_RING_25HZ_SINE, VP_PTABLE_NULL, VP_PTABLE_NULL);
#elif (SUPPLY_CONFIGURATION == BBABS_SUPPLY)
    status = VpInitDevice(&devCtx, BBABS_DEV_PROFILE, BBABS_AC_600_FXS, BBABS_DC_25MA_CC, BBABS_RING_25HZ_SINE, VP_PTABLE_NULL, VP_PTABLE_NULL);
#elif (SUPPLY_CONFIGURATION == BUCK_BOOST_SUPPLY)
    //status = VpInitDevice(&devCtx, BB_DEV_PROFILE, BB_AC_600_FXS, BB_DC_25MA_CC, BB_RING_25HZ_SINE, VP_PTABLE_NULL, VP_PTABLE_NULL);
	status = VpInitDevice(&devCtx, DEV_PROFILE_Buck_Boost, AC_FXS_RF14_600R_DEF, DC_FXS_miSLIC_BB_CN, RING_miSLIC_BB_China, VP_PTABLE_NULL, VP_PTABLE_NULL);
#else 
    #error "SUPPLY_CONFIGURATION is incorrectly set"
#endif

    if (status != VP_STATUS_SUCCESS) {
        QS_DEBUG("Device not properly initialized: %s\n", MapStatus(status));
        exit(-1);
    } else {
        QS_DEBUG("\nDevice Initialization started\n");
    }
	
    /*
     * The following two function calls were described above as Linux specific.
     * They are used to call a user defined function at set increments.  The
     * user may have several other methods to perform the same.
     */
    //signal(SIGVTALRM, alarmHandle);
    //setitimer(ITIMER_VIRTUAL, &timeStruct, &timeOldVal);

    /*
     * Once the "tick" is setup, the main program can enter a "while(1)" loop
     * and the user specified function will take over the application (with the
     * help of the Linux OS)
     */
	
    //while(TRUE);

    sleep(1);
    
    while(1)
    {
		alarmHandle(26);
		msleep_ext(TM_SYS_INTERVAL);

		check_dump_mode();
    }
    
    return 0;
}

static int hookoff_flag = 0;


void try_to_do_check_reset_work(void)
{
	int rt;
	int cnt;
	char msg_buff[200];
	
	static int rst_tickCounter = 0;
	static int rst_secondCounter = 0;

	const char *filepath_start_check_rst = "/data/slic.start.check.rst.flag";
	const char *filepath_stop_check_rst = "/tmp/log.slic.stop.check.rst";
	
	const char *filepath_reset_result = "/data/slic/rst.result";
	const char *filepath_reset_prefix = "/data/slic/rst";

	if(access(filepath_start_check_rst,F_OK)<0)
		return ;

	if(access(filepath_stop_check_rst, F_OK)>=0)
		return ;

	if(access("/data/slic/", F_OK)<0)
		system("mkdir /data/slic/ && chmod 777 /data/slic/");

	rst_tickCounter++;
	if(rst_tickCounter<SYS_TICK_OF_1S)
		return ;
	
	rst_tickCounter = 0;
	//QS_DEBUG("rst_secondCounter=%d\n", rst_secondCounter);
	
	rst_secondCounter++;
	if(rst_secondCounter<120)
		return ;

	rst_secondCounter = 0;
	
	rt = read_file_data((char *)filepath_reset_result, msg_buff, sizeof(msg_buff)-1);
	if(rt<=0)
		cnt = 0;
	else
	{
		cnt = atoi(msg_buff);
		if(cnt<0)
			cnt = 0;
	}

	cnt++;
	sprintf(msg_buff, "%d", cnt);
	write_file_data((char *)filepath_reset_result,  msg_buff, strlen(msg_buff), "w+");

	sprintf(msg_buff, "%s.%s", filepath_reset_prefix, get_local_time());
	write_file_data(msg_buff,  msg_buff, strlen(msg_buff), "w+");

	system("sync");
	system("reboot");
}

void handle_vbat_response(VpEventType event)
{
	static int fail_count = 0;
	
	VpLoopCondResultsType result;
	VpStatusType st;

	QS_DEBUG("VP_LINE_EVID_RD_LOOP: hasRt=%d handle=0x%02x\n", 
					event.hasResults, 
					event.parmHandle);

	if(event.hasResults!=TRUE || event.parmHandle!=TAG_GET_LOOP_COND_RESOULT)
		return ;

	st = VpGetResults(&event, &result);
	QS_DEBUG("%s get vbat st=%d res=%d %d %d %d %d %d %d %d %d %d\n",
		get_local_time(),st,
		/*			rloop ilg imt	 vsab vbat1   vbat2 vbat3 mspl selectedBat dcFeedReg
		 hookon: 	32767 32  30     6659 -7232   172	0     	0    	1			0
		 hookoff:	1058  28  -13858 1295 -3525   159	0     	0    	1			0
		 ringin:	32767 187 -2237  11387 -12454 160	0		0 		1 			0
					32767 144 -542   -6314 -12459 157	0 		0 		1 			0
					32767 13  -930   6535  -7826  155 	0 		0 		1 			0
					32767 -3  -923   6529  -7828  155 	0 		0 		1 			0
		 talking:	1588  18  -13843 1941 -3555   163 	0 		0 		1 			0
		 			1587  29  -13849 1941 -3557   165 	0 		0 		1 			0
		 			1588  38  -13852 1943 -3559   160	0 		0 		1 			0
		 			1491  46  -13860 1825 -3516   162 	0 		0 		1 			0
		 			1584  54  -13858 1939 -3558   160 	0 		0 		1 			0
		 error:	   11326 32767 32767 32767 32767  32767 0 		0 		1 			0
		*/
		result.rloop,                    /**< The loop resistance */
		result.ilg,                      /**< The longitudinal loop current */
		result.imt,                      /**< The metallic loop current */
		result.vsab,                     /**< The tip/ring voltage */
		result.vbat1,                    /**< The Battery 1 voltage */
		result.vbat2,                    /**< The Battery 2 voltage */
		result.vbat3,                    /**< The Battery 3 voltage */
		result.mspl,                     /**< Metering signal peak level */
		(int)result.selectedBat,      		 /**< Battery that is presently used for */
		(int)result.dcFeedReg);

	if(st!=VP_STATUS_SUCCESS)
		fail_count++;
	else
	{
		if(result.vbat1==32767 && result.vbat2==32767)
			fail_count++;
		else
			fail_count = 0;
	}
	
	if(fail_count>5)
	{
		QS_DEBUG("write log.mislic.read.vbat.fail\n");
		system("touch /tmp/log.mislic.read.vbat.fail");
		exit(-1);
	}
}

void vp_set_pcm_rxonly(int enable)
{
	static VpOptionPcmTxRxCntrlType op;

	if(enable)
		op = VP_OPTION_PCM_RX_ONLY;
	else
		op = VP_OPTION_PCM_BOTH;

	//QS_DEBUG("vp_set_pcm_rxonly en=%d\n", op);	
		
	VpSetOption(VP_NULL, &devCtx, VP_OPTION_ID_PCM_TXRX_CNTRL, &op); 
}



/**
 * Function:  alarmHandle()
 *
 * Description: This function is called at a periodic rate to perform all
 * required API operations.  It implements the functional requirements of the
 * application mentioned in the header.
 */

static int alarmHandle_work_flag = 0;
void alarmHandle(int val)
{
    VpEventType event;
    static bool deviceInitialized = FALSE;
    static uint8 cmpCounter = 0;

    char msg_buff[200];


    if(alarmHandle_work_flag==1)
    {
    	QS_DEBUG("skip alarmHandle\n");
    	return ;
	}
	
    alarmHandle_work_flag = 1;

    /*
     * This loop will query the FXS device for events, and when an event is
     * found, it will parse the event and perform further operations.
     */

    skip_param_warnings(val);
    while(VpGetEvent(&devCtx, &event)) 
    {
        /* Print the incoming event */
        UtilPrintEvent(&event); /* See "../common/utils.c" */

        /* Parse the RESPONSE category */
        if (event.eventCategory == VP_EVCAT_RESPONSE) {
            if(event.eventId == VP_DEV_EVID_DEV_INIT_CMP) 
            {
                uint8 lineNum = 0;
                InitSetMasks();
                
                QS_DEBUG("Device Initialization completed\n");

                for (; lineNum < NUM_LINES; lineNum++) 
                {
                	QS_DEBUG("Starting VpSetLineState lineNum %i\n", lineNum);
                    status = VpSetLineState(&lineCtx[lineNum], VP_LINE_STANDBY);
                    if (status != VP_STATUS_SUCCESS) {
                        QS_DEBUG("Error setting lineNum %i to VP_LINE_STANDBY: %s\n", lineNum, MapStatus(status));
                        alarmHandle_work_flag = 0;
                        exit(-1);
                    }
                    
                    QS_DEBUG("-------- Starting Calibration on lineNum %i -------\n", lineNum);
                    status = VpCalLine(&lineCtx[lineNum]);
                    if (status != VP_STATUS_SUCCESS) {
                        QS_DEBUG("Error calling VpCalLine on lineNum %i:: %s\n", lineNum, MapStatus(status));
                        alarmHandle_work_flag = 0;
                        exit(-1);
                    }
                }
            }
            else if (event.eventId == VP_EVID_CAL_CMP) 
            {
                if (++cmpCounter == NUM_LINES) 
                {
                    deviceInitialized = TRUE;
                    QS_DEBUG("Calibration completed on both lines\n\n");

                    init_tz_env_when_get_cal_cmp();
                }
            }
			else if (event.eventId == VP_LINE_EVID_RD_LOOP) 
			{
				handle_vbat_response(event);
			}
        /* Parse the SIGNALING category */
        }
        else if (event.eventCategory == VP_EVCAT_SIGNALING) 
        {
            if (event.eventId == VP_LINE_EVID_HOOK_OFF) 
            {
            	hookoff_flag = 1;
            	
                //status = VpSetLineState(event.pLineCtx, VP_LINE_ACTIVE);
                status = VpSetLineState(event.pLineCtx, VP_LINE_TALK);
				//VpSetLineTone(event.pLineCtx,T_US_SIGNEL,C_CALL,VP_NULL);

				//VpSetlineToneMix(450,0,150,150);
				
                if (status != VP_STATUS_SUCCESS) {
                    QS_DEBUG("Error running VpSetLineState on channel %d: %s\n", event.channelId, MapStatus(status));
                    alarmHandle_work_flag = 0;
                    exit(-1);
                }
                QS_DEBUG("Set channel %d in ACTIVE state\n", event.channelId);

                encode_data_to_sock_msg(APP_NAME_RPOSLIC, KEY_HOOKOFF, NULL, msg_buff);
				send_data_to_app_by_sock_path(APP_SOCK_PATH_TZPHONE, msg_buff, strlen(msg_buff));

				hookoff_flag = 1;
            } 
            else if (event.eventId == VP_LINE_EVID_HOOK_ON) 
            {
            	hookoff_flag = 0;
            	
                status = VpSetLineState(event.pLineCtx, VP_LINE_STANDBY);
                if (status != VP_STATUS_SUCCESS) {
                    QS_DEBUG("Error running VpSetLineState on channel %d: %s\n", event.channelId, MapStatus(status));
					alarmHandle_work_flag = 0;
                    exit(-1);
                }
                QS_DEBUG("Set channel %d in STANDBY state\n", event.channelId);
                
                encode_data_to_sock_msg(APP_NAME_RPOSLIC, KEY_HOOKON, NULL, msg_buff);
				send_data_to_app_by_sock_path(APP_SOCK_PATH_TZPHONE, msg_buff, strlen(msg_buff));
            }
            else if(event.eventId == VP_LINE_EVID_DTMF_DIG)
            {
				QS_DEBUG("--> evtData=0x%02x\n", event.eventData);
				
                VpSetLineTone(event.pLineCtx, VP_PTABLE_NULL, VP_PTABLE_NULL, VP_NULL);
				if(event.eventData & VP_DIG_SENSE_MAKE)
				{
					vp_set_pcm_rxonly(true);
					QS_DEBUG("	key %d	UP\n", event.eventData&0x0f);
				}
				else
				{
					vp_set_pcm_rxonly(false);
					QS_DEBUG("	key %d	Down\n", event.eventData&0x0f);

					int keycode;
					
					switch(event.eventData&0x0f)
					{
					case 1: keycode=KEY_1; break;
					case 2: keycode=KEY_2; break;
					case 3: keycode=KEY_3; break;
					case 4: keycode=KEY_4; break;
					case 5: keycode=KEY_5; break;
					case 6: keycode=KEY_6; break;
					case 7: keycode=KEY_7; break;
					case 8: keycode=KEY_8; break;
					case 9: keycode=KEY_9; break;
					case 10: keycode=KEY_0; break;
					case 11: keycode=KEY_XING; break;
					case 12: keycode=KEY_JING; break;
					default: continue;
					}

					encode_data_to_sock_msg(APP_NAME_RPOSLIC, keycode, NULL, msg_buff);
        			send_data_to_app_by_sock_path(APP_SOCK_PATH_TZPHONE, msg_buff, strlen(msg_buff));
				}
            }
			/*else if (event.eventId == VP_LINE_EVID_STARTPULSE)
			{

			}
			else if (event.eventId == VP_LINE_EVID_PULSE_DIG)
			{

			}*/
			else if (event.eventId == VP_LINE_EVID_FLASH)
			{
				QS_DEBUG("	Get Hook Flash\n");
				encode_data_to_sock_msg(APP_NAME_RPOSLIC, KEY_HOOKFLASH, NULL, msg_buff);
				send_data_to_app_by_sock_path(APP_SOCK_PATH_TZPHONE, msg_buff, strlen(msg_buff));
			}
        }

    } /* end while(VpGetEvent(&devCtx, &event)) */

    if(deviceInitialized==TRUE)
    {
    	static int tickCounter = 0;
    	
		tickCounter++;
		if(tickCounter>=MAX_CHECK_FEEDBACK_INTERVAL)
		{
			
			VpLineStateType line_st=0;
	        int rt = VpGetLineState(&lineCtx[0], &line_st);
	                
	        sprintf(msg_buff, "%s VpGetLineState rt=%d st=%d", 
	        			get_local_time(), 
	        			rt, 
	        			line_st);
			//QS_DEBUG("%s\n", msg_buff);
			
			rt = write_file_data(TZ_SLIC_LINEFEED_STATE_PATH, msg_buff, strlen(msg_buff), "w+");
			if(rt <= 0)
				QS_DEBUG("write %s failed\n", TZ_SLIC_LINEFEED_STATE_PATH);

			tickCounter = 0;
		}

		try_to_do_check_reset_work();
    }

    alarmHandle_work_flag = 0;
}

/**
 * InitSetMasks(void)
 *
 * Description: This function initialize the masks to unmask all events
 * except the rollover event.
 */

void InitSetMasks(void)
{
    VpOptionEventMaskType eventMask;

    /* mask everything */
    VpMemSet(&eventMask, 0xFF, sizeof(VpOptionEventMaskType));

    /* unmask only the events the application is interested in */
    eventMask.faults = VP_EVCAT_FAULT_UNMASK_ALL;
    eventMask.signaling = VP_EVCAT_SIGNALING_UNMASK_ALL | VP_DEV_EVID_TS_ROLLOVER ;
    eventMask.response = VP_EVCAT_RESPONSE_UNMASK_ALL;
    eventMask.test = VP_EVCAT_TEST_UNMASK_ALL;
    eventMask.process = VP_EVCAT_PROCESS_UNMASK_ALL;

    /* inform the API of the mask */
    VpSetOption(VP_NULL, &devCtx, VP_OPTION_ID_EVENT_MASK, &eventMask);

    VpOptionDtmfModeType op_dtmf;
    op_dtmf.dtmfControlMode = VP_OPTION_DTMF_DECODE_ON; 
    op_dtmf.direction = VP_DIRECTION_US;
	VpSetOption(VP_NULL, &devCtx, VP_OPTION_ID_DTMF_MODE, &op_dtmf);

	VpOptionTimeslotType op_timeSlot;
	op_timeSlot.rx = 0;
	op_timeSlot.tx = 0;
	VpSetOption(VP_NULL, &devCtx, VP_OPTION_ID_TIMESLOT, &op_timeSlot);

	VpOptionCodecType op_codec;
	op_codec = VP_OPTION_LINEAR;
	VpSetOption(VP_NULL, &devCtx, VP_OPTION_ID_CODEC, &op_codec);

	VpOptionPulseModeType op_pulseMode;
	op_pulseMode = VP_OPTION_PULSE_DECODE_ON;
	VpSetOption(VP_NULL, &devCtx, VP_OPTION_ID_PULSE_MODE, &op_pulseMode);

	VpOptionPulseType op_pulseType;
	//1. breakMin < breakMax < flashMin < flashMax < onHookMin
	//2. makeMin < makeMax < interDigitMin
											/* default values: */
	op_pulseType.breakMin = 20 * 8; 		/* 33 milliseconds */
	op_pulseType.breakMax = 80 * 8; 		/* 100ms */
	op_pulseType.makeMin = 17 * 8; 			/* 17ms */
	op_pulseType.makeMax = 75 * 8; 			/* 75ms */
	op_pulseType.interDigitMin = 250 * 8; 	/* 250ms */
	op_pulseType.flashMin = 81 * 8; 		/* 250ms */
	op_pulseType.flashMax = 700 * 8; 		/* 1300ms */
	VpSetOption(VP_NULL, &devCtx, VP_DEVICE_OPTION_ID_PULSE, &op_pulseType); 
}

int tzphone_to_slic_msg_handler(char *app_name, int msgId, char *msgInfo)
{	
	int len;
	int n;
	int timer_en=0;
	int ton=0, toff=0;
	
	tz_dbg(LOG_LEVEL_VP_INFO, "tzphone_to_slic_msg app_name=%s id=%d msg=%s", app_name, msgId, msgInfo);
	if(strcmp(app_name, APP_NAME_TZPHONE)!=0)
		return 0;

	switch(msgId)
	{
	case MSG_SLIC_REVERSE_ACTIVE:
		VpSetLineState(&lineCtx[0], VP_LINE_TALK_POLREV);
		break;
		
	case MSG_SLIC_TONE_START:
	{	
		if(msgInfo==NULL || strlen(msgInfo)<1)
			break;
		n = sscanf(msgInfo, "%d %d %d", &timer_en, &ton, &toff);
		//VpSetlineToneMix(450,0,150,150);
		if(n>0 && n<3)
		{
			if(timer_en==0)
				{
				
				VpSetLineState(&lineCtx[0], VP_LINE_TALK);
			    VpSetlineToneMix(450,0,350,0);
				}

		}
		else if(n==3)
		{
			if(timer_en==0)
			{
				VpSetLineState(&lineCtx[0], VP_LINE_TALK);
				VpSetlineToneMix(450,0,350,0);
			}
			else
			{
			
			    VpSetLineState(&lineCtx[0], VP_LINE_TALK);
				VpSetlineToneMix(450,0,ton,toff);

			}
		}

		/*ProSLIC_ToneGenStop(slic_dev_fd->currentChanPtr);

		n = sscanf(msgInfo, "%d %d %d", &timer_en, &ton, &toff);
		tz_slic_dbg("MSG_SLIC_TONE_START n=%d en=%d ton=%d toff=%d",
					n, timer_en, ton, toff);
		if(n>0 && n<3)
		{
			if(timer_en==0)
				ProSLIC_ToneGenStart(slic_dev_fd->currentChanPtr, 0);
		}
		else if(n==3)
		{
			if(timer_en==0)
				ProSLIC_ToneGenStart(slic_dev_fd->currentChanPtr, 0);
			else
			{
				set_tonegen_osc_timers(slic_dev_fd,ton,toff);
				toggle_tonegen_osc(slic_dev_fd);
			}
		}*/
	}
		break;
	case MSG_SLIC_TONE_STOP:
		//ProSLIC_ToneGenStop(slic_dev_fd->currentChanPtr);
		
		VpSetLineState(&lineCtx[0], VP_LINE_TALK);
		VpSetlineToneMix(450,0,0,350);
		break;

	case MSG_SLIC_RING_START:
		len = strlen(msgInfo);
		if(len>=32)
			msgInfo[32-1]='\0';
	
		if(len>0)
		{
			static char cid_data[200];
			int cid_data_len;

			cid_data_len = encode_cid_format(msgInfo, cid_data);
			tz_print_bin("CID", cid_data, cid_data_len);

			VpInitCid(&lineCtx[0], cid_data_len, (unsigned char *)cid_data);
		}
		
		VpSetLineState(&lineCtx[0], VP_LINE_RINGING);
		break;

	case MSG_SLIC_RING_STOP:
		//ring_cmd_flag = 0;
		//ProSLIC_RingStop(slic_dev_fd->currentChanPtr);

		if(hookoff_flag==0)
			VpSetLineState(&lineCtx[0], VP_LINE_STANDBY);
		else
			VpSetLineState(&lineCtx[0], VP_LINE_TALK);
		break;	

	case MSG_SLIC_INCOMING_CALLWAIT_TONE:
		n = sscanf(msgInfo, "%d %d %d", &timer_en, &ton, &toff);
		if(n>0 && n<3)
		{
		
		    VpSetLineState(&lineCtx[0], VP_LINE_TALK);
			VpSetlineToneMix(450,0,1000,1000);

		}
		else if(n==3)
		{
		  if (ton == 500 && toff == 1500)
		     toff = 1200;
		  VpSetLineState(&lineCtx[0], VP_LINE_TALK);
		  VpSetlineToneMix(450,0,ton,toff);

		}

		
		/*n = sscanf(msgInfo, "%d %d %d", &timer_en, &ton, &toff);
		tz_slic_dbg("MSG_SLIC_INCOMING_CALLWAIT_TONE n=%d en=%d ton=%d toff=%d",
					n, timer_en, ton, toff);

		if(n>0 && n<3)
		{
			set_tonegen_osc_timers(slic_dev_fd, 1000, 1000);
			toggle_tonegen_osc(slic_dev_fd);
		}
		else if(n==3)
		{
			set_tonegen_osc_timers(slic_dev_fd,ton,toff);
			toggle_tonegen_osc(slic_dev_fd);
		}*/
		break;

	case MSG_SLIC_TZPHONE_IDLE:
		if(hookoff_flag==0)
		{
			VpGetLoopCond(&lineCtx[0], TAG_GET_LOOP_COND_RESOULT);
			QS_DEBUG("send_get_vbat_request\n");
		}
		break;
		
	default:
		break;
	}
	return 1;
}


void init_tz_env_when_get_cal_cmp(void)
{
	tSOCK_SRV_PARAMS srv;
	char msg_buff[64];
	
	strcpy(srv.sock_path, APP_SOCK_PATH_RPOSLIC);
	srv.cb = tzphone_to_slic_msg_handler;
	if(create_socket_srv_thread(&srv)==0)
	{
		QS_DEBUG("create_socket_srv_thread fail\n");
		return ;
	}

	VpInitRing(&lineCtx[0], RING_CAD_STD, CLI_TYPE1_US);

	encode_data_to_sock_msg(APP_NAME_RPOSLIC, KEY_HOOKON, NULL, msg_buff);
	send_data_to_app_by_sock_path(APP_SOCK_PATH_TZPHONE, msg_buff, strlen(msg_buff));
}

int read_file_data(char *filename, char *out_buf, int buf_len)
{   
    FILE *fp;
	int n;

    out_buf[0]='\0';

    fp = fopen(filename, "r");
    if(fp == NULL)
    	return 0;

    n = fread(out_buf, 1, buf_len, fp);
    fclose(fp);

    if(n<=0)
    	return 0;

    out_buf[n]='\0';
    return n;
}



int write_file_data(char *filepath, char *data, int data_size, char *attr)
{
	int n;
	FILE *fp;
	
	fp = fopen(filepath, attr);
	if(fp == NULL)
		return -1;

	n = fwrite(data, 1, data_size, fp);
	fclose(fp);

	return n;
}


/*void *running_check_state_pthread(void *param)
{
	char temp[100];

	pthread_detach(pthread_self());
	skip_param_warnings(param);

	while(1)
	{
		VpLineStateType line_st=0;
        int rt = VpGetLineState(&lineCtx[0], &line_st);
                
        sprintf(temp, "VpGetLineState rt=%d st=%d\n", rt, line_st);
		QS_DEBUG("%s", temp);

		write_file_data(TZ_SLIC_LINEFEED_STATE_PATH, temp, strlen(temp), "w+");

        sleep(5);
	}

	return NULL;	
}

int init_check_state_pthread(void)
{
	int rt;
	pthread_t pid;

	rt = pthread_create(&pid, NULL, running_check_state_pthread, NULL);
	if(rt!=0)
	{
		QS_DEBUG("init_keyboard_pthread failed\n");
		return 0;
	}
	
	return 1;
}*/


static unsigned char calc_cid_checksum(char *str, int nLen)
{
    int i=0;
    uint8 sum = 0;
	for(i=0; i<nLen; i++)
        sum += str[i];
    return -sum;
}

static int encode_cid_format(const char *phone_num, char *out_buff)
{
	char sTime[32] = "02201030";
	int i;
    unsigned char TxBuf[256];
    unsigned char *pTxBuf=TxBuf;
	int nTxLen;
	int nNumLen;

	struct timeval    tv;  
    struct timezone tz;  
    struct tm         *p;  

    char *ptr;

    ptr = strchr((char *)phone_num, '\r');
    if(ptr!=NULL)
    	*ptr = '\0';

    ptr = strchr((char *)phone_num, '\n');
    if(ptr!=NULL)
    	*ptr = '\0';

    nNumLen = strlen((char *)phone_num);	
    if(nNumLen==0)
    	return 0;
      
    gettimeofday(&tv, &tz);  
    p = localtime(&tv.tv_sec);  
    sprintf(sTime, "%02d%02d%02d%02d", 
    	1+p->tm_mon, p->tm_mday, 
    	p->tm_hour, p->tm_min);  

    //QS_DEBUG("sTime=%s\n", sTime);
	
	*pTxBuf++ = 0x80;
	*pTxBuf++ = (2+8)+(2+nNumLen); //msg length  
   	*pTxBuf++  = 0x01;
	*pTxBuf++  = 0x08;
	
   	for(i=0; i<8; i++)
      	*pTxBuf++  = sTime[i];

	if(nNumLen!=0)
	{
		*pTxBuf++ =0x02;
		*pTxBuf++ =nNumLen;
		memcpy(pTxBuf, phone_num, nNumLen);	 
		pTxBuf += nNumLen;
	}

	nTxLen = pTxBuf-TxBuf;
	*pTxBuf++ = (calc_cid_checksum((char *)TxBuf, nTxLen));
	nTxLen = pTxBuf-TxBuf;

	memcpy(out_buff, TxBuf, nTxLen);
	out_buff[nTxLen]='\0';

	return nTxLen;
}


void check_dump_mode(void)
{
	static int count = 0;

	count++;
	if(count<100)
		return ;

	count = 0;
	
	if(access("/tmp/log.slic.dump.flag", F_OK)<0)
		return ;
		
	remove("/tmp/log.slic.dump.flag");
	VpRegisterDump(&devCtx);
}

