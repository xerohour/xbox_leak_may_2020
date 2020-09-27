/*++

Copyright (c) 2001-2002  Microsoft Corporation

Module Name:

    mountmu.c

Abstract:

    This module implements the XTL memory unit services.

--*/

#include "basedll.h"
#include <usbxapi.h>
#include <xboxp.h>
#include "..\..\fatx\fat.h"

#ifdef XAPILIBP

extern CRITICAL_SECTION XapiMountMUCriticalSection;

#ifdef DBG
extern ULONG g_XapiCurrentUserDevices;
#endif

#else  // XAPILIBP

INITIALIZED_CRITICAL_SECTION(XapiMountMUCriticalSection);

#ifdef DBG
ULONG g_XapiCurrentUserDevices = 0;
#endif

#endif // XAPILIBP

#ifndef XAPILIBP

DWORD
WINAPI
XMountMU(
    IN DWORD dwPort,
    IN DWORD dwSlot,
    OUT POCHAR pchDrive
    )
{
    NTSTATUS Status;
    OCHAR szTitleId[CCHMAX_HEX_DWORD];
    OCHAR szDeviceName[64];
    OCHAR szDosDevice[64];
    OCHAR chDrive;
    OBJECT_STRING DeviceName, DosDevice;

    RIP_ON_NOT_TRUE_WITH_MESSAGE(XPP_XInitDevicesHasBeenCalled, "XMountMU: XInitDevices must be called first!");

    if (NULL != pchDrive)
    {
        *pchDrive = OBJECT_NULL;
    }
    
    EnterCriticalSection(&XapiMountMUCriticalSection);

#if DBG
    if ((dwPort < MU_PORT_MIN) || (dwPort > MU_PORT_MAX))
    {
        RIP("XMountMU() invalid parameter (dwPort)");
    }

    if ((dwSlot < MU_SLOT_MIN) || (dwSlot > MU_SLOT_MAX))
    {
        RIP("XMountMU() invalid parameter (dwSlot)");
    }
    
    if(MU_MaxUserDevices <= g_XapiCurrentUserDevices)
    {
        RIP("XMountMU() more devices than specified in XInitDevices");
    }
#endif // DBG

    //
    //  Make sure that the device is not already mounted.
    //  (if it is, we sort of fail)
    chDrive = MU_DRIVE_LETTER_FROM_PORT_SLOT(dwPort, dwSlot);
    if(MU_IS_MOUNTED(chDrive))
    {
        RIP("XMountMU() MU already mounted.");
        *pchDrive = chDrive;
        LeaveCriticalSection(&XapiMountMUCriticalSection);
        return ERROR_ALREADY_ASSIGNED;
    }

    //
    // Setup the string buffer
    //
    DeviceName.Length = 0;
    DeviceName.MaximumLength = sizeof(szDeviceName)/sizeof(OCHAR)-1;
    DeviceName.Buffer = szDeviceName;

    //
    //  create the device object
    //
    Status = MU_CreateDeviceObject(
                dwPort,
                dwSlot,
                &DeviceName
                );

    if(NT_SUCCESS(Status))
    {
        soprintf(szDosDevice,
                 OTEXT("\\??\\%c:"),
                 chDrive);

        RtlInitObjectString(&DosDevice, szDosDevice);

        DwordToStringO(XeImageHeader()->Certificate->TitleID, szTitleId);

        //
        //  Tack on a '\\' to the end.  This way we are passing the MU's root directory,
        //  rather than the raw volume.  This is important to prevent the raw file system
        //  from mounting by mistake on a corrupt or unformatted MU.  The main evil of
        //  mounting the raw file system is that the error codes coming will be really
        //  strange.
        //
        ASSERT(DeviceName.Length+sizeof(OCHAR)<=DeviceName.MaximumLength);
        DeviceName.Buffer[DeviceName.Length++ / sizeof(OCHAR)] = OTEXT('\\');

        Status = XapiMapLetterToDirectory((PCOBJECT_STRING)&DosDevice,
                                          (PCOBJECT_STRING)&DeviceName,
                                          szTitleId,
                                          TRUE,
                                          XeImageHeader()->Certificate->TitleName,
                                          FALSE);

        if (NT_SUCCESS(Status))
        {
            if (NULL != pchDrive)
            {
                *pchDrive = chDrive;
            }
            MU_SET_MOUNTED(chDrive);
            #if DBG
            g_XapiCurrentUserDevices++;
            #endif
        }
        else
        {
            // Dismount the volume if necessary.
            IoDismountVolumeByName(&DeviceName);

            // We must assume that either a handle was successfully opened and
            // the filesystem was dismounted, or that it never managed to mount.
            // If not, then the following step breaks all further attempts to
            // mount MU's, but things were probably pretty well broken before
            // this anyway.
            MU_CloseDeviceObject(dwPort, dwSlot);
        }
    }
		
    LeaveCriticalSection(&XapiMountMUCriticalSection);

    return RtlNtStatusToDosError(Status);
}


