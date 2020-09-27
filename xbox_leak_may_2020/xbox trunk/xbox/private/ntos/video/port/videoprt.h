/*++

Copyright (c) 1991-1999  Microsoft Corporation

Module Name:

    videoprt.h

Abstract:

    This module contains the structure definitions private to the video port
    driver.

Author:

    Andre Vachon (andreva) 02-Dec-1991

Notes:

Revision History:

--*/

#ifndef __VIDEOPRT_H__
#define __VIDEOPRT_H__

#define _NTDRIVER_

#ifndef FAR
#define FAR
#endif

#include "dderror.h"
#include "ntos.h"
#include "pci.h"
#include "wdmguid.h"
#include "stdarg.h"
#include "stdio.h"
#include "zwapi.h"
#include "ntiologc.h"

#include "ntddvdeo.h"
#include "video.h"
#include "ntagp.h"
#include "agp.h"
#include "inbv.h"


//
//  Forward declare some basic driver objects.
//

typedef struct _FDO_EXTENSION       *PFDO_EXTENSION;
typedef struct _CHILD_PDO_EXTENSION *PCHILD_PDO_EXTENSION;


//
// Debugging Macro
//
//
// When an IO routine is called, we want to make sure the miniport
// in question has reported its IO ports.
// VPResourceReported is TRUE when a miniport has called VideoPort-
// VerifyAccessRanges.
// It is set to FALSE as a default, and set back to FALSE when finishing
// an iteration in the loop of VideoPortInitialize (which will reset
// the default when we exit the loop also).
//
// This flag will also be set to TRUE by the VREATE entry point so that
// the IO functions always work after init.
//

#if DBG

#undef VideoDebugPrint
#define pVideoDebugPrint(arg) VideoPortDebugPrint arg

#define IS_ACCESS_RANGES_DEFINED()                                         \
    {                                                                      \
        if (!VPResourcesReported) {                                        \
                                                                           \
            pVideoDebugPrint((0, "The miniport driver is trying to access" \
                                 " IO ports or memory location before the" \
                                 " ACCESS_RANGES have been reported to"    \
                                 " the port driver with the"               \
                                 " VideoPortVerifyAccessRanges(). Please"  \
                                 " fix the miniport driver\n"));           \
                                                                           \
            DbgBreakPoint();                                               \
                                                                           \
        }                                                                  \
    }

#else

#define pVideoDebugPrint(arg)
#define IS_ACCESS_RANGES_DEFINED()

#endif

//
// Useful registry buffer length.
//

#define STRING_LENGTH 60

//
// Queue link for mapped addresses stored for unmapping
//

typedef struct _MAPPED_ADDRESS {
    struct _MAPPED_ADDRESS *NextMappedAddress;
    PVOID MappedAddress;
    PHYSICAL_ADDRESS PhysicalAddress;
    ULONG NumberOfUchars;
    ULONG RefCount;
    UCHAR InIoSpace;
    BOOLEAN bNeedsUnmapping;
    BOOLEAN bLargePageRequest;
} MAPPED_ADDRESS, *PMAPPED_ADDRESS;

//
// BusDataRegistry variables
//

typedef struct _VP_QUERY_DEVICE {
    PVOID MiniportHwDeviceExtension;
    PVOID CallbackRoutine;
    PVOID MiniportContext;
    VP_STATUS MiniportStatus;
    ULONG DeviceDataType;
} VP_QUERY_DEVICE, *PVP_QUERY_DEVICE;


//
// Definition of the data passed in for the VideoPortGetRegistryParameters
// function for the DeviceDataType.
//

#define VP_GET_REGISTRY_DATA 0
#define VP_GET_REGISTRY_FILE 1

typedef struct _VIDEO_PORT_DRIVER_EXTENSION {

    UNICODE_STRING RegistryPath;
    VIDEO_HW_INITIALIZATION_DATA HwInitData;

} VIDEO_PORT_DRIVER_EXTENSION, *PVIDEO_PORT_DRIVER_EXTENSION;


//
// PnP Detection flags
//

