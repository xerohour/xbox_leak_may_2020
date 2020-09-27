/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    fatx.h

Abstract:

    This module contains the private data structures and procedure prototypes
    for the FAT file system driver.

--*/

#ifndef _FATX_
#define _FATX_

#include <ntos.h>
#include <ntdddisk.h>
#include <limits.h>
#include "fat.h"

#include <pshpack4.h>

//
// DBG sensitive DbgPrint wrapper.
//

#if DBG
#define FatxDbgPrint(x)                         DbgPrint x
#else
#define FatxDbgPrint(x)
#endif

//
// Bit flag macros.
//

#define FatxIsFlagSet(flagset, flag)            (((flagset) & (flag)) != 0)
#define FatxIsFlagClear(flagset, flag)          (((flagset) & (flag)) == 0)

//
// Returns the number of elements in the supplied array.
//

#define ARRAY_ELEMENTS(array)                                                 \
    (sizeof((array)) / sizeof((array)[0]))

//
// Define file control block flags.
//

#define FAT_FCB_VOLUME                          0x01
#define FAT_FCB_DIRECTORY                       0x02
#define FAT_FCB_ROOT_DIRECTORY                  0x04
#define FAT_FCB_DISABLE_LAST_WRITE_TIME         0x08
#define FAT_FCB_DELETE_ON_CLOSE                 0x10
#define FAT_FCB_TRUNCATE_ON_CLOSE               0x20
#define FAT_FCB_UPDATE_DIRECTORY_ENTRY          0x40
#define FAT_FCB_FREE_FILE_NAME_BUFFER           0x80

//
// File control block cluster cache.
//

typedef struct _FAT_FCB_CLUSTER_CACHE_ENTRY {
    ULONG PhysicalClusterNumber : 24;
    ULONG LruFlink : 8;
    ULONG FileClusterNumber : 24;
    ULONG LruBlink : 8;
    ULONG ClusterRunLength;
} FAT_FCB_CLUSTER_CACHE_ENTRY, *PFAT_FCB_CLUSTER_CACHE_ENTRY;

#define FAT_DIRECTORY_CLUSTER_CACHE_ENTRIES     2
#define FAT_FILE_CLUSTER_CACHE_ENTRIES          10

//
// File control block.
//

typedef struct _FAT_FCB_BASE {
    UCHAR Flags;
    UCHAR PathNameLength;
    UCHAR FileNameLength;
    UCHAR FileAttributes;
    ULONG ReferenceCount;
    SHARE_ACCESS ShareAccess;
    UCHAR ClusterCacheLruHead;
} FAT_FCB_BASE, *PFAT_FCB_BASE;

typedef struct _FAT_FCB {
    FAT_FCB_BASE;
    LIST_ENTRY SiblingFcbLink;
    struct _FAT_FCB *ParentFcb;
    ULONG FirstCluster;
    ULONG EndingCluster;
    ULONG FileSize;
    ULONG AllocationSize;
    ULONG DirectoryByteOffset;
    POSTR FileNameBuffer;
    LARGE_INTEGER LastWriteTime;
    FAT_TIME_STAMP CreationTime;
    FAT_TIME_STAMP LastAccessTime;
    union {
        struct {
            ERWLOCK FileMutex;
#if DBG
            PKTHREAD FileMutexExclusiveOwner;
#endif
            FAT_FCB_CLUSTER_CACHE_ENTRY ClusterCache[FAT_FILE_CLUSTER_CACHE_ENTRIES];
            OCHAR OriginalFileName[0];
        } File;
        struct {
            LIST_ENTRY ChildFcbList;
            ULONG DirectoryByteOffsetLookupHint;
            FAT_FCB_CLUSTER_CACHE_ENTRY ClusterCache[FAT_DIRECTORY_CLUSTER_CACHE_ENTRIES];
            OCHAR OriginalFileName[0];
        } Directory;
    };
} FAT_FCB, *PFAT_FCB;

//
// Define the base sizes of the various types of file control blocks.
//

#define FAT_DIRECTORY_FCB_SIZE                  FIELD_OFFSET(FAT_FCB, Directory.OriginalFileName)
#define FAT_FILE_FCB_SIZE                       FIELD_OFFSET(FAT_FCB, File.OriginalFileName)

//
// Volume device extension data.
//

