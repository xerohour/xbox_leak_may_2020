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

#include "rm.h"
#include "nvcm.h"
#include "modeext.h"

#if (_WIN32_WINNT >= 0x500)
ULONG
NVGetChildDescriptor(
    PVOID HwDeviceExtension,
    PVIDEO_CHILD_ENUM_INFO ChildEnumInfo,
    PVIDEO_CHILD_TYPE pChildType,
    PVOID pvChildDescriptor,
    PULONG pHwId,
    PULONG pUnused
    );

#endif  // #if (_WIN32_WINNT >= 0x500)

#if defined(ALLOC_PRAGMA)
#if (_WIN32_WINNT >= 0x500)
#pragma alloc_text(PAGE, NVGetChildDescriptor)
#endif  // #if (_WIN32_WINNT >= 0x500)
#endif  // #if defined(ALLOC_PRAGMA)

ULONG ConvertDeviceMaskToIndex(
    ULONG ulDeviceMask);

BOOLEAN
GetDdcInformation(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    ULONG  ulDisplayIndex,
    PUCHAR QueryBuffer,
    ULONG  BufferSize);

#ifdef NVPE
VP_STATUS NVRegistryCallbackPnpId(PVOID HwDeviceExtension, PVOID Context, PWSTR ValueName, PVOID ValueData, ULONG ValueLength);
#endif

extern VP_STATUS
NVRegistryCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    );
extern VP_STATUS NVGetRegistryTwinviewInfo (
    PHW_DEVICE_EXTENSION hwDeviceExtension,
    NVTWINVIEWDATA      *pTwinviewInfo
    );
extern VP_STATUS NVSetRegistryTwinviewInfo (
    PHW_DEVICE_EXTENSION hwDeviceExtension,
    NVTWINVIEWDATA      *pTwinviewInfo
    );


BOOLEAN NVInitialize_DacsAndEDID(PHW_DEVICE_EXTENSION HwDeviceExtension, BOOLEAN bBootTime);
ULONG MaskToDeviceType(ULONG ulDeviceMask);
ULONG MaskToFindModeEntryDeviceType(ULONG ulDeviceMask);
VOID  ChoosePrimaryDevice(PHW_DEVICE_EXTENSION HwDeviceExtension, NVTWINVIEWDATA *pData);
BOOLEAN  bQueryinSpanningMode(PHW_DEVICE_EXTENSION hwDeviceExtension);
BOOLEAN  bAllowHotKeyInSpanningMode(PHW_DEVICE_EXTENSION hwDeviceExtension);

BOOLEAN  bQueryInFSDOSorVGAMode(PHW_DEVICE_EXTENSION hwDeviceExtension);
BOOLEAN  bQuerySpanDualDevicesSame(PHW_DEVICE_EXTENSION hwDeviceExtension, ULONG ulDeviceMask0, ULONG ulDeviceMask1);
BOOLEAN ACPIValidateChildStateShouldFail(PHW_DEVICE_EXTENSION hwDeviceExtension);

extern VOID EnterFPCode(VOID *pSave, VOID *pMutex);
extern VOID ExitFPCode(VOID *pSave, VOID *pMutex);
extern int FindModeEntry (LPDISPDATA lpDispData,
    LPCHAR      lpEdidBuffer,
    ULONG       dwEdidSize,
    LPMODEINFO  lpModeDesc,
    LPMODEOUT   lpModeOut,
    LPMODEINFO  lpModeNew);
extern VOID HandleDualviewHeads(PHW_DEVICE_EXTENSION HwDeviceExtension, BOOLEAN bEnableDualView);

//**************************************************************************
// NT 5.0 specific entry points.
//**************************************************************************

#if (_WIN32_WINNT >= 0x0500)
//
// Returns the corresponding nVidia device mask given a ACPI hardware ID.
// If the ACPIHwId is an invalid device, returns 0.
//
ULONG ACPIHwIdToMask( ULONG ACPIHwId)
{
    switch (ACPIHwId)
    {
    // Refer to Table A-2 of Appenidix A of the document
    // "ACPI extensions for Display Adapters" for a list 
    // of the "commonly used device IDs"
    case QUERY_ACPI_CRT_ID:
        // This is the Monitor. Our device mask is 0x1 for the CRT.
        return (0x1);
    case QUERY_ACPI_DFP_ID:
        // This is the DFP. Our device mask is 0x10000 for the DFP.
        return(0x10000);
    case QUERY_ACPI_TV_ID:
        // This is the TV. Our device mask is 0x100 for the TV.
        return(0x100);
    case 0:
        // Other device. What do we do here?
        VideoDebugPrint((0,"Oops! ChildEnumInfo->ACPIHwId is 0. Not sure what we do here!\n"));
        return(0);
    default:
        VideoDebugPrint((1,"OOps! Error: Unknown ACPIHwID: 0x%x\n", ACPIHwId));
        return(0);
    }
}

//
// Returns TRUE if we are in spanning mode. else returns FALSE
//
BOOLEAN  bQueryInSpanningMode(PHW_DEVICE_EXTENSION hwDeviceExtension)
{
    NV_DESKTOP_INFO * pInfo;
    pInfo = &hwDeviceExtension->DeskTopInfo;

    //
    // Detect if we are in spanning mode.
    //
    if (MULTIMON_MODE(pInfo->ulDesktopWidth, pInfo->ulDesktopHeight))
    {
        return(TRUE);
    }

    //
    // Handle the special resolution of 6x4, 8x6 and 10x7 which are present in spanning modes also.
    //
    if (pInfo->ulTwinView_State == NVTWINVIEW_STATE_SPAN)
    {
        return(TRUE);

    }

    //
    // We are ins tandard or clone mode.
    //
    return(FALSE);

}

//
// Returns TRUE if we are in FSDOS or VGA mode. else returns FALSE
//
BOOLEAN  bQueryInFSDOSorVGAMode(PHW_DEVICE_EXTENSION hwDeviceExtension)
{
    NV_DESKTOP_INFO * pInfo;
    pInfo = &hwDeviceExtension->DeskTopInfo;

    //
    // Detect if we are in full screen mode.
    // Or if we are in VGA mode (such as via the F8 option of "VGA mode" at bootup time.
    // Here we want to pass VALIDATE_CHILD_DEVICE but fail SET_CHILD_DEVICE so that the OS will
    // call the VBIOS to do the switching. Note that the GET_CHILD_STATE has to be consistent with the
    // devices specified in VALIDATE_CGILD_DEVICE and SET_CHILD_DEVICE.
    //
    if (hwDeviceExtension->SystemModeSetHasOccurred && pInfo->ulDesktopMode == 0)
    {
        return(TRUE);
    }
    if (hwDeviceExtension->SystemModeSetHasOccurred == 0)
    {
        return(TRUE);
    }
    //
    // We are in high res mode.
    //
    return(FALSE);
}

//
// Returns TRUE if
// - system is currently in spanning mode. AND
// - same devices being used
//
BOOLEAN  bQuerySpanDualDevicesSame(PHW_DEVICE_EXTENSION hwDeviceExtension, ULONG ulDeviceMask0, ULONG ulDeviceMask1)
{
    ULONG ulCombinedMask_Current;
    ULONG ulCombinedMask_Requested;
    NV_DESKTOP_INFO * pInfo;
    pInfo = &hwDeviceExtension->DeskTopInfo;
    
    if (bQueryInSpanningMode(hwDeviceExtension))
    {
        ulCombinedMask_Current = pInfo->ulDeviceMask[0] | pInfo->ulDeviceMask[1];
        ulCombinedMask_Requested = ulDeviceMask0 | ulDeviceMask1;
        if (ulCombinedMask_Current == ulCombinedMask_Requested)
        {
            return(TRUE);
        }
    }
    return(FALSE);
}

//
// Returns TRUE we should do HK switches in spanning mode.
// By default, we disallow HK switches in spanning mode except when one or both of the external devices
// (CRT or TV) has been disconnected and the NVSVC has been started.
//
BOOLEAN  bAllowHotKeyInSpanningMode(PHW_DEVICE_EXTENSION hwDeviceExtension)
{
    NV_DESKTOP_INFO * pInfo;
    ULONG ulExternalDeviceMask;
    ULONG ulDeviceMask, ulHead, i, ulTmp;
    pInfo = &hwDeviceExtension->DeskTopInfo;

    //
    // sanity check. Make sure we are called for spanning mode.
    //
    if (pInfo->ulTwinView_State != NVTWINVIEW_STATE_SPAN)
    {
        return(TRUE);
    }
    //
    // Construct device masks of the external devices (TV or CRT) being used in the spanning mode.
    //
    ulExternalDeviceMask = 0;
    for (i=0; i < pInfo->ulNumberDacsActive; i++)
    {
        ulHead = pInfo->ulDeviceDisplay[i];
        ulDeviceMask = pInfo->ulDeviceMask[ulHead];
        if (ulDeviceMask & (BITMASK_ALL_CRT | BITMASK_ALL_TV))
        {
            ulExternalDeviceMask |= ulDeviceMask;
        }
    }
    //
    // Now check to see if all of these are still connected
    //
    ulTmp = ulExternalDeviceMask;
    if (!RmConfigGetExKernel(hwDeviceExtension->DeviceReference,
                        NV_CFGEX_CHECK_CONNECTED_DEVICES, &ulTmp, 
                        sizeof(ULONG)))
    {
        VideoDebugPrint((1, "Something wrong. RM failed for CHECK_CONNECTED_DEVICES\n"));
        return(FALSE);
    }
    if (ulTmp != ulExternalDeviceMask)
    {
        //
        // We need to allow the HK switch since one or both of the external devices are missing.
        // We can do the switch only if the NVSVC has started.
        //
        if (hwDeviceExtension->ulNVSVCStatus == NVSVC_STATUS_STARTED)
        {
            return(TRUE);
        }
    }
    return(FALSE);
}