DWORD
WINAPI
XMountMURoot(
    IN DWORD dwPort,
    IN DWORD dwSlot,
    OUT POCHAR pchDrive
    )
{
    NTSTATUS Status;
    OCHAR szDeviceName[64];
    OCHAR szDosDevice[64];
    OCHAR chDrive;
    OBJECT_STRING DeviceName, DosDevice;
    OBJECT_ATTRIBUTES Obja;
    HANDLE DirHandle;
    IO_STATUS_BLOCK IoStatusBlock;

    //
    // Fill out the MU type so that XAPI can
    // find it without explicitly referencing it.
    // This way the MU driver is not sucked in
    // by XAPI unless the MU is used.
    //

    if (NULL != pchDrive)
    {
        *pchDrive = OBJECT_NULL;
    }

    EnterCriticalSection(&XapiMountMUCriticalSection);
    
#if DBG
    if ((dwPort < MU_PORT_MIN) || (dwPort > MU_PORT_MAX))
    {
        RIP("XMountMURoot() invalid parameter (dwPort)");
    }

    if ((dwSlot < MU_SLOT_MIN) || (dwSlot > MU_SLOT_MAX))
    {
        RIP("XMountMURoot() invalid parameter (dwSlot)");
    }
    if(MU_MaxUserDevices <= g_XapiCurrentUserDevices)
    {
        RIP("XMountMU() more devices than specified in XInitDevices");
    }
#endif // DBG

    //
    //  Make sure that the device is not already mounted.
    //  (if it is, we sort of fail)
    chDrive = MU_DRIVE_LETTER_FROM_PORT_SLOT(dwPort, dwSlot);
    if(MU_IS_MOUNTED(chDrive))
    {
        RIP("XMountMU() MU already mounted.");
        *pchDrive = chDrive;
        LeaveCriticalSection(&XapiMountMUCriticalSection);
        return ERROR_ALREADY_ASSIGNED;
    }

    //
    // Setup the string buffer
    //
    DeviceName.Length = 0;
    //lie - so we have guaranteed space for a back slash (lie leave room for NULL, and a '\\')
    DeviceName.MaximumLength = sizeof(szDeviceName)/sizeof(OCHAR)-2;
    DeviceName.Buffer = szDeviceName;

    //
    //  create the device object
    //
    Status = MU_CreateDeviceObject(
                dwPort,
                dwSlot,
                &DeviceName
                );

    if(NT_SUCCESS(Status))
    {
        //We previously lied about the maximum length to guarantee room for a backslash
        //undo that lie.
        DeviceName.MaximumLength++;

        //Add a back slash to the name returned from MU_CreateDeviceObject
        szDeviceName[DeviceName.Length++] = OTEXT('\\');
        szDeviceName[DeviceName.Length] = OTEXT('\0');

        //Attempt to open the root directory (this effectively mounts the drive).
        InitializeObjectAttributes(
            &Obja,
            (POBJECT_STRING)&DeviceName,
            OBJ_CASE_INSENSITIVE,
            NULL,
            NULL
            );

        Status = NtCreateFile(
                    &DirHandle,
                    FILE_LIST_DIRECTORY | SYNCHRONIZE,
                    &Obja,
                    &IoStatusBlock,
                    NULL,
                    FILE_ATTRIBUTE_NORMAL,
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    FILE_OPEN_IF,
                    FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT
                    );

        //Whack the backslash off the device name
        szDeviceName[--DeviceName.Length] = OTEXT('\0');

        //If we succeeded in opening the root directory we can
        //create the symbolic link, etc.
        if (NT_SUCCESS(Status))
        {
            NtClose(DirHandle);  //Close the handle, we no longer need it

            soprintf(szDosDevice,
                 OTEXT("\\??\\%c:"),
                 chDrive);
            RtlInitObjectString(&DosDevice, szDosDevice);

            Status = IoCreateSymbolicLink(&DosDevice, &DeviceName);
            if(NT_ERROR(Status))
            {
                XDBGWRN("XAPI", "XMountMURoot() - IoCreateSymbolicLink failed with 0x%0.8x", Status);
            }
        }

        if (NT_SUCCESS(Status))
        {
            if (NULL != pchDrive)
            {
                *pchDrive = chDrive;
            }
            MU_SET_MOUNTED(chDrive);
	        #if DBG
            g_XapiCurrentUserDevices++;
            #endif
        } else
        //
        //  Failed to open the root or to create a symbolic link.  This does not mean a filesystem
        //  didn't mount, just in case we must force a dismount.
        {
            IoDismountVolumeByName(&DeviceName);

            //We must assume that either a handle was successfully opened and the filesystem was dismounted, or
            //that it never managed to mount.  If not, then the following step breaks all further attempts to
            //mount MU's, but things were probably pretty well broken before this anyway.
            MU_CloseDeviceObject(dwPort, dwSlot);
        }
    }

    LeaveCriticalSection(&XapiMountMUCriticalSection);

    return RtlNtStatusToDosError(Status);
}


