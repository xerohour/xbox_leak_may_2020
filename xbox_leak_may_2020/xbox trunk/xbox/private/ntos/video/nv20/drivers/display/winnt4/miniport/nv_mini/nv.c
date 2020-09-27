//******************************************************************************
//
// Copyright (c) 1992  Microsoft Corporation
//
// Module Name:
//
//     NV.C
//
// Abstract:
//
//     This is the miniport driver for NV Adapters.
//
// Environment:
//
//     kernel mode only
//
// Notes:
//
// Revision History:
//
//     This driver was adapted from the ET4000 Miniport driver
//
//******************************************************************************

//******************************************************************************
//
// Copyright (c) 1996,1997  NVidia Corporation. All Rights Reserved
//
//******************************************************************************

#include "dderror.h"
#include "devioctl.h"
#include "miniport.h"

#include "ntddvdeo.h"
#include "video.h"

#include "nv.h"

#include "nvos.h"
#include "nvntioctl.h"
#include "nvapi.h"

#include "modedefs.h"

#include "nvMultiMon.h"

#ifdef PC98
#include "modes.h"
#endif // PC98

#include "rm.h"
#include "nvcm.h"

// To get the twinview definitions
#include "nvMultiMon.h"

// to get the multi device helper routines
#include "multidev.h"


#include <nvRmStereo.h>


// modeset DLL definitions                    
#include "modeext.h"

extern int FindModeEntry (LPDISPDATA lpDispData,
    LPCHAR      lpEdidBuffer,
    ULONG       dwEdidSize,
    LPMODEINFO  lpModeDesc,
    LPMODEOUT   lpModeOut,
    LPMODEINFO  lpModeNew);

#if (_WIN32_WINNT >= 0x0500)

// Normally we would include #ntddk.h, but that would include a bunch
// of other structures that would cause redefinitions / errors.

typedef struct _IO_STATUS_BLOCK
    {
    union
        {
        ULONG Status;
        PVOID Pointer;
        };

    ULONG *Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

extern ULONG ZwCreateFile(
  PVOID FileHandle,
  ULONG DesiredAccess,
  PVOID ObjectAttributes,
  PVOID IoStatusBlock,
  PVOID AllocationSize,
  ULONG FileAttributes,
  ULONG ShareAccess,
  ULONG CreateDisposition,
  ULONG CreateOptions,
  PVOID EaBuffer,
  ULONG EaLength );

extern ULONG ZwWriteFile(
  PVOID  FileHandle,
  PVOID  Event,
  PVOID  ApcRoutine,
  PVOID  ApcContext,
  PVOID  IoStatusBlock,
  PVOID  Buffer,
  ULONG  Length,
  PVOID  ByteOffset,
  PVOID  Key );

extern ULONG ZwReadFile(
  PVOID  FileHandle,
  PVOID  Event,
  PVOID  ApcRoutine,
  PVOID  ApcContext,
  PVOID  IoStatusBlock,
  PVOID  Buffer,
  ULONG  Length,
  PVOID  ByteOffset,
  PVOID  Key );

extern ULONG ZwDeviceIoControlFile(
  PVOID  FileHandle,
  PVOID  Event,
  PVOID  ApcRoutine,
  PVOID  ApcContext,
  PVOID  IoStatusBlock,
  ULONG  IoControlCode,
  PVOID  InputBuffer,
  ULONG  InputBufferLength,
  PVOID  OutputBuffer,
  ULONG  OutputBufferLength
);

extern ULONG ZwClose( PVOID Handle );

extern BOOLEAN
PsGetVersion(
    PULONG MajorVersion OPTIONAL,
    PULONG MinorVersion OPTIONAL,
    PULONG BuildNumber OPTIONAL,
    PNV_UNICODE_STRING CSDVersion OPTIONAL
    );

#endif

extern InitializeCommonModesetCode(PHW_DEVICE_EXTENSION HwDeviceExtension);
extern VOID ReadMonitorRestrictionModeList(PHW_DEVICE_EXTENSION HwDeviceExtension);
VOID ChoosePrimaryDevice(PHW_DEVICE_EXTENSION HwDeviceExtension, NVTWINVIEWDATA *pData);

VP_STATUS
NVGetColorLookup(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PVIDEO_CLUT ClutBuffer,
    ULONG ClutBufferSize
    );

VP_STATUS
NVReadMonitorTimingModeCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    );

VP_STATUS
NVReadRegistrySwitchesCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    );
VP_STATUS
NVReadRegistryTwinViewInfoCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    );
VP_STATUS
NVReadRegistrySaveSettingsCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    );
VP_STATUS
NVReadRegistryBinaryCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    );

BOOLEAN  GetHeadPhysicalModeList(
    PHW_DEVICE_EXTENSION hwDeviceExtension, 
    HEAD_MODE_LIST_INFO * pInput,
    ULONG * pRetSize);

ULONG ConvertDeviceMaskToIndex(
    ULONG ulDeviceMask);

ULONG ConvertConnectedMaskToNumDevices(
    ULONG ulConnectedDeviceMask);

VOID InitializeACPIStates(
    PHW_DEVICE_EXTENSION hwDeviceExtension);

VOID vInitializeSaveSettings(
    PHW_DEVICE_EXTENSION hwDeviceExtension);
VOID vWriteSaveSettings(
    PHW_DEVICE_EXTENSION hwDeviceExtension);
VOID vReadSaveSettings(
    PHW_DEVICE_EXTENSION hwDeviceExtension);
VOID vPrintSaveSettings(
    PHW_DEVICE_EXTENSION hwDeviceExtension);

static VOID SetSafeDeviceMasks_SingleHeadBoard(PHW_DEVICE_EXTENSION hwDeviceExtension);
static VOID SetSafeDeviceMasks_DualHeadBoard(PHW_DEVICE_EXTENSION hwDeviceExtension);
ULONG MaskToDeviceType( ULONG ulDeviceMask);
ULONG MaskToFindModeEntryDeviceType( ULONG ulDeviceMask);
ULONG MaskToTVFormat( ULONG ulDeviceMask);
ULONG ACPIHwIdToMask( ULONG ACPIHwId);
VOID HandleBIOSHeads(PHW_DEVICE_EXTENSION hwDeviceExtension);
VOID HandleBestResolution(PHW_DEVICE_EXTENSION hwDeviceExtension);
BOOLEAN   TwinViewDataOK(PHW_DEVICE_EXTENSION hwDeviceExtension, NVTWINVIEWDATA *pData);
VOID   InvalidateTwinViewData(PHW_DEVICE_EXTENSION hwDeviceExtension, NVTWINVIEWDATA *pData);
VOID InitFromTwinViewInfo(PHW_DEVICE_EXTENSION HwDeviceExtension);

VOID Verify1400x1050DFP(PHW_DEVICE_EXTENSION hwDeviceExtension);
ULONG WriteFPModeToRegistry (PHW_DEVICE_EXTENSION hwDeviceExtension);
extern VOID EnterFPCode(VOID *pSave, VOID *pMutex);
extern VOID ExitFPCode(VOID *pSave, VOID *pMutex);


extern VOID SetTimingOverRide(PHW_DEVICE_EXTENSION HwDeviceExtension);
extern ULONG GetBestResolution(PHW_DEVICE_EXTENSION HwDeviceExtension, ULONG ulEDIDIndex, ULONG ulHead, ULONG ulDeviceMask, MODE_ENTRY *pBestMode);
// 
//extern VOID SetBestResolution(PHW_DEVICE_EXTENSION HwDeviceExtension, ULONG ulWidth, ULONG ulHeight, ULONG ulDepth, ULONG ulRefreshRate);
// 
//  Use CheckForcedResolution with the last parameter TRUE if you want to SetTheBestResolution
//
//extern   
//
// Function is now in nv.c since I could not get it to compile including nv.h in nvfloat.c

void CheckForcedResolution(PHW_DEVICE_EXTENSION HwDeviceExtension, USHORT ulWidth, USHORT ulHeight, USHORT ulDepth, USHORT ulRefreshRate, BOOLEAN SetBestResolution );

extern VOID SignalModeSwitchEvent();
extern VOID SignalNVCplHotKeyEvent();
extern VOID InitNVSVCEvents();


//******************************************************************************
// The following declarations are needed to use the memory manager functions
//******************************************************************************
#define NV_PWR_MGMT
#define SYSMEM_FUDGE    (0x1000)

#define NTKERNELAPI DECLSPEC_IMPORT


extern VOID NVEnableBusMastering(PHW_DEVICE_EXTENSION);
extern VOID NVEnableVGASubsystem(PHW_DEVICE_EXTENSION);
extern VOID NVExtractBiosImage(PHW_DEVICE_EXTENSION);
extern VOID NVClearMutexPmeAudBuff0(PHW_DEVICE_EXTENSION);
extern U016 ModeTimingTable[NUMBER_OF_MODES*NUMBER_OF_RATES][10];
extern VOID NV_DisableNVInterrupts(PHW_DEVICE_EXTENSION);
extern VOID NV_ReEnableNVInterrupts(PHW_DEVICE_EXTENSION);
extern VOID SetGlobalHwDev(PHW_DEVICE_EXTENSION);
extern VOID BuildAvailableModesTable(PHW_DEVICE_EXTENSION);
extern VOID LoadRegistrySettings(PHW_DEVICE_EXTENSION);
extern VOID WakeUpCard(PHW_DEVICE_EXTENSION);
extern VOID NV_SetPixMixBits(PHW_DEVICE_EXTENSION);
extern VOID NVRestoreSpecificRegistersForPwrMgmt(PHW_DEVICE_EXTENSION);
extern VOID NVSaveSpecificRegistersForPwrMgmt(PHW_DEVICE_EXTENSION);
extern VOID NV_ManualTextModeSet(PHW_DEVICE_EXTENSION);
extern ULONG NVObReferenceObjectByHandle(HANDLE, HANDLE*);


extern void NVMapPhysIntoUserSpace(
        PHYSICAL_ADDRESS pa,
        LONG byteLength,
        PVOID *userVirtualAddress);

extern void NvUnmapPhysFromUserSpace(
        NV_SYSMEM_SHARE_MEMORY *pSysmemShareMemory
        );

extern void NvMapIntoUserSpace(
        NV_SYSMEM_SHARE_MEMORY *pSysmemShareMemory
        );

extern void NvUnmapFromUserSpace(
        NV_SYSMEM_SHARE_MEMORY *pSysmemShareMemory
        );

extern void
NvLockdownMemory(
    NV_LOCK_DOWN_MEMORY *pLockdownMemory
    );

extern void
NvUnlockMemory(
    NV_UNLOCK_MEMORY *pUnlockMemory
    );

extern void
NvSystemMemoryShare(
    NV_SYSMEM_SHARE_MEMORY *pSysmemShareMemory
    );

extern void
NvSystemMemoryUnShare(
    NV_SYSMEM_SHARE_MEMORY *pSysmemShareMemory
    );

NTKERNELAPI PHYSICAL_ADDRESS
MmGetPhysicalAddress (
    IN PVOID BaseAddress
    );

typedef enum _MEMORY_CACHING_TYPE_ORIG {
    MmFrameBufferCached = 2
} MEMORY_CACHING_TYPE_ORIG;

typedef enum _MEMORY_CACHING_TYPE {
    MmNonCached = FALSE,
    MmCached = TRUE,
    MmWriteCombined = MmFrameBufferCached,
    MmHardwareCoherentCached,
    MmCachingTypeDoNotUse1,
    MmCachingTypeDoNotUse2,
    MmMaximumCacheType
} MEMORY_CACHING_TYPE;

NTKERNELAPI
PVOID
MmMapIoSpace (
    IN PHYSICAL_ADDRESS PhysicalAddress,
    IN size_t NumberOfBytes,
    IN MEMORY_CACHING_TYPE CacheType
    );

NTKERNELAPI
VOID
MmUnmapIoSpace (
    IN PVOID BaseAddress,
    IN size_t NumberOfBytes
    );


//******************************************************************************
//
// Function declarations
//
//******************************************************************************
BOOLEAN bScanHeadDeviceOptions(PHW_DEVICE_EXTENSION HwDeviceExtension);

SHORT GetNextWordFromRegistry(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PUCHAR *SrcStrPtr
    );

SHORT my_wtoi(
    PUCHAR *SrcStrPtr
    );

VP_STATUS
NVFindAdapter(
    PVOID HwDeviceExtension,
    PVOID HwContext,
    PWSTR ArgumentString,
    PVIDEO_PORT_CONFIG_INFO ConfigInfo,
    PUCHAR Again
    );

BOOLEAN
NVInitialize(
    PVOID HwDeviceExtension
    );

BOOLEAN
NVInitialize_DacsAndEDID(
    PHW_DEVICE_EXTENSION HwDeviceExtension, BOOLEAN bBootTime
    );

BOOLEAN
NVOneTimeInit_TwinView(
    PHW_DEVICE_EXTENSION HwDeviceExtension
    );

BOOLEAN
NVStartIO(
    PVOID HwDeviceExtension,
    PVIDEO_REQUEST_PACKET RequestPacket
    );

BOOLEAN
NVInterrupt(
    PVOID HwDeviceExtension
    );

VP_STATUS
NVRegistryCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    );


#if (_WIN32_WINNT >= 0x0500)
#ifdef NT_BUILD_NV4
extern BOOLEAN
NVResetHW(
    PVOID hwDeviceExtension,
    ULONG Column,
    ULONG Rows
    );
#endif
#endif

#if (_WIN32_WINNT >= 0x0500)

ULONG
NVGetChildDescriptor(
    PVOID HwDeviceExtension,
    PVIDEO_CHILD_ENUM_INFO ChildEnumInfo,
    PVIDEO_CHILD_TYPE pChildType,
    PVOID pvChildDescriptor,
    PULONG pHwId,
    PULONG pUnused
    );

VP_STATUS
NVGetChildState(
    PHW_DEVICE_EXTENSION hwDeviceExtension,
    PVIDEO_REQUEST_PACKET RequestPacket
    );

VP_STATUS
NVValidateChildState(
    PHW_DEVICE_EXTENSION hwDeviceExtension,
    PVIDEO_REQUEST_PACKET RequestPacket
    );

VP_STATUS
NVSetChildState(
    PHW_DEVICE_EXTENSION hwDeviceExtension,
    PVIDEO_REQUEST_PACKET RequestPacket
    );

VP_STATUS
NVSwitchDualView(
    PHW_DEVICE_EXTENSION hwDeviceExtension,
    ULONG                dwViewEnable
    );

#ifdef NV_PWR_MGMT
VP_STATUS
NVGetPowerState(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    ULONG HwDeviceId,
    PVIDEO_POWER_MANAGEMENT VideoPowerManagement
    );

VP_STATUS
NVSetPowerState(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    ULONG HwDeviceId,
    PVIDEO_POWER_MANAGEMENT VideoPowerManagement
    );
#endif // #ifdef NV_PWR_MGMT

#endif // if (_WIN32_WINNT >= 0x0500)

// Support to validate a mode on a particular display device for control panel
BOOL NVValidateDeviceMode(PHW_DEVICE_EXTENSION HwDeviceExtension, VALMODEXTR *vmx);

//******************************************************************************
// Global variable used to detect if the ALI 1541 chipset is found.
// It needs to be global since the hwDeviceExtension is not preserved.
//******************************************************************************

ULONG Ali_1541_Chipset_Found = FALSE;

//******************************************************************************
// Value of VIA GA Translation Table Base register (offset 88)
//******************************************************************************

ULONG   ViaAgpRegister;


//******************************************************************************
// Global variable used to detect if several adapters are present
//******************************************************************************

ULONG MultiAdaptersPresent = FALSE;

//******************************************************************************
// Global variable used to track the slots where devices are located on the bus
// initialized with -1 (so DetectNextDevice starts looking for slot 0
//******************************************************************************

ULONG ulFindSlot = 0xffffffff;  

//******************************************************************************
// Static area buffer used to read information from the registry
//******************************************************************************

U016 registry_data[MAX_STRING_REGISTRY_DATA_WORDS];

//
//  Global flag to ensure RmInitRm() to be called only once.
//

BOOLEAN g_bIsRmInitRmCalled = FALSE;

//******************************************************************************
//
// Private function prototypes.
//
//******************************************************************************

VP_STATUS
NVQueryAvailableModes(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PVIDEO_MODE_INFORMATION ModeInformation,
    ULONG ModeInformationSize,
    ULONG_PTR* OutputSize
    );

VP_STATUS
NVQueryNumberOfAvailableModes(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PVIDEO_NUM_MODES NumModes,
    ULONG NumModesSize,
    ULONG_PTR* OutputSize
    );

VP_STATUS
NVQueryCurrentMode(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PVIDEO_MODE_INFORMATION ModeInformation,
    ULONG ModeInformationSize,
    ULONG_PTR* OutputSize
    );

BOOLEAN
NVIsPresent(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    //PVIDEO_ACCESS_RANGE pAccessRange,
    PULONG NV1Slot
    );

VP_STATUS
NVSetColorLookup(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PVIDEO_CLUT ClutBuffer,
    ULONG ClutBufferSize
    );

VOID
NVValidateModes(
    PHW_DEVICE_EXTENSION HwDeviceExtension
    );

VP_STATUS
NVSetMode(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PVIDEO_MODE Mode,
    ULONG ModeSize
    );

VOID NV10CursorTimer(
    PVOID HwDeviceExtension
);

BOOL NVGetTimingForDac(PHW_DEVICE_EXTENSION hwDeviceExtension, HEAD_RESOLUTION_INFO *pResolution, DAC_TIMING_VALUES * pTimingInfo);

//////// H.AZAR
#ifdef NVPE
VP_STATUS NVQueryInterface( PVOID HwDeviceExtension, PQUERY_INTERFACE QueryInterface);
VP_STATUS NVPEProcessCommand(PVOID pHwDeviceExtension, PVIDEO_REQUEST_PACKET pRequestPacket);
VP_STATUS NVRegistryCallbackPnpId(PVOID HwDeviceExtension, PVOID Context, PWSTR ValueName, PVOID ValueData, ULONG ValueLength);
#endif

#ifdef PC98

// global device extension pointer and frame buffer pointer
// used for local POST and mode sets
extern PHW_DEVICE_EXTENSION HwDeviceExtension;
extern PHWREG fbAddr;
VOID NvPostEx(PHW_DEVICE_EXTENSION pHwDevExt)
{
    PHYSICAL_ADDRESS physFbAddr;
    ULONG length, inIoSpace;

    // init global device extension for local POST
    HwDeviceExtension = pHwDevExt;

    // map the frame buffer
    physFbAddr.HighPart = 0x00000000;
    physFbAddr.LowPart = NVAccessRange[NV_PHYSICAL_DFB_ADDRESS].RangeStart.LowPart;
    physFbAddr.LowPart += RangeOffsets[NV_PDFB_INDEX].ulOffset;
    length = 0x400000;
    inIoSpace = FALSE;
    fbAddr = 0;
    VideoPortMapMemory(
        pHwDevExt,
        physFbAddr,
        &length,
        &inIoSpace,
        &fbAddr
    );

    // perform the POST based on global fbAddr and addresses in the device extension
    NvPost();

    // unmap the frame buffer
    VideoPortUnmapMemory(
        pHwDevExt,
        fbAddr,
        0
    );
}

#endif // PC98

//******************************************************************************
// The functions listed below are pageable
//******************************************************************************

#if defined(ALLOC_PRAGMA)

#pragma alloc_text(PAGE,DriverEntry)
#pragma alloc_text(PAGE,NVFindAdapter)
#pragma alloc_text(PAGE,NVInitialize)
#pragma alloc_text(PAGE,NVStartIO)
#pragma alloc_text(PAGE,NVIsPresent)
#pragma alloc_text(PAGE,NVSetColorLookup)

#if (_WIN32_WINNT >= 0x500)
#pragma alloc_text(PAGE, NVGetPowerState)
#pragma alloc_text(PAGE, NVSetPowerState)
#endif  // #if (_WIN32_WINNT >= 0x500)

#endif  // #if defined(ALLOC_PRAGMA)

//******************************************************************************
//
// Function: DriverEntry()
//
// Routine Description:
//
//     Installable driver initialization entry point.
//     This entry point is called directly by the I/O system.
//
// Arguments:
//
//     Context1 - First context value passed by the operating system. This is
//         the value with which the miniport driver calls VideoPortInitialize().
//
//     Context2 - Second context value passed by the operating system. This is
//         the value with which the miniport driver calls VideoPortInitialize().
//
// Return Value:
//
//     Status from VideoPortInitialize()
//
//******************************************************************************


ULONG DriverEntry(
    PVOID Context1,
    PVOID Context2
    )


    {

    VIDEO_HW_INITIALIZATION_DATA hwInitData;
    ULONG status;
    ULONG initializationStatus;



    //**************************************************************************
    // Zero out structure.
    //**************************************************************************

    VideoPortZeroMemory(&hwInitData, sizeof(VIDEO_HW_INITIALIZATION_DATA));

    //**************************************************************************
    // Specify sizes of structure and extension.
    //**************************************************************************

    hwInitData.HwInitDataSize = sizeof(VIDEO_HW_INITIALIZATION_DATA);

    //**************************************************************************
    // Set entry points.
    //**************************************************************************

    hwInitData.HwFindAdapter = NVFindAdapter;
    hwInitData.HwInitialize = NVInitialize;
    hwInitData.HwStartIO = NVStartIO;
    hwInitData.HwInterrupt = NVInterrupt;
    // This timer is only enabled on NV10 and alpha cursor blending.
//    hwInitData.HwTimer = NV10CursorTimer;

    //**************************************************************************
    // New NT 5.0 EntryPoint
    //**************************************************************************

#if (_WIN32_WINNT >= 0x0500)

    hwInitData.HwGetVideoChildDescriptor = NVGetChildDescriptor;

#ifdef NT_BUILD_NV4
    // To fix a hang on soft reboot issue for Toshiba and Dell.
    hwInitData.HwResetHw = NVResetHW;
#endif

#ifdef NV_PWR_MGMT
    hwInitData.HwGetPowerState = NVGetPowerState;
    hwInitData.HwSetPowerState = NVSetPowerState;
#endif // #ifdef NV_PWR_MGMT

#ifdef NVPE
    hwInitData.HwQueryInterface = NVQueryInterface;

#endif

#endif

    //**************************************************************************
    // Determine the size we require for the device extension.
    //**************************************************************************

    hwInitData.HwDeviceExtensionSize = sizeof(HW_DEVICE_EXTENSION);

    //**************************************************************************
    // Always start with parameters for device0 in this case.
    // We can leave it like this since we know we will only ever find one
    // VGA type adapter in a machine.
    //**************************************************************************

//    hwInitData.StartingDeviceNumber = 0;

    //**************************************************************************
    // Once all the relevant information has been stored, call the video
    // port driver to do the initialization.
    // For this device we will repeat this call three times, for ISA, EISA
    // and MCA.
    // We will return the minimum of all return values.
    //
    //
    // NOTE: For NV, we must be running on a PCI Bus
    //
    //**************************************************************************

    hwInitData.AdapterInterfaceType = PCIBus;

    initializationStatus = VideoPortInitialize(Context1,
                                               Context2,
                                               &hwInitData,
                                               NULL);

    if (initializationStatus == NO_ERROR)
       {
       return initializationStatus;
       }

    //**************************************************************************
    // We didn't find the card on any bus type, so lets
    // return the last error.
    //**************************************************************************

    return initializationStatus;

    } // end DriverEntry()


//**************************************************************************
// NT 5.0 specific entry points.
//**************************************************************************


#if (_WIN32_WINNT >= 0x0500)

#ifdef NV_PWR_MGMT

#ifndef _WIN64
VOID
NVAMD751ChipsetSaveHibernationRegisters(
    PHW_DEVICE_EXTENSION HwDeviceExtension
    )
{
    PCI_COMMON_CONFIG AGPConfigInfo;
    PHYSICAL_ADDRESS physaddr;
    ULONG GetBusDataStatus;
    #define RM_PAGE_MASK 0x0FFF
    #define RM_PAGE_SIZE 4096

    GetBusDataStatus = HalGetBusData(
                PCIConfiguration,           // bus data type
                0,                          // bus number
                0,                          // slot number
                &AGPConfigInfo,
                sizeof(PCI_COMMON_CONFIG));

    // Sanity check. Check the vendor ID and device ID.
    if ((GetBusDataStatus <= 2) || (AGPConfigInfo.VendorID != 0x1022) ||  (AGPConfigInfo.DeviceID != 0x7006)) {
        VideoDebugPrint((0, "*** NVAMD751ChipsetSaveHibernationRegisters(). Something wrong. The chipset is not AMD 751 even though it should be.\n"));
        return;
    }

    // AMD 751 has its AGP related regs memory-mapped via BAR1 (setup this mapping).
    physaddr.HighPart = 0;
    physaddr.LowPart  = AGPConfigInfo.u.type0.BaseAddresses[1] & ~RM_PAGE_MASK;
    HwDeviceExtension->AMD751_BAR1_Mapped_Address = (PHWREG)MmMapIoSpace(physaddr, RM_PAGE_SIZE, FALSE);
    VideoDebugPrint((3, "hwDeviceExtension->AMD751_BAR1_Mapped_Address: 0x%p\n",HwDeviceExtension->AMD751_BAR1_Mapped_Address));
    if (HwDeviceExtension->AMD751_BAR1_Mapped_Address == (PHWREG)NULL) {
        VideoDebugPrint((0,"*** Failed to map AMD 751's AGP chipset registers\n"));
    } else {
        // Save the registers values. We restore them later while coming back up from hibernation.
        HwDeviceExtension->AMD751_AGP_Enable_And_Status_Register_Saved_Value_USHORT =   HwDeviceExtension->AMD751_BAR1_Mapped_Address->Reg016[1];
        HwDeviceExtension->AMD751_AGP_GART_Base_Address_Register_Saved_Value_ULONG  =   HwDeviceExtension->AMD751_BAR1_Mapped_Address->Reg032[1];
        VideoDebugPrint((3, "Saved AMD751 Status register: 0x%x\n",HwDeviceExtension->AMD751_AGP_Enable_And_Status_Register_Saved_Value_USHORT));
        VideoDebugPrint((3, "Saved AMD751 GART Base register: 0x%x\n",HwDeviceExtension->AMD751_AGP_GART_Base_Address_Register_Saved_Value_ULONG));
    }
}

VOID
NVAMD751ChipsetRestoreHibernationRegisters(
    PHW_DEVICE_EXTENSION HwDeviceExtension
    )
{
    // Sanity check. Ensure the chipset is AMD751.
    if (HwDeviceExtension->AMD751ChipsetFound != TRUE) {
        VideoDebugPrint((0, "*** NVAMD751ChipsetRestoreHibernationRegisters(). Something wrong. The chipset is not AMD 751 even though it should be.\n"));
        return;
    }
    //***********************************************************************
    // Two of the AMD 751 chipset's BAR1 registers are not set correctly when
    // the machines wakes up from hibernation. So we restore these
    // Enable and Status register and GART base register to the values at the
    // normal bootup time. 
    //***********************************************************************
    if ((VOID *)HwDeviceExtension->AMD751_BAR1_Mapped_Address != NULL) {
        HwDeviceExtension->AMD751_BAR1_Mapped_Address->Reg016[1] = HwDeviceExtension->AMD751_AGP_Enable_And_Status_Register_Saved_Value_USHORT;
        HwDeviceExtension->AMD751_BAR1_Mapped_Address->Reg032[1] = HwDeviceExtension->AMD751_AGP_GART_Base_Address_Register_Saved_Value_ULONG;
        VideoDebugPrint((3, "Restored AMD751 Status register: 0x%x\n",HwDeviceExtension->AMD751_AGP_Enable_And_Status_Register_Saved_Value_USHORT));
        VideoDebugPrint((3, "Restored AMD751 GART Base register: 0x%x\n",HwDeviceExtension->AMD751_AGP_GART_Base_Address_Register_Saved_Value_ULONG));
    }
}
#endif // #ifndef _WIN64

VP_STATUS
NVGetPowerState(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    ULONG HwDeviceId,
    PVIDEO_POWER_MANAGEMENT VideoPowerManagement
    )

/*++

Routine Description:

    This function is called to see if a given device can go into a given
    power state.

Arguments:

    HwDeviceExtension    - Pointer to our hardware device extension structure.


    HwDeviceId           - Private unique 32 bit ID identifing the device.
                           0xFFFFFFFF indicates the NV card itself.

    VideoPowerManagement - Pointer to the power management structure which
                           indicates the power state in question.

Return Value:

    NO_ERROR if the device can go into the requested power state,
    otherwise an appropriate error code is returned.

--*/
{
ULONG CurrentPowerState;

    //*************************************************************************
    // Unattended install with Intel 810 present:
    // Make sure to SKIP this call, if the device has NOT been mapped!!!
    // (Skip this function if currently running unattended install with
    // an Intel 810)
    //*************************************************************************

    if (HwDeviceExtension->DeviceMappingFailed == TRUE)
        return NO_ERROR;

    //
    // on ACPI systems, Win2K actually calls us for "device ID 0 - Other".
    // Not sure how we should handle this device id. 
    // we simply return  true for now.
    //
    if (HwDeviceId == 0)
    {
        return(NO_ERROR);
    }

    //
    // For getPowerState, we treat the ACPI(100,110,200) and non-ACPI(QUERY_NONDDC_MONITOR_ID, QUERY_MONITOR_ID) device Ids
    // the same way. (Since all monitors essentially have only on/off support in the resMan. If the resMan were to support
    // a finer granularity of power management for monitors, this code should change.)
    // For SetPowerState, we treat yhe ACPI and non-ACPI device IDs differently since ACPI device IDs are mask based
    // and non-ACPI devices are head based.
    //
    if ((HwDeviceId == QUERY_NONDDC_MONITOR_ID)  ||
        (HwDeviceId == QUERY_NONDDC_MONITOR_ID2) ||
        (HwDeviceId == QUERY_MONITOR_ID)         ||
        (HwDeviceId == QUERY_MONITOR_ID2)        ||
        (HwDeviceId == QUERY_ACPI_CRT_ID)        ||
        (HwDeviceId == QUERY_ACPI_DFP_ID)        ||
        (HwDeviceId == QUERY_ACPI_TV_ID))
    {
        //
        // We deal with only the head0 since the Win2K OS is aware of only head0.
        //
        ULONG ulPrimaryHead;
        BOOL bRet;
        ulPrimaryHead = HwDeviceExtension->DeskTopInfo.ulDeviceDisplay[0];
        switch (VideoPowerManagement->PowerState)
        {
        case VideoPowerOn:
            if (HwDeviceExtension->NVMonitorPowerState[ulPrimaryHead][NV_VideoPowerOn])
            {
                return(NO_ERROR);
            }
            break;
        case VideoPowerOff:
            if (HwDeviceExtension->NVMonitorPowerState[ulPrimaryHead][NV_VideoPowerOff])
            {
                return(NO_ERROR);
            }
            break;
        case VideoPowerHibernate:
            if (HwDeviceExtension->NVMonitorPowerState[ulPrimaryHead][NV_VideoPowerHibernate])
            {
                return(NO_ERROR);
            }
            break;
        case VideoPowerSuspend:
            if (HwDeviceExtension->NVMonitorPowerState[ulPrimaryHead][NV_VideoPowerSuspend])
            {
                return(NO_ERROR);
            }
            break;
        case VideoPowerStandBy:
            if (HwDeviceExtension->NVMonitorPowerState[ulPrimaryHead][NV_VideoPowerStandBy])
            {
                return(NO_ERROR);
            }
            break;
        case VideoPowerShutdown:
            return(NO_ERROR);
            break;

        default:
            ASSERT(FALSE);
            return ERROR_INVALID_PARAMETER;
            break;
        }

        //
        // The requested monitor power state is not supported by ResMan.
        //
        VideoDebugPrint((0,"Oops! NVGetPowerState for Monitor, QueriedState: 0x%x not supported\n",
                        VideoPowerManagement->PowerState));
        return(ERROR_INVALID_PARAMETER);

    } 
    else if (HwDeviceId == DISPLAY_ADAPTER_HW_ID) 

    {
        BOOL bRet;
        
        //
        // We are querying power support for the graphics card.
        //


        switch (VideoPowerManagement->PowerState)
        {
        
        case VideoPowerOn:
            if (HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerOn])
            {
                return(NO_ERROR);
            }
            break;
            
        case VideoPowerHibernate:
            if (HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerHibernate])
            {
                return(NO_ERROR);
            }
            break;
        case VideoPowerStandBy:
            if (HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerStandBy])
            {
                return(NO_ERROR);
            }
            break;
        case VideoPowerSuspend:
            if (HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerSuspend])
            {
                return(NO_ERROR);
            }
            break;
        case VideoPowerOff:
            if (HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerOff])
            {
                return(NO_ERROR);
            }
            break;

        case VideoPowerShutdown:
            return(NO_ERROR);
            break;

        default:
            ASSERT(FALSE);
            return ERROR_INVALID_PARAMETER;
            break;
        }
        // The requested monitor power state is not supported by ResMan.
        VideoDebugPrint((0,"Oops! NVGetPowerState for Adapter, QueriedState: 0x%x not supported\n",
                        VideoPowerManagement->PowerState));
        return(ERROR_INVALID_PARAMETER);

    } else {

        VideoDebugPrint((0, "Unknown HwDeviceId\n"));
        ASSERT(FALSE);
        return ERROR_INVALID_PARAMETER;
    }
}



VP_STATUS
NVSetPowerState(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    ULONG HwDeviceId,
    PVIDEO_POWER_MANAGEMENT VideoPowerManagement
    )

/*++

Routine Description:

    Set the power state for a given device.

Arguments:

    HwDeviceExtension - Pointer to our hardware device extension structure.

    HwDeviceId        - Private unique 32 bit ID identifing the device.

    VideoPowerManagement - Power state information.

Return Value:

    TRUE if power state can be set,
    FALSE otherwise.

--*/

{

    ULONG ulStopImage, ModesetStopImage;

    //
    // Print the request for debugging purposes.
    //
    VideoDebugPrint((1, "nvSetPowerState(): HwDeviceId: 0x%x, Power State: ", HwDeviceId));
    switch (VideoPowerManagement->PowerState)
    {
    
    case VideoPowerOn:
        VideoDebugPrint((1, "VideoPowerOn\n"));
        break;
    case VideoPowerHibernate:
        VideoDebugPrint((1, "VideoPowerHibernate\n"));
        break;

    case VideoPowerStandBy:         
        VideoDebugPrint((1, "VideoPowerStandBy\n"));
        break;
    case VideoPowerSuspend:
        VideoDebugPrint((1, "VideoPowerSuspend\n"));
        break;
    case VideoPowerOff:
        VideoDebugPrint((1, "VideoPowerOff\n"));
        break;
    case VideoPowerShutdown:
        VideoDebugPrint((1, "VideoPowerShutdown\n"));
        break;
    default:
        VideoDebugPrint((1, "Unknown power state: 0x%x\n", VideoPowerManagement->PowerState));
        break;
    }

    //
    // We should not turn on the monitor power state if we have defered a modeset via NVSVC.
    //
    //
    ulStopImage = 0;
    ModesetStopImage = 0;
    if (HwDeviceExtension->ulInduceModeChangeDeviceMask)
    {
        ulStopImage = 1;
    }

    //
    // We should also not turn on the monitor power state if Sony's ModesetStopImage feature is turned on.
    //
    if (HwDeviceExtension->EnableModesetStopImage)
    {
        //
        // Read the registry entry.
        //
        VideoPortGetRegistryParameters(HwDeviceExtension,
                                            L"ModesetStopImage",
                                            FALSE,
                                            NVRegistryCallback,
                                            &(ModesetStopImage));
        if (ModesetStopImage == 1)
        {
            ulStopImage = 1;
        }
    }

    //*************************************************************************
    // Unattended install with Intel 810 present:
    // Make sure to SKIP this call, if the device has NOT been mapped!!!
    // (Skip this function if currently running unattended install with
    // an Intel 810)
    //*************************************************************************
    if (HwDeviceExtension->DeviceMappingFailed == TRUE)
        return NO_ERROR;

    //
    // Make sure we recognize the device.
    //

    if ((HwDeviceId == QUERY_NONDDC_MONITOR_ID)  ||
        (HwDeviceId == QUERY_MONITOR_ID)         ||
        (HwDeviceId == QUERY_NONDDC_MONITOR_ID2) ||
        (HwDeviceId == QUERY_MONITOR_ID2))
    {

        ULONG i, ulHead, State;
        BOOL bRet;

        HwDeviceExtension->curMonitorPowerState = VideoPowerManagement->PowerState;

        for (i = 0; i < HwDeviceExtension->DeskTopInfo.ulNumberDacsActive; i++)
        {
            ulHead = HwDeviceExtension->DeskTopInfo.ulDeviceDisplay[i];

            switch (VideoPowerManagement->PowerState)
            {
            
            case VideoPowerOn:
                //
                // If the image has been stopped, we should not turn the monitor on
                //
                if (ulStopImage)
                {
                    VideoDebugPrint((1, "Image has been stopped, so ignoring monitor poweron\n"));
                    return(NO_ERROR);
                }
                State = HwDeviceExtension->NVMonitorPowerState[ulHead][NV_VideoPowerOn];
                break;
            case VideoPowerHibernate:
                State = HwDeviceExtension->NVMonitorPowerState[ulHead][NV_VideoPowerHibernate];
                break;

            case VideoPowerStandBy:
                State = HwDeviceExtension->NVMonitorPowerState[ulHead][NV_VideoPowerStandBy];
                break;
            case VideoPowerSuspend:
                State = HwDeviceExtension->NVMonitorPowerState[ulHead][NV_VideoPowerSuspend];
                break;
            case VideoPowerOff:
                State = HwDeviceExtension->NVMonitorPowerState[ulHead][NV_VideoPowerOff];
                break;
            case VideoPowerShutdown:
                return(NO_ERROR);
                break;

            default:
                VideoDebugPrint((0, "Unknown power state.\n"));
                ASSERT(FALSE);
                return ERROR_INVALID_PARAMETER;
            }

            bRet = RmSetPowerState(HwDeviceExtension->DeviceReference, ulHead, State);
            if (bRet == FALSE)
            {
                VideoDebugPrint((0,"Oops! RmSetPowerState returned FALSE for monitor %d, for State: 0x%x\n",
                                 ulHead, State));
            }

        }
        return(NO_ERROR);
    }

    //
    // The ACPI table has a "device ID 0 - for Other". Not sure what this device ID refers to.
    //
    if (HwDeviceId == 0)
    {
        VideoDebugPrint((0,"nvSetPowerState: HwID: 0x0, not sure what to do ,returning NO_ERROR\n"));
        return(NO_ERROR);
    }

    //
    // Handle the ACPI devices - CRT, TV, DFP on a laptop
    //
    if ((HwDeviceId == QUERY_ACPI_CRT_ID) ||
        (HwDeviceId == QUERY_ACPI_DFP_ID) ||
        (HwDeviceId == QUERY_ACPI_TV_ID))
    {

        ULONG i, ulHead, State, ulDeviceMask;
        BOOL bRet;

        HwDeviceExtension->curMonitorPowerState = VideoPowerManagement->PowerState;

        ulDeviceMask = ACPIHwIdToMask(HwDeviceId);
        if (ulDeviceMask == 0)
        {
            // Invalid device. simply return NO_ERROR.
            VideoDebugPrint((0,"Invalid deviceId, returning NO_ERROR\n"));
            return(NO_ERROR);
        }

        // Now we need to convert the device mask to a head. Since our ResMan power management interface is
        // specified in terms of head and not device mask. This leaves us with a hole. What if the device 
        // that we watnto set the power state is not currently one of the active heads? Hopefully win2K does not
        // call us in this fashion. Otherwise we will have to make more changes in resman's internal power management code
        // and its interface to the miniport.


        for (i = 0; i < HwDeviceExtension->DeskTopInfo.ulNumberDacsActive; i++)
        {
            ulHead = HwDeviceExtension->DeskTopInfo.ulDeviceDisplay[i];
            if (HwDeviceExtension->DeskTopInfo.ulDeviceMask[ulHead] == ulDeviceMask)
            {
                // We found the head for which this requested device is attached.
                switch (VideoPowerManagement->PowerState)
                {
                
                case VideoPowerOn:
                    State = HwDeviceExtension->NVMonitorPowerState[ulHead][NV_VideoPowerOn];
                    //
                    // If the image has been stopped, we should not turn the monitor on
                    //
                    if (ulStopImage)
                    {
                        VideoDebugPrint((1, "Image has been stopped, so ignoring monitor poweron\n"));
                        return(NO_ERROR);
                    }
                    break;
                case VideoPowerHibernate:
                    State = HwDeviceExtension->NVMonitorPowerState[ulHead][NV_VideoPowerHibernate];
                    break;

                case VideoPowerStandBy:
                    State = HwDeviceExtension->NVMonitorPowerState[ulHead][NV_VideoPowerStandBy];
                    break;
                case VideoPowerSuspend:
                    State = HwDeviceExtension->NVMonitorPowerState[ulHead][NV_VideoPowerSuspend];
                    break;
                case VideoPowerOff:
                    State = HwDeviceExtension->NVMonitorPowerState[ulHead][NV_VideoPowerOff];
                    break;
                case VideoPowerShutdown:
                    return(NO_ERROR);
                    break;
                default:
                    VideoDebugPrint((0, "Unknown power state.\n"));
                    ASSERT(FALSE);
                    return ERROR_INVALID_PARAMETER;
                }

                bRet = RmSetPowerState(HwDeviceExtension->DeviceReference, ulHead, State);
                if (bRet == FALSE)
                {
                    VideoDebugPrint((0,"Oops! RmSetPowerState returned FALSE for monitor %d, for State: 0x%x\n",
                                     ulHead, State));
                }
                return(NO_ERROR);
            }
        }

        // Oops! We did not find any active head which was attached to this device!
        VideoDebugPrint((1,"Did not find the head for the requested deviceMask: 0x%x was attached\n", HwDeviceId));
        return(NO_ERROR);
    }

    if (HwDeviceId == DISPLAY_ADAPTER_HW_ID)
    {


        switch (VideoPowerManagement->PowerState)
        {
        case VideoPowerOn:
            {
                ULONG State;
                BOOL bRet;


                //
                // If the device was previously turned off, we need to reinitialize it.
                //


                if ( (HwDeviceExtension->curAdapterPowerState == VideoPowerHibernate)  ||
                     (HwDeviceExtension->curAdapterPowerState == VideoPowerStandBy)    ||
                     (HwDeviceExtension->curAdapterPowerState == VideoPowerSuspend)    ||
                     (HwDeviceExtension->curAdapterPowerState == VideoPowerOff))
                {
                    VIDEO_REQUEST_PACKET requestPacket;
                    STATUS_BLOCK statusblock;

                    // 
                    // The display driver needs to know whether we are resuming from hibernation/stdby/suspend so write the
                    // power down state to the registry.
                    //
                    VideoPortSetRegistryParameters(HwDeviceExtension,
                                      L"ResumingFromAdapterPowerDownState",
                                      &HwDeviceExtension->curAdapterPowerState,
                                      sizeof(ULONG));


                    if (HwDeviceExtension->CurrentMode==NULL)
                    {
                        //
                        //  If CurrentMode is not initialized, we know that our chip is
                        //  never initialized at all therefore, we must exit immediately.
                        //  And we should return no error. (configuration: ATI on motherboard
                        //  primary, NV3 as secondary, not posted)

                        //  NOTE:  Keep this fix here for now, until we can verify that
                        //  we can write to hardware registers in RmLoadState().
                        //  i.e....If this device was NOT posted, I'm not convinced that
                        //  we'll be able to touch the hardware registers (device is NOT on).
                        //  We should be able to delete this check once BIOS post code is added here.
                        //
                        return(NO_ERROR);
                    }


#ifndef IA64
                    //**********************************************************
                    // If the Via chipset is found, then we need to
                    // fixup one of the THEIR registers in order for AGP to work.
                    // Somewhat dangerous, but hibernation won't resume in
                    // AGP mode unless this register is programmed correctly.
                    //**********************************************************

                    if (HwDeviceExtension->Via694ChipsetFound == TRUE)
                    {

                        //******************************************************
                        // Save the value of this VIA AGP register
                        //******************************************************

                        /*
                        _asm {
                             mov    eax,0x80000088
                             mov    dx,0xcf8
                             out    dx,eax
    
                             mov    dx,0xcfc
                             mov    eax,ViaAgpRegister
                             out    dx,eax
                             }
                        */

                        HalSetBusDataByOffset(PCIConfiguration,
                                              0,    //  Bus 0
                                              0,    //  Slot 0
                                              &ViaAgpRegister,
                                              (0x88 >> 2),  //  Offset in byte
                                              sizeof(ULONG));   //  Size in byte
                    }

                    //**********************************************************
                    // If the AMD 751 chipset is found, then we need to 
                    // fixup two of THEIR registers in order for the AGP to work.
                    //**********************************************************
                    if (HwDeviceExtension->AMD751ChipsetFound == TRUE)
                    {
                        NVAMD751ChipsetRestoreHibernationRegisters(HwDeviceExtension);
                    }
#endif // IA64
                }

                // Call the resman to set to PowerOn state
                State = HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerOn];
                bRet = RmSetPowerState(HwDeviceExtension->DeviceReference, 0, State);
                if (bRet == FALSE)
                {
                    VideoDebugPrint((0,"Oops! RmSetPowerState returned FALSE for adapter for State: 0x%x\n",
                                     State));
                }

                if (HwDeviceExtension->SavedPMCState != 0)
                {
                    NV_ReEnableNVInterrupts(HwDeviceExtension);
                }

                HwDeviceExtension->curAdapterPowerState = VideoPowerManagement->PowerState;

                return(NO_ERROR);
            }


            break;


        case VideoPowerHibernate:

        case VideoPowerStandBy:

        case VideoPowerSuspend:


        case VideoPowerOff:


            {
                VIDEO_REQUEST_PACKET requestPacket;
                STATUS_BLOCK statusblock;
                ULONG State;
                BOOL bRet;
                ULONG PrevDevMask;
                VP_STATUS RegStatus;

                //
                // if the PrevDevMask registry key exists, set it to zero.
                // This is to ensure that we do the begin/end modeset bracket in
                // the display driver.
                //
                PrevDevMask = 0;
                RegStatus = VideoPortGetRegistryParameters(HwDeviceExtension,
                                            L"PrevDevMask",
                                            FALSE,
                                            NVRegistryCallback,
                                            &(PrevDevMask));
                if (RegStatus == NO_ERROR)
                {
                    PrevDevMask = 0;
                    RegStatus = VideoPortSetRegistryParameters(HwDeviceExtension,
                                                L"PrevDevMask",
                                                &(PrevDevMask),
                                                sizeof(ULONG));
                }


                HwDeviceExtension->curAdapterPowerState = VideoPowerManagement->PowerState;

#ifndef IA64
                //**************************************************************
                // If the Via chipset is found, then we need to
                // fixup one of the THEIR registers in order for AGP to work.
                // Somewhat dangerous, but hibernation won't resume in
                // AGP mode unless this register is programmed correctly.
                //**************************************************************

                if (HwDeviceExtension->Via694ChipsetFound == TRUE)
                {

                    //**********************************************************
                    // Save the value of this VIA AGP register
                    //**********************************************************

                    /*
                    _asm {
                         mov    eax,0x80000088
                         mov    dx,0xcf8
                         out    dx,eax

                         mov    dx,0xcfc
                         in     eax,dx
                         mov    ViaAgpRegister,eax
                         }
                    */

                    HalGetBusDataByOffset(PCIConfiguration,
                                          0,    //  Bus 0
                                          0,    //  Slot 0
                                          &ViaAgpRegister,
                                          (0x88 >> 2),  //  Offset in bytes
                                          sizeof(ULONG));   //  Size in bytes
                }

                //**********************************************************
                // If the AMD 751 chipset is found, then we need to 
                // fixup two of THEIR registers in order for the AGP to work.
                //**********************************************************
                if (HwDeviceExtension->AMD751ChipsetFound == TRUE)
                {
                    NVAMD751ChipsetSaveHibernationRegisters(HwDeviceExtension);
                }
#endif // IA64


                switch (  VideoPowerManagement->PowerState)
                {
                
                case VideoPowerHibernate:
                    State = HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerHibernate];
                    break;
                case VideoPowerStandBy:
                    State = HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerStandBy];
                    break;
                case VideoPowerSuspend:
                    State = HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerSuspend];
                    break;
                case VideoPowerOff:
                    State = HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerOff];
                    break;
                default:
                    VideoDebugPrint((0,"Oops! NvSetPowerState(): some invalid PowerState: 0x%x, line: %d\n",
                                     VideoPowerManagement->PowerState, __LINE__));
                    // use some safe off value.
                    State = HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerOff];
                    break;
                }

                // We are going out of high res mode. So indicate it.
                HwDeviceExtension->SystemModeSetHasOccurred = 0;
                VideoPortSetRegistryParameters(HwDeviceExtension,
                                      L"SystemModeSetHasOccurred",
                                      &HwDeviceExtension->SystemModeSetHasOccurred,
                                      sizeof(ULONG));

                NV_DisableNVInterrupts(HwDeviceExtension);

                // Call the ResMan to set the adapter power down to the required state
                // The 'head' arguement is irrelevant since we are setting the adapter power state.
                bRet = RmSetPowerState(HwDeviceExtension->DeviceReference, 0, State);
                if (bRet == FALSE)
                {
                    VideoDebugPrint((0,"Oops! RmSetPowerState returned FALSE for adapter for State: 0x%x\n",
                                 State));
                }

                return(NO_ERROR);
            }

            break;

        case VideoPowerShutdown:
            // Disable interrupts

            NV_DisableNVInterrupts(HwDeviceExtension);
            return(NO_ERROR);

        default:
            //
            // Unrecognized power state.
            //
            VideoDebugPrint((0,"Oops! NvSetPowerState(): some invalid PowerState: 0x%x, line: %d\n",
                                     VideoPowerManagement->PowerState, __LINE__));
            ASSERT(FALSE);
            return ERROR_INVALID_PARAMETER;
            break;
        }
        return(NO_ERROR);
    }

    VideoDebugPrint((0, "Unknown HwDeviceId: 0x%x\n", HwDeviceId));
    ASSERT(FALSE);
    return ERROR_INVALID_PARAMETER;

}

#endif // #ifdef NV_PWR_MGMT

#endif // #ifdef (_WIN32_WINNT >= 0x0500)


#if _WIN32_WINNT >= 0x0500
ULONG NVGetNumBootDevices(PHW_DEVICE_EXTENSION HwDeviceExtension)
{
    ULONG ulConnectedDeviceMask, ulNumberDacsOnBoard;

    ulConnectedDeviceMask = BITMASK_INVALID_DEVICE;
    if (!RmConfigGetKernel(HwDeviceExtension->DeviceReference, NV_CFG_GET_ALL_DEVICES, &ulConnectedDeviceMask))
    {
        return 1;
    }

    if (!RmConfigGetExKernel(HwDeviceExtension->DeviceReference,
                            NV_CFGEX_CHECK_CONNECTED_DEVICES, &ulConnectedDeviceMask, 
                            sizeof(ulConnectedDeviceMask)))
    {
        return 1;
    }
    if (ulConnectedDeviceMask == 0)
    {
        return 1;
    }
    if (!RmConfigGetKernel(HwDeviceExtension->DeviceReference, NV_CFG_NUMBER_OF_HEADS, &ulNumberDacsOnBoard))
    {
        return 1;
    }

    if (ulNumberDacsOnBoard < 2)
    {
        return 1;
    }

    if (HwDeviceExtension->ulDualViewStatus & DUALVIEW_STATUS_MOBILE)
    {
        if (ulConnectedDeviceMask & BITMASK_ALL_DFP)
            return 2;
        else
        {
            // A mobile without Panel????
            ASSERT(FALSE);
            return 1;
        }
    }
    else if (ConvertConnectedMaskToNumDevices(ulConnectedDeviceMask) > 1)
    {
        return 2;
    }
    return 1;
}
#endif

VP_STATUS NVGetRegistryTwinviewInfo (
    PHW_DEVICE_EXTENSION hwDeviceExtension,
    NVTWINVIEWDATA      *pTwinviewInfo
    )
{
    VP_STATUS status = VideoPortGetRegistryParameters(hwDeviceExtension,
                                                      L"TwinviewInfo",
                                                      FALSE,
                                                      NVReadRegistryTwinViewInfoCallback,
                                                      pTwinviewInfo);

    if (status == NO_ERROR)
    {
        VideoDebugPrint((2, "NVGetRegistryTwinviewInfo : HwExtension=0x%08x\n", hwDeviceExtension));
        VideoDebugPrint((2, "State: 0x%x, Orientation: 0x%x, DeskTopRectl: (%d, %d), (%d, %d), AllDeviceMask: 0x%x, ConnectedDevMask: 0x%x, Head Mapping: %d, %d\n",
            pTwinviewInfo->dwState, pTwinviewInfo->dwOrientation, pTwinviewInfo->DeskTopRectl.left,pTwinviewInfo->DeskTopRectl.top,pTwinviewInfo->DeskTopRectl.right,
            pTwinviewInfo->DeskTopRectl.bottom, pTwinviewInfo->dwAllDeviceMask, pTwinviewInfo->dwConnectedDeviceMask, pTwinviewInfo->dwDeviceDisplay[0],pTwinviewInfo->dwDeviceDisplay[1]));
        VideoDebugPrint((2, "dwDeviceMask[2] = {%x,%x}   dwDeviceType[2] = {%x,%x}\n", 
                        pTwinviewInfo->NVTWINVIEWUNION.nvtwdevdata[0].dwDeviceMask, pTwinviewInfo->NVTWINVIEWUNION.nvtwdevdata[1].dwDeviceMask,
                        pTwinviewInfo->NVTWINVIEWUNION.nvtwdevdata[0].dwDeviceType, pTwinviewInfo->NVTWINVIEWUNION.nvtwdevdata[1].dwDeviceType));
        VideoDebugPrint((2,"........................\n"));
    }
    else
    {
        VideoDebugPrint((1, "NVGetRegistryTwinviewInfo : HwExtension=0x%08x\n", hwDeviceExtension));
        VideoDebugPrint((1, "NVGetRegistryTwinviewInfo Failed\n"));
    }

    return status;
}

VP_STATUS NVSetRegistryTwinviewInfo (
    PHW_DEVICE_EXTENSION hwDeviceExtension,
    NVTWINVIEWDATA      *pTwinviewInfo
    )
{
    VideoDebugPrint((2,"NVSetRegistryTwinviewInfo : HwExtension=0x%08x\n", hwDeviceExtension));
    VideoDebugPrint((2,"State: 0x%x, Orientation: 0x%x, DeskTopRectl: (%d, %d), (%d, %d), AllDeviceMask: 0x%x, ConnectedDevMask: 0x%x, Head Mapping: %d, %d\n",
        pTwinviewInfo->dwState, pTwinviewInfo->dwOrientation, pTwinviewInfo->DeskTopRectl.left,pTwinviewInfo->DeskTopRectl.top,pTwinviewInfo->DeskTopRectl.right,
        pTwinviewInfo->DeskTopRectl.bottom, pTwinviewInfo->dwAllDeviceMask, pTwinviewInfo->dwConnectedDeviceMask, pTwinviewInfo->dwDeviceDisplay[0],pTwinviewInfo->dwDeviceDisplay[1]));
    VideoDebugPrint((2, "dwDeviceMask[2] = {%x,%x}   dwDeviceType[2] = {%x,%x}\n", 
                     pTwinviewInfo->NVTWINVIEWUNION.nvtwdevdata[0].dwDeviceMask, pTwinviewInfo->NVTWINVIEWUNION.nvtwdevdata[1].dwDeviceMask,
                     pTwinviewInfo->NVTWINVIEWUNION.nvtwdevdata[0].dwDeviceType, pTwinviewInfo->NVTWINVIEWUNION.nvtwdevdata[1].dwDeviceType));
    VideoDebugPrint((2,"........................\n"));

    return VideoPortSetRegistryParameters(hwDeviceExtension,
                                   L"TwinviewInfo",
                                   pTwinviewInfo,
                                   sizeof(NVTWINVIEWDATA));
}

VP_STATUS NVInitRm (
    PHW_DEVICE_EXTENSION hwDeviceExtension
    )
{
    if (hwDeviceExtension->bRmInitialized)
    {
        NV_ReEnableNVInterrupts(hwDeviceExtension);
        return NO_ERROR;
    }
    
    hwDeviceExtension->bRmInitialized = TRUE;
    
    if (hwDeviceExtension->DeviceMappingFailed == FALSE)
    {
        //**************************************************************************
        // Map the frame buffer here, so the Resource Manager can also get access to it,
        // instead of enablehardware in the display driver.
        //**************************************************************************

        if (NVMapFrameBuffer(hwDeviceExtension) != NO_ERROR)
        {
            return ERROR_INVALID_PARAMETER;
        }

        //**************************************************************************
        // For NV3, instead of passing the Frame buffer pointer back to the RM,
        // we will ONLY map instance memory beginning at address = framebuffer + 12Mb
        // This is because , for NV3, the RM accesses instance memory using the 'frame buffer'
        // ptr, and it must NOT be write combined.  Essentially, for NV3, we map
        // the frame buffer (write combined) and give this to the display driver.
        // But we also map 4Mb of instance memory (NON write combined) and
        // give it to the resource manager.
        //
        //  FrameBufferPtr = WRITE COMBINED mapping of frame buffer
        //  FrameBufferBase = NON write combined mapping of framebuffer+12Mb
        //                    used to access instance memory
        //
        // This is DIFFERENT from NV4 !!
        //**************************************************************************

        SetGlobalHwDev(hwDeviceExtension);

        //**************************************************************************
        // Make sure Non_primary monitor is alive, so that
        // dacConnectMonitorStatus in the RM will correctly detect the monitor!!
        // Only do this for NON primary devices.  Otherwise, the screen
        // will get garbled on the primary monitor.
        //**************************************************************************

        if (MultiAdaptersPresent == TRUE)
           if (hwDeviceExtension->NonPrimaryDevice == TRUE)
                WakeUpCard(hwDeviceExtension);

        // initialize the device
        if (!RmInitNvDevice(hwDeviceExtension->DeviceReference,
                            hwDeviceExtension->NVAccessRanges[NV_PHYSICAL_DFB_ADDRESS].RangeStart.LowPart,
                            hwDeviceExtension->FrameBufferBase))
        {
            return ERROR_INVALID_PARAMETER;
        }
    }

    return NO_ERROR;
}

//******************************************************************************
//
// Function:    NVFindAdapter()
//
// Routine Description:
//
//     This routine is called to determine if the adapter for this driver
//     is present in the system.
//     If it is present, the function fills out some information describing
//     the adapter.
//
// Arguments:
//
//     HwDeviceExtension - Supplies the miniport driver's adapter storage. This
//         storage is initialized to zero before this call.
//
//     HwContext - Supplies the context value which was passed to
//         VideoPortInitialize().
//
//     ArgumentString - Supplies a NULL terminated ASCII string. This string
//         originates from the user.
//
//     ConfigInfo - Returns the configuration information structure which is
//         filled by the miniport driver. This structure is initialized with
//         any known configuration information (such as SystemIoBusNumber) by
//         the port driver. Where possible, drivers should have one set of
//         defaults which do not require any supplied configuration information.
//
//     Again - Indicates if the miniport driver wants the port driver to call
//         its VIDEO_HW_FIND_ADAPTER function again with a new device extension
//         and the same config info. This is used by the miniport drivers which
//         can search for several adapters on a bus.
//
// Return Value:
//
//     This routine must return:
//
//     NO_ERROR - Indicates a host adapter was found and the
//         configuration information was successfully determined.
//
//     ERROR_INVALID_PARAMETER - Indicates an adapter was found but there was an
//         error obtaining the configuration information. If possible an error
//         should be logged.
//
//     ERROR_DEV_NOT_EXIST - Indicates no host adapter was found for the
//         supplied configuration information.
//
//******************************************************************************

VP_STATUS NVFindAdapter(
    PVOID HwDeviceExtension,
    PVOID HwContext,
    PWSTR ArgumentString,
    PVIDEO_PORT_CONFIG_INFO ConfigInfo,
    PUCHAR Again
    )

    {
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    ULONG NV1Slot;
    ULONG OutData;
    ULONG Status;
    PVOID NonCachedSystemMemoryPtr;
    ULONG TestSize;
    ULONG SysMemTextureFlag2;
    ULONG SysMemTextureFlag4;
    ULONG SysMemTextureFlag6;
    ULONG DMADisableFlag;
    ULONG DMAEnableFlag;
    ULONG DisableVBlankFlag;
    VP_STATUS   status;
    int i;
    PCI_COMMON_CONFIG pciData;
    PCI_COMMON_CONFIG ALIPciData;
    PCI_COMMON_CONFIG IntelPciData;
    ULONG SlotSearchNum;
    ULONG BusSearchNum;
    ULONG GetBusDataStatus;
    ULONG AliFoundStatus = FALSE;
    ULONG Intel810FoundStatus = FALSE;
    USHORT usVendorID, usDeviceID;
    BOOLEAN bIsPrimaryDevice;


    //**************************************************************************
    // Initialize this local variable before using it.
    //**************************************************************************
    //numAccessRanges = 0;

    //**************************************************************************
    // Make sure the size of the structure is at least as large as what we
    // are expecting (check version of the config info structure).
    //**************************************************************************

    if (ConfigInfo->Length < sizeof(VIDEO_PORT_CONFIG_INFO)) {

        return ERROR_INVALID_PARAMETER;

    }
#if (_WIN32_WINNT >= 0x0500)
    //**************************************************************************
    // Try to get VideoPortGetAssociatedDeviceID routine address
    //**************************************************************************
    hwDeviceExtension->pfnVideoPortGetAssociatedDeviceID = 
        ConfigInfo->VideoPortGetProcAddress(hwDeviceExtension, "VideoPortGetAssociatedDeviceID");
#endif
    //  Make sure that we only called this once
    //  The reason why RmInitRm() is moved to FindAdapter is that
    //  W2k/Whistler will unload miniport right after DriverEntry
    //  during setup.  Since there is no way to clean up any buffer
    //  allocation which we allocate in RmInitRm(), we end up holding
    //  a chunk of memory.
    //  Also, RmInitRm allocates global clientInfo and dmaListElementPool.
    //  In W2k/Whistler, DriverEntry is called multiple time depending on
    //  the number of devices.  If RmInitRm() is called more than one time,
    //  the new allocation will overwritten the previous one without freeing
    //  them first.  This leads to memory leads.

    if(!g_bIsRmInitRmCalled)
    {
        if(!RmInitRm())
        {
            return ERROR_NOT_ENOUGH_MEMORY;
        }
        g_bIsRmInitRmCalled = TRUE;
    }

    //**************************************************************************
    // Look for ALI 15411 chipset
    // Need to implement a workaround for chipset cache issues
    //**************************************************************************

    SlotSearchNum = 0;
    GetBusDataStatus = FALSE;
    AliFoundStatus   = FALSE;
    hwDeviceExtension->Via694ChipsetFound = FALSE;
    hwDeviceExtension->AMD751ChipsetFound = FALSE;


#if (_WIN32_WINNT >= 0x0500)
#if !defined(_WIN64)

    // Try to find ALI1541 chipset under Win2k
    //
    GetBusDataStatus = HalGetBusData(
                PCIConfiguration,           // bus data type
                0,                          // bus number
                0,                          // slot number
                &ALIPciData,
                sizeof(PCI_COMMON_HDR_LENGTH));

    //**************************************************************************
    // If status is Non-zero, and greater than 2, check the vendor id, device id
    //**************************************************************************
   
    if (GetBusDataStatus > 2)
        {
        if ((ALIPciData.DeviceID == 0x1541) &&
            (ALIPciData.VendorID == 0x10B9))
            {
            AliFoundStatus = TRUE;
            }


        //**********************************************************************
        // Use the same structure, see if a VIA 694 chipset was found
        //**********************************************************************
        if ((ALIPciData.DeviceID == 0x691) &&
            (ALIPciData.VendorID == 0x1106))
            {
            hwDeviceExtension->Via694ChipsetFound = TRUE;
            }
    

        //**********************************************************************
        // Use the same structure, see if an AMD 751 chipset was found
        //**********************************************************************
        if ((ALIPciData.DeviceID == 0x7006) &&
            (ALIPciData.VendorID == 0x1022))
            {
                hwDeviceExtension->AMD751ChipsetFound = TRUE;
                // Note: Do not try to get the BAR1 address of the chipset.
                // It has not been set correctly at this point in time.
                // We will do that when we are going down in hibernation.
            }

        }
    //**************************************************************************
    // Try to find an Intel 810 chip
    //**************************************************************************

    GetBusDataStatus = HalGetBusData(
                PCIConfiguration,           // bus data type
                0,                          // bus number
                1,                          // slot number
                &IntelPciData,
                sizeof(PCI_COMMON_HDR_LENGTH));

    //**************************************************************************
    // If status is Non-zero, and greater than 2, check the vendor id, device id
    //**************************************************************************

    if (GetBusDataStatus > 2)
        {
        if ( ((IntelPciData.DeviceID == 0x7121) || (IntelPciData.DeviceID == 0x7123) || (IntelPciData.DeviceID == 0x7125)) &&
            (IntelPciData.VendorID == 0x8086))
            {
            Intel810FoundStatus = TRUE;
            }

        }
#endif // !defined(_WIN64)


    //*************************************************************************
    // Save the size of physical system memory in the device extension.
    //*************************************************************************
    hwDeviceExtension->SystemMemorySize = ConfigInfo->SystemMemorySize;

#else // (_WIN32_WINNT >= 0x0500)

    // Try to find ALI1541 chipset under NT4

    while ( (SlotSearchNum<8) && (AliFoundStatus == FALSE) )
        {

        GetBusDataStatus = VideoPortGetBusData(
                           hwDeviceExtension,
                           PCIConfiguration,
                           SlotSearchNum,
                           &ALIPciData,
                           0,
                           PCI_COMMON_HDR_LENGTH
                           );

        //**************************************************************************
        // If status is Non-zero, and greater than 2, check the vendor id, device id
        //**************************************************************************

        if (GetBusDataStatus > 2)
            {
            if ((ALIPciData.DeviceID == 0x1541) &&
                (ALIPciData.VendorID == 0x10B9))
                {
                AliFoundStatus = TRUE;
                }

            }


        SlotSearchNum++;

        }

#endif

    //**************************************************************************
    // Tell the display driver to implement an IO workaround for the ALI 1541 chipset
    // Since FindAdapter gets called for the PCI and AGP buses, and
    // the HwDeviceExtension is NOT preserved, we unfortunately have to resort
    // to using a global variable.
    //**************************************************************************

    if (AliFoundStatus)
        Ali_1541_Chipset_Found = TRUE;

    //**************************************************************************
    // Check and see if NV adapter is present.  If NV is present, the
    // the BASE PHYSICAL address of NV is stored in the NVAccessRange
    // member of the device extension.
    //**************************************************************************

    // search for nvidia boards 
    Status = DetectNextDevice(hwDeviceExtension, 
                              &ulFindSlot,                            // global variable !!!
                              2,
                              hwDeviceExtension->NVAccessRanges, 
                              &usVendorID,
                              &usDeviceID,
                              &bIsPrimaryDevice, 
                              TRUE);                                   // claim resources
    if (NO_ERROR != Status)
    {
        return(ERROR_DEV_NOT_EXIST);
    }

    //
    //  update some board related data in the device extension
    //
    hwDeviceExtension->ulSlot       = ulFindSlot;
    hwDeviceExtension->ulChipID     = (ULONG)(usDeviceID) & 0xffff;
    hwDeviceExtension->ulRevLevel   = REV_UNDEF;
    hwDeviceExtension->BoardID      = usVendorID;

    //
    // call our old NVIsPresent routine, this now only updates some RangeOffsets...
    //
    if (!NVIsPresent(hwDeviceExtension, &ulFindSlot))
        return(ERROR_DEV_NOT_EXIST);

    NV1Slot = ulFindSlot;


#if (_WIN32_WINNT >= 0x0500)

    //**************************************************************************
    // Code change request for Win2k per Microsoft
    // Fail if there is a resource conflict
    //**************************************************************************

    if ( (ConfigInfo->BusInterruptLevel == 0 ) && (ConfigInfo->BusInterruptVector == 0))
        return(ERROR_DEV_NOT_EXIST);

#endif

    //**************************************************************************
    // Initialize the Ali_1541 flag in the hwDeviceExtension according
    // to the global flag (Since VideoPortGetBusData doesn't work
    // across buses)
    //**************************************************************************

    if (Ali_1541_Chipset_Found == TRUE)
        hwDeviceExtension->AliChipset_Problem_Found = TRUE;


    //**************************************************************************
    // Assume that the device mapping will always succeed
    //**************************************************************************

    hwDeviceExtension->DeviceMappingFailed = FALSE;

    //**************************************************************************
    // Now map the NV registers so we can get to them
    //**************************************************************************
    Status = NVMapMemoryRanges(hwDeviceExtension);
    if (Status != NO_ERROR)
        {
        //**********************************************************************
        // Our device mapping requires 16Mb.  This call has ALWAYS succeeded, even on 32Mb
        // systems.  However, during Win2k unattended install with an Intel 810 present,
        // this call may fail. In order to workaround this, We set a flag acknowledging
        // this low memory situation.  Subsequent functions will have to be forced
        // to succeed (and do nothing), at least enough to get through the unattended install.
        //**********************************************************************

        if (Intel810FoundStatus == TRUE)

            {
            hwDeviceExtension->DeviceMappingFailed = TRUE;

            ConfigInfo->VdmPhysicalVideoMemoryAddress.LowPart  = 0x000A0000;
            ConfigInfo->VdmPhysicalVideoMemoryAddress.HighPart = 0x00000000;
            ConfigInfo->VdmPhysicalVideoMemoryLength           = 0x00020000;

            ConfigInfo->NumEmulatorAccessEntries     = 0;
            ConfigInfo->EmulatorAccessEntries        = NULL;
            ConfigInfo->EmulatorAccessEntriesContext = 0;

            ConfigInfo->HardwareStateSize = 0;

            *Again = 0;

            //******************************************************************
            // The only thing this function needed to do anyway
            // was to detect the presence of our chip
            //******************************************************************

            return NO_ERROR;
            }

        return(ERROR_INVALID_PARAMETER);
        }


    //**************************************************************************
    // Show all relevant Linear memory ranges to help debug
    //**************************************************************************

    VideoDebugPrint((0,"PMC = 0x%p \n", hwDeviceExtension->NV1_Lin_PMC_Registers));
    VideoDebugPrint((0,"PFB = 0x%p \n",hwDeviceExtension->NV1_Lin_PFB_Registers));
    VideoDebugPrint((0,"PRAMIN = 0x%p \n",hwDeviceExtension->NV3_Lin_PRAMIN_Registers));
    VideoDebugPrint((0,"PRAMFC = 0x%p \n",hwDeviceExtension->NV1_Lin_PRAMFC_Registers));
    VideoDebugPrint((0,"PRAMHT = 0x%p \n",hwDeviceExtension->NV1_Lin_PRAMHT_Registers));
    VideoDebugPrint((0,"PFIFO = 0x%p \n",hwDeviceExtension->NV1_Lin_PFIFO_Registers));
    VideoDebugPrint((0,"PBUS = 0x%p \n",hwDeviceExtension->NV1_Lin_PBUS_Registers));
    VideoDebugPrint((0,"PGRAPH = 0x%p \n",hwDeviceExtension->NV1_Lin_PGRAPH_Registers));
    VideoDebugPrint((0,"CONFIG = 0x%p \n",hwDeviceExtension->NV1_Lin_CONFIG_Registers));
    VideoDebugPrint((0,"PRAMDAC = 0x%p \n",hwDeviceExtension->NV3_Lin_PRAMDAC_Registers));
    VideoDebugPrint((0,"USERDAC = 0x%p \n",hwDeviceExtension->NV3_Lin_USERDAC_Registers));
    VideoDebugPrint((0,"PRMVIO = 0x%p \n",hwDeviceExtension->NV3_Lin_PRMVIO_Registers));
    VideoDebugPrint((0,"PRMCIO = 0x%p \n",hwDeviceExtension->NV3_Lin_PRMCIO_Registers));


#ifdef PC98

    //
    // POST the chip if we're running on a system with no VGA BIOS
    //
    NvPostEx(hwDeviceExtension);

#endif // PC98

    // initialize the RM device register mapping
    if (
        !RmInitNvMapping(
            (VOID*)hwDeviceExtension,
            hwDeviceExtension->NVAccessRanges[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart,
            hwDeviceExtension->NvRegisterBase,
            &hwDeviceExtension->DeviceReference
        )
    )
    {
        return ERROR_DEV_NOT_EXIST;
    }

    // We need to update the IRQ info as the RM has not done this
    {
       U032 oldValue;

       // Get the hals view of the IRQ line from the adapter
       VideoPortGetBusData(
            hwDeviceExtension,
            PCIConfiguration,
            NV1Slot,
            &pciData,
            0,
            PCI_COMMON_HDR_LENGTH
            );

       // Go ahead and set the mapping
       RmConfigSetKernel(hwDeviceExtension->DeviceReference, NV_CFG_IRQ,
                                   (ULONG)pciData.u.type0.InterruptLine, &oldValue);

    }

    //**************************************************************************
    // Flag to denote if adapter is NON primary (ie..device was not booted)
    //**************************************************************************

    hwDeviceExtension->NonPrimaryDevice = FALSE;



    if (!bIsPrimaryDevice)
    {
        VP_STATUS ntStatus = NO_ERROR;
        PHYSICAL_ADDRESS fbPhysAddr;
        ULONG qtyBytesSet, inIoSpace, fbLength;
        PVOID fbAddr;
        // hook up the HAL state now that the chip is accessible
        if (!RmInitNvHal(hwDeviceExtension->DeviceReference))
        {
            return ERROR_DEV_NOT_EXIST;
        }

        // create a large enuff temporary FB mapping to POST the device
        fbPhysAddr.HighPart = 0x00000000;
        fbPhysAddr.LowPart  = hwDeviceExtension->NVAccessRanges[NV_PHYSICAL_DFB_ADDRESS].RangeStart.LowPart;
        fbPhysAddr.LowPart += RangeOffsets[NV_PDFB_INDEX].ulOffset;
        fbLength = 0x04000000 + BIOS_MEM_SIZE_ADJUST_FACTOR;
        inIoSpace = VIDEO_MEMORY_SPACE_P6CACHE;
        fbAddr = NULL;
        ntStatus = VideoPortMapMemory
        (
            hwDeviceExtension,
            fbPhysAddr,
            &fbLength,
            &inIoSpace,
            &fbAddr
        );
        if (ntStatus != NO_ERROR)
        {
            return ntStatus;
        }


        //**********************************************************************
        // Extract the BIOS image (64k) from either PROM or PRAMIN
        // (This function call behaves differently depending on whether
        // MEMORY_SPACE is set by VideoPortSetBusData)
        //**********************************************************************

        NVExtractBiosImage(hwDeviceExtension);


        // POST the device
        if (!RmPostNvDevice(hwDeviceExtension->DeviceReference, (PHWREG)(&(hwDeviceExtension->SavedBiosImageArray[0])), (PHWREG)fbAddr))
        {
            return ERROR_INVALID_PARAMETER;
        }

        // free up the temporary FB mapping
        VideoPortUnmapMemory(
            hwDeviceExtension,
            fbAddr,
            0
        );

        //**************************************************************************
        // If multiple adapters are present, do NOT invoke reset device
        //**************************************************************************

        MultiAdaptersPresent = TRUE;

        //**************************************************************************
        // Flag to denote if adapter is NON primary (ie..device was not booted)
        //**************************************************************************

        hwDeviceExtension->NonPrimaryDevice = TRUE;
    }
    else
    {
        // hook up the HAL state for the primary device
        if (!RmInitNvHal(hwDeviceExtension->DeviceReference))
        {
            return ERROR_DEV_NOT_EXIST;
        }
    }

    //**************************************************************************
    // If this is the PRIMARY adapter, we still need to save off the BIOS image
    // (ExtractBiosImage behaves differently, the BIOS image will be in PROM or PRAMIN)
    //**************************************************************************

    if (hwDeviceExtension->NonPrimaryDevice == FALSE)
        NVExtractBiosImage(hwDeviceExtension);



    //**************************************************************************
    // Fill in NVInfo information structure
    //**************************************************************************

    if (NVGetNVInfo(hwDeviceExtension) == FALSE)
        return(ERROR_DEV_NOT_EXIST);        // Default to VGA if error getting info


    //
    // Store Memory Size
    //

    VideoPortSetRegistryParameters(HwDeviceExtension,
                                   L"HardwareInformation.MemorySize",
                                   &(hwDeviceExtension->NvInfo.Framebuffer.RamSize),
                                   sizeof(ULONG));

    //
    // We have this so that the int10 will also work on the VGA also if we
    // use it in this driver.
    //

    ConfigInfo->VdmPhysicalVideoMemoryAddress.LowPart  = 0x000A0000;
    ConfigInfo->VdmPhysicalVideoMemoryAddress.HighPart = 0x00000000;
    ConfigInfo->VdmPhysicalVideoMemoryLength           = 0x00020000;

    //
    // Clear out the Emulator entries and the state size since this driver
    // does not support them.
    //

    ConfigInfo->NumEmulatorAccessEntries     = 0;
    ConfigInfo->EmulatorAccessEntries        = NULL;
    ConfigInfo->EmulatorAccessEntriesContext = 0;

    //
    // This driver does not do SAVE/RESTORE of hardware state.
    //

    ConfigInfo->HardwareStateSize = 0;

    //**************************************************************************
    // Check the DisableVBLANK registry setting
    //**************************************************************************

    DisableVBlankFlag = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       L"DisableVBlank",
                                       FALSE,
                                       NVRegistryCallback,
                                       NULL);

    if (DisableVBlankFlag == NO_ERROR)
        hwDeviceExtension->DisableVBlankFlag = TRUE;
    else
        hwDeviceExtension->DisableVBlankFlag = FALSE;

    //**************************************************************************
    // Check to see if we should handle IF09 format video data.
    //**************************************************************************
    hwDeviceExtension->bEnableIF09 = TRUE;
    status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       L"IF09",
                                       FALSE,
                                       NVRegistryCallback,
                                       &(hwDeviceExtension->bEnableIF09));
#if _WIN32_WINNT < 0x0500
    //**************************************************************************
    // Check the EnableExtRefresh registry setting. If this is false or
    // not present we limit refresh rates to 85Hz.
    //**************************************************************************
    hwDeviceExtension->bExtRefreshDisable = FALSE;
    status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       L"DisableExtRefresh",
                                       FALSE,
                                       NVRegistryCallback,
                                       &(hwDeviceExtension->bExtRefreshDisable));
#endif // #if _WIN32_WINNT < 0x0500

    //**************************************************************************
    // Check the Disable19x12Mode registry setting.
    //**************************************************************************
    hwDeviceExtension->bMax16x12 = FALSE;
    status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       L"Max16x12",
                                       FALSE,
                                       NVRegistryCallback,
                                       &(hwDeviceExtension->bMax16x12));

    hwDeviceExtension->bDisable19x12x32at60 = FALSE;
    status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       L"Disable19x12x32at60",
                                       FALSE,
                                       NVRegistryCallback,
                                       &(hwDeviceExtension->bDisable19x12x32at60));

    hwDeviceExtension->bDisable16x12x32ge70 = FALSE;
    status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       L"Disable16x12x32ge70",
                                       FALSE,
                                       NVRegistryCallback,
                                       &(hwDeviceExtension->bDisable16x12x32ge70));

    hwDeviceExtension->bDisable19x10x32ge70 = FALSE;
    status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       L"Disable19x10x32ge70",
                                       FALSE,
                                       NVRegistryCallback,
                                       &(hwDeviceExtension->bDisable19x10x32ge70));

    hwDeviceExtension->ulDisableDfpModesGreaterThan = 0;
    status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       L"DisableDfpModesGreaterThan",
                                       FALSE,
                                       NVRegistryCallback,
                                       &(hwDeviceExtension->ulDisableDfpModesGreaterThan));

    //**************************************************************************
    // DellSwitch1 registry switch 1:
    //
    // 1) Add 1600x1024 modes 76hz for specific customers
    // 2) Also change 1920x1200 75Hz to 1920x1200 76Hz
    //**************************************************************************

    hwDeviceExtension->bDellSwitch1 = FALSE;
    status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       L"DellSwitch1",
                                       FALSE,
                                       NVRegistryCallback,
                                       &(hwDeviceExtension->bDellSwitch1));


    //**************************************************************************
    // OGL Stencil registry switch
    //**************************************************************************

    hwDeviceExtension->OglStencilSwitch = FALSE;
    status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       L"OglStencil",
                                       FALSE,
                                       NVRegistryCallback,
                                       &(hwDeviceExtension->OglStencilSwitch));

    //**************************************************************************
    // Force SW cursor registry switch
    //**************************************************************************

    hwDeviceExtension->ulForceSwCursor = FALSE;
    status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       L"ForceSwCursor",
                                       FALSE,
                                       NVRegistryCallback,
                                       &(hwDeviceExtension->ulForceSwCursor));


    //**************************************************************************
    // This function is only meaningful for NV4 or better (uses newer RM modeset code)
    // It creates the list of available modes, depending on
    // what modes are available in vesadata.c, and what modes
    // are specified in the registry
    //**************************************************************************

    BuildAvailableModesTable(HwDeviceExtension);

    //**************************************************************************
    // This function is only meaningful for NV4 or better
    // If the NV registry switches are NOT present, then write them to the registry.
    // (Otherwise read the registry values).
    // Then load the registry values into the NT_NV_REGISTRY structure
    // so the miniport/display driver can get to them.
    //**************************************************************************

    LoadRegistrySettings(HwDeviceExtension);


    //**************************************************************************
    // ** MODE DATA FIXUP HACK **
    // The ModeTimingTable is normally a fixed STATIC data table.
    // We can't just ADD another refresh rate without some major changes.
    // So for now, just manually change the 1920x1200 75Hz timing to 1920x1200 76Hz
    // (Note: ValidateModes in nvmode.c also needs to be changed!)
    //
    // OEM Customer wants a SPECIFIC 76hz timing which is NOT GTF or DMT compliant.
    // So we'll accommodate them an use their specific timing....
    //**************************************************************************

    if (hwDeviceExtension->bDellSwitch1)
        {

// Standard GTF timing

//        ModeTimingTable[14*NUMBER_OF_RATES+3][0] = 2624;
//        ModeTimingTable[14*NUMBER_OF_RATES+3][1] = 2064;
//        ModeTimingTable[14*NUMBER_OF_RATES+3][2] = 2272;
//        ModeTimingTable[14*NUMBER_OF_RATES+3][3] = 1253;
//        ModeTimingTable[14*NUMBER_OF_RATES+3][4] = 1201;
//        ModeTimingTable[14*NUMBER_OF_RATES+3][5] = 1204;
//        ModeTimingTable[14*NUMBER_OF_RATES+3][6] = 24988;
//        ModeTimingTable[14*NUMBER_OF_RATES+3][7] = 7600;

// OEM specific requested timing (for Wide Sony monitor)

        ModeTimingTable[14*NUMBER_OF_RATES+3][0] = 2584;
        ModeTimingTable[14*NUMBER_OF_RATES+3][1] = 1984;
        ModeTimingTable[14*NUMBER_OF_RATES+3][2] = 2240;
        ModeTimingTable[14*NUMBER_OF_RATES+3][3] = 1250;
        ModeTimingTable[14*NUMBER_OF_RATES+3][4] = 1203;
        ModeTimingTable[14*NUMBER_OF_RATES+3][5] = 1206;
        ModeTimingTable[14*NUMBER_OF_RATES+3][6] = 24548;
        ModeTimingTable[14*NUMBER_OF_RATES+3][7] = 7600;
        ModeTimingTable[14*NUMBER_OF_RATES+3][8] = BUFFER_HSYNC_NEGATIVE;
        ModeTimingTable[14*NUMBER_OF_RATES+3][9] = BUFFER_VSYNC_NEGATIVE;


        }


#if _WIN32_WINNT >= 0x0500
    //**************************************************************************
    // Determine if we should DISABLE AGP support (Default=ENABLE AGP support)
    //**************************************************************************
    status = VideoPortGetRegistryParameters(hwDeviceExtension,
                                       L"DisableAgp",
                                       FALSE,
                                       NVRegistryCallback,
                                       &(hwDeviceExtension->bDisableAgp));


#endif // #if _WIN32_WINNT >= 0x0500


#ifdef NVPE
    //**************************************************************************
    // Determine if we should DISABLE VPE (Video Port Extension that controls
    // the Media Port). Default is VPE Enabled !
    //**************************************************************************
    status = VideoPortGetRegistryParameters(
                hwDeviceExtension,
                L"VPEENABLE",
                FALSE,
                NVRegistryCallbackPnpId,
                (PVOID)(&(hwDeviceExtension->dwVPEEnabled)));
    if (status != NO_ERROR)
    {
       // most probably, we didn't find the VPEEnable key, so enable VPE by default !!!
       hwDeviceExtension->dwVPEEnabled = TRUE;
    }
#endif


    *Again = 0;

#if _WIN32_WINNT < 0x0500

    //
    // on NT4 *Again == TRUE indicates the OS to call us back for a further device with FindAdapter and a new hwDeviceExtension
    // so we check here if this is necessary 
    // different here is that we don't want to claim the resources
    //
    {
        ULONG ulNextSlot;
        BOOLEAN bPrimDev;
        USHORT usVendorID, usDeviceID;
        ulNextSlot = ulFindSlot;
        if (NO_ERROR == DetectNextDevice( hwDeviceExtension, 
                                        &ulNextSlot,
                                        2,
                                        NULL, 
                                        &usVendorID,
                                        &usDeviceID,
                                        &bPrimDev,
                                        FALSE))
        {
            *Again = TRUE;
        }
        else
        {
            // reset global slot counter to be ready for the next bus...
            ulFindSlot = 0xFFFFFFFF;
        }
    }
#endif
    
    NVUpdateRegNVInfo(hwDeviceExtension);

#if _WIN32_WINNT >= 0x0500
    //**************************************************************************
    // Copy driver registry path over
    //**************************************************************************
    for (i = 0; i < sizeof(hwDeviceExtension->DriverRegPath) && ConfigInfo->DriverRegistryPath[i] != 0; i++)
        hwDeviceExtension->DriverRegPath[i] = (UCHAR)ConfigInfo->DriverRegistryPath[i];
    if (i < sizeof(hwDeviceExtension->DriverRegPath))
        hwDeviceExtension->DriverRegPath[i] = 0;

    //
    // Read in the registry entry to see if we shoud export the EDID for the DFP or not.
    // This is only to fix a Win2k PnP bug of failing to install PnP monitor.  It only affects Win2k
    //
    hwDeviceExtension->DFPAsDefaultMonitor = 0;
    if (!(ConfigInfo->VideoPortGetProcAddress)(hwDeviceExtension, "VideoPortCreateSecondaryDisplay"))
    {
        VideoPortGetRegistryParameters(hwDeviceExtension,
                                       L"DFPAsDefaultMonitor",
                                       FALSE,
                                       NVRegistryCallback,
                                       (PVOID)&hwDeviceExtension->DFPAsDefaultMonitor);
    }

    //**************************************************************************
    // Enable DualView based several rules
    // 1. Check if setup allows it
    // 2. Check if OS supports it 
    // 3. Check if the chipn has the capability
    //**************************************************************************
    status = VideoPortGetRegistryParameters(hwDeviceExtension,
                                       L"EnableDualView",
                                       FALSE,
                                       NVRegistryCallback,
                                       &OutData);
    if (status == NO_ERROR && OutData)
    {
        typedef
        VP_STATUS
        (*PVIDEO_PORT_CREATE_SECONDARY_DISPLAY)(
            IN PVOID     HwDeviceExtension,
            IN OUT PVOID *SecondaryDeviceExtension,
            IN ULONG     ulFlag
        );
        PVIDEO_PORT_CREATE_SECONDARY_DISPLAY VideoPortCreateSecondaryDisplay = NULL;

        if (ConfigInfo->VideoPortGetProcAddress)
        {
            VideoPortCreateSecondaryDisplay = (PVIDEO_PORT_CREATE_SECONDARY_DISPLAY)
                                              ((ConfigInfo->VideoPortGetProcAddress)(hwDeviceExtension, "VideoPortCreateSecondaryDisplay"));
            if (VideoPortCreateSecondaryDisplay)
            {
                //
                // For Dualview, we moved NVInitRm from HwInitialize to FindAdapter for the reason of detecting DACs beforehand.
                // One side effect of this change is that NVInitRm enables interrupt by itself, thus cause VideoPortFindAdapter
                // having trouble at IOConnectInterrupt (An Interrupt storm happens on toshiba when Reboot).  The fix is to 
                // Disable Interrupt explicitly here and enable it explicitly at NVInitialize.
                //
                if (NVInitRm(hwDeviceExtension) != NO_ERROR)
                    return ERROR_INVALID_PARAMETER;
                NV_DisableNVInterrupts(hwDeviceExtension);

                // Check if it's Laptop
                status = VideoPortGetRegistryParameters(hwDeviceExtension,
                                                   L"DualviewMobile",
                                                   FALSE,
                                                   NVRegistryCallback,
                                                   &OutData);
                if (status == NO_ERROR && OutData)
                {
                   hwDeviceExtension->ulDualViewStatus |= DUALVIEW_STATUS_MOBILE;
                }

                //
                // Only we have two DACs (and physically connected two devices for Desktop),
                // have enough video memory, we enable DualView
                //
                if (NVGetNumBootDevices(hwDeviceExtension) > 1 &&
                    hwDeviceExtension->NvInfo.Framebuffer.RamSize >= VIDEOMEMSIZELIMIT_FOR_DUALVIEW)
                {
                    PHW_DEVICE_EXTENSION hwDE2;
                    status = VideoPortCreateSecondaryDisplay(HwDeviceExtension,
                                                             (PVOID*)&hwDE2,
                                                             (hwDeviceExtension->ulDualViewStatus & DUALVIEW_STATUS_MOBILE) ? VIDEO_DUALVIEW_REMOVABLE : 0);
                    if (status == NO_ERROR)
                    {
                        hwDeviceExtension->ulDualViewStatus |= DUALVIEW_STATUS_ENABLE_DUALVIEW;

                        VideoPortMoveMemory(hwDE2, hwDeviceExtension, sizeof(HW_DEVICE_EXTENSION));
                        hwDE2->ulDualViewStatus |= DUALVIEW_STATUS_SECONDARY;
                        hwDE2->pDualViewAssociatedDE = hwDeviceExtension;
                        hwDeviceExtension->pDualViewAssociatedDE = hwDE2;

                        // BUGBUG  We assume secondary display always be primary device index plus 1.
                        // But the assumption is broken on combination of Multimon and Dualview
                        // The regkey path should only be used for debug purposes
                        for (i = 0; i < sizeof(hwDE2->DriverRegPath) && ConfigInfo->DriverRegistryPath[i] != 0; i++)
                            hwDE2->DriverRegPath[i] = (UCHAR)ConfigInfo->DriverRegistryPath[i];
                        if (i < sizeof(hwDE2->DriverRegPath))
                            hwDE2->DriverRegPath[i] = 0;
                        if (i != 0)
                            hwDE2->DriverRegPath[i]++;

                        NVUpdateRegNVInfo(hwDE2);
                    }
                }
            }
        }
    }

#endif // #if _WIN32_WINNT >= 0x0500

    //**************************************************************************
    // Indicate a successful completion status.
    //**************************************************************************

    return NO_ERROR;

    } // NVFindAdapter()


#if (_WIN32_WINNT >= 0x0500)
//*******************************************************************************
//
//  Function: NVUpdateDualViewDevice
//
//  For DualView.
//
//  Update the secondary display Hardware_Device_Extension and copy relevant
//  information after HwInitialize() be called.
//
//*******************************************************************************

VOID NVUpdateDualViewDevice (PHW_DEVICE_EXTENSION pHwDE)
    {
    PHW_DEVICE_EXTENSION    pHwDE2;
    ULONG                   status;
    ULONG                   SecondviewMemSize, dwMinSize = 640*480;

    //
    // We can not copy ALL HwDeviceExtension data
    //
    if ((pHwDE2 = pHwDE->pDualViewAssociatedDE) == NULL)
        return;

    pHwDE2->CursorTopScanLine    = pHwDE->CursorTopScanLine;
    pHwDE2->CursorBottomScanLine = pHwDE->CursorBottomScanLine;
    pHwDE2->CursorEnable         = pHwDE->CursorEnable;
    pHwDE2->curAdapterPowerState = pHwDE->curAdapterPowerState;
    pHwDE2->curMonitorPowerState = pHwDE->curMonitorPowerState;
    pHwDE2->BiosArea             = pHwDE->BiosArea;
    pHwDE2->flMapMemory          = pHwDE->flMapMemory;

    pHwDE2->NvInfo          = pHwDE->NvInfo;
    pHwDE2->FrameBufferPtr  = pHwDE->FrameBufferPtr;
    pHwDE2->FrameBufferBase = pHwDE->FrameBufferBase;

    pHwDE2->TotalInstanceMemory = pHwDE->TotalInstanceMemory;

    pHwDE2->AgpPagesTotal   = pHwDE->AgpPagesTotal;
    pHwDE2->bAgp            = pHwDE->bAgp;
    pHwDE2->AgpServices     = pHwDE->AgpServices;

    pHwDE2->GammaCache      = pHwDE->GammaCache;
    VideoPortMoveMemory(pHwDE2->GammaCacheData, pHwDE->GammaCacheData, sizeof(pHwDE->GammaCacheData));

    pHwDE2->ulCurrentMacrovisionKey = pHwDE->ulCurrentMacrovisionKey;
    pHwDE2->ulNextMacrovisionKey    = pHwDE->ulNextMacrovisionKey;
    pHwDE2->ulMacrovisionMode       = pHwDE->ulMacrovisionMode;

    pHwDE2->MaxFlatPanelWidth  = pHwDE->MaxFlatPanelWidth;
    pHwDE2->MaxFlatPanelHeight = pHwDE->MaxFlatPanelHeight;

    //
    // Corresponding to NVInitialize_DacsAndEDID(pHwDE2, TRUE);
    // The heads need to be reversed
    //
    pHwDE2->EdidCollection  = pHwDE->EdidCollection;
    pHwDE2->DeskTopInfo     = pHwDE->DeskTopInfo;
    {
        NVTWINVIEWDATA TwinviewInfo;

        if (NVGetRegistryTwinviewInfo(pHwDE, &TwinviewInfo) == NO_ERROR)
        {
            NVSetRegistryTwinviewInfo(pHwDE2, &TwinviewInfo);
        }
    }
    
    SetTimingOverRide(pHwDE2);

    //
    // Corresponding to InitializeCommonModesetCode()
    //
    pHwDE2->pFPStateSave         = pHwDE->pFPStateSave;
    pHwDE2->pFPMutex             = pHwDE->pFPMutex;
    VideoPortMoveMemory(pHwDE2->maxPixelClockFrequency_8bpp, pHwDE->maxPixelClockFrequency_8bpp, sizeof(pHwDE->maxPixelClockFrequency_8bpp));
    VideoPortMoveMemory(pHwDE2->maxPixelClockFrequency_16bpp, pHwDE->maxPixelClockFrequency_16bpp, sizeof(pHwDE->maxPixelClockFrequency_16bpp));
    VideoPortMoveMemory(pHwDE2->maxPixelClockFrequency_32bpp, pHwDE->maxPixelClockFrequency_32bpp, sizeof(pHwDE->maxPixelClockFrequency_32bpp));

    NVSetBiosInfo(pHwDE2);
    NVValidateModes(pHwDE2);
    }
#endif


//******************************************************************************
//
// Function: NVInitialize()
//
// Routine Description:
//
//     This routine does one time initialization of the device.
//
// Arguments:
//
//     HwDeviceExtension - Pointer to the miniport driver's adapter information.
//
// Return Value:
//
//     None.
//
//******************************************************************************

BOOLEAN NVInitialize(
    PVOID HwDeviceExtension
    )
    {

    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    ULONG Status;


    //**************************************************************************
    // Check for unattended install with Intel 810.
    // If so, then just return true.
    //**************************************************************************

    if (hwDeviceExtension->DeviceMappingFailed == TRUE)
        return TRUE;

    //**************************************************************************
    // Set up the default cursor position and type.
    //**************************************************************************


    hwDeviceExtension->CursorTopScanLine = 0;
    hwDeviceExtension->CursorBottomScanLine = 31;
    hwDeviceExtension->CursorEnable = TRUE;

    hwDeviceExtension->BiosArea = (PUSHORT)NULL;

    //***************************************************************************
    // Default to disabling write combining when mapping fb.
    //***************************************************************************
    hwDeviceExtension->flMapMemory = 0;

    //***************************************************************************
    // Initialize current power state.
    //***************************************************************************
    hwDeviceExtension->curAdapterPowerState = VideoPowerOn;
    hwDeviceExtension->curMonitorPowerState = VideoPowerOn;

    if (NVInitRm(hwDeviceExtension) != NO_ERROR)
        return FALSE;

    // One time initialization of the twinview structures.
    NVOneTimeInit_TwinView(hwDeviceExtension);

    // One time initialization of the saveSettings table.
    vInitializeSaveSettings(hwDeviceExtension);

    // Debug print
    vPrintSaveSettings(hwDeviceExtension);

    // Detect the Head info and read in the EDIDs
    // The TRUE indicates it is a boottime operation.
    if (!NVInitialize_DacsAndEDID(hwDeviceExtension, TRUE))
    {
        return FALSE;
    }

    // Initialize our CRT timing standard overrides.
    SetTimingOverRide(HwDeviceExtension);

    // One time initialization of the common mode set code fields.
    InitializeCommonModesetCode(hwDeviceExtension);

    // One time initialization of the mapping of power states from Win2K's states to nVidia's states.
    InitializeACPIStates(hwDeviceExtension);


    //**************************************************************************
    // Store BIOS revision string in the registry. This must be
    // done now since we need to call the RM to get the BIOS rev,
    // and the RM is not fully initialized at FindAdapter time.
    //**************************************************************************
    NVSetBiosInfo(hwDeviceExtension);

    //**************************************************************************
    // Get monitor info (TV out) into NVInfo struct
    //**************************************************************************
    NVGetMonitorType(hwDeviceExtension);

    //**************************************************************************
    // Get total amount of instance memory.
    // We need to save/restore all of instance memory across a Win2k standby
    //**************************************************************************

    RmConfigGetKernel(hwDeviceExtension->DeviceReference, NV_CFG_INSTANCE_TOTAL, &(hwDeviceExtension->TotalInstanceMemory));

    if (hwDeviceExtension->TotalInstanceMemory > MAX_INSTANCE_MEM_SIZE)
        {
        VideoDebugPrint((2, "ERROR:  ----------------------------- !!!!\n"));
        VideoDebugPrint((2, "ERROR:  Need to increase size of      !!!!\n"));
        VideoDebugPrint((2, "ERROR:  SavedInstanceMemory buffer    !!!!\n"));
        VideoDebugPrint((2, "ERROR:  in HwDeviceExtension          !!!!\n"));
        VideoDebugPrint((2, "ERROR:  ----------------------------- !!!!\n"));
        }

    //**************************************************************************
    // Validate the modes.
    // This function call was originally in NVFindAdapter.
    // However, since it depends on the result of GetMonitorType,
    // (whether we're running on a TV or not), it needs to occur
    // AFTER GetMonitorType.
    //**************************************************************************

    NVValidateModes(hwDeviceExtension);

    // Check to see if the DFP supports 1400x1050 natively on a laptop.
    Verify1400x1050DFP(hwDeviceExtension);

#if (_WIN32_WINNT >= 0x0500) && (!defined(DISABLE_AGP))
    {
    //**************************************************************************
    // Check registry switch to see if we disabled AGP support
    //**************************************************************************

    if (hwDeviceExtension->bDisableAgp)
        goto Agp_Done;

    //**************************************************************************
    // Disable AGP if running on an ALI 1541 chipset
    //**************************************************************************

    if (hwDeviceExtension->AliChipset_Problem_Found)
        goto Agp_Done;

    hwDeviceExtension->AgpPagesTotal = ((MAX_AGP_HEAP + (PAGE_SIZE - 1)) &
                                            ~(PAGE_SIZE - 1)) / PAGE_SIZE;

    //***********************************************************
    // Determine if AGP memory is available.
    //***********************************************************
    hwDeviceExtension->bAgp =
        VideoPortGetAgpServices(hwDeviceExtension,
            &(hwDeviceExtension->AgpServices));
    }
Agp_Done:

    //**************************************************************************
    // If successful, let the RM know where the AGP services are
    //**************************************************************************
    if (hwDeviceExtension->bAgp)
    {
        RmSetAgpServices(hwDeviceExtension->DeviceReference,
                         &hwDeviceExtension->AgpServices);
    }

#endif // #if _WIN32_WINNT >= 0x0500

    {
      PVIDEO_CLUTDATA   pvcdItem;
      int i, iHead;

      VideoDebugPrint((1, "NVInitialize - Initializing the Gamma cache values to defaults\n"));

      // - Init the clut params
      hwDeviceExtension->GammaCache.NumEntries = 256;
          hwDeviceExtension->GammaCache.FirstEntry = 0;

                pvcdItem = &hwDeviceExtension->GammaCache.LookupTable->RgbArray;

      // - Init the CLUT data to a default gamma ramp
      for(iHead = 0; iHead < NV_NO_DACS; iHead++)
      {
        for(i = 0; i < 256; i++) 
        {
            pvcdItem[i+iHead*256].Blue   = (UCHAR)i; /* blue value */
            pvcdItem[i+iHead*256].Green  = (UCHAR)i; /* green value */
            pvcdItem[i+iHead*256].Red    = (UCHAR)i; /* red value */
            pvcdItem[i+iHead*256].Unused = (UCHAR)0; /* unused value */
        }
      }

    }

   
    // To rid oursleves of some nasty visual effects when opening the panel when on a TV,
    // we need to stash the encoder status and type in the registry for later reference
    {
        NV_CFGEX_GET_TV_ENCODER_MULTI_PARAMS TVInfo;
        ULONG TVEncoderType;
        ULONG TVEncoderID;

        // Assume that this adapter lacks a TV encoder 
        TVEncoderType = TV_ENCODER_NONE;
        TVEncoderID = 0;

        // Use head 0 for this case
        TVInfo.Head = 0;
        TVInfo.EncoderType = TV_ENCODER_NONE;
        TVInfo.EncoderID = 0;

        // Attemp the resman call for the encoder info...
        if (RmConfigGetExKernel(hwDeviceExtension->DeviceReference,
                                NV_CFGEX_GET_TV_ENCODER_INFO_MULTI, &TVInfo, 
                                sizeof(NV_CFGEX_GET_TV_ENCODER_MULTI_PARAMS)))

        {

            // We have a valid blob from the resman, so extract the encoder info
            TVEncoderType = TVInfo.EncoderType;
            TVEncoderID = TVInfo.EncoderID;

        }
        else
        {

            // The resman had issues with our request, so spew a warning
            VideoDebugPrint((0, "NVInitialize -  RmConfigGetExKernel() returned error for NV_CFGEX_GET_TV_ENCODER_INFO_MULTI\n"));
        }

        //
        // Save the encoder info in the HwDevExtension as it gets referenced frequently
        //
        hwDeviceExtension->ulTVEncoderType = TVEncoderType;
        hwDeviceExtension->ulTVEncoderID =   TVEncoderID;

        //
        // Set up the max TV resolution. We need to pass it to the common modeset code while calling
        // FindModeEntry().
        //
        if (TVInfo.EncoderType == NV_ENCODER_BROOKTREE_871)
        {
            hwDeviceExtension->ulMaxTVOutResolution = (1024 << 16) | 768;
        }
        else
        {
            hwDeviceExtension->ulMaxTVOutResolution = (800 << 16) | 600;
        }


        // No matter what the outcome above, flush the derived encoder values to the registry

        VideoPortSetRegistryParameters( HwDeviceExtension,
                                        L"TVEncoderType",
                                        &TVEncoderType,
                                        sizeof(ULONG));

        VideoPortSetRegistryParameters( HwDeviceExtension,
                                        L"TVEncoderID",
                                        &TVEncoderID,
                                        sizeof(ULONG));

    }

    // After initialing the EDIDs, parse the EDIDs for the best supported resolution the first time boot
    // after driver installation.
    HandleBestResolution(HwDeviceExtension);
    

#if (_WIN32_WINNT >= 0x0500)
    // Initializing the macrovision
    hwDeviceExtension->ulCurrentMacrovisionKey = -1;
    hwDeviceExtension->ulNextMacrovisionKey = 0x99999999;
    hwDeviceExtension->ulMacrovisionMode = 3;

    NVUpdateDualViewDevice(hwDeviceExtension);
#endif
    return TRUE;

    } // NVInitialize()


//******************************************************************************
//
// Function: NVStartIO()
//
// Routine Description:
//
//    This routine is the main execution routine for the miniport driver. It
//    acceptss a Video Request Packet, performs the request, and then returns
//    with the appropriate status.
//
// Arguments:
//
//    HwDeviceExtension - Pointer to the miniport driver's adapter information.
//
//    RequestPacket - Pointer to the video request packet. This structure
//        contains all the parameters passed to the VideoIoControl function.
//
// Return Value:
//
//    This routine will return error codes from the various support routines
//    and will also return ERROR_INSUFFICIENT_BUFFER for incorrectly sized
//    buffers and ERROR_INVALID_FUNCTION for unsupported functions.
//
//******************************************************************************


BOOLEAN
NVStartIO(
    PVOID HwDeviceExtension,
    PVIDEO_REQUEST_PACKET RequestPacket
    )


    {
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    VP_STATUS status;
    VIDEO_MODE videoMode;
    PVIDEO_MEMORY_INFORMATION memoryInformation;
    ULONG inIoSpace;

    PVIDEO_SHARE_MEMORY pShareMemory;
    PVIDEO_SHARE_MEMORY_INFORMATION pShareMemoryInformation;
    PHYSICAL_ADDRESS shareAddress;
    PVOID virtualAddress;
    ULONG byteLength;
    ULONG sharedViewSize;
    VIDEO_X86_BIOS_ARGUMENTS biosArguments;
    ULONG Address;
    PHYSICAL_ADDRESS NVFrameBuffer;
    PHYSICAL_ADDRESS AliFixupBaseAddress;
    ULONG IOLength;
    PVOID AliVirtualAddress;
    PHWINFO NVInfo;

    //**************************************************************************
    // Switch on the IoContolCode in the RequestPacket. It indicates which
    // function must be performed by the driver.
    //**************************************************************************

    VideoDebugPrint((2, "W32StartIO Entry - %08.8x\n", RequestPacket->IoControlCode));

    switch (RequestPacket->IoControlCode) {
    
#ifdef ENABLE_WIN2K_HOTKEY_CODE
#if (_WIN32_WINNT >= 0x0500)

    //**************************************************************************
    // GET_CHILD_STATE
    //**************************************************************************
    case IOCTL_VIDEO_GET_CHILD_STATE:
        
        VideoDebugPrint((1, "\nW32StartIO - SetChildState\n"));
        status = NVGetChildState(HwDeviceExtension, RequestPacket);
        break;

    //**************************************************************************
    // VALIDATE_CHILD_STATE_CONFIGURATION
    //**************************************************************************
    case IOCTL_VIDEO_VALIDATE_CHILD_STATE_CONFIGURATION:

        VideoDebugPrint((1, "\nW32StartIO - ValidateChildStateConfiguration\n"));
        status = NVValidateChildState(HwDeviceExtension, RequestPacket);
        break;

    //**************************************************************************
    // SET_CHILD_STATE_CONFIGURATION
    //**************************************************************************
    case IOCTL_VIDEO_SET_CHILD_STATE_CONFIGURATION:

        VideoDebugPrint((1, "\nW32StartIO - SetChildStateConfiguration\n"));
        status = NVSetChildState(HwDeviceExtension, RequestPacket);
        break;

#endif _WIN32_WINNT >= 0x0500
#endif ENABLE_WIN2K_HOTKEY_CODE

#if (_WIN32_WINNT >= 0x0500)
    //**************************************************************************
    // SWITCH_DUALVIEW
    //**************************************************************************
    case IOCTL_VIDEO_SWITCH_DUALVIEW:

        VideoDebugPrint((1, "\nW32StartIO - Switch Dualview\n"));
        status = NVSwitchDualView(HwDeviceExtension, *((PULONG)RequestPacket->InputBuffer));
        break;
#endif _WIN32_WINNT >= 0x0500

    //**************************************************************************
    // SHARE_VIDEO_MEMORY
    //**************************************************************************

    case IOCTL_VIDEO_SHARE_VIDEO_MEMORY:


        VideoDebugPrint((2, "W32StartIO - ShareVideoMemory\n"));

        if ( (RequestPacket->OutputBufferLength < sizeof(VIDEO_SHARE_MEMORY_INFORMATION)) ||
             (RequestPacket->InputBufferLength < sizeof(VIDEO_MEMORY)) )
            {
            VideoDebugPrint((0, "IOCTL_VIDEO_SHARE_VIDEO_MEMORY - ERROR_INSUFFICIENT_BUFFER\n"));
            status = ERROR_INSUFFICIENT_BUFFER;
            break;
            }

        pShareMemory = RequestPacket->InputBuffer;

        if ( (pShareMemory->ViewOffset > hwDeviceExtension->AdapterMemorySize) ||
             ((pShareMemory->ViewOffset + pShareMemory->ViewSize) >
                  hwDeviceExtension->AdapterMemorySize) )
            {
            VideoDebugPrint((0, "IOCTL_VIDEO_SHARE_VIDEO_MEMORY - ERROR_INVALID_PARAMETER\n"));
            status = ERROR_INVALID_PARAMETER;
            break;
            }

        RequestPacket->StatusBlock->Information =
                                    sizeof(VIDEO_SHARE_MEMORY_INFORMATION);

        //**********************************************************************
        // Beware: the input buffer and the output buffer are the same
        // buffer, and therefore data should not be copied from one to the
        // other
        //**********************************************************************

        //**********************************************************************
        //
        // NOTE: AdapterMemorySize was adjusted in NVValidateModes
        //       to compensate for INSTANCE memory in offscreen memory.
        //       That is, Hash Table, Fifo Context, and Run Out information
        //       is stored in PRAMIN memory, which also exists in offscreen memory.
        //       We must make sure NO ONE (but the miniport) touches this offscreen memory.
        //
        //       When sharing memory (for Direct X), we must make sure that
        //       we don't give anyone else access to the last 196k or so of offscreen VRAM.
        //       Otherwise, someone else may corrupt our instance memory.
        //**********************************************************************

        virtualAddress = pShareMemory->ProcessHandle;
        sharedViewSize = pShareMemory->ViewSize;

        inIoSpace = 0;

        //**********************************************************************
        // NOTE: we are ignoring ViewOffset
        //**********************************************************************

        shareAddress.HighPart = 0x00000000;
        shareAddress.LowPart = hwDeviceExtension->NVAccessRanges[NV_PHYSICAL_DFB_ADDRESS].RangeStart.LowPart;
        shareAddress.LowPart += RangeOffsets[NV_PDFB_INDEX].ulOffset;

        //**********************************************************************
        // Specifying virtualAddress = 0 will cause this address to be
        // mapped into the address space of the current process
        //**********************************************************************
        inIoSpace |= hwDeviceExtension->flMapMemory;
        status = VideoPortMapMemory(hwDeviceExtension,
                                    shareAddress,
                                    &sharedViewSize,
                                    &inIoSpace,
                                    &virtualAddress);

        pShareMemoryInformation = RequestPacket->OutputBuffer;

        //**********************************************************************
        // Returned shared virtual address to specific process
        //**********************************************************************

        pShareMemoryInformation->SharedViewOffset = pShareMemory->ViewOffset;
        pShareMemoryInformation->VirtualAddress = virtualAddress;
        pShareMemoryInformation->SharedViewSize = sharedViewSize;

        break;

    //**************************************************************************
    // UNSHARE_VIDEO_MEMORY
    //**************************************************************************

    case IOCTL_VIDEO_UNSHARE_VIDEO_MEMORY:

        VideoDebugPrint((2, "W32StartIO - UnshareVideoMemory\n"));

        if (RequestPacket->InputBufferLength < sizeof(VIDEO_SHARE_MEMORY))
            {
            status = ERROR_INSUFFICIENT_BUFFER;
            break;
            }

        pShareMemory = RequestPacket->InputBuffer;

        status = VideoPortUnmapMemory(hwDeviceExtension,
                                      pShareMemory->RequestedVirtualAddress,
                                      pShareMemory->ProcessHandle);

        break;

    //**************************************************************************
    // QUERY_PUBLIC_ACCESS_RANGES
    //**************************************************************************

    case IOCTL_VIDEO_QUERY_PUBLIC_ACCESS_RANGES:


        VideoDebugPrint((1, "NVStartIO - Map W32 MMU or ACL\n"));

        //**********************************************************************
        // Currently, we'll be giving the display driver
        // public access to 5 ranges (in addition to the frame buffer)
        //
        //     1) User Channel
        //     2) Graphics Status register (to check for Engine Busy)
        //     3) Vertical Blank (PFB_CONFIG_0 register)
        //     4) Frame Buffer Start Address
        //     5) DAC Registers
        //
        // The Frame buffer is mapped separately in VIDEO_MAP_VIDEO_MEMORY
        //
        //
        // NV3: We also need access to the following memory ranges for NV3
        //      to access the cursor, vertical,miscellaneous registers
        //      which are different from NV1.
        //
        //     6) PRMCIO
        //     7) PRMVIO
        //     8) PRAMDAC
        //     9) PRAMIN (So we can get to the cursor image data)
        //     10) TEXTURE DATA (in System Memory)
        //     11) PGRAPH Registers
        //     12) DMA Buffer Area (Fixed/Locked contiguous memory)
        //     13) PFIFO Registers
        //
        //**********************************************************************

        if (RequestPacket->OutputBufferLength <
            13 * sizeof(VIDEO_PUBLIC_ACCESS_RANGES))
            {
            status = ERROR_INSUFFICIENT_BUFFER;
            }

        else

            {
            PVIDEO_PUBLIC_ACCESS_RANGES portAccess;
            PVOID                       base;
            ULONG                       ulIndex;

            //******************************************************************
            // Currently, we'll be giving the display driver
            // public access to 5 ranges (in addition to the frame buffer)
            //
            //     1) User Channel
            //     2) Graphics Status register (to check for Engine Busy)
            //     3) Vertical Blank (PFB_CONFIG_0 register)
            //     4) Frame Buffer Start Address
            //     5) DAC Registers
            //
            // The Frame buffer is mapped separately in VIDEO_MAP_VIDEO_MEMORY
            //
            //
            // NV3: We also need access to the following memory ranges for NV3
            //      to access the cursor,vertical,miscellaneous registers
            //      which are different from NV1.
            //
            //     6) PRMCIO
            //     7) PRMVIO
            //     8) PRAMDAC
            //     9) PRAMIN (So we can get to the cursor image data)
            //     10) TEXTURE DATA (in System Memory)
            //     11) PGRAPH Registers
            //
            //******************************************************************

            RequestPacket->StatusBlock->Information =
                12 * sizeof(VIDEO_PUBLIC_ACCESS_RANGES);

            portAccess = RequestPacket->OutputBuffer;

            //******************************************************************
            // Map the NV user channel area (Treat it like Memory Mapped IO)
            //******************************************************************

            portAccess->VirtualAddress  = (PVOID) NULL;
            portAccess->InIoSpace       = FALSE;
            portAccess->MappedInIoSpace = portAccess->InIoSpace;

            //******************************************************************
            // We now use just 1 mapping to access ALL registers, and
            // just 1 mapping to access the framebuffer.  Display Driver,
            // miniport, and Resource Manager now use the same mappings!
            // See NVMapMemoryRanges() and NVMapFrameBuffer() for more info.
            //******************************************************************

            status = NO_ERROR;

            portAccess->VirtualAddress  = (PVOID)(hwDeviceExtension->NvRegisterBase);
            base                         = (PVOID)(hwDeviceExtension->NvRegisterBase);



//Map_PGRAPH_Status:

            //******************************************************************
            // Map the second range for the PGRAPH Status register
            //******************************************************************

            portAccess++;
            portAccess->VirtualAddress = (PUCHAR)(base) +
                RangeOffsets[NV_PGRAPH_STATUS_INDEX].ulOffset;

//Map_PFG_CONFIG_0:

            //******************************************************************
            // Map the third range for the PFB_CONFIG_0 register
            // (so we can check for vertical blanking)
            //******************************************************************

            portAccess++;
            portAccess->VirtualAddress = (PUCHAR)(base) +
                RangeOffsets[NV_PFB_CONFIG_0_INDEX].ulOffset;

//Map_PFB_Start_Address:

            //******************************************************************
            // Map the fourth range for the PFB_START_ADDRESS register
            // (so we can flip the current buffer)
            //******************************************************************

            portAccess++;
            portAccess->VirtualAddress  = (PVOID) NULL;
            portAccess->InIoSpace       = FALSE;
            portAccess->MappedInIoSpace = portAccess->InIoSpace;

//Map_DAC_Regs:

            //******************************************************************
            // Map the fifth range for the DACRegs memory range
            //******************************************************************

            portAccess++;
            portAccess->VirtualAddress  = (PVOID) NULL;
            portAccess->InIoSpace       = FALSE;
            portAccess->MappedInIoSpace = portAccess->InIoSpace;



//Map_PRMCIO_Regs:

            //******************************************************************
            // Map the sixth range for the NV3 PRMCIO memory range
            //******************************************************************

            portAccess++;
            portAccess->VirtualAddress  = (PVOID) NULL;

            //******************************************************************
            // PRMCIO only exists for NV3.
            // Don't map it for NV1.
            // TO DO:  Might want to put this in NV3.C
            //         We want to separate out all NV1/NV3 specific code!!!
            //******************************************************************

            portAccess->VirtualAddress = (PUCHAR)(base) +
                RangeOffsets[NV_PRMCIO_INDEX].ulOffset;


//Map_PRMVIO_Regs:

            //******************************************************************
            // Map the seventh range for the NV3 PRMVIO memory range
            //******************************************************************

            portAccess++;
            portAccess->VirtualAddress = (PVOID) NULL;

            //******************************************************************
            // PRMVIO only exists for NV3.
            // Don't map it for NV1.
            // TO DO:  Might want to put this in NV3.C
            //         We want to separate out all NV1/NV3 specific code!!!
            //******************************************************************

            portAccess->VirtualAddress = (PUCHAR)(base) +
                RangeOffsets[NV_PRMVIO_INDEX].ulOffset;


//Map_PRAMDAC_Regs:

            //******************************************************************
            // Map the eighth range for the NV3 PRAMDAC memory range
            //******************************************************************

            portAccess++;
            portAccess->VirtualAddress = (PVOID) NULL;

            //******************************************************************
            // PRAMDAC only exists for NV3.
            // Don't map it for NV1.
            // TO DO:  Might want to put this in NV3.C
            //         We want to separate out all NV1/NV3 specific code!!!
            //******************************************************************

            portAccess->VirtualAddress = (PUCHAR)(base) +
                RangeOffsets[NV_PRAMDAC_INDEX].ulOffset;


//Map_PRAMIN_Memory:

            //******************************************************************
            // Map the ninth range for the NV3 PRAMIN Instance Memory
            //******************************************************************

            portAccess++;
            portAccess->VirtualAddress  = (PVOID) NULL;
            portAccess->InIoSpace       = FALSE;
            portAccess->MappedInIoSpace = portAccess->InIoSpace;

//Map_Texture_Buffer:

            //******************************************************************
            // Map the Tenth range to contain information about
            // the texture buffer in system memory.
            // We'll use Virtual Address  -> Virtual Address
            //           InIoSpace        -> Physical Address
            //           MappedInIoSpace  -> Length of buffer
            //******************************************************************

            portAccess++;
            portAccess->VirtualAddress  = (PVOID) NULL;
            portAccess->InIoSpace       = FALSE;
            portAccess->MappedInIoSpace = FALSE;



//Map_PGRAPH_Regs:
            portAccess++;
            portAccess->VirtualAddress = (PVOID) NULL;

            //******************************************************************
            // Map the Eleventh range for the NV3 PGRAPH registers
            //******************************************************************

            portAccess->VirtualAddress = (PUCHAR)(base) +
                RangeOffsets[NV_PGRAPH_INDEX].ulOffset;

//Map_PFIFO_Regs:
            portAccess++;
            portAccess->VirtualAddress = (PVOID) NULL;

            //******************************************************************
            // Map the Twelfth range for the NV3 PFIFO registers
            //******************************************************************

            portAccess->VirtualAddress = (PUCHAR)(base) +
                RangeOffsets[NV_PFIFO_INDEX].ulOffset;


//Map_NV3_DMA_Buffer_Area:

            //******************************************************************
            // Map the 13th range to contain information about
            // the DMA buffer in system memory.
            // We'll use Virtual Address  -> Virtual Address
            //           InIoSpace        -> Physical Address
            //           MappedInIoSpace  -> Length of buffer
            //******************************************************************

            portAccess++;
            portAccess->VirtualAddress  = (PVOID) NULL;
            portAccess->InIoSpace       = FALSE;
            portAccess->MappedInIoSpace = FALSE;

            }

        break;

    //**************************************************************************
    // FREE_PUBLIC_ACCESS_RANGES
    //**************************************************************************

    case IOCTL_VIDEO_FREE_PUBLIC_ACCESS_RANGES:


        VideoDebugPrint((2, "NVStartIO - FreePublicAccessRanges\n"));

        {
        PVIDEO_MEMORY mappedMemory;
        ULONG i;

        if (RequestPacket->InputBufferLength < sizeof(VIDEO_MEMORY))
            {
            status = ERROR_INSUFFICIENT_BUFFER;
            break;
            }

        status = NO_ERROR;


        //******************************************************************
        // We now use just 1 mapping to access ALL registers, and
        // just 1 mapping to access the framebuffer.  Display Driver,
        // miniport, and Resource Manager now use the same mappings!
        // See NVMapMemoryRanges() and NVMapFrameBuffer() for more info.
        //
        // It never gets unmapped, and is always available
        //******************************************************************

        }

        break;

    //**************************************************************************
    // MAP_VIDEO_MEMORY
    //**************************************************************************

    case IOCTL_VIDEO_MAP_VIDEO_MEMORY:

        VideoDebugPrint((2, "NVStartIO - MapVideoMemory\n"));

        if ( (RequestPacket->OutputBufferLength <
              (RequestPacket->StatusBlock->Information =
                                     sizeof(VIDEO_MEMORY_INFORMATION))) ||
             (RequestPacket->InputBufferLength < sizeof(VIDEO_MEMORY)) )
            {
            status = ERROR_INSUFFICIENT_BUFFER;
            }

        //*********************************************************************
        // Make sure to set the VideoRamLength!
        //
        // NOTE: AdapterMemorySize was adjusted in NVValidateModes
        //       to compensate for INSTANCE memory in offscreen memory.
        //       That is, Hash Table, Fifo Context, and Run Out information
        //       is stored in PRAMIN memory, which also exists in offscreen memory.
        //       We must make sure NO ONE (but the miniport) touches this offscreen memory.
        //
        //*********************************************************************

        memoryInformation = RequestPacket->OutputBuffer;

        memoryInformation->VideoRamBase = ((PVIDEO_MEMORY)
                (RequestPacket->InputBuffer))->RequestedVirtualAddress;

        //*********************************************************************
        // Get the ptr to the NVInfo structure and initialize the video ram
        // length to be mapped.  DO NOT use hwDeviceExtension->AdapterMemorySize
        // because that value is NOT aligned on a MB boundary (it's adjusted to
        // exclude a portion of video memory).  Using that value would cause
        // the VideoPortMapMemory call to NOT correctly set write combining.
        // Use Framebuffer.RamSize instead !!!
        //*********************************************************************

        NVInfo = &(hwDeviceExtension->NvInfo);

        //**********************************************************************
        // Frame buffer is now mapped earlier, in NVMapFrameBuffer
        // So just return that mapping...Mapping is done in ONE place
        // ONLY, so the miniport, RM, and display driver can access it.
        // (No more multiple mappings of the frame buffer!)
        //**********************************************************************

        memoryInformation->FrameBufferBase = hwDeviceExtension->FrameBufferPtr;
        memoryInformation->FrameBufferLength = NVInfo->Framebuffer.RamSize;

        status = NO_ERROR;


        break;

    //**************************************************************************
    // UNMAP_VIDEO_MEMORY
    //**************************************************************************

    case IOCTL_VIDEO_UNMAP_VIDEO_MEMORY:

        VideoDebugPrint((2, "NVStartIO - UnMapVideoMemory\n"));

        if (RequestPacket->InputBufferLength < sizeof(VIDEO_MEMORY))
            {
            status = ERROR_INSUFFICIENT_BUFFER;
            }

        //**********************************************************************
        // Frame buffer is now mapped earlier, in NVMapFrameBuffer
        // It's mapped one time, and doesn't get unmapped
        //**********************************************************************

        status = NO_ERROR;

        break;

    //**************************************************************************
    // QUERY_AVAIL_MODES
    //**************************************************************************

    case IOCTL_VIDEO_QUERY_AVAIL_MODES:

        VideoDebugPrint((2, "NVStartIO - QueryAvailableModes\n"));

        status = NVQueryAvailableModes(hwDeviceExtension,
                                        (PVIDEO_MODE_INFORMATION)
                                            RequestPacket->OutputBuffer,
                                        RequestPacket->OutputBufferLength,
                                        &RequestPacket->StatusBlock->Information);

        break;

    //**************************************************************************
    // QUERY_NUM_AVAIL_MODES
    //**************************************************************************

    case IOCTL_VIDEO_QUERY_NUM_AVAIL_MODES:

        VideoDebugPrint((2, "NVStartIO - QueryNumAvailableModes\n"));

        status = NVQueryNumberOfAvailableModes(hwDeviceExtension,
                                                (PVIDEO_NUM_MODES)
                                                    RequestPacket->OutputBuffer,
                                                RequestPacket->OutputBufferLength,
                                                &RequestPacket->StatusBlock->Information);

        break;

    //**************************************************************************
    // QUERY_CURRENT_MODE
    //**************************************************************************

    case IOCTL_VIDEO_QUERY_CURRENT_MODE:

        VideoDebugPrint((2, "NVStartIO - QueryCurrentMode\n"));

        status = NVQueryCurrentMode(hwDeviceExtension,
                                     (PVIDEO_MODE_INFORMATION) RequestPacket->OutputBuffer,
                                     RequestPacket->OutputBufferLength,
                                     &RequestPacket->StatusBlock->Information);

        break;

    //**************************************************************************
    // GET TIMING OF DAC at giving resolution
    //
    //**************************************************************************
    case IOCTL_VIDEO_GET_TIMING_DAC:
    {
        HEAD_RESOLUTION_INFO * pResolution;
        DAC_TIMING_VALUES * pTimingInfo;

        status = ERROR_INSUFFICIENT_BUFFER;
        if(RequestPacket->InputBufferLength == sizeof(HEAD_RESOLUTION_INFO))
        {
            pResolution = (HEAD_RESOLUTION_INFO *) RequestPacket->InputBuffer;
            if(RequestPacket->OutputBufferLength == sizeof(DAC_TIMING_VALUES))
            {
                pTimingInfo = (DAC_TIMING_VALUES *) RequestPacket->OutputBuffer;
                if(NVGetTimingForDac(hwDeviceExtension, pResolution, pTimingInfo))
                {
                    // Patch for Timing values.  Our engine only accepts values aligned to 8
                    pTimingInfo->HorizontalTotal &= 0xFFFFFFFC;
                    pTimingInfo->HorizontalBlankStart &= 0xFFFFFFFC;
                    pTimingInfo->HorizontalRetraceStart &= 0xFFFFFFFC;
                    pTimingInfo->HorizontalRetraceEnd &= 0xFFFFFFFC;
                    pTimingInfo->HorizontalBlankEnd &= 0xFFFFFFFC;

                    RequestPacket->StatusBlock->Information = sizeof(DAC_TIMING_VALUES);
                    status = NO_ERROR;
                }
            }
        }
    }

    break;

    //**************************************************************************
    // Call RM before set mdoe
    //
    //**************************************************************************
    case IOCTL_VIDEO_PREMODE_SET:
    {
        ULONG * pulHead;

        status = ERROR_INSUFFICIENT_BUFFER;
        if(RequestPacket->InputBufferLength == sizeof(ULONG))
        {
            pulHead = (ULONG *) RequestPacket->InputBuffer;
            if(RmPreModeSet(hwDeviceExtension->DeviceReference, *pulHead))
            {
                    status = NO_ERROR;
            }
        }


    }
    break;

    //**************************************************************************
    // Call RM after set mdoe
    //
    //**************************************************************************
    case IOCTL_VIDEO_POSTMODE_SET:
    {
        ULONG * pulHead;

        status = ERROR_INSUFFICIENT_BUFFER;
        if(RequestPacket->InputBufferLength == sizeof(ULONG))
        {
            pulHead = (ULONG *) RequestPacket->InputBuffer;
            if(RmPostModeSet(hwDeviceExtension->DeviceReference, *pulHead))
            {
                    status = NO_ERROR;

            }

        }


    }
    break;

    //**************************************************************************
    // Setup Virtual Desktop information
    //**************************************************************************
    case IOCTL_VIDEO_SET_DESKTOP_INFO_MODE:
        {
            NV_DESKTOP_INFO * pDeskTop;
            ULONG i;

            if(RequestPacket->InputBufferLength == sizeof(NV_DESKTOP_INFO))
            {
                pDeskTop = (NV_DESKTOP_INFO *) RequestPacket->InputBuffer;
                hwDeviceExtension->DeskTopInfo.ulDesktopModeIndex = pDeskTop->ulDesktopModeIndex;

                hwDeviceExtension->DeskTopInfo.ulDesktopMode = pDeskTop->ulDesktopMode;
                hwDeviceExtension->DeskTopInfo.ulTwinView_State = pDeskTop->ulTwinView_State;
                hwDeviceExtension->DeskTopInfo.ulNumberDacsOnBoard = pDeskTop->ulNumberDacsOnBoard;
                hwDeviceExtension->DeskTopInfo.ulNumberDacsConnected = pDeskTop->ulNumberDacsConnected;
                hwDeviceExtension->DeskTopInfo.ulNumberDacsActive = pDeskTop->ulNumberDacsActive;

                hwDeviceExtension->DeskTopInfo.ulDesktopWidth = pDeskTop->ulDesktopWidth;
                hwDeviceExtension->DeskTopInfo.ulDesktopHeight = pDeskTop->ulDesktopHeight;

                hwDeviceExtension->DeskTopInfo.ulAllDeviceMask = pDeskTop->ulAllDeviceMask;
                hwDeviceExtension->DeskTopInfo.ulConnectedDeviceMask = pDeskTop->ulConnectedDeviceMask;

                for(i = 0; i < NV_NO_DACS; i++)
                {
                    hwDeviceExtension->DeskTopInfo.ulDisplayWidth[i] = pDeskTop->ulDisplayWidth[i];
                    hwDeviceExtension->DeskTopInfo.ulDisplayHeight[i] = pDeskTop->ulDisplayHeight[i];
                    hwDeviceExtension->DeskTopInfo.ulDisplayRefresh[i] = pDeskTop->ulDisplayRefresh[i];
                    hwDeviceExtension->DeskTopInfo.ulDisplayPixelDepth[i] = pDeskTop->ulDisplayPixelDepth[i];
                    hwDeviceExtension->DeskTopInfo.ulDeviceDisplay[i] = pDeskTop->ulDeviceDisplay[i];
                    hwDeviceExtension->DeskTopInfo.ulDeviceMask[i] = pDeskTop->ulDeviceMask[i];
                    hwDeviceExtension->DeskTopInfo.ulDeviceType[i] = pDeskTop->ulDeviceType[i];
                    hwDeviceExtension->DeskTopInfo.ulTVFormat[i] = pDeskTop->ulTVFormat[i];
                }

                // TODO ...  
                // Display driver already checks this; Miniport driver should double check ??
                // Should call RM to check dual head support
                if(1)
                    *((BOOLEAN *) RequestPacket->OutputBuffer) =  TRUE ;
                else
                    *((BOOLEAN *) RequestPacket->OutputBuffer) =  FALSE ;

                RequestPacket->StatusBlock->Information = sizeof(BOOLEAN);
                status = NO_ERROR;
                break;
            }
            else
            {
                VideoDebugPrint((0, "IOCTL_VIDEO_SET_DESKTOP_DISPLAY_MODE - ERROR_INSUFFICIENT_BUFFER\n"));
                status = ERROR_INSUFFICIENT_BUFFER;
                break;
            }
        }

        break;

    
    case IOCTL_VIDEO_CLEAR_FRAMEBUFFER:
        {
        ULONG i;
        if (hwDeviceExtension->FrameBufferPtr !=NULL)
            {
            //**********************************************************************
            // Clear the framebuffer prior to setting the video mode.
            // This is for aesthetics only.
            // Back up 1Mb to make absolutely sure we don't walk over any instance memory
            //**********************************************************************
            for (i = 0; i < ( (hwDeviceExtension->AdapterMemorySize-0x100000) >> 2); i++)
                {
                ((PULONG) hwDeviceExtension->FrameBufferPtr)[i] = 0;
                }
            }
        }
        status = NO_ERROR;
        break;

    //**************************************************************************
    // SET_CURRENT_MODE
    //**************************************************************************

    case IOCTL_VIDEO_SET_CURRENT_MODE:

        VideoDebugPrint((2, "NVStartIO - SetCurrentModes\n"));

        // Call the RM to enable hot key functionality
        RmSetDriverHotkeyHandling(hwDeviceExtension->DeviceReference, TRUE);

        // Also clear the hotkeyPending flag since a modeset will be done.
        hwDeviceExtension->ulHotKeyPending = 0;

#if 1
   {   //check for last saved flat panel mode and set it if necessary
        ULONG  regmode  = -1;
        BOOL   fpstatus = 0;
        BOOL   Status;
        ULONG  head;
        ULONG  i;
        NV_CFGEX_GET_FLATPANEL_INFO_MULTI_PARAMS fpGetParams; 
        NV_CFGEX_SET_FLATPANEL_INFO_MULTI_PARAMS fpSetParams;
         
        Status = VideoPortGetRegistryParameters(hwDeviceExtension,
                                           L"FlatPanelMode",
                                           FALSE,
                                           NVRegistryCallback,
                                           &regmode);
                                          
        if (Status == NO_ERROR && regmode <= 2)
        {
            for (i=0;i<hwDeviceExtension->DeskTopInfo.ulNumberDacsActive; i++)
            {
                head = hwDeviceExtension->DeskTopInfo.ulDeviceDisplay[i];
                if(MaskToDeviceType(hwDeviceExtension->DeskTopInfo.ulDeviceMask[head]) ==
                                    MONITOR_TYPE_FLAT_PANEL)
                {
                    fpGetParams.Head = head;
                    fpstatus = RmConfigGetExKernel(hwDeviceExtension->DeviceReference,
                                        NV_CFGEX_GET_FLAT_PANEL_INFO_MULTI,
                                        &(fpGetParams), 
                                        sizeof(NV_CFGEX_GET_FLATPANEL_INFO_MULTI_PARAMS));
    
                    if( fpstatus == TRUE && 
                        regmode  != fpGetParams.FlatPanelMode && 
                        fpGetParams.FlatPanelMode  <= 2 )
                    {
                        fpSetParams.CommitChanges = 0; //already committed
                        fpSetParams.FlatPanelMode = regmode;
                        fpSetParams.Head = head;
                        RmConfigSetExKernel(hwDeviceExtension->DeviceReference,
                                            NV_CFGEX_SET_FLAT_PANEL_SCALING,
                                            &fpSetParams, 
                                            sizeof(NV_CFGEX_SET_FLATPANEL_INFO_MULTI_PARAMS));
                    } //if modes arent the same
                }//if flat panel
            }//for each active dac
        }//if registry returns no error
        else
        {
            //else the registry didn't tell us what to do, so default to adapter scaling. NEC hi-priority request.
            for (i=0;i<hwDeviceExtension->DeskTopInfo.ulNumberDacsActive; i++)
            {
                head = hwDeviceExtension->DeskTopInfo.ulDeviceDisplay[i];
                if(MaskToDeviceType(hwDeviceExtension->DeskTopInfo.ulDeviceMask[head]) ==
                                    MONITOR_TYPE_FLAT_PANEL)
                {
                    {
                        fpSetParams.CommitChanges = 1; //not committed yet
                        regmode = NV_CFGEX_GET_FLATPANEL_INFO_SCALED;
                        fpSetParams.FlatPanelMode = regmode;
                        fpSetParams.Head = head;
                        RmConfigSetExKernel(hwDeviceExtension->DeviceReference,
                                            NV_CFGEX_SET_FLAT_PANEL_SCALING,
                                            &fpSetParams, 
                                            sizeof(NV_CFGEX_SET_FLATPANEL_INFO_MULTI_PARAMS));
                    } //if modes arent the same
                }//if flat panel
            }//for each active dac     
        }
    } //end of checking for last saved flat panel mode and setting.
#endif    

        status = NVSetMode(hwDeviceExtension,
                              (PVIDEO_MODE) RequestPacket->InputBuffer,
                              RequestPacket->InputBufferLength);

        //******************************************************************************
        // Did caller ask if we supported hw video overlay?
        //******************************************************************************
        if (RequestPacket->OutputBufferLength  == sizeof(BOOLEAN))
            {
            //******************************************************************************
            // return whether or hw video overlay is supported in the current mode.
            //******************************************************************************
            *((BOOLEAN *) RequestPacket->OutputBuffer) = hwDeviceExtension->bHwVidOvl;
            RequestPacket->StatusBlock->Information = sizeof(BOOLEAN);
            }


// removed hard coded status....

        //
        // Refresh gamma clut after modeset
        //
        // Update the gamma concept if we are not on an NV3
        if( !(hwDeviceExtension->DeskTopInfo.ulDesktopMode & NV_ENABLE_CLASS_SET_MODE))
        {

            VideoDebugPrint((2, "IOCTL_VIDEO_SET_CURRENT_MODE - Updating gamma registers\n"));

                // We need to make sure that we wait for the vblank interrupt handler to run at least once before
                // we update the gamma values, as the handler will also write out the values in the dac.gamma table
                // and clobber anything that we have written.  This is very backdoor, and should be handled in a more
                // proper way, say by letting the vblank code update these for us by letting it know what we want.


            if(hwDeviceExtension->ulChipID != NV3_DEVICE_NV3_ID)
            {
               U032   StartVBlank,CurrentVBlank;

               // Do this: Get some vblank count and wait for it to change

               // Get the first relative start value
               if(RmConfigGetKernel(hwDeviceExtension->DeviceReference, NV_CFG_VBLANK_COUNTER, &CurrentVBlank)) {

                  StartVBlank = CurrentVBlank;
                  while(CurrentVBlank == StartVBlank) {    // Wait till they are different
                        if(!RmConfigGetKernel(hwDeviceExtension->DeviceReference, NV_CFG_VBLANK_COUNTER, &CurrentVBlank)) {
                           CurrentVBlank = StartVBlank + 1;   // This is how we break out of here if the RM fails
                        }
                  }
               }

                NV_SetPixMixBits(hwDeviceExtension);

                NVSetColorLookup(hwDeviceExtension,
                            (PVIDEO_CLUT) &hwDeviceExtension->GammaCache,
                            MAX_CLUT_SIZE);
            }
        }

        //**************************************************************************
        //
        // We need to let the control panel know that the modeset has occurred.
        // For example, in TwinView configuration, NT4.0/Win2K sometimes ignores a 
        // modeset request for the same mode even though we could be changing the mode
        // from normal to clone, but with the same resolution.
        // nVidia control sets this registry entry to 0, calls Win2K to do a modeset
        // and then tests this registry entry to see if the display driver actually
        // was called or not.
        //
        //**************************************************************************

        {
          ULONG NvCplModeSetHasOccurred;      // Used only by the control panel.
          
          NvCplModeSetHasOccurred = 1;
          hwDeviceExtension->SystemModeSetHasOccurred = 1;

          VideoPortSetRegistryParameters(hwDeviceExtension,
                                      L"NvCplModeSetHasOccurred",
                                      &NvCplModeSetHasOccurred,
                                      sizeof(ULONG));
          VideoPortSetRegistryParameters(hwDeviceExtension,
                                      L"SystemModeSetHasOccurred",
                                      &hwDeviceExtension->SystemModeSetHasOccurred,
                                      sizeof(ULONG));
        }
        break;

    //**************************************************************************
    // Store the unique integer ID for the ppdev for the ensuing IOCTL_RESET_DEVICE.
    //**************************************************************************
    case IOCTL_VIDEO_SET_UNIQUENESS_VGA:
        {
            ULONG ulUniqueness;
            status = NO_ERROR;

            if(RequestPacket->InputBufferLength < sizeof(ULONG))
            {
                VideoDebugPrint((0, "IOCTL_VIDEO_SET_UNIQUENESS_VGA - ERROR_INSUFFICIENT_BUFFER\n"));
                status = ERROR_INSUFFICIENT_BUFFER;
                break;
            }
            ulUniqueness = *(ULONG *)RequestPacket->InputBuffer;
            hwDeviceExtension->ulUniquenessVGA = ulUniqueness;
            VideoDebugPrint((1, "UniquenessVGA stored: 0x%x\n",ulUniqueness));
        }
        break;

    //**************************************************************************
    // Store Status of the NVSVC service.
    //**************************************************************************
    case IOCTL_VIDEO_SET_NVSVC_STATUS:
        {
            ULONG ulNVSVCStatus;
            status = NO_ERROR;

            if(RequestPacket->InputBufferLength < sizeof(ULONG))
            {
                VideoDebugPrint((0, "IOCTL_VIDEO_SET_NVSVC_STATUS - ERROR_INSUFFICIENT_BUFFER\n"));
                //
                // Set the status to stopped to be safe.
                //
                hwDeviceExtension->ulNVSVCStatus = NVSVC_STATUS_STOPPED;
                status = ERROR_INSUFFICIENT_BUFFER;
                break;
            }
            ulNVSVCStatus = *(ULONG *)RequestPacket->InputBuffer;
            hwDeviceExtension->ulNVSVCStatus = ulNVSVCStatus;
            VideoDebugPrint((1, "ulNVSVCStatus stored: 0x%x\n",ulNVSVCStatus));
        }
        break;

    //**************************************************************************
    // Store Status of the NVSVC service.
    //**************************************************************************
    case IOCTL_VIDEO_SET_NVSVC_MODESET_DEVMASK:
        {
            ULONG ulInduceModeChangeDeviceMask;
            status = NO_ERROR;

            if(RequestPacket->InputBufferLength < sizeof(ULONG))
            {
                VideoDebugPrint((0, "IOCTL_VIDEO_SET_NVSVC_MODESET_DEVMASK - ERROR_INSUFFICIENT_BUFFER\n"));
                hwDeviceExtension->ulInduceModeChangeDeviceMask = 0;
                status = ERROR_INSUFFICIENT_BUFFER;
                break;
            }
            ulInduceModeChangeDeviceMask = *(ULONG *)RequestPacket->InputBuffer;
            hwDeviceExtension->ulInduceModeChangeDeviceMask = ulInduceModeChangeDeviceMask;
            VideoDebugPrint((1, "ulInduceModeChangeDeviceMask stored: 0x%x\n",ulInduceModeChangeDeviceMask));
        }
        break;

    //**************************************************************************
    // Signal the NVModesetEvent to invoke the NVSVC
    //**************************************************************************
    case IOCTL_VIDEO_SIGNAL_NVSVC_MODESET_EVENT:
        {
            VideoDebugPrint((1, "NVStartIO - SignalNVSVCModeSetEvent\n"));
            status = NO_ERROR;
            SignalModeSwitchEvent();
        }
        break;

    //*****************************************************************************
    // Signal the control panel thread to indicate a hotkey switch has taken place.
    //*****************************************************************************
    case IOCTL_VIDEO_SIGNAL_NVCPL_HOTKEY_EVENT:
        {
            VideoDebugPrint((1, "NVStartIO - SignalNVCplHotKeyEvent\n"));
            status = NO_ERROR;
            SignalNVCplHotKeyEvent();
        }
        break;


    //**************************************************************************
    // RESET_DEVICE
    //**************************************************************************

    case IOCTL_VIDEO_RESET_DEVICE:

        // Clean up virtual desktop flag.
        hwDeviceExtension->DeskTopInfo.ulDesktopMode = 0;

        VideoDebugPrint((2, "NVStartIO - Reset Device\n"));

        status = NO_ERROR;
        
#if (_WIN32_WINNT >= 0x0500)
        hwDeviceExtension->ulDualViewStatus &= ~DUALVIEW_STATUS_GRAPHICS_MODE;

        //
        // For DualView(MHS), we need to check whether the other view is in graphics mode.
        //
        if (hwDeviceExtension->pDualViewAssociatedDE)
        {
            //
            // If another display still in graphics mode, do nothing.
            //
            if (hwDeviceExtension->pDualViewAssociatedDE->ulDualViewStatus & DUALVIEW_STATUS_GRAPHICS_MODE)
            {
                break;
            }
        }
#endif

        // Call the RM to disable hot key functionality
        RmSetDriverHotkeyHandling(hwDeviceExtension->DeviceReference, FALSE);

        if(hwDeviceExtension->ulRmTurnedOffMonitor)
        {
            // PowerManagement turn off the monitor 
            // It is not necessary to call BIOS to reset the chip
            break;
        }

        //**********************************************************************
        // Because we're going to do a reset, we need to write current value
        // to registry.  Temporary fix, used to keep mobile scaling hotkey in
        // some sort of sync with registry.
        //**********************************************************************

        WriteFPModeToRegistry (hwDeviceExtension);
        
        //**********************************************************************
        // Must *NOT* call Int10 in a multiple adapter configuration
        //**********************************************************************

        if (MultiAdaptersPresent == FALSE)
            {
            //******************************************************************
            // Make sure that NV interrupts are disabled before we call Int10!!!
            // Otherwise, there may be conflicts writing to the hardware registers
            // (especially in an SMP system)
            //******************************************************************

            NV_DisableNVInterrupts(hwDeviceExtension);


#if _WIN32_WINNT < 0x0500

            //******************************************************************
            // For NT4, always use the bios to do the modeset.
            // SMP issue that used to reqire setting the text mode by hand
            // seems to be gone now.
            //******************************************************************

                status = RMVideoResetDevice(hwDeviceExtension->DeviceReference);
                if (status != NO_ERROR)
                    VideoDebugPrint((0, "NV: IOCTL_VIDEO_RESET_DEVICE FAILED\n"));

#else
            //******************************************************************
            // For Win2k, always use Int10
            //******************************************************************

            if(hwDeviceExtension->curAdapterPowerState == VideoPowerOn ||
                hwDeviceExtension->curAdapterPowerState == VideoPowerHibernate)
            {
                //FPMode needs to be saved/restored because of bios bug in nv11
                status = RMVideoResetDevice(hwDeviceExtension->DeviceReference);
                if (status != NO_ERROR)
                    VideoDebugPrint((0, "NV: IOCTL_VIDEO_RESET_DEVICE FAILED\n"));
            }
#endif

            //******************************************************************
            // Turn NV interrupts back on
            //******************************************************************

            NV_ReEnableNVInterrupts(hwDeviceExtension);
            }


        break;

    //**************************************************************************
    // SET_COLOR_REGISTERS
    //**************************************************************************

    case IOCTL_VIDEO_SET_COLOR_REGISTERS:

        VideoDebugPrint((2, "NVStartIO - SetColorRegs\n"));

        status = NVSetColorLookup(hwDeviceExtension,
                                   (PVIDEO_CLUT) RequestPacket->InputBuffer,
                                   RequestPacket->InputBufferLength);


// Initialize this adapters CLUT to the requested gamma ramp
   {

      PVIDEO_CLUTDATA   pvcdItemSrc,pvcdItemDst;
      PVIDEO_CLUT       srcclut;
      ULONG index,item,NumEntries,FirstEntry;

      VideoDebugPrint((1, "IOCTL_VIDEO_SET_COLOR_REGISTERS - Saving gamma mods in gammacache\n"));

      // - Make things nice to look at
          pvcdItemSrc = &(((PVIDEO_CLUT)RequestPacket->InputBuffer)->LookupTable->RgbArray);
          pvcdItemDst = &hwDeviceExtension->GammaCache.LookupTable->RgbArray;
      srcclut = (PVIDEO_CLUT)RequestPacket->InputBuffer;

      // - Set the limits on the item count
      NumEntries = srcclut->NumEntries;
      FirstEntry = srcclut->FirstEntry;

      // - Copy over the gamma data
      for(item = FirstEntry; item < NumEntries; item++) {
           index = item + FirstEntry;
           pvcdItemDst[index].Blue   = pvcdItemSrc[index].Blue;
           pvcdItemDst[index].Green  = pvcdItemSrc[index].Green;
           pvcdItemDst[index].Red    = pvcdItemSrc[index].Red;
           pvcdItemDst[index].Unused = pvcdItemSrc[index].Unused;
      }

    }

        break;

    //**************************************************************************
    // Enable the PIXMIX bits in NV_PRAMDAC_GENERAL_CONTROL
    //**************************************************************************
    case IOCTL_VIDEO_ENABLE_PIXMIX_INDEX:
            //
            //      Set PIXMIX bits to 11b
            //
            NV_SetPixMixBits(hwDeviceExtension);

            status = NO_ERROR;
            break;

    //**************************************************************************
    // ENABLE_VDM
    // Do we need to implement this call ??
    // We'll need to implement ResetDevice because we'll be using VgaCompatible=0
    //**************************************************************************

    case IOCTL_VIDEO_ENABLE_VDM:

        VideoDebugPrint((2, "NVStartIO - EnableVDM\n"));

        hwDeviceExtension->TrappedValidatorCount = 0;
        hwDeviceExtension->SequencerAddressValue = 0;

        hwDeviceExtension->CurrentNumVdmAccessRanges =
            NUM_MINIMAL_VGA_VALIDATOR_ACCESS_RANGE;
        hwDeviceExtension->CurrentVdmAccessRange =
            MinimalVgaValidatorAccessRange;

        VideoPortSetTrappedEmulatorPorts(hwDeviceExtension,
                                         hwDeviceExtension->CurrentNumVdmAccessRanges,
                                         hwDeviceExtension->CurrentVdmAccessRange);

        status = NO_ERROR;

        break;

    //**************************************************************************
    // Private IOCTLs established with the driver
    //**************************************************************************


    //**************************************************************************
    // GET_VIDEO_CARD_INFO
    // This call is unnecessary , we don't do banking
    //**************************************************************************

    case IOCTL_VIDEO_GET_VIDEO_CARD_INFO:

        VideoDebugPrint((2, "NVStartIO - Get video card info\n"));

        if (RequestPacket->OutputBufferLength <
            (RequestPacket->StatusBlock->Information =
            sizeof(VIDEO_COPROCESSOR_INFORMATION)) )
            {
            status = ERROR_INSUFFICIENT_BUFFER;
            break;
            }

        // return the Coproc Base Address.

        ((PVIDEO_COPROCESSOR_INFORMATION) RequestPacket->OutputBuffer)->ulChipID =
            hwDeviceExtension->ulChipID;

        ((PVIDEO_COPROCESSOR_INFORMATION) RequestPacket->OutputBuffer)->ulRevLevel =
            hwDeviceExtension->ulRevLevel;

        ((PVIDEO_COPROCESSOR_INFORMATION) RequestPacket->OutputBuffer)->ulVideoMemory =
            hwDeviceExtension->AdapterMemorySize;

        status = NO_ERROR;

        break;


    case IOCTL_VIDEO_MAP_IO_PORT_FOR_ALI:

        VideoDebugPrint((2, "NVStartIO - Map IO port for ALI\n"));

        if ( (RequestPacket->OutputBufferLength <
              (RequestPacket->StatusBlock->Information =
                                     sizeof(VIDEO_MEMORY_INFORMATION))) ||
             (RequestPacket->InputBufferLength < sizeof(VIDEO_MEMORY)) )
            {
            status = ERROR_INSUFFICIENT_BUFFER;
            }
        RequestPacket->StatusBlock->Information = sizeof(VIDEO_MEMORY_INFORMATION);


        memoryInformation = RequestPacket->OutputBuffer;
        memoryInformation->FrameBufferBase   = 0;
        memoryInformation->FrameBufferLength = FALSE;
        hwDeviceExtension->AliFixupIOBase    = NULL;

        //**********************************************************************
        // This code is ONLY necessary for the ALI 1541 chipset
        //**********************************************************************

        if (hwDeviceExtension->AliChipset_Problem_Found == TRUE)
            {
            //******************************************************************
            // Setup to map an IO register
            //******************************************************************

            AliFixupBaseAddress.HighPart = 0x00000000;
            AliFixupBaseAddress.LowPart  = 0x000003d0;
            IOLength            = 4;
            inIoSpace           = TRUE;
            AliVirtualAddress   = NULL;

            //******************************************************************
            // Map the IO port, and pass it back to the display driver
            //******************************************************************

            status = VideoPortMapMemory(hwDeviceExtension,
                                     AliFixupBaseAddress,
                                     &(IOLength),
                                     &(inIoSpace),
                                     &AliVirtualAddress);

            memoryInformation->FrameBufferBase   = AliVirtualAddress;
            memoryInformation->FrameBufferLength = TRUE;
            hwDeviceExtension->AliFixupIOBase    = AliVirtualAddress;
            }



        status = NO_ERROR;

        break;

    case IOCTL_VIDEO_UNMAP_IO_PORT_FOR_ALI:

        VideoDebugPrint((2, "NVStartIO - Free mapping for IO port \n"));

        //**********************************************************************
        // This code is ONLY necessary for the ALI 1541 chipset
        //**********************************************************************

        if (hwDeviceExtension->AliChipset_Problem_Found == TRUE)
            {

            //******************************************************************
            // Free up the mapping for the IO port
            //******************************************************************

            if (hwDeviceExtension->AliFixupIOBase !=NULL)
                {
                status = VideoPortUnmapMemory(hwDeviceExtension,
                                         hwDeviceExtension->AliFixupIOBase,
                                         0);
                }

            hwDeviceExtension->AliFixupIOBase = NULL;
            }


        status = NO_ERROR;

        break;

    case IOCTL_VIDEO_MAP_NV_PFIFO_USER:
        {
        PHYSICAL_ADDRESS            base, pa;
        ULONG                       length;

        VideoDebugPrint((2, "NVStartIO - Map PFIFO User\n"));

        if (RequestPacket->OutputBufferLength < sizeof(ULONG) )
            {
            status = ERROR_INSUFFICIENT_BUFFER;
            break;
            }

        //**********************************************************************
        // Return mapping to PFIFO registers in user address space
        //**********************************************************************

        virtualAddress = (PVOID)0L;

        //******************************************************************
        // Get physical base address again (since we modified it up above
        // for the PFIFO registers
        //******************************************************************

        base = hwDeviceExtension->NVAccessRanges[NV_PHYSICAL_BASE_ADDRESS].RangeStart;

        //**************************************************************
        // Call VideoPort to map the range for us
        //**************************************************************

        pa          = base;
        pa.HighPart = 0x00000000;
        pa.LowPart  += RangeOffsets[NV_PFIFO_INDEX].ulOffset;
        sharedViewSize = RangeOffsets[NV_PFIFO_INDEX].ulLength;

        NVMapPhysIntoUserSpace(pa, sharedViewSize, &virtualAddress);
        *(PVOID *)RequestPacket->OutputBuffer = virtualAddress;
        RequestPacket->StatusBlock->Information = sizeof(PVOID);

        status = NO_ERROR;

        }
        break;

    case IOCTL_VIDEO_UNMAP_NV_PFIFO_USER:
        {
        NV_SYSMEM_SHARE_MEMORY *pSysmemShareMemoryIn = NULL;
        NV_SYSMEM_SHARE_MEMORY *pSysmemShareMemoryOut = NULL;
        PHYSICAL_ADDRESS        physAddr;

        VideoDebugPrint((2, "NVStartIO - UnMap PFIFO User\n"));

        if ( (RequestPacket->OutputBufferLength < sizeof(NV_SYSMEM_SHARE_MEMORY)) ||
             (RequestPacket->InputBufferLength < sizeof(NV_SYSMEM_SHARE_MEMORY)) )
            {
            VideoDebugPrint((0, "IOCTL_VIDEO_UNMAP_TO_USER - ERROR_INSUFFICIENT_BUFFER\n"));
            status = ERROR_INSUFFICIENT_BUFFER;
            break;
            }

        pSysmemShareMemoryIn = RequestPacket->InputBuffer;
        RequestPacket->StatusBlock->Information = sizeof(NV_SYSMEM_SHARE_MEMORY);
        // pSysmemShareMemoryIn->pSystemMdl points to previously allocated MDL
        NvUnmapPhysFromUserSpace(pSysmemShareMemoryIn);
        pSysmemShareMemoryOut = RequestPacket->OutputBuffer;
        status = NO_ERROR;
        }
        break;

    case IOCTL_VIDEO_MAP_NV_PBUS_USER:
        {
        PHYSICAL_ADDRESS            base, pa;
        ULONG                       length;

        VideoDebugPrint((2, "NVStartIO - Map PBUS User\n"));

        if (RequestPacket->OutputBufferLength < sizeof(ULONG) )
            {
            status = ERROR_INSUFFICIENT_BUFFER;
            break;
            }

        //**********************************************************************
        // Return mapping to PBUS registers in user address space
        //**********************************************************************

        virtualAddress = (PVOID)0L;

        //******************************************************************
        // Get physical base address again (since we modified it up above
        // for the PBUS registers
        //******************************************************************

        base = hwDeviceExtension->NVAccessRanges[NV_PHYSICAL_BASE_ADDRESS].RangeStart;

        //**************************************************************
        // Call VideoPort to map the range for us
        //**************************************************************

        pa          = base;
        pa.HighPart = 0x00000000;
        pa.LowPart  += RangeOffsets[NV_PBUS_INDEX].ulOffset;
        sharedViewSize = RangeOffsets[NV_PBUS_INDEX].ulLength;

        NVMapPhysIntoUserSpace(pa, sharedViewSize, &virtualAddress);
        *(PVOID *)RequestPacket->OutputBuffer = virtualAddress;
        RequestPacket->StatusBlock->Information = sizeof(PVOID);

        status = NO_ERROR;

        }
        break;

    case IOCTL_VIDEO_UNMAP_NV_PBUS_USER:
        {
        NV_SYSMEM_SHARE_MEMORY *pSysmemShareMemoryIn = NULL;
        NV_SYSMEM_SHARE_MEMORY *pSysmemShareMemoryOut = NULL;
        PHYSICAL_ADDRESS        physAddr;

        VideoDebugPrint((2, "NVStartIO - UnMap PBUS User\n"));

        if ( (RequestPacket->OutputBufferLength < sizeof(NV_SYSMEM_SHARE_MEMORY)) ||
             (RequestPacket->InputBufferLength < sizeof(NV_SYSMEM_SHARE_MEMORY)) )
            {
            VideoDebugPrint((0, "IOCTL_VIDEO_UNMAP_TO_USER - ERROR_INSUFFICIENT_BUFFER\n"));
            status = ERROR_INSUFFICIENT_BUFFER;
            break;
            }

        pSysmemShareMemoryIn = RequestPacket->InputBuffer;
        RequestPacket->StatusBlock->Information = sizeof(NV_SYSMEM_SHARE_MEMORY);
        // pSysmemShareMemoryIn->pSystemMdl points to previously allocated MDL
        NvUnmapPhysFromUserSpace(pSysmemShareMemoryIn);
        pSysmemShareMemoryOut = RequestPacket->OutputBuffer;
        status = NO_ERROR;
        }
        break;

    case IOCTL_VIDEO_ENABLE_WRITE_COMBINING:

        VideoDebugPrint((2, "NVStartIO - VIDEO_ENABLE_WRITE_COMBINING\n"));

        //**********************************************************************
        // Set flags to enable fb write combining.
        //**********************************************************************
        hwDeviceExtension->flMapMemory |= VIDEO_MEMORY_SPACE_P6CACHE;
        status = NO_ERROR;

        break;

    case IOCTL_VIDEO_GET_TV_CURSOR_ADJUST_INFO:

        VideoDebugPrint((2, "NVStartIO - Return TV cursor adjustment values\n"));

        if (RequestPacket->OutputBufferLength < sizeof(TV_CURSOR_ADJUST_INFO) )
            {
            status = ERROR_INSUFFICIENT_BUFFER;
            break;
            }

        //**********************************************************************
        // Return information about our DMA buffer
        //**********************************************************************

        RequestPacket->StatusBlock->Information = sizeof(TV_CURSOR_ADJUST_INFO);

        ((TV_CURSOR_ADJUST_INFO *)(RequestPacket->OutputBuffer))->MonitorType =
                                  hwDeviceExtension->NvInfo.Framebuffer.MonitorType;

        ((TV_CURSOR_ADJUST_INFO *)(RequestPacket->OutputBuffer))->Underscan_x =
                                  hwDeviceExtension->NvInfo.Framebuffer.Underscan_x;

        ((TV_CURSOR_ADJUST_INFO *)(RequestPacket->OutputBuffer))->Underscan_y =
                                  hwDeviceExtension->NvInfo.Framebuffer.Underscan_y;

        ((TV_CURSOR_ADJUST_INFO *)(RequestPacket->OutputBuffer))->Scale_x =
                                  hwDeviceExtension->NvInfo.Framebuffer.Scale_x;

        ((TV_CURSOR_ADJUST_INFO *)(RequestPacket->OutputBuffer))->Scale_y =
                                  hwDeviceExtension->NvInfo.Framebuffer.Scale_y;

        ((TV_CURSOR_ADJUST_INFO *)(RequestPacket->OutputBuffer))->FilterEnable =
                                  hwDeviceExtension->NvInfo.Framebuffer.FilterEnable;

        ((TV_CURSOR_ADJUST_INFO *)(RequestPacket->OutputBuffer))->TVCursorMin =
                                  hwDeviceExtension->NvInfo.Dac.TVCursorMin;

        ((TV_CURSOR_ADJUST_INFO *)(RequestPacket->OutputBuffer))->TVCursorMax =
                                  hwDeviceExtension->NvInfo.Dac.TVCursorMax;

        status = NO_ERROR;

        break;

    //**************************************************************************
    // GET_IF09_VIDEO_SUPPORT
    //**************************************************************************

    case IOCTL_VIDEO_GET_IF09_VIDEO_SUPPORT:

        VideoDebugPrint((2, "NVStartIO - IOCTL_VIDEO_GET_IF09_VIDEO_SUPPORT\n"));

        //******************************************************************************
        // Should we support IF09 format video data?
        //******************************************************************************
        if (RequestPacket->OutputBufferLength  == sizeof(ULONG))
            {
            //******************************************************************************
            // return whether or IF09 format video data should be supported.
            //******************************************************************************
            *((ULONG *) RequestPacket->OutputBuffer) = hwDeviceExtension->bEnableIF09;
            RequestPacket->StatusBlock->Information = sizeof(ULONG);
            status = NO_ERROR;
            }
        else
            {
            status = ERROR_INSUFFICIENT_BUFFER;
            }

        break;

    //**************************************************************************
    // GET_COMMON_DEVICE_INFO
    //**************************************************************************

    case IOCTL_VIDEO_GET_COMMON_DEVICE_INFO:

        VideoDebugPrint((2, "NVStartIO - IOCTL_VIDEO_GET_COMMON_DEVICE_INFO\n"));

        if (RequestPacket->OutputBufferLength  == sizeof(NV_COMMON_DEVICE_INFO))
        {
            NV_COMMON_DEVICE_INFO *pCommonDeviceInfo = (NV_COMMON_DEVICE_INFO *)RequestPacket->OutputBuffer;
            NV_OS_VERSION_INFO    *pOsVersionInfo    = &(pCommonDeviceInfo->OsVersionInfo);

            pCommonDeviceInfo->ulDeviceReference   = hwDeviceExtension->DeviceReference;
            pCommonDeviceInfo->ulEnableDualView    = 0;
            pCommonDeviceInfo->ulDualViewSecondary = 0;

            pOsVersionInfo->dwBuildType    = 0xffffffff;
            pOsVersionInfo->dwMajorVersion = 0xffffffff;
            pOsVersionInfo->dwMinorVersion = 0xffffffff;
            pOsVersionInfo->dwBuildNumber  = 0xffffffff;

#if _WIN32_WINNT >= 0x0500
            pCommonDeviceInfo->ulEnableDualView = (hwDeviceExtension->ulDualViewStatus & DUALVIEW_STATUS_ENABLE_DUALVIEW) ? 1 : 0;
            pCommonDeviceInfo->ulDualViewSecondary = (hwDeviceExtension->ulDualViewStatus & DUALVIEW_STATUS_SECONDARY) ? 1 : 0;
            pOsVersionInfo->dwBuildType = PsGetVersion(&pOsVersionInfo->dwMajorVersion,
                                                       &pOsVersionInfo->dwMinorVersion,
                                                       &pOsVersionInfo->dwBuildNumber, NULL);
            pCommonDeviceInfo->SysMemInfo.SystemMemorySize   = hwDeviceExtension->SystemMemorySize;
            pCommonDeviceInfo->SysMemInfo.AgpAllocationLimit = hwDeviceExtension->AgpServices.AllocationLimit;
#endif

            RequestPacket->StatusBlock->Information = sizeof(NV_COMMON_DEVICE_INFO);
            status = NO_ERROR;
        }
        else
        {
            status = ERROR_INSUFFICIENT_BUFFER;
        }

        break;

    //**************************************************************************
    // GET_OGL_STENCIL_SWITCH
    //**************************************************************************

    case IOCTL_VIDEO_GET_OGL_STENCIL_SWITCH:

        VideoDebugPrint((2, "NVStartIO - IOCTL_VIDEO_GET_OGL_STENCIL_SWITCH\n"));

        if (RequestPacket->OutputBufferLength  == sizeof(ULONG))
        {
            *((ULONG *) RequestPacket->OutputBuffer) = hwDeviceExtension->OglStencilSwitch;
            RequestPacket->StatusBlock->Information = sizeof(ULONG);
            status = NO_ERROR;
        }
        else
        {
            status = ERROR_INSUFFICIENT_BUFFER;
        }

        break;


    //**********************************************************************************
    // The following IOCTLs allow communication between the display driver and the RM
    //**********************************************************************************

    //****CHECK FOR ERROR_INSUFFICIENT_BUFFER here

    // MP/RM merge...

    // TO DO: remove open and close
    // open and close are here for compatibility
    case NVAPI_IOCTL_OPEN:
        status = NO_ERROR;
        break;

    case NVAPI_IOCTL_CLOSE:
        status = NO_ERROR;
        break;

    case IOCTL_NV01_ALLOC_ROOT:
        Nv01AllocRoot((PVOID)(*(ULONG_PTR *)(RequestPacket->InputBuffer)));
        status = RequestPacket->StatusBlock->Status;
        break;

    case IOCTL_NV01_ALLOC_DEVICE:
        Nv01AllocDevice((PVOID)(*(ULONG_PTR *)(RequestPacket->InputBuffer)));
        status = RequestPacket->StatusBlock->Status;
        break;

    case IOCTL_NV01_ALLOC_CONTEXT_DMA:
        Nv01AllocContextDma((PVOID)(*(ULONG_PTR *)(RequestPacket->InputBuffer)));
        status = RequestPacket->StatusBlock->Status;
        break;

    case IOCTL_NV01_ALLOC_CHANNEL_PIO:
        Nv01AllocChannelPio((PVOID)(*(ULONG_PTR *)(RequestPacket->InputBuffer)));
        status = RequestPacket->StatusBlock->Status;
        break;

    case IOCTL_NV03_ALLOC_CHANNEL_DMA:
        Nv03AllocChannelDma((PVOID)(*(ULONG_PTR *)(RequestPacket->InputBuffer)));
        status = RequestPacket->StatusBlock->Status;
        break;

    case IOCTL_NV01_ALLOC_MEMORY:
        Nv01AllocMemory((PVOID)(*(ULONG_PTR *)(RequestPacket->InputBuffer)));
        status = RequestPacket->StatusBlock->Status;
        break;

    case IOCTL_NV01_ALLOC_OBJECT:
        Nv01AllocObject((PVOID)(*(ULONG_PTR *)(RequestPacket->InputBuffer)));
        status = RequestPacket->StatusBlock->Status;
        break;

    case IOCTL_NV01_ALLOC_EVENT:
        Nv01AllocEvent((PVOID)(*(ULONG_PTR *)(RequestPacket->InputBuffer)));
        status = RequestPacket->StatusBlock->Status;
        break;

    case IOCTL_NV04_ALLOC:
        Nv04Alloc((PVOID)(*(ULONG_PTR *)(RequestPacket->InputBuffer)));
        status = RequestPacket->StatusBlock->Status;
        break;

    case IOCTL_NV01_FREE:
        Nv01Free((PVOID)(*(ULONG_PTR *)(RequestPacket->InputBuffer)));
        status = RequestPacket->StatusBlock->Status;
        break;

    case IOCTL_NV03_DMA_FLOW_CONTROL:
        Nv03DmaFlowControl((PVOID)(*(ULONG_PTR *)(RequestPacket->InputBuffer)));
        status = RequestPacket->StatusBlock->Status;
        break;

    case IOCTL_NV01_ARCH_HEAP:
        Nv03ArchHeap((PVOID)(*(ULONG_PTR *)(RequestPacket->InputBuffer)));
        status = RequestPacket->StatusBlock->Status;
        break;

    case IOCTL_NV01_CONFIG_VERSION:
        Nv01ConfigVersion((PVOID)(*(ULONG_PTR *)(RequestPacket->InputBuffer)));
        status = RequestPacket->StatusBlock->Status;
        break;

    case IOCTL_NV01_CONFIG_GET:
        Nv01ConfigGet((PVOID)(*(ULONG_PTR *)(RequestPacket->InputBuffer)));
        status = RequestPacket->StatusBlock->Status;
        break;

    case IOCTL_NV01_CONFIG_SET:
        Nv01ConfigSet((PVOID)(*(ULONG_PTR *)(RequestPacket->InputBuffer)));
        status = RequestPacket->StatusBlock->Status;
        break;

    case IOCTL_NV04_CONFIG_GET_EX:
        Nv04ConfigGetEx((PVOID)(*(ULONG_PTR *)(RequestPacket->InputBuffer)));
        status = RequestPacket->StatusBlock->Status;
        break;

    case IOCTL_NV04_CONFIG_SET_EX:
        Nv04ConfigSetEx((PVOID)(*(ULONG_PTR *)(RequestPacket->InputBuffer)));
        status = RequestPacket->StatusBlock->Status;
        break;

    case IOCTL_NV01_CONFIG_UPDATE:
        Nv01ConfigUpdate((PVOID)(*(ULONG_PTR *)(RequestPacket->InputBuffer)));
        status = RequestPacket->StatusBlock->Status;
        break;

    case IOCTL_NV01_DEBUG_CONTROL:
        Nv01DebugControl((PVOID)(*(ULONG_PTR *)(RequestPacket->InputBuffer)));
        status = RequestPacket->StatusBlock->Status;
        break;

    case IOCTL_NV04_DIRECT_METHOD_CALL:
        Nv04DirectMethodCall((PVOID)(*(ULONG_PTR *)(RequestPacket->InputBuffer)));
        status = RequestPacket->StatusBlock->Status;
        break;

        //...jsw

    case IOCTL_NV04_I2C_ACCESS:
         {
            NVOS_I2C_ACCESS_PARAMS *i2cparams;

            i2cparams = (NVOS_I2C_ACCESS_PARAMS *)RequestPacket->InputBuffer;

            // Make sure that the data handed us is within limits
            if( (RequestPacket->InputBufferLength  < sizeof(NVOS_I2C_ACCESS_PARAMS)) ||
                (RequestPacket->OutputBufferLength < sizeof(NVOS_I2C_ACCESS_PARAMS)) ){

               status = ERROR_INSUFFICIENT_BUFFER;
               RequestPacket->StatusBlock->Information = 0;
            }
            else {

               // Have the RM help us with this one...
               Nv04I2CAccess((NVOS_I2C_ACCESS_PARAMS *)i2cparams);

               // Make sure that we only give back the right amount
               RequestPacket->StatusBlock->Information = sizeof(NVOS_I2C_ACCESS_PARAMS);
               status = NO_ERROR;
            }
         }
        break;


    //**************************************************************************
    // SYSMEM_SHARE_MEMORY
    //**************************************************************************

    case IOCTL_VIDEO_SYSMEM_SHARE_MEMORY:
        {
        NV_SYSMEM_SHARE_MEMORY *pSysmemShareMemoryIn = NULL;
        NV_SYSMEM_SHARE_MEMORY *pSysmemShareMemoryOut = NULL;
        PHYSICAL_ADDRESS        physAddr;

        if ( (RequestPacket->OutputBufferLength < sizeof(NV_SYSMEM_SHARE_MEMORY)) ||
             (RequestPacket->InputBufferLength < sizeof(NV_SYSMEM_SHARE_MEMORY)) )
            {
            VideoDebugPrint((0, "IOCTL_VIDEO_SYSMEM_SHARE_MEMORY - ERROR_INSUFFICIENT_BUFFER\n"));
            status = ERROR_INSUFFICIENT_BUFFER;
            break;
            }

        pSysmemShareMemoryIn = RequestPacket->InputBuffer;
        RequestPacket->StatusBlock->Information = sizeof(NV_SYSMEM_SHARE_MEMORY);
        NvSystemMemoryShare(pSysmemShareMemoryIn);
        pSysmemShareMemoryOut = RequestPacket->OutputBuffer;

        pSysmemShareMemoryOut->pSystemMdl       = pSysmemShareMemoryIn->pSystemMdl;
        pSysmemShareMemoryOut->physicalAddress  = pSysmemShareMemoryIn->physicalAddress;
        pSysmemShareMemoryOut->ddVirtualAddress = pSysmemShareMemoryIn->ddVirtualAddress;
        pSysmemShareMemoryOut->byteLength       = pSysmemShareMemoryIn->byteLength;
        pSysmemShareMemoryOut->bDontModifyMDL   = pSysmemShareMemoryIn->bDontModifyMDL;

        VideoDebugPrint((0, "SHARE: pSystemMdl = 0x%p\n",
                        pSysmemShareMemoryOut->pSystemMdl));
        VideoDebugPrint((0, "SHARE: physicalAddress = 0x%p\n",
                        pSysmemShareMemoryOut->physicalAddress));
        VideoDebugPrint((0, "SHARE: ddVirtualAddress = 0x%p\n",
                        pSysmemShareMemoryOut->ddVirtualAddress));
        VideoDebugPrint((0, "SHARE: bDontModifyMDL = 0x%p\n",
                        pSysmemShareMemoryOut->bDontModifyMDL));

        status = NO_ERROR;

        }
        break;

    //**************************************************************************
    // SYSMEM_UNSHARE_MEMORY
    //**************************************************************************

    case IOCTL_VIDEO_SYSMEM_UNSHARE_MEMORY:
        {
        NV_SYSMEM_SHARE_MEMORY *pSysmemShareMemoryIn;

        if (RequestPacket->InputBufferLength < sizeof(NV_SYSMEM_SHARE_MEMORY))
            {
            status = ERROR_INSUFFICIENT_BUFFER;
            break;
            }
        pSysmemShareMemoryIn = RequestPacket->InputBuffer;
        VideoDebugPrint((0, "UNSHARE: pSystemMdl = 0x%p\n",
                        pSysmemShareMemoryIn->pSystemMdl));
        VideoDebugPrint((0, "UNSHARE: ddVirtualAddress = 0x%p\n",
                        pSysmemShareMemoryIn->ddVirtualAddress));
        VideoDebugPrint((0, "UNSHARE: bDontModifyMDL = 0x%p\n",
                        pSysmemShareMemoryIn->bDontModifyMDL));
        NvSystemMemoryUnShare(pSysmemShareMemoryIn);
        status = NO_ERROR;
        }
        break;

    //**************************************************************************
    // LOCK_DOWN_MEMORY
    //**************************************************************************

    case IOCTL_VIDEO_LOCK_DOWN_MEMORY:
        {
        NV_LOCK_DOWN_MEMORY *pLockdownMemoryIn = NULL;
        NV_LOCK_DOWN_MEMORY *pLockdownMemoryOut = NULL;

        if ( (RequestPacket->OutputBufferLength < sizeof(NV_LOCK_DOWN_MEMORY)) ||
             (RequestPacket->InputBufferLength < sizeof(NV_LOCK_DOWN_MEMORY)) )
            {
            VideoDebugPrint((0, "IOCTL_VIDEO_LOCK_DOWN_MEMORY - ERROR_INSUFFICIENT_BUFFER\n"));
            status = ERROR_INSUFFICIENT_BUFFER;
            break;
            }
        pLockdownMemoryIn = RequestPacket->InputBuffer;
        RequestPacket->StatusBlock->Information = sizeof(NV_LOCK_DOWN_MEMORY);
        NvLockdownMemory(pLockdownMemoryIn);
        if (pLockdownMemoryIn->pMdl == NULL)
            {
            VideoDebugPrint((0, "IOCTL_VIDEO_LOCK_DOWN_MEMORY - Fails lock - ERROR_INSUFFICIENT_BUFFER\n"));
            status = ERROR_INSUFFICIENT_BUFFER;
            break;
            }
        pLockdownMemoryOut = RequestPacket->OutputBuffer;
        pLockdownMemoryOut->pMdl           = pLockdownMemoryIn->pMdl;
        pLockdownMemoryOut->virtualAddress = pLockdownMemoryIn->virtualAddress;
        pLockdownMemoryOut->byteLength     = pLockdownMemoryIn->byteLength;

        VideoDebugPrint((0, "LOCKDOWN: virtualAddress = 0x%p\n",
                        pLockdownMemoryOut->virtualAddress));
        VideoDebugPrint((0, "LOCKDOWN: pMdl = 0x%p\n",
                        pLockdownMemoryOut->pMdl));
        status = NO_ERROR;

        VideoDebugPrint((2, "W32StartIO - LockDownMemory Exit\n"));
        }
        break;

    //**************************************************************************
    // UNLOCK_MEMORY
    //**************************************************************************

    case IOCTL_VIDEO_UNLOCK_MEMORY:
        {
        NV_UNLOCK_MEMORY *pUnlockMemoryIn = NULL;
        NV_UNLOCK_MEMORY *pUnlockMemoryOut = NULL;

        if ( (RequestPacket->OutputBufferLength < sizeof(NV_UNLOCK_MEMORY)) ||
             (RequestPacket->InputBufferLength < sizeof(NV_UNLOCK_MEMORY)) )
            {
            VideoDebugPrint((0, "IOCTL_VIDEO_UNLOCK_MEMORY - ERROR_INSUFFICIENT_BUFFER\n"));
            status = ERROR_INSUFFICIENT_BUFFER;
            break;
            }
        pUnlockMemoryIn = RequestPacket->InputBuffer;
        RequestPacket->StatusBlock->Information = sizeof(NV_UNLOCK_MEMORY);
        NvUnlockMemory(pUnlockMemoryIn);
        pUnlockMemoryOut = RequestPacket->OutputBuffer;
        pUnlockMemoryOut->pMdl = pUnlockMemoryIn->pMdl;
        VideoDebugPrint((0, "UNLOCK: pMdl = 0x%p\n", pUnlockMemoryOut->pMdl));
        status = NO_ERROR;
        }
        break;

    //**************************************************************************
    // MAP_TO_USER
    //**************************************************************************

    case IOCTL_VIDEO_MAP_TO_USER:
        {
        NV_SYSMEM_SHARE_MEMORY *pSysmemShareMemoryIn = NULL;
        NV_SYSMEM_SHARE_MEMORY *pSysmemShareMemoryOut = NULL;
        PHYSICAL_ADDRESS        physAddr;

        if ( (RequestPacket->OutputBufferLength < sizeof(NV_SYSMEM_SHARE_MEMORY)) ||
             (RequestPacket->InputBufferLength < sizeof(NV_SYSMEM_SHARE_MEMORY)) )
            {
            VideoDebugPrint((0, "IOCTL_VIDEO_MAP_TO_USER - ERROR_INSUFFICIENT_BUFFER\n"));
            status = ERROR_INSUFFICIENT_BUFFER;
            break;
            }

        pSysmemShareMemoryIn = RequestPacket->InputBuffer;
        RequestPacket->StatusBlock->Information = sizeof(NV_SYSMEM_SHARE_MEMORY);
        // pSysmemShareMemoryIn->ddVirtualAddress points to kernel address which may be locked down
        // pSysmemShareMemoryIn->byteLength points to length of kernel address memory
        NvMapIntoUserSpace(pSysmemShareMemoryIn);
        pSysmemShareMemoryOut = RequestPacket->OutputBuffer;
        pSysmemShareMemoryOut->userVirtualAddress = pSysmemShareMemoryIn->userVirtualAddress;
        pSysmemShareMemoryOut->physicalAddress    = pSysmemShareMemoryIn->physicalAddress;
        status = NO_ERROR;
        }
        break;

    //**************************************************************************
    // UNMAP_FROM_USER
    //**************************************************************************

    case IOCTL_VIDEO_UNMAP_FROM_USER:
        {
        NV_SYSMEM_SHARE_MEMORY *pSysmemShareMemoryIn = NULL;
        NV_SYSMEM_SHARE_MEMORY *pSysmemShareMemoryOut = NULL;
        PHYSICAL_ADDRESS        physAddr;

        if ( (RequestPacket->OutputBufferLength < sizeof(NV_SYSMEM_SHARE_MEMORY)) ||
             (RequestPacket->InputBufferLength < sizeof(NV_SYSMEM_SHARE_MEMORY)) )
            {
            VideoDebugPrint((0, "IOCTL_VIDEO_MAP_TO_USER - ERROR_INSUFFICIENT_BUFFER\n"));
            status = ERROR_INSUFFICIENT_BUFFER;
            break;
            }

        pSysmemShareMemoryIn = RequestPacket->InputBuffer;
        RequestPacket->StatusBlock->Information = sizeof(NV_SYSMEM_SHARE_MEMORY);
        // pSysmemShareMemoryIn->pSystemMdl points to previously allocated MDL
        NvUnmapFromUserSpace(pSysmemShareMemoryIn);
        pSysmemShareMemoryOut = RequestPacket->OutputBuffer;
        status = NO_ERROR;
        }
        break;


#if (_WIN32_WINNT >= 0x0500)

    //**************************************************************************
    // NT5 only !
    // IOCTL_VIDEO_HANDLE_VIDEOPARAMETERS
    //**************************************************************************

    case IOCTL_VIDEO_HANDLE_VIDEOPARAMETERS:

        VideoDebugPrint((2, "NVStartIO - IOCTL Handle Video Parameters\n"));

        status = ERROR_INVALID_FUNCTION;
        
        if( (RequestPacket->InputBufferLength  == sizeof(VIDEOPARAMETERS) ) &&
            (RequestPacket->OutputBufferLength == sizeof(VIDEOPARAMETERS) ) )
        {
            VIDEOPARAMETERS * pInVideoParameters  = (VIDEOPARAMETERS *) RequestPacket->InputBuffer;
            VIDEOPARAMETERS * pOutVideoParameters = (VIDEOPARAMETERS *) RequestPacket->OutputBuffer;
            ULONG i, dwTemp;
            NV_CFGEX_VIDEO_ENCODER_TYPE_PARAMS nvEncoder;

            switch(pInVideoParameters->dwCommand)
            {
                case VP_COMMAND_GET:
                {
                    pOutVideoParameters->dwMode = VP_MODE_WIN_GRAPHICS;
                    pOutVideoParameters->dwFlags = 0;
                    pOutVideoParameters->dwTVStandard = 0;
            
                    for(i = 0; i < hwDeviceExtension->DeskTopInfo.ulNumberDacsActive; i++)
                    {
                        if( (hwDeviceExtension->DeskTopInfo.ulDeviceType[i] == MONITOR_TYPE_NTSC) ||
                            (hwDeviceExtension->DeskTopInfo.ulDeviceType[i] == MONITOR_TYPE_PAL) )
                        {
                            pOutVideoParameters->dwFlags  = VP_FLAGS_TV_MODE;
                            pOutVideoParameters->dwFlags |= VP_FLAGS_TV_STANDARD;
                            if(hwDeviceExtension->DeskTopInfo.ulDeviceType[i] == MONITOR_TYPE_NTSC)
                            {
                                pOutVideoParameters->dwTVStandard = pOutVideoParameters->dwAvailableTVStandard 
                                    = VP_TV_STANDARD_NTSC_M;
                            }
                            else
                            {
                                pOutVideoParameters->dwTVStandard = pOutVideoParameters->dwAvailableTVStandard
                                    = VP_TV_STANDARD_PAL_B;
                            }

                            pOutVideoParameters->dwAvailableModes = VP_MODE_WIN_GRAPHICS;
            
                            pOutVideoParameters->dwFlags |= VP_FLAGS_MAX_UNSCALED;
                            pOutVideoParameters->dwMaxUnscaledX = hwDeviceExtension->DeskTopInfo.ulDisplayWidth[i];
                            pOutVideoParameters->dwMaxUnscaledY = hwDeviceExtension->DeskTopInfo.ulDisplayHeight[i];
            
                            pOutVideoParameters->dwFlags |= VP_FLAGS_POSITION;
                            pOutVideoParameters->dwPositionX = 0;
                            pOutVideoParameters->dwPositionY = 0;
            
                            pOutVideoParameters->dwCPType = 0;
                            pOutVideoParameters->dwCPStandard = 0;
            
                            nvEncoder.Head = i;
                            RmConfigGetExKernel(hwDeviceExtension->DeviceReference, NV_CFGEX_VIDEO_ENCODER_TYPE, &nvEncoder, sizeof(NV_CFGEX_VIDEO_ENCODER_TYPE));
                            switch(nvEncoder.EncoderType)
                            {
                                case NV_ENCODER_BROOKTREE_868:
                                case NV_ENCODER_CHRONTEL_7003:
                                case NV_ENCODER_CHRONTEL_7006:
                                case NV_ENCODER_CHRONTEL_7008:
                                    // Not macrovision support
                                    break;
                                default:
                                    // macrovision support
                                    pOutVideoParameters->dwFlags |= VP_FLAGS_COPYPROTECT;
                                    pOutVideoParameters->dwCPType = VP_CP_TYPE_APS_TRIGGER;
                                    pOutVideoParameters->bCP_APSTriggerBits = hwDeviceExtension->ulMacrovisionMode; 
                                    switch(hwDeviceExtension->DeskTopInfo.ulTVFormat[i])
                                    {
                                        case NTSC_M:
                                            dwTemp = VP_TV_STANDARD_NTSC_M;
                                        break;
                                        case NTSC_J:
                                            dwTemp = VP_TV_STANDARD_NTSC_M_J;
                                        break;
                                        case PAL_M:
                                            dwTemp = VP_TV_STANDARD_PAL_M;
                                        break;
                                        case PAL_A:
                                            dwTemp = VP_TV_STANDARD_PAL_B | VP_TV_STANDARD_PAL_D | VP_TV_STANDARD_PAL_G |
                                                 VP_TV_STANDARD_PAL_H | VP_TV_STANDARD_PAL_I;
                                        break;
                                        case PAL_N:
                                            dwTemp = VP_TV_STANDARD_PAL_N;
                                        break;
                                        case PAL_NC:
                                        default:
                                            dwTemp = VP_TV_STANDARD_WIN_VGA;
                                        break;
                                    }
                                    pOutVideoParameters->dwCPStandard = dwTemp;
                                    break;
                                break;
                            }
                            break;
                        } // if
                    } // for
                    RequestPacket->StatusBlock->Information = sizeof(VIDEOPARAMETERS);
                    VideoDebugPrint((0, "NVStartIO - IOCTL Handle Video Parameters Get dwCommand \n"));
                    status = NO_ERROR;
                    break;
                }
                case VP_COMMAND_SET:
                {
                    if((pInVideoParameters->dwFlags  &  VP_FLAGS_COPYPROTECT) &&
                       (pInVideoParameters->dwCPType == VP_CP_TYPE_APS_TRIGGER) )
                    {
                        NV_CFGEX_SET_MACROVISION_MODE_PARAMS nvMacrovision;

                        memcpy((PUCHAR) pOutVideoParameters, (PUCHAR) pInVideoParameters, sizeof(VIDEOPARAMETERS));
                        for(i = 0; i < hwDeviceExtension->DeskTopInfo.ulNumberDacsActive; i++)
                        {
                            if( (hwDeviceExtension->DeskTopInfo.ulDeviceType[i] == MONITOR_TYPE_NTSC) ||
                                (hwDeviceExtension->DeskTopInfo.ulDeviceType[i] == MONITOR_TYPE_PAL) )
                            {
                                nvEncoder.Head = i;
                                RmConfigGetExKernel(hwDeviceExtension->DeviceReference, NV_CFGEX_VIDEO_ENCODER_TYPE, 
                                        &nvEncoder, sizeof(NV_CFGEX_VIDEO_ENCODER_TYPE));
                                switch(nvEncoder.EncoderType)
                                {
                                    case NV_ENCODER_BROOKTREE_868:
                                    case NV_ENCODER_CHRONTEL_7003:
                                    case NV_ENCODER_CHRONTEL_7006:
                                    case NV_ENCODER_CHRONTEL_7008:
                                    // Not macrovision support
                                        break;
                                    default:
                                        // macrovision support
                                        nvMacrovision.head = i;
                                        nvMacrovision.mode = pInVideoParameters->bCP_APSTriggerBits;
                                        nvMacrovision.resx = hwDeviceExtension->DeskTopInfo.ulDisplayWidth[i];
                                        nvMacrovision.resy = hwDeviceExtension->DeskTopInfo.ulDisplayHeight[i];
                                        switch(pInVideoParameters->dwCPCommand)
                                        {
                                            case VP_CP_CMD_ACTIVATE:
                                                if(RmConfigSetExKernel(hwDeviceExtension->DeviceReference,
                                                     NV_CFGEX_SET_MACROVISION_MODE,
                                                     &nvMacrovision,
                                                     sizeof(NV_CFGEX_SET_MACROVISION_MODE_PARAMS))) 
                                                {
                                                    pOutVideoParameters->dwCPKey = hwDeviceExtension->ulCurrentMacrovisionKey 
                                                                                 = hwDeviceExtension->ulNextMacrovisionKey++;
                                                }
                                            break;
                                            case VP_CP_CMD_DEACTIVATE:
                                                if(pInVideoParameters->dwCPKey == hwDeviceExtension->ulCurrentMacrovisionKey)
                                                {
                                                    nvMacrovision.mode = 0;
                                                    if(RmConfigSetExKernel(hwDeviceExtension->DeviceReference,
                                                         NV_CFGEX_SET_MACROVISION_MODE,
                                                         &nvMacrovision,
                                                         sizeof(NV_CFGEX_SET_MACROVISION_MODE_PARAMS))) 
                                                    {
                                                        // Disabled successfully
                                                    }
                                                    else
                                                    {
                                                        // Failed to disable it.
                                                    }
                                                }
                                            break;
                                            case VP_CP_CMD_CHANGE:
                                                if(pInVideoParameters->dwCPKey == hwDeviceExtension->ulCurrentMacrovisionKey)
                                                {
                                                    nvMacrovision.mode  = pInVideoParameters->bCP_APSTriggerBits & 0x03;
                                                    if(RmConfigSetExKernel(hwDeviceExtension->DeviceReference,
                                                         NV_CFGEX_SET_MACROVISION_MODE,
                                                         &nvMacrovision,
                                                         sizeof(NV_CFGEX_SET_MACROVISION_MODE_PARAMS))) 
                                                    {
                                                        // changed successfully
                                                    }
                                                    else
                                                    {
                                                        // Failed to change it.
                                                    }
                                                }
                                            break;
                                        }
                                        RequestPacket->StatusBlock->Information = sizeof(VIDEOPARAMETERS);
                                        hwDeviceExtension->ulMacrovisionMode = nvMacrovision.mode;
                                } // switch
                            } // if
                        } // for
                   } // if
                   status = NO_ERROR;
                    break; //  
                } // 
                default:
                    break;
             } // switch
        }
        break;

        case IOCTL_VIDEO_SHOULD_AGP_BE_ENABLED:
            {
                if (hwDeviceExtension->bAgp)
                    status = NO_ERROR;
                else
                    status = ERROR_INVALID_FUNCTION;
            }
            break;

        //**************************************************************************
        // IOCTL_VIDEO_ALLOC_AGP_MEM
        //**************************************************************************

        case IOCTL_VIDEO_ALLOC_AGP_MEM:
            {
            ULONG       cPages;
            PAGP_MEM    pBlock, pOutputBuffer;
            PVOID       VirtualAddress;
            VideoDebugPrint((2, "NVStartIO - IOCTL Alloc AGP Mem\n"));

            if ((RequestPacket->OutputBufferLength < sizeof(AGP_MEM)) ||
                 (RequestPacket->InputBufferLength < sizeof(AGP_MEM)))
                {
                VideoDebugPrint((0, "IOCTL_VIDEO_ALLOC_AGP_MEM - ERROR_INSUFFICIENT_BUFFER\n"));
                status = ERROR_INSUFFICIENT_BUFFER;
                break;
                }
            else if (!hwDeviceExtension->bAgp)
                {
                status = ERROR_INVALID_FUNCTION;
                break;
                }
            pBlock = (PAGP_MEM) (RequestPacket->InputBuffer);
            pOutputBuffer = (PAGP_MEM) (RequestPacket->OutputBuffer);
            cPages = ((pBlock->cb / PAGE_SIZE) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
            //
            // WARNING: The (ULONG_PTR) cast makes the warning go away, 
            //          but it is possible this is broken since hContext
            //          may not be big enough on 64 bit systems.
            //
            if (!(hwDeviceExtension->AgpVirtualBaseAddress =
                    VideoPortAgpReserveVirtual(
                        hwDeviceExtension,
                        (HANDLE)((ULONG_PTR)pBlock->hContext),
                        hwDeviceExtension->AgpPhysContext,
                        (PVOID *)(&(hwDeviceExtension->dummyVirtualContext)))))
                {
                status = ERROR_NOT_ENOUGH_MEMORY;
                break;
                }
            *pOutputBuffer = *pBlock;
            if (!(pOutputBuffer->VirtualAddress =
                    VideoPortAgpCommitVirtual(
                        hwDeviceExtension,
                        hwDeviceExtension->dummyVirtualContext,
                        cPages, 0)))
                {
                VideoPortAgpReleaseVirtual(
                    hwDeviceExtension,
                    hwDeviceExtension->dummyVirtualContext);
                hwDeviceExtension->dummyVirtualContext = NULL;
                status = ERROR_NOT_ENOUGH_MEMORY;
                break;
                }
                RequestPacket->StatusBlock->Information = sizeof(AGP_MEM);
                status = NO_ERROR;
            }
            break;

        //**************************************************************************
        // IOCTL_VIDEO_FREE_AGP_MEM
        //**************************************************************************

        case IOCTL_VIDEO_FREE_AGP_MEM:
            {
            PVOID       VirtualAddress;
            PAGP_MEM    pBlock;
            ULONG       cPages;

            VideoDebugPrint((2, "NVStartIO - IOCTL Free AGP Mem\n"));
            if (RequestPacket->InputBufferLength < sizeof(AGP_MEM))
                {
                status = ERROR_INSUFFICIENT_BUFFER;
                break;
                }
            else if (!hwDeviceExtension->bAgp)
                {
                status = ERROR_INVALID_FUNCTION;
                break;
                }
            pBlock = (PAGP_MEM) RequestPacket->InputBuffer;
            cPages = ((pBlock->cb / PAGE_SIZE) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
            VideoPortAgpFreeVirtual(
                hwDeviceExtension,
                hwDeviceExtension->dummyVirtualContext,
                cPages,
                (ULONG)((PUCHAR)pBlock->VirtualAddress - (PUCHAR)hwDeviceExtension->dummyVirtualContext)
            );
            VideoPortAgpReleaseVirtual(hwDeviceExtension,
                hwDeviceExtension->dummyVirtualContext);
            hwDeviceExtension->dummyVirtualContext = NULL;
            status = NO_ERROR;
            }
            break;

        ////// all VPE commands go thru here !
#ifdef NVPE
        case IOCTL_VIDEO_VPE_COMMAND:
            VideoDebugPrint((2, "NVStartIO - IOCTL Video Port Extension Command\n"));
            status = NVPEProcessCommand(hwDeviceExtension, RequestPacket);
            break;
#endif  // #ifdef NVPE
#endif  // _WIN32_WINNT >= 0x0500




        //****************************************************************************************************
        // IOCTL_VIDEO_SET_TWINVIEW_STATE
        // Sets the user selected state of Clone/Normal/Multimon.
        // Also sets the other pieces of info like DeviceType, Physical and virtual rectl for each head etc..
        //****************************************************************************************************

        case IOCTL_VIDEO_SET_TWINVIEW_STATE:
            {
                NVTWINVIEWDATA *pInfo;

                VideoDebugPrint((6, "IOCTL_VIDEO_SET_TWINVIEW_STATE():\n"));
                if(RequestPacket->InputBufferLength < sizeof(NVTWINVIEWDATA)) 
                {
                    VideoDebugPrint((0, "IOCTL_VIDEO_SET_TWINVIEW_STATE(): Error: pInfo is wrong Size.\n"));
                    status = ERROR_INVALID_PARAMETER;
                    break;
                }

                pInfo = (NVTWINVIEWDATA *)(RequestPacket->InputBuffer);
                if (pInfo == NULL)
                {
                    VideoDebugPrint((0, "IOCTL_VIDEO_SET_TWINVIEW_STATE(): Error: pInfo is NULL.\n"));
                    status = ERROR_INVALID_PARAMETER;
                    break;
                }

                status = NVSetRegistryTwinviewInfo(hwDeviceExtension, pInfo);
            }
            break;

        //****************************************************************************************************
        // IOCTL_VIDEO_GET_TWINVIEW_STATE
        // Gets the user selected state of Clone/Normal/Multimon.
        // Also gets the other pieces of info like DeviceType, Physical and virtual rectl for each head etc..
        //****************************************************************************************************

        case IOCTL_VIDEO_GET_TWINVIEW_STATE:
            {
                NVTWINVIEWDATA *pInfo;

                VideoDebugPrint((6, "IOCTL_VIDEO_GET_TWINVIEW_STATE():\n"));
                if(RequestPacket->OutputBufferLength < sizeof(NVTWINVIEWDATA)) 
                {
                    VideoDebugPrint((0, "IOCTL_VIDEO_GET_TWINVIEW_STATE(): Error: pInfo is wrong Size.\n"));
                    status = ERROR_INVALID_PARAMETER;
                    break;
                }

                pInfo = (NVTWINVIEWDATA *)(RequestPacket->OutputBuffer);
                if (pInfo == NULL)
                {
                    VideoDebugPrint((0, "IOCTL_VIDEO_GET_TWINVIEW_STATE(): Error: pInfo is NULL.\n"));
                    status = ERROR_INVALID_PARAMETER;
                    break;
                }

                status = NVGetRegistryTwinviewInfo(hwDeviceExtension, pInfo);
                if (status != NO_ERROR)
                {
                    VideoPortZeroMemory(pInfo, sizeof(NVTWINVIEWDATA));
                    pInfo->dwState = hwDeviceExtension->DeskTopInfo.ulTwinView_State;
                }
                RequestPacket->StatusBlock->Information = sizeof(NVTWINVIEWDATA);
                status = NO_ERROR;
            }
            break;

        //****************************************************************************************************
        // IOCTL_VIDEO_GET_PCI_NUMBER
        //****************************************************************************************************
        case IOCTL_VIDEO_QUERY_PCI_SLOT:
        {
            ULONG * pulTemp;

            status = NO_ERROR;
            if(RequestPacket->OutputBufferLength < sizeof(ULONG)) 
            {
                VideoDebugPrint((0, "IOCTL_VIDEO_QUERY_PCI_SLOT(): Error: pInfo is wrong Size.\n"));
                status = ERROR_INVALID_PARAMETER;
                break;
            }
            pulTemp = (ULONG *)(RequestPacket->OutputBuffer);

            *pulTemp = hwDeviceExtension->ulSlot; 

            if (status == NO_ERROR)
                RequestPacket->StatusBlock->Information = sizeof(ULONG);
            else
                RequestPacket->StatusBlock->Information = 0;

        }
        break;

        //****************************************************************************************************
        // IOCTL_VIDEO_GET_NUMBER_DACS_ON_BOARD
        // Gets the number of DACS on the board
        //****************************************************************************************************

        case IOCTL_VIDEO_GET_NUMBER_DACS:
            {
                GET_NUMBER_DACS *pInfo;
                NV_DESKTOP_INFO *pDeskTopInfo;
                ULONG i, Count = 0;

                VideoDebugPrint((6, "IOCTL_VIDEO_GET_NUMBER_DACS():\n"));

                status = NO_ERROR;
                if(RequestPacket->OutputBufferLength < sizeof(GET_NUMBER_DACS)) 
                {
                    VideoDebugPrint((0, "IOCTL_VIDEO_GET_NUMBER_DACS(): Error: pInfo is wrong Size.\n"));
                    status = ERROR_INVALID_PARAMETER;
                    break;
                }

                pInfo = (GET_NUMBER_DACS *)(RequestPacket->OutputBuffer);
                if (pInfo == NULL)
                {
                    VideoDebugPrint((0, "IOCTL_VIDEO_GET_NUMBER_DACS(): Error: pInfo is NULL.\n"));
                    status = ERROR_INVALID_PARAMETER;
                    break;
                }

                // Extract the info from the hwDeviceExtension.
                pDeskTopInfo = (NV_DESKTOP_INFO *)&hwDeviceExtension->DeskTopInfo;

                // Stuff the info to send back to the display driver.
                pInfo->ulNumberDacsOnBoard = pDeskTopInfo->ulNumberDacsOnBoard;
                pInfo->ulNumberDacsConnected = pDeskTopInfo->ulNumberDacsConnected;
                pInfo->ulAllDeviceMask = pDeskTopInfo->ulAllDeviceMask;
                pInfo->ulConnectedDeviceMask = pDeskTopInfo->ulConnectedDeviceMask;
                pInfo->Mobile = hwDeviceExtension->Mobile;
                pInfo->UseHWSelectedDevice = hwDeviceExtension->UseHWSelectedDevice;
                pInfo->GDIModePruning = hwDeviceExtension->GDIModePruning;
                pInfo->UseHotKeySameMode = hwDeviceExtension->UseHotKeySameMode;
                pInfo->PanScanSelection = hwDeviceExtension->PanScanSelection;
                pInfo->ulDFPSupports1400x1050 = hwDeviceExtension->ulDFPSupports1400x1050;
                pInfo->ulHotKeyPending = hwDeviceExtension->ulHotKeyPending;
                pInfo->ulUniquenessVGA = hwDeviceExtension->ulUniquenessVGA;
                pInfo->ACPISystem = hwDeviceExtension->ACPISystem;

                for (i=0; i < pDeskTopInfo->ulNumberDacsOnBoard; i++)
                {
                    pInfo->ulDeviceMask[i] = pDeskTopInfo->ulDeviceMask[i];
                    pInfo->ulDeviceDisplay[i] = pDeskTopInfo->ulDeviceDisplay[i];
                    pInfo->ulDeviceType[i] = pDeskTopInfo->ulDeviceType[i];
                    pInfo->ulTVFormat[i] = pDeskTopInfo->ulTVFormat[i];
                    pInfo->ulHeadDeviceOptions[i] = pDeskTopInfo->ulHeadDeviceOptions[i];
                }


                VideoDebugPrint((3,"IOCTL_GET_NUMBER_DACS: Number of heads on board returning to display driver: 0x%x, number connected: 0x%x\n",
                    pInfo->ulNumberDacsOnBoard,pInfo->ulNumberDacsConnected));

                if (status == NO_ERROR)
                    RequestPacket->StatusBlock->Information = sizeof(GET_NUMBER_DACS);
                else
                    RequestPacket->StatusBlock->Information = 0;
            }
            break;

        case IOCTL_VIDEO_INDUCE_HEAD_DEVICE_SCANNING:
            {
                VideoDebugPrint((1, "IOCTL_VIDEO_INDUCE_HEAD_DEVICE_SCANNING():\n"));
                status = NO_ERROR;
#if _WIN32_WINNT >= 0x0500
                if (hwDeviceExtension->ulDualViewStatus & DUALVIEW_STATUS_SECONDARY) {
                    hwDeviceExtension = hwDeviceExtension->pDualViewAssociatedDE;
                }
#endif
                bScanHeadDeviceOptions(hwDeviceExtension);
                RequestPacket->StatusBlock->Information = 0;
#if _WIN32_WINNT >= 0x0500
                //******************************************************************
                // bScanHeadDeviceOptions has updated ulConnectedDeviceMask, so now's a good time to
                // have our NVGetChildDescriptor callback run (triggerd by VideoPortEnumerateChildren).
                // This allows new ACPI HwIds to be passed to VALIDATE/SET_CHILD_STATE_CONFIGURATION
                //******************************************************************
                VideoPortEnumerateChildren(hwDeviceExtension, NULL);
#endif // _WIN32_WINNT >= 0x0500
            }
            break;

    case IOCTL_VIDEO_INDUCE_TIMING_OVERRIDE:
            {
                VideoDebugPrint((1, "IOCTL_VIDEO_INDUCE_TIMING_OVERRIDE():\n"));
                status = NO_ERROR;
                SetTimingOverRide(hwDeviceExtension);
                RequestPacket->StatusBlock->Information = 0;
            }
            break;


        //****************************************************************************************************
        // IOCTL_VIDEO_GET_PHYSICAL_MODE_LIST
        // Returns the number and list of physical modes for the specified head.
        // Input is the list of virtual modes for that head (no doubled resolutions but single screen resolutions)
        //****************************************************************************************************

        case IOCTL_VIDEO_GET_HEAD_PHYSICAL_MODE_LIST:
            {
                HEAD_MODE_LIST_INFO *pInput, *pOutput;
                ULONG ulRet, ulRetSize;
                

                VideoDebugPrint((6, "IOCTL_VIDEO_GET_HEAD_PHYSICAL_MODE_LIST():\n"));
                status = NO_ERROR;
                
                if(RequestPacket->InputBufferLength < sizeof(HEAD_MODE_LIST_INFO)) 
                {
                    VideoDebugPrint((0, "IOCTL_VIDEO_GET_HEAD_PHYSICAL_MODE_LIST(): Error: InputBufferLength is wrong Size: 0x%x\n",
                        RequestPacket->InputBufferLength));
                    status = ERROR_INVALID_PARAMETER;
                    break;
                }

                pInput = (HEAD_MODE_LIST_INFO *)(RequestPacket->InputBuffer);
                
                if (pInput == NULL)
                {
                    VideoDebugPrint((0, "IOCTL_VIDEO_GET_HEAD_PHYSICAL_MODE_LIST(): Error: pInput is NULL.\n"));
                    status = ERROR_INVALID_PARAMETER;
                    break;
                }
                


                ulRetSize = 0;
                ulRet = GetHeadPhysicalModeList(hwDeviceExtension, 
                            pInput, &ulRetSize);

                if (ulRet == FALSE)
                {
                    status = ERROR_INVALID_PARAMETER;
                }


                VideoDebugPrint((3,"IOCTL_VIDEO_GET_HEAD_PHYSICAL_MODE_LIST: Number of PhysicalModes for head: %d returned is: %d\n",
                    pInput->ulHead, (ulRetSize/sizeof(MODE_ENTRY))));

                if (status == NO_ERROR)
                    RequestPacket->StatusBlock->Information = ulRetSize;
                else
                    RequestPacket->StatusBlock->Information = 0;
            }
            break;



        //**************************************************************************
        // IOCTL_VIDEO_SET_REGISTRY_VALUE
        // Sets a registry value.
        // Supports only DWORD entries right now.
        //**************************************************************************

        case IOCTL_VIDEO_SET_REGISTRY_VALUE:
            {
                PNV_REGISTRY_STRUCT pRegStruct;

                pRegStruct = (PNV_REGISTRY_STRUCT)(RequestPacket->InputBuffer);

                status = VideoPortSetRegistryParameters(hwDeviceExtension,
                                       pRegStruct->keyName,
                                       pRegStruct->keyVal,
                                       pRegStruct->keyValSize);
            }
            break;

        //**************************************************************************
        // IOCTL_VIDEO_GET_REGISTRY_PATH
        // Queries a registry path.
        //**************************************************************************
        case IOCTL_VIDEO_GET_REGISTRY_PATH:
            {
                if (strlen(hwDeviceExtension->DriverRegPath) > RequestPacket->OutputBufferLength)
                    status = ERROR_INVALID_PARAMETER;
                else
                {
                    strcpy((PUCHAR)(RequestPacket->OutputBuffer), hwDeviceExtension->DriverRegPath);
                    RequestPacket->StatusBlock->Information = strlen(hwDeviceExtension->DriverRegPath);
                    status = NO_ERROR;
                }
            }
            break;

        //**************************************************************************
        // IOCTL_VIDEO_QUERY_REGISTRY_VAL
        // Queries a registry value.
        // We support only DWORD entries right now.
        //**************************************************************************

        case IOCTL_VIDEO_QUERY_REGISTRY_VAL:
            {
                PWSTR     regKeyName;
                PVOID     regValData;

                regKeyName = (PWSTR)(RequestPacket->InputBuffer);
                regValData = (PVOID)(RequestPacket->OutputBuffer);

                status = VideoPortGetRegistryParameters(hwDeviceExtension,
                                       regKeyName,
                                       FALSE,
                                       NVRegistryCallback,
                                       regValData);
                if (status == NO_ERROR)
                    RequestPacket->StatusBlock->Information = sizeof(ULONG);
                else
                    RequestPacket->StatusBlock->Information = 0;

            }
            break;

        //**************************************************************************
        // IOCTL_VIDEO_QUERY_REGISTRY_BINARY_VAL
        // Queries a Binary registry value.
        //**************************************************************************

        case IOCTL_VIDEO_QUERY_REGISTRY_BINARY_VAL:
            {
                PWSTR     regKeyName;
                PNV_REGISTRY_STRUCT pRegStructInput;
                NV_REGISTRY_STRUCT  RegStruct;
                PVOID pOutput;
                ULONG OutputLength;
                
                pRegStructInput = (PNV_REGISTRY_STRUCT)(RequestPacket->InputBuffer);
                regKeyName = (PWSTR)(pRegStructInput->keyName);
                pOutput = (PVOID)(RequestPacket->OutputBuffer);
                OutputLength = (ULONG)(RequestPacket->OutputBufferLength);
                
                // Copy the required values into the input structure
                // Note that the OS preserves/restores the content of the  the 
                RegStruct.keyVal = pOutput;
                RegStruct.keyValSize = OutputLength;
                RegStruct.keyName = regKeyName;

                status = VideoPortGetRegistryParameters(hwDeviceExtension,
                                       regKeyName,
                                       FALSE,
                                       NVReadRegistryBinaryCallback,
                                       (PVOID)&RegStruct);
                if (status == NO_ERROR)
                {
                    RequestPacket->StatusBlock->Information = RegStruct.keyValSize;
                }
                else
                {
                    RequestPacket->StatusBlock->Information = 0;
                }
            }
            break;

#if (_WIN32_WINNT >= 0x0500)

#ifdef NV_ZW_FILE_IO_ENABLED
        //**************************************************************************
        // IOCTL_VIDEO_ZW_XXX
        // Wrappers for the Zw file IO functions.
        //**************************************************************************

        case IOCTL_VIDEO_ZW_CREATE_FILE:
            {
                PNV_ZW_CREATE_FILE_IN pZwCreateFileIn = (PNV_ZW_CREATE_FILE_IN)RequestPacket->InputBuffer;
                IO_STATUS_BLOCK IoBlock;
                PVOID hFile;
                PVOID *phFileOutput = RequestPacket->OutputBuffer;

                status = ZwCreateFile( &hFile,
                                       pZwCreateFileIn->ulDesiredAccess,
                                       &pZwCreateFileIn->ObjectAttributes,
                                       &IoBlock,
                                       &pZwCreateFileIn->liAllocationSize,
                                       pZwCreateFileIn->ulFileAttributes,
                                       pZwCreateFileIn->ulShareAccess,
                                       pZwCreateFileIn->ulCreateDisposition,
                                       pZwCreateFileIn->ulCreateOptions,
                                       pZwCreateFileIn->pvEaBuffer,
                                       pZwCreateFileIn->ulEaLength );

    

                if (status == NO_ERROR)
                {
                    *phFileOutput = hFile;
                    RequestPacket->StatusBlock->Information = sizeof(hFile);
                }
                else
                {
                    *phFileOutput = NULL;
                    RequestPacket->StatusBlock->Information = 0;
                }

            }
            break;

        case IOCTL_VIDEO_ZW_WRITE_FILE:
            {
                PNV_ZW_RW_FILE_IN pZwWriteFileIn = (PNV_ZW_RW_FILE_IN)RequestPacket->OutputBuffer;
                IO_STATUS_BLOCK IoBlock;

                status = ZwWriteFile( pZwWriteFileIn->pvFileHandle,
                                      pZwWriteFileIn->pvEvent,
                                      pZwWriteFileIn->pvApcRoutine,
                                      pZwWriteFileIn->pvApcContext,
                                      &IoBlock,
                                      pZwWriteFileIn->pvBuffer,
                                      pZwWriteFileIn->ulLength,
                                      &pZwWriteFileIn->liByteOffset,
                                      pZwWriteFileIn->pvKey );

                if (status == NO_ERROR)
                    RequestPacket->StatusBlock->Information = sizeof(ULONG);
                else
                    RequestPacket->StatusBlock->Information = 0;

            }
            break;

        case IOCTL_VIDEO_ZW_READ_FILE:
            {
                PNV_ZW_RW_FILE_IN pZwReadFileIn = (PNV_ZW_RW_FILE_IN)RequestPacket->OutputBuffer;
                IO_STATUS_BLOCK IoBlock;

                status = ZwReadFile( pZwReadFileIn->pvFileHandle,
                                     pZwReadFileIn->pvEvent,
                                     pZwReadFileIn->pvApcRoutine,
                                     pZwReadFileIn->pvApcContext,
                                     &IoBlock,
                                     pZwReadFileIn->pvBuffer,
                                     pZwReadFileIn->ulLength,
                                     &pZwReadFileIn->liByteOffset,
                                     pZwReadFileIn->pvKey );

                if (status == NO_ERROR)
                    RequestPacket->StatusBlock->Information = pZwReadFileIn->ulLength;
                else
                    RequestPacket->StatusBlock->Information = 0;

            }
            break;

        case IOCTL_VIDEO_ZW_CLOSE_HANDLE:
            {
                PVOID *phFile = RequestPacket->InputBuffer;
                status = ZwClose( *phFile );

                if (status == NO_ERROR)
                    RequestPacket->StatusBlock->Information = sizeof(ULONG);
                else
                    RequestPacket->StatusBlock->Information = 0;

            }
            break;

        case IOCTL_VIDEO_CONTROL_DEVICE:
            {
                PNV_ZW_DEVICE_IO_CONTROL_FILE pControlDevice = (PNV_ZW_DEVICE_IO_CONTROL_FILE)RequestPacket->InputBuffer;
                IO_STATUS_BLOCK IoBlock;

                status = ZwDeviceIoControlFile( pControlDevice->hDevice,
                                                NULL,
                                                NULL,
                                                NULL,
                                                &IoBlock,
                                                pControlDevice->ulIoControlCode,
                                                pControlDevice->pvInputBuffer,
                                                pControlDevice->ulInputBufferSize,
                                                RequestPacket->OutputBuffer,
                                                RequestPacket->OutputBufferLength );

                if( status == NO_ERROR )
                    RequestPacket->StatusBlock->Information = RequestPacket->OutputBufferLength;
                else
                    RequestPacket->StatusBlock->Information = 0ul;
            }
            break;

        case IOCTL_VIDEO_ZW_GET_FILE_IO_FUNCS:
            {
                PNV_ZW_FILE_IO_FUNCS pFuncs = (PNV_ZW_FILE_IO_FUNCS)RequestPacket->OutputBuffer;

                pFuncs->pvZwCreateFile = (LPNVZWCREATEFILEFUNCTION)ZwCreateFile;
                pFuncs->pvZwWriteFile = (LPNVZWWRITEFILEFUNCTION)ZwWriteFile;
                pFuncs->pvZwReadFile = (LPNVZWREADFILEFUNCTION)ZwReadFile;
                pFuncs->pvZwClose = (LPNVZWCLOSEFUNCTION)ZwClose;
                pFuncs->pvZwDeviceIoControlFile = (LPNVZWDEVICEIOCONTROLFILEFUNCTION)ZwDeviceIoControlFile;

                status = NO_ERROR;
                RequestPacket->StatusBlock->Information = sizeof(NV_ZW_FILE_IO_FUNCS);
            }
            break;

#else

        case IOCTL_VIDEO_ZW_CREATE_FILE:
            {
                VideoDebugPrint((0, "IOCTL_VIDEO_ZW_CREATE_FILE not enabled.\n"));
                VideoDebugPrint((0, "  miniport must be built with NV_ZW_FILE_IO_ENABLED defined \n"));
                status = ERROR_INVALID_FUNCTION;
                RequestPacket->StatusBlock->Information = 0;
            }
            break;

        case IOCTL_VIDEO_ZW_WRITE_FILE:
            {
                VideoDebugPrint((0, "IOCTL_VIDEO_ZW_WRITE_FILE not enabled.\n"));
                VideoDebugPrint((0, "  miniport must be built with NV_ZW_FILE_IO_ENABLED defined \n"));
                status = ERROR_INVALID_FUNCTION;
                RequestPacket->StatusBlock->Information = 0;
            }
            break;

        case IOCTL_VIDEO_ZW_READ_FILE:
            {
                VideoDebugPrint((0, "IOCTL_VIDEO_ZW_READ_FILE not enabled.\n"));
                VideoDebugPrint((0, "  miniport must be built with NV_ZW_FILE_IO_ENABLED defined \n"));
                status = ERROR_INVALID_FUNCTION;
                RequestPacket->StatusBlock->Information = 0;
            }
            break;

        case IOCTL_VIDEO_ZW_CLOSE_HANDLE:
            {
                VideoDebugPrint((0, "IOCTL_VIDEO_ZW_CLOSE_HANDLE not enabled.\n"));
                VideoDebugPrint((0, "  miniport must be built with NV_ZW_FILE_IO_ENABLED defined \n"));
                status = ERROR_INVALID_FUNCTION;
                RequestPacket->StatusBlock->Information = 0;
            }
            break;

        case IOCTL_VIDEO_CONTROL_DEVICE:
            {
                VideoDebugPrint((0, "IOCTL_VIDEO_CONTROL_DEVICE not enabled.\n"));
                VideoDebugPrint((0, "  miniport must be built with NV_ZW_FILE_IO_ENABLED defined \n"));
                status = ERROR_INVALID_FUNCTION;
                RequestPacket->StatusBlock->Information = 0;
            }
            break;

        case IOCTL_VIDEO_ZW_GET_FILE_IO_FUNCS:
            {
                VideoDebugPrint((0, "IOCTL_VIDEO_ZW_GET_FILE_IO_FUNCS not enabled.\n"));
                VideoDebugPrint((0, "  miniport must be built with NV_ZW_FILE_IO_ENABLED defined \n"));
                status = ERROR_INVALID_FUNCTION;
                RequestPacket->StatusBlock->Information = 0;
            }
            break;
#endif

#endif // (_WIN32_WINNT >= 0x0500)


      // Sets a Display value ( vga, fp or tv, and tv type: ntsc or pal) in the registry
      case IOCTL_VIDEO_SET_DISPLAY_DEVICE:
         {
            NV_CFGEX_SET_DISPLAY_TYPE_PARAMS sdtp;
            PSETGETDEVTYPE sgdt;
            BOOL  ValidParams;

            if(RequestPacket->InputBufferLength < sizeof(SETGETDEVTYPE)) {
               status = ERROR_INVALID_PARAMETER;
            }
            else {

               sgdt = (PSETGETDEVTYPE)RequestPacket->InputBuffer;
               VideoDebugPrint((2, "NVStartIO - SetDisplayDevice\n"));

               switch(sgdt->dwDeviceType) {
                  case 0:     // NV_DISPLAY_TYPE_VGA
                     sdtp.Type = DISPLAY_TYPE_MONITOR;
                     sdtp.TVType = 0;  // Play it safe
                     ValidParams = TRUE;
                     break;
                  case 2:     // NV_DISPLAY_TYPE_TV_NTSC
                     sdtp.Type = DISPLAY_TYPE_TV;
                     sdtp.TVType = sgdt->dwTvFormat;
                     ValidParams = TRUE;
                     break;
                  case 3:     // NV_DISPLAY_TYPE_TV_PAL
                     sdtp.Type = DISPLAY_TYPE_TV;
                     sdtp.TVType = sgdt->dwTvFormat;
                     ValidParams = TRUE;
                     break;
                  case 4:     // NV_DISPLAY_TYPE_FLAT_PANEL
                     sdtp.Type = DISPLAY_TYPE_FLAT_PANEL;
                     sdtp.TVType = 0;  // Play it safe
                     ValidParams = TRUE;
                     break;
                  default: // ?
                     ValidParams = FALSE;
                     break;
               }

               // - If all of the params are cool, attempt the settings
               if(ValidParams) {

                  VideoDebugPrint((1, "IOCTL_VIDEO_SET_DISPLAY_DEVICE: Type: %d, TVType: %d\n",sdtp.Type,sdtp.TVType));

                  // - Go set the values in the registry
                  if(RmConfigSetExKernel(hwDeviceExtension->DeviceReference,
                                         NV_CFGEX_SET_DISPLAY_TYPE,
                                         &sdtp,
                                         sizeof(NV_CFGEX_SET_DISPLAY_TYPE_PARAMS))) {

                     // The Rm call seems to have succeded
                     status = NO_ERROR;
                  }
                  else {

                     VideoDebugPrint((1, "IOCTL_VIDEO_SET_DISPLAY_DEVICE: RmConfigSetExKernel failed\n"));
                     status = ERROR_INVALID_PARAMETER;   // This is not correct, but the other error codes are
                                                         // just as incorrect!
                  }

                  // - Nothing to return to the caller
                  RequestPacket->StatusBlock->Information = 0;

               }
               else {

                  VideoDebugPrint((1, "IOCTL_VIDEO_SET_DISPLAY_DEVICE: The parameters were in error.\n"));
                  // - Wrap up IOCTL control info for failure
                  status = ERROR_INVALID_PARAMETER;
               }
            }
         }
         break;


      // Gets a Display value ( vga, fp or tv, and tv type: ntsc or pal) from the registry
      case IOCTL_VIDEO_GET_DISPLAY_DEVICE:
         {

            NV_CFGEX_GET_DISPLAY_TYPE_PARAMS sdtp;
            PSETGETDEVTYPE sgdt;
            BOOL  ValidRegData;

            if(RequestPacket->OutputBufferLength < sizeof(SETGETDEVTYPE)) {
               status = ERROR_INSUFFICIENT_BUFFER;
            }
            else {

               VideoDebugPrint((2, "NVStartIO - GetDisplayDevice\n"));
               sgdt = (PSETGETDEVTYPE)RequestPacket->OutputBuffer;
               status = NO_ERROR;

               // - Go get the values in the registry
               ValidRegData = FALSE;
               if(RmConfigGetExKernel(hwDeviceExtension->DeviceReference,
                                      NV_CFGEX_GET_DISPLAY_TYPE,
                                      &sdtp,
                                      sizeof(NV_CFGEX_GET_DISPLAY_TYPE_PARAMS))) {
                  ValidRegData = TRUE;
               }

               if(ValidRegData) {

                  // - Update the values in the passed in buffer
                  switch(sdtp.Type) {

                     case DISPLAY_TYPE_MONITOR:
                        sgdt->dwDeviceType = 0; // NV_DISPLAY_TYPE_VGA
                        sgdt->dwTvFormat = 0;
                        sgdt->dwSuccessFlag = TRUE;
                        break;

                     case DISPLAY_TYPE_TV:
                        switch(sdtp.TVType) {
                           case 0: // NV_REG_TVFORMAT_NTSCM
                           case 1: // NV_REG_TVFORMAT_NTSCJ
                              sgdt->dwDeviceType = 2; // NV_DISPLAY_TYPE_TV_NTSC
                              sgdt->dwTvFormat = sdtp.TVType;
                              sgdt->dwSuccessFlag = TRUE;
                              break;
                           case 2: // NV_REG_TVFORMAT_PALM
                           case 3: // NV_REG_TVFORMAT_PALB
                           case 4: // NV_REG_TVFORMAT_PALN
                           case 5: // NV_REG_TVFORMAT_PALNC
                              sgdt->dwDeviceType = 3; // NV_DISPLAY_TYPE_TV_PAL
                              sgdt->dwTvFormat = sdtp.TVType;
                              sgdt->dwSuccessFlag = TRUE;
                              break;
                           default:
                              sgdt->dwDeviceType = 2; // Make this NTSC just in case someone does not pay attention
                              sgdt->dwTvFormat = 0;
                              sgdt->dwSuccessFlag = FALSE;
                              break;
                        }
                        break;

                     case DISPLAY_TYPE_FLAT_PANEL: // NV_DISPLAY_TYPE_FLAT_PANEL
                        sgdt->dwDeviceType = 4;
                        sgdt->dwTvFormat = 0;
                        sgdt->dwSuccessFlag = TRUE;
                        break;


                     default:
                        sgdt->dwDeviceType = 0; // NV_DISPLAY_TYPE_VGA
                        sgdt->dwTvFormat = 0;
                        sgdt->dwSuccessFlag = TRUE;
                        break;
                  }
               }
               else {   // Data in reg not avail, so return faliure
                     sgdt->dwDeviceType = 0;
                     sgdt->dwTvFormat = 0;
                     sgdt->dwSuccessFlag = FALSE;
               }

               // - Wrap up IOCTL control info
               RequestPacket->StatusBlock->Information = sizeof(SETGETDEVTYPE);
               status = NO_ERROR;

            }
         }
         break;

     case IOCTL_VIDEO_GET_TV_ENCODER_TYPE:
         {
            NV_CFGEX_GET_TV_ENCODER_MULTI_PARAMS TVInfo;
            VideoDebugPrint((2, "NVStartIO -  IOCTL_VIDEO_GET_TV_ENCODER_TYPE\n"));
            if ( RequestPacket->OutputBufferLength < sizeof(NV_CFGEX_GET_TV_ENCODER_MULTI_PARAMS) )
            {
                status = ERROR_INVALID_PARAMETER;
            }
            else
            {
                TVInfo.Head        = 0;
                TVInfo.EncoderID   = hwDeviceExtension->ulTVEncoderID;
                TVInfo.EncoderType = hwDeviceExtension->ulTVEncoderType;

                // We have a valid blob from the resman, so extract the encoder info
                VideoDebugPrint((2,"From RM: TVEncoderType = 0x%x, TVEncoderID = 0x%x\n",TVInfo.EncoderType,TVInfo.EncoderID));
                *((NV_CFGEX_GET_TV_ENCODER_MULTI_PARAMS*)RequestPacket->OutputBuffer) = TVInfo;
                RequestPacket->StatusBlock->Information = sizeof(NV_CFGEX_GET_TV_ENCODER_MULTI_PARAMS);
                status = NO_ERROR;
            }
         }
         break;

      // - NV_ESC_VALIDATE_DEVICE_MODE path into the miniport
      case IOCTL_VALIDATE_DEVICE_MODE:
         {

            BOOL  ValidParams;
            VALMODEXTR *vmx;

            if( (RequestPacket->InputBufferLength < sizeof(VALMODEXTR)) ||
                (RequestPacket->OutputBufferLength < sizeof(ULONG)) ){
               status = ERROR_INVALID_PARAMETER;
            }
            else {

               vmx = (VALMODEXTR *)RequestPacket->InputBuffer;
               VideoDebugPrint((1, "NVStartIO - ValidateDeviceMode\n"));
               VideoDebugPrint((1, "NVStartIO -   dwHRes = %ld\n",vmx->dwHRes));
               VideoDebugPrint((1, "NVStartIO -   dwVRes = %ld\n",vmx->dwVRes));
               VideoDebugPrint((1, "NVStartIO -   dwBpp = %ld\n",vmx->dwBpp));
               VideoDebugPrint((1, "NVStartIO -   dwRefresh = %ld\n",vmx->dwRefresh));
               VideoDebugPrint((1, "NVStartIO -   dwDeviceType = %ld\n",vmx->dwDeviceType));

               ValidParams = TRUE;

               // - If all of the params are cool, attempt the validate mode function
               if(ValidParams) {

                  // - Go set the values in the device context, no error checking for now...
                  if(NVValidateDeviceMode(hwDeviceExtension, vmx)) {

                     // - That mode is VALID for this device
                     *(ULONG *)RequestPacket->OutputBuffer = TRUE;
                  }
                  else {

                     // - That mode is NOT VALID for this device
                     *(ULONG *)RequestPacket->OutputBuffer = FALSE;
                  }

                  RequestPacket->StatusBlock->Information = sizeof(ULONG);
                  status = NO_ERROR;

               }
               else {
                  RequestPacket->StatusBlock->Information = 0;
                  status = ERROR_INVALID_PARAMETER;
               }
            }
         }
         break;


      // - This next one seems like a lot of work for nothing, but is keeps it comp with 9x for now
      case IOCTL_VIDEO_IS_NVIDIA_BOARD_AND_DRIVER:
         {

            if(RequestPacket->OutputBufferLength < sizeof(ULONG)) {
               status = ERROR_INVALID_PARAMETER;
            }
            else {

               VideoDebugPrint((2, "NVStartIO - IOCTL_VIDEO_IS_NVIDIA_BOARD_AND_DRIVER\n"));

               // - We use the device reference + 1
               *(ULONG *)RequestPacket->OutputBuffer = hwDeviceExtension->DeviceReference + 1;
               RequestPacket->StatusBlock->Information = sizeof(ULONG);
               status = NO_ERROR;

            }
         }
         break;

      // - This IOCTL return the current gamma cache clut stored in the device extension
      case IOCTL_VIDEO_GET_GAMMA_CLUT:
         {

            if(RequestPacket->OutputBufferLength != MAX_CLUT_SIZE) {
               status = ERROR_INVALID_PARAMETER;
            }
            else {

               VideoDebugPrint((2, "NVStartIO - IOCTL_VIDEO_GET_GAMMA_CLUT\n"));

               status = NVGetColorLookup(hwDeviceExtension,
                                         (PVIDEO_CLUT)RequestPacket->OutputBuffer,
                                         RequestPacket->OutputBufferLength );

               RequestPacket->StatusBlock->Information = MAX_CLUT_SIZE;

            }
         }
         break;
         
        
        //**************************************************************************
        // Save/Restore the Gamma values for the mode switching.
        //**************************************************************************
        case IOCTL_VIDEO_SAVE_GAMMA_VALUES:
            if(RequestPacket->InputBufferLength != sizeof(hwDeviceExtension->GammaCacheData)) 
            {
                status = ERROR_INVALID_PARAMETER;
            }
            else 
            {
                ULONG i;
                ULONG * pGammaCache;
                ULONG * pCallerBuffer;
                
                pGammaCache = (ULONG *) &hwDeviceExtension->GammaCache.LookupTable->RgbArray;
                pCallerBuffer = (ULONG *) RequestPacket->InputBuffer;
                
                for(i = 0; i < sizeof(hwDeviceExtension->GammaCacheData) / sizeof(ULONG); i++)
                {
                    *pGammaCache++ = *pCallerBuffer++;
                }
                status = NO_ERROR;
            }
            break;
            
        case IOCTL_VIDEO_RESTORE_GAMMA_VALUES:
            if(RequestPacket->OutputBufferLength != sizeof(hwDeviceExtension->GammaCacheData)) 
            {
                status = ERROR_INVALID_PARAMETER;
            }
            else 
            {
                ULONG i;
                ULONG * pGammaCache;
                ULONG * pCallerBuffer;
                
                pGammaCache = (ULONG *) &hwDeviceExtension->GammaCache.LookupTable->RgbArray;
                pCallerBuffer = (ULONG *) RequestPacket->OutputBuffer;
                
                for(i = 0; i < sizeof(hwDeviceExtension->GammaCacheData) / sizeof(ULONG); i++)
                {
                    *pCallerBuffer++ = *pGammaCache++;
                }
                status = NO_ERROR;
                RequestPacket->StatusBlock->Information =sizeof(hwDeviceExtension->GammaCacheData);
            }
            break;
        //**************************************************************************
        // CONVERT_RING3_HANDLE
        // Converts a ring3 handle to a ring0 handle
        //**************************************************************************

        case IOCTL_CONVERT_RING3_HANDLE:
            {
            HANDLE** input = (HANDLE**) RequestPacket->InputBuffer;

            status = NVObReferenceObjectByHandle(*(input[0]), input[1]);
            }
            break;

        //**************************************************************************
        // MAP_PHYS_ADDR
        //**************************************************************************

        case IOCTL_VIDEO_MAP_PHYS_ADDR:
            {
            NV_SYSMEM_SHARE_MEMORY  *pShareMemory, *pShareMemoryInformation;
            NV_CFGEX_AGP_LINEAR_BASE_PARAMS params;
            U032 physicalAGPBase;

            VideoDebugPrint((2, "W32StartIO - MapPhysAddr\n"));

            if ( (RequestPacket->OutputBufferLength < sizeof(NV_SYSMEM_SHARE_MEMORY) ||
                 (RequestPacket->InputBufferLength < sizeof(NV_SYSMEM_SHARE_MEMORY)) ))
                {
                VideoDebugPrint((0, "IOCTL_VIDEO_MAP_PHYS_ADDR - ERROR_INSUFFICIENT_BUFFER\n"));
                status = ERROR_INSUFFICIENT_BUFFER;
                break;
                }

            pShareMemory = (NV_SYSMEM_SHARE_MEMORY *) RequestPacket->InputBuffer;
            RequestPacket->StatusBlock->Information =
                                        sizeof(NV_SYSMEM_SHARE_MEMORY);

            sharedViewSize = pShareMemory->byteLength;

            //**********************************************************************
            // NOTE: we are ignoring ViewOffset
            //**********************************************************************

            shareAddress.HighPart = 0x00000000;
            PHYSICAL_ADDRESS_VALUE(shareAddress) = pShareMemory->physicalAddress;
            pShareMemoryInformation = (NV_SYSMEM_SHARE_MEMORY *) RequestPacket->OutputBuffer;

            //**********************************************************************
            // Map the physical address into a non-cached kernel mode address.
            //**********************************************************************
            pShareMemoryInformation->ddVirtualAddress =
                MmMapIoSpace(shareAddress,
                            (size_t) sharedViewSize,
                            (MmNonCached | MmWriteCombined));

            if (pShareMemoryInformation->ddVirtualAddress == NULL)
                {
                status = ERROR_INVALID_PARAMETER;
                break;
                }

            //**********************************************************************
            // Set the RM's AGP linear base if a mapping doesn't exist already. We
            // don't expect this to ever been needed, since the RM should've been
            // able to create their own mapping to the AGP phys base. Just in case,
            // set it up if it's not already.
            //**********************************************************************
            RmConfigGetKernel(hwDeviceExtension->DeviceReference,
                              NV_CFG_AGP_PHYS_BASE, &physicalAGPBase);

            RmConfigGetExKernel(hwDeviceExtension->DeviceReference,
                                NV_CFGEX_AGP_LINEAR_BASE,
                                &params,
                                sizeof(NV_CFGEX_AGP_LINEAR_BASE_PARAMS));
            if ((params.linaddr == NULL) &&
                (physicalAGPBase == pShareMemory->physicalAddress)) {
                
                // update the RM's linear base
                params.linaddr = pShareMemoryInformation->ddVirtualAddress;
                RmConfigSetExKernel(hwDeviceExtension->DeviceReference,
                                    NV_CFGEX_AGP_LINEAR_BASE,
                                    &params,
                                    sizeof(NV_CFGEX_AGP_LINEAR_BASE_PARAMS));
            }

            //**********************************************************************
            // Returned amount of mapped memory to specific process
            //**********************************************************************

            pShareMemoryInformation->byteLength = sharedViewSize;

            status = NO_ERROR;
            }
            break;

        //**************************************************************************
        // UNMAP_PHYS_ADDR
        //**************************************************************************

        case IOCTL_VIDEO_UNMAP_PHYS_ADDR:
            {
            NV_SYSMEM_SHARE_MEMORY  *pShareMemory;

            VideoDebugPrint((2, "W32StartIO - UnmapPhysAddr\n"));

            if (RequestPacket->InputBufferLength < sizeof(NV_SYSMEM_SHARE_MEMORY))
                {
                VideoDebugPrint((0, "IOCTL_VIDEO_MAP_PHYS_ADDR - ERROR_INSUFFICIENT_BUFFER\n"));
                status = ERROR_INSUFFICIENT_BUFFER;
                break;
                }

            pShareMemory = (NV_SYSMEM_SHARE_MEMORY *) RequestPacket->InputBuffer;
            RequestPacket->StatusBlock->Information =
                                        sizeof(NV_SYSMEM_SHARE_MEMORY);

            sharedViewSize = pShareMemory->byteLength;

            //**********************************************************************
            // Unmap the virtual address.
            //**********************************************************************
            MmUnmapIoSpace(pShareMemory->ddVirtualAddress,
                        (size_t) sharedViewSize);

            pShareMemory->ddVirtualAddress = NULL;
            status = NO_ERROR;
            }
            break;

        case IOCTL_VIDEO_TIMER_UPDATE_XY_FOR_NV10_CURSOR:
            {
                ULONG * pulXY;

                if(RequestPacket->InputBufferLength != sizeof(ULONG))
                {

                    VideoDebugPrint((0, "IOCTL_VIDEO_TIMER_UPDATE_XY_FOR_NV10_CURSOR - ERROR_INSUFFICIENT_BUFFER\n"));
                    status = ERROR_INSUFFICIENT_BUFFER;
                    break;
                }
                else
                {
                    pulXY = (ULONG *)RequestPacket->InputBuffer;
                    hwDeviceExtension->NvCursorInfo.lCursorX= (LONG)( (SHORT)(*pulXY & 0xFFFF));
                    hwDeviceExtension->NvCursorInfo.lCursorY= (LONG)( (SHORT)(*pulXY >> 16));
                    status = NO_ERROR;
                    break;
                }
            }
            break;

        case IOCTL_VIDEO_START_TIMER_FOR_NV10_CURSOR:
            {
                NV_CURSOR_INFO * pNvCursorInfo;

                if(RequestPacket->InputBufferLength != sizeof(NV_CURSOR_INFO))
                {

                    VideoDebugPrint((0, "IOCTL_VIDEO_START_TIMER_FOR_NV10_CURSOR - ERROR_INSUFFICIENT_BUFFER\n"));
                    status = ERROR_INSUFFICIENT_BUFFER;
                    break;
                }
                else
                {
                    pNvCursorInfo = (NV_CURSOR_INFO * )RequestPacket->InputBuffer;

                    hwDeviceExtension->NvCursorInfo.lCursorX = pNvCursorInfo->lCursorX;
                    hwDeviceExtension->NvCursorInfo.lCursorY = pNvCursorInfo->lCursorY;
                    hwDeviceExtension->NvCursorInfo.lCursorWidth = pNvCursorInfo->lCursorWidth;
                    hwDeviceExtension->NvCursorInfo.lCursorHeight = pNvCursorInfo->lCursorHeight;
                    hwDeviceExtension->NvCursorInfo.lScreenStride = pNvCursorInfo->lScreenStride;
                    hwDeviceExtension->NvCursorInfo.lScreenPixelDepth = pNvCursorInfo->lScreenPixelDepth;
                    hwDeviceExtension->NvCursorInfo.ulHwCursorWidth = pNvCursorInfo->ulHwCursorWidth;
                    hwDeviceExtension->NvCursorInfo.ulOriginalImageMemOffset = pNvCursorInfo->ulOriginalImageMemOffset;
                    hwDeviceExtension->NvCursorInfo.ulCursorMemOffset = pNvCursorInfo->ulCursorMemOffset;

                    VideoPortStartTimer(hwDeviceExtension);
                    status = NO_ERROR;
                }
            }
            break;

        case IOCTL_VIDEO_STOP_TIMER_FOR_NV10_CURSOR:
            {
                VideoPortStopTimer(hwDeviceExtension);
                status = NO_ERROR;
            }
            break;


#if !defined(_WIN64)
        case IOCTL_VIDEO_STEREO_GLASSES_SWITCH:
            {
                if (   (NULL!=RequestPacket->InputBuffer)
                    && (RequestPacket->InputBufferLength==sizeof(NV_CFG_STEREO_PARAMS)) )
                {
                    NV_CFG_STEREO_PARAMS  StereoParamsOld;

                    // this interface is used for workstation opengl api stewreo so far !
                    ASSERT( ((NV_CFG_STEREO_PARAMS*)RequestPacket->InputBuffer)->Flags & STEREOCFG_WS_OGL_API_STEREO);

                    // NOT IA64 SAVE !! because we cast a pointer to U032 !!
                    if( RmConfigSetKernel( hwDeviceExtension->DeviceReference,
                                           NV_CFG_STEREO_CONFIG,
                                           (U032)RequestPacket->InputBuffer,
                                           (U032*)&StereoParamsOld)
                      )
                    {
                        status = NO_ERROR;
                    }
                }
                else
                {
                    VideoDebugPrint((0, "IOCTL_VIDEO_STEREO_GLASSES_SWITCH failed because of invalid RequestPacket->InputBuffer data!"));
                }

            }
            break;
#endif

        case IOCTL_VIDEO_QUERY_NUM_OF_DEVICE_OBJECTS:  // should be called only from the multi dev layer of the NT4 display driver!
            {
                PVOID pvDevObj;
                ULONG ulNumDevObj;

                if (RequestPacket->OutputBufferLength < sizeof(QUERY_NUM_OF_DEVICE_OBJECTS) ||
                    RequestPacket->InputBufferLength < sizeof(QUERY_NUM_OF_DEVICE_OBJECTS))
                {
                  status = ERROR_INSUFFICIENT_BUFFER;
                  break;
                }
                pvDevObj = ((QUERY_NUM_OF_DEVICE_OBJECTS *)(RequestPacket->InputBuffer))->pvDevObj;  // need hDriver in InputBuffer
                ASSERT(pvDevObj);
                if (pvDevObj)
                {
                    ((QUERY_NUM_OF_DEVICE_OBJECTS *)(RequestPacket->OutputBuffer))->ulNumDevObj = ulQueryNumDevices(pvDevObj);

                    status = NO_ERROR;
                    RequestPacket->StatusBlock->Information = sizeof(QUERY_NUM_OF_DEVICE_OBJECTS);
                }
            }  
            break;

        case IOCTL_VIDEO_GET_DEVICE_OBJECTS:  // should be called only from the multi dev layer of the NT4 display driver!
            {
                ULONG ulNumDevObj;
                PVOID pvDevObj;
                PVOID *ppvOutDevObj;

                if (RequestPacket->InputBufferLength < (sizeof (GET_DEVICE_OBJECTS)))
                {
                    status = ERROR_INSUFFICIENT_BUFFER;
                    break;
                }
                pvDevObj    = ((GET_DEVICE_OBJECTS *)(RequestPacket->InputBuffer))->pvDevObj;     // need hDriver in InputBuffer
                ulNumDevObj = ((GET_DEVICE_OBJECTS *)(RequestPacket->InputBuffer))->ulNumDevObj;  


                if (RequestPacket->OutputBufferLength < (sizeof (GET_DEVICE_OBJECTS) + (ulNumDevObj -1) * sizeof(PVOID) )) 
                {
                    status = ERROR_INSUFFICIENT_BUFFER;
                    break;
                }

                ppvOutDevObj = &((GET_DEVICE_OBJECTS *)(RequestPacket->OutputBuffer))->apvDevObj[0];

                if (!bGetDevices(pvDevObj, ppvOutDevObj))
                {
                    status = ERROR_INSUFFICIENT_BUFFER;
                }
                else
                {
                    RequestPacket->StatusBlock->Information = (sizeof (GET_DEVICE_OBJECTS) + (ulNumDevObj -1) * sizeof(PVOID) );
                    status = NO_ERROR;
                }
            }
            break;

    default:

        //**************************************************************************
        // if we get here, an invalid IoControlCode was specified.
        //**************************************************************************

        VideoDebugPrint((1, "Fell through vga startIO routine - invalid command\n"));

        status = ERROR_INVALID_FUNCTION;

        break;
        }

        //  Zero out the Information in StatusBlock when Error occurs.
        
        if(status != NO_ERROR)
        {
            RequestPacket->StatusBlock->Information = 0;
        }

    RequestPacket->StatusBlock->Status = status;
#if DBG
    VideoDebugPrint((2, "W32StartIO Exit  - %08.8x\n", RequestPacket->IoControlCode));
#endif
    return TRUE;

    } // NVStartIO()

//******************************************************************************
//
// Function: NVInterrupt()
//
// Routine Description:
//   This function services interrupts generated by the device.
//
// Arguments:
//
//    HwDeviceExtension - Pointer to the miniport driver's device extension.
//
// Return Value:
//   TRUE -- if this device caused the interrupt and when the interrupt is
//           serviced and the interrupt reset
//
//   FALSE -- if this device was not responsible for the interrupt
//
//******************************************************************************


BOOLEAN NVInterrupt(
    PVOID HwDeviceExtension
)
{
        PHW_DEVICE_EXTENSION hwDeviceExtension = (PHW_DEVICE_EXTENSION)HwDeviceExtension;
        BOOLEAN serviced;

        // Only service the interrupt when the power of adapter is on
        if(hwDeviceExtension->curAdapterPowerState != VideoPowerOn)
            return(FALSE);

        serviced = (BOOLEAN) RmIsr(hwDeviceExtension->DeviceReference);

        return serviced;
}

//******************************************************************************
//
// private routines
//
//******************************************************************************


//******************************************************************************
//
// Function: NVSetColorLookup()
//
// Routine Description:
//
//     This routine sets a specified portion of the DAC color lookup table
//     settings.
//
// Arguments:
//
//     HwDeviceExtension - Pointer to the miniport driver's device extension.
//
//     ClutBufferSize - Length of the input buffer supplied by the user.
//
//     ClutBuffer - Pointer to the structure containing the color lookup table.
//
// Return Value:
//
//     NO_ERROR - information returned successfully
//
//     ERROR_INSUFFICIENT_BUFFER - input buffer not large enough for input data.
//
//     ERROR_INVALID_PARAMETER - invalid clut size.
//
//******************************************************************************


VP_STATUS
NVSetColorLookup(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PVIDEO_CLUT ClutBuffer,
    ULONG ClutBufferSize
    )


    {
    USHORT i;
    ULONG OutData;

    //**************************************************************************
    // Check if the size of the data in the input buffer is large enough.
    //**************************************************************************

    if ( (ClutBufferSize < sizeof(VIDEO_CLUT) - sizeof(ULONG)) ||
         (ClutBufferSize < sizeof(VIDEO_CLUT) +
                     (sizeof(ULONG) * (ClutBuffer->NumEntries - 1)) ) ) {

        return ERROR_INSUFFICIENT_BUFFER;

    }

    //**************************************************************************
    // Check to see if the parameters are valid.
    //**************************************************************************

    if ( (ClutBuffer->NumEntries == 0) ||
         (ClutBuffer->FirstEntry > VIDEO_MAX_COLOR_REGISTER) ||
         (ClutBuffer->FirstEntry + ClutBuffer->NumEntries >
             VIDEO_MAX_COLOR_REGISTER + 1) ) {

        return ERROR_INVALID_PARAMETER;

    }

    //**************************************************************************
    // Set palette for either.
    //**************************************************************************
    NV_SetColorLookup(HwDeviceExtension,ClutBuffer,ClutBufferSize);

    return NO_ERROR;

    } // end NVSetColorLookup()



//******************************************************************************
//
// Function: NVGetColorLookup()
//
// Routine Description:
//
//     This routine gets a specified portion of the DAC color lookup table
//     settings.
//
// Arguments:
//
//     HwDeviceExtension - Pointer to the miniport driver's device extension.
//
//     ClutBufferSize - Length of the output buffer supplied by the user.
//
//     ClutBuffer - Pointer to the buffer to store the color lookup table.
//
// Return Value:
//
//     NO_ERROR - information returned successfully
//
//     ERROR_INSUFFICIENT_BUFFER - output buffer not large enough for clut data.
//
//     ERROR_INVALID_PARAMETER - invalid clut size.
//
//******************************************************************************

VP_STATUS
NVGetColorLookup(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PVIDEO_CLUT ClutBuffer,
    ULONG ClutBufferSize
    )

    {
      PHW_DEVICE_EXTENSION hwDeviceExtension = (PHW_DEVICE_EXTENSION)HwDeviceExtension;
      PVIDEO_CLUTDATA   pvcdItemSrc,pvcdItemDst;
      PVIDEO_CLUT       srcclut,dstclut;
      ULONG index,item,NumEntries,FirstEntry;

      //**************************************************************************
      // Get data from gamma cache.
      //**************************************************************************

      // - Make things nice to look at
                pvcdItemSrc = &hwDeviceExtension->GammaCache.LookupTable->RgbArray;
                pvcdItemDst = &ClutBuffer->LookupTable->RgbArray;
      srcclut = &hwDeviceExtension->GammaCache;
      dstclut = ClutBuffer;

      // - Copy over the clut specs
      dstclut->NumEntries = srcclut->NumEntries;
      dstclut->FirstEntry = srcclut->FirstEntry;

      // - Set the limits on the item copy
      NumEntries = srcclut->NumEntries;
      FirstEntry = srcclut->FirstEntry;

      // - Copy over the gamma data
      for(item = FirstEntry; item < NumEntries; item++) {
           index = item + FirstEntry;
           pvcdItemDst[index].Blue   = pvcdItemSrc[index].Blue;
           pvcdItemDst[index].Green  = pvcdItemSrc[index].Green;
           pvcdItemDst[index].Red    = pvcdItemSrc[index].Red;
           pvcdItemDst[index].Unused = pvcdItemSrc[index].Unused;
      }

    return NO_ERROR;

    } // end NVGetColorLookup()



//******************************************************************************
//
// Function: NVRegistryCallback()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************


VP_STATUS
NVRegistryCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    )


    {
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    SHORT shortvalue;
    PUCHAR SrcStrPtr;
    SHORT *DestPtr;

    //**********************************************************************
    // If the destination ptr is null, we don't care about the actual value, just
    // the fact that the Value name was found in the registry.
    //**********************************************************************
    if (Context == NULL)
        {
            return(NO_ERROR);
        }

    //**********************************************************************
    // Deals with data if size > 4 bytes
    // If > 4 bytes, we'll treat it as Unicode string data  (MODE ENTRIES)
    // and not dword registry switches
    //**********************************************************************

    if (ValueLength > sizeof(ULONG))
        {
        //*******************************************************************
        // Copy string data into our temporary buffer registry_data[]
        // Since this callback can be called MANY times per each
        // VideoPortGetRegistry call, we need to manually keep track of where
        // to store the incoming data, since the Context ptr is NOT
        // automatically incremented.  It's always equal to the
        // base address of registry_data.  That is, the RegistryDataOffset value
        // is incremented each time we read a short value from the registry,
        // so that the next time this callback gets called, we'll know where
        // we left off.
        //*******************************************************************

        DestPtr = (SHORT*)((PUCHAR)Context + hwDeviceExtension->RegistryDataOffset);
        SrcStrPtr = (PUCHAR)(ValueData);

        //**********************************************************************
        // Reading mode data numbers from the current mode entry line.
        // This registry callback will continue getting called for each line
        // until the last line is encountered.
        //**********************************************************************

        while (SrcStrPtr < (PUCHAR)ValueData + ValueLength - 2)
            {

            shortvalue = GetNextWordFromRegistry(hwDeviceExtension,&SrcStrPtr);
            *DestPtr = shortvalue;

            //******************************************************************
            // Need to manually keep track of where we need to store the numbers
            // (short values) the next time the registry call back gets called.
            //******************************************************************

            (hwDeviceExtension->RegistryDataOffset)+=2;
            DestPtr++;

            //******************************************************************
            // Debug Safety Check:
            // Make sure we don't go past the end of the registry_data table
            // Backup 30 words from end of table
            //******************************************************************

            if (hwDeviceExtension->RegistryDataOffset > ( (MAX_STRING_REGISTRY_DATA_WORDS*2) - (30*2))  )
                {
                VideoDebugPrint((0, "!!!                                             !!!\n"));
                VideoDebugPrint((0, "!!! ERROR: ERROR ERROR ERROR ERROR ERROR ERROR  !!!\n"));
                VideoDebugPrint((0, "!!! ERROR: Went past end of registry_data table !!!\n"));
                VideoDebugPrint((0, "!!! ERROR: Went past end of registry_data table !!!\n"));
                VideoDebugPrint((0, "!!! ERROR: Went past end of registry_data table !!!\n"));
                VideoDebugPrint((0, "!!! ERROR: Went past end of registry_data table !!!\n"));
                VideoDebugPrint((0, "!!! ERROR: Went past end of registry_data table !!!\n"));
                VideoDebugPrint((0, "!!! ERROR: ERROR ERROR ERROR ERROR ERROR ERROR  !!!\n"));
                VideoDebugPrint((0, "!!!                                             !!!\n"));
                }

            }



        return(NO_ERROR);


        }

    //*************************************************************************
    // Read dword entries
    //*************************************************************************

    memcpy((PUCHAR) Context, (PUCHAR) ValueData, (size_t) ValueLength);
    return(NO_ERROR);
    } // end NVRegistryCallback()



//******************************************************************************
//
// Function: NVReadLocalRegistryCallback()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************


VP_STATUS
NVReadRegistrySwitchesCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    )


    {
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    SHORT shortvalue;
    PUCHAR SrcStrPtr;
    SHORT *DestPtr;

    //**********************************************************************
    // If the destination ptr is null, we don't care about the actual value, just
    // the fact that the Value name was found in the registry.
    //**********************************************************************
    if (Context == NULL)
        {
            return(NO_ERROR);
        }


    //*************************************************************************
    // Copy ValueLength bytes
    //*************************************************************************

    memcpy((PUCHAR) Context, (PUCHAR) ValueData, ValueLength);
    return(NO_ERROR);
    } // end NVReadLocalRegistryCallback()

//******************************************************************************
//
// Function: NVReadRegistryTwinViewInfoCallback()
//
// Routine Description:     Used to read the "TwinViewInfo" registry entry.
//                          Do not use it for any other purpose.
//
// Arguments:
//
// Return Value:
//
//******************************************************************************


VP_STATUS
NVReadRegistryTwinViewInfoCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    )


    {
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    SHORT shortvalue;
    PUCHAR SrcStrPtr;
    SHORT *DestPtr;

    //**********************************************************************
    // If the destination ptr is null, we don't care about the actual value, just
    // the fact that the Value name was found in the registry.
    //**********************************************************************
    if (Context == NULL)
        {
            return(NO_ERROR);
        }

    // In rare pathlogical condidtions, it is possible for the disk copy of the "TwinViewInfo" structure to be different
    // than the incore structure. This happens when for example the structure gets modified in the source tree but
    // there is a leftover disk entry from a previous version of driver.
    if (ValueLength != sizeof(NVTWINVIEWDATA))
    {
        VideoDebugPrint((1, "Oops!TwinViewCallback(): size mismatch Diskcopy size: %d, struct size: %d\n",
                         ValueLength, sizeof(NVTWINVIEWDATA)));
        return(ERROR_INVALID_PARAMETER);
    }


    //*************************************************************************
    // Copy ValueLength bytes
    //*************************************************************************

    memcpy((PUCHAR) Context, (PUCHAR) ValueData, ValueLength);
    return(NO_ERROR);
    } // end NVReadLocalRegistryCallback()

//******************************************************************************
//
// Function: NVReadRegistrySaveSettingsCallback()
//
// Routine Description:     Used to read the "SaveSettings" registry entry.
//                          Do not use it for any other purpose.
//
// Arguments:
//
// Return Value:
//
//******************************************************************************


VP_STATUS
NVReadRegistrySaveSettingsCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    )


    {
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    SHORT shortvalue;
    PUCHAR SrcStrPtr;
    SHORT *DestPtr;
    ULONG ulCurrentTwinViewInfoSize;
    ULONG ulExpectedDataSize;

    //**********************************************************************
    // If the destination ptr is null, we don't care about the actual value, just
    // the fact that the Value name was found in the registry.
    //**********************************************************************
    if (Context == NULL)
        {
            return(NO_ERROR);
        }

    ulCurrentTwinViewInfoSize = sizeof(NVTWINVIEWDATA);
    ulExpectedDataSize = ulCurrentTwinViewInfoSize * NUM_ENTRIES_SAVE_SETTINGS;

    // In rare pathlogical condidtions, it is possible for a data mismatch. For example, the user could have manually
    // copied the driver binaries without doing a full driver installation. A full driver kit installation will blow
    // away any existing "SaveSettings" registry entry.
    if (ValueLength != ulExpectedDataSize)
    {
        VideoDebugPrint((1, "Oops!SaveSettingsCallback(): size mismatch Diskcopy size: %d, expected size: %d\n",
                         ValueLength, ulExpectedDataSize));
        return(ERROR_INVALID_PARAMETER);
    }

    // Sanity check. Make sure sufficient array was set aside to read in the SaveSettings.
    if (ValueLength > sizeof(hwDeviceExtension->SaveSettings))
    {
        VideoDebugPrint((1, "Oops!SaveSettingsCallback(): not enough array size in hwdevextension: size: %d, expected size: %d\n",
                         ValueLength, sizeof(hwDeviceExtension->SaveSettings)));
        return(ERROR_INVALID_PARAMETER);
    }

    // Sanity check. Should never happen
    if (ValueData == NULL)
    {
        VideoDebugPrint((1, "Oops!SaveSettingsCallback(): ValueData pointer is NULL\n"));
        return(ERROR_INVALID_PARAMETER);
    }


    //*************************************************************************
    // Copy ValueLength bytes
    //*************************************************************************

    memcpy((PUCHAR) Context, (PUCHAR) ValueData, ValueLength);
    return(NO_ERROR);
    } // end NVReadSaveSettingsCallback()

//******************************************************************************
//
// Function: NVReadRegistryCallbackBinaryValue()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************


VP_STATUS
NVReadRegistryBinaryCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    )


    {
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    PNV_REGISTRY_STRUCT pRegStruct;




    //**********************************************************************
    // If the destination ptr is null, we return error.
    // The user sent us a null output pointer.
    //**********************************************************************
    if (Context == NULL)
    {
        return(ERROR_INVALID_PARAMETER);
    }

    pRegStruct = (PNV_REGISTRY_STRUCT)Context;

    // Check to see if the user passed us a non-NULL buffer
    if (pRegStruct->keyVal == NULL)
    {
        // Tell him what is the expected buffer size.
        pRegStruct->keyValSize = ValueLength;
        return(NO_ERROR);
    }

    // Check to see if the user passed us enough of a buffer
    if (pRegStruct->keyValSize < ValueLength || pRegStruct->keyValSize == 0)
    {
        // The user did not send us enough buffer. Tell him what is the 
        // expected buffer size.
        pRegStruct->keyValSize = ValueLength;
        return(NO_ERROR);
    }

    //*************************************************************************
    // Copy ValueLength bytes
    //*************************************************************************

    memcpy((PUCHAR) pRegStruct->keyVal, (PUCHAR) ValueData, ValueLength);
    pRegStruct->keyValSize = ValueLength;
    return(NO_ERROR);
    } // end NVReadLocalRegistryCallback()



//******************************************************************************
//
// Function: NVReadMonitorTimingModeCallback()
//
// Routine Description: If there is a reg entry and it fits, copy it
//                      If it is to big, copy the string L"DMT" instead
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VP_STATUS
NVReadMonitorTimingModeCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    )


{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    SHORT shortvalue;
    PUCHAR SrcStrPtr;
    SHORT *DestPtr;

    //**********************************************************************
    // If the destination ptr is null, we don't care about the actual value, just
    // the fact that the Value name was found in the registry.
    //**********************************************************************
    if(Context == NULL) {
      return(NO_ERROR);
    }


    //*************************************************************************
    // Compare the value to the GTF and DMT strings
    //*************************************************************************

    if(!memcmp( (PUCHAR)L"GTF", (PUCHAR) ValueData, sizeof(L"GTF"))) {
        VideoDebugPrint((1, "GTF overide detected\n"));
        *(ULONG *)Context = TRUE;
        }
    else {
        *(ULONG *)Context = FALSE;
        }

    return(NO_ERROR);
}


//******************************************************************************
//
// Function:    NVSetBiosInfo(HwDeviceExtension)
//
// Routine Description:
//  Attempts to locate BIOS version string. First it searches for
//  a standard NV Bios Info block. If that is not present, it
//  assumes this is a Viper board and scans for a version
//  string.
//
// Arguments:
//
// Return Value:
//
//******************************************************************************
VOID NVSetBiosInfo(
    PHW_DEVICE_EXTENSION HwDeviceExtension
    )
{
    PUCHAR VersionStr = "Version ";
    PUCHAR pBios = HwDeviceExtension->pBios;
    USHORT  i, j, c;
    BOOLEAN Found = FALSE;
    WCHAR   wcNameStr[BIOS_STR_MAX] = L"<unavailable>";
    CHAR    NameStr[BIOS_STR_MAX];

    //********************************************************
    // Ask RM for NVidia BIOS revision.
    //********************************************************
    if (RmConfigGetKernel(HwDeviceExtension->DeviceReference, NV_CFG_BIOS_REVISION, &(HwDeviceExtension->BiosRevision))
        && HwDeviceExtension->BiosRevision)
        {

        //********************************************************
        // See if it's an Elsa BIOS: Elsa uses a similar data structure
        // as the NVIDIA BIOS, but the version string is 24-bits instead of 32.
        //********************************************************
        if (pBios != NULL)
        {
            strcpy(NameStr, "ELSA");
            for (i = 0; i < MAX_BIOS_SCAN; i++)
                if (Found = (VideoPortCompareMemory(NameStr, &pBios[i], strlen(NameStr)) == strlen(NameStr)))
                    break;
        }

        strcpy(NameStr, "Version ");

        // If Found, it's an ELSA BIOS

        if (Found)
        {
            wtoa((USHORT) ((HwDeviceExtension->BiosRevision & 0x00ff0000) >> 16), &(NameStr[strlen(NameStr)]), 16, 0);
            strcat(NameStr, ".");
            wtoa((USHORT) ((HwDeviceExtension->BiosRevision & 0x0000ff00) >> 8), &(NameStr[strlen(NameStr)]), 16, 2);
            strcat(NameStr, ".");
            wtoa((USHORT) ((HwDeviceExtension->BiosRevision & 0x000000ff)), &(NameStr[strlen(NameStr)]), 16, 2);
        }
        else
        {
            wtoa((USHORT) ((HwDeviceExtension->BiosRevision & 0xff000000) >> 24), &(NameStr[strlen(NameStr)]), 16, 0);
            strcat(NameStr, ".");
            wtoa((USHORT) ((HwDeviceExtension->BiosRevision & 0x00ff0000) >> 16), &(NameStr[strlen(NameStr)]), 16, 2);
            strcat(NameStr, ".");
            wtoa((USHORT) ((HwDeviceExtension->BiosRevision & 0x0000ffff)), &(NameStr[strlen(NameStr)]), 16, 4);
        }

        //*******************************************************
        // Convert BIOS version string to unicode.
        //*******************************************************
        for (c = i = 0; c < strlen(NameStr); c++)
            {
            wcNameStr[c] = (WCHAR)(NameStr[i + c]) & (WCHAR) 0xff;
            }
        wcNameStr[c] = (WCHAR) 0;
        goto WriteRegistryVersionStr;
        }

    //********************************************************
    // Search for Diamond Viper BIOS: it doesn't use the same
    // version strings as the standard NV BIOS.
    //********************************************************
    if (pBios == NULL)
        return;

    strcpy(NameStr, "Viper ");
    for (i = 0; i < MAX_BIOS_SCAN; i++)
        if (Found = (VideoPortCompareMemory(NameStr, &pBios[i], strlen(NameStr)) == strlen(NameStr)))
            break;

    if (!Found)
        //********************************************************
        // Didn't find a Viper BIOS: write the default version
        // string to the registry.
        //********************************************************
        goto WriteRegistryVersionStr;

    Found = FALSE;
    for ( i += (strlen(NameStr) + 1); i < MAX_BIOS_SCAN; i++)
        if (Found = (VideoPortCompareMemory(VersionStr, &pBios[i], strlen(VersionStr)) == strlen(VersionStr)))
            break;

    if (!Found)
        //********************************************************
        // Didn't find a Version String in the Viper BIOS: write
        // the default version string to the registry.
        //********************************************************
        goto WriteRegistryVersionStr;

    //*******************************************************
    // Determine length of BIOS version string.
    //*******************************************************
    Found = FALSE;
    for (j = i; j < MAX_BIOS_SCAN; j++)
        if (Found = (pBios[j] == '\r'))
            break;

    if (!Found || (j == i))
        return;

    //*******************************************************
    // Convert BIOS version string to unicode.
    //*******************************************************
    for (c = 0; (c < j - i) && (c < BIOS_STR_MAX - 1); c++)
        {
        wcNameStr[c] = (WCHAR)(pBios[i + c] & 0xffffL);
        }

    wcNameStr[c] = (WCHAR) 0;

WriteRegistryVersionStr:
    //*******************************************************
    // Write version string to registry so display applet
    // can access it.
    //*******************************************************
    VideoPortSetRegistryParameters(HwDeviceExtension,
                L"HardwareInformation.BiosString",
                wcNameStr,
                (wcslen(wcNameStr) + 1) * sizeof(WCHAR));
    return;
}

//******************************************************************************
//
// Function: wtoa(Number, pString, radix, width
//
// Routine Description:
//  Converts an unsigned 16 number to an ASCII string.
//
// Arguments:
//      Number      Number to be converted
//      pString     ptr to buffer for converted string. Must be
//                  a min of 6 bytes long.
//      radix       base for conversion
//      width       If 0, do not store leading 0s in pString, otherwise == number of nibbles in input number
//
// Return Value:
//
//******************************************************************************
VOID wtoa(USHORT Number, PUCHAR pString, UCHAR radix, UCHAR width)
{
    ULONG   divisor;
    USHORT  digit;
    USHORT  i;


    if (width)
        divisor = 1 << (4 * (width - 1));
    else
    {
        divisor = radix;
        while (divisor <= Number)
            divisor *= radix;
    }

    for (i = 0; divisor; (divisor /= radix))
        {
        digit = (USHORT)(Number / divisor);
        if (!(!i && !digit && !width))
            if (digit < 10)
                pString[i++] = (UCHAR) ((digit) + '0');
            else
                pString[i++] = (UCHAR) ((digit - 10) + 'A');
        Number = (USHORT) (Number % divisor);
        }
    pString[i]='\0';
}

#if _WIN32_WINNT < 0x0500
//******************************************************************************
//
// Function: wcslen(pString)
//
// Routine Description:
//  Our implementation of a wide char string length routine. We
//  can't call the c lib version of wcslen from this kernel mode
//  driver.
//
// Arguments:
//      pString     wide char string
//
// Return Value:
//      Length of string in wide chars, less the null terminator.
//******************************************************************************
ULONG wcslen(PWCHAR pString)
{
    ULONG i;

    for (i = 0; pString[i]; i++);
    return(i);
}
#endif // #if _WIN32_WINNT < 0x0500



//******************************************************************************
//
// Function: GetNextWordFromRegistry
//
// Routine Description:
//
//          Read the next value in the mode entry line.
//          This function 1) skips unicode white space, and then
//          Converts the unicode string to an integer (short) value.
//
// Arguments:
//
// Return Value:
//
//******************************************************************************


SHORT GetNextWordFromRegistry(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PUCHAR *SrcStrPtr
    )


    {
    SHORT value;
    UCHAR ch;

    ch = **SrcStrPtr;

    //******************************************************************************
    // Skip over empty space in unicode string
    //******************************************************************************

    while ((ch == 0) || (ch == ' '))
        {
        (*SrcStrPtr)++;                 // Advance the unicode string ptr
        ch = **SrcStrPtr;
        }

    //******************************************************************************
    // We're now at the beginning of the next 'unicode' number
    //******************************************************************************

    value = my_wtoi(SrcStrPtr);

    return(value);
    }


//******************************************************************************
//
// Function: my_wtoi
//
// Routine Description:
//
//      Converts unicode string to an integer (short value)
//
// Arguments:
//
// Return Value:
//
//******************************************************************************


SHORT my_wtoi(
    PUCHAR *SrcStrPtr
    )

    {
    short i,n;

    n=0;
    for (i=0  ; ((**SrcStrPtr) >= '0') && ((**SrcStrPtr) <='9')  ; (*SrcStrPtr)+=2 )
        n=10 * n + ((**SrcStrPtr) - '0');

    return(n);
    }

//******************************************************************************
//
// Function: my_strupr
//
// Routine Description:
//
//      Converts string to all upper case
//
// Arguments:
//
// Return Value:
//
//******************************************************************************


VOID my_strupr(
    PUCHAR s
    )

    {
    short i;

    for (i=0; s[i] != '\0'; i++)
        if ( (s[i] >= 'a') && ( s[i] <='z')  )
            s[i] = (s[i] - 'a') + 'A';

    }

//******************************************************************************
//
// Function: my_strcmp
//
// Routine Description:
//
//      String compare routine.
//      Returns Zero if strings are equal.
//      Returns NonZero if string are not equal
//
// Arguments:
//
// Return Value:
//
//******************************************************************************


ULONG my_strcmp(
    PUCHAR s,
    PUCHAR t
    )

    {
    short i;


    for (i=0; s[i] == t[i] ; i++)
        if (s[i] == '\0')
            return(0);

    return(s[i] - t[i]);
    }


//******************************************************************************
//
// Function: my_strcpy
//
// Routine Description:
//
//      Converts string to all upper case
//
// Arguments:
//
// Return Value:
//
//******************************************************************************


VOID my_strcpy(
    PUCHAR s,
    PUCHAR t
    )

    {
    short i;

    i=0;

    while (  ( s[i] = t[i]  ) != '\0')
        i++;
    }


//******************************************************************************
//
// Function: NV10CursorTimer
//
// Routine Description:
//
//      This function is to workaround the NV10 HW cursor alpha blending problem.
//      Please also see NV10SetPointerShape and NV10MovePointer in NV10ptr.c in
//      displays driver.
//
// Arguments:
//
// Return Value:
//
//******************************************************************************
VOID NV10CursorTimer(
    PVOID HwDeviceExtension
)
{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    LONG i, j, x, y;
    LONG lScreenStride;
    LONG lScreenPixelDepth;

    ULONG ulAlpha;
    ULONG ulBgdColor;
    ULONG ulBgdRed;
    ULONG ulBgdGreen;
    ULONG ulBgdBlue;
    ULONG ulSrcRed;
    ULONG ulSrcGreen;
    ULONG ulSrcBlue;
    ULONG ulSrcColor;
    ULONG ulDstColor;
    ULONG ulHwCursorWidth;      // HW : 32 or 64

    NV_CURSOR_INFO * pNvCursorInfo;

    UCHAR * pjScreenLoc;
    ULONG * pulDstCursor;
    ULONG * pulSrcCursor;


    pNvCursorInfo = &hwDeviceExtension->NvCursorInfo;

    lScreenStride = pNvCursorInfo->lScreenStride;
    ulHwCursorWidth = pNvCursorInfo->ulHwCursorWidth;
    lScreenPixelDepth = pNvCursorInfo->lScreenPixelDepth;

    pulDstCursor = (ULONG *)( (UCHAR *)hwDeviceExtension->FrameBufferPtr + pNvCursorInfo->ulCursorMemOffset);
    pulSrcCursor = (ULONG *)( (UCHAR *)hwDeviceExtension->FrameBufferPtr + pNvCursorInfo->ulOriginalImageMemOffset);

    x = pNvCursorInfo->lCursorX;
    y = pNvCursorInfo->lCursorY;

    pjScreenLoc = (PUCHAR)hwDeviceExtension->FrameBufferPtr + y * lScreenStride + x * lScreenPixelDepth;

    for(i = 0; i < pNvCursorInfo->lCursorHeight; i++)
    {
        if( (y+i) < 0 ) continue;

        for(j = 0; j < pNvCursorInfo->lCursorWidth; j++)
        {
            if( (x+j) < 0) continue;
            ulSrcColor = *(i * ulHwCursorWidth + j + pulSrcCursor);
            ulAlpha = (ulSrcColor >> 24);
            if( (!ulAlpha) || (ulAlpha == 0xFF))
            {
                // HW will take care it
                continue;
            }
            else
            {

                // HW is broken;  SW Alpha blending
                ulBgdColor = *(ULONG *)(i * lScreenStride + j * lScreenPixelDepth + pjScreenLoc);

                if(lScreenPixelDepth == 2)
                {
                    // Screen is on 16 BPP mode.
                    ulBgdRed   = (ulBgdColor >> 11) & 0x1F;
                    ulBgdGreen = (ulBgdColor >> 05) & 0x3F;
                    ulBgdBlue  = (ulBgdColor & 0x1F);

                    ulBgdRed   = ulBgdRed  * 0xFF / 0x1F;         // convert to 8 bits format
                    ulBgdGreen = ulBgdGreen  * 0xFF / 0x3F;       // convert to 8 bits format
                    ulBgdBlue  = ulBgdBlue * 0xFF / 0x1F;

                }
                else
                {
                    // Screen is on 32 BPP mode
                    ulBgdRed   = (ulBgdColor >> 16) & 0xFF;
                    ulBgdGreen = (ulBgdColor >> 8) & 0xFF;
                    ulBgdBlue  = (ulBgdColor & 0xFF);
                }

                ulSrcRed   = (ulSrcColor >> 16) & 0xFF;
                ulSrcGreen = (ulSrcColor >> 8) & 0xFF;
                ulSrcBlue  = (ulSrcColor & 0xFF);

                ulSrcRed    = ulSrcRed   + (((255 - ulAlpha) * ulBgdRed + 128) >> 8) ;
                ulSrcRed   &= 0xFF;

                ulSrcGreen  = ulSrcGreen + (((255 - ulAlpha) * ulBgdGreen + 128) >> 8);
                ulSrcGreen &= 0xFF;

                ulSrcBlue   = ulSrcBlue  + (((255 - ulAlpha) * ulBgdBlue + 128) >> 8);
                ulSrcBlue  &= 0xFF;

                ulDstColor =  (0xFF000000) | (ulSrcRed << 16) | (ulSrcGreen << 8) | ulSrcBlue;

                *(i * ulHwCursorWidth + j + pulDstCursor) = ulDstColor;
            }
        }
    }

}




#ifdef NVPE
//******************************************************************************
//
// Function: NVRegistryCallbackPnpId()
//
// Routine Description: callback function used to read 'pnpid' strings from
//                      registry
// Arguments:
//
// Return Value:
//
//******************************************************************************


VP_STATUS NVRegistryCallbackPnpId(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength)
{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    SHORT shortvalue;
    PUCHAR SrcStrPtr;
    SHORT *DestPtr;

    //**********************************************************************
    // If the destination ptr is null, we don't care about the actual value, just
    // the fact that the Value name was found in the registry.
    //**********************************************************************
    if (Context == NULL)
        return (NO_ERROR);

    VideoPortMoveMemory(Context, ValueData, ValueLength);

    return(NO_ERROR);

} // end NVRegistryCallbackPnpId()

#endif


//
// Detects the number of heads present and reads in the EDID for each head.
// bBootTime: If TRUE, sets safe default masks.
//

BOOLEAN NVInitialize_DacsAndEDID(PHW_DEVICE_EXTENSION HwDeviceExtension, BOOLEAN bBootTime)
{
    ULONG Count = 0;
    BOOLEAN bRet = TRUE;
    NV_DESKTOP_INFO * pInfo;
    LONG i, numChildren;
    ULONG ulConnectedDeviceMask;
    NVTWINVIEWDATA TwinViewData;
    VP_STATUS Status;
    PVOID ptr;

    VideoDebugPrint((2, "INVInitialize_DacsAndEDID():\n"));

    if (bBootTime)
        HwDeviceExtension->bNeedRetrieveEDID = FALSE;
    
    pInfo = (NV_DESKTOP_INFO *) &HwDeviceExtension->DeskTopInfo;
    pInfo->ulNumberDacsOnBoard = 1;
    pInfo->ulNumberDacsConnected = 1;

    // Get the bitmask for all the connectors in the system.
    pInfo->ulAllDeviceMask = BITMASK_INVALID_DEVICE;
    RmConfigGetKernel(HwDeviceExtension->DeviceReference, NV_CFG_GET_ALL_DEVICES, &pInfo->ulAllDeviceMask);
    VideoDebugPrint((1,"ulAllDeviceMask: 0x%x\n",pInfo->ulAllDeviceMask));

    // Get the connect status for all the connectors.
    pInfo->ulConnectedDeviceMask = pInfo->ulAllDeviceMask;
    ulConnectedDeviceMask = pInfo->ulConnectedDeviceMask;

    if (RmConfigGetExKernel(HwDeviceExtension->DeviceReference,
                            NV_CFGEX_CHECK_CONNECTED_DEVICES, &pInfo->ulConnectedDeviceMask, 
                            sizeof(pInfo->ulConnectedDeviceMask)))
    {
        // success.
        VideoDebugPrint((1,"ulConnectedDeviceMask: 0x%x\n",pInfo->ulConnectedDeviceMask));
        if (pInfo->ulConnectedDeviceMask == 0)
        {
            VideoDebugPrint((0,"Oops! ConnectedDevicemask is 0. Assume a CRT\n"));
            pInfo->ulConnectedDeviceMask = 0x1;

        }
        ulConnectedDeviceMask = pInfo->ulConnectedDeviceMask;
    }
    else
    {
        // The resman had issues with our request, so spew a warning
        VideoDebugPrint((0, "NVInitialize -  RmConfigGetExKernel() returned error for NV_CFGEX_CHECK_CONNECTED_DEVICESI\n"));
    }


    if (!RmConfigGetKernel(HwDeviceExtension->DeviceReference, NV_CFG_NUMBER_OF_HEADS, &pInfo->ulNumberDacsOnBoard))
    {
        VideoDebugPrint((0,"Ooops! RmConfigGetKernel() failed for NV_CFG_NUMBER_OF_HEADS\n"));
        bRet = FALSE;
    }
    else
    {
        if (pInfo->ulNumberDacsOnBoard == 1)
        {
            pInfo->ulNumberDacsConnected = 1;
            if (bBootTime) 
            {
                SetSafeDeviceMasks_SingleHeadBoard(HwDeviceExtension);
            }
        }
        if (pInfo->ulNumberDacsOnBoard == 2)
        {
            if (ConvertConnectedMaskToNumDevices(ulConnectedDeviceMask) == 1)
            {
                pInfo->ulNumberDacsConnected = 1;
            }
            if (ConvertConnectedMaskToNumDevices(ulConnectedDeviceMask) > 1)
            {
                pInfo->ulNumberDacsConnected = 2;
            }
            if (bBootTime)
            {
                SetSafeDeviceMasks_DualHeadBoard(HwDeviceExtension);
                InitFromTwinViewInfo(HwDeviceExtension);
            }
        }
        HandleBIOSHeads(HwDeviceExtension);

        // Print the info collected
        VideoDebugPrint((1,"DacsOnBoard: %d, DacsConnected: %d, AllDeviceMask: 0x%x, ConnectedDeviceMask: 0x%x,\n",
                 pInfo->ulNumberDacsOnBoard, pInfo->ulNumberDacsConnected,pInfo->ulAllDeviceMask,  pInfo->ulConnectedDeviceMask));
    } 


    VideoDebugPrint((2,"Number of heads on board returning to display driver: 0x%x, number connected: 0x%x\n",
        pInfo->ulNumberDacsOnBoard,pInfo->ulNumberDacsConnected));

    numChildren = ConvertConnectedMaskToNumDevices(ulConnectedDeviceMask);
    ptr = ExAllocatePool(PagedPool, sizeof(EDID_COLLECTION)+sizeof(EDID_UNIT)*(numChildren+1));
    if (ptr == NULL)
    {
        return FALSE;
    }
    if (HwDeviceExtension->EdidCollection)
    {
        ExFreePool(HwDeviceExtension->EdidCollection);
        HwDeviceExtension->EdidCollection = NULL;
    }
    HwDeviceExtension->EdidCollection = (LPEDID_COLLECTION)ptr;
    VideoPortZeroMemory(HwDeviceExtension->EdidCollection, sizeof(EDID_COLLECTION)+sizeof(EDID_UNIT)*(numChildren+1));
    //
    // By default, all HeadMaps points to a dummy EdidUnit, which is the last one EdidUnits array
    // It's also why we allocate one more EdidUnit
    //
    for (i = 0; i < NV_NO_CONNECTORS; i++)
    {
        HwDeviceExtension->EdidCollection->HeadMaps[i] = &(HwDeviceExtension->EdidCollection->EdidUnits[numChildren]);
    }

    //
    // Now read the EDID for all the connected devices.
    // Get the EDID buffer from the RM always.
    //
    numChildren = 0;
    for (i = 0; i < NV_NO_CONNECTORS; i++)
    {
        ULONG ulDeviceMask;
        ulDeviceMask = (1 << i);

        // See if this device is connected
        if (ulConnectedDeviceMask & ulDeviceMask)
        {
            NV_CFGEX_GET_LOGICAL_DEV_EDID_PARAMS EdidParams;
            LPEDID_UNIT pEdidUnit = &HwDeviceExtension->EdidCollection->EdidUnits[numChildren];
            ULONG EdidBufferSize          = EDID_V2_SIZE;

            EdidParams.displayMap   = ulDeviceMask;
            EdidParams.edidBuffer   = (UCHAR *) &(pEdidUnit->EDIDBuffer[0]);
            EdidParams.bufferLength = &EdidBufferSize;

            if (!RmConfigGetExKernel(HwDeviceExtension->DeviceReference, NV_CFGEX_GET_LOGICAL_DEV_EDID, &EdidParams, sizeof(NV_CFGEX_GET_LOGICAL_DEV_EDID_PARAMS) ))
            {
                VideoDebugPrint((1, "RmConfigGetExKernel() returned error for NV_CFGEX_GET_LOGICAL_DEV_EDID for DeviceMask: 0x%x\n",ulDeviceMask));
                // Reset all the data fields.
                VideoPortZeroMemory(pEdidUnit, sizeof(EDID_UNIT));
            }
            else
            {
                pEdidUnit->Valid = 1;
                pEdidUnit->Size  = EdidBufferSize;
            }
            HwDeviceExtension->EdidCollection->HeadMaps[i] = &(HwDeviceExtension->EdidCollection->EdidUnits[numChildren]);
            numChildren++;
        }
    }

    // Now read the TwinViewInfo registry structure. If it is invalid with the current configuration,
    // Invalidate it. For example, the user might have done a output device switch to TV, shut down the system,
    // reboot without the TV.
    Status = NVGetRegistryTwinviewInfo(HwDeviceExtension, &TwinViewData);
    if (Status == NO_ERROR)
    {
        // Check to see if the data is valid.
        if (!TwinViewDataOK(HwDeviceExtension, &TwinViewData))
        {
            // Invalidate the registry data.
            InvalidateTwinViewData(HwDeviceExtension, &TwinViewData);
        }
    }

#if (_WIN32_WINNT >= 0x0500)
    if (HwDeviceExtension->pDualViewAssociatedDE)
        HwDeviceExtension->pDualViewAssociatedDE->EdidCollection  = HwDeviceExtension->EdidCollection;
#endif

    return(TRUE);
}




//
// Returns TRUE if the device list is successfully scanned
//
BOOLEAN bScanHeadDeviceOptions(PHW_DEVICE_EXTENSION HwDeviceExtension)
{
    // The FALSE indicates it is not a boottime operation.
    NVInitialize_DacsAndEDID(HwDeviceExtension, FALSE);
    return(TRUE);
} // End function bScanHeadDeviceOptions()



//
// This function should be called once at boot time.
// It initializes the DeskTopInfo structure to safe values.
//
BOOLEAN NVOneTimeInit_TwinView(PHW_DEVICE_EXTENSION HwDeviceExtension)
{
    ULONG i, Count = 0;
    BOOLEAN bRet = TRUE;
    NV_DESKTOP_INFO * pInfo;
    VP_STATUS status;
    ULONG ulValue;
    
    VideoDebugPrint((2, "NVOneTimeInit_TwinView():\n"));

    //
    // If we don't create the events, NVSVC can not open the events and will keep spitting out debug messages.
    // Just creating the events is harmless even if the NVSVC is never installed or started.
    //
    InitNVSVCEvents();
    HwDeviceExtension->ulNVSVCStatus = NVSVC_STATUS_STOPPED;

    //
    // We are booting up the system. So no modeset has happended yet.
    // The display driver needs to know this because on Mobile systems, we need to
    // honor the BIOS's output device selections at boot time.
    //
    HwDeviceExtension->SystemModeSetHasOccurred = 0;
    VideoPortSetRegistryParameters(HwDeviceExtension,
                                      L"SystemModeSetHasOccurred",
                                      &HwDeviceExtension->SystemModeSetHasOccurred,
                                      sizeof(ULONG));

    //
    // Per Terry, blindly create "NvCplSetAtStartup" with a value of 0xFFFFFFFF at boot time. This will be
    // used and managed by the control panel.
    //
    ulValue = 0xFFFFFFFF;
    VideoPortSetRegistryParameters(HwDeviceExtension,
                                      L"NvCplSetAtStartup",
                                      &ulValue,
                                      sizeof(ULONG));
    
    pInfo = (NV_DESKTOP_INFO *) &HwDeviceExtension->DeskTopInfo;
    pInfo->ulDesktopModeIndex = 0;

    pInfo->ulDesktopMode = 0;
    //
    // Initialize the state to Standard mode. This will be changed to clone mode only in the case of 
    // 'UseHwSelectedDevice' (i.e. laptops) being on and the BIOSDeviceMask indicating two devices.
    //
    pInfo->ulTwinView_State = NVTWINVIEW_STATE_NORMAL;
    pInfo->ulNumberDacsOnBoard = 1;
    pInfo->ulNumberDacsConnected = 1;
    pInfo->ulNumberDacsActive = 1;
    pInfo->ulAllDeviceMask = BITMASK_CRT0;
    pInfo->ulConnectedDeviceMask = BITMASK_CRT0;

    pInfo->ulDesktopWidth = 640;
    pInfo->ulDesktopHeight = 480;

    for(i = 0; i < NV_NO_DACS; i++)
    {
        pInfo->ulDisplayWidth[i] = 640;
        pInfo->ulDisplayHeight[i] = 480;
        pInfo->ulDisplayRefresh[i] = 60;
        pInfo->ulDisplayPixelDepth[i] = 8;
        pInfo->ulDeviceDisplay[i] = i;
        pInfo->ulDeviceMask[i] = BITMASK_CRT0;
        pInfo->ulDeviceType[i] = MONITOR_TYPE_VGA;
        pInfo->ulTVFormat[i] = 0;
    }

    HwDeviceExtension->ulUniquenessVGA = 0;
    HwDeviceExtension->ulNVSVCStatus = NVSVC_STATUS_STOPPED;
    HwDeviceExtension->ulInduceModeChangeDeviceMask = 0;

    return(bRet);
}


//
// Returns the number and list of physical modes for the specified head.
// The following filters are applied
// 1. PixelClockFrequency of the specified head. Note if ulHead is 1 and ulDeviceType is CRT, then the 
//  pixelFreq of Head1 (150MHZ) is used. For other device types (DFP,TV) the head0's (350MHZ) pixel freq is used.
// 2. If a MonitorModeList is found, use this list (after filtering against the pixelFreq) and Return.
// 3. If ulEnableDDC is enabled, then parse the EDID for this to get the modes.
//
BOOLEAN  GetHeadPhysicalModeList(
    PHW_DEVICE_EXTENSION HwDeviceExtension, 
    HEAD_MODE_LIST_INFO * pInput,
    ULONG * pRetSize)
{
    ULONG ulHead;
    ULONG ulEnableDDC;
    MODE_ENTRY *MonitorModeList, *VirtualModeList;
    ULONG NumMonitorModes, NumVirtualModes, NumValidModes;
    MODE_ENTRY *DestPtr;
    LPDISPDATA lpDispData;
    LPMODEINFO lpModeDesc;
    LPMODEOUT  lpModeOut;
    LPMODEINFO lpModeNew;
    ULONG ulFrequencyHead;
    DISPDATA DispData;
    MODEINFO ModeDesc;
    MODEOUT  ModeOut;
    MODEINFO ModeNew;
    ULONG ulRet, i, j;
    BOOLEAN bFoundMatchInVirtualList;
    ULONG ulHeadEDID;
    LPEDID_UNIT pEdidUnit;
    ULONG ulDeviceType;
    ULONG ulWidth, ulHeight;

    VideoDebugPrint((3,"Enter GetHeadPhysicalModeList()\n"));

    lpDispData = &DispData;
    lpModeDesc = &ModeDesc;
    lpModeOut = &ModeOut;
    lpModeNew = &ModeNew;
    

    ulHead = pInput->ulHead;
    ulEnableDDC = pInput->ulEnableDDC;
    
    ulHeadEDID =  ConvertDeviceMaskToIndex(pInput->ulDeviceMask);
    pEdidUnit = HwDeviceExtension->EdidCollection->HeadMaps[ulHeadEDID];

    // If the device type is TV, w simply return the virtual mode list with only
    // 640x480 and 800x600 for all color depths at only 60HZ
    // For brooktree871 encoder, add the 1024x768 resolution

    pInput->ulDeviceType = MaskToDeviceType(pInput->ulDeviceMask);

    
    if (pInput->ulDeviceType == MONITOR_TYPE_NTSC || pInput->ulDeviceType == MONITOR_TYPE_PAL)
    {
        DestPtr = pInput->pPhysicalModeList;
        VirtualModeList = pInput->pVirtualModeList;

        NumVirtualModes = pInput->ulNumVirtualModes;
        NumValidModes = NumVirtualModes;

        for (i=0; i < NumVirtualModes; i++)
        {

            if (HwDeviceExtension->ulTVEncoderType == NV_ENCODER_BROOKTREE_871)
            {
                if (VirtualModeList->Width > 1024 || VirtualModeList->Height > 768 || VirtualModeList->RefreshRate != 60)
                { 
                    // Not a valid mode.
                    NumValidModes--;
                    VirtualModeList++;
                    continue;
                }
            }
            else if (VirtualModeList->Width > 800 || VirtualModeList->Height > 600 || VirtualModeList->RefreshRate != 60)
            {
                // Not a valid mode.
                NumValidModes--;
                VirtualModeList++;
                continue;
            }

            // The mode passes all filters.
            // Copy this valid mode and process the next mode from the monitor modelist.
            if (DestPtr)
            {
                *DestPtr++ = *VirtualModeList++;
            }
            else
            {
                VirtualModeList++;
            }
            
        }
        *pRetSize = NumValidModes * sizeof(MODE_ENTRY);
        return(TRUE);
    }

    // Set up the generic fields required for the FindModeEntry() call.
    //
    // Check if CRT is connected for second head. Only then should we
    // use the pixel clock for the second head. Otherwise, we should use the pixel clock of
    // DAC 0 for both heads.
    //
    ulFrequencyHead = ulHead;
    if (ulHead == 1 && pInput->ulDeviceType != MONITOR_TYPE_VGA) {
         ulFrequencyHead = 0;
    }
    lpDispData->lpBoardRegistryData = NULL;
    lpDispData->lpMonitorRegistryData = NULL;
    lpDispData->dwMonitorInfType = PLUG_AND_PLAY_DEVICE;
    lpDispData->dwDeviceID = HwDeviceExtension->ulChipID;
    lpDispData->dwCRTCIndex = ulHead;
    lpDispData->dwVideoMemoryInBytes = HwDeviceExtension->AdapterMemorySize;
    lpDispData->dwMaxDacSpeedInHertz8bpp= HwDeviceExtension->maxPixelClockFrequency_8bpp[ulFrequencyHead];
    lpDispData->dwMaxDacSpeedInHertz16bpp = HwDeviceExtension->maxPixelClockFrequency_16bpp[ulFrequencyHead];
    lpDispData->dwMaxDacSpeedInHertz32bpp = HwDeviceExtension->maxPixelClockFrequency_32bpp[ulFrequencyHead];
    lpDispData->dwMaxDfpScaledXResInPixels = HwDeviceExtension->MaxFlatPanelWidth;
    lpDispData->lpfnGetModeSize = 0;  // GK: WHat is this?
    lpDispData->dwContext1 = (PVOID)HwDeviceExtension;
    lpDispData->dwContext2 = (PVOID)0;
    lpDispData->dwMaxTVOutResolution = HwDeviceExtension->ulMaxTVOutResolution;

    
    // Read in the monitor restriction mode lists for each head.
    ReadMonitorRestrictionModeList(HwDeviceExtension);

    
    // use the right monitor mode list entry for this head.
    switch (ulHead)
    {
        case 0:
            MonitorModeList = (MODE_ENTRY *)&HwDeviceExtension->RestrictionModeTable0[0];
            NumMonitorModes = HwDeviceExtension->NumRestrictionModes0;
            break;
        case 1:
            MonitorModeList = (MODE_ENTRY *)&HwDeviceExtension->RestrictionModeTable1[0];
            NumMonitorModes = HwDeviceExtension->NumRestrictionModes1;
            break;
        default:
            VideoDebugPrint((1,"Oops! Invalid Head: 0x%x\n",ulHead));
            MonitorModeList = (MODE_ENTRY *)&HwDeviceExtension->RestrictionModeTable0[0];
            NumMonitorModes = HwDeviceExtension->NumRestrictionModes0;
            break;
    }

    
 
    // If the monitor mode list is present, then we simply return this mode list for this head.
    // We do not bother about EDID for this case.
    if (NumMonitorModes != 0)
    {
        DestPtr = pInput->pPhysicalModeList;
        
        NumVirtualModes = pInput->ulNumVirtualModes;
        NumValidModes = NumMonitorModes;

        lpModeDesc->MIDevData.cType     = (CHAR)MaskToFindModeEntryDeviceType(pInput->ulDeviceMask);
        lpModeDesc->MIDevData.cNumber   = (CHAR)MaskToDeviceType(pInput->ulDeviceMask);
        lpModeDesc->MIDevData.cFormat   = (CHAR)pInput->ulTVType;
        lpModeDesc->MIDevData.cReserved = 0x0;

        EnterFPCode(HwDeviceExtension->pFPStateSave, HwDeviceExtension->pFPMutex);
        for (i=0; i < NumMonitorModes; i++)
        {
            VirtualModeList = pInput->pVirtualModeList;
            bFoundMatchInVirtualList = FALSE;
            // First validate against the virtual mode list.

            for (j=0; j < NumVirtualModes; j++)
            {
                if (VirtualModeList->Width == MonitorModeList->Width &&
                    VirtualModeList->Height == MonitorModeList->Height &&
                    VirtualModeList->Depth == MonitorModeList->Depth &&
                    VirtualModeList->RefreshRate == MonitorModeList->RefreshRate &&
                    MonitorModeList->ValidMode)
                {
                    bFoundMatchInVirtualList = TRUE;
                    break;
                }
                VirtualModeList++;

            }
            // If we did not find a match for this specified mode in the virtual mode list, this mode is not valid.
            if (bFoundMatchInVirtualList == FALSE)
            {
                // Not a valid mode.
                NumValidModes--;
                MonitorModeList++;
                continue;
            }

            // Now validate against the pixel frequency of the head.
            lpModeDesc->dwXRes = MonitorModeList->Width;
            lpModeDesc->dwYRes = MonitorModeList->Height;
            lpModeDesc->dwBpp = MonitorModeList->Depth;
            lpModeDesc->dwRefreshRate = MonitorModeList->RefreshRate;
            lpModeDesc->dwOriginalRefreshRate = MonitorModeList->RefreshRate;
            lpModeDesc->dwDevType = MaskToFindModeEntryDeviceType(pInput->ulDeviceMask);
            lpModeDesc->dwTVFormat = pInput->ulTVType;


            // We want to ignore the EDID since we are using the explicit monitor mode list.
            lpDispData->dwMonitorInfType = UNKNOWN_DEVICE;
            // Call the modeset DLL to see if this mode is valid.
            // We are not interested in the mode timings but just to see if this is a valid mode.
            VideoDebugPrint((3,"Before: lpModeDescr: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
                lpModeDesc->dwXRes, lpModeDesc->dwYRes, lpModeDesc->dwBpp, lpModeDesc->dwRefreshRate, 
                lpModeDesc->dwDevType, lpModeDesc->dwOriginalRefreshRate));

            ulRet = FindModeEntry(lpDispData, pEdidUnit->EDIDBuffer, pEdidUnit->Size,
                                  lpModeDesc, lpModeOut, lpModeNew);
            VideoDebugPrint((3,"ulRet: 0x%x\n",ulRet));
            VideoDebugPrint((3,"After: lpModeDescr: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
                lpModeDesc->dwXRes, lpModeDesc->dwYRes, lpModeDesc->dwBpp, lpModeDesc->dwRefreshRate, 
                lpModeDesc->dwDevType, lpModeDesc->dwOriginalRefreshRate));
            VideoDebugPrint((3,"After: lpModeNew: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
                lpModeNew->dwXRes, lpModeNew->dwYRes, lpModeNew->dwBpp, lpModeNew->dwRefreshRate, 
                lpModeNew->dwDevType, lpModeNew->dwOriginalRefreshRate));

            if (ulRet != 0)
            {
                // Not a valid mode.
                NumValidModes--;
                MonitorModeList++;
                continue;
            }
            
            if((pInput->ulDeviceType == MONITOR_TYPE_FLAT_PANEL) && (HwDeviceExtension->ulDisableDfpModesGreaterThan))
            {
                ulWidth = HwDeviceExtension->ulDisableDfpModesGreaterThan >> 16;
                ulHeight = HwDeviceExtension->ulDisableDfpModesGreaterThan & 0xFFFF;
                if ((VirtualModeList->Width > ulWidth) && (VirtualModeList->Height > ulHeight))
                { 
                    // Not a valid mode.
                    NumValidModes--;
                    VirtualModeList++;
                    continue;
                }
            }

            // The mode passes all filters.
            // Copy this valid mode and process the next mode from the monitor modelist.
            if (DestPtr)
            {
                *DestPtr++ = *MonitorModeList++;
            }
            else
            {
                MonitorModeList++;
            }
        }
        *pRetSize = NumValidModes * sizeof(MODE_ENTRY);
        ExitFPCode(HwDeviceExtension->pFPStateSave, HwDeviceExtension->pFPMutex);
        return(TRUE);
    }

    //
    // There is no specified monitor mode list for this head.
    // Now see if we need to parse the EDID.
    //
    if (pInput->ulEnableDDC)
    {
        // There is no monitor mode list for this head and DDC is enabled by the user for this head.
        // So we need to filter against pixel clock freq and EDID.
        if (pEdidUnit->Valid)
        {
            lpDispData->dwMonitorInfType = PLUG_AND_PLAY_DEVICE;
        }
        else
        {
            lpDispData->dwMonitorInfType = UNKNOWN_DEVICE;
        }
    }
    else
    {
        // There is no monitor mode list for this head and DDC is disabled by the user for this head.
        // So we need to filter against pixel clock freq only.
        lpDispData->dwMonitorInfType = UNKNOWN_DEVICE;
        // BUGBUG: I am assuming that just setting the UNKNOWN_DEVICE is sufficient while still passing
        // in the EDID and EDIDSize into FindModeEntry().
    }
    // The user does not want EDID detection for this head.
    // We need to only do the filtering of all virtual list modes against the pixel clock.

    DestPtr = pInput->pPhysicalModeList;
    VirtualModeList = pInput->pVirtualModeList;

    NumVirtualModes = pInput->ulNumVirtualModes;
    NumValidModes = NumVirtualModes;
    EnterFPCode(HwDeviceExtension->pFPStateSave, HwDeviceExtension->pFPMutex);

    lpModeDesc->MIDevData.cType     = (CHAR)MaskToFindModeEntryDeviceType(pInput->ulDeviceMask);
    lpModeDesc->MIDevData.cNumber   = (CHAR)MaskToDeviceType(pInput->ulDeviceMask);
    lpModeDesc->MIDevData.cFormat   = (CHAR)pInput->ulTVType; 
    lpModeDesc->MIDevData.cReserved = 0x0;

    for (i=0; i < NumVirtualModes; i++)
    {

        // Now validate against the pixel frequency of the head.
        lpModeDesc->dwXRes = VirtualModeList->Width;
        lpModeDesc->dwYRes = VirtualModeList->Height;
        lpModeDesc->dwBpp  = VirtualModeList->Depth;
        lpModeDesc->dwRefreshRate   = VirtualModeList->RefreshRate;
        lpModeDesc->dwOriginalRefreshRate = VirtualModeList->RefreshRate;
        lpModeDesc->dwDevType       = MaskToFindModeEntryDeviceType(pInput->ulDeviceMask);
        lpModeDesc->dwTVFormat      = pInput->ulTVType;


        // Call the modeset DLL to see if this mode is valid.
        // We are not interested in the mode timings but just to see if this is a valid mode.
        VideoDebugPrint((3,"Before: lpModeDescr: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
                         lpModeDesc->dwXRes, lpModeDesc->dwYRes, lpModeDesc->dwBpp, lpModeDesc->dwRefreshRate, 
                         lpModeDesc->dwDevType, lpModeDesc->dwOriginalRefreshRate));

        ulRet = FindModeEntry(lpDispData, pEdidUnit->EDIDBuffer, pEdidUnit->Size,
                              lpModeDesc, lpModeOut, lpModeNew);
        VideoDebugPrint((3,"ulRet: 0x%x\n",ulRet));
        VideoDebugPrint((3,"After: lpModeDescr: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
            lpModeDesc->dwXRes, lpModeDesc->dwYRes, lpModeDesc->dwBpp, lpModeDesc->dwRefreshRate, 
            lpModeDesc->dwDevType, lpModeDesc->dwOriginalRefreshRate));
        VideoDebugPrint((3,"After: lpModeNew: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
            lpModeNew->dwXRes, lpModeNew->dwYRes, lpModeNew->dwBpp, lpModeNew->dwRefreshRate, 
            lpModeNew->dwDevType, lpModeNew->dwOriginalRefreshRate));

        if (ulRet != 0)
        {
            // Not a valid mode.
            NumValidModes--;
            VirtualModeList++;
            continue;
        }

        if((pInput->ulDeviceType == MONITOR_TYPE_FLAT_PANEL) && (HwDeviceExtension->ulDisableDfpModesGreaterThan))
        {
            ulWidth = HwDeviceExtension->ulDisableDfpModesGreaterThan >> 16;
            ulHeight = HwDeviceExtension->ulDisableDfpModesGreaterThan & 0xFFFF;
            if ((VirtualModeList->Width > ulWidth) && (VirtualModeList->Height > ulHeight))
            { 
                // Not a valid mode.
                NumValidModes--;
                VirtualModeList++;
                continue;
            }
        }

        // The mode passes all filters.
        // Copy this valid mode and process the next mode from the monitor modelist.
        if (DestPtr)
        {
            *DestPtr++ = *VirtualModeList++;
        }
        else
        {
            VirtualModeList++;
        }
    }
    *pRetSize = NumValidModes * sizeof(MODE_ENTRY);
    ExitFPCode(HwDeviceExtension->pFPStateSave, HwDeviceExtension->pFPMutex);
    return(TRUE);
 
}



//
// This function initializes the mapping layer between Win2K's power states and nVidia's power states
//
VOID InitializeACPIStates(
    PHW_DEVICE_EXTENSION HwDeviceExtension)
{
    BOOL bRet;
    ULONG Caps, ulHead;
    ULONG HighestNVAdapterState, i;
    TOSHIBA_HOTKEY_MODE ToshibaHotKeyMode;
    VP_STATUS status;
    ULONG ModeSetInitiated;
    ULONG InduceModeChange;
    ULONG ResumingFromAdapterPowerDownState;
    ULONG PrevDevMask;

    ulHead = 0;

    
    //
    // Note, at this point in time, it does not matter if we have actually done a modeset to the head or
    // not. Anyways, we support only two states for monitors. Its the capabilities of the adapter we
    // need to get.
    //
    bRet = RmGetPowerCaps(HwDeviceExtension->DeviceReference, ulHead, &Caps);

    //
    // Initialize the monitor power states
    //
    for (i=0; i < HwDeviceExtension->DeskTopInfo.ulNumberDacsOnBoard; i++ )
    {
        HwDeviceExtension->NVMonitorPowerState[i][NV_VideoPowerOn] = NV_POWER_MONITOR_STATE_0;            
        HwDeviceExtension->NVMonitorPowerState[i][NV_VideoPowerStandBy] = NV_POWER_MONITOR_STATE_1;            
        HwDeviceExtension->NVMonitorPowerState[i][NV_VideoPowerSuspend] = NV_POWER_MONITOR_STATE_1;            
        HwDeviceExtension->NVMonitorPowerState[i][NV_VideoPowerOff] = NV_POWER_MONITOR_STATE_1;            
        //
        // If the RM exports an explicit monitor HIBERNATE state, we use that explicit state.
        // This explicit state would leave the monitor in an On state.
        // Otherwise, we map Hibernate to the Off state. Note that Win2K dictates that we use On state for
        // hibernate.
        //
        if (Caps & NV_POWER_MONITOR_STATE_HIBERNATE)
        {
            HwDeviceExtension->NVMonitorPowerState[i][NV_VideoPowerHibernate] = NV_POWER_MONITOR_STATE_HIBERNATE;
        }
        else
        {
            HwDeviceExtension->NVMonitorPowerState[i][NV_VideoPowerHibernate] = NV_POWER_MONITOR_STATE_1;            
        }
    }

    //
    // Search for the highest supported adapter power state
    //
    if ((Caps & NV_POWER_ADAPTER_STATE_3) &&
        (Caps & NV_POWER_ADAPTER_STATE_1) &&
        (Caps & NV_POWER_ADAPTER_STATE_0))
    {
        //
        // This is the case for the desktop.
        //
        HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerOn] = NV_POWER_ADAPTER_STATE_0;            
        HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerStandBy] = NV_POWER_ADAPTER_STATE_1;            
        HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerSuspend] = NV_POWER_ADAPTER_STATE_3;            
        HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerOff] = NV_POWER_ADAPTER_STATE_3;            
        //
        // We support the hibernate state only if the "context save" feature is turned on.
        // If this feature is turned on, we assume that the highest numbered state (3 in this case)
        // will support this context save feature. If the RM exports an explicit HIBERNATE state,
        // then we use that explicit state.
        // This explicit state would leave the adaptor in an On state but does the save/restore.
        // Note that Win2K dictates that we use On state for hibernate.
        //
        if (Caps & NV_POWER_ADAPTER_FEATURE_CTXSAVED)
        {
            if (Caps & NV_POWER_ADAPTER_STATE_HIBERNATE)
            {
                HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerHibernate] = NV_POWER_ADAPTER_STATE_HIBERNATE;
            }
            else
            {
                HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerHibernate] = NV_POWER_ADAPTER_STATE_3;            
            }
        }
        else
        {
            HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerHibernate] = 0; // not supported
        }
     }

    if ((Caps & NV_POWER_ADAPTER_STATE_3) &&
        (Caps & NV_POWER_ADAPTER_STATE_2) &&
        (Caps & NV_POWER_ADAPTER_STATE_0))
    {
        //
        // This is the case for the laptop.
        //
        HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerOn] = NV_POWER_ADAPTER_STATE_0;            
        HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerStandBy] = NV_POWER_ADAPTER_STATE_2;            
        HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerSuspend] = NV_POWER_ADAPTER_STATE_2;            
        HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerOff] = NV_POWER_ADAPTER_STATE_2;            
        //
        // We support the hibernate state only if the "context save" feature is turned on.
        // If this feature is turned on, we assume that the highest numbered state (3 in this case)
        // will support this context save feature. If the RM exports an explicit HIBERNATE state,
        // then we use that explicit state.
        // This explicit state would leave the adaptor in an On state but does the save/restore.
        // Note that Win2K dictates that we use On state for hibernate.
        //
        if (Caps & NV_POWER_ADAPTER_FEATURE_CTXSAVED)
        {
            if (Caps & NV_POWER_ADAPTER_STATE_HIBERNATE)
            {
                HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerHibernate] = NV_POWER_ADAPTER_STATE_HIBERNATE;
            }
            else
            {
                HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerHibernate] = NV_POWER_ADAPTER_STATE_3;            
            }
        }
        else
        {
            HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerHibernate] = 0; // not supported
        }
     }

    // Hotkey Initialization.
    HwDeviceExtension->ulHotKeyPending = 0;
    ToshibaHotKeyMode.ValidMode = 0;
    status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                            L"ForceGDIMode",
                                            FALSE,
                                            NVReadRegistryBinaryCallback,
                                            (PVOID)&ToshibaHotKeyMode);
    if (status == NO_ERROR)
    {
        if (ToshibaHotKeyMode.ValidMode == 1)
        {
            // Invalidate it. This may happen as a rare pathological condition.
            // For example, the user has hit the hotkey function, miniport has written the 'ToshibaHotKeyMode'
            // registry entry, but before the DrvEnablePdev or DrvAssertMode(TRUE) is called, system is rebooted.
            ToshibaHotKeyMode.ValidMode = 0;
            status = VideoPortSetRegistryParameters(HwDeviceExtension,
                                                    L"ForceGDIMode",
                                                    &ToshibaHotKeyMode,
                                                    sizeof(TOSHIBA_HOTKEY_MODE));
        }

    }

    // if the ModeSetInitiated registry key exists, set it to zero.
    ModeSetInitiated = 0;
    status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                            L"NVCplInduceModeSetInitiated",
                                            FALSE,
                                            NVRegistryCallback,
                                            &(ModeSetInitiated));
    if (status == NO_ERROR)
    {
        ModeSetInitiated = 0;
        status = VideoPortSetRegistryParameters(HwDeviceExtension,
                                                L"NVCplInduceModeSetInitiated",
                                                &ModeSetInitiated,
                                                sizeof(ULONG));
    }

    // if the InduceModeChange registry key exists, set it to zero.
    InduceModeChange = 0;
    status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                            L"InduceModeChange",
                                            FALSE,
                                            NVRegistryCallback,
                                            &(InduceModeChange));
    if (status == NO_ERROR)
    {
        InduceModeChange = 0;
        status = VideoPortSetRegistryParameters(HwDeviceExtension,
                                                L"InduceModeChange",
                                                &InduceModeChange,
                                                sizeof(ULONG));
    }

    // if the ResumingFromAdapterPowerDownState registry key exists, set it to zero.
    ResumingFromAdapterPowerDownState = 0;
    status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                            L"ResumingFromAdapterPowerDownState",
                                            FALSE,
                                            NVRegistryCallback,
                                            &(ResumingFromAdapterPowerDownState));
    if (status == NO_ERROR)
    {
        ResumingFromAdapterPowerDownState = 0;
        status = VideoPortSetRegistryParameters(HwDeviceExtension,
                                                L"ResumingFromAdapterPowerDownState",
                                                &(ResumingFromAdapterPowerDownState),
                                                sizeof(ULONG));
    }

    // if the PrevDevMask registry key exists, set it to zero.
    PrevDevMask = 0;
    status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                            L"PrevDevMask",
                                            FALSE,
                                            NVRegistryCallback,
                                            &(PrevDevMask));
    if (status == NO_ERROR)
    {
        PrevDevMask = 0;
        status = VideoPortSetRegistryParameters(HwDeviceExtension,
                                                L"PrevDevMask",
                                                &(PrevDevMask),
                                                sizeof(ULONG));
    }

}

//
// Returns the index from the device mask.
// Input: A 32bit bitmask with only one of the 32bits set specifying a unique device
// Return Value: Index of this bitmask
//      For example: 
//      input: 0x4, Return: 2
//      input: 0x20, Return: 5
//      input: 0x1, Return: 0
//      input: 0x0, Return: 0
//
ULONG ConvertDeviceMaskToIndex(
    ULONG ulDeviceMask)
{
    ULONG i;
    ULONG ulTmp;
    ulTmp = ulDeviceMask;

  
    i=0;
    for (;;)
    {
        if (ulTmp == 0)
        {
            VideoDebugPrint((1,"Oops! ConvertDeviceMaskToIndex(): Invalid ulDeviceMask: 0x%x\n",ulDeviceMask));
            return(0);
        }
        if (ulTmp & 0x1)
        {
            return(i);
        }
        // shift right by one and increment the index
        ulTmp >>= 1;
        i++;
    }
    
}

//
// Returns the number of bits that are set in the mask.
// 
ULONG ConvertConnectedMaskToNumDevices(
    ULONG ulConnectedDeviceMask)
{
    ULONG i, NumDevices, ulDeviceMask;
    NumDevices = 0;
    for(i=0; i < NV_NO_CONNECTORS; i++)
    {
        ulDeviceMask = (1 << i);
        if(ulConnectedDeviceMask & ulDeviceMask)
        {
            NumDevices++;
        }
    }
    return(NumDevices);

}

//
// Sets some safe default device masks in the hwdeviceExtension based on the current config returned by the miniport.
// Later we will read the TwinViewInfo registry data to see if should use the device masks specified in the registry
//
static VOID SetSafeDeviceMasks_SingleHeadBoard( PHW_DEVICE_EXTENSION HwDeviceExtension)
{
    ULONG i;
    NV_DESKTOP_INFO * pInfo;
    
    pInfo = (NV_DESKTOP_INFO *) &HwDeviceExtension->DeskTopInfo;
    
    pInfo->ulDeviceMask[0] = LOWESTBIT(pInfo->ulConnectedDeviceMask);
    if (pInfo->ulDeviceMask[0])
    {
        return;
    }

    // Something wrong. No devices are connected. So use a default CRT0.
    VideoDebugPrint((0,"Oops!  SetSafeDeviceMasks(): No devices connected. So assume CRT0"));
    pInfo->ulDeviceMask[0] = BITMASK_CRT0;
    return;
}

//
// We need to set up a safe standard mode device mask and deviceDisplay mapping array. This is needed for the special 
// case of the first time boot after driver install since there will no TwinViewInfo structure present after a 
// driver install.
// Later we will read the TwinViewInfo registry data to see if should use the device masks specified in the registry.
//
static VOID SetSafeDeviceMasks_DualHeadBoard( PHW_DEVICE_EXTENSION HwDeviceExtension)
{
    ULONG i;
    ULONG DeviceMask;
    NV_CFGEX_GET_DEVICES_CONFIGURATION_PARAMS nvConfig;
    ULONG status;
    NV_DESKTOP_INFO * pInfo;

    pInfo = (NV_DESKTOP_INFO *) &HwDeviceExtension->DeskTopInfo;

    for (i = 0; i < NV_NO_DACS; i++)
    {
        pInfo->ulDeviceMask[i] = BITMASK_INVALID_DEVICE;
    }


    if (pInfo->ulNumberDacsConnected == 0)
    {
        VideoDebugPrint((0,"Oops!  SetSafeDeviceMasks(): No devices connected. So assume CRT0\n"));
        pInfo->ulDeviceMask[0] = BITMASK_CRT0;
        pInfo->ulConnectedDeviceMask = BITMASK_CRT0;
        pInfo->ulNumberDacsConnected = 1;
        return;
    }

    if (pInfo->ulNumberDacsConnected == 1 || pInfo->ulNumberDacsConnected == 2)
    {
        for (i = 0; i < NV_NO_CONNECTORS; i++)
        {
            if (pInfo->ulConnectedDeviceMask & (1 << i))
            {
                DeviceMask = (1 << i);
                // Ask ResMan what head to use.
                nvConfig.DevicesConfig = DeviceMask;
                nvConfig.OldDevicesConfig = 0;
                nvConfig.OldDevicesAllocationMap = 0;
                nvConfig.DevicesAllocationMap = 0;

                
                if (!RmConfigGetExKernel (HwDeviceExtension->DeviceReference, 
                              NV_CFGEX_GET_DEVICES_CONFIGURATION, &nvConfig, sizeof(NV_CFGEX_GET_DEVICES_CONFIGURATION_PARAMS)))
                {

                    VideoDebugPrint((0, "SetSafeDeviceMasks_DualHeadBoard - Cannot get DEVICES_CONFIG value from RM\n"));
                }
                else
                {
                    if (nvConfig.DevicesAllocationMap > 1)
                    {
                        VideoDebugPrint((0,"Oops! Invalid DevAllocationMap from RM: 0x%x\n",nvConfig.DevicesAllocationMap));
                    }
                    if (nvConfig.DevicesAllocationMap == 0)
                    {
                        pInfo->ulDeviceMask[0] = DeviceMask;
                        pInfo->ulDeviceMask[1] = BITMASK_INVALID_DEVICE;
                        pInfo->ulDeviceDisplay[0] = 0;
                        pInfo->ulDeviceDisplay[1] = 1;
                    }
                    if (nvConfig.DevicesAllocationMap == 1)
                    {
                        pInfo->ulDeviceMask[1] = DeviceMask;
                        pInfo->ulDeviceMask[0] = BITMASK_INVALID_DEVICE;
                        pInfo->ulDeviceDisplay[0] = 1;
                        pInfo->ulDeviceDisplay[1] = 0;
                    }

                }
                return;
            }
        }
        return;
    }

}// end of SetSafeDeviceMasks_DualHeadBoard()

//
// Input: bitmask for a single device
// Return: device type of this device as expected by the display driver and the control panel.
//
ULONG MaskToDeviceType( ULONG ulDeviceMask)
{
    ULONG ulRet;

    if (ulDeviceMask & BITMASK_ALL_CRT) ulRet = MONITOR_TYPE_VGA; 
    else if (ulDeviceMask & BITMASK_ALL_DFP) ulRet = MONITOR_TYPE_FLAT_PANEL;
    else if (ulDeviceMask & BITMASK_ALL_TV) ulRet = MONITOR_TYPE_NTSC; 
    else ulRet = INVALID_DEVICE_TYPE;

    return(ulRet);
}

//
// Input: bitmask for a single device
// Return: device type of this device as expected by the common modeset code and FindModeEntry().
//
ULONG MaskToFindModeEntryDeviceType( ULONG ulDeviceMask)
{
    ULONG ulRet;

    if (ulDeviceMask & BITMASK_ALL_CRT) ulRet = DEVTYPE_CRT; 
    else if (ulDeviceMask & BITMASK_ALL_DFP) ulRet = DEVTYPE_DFP;
    else if (ulDeviceMask & BITMASK_ALL_TV) ulRet = DEVTYPE_TV; 
    else ulRet = INVALID_DEVICE_TYPE;

    return(ulRet);
}

//
// Input: bitmask for a single device
// Return: TV Format of this device. Currently returns only NTSC_M format
//
ULONG MaskToTVFormat( ULONG ulDeviceMask)
{
    return(NTSC_M);
}

//
// If the system is a desktop, this routine does nothing.
// If the system is a laptop, we should use the output deive posted by the BIOS for the desktop.
// We honor the BIOS output only at boot time.
// BUGBUG: Currently this function only handles a single output device posted by the BIOS.
// The code does not yet support when the BIOS posts to two devices (sort of BIOS clone mode).
// We follow the following Kevin K's specs for laptops:
//      If a single bit is enabled, that should be the primary boot device.  
//      If more than one bit is enabled, and Windows was previously in standard or clone mode,
//          the driver should enter clone mode with the associated devices.  I would give priority
//          (what gets chosen as the primary) to the LCD, CRT, then TV (in that order if bit enabled).
//      If more than one bit is enabled, and Windows was previously in extended mode,
//          the driver should remain in extended mode with the associated devices.  I would give
//          priority (what gets chosen as the primary) to the LCD, CRT, then TV (in that order if bit enabled).
//
//
VOID HandleBIOSHeads(PHW_DEVICE_EXTENSION HwDeviceExtension)
{
    ULONG ulHead;
    ULONG Status, RetSize;
    NV_CFGEX_GET_DEVICES_CONFIGURATION_PARAMS nvConfig;

    NV_DESKTOP_INFO * pInfo;
    NV_DESKTOP_INFO sOri;
    ULONG DeviceMask[NV_NO_DACS] = {0, 0};
    ULONG i, status;
    NVTWINVIEWDATA sData;
    NVTWINVIEW_DEVICE_TYPE_DATA  * pHeadData;
    NVTWINVIEW_DEVICE_TYPE_DATA  * pHead0, * pHead1;
    ULONG TwinViewDataPresent;
    ULONG Width, Height, Depth, Refresh;
    ULONG ulPrimaryHead;
    ULONG SwapHeadMapping;
    ULONG ulTmp;
    

    pInfo = (NV_DESKTOP_INFO *) &HwDeviceExtension->DeskTopInfo;
    
    // Save this structure off for later use.
    sOri = *pInfo;
    
    //
    // Initialize the registry keys to zero.
    //
    HwDeviceExtension->Mobile = 0;
    HwDeviceExtension->UseHWSelectedDevice = 0;
    HwDeviceExtension->GDIModePruning = 0;
    HwDeviceExtension->UseHotKeySameMode = 0;
    HwDeviceExtension->PanScanSelection = 0;
    HwDeviceExtension->EnableModesetStopImage = 0;
    HwDeviceExtension->EnumerateTVAsACPIDevice = 0;

    // Read the registry entry "Mobile" to see if we are on a desktop or laptop.
    Status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       L"Mobile",
                                       FALSE,
                                       NVRegistryCallback,
                                       &(HwDeviceExtension->Mobile));
    VideoDebugPrint((3,"HwDeviceExtension->Mobile registry value: 0x%x\n",HwDeviceExtension->Mobile));

    Status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       L"UseHWSelectedDevice",
                                       FALSE,
                                       NVRegistryCallback,
                                       &(HwDeviceExtension->UseHWSelectedDevice));
    VideoDebugPrint((3,"HwDeviceExtension->UseHWSelectedDevice registry value: 0x%x\n",HwDeviceExtension->UseHWSelectedDevice));

    Status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       L"GDIModePruning",
                                       FALSE,
                                       NVRegistryCallback,
                                       &(HwDeviceExtension->GDIModePruning));
    VideoDebugPrint((3,"HwDeviceExtension->GDIModePruning registry value: 0x%x\n",HwDeviceExtension->GDIModePruning));

    Status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       L"UseHotKeySameMode",
                                       FALSE,
                                       NVRegistryCallback,
                                       &(HwDeviceExtension->UseHotKeySameMode));
    VideoDebugPrint((3,"HwDeviceExtension->UseHotKeySameMode registry value: 0x%x\n",HwDeviceExtension->UseHotKeySameMode));

    Status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       L"PanScanSelection",
                                       FALSE,
                                       NVRegistryCallback,
                                       &(HwDeviceExtension->PanScanSelection));
    VideoDebugPrint((3,"HwDeviceExtension->PanScanSelection registry value: 0x%x\n",HwDeviceExtension->PanScanSelection));

    Status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       L"EnableModesetStopImage",
                                       FALSE,
                                       NVRegistryCallback,
                                       &(HwDeviceExtension->EnableModesetStopImage));
    VideoDebugPrint((3,"HwDeviceExtension->EnableModesetStopImage registry value: 0x%x\n",HwDeviceExtension->EnableModesetStopImage));

    Status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       L"EnumerateTVAsACPIDevice",
                                       FALSE,
                                       NVRegistryCallback,
                                       &(HwDeviceExtension->EnumerateTVAsACPIDevice));
    VideoDebugPrint((3,"HwDeviceExtension->EnumerateTVAsACPIDevice registry value: 0x%x\n",HwDeviceExtension->EnumerateTVAsACPIDevice));


    if (HwDeviceExtension->UseHWSelectedDevice)
    {
        ULONG BIOSDeviceMask;
        // We are on a laptop (either dell or toshiba).
        // So we should use the same devices that the BIOS used for the desktop.
        // We honor the BIOS's selections only at boot time.
        if (HwDeviceExtension->SystemModeSetHasOccurred)
        {
            // A high res modeset has already occurred. So don't bother about the BIOS
            return;
           
        }
        
        // Read the twinviewinfo data from the registry if it is present.
        TwinViewDataPresent = 0;
        status = NVGetRegistryTwinviewInfo(HwDeviceExtension, &sData);
        if (status == NO_ERROR)
        {
            if (TwinViewDataOK(HwDeviceExtension, &sData))
            {
                TwinViewDataPresent = 1;
            }
        }
                


        if (!RmConfigGetKernel(HwDeviceExtension->DeviceReference,
                                  NV_CFG_GET_BOOT_DEVICES, &BIOSDeviceMask))
        {
            VideoDebugPrint((0,"Oops!    NV_CFG_GET_BOOT_DEVICES failed\n"));
        }

        VideoDebugPrint((1,"BIOSDeviceMask returned by ResMan: 0x%x\n",BIOSDeviceMask));

        // see if the BIOS had posted on a single device or two devices.
        if (ConvertConnectedMaskToNumDevices(BIOSDeviceMask) == 1)
        {
            // BIOS posted on a single device.
            if (TwinViewDataPresent == 0)
            {
                ulHead = pInfo->ulDeviceDisplay[0];
                if (pInfo->ulDeviceMask[ulHead] == BIOSDeviceMask)
                {
                    // Nothing to do.
                    return;
                }

            }

            // Ask ResMan for which head to use.
            // Ask ResMan what head to use.
            nvConfig.DevicesConfig = BIOSDeviceMask;
            nvConfig.OldDevicesConfig = 0;
            nvConfig.OldDevicesAllocationMap = 0;
            nvConfig.DevicesAllocationMap = 0;
            if (!RmConfigGetExKernel(HwDeviceExtension->DeviceReference,
                                     NV_CFGEX_GET_DEVICES_CONFIGURATION, &nvConfig, sizeof(NV_CFGEX_GET_DEVICES_CONFIGURATION_PARAMS)))
            {

                VideoDebugPrint((0, "SetSafeDeviceMasks_DualHeadBoard - Cannot get DEVICES_CONFIG value from RM\n"));
            }
            else
            {
                if (nvConfig.DevicesAllocationMap > 1)
                {
                    VideoDebugPrint((0,"Oops! Invalid DevAllocationMap from RM: 0x%x\n",nvConfig.DevicesAllocationMap));
                }
                if (nvConfig.DevicesAllocationMap == 0)
                {
                    pInfo->ulDeviceMask[0] = BIOSDeviceMask;
                    pInfo->ulDeviceMask[1] = BITMASK_INVALID_DEVICE;
                    pInfo->ulDeviceDisplay[0] = 0;
                    pInfo->ulDeviceDisplay[1] = 1;
                }
                if (nvConfig.DevicesAllocationMap == 1)
                {
                    pInfo->ulDeviceMask[1] = BIOSDeviceMask;
                    pInfo->ulDeviceMask[0] = BITMASK_INVALID_DEVICE;
                    pInfo->ulDeviceDisplay[0] = 1;
                    pInfo->ulDeviceDisplay[1] = 0;
                }
            }

            if (TwinViewDataPresent)
            {
                ulHead = sData.dwDeviceDisplay[0];
                if ((sData.dwState == NVTWINVIEW_STATE_NORMAL || sData.dwState == NVTWINVIEW_STATE_DUALVIEW) && 
                    sData.NVTWINVIEWUNION.nvtwdevdata[ulHead].dwDeviceMask == BIOSDeviceMask)
                {
                    // Nothing to do
                    return;
                }
                //
                // If the TwinViewInfo registry exists, we need to patch it to reflect the BIOS.
                // For the single BIOS device, we simply invalidate the registry.
                //
                // This functions also writes the invalidated TwinViewInfo structure to the registry.
                InvalidateTwinViewData(HwDeviceExtension, &sData);
            }
            
        }


        if (ConvertConnectedMaskToNumDevices(BIOSDeviceMask) >= 2)
        {
            // We need to do this only if the connfig has been changed underneath us such as by the
            // user hot-keying around in BIOS mode.
            if (pInfo->ulNumberDacsActive == 2)
            {
                // So we were in clone or multimon mode. Were we using the same BIOS devices?
                if ((pInfo->ulDeviceMask[0] | pInfo->ulDeviceMask[1]) == BIOSDeviceMask)
                {
                    // Nothing to do.
                    return;
                }

            }
            if (TwinViewDataPresent && (sData.dwState == NVTWINVIEW_STATE_CLONE || sData.dwState == NVTWINVIEW_STATE_SPAN))
            {
                // So we were in clone or multimon mode. Were we using the same BIOS devices?
                if ((sData.NVTWINVIEWUNION.nvtwdevdata[0].dwDeviceMask | sData.NVTWINVIEWUNION.nvtwdevdata[1].dwDeviceMask) 
                    == BIOSDeviceMask)
                {
                    // Nothing to do.
                    return;
                }

            }


            pInfo->ulDeviceDisplay[0] = 0;
            pInfo->ulDeviceDisplay[1] = 1;

            // Grab the first output device.
            DeviceMask[0] = LOWESTBIT(BIOSDeviceMask);
            DeviceMask[1] = LOWESTBIT(BIOSDeviceMask & ~DeviceMask[0]);

            // Ask ResMan what head to use for the devices.
            nvConfig.DevicesConfig = DeviceMask[0] | DeviceMask[1];
            nvConfig.OldDevicesConfig = 0;
            nvConfig.OldDevicesAllocationMap = 0;
            nvConfig.DevicesAllocationMap = 0;
            status = RmConfigGetExKernel (HwDeviceExtension->DeviceReference,
                                          NV_CFGEX_GET_DEVICES_CONFIGURATION, &nvConfig, sizeof(NV_CFGEX_GET_DEVICES_CONFIGURATION_PARAMS));
            if (status == 0)
            {

                VideoDebugPrint((0, "Oops! SetSafeDeviceMasks_DualHeadBoard - Cannot get DEVICES_CONFIG value from RM\n"));
                // Set to a safe single head mode.
                pInfo->ulNumberDacsConnected = 1;
                pInfo->ulConnectedDeviceMask = DeviceMask[0];
                pInfo->ulAllDeviceMask = DeviceMask[0];
                pInfo->ulDeviceMask[0] = DeviceMask[0];
                pInfo->ulDeviceMask[1] = BITMASK_INVALID_DEVICE;
            }
            else
            {
                // We got a successful head assignment for both the devices.
                switch (nvConfig.DevicesAllocationMap)
                {
                case 0x10:
                    // Head assignment 1, 0
                    pInfo->ulDeviceMask[0] = DeviceMask[0];
                    pInfo->ulDeviceMask[1] = DeviceMask[1];

                    break;
                case 0x01:
                    // Head assignment 0, 1
                    pInfo->ulDeviceMask[0] = DeviceMask[1];
                    pInfo->ulDeviceMask[1] = DeviceMask[0];
                    break;
                default:
                    VideoDebugPrint((0,"Oops! Unexpected head allocationMap: 0x%x\n",  nvConfig.DevicesAllocationMap));
                    // Set to a safe single head mode.
                    pInfo->ulNumberDacsConnected = 1;
                    pInfo->ulConnectedDeviceMask = DeviceMask[0];
                    pInfo->ulAllDeviceMask = DeviceMask[0];
                    pInfo->ulDeviceMask[0] = DeviceMask[0];
                    pInfo->ulDeviceMask[1] = BITMASK_INVALID_DEVICE;
                    break;
                }

                for (i=0; i < pInfo->ulNumberDacsConnected; i++)
                {
                    pInfo->ulDeviceType[i] = MaskToDeviceType(pInfo->ulDeviceMask[i]);
                    pInfo->ulTVFormat[i] = MaskToTVFormat(pInfo->ulDeviceMask[i]);
                }
            }

            //
            // If the TwinViewInfo registry exists, we need to patch it to reflect the BIOS.
            // We patch the following fields.
            // - dwState
            // - dwDeviceDisplay
            // - DeviceMask
            // - DeviceType.
            // Note: We don't need to worry about the desktop resolution here since the DELL or Toshiba logic kicks in
            // DrvGetModes(). For example, on DELL where there is no pan-scan in standard mode, Windows may be in 16x12
            // on CRT Standard mode, the user may hotkey to DFP in BIOS, then Win2K will fall back to 8x6 or 6x4 since DrvGetModes() 
            // will filter out 16x12. TODO: It could be handled more elegantly by figuring out the best-fit mode for this
            // BIOS device and writing it into the ToshibaHotKeyMode registry entry. Later.
            //
            if (TwinViewDataPresent)
            {
                Width = sData.DeskTopRectl.right - sData.DeskTopRectl.left;
                Height = sData.DeskTopRectl.bottom - sData.DeskTopRectl.top;
                ulPrimaryHead = sData.dwDeviceDisplay[0];
                Depth = sData.NVTWINVIEWUNION.nvtwdevdata[ulPrimaryHead].dwBpp;
                Refresh = sData.NVTWINVIEWUNION.nvtwdevdata[ulPrimaryHead].dwRefresh;
            }
            else
            {

                Width = sOri.ulDesktopWidth;
                Height = sOri.ulDesktopHeight;
                ulPrimaryHead = sOri.ulDeviceDisplay[0];
                Depth = sOri.ulDisplayPixelDepth[ulPrimaryHead];
                Refresh = sOri.ulDisplayRefresh[ulPrimaryHead];
            }


            // Set the desktop resolution.
            sData.DeskTopRectl.left = 0;
            sData.DeskTopRectl.top = 0;
            sData.DeskTopRectl.right = Width;
            sData.DeskTopRectl.bottom = Height;

            
            // If it does not exist or is standard/clone mode, create a full registry entry for clone mode.
            // Lets handle all cases the same for now. Later, we will need to remember the modes per device.
            if (TwinViewDataPresent == 0 || 
               (TwinViewDataPresent == 1 && (sData.dwState == NVTWINVIEW_STATE_NORMAL ||
                                             sData.dwState == NVTWINVIEW_STATE_DUALVIEW ||
                                             sData.dwState == NVTWINVIEW_STATE_CLONE)))
            {
                sData.dwState = NVTWINVIEW_STATE_CLONE;
                sData.dwAllDeviceMask = pInfo->ulAllDeviceMask;
                sData.dwConnectedDeviceMask = pInfo->ulConnectedDeviceMask;

                for (i=0; i < NV_NO_DACS; i++)
                {
                    sData.dwDeviceDisplay[i] = pInfo->ulDeviceDisplay[i];

                    pHeadData = &(sData.NVTWINVIEWUNION.nvtwdevdata[i]);
                    pHeadData->dwDeviceMask = pInfo->ulDeviceMask[i];
                    pHeadData->dwDeviceType = pInfo->ulDeviceType[i];
                    pHeadData->dwTVFormat = pInfo->ulTVFormat[i];
                    //
                    // BUGBUG we should really remember the last mode info for each device and use that. Later.
                    //
                    pHeadData->dwBpp = Depth;
                    pHeadData->dwRefresh = Refresh;
                    pHeadData->VirtualRectl.left = 0;
                    pHeadData->VirtualRectl.top = 0;
                    pHeadData->VirtualRectl.right = Width;
                    pHeadData->VirtualRectl.bottom = Height;
                    pHeadData->PhysicalRectl.left = 0;
                    pHeadData->PhysicalRectl.top = 0;
                    pHeadData->PhysicalRectl.right = Width;
                    pHeadData->PhysicalRectl.bottom = Height;
                }
            }

            // Handle the special case of when windows is in spanning mode and BIOS is in clone mode.
            // Per Kevin's specs we need to put windows into spanning mode with the BIOS heads.
            if (TwinViewDataPresent == 1 && sData.dwState == NVTWINVIEW_STATE_SPAN)
            {
                sData.dwAllDeviceMask = pInfo->ulAllDeviceMask;
                sData.dwConnectedDeviceMask = pInfo->ulConnectedDeviceMask;

                for (i=0; i < NV_NO_DACS; i++)
                {
                    sData.dwDeviceDisplay[i] = pInfo->ulDeviceDisplay[i];
                    pHeadData = &(sData.NVTWINVIEWUNION.nvtwdevdata[i]);
                    pHeadData->dwDeviceMask = pInfo->ulDeviceMask[i];
                    pHeadData->dwDeviceType = pInfo->ulDeviceType[i];
                    pHeadData->dwTVFormat = pInfo->ulTVFormat[i];
                }
            }

            // Now we have chosen wither clone or span mode.
            // Always make LCD/CRT the primary in that order.
            // We change the dvice mapping array to reflect that.
            ChoosePrimaryDevice(HwDeviceExtension, &sData);

            // Now write the TwinViewInfo to registry.
            NVSetRegistryTwinviewInfo(HwDeviceExtension, &sData);
        }
    }

    return;
}


VOID HandleDualviewHeads(PHW_DEVICE_EXTENSION HwDeviceExtension, BOOLEAN bEnableDualView)
{
#if (_WIN32_WINNT >= 0x0500)
    ULONG ulHead;
    ULONG i, Status, TwinViewDataPresent = 0;
    NV_CFGEX_GET_DEVICES_CONFIGURATION_PARAMS nvConfig;

    NV_DESKTOP_INFO *pInfo, *pInfo2;
    ULONG DeviceMask[NV_NO_DACS] = {0, 0}, SecondHeadMask;
    NVTWINVIEWDATA sData;
    NVTWINVIEW_DEVICE_TYPE_DATA  *pHeadData;
    ULONG ulConnectedDeviceMask;

    //
    // If Dualview, use ResMan default head setting
    //
    if (HwDeviceExtension->pDualViewAssociatedDE)
    {
        pInfo = (NV_DESKTOP_INFO *) &HwDeviceExtension->DeskTopInfo;
        pInfo2 = (NV_DESKTOP_INFO *) &HwDeviceExtension->pDualViewAssociatedDE->DeskTopInfo;
    
        // Read the twinviewinfo data from the registry if it is present.
        VideoPortZeroMemory(&sData, sizeof(sData));
        if (NVGetRegistryTwinviewInfo(HwDeviceExtension, &sData) == NO_ERROR)
        {
            if (TwinViewDataOK(HwDeviceExtension, &sData))
            {
                TwinViewDataPresent = 1;
            }
            else
            {
                VideoPortZeroMemory(&sData, sizeof(sData));
            }
        }

        if (HwDeviceExtension->ulDualViewStatus & DUALVIEW_STATUS_MOBILE)
        {
            DeviceMask[0] = (pInfo->ulConnectedDeviceMask & BITMASK_ALL_DFP);
            if (DeviceMask[0] == 0)
            {
                // A laptop wothout LCD ??????
                ASSERT(FALSE);
                DeviceMask[0] = pInfo->ulAllDeviceMask & BITMASK_ALL_DFP;
                if (DeviceMask[0] == 0)
                    DeviceMask[0] = BITMASK_ALL_DFP;
            }
            DeviceMask[0] = LOWESTBIT(DeviceMask[0]);

            // Continue further and grab the next device.
            DeviceMask[1] = (pInfo->ulConnectedDeviceMask & ~DeviceMask[0]);
            if (DeviceMask[1] == 0)
            {
                // If on laptop, there is no devices other than LCD, force the secondary to be CRT0
                DeviceMask[1] = pInfo->ulAllDeviceMask & (~BITMASK_ALL_DFP);
                if (DeviceMask[1] == 0)
                {
                    ASSERT(FALSE);
                    DeviceMask[1] = BITMASK_CRT0;
                }
            }
            DeviceMask[1] = LOWESTBIT(DeviceMask[1]);

            ulConnectedDeviceMask = DeviceMask[0] | DeviceMask[1];
        }
        else
        {
            ulConnectedDeviceMask = pInfo->ulConnectedDeviceMask;
            // If on LCD/CRT/TV all exist, pick CLD and CRT
            if ((ulConnectedDeviceMask & BITMASK_ALL_CRT) &&
                (ulConnectedDeviceMask & BITMASK_ALL_TV) &&
                (ulConnectedDeviceMask & BITMASK_ALL_DFP))
                ulConnectedDeviceMask &= ~BITMASK_ALL_TV;
        }

        // Config the heads
        if (ConvertConnectedMaskToNumDevices(ulConnectedDeviceMask) > 1)
        {
            // Grab the first output device.
            DeviceMask[0] = LOWESTBIT(ulConnectedDeviceMask);
            DeviceMask[1] = LOWESTBIT(ulConnectedDeviceMask & ~DeviceMask[0]);

            // Ask ResMan what head to use for the devices.
            nvConfig.DevicesConfig = DeviceMask[0] | DeviceMask[1];
            nvConfig.OldDevicesConfig = 0;
            nvConfig.OldDevicesAllocationMap = 0;
            nvConfig.DevicesAllocationMap = 0;
            Status = RmConfigGetExKernel (HwDeviceExtension->DeviceReference,
                                          NV_CFGEX_GET_DEVICES_CONFIGURATION, &nvConfig, sizeof(NV_CFGEX_GET_DEVICES_CONFIGURATION_PARAMS));
            if (Status == 0)
            {
                VideoDebugPrint((0, "Oops! HandleDualViewHeads - Cannot get DEVICES_CONFIG value from RM\n"));
                ASSERT(FALSE);
                if (HwDeviceExtension->ulDualViewStatus & DUALVIEW_STATUS_MOBILE)
                {
                    pInfo->ulDeviceMask[0] = DeviceMask[1];
                    pInfo->ulDeviceMask[1] = DeviceMask[0];
                }
                else
                {
                    // Set to a safe single head mode.
                    pInfo->ulNumberDacsConnected = 1;
                    pInfo->ulConnectedDeviceMask = DeviceMask[0];
                    pInfo->ulAllDeviceMask = DeviceMask[0];
                    pInfo->ulDeviceMask[0] = DeviceMask[0];
                    pInfo->ulDeviceMask[1] = BITMASK_INVALID_DEVICE;
                }
            }
            else
            {
                // We got a successful head assignment for both the devices.
                switch (nvConfig.DevicesAllocationMap)
                {
                case 0x10:
                    // Head assignment 1, 0
                    pInfo->ulDeviceMask[0] = DeviceMask[0];
                    pInfo->ulDeviceMask[1] = DeviceMask[1];
                    break;
                case 0x01:
                    // Head assignment 0, 1
                    pInfo->ulDeviceMask[0] = DeviceMask[1];
                    pInfo->ulDeviceMask[1] = DeviceMask[0];
                    break;
                default:
                    VideoDebugPrint((0,"Oops! Unexpected head allocationMap: 0x%x\n",  nvConfig.DevicesAllocationMap));
                    ASSERT(FALSE);
                    // Set to a safe single head mode.
                    pInfo->ulNumberDacsConnected = 1;
                    pInfo->ulConnectedDeviceMask = DeviceMask[0];
                    pInfo->ulAllDeviceMask = DeviceMask[0];
                    pInfo->ulDeviceMask[0] = DeviceMask[0];
                    pInfo->ulDeviceMask[1] = BITMASK_INVALID_DEVICE;
                    break;
                }

                for (i = 0; i < pInfo->ulNumberDacsConnected; i++)
                {
                    pInfo->ulDeviceType[i] = MaskToDeviceType(pInfo->ulDeviceMask[i]);
                    pInfo->ulTVFormat[i] = MaskToTVFormat(pInfo->ulDeviceMask[i]);
                }

                pInfo->ulDeviceDisplay[0] = 0;
                pInfo->ulDeviceDisplay[1] = 1;
                if ((HwDeviceExtension->ulDualViewStatus & DUALVIEW_STATUS_MOBILE) &&
                    (pInfo->ulDeviceMask[1] & BITMASK_ALL_DFP))
                {
                    pInfo->ulDeviceDisplay[0] = 1;
                    pInfo->ulDeviceDisplay[1] = 0;
                }

                VideoDebugPrint((2, "HandleDualviewHeads: ulDeviceMask[]={0x%x,0x%x}\n",
                                 pInfo->ulDeviceMask[0], pInfo->ulDeviceMask[1]));
                VideoDebugPrint((2, "                     ulDeviceDisplay[]={%d,%d}\n",
                                 pInfo->ulDeviceDisplay[0], pInfo->ulDeviceDisplay[1]));
            }

            // Second view reversed the head mapping
            for (i = 0; i < NV_NO_DACS; i++)
            {
                pInfo2->ulDeviceMask[i] = pInfo->ulDeviceMask[i];
                pInfo2->ulDeviceType[i] = pInfo->ulDeviceType[i];
                pInfo2->ulTVFormat[i] = pInfo->ulTVFormat[i];
            }
            sData.dwDeviceDisplay[0] = pInfo2->ulDeviceDisplay[1] = pInfo->ulDeviceDisplay[0];
            sData.dwDeviceDisplay[1] = pInfo2->ulDeviceDisplay[0] = pInfo->ulDeviceDisplay[1];
            sData.dwAllDeviceMask = pInfo2->ulAllDeviceMask = pInfo->ulAllDeviceMask;
            sData.dwConnectedDeviceMask = pInfo2->ulConnectedDeviceMask = pInfo->ulConnectedDeviceMask;

            if (bEnableDualView)
            {
                pInfo->ulTwinView_State = NVTWINVIEW_STATE_DUALVIEW;
            }
            else
            {
                if (sData.dwState == NVTWINVIEW_STATE_DUALVIEW || sData.dwState == NVTWINVIEW_STATE_NORMAL)
                {
                    pInfo->ulTwinView_State = NVTWINVIEW_STATE_NORMAL;
                    // Put secondary head to invalid device for primary view
                    ulHead = pInfo->ulDeviceDisplay[1];
                    pInfo->ulDeviceMask[ulHead] = BITMASK_INVALID_DEVICE;
                }
                else
                    pInfo->ulTwinView_State = sData.dwState;
            }
            sData.dwState = pInfo->ulTwinView_State;

            for (i = 0; i < NV_NO_DACS; i++)
            {
                sData.dwDeviceDisplay[i] = pInfo->ulDeviceDisplay[i];

                pHeadData = &(sData.NVTWINVIEWUNION.nvtwdevdata[i]);
                pHeadData->dwDeviceMask = pInfo->ulDeviceMask[i];
                pHeadData->dwDeviceType = pInfo->ulDeviceType[i];
                pHeadData->dwTVFormat = pInfo->ulTVFormat[i];
            }

            // Now write the TwinViewInfo to registry.
            // Primary view needs to be updated only when Dualview is enabled or TwinViewData state
            // is not Spanning or Clone
            // A scenario would be from Dualview to Spanning mode.
            if (bEnableDualView || !TwinViewDataPresent ||
                sData.dwState == NVTWINVIEW_STATE_DUALVIEW || sData.dwState == NVTWINVIEW_STATE_NORMAL)
            {
                VideoPortZeroMemory(&sData.DeskTopRectl, sizeof(sData.DeskTopRectl));
                NVSetRegistryTwinviewInfo(HwDeviceExtension, &sData);
            }

            // Second view should always be updated.  It's only reversed head setting from primary view.
            // The state is always DUALVIEW
            pInfo2->ulTwinView_State = sData.dwState = NVTWINVIEW_STATE_DUALVIEW;
            for (i = 0; i < NV_NO_DACS; i++)
            {
                sData.dwDeviceDisplay[i] = pInfo2->ulDeviceDisplay[i];
                pHeadData = &(sData.NVTWINVIEWUNION.nvtwdevdata[i]);
                pHeadData->dwDeviceMask = pInfo2->ulDeviceMask[i];
            }
            VideoPortZeroMemory(&sData.DeskTopRectl, sizeof(sData.DeskTopRectl));
            NVSetRegistryTwinviewInfo(HwDeviceExtension->pDualViewAssociatedDE, &sData);
        }
        else
        {
            // At this point, we know CRT and LCD have to exist
            ASSERT(FALSE);
        }
    }
#endif
}

//
// Returns true if the tinviewData is safe. Otherwise returns FALSE>
//
BOOLEAN TwinViewDataOK(PHW_DEVICE_EXTENSION hwDeviceExtension, NVTWINVIEWDATA *pData)
{
    BOOLEAN bRet = TRUE;
    ULONG i, ulHead, ulDeviceMask;

    if (pData->DeskTopRectl.left == 0 && pData->DeskTopRectl.top == 0 && 
        pData->DeskTopRectl.right == 0 && pData->DeskTopRectl.bottom == 0)
    {
        
        return(FALSE);
    }

    if (pData->dwState != NVTWINVIEW_STATE_SPAN && pData->dwState != NVTWINVIEW_STATE_CLONE &&
        pData->dwState != NVTWINVIEW_STATE_DUALVIEW && pData->dwState != NVTWINVIEW_STATE_NORMAL) 
    {
       
        return(FALSE);
    }

    if ((pData->dwState == NVTWINVIEW_STATE_NORMAL || pData->dwState == NVTWINVIEW_STATE_DUALVIEW) && pData->dwDeviceDisplay[0] > 1)
    {
       
       return(FALSE);
    }

    if (pData->dwState == NVTWINVIEW_STATE_SPAN || pData->dwState == NVTWINVIEW_STATE_CLONE)
    {
        if (pData->dwDeviceDisplay[0] > 1 || 
            pData->dwDeviceDisplay[1] > 1 ||
            (pData->dwDeviceDisplay[0] == 0 && pData->dwDeviceDisplay[1] == 0) ||
            (pData->dwDeviceDisplay[0] == 1 && pData->dwDeviceDisplay[1] == 1))
        {
       
            return(FALSE);
        }
    }

    if (pData->dwState == NVTWINVIEW_STATE_SPAN || pData->dwState == NVTWINVIEW_STATE_CLONE) 
    {
        if (pData->NVTWINVIEWUNION.nvtwdevdata[0].dwBpp != pData->NVTWINVIEWUNION.nvtwdevdata[1].dwBpp)
        {
       
            return(FALSE);
        }
    }


    if (pData->NVTWINVIEWUNION.nvtwdevdata[0].dwBpp == 0 && pData->NVTWINVIEWUNION.nvtwdevdata[1].dwBpp == 0)
    {
       
        return(FALSE);
    }

    if (pData->NVTWINVIEWUNION.nvtwdevdata[0].dwRefresh == 0 && pData->NVTWINVIEWUNION.nvtwdevdata[1].dwRefresh == 0)
    {
       
        return(FALSE);
    }
    if (pData->NVTWINVIEWUNION.nvtwdevdata[0].VirtualRectl.left == 0 && pData->NVTWINVIEWUNION.nvtwdevdata[0].VirtualRectl.top == 0 
        && pData->NVTWINVIEWUNION.nvtwdevdata[0].VirtualRectl.right == 0 && pData->NVTWINVIEWUNION.nvtwdevdata[0].VirtualRectl.bottom == 0 
        && pData->NVTWINVIEWUNION.nvtwdevdata[1].VirtualRectl.left == 0 && pData->NVTWINVIEWUNION.nvtwdevdata[1].VirtualRectl.top == 0 
        && pData->NVTWINVIEWUNION.nvtwdevdata[1].VirtualRectl.right == 0 && pData->NVTWINVIEWUNION.nvtwdevdata[1].VirtualRectl.bottom == 0)
    {
       
        return(FALSE);
    }

    if (pData->NVTWINVIEWUNION.nvtwdevdata[0].PhysicalRectl.left == 0 && pData->NVTWINVIEWUNION.nvtwdevdata[0].PhysicalRectl.top == 0 
        && pData->NVTWINVIEWUNION.nvtwdevdata[0].PhysicalRectl.right == 0 && pData->NVTWINVIEWUNION.nvtwdevdata[0].PhysicalRectl.bottom == 0 
        && pData->NVTWINVIEWUNION.nvtwdevdata[1].PhysicalRectl.left == 0 && pData->NVTWINVIEWUNION.nvtwdevdata[1].PhysicalRectl.top == 0 
        && pData->NVTWINVIEWUNION.nvtwdevdata[1].PhysicalRectl.right == 0 && pData->NVTWINVIEWUNION.nvtwdevdata[1].PhysicalRectl.bottom == 0)
    {
       
        return(FALSE);
    }

    // If there is just one head on the board and the registry says any mode other than normal, mark it as invalid.
    if (hwDeviceExtension)
    {

        if (hwDeviceExtension->DeskTopInfo.ulNumberDacsOnBoard == 1 &&
            (pData->dwState == NVTWINVIEW_STATE_SPAN || pData->dwState == NVTWINVIEW_STATE_CLONE))
        {
       
            return(FALSE);
        }
    }

    // Check to see if the user has swapped the board
    if (pData->dwAllDeviceMask != hwDeviceExtension->DeskTopInfo.ulAllDeviceMask)
    {
        return(FALSE);
    }

    // Now check to see if the output device specified in the registry are actually present in this
    // current configuration.
    if (pData->dwState == NVTWINVIEW_STATE_NORMAL || pData->dwState == NVTWINVIEW_STATE_DUALVIEW)
    {
        ulHead = pData->dwDeviceDisplay[0];
        ulDeviceMask = pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwDeviceMask;
        if (!(ulDeviceMask & hwDeviceExtension->DeskTopInfo.ulConnectedDeviceMask))
        {
            // The specified device is no longer connected
            return(FALSE);
        }
    }
    
    // Check the connected devices for clone/span modes.
    if (pData->dwState == NVTWINVIEW_STATE_CLONE || pData->dwState == NVTWINVIEW_STATE_SPAN)
    {
        ulDeviceMask = pData->NVTWINVIEWUNION.nvtwdevdata[0].dwDeviceMask;
        if (!(ulDeviceMask & hwDeviceExtension->DeskTopInfo.ulConnectedDeviceMask))
        {
            // The specified device is no longer connected
            return(FALSE);
        }
        ulDeviceMask = pData->NVTWINVIEWUNION.nvtwdevdata[1].dwDeviceMask;
        if (!(ulDeviceMask & hwDeviceExtension->DeskTopInfo.ulConnectedDeviceMask))
        {
            // The specified device is no longer connected
            return(FALSE);
        }
    }
    return(TRUE);
}

//
// Invalidates the twinview data and writes back to the registry. Ideally we would have just wanted to delete
// the TwinViewInfo structure registry entry but we can not delete registry entries from the miniport.
//
VOID InvalidateTwinViewData(PHW_DEVICE_EXTENSION hwDeviceExtension, NVTWINVIEWDATA *pData)
{
    VideoPortZeroMemory(&pData->DeskTopRectl, sizeof(pData->DeskTopRectl));
    
    NVSetRegistryTwinviewInfo(hwDeviceExtension, pData);
    return;
}

//
// Select the primary device correctly.
// For laptops, we want LCD/CRT/TV in that order.
// For desktops, this function does nothing.
// This function updates both the pData and the hwDevExt->DeskTopInfo structures.
//
VOID ChoosePrimaryDevice(PHW_DEVICE_EXTENSION HwDeviceExtension, NVTWINVIEWDATA *pData)
{
    NV_DESKTOP_INFO * pInfo;
    NVTWINVIEW_DEVICE_TYPE_DATA  * pHead0, * pHead1;
    ULONG SwapHeadMapping;
    ULONG ulTmp;

    pInfo = &HwDeviceExtension->DeskTopInfo;
    pHead0 = &(pData->NVTWINVIEWUNION.nvtwdevdata[0]);
    pHead1 = &(pData->NVTWINVIEWUNION.nvtwdevdata[1]);

    // Initialize the flag to zero.
    SwapHeadMapping = 0;
    if (pData->dwDeviceDisplay[0] == 0)
    {
        // Head0 is the primary. 
        // Now check the physical head types to make DFP the primary device.
        if ((pHead0->dwDeviceType == MONITOR_TYPE_VGA && pHead1->dwDeviceType == MONITOR_TYPE_FLAT_PANEL) ||
            ((pHead0->dwDeviceType == MONITOR_TYPE_NTSC || pHead0->dwDeviceType == MONITOR_TYPE_PAL) && 
             pHead1->dwDeviceType == MONITOR_TYPE_FLAT_PANEL))
        {
            // We need to swap the primary device.
            SwapHeadMapping = 1;
        }

        // Now check the physical head types to make CRT the primary device.
        if ((pHead0->dwDeviceType == MONITOR_TYPE_NTSC || pHead0->dwDeviceType == MONITOR_TYPE_PAL) && 
            pHead1->dwDeviceType == MONITOR_TYPE_VGA)
        {
            // We need to swap the primary device.
            SwapHeadMapping = 1;
        }
    }

    if (pData->dwDeviceDisplay[0] == 1)
    {
        // Head1 is the primary. 
        // Now check the physical head types to make DFP the primary device.
        if ((pHead1->dwDeviceType == MONITOR_TYPE_VGA && pHead0->dwDeviceType == MONITOR_TYPE_FLAT_PANEL) ||
            ((pHead1->dwDeviceType == MONITOR_TYPE_NTSC || pHead1->dwDeviceType == MONITOR_TYPE_PAL) && 
             pHead0->dwDeviceType == MONITOR_TYPE_FLAT_PANEL))
        {
            // We need to swap the primary device.
            SwapHeadMapping = 1;
        }

        // Now check the physical head types to make CRT the primary device.
        if ((pHead1->dwDeviceType == MONITOR_TYPE_NTSC || pHead1->dwDeviceType == MONITOR_TYPE_PAL) && 
            pHead0->dwDeviceType == MONITOR_TYPE_VGA)
        {
            // We need to swap the primary device.
            SwapHeadMapping = 1;
        }
    }

    // Swap the head mapping if we need to
    if (SwapHeadMapping)
    {
        // Swap the registry data
        ulTmp = pData->dwDeviceDisplay[0];
        pData->dwDeviceDisplay[0] = pData->dwDeviceDisplay[1];
        pData->dwDeviceDisplay[1] = ulTmp;

        // Swap the incore data in HwDevExt.
        ulTmp = pInfo->ulDeviceDisplay[0];
        pInfo->ulDeviceDisplay[0] = pInfo->ulDeviceDisplay[1];
        pInfo->ulDeviceDisplay[1] = ulTmp;
    }
}

//
// Verifies if the DFP supports 1400x1050. This is done if the EnablePanScanAl lregistry entry is set.
// This routine should be called after nvInitialize_dacsAndEdid(),InitializeCommonModeSetCode() 
// have been called.
//
VOID Verify1400x1050DFP(PHW_DEVICE_EXTENSION HwDeviceExtension)
{
    LPDISPDATA lpDispData;
    LPMODEINFO lpModeDesc;
    LPMODEOUT  lpModeOut;
    LPMODEINFO lpModeNew;
    ULONG ulFrequencyHead;
    DISPDATA DispData;
    MODEINFO ModeDesc;
    MODEOUT  ModeOut;
    MODEINFO ModeNew;

    NV_DESKTOP_INFO *pInfo;
    ULONG ulDeviceMask;
    ULONG ulHead, ulIndex, ulRet;
    LPEDID_UNIT pEdidUnit;

    VideoDebugPrint((1,"Verify1400x1050DFP()\n"));

    pInfo = &HwDeviceExtension->DeskTopInfo;
    HwDeviceExtension->ulDFPSupports1400x1050 = 0;

    // Test the conditions to enable this check.
    if ((pInfo->ulConnectedDeviceMask & 0xFF0000) == 0)
    {
        // No DFP in this system, so return.
        return;
    }

    if (HwDeviceExtension->PanScanSelection != PAN_SCAN_SELECTION_ENABLE_ALL)
    {
        // We are not asked to pan scan always. So return.
        return;
    }

    lpDispData = &DispData;
    lpModeDesc = &ModeDesc;
    lpModeOut = &ModeOut;
    lpModeNew = &ModeNew;

    lpDispData->lpBoardRegistryData = NULL;
    lpDispData->lpMonitorRegistryData = NULL;
    lpDispData->dwDeviceID = HwDeviceExtension->ulChipID;
    lpDispData->dwVideoMemoryInBytes = HwDeviceExtension->AdapterMemorySize;
    lpDispData->lpfnGetModeSize = 0;  // GK: WHat is this?
    lpDispData->dwContext1 = (PVOID)HwDeviceExtension;
    lpDispData->dwContext2 = (PVOID)0;
    lpDispData->dwMaxDfpScaledXResInPixels = HwDeviceExtension->MaxFlatPanelWidth;
    lpDispData->dwMaxTVOutResolution = HwDeviceExtension->ulMaxTVOutResolution;

    lpModeDesc->dwXRes = 1400;
    lpModeDesc->dwYRes = 1050;
    lpModeDesc->dwBpp = 8;
    lpModeDesc->dwRefreshRate = 60;
    lpModeDesc->dwOriginalRefreshRate = 60;

    // DFP goes to physical head 0.
    ulHead = 1;
    ulDeviceMask = 0x10000;
    ulIndex = ConvertDeviceMaskToIndex(ulDeviceMask);
    pEdidUnit = HwDeviceExtension->EdidCollection->HeadMaps[ulIndex];
    ulFrequencyHead = 0;

    if (pEdidUnit->Valid)
    {
        lpDispData->dwMonitorInfType = PLUG_AND_PLAY_DEVICE;
    }
    else
    {
        lpDispData->dwMonitorInfType = UNKNOWN_DEVICE;
    }
    lpDispData->dwCRTCIndex = ulHead;
    lpDispData->dwMaxDacSpeedInHertz8bpp= HwDeviceExtension->maxPixelClockFrequency_8bpp[ulFrequencyHead];
    lpDispData->dwMaxDacSpeedInHertz16bpp = HwDeviceExtension->maxPixelClockFrequency_16bpp[ulFrequencyHead];
    lpDispData->dwMaxDacSpeedInHertz32bpp = HwDeviceExtension->maxPixelClockFrequency_32bpp[ulFrequencyHead];

    // Now validate against the pixel frequency of the head.

    lpModeDesc->dwDevType  = MaskToFindModeEntryDeviceType(ulDeviceMask);
    lpModeDesc->dwTVFormat = 0;

    lpModeDesc->MIDevData.cType     = (CHAR)MaskToFindModeEntryDeviceType(ulDeviceMask);
    lpModeDesc->MIDevData.cNumber   = (CHAR)MaskToDeviceType(ulDeviceMask);
    lpModeDesc->MIDevData.cFormat   = (CHAR)0;
    lpModeDesc->MIDevData.cReserved = 0x0;

    // Call the modeset DLL to see if this mode is valid.
    // We are not interested in the mode timings but just to see if this is a valid mode.
    VideoDebugPrint((1,"Before: lpModeDescr: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
                     lpModeDesc->dwXRes, lpModeDesc->dwYRes, lpModeDesc->dwBpp, lpModeDesc->dwRefreshRate, 
                     lpModeDesc->dwDevType, lpModeDesc->dwOriginalRefreshRate));
    VideoDebugPrint((1,"ulDeviceMask: 0x%x, ulIndex: %d, MonitorInfType: 0x%x, EDIDBufferValid: 0x%x\n",
                     ulDeviceMask, ulIndex, lpDispData->dwMonitorInfType, pEdidUnit->Valid));

    EnterFPCode(HwDeviceExtension->pFPStateSave, HwDeviceExtension->pFPMutex);
    ulRet = FindModeEntry(lpDispData, pEdidUnit->EDIDBuffer, pEdidUnit->Size,
                          lpModeDesc, lpModeOut, lpModeNew);
    ExitFPCode(HwDeviceExtension->pFPStateSave, HwDeviceExtension->pFPMutex);
    VideoDebugPrint((1,"ulRet: 0x%x\n",ulRet));
    VideoDebugPrint((1,"After: lpModeDescr: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
                     lpModeDesc->dwXRes, lpModeDesc->dwYRes, lpModeDesc->dwBpp, lpModeDesc->dwRefreshRate, 
                     lpModeDesc->dwDevType, lpModeDesc->dwOriginalRefreshRate));
    VideoDebugPrint((1,"After: lpModeNew: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
                     lpModeNew->dwXRes, lpModeNew->dwYRes, lpModeNew->dwBpp, lpModeNew->dwRefreshRate, 
                     lpModeNew->dwDevType, lpModeNew->dwOriginalRefreshRate));

    if (ulRet != 0)
    {
        // This mode is not physically supported.
        HwDeviceExtension->ulDFPSupports1400x1050 = 0;
        return;
    }

    // BUGBUG: FindModeEntry returns 0 even if the DFP does not support this mode. Not sure why.
    // So check the lpModeNew also as a backup test.
    if (lpModeNew->dwXRes != lpModeDesc->dwXRes ||
        lpModeNew->dwYRes != lpModeDesc->dwYRes ||
        lpModeNew->dwBpp != lpModeDesc->dwBpp ||
        lpModeNew->dwRefreshRate != lpModeDesc->dwRefreshRate
        )
    {
        // This mode is not physically supported.
        HwDeviceExtension->ulDFPSupports1400x1050 = 0;
        return;
    }


    // This mode is natively supported.
    HwDeviceExtension->ulDFPSupports1400x1050 = 1;
    
    return;
}


//   DetectNextDevice   walks thru the bus (specified indirectly thru pHDE) starting at (*pulPrevDevice + 1) and checks for 
//                      presense of nvidia boards. It returns the Vendos/Device ID of the found board and the slot where it 
//                      is located on the bus (*pulPrevDevice). If bClaimResources is TRUE it also claims the boards memory
//                      access ranges in the registry (using VideoPortGetAccessRanges). As a result it also indicates whether
//                      the board is our primary device (VGA is enabled) or a secondary device in *pbIsPrimaryDevice.
//
VP_STATUS DetectNextDevice( IN      PHW_DEVICE_EXTENSION  pHDE,                     // our devices 'global' data struct
                            IN OUT  ULONG                 *pulPrevDevice,           // in : slot where the last nvidia device was detected (or where the search begins)
                                                                                    // out: slot where the next nvidia device is located                                                                                    
                                                                                    // NOTE: can be 0xffffffff : start looking at dev 0 and must be lower than 32 (5bit)
                            IN      ULONG                 ulNumAccessRanges,        // is needed the to claim the resources, for now it's always 2
                               OUT  PVIDEO_ACCESS_RANGE   pAccessRanges,            // if bClaimResources == TRUE this exports the device's mapping
                               OUT  USHORT                *pusVendorID,             // 
                               OUT  USHORT                *pusDeviceID,             //
                               OUT  BOOLEAN               *pbIsPrimaryDevice,       // 
                            IN      BOOLEAN               bClaimResources)          //
{
    VP_STATUS         status = ERROR_DEV_NOT_EXIST;
    ULONG             ulDevice;
    PCI_SLOT_NUMBER   psnSlot;

    ASSERT(pHDE);
    ASSERT(pulPrevDevice);
    ASSERT((*pulPrevDevice == 0xffffffff) || (*pulPrevDevice < PCI_MAX_DEVICES));
    ASSERT(2 == ulNumAccessRanges);
    ASSERT(pAccessRanges);
    ASSERT(pusVendorID);
    ASSERT(pusDeviceID);
    ASSERT(pbIsPrimaryDevice);

    if ((*pulPrevDevice == 0xffffffff) || (*pulPrevDevice < (PCI_MAX_DEVICES - 1)))  // no need to look behind slot 31
    {
        //
        // walk thru our bus and try to detect nvidia boards
        //
        for (ulDevice = *pulPrevDevice + 1; ulDevice < PCI_MAX_DEVICES; ulDevice++)
        {
            psnSlot.u.bits.DeviceNumber  = ulDevice;
            psnSlot.u.bits.FunctionNumber= 0;
            psnSlot.u.bits.Reserved      = 0;

            status = DetectDevice(pHDE, 
                                psnSlot, 
                                ulNumAccessRanges, 
                                pAccessRanges,
                                pusVendorID,
                                pusDeviceID,
                                pbIsPrimaryDevice, 
                                bClaimResources);

            if (NO_ERROR == status)
            {
                // 
                // return slot # 
                //
                *pulPrevDevice = psnSlot.u.AsULONG;
                break;
            }
        }
    }
    return status;
}


//
// Sets the display resolution to the highest supported resolution at 16BPP. The refresh rate chosen will be the
// highest supported at this resolution.
// Note: Currently this logic is supported only if we are booting into Standard mode.
//
VOID HandleBestResolution(PHW_DEVICE_EXTENSION HwDeviceExtension)
{
    ULONG ulHead, ulDeviceMask, ulEDIDIndex, ulDeviceType, ulMaxTVOutResolution;
    ULONG UseBestResolution;
    NV_DESKTOP_INFO *pInfo;
    MODE_ENTRY BestMode;
    ULONG ulRet;
    ULONG RegStatus;

    UseBestResolution = 0;
    pInfo = &HwDeviceExtension->DeskTopInfo;


    RegStatus = VideoPortGetRegistryParameters(HwDeviceExtension,
                    L"UseBestResolution",
                    FALSE,
                    NVRegistryCallback,
                    &(UseBestResolution));

    // Get the best resolution for this chosen device.
    // Handle only the case when we are going to boot into single device standard mode.
    if (pInfo->ulNumberDacsActive == 1)
    {
        if (RegStatus == NO_ERROR && UseBestResolution == 1)
        {
            LPEDID_UNIT pEdidUnit;

            ulHead = pInfo->ulDeviceDisplay[0];
            ulDeviceMask = pInfo->ulDeviceMask[ulHead];
            ulDeviceType = MaskToDeviceType(ulDeviceMask);
            ulEDIDIndex = ConvertDeviceMaskToIndex(ulDeviceMask);
            pEdidUnit = HwDeviceExtension->EdidCollection->HeadMaps[ulEDIDIndex];

            // If there is a valid EDID for CRT or DFP, process it. Otherwise, do nothing.
            if (ulDeviceType == MONITOR_TYPE_VGA || ulDeviceType == MONITOR_TYPE_FLAT_PANEL)
            {
                if (pEdidUnit->Valid)
                {
                    // parse the EDID
                    ulRet = GetBestResolution(HwDeviceExtension, ulEDIDIndex, ulHead, ulDeviceMask, &BestMode);
                    if (ulRet)
                    {
                        BestMode.ValidMode = 1;
                        CheckForcedResolution(HwDeviceExtension, BestMode.Width, BestMode.Height, 
                            BestMode.Depth, BestMode.RefreshRate, TRUE);
                    }
                }
            }

            // If the device is TV, we pick the best resolution.
            if (ulDeviceType == MONITOR_TYPE_NTSC || ulDeviceType == MONITOR_TYPE_PAL)
            {
                ulMaxTVOutResolution = HwDeviceExtension->ulMaxTVOutResolution;
                BestMode.ValidMode = 1;
                BestMode.Width = (USHORT)((ulMaxTVOutResolution & 0xffff0000) >> 16);
                BestMode.Height = (USHORT)(ulMaxTVOutResolution & 0xffff);
                BestMode.Depth = 32;
                BestMode.RefreshRate = 60;
                CheckForcedResolution(HwDeviceExtension, BestMode.Width, BestMode.Height, 
                    BestMode.Depth, BestMode.RefreshRate, TRUE);
                
            }
        } // if UseBestResolution == 1

        else
        {
            // Else, even if UseBestResolution flag was cleared, we could be in 
            // the middle of a one-time-after-reboot-forced-mode sequence so
            // we NEED to check the status of this sequence so DrvEnablePdev
            // will be able to force the mode since we are now passing again
            // in hwinit() (rebooted)
            // (see CheckForcedResolution() (in this file) for details)

            // Even if a one-time-forced-mode sequence is active, only pursue it
            // if we are in standard mode...            
            
            // Note that when setbestresolution (last param) is FALSE, the modeinfo
            // is not used anyways -> see checkforced...
            //
            CheckForcedResolution(HwDeviceExtension, 640, 480, 4, 1, FALSE);

        }


    } // if standard mode
        

    // If the regkey exists, set it to 0.
    if (RegStatus == NO_ERROR && UseBestResolution == 1)
    {
        UseBestResolution = 0;
        VideoPortSetRegistryParameters(HwDeviceExtension,
                    L"UseBestResolution",
                    &UseBestResolution,
                    sizeof(ULONG));
    }

        
    // done
    return;
}

//
// If the twinViewInfo registry structure is present and is valid, use twinVieInfo to initiliaze the 
// fields in HwDeviceExtension strutcure.
// This handles the case when UseHwSelectedDevice has been disabled such as on compal. We still need to initialize
// device masks correctly so that at boot time, GET_CHILD_STATE returns the correct device state for
// ACPI systems. This ensures Win2K uses the correct resolution to bootup.
//
// Note: 
// - This function should be called just once, at boot time.
//
VOID InitFromTwinViewInfo(PHW_DEVICE_EXTENSION HwDeviceExtension)
{
    NVTWINVIEWDATA sData;
    NVTWINVIEW_DEVICE_TYPE_DATA * pHeadData;
    ULONG ulCombinedMask;
    NV_DESKTOP_INFO * pInfo;
    ULONG i,j, ulHead;
    VP_STATUS Status;

    VideoDebugPrint((1, "InitFromTwinViewInfo():\n"));
    pInfo = (NV_DESKTOP_INFO *) &HwDeviceExtension->DeskTopInfo;

    //
    // Read in the twinview info structure
    //
    if (NVGetRegistryTwinviewInfo(HwDeviceExtension, &sData) != NO_ERROR) 
    {
        return;
    }

    //
    // See if the twinviewInfo is valid before using it.
    //
    if (TwinViewDataOK(HwDeviceExtension, &sData))
    {
        pInfo->ulDesktopWidth = sData.DeskTopRectl.right - sData.DeskTopRectl.left;
        pInfo->ulDesktopHeight = sData.DeskTopRectl.bottom - sData.DeskTopRectl.top;
        pInfo->ulTwinView_State = sData.dwState;

        //
        // Set the number of active dacs based on the mode.
        //
        if (sData.dwState == NVTWINVIEW_STATE_NORMAL || sData.dwState == NVTWINVIEW_STATE_DUALVIEW)
        {
            pInfo->ulNumberDacsActive = 1;
        }
        if (sData.dwState == NVTWINVIEW_STATE_CLONE)
        {
            pInfo->ulNumberDacsActive = 2;
        }
        if (sData.dwState == NVTWINVIEW_STATE_SPAN)
        {
            if (MULTIMON_MODE(pInfo->ulDesktopWidth, pInfo->ulDesktopHeight))
            {
                pInfo->ulNumberDacsActive = 2;
            }
            else
            {
                pInfo->ulNumberDacsActive = 1;
            }
        }
        
        //
        // Transfer the head mapping array,device masks and other info for the clone, spanning and standard modes.
        //
        if (sData.dwState == NVTWINVIEW_STATE_CLONE || sData.dwState == NVTWINVIEW_STATE_NORMAL ||
            sData.dwState == NVTWINVIEW_STATE_DUALVIEW || sData.dwState == NVTWINVIEW_STATE_SPAN)
        {
            for (i=0; i < pInfo->ulNumberDacsActive; i++)
            {
                ulHead = sData.dwDeviceDisplay[i];
                pInfo->ulDeviceDisplay[i] = ulHead;
                pHeadData = &(sData.NVTWINVIEWUNION.nvtwdevdata[ulHead]);
                pInfo->ulDeviceMask[ulHead] = pHeadData->dwDeviceMask;
                pInfo->ulDeviceType[ulHead] = pHeadData->dwDeviceType;
                pInfo->ulTVFormat[ulHead] = pHeadData->dwTVFormat;
                pInfo->ulDisplayWidth[ulHead] = pHeadData->PhysicalRectl.right - pHeadData->PhysicalRectl.left;
                pInfo->ulDisplayHeight[ulHead] = pHeadData->PhysicalRectl.bottom - pHeadData->PhysicalRectl.top;
                pInfo->ulDisplayRefresh[ulHead] = pHeadData->dwRefresh;
                pInfo->ulDisplayPixelDepth[ulHead] = pHeadData->dwBpp;
            }
        }
        else
        {
            ASSERT(FALSE);
        }
    }

    return;
}

// Writes the current flat panel scaling mode to the registry.
// Temporary until resman is able to do this in a more timely fashion.
ULONG WriteFPModeToRegistry (PHW_DEVICE_EXTENSION hwDeviceExtension)
{   
    BOOL   fpstatus = 0;
    ULONG  head;
    ULONG  i;
    NV_CFGEX_GET_FLATPANEL_INFO_MULTI_PARAMS fpGetParams; 
     
                                      
    for (i=0;i<hwDeviceExtension->DeskTopInfo.ulNumberDacsActive; i++)
    {
        head = hwDeviceExtension->DeskTopInfo.ulDeviceDisplay[i];
        if(MaskToDeviceType(hwDeviceExtension->DeskTopInfo.ulDeviceMask[head]) ==
                            MONITOR_TYPE_FLAT_PANEL)
        {
            fpGetParams.Head = head;
            fpstatus = RmConfigGetExKernel(hwDeviceExtension->DeviceReference,
                                NV_CFGEX_GET_FLAT_PANEL_INFO_MULTI,
                                &(fpGetParams), 
                                sizeof(NV_CFGEX_GET_FLATPANEL_INFO_MULTI_PARAMS));

            if( fpstatus == TRUE && 
                fpGetParams.FlatPanelMode  <= 2 )
            {
                VideoPortSetRegistryParameters(hwDeviceExtension,
                                  L"FlatPanelMode",
                                  &fpGetParams.FlatPanelMode,
                                  sizeof(ULONG));
            } //if scaling mode is reasonable
        }//if flat panel
    }//for each active dac
    return fpGetParams.FlatPanelMode;
} //end WriteFPModeToRegistry.

//
// Reads the SaveSettings into the hwDevExtension from the registry and returns TRUE.
// If the registry entry is absent, returns FALSE.
//
BOOLEAN bReadSaveSettings(
    PHW_DEVICE_EXTENSION hwDeviceExtension)
{
    VP_STATUS RegStatus;
    RegStatus = VideoPortGetRegistryParameters(
                        hwDeviceExtension,
                        L"SaveSettings",
                        FALSE,
                        NVReadRegistrySaveSettingsCallback,
                        hwDeviceExtension->SaveSettings);
    if (RegStatus == NO_ERROR)
    {
    
        return(TRUE);
    }
    VideoDebugPrint((1,"SaveSettings regKey could not be read: error: 0x%x\n",RegStatus));
    return(FALSE);
}

//
// Debug helper routine to print the SaveSettings table.
//
VOID vPrintSaveSettings(
    PHW_DEVICE_EXTENSION hwDeviceExtension)
{
    ULONG i, ulHead;
    ULONG ulDeviceMask, ulDeviceMask0, ulDeviceMask1;
    NVTWINVIEWDATA *pData;
    NVTWINVIEW_DEVICE_TYPE_DATA *pDevData0, *pDevData1;
    CHAR *pcMode;

    for (i=0; i < NUM_ENTRIES_SAVE_SETTINGS; i++)
    {
        pData = &hwDeviceExtension->SaveSettings[i];
        pDevData0 = &pData->NVTWINVIEWUNION.nvtwdevdata[0];
        pDevData1 = &pData->NVTWINVIEWUNION.nvtwdevdata[1];
        switch (pData->dwState)
        {
        case NVTWINVIEW_STATE_NORMAL:
            pcMode = "Standard";
            break;
        case NVTWINVIEW_STATE_CLONE:
            pcMode = "Clone";
            break;
        case NVTWINVIEW_STATE_SPAN:
            pcMode = "Spanning";
            break;
        default:
            pcMode = "Invalid mode";
            break;
        }
        VideoDebugPrint((1,"%d: %s,%d,  (%d,%d), (0x%x, %d), (0x%x, %d)\n",i,
            pcMode, pData->dwOrientation, pData->dwDeviceDisplay[0], pData->dwDeviceDisplay[1], 
            pDevData0->dwDeviceMask, pDevData0->dwTVFormat, pDevData1->dwDeviceMask, pDevData1->dwTVFormat));

        if (pData->DeskTopRectl.left == 0 && pData->DeskTopRectl.top == 0 &&
            pData->DeskTopRectl.right == 0 && pData->DeskTopRectl.bottom == 0)   
        {
            // don't print this since this slot is not valid.
        }
        else
        {
            // This slot is valid and occupied. Print the mode info.
            VideoDebugPrint((1, "   Desktop: (%d, %d), (%d, %d)\n", 
                pData->DeskTopRectl.left,pData->DeskTopRectl.top,pData->DeskTopRectl.right, pData->DeskTopRectl.bottom));
            VideoDebugPrint((1,"    Head0 virt: (%d, %d), (%d, %d)\n",
                pDevData0->VirtualRectl.left,pDevData0->VirtualRectl.top,pDevData0->VirtualRectl.right,pDevData0->VirtualRectl.bottom));
            VideoDebugPrint((1,"    Head0 phys: (%d, %d), (%d, %d), %dbpp, %dHZ\n",
                pDevData0->PhysicalRectl.left,pDevData0->PhysicalRectl.top,pDevData0->PhysicalRectl.right,pDevData0->PhysicalRectl.bottom,
                pDevData0->dwBpp, pDevData0->dwRefresh));
            VideoDebugPrint((1,"    Head1 virt: (%d, %d), (%d, %d)\n",
                pDevData1->VirtualRectl.left,pDevData1->VirtualRectl.top,pDevData1->VirtualRectl.right,pDevData1->VirtualRectl.bottom));
            VideoDebugPrint((1,"    Head0 phys: (%d, %d), (%d, %d), %dbpp, %dHZ\n",
                pDevData1->PhysicalRectl.left,pDevData1->PhysicalRectl.top,pDevData1->PhysicalRectl.right,pDevData1->PhysicalRectl.bottom,
                pDevData1->dwBpp, pDevData1->dwRefresh));
            VideoDebugPrint((1,"\n"));
        }
        
    }

    return;
}

//
// Writes the SaveSettings from hwDeviceExtension into registry
//
VOID vWriteSaveSettings(
    PHW_DEVICE_EXTENSION hwDeviceExtension)
{
    VP_STATUS RegStatus;

    RegStatus = VideoPortSetRegistryParameters(hwDeviceExtension,
                           L"SaveSettings",
                           hwDeviceExtension->SaveSettings,
                           sizeof(hwDeviceExtension->SaveSettings));
    return;
}

//
// If the "SaveSettings" is absent (as with fresh driver installation), creates the structure in registry. 
// If the "SaveSettings" is present but invalid (as with manual driver binary loads), resets the structure.
// If the "SaveSettings" is present and valid, then does nothing.
// Should be called once at boot time.
// Supported devices for now are CRT0, LCD, TV and CRT1
//
VOID vInitializeSaveSettings(
    PHW_DEVICE_EXTENSION hwDeviceExtension)
{
    ULONG i, ulHead;
    ULONG ulDeviceMask, ulDeviceMask0, ulDeviceMask1;
    VP_STATUS RegStatus;
    NVTWINVIEWDATA *pData;
    NVTWINVIEW_DEVICE_TYPE_DATA *pDevData0, *pDevData1;

    VideoDebugPrint((1,"Enter vInitializeSaveSettings\n"));

    if (bReadSaveSettings(hwDeviceExtension))
    {
        //
        // Valid SaveSettings was found and has been read into the hwDeviceExtension.
        // Nothing else to do. Return.
        //
        return;
    }
    
    //
    // Either the "saveSettings" was not found or was found to be invalid. So we need to initialize it.
    // First initialize the mapping array. Note that this does not matter for now, since per Ilyas's requirements,
    // LCD+CRT is to be treated the same as CRT+LCD.
    //
    for (i=0; i < NUM_ENTRIES_SAVE_SETTINGS; i++)
    {
        pData = &hwDeviceExtension->SaveSettings[i];
        pData->dwDeviceDisplay[0] = 0;
        pData->dwDeviceDisplay[1] = 1;
    }

    //
    // Set the standard devices.
    //
    for (i=0; i < 4; i++)
    {
        pData = &hwDeviceExtension->SaveSettings[i];
        pDevData0 = &pData->NVTWINVIEWUNION.nvtwdevdata[0];
        pDevData1 = &pData->NVTWINVIEWUNION.nvtwdevdata[1];
        pData->dwState = NVTWINVIEW_STATE_NORMAL;
        switch (i)
        {
        case 0:
            ulDeviceMask = BITMASK_CRT0;
            break;
        case 1:
            ulDeviceMask = BITMASK_DFP0;
            break;
        case 2:
            ulDeviceMask = BITMASK_TV0;
            break;
        case 3:
            ulDeviceMask = BITMASK_CRT1;
            break;
        default:
            VideoDebugPrint((1, "InitializeSaveSettings(): Invalid index: %d\n",i));
            break;
        }
        //
        // Set the device mask and the TV type. The TV type set here does not matter since TV type is
        // not part of the QUERY_SAVE_SETTINGS. Set both head's devicemask just to be uniform.
        //
        pDevData0->dwDeviceMask = ulDeviceMask;
        pDevData0->dwTVFormat = NTSC_M; 
        pDevData1->dwDeviceMask = ulDeviceMask;
        pDevData1->dwTVFormat = NTSC_M; 
    }

    //
    // Set the clone devices.
    //
    for (i=4; i < 10; i++)
    {
        pData = &hwDeviceExtension->SaveSettings[i];
        pDevData0 = &pData->NVTWINVIEWUNION.nvtwdevdata[0];
        pDevData1 = &pData->NVTWINVIEWUNION.nvtwdevdata[1];
        pData->dwState = NVTWINVIEW_STATE_CLONE;
        switch (i)
        {
        case 4:
            ulDeviceMask0 = BITMASK_CRT0;
            ulDeviceMask1 = BITMASK_DFP0;
            break;
        case 5:
            ulDeviceMask0 = BITMASK_CRT0;
            ulDeviceMask1 = BITMASK_TV0;
            break;
        case 6:
            ulDeviceMask0 = BITMASK_CRT0;
            ulDeviceMask1 = BITMASK_CRT1;
            break;
        case 7:
            ulDeviceMask0 = BITMASK_DFP0;
            ulDeviceMask1 = BITMASK_TV0;
            break;
        case 8:
            ulDeviceMask0 = BITMASK_DFP0;
            ulDeviceMask1 = BITMASK_CRT1;
            break;
        case 9:
            ulDeviceMask0 = BITMASK_TV0;
            ulDeviceMask1 = BITMASK_CRT1;
            break;
        default:
            VideoDebugPrint((1, "InitializeSaveSettings(): Invalid index: %d\n",i));
            break;
        }
        //
        // Set the device mask and the TV type. The TV type set here does not matter since TV type is
        // not part of the QUERY_SAVE_SETTINGS.
        //
        pDevData0->dwDeviceMask = ulDeviceMask0;
        pDevData0->dwTVFormat = NTSC_M; 
        pDevData1->dwDeviceMask = ulDeviceMask1;
        pDevData1->dwTVFormat = NTSC_M; 
    }

    //
    // Set the horizontal span devices.
    //
    for (i=10; i < 16; i++)
    {
        pData = &hwDeviceExtension->SaveSettings[i];
        pDevData0 = &pData->NVTWINVIEWUNION.nvtwdevdata[0];
        pDevData1 = &pData->NVTWINVIEWUNION.nvtwdevdata[1];
        pData->dwState = NVTWINVIEW_STATE_SPAN;
        pData->dwOrientation = HORIZONTAL_ORIENTATION;
        switch (i)
        {
        case 10:
            ulDeviceMask0 = BITMASK_CRT0;
            ulDeviceMask1 = BITMASK_DFP0;
            break;
        case 11:
            ulDeviceMask0 = BITMASK_CRT0;
            ulDeviceMask1 = BITMASK_TV0;
            break;
        case 12:
            ulDeviceMask0 = BITMASK_CRT0;
            ulDeviceMask1 = BITMASK_CRT1;
            break;
        case 13:
            ulDeviceMask0 = BITMASK_DFP0;
            ulDeviceMask1 = BITMASK_TV0;
            break;
        case 14:
            ulDeviceMask0 = BITMASK_DFP0;
            ulDeviceMask1 = BITMASK_CRT1;
            break;
        case 15:
            ulDeviceMask0 = BITMASK_TV0;
            ulDeviceMask1 = BITMASK_CRT1;
            break;
        default:
            VideoDebugPrint((1, "InitializeSaveSettings(): Invalid index: %d\n",i));
            break;
        }
        //
        // Set the device mask and the TV type. The TV type set here does not matter since TV type is
        // not part of the QUERY_SAVE_SETTINGS.
        //
        pDevData0->dwDeviceMask = ulDeviceMask0;
        pDevData0->dwTVFormat = NTSC_M; 
        pDevData1->dwDeviceMask = ulDeviceMask1;
        pDevData1->dwTVFormat = NTSC_M; 
    }

    //
    // Set the vertical span devices.
    //
    for (i=16; i < 22; i++)
    {
        pData = &hwDeviceExtension->SaveSettings[i];
        pDevData0 = &pData->NVTWINVIEWUNION.nvtwdevdata[0];
        pDevData1 = &pData->NVTWINVIEWUNION.nvtwdevdata[1];
        pData->dwState = NVTWINVIEW_STATE_SPAN;
        pData->dwOrientation = VERTICAL_ORIENTATION;
        switch (i)
        {
        case 16:
            ulDeviceMask0 = BITMASK_CRT0;
            ulDeviceMask1 = BITMASK_DFP0;
            break;
        case 17:
            ulDeviceMask0 = BITMASK_CRT0;
            ulDeviceMask1 = BITMASK_TV0;
            break;
        case 18:
            ulDeviceMask0 = BITMASK_CRT0;
            ulDeviceMask1 = BITMASK_CRT1;
            break;
        case 19:
            ulDeviceMask0 = BITMASK_DFP0;
            ulDeviceMask1 = BITMASK_TV0;
            break;
        case 20:
            ulDeviceMask0 = BITMASK_DFP0;
            ulDeviceMask1 = BITMASK_CRT1;
            break;
        case 21:
            ulDeviceMask0 = BITMASK_TV0;
            ulDeviceMask1 = BITMASK_CRT1;
            break;
        default:
            VideoDebugPrint((1, "InitializeSaveSettings(): Invalid index: %d\n",i));
            break;
        }
        //
        // Set the device mask and the TV type. The TV type set here does not matter since TV type is
        // not part of the QUERY_SAVE_SETTINGS.
        //
        pDevData0->dwDeviceMask = ulDeviceMask0;
        pDevData0->dwTVFormat = NTSC_M; 
        pDevData1->dwDeviceMask = ulDeviceMask1;
        pDevData1->dwTVFormat = NTSC_M; 
    }

    //
    // Now write the initialized table to registry.
    //
    vWriteSaveSettings(hwDeviceExtension);
    VideoDebugPrint((1,"Exit vInitializeSaveSettings\n"));
    return;
}



//****************************************************************************************
//
//   CheckForcedResolution()
//
//   ( It was SetBestResolution() when it was doing this ->
//     
//     Writes the preferred mode into the DefaultSettings part of the registry.
//     (Hack to force the best mode for next bootup: we're writing to an OS key (hardware profile)
//     that don't belong to us)
//   )
//
// New method: Use a private key to tell us to force a mode during the next bootup. 
//             The key will contain the mode info.
//
// THIS FUNCTION MUST BE CALLED at hwinit(), even if we don't want to set the best resolution :
//  
//  the last parameter, BOOLEAN SetBestResolution, is used for that purpose:
//  When we want to set the best res (using the other function parameters) on next reboot, 
//  set this to TRUE, otherwise FALSE
// 
//  Note that even if the function is called with SetBestResolution == TRUE, 
//  if the ForceModeNextBoot key is set to 1, a forced mode will overide the first modeset 
//  sent by GDI anyway, like it is supposed to. (Because the one-time-after-reboot-forced-mode
//  was already induced, we already have a mode to set, from a previous call to this function)
//
//****************************************************************************************

VOID CheckForcedResolution(
    
            PHW_DEVICE_EXTENSION HwDeviceExtension, 
            USHORT               usWidth, 
            USHORT               usHeight, 
            USHORT               usDepth, 
            USHORT               usRefreshRate,
            BOOLEAN              SetBestResolution )
{
    MODE_ENTRY              ModeEntry;
    NV_REGISTRY_STRUCT      RegStruct;
    VP_STATUS               RegStatus;
    
    WCHAR                   ReBootedNowForceMode_namew[] = {L"ReBootedNowForceMode"};
   
    ULONG                   ForceModeNextBoot;  // see below

    BOOLEAN                 ModeEntryIsUpdated; // did we update the modeentry struct or kept the data from the registry ?



    ModeEntryIsUpdated = FALSE;


    /*
    
    This method won't work all the time: for example, on winxp the path is different, and on Toshiba
    compal (bug 35795) the first boot resolution was incorrect if you had a clean system because the
    NV4 key didnt existed yet)
    
    RtlWriteRegistryValue(RTL_REGISTRY_ABSOLUTE, 
        L"\\Registry\\Machine\\System\\CurrentControlSet\\hardware Profiles\\Current\\System\\CurrentControlSet\\Services\\NV4\\Device0",
        L"DefaultSettings.XResolution",
        REG_DWORD, &usWidth, sizeof(ULONG));

    RtlWriteRegistryValue(RTL_REGISTRY_ABSOLUTE, 
        L"\\Registry\\Machine\\System\\CurrentControlSet\\hardware Profiles\\Current\\System\\CurrentControlSet\\Services\\NV4\\Device0",
        L"DefaultSettings.YResolution",
        REG_DWORD, &usHeight, sizeof(ULONG));
    
    RtlWriteRegistryValue(RTL_REGISTRY_ABSOLUTE, 
        L"\\Registry\\Machine\\System\\CurrentControlSet\\hardware Profiles\\Current\\System\\CurrentControlSet\\Services\\NV4\\Device0",
        L"DefaultSettings.BitsPerPel",
        REG_DWORD, &usDepth, sizeof(ULONG));

    RtlWriteRegistryValue(RTL_REGISTRY_ABSOLUTE, 
        L"\\Registry\\Machine\\System\\CurrentControlSet\\hardware Profiles\\Current\\System\\CurrentControlSet\\Services\\NV4\\Device0",
        L"DefaultSettings.VRefresh",
        REG_DWORD, &usRefreshRate, sizeof(ULONG));
        
    */

    // Write two reg keys: mode info of the mode to force for next bootup, and a flag to tell us when we passed
    // through miniport initialization again for the first mode set of the next boot: 
    //
    // This is done because otherwise, in the drvenablepdev, we won't know if the user tried to change a res
    // without rebooting: if he did just that, we don't want to overide his new choice! We only want to force
    // the best resolution *for the next boot*  *IF AND ONLY IF* the user has not selected a new res prior to that.
    // 
    // One key will be: ForceModeNextBoot  (set here, cleared if  - user changes mode before rebooting OR
    //                                                            - we have rebooted (passed through hwinit again
    //                                                              and this flag was set)
    //
    // The other will tell the DrvEnablePdev function to force a mode: we will use MODE_INFO.validmode
    // (set during hwinit IF ForceModeNextBoot is set, cleared during DrvEnablePdev if it was set)
    //
    // Since this function IS CALLED by hwinit (through handlebestresolution()), we:
    //
    // 1) read ReBootedNowForceMode;
    //
    // 2) read the ForceModeNextBoot flag:  if it is set -> clear it and set ReBootedNowForceMode.ValidMode = 1;
    //                                      else 
    //                                          -> set it (to induce a one-time-after-reboot-forced-mode, since
    //                                             we call setbestresolution for this purpose ) 
    //                                             IF SETBESTRESOLUTION is false, do not set it!!
    //                                             (note that ReBootedNowForceMode.ValidMode = 0);
    // 3) write back the keys;
    //
    //
    //
    // In DrvEnablePdev(): ***!ASSUMING it is called ONLY on modeset!*** 
    //
    //
    //    1) read the ForceModeNextBoot flag: 
    //
    //                                 if it is set ->        clear it ! (the user changed the mode but we haven't rebooted
    //                                                        yet because otherwise, on reboot, in this very function,
    //                                                        we would have cleared it);   ReBootedNowForceMode.ValidMode is 
    //                                                        probably = 0, else the state is inconsistant.
    //                                 else 
    //                                     -> 
    //                                 read the ReBootedNowForceMode.ValidMode flag:  
    //                       
    //                                     if it is set -> clear it and overide the GDI mode with
    //                                                     ReBootedNowForceMode;
    //                                     else 
    //                                         -> nothing special;
    //
    //
    // NOTE: This seems similar to what we do with the UseBestResolution key, but then we might ask "why not use
    //       only that key in the first place?" Because this key is not set *first* by the miniport bootup, but 
    //       by the INF file ! So if the flag is set by the INF, we might think it's ok to turn on
    //       ReBootedNowForceMode.ValidMode, but if the user decides to change the res before rebooting (after the
    //       first install of the drv without reboot) the behavior is going to be wrong: we're going to overide 
    //       the modeset triggered by the user ( because ReBootedNowForceMode.ValidMode will be true! )

    
    // Copy the required values into the input context (submitted to the callback)
    
    RegStruct.keyVal     = (PVOID) &ModeEntry;
    RegStruct.keyValSize = sizeof(MODE_ENTRY);
    RegStruct.keyName    = (PWSTR) ReBootedNowForceMode_namew;

    RegStatus = VideoPortGetRegistryParameters(
                           HwDeviceExtension,
                           ReBootedNowForceMode_namew,
                           FALSE,
                           NVReadRegistryBinaryCallback,
                           (PVOID)&RegStruct);
    
    
    if(RegStatus != NO_ERROR)  
    {
        // the key wasn't there, so fill the structure
        //
        // We only need to to this when we are actually setting 
        // the best resolution, when we check for the forced res
        // sequence, we must have this key valid, so no error
        // on the read should occur.  But if we just check the forced
        // sequence, and WE HAVE an error (the ModeEntry key isnt there, 
        // we will fill a valid mode); this is not necessarily an error
        // condition: if we check the forced resolution sequence, but
        // we don't want to set it, AND the sequenced WAS NOT "started" (by setting the 
        // ForceModeNextBoot key) it is possible not to have any modeentry
        // (ReBootedNowForceMode) registry key set for this...
        // 

        if( SetBestResolution )
        {
            ModeEntry.Depth       = usDepth;        // in bits
            ModeEntry.Height      = usHeight;
            ModeEntry.Width       = usWidth;
            ModeEntry.RefreshRate = usRefreshRate;
            ModeEntry.ValidMode   = 0;              
        }
        else 
        {
            ModeEntry.Depth       = 4;        // in bits
            ModeEntry.Height      = 640;      //
            ModeEntry.Width       = 480;      //
            ModeEntry.RefreshRate = 1;        // this is what windows set as the "safest mode request" 
                                              // in VGASAVE reg key and this is what it sends the driver in safe mode
            ModeEntry.ValidMode   = 0;              
        }
        
        ModeEntryIsUpdated = TRUE;
    }
    
    // when mode data is taken without error from the regkey, assume it is ok


    ForceModeNextBoot = 0;

    RegStatus = VideoPortGetRegistryParameters(
                    HwDeviceExtension,
                    L"ForceModeNextBoot",
                    FALSE,
                    NVRegistryCallback,       
                    (PVOID) &(ForceModeNextBoot));    // This param is passed to our callback. This callback uses ULONGs
    
    if( (RegStatus == NO_ERROR) && (ForceModeNextBoot == 1) )
    {
        // Here, if the forcemode flag was set, we should have read (without error) the mode entry
        // from the registry.  We don't keep and check the status NO_ERROR here because even if 
        // it was invalid, now it is (this case should never happen but it won't do us harm)
        
        ForceModeNextBoot     = 0;
        ModeEntry.ValidMode   = 1;

        ModeEntryIsUpdated = TRUE; // this is not really true, because only one field is updated,
                                   // but we use ModeEntryIsUpdated also to avoid an unecessary 
                                   // write back to the registry ( ModeEntryIsUpdated == FALSE tells
                                   // us we have a mode info from the registry, not from the values
                                   // passed at SetBestResolution()


    } // ... if the key ForceModeNextBoot was set (and it existed)    
    
    else if ( SetBestResolution )     
    {
        // The ForceModeNextBoot key didn't exist, or it was not set:
        // We set it here to induce a one-time-after-reboot-forced-mode, since
        // we called this function with setbestresolution == TRUE for this purpose 
        // We don't call with SetBestResolution == TRUE at each boot because we
        // clear the UseBestResolution flag in HandleBestResolution

        // (the next time we boot, we clear ForceModeNextBoot, by making a call to this
        //  function but with SetBestResolution == FALSE, and we also set modeentry.validmode.  
        //  Even if the function is
        //  called with SetBestResolution == TRUE, if the ForceModeNextBoot key is set
        //  to 1, a forced mode will overide the first modeset sent by GDI, but we
        //  won't update the mode_entry for this mode, so the other function parameters are 
        //  not used, )

        ForceModeNextBoot = 1;

        // Here, one case could make use choose the wrong best resolution:
        // if the key ReBootedNowForceMode was not removed by the wizard uninstall 
        // it could be valid, and we would read some old best mode.  So,
        // if we haven't updated the modeentry, we force it here:

        if( ModeEntryIsUpdated == FALSE )
        {
            ModeEntry.Depth       = usDepth;        // in bits
            ModeEntry.Height      = usHeight;
            ModeEntry.Width       = usWidth;
            ModeEntry.RefreshRate = usRefreshRate;
            ModeEntry.ValidMode   = 0; 
            
            ModeEntryIsUpdated = TRUE;
        }
    
    } // ... else the key ForceModeNextBoot was cleared


    // Write back the keys
    //
    //
    // Note: if the key doesn't exist, it will be created

    
    if( ModeEntryIsUpdated )
    {
        RegStatus = VideoPortSetRegistryParameters(
                           HwDeviceExtension,
                           L"ReBootedNowForceMode",
                           (PVOID) &ModeEntry,
                           sizeof(MODE_ENTRY));    
    }
        
    // Write the ForceModeNextBoot flag 
    
    RegStatus = VideoPortSetRegistryParameters(
                       HwDeviceExtension,
                       L"ForceModeNextBoot",
                       (PVOID) &ForceModeNextBoot,
                       sizeof(ULONG));

} // ... CheckForcedResolution()