//
// Returns TRUE if we should not do hot key swtiches.
// Returns FALSE if we should do hot key switches.
// The current set of conditions for which we do not do hotkeying are
// 1. we are in full screen DOS mode. Let BIOS handle it.
// 2. we are in spanning mode.
// 3. Video app is active: BUGBUG: Not implemented yet.
//
BOOLEAN ACPIValidateChildStateShouldFail(PHW_DEVICE_EXTENSION hwDeviceExtension)
{
    NV_DESKTOP_INFO * pInfo;

    pInfo = &hwDeviceExtension->DeskTopInfo;
    

    // Note: The FSDOS or VGA mode boot case are now handled in nvSetChildState() call.

    // Detect if we are in spanning mode.
    if (bQueryInSpanningMode(hwDeviceExtension))
    {
        //
        // We ignore hotkeys in spanning mode if the external device (either CRT or TV) is absent.
        //
        if (!bAllowHotKeyInSpanningMode(hwDeviceExtension))
        {
            return(TRUE);
        }
    }

    return(FALSE);
}

//
// Writes the current mode info to the registry so that the DrvEnablePdev() in the display driver can
// force GDI to use the same mode across hotkey switches. By default, GDI remembers the mode
// on each devices and tries to use that remembered mode.
//
VOID ACPIToshibaModeToRegistry(PHW_DEVICE_EXTENSION hwDeviceExtension)
{
    TOSHIBA_HOTKEY_MODE sMode;
    NV_DESKTOP_INFO *pInfo;
    ULONG ulPrimaryHead;

    pInfo = &hwDeviceExtension->DeskTopInfo;
    ulPrimaryHead = pInfo->ulDeviceDisplay[0];

    // The display driver should invalidate this registry entry by writing the ValidMode field to zero
    // after it uses this info in either DrvEnablePdev() or DrvAssertMode(). GDI calls DrvAssertMode() if 
    // the same desktop is maintained. GDI calls DrvEnablePdev() if the remembered mode is different than the
    // current desktop for this device.
    // Also, the FindAdapter() boottime routine should set this field
    // to zero if this registry entry exists. This is to handle the pathologicl case of when the user has hit the 
    // hotkey function key but the system is powered off before the corresponding DrvEnablePdev() or DrvAssertMode()
    // is received.
    sMode.ValidMode = 1;
    sMode.Width = (USHORT)pInfo->ulDesktopWidth;
    sMode.Height = (USHORT)pInfo->ulDesktopHeight;
    sMode.Depth = (USHORT)pInfo->ulDisplayPixelDepth[ulPrimaryHead];
    sMode.RefreshRate = (USHORT)pInfo->ulDisplayRefresh[ulPrimaryHead];

    // Write the info to registry.
    VideoPortSetRegistryParameters(hwDeviceExtension,
                                       L"ForceGDIMode",
                                       &sMode,
                                       sizeof(TOSHIBA_HOTKEY_MODE));

}