#define PNP_ENABLED           0x001
#define LEGACY_DETECT         0x002
#define VGA_DRIVER            0x004
#define LEGACY_DRIVER         0x008
#define REPORT_DEVICE         0x020
#define UPGRADE_FAIL_START    0x040
#define FINDADAPTER_SUCCEEDED 0x080
#define UPGRADE_FAIL_HWINIT   0x100
#define VGA_DETECT            0x200


//
// ResetHW Structure
//

typedef struct _VP_RESET_HW {
    PVIDEO_HW_RESET_HW ResetFunction;
    PVOID HwDeviceExtension;
} VP_RESET_HW, *PVP_RESET_HW;


//
// AGP Support
//

#define POOL_TAG 0x72745076 // 'vPrt'

typedef struct _AGP_ALLOCATION
{
    PHYSICAL_ADDRESS PhysicalAddress;
    PVOID VirtualAddress;
    ULONG Pages;
    PMDL Mdl;
    struct _AGP_ALLOCATION *Next;
} AGP_ALLOCATION, *PAGP_ALLOCATION;


//
// Private EVENT support for miniport.
//

//
//  This flag indicates that the enveloping VIDEO_PORT_EVENT has a PKEVENT
//  field filled in by ObReferenceObjectByHandle(). It cannot be waited on
//  at all. Must be consistent with that in pw32kevt.h in gre.
//

#define ENG_EVENT_FLAG_IS_MAPPED_USER       0x1

//
//  This flag indicates that the enveloping VIDEO_PORT_EVENT is about to be
//  deleted and that the display driver callback is ongoing. Must be consistent
//  with that in pw32kevt.h in gre.
//

#define ENG_EVENT_FLAG_IS_INVALID           0x2

//
//  This flag indicates that the enveloping VIDEO_PORT_EVENT is in a DPC.
//  It signals the GDI Engine not to delete the VIDEO_POR_EVENT. It signals
//  the VideoPortSetEventDPC to set it then free it. Must be consistent with
//  that in pw32kevt.h in gre.
//

#define ENG_EVENT_FLAG_IN_DPC               0x4


typedef struct _VIDEO_PORT_EVENT {
    PVOID   pKEvent;
    volatile ULONG fFlags;
} VIDEO_PORT_EVENT, *PVIDEO_PORT_EVENT;


#define EDID_BUFFER_SIZE 256


typedef enum _HW_INIT_STATUS
{
    HwInitNotCalled,  // HwInitialize has not yet been called
    HwInitSucceeded,  // HwInitialize has been called and succeeded
    HwInitFailed      // HwInitialize has been called and failed
} HW_INIT_STATUS, *PHW_INIT_STATUS;


#define  GET_FDO_EXT(p) (((PFDO_EXTENSION)(p)) - 1)

//
// Define HW_DEVICE_EXTENSION verification macro.
//

#define IS_HW_DEVICE_EXTENSION(p) (GET_FDO_EXT(p)->HwDeviceExtension == (p))

typedef struct _ALLOC_ENTRY {
  PVOID Address;
  ULONG Size;
  struct _ALLOC_ENTRY *Next;
} *PALLOC_ENTRY, ALLOC_ENTRY;

//
// Device Extension for the FUNCTIONAL Driver Object (FDO)
//

