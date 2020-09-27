/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    driver.c

Abstract:

    This module implements routines that apply to the driver object including
    initialization and IRP dispatch routines.

--*/

#include "gdfx.h"

//
// Driver object for the GDF file system.
//
DECLSPEC_RDATA DRIVER_OBJECT GdfxDriverObject = {
    NULL,                               // DriverStartIo
    NULL,                               // DriverDeleteDevice
    GdfxDismountVolume,                 // DriverDismountVolume
    {
        GdfxFsdCreate,                  // IRP_MJ_CREATE
        GdfxFsdClose,                   // IRP_MJ_CLOSE
        GdfxFsdRead,                    // IRP_MJ_READ
        IoInvalidDeviceRequest,         // IRP_MJ_WRITE
        GdfxFsdQueryInformation,        // IRP_MJ_QUERY_INFORMATION
        GdfxFsdSetInformation,          // IRP_MJ_SET_INFORMATION
        IoInvalidDeviceRequest,         // IRP_MJ_FLUSH_BUFFERS
        GdfxFsdQueryVolumeInformation,  // IRP_MJ_QUERY_VOLUME_INFORMATION
        GdfxFsdDirectoryControl,        // IRP_MJ_DIRECTORY_CONTROL
        GdfxFsdFileSystemControl,       // IRP_MJ_FILE_SYSTEM_CONTROL
        GdfxFsdDeviceControl,           // IRP_MJ_DEVICE_CONTROL
        IoInvalidDeviceRequest,         // IRP_MJ_INTERNAL_DEVICE_CONTROL
        IoInvalidDeviceRequest,         // IRP_MJ_SHUTDOWN
        IoInvalidDeviceRequest,         // IRP_MJ_CLEANUP
    }
};

//
// All file system access is guarded by this single global lock.
//
INITIALIZED_CRITICAL_SECTION(GdfxGlobalMutex);
