/*
    FILE:   modeset.c
    DATE:   4/8/99

    This file is the generic entry point for the modeset code.
    It contains all the high level logic when modesetting.

    The main entry point is
    FindModeEntry (lpDispData,lpEdidBuffer,dwEdidSize,lpModeDesc,lpModeOut);

    You should consult the header for FindModeEntry for specific details
    about the arguments.
*/

#include "cmntypes.h"
#include "modeext.h"
#include "modeset.h"
#include "debug.h"
#include "restime.h"
#include "tvmode.h"
#include "digmode.h"
#include "dmt.h"
#include "osapi.h"
#include "utils.h"

char    szDebug[512];
ULONG   dwDebugLevel = 0;

/*
    Function:   FindModeEntry

    Purpose:

    This routine validates and/or sets the mode. General idea:

    1) For NTSC and PAL devices, the requested mode is compared
       against the restriction list and the master mode list.
       The resolution is adjusted downward as necessary to find
       a mode that is allowed. If no mode is found that is
       permitted, the safe timing is returned.
       MODE_RESOLUTION_ADJUSTED is the actual return value if
       the mode timings returned are not for the mode that was
       requested.

       For NTSC and PAL, the only relevant input fields are:
       lpDispData (all fields), lpEdidBuffer should be NULL,
       dwEdidSize should be 0, lpModeDesc->nXRes, lpModeDesc->nYRes,
       lpModeDesc->nBpp, lpModeDesc->dwDevType should be DEVTYPE_NTSC
       or DEVTYPE_PAL as appropriate.

       On Exit, lpModeOut->rXRes and lpModeOut->rYRes will be assigned
       with the "best fit" mode that can be set. If you are validating
       an NTSC or PAL mode, then if this routine returns anything
       except MODE_EXACT_MATCH, the mode did not validate.

    2) For CRT and PANEL devices, the requested mode is compared
       against the restriction list and the master mode list.
       If necessary, first the refresh rate and as a last resort
       the resolution can be adjusted downward to find a mode that
       is allowed. If no mode is found that is permitted, a set of
       safe settings for some default mode is returned.

       For CRT and PANEL, the only relevant input fields are:
       lpDispData (all fields), lpEdidBuffer should point to a
       valid EDID if the device has one -- otherwise it shoud be NULL,
       dwEdidSize should be the size of the EDID in bytes -- otherwise
       it should be 0, lpModeDesc->nXRes, lpModeDesc->nYRes,
       lpModeDesc->nBpp, lpModeDesc->dwRefreshRate, lpModeDesc->dwDevType
       should be DEVTYPE_CRT or DEVTYPE_PANEL as appropriate,
       lpModeDesc->dwRefreshRate is a specific refresh rate value.

       For CRT and PANEL, the only possible return values are:
       MODE_RESOLUTION_ADJUSTED, MODE_REFRESH_RATE_ADJUSTED,
       or MODE_RESOLUTION_ADJUSTED | MODE_REFRESH_RATE_ADJUSTED.
       This flavor of the routine can NEVER fail. The lpModeOut
       structure should be filled out in its entirety -- all fields
       need to have describe a coherent valid timing. No exceptions.

    Arguments:
        LPDISPDATA      lpDispData
        LPCHAR          lpEdidBuffer
        ULONG           dwEdidSize
        LPMODEINFO      lpModeDesc
        LPMODEOUT       lpModeOut

    Preserve:   Do not change any fields in any of the arguments except
                to pass back the actual mode that was set in lpModeOut.
*/