typedef struct _FAT_VOLUME_EXTENSION {
    union {
        FSCACHE_EXTENSION CacheExtension;
        FSCACHE_EXTENSION;
    };
    PFAT_FCB RootDirectoryFcb;
    ULONG FileObjectCount;
    ULONG NumberOfClusters;
    ULONG BytesPerCluster;
    UCHAR SectorShift;
    UCHAR ClusterShift;
    UCHAR FatEntryShift;
    UCHAR Flags;
    ULONGLONG FatByteOffset;
    ULONGLONG FileAreaByteOffset;
    ULONG NumberOfClustersAvailable;
    ULONG FirstAvailableClusterHint;
    ERWLOCK VolumeMutex;
    RTL_CRITICAL_SECTION ClusterCacheMutex;
    FAT_FCB_BASE VolumeFcb;
    ULONG SerialNumber;
    LONG DismountBlockCount;
    PKEVENT DismountUnblockEvent;
#if DBG
    PKTHREAD VolumeMutexExclusiveOwner;
#endif
} FAT_VOLUME_EXTENSION, *PFAT_VOLUME_EXTENSION;

//
// Define volume device extension flags.
//

#define FAT_VOLUME_DISMOUNTED                   0x01
#define FAT_VOLUME_FAT16X                       0x02

//
// Macro to round a size in bytes up to a multiple of the sector size.
//

#define FatxRoundToSectors(volume, size) \
    (((ULONG)(size) + ((volume)->SectorSize - 1)) & \
        ~((volume)->SectorSize - 1))

//
// Macro to check if a length is sector aligned.
//

#define FatxIsSectorAligned(volume, value) \
    ((LOGICAL)(((ULONG)(value) & ((volume)->SectorSize - 1)) == 0))

//
// Macro to verify that a cluster number is valid for the supplied volume.
//

#define FatxIsValidCluster(volume, cluster) \
    ((BOOLEAN)(((ULONG)((cluster) - FAT_RESERVED_FAT_ENTRIES)) < (volume)->NumberOfClusters))

//
// Macro to map a cluster number to the file area byte offset.
//

#define FatxClusterToPhysicalByteOffset(volume, cluster) \
    ((volume)->FileAreaByteOffset + \
        (((ULONGLONG)((cluster) - FAT_RESERVED_FAT_ENTRIES)) << (volume)->ClusterShift))

//
// Macro to map a cluster number to the file allocation table byte offset.
// Valid cluster numbers are limited to 28-bits, so we safely do the below shift
// with a 32-bit type.  This macro operates correctly for FAT16 and FAT32
// volumes.
//

#define FatxClusterToFatByteOffset(volume, cluster) \
    (((ULONG)(cluster)) << (volume)->FatEntryShift)

//
// Macro to convert a FAT16X entry to a FAT32 entry.  The special entries are
// extended to their FAT32 equivalents while normal file clusters are zero
// extended.
//

#define FatxConvertFat16XToFat32(cluster) \
    ((cluster) < FAT_CLUSTER16_RESERVED ? (cluster) : ((ULONG)(SHORT)(cluster)))

//
// Macro to read the cluster from the supplied FAT entry pointer.  This macro
// operates correctly for FAT16X and FAT32 volumes.
//

#define FatxReadFatEntry(volume, fatentry) \
    (FatxIsFlagClear((volume)->Flags, FAT_VOLUME_FAT16X) ? \
        (*(PULONG)fatentry) : \
        FatxConvertFat16XToFat32(*(PUSHORT)fatentry))

//
// Macro to write the cluster to the supplied FAT entry pointer.  This macro
// operates correctly for FAT16X and FAT32 volumes.
//

#define FatxWriteFatEntry(volume, fatentry, cluster) \
    (FatxIsFlagClear((volume)->Flags, FAT_VOLUME_FAT16X) ? \
        ((*(PULONG)fatentry) = (cluster)) : \
        ((*(PUSHORT)fatentry) = (USHORT)(cluster)))

//
// Macro to compute the offset into the cluster of the supplied byte offset.
//

#define FatxByteOffsetIntoCluster(volume, lbo) \
    ((ULONG)(lbo) & ((volume)->BytesPerCluster - 1))

//
// Macro to round a size in bytes up to a multiple of the cluster size.
//

#define FatxRoundToClusters(volume, size) \
    (((ULONG)(size) + ((volume)->BytesPerCluster - 1)) & \
        ~((volume)->BytesPerCluster - 1))

//
// Macro to safely increment the dismount block count for the volume.
//

#define FatxIncrementDismountBlockCount(volume) \
    InterlockedIncrement(&(volume)->DismountBlockCount)

//
// Macro to safely decrement the dismount block count for the volume and to take
// action if the count indicates that a dismount should occur.
//

#define FatxDecrementDismountBlockCount(volume) \
    if (InterlockedDecrement(&(volume)->DismountBlockCount) < 0) \
        FatxSignalDismountUnblockEvent(volume)

