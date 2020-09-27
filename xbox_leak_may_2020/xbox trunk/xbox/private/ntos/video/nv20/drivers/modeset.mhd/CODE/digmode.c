/*
    FILE:   digmode.c
    DATE:   4/8/99

    This file contains the code for mode setting/determination
    ona CRT or FLAT PANEL.
*/

#include "cmntypes.h"
#include "modeext.h"
#include "modeset.h"
#include "debug.h"
#include "restime.h"
#include "digmode.h"
#include "dmt.h"
#include "edid.h"
#include "gtf.h"
#include "gtfmath.h"
#include "osapi.h"

#define     BACKOFF_MODE        0x01
#define     GOT_TIMINGS         0x02
#define     RANGE_CHECK_MODE    0x04

/*==========================================================================
    Function:   FindDigModeEntry

    Purpose:    Finds a set of timings for a mode for a CRT or DFP
                that will work on the device.

    Arguments:
                lpEdidBuffer    ptr to an EDID (or NULL if no edid)
                dwEdidSize      length of edid in bytes (or 0 if no edid)
                lpDispData      ptr to DISPDATA struct descirbing board
                lpModeOrig      ptr to MODEINFO desribing requested mode
                lpModeOut       ptr to place to store timings for final
                                mode decided upon (not necessarily the
                                same mode as given by lpModeOrig)
                lpModeList      ptr to a MODEENTRY array -- master mode list
                lpRTList        ptr to a RESTIME array -- R&T list

    Returns:    some bitwise ORing of the flags in modeext.h
                MODE_EXACT_MATCH
                MODE_RESOLUTION_ADJUSTED
                MODE_PIXEL_DEPTH_ADJUSTED
                MODE_REFRESH_RATE_ADJUSTED

    Preserve:
===========================================================================*/
int CFUNC
FindDigModeEntry (
    LPCHAR      lpEdidBuffer,
    ULONG       dwEdidSize,
    LPDISPDATA  lpDispData,
    LPMODEINFO  lpModeOrig,
    LPMODEOUT   lpModeOut,
    LPMODEENTRY lpModeList,
    LPRESTIME   lpRTList)
{
    int             i, nFlag, nIndex, bDoneRT, iRRFlag;
    MODEINFO        ModeDesc, LimitMode, ModeDFP;
    RTMODE          sRTMode;
    ULONG           dwMonitorID, dwMemoryUsed, dwPitch, dwDirection;
    ULONG           dwDacSpeedInHertz, dwDacSpeedLimitInHertz;
    USHORT          wActFlags;
    ULONG           wAllowAllModes;

    // Put out a bit of useful logging information.
    DBG_PRINT0(DL_0, "\r\nEntering FindDigModeEntry.");

    // Make a copy so we can modify the mode as we go along without
    // changing the original requested mode.
    ModeDesc = lpModeOrig[0];

    wAllowAllModes = GetAllowAllModesFlag(lpDispData->lpBoardRegistryData);

    // Decrease refresh rate in BackOffDigModeEntry() by default.
    iRRFlag = 1;
    // Set positive direction
    dwDirection = 1;

  do
  {
    // We will be modifying this as we go along.
    wActFlags = RANGE_CHECK_MODE;

    /////////////////////////////////////////////////////////////////
    // First, see if this mode is in the master mode list
    DBG_PRINT0(DL_1, "\r\nSearching for Mode in Master Mode List.");
    DBG_PRINT1(DL_1, "\r\nXRes = %ld", ModeDesc.dwXRes);
    DBG_PRINT1(DL_1, "\r\nYRes = %ld", ModeDesc.dwYRes);
    DBG_PRINT1(DL_1, "\r\nBpp  = %ld", ModeDesc.dwBpp);
    DBG_PRINT1(DL_1, "\r\nRefR = %ld", ModeDesc.dwRefreshRate);
    if (!wAllowAllModes)
    {
        for (i = 0; lpModeList[i].dwXRes != 0; i++)
        {
            if  ((lpModeList[i].dwXRes == ModeDesc.dwXRes) &&
                 (lpModeList[i].dwYRes == ModeDesc.dwYRes) &&
                 (lpModeList[i].dwBpp  == ModeDesc.dwBpp)  &&
                 (lpModeList[i].dwRefreshRate  == ModeDesc.dwRefreshRate))
                break;
        }

        // Did we find the mode in the list?
        if  (lpModeList[i].dwXRes == 0)
        {
            DBG_PRINT0(DL_1, "\r\nFound Mode in Master List.");
            wActFlags |= BACKOFF_MODE;
        }
    }
    /////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////
    // NO EDID AND NO INF blocks certain modes
    if  (!(wAllowAllModes) && !(wActFlags & BACKOFF_MODE) && (dwEdidSize == 0) &&
          (lpDispData->dwMonitorInfType != SPECIFIC_DEVICE))
    {
        if  (ModeDesc.MIDevData.cType == DEVTYPE_CRT)
        {
            // If the CRT has no inf and no edid, then we
            // check the registry to see what the upper limit
            // should be in this situation.
            if  (GetMonitorLimitMode (lpDispData, &LimitMode))
            {
                if  ((ModeDesc.dwRefreshRate > LimitMode.dwRefreshRate) ||
                     (ModeDesc.dwXRes > LimitMode.dwXRes) ||
                     (ModeDesc.dwYRes > LimitMode.dwYRes))
                {
                    wActFlags |= BACKOFF_MODE;
                }
                else
                {
                    wActFlags &= ~RANGE_CHECK_MODE;
                }
            }
            else
            {
                wActFlags &= ~RANGE_CHECK_MODE;
            }
        }

        if  (ModeDesc.MIDevData.cType == DEVTYPE_DFP)
        {
            // If the DFP has no inf and no edid, then we block any
            // refreshrate higher than 60Hz and block all resolutions
            // higher than 1024x768.
            if  ((ModeDesc.dwRefreshRate > 60) ||
                 (ModeDesc.dwXRes > 1024) ||
                 (ModeDesc.dwYRes > 768))
            {
                wActFlags |= BACKOFF_MODE;
            }
            else
            {
                wActFlags &= ~RANGE_CHECK_MODE;
            }
        }
    }
    /////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////
    // NO EDID AND YES INF means trust the INF on everything.
    if  (!(wAllowAllModes) && !(wActFlags & BACKOFF_MODE) && (dwEdidSize == 0) &&
          (lpDispData->dwMonitorInfType == SPECIFIC_DEVICE))
    {
        // Nothing to do here right now.
        // We just assume that whatever we are asked to set
        // can be done because Windows wouldn't call us with
        // some mode that is not in the .inf.
    }
    /////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////
    // YES EDID AND NO INF means trust the EDID on everything.
    if  (!(wAllowAllModes) && !(wActFlags & BACKOFF_MODE) && (dwEdidSize != 0) &&
          (lpDispData->dwMonitorInfType != SPECIFIC_DEVICE))
    {
        // Nothing to do here right now.
    }
    /////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////
    // YES EDID AND YES INF case. Edid range limits have top priority
    // and, if they exist, ModeWithinMonitorTimingLimits will check
    // against those later. If there are no range limits in the EDID,
    // then we will use the range string from the .inf file.
    if  (!(wAllowAllModes) && !(wActFlags & BACKOFF_MODE) && (dwEdidSize != 0) &&
          (lpDispData->dwMonitorInfType == SPECIFIC_DEVICE))
    {
        // Note that there is nothing to do here right now.
    }
    /////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////
    // We need to block certain modes on DFPs.
    if  (!(wAllowAllModes) && !(wActFlags & BACKOFF_MODE) && (ModeDesc.MIDevData.cType == DEVTYPE_DFP))
    {
        // Block 480x360 and 960x720. DFPs often have trouble
        // with these modes.
        if  (((ModeDesc.dwXRes == 480) && (ModeDesc.dwYRes == 360)) ||
            ((ModeDesc.dwXRes == 960) && (ModeDesc.dwYRes == 720)))
            wActFlags |= BACKOFF_MODE;

        // HACK!!! We will fix this for real shortly, but this should
        // work for now.
        //
        // On DFPs, block any X,Y res that is bigger than the largest
        // mode in the EDID. Any mode that is smaller can still be
        // blocked by the pixel frequency range of the chip's DFP
        // scaler which is 8Mhz - 350Mhz.
        //
        // I'm not worried about any mode exceeding 350Mhz right now,
        // so I am going to omit the check for it. The check would
        // have to be put into ModeWithinMonitorTimingLimits() and
        // I'm trying to keep this hack localized.
        if  (dwEdidSize != 0)
        {
            EdidFindLargestEdidMode (lpEdidBuffer, dwEdidSize, &ModeDFP);
            if  ((ModeDesc.dwXRes > ModeDFP.dwXRes) ||
                 (ModeDesc.dwYRes > ModeDFP.dwYRes))
            {
                // Block modes that are larger than largest EDID mode.
                wActFlags |= BACKOFF_MODE;
            }
            else
            {
                // Let any smaller X,Y res go through. The refresh
                // rate is irrelevant right now, because the RM is
                // currently assuming fixed frequency DFPs at 60Hz.
                wActFlags &= ~RANGE_CHECK_MODE;
            }
        }
    }
    /////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////
    // We need to block certain modes according to restriction
    // and timing strings.

    if  (!(wAllowAllModes) && !(wActFlags & BACKOFF_MODE))
    {
        // See if we find a matching R&T string
        sRTMode.dwXRes = ModeDesc.dwXRes;
        sRTMode.dwYRes = ModeDesc.dwYRes;
        sRTMode.dwBpp  = ModeDesc.dwBpp;
        sRTMode.dwRefreshRate = ModeDesc.dwRefreshRate;

        if  (dwEdidSize)
            dwMonitorID = EdidGetMonitorId (lpEdidBuffer, dwEdidSize);
        else
            dwMonitorID = UKWN_ID;

        for (nIndex = 0, bDoneRT = FALSE; !bDoneRT;)
        {
            nIndex = RTFindNextMatchingString (lpRTList, nIndex + 1,
                        lpDispData, &sRTMode, dwMonitorID);

            if  (nIndex != RTERR_NOT_FOUND)
            {
                DBG_PRINT1(DL_0, "\r\nFound R&T string matching board data # %ld", (ULONG) nIndex);

                // There was. Let's see what that tells us.
                nFlag = ProcessRTWithDigModeEntry (lpDispData,
                                lpEdidBuffer, dwEdidSize,
                                lpRTList + nIndex, &ModeDesc, lpModeOut);

                // > 0 means that the R&T string accepted the mode and
                // we have a set of timings in lpModeOut. We're done!
                if  (nFlag > 0)
                {
                    wActFlags |= GOT_TIMINGS;
                    bDoneRT = TRUE;
                }

                if  (nFlag == 2)
                    wActFlags &= ~RANGE_CHECK_MODE;

                // < 0 means that this mode was explicitly rejected by
                // the R&T string -- probably a NONE in the Timing
                // Standard field. We need to back off the mode.
                if  (nFlag < 0)
                {
                    wActFlags |= BACKOFF_MODE;
                    bDoneRT = TRUE;
                }
            }
            else
                bDoneRT = TRUE;
        }
    }
    /////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////
    // At this point, check to see if we want to override any
    // timing standards that were already selected.
    if  (!(wActFlags & BACKOFF_MODE) && !(wActFlags & GOT_TIMINGS))
    {
        nFlag = CheckTimingStandardOverride (lpEdidBuffer, dwEdidSize,
                                    lpDispData, &ModeDesc, lpModeOut);
        if  (nFlag > 0)
        {
            wActFlags |= GOT_TIMINGS;
            DBG_PRINT0(DL_0, "\r\nOverride Timing Found!");
        }

        if  (nFlag == 2)
            wActFlags &= ~RANGE_CHECK_MODE;
    }
    /////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////
    // If an R&T string established the timing parameters, we
    // would not have gotten here. So at this point, just use
    // the "normal" logic to get the timing parameters.

    if  ((!(wActFlags & BACKOFF_MODE)) && !(wActFlags & GOT_TIMINGS) && (dwEdidSize == 0))
    {
        if  (DmtFindMode (&ModeDesc, lpModeOut))
        {
            wActFlags |= GOT_TIMINGS;
            wActFlags |= RANGE_CHECK_MODE;
            DBG_PRINT0(DL_0, "\r\nDmt timing found.");
        }

        if  (!(wActFlags & GOT_TIMINGS))
        {
            if  (GtfFindMode (&ModeDesc, lpModeOut))
            {
                wActFlags |= GOT_TIMINGS;
                wActFlags |= RANGE_CHECK_MODE;
                DBG_PRINT0(DL_0, "\r\nGtf timing found.");
            }
        }
    }
    /////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////
    // If an R&T string established the timing parameters, we
    // would not have gotten here. So at this point, just use
    // the "normal" logic to get the timing parameters.

    if  ((!(wActFlags & BACKOFF_MODE)) && (!(wActFlags & GOT_TIMINGS)) && (dwEdidSize != 0))
    {
        if  (EdidFindDetailedMode (lpEdidBuffer, dwEdidSize, &ModeDesc, lpModeOut))
        {
            DBG_PRINT0(DL_0, "\r\nEdidDetailedTiming Found!");
            wActFlags = (wActFlags & ~RANGE_CHECK_MODE) | GOT_TIMINGS;
        }
        else if  (EdidFindStandardMode (lpEdidBuffer, dwEdidSize, &ModeDesc, lpModeOut))
        {
            DBG_PRINT0(DL_0, "\r\nEdidStandardTiming Found!");
            wActFlags = (wActFlags & ~RANGE_CHECK_MODE) | GOT_TIMINGS;
        }
        else if  (EdidFindEstablishedMode (lpEdidBuffer, dwEdidSize, &ModeDesc, lpModeOut))
        {
            DBG_PRINT0(DL_0, "\r\nEdidEstablishedTiming Found!");
            wActFlags = (wActFlags & ~RANGE_CHECK_MODE) | GOT_TIMINGS;
        }
        else if  ((EdidIsGtfMonitor (lpEdidBuffer, dwEdidSize)) && GtfFindMode (&ModeDesc, lpModeOut)
                  && (ModeDesc.dwXRes > 400) && (ModeDesc.dwYRes > 300))
        {
            wActFlags |= GOT_TIMINGS;
            DBG_PRINT0(DL_0, "\r\nGtf timing found.");
        }
        else if  (DmtFindMode (&ModeDesc, lpModeOut))
        {
            wActFlags |= GOT_TIMINGS;
            DBG_PRINT0(DL_0, "\r\nDmt timing found.");
        }
        else if  (GtfFindMode (&ModeDesc, lpModeOut))
        {
            wActFlags |= GOT_TIMINGS;
            DBG_PRINT0(DL_0, "\r\nGtf timing found.");
        }
    }
    /////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////
    if  (!(wActFlags & BACKOFF_MODE) && (wActFlags & GOT_TIMINGS))
    {
        if (!wAllowAllModes)
        {
            // We weren't explicitly asked to back off and we have some
            // timings, so range check them if requested.
            if  (!(wActFlags & RANGE_CHECK_MODE) ||
                (iRRFlag = ModeWithinMonitorTimingLimits (lpDispData, lpModeOut, lpEdidBuffer, dwEdidSize)) == 0)
            {
                // Now verify that there is enough memory and that
                // the DAC is fast enough to handle the mode.
                GetPitchAndSize (lpDispData, ModeDesc.dwXRes,
                                    ModeDesc.dwYRes, ModeDesc.dwBpp,
                                    &dwPitch, &dwMemoryUsed);
                dwDacSpeedInHertz = Multiply32By32 (lpModeOut->dwPixelClock, 10000);

                // Get the dac speed limit for the given pixel depth
                if  (ModeDesc.dwBpp == 8)
                    dwDacSpeedLimitInHertz = lpDispData->dwMaxDacSpeedInHertz8bpp;

                if  ((ModeDesc.dwBpp == 16) || (ModeDesc.dwBpp == 15))
                    dwDacSpeedLimitInHertz = lpDispData->dwMaxDacSpeedInHertz16bpp;

                if  ((ModeDesc.dwBpp == 32) || (ModeDesc.dwBpp == 24))
                    dwDacSpeedLimitInHertz = lpDispData->dwMaxDacSpeedInHertz32bpp;

                if  ((lpDispData->dwVideoMemoryInBytes >= dwMemoryUsed) &&
                     (dwDacSpeedLimitInHertz >= dwDacSpeedInHertz))
                    return  (GetModeFlags (lpModeOrig, &ModeDesc));
                else
                {
                    iRRFlag = 1;
                    wActFlags |= BACKOFF_MODE;
                }
            }
            else
            {
                if(iRRFlag < 0)
                    dwDirection = 0;

                wActFlags |= BACKOFF_MODE;
            }
        }
        else
            return  (GetModeFlags (lpModeOrig, &ModeDesc));
    }

    if  ((wActFlags & BACKOFF_MODE) || !(wActFlags & GOT_TIMINGS))
    {
        // If we are directed to backoff explicitly, or we didn't
        // find any timings, then go ahead and backoff.
        if  ((dwDirection == 0 && iRRFlag > 0) ||
            !BackOffDigModeEntry (lpModeList, &ModeDesc, lpModeOrig, iRRFlag))
        {
            // Just return safe settings if we couldn't back off
            return  (GetSafeTiming (&ModeDesc, lpModeOut));
        }
    }
    /////////////////////////////////////////////////////////////

  } while (TRUE);

}


