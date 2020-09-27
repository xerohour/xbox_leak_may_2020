/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    xprofp.h

Abstract:

    Kernel profiler functions

--*/

#ifndef _XPROFP_H
#define _XPROFP_H

#ifdef __cplusplus
extern "C" {
#endif

NTSTATUS
XProfpControl(
    ULONG Action,
    ULONG Param
    );

#define XPROF_START             1
#define XPROF_STOP              2
#define XPROF_COMMENT           4
#define XPROF_THREAD_SWITCH     5
#define XPROF_DPC_BEGIN         6
#define XPROF_DPC_END           7

#define XPROF_DPC_ENABLED       0x80000000

/*
    action: XPROF_START
    param: size of profiling data buffer in MB, default = 32

    action: XPROF_STOP
    param: none

    action: XPROF_COMMENT
    param: user-defined parameter

 */

/*
 !!! Profiling data record format:
    timestamp: bits 63-32
    timestamp: bits 31-0
    type-specific data

    Bits 63-61 of the timestamp are used as flags.

    000 - fastcap start: just before calling a function
        caller function: ULONG
        callee function: ULONG

    001 - fastcap end: just after calling a function
        caller function: ULONG

    010 - _penter:
        caller function: ULONG
        ESP: ULONG

    011 - _pexit:
        caller function: ULONG
        ESP: ULONG

    100 - callcap start:
        caller function: ULONG

    101 - callcap exit:
        caller function: ULONG

    110 - reserved

    111 - control records:
        record type: ULONG
        param: ULONG

    If type is XPROF_START or XPROF_STOP, param is the current thread ID
    If type is XPROF_COMMENT, param is the user-defined parameter.
    If type is XPROF_THREAD_SWITCH, param is the new thread ID.
    If type is XPROF_DPC_START or XPROF_DPC_END, param is the DPC function address.

    We don't need to record thread ID in each record because we can always derive
    the current thread ID from the XPROF_START and XPROF_THREAD_SWITCH records.

 */

#define XPROFREC_TYPEMASK       0xE0000000
#define XPROFREC_FASTCAP_START  0x00000000
#define XPROFREC_FASTCAP_END    0x20000000
#define XPROFREC_PENTER         0x40000000
#define XPROFREC_PEXIT          0x60000000
#define XPROFREC_CALLCAP_START  0x80000000
#define XPROFREC_CALLCAP_END    0xA0000000
#define XPROFREC_CONTROL        0xE0000000

//
// Maximum size for all types of profile data records.
// Note that we're setting this to be a larger value than necessary
// in case we need to add new types of profiling data records
// in the future.
//
#define XPROFREC_MAXSIZE (32*sizeof(ULONG))

//
// Profile data file header
//
#define XPROF_FILE_VERSION 0x00000001
#define XPROF_FILE_HEADER_SIZE sizeof(XProfpFileHeader)
#define XPROF_MAX_MODULES 16
#define XPROF_MAX_MODULE_NAMELEN 48

typedef struct {
    ULONG version;                              // 0x000 + 0x004
    ULONG module_count;                         // 0x004 + 0x004
    struct {                                    // 0x008 + 0x010 * 0x040
        ULONG loadaddr;
        ULONG size;
        ULONG reserved[2];
        CHAR name[XPROF_MAX_MODULE_NAMELEN];
    } modules[XPROF_MAX_MODULES];
    UCHAR reserved[0x1000-0x408];               // ..... = 0x1000
} XProfpFileHeader;


//
// Global data structures maintained by the profiler
//
// NOTE: Do NOT change the first two fields of this structure,
// unless you also change the offsets in the assembly functions
// CAP_Start_Profiling and CAP_End_Profiling.
//
typedef struct {
    ULONG* bufnext;
        // Points to the next available space in the profiling data buffer

    ULONG* bufend;
        // End of profiling data buffer

    ULONG* bufnext_stopped;
        // Value of bufnext when the profiling session is stopped
        // NULL if profiling session is active or hasn't been stopped

    ULONG* bufstart;
        // Start of profiling data buffer
        // NULL if profiling session isn't active

    ULONG start_type;
        // Unused

    ULONG start_param;
        // Parameter that was passed to XPROF_START_x

    LONG lock;
        // Global access lock:
        //  0 if the lock is free
        //  otherwise, the lock is busy

    LONG reserved;
} XProfpGlobals;

//
// Retrieve kernel profiling global data structure
//
XProfpGlobals*
XProfpGetData();

//
// Runtime support function for sweeping BBT data into a file
//
NTSTATUS IrtSweep(HANDLE file);

#ifdef __cplusplus
}
#endif

#endif // !_XPROFP_H