//
// Handler for hot key switching for switching into single device configuration.
// Blows away any info existing in the TwinViewInfo registry structure (if it exists).
// Writes the full TwinViewInfo structure appropriately.
//
VOID ACPISingleDeviceSwitch(PHW_DEVICE_EXTENSION HwDeviceExtension, ULONG ulDeviceMask)
{
    NVTWINVIEWDATA Data;
    NVTWINVIEWDATA * pData;
    NVTWINVIEW_DEVICE_TYPE_DATA * pDevData0;
    NVTWINVIEW_DEVICE_TYPE_DATA * pDevData1;
    ULONG ulPrimaryHead;
    NV_DESKTOP_INFO *pInfo;
    ULONG GDIWidth, GDIHeight, GDIRefreshRate, GDIDepth, GDIPrimaryHead;
    ULONG i;
    NV_CFGEX_GET_DEVICES_CONFIGURATION_PARAMS nvConfig;
    ULONG ulRet;

    // FindModeEntry() related fields.
    LPDISPDATA  lpDispData;
    LPMODEINFO  lpModeDesc;
    LPMODEOUT   lpModeOut;
    LPMODEINFO  lpModeNew;
    ULONG       ulFrequencyHead;
    DISPDATA    DispData;
    MODEINFO    ModeDesc;
    MODEOUT     ModeOut;
    MODEINFO    ModeNew;
    ULONG       ulDeviceType, ulTVFormat, ulIndex;
    LPEDID_UNIT pEdidUnit;


    ulDeviceType = MaskToDeviceType(ulDeviceMask);
    ulIndex = ConvertDeviceMaskToIndex(ulDeviceMask);
    pEdidUnit = HwDeviceExtension->EdidCollection->HeadMaps[ulIndex];
    ulTVFormat = NTSC_M; // Hardcoded. Does not matter. Display driver InitMultiMon() gets the correct TVFormat.
    pInfo = &HwDeviceExtension->DeskTopInfo;

    pData = &Data;
    pData->dwState = NVTWINVIEW_STATE_NORMAL;
    pData->dwOrientation = 0;
    pData->dwAllDeviceMask = pInfo->ulAllDeviceMask;
    pData->dwConnectedDeviceMask = pInfo->ulConnectedDeviceMask;
    pDevData0 = &pData->NVTWINVIEWUNION.nvtwdevdata[0];
    pDevData1 = &pData->NVTWINVIEWUNION.nvtwdevdata[1];

    // Initialize the rectangles to zeros.
    pData->DeskTopRectl.left = 0;
    pData->DeskTopRectl.top = 0;
    pData->DeskTopRectl.right = 0;
    pData->DeskTopRectl.bottom = 0;

    for (i=0; i < NV_NO_DACS; i++)
    {
        NVTWINVIEW_DEVICE_TYPE_DATA * pDevData;

        pDevData = &pData->NVTWINVIEWUNION.nvtwdevdata[i];
        pDevData->VirtualRectl.left = 0;
        pDevData->VirtualRectl.top = 0;
        pDevData->VirtualRectl.right = 0;
        pDevData->VirtualRectl.bottom = 0;
        pDevData->PhysicalRectl.left = 0;
        pDevData->PhysicalRectl.top = 0;
        pDevData->PhysicalRectl.right = 0;
        pDevData->PhysicalRectl.bottom = 0;
        pDevData->dwBpp = 0;
        pDevData->dwRefresh = 0;
        pDevData->dwDeviceMask = BITMASK_INVALID_DEVICE;
        pDevData->dwDeviceType = INVALID_DEVICE_TYPE;
        pDevData->dwTVFormat = 0;
        pDevData->dwNumPhysModes = 0;
        pDevData->dwNumVirtualModes = 0;
    }


    // Get our current desktop info
    GDIWidth = pInfo->ulDesktopWidth;
    GDIHeight = pInfo->ulDesktopHeight;

    // If currently we are in spanning mode, we need to half the resolution. Careful of the special
    // 6x4, 8x6 and 10x7 modes in spanning mode.
    if (bQueryInSpanningMode(HwDeviceExtension))
    {
        if HORIZONTAL_MODE(GDIWidth, GDIHeight)
        {
            GDIWidth /= 2;
        }
        else
        if VERTICAL_MODE(GDIWidth, GDIHeight)
        {
            GDIHeight /= 2;
        }
    }

    
    GDIPrimaryHead = pInfo->ulDeviceDisplay[0];
    GDIRefreshRate = pInfo->ulDisplayRefresh[GDIPrimaryHead];
    GDIDepth = pInfo->ulDisplayPixelDepth[GDIPrimaryHead];

    // Set the desktopRectl with this info.
    // Note that we _have_ to maintain the same width, height and depth. We could get away with
    // a different refresh rate.
    pData->DeskTopRectl.right = GDIWidth;
    pData->DeskTopRectl.bottom = GDIHeight;


    // Ask the resman what head to use for this device.
    // Ask ResMan what head to use.
    nvConfig.DevicesConfig = ulDeviceMask;
    nvConfig.OldDevicesConfig = 0;
    nvConfig.OldDevicesAllocationMap = 0;
    nvConfig.DevicesAllocationMap = 0;


    if (!RmConfigGetExKernel (HwDeviceExtension->DeviceReference, 
                              NV_CFGEX_GET_DEVICES_CONFIGURATION, &nvConfig, sizeof(NV_CFGEX_GET_DEVICES_CONFIGURATION_PARAMS)))
    {

        VideoDebugPrint((0, "ACPISingleDeviceSwitch - Cannot get DEVICES_CONFIG value from RM\n"));
    }
    else
    {
        if (nvConfig.DevicesAllocationMap > 1)
        {
            VideoDebugPrint((0,"Oops! Invalid DevAllocationMap from RM: 0x%x\n",nvConfig.DevicesAllocationMap));
        }


        if (nvConfig.DevicesAllocationMap == 0)
        {
            pDevData0->dwDeviceMask = ulDeviceMask;
            pDevData0->dwDeviceType = ulDeviceType;
            pDevData0->dwTVFormat = ulTVFormat;
            pDevData0->VirtualRectl.right = GDIWidth;
            pDevData0->VirtualRectl.bottom = GDIHeight;
            pDevData0->PhysicalRectl.right = GDIWidth;
            pDevData0->PhysicalRectl.bottom = GDIHeight;
            pDevData0->dwBpp = GDIDepth;
            pDevData0->dwRefresh = GDIRefreshRate;

            pDevData1->dwDeviceMask = BITMASK_INVALID_DEVICE;
            pData->dwDeviceDisplay[0] = 0;
            pData->dwDeviceDisplay[1] = 1;
        }
        if (nvConfig.DevicesAllocationMap == 1)
        {
            pDevData1->dwDeviceMask = ulDeviceMask;
            pDevData1->dwDeviceType = ulDeviceType;
            pDevData1->VirtualRectl.right = GDIWidth;
            pDevData1->VirtualRectl.bottom = GDIHeight;
            pDevData1->PhysicalRectl.right = GDIWidth;
            pDevData1->PhysicalRectl.bottom = GDIHeight;
            pDevData1->dwBpp = GDIDepth;
            pDevData1->dwRefresh = GDIRefreshRate;

            pDevData0->dwDeviceMask = BITMASK_INVALID_DEVICE;
            pData->dwDeviceDisplay[0] = 1;
            pData->dwDeviceDisplay[1] = 0;
        }
    }

    ulPrimaryHead = pData->dwDeviceDisplay[0];


    // Now see if the current desktop can be supported natively on this device.
    // If so, we will use the same dimensions for the physical mode also.
    // Otherwise, we will have to use pan-scan.
    lpDispData = &DispData;
    lpModeDesc = &ModeDesc;
    lpModeOut = &ModeOut;
    lpModeNew = &ModeNew;

    ulFrequencyHead = ulPrimaryHead;
    if (ulPrimaryHead == 1 && ulDeviceType != MONITOR_TYPE_VGA)
    {
        ulFrequencyHead = 0;
    }
    lpDispData->lpBoardRegistryData = NULL;
    lpDispData->lpMonitorRegistryData = NULL;
    if (pEdidUnit->Valid)
    {
        lpDispData->dwMonitorInfType = PLUG_AND_PLAY_DEVICE;
    }
    else
    {
        lpDispData->dwMonitorInfType = UNKNOWN_DEVICE;
    }
    lpDispData->dwDeviceID = HwDeviceExtension->ulChipID;
    lpDispData->dwCRTCIndex = ulPrimaryHead;
    lpDispData->dwVideoMemoryInBytes = HwDeviceExtension->AdapterMemorySize;
    lpDispData->dwMaxDacSpeedInHertz8bpp= HwDeviceExtension->maxPixelClockFrequency_8bpp[ulFrequencyHead];
    lpDispData->dwMaxDacSpeedInHertz16bpp = HwDeviceExtension->maxPixelClockFrequency_16bpp[ulFrequencyHead];
    lpDispData->dwMaxDacSpeedInHertz32bpp = HwDeviceExtension->maxPixelClockFrequency_32bpp[ulFrequencyHead];
    lpDispData->dwMaxDfpScaledXResInPixels = HwDeviceExtension->MaxFlatPanelWidth;
    lpDispData->lpfnGetModeSize = 0;  // GK: WHat is this?
    lpDispData->dwContext1 = (PVOID)HwDeviceExtension;
    lpDispData->dwContext2 = (PVOID)0;
    lpDispData->dwMaxTVOutResolution = HwDeviceExtension->ulMaxTVOutResolution;

    // Now validate against the pixel frequency of the head.
    lpModeDesc->dwXRes = GDIWidth;
    lpModeDesc->dwYRes = GDIHeight;
    lpModeDesc->dwBpp  = GDIDepth;
    lpModeDesc->dwRefreshRate   = GDIRefreshRate;
    lpModeDesc->dwOriginalRefreshRate = GDIRefreshRate;
    lpModeDesc->dwDevType  = MaskToFindModeEntryDeviceType(ulDeviceMask);
    lpModeDesc->dwTVFormat = ulTVFormat;

    lpModeDesc->MIDevData.cType     = (CHAR)MaskToFindModeEntryDeviceType(ulDeviceMask);
    lpModeDesc->MIDevData.cNumber   = (CHAR)MaskToDeviceType(ulDeviceMask);
    lpModeDesc->MIDevData.cFormat   = (CHAR)ulTVFormat;
    lpModeDesc->MIDevData.cReserved = 0x0;

    // Call the modeset DLL to see if this mode is valid.
    // We are not interested in the mode timings but just to see if this is a valid mode.
    VideoDebugPrint((1,"Before: lpModeDescr: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
                     lpModeDesc->dwXRes, lpModeDesc->dwYRes, lpModeDesc->dwBpp, lpModeDesc->dwRefreshRate, 
                     lpModeDesc->dwDevType, lpModeDesc->dwOriginalRefreshRate));
    VideoDebugPrint((1,"ulDeviceMask: 0x%x, ulIndex: %d, MonitorInfType: 0x%x, EDIDBufferValid: 0x%x\n",
                     ulDeviceMask, ulIndex, lpDispData->dwMonitorInfType, pEdidUnit->Valid)); 


    if (ulDeviceType == MONITOR_TYPE_NTSC || ulDeviceType == MONITOR_TYPE_PAL)
    {
        ulRet = 0;

        if (HwDeviceExtension->ulTVEncoderType == NV_ENCODER_BROOKTREE_871)
        {
            if (lpModeDesc->dwXRes > 1024 || lpModeDesc->dwYRes > 768 || lpModeDesc->dwRefreshRate != 60)
            { 
                // Not a valid mode. Limit it to 1024x768x60HZ.
                ulRet = 1; // Any non-zero value will do.
                lpModeNew->dwXRes = 1024;
                lpModeNew->dwYRes = 768;
                lpModeNew->dwBpp = lpModeDesc->dwBpp;
                lpModeNew->dwRefreshRate = 60; 
                lpModeNew->dwDevType = lpModeDesc->dwDevType;
                lpModeNew->dwOriginalRefreshRate = lpModeDesc->dwRefreshRate;
            }
        }
        else if (lpModeDesc->dwXRes > 800 || lpModeDesc->dwYRes > 600 || lpModeDesc->dwRefreshRate != 60)
        {
            // Not a valid mode. Limit to 800x600x60
                ulRet = 1; // Any non-zero value will do.
                lpModeNew->dwXRes = 800;
                lpModeNew->dwYRes = 600;
                lpModeNew->dwBpp = lpModeDesc->dwBpp;
                lpModeNew->dwRefreshRate = 60; 
                lpModeNew->dwDevType = lpModeDesc->dwDevType;
                lpModeNew->dwOriginalRefreshRate = lpModeDesc->dwRefreshRate;
        }
    }
    else
    {
        // Not TV. Ask the common modeset code.
        EnterFPCode(HwDeviceExtension->pFPStateSave, HwDeviceExtension->pFPMutex);
        ulRet = FindModeEntry(lpDispData, pEdidUnit->EDIDBuffer, pEdidUnit->Size,
                              lpModeDesc, lpModeOut, lpModeNew);
        ExitFPCode(HwDeviceExtension->pFPStateSave, HwDeviceExtension->pFPMutex);
    }

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
        // Use the recommended physically supported mode info - width, height and refresh.
        // We can not change the depth.
        if (ulPrimaryHead == 0)
        {
            pDevData0->PhysicalRectl.right = lpModeNew->dwXRes;
            pDevData0->PhysicalRectl.bottom = lpModeNew->dwYRes;
            pDevData0->dwRefresh = lpModeNew->dwRefreshRate;
        }
        if (ulPrimaryHead == 1)
        {
            pDevData1->PhysicalRectl.right = lpModeNew->dwXRes;
            pDevData1->PhysicalRectl.bottom = lpModeNew->dwYRes;
            pDevData1->dwRefresh = lpModeNew->dwRefreshRate;
        }
    }

    NVSetRegistryTwinviewInfo(HwDeviceExtension, pData);

    // For Toshiba, update the desired desktop mode info.
    if (HwDeviceExtension->UseHotKeySameMode)
    {
        ACPIToshibaModeToRegistry(HwDeviceExtension);
    }

    // Update the DesktopInfo structure of hwDevExtension since DrvGetModes() will get called before
    // DrvASsertMode().
    pInfo->ulNumberDacsActive = 1;
    for (i = 0; i < NV_NO_DACS; i++)
    {
        NVTWINVIEW_DEVICE_TYPE_DATA * pDevData;
        pDevData = &pData->NVTWINVIEWUNION.nvtwdevdata[i];

        pInfo->ulDeviceDisplay[i] = pData->dwDeviceDisplay[i];
        pInfo->ulDeviceMask[i] = pDevData->dwDeviceMask;
        pInfo->ulDeviceType[i] = pDevData->dwDeviceType;
        pInfo->ulTVFormat[i] = pDevData->dwTVFormat;
    }
    return;
}


