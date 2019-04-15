/** \file sys_service.c
 * sys_service.c
 *
 *  This file implements the required system services for the API-II using a
 * Linux OS running on the UVB.  The user should replace the functions provided
 * here with the equivalent based on their OS and hardware.
 *
 * Copyright (c) 2011, Microsemi Corporation
 */
#include "vp_api_types.h"
#include "sys_service.h"
#include <fcntl.h>
#include <pthread.h>


//#include <cutils/log.h>

#define LOG_TAG "volte"

void msleep_ext(unsigned int ms)
{
	int val = ms;
	struct timeval tmv;

	if(val < 10)
		val = 10;

	if(val > 1000)
		val = 1000;
		
    tmv.tv_sec = 0;
    tmv.tv_usec = val*1000;
	select(0, NULL, NULL, NULL, &tmv);
}

void nsleep_ext(unsigned int ns)
{
	unsigned int val = ns;
	struct timeval tmv;

	if(val < 10)
		val = 10;
		
    tmv.tv_sec = 0;
    tmv.tv_usec = val;
    tz_dbg(LOG_LEVEL_SYS_SRV, "nsleep_ext usec=%u", tmv.tv_usec);
	select(0, NULL, NULL, NULL, &tmv);
}

/*
 * VpSysWait() function implementation is needed only for CSLAC devices
 * (880, 790). For other devices this function could be commented.
 */
void
VpSysWait(
    uint8 time)  /* Time specified in increments of 125uS (e.g. 4 = 500uS) */
{
    /* Blocking delay function added here based on OS */
    nsleep_ext(time*125);
    tz_dbg(LOG_LEVEL_SYS_SRV, "VpSysWait t=%d", time);
    return;
}

pthread_mutex_t vp_sys_mutex[VP_NUM_CRITICAL_SEC_TYPES];

int init_sys_mutex(void)
{
	int i;
	int rt;
	
	for(i=0;i<VP_NUM_CRITICAL_SEC_TYPES;i++)
	{
		rt = pthread_mutex_init(&(vp_sys_mutex[i]),NULL);
		if(rt != 0)
			return rt;
	}

	return 0;
}

void lock_sys_mutex(VpCriticalSecType criticalSecType)
{
	pthread_mutex_lock(&(vp_sys_mutex[criticalSecType]));
}

void unlock_sys_mutex(VpCriticalSecType criticalSecType)
{
	pthread_mutex_unlock(&(vp_sys_mutex[criticalSecType]));
}

/*
 * VpSysInit()
 *
 * This function isn't part of the official System Services Layer, but is
 * required for the ZTAP implementation to initialize semaphores.  Applications
 * should call this function once (and only once) before calling any VP-API-II
 * functions that might use critical sections.
 */
bool
VpSysInit(void)
{
	int rt;
	
	rt = init_sys_mutex();
	if (rt != 0) {
		tz_dbg(LOG_LEVEL_SYS_SRV, "VpSysInit failed[%d]",rt);
		return FALSE;
	}
	
	tz_dbg(LOG_LEVEL_SYS_SRV, "VpSysInit ok");
	return TRUE;
}



/*
 * VpSysEnterCritical(), VpSysExitCritical():
 *
 *  These functions allow for disabling interrupts while executing nonreentrant
 * portions of VoicePath API code. Note that the following implementations of
 * enter/exit critical section functions are simple implementations. These
 * functions could be expanded (if required) to handle different critical
 * section types differently.
 *
 * Params:
 *  VpDeviceIdType deviceId: Device Id (chip select ID)
 *  VpCriticalSecType: Critical section type
 *
 * Return:
 *  Number of critical sections currently entered for the device.
 */
uint8
VpSysEnterCritical(
    VpDeviceIdType deviceId,
    VpCriticalSecType criticalSecType)
{
    /*
     * Code to check if semaphore can be taken, and if so increment. Block if
     * semaphore cannot be taken for this process ID.
     */
    /*
     * criticalDepth++;
     * return criticalDepth;
     */
    /* Prevent compiler from generating error */
    skip_param_warnings(deviceId);
    
    lock_sys_mutex(criticalSecType);
    
    return 0;
} /* VpSysEnterCritical() */
uint8
VpSysExitCritical(
    VpDeviceIdType deviceId,
    VpCriticalSecType criticalSecType)
{
    /* Code to decrement semaphore */
    /*
     * criticalDepth--;
     * return criticalDepth;
     */
    /* Prevent compiler from generating error */
    skip_param_warnings(deviceId);
    
    unlock_sys_mutex(criticalSecType);
    
    return 0;
} /* VpSysExitCritical() */
/**
 * VpSysDisableInt(), VpSysEnableInt(), and VpSysTestInt()
 *
 *  These functions are used by the CSLAC device family for interrupt driven
 * polling modes. These are called by the API to detect when a non-masked
 * device status has changed.  If using SIMPLE_POLL mode, these functions do not
 * require implementation.
 *
 * Preconditions:
 *  None. The implementation of these functions is architecture dependent.
 *
 * Postconditions:
 *  VpSysDisableInt() - The interrupt associated with the deviceId passed is
 * disabled.
 *
 * VpSysEnableInt() - The interrupt associated with the deviceId passed is
 * enabled.
 *
 * VpSysTestInt() - The return value is TRUE if an interrupt occurred, otherwise
 * return FALSE.
 *
 * These functions are needed only for CSLAC devices
 * (880, 790). For other devices these functions could be commented.
 *
 */