int WINAPI
FindModeEntry (
    LPDISPDATA lpDispData,
    LPCHAR      lpEdidBuffer,
    ULONG       dwEdidSize,
    LPMODEINFO  lpModeDesc,
    LPMODEOUT   lpModeOut,
    LPMODEINFO  lpModeNew)
{
    int         nRet;
    LPRESTIME   lpRTList;
    LPMODEENTRY lpModeList;
    MODEINFO    ModeDesc;
    ULONG       dwModeSize;

    // Dump out what we are searching for
    DBG_PRINT0(DL_0, "\r\nSearching to match the following mode:");
    DBG_PRINT1(DL_0, "\r\nXRes           = %ld", lpModeDesc->dwXRes);
    DBG_PRINT1(DL_0, "\r\nYRes           = %ld", lpModeDesc->dwYRes);
    DBG_PRINT1(DL_0, "\r\nBpp            = %ld", lpModeDesc->dwBpp);
    DBG_PRINT1(DL_0, "\r\nRefreshRate    = %ld", lpModeDesc->dwRefreshRate);
    DBG_PRINT1(DL_0, "\r\ndwDevType      = %ld", lpModeDesc->MIDevData.cType);

    DBG_PRINT0(DL_0, "\r\n\nFor the following board:");
    DBG_PRINT1(DL_0, "\r\nDeviceID                = %lx", lpDispData->dwDeviceID);
    DBG_PRINT1(DL_0, "\r\nMaxDacSpeedInHertz8bpp  = %lx", lpDispData->dwMaxDacSpeedInHertz8bpp);
    DBG_PRINT1(DL_0, "\r\nMaxDacSpeedInHertz16bpp = %lx", lpDispData->dwMaxDacSpeedInHertz16bpp);
    DBG_PRINT1(DL_0, "\r\nMaxDacSpeedInHertz32bpp = %lx", lpDispData->dwMaxDacSpeedInHertz32bpp);
    DBG_PRINT1(DL_0, "\r\nVideoMemorySize         = %lx", lpDispData->dwVideoMemoryInBytes);

    // Grab the restriction and timing list
    lpRTList = GetMasterRTList (lpDispData);

    // Next, grab the mode list
    lpModeList = GetMasterModeList (lpDispData);

    // Let's assign this to a local so we can change it if we need
    // to without affecting the source.
    ModeDesc = lpModeDesc[0];

    // Let's handle the NTSC/PAL cases here, and be done with them.
    if  (lpModeDesc->MIDevData.cType == DEVTYPE_TV)
    {
        DBG_PRINT0 (DL_0, "\r\nDevice type is NTSC or PAL.");
        nRet = FindTVModeEntry (lpDispData, &ModeDesc, lpModeOut,
                                lpModeList, lpRTList);
    }
    else
    {
        // At this point, we know we are a CRT or FLAT PANEL.
        DBG_PRINT0 (DL_0, "\r\nDevice type is CRT or FLAT PANEL.");

        nRet = FindDigModeEntry (lpEdidBuffer, dwEdidSize, lpDispData,
                          &ModeDesc, lpModeOut, lpModeList, lpRTList);
    }

    // We need to set the pitch in the MODEOUT
    GetPitchAndSize (lpDispData, ModeDesc.dwXRes, ModeDesc.dwYRes,
                ModeDesc.dwBpp, &(lpModeOut->dwPitchInBytes), &dwModeSize);

    // Convert the MODEOUT structure back into a MODEINFO structure
    SetupParamsForModeInfo (lpModeOut, &ModeDesc, lpModeNew);

    FreeMasterRTList (lpRTList);
    FreeMasterModeList (lpModeList);

    return  (nRet);
}


/*
    Function:   FindVirtualModeEntry

    Purpose:

    This routine validates modes just against the amount of memory
    they will consume.No checking is done to see if the mode is
    settable on a CRTC or to get timing parameters for the mode.

    If the mode does not fit in the size specified, the mode is
    backed off progressively in resolution until it will fit.

    Arguments:
                lpRegData       LPREGDATA
                lpModeToTest    LPMODEINFO desribing the mode we want to
                                start out with
                lpModeThatFit   This will hold the mode that actually
                                will fit in dwAvailableMemory. It might
                                be the same as lpModeToTest.
                dwAvailableMemory memory available for the mode
                pfnGetModeMemory  Callback function to get the amount
                                of memory a mode takes up
                dwContext1      data to be passed into callback
                dwContext2      data to be passed into callback

        The format of the callback is:
        pfnGetModeMemory (dwContext1, dwContext2, xres, yres, bpp)
        It returns a ULONG that specifies the memory required to set
        the mode.

    Returns:    TRUE
    Preserve:
*/
typedef int (WINAPI *PFNGETMODEMEMORY) (ULONG, ULONG, ULONG, ULONG, ULONG);