/*==========================================================================
    Function:   CheckTimingStandardOverride

    Purpose:    Checks to see if we are supposed to use a particular
                timing standard (like DMT or GTF). If we are, sees if
                there is a set of timings to match the requested mode.

    Arguments:
                lpEdidBuffer    ptr to an EDID (or NULL if no edid)
                dwEdidSize      length of edid in bytes (or 0 if no edid)
                lpDispData      ptr to DISPDATA struct descirbing board
                lpModeDesc      ptr to MODEINFO desribing requested mode
                lpModeOut       ptr to place to store timings for
                                lpModeDesc mode if needed.

    Returns:    > 0     then a timing standard override was requested
                        and lpModeOut has the set of timings for the
                        mode passed in in lpModeDesc
                        if 1 then range check mode
                        if 2 then dont range check mode
                0       Either no timing standard override was requested
                        or one was but there was no set of timings in
                        that timing standard for the mode given by
                        lpModeDesc

    Preserve:
===========================================================================*/
int CFUNC
CheckTimingStandardOverride (
    LPCHAR      lpEdidBuffer,
    ULONG       dwEdidSize,
    LPDISPDATA  lpDispData,
    LPMODEINFO  lpModeDesc,
    LPMODEOUT   lpModeOut)
{
    ULONG   dwTimingStandard;
    MODEOUT sModeOutTemp;
    int     nRet;

    dwTimingStandard = GetTimingStandardOverride (lpDispData);

    if  (dwTimingStandard == DMTV_ID)
    {
        if  (DmtFindMode (lpModeDesc, lpModeOut))
        {
            nRet = 1;

            if  (dwEdidSize)
            {
                if  (EdidFindStandardMode (lpEdidBuffer, dwEdidSize,lpModeDesc,lpModeOut))
                    nRet = 2;
                
                if  (EdidFindEstablishedMode (lpEdidBuffer, dwEdidSize, lpModeDesc, lpModeOut))
                    nRet = 2;
            }
    
            DBG_PRINT0(DL_0, "\r\nDmt Timing Found!");
            return  (nRet);
        }
    }

    if  (dwTimingStandard == GTFV_ID)
    {
        if  (GtfFindMode (lpModeDesc, lpModeOut))
        {
            nRet = 1;

            if  (dwEdidSize &&
				EdidFindStandardMode (lpEdidBuffer, dwEdidSize, lpModeDesc, &sModeOutTemp) &&
                !DmtFindMode (lpModeDesc, &sModeOutTemp))
                    nRet = 2;

            DBG_PRINT0(DL_0, "\r\nGtf Timing Found!");
            return  (nRet);
        }
    }

    return  (FALSE);
}