typedef struct _FDO_EXTENSION {

    //
    // Location of the miniport device extension.
    //

    PVOID HwDeviceExtension;

    //
    // Power management mappings.
    //

    DEVICE_POWER_STATE DeviceMapping[PowerSystemMaximum] ;
    BOOLEAN IsMappingReady ;

    //
    // Event object for pVideoPortDispatch synchronization.
    //

    KMUTEX                   SyncMutex;

    //
    // Track whether the device has been opened.
    //

    BOOLEAN                  DeviceOpened;

    ////////////////////////////////////////////////////////////////////////////
    //
    //  END common header.
    //
    ////////////////////////////////////////////////////////////////////////////

    //
    // Adapter device objects
    //

    PDEVICE_OBJECT FunctionalDeviceObject;
    PDEVICE_OBJECT PhysicalDeviceObject;
    PDEVICE_OBJECT AttachedDeviceObject;

    //
    // Pointer to the miniport config info so that the port driver
    // can modify it when the miniport is asking for configuration information.
    //

    PVIDEO_PORT_CONFIG_INFO MiniportConfigInfo;

    //
    // Miniport exports
    //

    PVIDEO_HW_FIND_ADAPTER         HwFindAdapter;
    PVIDEO_HW_INITIALIZE           HwInitialize;
    PVIDEO_HW_INTERRUPT            HwInterrupt;
    PVIDEO_HW_START_IO             HwStartIO;
    PVIDEO_HW_TIMER                HwTimer;
    PVIDEO_HW_POWER_SET            HwSetPowerState;
    PVIDEO_HW_POWER_GET            HwGetPowerState;
    PVIDEO_HW_QUERY_INTERFACE      HwQueryInterface;
    PVIDEO_HW_CHILD_CALLBACK       HwChildCallback;

    //
    // Legacy resources used by the driver and reported to Plug and Play
    // via FILTER_RESOURCE_REQUIREMENTS.
    //

    PVIDEO_ACCESS_RANGE HwLegacyResourceList;
    ULONG               HwLegacyResourceCount;

    //
    // Linked list of all memory mapped io space (done through MmMapIoSpace)
    // requested by the miniport driver.
    // This list is kept so we can free up those ressources if the driver
    // fails to load or if it is unloaded at a later time.
    //

    PMAPPED_ADDRESS MappedAddressList;

    //
    // Interrupt object
    //

    PKINTERRUPT InterruptObject;

    //
    // Interrupt vector, irql and mode
    //

    ULONG InterruptVector;
    KIRQL InterruptIrql;
    ULONG InterruptAffinity;
    KINTERRUPT_MODE InterruptMode;
    BOOLEAN InterruptsEnabled;

    //
    // Information about the BUS on which the adapteris located
    //

    INTERFACE_TYPE AdapterInterfaceType;
    ULONG SystemIoBusNumber;

    //
    // Size of the miniport device extensions.
    //

    ULONG HwDeviceExtensionSize;
    ULONG HwChildDeviceExtensionSize;

    //
    // Determines the size required to save the video hardware state
    //

    ULONG HardwareStateSize;

    //
    // Pointer to the path name indicating the path to the drivers node in
    // the registry's current control set
    //

    PWSTR DriverRegistryPath;
    ULONG DriverRegistryPathLength;

    //
    // Total memory usage of PTEs by a miniport driver.
    // This is used to track if the miniport is mapping too much memory
    //

    ULONG MemoryPTEUsage;

    //
    // Pointer to the video request packet;
    //

    PVIDEO_REQUEST_PACKET Vrp;

    //
    // Determines if the port driver is currently handling an attach caused by
    // a video filter drivers.
    //

    BOOLEAN bAttachInProgress;

    //
    // Has the drivers HwInitialize routine been called.
    //

    HW_INIT_STATUS HwInitStatus;

    //
    // VDM and int10 support
    //

    PHYSICAL_ADDRESS VdmPhysicalVideoMemoryAddress;
    ULONG VdmPhysicalVideoMemoryLength;

    //
    // Memory allocation values
    //

#if DBG
    LONG           FreeAllocation;
    PALLOC_ENTRY   AllocationHead;
    PAGED_LOOKASIDE_LIST AllocationList;
#endif

    //
    // DPC Support
    //

    KDPC Dpc;

    ////////////////////////////////////////////////////////////////////////////
    //
    // Plug and Play Support
    //
    ////////////////////////////////////////////////////////////////////////////

    PCM_RESOURCE_LIST ResourceList;
    PCM_RESOURCE_LIST AllocatedResources;   // bus driver list

    PCM_RESOURCE_LIST RawResources;         // complete list
    PCM_RESOURCE_LIST TranslatedResources;  // translated complete list

    ULONG             DeviceNumber;
    ULONG             SlotNumber;

    //
    // Indicates whether we can enumerate children right away, or if
    // we need to wait for HwInitialize to be called first.
    //

    BOOLEAN AllowEarlyEnumeration;

    //
    // Interface for communication with our bus driver.
    //

    BOOLEAN ValidBusInterface;
    BUS_INTERFACE_STANDARD BusInterface;

    //
    // Flags that indicate type of driver (VGA, PNP, etc)
    //

    ULONG             Flags;

    //
    // AGP Support
    //

    AGP_BUS_INTERFACE_STANDARD     AgpInterface;

    //
    // Callout support - Phydisp of the device in GDI
    //

    PVOID             PhysDisp;

} FDO_EXTENSION, *PFDO_EXTENSION;