//
// Handler for hot key switching for switching into dual device configuration.
// Blows away any info existing in the TwinViewInfo registry structure (if it exists).
// Writes the full TwinViewInfo structure appropriately.
//
VOID ACPIDualDeviceSwitch(PHW_DEVICE_EXTENSION hwDeviceExtension, ULONG ulDeviceMask0, ULONG ulDeviceMask1)
{
    NVTWINVIEWDATA Data;
    NVTWINVIEWDATA * pData;
    NVTWINVIEW_DEVICE_TYPE_DATA * pDevData0;
    NVTWINVIEW_DEVICE_TYPE_DATA * pDevData1;
    NVTWINVIEW_DEVICE_TYPE_DATA * pDevData;
    ULONG ulHead;
    NV_DESKTOP_INFO *pInfo;
    ULONG GDIWidth, GDIHeight, GDIRefreshRate, GDIDepth, GDIPrimaryHead;
    ULONG i;
    NV_CFGEX_GET_DEVICES_CONFIGURATION_PARAMS nvConfig;
    ULONG ulRet;
    ULONG ulIndex;
    PHW_DEVICE_EXTENSION HwDeviceExtension;
    ULONG ulTmp;

    #define CRT_MASK 0x1
    #define DFP_MASK 0x10000
    #define TV_MASK 0x100

    // FindModeEntry() related fields.
    LPDISPDATA lpDispData;
    LPMODEINFO lpModeDesc;
    LPMODEOUT  lpModeOut;
    LPMODEINFO lpModeNew;
    ULONG ulFrequencyHead;
    DISPDATA DispData;
    MODEINFO ModeDesc;
    MODEOUT  ModeOut;
    MODEINFO ModeNew;
    ULONG ulDeviceType0, ulTVFormat0, ulIndex0;
    ULONG ulDeviceType1, ulTVFormat1, ulIndex1;

    HwDeviceExtension = hwDeviceExtension;


    if (ulDeviceMask0 > ulDeviceMask1)
    {
        // For ease of interacting with the resman to get the devices_configuration, we want to keep
        // deviceMask0 to be the lesser of the mask
        ulTmp = ulDeviceMask0;
        ulDeviceMask0 = ulDeviceMask1;
        ulDeviceMask1 = ulTmp;
    }

    ulDeviceType0 = MaskToDeviceType(ulDeviceMask0);
    ulIndex0 = ConvertDeviceMaskToIndex(ulDeviceMask0);
    ulTVFormat0 = NTSC_M; // Hardcoded. Does not matter. Display driver InitMultiMon() gets the correct TVFormat.
    ulDeviceType1 = MaskToDeviceType(ulDeviceMask1);
    ulIndex1 = ConvertDeviceMaskToIndex(ulDeviceMask1);
    ulTVFormat1 = NTSC_M; // Hardcoded. Does not matter. Display driver InitMultiMon() gets the correct TVFormat.

    pInfo = &HwDeviceExtension->DeskTopInfo;

    pData = &Data;
    pData->dwState = NVTWINVIEW_STATE_CLONE;
    pData->dwOrientation = 0;
    pData->dwAllDeviceMask = pInfo->ulAllDeviceMask;
    pData->dwConnectedDeviceMask = pInfo->ulConnectedDeviceMask;
    pData->dwDeviceDisplay[0] = 0;
    pData->dwDeviceDisplay[1] = 1;
    pDevData0 = &pData->NVTWINVIEWUNION.nvtwdevdata[0];
    pDevData1 = &pData->NVTWINVIEWUNION.nvtwdevdata[1];

    // Initialize the rectangles to zeros.
    pData->DeskTopRectl.left = 0;
    pData->DeskTopRectl.top = 0;
    pData->DeskTopRectl.right = 0;
    pData->DeskTopRectl.bottom = 0;

    for (i=0; i < NV_NO_DACS; i++)
    {
        NVTWINVIEW_DEVICE_TYPE_DATA * pDevData;

        pDevData = &pData->NVTWINVIEWUNION.nvtwdevdata[i];
        pDevData->VirtualRectl.left = 0;
        pDevData->VirtualRectl.top = 0;
        pDevData->VirtualRectl.right = 0;
        pDevData->VirtualRectl.bottom = 0;
        pDevData->PhysicalRectl.left = 0;
        pDevData->PhysicalRectl.top = 0;
        pDevData->PhysicalRectl.right = 0;
        pDevData->PhysicalRectl.bottom = 0;
        pDevData->dwBpp = 0;
        pDevData->dwRefresh = 0;
        pDevData->dwDeviceMask = BITMASK_INVALID_DEVICE;
        pDevData->dwDeviceType = INVALID_DEVICE_TYPE;
        pDevData->dwTVFormat = 0;
        pDevData->dwNumPhysModes = 0;
        pDevData->dwNumVirtualModes = 0;
    }


    // Get our current desktop info
    
    GDIWidth = pInfo->ulDesktopWidth;
    GDIHeight = pInfo->ulDesktopHeight;
    GDIPrimaryHead = pInfo->ulDeviceDisplay[0];
    GDIRefreshRate = pInfo->ulDisplayRefresh[GDIPrimaryHead];
    GDIDepth = pInfo->ulDisplayPixelDepth[GDIPrimaryHead];

    // Set the desktopRectl with this info.
    // Note that we _have_ to maintain the same width, height and depth. We could get away with
    // a different refresh rate.
    pData->DeskTopRectl.right = GDIWidth;
    pData->DeskTopRectl.bottom = GDIHeight;

    for (i=0; i < NV_NO_DACS; i++)
    {
        NVTWINVIEW_DEVICE_TYPE_DATA * pDevData;

        pDevData = &pData->NVTWINVIEWUNION.nvtwdevdata[i];
        pDevData->VirtualRectl.right = GDIWidth;
        pDevData->VirtualRectl.bottom = GDIHeight;
        pDevData->PhysicalRectl.right = GDIWidth;
        pDevData->PhysicalRectl.bottom = GDIHeight;
        pDevData->dwBpp = GDIDepth;
        pDevData->dwRefresh = GDIRefreshRate;
    }


    // Ask the resman what head to use for these devices.
    // Ask ResMan what head to use.
    nvConfig.DevicesConfig = ulDeviceMask0 | ulDeviceMask1;
    nvConfig.OldDevicesConfig = 0;
    nvConfig.OldDevicesAllocationMap = 0;
    nvConfig.DevicesAllocationMap = 0;


    if (!RmConfigGetExKernel (HwDeviceExtension->DeviceReference, 
                              NV_CFGEX_GET_DEVICES_CONFIGURATION, &nvConfig, sizeof(NV_CFGEX_GET_DEVICES_CONFIGURATION_PARAMS)))
    {

        VideoDebugPrint((0, "ACPIDualDeviceSwitch - Cannot get DEVICES_CONFIG value from RM\n"));
    }
    else
    {
        // We got a successful head assignment for both the devices.
        switch (nvConfig.DevicesAllocationMap)
        {
        case 0x10:
            // Head assignment 1, 0
            pDevData0->dwDeviceMask = ulDeviceMask0;
            pDevData0->dwDeviceType = ulDeviceType0;
            pDevData0->dwTVFormat = ulTVFormat0;
            pDevData1->dwDeviceMask = ulDeviceMask1;
            pDevData1->dwDeviceType = ulDeviceType1;
            pDevData1->dwTVFormat = ulTVFormat1;
            break;
        case 0x01:
            // Head assignment 0, 1
            pDevData0->dwDeviceMask = ulDeviceMask1;
            pDevData0->dwDeviceType = ulDeviceType1;
            pDevData0->dwTVFormat = ulTVFormat1;
            pDevData1->dwDeviceMask = ulDeviceMask0;
            pDevData1->dwDeviceType = ulDeviceType0;
            pDevData1->dwTVFormat = ulTVFormat0;
            break;
        default:
            VideoDebugPrint((0,"Oops! Unexpected head allocationMap: 0x%x\n",  nvConfig.DevicesAllocationMap));
            break;
        }

        // The combinations we have are: CRT+DFP, DFP+TV, CRT+TV.
        // Make sure the primary head is preferably CRT, DFP, TV in that order because they typically support
        // more number of modes in that order. One of clone mode's restriction is that all modes must
        // be natively supported by the primary device per Peter Daifuku's specs.
        if ((pDevData0->dwDeviceMask == DFP_MASK && pDevData1->dwDeviceMask == CRT_MASK) ||
            (pDevData0->dwDeviceMask == TV_MASK && pDevData1->dwDeviceMask == DFP_MASK) ||
            (pDevData0->dwDeviceMask == TV_MASK && pDevData1->dwDeviceMask == CRT_MASK))
        {
            // Swap the heads.
            pData->dwDeviceDisplay[0] = 1;
            pData->dwDeviceDisplay[1] = 0;
        }
        else
        {
            // Keep the same order
            pData->dwDeviceDisplay[0] = 0;
            pData->dwDeviceDisplay[1] = 1;
        }
    }

    


    // Now see if the current desktop can be supported natively on this device.
    // If so, we will use the same dimensions for the physical mode also.
    // Otherwise, we will have to use pan-scan.
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

    lpModeDesc->dwXRes = GDIWidth;
    lpModeDesc->dwYRes = GDIHeight;
    lpModeDesc->dwBpp = GDIDepth;
    lpModeDesc->dwRefreshRate = GDIRefreshRate;
    lpModeDesc->dwOriginalRefreshRate = GDIRefreshRate;

    for (i = 0; i < NV_NO_DACS; i++)
    {
        LPEDID_UNIT pEdidUnit;
        ulHead = pData->dwDeviceDisplay[i];
        pDevData = &pData->NVTWINVIEWUNION.nvtwdevdata[ulHead];
        ulIndex = ConvertDeviceMaskToIndex(pDevData->dwDeviceMask);
        pEdidUnit = HwDeviceExtension->EdidCollection->HeadMaps[ulIndex];
        ulFrequencyHead = ulHead;
        if (ulHead == 1 && pDevData->dwDeviceType != MONITOR_TYPE_VGA)
        {
            ulFrequencyHead = 0;
        }
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

        lpModeDesc->dwDevType  = MaskToFindModeEntryDeviceType(pDevData->dwDeviceMask);
        lpModeDesc->dwTVFormat = pDevData->dwTVFormat;

        lpModeDesc->MIDevData.cType     = (CHAR)MaskToFindModeEntryDeviceType(pDevData->dwDeviceMask);
        lpModeDesc->MIDevData.cNumber   = (CHAR)MaskToDeviceType(pDevData->dwDeviceMask);
        lpModeDesc->MIDevData.cFormat   = (CHAR)pDevData->dwTVFormat;
        lpModeDesc->MIDevData.cReserved = 0x0;

        // Call the modeset DLL to see if this mode is valid.
        // We are not interested in the mode timings but just to see if this is a valid mode.
        VideoDebugPrint((1,"Before: lpModeDescr: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
                         lpModeDesc->dwXRes, lpModeDesc->dwYRes, lpModeDesc->dwBpp, lpModeDesc->dwRefreshRate, 
                         lpModeDesc->dwDevType, lpModeDesc->dwOriginalRefreshRate));
        VideoDebugPrint((1,"ulDeviceMask: 0x%x, ulIndex: %d, MonitorInfType: 0x%x, EDIDBufferValid: 0x%x\n",
                         pDevData->dwDeviceMask, ulIndex, lpDispData->dwMonitorInfType, pEdidUnit->Valid)); 

        if (pDevData->dwDeviceType == MONITOR_TYPE_NTSC || pDevData->dwDeviceType == MONITOR_TYPE_PAL)
        {
            ulRet = 0;

            if (HwDeviceExtension->ulTVEncoderType == NV_ENCODER_BROOKTREE_871) 
            {
                if (lpModeDesc->dwXRes > 1024 || lpModeDesc->dwYRes > 768 || lpModeDesc->dwRefreshRate != 60)
                { 
                    // Not a valid mode. Limit it to 1024x768x60HZ.
                    ulRet = 1; // Any non-zero value will do.
                    lpModeNew->dwXRes = 1024;
                    lpModeNew->dwYRes = 768;
                    lpModeNew->dwBpp = lpModeDesc->dwBpp;
                    lpModeNew->dwRefreshRate = 60; 
                    lpModeNew->dwDevType = lpModeDesc->dwDevType;
                    lpModeNew->dwOriginalRefreshRate = lpModeDesc->dwRefreshRate;
                }
            }
            else if (lpModeDesc->dwXRes > 800 || lpModeDesc->dwYRes > 600 || lpModeDesc->dwRefreshRate != 60)
            {
                // Not a valid mode. Limit to 800x600x60
                ulRet = 1; // Any non-zero value will do.
                lpModeNew->dwXRes = 800;
                lpModeNew->dwYRes = 600;
                lpModeNew->dwBpp = lpModeDesc->dwBpp;
                lpModeNew->dwRefreshRate = 60; 
                lpModeNew->dwDevType = lpModeDesc->dwDevType;
                lpModeNew->dwOriginalRefreshRate = lpModeDesc->dwRefreshRate;
            }
        }
        else
        {
            EnterFPCode(HwDeviceExtension->pFPStateSave, HwDeviceExtension->pFPMutex);
            ulRet = FindModeEntry(lpDispData, pEdidUnit->EDIDBuffer, pEdidUnit->Size,
                                  lpModeDesc, lpModeOut, lpModeNew);
            ExitFPCode(HwDeviceExtension->pFPStateSave, HwDeviceExtension->pFPMutex);
        }

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
            // Use the recommended physically supported mode info - width, height and refresh.
            // We can not change the depth.

            pDevData->PhysicalRectl.right = lpModeNew->dwXRes;
            pDevData->PhysicalRectl.bottom = lpModeNew->dwYRes;
            pDevData->dwRefresh = lpModeNew->dwRefreshRate;

        }

    }

    // Update the DesktopInfo structure of hwDevExtension since DrvGetModes() will get called before
    // DrvASsertMode().
    pInfo->ulNumberDacsActive = 2;
    for (i = 0; i < NV_NO_DACS; i++)
    {
        NVTWINVIEW_DEVICE_TYPE_DATA * pDevData;
        pDevData = &pData->NVTWINVIEWUNION.nvtwdevdata[i];

        pInfo->ulDeviceDisplay[i] = pData->dwDeviceDisplay[i];
        pInfo->ulDeviceMask[i] = pDevData->dwDeviceMask;
        pInfo->ulDeviceType[i] = pDevData->dwDeviceType;
        pInfo->ulTVFormat[i] = pDevData->dwTVFormat;
    }

    ChoosePrimaryDevice(HwDeviceExtension, pData);

    NVSetRegistryTwinviewInfo(HwDeviceExtension, pData);

    // For Toshiba, update the desired desktop mode info.
    if (HwDeviceExtension->UseHotKeySameMode)
    {
        ACPIToshibaModeToRegistry(HwDeviceExtension);
    }

     return;
}

