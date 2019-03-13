/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __ADB_TRACE_H
#define __ADB_TRACE_H
#include <syslog.h>
#if !ADB_HOST
#include <android/log.h>
#endif

/* define ADB_TRACE to 1 to enable tracing support, or 0 to disable it */
#define  ADB_TRACE    1

/* IMPORTANT: if you change the following list, don't
 * forget to update the corresponding 'tags' table in
 * the adb_trace_init() function implemented in adb.c
 */
typedef enum {
    TRACE_ADB = 0,   /* 0x001 */
    TRACE_SOCKETS,
    TRACE_PACKETS,
    TRACE_TRANSPORT,
    TRACE_RWX,       /* 0x010 */
    TRACE_USB,
    TRACE_SYNC,
    TRACE_SYSDEPS,
    TRACE_JDWP,      /* 0x100 */
    TRACE_SERVICES,
    TRACE_AUTH,
    TRACE_FDEVENT,
} AdbTrace;

#if ADB_TRACE

#if !ADB_HOST
/*
 * When running inside the emulator, guest's adbd can connect to 'adb-debug'
 * qemud service that can display adb trace messages (on condition that emulator
 * has been started with '-debug adb' option).
 */

/* Delivers a trace message to the emulator via QEMU pipe. */
void adb_qemu_trace(const char* fmt, ...);
/* Macro to use to send ADB trace messages to the emulator. */
#define DQ(...)    adb_qemu_trace(__VA_ARGS__)
#else
#define DQ(...) ((void)0)
#endif  /* !ADB_HOST */

extern int     adb_trace_mask;
extern unsigned char    adb_trace_output_count;
void    adb_trace_init(void);

#  define ADB_TRACING  ((adb_trace_mask & (1 << TRACE_TAG)) != 0)

/* you must define TRACE_TAG before using this macro */
#if ADB_HOST
#  define  D(...)                    	 syslog(LOG_INFO,__VA_ARGS__)
#  define  DR(...)          	 syslog(LOG_INFO,__VA_ARGS__)
#  define  DD(...)         	syslog(LOG_INFO,__VA_ARGS__)
#else
#  define  D(...)                   syslog(LOG_INFO,__VA_ARGS__)
#  define  DR(...)               syslog(LOG_INFO,__VA_ARGS__)
#  define  DD(...)               syslog(LOG_INFO,__VA_ARGS__)
#endif /* ADB_HOST */
#else
#  define  D(...)          syslog(LOG_INFO,__VA_ARGS__)
#  define  DR(...)         syslog(LOG_INFO,__VA_ARGS__)
#  define  DD(...)       syslog(LOG_INFO,__VA_ARGS__)
#  define  ADB_TRACING     0
#endif /* ADB_TRACE */

#endif /* __ADB_TRACE_H */