DWORD
WINAPI
XUnmountMU(
    IN DWORD dwPort,
    IN DWORD dwSlot
    )
{
    NTSTATUS Status;
    OCHAR szDosDevice[64];
    OBJECT_STRING DosDevice;
    OBJECT_ATTRIBUTES Obja;
    HANDLE VolHandle;
    IO_STATUS_BLOCK IoStatusBlock;
    OCHAR chDrive = MU_DRIVE_LETTER_FROM_PORT_SLOT(dwPort, dwSlot);

#if DBG
    if ((dwPort < MU_PORT_MIN) || (dwPort > MU_PORT_MAX))
    {
        RIP("XUnmountMU() invalid parameter (dwPort)");
    }

    if ((dwSlot < MU_SLOT_MIN) || (dwSlot > MU_SLOT_MAX))
    {
        RIP("XUnmountMU() invalid parameter (dwSlot)");
    }
#endif // DBG

    EnterCriticalSection(&XapiMountMUCriticalSection);

    //
    //  Fail if the device is not already mounted.
    //
    if(!MU_IS_MOUNTED(chDrive))
    {
        RIP("XUnmountMU() MU not mounted.");
        LeaveCriticalSection(&XapiMountMUCriticalSection);
        return ERROR_INVALID_DRIVE;
    }

    //
    //  Unmount an alternate drive if it is mapped.
    //
    if (XapiMuInfo.DriveWithAltDriveMapped == chDrive)
    {
        //
        // An alternate drive is mapped to this MU drive, so let's auto dismount it here
        //

        XDBGWRN("XAPI", "XUnmountMU() called on MU drive (%c:) with alternate drive mapped (%c:)",
                chDrive, HD_ALT_UDATA_DRIVE);

        XUnmountAlternateTitle(HD_ALT_UDATA_DRIVE);
    }

    //
    //  open the symbolic link
    //
    soprintf(szDosDevice, OTEXT("\\??\\%c:"), chDrive);
    RtlInitObjectString(&DosDevice, szDosDevice);

    InitializeObjectAttributes(
        &Obja,
        (POBJECT_STRING)&DosDevice,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
        );

    Status = NtCreateFile(
                    &VolHandle,
                    SYNCHRONIZE,
                    &Obja,
                    &IoStatusBlock,
                    NULL,
                    FILE_ATTRIBUTE_NORMAL,
                    0,
                    FILE_OPEN,
                    FILE_SYNCHRONOUS_IO_NONALERT
                    );

    //
    //  send FSCTL_DISMOUNT_VOLUME
    //
    if(NT_SUCCESS(Status))
    {
        Status = NtFsControlFile(VolHandle, NULL, NULL, NULL, &IoStatusBlock, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0);
        NtClose(VolHandle); //Close handle whether the dismount succeeded or not.
    }

    if(NT_SUCCESS(Status))
    {
        //
        // Delete the symbolic link.
        //

        Status = IoDeleteSymbolicLink(&DosDevice);

        //
        //  Tell the MU driver to delete the DeviceObject
        //

        MU_CloseDeviceObject(dwPort, dwSlot);

        //
        //  Mark the drive as not mounted
        //

        MU_CLEAR_MOUNTED(chDrive);
        #if DBG
        g_XapiCurrentUserDevices--;
        #endif    
    }

    LeaveCriticalSection(&XapiMountMUCriticalSection);

    return RtlNtStatusToDosError(Status);
}


