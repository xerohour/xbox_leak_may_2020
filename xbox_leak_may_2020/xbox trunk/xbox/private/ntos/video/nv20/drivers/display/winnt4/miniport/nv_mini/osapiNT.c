/*
    FILE:   osapi.c
    DATE:   4/8/99

    This file holds code for functions that are called back to by
    the generic modeset code.
*/

#include "cmntypes.h"
#include "modeext.h"
#include "modeset.h"
#include "debug.h"
#include "nvreg.h"
#include "restime.h"
#include "osapi.h"
#include "utils.h"
#include "edid.h"
#include "gtfmath.h"
#include "dmt.h"

#include "dderror.h"
#include "devioctl.h"
#include "miniport.h"
#include "ntddvdeo.h"
#include "video.h"
#include "nv.h"

// Bring this in to get the flat panel structs
#include "nvcm.h"
#include "cmdcnst.h"
#include "monitor.h"
#include "vesadata.h"
#include "nvMultiMon.h"

extern VP_STATUS NVGetRegistryTwinviewInfo (
    PHW_DEVICE_EXTENSION hwDeviceExtension,
    NVTWINVIEWDATA      *pTwinviewInfo
    );

extern BOOLEAN   TwinViewDataOK(PHW_DEVICE_EXTENSION hwDeviceExtension, NVTWINVIEWDATA *pData);
VOID SetTimingOverRide(PHW_DEVICE_EXTENSION HwDeviceExtension);
ULONG GetBestResolution(PHW_DEVICE_EXTENSION HwDeviceExtension, ULONG ulEDIDIndex, ULONG ulHead, ULONG ulDeviceMask, MODE_ENTRY *pBestMode);
extern int CFUNC EdidFindLargestEdidMode (LPCHAR lpEdidBuffer, ULONG dwEdidSize,LPMODEINFO lpModeInfo);
extern VOID EnterFPCode(VOID *pSave, VOID *pMutex);
extern VOID ExitFPCode(VOID *pSave, VOID *pMutex);
extern ULONG GetTimingDacCommonModesetCode(PHW_DEVICE_EXTENSION HwDeviceExtension, HEAD_RESOLUTION_INFO *pResolution, DAC_TIMING_VALUES * pTimingInfo);

extern VP_STATUS
NVRegistryCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    );

extern VP_STATUS
NVReadMonitorTimingModeCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    );

//******************************************************************************
//
// External Function Declarations
//
//******************************************************************************

extern BOOL RmConfigGetExKernel
(
    U032 deviceReference,
    U032 index,
    VOID* pParms,
    U032 parmSize
);

extern BOOL RmConfigGetKernel (U032, U032, U032*);


// GK UNDO
float __fltused = 0.0;

// Hack. See the '4' below. Thats because the modeentry structure has 4 ULONG fields in it.
// Later we should dynamically malloc it. For now, we create a static global array.                                                                                                     
#define MAX_VALID_MODE_TABLE_DATA_WORDS_MODESET (MAX_STRING_REGISTRY_LINES * MAX_VALID_MODE_REFRESH_RATES * 4)

ULONG glpModeEntry[MAX_VALID_MODE_TABLE_DATA_WORDS_MODESET];


