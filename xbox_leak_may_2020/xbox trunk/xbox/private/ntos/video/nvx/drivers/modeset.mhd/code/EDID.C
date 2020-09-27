/*
    FILE:   edid.c
    DATE:   4/8/99

    This file is the generic entry point for the edid modeset code.
*/

#include "cmntypes.h"
#include "modeext.h"
#include "modeset.h"
#include "debug.h"
#include "edid.h"
#include "dmt.h"
#include "gtf.h"
#include "gtfmath.h"
#include "restime.h"
#include "utils.h"

//*****************************************************************************
//
// EDID Established Timings
//
// These are the timings from the VESA Monitor Timing Specification
// (Version 1.0, Revision 0.8) which correspond to the modes in the
// Established Timings block of the EDID.
//
// Horizontal Visible, Vertical Visible
// Refresh (Hz * 100)
// Horizontal Total, Blank Start, Sync Start, Sync End, Blank End
// Vertical Total, Blank Start, Sync Start, Sync End, Blank End
// DotClock (MHz * 100)
// HSync polarity
// VSync polarity
//
// NOTE: GTF Timings dictate no borders, so Blank Start == Visible and
// Blank End == Total.
//

MODESTRUC sZeroMode[] =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

MODESTRUC EDIDEstablishedTimingTbl[] =
{
    // Settings for 1280x1024:
    1280,1024,75, 1688,1280,1296,1440,1688,1066,1024,1025,1028,1066,13500,BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,

    // Settings for 1024x768:
    1024,768,75, 1312,1024,1040,1136,1312,800,768,769,772,800,7875,BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,
    1024,768,70, 1328,1024,1048,1184,1328,806,768,771,777,806,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_NEGATIVE,
    1024,768,60, 1344,1024,1048,1184,1344,806,768,771,777,806,6500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_NEGATIVE,

    // Settings for 800x600:
    800,600,75, 1056,800,816,896,1056,625,600,601,604,625,4950,BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,
    800,600,72, 1040,800,856,976,1040,666,600,637,643,666,5000,BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,
    800,600,60, 1056,800,840,968,1056,628,600,601,605,628,4000,BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,
    800,600,56, 1024,800,824,896,1040,625,600,601,603,625,3600,BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,

    // Settings for 640x480:
    640,480,75, 840,640,656,720,840,500,480,481,484,500,3150,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_NEGATIVE,
    640,480,72, 832,648,664,704,824,520,488,489,492,512,3150,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_NEGATIVE,

    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

MODESTRUC EDIDDetailedTimingTable[9] =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

MODESTRUC EDIDEstablishedTimingTable[11] =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

MODESTRUC EDIDStandardTimingTable[9] =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};



/*
    Function:   EdidGetVersion

    Purpose:    Returns EDID version multiplied by 0x100 so 1.10 is 0x110.

    Arguments:  NONE

    Returns:    EDID version multiplied by 100 if one exists.
                FALSE if no valid EDID

    Preserve:
*/
int CFUNC
EdidGetVersion (LPCHAR lpEdidBuffer, ULONG dwEdidSize)
{
    int     nEdidVersion;

    // Edid Version is 1.x if the very first byte is a 0.
    if (lpEdidBuffer[0] == 0x00)
    {
        LPEDIDV1STRUC   lpV1;

        lpV1 = (LPEDIDV1STRUC) lpEdidBuffer;

        // For Version 1.x, first 8 bytes of EDID must be
        // 00h, FFh, FFh, FFh, FFh, FFh, FFh, 00h
        if  ((lpEdidBuffer[1] != 0x0FF) || (lpEdidBuffer[2] != 0x0FF) ||
             (lpEdidBuffer[3] != 0x0FF) || (lpEdidBuffer[4] != 0x0FF) ||
             (lpEdidBuffer[5] != 0x0FF) || (lpEdidBuffer[6] != 0x0FF) ||
             (lpEdidBuffer[7] != 0x000))
        {
            DBG_PRINT0(DL_1, "\r\nBad Edid Version 1.X");
            return  (FALSE);
        }

        nEdidVersion = (int) ((((USHORT) lpV1->bVersionNumber) << 8) +
                       ((USHORT) lpV1->bRevisionNumber));
    }
    else
    {
        if ((lpEdidBuffer[0] & 0xF0) == 0x20)     // Version 2.x?
        {
            nEdidVersion = (int) ((USHORT) lpEdidBuffer[0] << 4);
        }
        else                                    // Invalid Version number
        {
            DBG_PRINT0(DL_1, "\r\nBad Edid Version 2.X");
            return  (FALSE);
        }
    }

    DBG_PRINT1(DL_1, "\r\nEdid Version %x", nEdidVersion);
    return (nEdidVersion);
}


/*
    Function:   EdidFindNumTimingCodes

    Purpose:    Finds the number of luminance tables, size of luminance
                table, number of frequency ranges, number of detailed
                range limits, number of timing codes, and number of
                detailed timings. This is only valid for Version 2.x
                EDIDs.

    Arguments:  lpEdidBuffer    EDID data
                dwEdidSize      length of EDID
                lpTC            Ptr to array for Timing Code return values.

    Returns:    TRUE    timing codes were located and lpTC was fiiled in
                FALSE   no timing codes were found

    Preserve:
*/

int CFUNC
EdidFindNumTimingCodes (
    LPCHAR          lpEdidBuffer,
    ULONG           dwEdidSize,
    LPTIMINGCODES   lpTC)
{
    ULONG           dwInfo;
    USHORT          wLuminanceInfo;
    LPEDIDV2STRUC   lpV2 = (LPEDIDV2STRUC) lpEdidBuffer;

    dwInfo = (ULONG) lpV2->wTimingInfoMap;

    // Calculate Luminance Table. There is one table if bit 5 is set.
    if (dwInfo & 0x20)
    {
        lpTC->dwNumLuminanceTables = 1;

        // Let n = EdidBuffer[0x80] bits [4:0]
        // if EdidBuffer[0x80] bit 7 = 0
        //    then the size of the luminance table is n + 1
        // if EdidBuffer[0x80] bit 7 = 1
        //    then the size of the luminance table is 3n + 1
        wLuminanceInfo = (USHORT) lpV2->bTableDescriptors[0];
        if (wLuminanceInfo & 0x80)
            lpTC->dwSizeLuminanceTables =
                        (ULONG) (3 * (wLuminanceInfo & 0x1F) + 1);
        else
            lpTC->dwSizeLuminanceTables = (ULONG) (wLuminanceInfo + 1);
    }
    else
    {
        lpTC->dwNumLuminanceTables  = 0;
        lpTC->dwSizeLuminanceTables = 0;
    }

    lpTC->dwNumFreqRanges      = (dwInfo  >> 2) & 7;
    lpTC->dwNumRangeLimits     = dwInfo & 0x03;
    lpTC->dwNumTimingCodes     = (dwInfo >> (8 + 3)) & 0x1F;
    lpTC->dwNumDetailedTimings = (dwInfo >> 8) & 0x07;

    return  (TRUE);
}


/*
    Function:   EdidGetMonitorLimits

    Purpose:    Find the limits of operation by searching for a Monitor
                Descriptor Block with a Data Tag of FDh (Monitor Range
                Limits).

    Arguments:  EdidVersion     Version of EDID block.
                MonitorLimits   Ptr to array of monitor limits.

    Returns:    TRUE    Monitor Limits
                FALSE   No Monitor Limits

    Preserve:
*/