DWORD
WINAPI
XReadMUMetaData(
    IN DWORD dwPort,
    IN DWORD dwSlot,
    IN LPVOID lpBuffer,
    IN DWORD dwByteOffset,
    IN DWORD dwNumberOfBytesToRead
    )
{
    NTSTATUS Status;
    OCHAR szDeviceName[64];
    OBJECT_STRING DeviceName;
    OCHAR chDrive = MU_DRIVE_LETTER_FROM_PORT_SLOT(dwPort, dwSlot);
    PDEVICE_OBJECT DeviceObject;
    PFAT_VOLUME_METADATA VolumeMetadata;
    LARGE_INTEGER StartingOffset;
    DISK_GEOMETRY DiskGeometry;
    PARTITION_INFORMATION PartitionInformation;

#if DBG
    RIP_ON_NOT_TRUE_WITH_MESSAGE(XPP_XInitDevicesHasBeenCalled, "XReadMUMetaData: XInitDevices must be called first!");

    if ((dwPort < MU_PORT_MIN) || (dwPort > MU_PORT_MAX))
    {
        RIP("XReadMUMetaData() invalid parameter (dwPort)");
    }

    if ((dwSlot < MU_SLOT_MIN) || (dwSlot > MU_SLOT_MAX))
    {
        RIP("XReadMUMetaData() invalid parameter (dwSlot)");
    }

    if (dwByteOffset >= PAGE_SIZE)
    {
        RIP("XReadMUMetaData() invalid parameter (dwByteOffset)");
    }

    if ((PAGE_SIZE - dwByteOffset) < dwNumberOfBytesToRead)
    {
        RIP("XReadMUMetaData() invalid parameter (dwNumberOfBytesToRead)");
    }
#endif // DBG

    EnterCriticalSection(&XapiMountMUCriticalSection);

    if(!MU_IS_MOUNTED(chDrive))
    {
        //
        // Setup the string buffer
        //
        DeviceName.Length = 0;
        //lie - so we have guaranteed space for a back slash (lie leave room for NULL, and a '\\')
        DeviceName.MaximumLength = sizeof(szDeviceName)/sizeof(OCHAR)-2;
        DeviceName.Buffer = szDeviceName;

        //
        //  create the device object
        //
        Status = MU_CreateDeviceObject(
                    dwPort,
                    dwSlot,
                    &DeviceName
                    );
    }
    else
    {
        Status = STATUS_SUCCESS;
    }

    if (NT_SUCCESS(Status))
    {
        DeviceObject = MU_GetExistingDeviceObject(dwPort, dwSlot),

        //
        //  Before the memory unit can accept read requests, it must first
        //  initialize its drive geometry information.  Send the same IOCTLs
        //  that the file system would send during the mount process.  Neither
        //  of these need to synchronized with a file system that has already
        //  mounted, so send them directly to the device.
        //

        Status = IoSynchronousDeviceIoControlRequest(IOCTL_DISK_GET_DRIVE_GEOMETRY,
            DeviceObject, NULL, 0, &DiskGeometry, sizeof(DISK_GEOMETRY),
            NULL, FALSE);

        if (NT_SUCCESS(Status))
        {
            Status = IoSynchronousDeviceIoControlRequest(IOCTL_DISK_GET_PARTITION_INFO,
                DeviceObject, NULL, 0, &PartitionInformation,
                sizeof(PARTITION_INFORMATION), NULL, FALSE);

            if (PartitionInformation.PartitionLength.QuadPart < PAGE_SIZE)
            {
                Status = STATUS_UNRECOGNIZED_VOLUME;
            }
        }

        if (NT_SUCCESS(Status))
        {
            //
            //  Allocate a block of memory to hold the volume metadata block.  All
            //  device objects must support at least PAGE_SIZE transfers in order to
            //  support the file system cache.  Memory units typically have at least
            //  PAGE_SIZE sector sizes, so there's no point in trying to reduce this
            //  memory requirement.
            //

            VolumeMetadata = (PFAT_VOLUME_METADATA)LocalAlloc(LMEM_FIXED, PAGE_SIZE);

            if (VolumeMetadata != NULL)
            {
                //
                //  Submit a read request for PAGE_SIZE bytes from the start of
                //  the device.  Note that we don't need to synchronize this
                //  with the file system because all of the file systems we care
                //  about immediately flush any metadata writes to media.
                //

                StartingOffset.QuadPart = 0;

                Status = IoSynchronousFsdRequest(IRP_MJ_READ, DeviceObject,
                    VolumeMetadata, PAGE_SIZE, &StartingOffset);

                if (NT_SUCCESS(Status))
                {
                    //
                    //  Verify that the volume is formatted as FATX.  Memory units
                    //  never use any other file system.
                    //

                    if (VolumeMetadata->Signature == FAT_VOLUME_SIGNATURE)
                    {
                        CopyMemory(lpBuffer, (LPBYTE)VolumeMetadata + dwByteOffset,
                            dwNumberOfBytesToRead);

                        Status = STATUS_SUCCESS;
                    }
                    else
                    {
                        Status = STATUS_UNRECOGNIZED_VOLUME;
                    }
                }

                LocalFree(VolumeMetadata);
            }
            else
            {
                Status = STATUS_INSUFFICIENT_RESOURCES;
            }
        }

        if(!MU_IS_MOUNTED(chDrive))
        {
            //
            //  Tell the MU driver to delete the DeviceObject
            //

            MU_CloseDeviceObject(dwPort, dwSlot);
        }
    }

    LeaveCriticalSection(&XapiMountMUCriticalSection);

    return RtlNtStatusToDosError(Status);
}

