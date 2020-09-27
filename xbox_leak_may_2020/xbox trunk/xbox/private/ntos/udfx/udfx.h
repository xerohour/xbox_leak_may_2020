/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    udfx.h

Abstract:

    This module contains the private data structures and procedure prototypes
    for the UDF file system driver.

--*/

#ifndef _UDFX_
#define _UDFX_

#include <ntos.h>
#include <ntddcdrm.h>
#include <ntdddisk.h>
#include "iso13346.h"

#include <pshpack4.h>

//
// DBG sensitive DbgPrint wrapper.
//

#if DBG
#define UdfxDbgPrint(x)                         DbgPrint x
#else
#define UdfxDbgPrint(x)
#endif

//
// Bit flag macros.
//

#define UdfxIsFlagSet(flagset, flag)            (((flagset) & (flag)) != 0)
#define UdfxIsFlagClear(flagset, flag)          (((flagset) & (flag)) == 0)

//
// Returns the number of elements in the supplied array.
//

#define ARRAY_ELEMENTS(array)                                                 \
    (sizeof((array)) / sizeof((array)[0]))

//
// Define the recognized version numbers of the UDF specification.
//

#define UDF_VERSION_100                         0x0100
#define UDF_VERSION_101                         0x0101
#define UDF_VERSION_102                         0x0102
#define UDF_VERSION_150                         0x0150

#define UDF_VERSION_MINIMUM                     UDF_VERSION_100
#define UDF_VERSION_MAXIMUM                     UDF_VERSION_150

//
// Define the static drive geometry for a CD-ROM device supported by this file
// system.
//

#define UDF_CD_SECTOR_SHIFT                     11
#define UDF_CD_SECTOR_SIZE                      (1 << UDF_CD_SECTOR_SHIFT)
#define UDF_CD_SECTOR_MASK                      (UDF_CD_SECTOR_SIZE - 1)

//
// Define a macro to align the supplied number of bytes down or up to the next
// sector boundary.
//

#define UDF_CD_SECTOR_ALIGN_DOWN(cb) \
    (((SIZE_T)(cb) & (~UDF_CD_SECTOR_MASK))
#define UDF_CD_SECTOR_ALIGN_UP(cb) \
    (((SIZE_T)(cb) + UDF_CD_SECTOR_SIZE - 1) & (~UDF_CD_SECTOR_MASK))

//
// Define a macro to check if a length is sector aligned.
//

#define UdfxIsSectorAligned(value) \
    ((LOGICAL)(((ULONG)(value) & UDF_CD_SECTOR_MASK) == 0))

//
// Define file control block flags.
//

#define UDF_FCB_VOLUME                          0x01
#define UDF_FCB_DIRECTORY                       0x02
#define UDF_FCB_ROOT_DIRECTORY                  0x04
#define UDF_FCB_EMBEDDED_DATA                   0x08

//
// File control block.
//

typedef struct _UDF_FCB {
    UCHAR Flags;
    UCHAR FileNameLength;
    USHORT EmbeddedDataOffset;
    ULONG AllocationSectorStart;
    LARGE_INTEGER FileSize;
    TIMESTAMP ModifyTime;
    ULONG ReferenceCount;
    struct _UDF_FCB *ParentFcb;
    LIST_ENTRY SiblingFcbLink;
    LIST_ENTRY ChildFcbList;
    OCHAR FileName[0];
} UDF_FCB, *PUDF_FCB;

//
// Volume device extension data.
//

typedef struct _UDF_VOLUME_EXTENSION {
    union {
        FSCACHE_EXTENSION CacheExtension;
        FSCACHE_EXTENSION;
    };
    PUDF_FCB VolumeFcb;
    PUDF_FCB RootDirectoryFcb;
    ULONG PhysicalSectorCount;
    ULONG PartitionSectorStart;
    ULONG PartitionSectorCount;
    ULONG FileObjectCount;
    BOOLEAN Dismounted;
} UDF_VOLUME_EXTENSION, *PUDF_VOLUME_EXTENSION;

//
// File system dismount routine.
//

NTSTATUS
UdfxDismountVolume(
    IN PDEVICE_OBJECT DeviceObject
    );

//
// IRP dispatch routines.
//

NTSTATUS
UdfxFsdClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
UdfxFsdCreate(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
UdfxFsdDeviceControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
UdfxFsdDirectoryControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
UdfxFsdFileSystemControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
UdfxFsdQueryInformation(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
UdfxFsdQueryVolumeInformation(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
UdfxFsdRead(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
UdfxFsdSetInformation(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

//
// Cache support routines.
//

NTSTATUS
UdfxMapLogicalSector(
    IN PUDF_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN ULONG LogicalSectorNumber,
    OUT PVOID *CacheBuffer
    );

//
// File control block support routines.
//

NTSTATUS
UdfxCreateFcbFromFileEntry(
    IN PUDF_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PLONGAD IcbExtent,
    IN PUDF_FCB ParentFcb OPTIONAL,
    IN POBJECT_STRING FileName OPTIONAL,
    OUT PUDF_FCB *ReturnedFcb
    );

BOOLEAN
UdfxFindOpenChildFcb(
    IN PUDF_FCB DirectoryFcb,
    IN POBJECT_STRING FileName,
    OUT PUDF_FCB *ReturnedFcb
    );

VOID
UdfxDereferenceFcb(
    IN PUDF_FCB Fcb
    );

//
// Synchronization routines.
//

#define UdfxAcquireGlobalMutexExclusive() \
    RtlEnterCriticalSectionAndRegion(&UdfxGlobalMutex)

#define UdfxReleaseGlobalMutex() \
    RtlLeaveCriticalSectionAndRegion(&UdfxGlobalMutex)

//
// Miscellaneous routines.
//

BOOLEAN
UdfxVerifyDescriptor(
    IN PDESTAG Descriptor,
    IN ULONG DescriptorSize,
    IN USHORT Ident,
    IN ULONG LogicalSectorNumber
    );

BOOLEAN
UdfxEqualOSTACS0StringAndObjectString(
    IN PUCHAR OSTACS0String,
    IN SIZE_T OSTACS0StringLength,
    IN POBJECT_STRING ObjectString
    );

NTSTATUS
UdfxOSTACS0StringToObjectString(
    IN PUCHAR OSTACS0String,
    IN SIZE_T OSTACS0StringLength,
    OUT POBJECT_STRING ObjectString
    );

LARGE_INTEGER
UdfxUdfTimestampToTime(
    IN PTIMESTAMP UdfTimestamp
    );

VOID
UdfxDeleteVolumeDevice(
    IN PDEVICE_OBJECT VolumeDeviceObject
    );

//
// External symbols.
//

extern DRIVER_OBJECT UdfxDriverObject;
extern RTL_CRITICAL_SECTION UdfxGlobalMutex;

#include <poppack.h>

#endif  // UDFX