#define MAXIMUM_MEM_LIMIT_K  64

//
// AGP Data Structures
//

typedef struct _REGION {
    ULONG Length;
    ULONG NumDwords;
    ULONG BitField[1];
} REGION, *PREGION;

typedef struct _RESERVATION_LIST RESERVATION_LIST, *PRESERVATION_LIST;
typedef struct _RESERVATION_LIST
{
    ULONG Offset;
    ULONG Pages;
    ULONG RefCount;
    BOOLEAN Reserved;
    PRESERVATION_LIST Next;
};

typedef struct _PHYSICAL_RESERVE_CONTEXT
{
    ULONG Pages;
    ULONG Caching;
    PVOID MapHandle;
    PHYSICAL_ADDRESS PhysicalAddress;
    PREGION Region;
} PHYSICAL_RESERVE_CONTEXT, *PPHYSICAL_RESERVE_CONTEXT;

typedef struct _VIRTUAL_RESERVE_CONTEXT
{
    HANDLE ProcessHandle;
    PEPROCESS Process;
    PVOID VirtualAddress;
    PPHYSICAL_RESERVE_CONTEXT PhysicalReserveContext;
    PRESERVATION_LIST ReservationList;
    PREGION Region;
} VIRTUAL_RESERVE_CONTEXT, *PVIRTUAL_RESERVE_CONTEXT;

typedef struct _DEVICE_ADDRESS DEVICE_ADDRESS, *PDEVICE_ADDRESS;
typedef struct _DEVICE_ADDRESS
{
    ULONG BusNumber;
    ULONG Slot;
    PDEVICE_ADDRESS Next;
};

//
// Support for GetProcAddress
//

typedef struct _PROC_ADDRESS
{
    PUCHAR FunctionName;
    PVOID  FunctionAddress;
} PROC_ADDRESS, *PPROC_ADDRESS;

#define PROC(x) #x, x

//
// Power Request Context Block
//

typedef struct tagPOWER_BLOCK
{
    PKEVENT     Event;
    union {
        NTSTATUS    Status;
        ULONG       FinalFlag;
    } ;
    PIRP        Irp ;
} POWER_BLOCK, *PPOWER_BLOCK;

//
// Global Data
//


#if DBG
extern ULONG VPResourcesReported;
extern CHAR *BusType[];
#endif

extern BOOLEAN VPFirstTime;
extern PVIDEO_WIN32K_CALLOUT Win32kCallout;
extern BOOLEAN EnableUSWC;
extern ULONG VideoDebugLevel;
extern ULONG VideoDeviceNumber;
extern PWSTR VideoClassString;
extern UNICODE_STRING VideoClassName;
extern VP_RESET_HW HwResetHw[];
extern PVOID PhysicalMemorySection;
extern ULONG VpC0000Compatible;
extern PVOID VgaHwDeviceExtension;
extern PDEVICE_OBJECT VgaCompatibleDevice;
extern PVIDEO_ACCESS_RANGE VgaAccessRanges;
extern ULONG NumVgaAccessRanges;
extern PDEVICE_OBJECT DeviceOwningVga;
extern PROC_ADDRESS VideoPortEntryPoints[];
extern VIDEO_ACCESS_RANGE VgaLegacyResources[];
extern ULONGLONG VpSystemMemorySize;



typedef
BOOLEAN
(*PSYNCHRONIZE_ROUTINE) (
    PKINTERRUPT             pInterrupt,
    PKSYNCHRONIZE_ROUTINE   pkSyncronizeRoutine,
    PVOID                   pSynchContext
    );

//
// Number of legacy vga resources
//

#define NUM_VGA_LEGACY_RESOURCES 3