#endif // ! XAPILIBP

#ifdef XAPILIBP

DWORD
WINAPI
XMUNameFromPortSlot(
    IN DWORD dwPort,
    IN DWORD dwSlot,
    OUT LPWSTR lpName,
    IN UINT cchName
    )
{
    DWORD dwError;
    WCHAR VolumeName[FAT_VOLUME_NAME_LENGTH];

    dwError = XReadMUMetaData(dwPort, dwSlot, VolumeName,
        FIELD_OFFSET(FAT_VOLUME_METADATA, VolumeName), sizeof(VolumeName));

    if (dwError == ERROR_SUCCESS)
    {
        lstrcpynW(lpName, VolumeName, min(cchName, FAT_VOLUME_NAME_LENGTH));
    }

    return dwError;
}

#endif // XAPILIBP

#ifndef XAPILIBP

DWORD
WINAPI
XMUPortFromDriveLetter(
    OCHAR chDrive
    )
{
    if ((chDrive >= MU_FIRST_DRIVE) && (chDrive <= MU_LAST_DRIVE))
    {
        return MU_PORT_FROM_DRIVE_LETTER(chDrive);
    }
    else
    {
        return -1;
    }
}

DWORD
WINAPI
XMUSlotFromDriveLetter(
    OCHAR chDrive
    )
{
    if ((chDrive >= MU_FIRST_DRIVE) && (chDrive <= MU_LAST_DRIVE))
    {
        return MU_SLOT_FROM_DRIVE_LETTER(chDrive);
    }
    else
    {
        return -1;
    }
}

#endif // ! XAPILIBP