//
// Reads the latest connected device mask into the hwDeviceExtension. 
// This is required for Dell. This is a no-op for Toshiba since the 
// connected device mask never changes.
//
VOID GetConnectedDevicesMask(PHW_DEVICE_EXTENSION hwDeviceExtension)
{
    ULONG ulPrev;
    NV_DESKTOP_INFO *pDeskTopInfo;

    pDeskTopInfo = (NV_DESKTOP_INFO *)&hwDeviceExtension->DeskTopInfo;
    ulPrev = hwDeviceExtension->DeskTopInfo.ulConnectedDeviceMask;
    pDeskTopInfo->ulConnectedDeviceMask = pDeskTopInfo->ulAllDeviceMask;

    if (RmConfigGetExKernel(hwDeviceExtension->DeviceReference,
                            NV_CFGEX_CHECK_CONNECTED_DEVICES, &pDeskTopInfo->ulConnectedDeviceMask, 
                            sizeof(pDeskTopInfo->ulConnectedDeviceMask)))
    {

        //
        // success.
        //
        if (pDeskTopInfo->ulConnectedDeviceMask == 0)
        {
            VideoDebugPrint((0,"Oops! ConnectedDevicemask is 0. Assume a CRT\n"));
            pDeskTopInfo->ulConnectedDeviceMask = 0x1;
        }
    }
    else
    {
        //
        // The resman had issues with our request, so spew a warning
        //
        VideoDebugPrint((0, "GetConnectedDevicesMask() -  RmConfigGetExKernel() returned error for NV_CFGEX_CHECK_CONNECTED_DEVICESI\n"));
    }

    //
    // If the new connected device mask is different than the previous one,
    // print a informational debug message.
    //
    if (ulPrev != pDeskTopInfo->ulConnectedDeviceMask)
    {
        VideoDebugPrint((1, "GetConnectedDevicesMask(): Previous ConnectedDevMask: 0x%x different than newConnectedMask: 0x%x\n",
                         ulPrev, pDeskTopInfo->ulConnectedDeviceMask));
    }
    else
    {
        VideoDebugPrint((2, "GetConnectedDevicesMask(): Previous ConnectedDevMask: 0x%x Matches with newConnectedMask: 0x%x\n",
                         ulPrev, pDeskTopInfo->ulConnectedDeviceMask));
    }
}


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

    Enumerate all devices controlled by the nVidia graphics chip.
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
    ULONG                ulDeviceMask, i, EDIDIndex;
    LPEDID_UNIT          pEdidUnit;

    //
    // Everytime enter here, need to invalidate EDID first
    //
    if (hwDeviceExtension->bNeedRetrieveEDID)
    {
        NVInitialize_DacsAndEDID(hwDeviceExtension, FALSE);
        hwDeviceExtension->bNeedRetrieveEDID = FALSE;
    }

    switch (ChildEnumInfo->ChildIndex) {

    case 0:
        //
        // This system supports ACPI. So mark it so. Note that only laptops seem to call this.
        // Desktops do not call this case.
        //
        hwDeviceExtension->ACPISystem = 1;
        if (hwDeviceExtension->pDualViewAssociatedDE)
        {
            hwDeviceExtension->pDualViewAssociatedDE->ACPISystem = 1;
        }

        //
        // Convert the ACPI Id to a device mask.
        //
        ulDeviceMask = ACPIHwIdToMask(ChildEnumInfo->ACPIHwId);
        if (ulDeviceMask == 0)
        {
            Status = VIDEO_ENUM_NO_MORE_DEVICES;
            return(Status);
        }
        
        //
        // Convert it's mask into an index inorder to retrieve it's EDID structure.
        //
        EDIDIndex = ConvertDeviceMaskToIndex(ulDeviceMask);
        pEdidUnit = hwDeviceExtension->EdidCollection->HeadMaps[EDIDIndex];


        //
        // If the device is a DFP and the registry tells us not to export the EDID, 
        // do not export the EDID.
        // This is to fix Win2k PnP bug of failing to install new PnP monitors
        //
        if (ulDeviceMask == 0x10000 && hwDeviceExtension->DFPAsDefaultMonitor)
        {
            VideoDebugPrint((1, "Not exporting the EDID for DFP per the registry setting\n"));
            *pChildType = Monitor;
            *pHwId = ChildEnumInfo->ACPIHwId;
            Status = VIDEO_ENUM_MORE_DEVICES;
            break;
        }

        //
        // Now we know our device mask. Return the EDID of this device, if it has a valid EDID.
        //
        if (pEdidUnit->Valid)
        {
            // Transfer the real EDID data. 
            VideoPortMoveMemory(pvChildDescriptor, pEdidUnit->EDIDBuffer, pEdidUnit->Size);
        }


        //
        // Set the child type as monitor
        //
        *pChildType = Monitor;

        //
        // Set the UID same as the ACPIHwId
        //
        *pHwId = ChildEnumInfo->ACPIHwId;
        Status = VIDEO_ENUM_MORE_DEVICES;
        break;


    case 1:
    case 2:
        //
        // If we are on a ACPI system (such as a laptop), we do not want to handle this case.
        // This case will handle non-ACPI systems (such as desktops).
        //
        if (hwDeviceExtension->ACPISystem)
        {
            Status = VIDEO_ENUM_INVALID_DEVICE;
            break;
        }

        //
        // For Dualview, we are going to return 2 CRT children
        //
        if (hwDeviceExtension->pDualViewAssociatedDE == NULL && 
            ChildEnumInfo->ChildIndex == 2)
        {
            Status = VIDEO_ENUM_INVALID_DEVICE;
            break;
        }

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

        //
        // Obtain the EDID structure via DDC.
        //

        if ( (hwDeviceExtension->DeviceMappingFailed == FALSE) &&
             (GetDdcInformation(HwDeviceExtension,
                                ChildEnumInfo->ChildIndex-1,
                                (PUCHAR) pvChildDescriptor,
                                ChildEnumInfo->ChildDescriptorSize))    )
        {
            *pHwId = QUERY_MONITOR_ID + (ChildEnumInfo->ChildIndex-1);

            VideoDebugPrint((1, "NvGetChildDescriptor - successfully read EDID structure\n"));

        } else {

            //
            // Alway return TRUE, since we always have a monitor output
            // on the card and it just may not be a detectable device.
            //

            *pHwId = QUERY_NONDDC_MONITOR_ID + (ChildEnumInfo->ChildIndex-1);

            VideoDebugPrint((1, "NvGetChildDescriptor - DDC not supported\n"));

        }

        Status = VIDEO_ENUM_MORE_DEVICES;
        break;

#ifdef NVPE
        case 3:

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
                    Status = VIDEO_ENUM_MORE_DEVICES;  // we want to be called again...
                }
                else
                    Status = VIDEO_ENUM_INVALID_DEVICE;

                break;

        case 4:

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
                    Status = VIDEO_ENUM_MORE_DEVICES;  // we want to be called again...
                }
                else
                    Status = VIDEO_ENUM_INVALID_DEVICE;

                break;

        case 5:

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
                    Status = VIDEO_ENUM_MORE_DEVICES;  // we want to be called again...
                }
                else
                    Status = VIDEO_ENUM_INVALID_DEVICE;

                break;
        case 6:

                // read pnpid of child capture driver from registry...
                Status = VideoPortGetRegistryParameters(
                                HwDeviceExtension,
                                L"pnpid_tvsnd",
                                FALSE,
                                NVRegistryCallbackPnpId,
                                (PVOID)(pvChildDescriptor));
                if (Status == NO_ERROR)
                {
                    // Check to see if I2C hardware is available
                    //if (I2cHardwarePresent())

                    *pHwId = NVTVSND_I2C_DEVICE_ID;
                    *pChildType = Other;
                    Status = VIDEO_ENUM_MORE_DEVICES;  // we want to be called again...
                }
                else
                    Status = VIDEO_ENUM_INVALID_DEVICE;

                break;