int WINAPI
FindVirtualModeEntry (LPDISPDATA lpDispData, LPMODEINFO lpModeToTest,
                      LPMODEINFO lpModeThatFit, ULONG dwAvailableMemory)
{
    LPMODEENTRY lpModeList;
    ULONG       dwModeMemory, dwPitch;

    // Next, grab the mode list
    lpModeList = GetMasterModeList (lpDispData);

    // Let's assign this to a local so we can change it if we need
    // to without affecting the source.
    *lpModeThatFit = *lpModeToTest;

    do
    {
        GetPitchAndSize (lpDispData, lpModeThatFit->dwXRes,
                        lpModeThatFit->dwYRes, lpModeThatFit->dwBpp,
                        &dwPitch, &dwModeMemory);

        if  (dwModeMemory <= dwAvailableMemory)
            break;
    }
    while   (BackOffDigModeEntry (lpModeList, lpModeThatFit, lpModeToTest, 0));

    // Free this up
    FreeMasterModeList (lpModeList);

    return  (TRUE);
}


/*
    Function:   FindNextSmallerResolution

    Purpose:    This routine finds the next lower resolution at the
                same refresh rate and pixel depth as requested. If
                it finds one, it modifies lpModeDesc to reflect the
                new mode and returns TRUE, else it returns FALSE.

                There are certain modes that the other code in the
                modeset DLL will accept if they are received exactly
                as requested, but which we will never "back off" to
                reach. These modes are the non-desktop modes --
                things like modes smaller.than 640x480 and also the
                960x720 mode for DirectX. These modes will not be
                "found" as next smaller resolutions.

    Arguments:
                lpModeList  ptr to MODEENTRY array -- master mode list
                lpModeDesc  ptr to MODEINFO -- mode to back off

    Returns:    TRUE    lpModeDesc has new backed off mode
                FALSE   there was no smaller mode in the lpModeList

    Preserve:
*/
int CFUNC
FindNextSmallerResolution (
    LPMODEENTRY lpModeList,
    LPMODEINFO  lpModeDesc)
{
    int         i;
    MODEINFO    ModeIn, ModeSmaller;

    // This should be easy. We look at the mode in lpModeDesc and
    // Find the next smaller spatial resolution mode in lpModeList
    // that is at the same pixel depth. Smaller mode is defined as:
    // A lower value for the X-Res is definitely smaller
    // A same value for the X-Res forces a look at the Y-Res
    //      A lower value for the Y-Res is smaller

    // Save the passed in mode because we are going to change it.
    ModeIn = lpModeDesc[0];

    // Set up the smaller mode to be 0 initially
    ModeSmaller.dwXRes = 0;
    ModeSmaller.dwYRes = 0;

    for (i = 0; lpModeList[i].dwXRes != 0; i++)
    {
        if  ((lpModeList[i].dwBpp == ModeIn.dwBpp) &&
             (lpModeList[i].dwXRes >= 640) &&
             (lpModeList[i].dwYRes >= 480) &&
             (lpModeList[i].dwXRes != 960) &&
             (((lpModeList[i].dwXRes <= ModeIn.dwXRes) && (lpModeList[i].dwYRes <  ModeIn.dwYRes)) ||
              ((lpModeList[i].dwXRes < ModeIn.dwXRes) && (lpModeList[i].dwYRes <=  ModeIn.dwYRes)) ||
              ((lpModeList[i].dwXRes * lpModeList[i].dwYRes) < (ModeIn.dwXRes * ModeIn.dwYRes))))
        {
            // The lpModeList resolution is smaller. But is it the
            // largest of the smaller modes? Remember we are looking
            // for the next immediate smaller mode.
            if  ((ModeSmaller.dwXRes < lpModeList[i].dwXRes) ||
                 ((ModeSmaller.dwXRes == lpModeList[i].dwXRes) &&
                  (ModeSmaller.dwYRes <  lpModeList[i].dwYRes)))
            {
                ModeSmaller.dwXRes = lpModeList[i].dwXRes;
                ModeSmaller.dwYRes = lpModeList[i].dwYRes;
                ModeSmaller.dwBpp  = lpModeList[i].dwBpp;
                ModeSmaller.dwRefreshRate  = lpModeList[i].dwRefreshRate;
            }
        }
    }

    if  (ModeSmaller.dwXRes != 0)
    {
        ModeSmaller.MIDevData.cType = lpModeDesc->MIDevData.cType;
        lpModeDesc[0] = ModeSmaller;
        return  (TRUE);
    }

    return  (FALSE);
}