/*
    GetMasterModeList

    This routine returns a ptr to a an array of MODEENTRY
    structures which describe the list of available modes for
    the board specified. This routine needs to ALWAYS return
    some mode list. If you have to just return a list of safe
    modes like 640,480,8,60Hz.

    You can alloc memory for the mode list because you will be
    given a chance to free the ptr later.
*/
LPMODEENTRY CFUNC
GetMasterModeList (LPDISPDATA lpBoardData)
{
    PHW_DEVICE_EXTENSION hwDeviceExtension;
    ULONG NumRegModes;
    PMODE_ENTRY SrcModeEntry;
    LPMODEENTRY DstModeEntry;
    LPMODEENTRY pRet;
    ULONG i;

    hwDeviceExtension = (PHW_DEVICE_EXTENSION)lpBoardData->dwContext1;
    NumRegModes = hwDeviceExtension->NumRegistryModes;
    SrcModeEntry = (PMODE_ENTRY)&(hwDeviceExtension->ValidModeTable[0]);
    DstModeEntry = (LPMODEENTRY)&glpModeEntry[0];

    
    //Copy the registry mode list into the modeset list.
    for (i=0; i < NumRegModes; i++)
    {
        if (SrcModeEntry->ValidMode)
        {
            // Do not pass spanning resolutions. Limit only to single resolutions.
            if (!MULTIMON_MODE(SrcModeEntry->Width, SrcModeEntry->Height))
            {
                DstModeEntry->dwXRes        = SrcModeEntry->Width;
                DstModeEntry->dwYRes        = SrcModeEntry->Height;
                DstModeEntry->dwBpp         = SrcModeEntry->Depth;
                DstModeEntry->dwRefreshRate = SrcModeEntry->RefreshRate;
                DstModeEntry++;
            }
        }
        SrcModeEntry++;
    }

    // The last entry needs to be zeroed out.
    DstModeEntry->dwXRes        = 0;
    DstModeEntry->dwYRes        = 0;
    DstModeEntry->dwBpp         = 0;
    DstModeEntry->dwRefreshRate = 0;

    // You can't return NULL -- this is just a template.
    pRet = (LPMODEENTRY)&glpModeEntry[0];
    return(pRet);
}


/*
    FreeMasterModeList

    This routine is called when the caller is done with the mode
    list it got earlier.
*/
int CFUNC
FreeMasterModeList (LPMODEENTRY lpModeList)
{
    return  (TRUE);
}

/*
    GetSafeModeList

    This routine just returns a few safe modes in the mode list.
    It always returns TRUE.
*/
int CFUNC GetSafeModeList (LPMODEENTRY lpModeList)
{
    return  (TRUE);
}


// Just in case we can't GlobalAlloc a list, we have a single entry one
// we can use here.
RESTIME     sRTList;

/*
    GetMasterRTList

    This routine returns a ptr to a an array of RESTIME
    structures which describe the list of R&T strings for
    the board specified. This routine needs to ALWAYS return
    some RT list. If you have to just return a blank list.

    You can alloc memory for the mode list because you will be
    given a chance to free the ptr later.
*/
LPRESTIME CFUNC
GetMasterRTList (LPDISPDATA lpBoardData)
{
    LPRESTIME   lpRTList;

    // Here is returning a blank list.
    lpRTList = &sRTList;
    lpRTList[0].dwXRes = 0;
    return  (lpRTList);
}


/*
    FreeMasterRTList

    This routine is called when the caller is done with the RTList.
*/
int CFUNC
FreeMasterRTList (LPRESTIME lpRTList)
{
    return  (TRUE);
}

/*
    GetTimingStandardOverride

    This routine gets any timing standard override for the board
    specified. It should return one of the constants defined in
    restime.h like DMTV_ID. A timing standard override is a
    timing standard that applies to all modes. Just something
    that the user might want to enforce.
*/
ULONG CFUNC
GetTimingStandardOverride (LPDISPDATA lpDispData)
{
    PHW_DEVICE_EXTENSION hwDeviceExtension;
    ULONG ulHead;
    
    hwDeviceExtension = (PHW_DEVICE_EXTENSION)lpDispData->dwContext1;
    ulHead = lpDispData->dwCRTCIndex;

    return(hwDeviceExtension->ulTimingOverrideTwinViewInfo[ulHead]);
    
}


/*
    Function:   GetPitchAndSize

    Purpose:    This routine returns the pitch for a given mode.

    Arguments:  lpDispData  - identifies the specific board that we
                              want the active display port for.
                dwXRes      - The X res in pixels for the mode
                              should be returned.
                dwBpp       - the bits per pixel for the mode

    Returns:    The pitch in bytes
*/