void
VpSysDisableInt(
    VpDeviceIdType deviceId)
{
	skip_param_warnings(deviceId);
    return;
}
void
VpSysEnableInt(
    VpDeviceIdType deviceId)
{
	skip_param_warnings(deviceId);
    return;
}
bool
VpSysTestInt(
    VpDeviceIdType deviceId)
{
	skip_param_warnings(deviceId);
    return FALSE;
}
/**
 * VpSysDtmfDetEnable(), VpSysDtmfDetDisable()
 *
 *  These functions are used by the CSLAC device family for devices that do not
 * internally detect DTMF. It is used for Caller ID type-II and is provided to
 * enable external DTMF detection.
 *
 * Preconditions:
 *  None. The implementation of these functions is application dependent.
 *
 * Postconditions:
 *  VpSysDtmfDetEnable() - The device/channel resource for DTMF detection is
 * enabled.
 *
 *  VpSysDtmfDetDisable() - The device/channel resource for DTMF detection is
 * disabled.
 *
 * These functions are needed only for CSLAC devices
 * (880, 790). For other devices these functions could be commented.
 *
 */
void
VpSysDtmfDetEnable(
    VpDeviceIdType deviceId,
    uint8 channelId)
{
	skip_param_warnings(deviceId);
	skip_param_warnings(channelId);
}
void
VpSysDtmfDetDisable(
    VpDeviceIdType deviceId,
    uint8 channelId)
{
	skip_param_warnings(deviceId);
	skip_param_warnings(channelId);
}
/*
 * The following functions VpSysTestHeapAcquire(),  VpSysTestHeapRelease()
 * VpSysPcmCollectAndProcess() and are needed only for CSLAC devices
 * (880). For other devices these functions could be commented. Please see
 * the LineTest API documentation for function requirements.
 */
void *
VpSysTestHeapAcquire(
    uint8 *pHeapId)
{
	skip_param_warnings(pHeapId);
    return VP_NULL;
} /* VpSysTestHeapAcquire() */
bool
VpSysTestHeapRelease(
    uint8 heapId)
{
	skip_param_warnings(heapId);
    return TRUE;
} /* VpSysTestHeapRelease() */
void
VpSysPcmCollectAndProcess(
    void *pLineCtx,
    VpDeviceIdType deviceId,
    uint8 channelId,
    uint8 startTimeslot,
    uint16 operationTime,
    uint16 settlingTime,
    uint16 operationMask)
{
	skip_param_warnings(pLineCtx);
	skip_param_warnings(deviceId);
	skip_param_warnings(channelId);
	skip_param_warnings(startTimeslot);
	skip_param_warnings(operationTime);
	skip_param_warnings(settlingTime);
	skip_param_warnings(operationMask);
} /* VpSysPcmCollectAndProcess() */



void tz_print_bin(char *title, char *bin, int len)
{
	int i,j=0;
	
	printf("%s: len=%d\n", title, len);
	printf("  ");

	j = 0;
	for(i=0;i<len;i++)
	{
		printf("%02x ", (unsigned char)bin[i]);
		
		j++;
		if(j==16)
		{
			j=0;
			printf("\r\n  ");
		}
	}
	
	printf("\r\n");
}

static int tz_log_level_flag = LOG_LEVEL_ALL;

void set_tz_log_level(int flag)
{
	tz_log_level_flag = flag;
}

void tz_dbg(int level, char *format, ...)
{
	int flag;
	va_list list;

	static char debug_buff[1024];

	flag = tz_log_level_flag&level;
	if(flag==0)
		return ;

	memset(debug_buff, 0, sizeof(debug_buff));
	va_start(list, format);
	vsprintf(debug_buff, format, list);
	va_end(list);

	//ALOGD("[tz_dbg] %s",debug_buff);

	if(strlen(debug_buff)>0)
		printf("[lv-0x%02x] %s\r\n", level, debug_buff);
	else
		printf(".\r\n");
}