#endif  // #ifdef NVPE

    case 7:
        //
        // Miniport will enumerate TV as a child device if the SBIOS has not done so. This is the 
        // case with Dell laptop where the SBIOS does not enumerate TV as an ACPI device 0x200 and hence
        // miniport would never get any power management calls for TV.
        //
        if (hwDeviceExtension->EnumerateTVAsACPIDevice)
        {
            VideoDebugPrint((1, "NvGetChildDescriptor(): case 6: For TV: returning 0x200\n"));
            //
            // 0x200 is the ACPI recommended HwDevID for TV. (QUERY_ACPI_TV_ID)
            //
            *pHwId = QUERY_ACPI_TV_ID;
            *pChildType = Monitor;
            Status = VIDEO_ENUM_MORE_DEVICES;
            break;
        }
        else
        {
            VideoDebugPrint((1, "NvGetChildDescriptor(): case 6: Not enumerating TV\n"));
            //
            // The return status should be VIDEO_ENUM_INVALID_DEVICE so that the OS enumerates further for more
            // devices.
            //
            Status = VIDEO_ENUM_INVALID_DEVICE;
            break;
        }
        break;

    case DISPLAY_ADAPTER_HW_ID:

        //
        // Special ID to handle return legacy PnP IDs for root enumerated
        // devices.
        //

        *pChildType = VideoChip;
        *pHwId      = DISPLAY_ADAPTER_HW_ID;

        Status = VIDEO_ENUM_MORE_DEVICES;
        break;


    default:

        hwDeviceExtension->bNeedRetrieveEDID = TRUE;
        Status = VIDEO_ENUM_NO_MORE_DEVICES;
        break;
    }


    return Status;
}