//
// These macros are used to protect threads which will enter the
// miniport.  We need to guarantee that only one thread enters
// the miniport at a time.
//

#define ACQUIRE_DEVICE_LOCK(DeviceExtension)           \
    KeWaitForSingleObject(&DeviceExtension->SyncMutex, \
                          Executive,                   \
                          KernelMode,                  \
                          FALSE,                       \
                          (PTIME)NULL);

#define RELEASE_DEVICE_LOCK(DeviceExtension)           \
    KeReleaseMutex(&DeviceExtension->SyncMutex,        \
                   FALSE);

//
// Define macros to stall execution for given number of milli or micro seconds.
// Single call to KeStallExecutionProcessor() can be done for 100us max.
//

#define DELAY_MILLISECONDS(n)                           \
{                                                       \
    ULONG ulCount;                                      \
    ULONG ulTotal = 10 * (n);                           \
                                                        \
    for (ulCount = 0; ulCount < (n); ulCount++)         \
        KeStallExecutionProcessor(100);                 \
}

#define DELAY_MICROSECONDS(n)                           \
{                                                       \
    ULONG ulCount = (n);                                \
                                                        \
    while (ulCount > 0)                                 \
    {                                                   \
        if (ulCount >= 100)                             \
        {                                               \
            KeStallExecutionProcessor(100);             \
            ulCount -= 100;                             \
        }                                               \
        else                                            \
        {                                               \
            KeStallExecutionProcessor(ulCount);         \
            ulCount = 0;                                \
        }                                               \
    }                                                   \
}

//
// Private function declarations
//

//
// ddc.c
//

BOOLEAN
DDCReadEdidSegment(
    IN PVOID pHwDeviceExtension,
    IN PI2C_CALLBACKS pI2CCallbacks,
    IN OUT PUCHAR pucEdidBuffer,
    IN ULONG ulEdidBufferSize,
    IN UCHAR ucEdidSegment,
    IN UCHAR ucEdidOffset,
    IN UCHAR ucSetOffsetAddress,
    IN UCHAR ucReadAddress,
    IN BOOLEAN bEnhancedDDC
    );

//
// agp.c
//

BOOLEAN
VpQueryAgpInterface(
    PFDO_EXTENSION DeviceExtension
    );

//
// edid.c
//

BOOLEAN
pVideoPortIsValidEDID(
    PVOID Edid
    );


VOID
pVideoPortGetEDIDId(
    PVOID  pEdid,
    PWCHAR pwChar
    );

PVOID
pVideoPortGetMonitordescription(
    PVOID pEdid
    );

//
// i2c.c
//

BOOLEAN
I2CStart(
    IN PVOID pHwDeviceExtension,
    IN PI2C_CALLBACKS pI2CCallbacks
    );

BOOLEAN
I2CStop(
    IN PVOID pHwDeviceExtension,
    IN PI2C_CALLBACKS pI2CCallbacks
    );

BOOLEAN
I2CWrite(
    IN PVOID pHwDeviceExtension,
    IN PI2C_CALLBACKS pI2CCallbacks,
    IN PUCHAR pucBuffer,
    IN ULONG ulLength
    );

BOOLEAN
I2CRead(
    IN PVOID pHwDeviceExtension,
    IN PI2C_CALLBACKS pI2CCallbacks,
    OUT PUCHAR pucBuffer,
    IN ULONG ulLength
    );

BOOLEAN
I2CWriteByte(
    IN PVOID pHwDeviceExtension,
    IN PI2C_CALLBACKS pI2CCallbacks,
    IN UCHAR ucByte
    );

BOOLEAN
I2CReadByte(
    IN PVOID pHwDeviceExtension,
    IN PI2C_CALLBACKS pI2CCallbacks,
    OUT PUCHAR pucByte,
    IN BOOLEAN bMore
    );

BOOLEAN
I2CWaitForClockLineHigh(
    IN PVOID pHwDeviceExtension,
    IN PI2C_CALLBACKS pI2CCallbacks
    );

//
// pnp.c
//