int CFUNC
EdidGetMonitorLimits (
    LPCHAR          lpEdidBuffer,
    ULONG           dwEdidSize,
    LPMONITORLIMITS lpML)
{
    int             i;
    int             nEdidVersion;
    TIMINGCODES     TimingCodes;
    ULONG           dwMinFrame, dwMaxFrame;
    ULONG           dwMinLine, dwMaxLine;
    ULONG           dwMinClock, dwMaxClock;

    nEdidVersion = EdidGetVersion (lpEdidBuffer, dwEdidSize);

    // If we couldn't figure out what version it was, then say that
    // there are no monitor range limits.
    if  (!nEdidVersion)
        return  (FALSE);

    if  (nEdidVersion < 0x200)
    {
        LPEDIDV1STRUC       lpV1 = (LPEDIDV1STRUC) lpEdidBuffer;
        LPMONITORDESCRITOR  lpMD = (LPMONITORDESCRITOR) lpV1->DetailedTimingDesc;

        // Start all the mins high and the maxs low, so they
        // will get changed when we find something more restrictive.
        lpML->dwMinHorz = 0x00;
        lpML->dwMaxHorz = 0x0FFFFFFFF;
        lpML->dwMinVert = 0x00;
        lpML->dwMaxVert = 0x0FFFFFFFF;

        // Start out with the maximum pixel clock being the largest
        // pixel clock found in all EDID timings.
        lpML->dwMaxPixelClock = EdidFindLargestPixelClock (lpEdidBuffer, dwEdidSize);
        lpML->dwMaxPixelClock = Multiply32By32 (lpML->dwMaxPixelClock, 10000);

        // There are 4 detailed timing descriptors in a version 1.X edid.
        // The detailed timing descriptor blocks do double duty as a
        // monitor range descriptor block when the first three bytes are 0,
        // and the fourth byte is FDh.
        for (i = 0; i < 4; i++, lpMD++)
        {
           // For Monitor Descriptor, the first 3 bytes must all be zero
           // and the monitor descriptor type must be a range limit.
           if   ((lpMD->wZeroFlag == 0) && (lpMD->bZeroFlag == 0) &&
                 (lpMD->bTypeDescriptorBlock == DT_MONITOR_RANGE_LIMITS))
           {
                dwMinLine  = Multiply32By32 (lpMD->bMinHorzRateInKHz, 1000);
                dwMaxLine  = Multiply32By32 (lpMD->bMaxHorzRateInKHz, 1000);
                dwMinFrame = (ULONG) lpMD->bMinVertRateInHz;
                dwMaxFrame = (ULONG) lpMD->bMaxVertRateInHz;

                // Now do an intersection
                if  (dwMinFrame > lpML->dwMinVert)
                    lpML->dwMinVert = dwMinFrame;

                if  (dwMaxFrame < lpML->dwMaxVert)
                    lpML->dwMaxVert = dwMaxFrame;

                if  (dwMinLine > lpML->dwMinHorz)
                    lpML->dwMinHorz = dwMinLine;

                if  (dwMaxLine < lpML->dwMaxHorz)
                    lpML->dwMaxHorz = dwMaxLine;

                // If the bMaxPixelClockInCKHz in the EDID range limits is
                // 0xFF, then the pixel clock is not specified.
                if (lpMD->bMaxPixelClockInCKHz != 0xFF)
                {
                    dwMaxClock = Multiply32By32 (lpMD->bMaxPixelClockInCKHz, 10000000);
                    if  (dwMaxClock < lpML->dwMaxPixelClock)
                        lpML->dwMaxPixelClock = dwMaxClock;
                }
                else
                {
                    MODEINFO    sMI;

                    // Find a pixel clock using largest mode from EDID. 
                    // GetMaximumRefreshRate fills out lpML->dwMaxPixelClock.
                    // 640 and 480 are being used for dwTargetXRes and
                    // dwTargetYRes respectively, but these parameters are
                    // irrelevant because we just want the pixel clock and
                    // not the refresh rate. Be sure to zero out
                    // lpML->dwMaxPixelClock, or GetMaximumRefreshRate will
                    // not a calculate a pixel clock.
                    sMI.dwRefreshRate = 0;
                    EdidFindLargestEdidMode (lpEdidBuffer, dwEdidSize, &sMI);

                    lpML->dwMaxPixelClock = 0;
                    GetMaximumRefreshRate (sMI.dwXRes, sMI.dwYRes,
                                           sMI.dwRefreshRate, lpML, 640, 480);

                }
            }
        }

        // If we didn't find any reasonable values, then outta here
        if  ((lpML->dwMinVert == 0x00) ||
             (lpML->dwMaxVert == 0x0FFFFFFFF))
        {
            return  (FALSE);
        }

        return  (TRUE);
    }


    if  (nEdidVersion >= 0x200)
    {
        LPEDIDV2STRUC           lpV2 = (LPEDIDV2STRUC) lpEdidBuffer;
        LPDISPLAYTIMINGRANGE    lpTR;
        LPDETAILEDTIMINGRANGE   lpDT;
        ULONG                   dwExtra;
        ULONG                   dwClockExtra;
        ULONG                   dwLargestHBlank, dwLargestVBlank;
        ULONG                   dwLargestHTotal, dwLargestVTotal;
        ULONG                   dwSmallestHBlank, dwSmallestVBlank;
        ULONG                   dwSmallestHTotal, dwSmallestVTotal;
        ULONG                   j, k;
        LPCHAR                  lpC;
        int                     nSum;

        EdidFindNumTimingCodes (lpEdidBuffer, dwEdidSize, &TimingCodes);

        // If there were no monitor range limits, then outta here
        if  ((TimingCodes.dwNumFreqRanges == 0) &&
             (TimingCodes.dwNumRangeLimits == 0))
            return  (FALSE);

        // There are two types of monitor range limits in a
        // version 2.XX EDID. The first is a simple min/max
        // horz frequemcy, min/max vertical frequency, and
        // min/max pixel clock. The second includes this
        // and also specifies min/max values for sync offsets,
        // sync pulse widths, blanking, image size, borders
        // interlaced/non-interlaced, and sync polarities.
        // In any case, what we want to do is return a
        // MONITORLIMITS structure that is the intersection
        // of any and all these. The point is that we assume
        // that all of them apply and hence we want to restrict
        // by every one of them. NOTE that MONITORLIMITS
        // doesn't deal with all the blanking, sync pulses,
        // etc in the more detailed type of range limit struct
        // in the EDID, so we will ignore that for now. All
        // we care about are the min and max for each of
        // pixel clock, horz freq and vert freq.

        // In version 2.XX of the EDID, there is a section which
        // can contain 5 different types of objects. The first,
        // if it exists, are luminance tables which we just skip over.

        i = (int) (TimingCodes.dwNumLuminanceTables *
                 TimingCodes.dwSizeLuminanceTables);

        // Start all the mins high and the maxs low, so they
        // will get changed when we find something more restrictive.
        lpML->dwMinHorz = 0x00;
        lpML->dwMaxHorz = 0x0FFFFFFFF;
        lpML->dwMinVert = 0x00;
        lpML->dwMaxVert = 0x0FFFFFFFF;
        lpML->dwMaxPixelClock = 0x0FFFFFFFF;

        // The next is the simple range limit structures
        // There can be more than one.
        for (j = 0; j < TimingCodes.dwNumFreqRanges; j++)
        {
            lpTR = (LPDISPLAYTIMINGRANGE) &(lpV2->bTableDescriptors[i]);
            lpC  = (LPCHAR) lpTR;

            // Be careful here. For fixed frequency devices, all these
            // fields are set to 0.
            for (nSum = 0, k = 0; k < 8; k++)
                nSum += (int) lpC[k];

            if  (nSum != 0)
            {
                dwExtra    = ((ULONG) lpTR->bLoHV);
                dwMinFrame = (((ULONG) lpTR->bHiMinVert) << 2) + ((dwExtra >> 6) & 0x03);
                dwMaxFrame = (((ULONG) lpTR->bHiMaxVert) << 2) + ((dwExtra >> 4) & 0x03);
                dwMinLine  = (((ULONG) lpTR->bHiMinHorz) << 2) + ((dwExtra >> 2) & 0x03);
                dwMinLine  = Multiply32By32 (dwMinLine, 1000);
                dwMaxLine  = (((ULONG) lpTR->bHiMaxHorz) << 2) + ((dwExtra >> 0) & 0x03);
                dwMaxLine  = Multiply32By32 (dwMaxLine, 1000);

                dwClockExtra = ((ULONG) lpTR->bHiMinMaxPixClock);
                dwMaxClock   = ((ULONG) lpTR->bLoMaxPixClock) + ((dwClockExtra & 0x0F) << 8);
                dwMaxClock   = Multiply32By32 (dwMaxClock, 1000000);

                // Now do an intersection
                if  (dwMinFrame > lpML->dwMinVert)
                    lpML->dwMinVert = dwMinFrame;

                if  (dwMaxFrame < lpML->dwMaxVert)
                    lpML->dwMaxVert = dwMaxFrame;

                if  (dwMinLine > lpML->dwMinHorz)
                    lpML->dwMinHorz = dwMinLine;

                if  (dwMaxLine < lpML->dwMaxHorz)
                    lpML->dwMaxHorz = dwMaxLine;

                if  (dwMaxClock < lpML->dwMaxPixelClock)
                    lpML->dwMaxPixelClock = dwMaxClock;
            }

            i += sizeof(DISPLAYTIMINGRANGE);
        }

        // Now, handle any of the more complicated monitor limit structs.
        for (j = 0; j < TimingCodes.dwNumRangeLimits; j++)
        {
            lpDT = (LPDETAILEDTIMINGRANGE) &(lpV2->bTableDescriptors[i]);

            dwMinClock = ((ULONG) lpDT->wMinPixelClock) * 10000;
            dwMaxClock = ((ULONG) lpDT->wMaxPixelClock) * 10000;

            // The highest vertical frequency is obtained by using
            // dividing the maximum pixel clock by the smallest
            // total pixel count.
            dwSmallestHBlank = ((ULONG) lpDT->bLowMinHBlank) +
                                ((((ULONG) lpDT->bHiMinHVBlank) & 0xF0) << 4);
            dwSmallestVBlank = ((ULONG) lpDT->bLowMinVBlank) +
                                ((((ULONG) lpDT->bHiMinHVBlank) & 0x0F) << 8);
            dwSmallestHTotal = dwSmallestHBlank + ((ULONG) lpDT->bLowHActive)
                                + ((((ULONG) lpDT->bHiHVActive) & 0xF0) << 4);
            dwSmallestVTotal = dwSmallestVBlank + ((ULONG) lpDT->bLowVActive)
                                + ((((ULONG) lpDT->bHiHVActive) & 0x0F) << 8);
            dwMaxFrame = dwMaxClock / (dwSmallestVTotal * dwSmallestHTotal);
            dwMaxLine  = dwMaxClock / dwSmallestHTotal;

            dwLargestHBlank = ((ULONG) lpDT->bLowMaxHBlank) +
                                ((((ULONG) lpDT->bHiMaxHVBlank) & 0xF0) << 4);
            dwLargestVBlank = ((ULONG) lpDT->bLowMaxVBlank) +
                                ((((ULONG) lpDT->bHiMaxHVBlank) & 0x0F) << 8);
            dwLargestHTotal = dwLargestHBlank + ((ULONG) lpDT->bLowHActive)
                                + ((((ULONG) lpDT->bHiHVActive) & 0xF0) << 4);
            dwLargestVTotal = dwLargestVBlank + ((ULONG) lpDT->bLowVActive)
                                + ((((ULONG) lpDT->bHiHVActive) & 0x0F) << 8);
            dwMinFrame = dwMinClock / (dwLargestVTotal * dwLargestHTotal);
            dwMinLine  = dwMinClock / dwLargestHTotal;

            // Now do an intersection
            if  (dwMinFrame > lpML->dwMinVert)
                lpML->dwMinVert = dwMinFrame;

            if  (dwMaxFrame < lpML->dwMaxVert)
                lpML->dwMaxVert = dwMaxFrame;

            if  (dwMinLine > lpML->dwMinHorz)
                lpML->dwMinHorz = dwMinLine;

            if  (dwMaxLine < lpML->dwMaxHorz)
                lpML->dwMaxHorz = dwMaxLine;

            if  (dwMaxClock < lpML->dwMaxPixelClock)
            {
                // Add about 5% to their max pixel clock because
                // if this is a fixed freq timing, will never match
                // it exactly.
                lpML->dwMaxPixelClock = (dwMaxClock * 21) / 20;
            }

            i += sizeof(DETAILEDTIMINGRANGE);
        }

        // If we didn't find any reasonable values, then outta here
        if  ((lpML->dwMinVert == 0x00) ||
             (lpML->dwMaxVert == 0x0FFFFFFFF))
        {
            return  (FALSE);
        }

        return  (TRUE);
    }

    return (FALSE);
}



