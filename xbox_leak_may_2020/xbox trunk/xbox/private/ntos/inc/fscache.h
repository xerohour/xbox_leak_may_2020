/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    fscache.h

Abstract:

    This module contains the public data structures and procedure prototypes
    for the file system cache.

--*/

#ifndef _FSCACHE_
#define _FSCACHE_

#include <pshpack4.h>

//
// Structure to contain the per device extension data required by the file
// system cache module.
//

typedef struct _FSCACHE_EXTENSION {
    PDEVICE_OBJECT TargetDeviceObject;
    LARGE_INTEGER PartitionLength;
    ULONG SectorSize;
} FSCACHE_EXTENSION, *PFSCACHE_EXTENSION;

//
// Structure that describes a cache entry (exposed in this header for the kernel
// debugger).
//

typedef struct _FSCACHE_ELEMENT {
    ULONG BlockNumber;
    PFSCACHE_EXTENSION CacheExtension;
    union {
        struct {
            ULONG UsageCount : 8;
            ULONG ReadInProgress : 1;
            ULONG ReadWaiters : 1;
            ULONG DeletePending : 1;
            ULONG Reserved : 1;
            ULONG CacheBufferBits : 20;
        };
        PCHAR CacheBuffer;
    };
    LIST_ENTRY ListEntry;
} FSCACHE_ELEMENT, *PFSCACHE_ELEMENT;

//
// Define the maximum number of pages that can be allocated to the file system
// cache.
//

#define FSCACHE_MAXIMUM_NUMBER_OF_CACHE_PAGES   2048

//
// Define the number of page table entries that will be used for FscWriteFFs.
//

#define FSCACHE_NUMBER_OF_WRITE_FF_PTES         8

//
// Prototypes.
//

NTKERNELAPI
NTSTATUS
FscSetCacheSize(
    IN PFN_COUNT NumberOfCachePages
    );

NTKERNELAPI
PFN_COUNT
FscGetCacheSize(
    VOID
    );

NTSTATUS
FscMapBuffer(
    IN PFSCACHE_EXTENSION CacheExtension,
    IN PIRP Irp,
    IN ULONGLONG ByteOffset,
    IN BOOLEAN MapAsReadWrite,
    OUT PVOID *CacheBuffer
    );

NTSTATUS
FscMapEmptyBuffer(
    IN PFSCACHE_EXTENSION CacheExtension,
    IN ULONGLONG ByteOffset,
    OUT PVOID *CacheBuffer
    );

NTSTATUS
FscWriteBuffer(
    IN PFSCACHE_EXTENSION CacheExtension,
    IN PIRP Irp,
    IN ULONGLONG ByteOffset,
    IN ULONG WriteLength,
    IN PVOID CacheBuffer
    );

VOID
FscDiscardBuffer(
    PVOID CacheBuffer
    );

VOID
FscUnmapBuffer(
    PVOID CacheBuffer
    );

NTSTATUS
FscWriteFFs(
    IN PFSCACHE_EXTENSION CacheExtension,
    IN PIRP Irp,
    IN ULONGLONG ByteOffset,
    IN ULONG WriteLength
    );

BOOLEAN
FscTestForFullyCachedIo(
    IN PIRP Irp,
    IN ULONG ByteOffsetLowPart,
    IN ULONG IoLength,
    IN BOOLEAN NonCachedEndOfFileTransfer
    );

NTSTATUS
FscCachedRead(
    IN PFSCACHE_EXTENSION CacheExtension,
    IN PIRP Irp,
    IN ULONGLONG ByteOffset,
    IN ULONG ReadLength,
    IN BOOLEAN NonCachedEndOfFileTransfer
    );

VOID
FscInvalidateDevice(
    IN PFSCACHE_EXTENSION CacheExtension
    );

VOID
FscInvalidateByteRange(
    IN PFSCACHE_EXTENSION CacheExtension,
    IN ULONGLONG ByteOffset,
    IN ULONG Length
    );

NTKERNELAPI
VOID
FscInvalidateIdleBlocks(
    VOID
    );

#include <poppack.h>

#endif  // FSCACHE
