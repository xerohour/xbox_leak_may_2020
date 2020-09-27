/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    rawx.h

Abstract:

    This module contains the private data structures and procedure prototypes
    for the RAW file system driver.

--*/

#ifndef _RAWX_
#define _RAWX_

#include <ntos.h>
#include <ntdddisk.h>
#include <ntddcdrm.h>

#include <pshpack4.h>

//
// DBG sensitive DbgPrint wrapper.
//

#if DBG
#define RawxDbgPrint(x)                         DbgPrint x
#else
#define RawxDbgPrint(x)
#endif

//
// Bit flag macros.
//

#define RawxIsFlagSet(flagset, flag)            (((flagset) & (flag)) != 0)
#define RawxIsFlagClear(flagset, flag)          (((flagset) & (flag)) == 0)

//
// Volume device extension data.
//

typedef struct _RAW_VOLUME_EXTENSION {
    PDEVICE_OBJECT TargetDeviceObject;
    SHARE_ACCESS ShareAccess;
    BOOLEAN Dismounted;
    ERWLOCK VolumeMutex;
} RAW_VOLUME_EXTENSION, *PRAW_VOLUME_EXTENSION;

//
// File system dismount routine.
//

NTSTATUS
RawxDismountVolume(
    IN PDEVICE_OBJECT DeviceObject
    );

//
// IRP dispatch routines.
//

NTSTATUS
RawxFsdCleanup(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
RawxFsdClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
RawxFsdCreate(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
RawxFsdFileSystemControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
RawxFsdQueryInformation(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
RawxFsdQueryVolumeInformation(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
RawxFsdSetInformation(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
RawxPassIrpDownToTargetDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

//
// Synchronization routines.
//

VOID
RawxAcquireGlobalMutexExclusive(
    VOID
    );

VOID
RawxReleaseGlobalMutex(
    VOID
    );

VOID
RawxAcquireVolumeMutexExclusive(
    IN PRAW_VOLUME_EXTENSION VolumeExtension
    );

VOID
RawxAcquireVolumeMutexShared(
    IN PRAW_VOLUME_EXTENSION VolumeExtension
    );

VOID
RawxReleaseVolumeMutex(
    IN PRAW_VOLUME_EXTENSION VolumeExtension
    );

#define RawxDpcReleaseVolumeMutex(volume) \
    ExReleaseReadWriteLock(&(volume)->VolumeMutex)

//
// Miscellaneous routines.
//

VOID
RawxDeleteVolumeDevice(
    IN PDEVICE_OBJECT VolumeDeviceObject
    );

//
// External symbols.
//

extern DRIVER_OBJECT RawxDriverObject;

#include <poppack.h>

#endif  // RAWX
