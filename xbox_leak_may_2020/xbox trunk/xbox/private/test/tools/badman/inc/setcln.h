/*****************************************************************
MODULE: SetCln.h
AUTHOR: JohnMil
DATE:   1-20-92

Copyright (c) 1992 Microsoft Corporation

This contains definitions required for the xxxxSetup and xxxxCleanup
functions.
*****************************************************************/


#include	<stdlib.h>
#include	<stdio.h>

#include	<nt.h>
#include	<ntrtl.h>
#include	<nturtl.h>

#include	<xtl.h>


/*
 * Used with bad pointer stuff, and GetNativeBuffer.  I want something that's
 * too small for a DWORD.
 */

#define SMALL_NATIVE_BUFFER_SIZE    2


/*
 * This is a convienient structure to use when you have to make multiple
 * allocations for a setup.  Just put what you need into the list and at
 * cleanup time walk the list.
 */

typedef struct  _se_cleanup_info {
    struct _se_cleanup_info *se_Next;
    DWORD           se_AllocType;
    DWORD           se_AllocSize;
    VOID            *se_Address;
} SE_CLEANUP_INFO;


#define CLEANUP_LINK(x, y)  ((x) -> se_Next = Temp)
#define GET_REQUIRED_SIZE   (-1)


void
ErrorPrint(
    HANDLE          hConOut,
    HANDLE          hLog,
    LPSTR           Module,
    int             CaseNo,
    LPSTR           Message
);


int
SCPrintf(
    HANDLE          hConOut,
    LPSTR           Format,
    ...
);


VOID
InfThread(
    VOID
);


PVOID
GetNativeBuffer(
    DWORD                   NumberOfBytes
);


VOID
FreeNativeBuffer(
    PVOID                   Pointer,
    DWORD                   NumberOfBytes
);


// this stuff goes into the pointer support

typedef enum    _pointer_type {
    NULL_POINTER,
    INVALID_POINTER,
    VALIDRANGE_POINTER,
    MALALIGNED_POINTER_1,
    CLEANUP
} POINTER_TYPE;


BOOL
MakeBadPointer(
    PVOID                   *Pointer,
    SE_CLEANUP_INFO         **CleanupInfo,
    POINTER_TYPE            PointerType,
    DWORD                   PointerSize,
    HANDLE                  hConOut,
    HANDLE                  hLog
);


BOOL WINAPI
DLLEntry(
    HANDLE hInst,
    DWORD  dwReason,
    LPVOID lpv );