NTSTATUS
pVideoPortSendIrpToLowerDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
pVideoPortPnpDispatch(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

//
// registry.c
//

NTSTATUS
VpGetFlags(
    IN PUNICODE_STRING RegistryPath,
    PVIDEO_HW_INITIALIZATION_DATA HwInitializationData,
    PULONG Flags
    );

NTSTATUS
VpSetEventCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PKEVENT Event
    );

//
// videoprt.c
//

NTSTATUS
pVideoPortCreateDeviceName(
    PWSTR           DeviceString,
    ULONG           DeviceNumber,
    PUNICODE_STRING UnicodeString,
    PWCHAR          UnicodeBuffer
    );

VOID
pVideoPortDebugPrint(
    ULONG DebugPrintLevel,
    PCHAR DebugMessage,
    ...
    );

NTSTATUS
pVideoPortDispatch(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

PVOID
pVideoPortFreeDeviceBase(
    IN PVOID HwDeviceExtension,
    IN PVOID MappedAddress
    );

PVOID
pVideoPortGetDeviceBase(
    IN PVOID HwDeviceExtension,
    IN PHYSICAL_ADDRESS IoAddress,
    IN ULONG NumberOfUchars,
    IN UCHAR InIoSpace,
    IN BOOLEAN bLargePage
    );

NTSTATUS
pVideoPortGetRegistryCallback(
    IN PWSTR ValueName,
    IN ULONG ValueType,
    IN PVOID ValueData,
    IN ULONG ValueLength,
    IN PVOID Context,
    IN PVOID EntryContext
    );

VOID
pVPInit(
    VOID
    );

NTSTATUS
VpCreateDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN ULONG DeviceExtensionSize,
    OUT PDEVICE_OBJECT *DeviceObject
    );

NTSTATUS
VideoPortFindAdapter(
    IN PDRIVER_OBJECT DriverObject,
    IN PVOID Argument2,
    IN PVIDEO_HW_INITIALIZATION_DATA HwInitializationData,
    IN PVOID HwContext,
    PDEVICE_OBJECT DeviceObject,
    PUCHAR nextMiniport
    );

NTSTATUS
VideoPortFindAdapter2(
    IN PDRIVER_OBJECT DriverObject,
    IN PVOID Argument2,
    IN PVIDEO_HW_INITIALIZATION_DATA HwInitializationData,
    IN PVOID HwContext,
    PDEVICE_OBJECT DeviceObject,
    PUCHAR nextMiniport
    );

NTSTATUS
VpAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    );

VP_STATUS
VpRegistryCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    );

NTSTATUS
VpGetBusInterface(
    PFDO_EXTENSION FdoExtension
    );

PVOID
VpGetProcAddress(
    IN PVOID HwDeviceExtension,
    IN PUCHAR FunctionName
    );

BOOLEAN
pVideoPortInterrupt(
    IN PKINTERRUPT Interrupt,
    IN PDEVICE_OBJECT DeviceObject
    );

VOID
pVideoPortMapToNtStatus(
    IN PSTATUS_BLOCK StatusBlock
    );

NTSTATUS
pVideoPortMapUserPhysicalMem(
    IN PFDO_EXTENSION FdoExtension,
    IN HANDLE ProcessHandle OPTIONAL,
    IN PHYSICAL_ADDRESS PhysicalAddress,
    IN OUT PULONG Length,
    IN OUT PULONG InIoSpace,
    IN OUT PVOID *VirtualAddress
    );

BOOLEAN
pVideoPortSynchronizeExecution(
    PVOID HwDeviceExtension,
    VIDEO_SYNCHRONIZE_PRIORITY Priority,
    PMINIPORT_SYNCHRONIZE_ROUTINE SynchronizeRoutine,
    PVOID Context
    );

VOID
pVideoPortHwTimer(
    IN PDEVICE_OBJECT DeviceObject,
    PVOID Context
    );

BOOLEAN
pVideoPortResetDisplay(
    IN ULONG Columns,
    IN ULONG Rows
    );

PHYSICAL_ADDRESS
AgpReservePhysical(
    IN PVOID Context,
    IN ULONG Pages,
    IN ULONG Caching,
    OUT PVOID *PhysicalReserveContext
    );

