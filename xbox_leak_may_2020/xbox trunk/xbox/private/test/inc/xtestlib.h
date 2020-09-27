/*++

Copyright (c) 1999-2000  Microsoft Corporation

Module Name:

    xtestlib.h

Abstract:

    This module contains the forward declaration of common routines
    in xtestlib

Author:

    Sakphong Chanbai (schanbai) 13-Mar-2000

Revision History:

--*/

#ifndef __XTESTLIB_H__
#define __XTESTLIB_H__

#ifdef __cplusplus
extern "C" {
#endif


VOID
NTAPI
FlushDiskCache(
    VOID
    );


VOID
NTAPI
RebootSystem(
    VOID
    );


BOOLEAN
NTAPI
IniRemoveStringCommentA(
    IN OUT PSTR String
    );


BOOLEAN
NTAPI
IniRemoveStringCommentW(
    IN OUT PWSTR String
    );

INT
NTAPI
GetIniInt( 
	IN LPCSTR	lpAppName,					// section name
	IN LPCSTR	lpKeyName,					// key name
	IN INT		nDefault,					// return value if key name not found
	IN LPCSTR	lpFileName					// initialization file name
);

FLOAT
NTAPI
GetIniFloat( 
	IN LPCSTR	lpAppName,					// section name
	IN LPCSTR	lpKeyName,					// key name
	IN FLOAT	fDefault,					// return value if key name not found
	IN LPCSTR	lpFileName					// initialization file name
);

INT
NTAPI
GetIniString( 
	IN LPCSTR	lpAppName,					// section name
	IN LPCSTR	lpKeyName,					// key name
	IN LPCSTR	lpDefault,					// return value if key name not found
	OUT LPSTR	lpReturnedString,			// holds the returned string
	IN DWORD	nSize,						// max size of returned string
	IN LPCSTR	lpFileName					// initialization file name
);


LPWSTR *
WINAPI
CommandLineToArgvW(
    IN LPCWSTR lpCmdLine,
    OUT int * pNumArgs
    );


LPSTR *
WINAPI
CommandLineToArgvA(
    IN LPCSTR lpCmdLine,
    OUT int * pNumArgs
    );


CHAR
WINAPI
XTestLibGetTestNameSpace(
    VOID
    );


CHAR
WINAPI
XTestLibGetTempNameSpace(
    VOID
    );


BOOL
WINAPI
XTestLibIsStressTesting(
    VOID
    );


BOOL
XTestLibIsRunningInDevkit(
    VOID
    );


ULONG
WINAPI
SetAllocationTrackingOptions(
    ULONG Flags
    );


BOOL
WINAPI
EnableAllocationTracking(
    HMODULE XeModuleHandle
    );


typedef struct _PRIVATE_HEAP_RECORD {

    HANDLE HeapHandle;
    ULONG  OwnerModuleIndex;

} PRIVATE_HEAP_RECORD, *PPRIVATE_HEAP_RECORD;


#define MAX_HEAP_HANDLE_TRACKING 1024


CONST PCHAR
WINAPI
XeModuleNameFromModuleIndex(
    ULONG ModuleIndex
    );


CONST PCHAR
WINAPI
XeModuleNameFromModuleHeader(
    PVOID ModuleHeader
    );


ULONG
WINAPI
XeModuleIndexFromAddress(
    ULONG_PTR Address
    );


CONST PVOID
WINAPI
XeModuleHeaderFromModuleIndex(
    ULONG ModuleIndex
    );


CONST PVOID
WINAPI
XeModuleHeaderFromModuleName(
    LPCSTR ModuleName
    );


PVOID
WINAPI
XTestLibLocateXImageHeader(
    VOID
    );


BOOL
WINAPI
GetHeapTrackingDatabase(
    PPRIVATE_HEAP_RECORD HeapRecordPtr,
    SIZE_T BufferSize
    );


VOID
QueryCpuTimesInformation(
    PLARGE_INTEGER KernelTime,
    PLARGE_INTEGER IdleTime,
    PLARGE_INTEGER DpcTime OPTIONAL,
    PLARGE_INTEGER InterruptTime OPTIONAL,
    PLARGE_INTEGER UserTime OPTIONAL
    );


LPSTR
WINAPI
NtStatusSymbolicName(
    IN  LONG  Status
);

LPSTR
WINAPI
WinErrorSymbolicName(
    IN  DWORD  LastError
);

#if defined( UNICODE ) || defined( _UNICODE )

#define IniRemoveStringComment     IniRemoveStringCommentW
#define CommandLineToArgv          CommandLineToArgvW

#else // UNICODE

#define IniRemoveStringComment     IniRemoveStringCommentA
#define CommandLineToArgv          CommandLineToArgvA

#endif // UNICODE


#include <profilep.h>


//
// Export table entry
//
typedef struct _EXP_TABLE_ENTRY {
    PCHAR pProcName;
    PROC pfnProc;
} EXP_TABLE_ENTRY, *PEXP_TABLE_ENTRY;


//
// Export table directory
//
typedef struct _EXP_TABLE_DIRECTORY {
    PCHAR ModuleName;
    PEXP_TABLE_ENTRY TableEntry;
} EXP_TABLE_DIRECTORY, *PEXP_TABLE_DIRECTORY;


//
// Useful macros to export functions table
//
#ifdef __cplusplus
#define __EXTRN_C extern "C"
#else
#define __EXTRN_C
#endif


#define EXPORT_SECTION_NAME ".EXP$U"


//
// Macro to define export table entry
//
#define BEGIN_EXPORT_TABLE( ModuleName ) \
    __EXTRN_C EXP_TABLE_ENTRY ModuleName##_ExportTableEntry[] = {

#define EXPORT_TABLE_ENTRY( ProcName, Proc ) \
        { ProcName, (PROC)Proc },


#define END_EXPORT_TABLE( ModuleName ) \
        { NULL, NULL } \
    };

#define DECLARE_EXPORT_DIRECTORY( ModuleName ) \
    extern EXP_TABLE_ENTRY ModuleName##_ExportTableEntry[]; \
    __EXTRN_C EXP_TABLE_DIRECTORY ModuleName##_ExportTableDirectory = { \
        #ModuleName, \
        ModuleName##_ExportTableEntry \
    };


#ifdef __cplusplus
}
#endif

#endif // __XTESTLIB_H__
