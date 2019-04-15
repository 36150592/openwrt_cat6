/*
 * sys_service.h
 *
 *  This file is the header for all standard types used in the API code.
 *
 * Copyright (c) 2011, Microsemi Corporation
 */
#ifndef SYS_SERVICE_H
#define SYS_SERVICE_H
#include "vp_api_types.h"
/* Critical section types */
typedef enum {
	VP_MPI_CRITICAL_SEC, 	/* MPI access critical code section */
	VP_HBI_CRITICAL_SEC, 	/* HBI access critical code section */
	VP_CODE_CRITICAL_SEC, 	/* Critical code section */
	VP_NUM_CRITICAL_SEC_TYPES, /* The number of critical section types */
    VP_CRITICAL_SEC_ENUM_SIZE = FORCE_STANDARD_C_ENUM_SIZE /* Portability Req. */
} VpCriticalSecType;

EXTERN uint8
VpSysEnterCritical(
    VpDeviceIdType deviceId,
    VpCriticalSecType criticalSecType);

EXTERN uint8
VpSysExitCritical(
    VpDeviceIdType deviceId,
    VpCriticalSecType criticalSecType);

EXTERN void
VpSysWait(
    uint8 time);

EXTERN bool
VpSysInit(void);


EXTERN void
VpSysDisableInt(
    VpDeviceIdType deviceId);

EXTERN void
VpSysEnableInt(
    VpDeviceIdType deviceId);

EXTERN bool
VpSysTestInt(
    VpDeviceIdType deviceId);

EXTERN void
VpSysDtmfDetEnable(
    VpDeviceIdType deviceId,
    uint8 channelId);

EXTERN void
VpSysDtmfDetDisable(
    VpDeviceIdType deviceId,
    uint8 channelId);

EXTERN void *
VpSysTestHeapAcquire(
    uint8 *pHeapId);

EXTERN bool
VpSysTestHeapRelease(
    uint8 heapId);

EXTERN void
VpSysPcmCollectAndProcess(
    void *pLineCtx,
    VpDeviceIdType deviceId,
    uint8 channelId,
    uint8 startTimeslot,
    uint16 operationTime,
    uint16 settlingTime,
    uint16 operationMask);


typedef enum{
	LOG_LEVEL_HAL = 0x01,
	LOG_LEVEL_SYS_SRV = 0x02,
	LOG_LEVEL_VP_ERROR = 0x04,
	LOG_LEVEL_VP_INFO = 0x08,
	LOG_LEVEL_ALL = 0xffff
}enumTZLogLevel;

extern char *get_local_time(void);
extern void tz_print_bin(char *title, char *bin, int len);
extern void set_tz_log_level(int flag);
extern void tz_dbg(int level, char *format, ...);
extern void msleep_ext(unsigned int ms);

#include <stdio.h>

#define VpSysDebugPrintf printf

#if 0
/* Enable the XML tag <DBG> debug output </DBG>  */
#undef ENABLE_DBG_TAG

#include <stdio.h>
#ifdef ENABLE_DBG_TAG
    #include <stdarg.h>
    EXTERN int
    VpSysDebugPrintf(
        const char *format, ...);
#else
    #define VpSysDebugPrintf printf
#endif /* ENABLE_DBG_TAG */
#endif

#endif /* SYS_SERVICE_H */

