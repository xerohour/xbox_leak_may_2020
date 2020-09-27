/*
    FILE:   tvmode.c
    DATE:   4/8/99

    This file contains the high level logic for the tv modeset code.
*/

#include "cmntypes.h"
#include "modeext.h"
#include "modeset.h"
#include "debug.h"
#include "restime.h"
#include "tvmode.h"
#include "dmt.h"
#include "gtf.h"

extern int CFUNC IsTvModeAccepted (LPDISPDATA lpDispData,
                                   LPMODEINFO lpModeDesc, LPRESTIME lpRTList);

extern int CFUNC SetupParamsForVidLutClass (LPMODESTRUC         lpMS,
                                            LPMODEOUT           lpModeOut,
                                            LPMODEINFO          lpModeIn);

/*
    FindTVModeEntry

    Timing info for NTSC and PAL modes is always DMT. The thing
    to remember here is that both are restricted to be 800x600
    or less.
*/

int CFUNC
FindTVModeEntry (
    LPDISPDATA  lpDispData,
    LPMODEINFO  lpModeDesc,
    LPMODEOUT   lpModeOut,
    LPMODEENTRY lpModeList,
    LPRESTIME   lpRTList)
{
    int         nRet, nIsNtsc;
    ULONG       MaxXRes;
    ULONG       MaxYRes;
    MODEINFO    ModeDesc;

    // Copy to a local
    MaxXRes = (lpDispData->dwMaxTVOutResolution >> 16) & 0xffff;
    MaxYRes = lpDispData->dwMaxTVOutResolution & 0xffff;
    ModeDesc = lpModeDesc[0];

    // Since this is a TV, which is a fixed frequency device, force the
    // refresh rate to appropriate refresh rate.
    if  ((lpModeDesc->MIDevData.cFormat == TVTYPE_NTSCM) ||
         (lpModeDesc->MIDevData.cFormat == TVTYPE_NTSCJ))
        nIsNtsc = TRUE;
    else
        nIsNtsc = FALSE;


    if  (nIsNtsc)
    {
        ModeDesc.dwRefreshRate = 60;
    }
    else
    {
        // 60 Hz works, 50 Hz doesn't (on Bt869), don't know why
        ModeDesc.dwRefreshRate = 60;
    }

    // Set this up
    nRet = MODE_EXACT_MATCH;

    // If the input resolution is greater than 800x600,
    // then drop it to 800x600.
    if  ((ModeDesc.dwXRes > MaxXRes) || (ModeDesc.dwYRes > MaxYRes))
    {
        ModeDesc.dwXRes = MaxXRes;
        ModeDesc.dwYRes = MaxYRes;
        nRet |= MODE_RESOLUTION_ADJUSTED;
    }

    do
    {
        // Start with the current mode. See if we can set this mode.
        // To do this, we process the RTList and see if this mode is
        // allowed or restricted in some way. At the time of this
        // writing the only way to restrict a TV mode is if there is
        // a NONE in the TimingStandard field of an R&T string. Then
        // the mode is rejected. Let's see if this happens with the
        // requested mode.
        if  (IsTvModeAccepted (lpDispData, &ModeDesc, lpRTList))
        {
            // Great! Return appropriately.
            if  (!DmtFindMode (&ModeDesc, lpModeOut))
            {
                // Bt 868/9
                MODESTRUC ntscModeBt = {720,512,60,870,720,770,820,870,525,512,518,520,525,2738,
                                        BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_NEGATIVE};
                MODESTRUC palModeBt = {720,544,60,840,720,770,820,840,550,544,546,548,550,2775,
                                       BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_NEGATIVE};
                // Chrontel 7007/8
                MODESTRUC ntscModeChrontel = {720,576,60,880,720,760,840,880,597,576,585,588,597,3150,
                                              BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_NEGATIVE};
                MODESTRUC palModeChrontel = {720,576,60,880,720,760,840,880,597,576,585,588,597,3150,
                                             BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_NEGATIVE};

                MODESTRUC *pNtscMode, *pPalMode, *pMode;
                switch (lpDispData->dwTVEncoderType & 0xFF00) {
                case TV_ENCODER_BROOKTREE:
                    pNtscMode = &ntscModeBt;
                    pPalMode = &palModeBt;
                    break;
                case TV_ENCODER_CHRONTEL:
                default:
                    pNtscMode = &ntscModeChrontel;
                    pPalMode = &palModeChrontel;
                    break;
                }

                pMode = nIsNtsc ? pNtscMode : pPalMode;

                // safe mode is 720x480 on NTSC, or 720x576 on PAL
                SetupParamsForVidLutClass (pMode, lpModeOut, lpModeDesc);
	        if(lpModeOut->wXRes != lpModeDesc->dwXRes)
	            nRet |= MODE_RESOLUTION_ADJUSTED;
            }
            return  (nRet);
        }

        // The TV mode wasn't accepted. Go to the registry mode
        // list and get the next lower resolution mode at the
        // pixel depth requested. We shall try that for the TV.
        nRet |= MODE_RESOLUTION_ADJUSTED;

        if  (!FindNextSmallerResolution (lpModeList, &ModeDesc))
        {
            // OK, if this doesn't happen, then we are already at
            // the losest mode available. Use a safe setting.
            DBG_PRINT0(DL_0, "\r\nNo lower resolution found.");
            DBG_PRINT0 (DL_0, "\r\nFailing TV Timing Standards. Use Safe Settings.");
            nRet = GetSafeTiming (lpModeDesc, lpModeOut);
            return  (nRet);
        }
    }
    while (TRUE);
}