/*
    Function:   EdidIsGtfMonitor

    Purpose:    Checks EDID block to see if display type supports GTF.

    Arguments:  lpEdidBuffer    The EDID
                dwEdidSize      EDID size in bytes

    Returns:    TRUE            Display type supports GTF.
                FALSE           Display type does not support GTF.

    Preserve:
*/

int CFUNC
EdidIsGtfMonitor (LPCHAR lpEdidBuffer, ULONG dwEdidSize)
{
    int             nEdidVersion;
    MONITORLIMITS   sML;

    // Get the EDID version
    nEdidVersion = EdidGetVersion (lpEdidBuffer, dwEdidSize);

    // If we couldn't figure out what version it was, then say that
    // the monitor is not GTF.
    if  (!nEdidVersion)
        return  (FALSE);

    if  (nEdidVersion < 0x200)
    {
        LPEDIDV1STRUC lpV1 = (LPEDIDV1STRUC) lpEdidBuffer;

        // Edid version 1.X check for GTF monitor
        if  (lpV1->bFeatureSupport & 0x01)
            return  (TRUE);

        return (FALSE);
    }

    if (nEdidVersion >= 0x200)
    {
        LPEDIDV2STRUC lpV2 = (LPEDIDV2STRUC) lpEdidBuffer;

        // Edid version 2.X check for GTF monitor
        if  (lpV2->bGTFSupportInfo & 0xF0)
        {
            // The GTF monitor bits are set! But the spec also
            // says that the monitor is not truly GTF compliant
            // unless there is also a MONITORLIMITS descriptor
            if  (EdidGetMonitorLimits(lpEdidBuffer, dwEdidSize, &sML))
            {
                return  (TRUE);
            }
        }

        return (FALSE);
    }

    return  (FALSE);
}



/*
    Function:   EdidGetMonitorId

    Purpose:    This returns the monitorID from the EDID. The monitor
                ID is defined as a DWORD from high byte to low byte
                as follows:
                high byte of product code, low byte of product code, high
                byte of manufacturer code, low byte of manufacturer code.

                If there is any kind of error then return UKWN_ID which
                is defined in restime.h  It means any monitor for which
                an EDID is not defined. Since you can't parse the EDID
                for some reason, this is a reasonable return value.

    Arguments:  lpEdidBuffer    The EDID
                dwEdidSize      EDID size in bytes

    Returns:    TRUE            Display type supports GTF.
                FALSE           Display type does not support GTF.

    Preserve:
*/

