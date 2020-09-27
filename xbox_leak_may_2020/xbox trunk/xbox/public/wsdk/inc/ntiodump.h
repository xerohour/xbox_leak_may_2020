/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) 1994-1999  Microsoft Corporation

Module Name:

    ntiodump.h

Abstract:

    This is the include file that defines all constants and types for
    accessing memory dump files.

Author:

    Darryl Havens (darrylh) 6-jan-1994

Revision History:


--*/

#ifndef _NTIODUMP_
#define _NTIODUMP_

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef MIDL_PASS
#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning( disable : 4200 ) // nonstandard extension used : zero-sized array in struct/union
#endif // MIDL_PASS

#ifdef __cplusplus
extern "C" {
#endif

//
// Define the information required to process memory dumps.
//


// Dump Types
typedef enum _DUMP_TYPES {
    DUMP_TYPE_INVALID = -1,
    DUMP_TYPE_UNKNOWN = 0,
    DUMP_TYPE_FULL,
    DUMP_TYPE_SUMMARY,
    DUMP_TYPE_HEADER,
    DUMP_TYPE_TRIAGE,
    DUMP_TYPE_TRIAGE_ACT_UPON
} DUMP_TYPE;

//
// Define dump header longword offset constants.
//

#define DH_PHYSICAL_MEMORY_BLOCK        25
#define DH_CONTEXT_RECORD               200
#define DH_EXCEPTION_RECORD             500

#define DH_REBOOT_AFTER_CRASHDUMP       900
#define DH_DUMP_TYPE                    994
#define DH_REQUIRED_DUMP_SPACE          1000
#define DH_CRASH_DUMP_TIMESTAMP         1008
#define DH_SUMMARY_DUMP_RECORD          1024


//
// Define the dump header structure.
//

typedef struct _DUMP_HEADER {
    ULONG Signature;
    ULONG ValidDump;
    ULONG MajorVersion;
    ULONG MinorVersion;
    ULONG_PTR DirectoryTableBase;
    PULONG PfnDataBase;
    PLIST_ENTRY PsLoadedModuleList;
    PLIST_ENTRY PsActiveProcessHead;
    ULONG MachineImageType;
    ULONG NumberProcessors;
    ULONG BugCheckCode;
    ULONG_PTR BugCheckParameter1;
    ULONG_PTR BugCheckParameter2;
    ULONG_PTR BugCheckParameter3;
    ULONG_PTR BugCheckParameter4;
    CHAR VersionUser[32];
    UCHAR PaeEnabled;
    UCHAR Spare3[3];
    ULONG Spare2;
} DUMP_HEADER, *PDUMP_HEADER;

#define VALID_KERNEL_VA                     1
#define VALID_CURRENT_USER_VA               2

//
// Only exists for summary dumps.
// If summary dump exists when header contains a valid signature
//

typedef struct _SUMMARY_DUMP_HEADER {
    ULONG Signature;
    ULONG ValidDump;
    ULONG DumpOptions;  // Summary Dump Options
    ULONG HeaderSize;   // Offset to the start of actual memory dump
    ULONG BitmapSize;   // Total bitmap size (i.e., maximum #bits)
    ULONG Pages;        // Total bits set in bitmap (i.e., total pages in sdump)
} SUMMARY_DUMP_HEADER, * PSUMMARY_DUMP_HEADER;


//
// Only exists for triage dumps.
//

typedef struct _TRIAGE_DUMP_HEADER {
    ULONG ServicePackBuild;             // What service pack of NT was this ?
    ULONG SizeOfDump;                   // Size in bytes of the dump
    ULONG ValidOffset;                  // Offset valid DWORD
    ULONG ContextOffset;                // Offset of CONTEXT record
    ULONG ExceptionOffset;              // Offset of EXCEPTION record
    ULONG MmOffset;                     // Offset of Mm information
    ULONG UnloadedDriversOffset;        // Offset of Unloaded Drivers
    ULONG PrcbOffset;                   // Offset of KPRCB
    ULONG ProcessOffset;                // Offset of EPROCESS
    ULONG ThreadOffset;                 // Offset of ETHREAD
    ULONG CallStackOffset;              // Offset of CallStack Pages
    ULONG SizeOfCallStack;              // Size in bytes of CallStack
    ULONG DriverListOffset;             // Offset of Driver List
    ULONG DriverCount;                  // Number of Drivers in list
    ULONG StringPoolOffset;             // Offset to the string pool
    ULONG StringPoolSize;               // Size of the string pool
    ULONG BrokenDriverOffset;           // Offset into the driver of the driver that crashed
    ULONG TriageOptions;                // Triage options in effect at crashtime
    ULONG BaseOfStack;                  // The base the callstack region.
} TRIAGE_DUMP_HEADER, * PTRIAGE_DUMP_HEADER;

//
// In the triage dump ValidFields field what portions of the triage-dump have
// been turned on.
//

#define TRIAGE_DUMP_CONTEXT          (0x0001)
#define TRIAGE_DUMP_EXCEPTION        (0x0002)
#define TRIAGE_DUMP_PRCB             (0x0004)
#define TRIAGE_DUMP_PROCESS          (0x0008)
#define TRIAGE_DUMP_THREAD           (0x0010)
#define TRIAGE_DUMP_STACK            (0x0020)
#define TRIAGE_DUMP_DRIVER_LIST      (0x0040)
#define TRIAGE_DUMP_BROKEN_DRIVER    (0x0080)

#define TRIAGE_DUMP_VALID       ( 'DGRT' )
#define TRIAGE_DUMP_SIZE        ( PAGE_SIZE * 16 )

#ifdef _NTLDRAPI_
typedef struct _DUMP_DRIVER_ENTRY {
    ULONG DriverNameOffset;
    LDR_DATA_TABLE_ENTRY LdrEntry;
} DUMP_DRIVER_ENTRY, * PDUMP_DRIVER_ENTRY;
#endif // _NTLDRAPI

//
// The DUMP_STRING is guaranteed to be both NULL terminated and length prefixed
// (prefix does not include the NULL).
//

typedef struct _DUMP_STRING {
    ULONG Length;                   // Length IN WCHARs of the string.
    WCHAR Buffer [0];               // Buffer.
} DUMP_STRING, * PDUMP_STRING;

#ifdef __cplusplus
}
#endif

#ifndef MIDL_PASS
#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning( default : 4200 ) // nonstandard extension used : zero-sized array in struct/union
#endif
#endif // MIDL_PASS

#endif // _NTIODUMP_