/*==========================================================================
    Functiom:   ProcessRTWithDigModeEntry

    Purpose:    Checks to see if we are supposed to use a particular
                timing standard (like DMT or GTF). If we are, sees if
                there is a set of timings to match the requested mode.

    Arguments:
                lpEdidBuffer    ptr to an EDID (or NULL if no edid)
                dwEdidSize      length of edid in bytes (or 0 if no edid)
                lpRTList        ptr to RESTIME structure we are processing
                lpModeDesc      ptr to MODEINFO desribing requested mode
                lpModeOut       ptr to place to store timings for
                                lpModeDesc mode if needed.

    Returns:
        There are three return values possible.
        > 0     This indicates that the mode was accepted by the R&T
                string and a set of timings have been placed in
                lpModeOut.
                If 2 then mode must not be range checked
                if 1, then mode should be range checked
        = 0     This R&T string did not apply to the lpModeDesc.
        < 0     The lpModeDesc mode was rejected by the R&T string.

    Preserve:
===========================================================================*/
int CFUNC
ProcessRTWithDigModeEntry (
            LPDISPDATA  lpDispData,
            LPCHAR      lpEdidBuffer,
            ULONG       dwEdidSize,
            LPRESTIME   lpRTList,
            LPMODEINFO  lpModeDesc,
            LPMODEOUT   lpModeOut)
{
    int i;

    // R&T string applies to this mode.
    // Let's walk through the timing standards.
    for (i = 0; i < MAX_DOT_FIELDS; i++)
    {
        switch (lpRTList->dwTimingStandard[i])
        {
            case NONE_ID:
                // This mode is prohibited. Fail it.
                DBG_PRINT0(DL_0, "\r\nR&T string specifies NONE timing.");
                return  (-1);

            case EDID_ID:
                DBG_PRINT0(DL_0, "\r\nR&T string specifies EDID timing.");

                if  (dwEdidSize)
                {
                    // Use an EDID detailed timing if present
                    if  (EdidFindDetailedMode (lpEdidBuffer,
                            dwEdidSize, lpModeDesc, lpModeOut))
                    {
                        DBG_PRINT0(DL_0, "\r\nEdidDetailedTiming Found!");
                        return  (1);
                    }

                    if  (EdidFindStandardMode (lpEdidBuffer,
                            dwEdidSize, lpModeDesc, lpModeOut))
                    {
                        DBG_PRINT0(DL_0, "\r\nEdidStandardTiming Found!");
                        return  (1);
                    }

                    if  (EdidFindEstablishedMode (lpEdidBuffer,
                            dwEdidSize, lpModeDesc, lpModeOut))
                    {
                        DBG_PRINT0(DL_0, "\r\nEdidEstablishedTiming Found!");
                        return  (1);
                    }
                }
                break;

            case DMTV_ID:
            case DMTR_ID:
                // Use a DMT timing if present
                DBG_PRINT0(DL_0, "\r\nR&T string specifies DMT timing.");

                if  (DmtFindMode (lpModeDesc, lpModeOut))
                {
                    if  (lpRTList->dwTimingStandard[i] == DMTR_ID)
                    {
                        // If there is an EDID, then make sure we
                        // are within the monitor limits
                        if  (!ModeWithinMonitorTimingLimits (lpDispData,
                                            lpModeOut, lpEdidBuffer,
                                            dwEdidSize))
                        {
                            DBG_PRINT0(DL_0, "\r\nDmt Timing rejected by monitor limits!");
                            break;
                        }
                    }

                    DBG_PRINT0(DL_0, "\r\nDmt Timing Found!");
                    if  (lpRTList->dwTimingStandard[i] == DMTR_ID)
                        return  (1);
                    else
                        return  (2);
                }
                break;

            case GTFV_ID:
            case GTFR_ID:
                // Use GTF unless we couldn't get an
                // Edid or unless the Edid indicated
                // the monitor was not GTF.
                DBG_PRINT0(DL_0, "\r\nR&T string specifies GTF timing.");

                if  (!dwEdidSize)
                {
                    DBG_PRINT0(DL_0, "\r\nNo Edid: Cannot use GTFV timing.");
                    break;
                }

                if  (!EdidIsGtfMonitor (lpEdidBuffer, dwEdidSize))
                {
                    DBG_PRINT0(DL_0, "\r\nNot GTF monitor: Cannot use GTFV timing.");
                    break;
                }

            // Fall through to FGTF case
            case FGTF_ID:
            case FGTR_ID:
                // Use a GTF timing regardless
                DBG_PRINT0(DL_0, "\r\nR&T string specifies FGTF timing.");

                if  (GtfFindMode (lpModeDesc, lpModeOut))
                {
                    if  ((lpRTList->dwTimingStandard[i] == GTFR_ID) ||
                         (lpRTList->dwTimingStandard[i] == FGTR_ID))
                    {
                        // If there is an EDID, then make sure we
                        // are within the monitor limits
                        if  (!ModeWithinMonitorTimingLimits (lpDispData,
                                            lpModeOut, lpEdidBuffer,
                                            dwEdidSize))
                        {
                            DBG_PRINT0(DL_0, "\r\nGtf Timing rejected by monitor limits!");
                            break;
                        }
                    }

                    // If there is no edid, or there are no
                    // monitor limits, or the GTF mode is
                    // within the monitor limits, then we
                    // have found the mode and are done!
                    DBG_PRINT0(DL_0, "\r\nGtf Timing found!");
                    if  ((lpRTList->dwTimingStandard[i] == FGTR_ID) ||
                         (lpRTList->dwTimingStandard[i] == GTFR_ID))
                        return  (1);
                    else
                        return  (2);
                }
                break;

            case OEMX_ID:
            case OEMR_ID:
                // Use the OEM timing in the TimingParameters
                DBG_PRINT0(DL_0, "\r\nR&T string specifies OEM timing.");

                lpModeOut[0] = lpRTList->sModeOut;

                if  (lpRTList->dwTimingStandard[i] == OEMR_ID) 
                {
                    // If there is an EDID, then make sure we
                    // are within the monitor limits
                    if  (!ModeWithinMonitorTimingLimits (lpDispData,
                                        lpModeOut, lpEdidBuffer,
                                        dwEdidSize))
                    {
                        DBG_PRINT0(DL_0, "\r\nOem Timing rejected by monitor limits!");
                        break;
                    }
                }

                DBG_PRINT0(DL_0, "\r\nOem Timing Found!");
                if  (lpRTList->dwTimingStandard[i] == OEMR_ID)                
                    return  (1);
                else
                    return  (2);

            default:
                // Bad Timing Standard Encountered
                DBG_PRINT0(DL_0, "\r\nBad Timing Standard. Ignoring it.");
        }
    }

    return  (0);
}