/*
    FindNextSmallerRefreshRate

    Purpose:    This routine finds the next lower refresh rate at
                the same resolution and pixel depth as requested.
                If it finds one, it modifies lpModeDesc to reflect
                the new mode and returns TRUE, else it returns FALSE.

    Arguments:
                lpModeList  ptr to MODEENTRY array -- master mode list
                lpModeDesc  ptr to MODEINFO -- mode to back off

    Returns:    TRUE    lpModeDesc has new backed off mode
                FALSE   there was no smaller mode in the lpModeList

    Preserve:
*/

int CFUNC
FindNextSmallerRefreshRate (
    LPMODEENTRY lpModeList,
    LPMODEINFO  lpModeDesc)
{
    int         i;
    MODEINFO    ModeIn, ModeSmaller;

    // This should be easy. We look at the mode in lpModeDesc and
    // Find the next smaller spatial resolution mode in lpModeList
    // that is at the same pixel depth. Smaller mode is defined as:
    // A lower value for the X-Res is definitely smaller
    // A same value for the X-Res forces a look at the Y-Res
    //      A lower value for the Y-Res is smaller

    // Save the passed in mode because we are going to change it.
    ModeIn = lpModeDesc[0];

    // Set up the smaller mode to be 0 initially
    ModeSmaller.dwRefreshRate = 0;

    for (i = 0; lpModeList[i].dwXRes != 0; i++)
    {
        if  ((lpModeList[i].dwBpp == ModeIn.dwBpp) &&
             (lpModeList[i].dwXRes == ModeIn.dwXRes) &&
             (lpModeList[i].dwYRes == ModeIn.dwYRes) &&
             (lpModeList[i].dwRefreshRate < ModeIn.dwRefreshRate))
        {
            // The lpModeList refreshRate is smaller. But is it the
            // largest of the smaller modes? Remember we are looking
            // for the next immediate smaller mode.
            if  ((ModeSmaller.dwRefreshRate < lpModeList[i].dwRefreshRate))
            {
                ModeSmaller.dwXRes = lpModeList[i].dwXRes;
                ModeSmaller.dwYRes = lpModeList[i].dwYRes;
                ModeSmaller.dwBpp  = lpModeList[i].dwBpp;
                ModeSmaller.dwRefreshRate  = lpModeList[i].dwRefreshRate;
            }
        }
    }

    if  (ModeSmaller.dwRefreshRate != 0)
    {
        ModeSmaller.MIDevData.cType = lpModeDesc->MIDevData.cType;
        lpModeDesc[0] = ModeSmaller;
        return  (TRUE);
    }

    return  (FALSE);
}


/*
    FindNextLargerRefreshRate

    Purpose:    This routine finds the next higher refresh rate at
                the same resolution and pixel depth as requested.
                If it finds one, it modifies lpModeDesc to reflect
                the new mode and returns TRUE, else it returns FALSE.

    Arguments:
                lpModeList  ptr to MODEENTRY array -- master mode list
                lpModeDesc  ptr to MODEINFO -- mode to back off

    Returns:    TRUE    lpModeDesc has new backed off mode
                FALSE   there was no smaller mode in the lpModeList

    Preserve:
*/

int CFUNC
FindNextLargerRefreshRate (
    LPMODEENTRY lpModeList,
    LPMODEINFO  lpModeDesc)
{
    int         i;
    MODEINFO    ModeIn, ModeLarger;

    // This should be easy. We look at the mode in lpModeDesc and
    // Find the next larger spatial resolution mode in lpModeList
    // that is at the same pixel depth. Larger mode is defined as:
    // A lower value for the X-Res is definitely smaller
    // A same value for the X-Res forces a look at the Y-Res
    //      A lower value for the Y-Res is smaller

    // Save the passed in mode because we are going to change it.
    ModeIn = lpModeDesc[0];

    for (i = 0; lpModeList[i].dwXRes != 0; i++)
    {
        if  ((lpModeList[i].dwBpp == ModeIn.dwBpp) &&
             (lpModeList[i].dwXRes == ModeIn.dwXRes) &&
             (lpModeList[i].dwYRes == ModeIn.dwYRes) &&
             (lpModeList[i].dwRefreshRate > ModeIn.dwRefreshRate))
        {
            // The lpModeList refreshRate is larger.
            ModeLarger.dwXRes = lpModeList[i].dwXRes;
            ModeLarger.dwYRes = lpModeList[i].dwYRes;
            ModeLarger.dwBpp  = lpModeList[i].dwBpp;
            ModeLarger.dwRefreshRate  = lpModeList[i].dwRefreshRate;
            ModeLarger.MIDevData.cType = lpModeDesc->MIDevData.cType;
            lpModeDesc[0] = ModeLarger;
            return  (TRUE);
        }
    }

    return  (FALSE);
}