//
// Macro to filter out unrecognized directory entry attributes.
//

#define FatxFilterFileAttributes(attributes) \
    ((attributes) & (FILE_ATTRIBUTE_READONLY | \
    FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | \
    FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_ARCHIVE))

//
// Macro to verify that the directory entry attributes are valid.
//

#define FatxIsValidFileAttributes(attributes) \
    FatxIsFlagClear(attributes, ~(FILE_ATTRIBUTE_READONLY | \
    FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | \
    FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_ARCHIVE))

//
// Structures to support asynchronous I/O.
//

typedef struct _FAT_ASYNC_IO_ENTRY {
    ULONG PhysicalSector;
    ULONG PhysicalIoLength;
} FAT_ASYNC_IO_ENTRY, *PFAT_ASYNC_IO_ENTRY;

typedef struct _FAT_ASYNC_IO_DESCRIPTOR {
    ULONG MajorFunction;
    ULONG IoLength;
    ULONG IoLengthRemaining;
    ULONG BufferOffset;
    ULONG NextAsyncIoEntry;
    PFAT_FCB FileFcb;
    FAT_ASYNC_IO_ENTRY Entries[0];
} FAT_ASYNC_IO_DESCRIPTOR, *PFAT_ASYNC_IO_DESCRIPTOR;

//
// Structures to support cluster allocation.
//

typedef struct _FAT_CLUSTER_RUN {
    ULONG PhysicalClusterNumber;
    ULONG ClusterRunLength;
} FAT_CLUSTER_RUN, *PFAT_CLUSTER_RUN;

#define FAT_MAXIMUM_CLUSTER_RUNS                FAT_FILE_CLUSTER_CACHE_ENTRIES

//
// File system dismount routine.
//

NTSTATUS
FatxDismountVolume(
    IN PDEVICE_OBJECT DeviceObject
    );

//
// IRP dispatch routines.
//