/*==========================================================================
;
; Function: BackOffDigModeEntry
;
; Purpose:  This routine backs off the current mode, first by the
;           refresh rate. If it can't do that, then it restores the
;           original refresh rate and backs off on the spatial
;           resolution. If it can't do that, then it returns a failure
;           code.
;
; Arguments:
;       lpModeList  LPMODEENTRY ptr points to mode list used to
;                   fetch next lower refresh rate/resolution from
;       lpModeDesc  Description of mode from which we should back off.
;       lpModeOrig  This was the original mode that the user called
;                   the modeset DLL with originally. It can be used
;                   in the back off algorithm as well. For example
;                   when the refresh rate cannot be backed off
;                   anymore, it may be desirable to restore the
;                   original refresh rate the user requested and
;                   then back off the reolution one notch.
;
; Returns:      0   this indicates that there was no mode that could
;                   be backed off to. We are at the end of our rope.
;               1   then lpModeDesc has been modified to hold the new
;                   backed off mode.
;
; Preserve:     All registers
;==========================================================================*/
int CFUNC
BackOffDigModeEntry (
        LPMODEENTRY lpModeList,
        LPMODEINFO  lpModeDesc,
        LPMODEINFO  lpModeOrig,
    int         iRRFlag)
{
    if(iRRFlag < 0 && FindNextLargerRefreshRate (lpModeList, lpModeDesc))
    {
    DBG_PRINT0(DL_0, "\r\nBacking off to mode with a larger refresh rate.");
    }
    else if (iRRFlag > 0 && FindNextSmallerRefreshRate (lpModeList, lpModeDesc))
    {
    DBG_PRINT0(DL_0, "\r\nBacking off to mode with a lower refresh rate.");
    }
    else
    {
        // We couldn't find a the same resolution and pixel depth
        // with a lower refresh rate. This may happen, for example,
        // if we were already at 60Hz. Let's reset the refresh rate
        // to what was originally requested and find the next lower
        // spatial resolution resolution at the same pixel depth.
        lpModeDesc[0].dwRefreshRate = lpModeOrig[0].dwRefreshRate;

        if  (FindNextSmallerResolution (lpModeList, lpModeDesc))
        {
            DBG_PRINT0(DL_0, "\r\nBacking off to mode with a lower resolution.");
        }
        else
        {
            // OK, if this doesn't happen, then we had already
            // scanned down from the mode/refresh rate requested
            // through all all lower resolutions/refresh rates and
            // none of them were settable.
            // Inidicate that there are no more modes to try.
            return  (FALSE);
        }
    }

    return  (TRUE);
}


