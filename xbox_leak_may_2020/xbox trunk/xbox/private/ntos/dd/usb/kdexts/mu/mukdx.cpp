/*++

Copyright (c) 1998	Microsoft Corporation

Module Name: 

    1394kdx.c

Abstract


Author:


Revision History:
--*/

#define KDEXTMODE

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <ntos.h>
#include <zwapi.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <imagehlp.h>

#include <memory.h>
#pragma hdrstop

#include <wdbgexts.h>
extern WINDBG_EXTENSION_APIS ExtensionApis;

#include <ntverp.h>
#include <imagehlp.h>

#include <mu.h>
#include "usb.h"
#include "urb.h"

//
//  Globals
//

EXT_API_VERSION        ApiVersion = {
    (VER_PRODUCTVERSION_W >> 8),
    (VER_PRODUCTVERSION_W & 0xff),
    EXT_API_VERSION_NUMBER,
    0
    };

WINDBG_EXTENSION_APIS  ExtensionApis;
USHORT                  SavedMajorVersion;
USHORT                  SavedMinorVersion;

PSTR    ApiDescriptions[] =
{
    "\nUSB MU driver debugging Extensions\n\n",
    "help           - displays help for this extension\n",
    "ext            - dump the contents of a device extension\n",
    "\n"
};

#define NUMBER_API_DESCRIPTIONS     3

//
//  LibMain32
//
BOOL
WINAPI
DLLMain(
    HINSTANCE   hInstance, 
    ULONG       ulReason, 
    LPVOID      pvReserved
    )
{
    switch(ulReason) {

        case DLL_PROCESS_ATTACH:
            break;

        case DLL_PROCESS_DETACH:
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;

        default:
            break;
    }

    return(TRUE);
} // DLLMain

DllInit(
    HANDLE hModule,
    DWORD  dwReason,
    DWORD  dwReserved
    )
{
    switch (dwReason) {
        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
            break;

        case DLL_PROCESS_ATTACH:
            break;
    }

    return TRUE;
}


void
WinDbgExtensionDllInit(
    PWINDBG_EXTENSION_APIS  pExtensionApis,
    USHORT                  MajorVersion,
    USHORT                  MinorVersion
    )
{
    ExtensionApis = *pExtensionApis;

    SavedMajorVersion = MajorVersion;
    SavedMinorVersion = MinorVersion;

    return;
} // WinDbgExtensionDllInit

void
CheckVersion(void)
{

} // CheckVersion

LPEXT_API_VERSION
ExtensionApiVersion(void)
{
    return(&ApiVersion);
} // ExtensionApiVersion

DECLARE_API(help)
{
    ULONG   i;

    for (i=0; i < NUMBER_API_DESCRIPTIONS; i++)
        dprintf(ApiDescriptions[i]);

    return;
} // version

DECLARE_API(ext)
{
    DEVICE_EXTENSION    ext;
    ULONG   Address, BytesRead;

    if (0 == args[0]) {

        dprintf("ext <PDEVICE_EXTENSION>\n");
        return;
    }

    sscanf(args, "%lx", &Address);

    dprintf("\nohci DeviceExtension = 0x%x\n\n", Address);

    if (!ReadMemory(Address, &ext, sizeof(DEVICE_EXTENSION), &BytesRead)) {

        dprintf("Unable to read extension\n");
        return;
    }

    if (BytesRead < sizeof(DEVICE_EXTENSION)) {

        dprintf("Only read %d bytes of extension, expected %d\n", BytesRead, sizeof(DEVICE_EXTENSION));
        return;
    }

    dprintf("\text.FdoDeviceObject = 0x%x\n", ext.FdoDeviceObject);
    dprintf("\text.IUsbDevice = 0x%x\n", ext.Device);
    dprintf("\text.UsbDeviceData.InterfaceNumber = 0x%x\n", ext.UsbDeviceData.InterfaceNumber);
    dprintf("\text.UsbDeviceData.PlayerSlot = 0x%x\n", ext.UsbDeviceData.PlayerSlot);
    dprintf("\text.UsbDeviceData.ControllerSlot = 0x%x\n", ext.UsbDeviceData.ControllerSlot);
    dprintf("\text.UsbDeviceData.BulkInEndpointAddress = 0x%x\n", ext.UsbDeviceData.BulkInEndpointAddress);
    dprintf("\text.UsbDeviceData.BulkOutEndpointAddress = 0x%x\n", ext.UsbDeviceData.BulkOutEndpointAddress);

    dprintf("\text.PendingIoCount = 0x%x\n", ext.PendingIoCount);
    dprintf("\text.DeviceFlags = 0x%x\n", ext.DeviceFlags);
    dprintf("\text.OriginalMrb = 0x%x\n", ext.OriginalMrb);

    dprintf("\text.PendingIrp = 0x%x\n", &ext.PendingIrp);

    dprintf("\text.DiskExtension.Fdo = 0x%x\n", ext.DiskExtension.Fdo);
    dprintf("\text.DiskExtension.SectorShift = 0x%x\n", ext.DiskExtension.SectorShift);
    dprintf("\text.DiskExtension.PartitionLength.LowPart = 0x%x\n", ext.DiskExtension.PartitionLength.LowPart);
    dprintf("\text.DiskExtension.PartitionLength.HighPart = 0x%x\n", ext.DiskExtension.PartitionLength.HighPart);

    dprintf("\text.CbwCswMdl = 0x%x\n", ext.CbwCswMdl);

    dprintf("\n");
    return;
} // device extension


