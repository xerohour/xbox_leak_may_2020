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

#if NVDLL == 0
LONG
MxInitSystem(
    IN PVOID Argument1
    );
#endif



// To get the twinview definitions
#include "nvMultiMon.h"

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
// of other structures that would cause redefinitions / errors.  We just
// want the definition for HalGetBusData so we can workaround the ALI 1541 chipset bugs

extern ULONG HalGetBusData(
    IN BUS_DATA_TYPE BusDataType,
    IN ULONG BusNumber,
    IN ULONG SlotNumber,
    IN PVOID Buffer,
    IN ULONG Length
    );

extern ULONG HalGetBusDataByOffset(
    IN BUS_DATA_TYPE BusDataType,
    IN ULONG BusNumber,
    IN ULONG SlotNumber,
    IN PVOID Buffer,
    IN ULONG Offset,
    IN ULONG Length
    );

extern ULONG HalSetBusDataByOffset(
    IN BUS_DATA_TYPE BusDataType,
    IN ULONG BusNumber,
    IN ULONG SlotNumber,
    IN PVOID Buffer,
    IN ULONG Offset,
    IN ULONG Length
    );

#endif

extern InitializeCommonModesetCode(PHW_DEVICE_EXTENSION HwDeviceExtension);
extern VOID ReadMonitorRestrictionModeList(PHW_DEVICE_EXTENSION HwDeviceExtension);


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


extern void NVMapPhysIntoUserSpace(
        PHYSICAL_ADDRESS pa,
        LONG byteLength,
        PVOID *userVirtualAddress);

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
    IN ULONG PhysicalAddress,
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
    PHW_DEVICE_EXTENSION HwDeviceExtension
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