ULONG CFUNC
EdidGetMonitorId (LPCHAR lpEdidBuffer, ULONG dwEdidSize)
{
    int     nEdidVersion;
    ULONG   dwMonitorID;

    // Get the EDID version
    nEdidVersion = EdidGetVersion (lpEdidBuffer, dwEdidSize);

    // If we couldn't figure out what version it was, then say that
    // the monitor is not GTF.
    if  (!nEdidVersion)
        return  (UKWN_ID);

    if  (nEdidVersion < 0x200)
    {
        LPEDIDV1STRUC lpV1 = (LPEDIDV1STRUC) lpEdidBuffer;

        dwMonitorID = (((ULONG) lpV1->wIDProductCode) << 16) |
                      ((ULONG) lpV1->wIDManufName);

        return  (dwMonitorID);
    }

    if (nEdidVersion >= 0x200)
    {
        LPEDIDV2STRUC lpV2 = (LPEDIDV2STRUC) lpEdidBuffer;

        dwMonitorID = (((ULONG) lpV2->wIDProductCode) << 16) |
                      ((ULONG) lpV2->wIDManufName);

        return  (dwMonitorID);
    }

    return  (UKWN_ID);
}



/*
    Function:   EdidBuildStandardTimingTable

    Purpose:    Using the Standard Timings field of the EDID block,
                create a table for the standard timings. NOTE: This
                procedure is only valid for Version 1.x EDID structures
                and the timings associated with these modes are assumed
                to be GTF timings.

    Arguments:  NONE

    Returns:    Nothing but copies supported modes to EDIDStandardTimingTable.

    Preserve:
*/

int CFUNC
EdidBuildStandardTimingTable (LPCHAR lpEdidBuffer, ULONG dwEdidSize)
{
    USHORT          wHorizontalRes;
    USHORT          wVerticalRes;
    USHORT          wRefreshRate;
    int             i, j, nIndex;
    int             FoundMode;
    int             nEdidVersion;
    GTFIN           GTFIn;
    GTFOUT          GTFOut;
    MODESTRUC       sGTF;
    LPMODESTRUC     lpDmtTable;
    LPEDIDV1STRUC   lpV1;

    lpV1 = (LPEDIDV1STRUC) lpEdidBuffer;

    // First, zero out all entries
    for (i = 0; i < 8; i++)
        EDIDStandardTimingTable[i] = sZeroMode[0];

    nEdidVersion = EdidGetVersion (lpEdidBuffer, dwEdidSize);

    // Please note that Standard timings are only valid on Version 1.x
    // EDIDs. Zeroing out the table above provides protection for
    // Version 2.x EDIDs.
    if  (nEdidVersion < 0x200)
    {
        for (nIndex = 0, i = 0; i < 8; i++)
        {
            wHorizontalRes = lpV1->wStandardTimingID[i] & 0x0FF;
            if  (wHorizontalRes != 0x01)                // Unused fields = 01
            {
                // The value in the EDID = (Horizontal active pixels/8) - 31
                wHorizontalRes += 31;
                wHorizontalRes <<= 3;
                wVerticalRes = lpV1->wStandardTimingID[i] >> 8;
                wRefreshRate = (wVerticalRes & 0x1F) + 60;

                switch (wVerticalRes & 0xC0)         // Aspect Ratio in 7:6
                {
                    case    0x00:
                        wVerticalRes = wHorizontalRes;          // 1:1
                        break;
                    case    0x40:
                        wVerticalRes = wHorizontalRes * 3 / 4;  // 4:3
                        break;
                    case    0x80:
                        wVerticalRes = wHorizontalRes * 4 / 5;  // 5:4
                        break;
                    case    0xC0:
                        wVerticalRes = wHorizontalRes * 9 / 16; // 16:9
                        break;
                }

                FoundMode = FALSE;

                // If monitor is not GTF, try DMT timings first.
                if (!EdidIsGtfMonitor (lpEdidBuffer, dwEdidSize))
                {
                    //First try to find matching DMT mode.
                    DmtGetDmtTablePointer (&lpDmtTable);

                    j = FindMode(lpDmtTable, (ULONG) wHorizontalRes,
                                    (ULONG) wVerticalRes, (ULONG) wRefreshRate);

                    // If a DMT mode was found.
                    if  (j >= 0)
                    {
                        sGTF.wHorizVisible    = lpDmtTable[j].wHorizVisible;
                        sGTF.wVertVisible     = lpDmtTable[j].wVertVisible;
                        sGTF.wRefresh         = lpDmtTable[j].wRefresh;
                        sGTF.wHorizTotal      = lpDmtTable[j].wHorizTotal;
                        sGTF.wHorizBlankStart = lpDmtTable[j].wHorizBlankStart;
                        sGTF.wHorizSyncStart  = lpDmtTable[j].wHorizSyncStart;
                        sGTF.wHorizSyncEnd    = lpDmtTable[j].wHorizSyncEnd;
                        sGTF.wHorizBlankEnd   = lpDmtTable[j].wHorizBlankEnd;
                        sGTF.wVertTotal       = lpDmtTable[j].wVertTotal;
                        sGTF.wVertBlankStart  = lpDmtTable[j].wVertBlankStart;
                        sGTF.wVertSyncStart   = lpDmtTable[j].wVertSyncStart;
                        sGTF.wVertSyncEnd     = lpDmtTable[j].wVertSyncEnd;
                        sGTF.wVertBlankEnd    = lpDmtTable[j].wVertBlankEnd;
                        sGTF.wDotClock        = lpDmtTable[j].wDotClock;
                        sGTF.wHSyncPolarity   = lpDmtTable[j].wHSyncPolarity;
                        sGTF.wVSyncPolarity   = lpDmtTable[j].wVSyncPolarity;

                        FoundMode = TRUE;
                    }

                }

                // No DMT mode was found--use GTF.
                if (!FoundMode)
                {
                    // Get GTF timings for mode.
                    GTFIn.dwHPixels      = (ULONG) wHorizontalRes;
                    GTFIn.dwVLines       = (ULONG) wVerticalRes;
                    GTFIn.dwMarginsRqd   = FALSE;
                    GTFIn.dwIntRqd       = FALSE;
                    GTFIn.dwIPParmType   = IPTYPE_VERTICAL_REFRESH_RATE;
                    GTFIn.dwIPFreqRqd    = (ULONG) wRefreshRate;
                    ComputeGTFParameters(&GTFIn, &GTFOut);

                    // Store all parameters in temporary MODESTRUC buffer. Note that
                    // horizontal GTF parameters are returned in character clocks and
                    // need to be converted to pixels.
                    sGTF.wHorizVisible    = (USHORT) (GTFOut.dwHActiveChars << 3);
                    sGTF.wVertVisible     = (USHORT) GTFOut.dwVActiveScans;
                    sGTF.wRefresh         = (USHORT) wRefreshRate;
                    sGTF.wHorizTotal      = (USHORT) (GTFOut.dwHTotalChars << 3);
                    sGTF.wHorizBlankStart = (USHORT) (GTFOut.dwHBlankStartChar << 3);
                    sGTF.wHorizSyncStart  = (USHORT) ((GTFOut.dwHBlankStartChar
                                                     + GTFOut.dwHFrontPorchChars) << 3);
                    sGTF.wHorizSyncEnd    = (USHORT) (sGTF.wHorizSyncStart
                                                     + (GTFOut.dwHSyncChars << 3));
                    sGTF.wHorizBlankEnd   = (USHORT) (GTFOut.dwHBlankEndChar << 3);
                    sGTF.wVertTotal       = (USHORT) GTFOut.dwVTotalScans;
                    sGTF.wVertBlankStart  = (USHORT) GTFOut.dwVBlankStartScan;
                    sGTF.wVertSyncStart   = (USHORT) (GTFOut.dwVBlankStartScan
                                                     + GTFOut.dwVFrontPorchScans);
                    sGTF.wVertSyncEnd     = (USHORT) (sGTF.wVertSyncStart
                                                     + GTFOut.dwVSyncScans);
                    sGTF.wVertBlankEnd    = (USHORT) GTFOut.dwVBlankEndScan;
                    sGTF.wDotClock        = (USHORT) GTFOut.dwPixelClockIn10KHertz;
                    sGTF.wHSyncPolarity   = BUFFER_HSYNC_NEGATIVE;
                    sGTF.wVSyncPolarity   = BUFFER_VSYNC_POSITIVE;


                }

                EDIDStandardTimingTable[nIndex++] = sGTF;
            }
        }
    }


    return  (TRUE);
}