VP_STATUS
NVGetChildState(
    PHW_DEVICE_EXTENSION hwDeviceExtension,
    PVIDEO_REQUEST_PACKET RequestPacket
    )
{
    ULONG *pOut;
    ULONG *pIn;
    ULONG ulACPIHwId, ulDeviceMask, ulHead;
    ULONG i;
    ULONG ulFoundActiveMatch;
    NVTWINVIEWDATA sData;
    NVTWINVIEW_DEVICE_TYPE_DATA * pDevData;
    ULONG NVCplInitiated;
    ULONG RegStatus, TwinViewInfoRegStatus;
    ULONG ulCombinedMask;

    RequestPacket->StatusBlock->Information = sizeof(ULONG);

    if (RequestPacket->OutputBufferLength < sizeof(ULONG))
    {
        VideoDebugPrint((0, "IOCTL_VIDEO_GET_CHILD_STATE - ERROR_INSUFFICIENT_OUTPUT_BUFFER\n"));
        return ERROR_INSUFFICIENT_BUFFER;
    }
    if (RequestPacket->InputBufferLength < sizeof(ULONG))
    {
        VideoDebugPrint((0, "IOCTL_VIDEO_GET_CHILD_STATE - ERROR_INSUFFICIENT_INPUT_BUFFER\n"));
        return ERROR_INSUFFICIENT_BUFFER;
    }

    pOut = (ULONG *) RequestPacket->OutputBuffer;
    pIn = (ULONG *) RequestPacket->InputBuffer;
    ulACPIHwId = *pIn;

    VideoDebugPrint((1, "\nW32StartIO - GetChildState: HWId: 0x%x\n",ulACPIHwId));

    if (hwDeviceExtension->DeskTopInfo.ulNumberDacsOnBoard == 2)
    {
        if (hwDeviceExtension->pDualViewAssociatedDE)
        {
            if (hwDeviceExtension->ACPISystem == 0)
            {
                if (ulACPIHwId == QUERY_NONDDC_MONITOR_ID ||
                    ulACPIHwId == QUERY_MONITOR_ID)
                {
                    *pOut = 1 | ((hwDeviceExtension->ulDualViewStatus & DUALVIEW_STATUS_SECONDARY) ? VIDEO_CHILD_DETACHED : 0);
                    VideoDebugPrint((1,"FAIL: NONDDC_MONITOR_ID/DDC_MONITOR_ID\n"));
                    return NO_ERROR;
                }
                if (ulACPIHwId == QUERY_NONDDC_MONITOR_ID2 ||
                    ulACPIHwId == QUERY_MONITOR_ID2)
                {
                    *pOut = 1 | ((hwDeviceExtension->ulDualViewStatus & DUALVIEW_STATUS_SECONDARY) ? 0 : VIDEO_CHILD_DETACHED);
                    VideoDebugPrint((1,"FAIL: DDC_MONITOR_ID\n"));
                    return NO_ERROR;
                }
            }
            else
            {
                ASSERT((hwDeviceExtension->ulDualViewStatus & DUALVIEW_STATUS_MOBILE) != 0);
                //
                // Dualview is enabled on an laptop.  All children need to be re-assigned
                //
                if ((hwDeviceExtension->ulDualViewStatus & DUALVIEW_STATUS_VIEW_ON) &&
                    (hwDeviceExtension->pDualViewAssociatedDE->ulDualViewStatus & DUALVIEW_STATUS_VIEW_ON))
                {
                    NV_DESKTOP_INFO *pDeskTop = &hwDeviceExtension->DeskTopInfo;
                    ULONG drivingDeviceId = 0;
                    
                    ulDeviceMask = pDeskTop->ulDeviceMask[pDeskTop->ulDeviceDisplay[0]];
                    
                    if (ulDeviceMask & BITMASK_ALL_CRT)
                        drivingDeviceId = QUERY_ACPI_CRT_ID;
                    else if (ulDeviceMask & BITMASK_ALL_DFP)
                        drivingDeviceId = QUERY_ACPI_DFP_ID;
                    else if (ulDeviceMask & BITMASK_ALL_TV)
                        drivingDeviceId = QUERY_ACPI_TV_ID;
                    
                    *pOut = (drivingDeviceId == ulACPIHwId) ? 1 : VIDEO_CHILD_DETACHED;
                    return NO_ERROR;
                }
            }
        }

        else if (hwDeviceExtension->ACPISystem == 0)
        {
            //
            // If the device is the  generic DDC or NON_DDC monitors that we report on desktop systems
            // (i.e. non-ACPI systems), return TRUE.
            //
            if (ulACPIHwId == QUERY_NONDDC_MONITOR_ID ||
                ulACPIHwId == QUERY_MONITOR_ID)
            {
                *pOut = TRUE;
                VideoDebugPrint((1,"SUCCESS: NONDDC_MONITOR_ID/DDC_MONITOR_ID\n"));
                return NO_ERROR;
            }
        }

    }

    //
    // For single headed cards on non-ACPI systems (such as desktops), 
    // we want to enable the "hide modes" checkbox in the NT panel,
    // so return TRUE.
    //
    if ((hwDeviceExtension->DeskTopInfo.ulNumberDacsOnBoard == 1) &&
        (hwDeviceExtension->ACPISystem == 0))
    {
        if (ulACPIHwId == QUERY_NONDDC_MONITOR_ID ||
            ulACPIHwId == QUERY_MONITOR_ID)
        {
            *pOut = TRUE;
            VideoDebugPrint((1,"SUCCESS: NONDDC_MONITOR_ID/DDC_MONITOR_ID\n"));
            return NO_ERROR;
        }
    }

    //
    // Read in the twinview info structure
    //
    TwinViewInfoRegStatus = NVGetRegistryTwinviewInfo(hwDeviceExtension, &sData);
    if ((TwinViewInfoRegStatus == NO_ERROR) && (hwDeviceExtension->DeskTopInfo.ulNumberDacsOnBoard == 2))
    {
        if (sData.dwState == NVTWINVIEW_STATE_SPAN)
        {
            VideoDebugPrint((1,"FAIL: Spanning mode: Don't remember this resolution\n"));
            *pOut = FALSE;
            return NO_ERROR;
        }
    }


    //
    // Special cases. If we a modeswitch via nVidia panel is initiated or a modeswitch via hotkey is
    // pending, use the desired devices from the twinViewInfo structure from the registry.
    // Otherwise, sometimes the taskbar gets confused and does not resize correctly.
    // We need to do this only for the laptops since desktops does not have this "confused taskbar" problem
    // as we enumerate only one child device for the desktops.
    //
    NVCplInitiated = 0;
    RegStatus = VideoPortGetRegistryParameters(hwDeviceExtension,
                               L"NVCplInduceModeSetInitiated",
                               FALSE,
                               NVRegistryCallback,
                               &(NVCplInitiated));
    if ((RegStatus == NO_ERROR && NVCplInitiated == 1 && hwDeviceExtension->ACPISystem == 1) ||
        (hwDeviceExtension->ulHotKeyPending == 1 && hwDeviceExtension->ACPISystem == 1))
    {
        //
        // Build up the device masks from the TwinViewInfo structure if present.
        //
        RegStatus = NVGetRegistryTwinviewInfo(hwDeviceExtension, &sData);
        if (TwinViewInfoRegStatus == NO_ERROR)
        {
            ASSERT(sData.dwState == NVTWINVIEW_STATE_CLONE || sData.dwState == NVTWINVIEW_STATE_NORMAL);

            if (sData.dwState == NVTWINVIEW_STATE_CLONE)
            {
                ulCombinedMask = 0;
                pDevData = &sData.NVTWINVIEWUNION.nvtwdevdata[0];
                ulCombinedMask |= pDevData->dwDeviceMask;
                pDevData = &sData.NVTWINVIEWUNION.nvtwdevdata[1];
                ulCombinedMask |= pDevData->dwDeviceMask;
            }
            if (sData.dwState == NVTWINVIEW_STATE_NORMAL)
            {
                ulCombinedMask = 0;
                ulHead = sData.dwDeviceDisplay[0];
                pDevData = &sData.NVTWINVIEWUNION.nvtwdevdata[ulHead];
                ulCombinedMask |= pDevData->dwDeviceMask;
            }

            //
            // Now compare the device mask
            //
            ulDeviceMask = ACPIHwIdToMask(ulACPIHwId);
            if (ulCombinedMask & ulDeviceMask)
            {
                // The requested device is one of nvPanel specified devices. So return success.
                *pOut = TRUE;
                VideoDebugPrint((1,"SUCCESS: nvPanel/HK Initiated Spanning mode: Active\n"));
                return NO_ERROR;
            }
            else
            {
                // This device is not one of the nvPanel specified devices. So return failure by returning FALSE.
                VideoDebugPrint((1,"FAIL: nvPanel/HK Initiated Spanning mode: not present\n"));
                *pOut = FALSE;
                return NO_ERROR;
            }
        } // if TwinViewInfo registry structure
    } //if nvCpl initiated or HK pending


    //
    // Check if we are in spanning mode.  Don't want to do mode prunning here
    //
    if (bQueryInSpanningMode(hwDeviceExtension))
    {
        *pOut = FALSE;
        VideoDebugPrint((1,"GetChildState: Inactive for Spanning\n"));
        return NO_ERROR;
    }

    // See if the device actually exists.
    if (ulACPIHwId == 0)
    {
        // Ignore devId 0 for now. Return success.
        *pOut = TRUE;
    
    }
    ulDeviceMask = ACPIHwIdToMask(ulACPIHwId);

    if (!(hwDeviceExtension->DeskTopInfo.ulConnectedDeviceMask & ulDeviceMask))
    {
        // This device is not currently present. So return failure by returning FALSE.
        VideoDebugPrint((1,"FAIL: not present\n"));
        *pOut = FALSE;
        return NO_ERROR;
    }

    // The device is present. Now see if the device is one of the active devices.
    ulFoundActiveMatch = 0;
    for (i = 0; i < hwDeviceExtension->DeskTopInfo.ulNumberDacsActive; i++)
    {
        ulHead = hwDeviceExtension->DeskTopInfo.ulDeviceDisplay[i];
        if (hwDeviceExtension->DeskTopInfo.ulDeviceMask[ulHead] == ulDeviceMask)
        {
            // We found the head for which this requested device is attached.
            ulFoundActiveMatch = 1;
        }
    }

    if (ulFoundActiveMatch)
    {
        // The requested device is one of currently active device. So return success.
        *pOut = TRUE;
        VideoDebugPrint((1,"SUCCESS: Active\n"));
    }
    else
    {
        // The requested device is not one of currently active device. So return failure.
        *pOut = FALSE;
        VideoDebugPrint((1,"FAIL: not Active\n"));
    }
    return NO_ERROR;
}

VP_STATUS
NVValidateChildState(
    PHW_DEVICE_EXTENSION hwDeviceExtension,
    PVIDEO_REQUEST_PACKET RequestPacket
    )
{
    ULONG *pOut;
    VIDEO_CHILD_STATE_CONFIGURATION *pIn;
    VIDEO_CHILD_STATE * pChildState;
    ULONG ulACPIHwId, ulDeviceMask, ulHead;
    ULONG i, j;
    ULONG ulFoundActiveMatch;

    RequestPacket->StatusBlock->Information = sizeof(ULONG);

    if (RequestPacket->OutputBufferLength < sizeof(ULONG))
    {
        VideoDebugPrint((0, "IOCTL_VIDEO_VALIDATE_CHILD_STATE_CONFIGURATION - ERROR_INSUFFICIENT_OUTPUT_BUFFER\n"));
        return ERROR_INSUFFICIENT_BUFFER;
    }
    if (RequestPacket->InputBufferLength < sizeof(VIDEO_CHILD_STATE_CONFIGURATION))
    {
        VideoDebugPrint((0, "IOCTL_VIDEO_VALIDATE_CHILD_STATE_CONFIGURATION - ERROR_INSUFFICIENT_INPUT_BUFFER\n"));
        return ERROR_INSUFFICIENT_BUFFER;
    }

    pOut = (ULONG *) RequestPacket->OutputBuffer;
    pIn = (VIDEO_CHILD_STATE_CONFIGURATION *) RequestPacket->InputBuffer;

    if (ACPIValidateChildStateShouldFail(hwDeviceExtension))
    {
        // We are in full screen DOS mode or spanning mode. So let the BIOS handle the hot keys.
        // This device is not currently present. So return failure by returning 0.
        *pOut = 0;
        VideoDebugPrint((1,"FAIL: curently in FSDOS or Spanning\n"));
        return NO_ERROR;
    }

    if (hwDeviceExtension->pDualViewAssociatedDE)
    {
        if ((hwDeviceExtension->ulDualViewStatus & DUALVIEW_STATUS_VIEW_ON) &&
            (hwDeviceExtension->pDualViewAssociatedDE->ulDualViewStatus & DUALVIEW_STATUS_VIEW_ON))
        {
            // Disable Hotkey Switch if under Dualview
            *pOut = 0;
            VideoDebugPrint((1,"FAIL: curently in FSDOS or Spanning\n"));
            return NO_ERROR;
        }
    }

    //
    // Refresh the connected device mask. 
    //
    GetConnectedDevicesMask(hwDeviceExtension);

    // Enumerate the devices. See if the device actually exists.
    // Even if one of the devices is invalid, we return failure.
    VideoDebugPrint((1, "pIN->Count: %d\n", pIn->Count));
    for (i=0; i < pIn->Count; i++)
    {
        pChildState =  &(pIn->ChildStateArray[i]);
        ulACPIHwId = pChildState->Id;
        VideoDebugPrint((1, "    HwID: 0x%x, State: 0x%x\n", pChildState->Id, pChildState->State));
        if (ulACPIHwId == 0)
        {
            // continue on to processing the next device. Ignore devId 0 for now. 
            continue;
        }
        ulDeviceMask = ACPIHwIdToMask(ulACPIHwId);
        //
        // Check against the connected devices only if this device state is to be turned on.
        // The DDK specs does not say what the behavior should be based on the device state.
        // If the state is to be turned off, then it does not matter if the device is present
        // or not, so I don't return any failure in this case. Again, note that this is a subjective
        // call on my part.
        //
        if (pChildState->State == 1)
        {
            if (!(hwDeviceExtension->DeskTopInfo.ulConnectedDeviceMask & ulDeviceMask))
            {
                // This device is not currently present. So return failure by returning 1.
                *pOut = 0;
                VideoDebugPrint((1, "FAIL: Not present\n"));
                return NO_ERROR;
            }
        }
    }
    VideoDebugPrint((1, "Return *pOut: 0x%x\n", *pOut));
    return NO_ERROR;
}