ULONG
NVGetChildDescriptor(
    PVOID HwDeviceExtension,
    PVIDEO_CHILD_ENUM_INFO ChildEnumInfo,
    PVIDEO_CHILD_TYPE pChildType,
    PVOID pvChildDescriptor,
    PULONG pHwId,
    PULONG pUnused
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

VOID
NVAMD751ChipsetSaveHibernationRegisters(
    PHW_DEVICE_EXTENSION HwDeviceExtension
    );

VOID
NVAMD751ChipsetRestoreHibernationRegisters(
    PHW_DEVICE_EXTENSION HwDeviceExtension
    );

extern BOOLEAN NV_VBE_DPMS_SetPowerState(PHW_DEVICE_EXTENSION HwDeviceExtension, ULONG PowerState);
extern BOOLEAN NV_VBE_DPMS_GetPowerState(PHW_DEVICE_EXTENSION HwDeviceExtension, ULONG *CurrentPowerState);

#endif // #ifdef NV_PWR_MGMT

BOOLEAN
GetDdcInformation(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PUCHAR QueryBuffer,
    ULONG BufferSize);
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

#define QUERY_MONITOR_ID            0x22446688
#define QUERY_NONDDC_MONITOR_ID     0x11223344
#define DISPLAY_ADAPTER_HW_ID           0xFFFFFFFF

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
#pragma alloc_text(PAGE, NVGetChildDescriptor)
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
    PVOID Context1
    )


    {

    VIDEO_HW_INITIALIZATION_DATA hwInitData;
    ULONG status;
    ULONG initializationStatus;

#if NVDLL == 0

    return MxInitSystem(Context1);

#endif


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
    hwInitData.HwTimer = NV10CursorTimer;

    //**************************************************************************
    // New NT 5.0 EntryPoint
    //**************************************************************************

#if (_WIN32_WINNT >= 0x0500)

    hwInitData.HwGetVideoChildDescriptor = NVGetChildDescriptor;

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
                                               NULL,
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
ULONG
NVGetChildDescriptor(
    PVOID HwDeviceExtension,
    PVIDEO_CHILD_ENUM_INFO ChildEnumInfo,
    PVIDEO_CHILD_TYPE pChildType,
    PVOID pvChildDescriptor,
    PULONG pHwId,
    PULONG pUnused
    )

/*++

Routine Description:

    Enumerate all devices controlled by the ATI graphics chip.
    This includes DDC monitors attached to the board, as well as other devices
    which may be connected to a proprietary bus.

Arguments:

    HwDeviceExtension - Pointer to our hardware device extension structure.

    ChildIndex        - Index of the child the system wants informaion for.

    pChildType        - Type of child we are enumerating - monitor, I2C ...

    pChildDescriptor  - Identification structure of the device (EDID, string)

    ppHwId            - Private unique 32 bit ID to passed back to the miniport

    pMoreChildren     - Should the miniport be called

Return Value:

    TRUE if the child device existed, FALSE if it did not.

Note:

    In the event of a failure return, none of the fields are valid except for
    the return value and the pMoreChildren field.

--*/

{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    ULONG                Status;
    ULONG                ulDacs;

    switch (ChildEnumInfo->ChildIndex) {

    case 0:

        //
        // Case 0 is used to enumerate devices found by the ACPI firmware.
        //
        // Since we do not support ACPI devices yet, we must return failure.
        //

        Status = ERROR_NO_MORE_DEVICES;
        break;


    case 1:

        //
        // This is the last device we enumerate.  Tell the system we don't
        // have any more.
        //

        *pChildType = Monitor;

        //**********************************************************************
        // Unattended install with Intel 810 present:
        // Make sure to SKIP this call, if the device has NOT been mapped!!!
        // (Skip this function if currently running unattended install with
        // an Intel 810)
        //**********************************************************************

#ifndef ENABLE_EXPORTING_EDID_TO_WIN2K
        //
        // Obtain the EDID structure via DDC.
        //

        RmConfigGetKernel(hwDeviceExtension->DeviceReference, NV_CFG_NUMBER_OF_HEADS, &ulDacs);
        // Supports more than one monitors
        if(ulDacs > 1)
        {
            *pHwId = QUERY_NONDDC_MONITOR_ID;
            Status = ERROR_MORE_DATA;
            break;
        } 
#endif

        if ( (hwDeviceExtension->DeviceMappingFailed == FALSE) &&
             (GetDdcInformation(HwDeviceExtension,
                              (PUCHAR) pvChildDescriptor,
                              ChildEnumInfo->ChildDescriptorSize))    )
        {
            *pHwId = QUERY_MONITOR_ID;

            VideoDebugPrint((1, "NvGetChildDescriptor - successfully read EDID structure\n"));

        } else {

            //
            // Alway return TRUE, since we always have a monitor output
            // on the card and it just may not be a detectable device.
            //

            *pHwId = QUERY_NONDDC_MONITOR_ID;

            VideoDebugPrint((1, "NvGetChildDescriptor - DDC not supported\n"));

        }

        Status = ERROR_MORE_DATA;
        break;

#ifdef NVPE
        case 2:
#define NVCAP_I2C_DEVICE_ID     0xCA000002

                // read pnpid of child capture driver from registry...
                Status = VideoPortGetRegistryParameters(
                                HwDeviceExtension,
                                L"pnpid_cap",
                                FALSE,
                                NVRegistryCallbackPnpId,
                                (PVOID)(pvChildDescriptor));
                if (Status == NO_ERROR)
                {
                    // Check to see if I2C hardware is available
                    //if (I2cHardwarePresent())

                    *pHwId = NVCAP_I2C_DEVICE_ID;
                    *pChildType = Other;
                    //VideoPortMoveMemory(pvChildDescriptor, L"nvcap", sizeof(L"nvcap")); //strPnpId, wcslen(strPnpId));
                    Status = ERROR_MORE_DATA;  // we want to be called again...
                }
                else
                    Status = ERROR_INVALID_NAME;

                break;

        case 3:
#define NVXBAR_I2C_DEVICE_ID    0xCA000003

                // read pnpid of child capture driver from registry...
                Status = VideoPortGetRegistryParameters(
                                HwDeviceExtension,
                                L"pnpid_xbar",
                                FALSE,
                                NVRegistryCallbackPnpId,
                                (PVOID)(pvChildDescriptor));
                if (Status == NO_ERROR)
                {
                    // Check to see if I2C hardware is available
                    //if (I2cHardwarePresent())

                    *pHwId = NVXBAR_I2C_DEVICE_ID;
                    *pChildType = Other;
                    Status = ERROR_MORE_DATA;  // we want to be called again...
                }
                else
                    Status = ERROR_INVALID_NAME;

                break;

        case 4:
#define NVTUNER_I2C_DEVICE_ID   0xCA000004

                // read pnpid of child capture driver from registry...
                Status = VideoPortGetRegistryParameters(
                                HwDeviceExtension,
                                L"pnpid_tuner",
                                FALSE,
                                NVRegistryCallbackPnpId,
                                (PVOID)(pvChildDescriptor));
                if (Status == NO_ERROR)
                {
                    // Check to see if I2C hardware is available
                    //if (I2cHardwarePresent())

                    *pHwId = NVTUNER_I2C_DEVICE_ID;
                    *pChildType = Other;
                    Status = ERROR_MORE_DATA;  // we want to be called again...
                }
                else
                    Status = ERROR_INVALID_NAME;

                break;
#endif  // #ifdef NVPE


    case DISPLAY_ADAPTER_HW_ID:

        //
        // Special ID to handle return legacy PnP IDs for root enumerated
        // devices.
        //

        *pChildType = VideoChip;
        *pHwId      = DISPLAY_ADAPTER_HW_ID;

        Status = ERROR_MORE_DATA;
        break;


    default:

        Status = ERROR_NO_MORE_DEVICES;
        break;
    }


    return Status;
}

#ifdef NV_PWR_MGMT

#ifdef ENABLE_RESMAN_ACPI_CODE

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


    if ((HwDeviceId == QUERY_NONDDC_MONITOR_ID) ||
        (HwDeviceId == QUERY_MONITOR_ID)) {
    
        // We deal with only the head0 since the Win2K OS is aware of only head0.
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
        default:
            ASSERT(FALSE);
            return ERROR_INVALID_PARAMETER;
            break;
        }

        // The requested monitor power state is not supported by ResMan.
        VideoDebugPrint((0,"Oops! NVGetPowerState for Monitor, QueriedState: 0x%x not supported\n",
                        VideoPowerManagement->PowerState));
        return(ERROR_INVALID_PARAMETER);

    } else if (HwDeviceId == DISPLAY_ADAPTER_HW_ID) 

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

    if ((HwDeviceId == QUERY_NONDDC_MONITOR_ID) ||
        (HwDeviceId == QUERY_MONITOR_ID))
    {

        ULONG i, ulHead, State;
        BOOL bRet;

        for (i = 0; i < HwDeviceExtension->DeskTopInfo.ulNumberDacsActive; i++)
        {
            ulHead = HwDeviceExtension->DeskTopInfo.ulDeviceDisplay[i];

            switch (VideoPowerManagement->PowerState)
            {
            
            case VideoPowerOn:
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
        return(TRUE);
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


                if ( (HwDeviceExtension->curPowerState == VideoPowerHibernate)  ||
                     (HwDeviceExtension->curPowerState == VideoPowerStandBy)    ||
                     (HwDeviceExtension->curPowerState == VideoPowerSuspend)    ||
                     (HwDeviceExtension->curPowerState == VideoPowerOff))
                {
                    VIDEO_REQUEST_PACKET requestPacket;
                    STATUS_BLOCK statusblock;


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

                HwDeviceExtension->curPowerState = VideoPowerManagement->PowerState;
                return(TRUE);
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


                HwDeviceExtension->curPowerState = VideoPowerManagement->PowerState;

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

                // Call the ResMan to set the adapter power down to the required state
                // The 'head' arguement is irrelevant since we are setting the adapter power state.
                bRet = RmSetPowerState(HwDeviceExtension->DeviceReference, 0, State);
                if (bRet == FALSE)
                {
                    VideoDebugPrint((0,"Oops! RmSetPowerState returned FALSE for adapter for State: 0x%x\n",
                                 State));
                }

                return(TRUE);

            }


            break;




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
        return(TRUE);
    }

    VideoDebugPrint((0, "Unknown HwDeviceId\n"));
    ASSERT(FALSE);
    return ERROR_INVALID_PARAMETER;

}



#else  ENABLE_RESMAN_ACPI_CODE
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


    if ((HwDeviceId == QUERY_NONDDC_MONITOR_ID) ||
        (HwDeviceId == QUERY_MONITOR_ID)) {

        VIDEO_X86_BIOS_ARGUMENTS biosArguments;

        //
        // We are querying the power support for the monitor.
        //

        if ((VideoPowerManagement->PowerState == VideoPowerOn) ||
            (VideoPowerManagement->PowerState == VideoPowerHibernate)) {

            return NO_ERROR;
        }

        VideoPortZeroMemory(&biosArguments, sizeof(VIDEO_X86_BIOS_ARGUMENTS));

        biosArguments.Eax = VESA_POWER_FUNCTION;
        biosArguments.Ebx = VESA_GET_POWER_FUNC;

        //**********************************************************************
        // For multi-monitor, we're not allowed to call the BIOS.
        // So we'll handle Power Management ourselves in the miniport.
        //**********************************************************************

//        VideoPortInt10(HwDeviceExtension, &biosArguments);
//
//        if ((biosArguments.Eax & 0xffff) == VESA_STATUS_SUCCESS) {
//
//            switch (VideoPowerManagement->PowerState) {
//
//            case VideoPowerOn:
//            case VideoPowerHibernate:
//                return NO_ERROR;
//
//            case VideoPowerStandBy:
//                return (biosArguments.Ebx & VESA_POWER_STANDBY) ?
//                       NO_ERROR : ERROR_DEVICE_REINITIALIZATION_NEEDED;
//
//            case VideoPowerSuspend:
//                return (biosArguments.Ebx & VESA_POWER_SUSPEND) ?
//                       NO_ERROR : ERROR_DEVICE_REINITIALIZATION_NEEDED;
//
//            case VideoPowerOff:
//                return (biosArguments.Ebx & VESA_POWER_OFF) ?
//                       NO_ERROR : ERROR_DEVICE_REINITIALIZATION_NEEDED;
//
//            default:
//                return ERROR_INVALID_PARAMETER;
//            }
//
//        } else {
//
//            VideoDebugPrint((1, "This device does not support Power Management.\n"));
//            return ERROR_DEVICE_REINITIALIZATION_NEEDED;
//        }


        //**********************************************************************
        // This call probably isn't necessary since we support all power states
        //**********************************************************************

        if (NV_VBE_DPMS_GetPowerState(HwDeviceExtension, &CurrentPowerState)== FALSE)
            {
            VideoDebugPrint((0, "Failed DPMS call.\n"));
            ASSERT(FALSE);
            return ERROR_DEVICE_REINITIALIZATION_NEEDED;
            }

        return NO_ERROR;



    } else if (HwDeviceId == DISPLAY_ADAPTER_HW_ID) {

        //
        // We are querying power support for the graphics card.
        //

        switch (VideoPowerManagement->PowerState) {

            case VideoPowerOn:
            case VideoPowerHibernate:
            case VideoPowerStandBy:
            case VideoPowerSuspend:
            case VideoPowerOff:
                return NO_ERROR;
                break;




            default:
                ASSERT(FALSE);
                return ERROR_INVALID_PARAMETER;
                break;
        }

    } else {

        VideoDebugPrint((0, "Unknown HwDeviceId"));
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

    if ((HwDeviceId == QUERY_NONDDC_MONITOR_ID) ||
        (HwDeviceId == QUERY_MONITOR_ID)) {

        VIDEO_X86_BIOS_ARGUMENTS biosArguments;

        VideoPortZeroMemory(&biosArguments, sizeof(VIDEO_X86_BIOS_ARGUMENTS));

        biosArguments.Eax = VESA_POWER_FUNCTION;
        biosArguments.Ebx = VESA_SET_POWER_FUNC;

        switch (VideoPowerManagement->PowerState) {

        case VideoPowerOn:
        case VideoPowerHibernate:
            biosArguments.Ebx |= VESA_POWER_ON;
            break;

        case VideoPowerStandBy:
            biosArguments.Ebx |= VESA_POWER_STANDBY;
            break;

        case VideoPowerSuspend:
            biosArguments.Ebx |= VESA_POWER_SUSPEND;
            break;

        case VideoPowerOff:
            biosArguments.Ebx |= VESA_POWER_OFF;
            break;

        default:
            VideoDebugPrint((0, "Unknown power state.\n"));
            ASSERT(FALSE);
            return ERROR_INVALID_PARAMETER;
        }

        //**********************************************************************
        // For multi-monitor, we're not allowed to call the BIOS.
        // So we'll handle Power Management ourselves in the miniport.
        //**********************************************************************

//        VideoPortInt10(HwDeviceExtension, &biosArguments);

        if (NV_VBE_DPMS_SetPowerState(HwDeviceExtension, VideoPowerManagement->PowerState)== FALSE)
            {
            VideoDebugPrint((0, "Failed DPMS call.\n"));
            ASSERT(FALSE);
            return ERROR_INVALID_PARAMETER;
            }

        return NO_ERROR;

    } else if (HwDeviceId == DISPLAY_ADAPTER_HW_ID) {

        switch (VideoPowerManagement->PowerState) {
            case VideoPowerOn:



                //
                // If the device was previously turned off, we need to reinitialize it.
                //


                if ( (HwDeviceExtension->curPowerState == VideoPowerHibernate)  ||
                     (HwDeviceExtension->curPowerState == VideoPowerStandBy)    ||
                     (HwDeviceExtension->curPowerState == VideoPowerSuspend)    ||
                     (HwDeviceExtension->curPowerState == VideoPowerOff))

                    {
                    VIDEO_REQUEST_PACKET requestPacket;
                    STATUS_BLOCK statusblock;

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
                    if (HwDeviceExtension->AMD751ChipsetFound == TRUE) {
                        NVAMD751ChipsetRestoreHibernationRegisters(HwDeviceExtension);
                    }
#endif // IA64

                    //**********************************************************
                    // Make absolutely sure bus mastering gets enabled.
                    // Some motherboard BIOS's don't turn it on
                    //**********************************************************

                    NVEnableBusMastering(HwDeviceExtension);

                    //**********************************************************
                    // Make absolutely sure VGA IO subsystem is enabled !!!!
                    // Some motherboard BIOS's don't turn it on
                    //**********************************************************

                    NVEnableVGASubsystem(HwDeviceExtension);

                    //**********************************************************
                    // Make sure to clear the NV3 'mutex' hardware register
                    //**********************************************************

                    NVClearMutexPmeAudBuff0(HwDeviceExtension);

                    //**********************************************************
                    // HIBERNATION:
                    // -----------
                    // If coming back from hibernation, ONLY post the card
                    // if it's NOT primary (primary cards are always posted when coming
                    // back from hibernation, secondary cards are NOT posted,
                    // so we need to post the card)
                    //
                    // S1 OR S3 STANDBY:
                    // ----------------
                    // Always POST the NV card (if coming back from S1 or S3 Standby)
                    //
                    // Preferably, we should only POST an NV card when coming back
                    // from S3 standby (power is removed from card) and not S1 (power remains).
                    // That's because, when posting a card, the card's OWN bios should be used
                    // to post the card, because each vendor's cards may vary in different ways.
                    // The POST code that we include here in the miniport is 'fallback' BIOS
                    // post code and has only been tested on reference NV boards.
                    // If systems were to always post the video card when resuming
                    // from S3 standby, this would NOT be a problem.  We wouldn't
                    // have to include fallback Bios post code here...
                    //
                    // But some systems (ie. Intel Kahneeta) allows users
                    // to DISABLE posting of their card when resuming from S3 Standby.
                    // So, we've included BIOS post code in our miniport driver (RmPostNvDevice)
                    // as a fall back.  Bottom line: Someone has to post the card
                    // if power was removed.  Preferably, the card's own BIOS should do it.
                    // Otherwise, the miniport will have to do it.
                    //
                    // Unfortunately, it doesn't seem like we can distinguish between
                    // S1 standby (where power is NOT removed) and S3 standby (power IS removed),
                    // since both use 'VideoPowerOff'. In addition, we have no idea
                    // if the user has DISABLED Bios Posting on S3 Resume, in the
                    // system BIOS setup.
                    //
                    // To make a long story short, it seems that we'll ALWAYS
                    // have to post our card when resuming from S1 or S3 Standby.
                    // (so the card may actually end up getting posted TWICE unecessarily)
                    //**********************************************************

                    if  (  (HwDeviceExtension->curPowerState != VideoPowerHibernate)  ||
                           ((HwDeviceExtension->curPowerState == VideoPowerHibernate) && (HwDeviceExtension->NonPrimaryDevice == TRUE)))
                        {

                        if ( (HwDeviceExtension->ulChipID == NV3_DEVICE_NV3_ID) &&
                             ((ULONG)(*(PULONG)(HwDeviceExtension->FrameBufferPtr)) == 0x33557799))
                            {
                            //**********************************************************
                            // Due to an S3 standby bug with NV3 and BIOS posting, we'll try to determine manually
                            // if this was S1 (Power stays on) or S3 (power removed).
                            // If power was removed, then we'll post NV3.  Otherwise, we won't post NV3
                            //**********************************************************

                            ; // Don't Post NV3 for S1 standby
                            }

                        else


                            {
                            if (!RmPostNvDevice(HwDeviceExtension->DeviceReference,(PHWREG)(&(HwDeviceExtension->SavedBiosImageArray[0])),(PHWREG)(HwDeviceExtension->FrameBufferBase)))
                                {
                                VideoDebugPrint((0, "ERROR:  ----------------------------- !!!! "));
                                VideoDebugPrint((0, "ERROR:  Could NOT Post the NV Device  !!!! "));
                                VideoDebugPrint((0, "ERROR:  Could NOT Post the NV Device  !!!! "));
                                VideoDebugPrint((0, "ERROR:  Could NOT Post the NV Device  !!!! "));
                                VideoDebugPrint((0, "ERROR:  ----------------------------- !!!! "));
                                }

                            }

                        }


                    //**********************************************************
                    // Restore Instance Memory and other registers
                    // which are NOT reloaded by RmLoadState
                    //**********************************************************

                    NVRestoreSpecificRegistersForPwrMgmt(HwDeviceExtension);

                    //**********************************************************
                    // AGP fixups for specific chipsets which have
                    // problems with AGP.
                    //**********************************************************

#if 0
                    RmUpdateAGPConfig(HwDeviceExtension->DeviceReference);
#endif

                    //**********************************************************
                    // Tell RM to reload device state.
                    //**********************************************************

                    if (!RmLoadState
                        (
                            HwDeviceExtension->DeviceReference,
                            HwDeviceExtension->LastWidth,
                            HwDeviceExtension->LastHeight,
                            HwDeviceExtension->LastDepth,
                            HwDeviceExtension->LastRefreshRate,
                            TRUE
                        )
                    )
                        {
                        return ERROR_INVALID_PARAMETER;
                        }
                    }
                    //**********************************************************
                    // Update the current power state
                    //**********************************************************

                    //
                    // Make sure the NV_PFIFO_CACHES register has the value 1 inorder to enable reassigning
                    // of the FIFO amongst different channels (clients). Otherwise, if you go down in standby/hibernate with 
                    // an OGL app, then on resumption the display driver hangs waiting on a notifier since this register
                    // would not have been enabled.
                    //
                    REG_WR32(0x00002500, 1);

                    HwDeviceExtension->curPowerState = VideoPowerManagement->PowerState;

                return NO_ERROR;
                break;


            case VideoPowerHibernate:
            case VideoPowerStandBy:
            case VideoPowerSuspend:
            case VideoPowerOff:

                {
                VIDEO_REQUEST_PACKET requestPacket;
                STATUS_BLOCK statusblock;


                HwDeviceExtension->curPowerState = VideoPowerManagement->PowerState;

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
                if (HwDeviceExtension->AMD751ChipsetFound == TRUE) {
                    NVAMD751ChipsetSaveHibernationRegisters(HwDeviceExtension);
                }
#endif // IA64
                    

                //**************************************************************
                // Save instance memory and registers which are not saved
                // by RmUnloadState
                //**************************************************************

                NVSaveSpecificRegistersForPwrMgmt(HwDeviceExtension);

                //**************************************************************
                // changing the mode destroys instance memory, unload (i.e. store away)
                // device state for the RM.
                //**************************************************************
                if (!RmUnloadState(HwDeviceExtension->DeviceReference))
                    {
                    return ERROR_INVALID_PARAMETER;
                    }


                }


                //**************************************************************
                // NV3 hack: Write a value (0x33557799) to the frame buffer and
                //           see if it's still present when we resume. This hopefully
                //           will allows us to distinguish between an S1 or S3 standby
                //**************************************************************

                if (HwDeviceExtension->ulChipID == NV3_DEVICE_NV3_ID)
                    {
                    ((ULONG)(*(PULONG)(HwDeviceExtension->FrameBufferPtr)) = 0x33557799);
                    }


                return NO_ERROR;
                break;




            default:
                //
                // Unrecognized power state.
                //
                ASSERT(FALSE);
                return ERROR_INVALID_PARAMETER;
                break;
        }

    } else {

        VideoDebugPrint((0, "Unknown HwDeviceId"));
        ASSERT(FALSE);
        return ERROR_INVALID_PARAMETER;
    }
}
#endif ENABLE_RESMAN_ACPI_CODE

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
    HwDeviceExtension->AMD751_BAR1_Mapped_Address = (PHWREG)MmMapIoSpace(physaddr.LowPart, RM_PAGE_SIZE, FALSE);
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

#endif // #ifdef NV_PWR_MGMT


#endif // #if (_WIN32_WINNT >= 0x0500)
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

    if (!NVIsPresent(hwDeviceExtension, &NV1Slot))
        return(ERROR_DEV_NOT_EXIST);


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


#if (_WIN32_WINNT >= 0x0500)

    // POST a non-primary device, if necessary
    VideoPortGetBusData(
        hwDeviceExtension,
        PCIConfiguration,
        NV1Slot,
        &pciData,
        0,
        PCI_COMMON_HDR_LENGTH
    );
#if defined(_WIN64)
    if (1)
#else
    if (
        !(pciData.Command & PCI_ENABLE_MEMORY_SPACE) ||
        !(pciData.Command & PCI_ENABLE_BUS_MASTER)
    )
#endif
    {
        VP_STATUS ntStatus = NO_ERROR;
        PHYSICAL_ADDRESS fbPhysAddr;
        ULONG qtyBytesSet, inIoSpace, fbLength;
        PVOID fbAddr;

        // enable the non-primary device on the PCI bus
        pciData.Command |= PCI_ENABLE_MEMORY_SPACE;
        pciData.Command |= PCI_ENABLE_BUS_MASTER;
        qtyBytesSet = VideoPortSetBusData(
            hwDeviceExtension,
            PCIConfiguration,
            NV1Slot,
            &pciData,
            0,
            PCI_COMMON_HDR_LENGTH
        );
        if (qtyBytesSet == 0)
        {
            return ERROR_INVALID_PARAMETER;
        }

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
#else
    // hook up the HAL state for the primary device
    if (!RmInitNvHal(hwDeviceExtension->DeviceReference))
    {
        return ERROR_DEV_NOT_EXIST;
    }
#endif



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
    // Check to see if VESA DMT monitor timings should be used
    //**************************************************************************
    hwDeviceExtension->bUseDMT = FALSE;
    status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       L"DMTOVERRIDE",
                                       FALSE,
                                       NVRegistryCallback,
                                       &(hwDeviceExtension->bUseDMT));


    //**************************************************************************
    // Check to see if there is a GTF method requested by the user instead ( DMTOVERRIDE has priority)
    //**************************************************************************
    hwDeviceExtension->bUseGTF = FALSE;

    // Try and get some kind of monitor timing selection
    status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                           L"MonitorTiming",
                                           FALSE,
                                           NVReadMonitorTimingModeCallback,
                                           &(hwDeviceExtension->bUseGTF));


    // The DMT overide has priority
    if(hwDeviceExtension->bUseDMT) {
        hwDeviceExtension->bUseGTF = FALSE;
    }


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

    //**************************************************************************
    // Indicate a successful completion status.
    //**************************************************************************

    return NO_ERROR;

    } // NVFindAdapter()


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
    // Map the frame buffer here, so the Resource Manager can also get access to it,
    // instead of enablehardware in the display driver.
    //**************************************************************************

    Status = NVMapFrameBuffer(hwDeviceExtension);
    if (Status != NO_ERROR)
        return(FALSE);

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
    hwDeviceExtension->curPowerState = VideoPowerOn;

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

    // One time initialization of the twinview structures.
    NVOneTimeInit_TwinView(hwDeviceExtension);




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
    if (
        !RmInitNvDevice(
            hwDeviceExtension->DeviceReference,
            hwDeviceExtension->NVAccessRanges[NV_PHYSICAL_DFB_ADDRESS].RangeStart.LowPart,
            hwDeviceExtension->FrameBufferBase
        )
    )
    {
        return FALSE;
    }

    // Detect the Head info and read in the EDIDs
    NVInitialize_DacsAndEDID(hwDeviceExtension);

    // One time initialization of the common mode set code fields.
    InitializeCommonModesetCode(hwDeviceExtension);