/*
    Function:   EdidFindStandardMode

    Purpose:    This routine finds if the mode exists in the EDID
                standard mode table.

    Arguments:  lpEdidBuffer    EDID
                dwEdidSize      size of EDID buffer
                lpModeDesc      mode to match
                lpModeOut       ptr to MODEOUT structure
                                put timing argumetns here if there is
                                a match on the input mode

    Returns:    TRUE if there was a match and lpModeOut was filled
                FALSE if there was no match

    Preserve:   Do not change any fields in lpModeDesc.
*/
int CFUNC
EdidFindStandardMode (
        LPCHAR      lpEdidBuffer,
        ULONG       dwEdidSize,
        LPMODEINFO  lpModeDesc,
        LPMODEOUT   lpModeOut)
{
    int i, nEdidVersion;

    // First, zero out all entries
    for (i = 0; i < 8; i++)
        EDIDStandardTimingTable[i] = sZeroMode[0];

    nEdidVersion = EdidGetVersion (lpEdidBuffer, dwEdidSize);

    // Please note that Standard timings are only valid on Version 1.x
    // EDIDs. Zeroing out the table above provides protection for
    // Version 2.x EDIDs.
    if  (nEdidVersion < 0x200)
    {
        EdidBuildStandardTimingTable (lpEdidBuffer, dwEdidSize);
    }

    i = FindMode (EDIDStandardTimingTable, lpModeDesc->dwXRes,
                    lpModeDesc->dwYRes, lpModeDesc->dwRefreshRate);

    if  (i < 0)
        return  (FALSE);

    SetupParamsForVidLutClass (EDIDStandardTimingTable + i, lpModeOut,
                               lpModeDesc);

    return  (TRUE);
}


/*
    Function:   EdidBuildEstablishedTimingTable

    Purpose:    Using the Established Timings field of the EDID block,
                create a table for the established timings. NOTE: This
                procedure is only valid for Version 1.x EDID structures.

    Arguments:  NONE

    Returns:    Nothing but copies supported modes from
                EDIDEstablishedTimingTable to EDIDEstablishedTimingTbl.

    Preserve:
*/

int CFUNC
EdidBuildEstablishedTimingTable (LPCHAR lpEdidBuffer, ULONG dwEdidSize)
{
    ULONG           dwTimingBits, dwT1, dwT2;
    int             i, nIndex;
    int             nEdidVersion;
    LPEDIDV1STRUC   lpV1;

    // Only use VESA Established timings and put them in contiguous
    // order from highest resolution to lowest resolution so they will
    // match the order of EDIDEstablishedTimingTable[].
    lpV1 = (LPEDIDV1STRUC) lpEdidBuffer;

    dwT1 = (ULONG) (lpV1->bEstablishedTimings1);
    dwT2 = (ULONG) (lpV1->bEstablishedTimings2);
    dwTimingBits  =  (dwT2 & 0x0F) + ((dwT2 & 0xC0) >> 2);
    dwTimingBits  |= ((dwT1 & 0x0F) << 6);

    // First, zero out all entries
    for (i = 0; i < 10; i++)
        EDIDEstablishedTimingTable[i] = sZeroMode[0];

    nEdidVersion = EdidGetVersion (lpEdidBuffer, dwEdidSize);

    // Please note that Standard timings are only valid on Version 1.x
    // EDIDs. Zeroing out the table above provides protection for
    // Version 2.x EDIDs.
    if  (nEdidVersion < 0x200)
    {
        for (nIndex = 0, i = 9; i >= 0; i--)
        {
            if (dwTimingBits & (1 << i))    // is this mode supported?
            {
                EDIDEstablishedTimingTable[nIndex++] = EDIDEstablishedTimingTbl[i];
            }
        }
    }

    return  (TRUE);
}


/*
    Function:   EdidFindEstablishedMode

    Purpose:    This routine finds if the mode exists in the EDID
                established mode table.

    Arguments:  lpEdidBuffer    EDID
                dwEdidSize      size of EDID buffer
                lpModeDesc      mode to match
                lpModeOut       ptr to MODEOUT structure
                                put timing argumetns here if there is
                                a match on the input mode

    Returns:    TRUE if there was a match and lpModeOut was filled
                FALSE if there was no match

    Preserve:   Do not change any fields in lpModeDesc.
*/
int CFUNC
EdidFindEstablishedMode (
        LPCHAR      lpEdidBuffer,
        ULONG       dwEdidSize,
        LPMODEINFO  lpModeDesc,
        LPMODEOUT   lpModeOut)
{
    int i, nEdidVersion;

    // First, zero out all entries
    for (i = 0; i < 10; i++)
        EDIDEstablishedTimingTable[i] = sZeroMode[0];

    nEdidVersion = EdidGetVersion (lpEdidBuffer, dwEdidSize);

    // Please note that Established timings are only valid on Version 1.x
    // EDIDs. Zeroing out the table above provides protection for
    // Version 2.x EDIDs.
    if  (nEdidVersion < 0x200)
    {
        EdidBuildEstablishedTimingTable (lpEdidBuffer, dwEdidSize);
    }

    i = FindMode (EDIDEstablishedTimingTable, lpModeDesc->dwXRes,
                    lpModeDesc->dwYRes, lpModeDesc->dwRefreshRate);

    if  (i < 0)
        return  (FALSE);

    SetupParamsForVidLutClass (EDIDEstablishedTimingTable + i, lpModeOut,
                               lpModeDesc);

    return  (TRUE);
}



/*
    Function:   EdidBuildDetailedTimingTable

    Purpose:    Using the Detailed Timing section of the EDID block,
                create a table for the detailed timings.

    Arguments:  EdidVersion     Version of EDID block.

    Returns:    Nothing but copies detailed timing modes to
                EDIDDetailedTimingTable.

    Preserve:
*/

