/*++

Copyright (c) 1999-2002  Microsoft Corporation

Module Name:

    xapip.h

Abstract:

    Contains common precompiled headers for kernel-mode
    Win32-equivalents for xbox

--*/

#ifndef _XAPIP_
#define _XAPIP_

//  Disable DECLSPEC_IMPORT decoration of locally implemented APIs.
#define _KERNEL32_
#define _USER32_
#define _GDI32_
#define _WINMM_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include "ntos.h"
#include <ntrtl.h>
#include <nturtl.h>
#include <ntdddisk.h>
#ifdef __cplusplus
}
#endif // __cplusplus
#include <xtl.h>
#include <xdbg.h>
#include <xapidrv.h>
#include <ldr.h>
#include <xbeimage.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern const IMAGE_TLS_DIRECTORY _tls_used;
extern ULONG _tls_index;

//
// Default process heap handle.
//

extern HANDLE XapiProcessHeap;

//
// General critical section used to guard XAPI data structures.
//

extern RTL_CRITICAL_SECTION XapiProcessLock;

#define XapiAcquireProcessLock() RtlEnterCriticalSection(&XapiProcessLock)
#define XapiReleaseProcessLock() RtlLeaveCriticalSection(&XapiProcessLock)

typedef struct _X_CACHE_DB_ENTRY
{
    DWORD    dwTitleId;
    ULONG    nCacheIndex;
    BOOL     fUsed;
} X_CACHE_DB_ENTRY, *PX_CACHE_DB_ENTRY;


FORCEINLINE BOOL XapiIsXapiThread()
{
    return (KeGetCurrentIrql() < DISPATCH_LEVEL &&
        KeGetCurrentThread()->TlsData != NULL);
}

//
// Prototypes
//

VOID
XapiInitProcess(
    VOID
    );

VOID
XapiBootToDash(
    DWORD dwReason,
    DWORD dwParameter1,
    DWORD dwParameter2
    );

BOOL
WINAPI
XapiFormatFATVolume(
    IN POBJECT_STRING pcVolume
    );

BOOL
WINAPI
XapiFormatFATVolumeEx(
    IN POBJECT_STRING pcVolume,
    IN ULONG BytesPerCluster
    );

NTSTATUS
XapiGetCachePartitions(
    IN PX_CACHE_DB_ENTRY pCacheEntriesBuffer,
    IN UINT cbBufferSize,
    OUT PDWORD pdwNumCacheEntries
    );

VOID
XapiDeleteCachePartition(
    IN DWORD dwTitleId
    );

NTSTATUS
XapiValidateDiskPartition(
    POBJECT_STRING PartitionName
    );

NTSTATUS
XapiValidateDiskPartitionEx(
    POBJECT_STRING PartitionName,
    ULONG BytesPerCluster
    );

NTSTATUS
XapiMapLetterToDirectory(
    PCOBJECT_STRING pcDriveString,
    PCOBJECT_STRING pcPathString,
    PCOSTR pcszTitleId,
    BOOL fCreateDirectory,
    LPCWSTR pcszTitleName,
    BOOL fUpdateTimestamp
    );

BOOL
XapiDeleteValueInMetaFile(
    HANDLE hMetaFile,
    LPCWSTR pszTag
    );

void
XapiInitAutoPowerDown();

#define CONSTANT_OBJECT_STRING(s)   { sizeof( s ) - sizeof( OCHAR ), sizeof( s ), s }

#define DwordToStringO(dword, dwordstr) \
{ \
    soprintf(dwordstr, OTEXT("%08lx"), dword); \
}

#define CCHMAX_HEX_DWORD 9


//
//  Exported by MU driver
//  used only by XAPI.
//
NTSTATUS
MU_CreateDeviceObject(
    IN  ULONG            Port,
    IN  ULONG            Slot,
    IN  POBJECT_STRING  DeviceName
    );

VOID
MU_CloseDeviceObject(
    IN  ULONG  Port,
    IN  ULONG  Slot
    );

PDEVICE_OBJECT
MU_GetExistingDeviceObject(
    IN  ULONG  Port,
    IN  ULONG  Slot
    );

#ifdef DBG
extern ULONG MU_MaxUserDevices;
#endif

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _XAPIP_