/*
    Function:   GetSafeTiming

    Purpose:    This routine returns a "safe" set of timings for
                a special default mode. At the time this was
                written, that was 640x480 @ 60Hz DMT timing.

    Arguments:
                lpModeOrig  ptr to MODEINFO -- requested mode
                lpModeOut   ptr to MODEOUT  -- place to store
                            timings for safe mode

    Returns:    This routine returns the difference betweeen the
                mode specified by lpModeDesc and the mode
                specified by lpModeOut as logical ORs of the
                flags in modeext.h.
                MODE_EXACT_MATCH
                MODE_RESOLUTION_ADJUSTED
                MODE_PIXEL_DEPTH_ADJUSTED
                MODE_REFRESH_RATE_ADJUSTED

    Preserve:
*/
int WINAPI
GetSafeTiming (LPMODEINFO lpModeOrig, LPMODEOUT lpModeOut)
{
    MODEINFO    ModeDesc;

    DBG_PRINT0(DL_0, "\r\nEntering GetSafeTiming.");

    ModeDesc.dwXRes = 640;
    ModeDesc.dwYRes = 480;
    ModeDesc.dwBpp  = lpModeOrig->dwBpp;
    ModeDesc.dwRefreshRate = 60;
    ModeDesc.MIDevData.cType = lpModeOrig->MIDevData.cType;
    ModeDesc.dwOriginalRefreshRate = lpModeOrig->dwOriginalRefreshRate;

    // Account for case where pixel depth is 24-bit which is not supported.
    if  (lpModeOrig->dwBpp == 24)
        ModeDesc.dwBpp  = 16;

    DmtFindMode (&ModeDesc, lpModeOut);

    return  (GetModeFlags (lpModeOrig, &ModeDesc));
}


/*
    Function:   GetModeFlags

    Purpose:    This routine computes the difference betweeen the
                mode specified by lpModeOrig and the mode
                specified by lpModeFound as logical ORs of the
                flags in modeext.h.

    Purpose:    This routine returns a "safe" set of timings for
                a special default mode. At the time this was
                written, that was 640x480 @ 60Hz DMT timing.

    Arguments:
                lpModeDesc  ptr to MODEINFO -- requested mode
                lpModeOut   ptr to MODEOUT  -- place to store
                            timings for safe mode

    Returns:    This routine returns the difference betweeen the
                mode specified by lpModeOrig and the mode
                specified by lpModeFound as logical ORs of the
                flags in modeext.h.
                MODE_EXACT_MATCH
                MODE_RESOLUTION_ADJUSTED
                MODE_PIXEL_DEPTH_ADJUSTED
                MODE_REFRESH_RATE_ADJUSTED

    Preserve:
*/
int CFUNC
GetModeFlags (LPMODEINFO lpModeOrig, LPMODEINFO lpModeFound)
{
    int     nRet;

    // Figure out how the input mode differs from the safe mode and
    // return flags indicating the differences.
    nRet = 0;

    if  ((lpModeOrig->dwXRes != lpModeFound->dwXRes) ||
         (lpModeOrig->dwYRes != lpModeFound->dwYRes))
        nRet |= MODE_RESOLUTION_ADJUSTED;

    if  (lpModeOrig->dwRefreshRate != lpModeFound->dwRefreshRate)
        nRet |= MODE_REFRESH_RATE_ADJUSTED;

    if  (lpModeOrig->dwBpp != lpModeFound->dwBpp)
        nRet |= MODE_PIXEL_DEPTH_ADJUSTED;

    return  (nRet);
}