int CFUNC
EdidBuildDetailedTimingTable(LPCHAR lpEdidBuffer, ULONG dwEdidSize)
{
    int         nEdidVersion;
    ULONG       dwNumTimings;
    ULONG       dwOffset;
    ULONG       i, j;
    ULONG       dwPixelClock;
    ULONG       dwHorizVertTotal;
    MODESTRUC   sTS;
    LPDETAILEDTIMINGDESCRIPTOR  lpDTD;
    LPEDIDV1STRUC   lpV1;
    TIMINGCODES     tc;
    LPCHAR          lpB;

    // First, zero out all entries in the detailed timing table
    for (i = 0; i < 10; i++)
        EDIDDetailedTimingTable[i] = sZeroMode[0];

    // Get the version
    nEdidVersion = EdidGetVersion (lpEdidBuffer, dwEdidSize);

    if  (nEdidVersion < 0x200)
    {
        // Version 1.x
        lpV1 = (LPEDIDV1STRUC) lpEdidBuffer;

        // In version 1.XX, the detailed timings are at a fixed offset
        dwOffset = (ULONG)((LPCHAR) &(lpV1->DetailedTimingDesc[0]) - (LPCHAR) lpV1);

        // With version 1.XX of the EDID, there are 4 detailed timing
        // descriptor slots. Any or all of them can be used. Unused
        // ones are marked with a 0 in the first three bytes.
        dwNumTimings = 4;
    }
    else
    {
        // Version 2.XX of the EDID
        EdidFindNumTimingCodes (lpEdidBuffer, dwEdidSize, &tc);

        // In version 2.XX, the detailed timings are in a section of
        // the EDID which holds many items that are not required.
        // Hence, it is necessary to compute the offset based upon
        // which of the optional items is present. These items begin
        // at offset 80h in the EDID.
        dwOffset = (tc.dwNumLuminanceTables * tc.dwSizeLuminanceTables +
                   tc.dwNumFreqRanges * 8 + tc.dwNumRangeLimits * 27 +
                   tc.dwNumTimingCodes * 4) + 0x80;

        // With version 2.XX of the EDID, there are a variable number
        // of detailed timing descriptor slots. All of them are used.
        // There needn't be any checking for leading zeros.
        dwNumTimings = tc.dwNumDetailedTimings;
    }

    for (j = 0, i = 0; i < dwNumTimings; i++)
    {
        lpDTD = (LPDETAILEDTIMINGDESCRIPTOR) &(lpEdidBuffer[dwOffset]);
        lpB = (LPCHAR) lpDTD;

        // Although technically we should only do the "three leading zeros"
        // check for version 1 of the EDID, if the first three bytes of a
        // detailed timing are zeros, then we should probably ignore the
        // timing for version 2.XX of the EDID since the first two bytes
        // are the pixel clock!
        if  ((lpB[0]) || (lpB[1]) || (lpB[2]))
        {
            sTS.wHorizVisible    = (USHORT) (lpDTD->bDTHorizontalActive
                                    + ((lpDTD->bDTHorizActiveBlank & 0xF0) << 4));
            sTS.wVertVisible     = (USHORT)(lpDTD->bDTVerticalActive
                                    + ((lpDTD->bDTVertActiveBlank & 0xF0) << 4));
            sTS.wHorizTotal      = sTS.wHorizVisible
                                    + (USHORT)(lpDTD->bDTHorizontalBlanking
                                    + ((lpDTD->bDTHorizActiveBlank & 0x0F) << 8));
            sTS.wHorizBlankStart = sTS.wHorizVisible;
            sTS.wHorizSyncStart  = sTS.wHorizBlankStart
                                    + (USHORT)(lpDTD->bDTHorizontalSync
                                    + ((lpDTD->bDTHorizVertSyncOverFlow & 0xC0) << 2));
            sTS.wHorizSyncEnd    = sTS.wHorizSyncStart
                                    + (USHORT)(lpDTD->bDTHorizontalSyncWidth
                                    + ((lpDTD->bDTHorizVertSyncOverFlow & 0x30) << 4));
            sTS.wHorizBlankEnd   = sTS.wHorizTotal;
            sTS.wVertTotal       = sTS.wVertVisible
                                    + (USHORT)(lpDTD->bDTVerticalBlanking
                                    + ((lpDTD->bDTVertActiveBlank & 0x0F) << 8));
            sTS.wVertBlankStart  = sTS.wVertVisible;
            sTS.wVertSyncStart   = sTS.wVertBlankStart
                                    + (USHORT)(((lpDTD->bDTVerticalSync & 0xF0) >> 4)
                                    + ((lpDTD->bDTHorizVertSyncOverFlow & 0x0C) << 2));
            sTS.wVertSyncEnd     = sTS.wVertSyncStart
                                    + (USHORT)((lpDTD->bDTVerticalSync & 0x0F)
                                    + ((lpDTD->bDTHorizVertSyncOverFlow & 0x03) << 4));
            sTS.wVertBlankEnd    = sTS.wVertTotal;
            sTS.wDotClock        = lpDTD->wDTPixelClock;

            if ((lpDTD->bDTFlags & 0x18) == 0x18)
            {
                sTS.wHSyncPolarity  = (lpDTD->bDTFlags & 0x2) >> 1;
                sTS.wVSyncPolarity  = (lpDTD->bDTFlags & 0x4) >> 2;
            }
            else if ((lpDTD->bDTFlags & 0x18) == 0x10)
            {
                sTS.wHSyncPolarity  = (lpDTD->bDTFlags & 0x2) >> 1;
                sTS.wVSyncPolarity  = BUFFER_VSYNC_POSITIVE;
            }
            else
            {
                sTS.wHSyncPolarity  = BUFFER_HSYNC_NEGATIVE;
                sTS.wVSyncPolarity  = BUFFER_VSYNC_POSITIVE;
            }

            dwPixelClock = Multiply32By32 (lpDTD->wDTPixelClock, 10000);
            dwHorizVertTotal = Multiply32By32 (sTS.wHorizTotal, sTS.wVertTotal);
            sTS.wRefresh = (USHORT) Divide32By32 (dwPixelClock +
                                    dwHorizVertTotal / 2, dwHorizVertTotal);

            EDIDDetailedTimingTable[j++] = sTS;
        }

        dwOffset += sizeof(DETAILEDTIMINGDESCRIPTOR);
    }

    return  (TRUE);
}


/*
    Function:   EdidFindDetailedMode

    Purpose:    This routine finds if the mode exists in the EDID
                detailed mode table.

    Arguments:  lpEdidBuffer    EDID
                dwEdidSize      size of EDID buffer
                lpModeDesc      mode to match
                lpModeOut       ptr to MODEOUT structure
                                put timing argumetns here if there is
                                a match on the input mode

    Returns:    TRUE if there was a match and lpModeOut was filled
                FALSE if there was no match

    Preserve:   Do not change any fields in lpModeDesc.
*/
int CFUNC
EdidFindDetailedMode (
        LPCHAR      lpEdidBuffer,
        ULONG       dwEdidSize,
        LPMODEINFO  lpModeDesc,
        LPMODEOUT   lpModeOut)
{
    int i;

    if  (!EdidBuildDetailedTimingTable (lpEdidBuffer, dwEdidSize))
        return  (FALSE);

    i = FindMode (EDIDDetailedTimingTable, lpModeDesc->dwXRes,
                    lpModeDesc->dwYRes, lpModeDesc->dwRefreshRate);

    if  (i < 0)
        return  (FALSE);

    SetupParamsForVidLutClass (EDIDDetailedTimingTable + i, lpModeOut,
                               lpModeDesc);

    return  (TRUE);
}


/*
    Function:   EdidFindExactModeWithHighestRR

    Purpose:    This routine searches for the highest refresh rate
                in the detailed, established, or standard timing
                sections that has a resolution exactly equal to the
                dwXRes and dwYRes in lpModeDesc. The refresh rate
                for that mode is returned. If there is no matching xres,
                yres, then 0 is returned.

    Arguments:  lpEdidBuffer    EDID
                dwEdidSize      size of EDID buffer
                lpModeDesc      mode to match

    Returns:    a non-0 is the highest refresh rate found in the detailed,
                established, or standard timing sections greater than
                or equal to the XRES, YRES given.
                0 if there was an error or a matching mode was not found

    Preserve:   Do not change any fields in lpModeDesc.
*/
int CFUNC
EdidFindExactModeWithHighestRR (
        LPCHAR      lpEdidBuffer,
        ULONG       dwEdidSize,
        LPMODEINFO  lpModeDesc)
{
    int             i, nHighestRR;

    nHighestRR = 0;

    EdidBuildDetailedTimingTable (lpEdidBuffer, dwEdidSize);
    if ((i = FindModeWithoutRR (EDIDDetailedTimingTable, lpModeDesc->dwXRes,
                      lpModeDesc->dwYRes)) != -1)
        nHighestRR = EDIDDetailedTimingTable[i].wRefresh;

    EdidBuildEstablishedTimingTable (lpEdidBuffer, dwEdidSize);
    if ((i = FindModeWithoutRR (EDIDEstablishedTimingTable, lpModeDesc->dwXRes,
                      lpModeDesc->dwYRes)) != -1)
        if  (EDIDEstablishedTimingTable[i].wRefresh > (USHORT) nHighestRR)
            nHighestRR = EDIDEstablishedTimingTable[i].wRefresh;

    EdidBuildStandardTimingTable (lpEdidBuffer, dwEdidSize);
    if ((i = FindModeWithoutRR (EDIDStandardTimingTable, lpModeDesc->dwXRes,
                      lpModeDesc->dwYRes)) != -1)
        if  (EDIDStandardTimingTable[i].wRefresh > (USHORT) nHighestRR)
            nHighestRR = EDIDStandardTimingTable[i].wRefresh;

    if  (nHighestRR)
        return (nHighestRR);

    return  (FALSE);
}