ULONG CFUNC
GetPitchAndSize (LPDISPDATA lpDispData, ULONG dwXRes, ULONG dwYRes, ULONG dwBpp, ULONG *pdwPitch, ULONG *pdwSize)
{
    PHW_DEVICE_EXTENSION hwDeviceExtension;
    NV_CFGEX_GET_SURFACE_DIMENSIONS_PARAMS nvPitch;
    ULONG PitchInBytes;

    hwDeviceExtension = (PHW_DEVICE_EXTENSION)lpDispData->dwContext1;

    nvPitch.Width = dwXRes;
    nvPitch.Height= dwYRes;
    nvPitch.Depth = dwBpp;

    RmConfigGetExKernel(hwDeviceExtension->DeviceReference, NV_CFGEX_GET_SURFACE_DIMENSIONS, &nvPitch, sizeof(NV_CFGEX_GET_SURFACE_DIMENSIONS_PARAMS));
    PitchInBytes = nvPitch.Pitch;

    *pdwPitch = nvPitch.Pitch;
    *pdwSize =  PitchInBytes * dwYRes;
            
    return(PitchInBytes);
}


/*
    Function:   GetMonitorInfRangeLimits

    Purpose:    This routine reads the "MaxResolution" key in the
                active monitor node in the registry to get the
                maximum resolution the monitor can do. It uses
                the maximum resolution itself (like "1600,1200")
                as another subkey under which is typically a
                monitor range limits string which looks like
                "30-95,60-120,-,+" where the first range is
                the horizontal limits inKHz and the second is
                the vertical range in hertz. The last two characters
                are horizontal and vertical sync polarities.
                This range limits string doesn't always exist.

    Arguments:
                lpDispData  - identifies the specific board that we
                              want the active display port for.
                lpML        - ptr to a monitor limits structure
                              that will be filled in with range
                              limits obtained or computed from
                              values stuffed in the registry
                              by the moitor .inf file.
                lpdwXRes    - ptr to a variable that will be set
                              to the maximum X-resolution found
                              for this monitor.
                lpdwYRes    - ptr to a variable that will be set
                              to the maximum Y-resolution found
                              for this monitor.

    Returns:    TRUE    then lpML, lpdwXRes, and lpdwYRes have been
                        filled in correctly.
                FALSE   then there was an error getting some value.
*/
int CFUNC
GetMonitorInfRangeLimits (
    LPDISPDATA      lpDispData,
    LPMONITORLIMITS lpML,
    ULONG           *lpdwXRes,
    ULONG           *lpdwYRes)
{
    return  (TRUE);
}


/*
    Function:   GetMonitorLimitMode

    Purpose:    This routine gets the mode which should be an
                upper bound for a monitor which has no edid and
                no .inf.

    Arguments:  lpDispData  - identifies the specific board that we
                              want the active display port for.
                lpModeInfo  - ptr to a place to store the limit mode

    Returns:    If this routine returns TRUE, then lpModeInfo was
                filled in with the limit mode. If this routine
                returns FALSE, then there is no limit mode.
*/
int CFUNC
GetMonitorLimitMode (LPDISPDATA lpDispData, LPMODEINFO lpModeInfo)
{
    return(FALSE);
}


/*
    Function:   GetAllowAllModesFlag

    Purpose:    WIN9X spacific function for testing only.
        This function get "AllowAllModes" registry key
        for specified DispData.

    Arguments:
                lpRegData  - identifies a specific board
    Returns:    "AllowAllModes" key value or FALSE.
*/
ULONG WINAPI GetAllowAllModesFlag(LPREGDATA lpRegData)
{
return(FALSE);
}


/*
    Function:   IsModeAcceptededByDriver

    Purpose:    This routine allows the display driver to perform any
                other restrictions on a potential mode that it wants to.
                This is the final step in the mode restriction. This
                routine has access to the output timing parameters
                so it can do all sorts of fun checking.

    Arguments:  lpDispData  - identifies the specific board that we
                              want the active display port for.
                lpModeOut   - the timing parameters for the mode
                              you may modify them in place if you
                              want them changed.
                lpEdidBuffer  ptr to the EDID
                dwEdidSize    size of the edid in bytes (0 if no edid)

    Returns:    TRUE    if this mode is allowed
                FALSE   if the mode should be rejected

                Note that it is possible to modify the timings in
                lpModeOut and still return TRUE. Your modified
                timings will be used. However, this capability
                should really only be used in exceptional cases.
                If there is some reason to be modifying timings,
                let's get it into the modeset dll rather than
                having the display driver under every OS do
                something special.
*/
ULONG CFUNC
IsModeAcceptedByDriver (LPDISPDATA lpDispData, LPMODEOUT lpModeOut,
                        LPCHAR lpEdidBuffer, ULONG dwEdidSize)
{
    return  (TRUE);
}