/*==========================================================================
    Function:   ModeWithinMonitorTimingLimits

    Purpose:    Checks to see if a given mode is within the timing
                limits for the monitor.

    Arguments:
                lpEdidBuffer    ptr to an EDID (or NULL if no edid)
                dwEdidSize      length of edid in bytes (or 0 if no edid)
                lpModeOut       timings to check

    Returns:    0   mode is within monitor limits
                #0   mode is not within limits
        ( > 0 - try to decrease refresh rate first )

    Preserve:
===========================================================================*/
int CFUNC
ModeWithinMonitorTimingLimits (
                LPDISPDATA  lpDispData,
                LPMODEOUT lpModeOut,
                LPCHAR    lpEdidBuffer,
                ULONG     dwEdidSize)
{
    ULONG           dwPixelClockInHertz;
    ULONG           dwHorzRate, dwMaxXRes, dwMaxYRes;
    USHORT          wGotRangeLimits;
    MONITORLIMITS   sML;

    // Assume that we don't find any range limits
    wGotRangeLimits = FALSE;

    if  (dwEdidSize == 0)
    {
        // No EDID ... if there is no INF, just say YES
        if  (lpDispData->dwMonitorInfType != SPECIFIC_DEVICE)
            return  (0);

        // Does the inf file have range limits?
        if  (GetMonitorInfRangeLimits (lpDispData, &sML, &dwMaxXRes, &dwMaxYRes))
            wGotRangeLimits = TRUE;
        else
            return  (0);
    }
    else
    {
        // There is an EDID ... Are there range limits?
        if  (EdidGetMonitorLimits (lpEdidBuffer, dwEdidSize, &sML))
        {
            // There are range limits, so don't use the .inf file
            wGotRangeLimits = TRUE;
        }
        else
        {
            // There are no range limits ... is there an .inf file?
            if  (lpDispData->dwMonitorInfType == SPECIFIC_DEVICE)
            {
                // Does the inf file have range limits?
                if  (GetMonitorInfRangeLimits (lpDispData, &sML, &dwMaxXRes, &dwMaxYRes))
                    wGotRangeLimits = TRUE;
                else
                    wGotRangeLimits = FALSE;
            }
        }
    }

    if  (!wGotRangeLimits)
    {
        // We didn't get range limits, but we wouldn't have gotten here,
        // if there had not been an EDID. SO, we must use the modes given
        // in the EDID to determine approximate range limits. We need to
        // find the standard, established or detailed mode that has the:
        // 1) largest pixel clock
        // 2) smallest pixel clock
        // 3) largest horizontal refresh rate
        // 4) smallest horizontal refresh rate
        // 5) largest vertical refresh rate
        // 6) smallest vertical refresh rate
        //
        // The particular mode in the EDID that gives each
        // of these values may be a different one.

        EdidBuildRangeLimits (lpEdidBuffer, dwEdidSize, &sML);
    }

    // First check the pixel clock
    dwPixelClockInHertz = Multiply32By32 (lpModeOut->dwPixelClock, 10000);
    if  (dwPixelClockInHertz > sML.dwMaxPixelClock)
        return  1;

    // If the maximum vertical is not zero, check the vertical
    // refresh rate directly. If the maximum vertical refresh rate
    // is zero, there are no limits so skip check.
    if (sML.dwMaxVert)
    {
        // If the minimum vertical is the same as the maximum vertical,
        // only check against the maximum vertical.
        if (sML.dwMinVert == sML.dwMaxVert)
        {
            if (lpModeOut->wRefreshRate > sML.dwMaxVert)
                return 1;
        }
        else
        {
            if  (lpModeOut->wRefreshRate < sML.dwMinVert)
        return -1;
            if  (lpModeOut->wRefreshRate > sML.dwMaxVert)
        return 1;
        }
    }

    // The horizontal refresh rate is the scan rate per frame.
    dwHorzRate = Divide32By32 (dwPixelClockInHertz + ((ULONG)lpModeOut->wHTotal / 2), (ULONG)lpModeOut->wHTotal);

    // Use integer part only
    dwHorzRate = Divide32By32(dwHorzRate , 1000) ;
    dwHorzRate = Multiply32By32(dwHorzRate , 1000) ;

    // If the maximum horizontal is not zero, check the horizontal
    // refresh rate directly. If the maximum horizontal refresh rate
    // is zero, there are no limits so skip check.
    if (sML.dwMaxHorz)
    {
        if (sML.dwMinHorz == sML.dwMaxHorz)
        {
            if  (dwHorzRate > sML.dwMaxHorz)
                return 1;
        }
        else
        {
            if  (dwHorzRate < sML.dwMinHorz)
                return -1;
        if  (dwHorzRate > sML.dwMaxHorz)
                return 1;
        }
    }

    return  0;
}

