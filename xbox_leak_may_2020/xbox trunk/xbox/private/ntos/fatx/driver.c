/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    driver.c

Abstract:

    This module implements routines that apply to the driver object including
    initialization and IRP dispatch routines.

--*/

#include "fatx.h"

//
// Driver object for the FAT file system.
//
DECLSPEC_RDATA DRIVER_OBJECT FatxDriverObject = {
    NULL,                               // DriverStartIo
    NULL,                               // DriverDeleteDevice
    FatxDismountVolume,                 // DriverDismountVolume
    {
        FatxFsdCreate,                  // IRP_MJ_CREATE
        FatxFsdClose,                   // IRP_MJ_CLOSE
        FatxFsdReadWrite,               // IRP_MJ_READ
        FatxFsdReadWrite,               // IRP_MJ_WRITE
        FatxFsdQueryInformation,        // IRP_MJ_QUERY_INFORMATION
        FatxFsdSetInformation,          // IRP_MJ_SET_INFORMATION
        FatxFsdFlushBuffers,            // IRP_MJ_FLUSH_BUFFERS
        FatxFsdQueryVolumeInformation,  // IRP_MJ_QUERY_VOLUME_INFORMATION
        FatxFsdDirectoryControl,        // IRP_MJ_DIRECTORY_CONTROL
        FatxFsdFileSystemControl,       // IRP_MJ_FILE_SYSTEM_CONTROL
        FatxFsdDeviceControl,           // IRP_MJ_DEVICE_CONTROL
        IoInvalidDeviceRequest,         // IRP_MJ_INTERNAL_DEVICE_CONTROL
        IoInvalidDeviceRequest,         // IRP_MJ_SHUTDOWN
        FatxFsdCleanup,                 // IRP_MJ_CLEANUP
    }
};