VOID
AgpReleasePhysical(
    PVOID Context,
    PVOID PhysicalReserveContext
    );

BOOLEAN
AgpCommitPhysical(
    PVOID Context,
    PVOID PhysicalReserveContext,
    ULONG Pages,
    ULONG Offset
    );

VOID
AgpFreePhysical(
    IN PVOID Context,
    IN PVOID PhysicalReserveContext,
    IN ULONG Pages,
    IN ULONG Offset
    );

PVOID
AgpReserveVirtual(
    IN PVOID Context,
    IN HANDLE ProcessHandle,
    IN PVOID PhysicalReserveContext,
    OUT PVOID *VirtualReserveContext
    );

VOID
AgpReleaseVirtual(
    IN PVOID Context,
    IN PVOID VirtualReserveContext
    );

PVOID
AgpCommitVirtual(
    IN PVOID Context,
    IN PVOID VirtualReserveContext,
    IN ULONG Pages,
    IN ULONG Offset
    );

VOID
AgpFreeVirtual(
    IN PVOID Context,
    IN PVOID VirtualReserveContext,
    IN ULONG Pages,
    IN ULONG Offset
    );

BOOLEAN
CreateBitField(
    ULONG Length,
    PREGION *Region
    );

VOID
ModifyRegion(
    PREGION Region,
    ULONG Offset,
    ULONG Length,
    BOOLEAN Set
    );

BOOLEAN
FindFirstRun(
    PREGION Region,
    PULONG Offset,
    PULONG Length
    );

NTSTATUS
VpAppendToRequirementsList(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIO_RESOURCE_REQUIREMENTS_LIST *RequirementsList,
    IN ULONG NumAccessRanges,
    IN PVIDEO_ACCESS_RANGE AccessRanges
    );

BOOLEAN
VpIsLegacyAccessRange(
    PFDO_EXTENSION fdoExtension,
    PVIDEO_ACCESS_RANGE AccessRange
    );

PCM_RESOURCE_LIST
VpRemoveFromResourceList(
    PCM_RESOURCE_LIST OriginalList,
    ULONG NumAccessRanges,
    PVIDEO_ACCESS_RANGE AccessRanges
    );

VOID
VpEnableDisplay(
    BOOLEAN bState
    );

VOID
VpWin32kCallout(
    PVIDEO_WIN32K_CALLBACKS_PARAMS calloutParams
    );

BOOLEAN
VpAllowFindAdapter(
    PFDO_EXTENSION fdoExtension
    );

ULONG
GetCmResourceListSize(
    PCM_RESOURCE_LIST CmResourceList
    );

VOID
pVideoPortDpcDispatcher(
    IN PKDPC Dpc,
    IN PVOID HwDeviceExtension,
    IN PMINIPORT_DPC_ROUTINE DpcRoutine,
    IN PVOID Context
    );

#if DBG
VOID
DumpRequirements(
    PIO_RESOURCE_REQUIREMENTS_LIST Requirements
    );

VOID
DumpResourceList(
    PCM_RESOURCE_LIST pcmResourceList
    );

PIO_RESOURCE_REQUIREMENTS_LIST
BuildRequirements(
    PCM_RESOURCE_LIST pcmResourceList
    );

VOID
DumpHwInitData(
    IN PVIDEO_HW_INITIALIZATION_DATA p
    );

VOID
DumpUnicodeString(
    IN PUNICODE_STRING p
    );
#endif

PCM_PARTIAL_RESOURCE_DESCRIPTOR
RtlUnpackPartialDesc(
    IN UCHAR Type,
    IN PCM_RESOURCE_LIST ResList,
    IN OUT PULONG Count
    );

ULONG
pVideoPortGetVgaStatusPci(
    PVOID HwDeviceExtension
    );

BOOLEAN
VpIsVgaResource(
    PVIDEO_ACCESS_RANGE AccessRange
    );

VOID
VpInterfaceDefaultReference(
    IN PVOID pContext
    );

VOID
VpInterfaceDefaultDereference(
    IN PVOID pContext
    );

#endif // ifndef __VIDEOPRT_H__