/*
    Function:   EdidFindSameModeOrLargerWithHighestRR

    Purpose:    This routine searches for the highest refresh rate
                in the detailed, established, or standard timing
                sections that has a resolution greater than or equal
                to the dwXRes and dwYRes in lpModeDesc. The refresh rate
                for that mode is returned. If there is no matching xres,
                yres, then 0 is returned.

    Arguments:  lpEdidBuffer    EDID
                dwEdidSize      size of EDID buffer
                lpModeDesc      mode to match

    Returns:    a non-0 is the highest refresh rate found in the detailed,
                established, or standard timing sections greater than
                or equal to the XRES, YRES given.
                0 if there was an error or a matching mode was not found

    Preserve:   Do not change any fields in lpModeDesc.
*/
int CFUNC
EdidFindSameModeOrLargerWithHighestRR (
        LPCHAR      lpEdidBuffer,
        ULONG       dwEdidSize,
        LPMODEINFO  lpModeDesc)
{
    int             nHighestRR, nCurrentRR;
    MONITORLIMITS   ML;

    EdidBuildDetailedTimingTable (lpEdidBuffer, dwEdidSize);
    nHighestRR = EdidFindHighestRRinTable (EDIDDetailedTimingTable, lpModeDesc);

    EdidBuildEstablishedTimingTable (lpEdidBuffer, dwEdidSize);
    nCurrentRR = EdidFindHighestRRinTable (EDIDEstablishedTimingTable, lpModeDesc);
    if  (nCurrentRR > nHighestRR)
        nHighestRR = nCurrentRR;

    EdidBuildStandardTimingTable (lpEdidBuffer, dwEdidSize);
    nCurrentRR = EdidFindHighestRRinTable (EDIDStandardTimingTable, lpModeDesc);
    if  (nCurrentRR > nHighestRR)
        nHighestRR = nCurrentRR;

    if  (nHighestRR < 60)
        nHighestRR = 60;

    if  (!EdidGetMonitorLimits (lpEdidBuffer, dwEdidSize, &ML))
        return (nHighestRR);

    if  ((ML.dwMaxHorz) && (ML.dwMaxVert))
    {
        // NOTE: If the MonitorLimits ptr passed in to the function
        // GetMaximumRefreshRate has a non-0 dwMaxPixelClock field,
        // then the first three arguments to GetMaximumRefreshRate
        // are irrelevant. We know that ML.dwMaxPixelClock is non-0
        // at this time, so we just pass in 0s for the parameters.
        nCurrentRR = (int) GetMaximumRefreshRate (0, 0, 0, &ML,
                                lpModeDesc->dwXRes, lpModeDesc->dwYRes);


        if (nCurrentRR > nHighestRR)
            nHighestRR = nCurrentRR;
    }

    return  (nHighestRR);
}


/*
    Function:   EdidFindHighestRRinTable

    Purpose:    This routine searches for the highest refresh rate
                in the table passed in. If there is no mode mode greater
                than or equal to dwXRes dwYres, then 0 is returned.

    Arguments:  lpEdidBuffer    EDID
                dwEdidSize      size of EDID buffer
                lpModeDesc      mode to match

    Returns:    a non-0 is the highest refresh rate found in the detailed,
                established, or standard timing sections greater than
                or equal to the XRES, YRES given.
                0 if there was an error or a matching mode was not found

    Preserve:   Do not change any fields in lpModeDesc.
*/
int CFUNC
EdidFindHighestRRinTable (
        LPMODESTRUC lpMS,
        LPMODEINFO  lpModeDesc)
{
    int          i, nBestIndex;

    nBestIndex = -1;

    for (i = 0; lpMS[i].wHorizVisible != 0x00; i++)
    {
        if (((ULONG) lpMS[i].wHorizVisible >= lpModeDesc->dwXRes) &&
            ((ULONG) lpMS[i].wVertVisible >= lpModeDesc->dwYRes))
        {
            if  (nBestIndex != -1)
            {
                if  (lpMS[i].wRefresh > lpMS[nBestIndex].wRefresh)
                    nBestIndex = i;
            }
            else
            {
                nBestIndex = i;
            }
        }
    }

    // If we didn't even find a mode greater than or equal to dwXRes
    // and dwYRes ...
    if  (nBestIndex == -1)
        return  (0);

    // We found one. Return the refresh rate.
    return  ((int) lpMS[nBestIndex].wRefresh);
}


/*
    Function:   EdidFindLargestEdidMode

    Purpose:    This routine searches the EDID for the largest mode of
                the detailed, established, and standard timings.

    Arguments:  lpEdidBuffer    EDID
                dwEdidSize      size of EDID buffer
                lpModeInfo      pointer to buffer for return parameters

    Returns:    TRUE and lpModeInfo filled out with largest XRes and YRes
                     found in EDID or 640x480.

*/
int CFUNC
EdidFindLargestEdidMode (
        LPCHAR      lpEdidBuffer,
        ULONG       dwEdidSize,
        LPMODEINFO  lpModeInfo)
{
    // Start with 640x480. Color depth is specified in registry.
    lpModeInfo->dwXRes          = 640;
    lpModeInfo->dwYRes          = 480;

    EdidBuildDetailedTimingTable (lpEdidBuffer, dwEdidSize);
    EdidFindLargestModeinTable (EDIDDetailedTimingTable, lpModeInfo);

    EdidBuildEstablishedTimingTable (lpEdidBuffer, dwEdidSize);
    EdidFindLargestModeinTable (EDIDEstablishedTimingTable, lpModeInfo);

    EdidBuildStandardTimingTable (lpEdidBuffer, dwEdidSize);
    EdidFindLargestModeinTable (EDIDStandardTimingTable, lpModeInfo);

    return  (TRUE);
}


/*
    Function:   EdidFindLargestModeinTable

    Purpose:    This routine searches the table pointed to by lpMS for
                a mode that is larger than the mode pointed to by
                lpModeInfo.

    Arguments:  lpMS            pointer to table to search
                lpModeInfo      pointer to mode to compare and to return
                                parameters.

    Returns:    TRUE and lpModeInfo filled out with largest XRes and YRes
                     found in table or mode originally in ModeInfo.

*/
int CFUNC
EdidFindLargestModeinTable (
        LPMODESTRUC lpMS,
        LPMODEINFO  lpModeInfo)
{
    int i;

    for (i = 0; lpMS[i].wHorizVisible != 0x00; i++)
    {
        if ((ULONG) lpMS[i].wHorizVisible >= lpModeInfo->dwXRes)
        {
            if ((ULONG) lpMS[i].wVertVisible >= lpModeInfo->dwYRes)
            {
                lpModeInfo->dwXRes = (ULONG) lpMS[i].wHorizVisible;
                lpModeInfo->dwYRes = (ULONG) lpMS[i].wVertVisible;
                if (lpMS[i].wRefresh > (USHORT) lpModeInfo->dwRefreshRate)
                     lpModeInfo->dwRefreshRate = (ULONG) lpMS[i].wRefresh;
            }
        }
    }

    return  (TRUE);
}


