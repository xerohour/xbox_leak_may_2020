/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    gdfx.h

Abstract:

    This module contains the private data structures and procedure prototypes
    for the GDF file system driver.

--*/

#ifndef _GDFX_
#define _GDFX_

#include <ntos.h>
#include <ntddcdrm.h>
#include <ntdddisk.h>
#include "gdformat.h"

#include <pshpack4.h>

//
// DBG sensitive DbgPrint wrapper.
//

#if DBG
#define GdfxDbgPrint(x)                         DbgPrint x
#else
#define GdfxDbgPrint(x)
#endif

//
// Bit flag macros.
//

#define GdfxIsFlagSet(flagset, flag)            (((flagset) & (flag)) != 0)
#define GdfxIsFlagClear(flagset, flag)          (((flagset) & (flag)) == 0)

//
// Returns the number of elements in the supplied array.
//

#define ARRAY_ELEMENTS(array)                                                 \
    (sizeof((array)) / sizeof((array)[0]))

//
// Define the static drive geometry for a CD-ROM device supported by this file
// system.
//

#define GDF_CD_SECTOR_SHIFT                     11
#define GDF_CD_SECTOR_SIZE                      (1 << GDF_CD_SECTOR_SHIFT)
#define GDF_CD_SECTOR_MASK                      (GDF_CD_SECTOR_SIZE - 1)

//
// Define a macro to align the supplied number of bytes down or up to the next
// sector boundary.
//

#define GDF_CD_SECTOR_ALIGN_DOWN(cb) \
    (((SIZE_T)(cb) & (~GDF_CD_SECTOR_MASK))
#define GDF_CD_SECTOR_ALIGN_UP(cb) \
    (((SIZE_T)(cb) + GDF_CD_SECTOR_SIZE - 1) & (~GDF_CD_SECTOR_MASK))

//
// Define a macro to check if a length is sector aligned.
//

#define GdfxIsSectorAligned(value) \
    ((LOGICAL)(((ULONG)(value) & GDF_CD_SECTOR_MASK) == 0))

//
// Define a macro to map a sector number to the physical byte offset.
//

#define GdfxSectorToPhysicalByteOffset(sector) \
    ((ULONGLONG)(sector) << GDF_CD_SECTOR_SHIFT)

//
// Define file control block flags.
//

#define GDF_FCB_VOLUME                          0x01
#define GDF_FCB_DIRECTORY                       0x02
#define GDF_FCB_ROOT_DIRECTORY                  0x04

//
// File control block.
//

typedef struct _GDF_FCB {
    ULONG FirstSector;
    ULONG FileSize;
    ULONG ReferenceCount;
    struct _GDF_FCB *ParentFcb;
    LIST_ENTRY SiblingFcbLink;
    LIST_ENTRY ChildFcbList;
    UCHAR Flags;
    UCHAR FileNameLength;
    OCHAR FileName[0];
} GDF_FCB, *PGDF_FCB;

//
// Volume device extension data.
//

typedef struct _GDF_VOLUME_EXTENSION {
    union {
        FSCACHE_EXTENSION CacheExtension;
        FSCACHE_EXTENSION;
    };
    PGDF_FCB VolumeFcb;
    PGDF_FCB RootDirectoryFcb;
    ULONG PartitionSectorCount;
    LARGE_INTEGER TimeStamp;
    ULONG FileObjectCount;
    BOOLEAN Dismounted;
} GDF_VOLUME_EXTENSION, *PGDF_VOLUME_EXTENSION;

//
// File system dismount routine.
//

NTSTATUS
GdfxDismountVolume(
    IN PDEVICE_OBJECT DeviceObject
    );

//
// IRP dispatch routines.
//

NTSTATUS
GdfxFsdClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
GdfxFsdCreate(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
GdfxFsdDeviceControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
GdfxFsdDirectoryControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
GdfxFsdFileSystemControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
GdfxFsdQueryInformation(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
GdfxFsdQueryVolumeInformation(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
GdfxFsdRead(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
GdfxFsdSetInformation(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

//
// Cache support routines.
//

NTSTATUS
GdfxMapLogicalSector(
    IN PGDF_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN ULONG LogicalSectorNumber,
    OUT PVOID *CacheBuffer
    );

//
// File control block support routines.
//

NTSTATUS
GdfxCreateFcb(
    IN PGDF_FCB ParentFcb OPTIONAL,
    IN POBJECT_STRING FileName,
    IN PGDF_DIRECTORY_ENTRY DirectoryEntry,
    OUT PGDF_FCB *ReturnedFcb
    );

BOOLEAN
GdfxFindOpenChildFcb(
    IN PGDF_FCB DirectoryFcb,
    IN POBJECT_STRING FileName,
    OUT PGDF_FCB *ReturnedFcb
    );

VOID
GdfxDereferenceFcb(
    IN PGDF_FCB Fcb
    );

//
// Synchronization routines.
//

#define GdfxAcquireGlobalMutexExclusive() \
    RtlEnterCriticalSectionAndRegion(&GdfxGlobalMutex)

#define GdfxReleaseGlobalMutex() \
    RtlLeaveCriticalSectionAndRegion(&GdfxGlobalMutex)

//
// Miscellaneous routines.
//

VOID
GdfxDeleteVolumeDevice(
    IN PDEVICE_OBJECT VolumeDeviceObject
    );

//
// External symbols.
//

extern DRIVER_OBJECT GdfxDriverObject;
extern RTL_CRITICAL_SECTION GdfxGlobalMutex;

#include <poppack.h>

#endif  // GDFX
