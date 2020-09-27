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

extern VP_STATUS
NVReadRegistryTwinViewInfoCallback(
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
                DstModeEntry->dwXRes        = SrcModeEntry->Width;
                DstModeEntry->dwYRes        = SrcModeEntry->Height;
                DstModeEntry->dwBpp         = SrcModeEntry->Depth;
                DstModeEntry->dwRefreshRate = SrcModeEntry->RefreshRate;
                DstModeEntry++;
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
    NVTWINVIEWDATA pInfo;
    ULONG ulHead;
    VP_STATUS status;

    hwDeviceExtension = (PHW_DEVICE_EXTENSION)lpDispData->dwContext1;
    status = VideoPortGetRegistryParameters(
                                       hwDeviceExtension,
                                       L"TwinviewInfo",
                                       FALSE,
                                       NVReadRegistryTwinViewInfoCallback,
                                       &pInfo);
    if (status == NO_ERROR)
    {
        ulHead = lpDispData->dwCRTCIndex;
        switch (pInfo.NVTWINVIEWUNION.nvtwdevdata[ulHead].dwTimingOverRide)
        {
            case TIMING_OVERRIDE_GTF:
                VideoDebugPrint((5,"GetTimingStandardOverride(): Forced to use GTF\n"));
                return  (GTFV_ID);
                break;
            case TIMING_OVERRIDE_DMT:
                VideoDebugPrint((5,"GetTimingStandardOverride(): Forced to use DMT\n"));
                return  (DMTV_ID);
                break;
            case TIMING_OVERRIDE_AUTO:
                return  (NONE_ID);
                break;
            default:
                VideoDebugPrint((5,"Oops! GetTimingStandardOverride(): Invalid TimingOverride from registry: 0x%x\n",
                    pInfo.NVTWINVIEWUNION.nvtwdevdata[ulHead].dwTimingOverRide));
                return  (NONE_ID);
                break;
        }
    }

    // TwinViewInfo registry entry not found.
    // So look at the old registry entries.
    // Note: Not sure if we should honor the old registry entries. Check with Peter.
    if(hwDeviceExtension->bUseGTF) {
        VideoDebugPrint((5,"GetTimingStandardOverride(): Forced to use GTF\n"));
        return  (GTFV_ID);
    }
    if(hwDeviceExtension->bUseDMT) {
        VideoDebugPrint((5,"GetTimingStandardOverride(): Forced to use DMT\n"));
        return  (DMTV_ID);
    }

    return  (NONE_ID);
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
                lpDispData - identifies the specific board that we
                             want the active display port for.
    Returns:    "AllowAllModes" key value or FALSE.
*/
ULONG CFUNC GetAllowAllModesFlag(LPDISPDATA lpDispData)
{
return(FALSE);
}