//
// First reads the "TwinViewInfo" registry to get the timing override.
// If there is no such registry entry, then reads the "DMTOVERRIDE" and "MonitorTiming" registry entries.
//
VOID SetTimingOverRide(PHW_DEVICE_EXTENSION HwDeviceExtension)
{
    NVTWINVIEWDATA pInfo;
    ULONG ulHead;
    VP_STATUS status;
    PHW_DEVICE_EXTENSION hwDeviceExtension;
    ULONG i;

    hwDeviceExtension = HwDeviceExtension;

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
    if(HwDeviceExtension->bUseDMT) {
        HwDeviceExtension->bUseGTF = FALSE;
    }

    
    status = NVGetRegistryTwinviewInfo(HwDeviceExtension, &pInfo);
    if (status == NO_ERROR)
    {
        // See if the twinview data is valid.
        if (TwinViewDataOK(HwDeviceExtension, &pInfo))
        {

            for (i=0; i < HwDeviceExtension->DeskTopInfo.ulNumberDacsOnBoard; i++)
            {
                ulHead = pInfo.dwDeviceDisplay[i];

                switch (pInfo.NVTWINVIEWUNION.nvtwdevdata[ulHead].dwTimingOverRide)
                {
                case TIMING_OVERRIDE_GTF:
                    VideoDebugPrint((5,"GetTimingStandardOverride(): Forced to use GTF\n"));
                    HwDeviceExtension->ulTimingOverrideTwinViewInfo[ulHead] = GTFV_ID;
                    break;
                case TIMING_OVERRIDE_DMT:
                    VideoDebugPrint((5,"GetTimingStandardOverride(): Forced to use DMT\n"));
                    HwDeviceExtension->ulTimingOverrideTwinViewInfo[ulHead] = DMTV_ID;
                    break;
                case TIMING_OVERRIDE_AUTO:
                    HwDeviceExtension->ulTimingOverrideTwinViewInfo[ulHead] = NONE_ID;
                    break;
                default:
                    VideoDebugPrint((5,"Oops! GetTimingStandardOverride(): Invalid TimingOverride from registry: 0x%x\n",
                                     pInfo.NVTWINVIEWUNION.nvtwdevdata[ulHead].dwTimingOverRide));
                    HwDeviceExtension->ulTimingOverrideTwinViewInfo[ulHead] = NONE_ID;
                    break;
                }
            }
            return;
        }
    }

    for (i=0; i < NV_NO_DACS; i++)
    {

        // TwinViewInfo registry entry not found.
        // So look at the old registry entries.
        // Note: Not sure if we should honor the old registry entries. Check with Peter.
        if (hwDeviceExtension->bUseGTF)
        {
            VideoDebugPrint((5,"GetTimingStandardOverride(): Forced to use GTF\n"));
            HwDeviceExtension->ulTimingOverrideTwinViewInfo[i] = GTFV_ID;
        }
        else if (hwDeviceExtension->bUseDMT)
        {
            VideoDebugPrint((5,"GetTimingStandardOverride(): Forced to use DMT\n"));
            HwDeviceExtension->ulTimingOverrideTwinViewInfo[i] = DMTV_ID;
        }
        else
        {
            VideoDebugPrint((5,"GetTimingStandardOverride(): Forced to use NONE_ID\n"));
            HwDeviceExtension->ulTimingOverrideTwinViewInfo[i] = NONE_ID;
        }
    }
    return;
}

