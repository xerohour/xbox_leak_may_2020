/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    driver.c

Abstract:

    This module implements routines that apply to the driver object including
    initialization and IRP dispatch routines.

--*/

#include "udfx.h"

//
// Driver object for the UDF file system.
//
DECLSPEC_RDATA DRIVER_OBJECT UdfxDriverObject = {
    NULL,                               // DriverStartIo
    NULL,                               // DriverDeleteDevice
    UdfxDismountVolume,                 // DriverDismountVolume
    {
        UdfxFsdCreate,                  // IRP_MJ_CREATE
        UdfxFsdClose,                   // IRP_MJ_CLOSE
        UdfxFsdRead,                    // IRP_MJ_READ
        IoInvalidDeviceRequest,         // IRP_MJ_WRITE
        UdfxFsdQueryInformation,        // IRP_MJ_QUERY_INFORMATION
        UdfxFsdSetInformation,          // IRP_MJ_SET_INFORMATION
        IoInvalidDeviceRequest,         // IRP_MJ_FLUSH_BUFFERS
        UdfxFsdQueryVolumeInformation,  // IRP_MJ_QUERY_VOLUME_INFORMATION
        UdfxFsdDirectoryControl,        // IRP_MJ_DIRECTORY_CONTROL
        UdfxFsdFileSystemControl,       // IRP_MJ_FILE_SYSTEM_CONTROL
        UdfxFsdDeviceControl,           // IRP_MJ_DEVICE_CONTROL
        IoInvalidDeviceRequest,         // IRP_MJ_INTERNAL_DEVICE_CONTROL
        IoInvalidDeviceRequest,         // IRP_MJ_SHUTDOWN
        IoInvalidDeviceRequest,         // IRP_MJ_CLEANUP
    }
};

//
// All file system access is guarded by this single global lock.
//
INITIALIZED_CRITICAL_SECTION(UdfxGlobalMutex);