/*
    IsTvModeAccepted

    This routine determines whether the mode in lpModeDesc is
    a valid TV mode (PAL or NTSC as determined by lpModeDesc->dwDevType.)
*/
int CFUNC
IsTvModeAccepted (
    LPDISPDATA  lpDispData,
    LPMODEINFO  lpModeDesc,
    LPRESTIME   lpRTList)
{
    int     nIndex, nIsNtsc;
    RTMODE  sRTMode;
    ULONG   dwMonitorID;

    // Start with the first restriction string
    nIndex = 1;

    if  ((lpModeDesc->MIDevData.cFormat == TVTYPE_NTSCM) ||
         (lpModeDesc->MIDevData.cFormat == TVTYPE_NTSCJ))
        nIsNtsc = TRUE;
    else
        nIsNtsc = FALSE;

    do
    {
        // There is an R&T string for this graphics board.
        // Does this string also match the mode requested?
        sRTMode.dwXRes = lpModeDesc->dwXRes;
        sRTMode.dwYRes = lpModeDesc->dwYRes;
        sRTMode.dwBpp  = lpModeDesc->dwBpp;
        sRTMode.dwRefreshRate = lpModeDesc->dwRefreshRate;


	// Reject the modes 720x576 & 720x480 for unsupported encoders
	if  ((lpDispData->dwTVEncoderType & 0xFF00) != TV_ENCODER_BROOKTREE &&
         (lpDispData->dwTVEncoderType & 0xFF00) != TV_ENCODER_CHRONTEL &&
	      sRTMode.dwXRes == 720)
		return  (FALSE);

        if  (nIsNtsc)
	{
            dwMonitorID = NTSC_ID;

	    // Reject 720x576 for NTSC
//	    if(sRTMode.dwXRes == 720 && sRTMode.dwYRes == 576)
//		return  (FALSE);
	}
        else
	{
            dwMonitorID = PALX_ID;

	    // Reject 720x480 for PAL
//	    if(sRTMode.dwXRes == 720 && sRTMode.dwYRes == 480)
//		return  (FALSE);
	}

        // Find a restriction string that matches the board data.
        nIndex = RTFindNextMatchingString (lpRTList, nIndex,
                    lpDispData, &sRTMode, dwMonitorID);

        if  (nIndex == RTERR_NOT_FOUND)
        {
            // If there were no R&T strings found for the graphics board
            // installed, then the mode is OK.
            return  (TRUE);
        }

        // See if there is a NONE entry in the TimingStandard
        // Field. If there isn't then the mode is OK
        if  (RTRestrictByTimingStandard (lpRTList, nIndex, NONE_ID))
            return  (FALSE);

        // This R&T string did not prohibit this mode. However,
        // since there might be another R&T string that restricts
        // NTSC or PAL, we continue with the rest of the RTList.
        nIndex++;
    }
    while   (nIndex <= (int) lpRTList[0].dwXRes);

    return TRUE;
}