/*
    Function:   EdidCmpToLargestEdidMode

    Purpose:    This routine searches the EDID for the largest mode
                of the detailed, established, and standard timings
                to determine if the requested mode is less than or
                equal to the largest mode found in the EDID.

    Arguments:  lpEdidBuffer    EDID
                dwEdidSize      size of EDID buffer
                lpModeDesc      mode to match

    Returns:
        The return value can be a logical OR of one of the first three
        flags with one of the second three flags.
        REQUESTED_MODE_SMALLER_XYRES_THAN_LARGEST_EDID_MODE
        REQUESTED_MODE_SAME_XYRES_AS_LARGEST_EDID_MODE
        REQUESTED_MODE_LARGER_XYRES_THAN_LARGEST_EDID_MODE

        REQUESTED_MODE_SMALLER_PXCLK_THAN_LARGEST_EDID_MODE
        REQUESTED_MODE_SAME_PXCLK_AS_LARGEST_EDID_MODE
        REQUESTED_MODE_LARGER_PXCLK_THAN_LARGEST_EDID_MODE

    Preserve:   Do not change any fields in lpModeDesc.
*/
int CFUNC
EdidCmpToLargestEdidMode (
    LPCHAR      lpEdidBuffer,
    ULONG       dwEdidSize,
    LPMODEINFO  lpMD)
{
    int         i, j, nRet;
    ULONG       dwMaxXRes, dwMaxYRes;
    ULONG       dwMaxPixelClock, dwMatchingPixelClock;
    LPMODESTRUC lpMS;
    MODEOUT     MO;

    // Start this off at 0
    nRet = 0;

    // Build all EDID tables.
    EdidBuildDetailedTimingTable (lpEdidBuffer, dwEdidSize);
    EdidBuildEstablishedTimingTable (lpEdidBuffer, dwEdidSize);
    EdidBuildStandardTimingTable (lpEdidBuffer, dwEdidSize);

    // Find the maximum pixel clock in the detailed timings.
    lpMS = EDIDDetailedTimingTable;
    dwMatchingPixelClock = 0;
    dwMaxPixelClock = (ULONG) lpMS[0].wDotClock;
    dwMaxXRes = (ULONG) lpMS[0].wHorizVisible;
    dwMaxYRes = (ULONG) lpMS[0].wVertVisible;
    for (j = 0; j < 3; j++)
    {
        if  (j == 0)
            lpMS = EDIDDetailedTimingTable;
        else if  (j == 1)
            lpMS = EDIDEstablishedTimingTable;
        else if  (j == 2)
            lpMS = EDIDStandardTimingTable;

        for (i = 0; lpMS[i].wHorizVisible != 0x00; i++)
        {
            // Find the largest resolution of the mode in this list
            if  ((lpMS[i].wHorizVisible > dwMaxXRes) &&
                 (lpMS[i].wVertVisible > dwMaxYRes))
            {
                dwMaxXRes = lpMS[i].wHorizVisible;
                dwMaxYRes = lpMS[i].wVertVisible;
            }

            // Find the largest pixel clock of the modes in this list
            if (lpMS[i].wDotClock > dwMaxPixelClock)
                dwMaxPixelClock = (ULONG) lpMS[i].wDotClock;

            // Remember if any of the EDID modes exactly matches
            // the mode passed in. Remember the pixel clock of the
            // matching mode.
            if  ((lpMS[i].wHorizVisible == lpMD->dwXRes) &&
                 (lpMS[i].wVertVisible == lpMD->dwYRes))
            {
                nRet = REQUESTED_MODE_XYRES_EXACTLY_MATCHES_AN_EDID_MODE;
                dwMatchingPixelClock = (ULONG) lpMS[i].wDotClock;
            }
        }
    }

    // Now, how does the mode passed in compare?
    if  ((lpMD->dwXRes == dwMaxXRes) && (lpMD->dwYRes == dwMaxYRes))
        nRet |= REQUESTED_MODE_SAME_XYRES_AS_LARGEST_EDID_MODE;
    else if  ((lpMD->dwXRes <= dwMaxXRes) && (lpMD->dwYRes <= dwMaxYRes))
        nRet |= REQUESTED_MODE_SMALLER_XYRES_THAN_LARGEST_EDID_MODE;
    else
        nRet |= REQUESTED_MODE_LARGER_XYRES_THAN_LARGEST_EDID_MODE;

    // We need to find a pixel clock for the mode passed in.
    if  (dwMatchingPixelClock == 0)
    {
        // The XRes and YRes of the mode passed in did not exactly
        // match any of the EDID modes, so we don't know what the
        // pixel clock of the passed in mode should be! Use the
        // pixel clock generated by Gtf in this case.
        GtfFindMode (lpMD, (LPMODEOUT) &MO);
        dwMatchingPixelClock = MO.dwPixelClock;
    }

    // Now we can compare the pixel clocks.
    if  (dwMatchingPixelClock < dwMaxPixelClock)
        nRet |= REQUESTED_MODE_SMALLER_PXCLK_THAN_LARGEST_EDID_MODE;
    else if (dwMatchingPixelClock == dwMaxPixelClock)
        nRet |= REQUESTED_MODE_SAME_PXCLK_AS_LARGEST_EDID_MODE;
    else
        nRet |= REQUESTED_MODE_LARGER_PXCLK_THAN_LARGEST_EDID_MODE;

    return  (nRet);
}


/*
    Function:   EdidFindLargestPixelClock

    Purpose:    This routine searches the EDID for the mode
                with the largest pixel clock and returns that
                largest pixel clock.

    Arguments:  lpEdidBuffer    EDID
                dwEdidSize      size of EDID buffer

    Returns:    largest pixel clock (may be 0 if the EDID has
                no modes init)

    Preserve:
*/
ULONG CFUNC
EdidFindLargestPixelClock (
    LPCHAR      lpEdidBuffer,
    ULONG       dwEdidSize)
{
    ULONG   dwMaxPixelClock;
    int     j;

    // Start out at 0
    dwMaxPixelClock = 0;

    EdidBuildDetailedTimingTable (lpEdidBuffer, dwEdidSize);
    for (j = 0; EDIDDetailedTimingTable[j].wHorizVisible != 0x00; j++)
        if  (dwMaxPixelClock < (ULONG) EDIDDetailedTimingTable[j].wDotClock)
            dwMaxPixelClock = (ULONG) EDIDDetailedTimingTable[j].wDotClock;

    EdidBuildEstablishedTimingTable (lpEdidBuffer, dwEdidSize);
    for (j = 0; EDIDEstablishedTimingTable[j].wHorizVisible != 0x00; j++)
        if  (dwMaxPixelClock < (ULONG) EDIDEstablishedTimingTable[j].wDotClock)
            dwMaxPixelClock = (ULONG) EDIDEstablishedTimingTable[j].wDotClock;

    EdidBuildStandardTimingTable (lpEdidBuffer, dwEdidSize);
    for (j = 0; EDIDStandardTimingTable[j].wHorizVisible != 0x00; j++)
        if  (dwMaxPixelClock < (ULONG) EDIDStandardTimingTable[j].wDotClock)
            dwMaxPixelClock = (ULONG) EDIDStandardTimingTable[j].wDotClock;

    return  (dwMaxPixelClock);
}