VP_STATUS
NVSetChildState(
    PHW_DEVICE_EXTENSION hwDeviceExtension,
    PVIDEO_REQUEST_PACKET RequestPacket
    )
{
    VIDEO_CHILD_STATE_CONFIGURATION *pIn;
    VIDEO_CHILD_STATE * pChildState;
    ULONG ulACPIHwId, ulDeviceMask, ulHead;
    ULONG i, j;
    ULONG ulFoundActiveMatch;
    ULONG GDIWidth, GDIHeight;
    ULONG ulDeviceMask0 = 0, ulDeviceMask1 = 0;
    ULONG ulNumTurnOn = 0;
    NV_DESKTOP_INFO * pInfo;

    pInfo = &hwDeviceExtension->DeskTopInfo;
    
    if (RequestPacket->InputBufferLength < sizeof(VIDEO_CHILD_STATE_CONFIGURATION))
    {
        VideoDebugPrint((0, "IOCTL_VIDEO_VALIDATE_CHILD_STATE_CONFIGURATION - ERROR_INSUFFICIENT_INPUT_BUFFER\n"));
        return ERROR_INSUFFICIENT_BUFFER;
    }

    // Detect if we are in spanning mode.
    if (bQueryInSpanningMode(hwDeviceExtension))
    {
        if (!bAllowHotKeyInSpanningMode(hwDeviceExtension))
        {
            // we can not return failure here since the OS will then try to use the 
            // system BIOS. So we do a fake success return here.
            VideoDebugPrint((1,"FAIL: currently in Span mode\n"));
            return NO_ERROR;
        }
    }


    pIn = (VIDEO_CHILD_STATE_CONFIGURATION *) RequestPacket->InputBuffer;

    // Count the number of devices that need to be turned on.
    // We can handle turning only 1 or 2 devices.
    VideoDebugPrint((1,"pIN->Count: %d\n",pIn->Count));
    for (i = 0; i < pIn->Count; i++)
    {
        pChildState =  &(pIn->ChildStateArray[i]);
        if (pChildState->State == 1)
        {
            ulNumTurnOn++;
        }
        VideoDebugPrint((1,"    HwID: 0x%x, State: 0x%x\n",pChildState->Id, pChildState->State));
    }

    // sanity check
    if (ulNumTurnOn <= 0 || ulNumTurnOn > 2)
    {
        // return success anyway.
        VideoDebugPrint((0,"Oops! ACPI SetDevice count is invalid: 0x%x\n",ulNumTurnOn));
        return NO_ERROR;
    }

    // Enumerate the devices. See if the device actually exists.
    // Even if one of the devices is invalid, we return failure.
    for (i = 0; i < pIn->Count; i++)
    {
        pChildState =  &(pIn->ChildStateArray[i]);
        ulACPIHwId = pChildState->Id;
        if (ulACPIHwId == 0)
        {
            VideoDebugPrint((0,"Oops! Don't know how to handle Device of zero in SetDevice\n"));
            return ERROR_INVALID_PARAMETER;
        }
        ulDeviceMask = ACPIHwIdToMask(ulACPIHwId);
        //
        // Check against the connected devices only if this device state is to be turned on.
        // The DDK specs does not say what the behavior should be based on the device state.
        // If the state is to be turned off, then it does not matter if the device is present
        // or not, so I don't return any failure in this case. Again, note that this is a subjective
        // call on my part.
        //
        if (pChildState->State == 1)
        {
            if (!(hwDeviceExtension->DeskTopInfo.ulConnectedDeviceMask & ulDeviceMask))
            {
                // This device is not currently present. So return failure by returning 1.
                VideoDebugPrint((0,"Oops! DeviceMask: 0x%x is not in the ConnectedDeviceMask: 0x%x\n",
                                 ulDeviceMask,hwDeviceExtension->DeskTopInfo.ulConnectedDeviceMask)); 
                return ERROR_INVALID_PARAMETER;
            }
        }

        // If the state of the device is set to 0, I don't know how to handle it.
        if (pChildState->State == 1)
        {
            // Initialize the first and second head's device masks.
            if (ulDeviceMask0 == 0)
            {
                ulDeviceMask0 = ulDeviceMask;
            }
            if (ulDeviceMask0 != 0)
            {
                // We have already set the DeviceMask0. So now set DeviceMask1
                ulDeviceMask1 = ulDeviceMask;
            }
        }

    }

    // Handle the case when we were called to turn off one device and turn on another
    if (pIn->Count == 2 && ulNumTurnOn == 1)
    {
        // see which of the two devices were asked to be turned on.
        if (ulDeviceMask1)
        {
            ulDeviceMask0 = ulDeviceMask1;
        }

    }
    // We were called with valid devices. Handle the single device and dual device cases separately.
    if (ulNumTurnOn == 1)
    {
        ACPISingleDeviceSwitch(hwDeviceExtension, ulDeviceMask0);
    }
    if (ulNumTurnOn == 2)
    {
        //
        // Special case. if we are in spanning mode and the two requested devices are the same, we do 
        // nothing and return success.
        //
        if (bQuerySpanDualDevicesSame(hwDeviceExtension, ulDeviceMask0, ulDeviceMask1))
        {
            VideoDebugPrint((1,"In spanning mode and same devices. So doing dummy return in nvSetChildState()\n")); 
            //
            // Now set the hotkeyPending flag so that the display driver does not obey BOOT_DEVICES.
            //
            hwDeviceExtension->ulHotKeyPending = 1;
            return NO_ERROR;
        }
        ACPIDualDeviceSwitch(hwDeviceExtension, ulDeviceMask0, ulDeviceMask1);
        //
        // Irrespective of dell or toshiba, if we get here, we know both dacs are connected.
        // So set NumDacConnected to 2. This handles the special case of hot plugging CRT on
        // Dell and getAvailableModes() in the display driver will still be thinking there is only
        // one dac connected as no device scan would have been done.
        //
        hwDeviceExtension->DeskTopInfo.ulNumberDacsConnected = 2;
    }

    // Now set the hotkeyPending flag.
    hwDeviceExtension->ulHotKeyPending = 1;

    //
    // Detect if we are in full screen DOS mode.
    // Or if we are in VGA mode (such as via the F8 option of "VGA mode" at bootup time.
    // Here we want to pass VALIDATE_CHILD_DEVICE but fail SET_CHILD_DEVICE so that the OS will
    // call the VBIOS to do the switching. Note that the GET_CHILD_STATE has to be consistent with the
    // devices specified in VALIDATE_CGILD_DEVICE and SET_CHILD_DEVICE.
    //
    if (bQueryInFSDOSorVGAMode(hwDeviceExtension))
    {
        VideoDebugPrint((1,"FAIL: currently in FSDOS or VGA mode\n"));
        //
        // Unset the hotkeyPending flag since we are not really doing the device switching but the VBIOS is.
        //
        hwDeviceExtension->ulHotKeyPending = 0;
        return ERROR_INVALID_PARAMETER;
    }

    return NO_ERROR;
}


//**************************************************************************************
//
// Function: NVSwitchDualView
//
// DESCRIPTION:
//  Anotify from GDI about switching to Dual View(MHS) or Single View mode.
//
// PARAMETERS:
//  pHwDeviceExtension  Points to per-display(view) device extension.
//  dwDualViewEnable    1 : View will be enabled
//                      0 : View will be disabled
//
// RETURN VALUE:
//  Status code.
//
//**************************************************************************************

VP_STATUS
NVSwitchDualView(
    PHW_DEVICE_EXTENSION hwDeviceExtension,
    ULONG                dwViewEnable
    )
{
    PHW_DEVICE_EXTENSION pHwDE1;

    VideoDebugPrint ((0, "****SwitchDualView: %s : %s\n", 
                      ((hwDeviceExtension->ulDualViewStatus & DUALVIEW_STATUS_SECONDARY) ? "Secondary" : "Primary"),
                      (dwViewEnable? "On" : "Off")));

    //
    // Assume GDI always call in order of Primary, Secondary. Only after all view be called, 
    // we can decide it will be in SingleView or DualView and then take actions 
    //

    if (dwViewEnable)
        hwDeviceExtension->ulDualViewStatus |= DUALVIEW_STATUS_VIEW_ON;
    else
        hwDeviceExtension->ulDualViewStatus &= ~DUALVIEW_STATUS_VIEW_ON;

    //
    // In the primary view call, we just set/clear ViewOn flags. 
    //        
    if (!(hwDeviceExtension->ulDualViewStatus & DUALVIEW_STATUS_SECONDARY))
    {
        return NO_ERROR;
    }

    pHwDE1 = hwDeviceExtension->pDualViewAssociatedDE;
    if ((pHwDE1->ulDualViewStatus & DUALVIEW_STATUS_VIEW_ON) && 
        (hwDeviceExtension->ulDualViewStatus & DUALVIEW_STATUS_VIEW_ON))
    {
        HandleDualviewHeads(pHwDE1, TRUE);
    }
    else
    {
        HandleDualviewHeads(pHwDE1, FALSE);
    }
    return NO_ERROR;
}

#endif // #if (_WIN32_WINNT >= 0x0500)