#ifdef ENABLE_RESMAN_ACPI_CODE
    // One time initialization of the mapping of power states from Win2K's states to nVidia's states.
    InitializeACPIStates(hwDeviceExtension);
#endif

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

#if 0
    if (hwDeviceExtension->TotalInstanceMemory > MAX_INSTANCE_MEM_SIZE)
        {
        VideoDebugPrint((0, "ERROR:  ----------------------------- !!!! "));
        VideoDebugPrint((0, "ERROR:  Need to increase size of      !!!! "));
        VideoDebugPrint((0, "ERROR:  SavedInstanceMemory buffer    !!!! "));
        VideoDebugPrint((0, "ERROR:  in HwDeviceExtension          !!!! "));
        VideoDebugPrint((0, "ERROR:  ----------------------------- !!!! "));
        }
#endif

    //**************************************************************************
    // Validate the modes.
    // This function call was originally in NVFindAdapter.
    // However, since it depends on the result of GetMonitorType,
    // (whether we're running on a TV or not), it needs to occur
    // AFTER GetMonitorType.
    //**************************************************************************

    NVValidateModes(hwDeviceExtension);

#if 0 // NUGOOP
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
#endif

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

       // Attemp the resman call for the encoder info...
       if(RmConfigGetExKernel(hwDeviceExtension->DeviceReference,
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
    

#if (_WIN32_WINNT >= 0x0500)
    // Initializing the macrovision
    hwDeviceExtension->ulCurrentMacrovisionKey = -1;
    hwDeviceExtension->ulNextMacrovisionKey = 0x99999999;
    hwDeviceExtension->ulMacrovisionMode = 3;
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
    ULONG AliVirtualAddress;
    PHWINFO NVInfo;

    //**************************************************************************
    // Switch on the IoContolCode in the RequestPacket. It indicates which
    // function must be performed by the driver.
    //**************************************************************************

    VideoDebugPrint((2, "W32StartIO Entry - %08.8x\n", RequestPacket->IoControlCode));

    switch (RequestPacket->IoControlCode) {

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
        //       To make a long story short, AdapterMemorySize was adjusted by
        //       64k in NVValidateModes (The last 64k of VRAM contains Instance memory).
        //       See NVValidateModes for more comments.
        //
        //       When sharing memory (for Direct X), we must make sure that
        //       we don't give anyone else access to the last 64k of offscreen VRAM.
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
        //       To make a long story short, AdapterMemorySize was adjusted by
        //       64k in NVValidateModes (The last 64k of VRAM contains Instance memory).
        //       See NVValidateModes for more comments.
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

        status = NVQueryAvailableModes(HwDeviceExtension,
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

        status = NVQueryNumberOfAvailableModes(HwDeviceExtension,
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

        status = NVQueryCurrentMode(HwDeviceExtension,
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

    

    //**************************************************************************
    // SET_CURRENT_MODE
    //**************************************************************************

        case IOCTL_VIDEO_SET_CURRENT_MODE:


        VideoDebugPrint((2, "NVStartIO - SetCurrentModes\n"));

        // Call the RM to enable hot key functionality
        RmSetDriverHotkeyHandling(hwDeviceExtension->DeviceReference, TRUE);


        status = NVSetMode(HwDeviceExtension,
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

                NV_SetPixMixBits(HwDeviceExtension);

                NVSetColorLookup(HwDeviceExtension,
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
          ULONG NvCplModeSetHasOccurred;
    
          NvCplModeSetHasOccurred = 1;

          VideoPortSetRegistryParameters(HwDeviceExtension,
                                      L"NvCplModeSetHasOccurred",
                                      &NvCplModeSetHasOccurred,
                                      sizeof(ULONG));
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

        // Call the RM to disable hot key functionality
        RmSetDriverHotkeyHandling(hwDeviceExtension->DeviceReference, FALSE);

        if(hwDeviceExtension->ulRmTurnedOffMonitor)
        {
            // PowerManagement turn off the monitor 
            // It is not necessary to call BIOS to reset the chip
            break;
        }


#ifdef  NT_BUILD_NV4
        // Reset the head to 0 because BIOS and VGA do not understand the multi-heads
        RmEnableHead(hwDeviceExtension->DeviceReference, 0);
#endif
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
            // For NT4, do a 'manual' text modeset for NV10 ONLY
            // We won't call the BIOS.
            //******************************************************************

            if ((hwDeviceExtension->ulChipID == NV10_DEVICE_NV10_ID)       ||
                (hwDeviceExtension->ulChipID == NV10DDR_DEVICE_NV10DDR_ID) ||
                (hwDeviceExtension->ulChipID == NV10GL_DEVICE_NV10GL_ID)   ||
                (hwDeviceExtension->ulChipID == NV11_DEVICE_NV11_ID)       ||
                (hwDeviceExtension->ulChipID == NV11DDR_DEVICE_NV11DDR_ID) ||
                (hwDeviceExtension->ulChipID == NV11M_DEVICE_NV11M_ID)     ||
                (hwDeviceExtension->ulChipID == NV11GL_DEVICE_NV11GL_ID)   ||
                (hwDeviceExtension->ulChipID == NV15_DEVICE_NV15_ID)       ||
                (hwDeviceExtension->ulChipID == NV15DDR_DEVICE_NV15DDR_ID) ||
                (hwDeviceExtension->ulChipID == NV15BR_DEVICE_NV15BR_ID)   ||
                (hwDeviceExtension->ulChipID == NV15GL_DEVICE_NV15GL_ID)   ||
                (hwDeviceExtension->ulChipID == NV20_DEVICE_NV20_ID)       ||
                (hwDeviceExtension->ulChipID == NV20_DEVICE_NV20_1_ID)     ||
                (hwDeviceExtension->ulChipID == NV20_DEVICE_NV20_2_ID)     ||
                (hwDeviceExtension->ulChipID == NV20_DEVICE_NV20_3_ID) )
                {
                NV_ManualTextModeSet(hwDeviceExtension);
                }

            else

            //******************************************************************
            // For NT4, use Int10 if NOT an NV10  (ie nv3,nv4,nv5,vanta,m64)
            //******************************************************************

                {
                VideoPortZeroMemory(&biosArguments, sizeof(VIDEO_X86_BIOS_ARGUMENTS));
                biosArguments.Eax = 0x0003;
                status = VideoPortInt10(hwDeviceExtension,&biosArguments);
                if (status != NO_ERROR)
                    VideoDebugPrint((0, "NV: IOCTL_VIDEO_RESET_DEVICE FAILED\n"));
                }

#else

            //******************************************************************
            // For Win2k, always use Int10
            //******************************************************************

            VideoPortZeroMemory(&biosArguments, sizeof(VIDEO_X86_BIOS_ARGUMENTS));
            biosArguments.Eax = 0x0003;
            status = VideoPortInt10(hwDeviceExtension,&biosArguments);
            if (status != NO_ERROR)
                VideoDebugPrint((0, "NV: IOCTL_VIDEO_RESET_DEVICE FAILED\n"));

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

        status = NVSetColorLookup(HwDeviceExtension,
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
            NV_SetPixMixBits(HwDeviceExtension);

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
            AliVirtualAddress   = 0;

            //******************************************************************
            // Map the IO port, and pass it back to the display driver
            //******************************************************************

            status = VideoPortMapMemory(hwDeviceExtension,
                                     AliFixupBaseAddress,
                                     &(IOLength),
                                     &(inIoSpace),
                                     (PVOID)&(AliVirtualAddress));

            memoryInformation->FrameBufferBase   = (PVOID)AliVirtualAddress;
            memoryInformation->FrameBufferLength = TRUE;
            hwDeviceExtension->AliFixupIOBase    = (PVOID)AliVirtualAddress;
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
    // GET_DEVICE_REF
    //**************************************************************************

    case IOCTL_VIDEO_GET_DEVICE_REF:

        VideoDebugPrint((2, "NVStartIO - IOCTL_VIDEO_GET_DEVICE_REF\n"));

        if (RequestPacket->OutputBufferLength  == sizeof(ULONG))
        {
            *((ULONG *) RequestPacket->OutputBuffer) = hwDeviceExtension->DeviceReference;
            RequestPacket->StatusBlock->Information = sizeof(ULONG);
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

        VideoDebugPrint((0, "SHARE: pSystemMdl = 0x%p\n",
                        pSysmemShareMemoryOut->pSystemMdl));
        VideoDebugPrint((0, "SHARE: physicalAddress = 0x%p\n",
                        pSysmemShareMemoryOut->physicalAddress));
        VideoDebugPrint((0, "SHARE: ddVirtualAddress = 0x%p\n",
                        pSysmemShareMemoryOut->ddVirtualAddress));

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
            if (!(hwDeviceExtension->AgpVirtualBaseAddress =
                    VideoPortAgpReserveVirtual(
                        hwDeviceExtension,
                        (HANDLE)pBlock->hContext,
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
                status = NVPEProcessCommand(HwDeviceExtension, RequestPacket);
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

                status = VideoPortSetRegistryParameters(HwDeviceExtension,
                                       L"TwinviewInfo",
                                       pInfo,
                                       sizeof(NVTWINVIEWDATA));
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


                status = VideoPortGetRegistryParameters(
                                       HwDeviceExtension,
                                       L"TwinviewInfo",
                                       FALSE,
                                       NVReadRegistryTwinViewInfoCallback,
                                       pInfo);
                if (status == NO_ERROR)
                    RequestPacket->StatusBlock->Information = sizeof(NVTWINVIEWDATA);
                else
                    RequestPacket->StatusBlock->Information = 0;
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

                // Extract the info from the HwDeviceExtension.
                pDeskTopInfo = (NV_DESKTOP_INFO *)&hwDeviceExtension->DeskTopInfo;

                // Stuff the info to send back to the display driver.
                pInfo->ulNumberDacsOnBoard = pDeskTopInfo->ulNumberDacsOnBoard;
                pInfo->ulNumberDacsConnected = pDeskTopInfo->ulNumberDacsConnected;
                pInfo->ulAllDeviceMask = pDeskTopInfo->ulAllDeviceMask;
                pInfo->ulConnectedDeviceMask = pDeskTopInfo->ulConnectedDeviceMask;
                for (i=0; i < pDeskTopInfo->ulNumberDacsOnBoard; i++)
                {
                    pInfo->ulDeviceMask[i] = pDeskTopInfo->ulDeviceMask[i];
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
                

                VideoDebugPrint((0, "IOCTL_VIDEO_INDUCE_HEAD_DEVICE_SCANNING():\n"));
                status = NO_ERROR;
                bScanHeadDeviceOptions(hwDeviceExtension);
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


                VideoDebugPrint((3,"IOCTL_GET_NUMBER_DACS: Number of PhysicalModes for head: %d returned is: %d\n",
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

                status = VideoPortSetRegistryParameters(HwDeviceExtension,
                                       pRegStruct->keyName,
                                       pRegStruct->keyVal,
                                       pRegStruct->keyValSize);
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

                status = VideoPortGetRegistryParameters(HwDeviceExtension,
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
                PNV_REGISTRY_STRUCT pRegStructOutput;

                
                pRegStructInput = (PNV_REGISTRY_STRUCT)(RequestPacket->InputBuffer);
                regKeyName = (PWSTR)(pRegStructInput->keyName);
                pRegStructOutput = (PNV_REGISTRY_STRUCT)(RequestPacket->OutputBuffer);
                // Copy the required values into the output structure
                pRegStructOutput->keyVal = pRegStructInput->keyVal;
                pRegStructOutput->keyValSize = pRegStructInput->keyValSize;

                status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       regKeyName,
                                       FALSE,
                                       NVReadRegistryBinaryCallback,
                                       (PVOID)pRegStructOutput);
            if (status == NO_ERROR)
            {
                RequestPacket->StatusBlock->Information = pRegStructOutput->keyValSize;
            }
            else
            {
                RequestPacket->StatusBlock->Information = 0;
                pRegStructOutput->keyValSize = 0;
            }
                 

            }
            break;



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
                  if(NVValidateDeviceMode(HwDeviceExtension, vmx)) {

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

               status = NVGetColorLookup(HwDeviceExtension,
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
                MmMapIoSpace(shareAddress.LowPart,
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

#if _WIN32_WINNT >= 0x0500
        //**************************************************************************
        // GET_SYSTEM_MEMORY_INFO
        //**************************************************************************

        case IOCTL_VIDEO_GET_SYSTEM_MEMORY_INFO:
            {
            NV_SYSMEM_INFO  *pSysMemInfo;

            VideoDebugPrint((2, "W32StartIO - GetSystemMemoryInfo\n"));

            if (RequestPacket->OutputBufferLength < sizeof(NV_SYSMEM_INFO))
                {
                VideoDebugPrint((0, "IOCTL_VIDEO_GET_SYSTEM_MEMORY_INFO - ERROR_INSUFFICIENT_BUFFER\n"));
                status = ERROR_INSUFFICIENT_BUFFER;
                break;
                }

            pSysMemInfo = (NV_SYSMEM_INFO *) RequestPacket->OutputBuffer;
            pSysMemInfo->SystemMemorySize =
                hwDeviceExtension->SystemMemorySize;
            pSysMemInfo->AgpAllocationLimit =
                hwDeviceExtension->AgpServices.AllocationLimit;
            RequestPacket->StatusBlock->Information =
                                        sizeof(NV_SYSMEM_INFO);

            status = NO_ERROR;
            }
            break;
#endif // _WIN32_WINNT >= 0x0500


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
        VideoDebugPrint((0, "Oops!TwinViewCallback(): size mismatch Diskcopy size: %d, struct size: %d\n",
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
        VideoDebugPrint((0, "GTF overide detected\n"));
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


#ifdef ENABLE_HEAD_API


//
// Detects the number of heads present and reads in the EDID for each head.
//

BOOLEAN NVInitialize_DacsAndEDID(PHW_DEVICE_EXTENSION HwDeviceExtension)
{
    ULONG Count = 0;
    BOOLEAN bRet = TRUE;
    NV_DESKTOP_INFO * pInfo;
    LONG i,j;
    ULONG ulConnectedDeviceMask;

    VideoDebugPrint((2, "INVInitialize_DacsAndEDID():\n"));


    pInfo = (NV_DESKTOP_INFO *) &HwDeviceExtension->DeskTopInfo;
    pInfo->ulNumberDacsOnBoard = 1;
    pInfo->ulNumberDacsConnected = 1;

    // Get the bitmask for all the connectors in the system.
    pInfo->ulAllDeviceMask = BITMASK_INVALID_DEVICE;
    RmConfigGetKernel(HwDeviceExtension->DeviceReference, NV_CFG_GET_ALL_DEVICES, &pInfo->ulAllDeviceMask);
    VideoDebugPrint((0,"ulAllDeviceMask: 0x%x\n",pInfo->ulAllDeviceMask));

    // Get the connect status for all the connectors.
    pInfo->ulConnectedDeviceMask = pInfo->ulAllDeviceMask;
    ulConnectedDeviceMask = pInfo->ulConnectedDeviceMask;

    if(RmConfigGetExKernel(HwDeviceExtension->DeviceReference,
                              NV_CFGEX_CHECK_CONNECTED_DEVICES, &pInfo->ulConnectedDeviceMask, 
                              sizeof(pInfo->ulConnectedDeviceMask))) 
           
       {

            // success.
            VideoDebugPrint((0,"ulConnectedDeviceMask: 0x%x\n",pInfo->ulConnectedDeviceMask));
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
        // if there are two dacs on board, detect to see if both are connected.
        // do the same for the case of a single head board also.
        if (pInfo->ulNumberDacsOnBoard == 2 || pInfo->ulNumberDacsOnBoard == 1)
        {
            ULONG Count = 0;
            if (ConvertConnectedMaskToNumDevices(ulConnectedDeviceMask) > 1)
            {
                pInfo->ulNumberDacsConnected = 2;
            }

            if (ConvertConnectedMaskToNumDevices(ulConnectedDeviceMask) == 1)
            {
                pInfo->ulNumberDacsConnected = 1;
            }

            if (ConvertConnectedMaskToNumDevices(ulConnectedDeviceMask) == 0)
            {
                VideoDebugPrint((0,"OOops! ResMan could not detect any connected monitors. So assume CRT connected to head 0\n"));
                pInfo->ulNumberDacsOnBoard = 1;
                pInfo->ulNumberDacsConnected = 1;
                pInfo->ulConnectedDeviceMask = BITMASK_CRT0;
            }



            // Print the info collected
            VideoDebugPrint((0,"DacsOnBoard: %d, DacsConnected: %d, AllDeviceMask: 0x%x, ConnectedDeviceMask: 0x%x,\n",
                             pInfo->ulNumberDacsOnBoard, pInfo->ulNumberDacsConnected,pInfo->ulAllDeviceMask,  pInfo->ulConnectedDeviceMask));
        }
    } // NumDacsOnBoard == 2 || 1


    VideoDebugPrint((2,"Number of heads on board returning to display driver: 0x%x, number connected: 0x%x\n",
        pInfo->ulNumberDacsOnBoard,pInfo->ulNumberDacsConnected));

    // Initialize all the EDID related fields
    for (i=0; i < NV_NO_CONNECTORS; i++)
    {   
        for (j=0; j < EDID_V2_SIZE; j++)
        {
            HwDeviceExtension->EDIDBuffer[i][j] = 0;
        }
        HwDeviceExtension->EDIDBufferValid[i] = 0;
        HwDeviceExtension->EDIDBufferSize[i] = 0;
    }

    //
    // Now read the EDID for all the connected devices.
    // Get the EDID buffer from the RM always.
    //
    for (i=0; i < NV_NO_CONNECTORS; i++)
    {
        ULONG ulDeviceMask;
        ulDeviceMask = (1 << i);

        // See if this device is connected
        if (ulConnectedDeviceMask & ulDeviceMask)
        {
            NV_CFGEX_GET_LOGICAL_DEV_EDID_PARAMS EdidParams;
            ULONG EdidBufferSize;

            EdidBufferSize          = EDID_V2_SIZE;
            EdidParams.displayMap = ulDeviceMask;
            EdidParams.edidBuffer   = (UCHAR *) &(HwDeviceExtension->EDIDBuffer[i][0]);
            EdidParams.bufferLength = &EdidBufferSize;

            if (!RmConfigGetExKernel(HwDeviceExtension->DeviceReference, NV_CFGEX_GET_LOGICAL_DEV_EDID, &EdidParams, sizeof(NV_CFGEX_GET_LOGICAL_DEV_EDID_PARAMS) ))
            {
                VideoDebugPrint((0, "oops!! RmConfigGetExKernel() returned error for NV_CFGEX_GET_LOGICAL_DEV_EDID for DeviceMask: 0x%x\n",ulDeviceMask));
                // Reset all the data fields.
                for (j=0; j < EDID_V2_SIZE; j++)
                {
                    HwDeviceExtension->EDIDBuffer[i][j] = 0;
                }
                HwDeviceExtension->EDIDBufferValid[i] = 0;
                HwDeviceExtension->EDIDBufferSize[i] = 0;

            }
            else
            {
                HwDeviceExtension->EDIDBufferValid[i] = 1;
                HwDeviceExtension->EDIDBufferSize[i] = EdidBufferSize;
            }
    
        }
    }

    return(TRUE);
}




//
// Returns TRUE if the device list is successfully scanned
//
BOOLEAN bScanHeadDeviceOptions(PHW_DEVICE_EXTENSION HwDeviceExtension)
{
    NVInitialize_DacsAndEDID(HwDeviceExtension);
    return(TRUE);
} // End function bScanHeadDeviceOptions()



#else ENABLE_HEAD_API

//
// Detects the number of heads present and reads in the EDID for each head.
//

BOOLEAN NVInitialize_DacsAndEDID(PHW_DEVICE_EXTENSION HwDeviceExtension)
{
    ULONG Count = 0;
    BOOLEAN bRet = TRUE;
    NV_DESKTOP_INFO * pInfo;
    LONG i;

    VideoDebugPrint((2, "INVInitialize_DacsAndEDID():\n"));


    pInfo = (NV_DESKTOP_INFO *) &HwDeviceExtension->DeskTopInfo;
    pInfo->ulNumberDacsOnBoard = 1;
    pInfo->ulNumberDacsConnected = 1;

    // Get the bitmask for all the connectors in the system.
    pInfo->ulAllDeviceMask = BITMASK_INVALID_DEVICE;
    RmConfigGetKernel(HwDeviceExtension->DeviceReference, NV_CFG_GET_ALL_DEVICES, &pInfo->ulAllDeviceMask);
    VideoDebugPrint((0,"ulAllDeviceMask: 0x%x\n",pInfo->ulAllDeviceMask));

    // Get the connect status for all the connectors.
    pInfo->ulConnectedDeviceMask = pInfo->ulAllDeviceMask;
    if(RmConfigGetExKernel(HwDeviceExtension->DeviceReference,
                              NV_CFGEX_CHECK_CONNECTED_DEVICES, &pInfo->ulConnectedDeviceMask, 
                              sizeof(pInfo->ulConnectedDeviceMask))) 
           
       {

            // success.
            VideoDebugPrint((0,"ulConnectedDeviceMask: 0x%x\n",pInfo->ulConnectedDeviceMask));

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
            // if there are two dacs on board, detect to see if both are connected.
            // do the same for the case of a single head board also.
            if (pInfo->ulNumberDacsOnBoard == 2 || pInfo->ulNumberDacsOnBoard == 1)
            {
                ULONG Count = 0;

                //
                // Now check to see if the head is really connected
                //
                for (i=0; i < (LONG)pInfo->ulNumberDacsOnBoard; i++)
                {
                    // Initialize the devicetype to some invalid value.
                    pInfo->ulDeviceMask[i] = BITMASK_INVALID_DEVICE;
                    pInfo->ulDeviceType[i] = INVALID_DEVICE_TYPE;

                    // Clear the flags first for the head device options.
                    pInfo->ulHeadDeviceOptions[i] = 0;

                    // We progressively test to see if a CRT/DFP/TV is connected on a given head.
                    // See if CRT is connected.
                    {
                        NV_CFGEX_GET_MONITOR_INFO_MULTI_PARAMS MonitorInfo;

                        MonitorInfo.Head = i;
                        MonitorInfo.MonitorConnected = 0;
                        if (!RmConfigGetExKernel (HwDeviceExtension->DeviceReference,
                                NV_CFGEX_GET_MONITOR_INFO_MULTI, &MonitorInfo, 
                                sizeof(NV_CFGEX_GET_MONITOR_INFO_MULTI_PARAMS))) 
                        {
                            VideoDebugPrint((0, "oops!! RmConfigGetExKernel() returned error for NV_CFGEX_GET_MONITOR_INFO_MULTI\n"));
                            bRet = FALSE;
                            break;
                        }
                        if (MonitorInfo.MonitorConnected == 1)
                        {
                            if (pInfo->ulDeviceType[i] == INVALID_DEVICE_TYPE) 
                            {
                                pInfo->ulDeviceType[i] = MONITOR_TYPE_VGA;
                                Count++;
                            }
                            pInfo->ulHeadDeviceOptions[i] |= HEAD_DEVICE_OPTION_CRT;
                        }
                    }

                    // See if DFP is connected
                    {
                        NV_CFGEX_GET_FLATPANEL_INFO_MULTI_PARAMS FPInfo;

                        FPInfo.Head = i;
                        FPInfo.FlatPanelConnected = 0;
                        if (!RmConfigGetExKernel (HwDeviceExtension->DeviceReference,
                             NV_CFGEX_GET_FLAT_PANEL_INFO_MULTI, &FPInfo, 
                             sizeof(NV_CFGEX_GET_FLATPANEL_INFO_MULTI_PARAMS))) 
                        {
                             VideoDebugPrint((0, "oops!! RmConfigGetExKernel() returned error for NV_CFGEX_GET_FLATPANEL_INFO_MULTI\n"));
                             bRet = FALSE;
                             break;
                        }
                        if (FPInfo.FlatPanelConnected == 1)
                        {
                            if (pInfo->ulDeviceType[i] == INVALID_DEVICE_TYPE) 
                            {
                                pInfo->ulDeviceType[i] = MONITOR_TYPE_FLAT_PANEL;
                                Count++;
                            }
                            pInfo->ulHeadDeviceOptions[i] |= HEAD_DEVICE_OPTION_DFP;
                        }
                    }
                    

                    // See if TV is connected.
                    {
                        NV_CFGEX_GET_TV_ENCODER_MULTI_PARAMS TVInfo;
                        ULONG TVType;
                        TVInfo.Head = i;
                        TVInfo.TVConnected = 0;
                        if (!RmConfigGetExKernel (HwDeviceExtension->DeviceReference,
                                NV_CFGEX_GET_TV_ENCODER_INFO_MULTI, &TVInfo, 
                                sizeof(NV_CFGEX_GET_TV_ENCODER_MULTI_PARAMS))) 
                        {
                            VideoDebugPrint((0, "oops!! RmConfigGetExKernel() returned error for NV_CFGEX_GET_TV_ENCODER_INFO_MULTI\n"));
                            bRet = FALSE;
                            break;
                        }
                        if (TVInfo.TVConnected == 1)
                        {
                            if (pInfo->ulDeviceType[i] == INVALID_DEVICE_TYPE) 
                            {
                                Count++;
                                // Detect a PAL or NTSC tv type.
                                // BUGBUG: Per Terry, ResMan can not detect
                                // a TV type. So lets assume a NTSC type.
                                pInfo->ulTVFormat[i] = NTSC_M;
                                pInfo->ulDeviceType[i] = MONITOR_TYPE_NTSC;
                            }
                            pInfo->ulHeadDeviceOptions[i] |= HEAD_DEVICE_OPTION_TV;
                        }
                    } // Detect TV
                } // for each DAC

                if (bRet == TRUE) 
                {
                    // Now for the case when the ResMan could not detect any monitors because the monitors are non-EDID or
                    // connected through a moniport port such as Omni-port.
                    // We will assume one monitor is connected in this case (for both single headed and dual headed 
                    // boards).
                    if (Count == 0)
                    {
                        VideoDebugPrint((0,"OOops! ResMan could not detect any connected monitors. So assume CRT connected to head 0\n"));
                        pInfo->ulNumberDacsOnBoard = 1;
                        pInfo->ulNumberDacsConnected = 1;
                        pInfo->ulAllDeviceMask = BITMASK_CRT0;
                        pInfo->ulConnectedDeviceMask = BITMASK_CRT0;
                        pInfo->ulDeviceMask[0] = BITMASK_CRT0;
                        pInfo->ulDeviceType[0] = MONITOR_TYPE_VGA;
                        pInfo->ulHeadDeviceOptions[0] = HEAD_DEVICE_OPTION_CRT;
                    }
                    else
                    {
                        pInfo->ulNumberDacsConnected = Count;
                    }
                    // Print the info collected
                    VideoDebugPrint((0,"DacsOnBoard: %d, DacsConnected: %d, AllDeviceMask: 0x%x, ConnectedDeviceMask: 0x%x,DeviceMask: (0x%x, 0x%x), DeviceType: (0x%x, 0x%x), DeviceOptions: (0x%x, 0x%x)\n",
                        pInfo->ulNumberDacsOnBoard, pInfo->ulNumberDacsConnected,pInfo->ulAllDeviceMask,  pInfo->ulConnectedDeviceMask,  
                        pInfo->ulDeviceMask[0],pInfo->ulDeviceMask[1],
                        pInfo->ulDeviceType[0],pInfo->ulDeviceType[1],
                        pInfo->ulHeadDeviceOptions[0],pInfo->ulHeadDeviceOptions[1]));
                }
            } // NumDacsOnBoard == 2



        }


    VideoDebugPrint((2,"Number of heads on board returning to display driver: 0x%x, number connected: 0x%x\n",
        pInfo->ulNumberDacsOnBoard,pInfo->ulNumberDacsConnected));

    //
    // Now read the EDID for all the heads.
    // Get the EDID buffer from the RM always.
    //
    i = (LONG)pInfo->ulNumberDacsOnBoard;

    /* for some unknown reasons, probably because the resman doesn't restore some registers correctly, doing the
        scan from head 0 to 1 will cause garbage on the CRT after updating from vga to nv driver as well as causing
        win2k system hang when switching mode in twinview, 20000707-112729, 20000812-121158
        and 20000725-202707.
     */
    while (--i >= 0)
    {
        ULONG ulHead;
        NV_CFGEX_GET_EDID_MULTI_PARAMS EdidParams;
        ULONG EdidBufferSize;



        // Get the mapped head. Do not use the ulCurentDac here as we are cycling
        // through the heads.

        ulHead = HwDeviceExtension->DeskTopInfo.ulDeviceDisplay[i];
        HwDeviceExtension->EDIDBufferValid[ulHead] = 0;
        HwDeviceExtension->EDIDBufferSize[ulHead] = 0;
        EdidBufferSize = 0;

        switch (pInfo->ulDeviceType[ulHead]) 
        {
        case MONITOR_TYPE_VGA:
            EdidBufferSize          = EDID_V2_SIZE;
            EdidParams.displayType  = DISPLAY_TYPE_MONITOR;
            EdidParams.Head  = ulHead;
            
            EdidParams.edidBuffer   = (UCHAR *) &(HwDeviceExtension->EDIDBuffer[ulHead][0]);
            EdidParams.bufferLength = &EdidBufferSize;
            if (!RmConfigGetExKernel(HwDeviceExtension->DeviceReference, NV_CFGEX_GET_EDID_MULTI, &EdidParams, sizeof(NV_CFGEX_GET_EDID_MULTI_PARAMS) ))
            {
                VideoDebugPrint((0, "oops!! RmConfigGetExKernel() returned error for NV_CFGEX_GET_EDID for head: 0x%x, deviceType: 0x%x\n", 
                                 ulHead, pInfo->ulDeviceType[ulHead]));
                bRet = FALSE;
            }
            else
            {
                HwDeviceExtension->EDIDBufferValid[ulHead] = 1;
                HwDeviceExtension->EDIDBufferSize[ulHead] = EdidBufferSize;
            }
            break;

        case MONITOR_TYPE_FLAT_PANEL:
            EdidBufferSize          = EDID_V2_SIZE;
            EdidParams.displayType  = DISPLAY_TYPE_FLAT_PANEL;
            EdidParams.Head  = ulHead;
            
            EdidParams.edidBuffer   = (UCHAR *) &(HwDeviceExtension->EDIDBuffer[ulHead][0]);
            EdidParams.bufferLength = &EdidBufferSize;
            if (!RmConfigGetExKernel(HwDeviceExtension->DeviceReference, NV_CFGEX_GET_EDID_MULTI, &EdidParams, sizeof(NV_CFGEX_GET_EDID_MULTI_PARAMS) ))
            {
                VideoDebugPrint((0, "oops!! RmConfigGetExKernel() returned error for NV_CFGEX_GET_EDID for head: 0x%x, deviceType: 0x%x\n", 
                                 ulHead, pInfo->ulDeviceType[ulHead]));
                bRet = FALSE;
            }
            else
            {
                HwDeviceExtension->EDIDBufferValid[ulHead] = 1;
                HwDeviceExtension->EDIDBufferSize[ulHead] = EdidBufferSize;
            }
            break;

        case MONITOR_TYPE_NTSC:
        case MONITOR_TYPE_PAL:
            // No EDID for TV.
            HwDeviceExtension->EDIDBufferValid[ulHead] = 0;
            HwDeviceExtension->EDIDBufferSize[ulHead] = 0;
            
            break;

        case INVALID_DEVICE_TYPE:
            // We could not detect any connected output device on this head so don't try reading the EDID
            HwDeviceExtension->EDIDBufferValid[ulHead] = 0;
            break;

        default:
            VideoDebugPrint((0,"OOops! NVInitialize_DacsAndEDID(): Invalid device type: 0x%x for MappedHead: 0x%x.\n",
                             pInfo->ulDeviceType[ulHead],ulHead));
            VideoDebugPrint((0,"No EDID being read for this head\n"));
            HwDeviceExtension->EDIDBufferValid[ulHead] = 0;
            HwDeviceExtension->EDIDBufferSize[ulHead] = 0;
            bRet = FALSE;
            break;
        } // switch deviceType

        // Write the EDID data to the registry for our own debug purposes.
        if (ulHead == 0)
        {
            VideoPortSetRegistryParameters(HwDeviceExtension,
                                      L"Head0_EDID_Buffer",
                                      &(HwDeviceExtension->EDIDBuffer[ulHead][0]),
                                      HwDeviceExtension->EDIDBufferSize[ulHead]);
            // Write the device type also for reference
            VideoPortSetRegistryParameters(HwDeviceExtension,
                                      L"Head0_EDID_DevType",
                                      &(pInfo->ulDeviceType[ulHead]),
                                      sizeof(ULONG));
        }
        if (ulHead == 1)
        {
            VideoPortSetRegistryParameters(HwDeviceExtension,
                                      L"Head1_EDID_Buffer",
                                      &(HwDeviceExtension->EDIDBuffer[ulHead][0]),
                                      HwDeviceExtension->EDIDBufferSize[ulHead]);
            // Write the device type also for reference
            VideoPortSetRegistryParameters(HwDeviceExtension,
                                      L"Head1_EDID_DevType",
                                      &(pInfo->ulDeviceType[ulHead]),
                                      sizeof(ULONG));
        }
    } // for each dac.
    
    return(bRet);
}




//
// Returns TRUE if the device list is successfully scanned
//
BOOLEAN bScanHeadDeviceOptions(PHW_DEVICE_EXTENSION HwDeviceExtension)
{
    ULONG Head;
    LONG i;
    ULONG Head0Connected = FALSE, Head1Connected = FALSE;

    i = (LONG)HwDeviceExtension->DeskTopInfo.ulNumberDacsOnBoard;

    /* for some unknown reasons, probably because the resman doesn't restore some registers correctly, doing the
        scan from head 0 to 1 will cause garbage on the CRT after updating from vga to nv driver as well as causing
        win2k system hang when switching mode in twinview, 20000707-112729, 20000812-121158
        and 20000725-202707.
   */
    while (--i >= 0)
    {
            Head = (ULONG)i;

            // Clear the flags first.
            HwDeviceExtension->DeskTopInfo.ulHeadDeviceOptions[Head] = 0;

            // See if CRT is connected.
            {
                NV_CFGEX_GET_MONITOR_INFO_MULTI_PARAMS MonitorInfo;

                MonitorInfo.Head = Head;
                MonitorInfo.MonitorConnected = 0;
                if (!RmConfigGetExKernel(HwDeviceExtension->DeviceReference,
                  NV_CFGEX_GET_MONITOR_INFO_MULTI, &MonitorInfo, sizeof(NV_CFGEX_GET_MONITOR_INFO_MULTI_PARAMS))) 
                {
                    VideoDebugPrint((0, "oops!! RmConfigGetExKernel() returned error for NV_CFGEX_GET_MONITOR_INFO_MULTI\n"));
                }
                if (MonitorInfo.MonitorConnected == 1)
                {
                    HwDeviceExtension->DeskTopInfo.ulHeadDeviceOptions[Head] |= HEAD_DEVICE_OPTION_CRT;
                    if (i == 0)
                    {
                        Head0Connected = TRUE;
                    }
                    if (i == 1)
                    {
                        Head1Connected = TRUE;
                    }
                    
                }
            }
            
            // See if DFP is connected
            {
                NV_CFGEX_GET_FLATPANEL_INFO_MULTI_PARAMS FPInfo;

                FPInfo.Head = Head;
                FPInfo.FlatPanelConnected = 0;
                if (!RmConfigGetExKernel(HwDeviceExtension->DeviceReference,
                  NV_CFGEX_GET_FLAT_PANEL_INFO_MULTI, &FPInfo, sizeof(NV_CFGEX_GET_FLATPANEL_INFO_MULTI_PARAMS))) 
                {
                    VideoDebugPrint((0, "oops!! RmConfigGetExKernel() returned error for NV_CFGEX_GET_FLATPANEL_INFO_MULTI\n"));
                }
                if (FPInfo.FlatPanelConnected == 1)
                {
                    HwDeviceExtension->DeskTopInfo.ulHeadDeviceOptions[Head] |= HEAD_DEVICE_OPTION_DFP;
                    if (i == 0)
                    {
                        Head0Connected = TRUE;
                    }
                    if (i == 1)
                    {
                        Head1Connected = TRUE;
                    }
                }
            }

            // See if TV is connected.
            {
                NV_CFGEX_GET_TV_ENCODER_MULTI_PARAMS TVInfo;
                ULONG TVType;
                TVInfo.Head = Head;
                TVInfo.TVConnected = 0;
                if (!RmConfigGetExKernel(HwDeviceExtension->DeviceReference,
                  NV_CFGEX_GET_TV_ENCODER_INFO_MULTI, &TVInfo, sizeof(NV_CFGEX_GET_TV_ENCODER_MULTI_PARAMS))) 
                {
                    VideoDebugPrint((0, "oops!! RmConfigGetExKernel() returned error for NV_CFGEX_GET_TV_ENCODER_INFO_MULTI\n"));
                }
                if (TVInfo.TVConnected == 1)
                {
                    HwDeviceExtension->DeskTopInfo.ulHeadDeviceOptions[Head] |= HEAD_DEVICE_OPTION_TV;
                    if (i == 0)
                    {
                        Head0Connected = TRUE;
                    }
                    if (i == 1)
                    {
                        Head1Connected = TRUE;
                    }
                } // if TVConnected == 1
            } // see if TV is connected
    } // for each head.

    if (Head0Connected == TRUE && Head1Connected == TRUE)
    {
        HwDeviceExtension->DeskTopInfo.ulNumberDacsConnected = 2;
    }
    if (Head0Connected == TRUE && Head1Connected == FALSE)
    {
        HwDeviceExtension->DeskTopInfo.ulNumberDacsConnected = 1;
    }
    if (Head0Connected == FALSE && Head1Connected == TRUE)
    {
        HwDeviceExtension->DeskTopInfo.ulNumberDacsConnected = 1;
    }
    if (Head0Connected == FALSE && Head1Connected == FALSE)
    {
        VideoDebugPrint((0,"Oops! Neither Head0 nor Head1 is connected! Assume NumberDacsConnected as 1\n"));
        HwDeviceExtension->DeskTopInfo.ulNumberDacsConnected = 1;
    }

    // Just some safety valves for the worst case scenario where we don't detect any output devices
    if (HwDeviceExtension->DeskTopInfo.ulNumberDacsOnBoard == 1)
    {
        if (HwDeviceExtension->DeskTopInfo.ulHeadDeviceOptions[0] == 0)
        {
            VideoDebugPrint((0,"Oops! Head: 0 has no detected device options. Set it for CRT\n"));
                HwDeviceExtension->DeskTopInfo.ulHeadDeviceOptions[0] = HEAD_DEVICE_OPTION_CRT;
        }
    }
    if (HwDeviceExtension->DeskTopInfo.ulNumberDacsOnBoard == 2)
    {
        if (HwDeviceExtension->DeskTopInfo.ulHeadDeviceOptions[0] == 0 && 
            HwDeviceExtension->DeskTopInfo.ulHeadDeviceOptions[1] == 0)
        {
            VideoDebugPrint((0,"Oops! Head0 and Head1 have no detected device options. Set it for CRT\n"));
                HwDeviceExtension->DeskTopInfo.ulHeadDeviceOptions[0] = HEAD_DEVICE_OPTION_CRT;
                HwDeviceExtension->DeskTopInfo.ulHeadDeviceOptions[1] = HEAD_DEVICE_OPTION_CRT;
        }
    }
   
    return(TRUE);
} // End function bScanHeadDeviceOptions()

#endif ENABLE_HEAD_API


//
// This function should be called once at boot time.
// It initializes the DeskTopInfo structure to safe values.
//
BOOLEAN NVOneTimeInit_TwinView(PHW_DEVICE_EXTENSION HwDeviceExtension)
{
    ULONG i, Count = 0;
    BOOLEAN bRet = TRUE;
    NV_DESKTOP_INFO * pInfo;

    VideoDebugPrint((2, "NVOneTimeInit_TwinView():\n"));

    
    pInfo = (NV_DESKTOP_INFO *) &HwDeviceExtension->DeskTopInfo;
    pInfo->ulDesktopModeIndex = 0;

    pInfo->ulDesktopMode = 0;
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

    VideoDebugPrint((3,"Enter GetHeadPhysicalModeList()\n"));

    lpDispData = &DispData;
    lpModeDesc = &ModeDesc;
    lpModeOut = &ModeOut;
    lpModeNew = &ModeNew;
    

    ulHead = pInput->ulHead;
    ulEnableDDC = pInput->ulEnableDDC;
    
    #ifdef ENABLE_HEAD_API
    ulHeadEDID =  ConvertDeviceMaskToIndex(pInput->ulDeviceMask);
    #else
    ulHeadEDID = ulHead;
    #endif
    

    
    // If the device type is TV, w simply return the virtual mode list with only
    // 640x480 and 8000x600 for all color depths at only 60HZ
    if (pInput->ulDeviceType == MONITOR_TYPE_NTSC || pInput->ulDeviceType == MONITOR_TYPE_PAL)
    {
        DestPtr = pInput->pPhysicalModeList;
        VirtualModeList = pInput->pVirtualModeList;

        NumVirtualModes = pInput->ulNumVirtualModes;
        NumValidModes = NumVirtualModes;
        for (i=0; i < NumVirtualModes; i++)
        {

            if (VirtualModeList->Width > 800 || VirtualModeList->Height > 600 || VirtualModeList->RefreshRate != 60)
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
            VideoDebugPrint((0,"Oops! Invalid Head: 0x%x\n",ulHead));
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
            lpModeDesc->dwDevType = pInput->ulDeviceType;
            lpModeDesc->dwTVFormat = pInput->ulTVType;
            // We want to ignore the EDID since we are using the explicit monitor mode list.
            lpDispData->dwMonitorInfType = UNKNOWN_DEVICE;
            // Call the modeset DLL to see if this mode is valid.
            // We are not interested in the mode timings but just to see if this is a valid mode.
            VideoDebugPrint((3,"Before: lpModeDescr: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
                lpModeDesc->dwXRes, lpModeDesc->dwYRes, lpModeDesc->dwBpp, lpModeDesc->dwRefreshRate, 
                lpModeDesc->dwDevType, lpModeDesc->dwOriginalRefreshRate));

            ulRet = FindModeEntry(lpDispData, &(HwDeviceExtension->EDIDBuffer[ulHeadEDID][0]), 
                                  HwDeviceExtension->EDIDBufferSize[ulHeadEDID], lpModeDesc, lpModeOut, lpModeNew);
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
        if (HwDeviceExtension->EDIDBufferValid[ulHeadEDID])
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
    for (i=0; i < NumVirtualModes; i++)
    {

        // Now validate against the pixel frequency of the head.
        lpModeDesc->dwXRes = VirtualModeList->Width;
        lpModeDesc->dwYRes = VirtualModeList->Height;
        lpModeDesc->dwBpp = VirtualModeList->Depth;
        lpModeDesc->dwRefreshRate = VirtualModeList->RefreshRate;
        lpModeDesc->dwOriginalRefreshRate = VirtualModeList->RefreshRate;
        lpModeDesc->dwDevType = pInput->ulDeviceType;
        lpModeDesc->dwTVFormat = pInput->ulTVType;
        // Call the modeset DLL to see if this mode is valid.
        // We are not interested in the mode timings but just to see if this is a valid mode.
        VideoDebugPrint((3,"Before: lpModeDescr: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
                         lpModeDesc->dwXRes, lpModeDesc->dwYRes, lpModeDesc->dwBpp, lpModeDesc->dwRefreshRate, 
                         lpModeDesc->dwDevType, lpModeDesc->dwOriginalRefreshRate));

        ulRet = FindModeEntry(lpDispData, &(HwDeviceExtension->EDIDBuffer[ulHeadEDID][0]), 
                              HwDeviceExtension->EDIDBufferSize[ulHeadEDID], lpModeDesc, lpModeOut, lpModeNew);
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



//
// This function initializes the mapping layer between Win2K's power states and nVidia's power states
//
VOID InitializeACPIStates(
    PHW_DEVICE_EXTENSION HwDeviceExtension)
{
    BOOL bRet;
    ULONG Caps, ulHead;
    ULONG HighestNVAdapterState, i;

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
        HwDeviceExtension->NVMonitorPowerState[i][NV_VideoPowerHibernate] = NV_POWER_MONITOR_STATE_1;            
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
        // We support the hibernate state only if the "context save" feature is turned on.
        // If this feature is turned on, we assume that the highest numbered state (3 in this case)
        // will support this context save feature
        if (Caps & NV_POWER_ADAPTER_FEATURE_CTXSAVED)
        {
            HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerHibernate] = NV_POWER_ADAPTER_STATE_3;            
        }
        else
        {
            HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerHibernate] = 0; // not supported
        }
        return;
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
        // We support the hibernate state only if the "context save" feature is turned on.
        // If this feature is turned on, we assume that the highest numbered state (3 in this case)
        // will support this context save feature
        if (Caps & NV_POWER_ADAPTER_FEATURE_CTXSAVED)
        {
            HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerHibernate] = NV_POWER_ADAPTER_STATE_3;            
        }
        else
        {
            HwDeviceExtension->NVAdapterPowerState[NV_VideoPowerHibernate] = 0; // not supported
        }
        return;
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
            VideoDebugPrint((0,"Oops! ConvertDeviceMaskToIndex(): Invalid ulDeviceMask: 0x%x\n",ulDeviceMask));
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