//
// Finds the best resolution for the device with the valid EDID index.
// Finds the largest resolution at 16bpp and the highest refresh rate at this resolution.
// Returns TRUE if it finds a best mode. Otherwise returns FALSE.
//
ULONG GetBestResolution(PHW_DEVICE_EXTENSION HwDeviceExtension, ULONG ulEDIDIndex, ULONG ulHead, ULONG ulDeviceMask, MODE_ENTRY *pBestMode)
{
    LPMODEINFO lpModeDesc;
    MODEINFO ModeDesc;
    HEAD_RESOLUTION_INFO sResolution;
    HEAD_RESOLUTION_INFO *pResolution;
    DAC_TIMING_VALUES * pTimingInfo;
    DAC_TIMING_VALUES   sTimingInfo;

    VideoDebugPrint((1,"GetBestResolution for index: %d\n", ulEDIDIndex));
    lpModeDesc = &ModeDesc;
    pTimingInfo = &sTimingInfo;
    pResolution = &sResolution;
    lpModeDesc->dwBpp = 32;
    lpModeDesc->dwRefreshRate = 60;

    EnterFPCode(HwDeviceExtension->pFPStateSave, HwDeviceExtension->pFPMutex);
    EdidFindLargestEdidMode (HwDeviceExtension->EdidCollection->HeadMaps[ulEDIDIndex]->EDIDBuffer,
                             HwDeviceExtension->EdidCollection->HeadMaps[ulEDIDIndex]->Size, lpModeDesc);
    ExitFPCode(HwDeviceExtension->pFPStateSave, HwDeviceExtension->pFPMutex);

    if (lpModeDesc->dwXRes == 640 && lpModeDesc->dwYRes == 480)
    {
        // the common modeset code fell back to defaut res.
        VideoDebugPrint((1,"Failure: FindLargestEdidMode(): (%d, %d), %d bpp, %d Hz\n", 
            lpModeDesc->dwXRes,lpModeDesc->dwYRes,lpModeDesc->dwBpp, lpModeDesc->dwRefreshRate));
        return(FALSE);
    }
    else
    {   
        // Print the mode info.
        VideoDebugPrint((1,"FindLargestEdidMode(): (%d, %d), %d bpp, %d Hz\n", 
            lpModeDesc->dwXRes,lpModeDesc->dwYRes,lpModeDesc->dwBpp, lpModeDesc->dwRefreshRate));

        // What we have now is the best possible mode using EDID only. Now we need to filter it against the other
        // parameters like video memory, pixel clock frequency etc..
        pResolution->ulHead = ulHead;
        pResolution->ulDeviceMask = ulDeviceMask;
        pResolution->ulDisplayWidth = lpModeDesc->dwXRes;
        pResolution->ulDisplayHeight = lpModeDesc->dwYRes;
        pResolution->ulDisplayPixelDepth = lpModeDesc->dwBpp;
        pResolution->ulDisplayRefresh = lpModeDesc->dwRefreshRate;
        pResolution->ulOption = 0;

        GetTimingDacCommonModesetCode(HwDeviceExtension, pResolution, pTimingInfo);

        // If the width, height and depth were maintained, then use the mode. Possibly, the refresh rate 
        // could have changed.
        if (pTimingInfo->HorizontalVisible == lpModeDesc->dwXRes &&
            pTimingInfo->VerticalVisible == lpModeDesc->dwYRes)
        {
            // Copy the info into the BestMode parameter.
            pBestMode->Width = (USHORT)lpModeDesc->dwXRes;
            pBestMode->Height = (USHORT)lpModeDesc->dwYRes;
            pBestMode->Depth = (USHORT)lpModeDesc->dwBpp;
            pBestMode->RefreshRate = (USHORT)pTimingInfo->Refresh;
            pBestMode->ValidMode = 1;
            VideoDebugPrint((1,"Success: GetTimingDac(): (%d, %d), %d bpp, %d Hz\n", 
                pTimingInfo->HorizontalVisible,pTimingInfo->VerticalVisible,
                lpModeDesc->dwBpp, pTimingInfo->Refresh));

            return(TRUE);
        }
        else
        {
            // Could not support the mode.
            VideoDebugPrint((1,"Failure: GetTimingDac(): (%d, %d), %d bpp, %d Hz\n", 
                pTimingInfo->HorizontalVisible,pTimingInfo->VerticalVisible,
                lpModeDesc->dwBpp, pTimingInfo->Refresh));
            return(FALSE);
        }
    }
}
