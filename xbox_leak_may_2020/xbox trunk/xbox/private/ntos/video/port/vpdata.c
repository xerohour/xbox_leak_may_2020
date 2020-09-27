/*++

Copyright (c) 1990-1999  Microsoft Corporation

Module Name:

  vpdata.c

Abstract:

    Global data module for the video port

Author:

    Andre Vachon (andreva) 12-Jul-1997

Environment:

    kernel mode only

Notes:

    This module is a driver which implements OS dependant functions on the
    behalf of the video drivers

Revision History:

--*/

#include "videoprt.h"

//
//
// Data that is NOT pageable
//
//

//
// Globals to support HwResetHw function
//

VP_RESET_HW HwResetHw[6];

//
// We check for resources reported in read/write functions which
// can be called at raised irql.
//

#if DBG
ULONG VPResourcesReported = TRUE;
#endif

//
// Debug Level for output routine (not pageable because VideoDebugPrint
// can be called at raised irql.
//

ULONG VideoDebugLevel = 0;

//
//
// Data that IS pageable
//
//

#if defined(ALLOC_PRAGMA)
#pragma data_seg("PAGE_DATA")
#endif


//
// Used to do first time initialization of the video port.
//

BOOLEAN VPFirstTime = TRUE;

//
// Callbacks to win32k
//

PVIDEO_WIN32K_CALLOUT Win32kCallout = NULL;

//
// Disable USWC is case the machine does not work properly with it.
//

BOOLEAN EnableUSWC = TRUE;

//
// Count to determine the number of video devices
//

ULONG VideoDeviceNumber = 0;

//
// Registry Class in which all video information is stored.
//

PWSTR VideoClassString = L"VIDEO";
UNICODE_STRING VideoClassName = {10,12,L"VIDEO"};

//
// Pointer to physical memory. It is created during driver initialization
// and is only closed when the driver is closed.
//

PVOID PhysicalMemorySection = NULL;

//
// Variable to determine if there is a ROM at physical address C0000 on which
// we can do the int 10
//

ULONG VpC0000Compatible = 0;

//
// HwDeviceExtension of the VGA miniport driver, if it is loaded.
//

PVOID VgaHwDeviceExtension = NULL;

//
// Pointer to the device object for the device that owns the VGA ports
//

PDEVICE_OBJECT VgaCompatibleDevice = NULL;

//
// Store the amount of physical memory in the machine.
//

ULONGLONG VpSystemMemorySize;

//
// This structure describes to which ports access is required.
//

#define MEM_VGA               0xA0000
#define MEM_VGA_SIZE          0x20000
#define VGA_BASE_IO_PORT      0x000003B0
#define VGA_START_BREAK_PORT  0x000003BB
#define VGA_END_BREAK_PORT    0x000003C0
#define VGA_MAX_IO_PORT       0x000003DF


PVIDEO_ACCESS_RANGE VgaAccessRanges = NULL;
ULONG               NumVgaAccessRanges = 0;
PDEVICE_OBJECT      DeviceOwningVga = NULL;


VIDEO_ACCESS_RANGE VgaLegacyResources[NUM_VGA_LEGACY_RESOURCES] = {
{
    VGA_BASE_IO_PORT, 0x00000000,
    VGA_START_BREAK_PORT - VGA_BASE_IO_PORT+ 1,
    1,
    1,
    1
},
{
    VGA_END_BREAK_PORT, 0x00000000,
    VGA_MAX_IO_PORT - VGA_END_BREAK_PORT + 1,
    1,
    1,
    1
},
{
    MEM_VGA, 0x00000000,
    MEM_VGA_SIZE,
    0,
    1,
    1
}
};

#if DBG

CHAR *BusType[] = { "Internal",
                    "Isa",
                    "Eisa",
                    "MicroChannel",
                    "TurboChannel",
                    "PCIBus",
                    "VMEBus",
                    "NuBus",
                    "PCMCIABus",
                    "CBus",
                    "MPIBus",
                    "MPSABus",
                    "ProcessorInternal",
                    "InternalPowerBus",
                    "PNPISABus",
                    "MaximumInterfaceType"
                };
#endif

PROC_ADDRESS VideoPortEntryPoints[] =
{
    PROC(VideoPortDDCMonitorHelper),
    PROC(VideoPortGetAgpServices),
    PROC(VideoPortGetRomImage),
    PROC(VideoPortGetAssociatedDeviceExtension),
    PROC(VideoPortAcquireDeviceLock),
    PROC(VideoPortReleaseDeviceLock),
    PROC(VideoPortAllocateBuffer),
    PROC(VideoPortReleaseBuffer),
#if defined(_X86_)
    "VideoPortInterlockedIncrement", InterlockedIncrement,
    "VideoPortInterlockedDecrement", InterlockedDecrement,
    "VideoPortInterlockedExchange", InterlockedExchange,
#else
    PROC(VideoPortInterlockedIncrement),
    PROC(VideoPortInterlockedDecrement),
    PROC(VideoPortInterlockedExchange),
#endif
    PROC(VideoPortGetVgaStatus),
    PROC(VideoPortQueueDpc),
    PROC(VideoPortQueryServices),
    {NULL, NULL}
};