/*
    FormatModeList

    This routine parses the passed in modelist into the desired
    mode structure format.

    The modes are returned as either MODEENTRY, MODEENTRYNORR, or
    MODEENTRYANDPITCH structures depending upon the value of dwType.

    If dwType is MODETYPE_MODEENTRY, then MODEENTRY structures are
    returned. If dwType is MODETYPE_MODEENTRYNORR, then MODEENTRYNORR
    structures are returned. If dwType is MODETYPE_MODEENTRYANDPITCH,
    then MODEENTRYANDPITCH structures are returned.
*/
int WINAPI
FormatModeList (LPDISPDATA lpDispData, LPCHAR lpModes, ULONG dwType, LPMODEENTRY lpME)
{
    int         j;
    ULONG       dwModeSize;

    switch  (dwType)
    {
        case MODETYPE_MODEENTRY:
        {
            LPMODEENTRY lpDst;
            lpDst = (LPMODEENTRY) lpModes;

            // Just copy the list if they provide a ptr
            // If not, at least count the items
            for (j = 0; lpME[j].dwXRes != 0; j++)
                if  (lpDst)
                    lpDst[j] = lpME[j];

            break;
        }

        case MODETYPE_MODEENTRYNORR:
        {
            // Only one mode per refresh rate

            int             k, l;
            int             nDuplicate;
            LPMODEENTRYNORR lpDst;

            lpDst = (LPMODEENTRYNORR) lpModes;
            j = 0;

            for (k = 0; lpME[k].dwXRes != 0; k++)
            {
                nDuplicate = FALSE;

                for (l = 0; l < j; l++)
                {
                    if  ((lpDst[l].dwXRes == lpME[k].dwXRes) &&
                         (lpDst[l].dwYRes == lpME[k].dwYRes) &&
                         (lpDst[l].dwBpp  == lpME[k].dwBpp))
                    {
                        nDuplicate = TRUE;
                        break;
                    }
                }

                if  (!nDuplicate)
                {
                    // Copy the entry if it is a valid ptr.
                    // Otherwise just count them.
                    if  (lpDst)
                    {
                        lpDst[j].dwXRes = lpME[k].dwXRes;
                        lpDst[j].dwYRes = lpME[k].dwYRes;
                        lpDst[j].dwBpp  = lpME[k].dwBpp;
                    }
                    j++;
                }
            }
            break;
        }

        case MODETYPE_MODEENTRYANDPITCH:
        {
            // We don't actually fill in the pitch here, but
            // we use the correct structure so that the caller can.
            // Only one mode per refresh rate

            int                 k, l;
            int                 nDuplicate;
            LPMODEENTRYANDPITCH lpDst;

            lpDst = (LPMODEENTRYANDPITCH) lpModes;
            j = 0;

            for (k = 0; lpME[k].dwXRes != 0; k++)
            {
                nDuplicate = FALSE;

                for (l = 0; l < j; l++)
                {
                    if  ((lpDst[l].dwXRes == lpME[k].dwXRes) &&
                         (lpDst[l].dwYRes == lpME[k].dwYRes) &&
                         (lpDst[l].dwBpp  == lpME[k].dwBpp))
                    {
                        nDuplicate = TRUE;
                        break;
                    }
                }

                if  (!nDuplicate)
                {
                    // Copy the entry if it is a valid ptr.
                    // Otherwise just count them.
                    if  (lpDst)
                    {
                        lpDst[j].dwXRes  = lpME[k].dwXRes;
                        lpDst[j].dwYRes  = lpME[k].dwYRes;
                        lpDst[j].dwBpp   = lpME[k].dwBpp;
                        GetPitchAndSize (lpDispData, lpME[k].dwXRes,
                                    lpME[k].dwYRes, lpME[k].dwBpp,
                                    &(lpDst[j].dwPitch), &dwModeSize);
                    }
                    j++;
                }
            }
            break;
        }
    }

    return  (j);
}


/*
    GetModeList

    This routine gets the master mode list and returns it.

    The modes are returned as either MODEENTRY, MODEENTRYNORR, or
    MODEENTRYANDPITCH structures depending upon the value of dwType.

    If dwType is MODETYPE_MODEENTRY, then MODEENTRY structures are
    returned. If dwType is MODETYPE_MODEENTRYNORR, then MODEENTRYNORR
    structures are returned. If dwType is MODETYPE_MODEENTRYANDPITCH,
    then MODEENTRYANDPITCH structures are returned.
*/
int WINAPI
GetModeList (LPDISPDATA lpDispData, LPCHAR lpModes, ULONG dwType)
{
    LPMODEENTRY lpME;
    int         j;

    lpME = GetMasterModeList (lpDispData);

    j = FormatModeList (lpDispData, lpModes, dwType, lpME);

    FreeMasterModeList (lpME);
    return  (j);
}