NTSTATUS
FatxFsdCleanup(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
FatxFsdClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
FatxFsdCreate(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
FatxFsdDeviceControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
FatxFsdDirectoryControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
FatxFsdFileSystemControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
FatxFsdFlushBuffers(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
FatxFsdQueryInformation(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
FatxFsdQueryVolumeInformation(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
FatxFsdReadWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
FatxFsdSetInformation(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

//
// File control block support routines.
//

NTSTATUS
FatxCreateFcb(
    IN PFAT_FCB ParentFcb OPTIONAL,
    IN ULONG FirstCluster,
    IN PDIRENT DirectoryEntry OPTIONAL,
    IN ULONG DirectoryByteOffset OPTIONAL,
    OUT PFAT_FCB *ReturnedFcb
    );

BOOLEAN
FatxFindOpenChildFcb(
    IN PFAT_FCB DirectoryFcb,
    IN POBJECT_STRING FileName,
    OUT PFAT_FCB *ReturnedFcb
    );

NTSTATUS
FatxUpdateDirectoryEntry(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB Fcb
    );

NTSTATUS
FatxMarkDirectoryEntryDeleted(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB Fcb
    );

NTSTATUS
FatxFileByteOffsetToCluster(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB Fcb,
    IN ULONG FileByteOffset,
    OUT PULONG ReturnedClusterNumber,
    OUT PULONG ReturnedClusterRunLength OPTIONAL
    );

NTSTATUS
FatxFileByteOffsetToPhysicalByteOffset(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB Fcb,
    IN ULONG FileByteOffset,
    IN BOOLEAN AcquireVolumeMutexShared,
    OUT PULONGLONG ReturnedPhysicalByteOffset,
    OUT PULONG ReturnedPhysicalRunLength
    );

VOID
FatxAppendClusterRunsToClusterCache(
    IN PFAT_FCB Fcb,
    IN ULONG FileClusterNumber,
    IN FAT_CLUSTER_RUN ClusterRuns[FAT_MAXIMUM_CLUSTER_RUNS],
    IN ULONG NumberOfClusterRuns
    );

NTSTATUS
FatxExtendFileAllocation(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB FileFcb,
    IN ULONG AllocationSize
    );

NTSTATUS
FatxSetAllocationSize(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB FileFcb,
    IN ULONG AllocationSize,
    IN BOOLEAN OverwritingFile,
    IN BOOLEAN DisableTruncation
    );

NTSTATUS
FatxExtendDirectoryAllocation(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB DirectoryFcb
    );

NTSTATUS
FatxIsDirectoryEmpty(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB DirectoryFcb
    );

NTSTATUS
FatxDeleteFile(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB DirectoryFcb,
    IN ULONG DirectoryByteOffset
    );

VOID
FatxDereferenceFcb(
    IN PFAT_FCB Fcb
    );

//
// File allocation table management routines.
//

NTSTATUS
FatxInitializeDirectoryCluster(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN ULONG ClusterNumber
    );

NTSTATUS
FatxLinkClusterChains(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN ULONG LinkClusterNumber,
    IN ULONG StartingClusterNumber
    );

NTSTATUS
FatxAllocateClusters(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN ULONG EndingCluster,
    IN ULONG ClustersNeeded,
    OUT FAT_CLUSTER_RUN ClusterRuns[FAT_MAXIMUM_CLUSTER_RUNS],
    OUT PULONG NumberOfClusterRuns,
    OUT PULONG ReturnedEndingCluster
    );

BOOLEAN
FatxFreeClusters(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN ULONG ClusterNumber,
    IN BOOLEAN MarkFirstAsLast
    );

NTSTATUS
FatxInitializeAllocationSupport(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp
    );

//
// Synchronization routines.
//

VOID
FatxAcquireGlobalMutexExclusive(
    VOID
    );

VOID
FatxReleaseGlobalMutex(
    VOID
    );

VOID
FatxAcquireVolumeMutexExclusive(
    IN PFAT_VOLUME_EXTENSION VolumeExtension
    );

VOID
FatxAcquireVolumeMutexShared(
    IN PFAT_VOLUME_EXTENSION VolumeExtension
    );

VOID
FatxReleaseVolumeMutex(
    IN PFAT_VOLUME_EXTENSION VolumeExtension
    );

#if DBG

VOID
FatxDpcReleaseVolumeMutex(
    IN PFAT_VOLUME_EXTENSION VolumeExtension
    );

#else

#define FatxDpcReleaseVolumeMutex(volume) \
    ExReleaseReadWriteLock(&(volume)->VolumeMutex)

#endif

VOID
FatxAcquireFileMutexExclusive(
    IN PFAT_FCB FileFcb
    );

VOID
FatxAcquireFileMutexShared(
    IN PFAT_FCB FileFcb
    );

VOID
FatxReleaseFileMutex(
    IN PFAT_FCB FileFcb
    );

#if DBG

VOID
FatxDpcReleaseFileMutex(
    IN PFAT_FCB FileFcb
    );

#else

#define FatxDpcReleaseFileMutex(filefcb) \
    ExReleaseReadWriteLock(&(filefcb)->File.FileMutex)

#endif

#define FatxAcquireClusterCacheMutex(volume) \
    RtlEnterCriticalSection(&(volume)->ClusterCacheMutex)

#define FatxReleaseClusterCacheMutex(volume) \
    RtlLeaveCriticalSection(&(volume)->ClusterCacheMutex)

//
// Miscellaneous routines.
//

BOOLEAN
FatxIsValidFatFileName(
    IN POBJECT_STRING FileName
    );

LARGE_INTEGER
FatxFatTimestampToTime(
    IN PFAT_TIME_STAMP FatTimestamp
    );

LOGICAL
FatxTimeToFatTimestamp(
    IN PLARGE_INTEGER Time,
    OUT PFAT_TIME_STAMP FatTimestamp
    );

LARGE_INTEGER
FatxRoundToFatTime(
    IN PLARGE_INTEGER Time
    );

NTSTATUS
FatxLookupElementNameInDirectory(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB DirectoryFcb,
    IN POBJECT_STRING ElementName,
    OUT PDIRENT ReturnedDirectoryEntry,
    OUT PULONG ReturnedDirectoryByteOffset,
    OUT PULONG ReturnedEmptyDirectoryByteOffset
    );

NTSTATUS
FatxFindNextDirectoryEntry(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB DirectoryFcb,
    IN ULONG DirectoryByteOffset,
    IN POBJECT_STRING TemplateFileName,
    OUT PDIRENT ReturnedDirectoryEntry,
    OUT PULONG ReturnedDirectoryByteOffset
    );

NTSTATUS
FatxVolumeIoCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

VOID
FatxDeleteVolumeDevice(
    IN PDEVICE_OBJECT VolumeDeviceObject
    );

VOID
FatxSignalDismountUnblockEvent(
    IN PFAT_VOLUME_EXTENSION VolumeExtension
    );

//
// External symbols.
//

extern DRIVER_OBJECT FatxDriverObject;

extern
UCHAR
FASTCALL
RtlFindFirstSetRightMember(
    IN ULONG Set
    );

#include <poppack.h>

#endif  // FATX
