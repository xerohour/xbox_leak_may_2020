/*
    FILE:   dmt.c
    DATE:   4/8/99

    This file is the generic entry point for the dmt modeset code.
*/

#include "cmntypes.h"
#include "modeext.h"
#include "modeset.h"
#include "utils.h"
#include "debug.h"

//*****************************************************************************
//
// VESA 1.0 rev 0.7 DMT timings
//
// DMT timing calculations were produced with the VESA DMT 1.0 rev 0.7
// Monitor Timing Specifications.
//
// Horizontal Visible, Vertical Visible
// Refresh (Hz * 100)
// Horizontal Total, Blank Start, Sync Start, Sync End, Blank End
// Vertical Total, Blank Start, Sync Start, Sync End, Blank End
// DotClock (MHz * 100)
// HSync polarity
// VSync polarity
//
// NOTE: Unlike GTF timings, DMT does not dictate zero border widths.
// Therefore we must include blank start and end positions.
//
MODESTRUC    DMTTimingTable[] =
{
    // Settings for 320x200 (mode doubled 640x400):
    //  NOTE: There is no 60Hz 320x200, so use 70Hz if anyone wants 60Hz
    320,200,60,800/2,640/2,656/2,752/2,288/2,449,400,412,414,449,2518/2,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_NEGATIVE,
    320,200,70,800/2,640/2,656/2,752/2,288/2,449,400,412,414,449,2518/2,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_NEGATIVE,
    320,200,85,832/2,640/2,672/2,736/2,832/2,445,400,401,404,445,3150/2,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_NEGATIVE,

    // Settings for 320x240 (mode doubled 640x480):
    320,240,60,800/2,648/2,656/2,752/2,792/2,525,488,490,492,517,2518/2,
        BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_NEGATIVE,
    320,240,72,832/2,648/2,664/2,704/2,824/2,520,488,489,492,512,3150/2,
        BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_NEGATIVE,
    320,240,75,840/2,640/2,656/2,720/2,840/2,500,480,481,484,500,3150/2,
        BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_NEGATIVE,
    320,240,85,832/2,640/2,696/2,752/2,832/2,509,480,481,484,509,3600/2,
        BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_NEGATIVE,

    // Settings for 400x300 (mode doubled 800x600):
    400,300,60,1056/2,800/2,840/2,968/2,1056/2,628,600,601,605,628,4000/2,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,
    400,300,72,1040/2,800/2,856/2,976/2,1040/2,666,600,637,643,666,5000/2,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,
    400,300,75,1056/2,800/2,816/2,896/2,1056/2,625,600,601,604,625,4950/2,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,
    400,300,85,1048/2,800/2,832/2,896/2,1048/2,631,600,601,604,631,5625/2,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,

    // Settings for 512x384 (mode doubled 1024x768):
    512,384,60,1344/2,1024/2,1048/2,1184/2,1344/2,806,768,771,777,806,6500/2,
        BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_NEGATIVE,
    512,384,70,1328/2,1024/2,1048/2,1184/2,1328/2,806,768,771,777,806,7500/2,
        BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_NEGATIVE,
    512,384,75,1312/2,1024/2,1040/2,1136/2,1312/2,800,768,769,772,800,7875/2,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,
    512,384,85,1376/2,1024/2,1072/2,1168/2,1376/2,808,768,769,772,808,9450/2,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,

    // Settings for 640x350:
    640,350,85,832,640,672,736,832,445,350,382,385,445,3150,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_NEGATIVE,

    // Settings for 640x400:
    640,400,60,800,640,656,752,288,449,400,412,414,449,2518,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_NEGATIVE,
    640,400,85,832,640,672,736,832,445,400,401,404,445,3150,
        BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    // Settings for 640x480:
    640,480,60,800,648,656,752,792,525,488,490,492,517,2518,
        BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_NEGATIVE,
    640,480,72,832,648,664,704,824,520,488,489,492,512,3150,
        BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_NEGATIVE,
    640,480,75,840,640,656,720,840,500,480,481,484,500,3150,
        BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_NEGATIVE,
    640,480,85,832,640,696,752,832,509,480,481,484,509,3600,
        BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_NEGATIVE,

    // Settings for 720x400:
    720,400,85,936,720,756,828,936,446,400,401,404,446,3550,
        BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    // Settings for 720x480
    720,480,60,800,728,744,776,792,525,488,490,492,517,2518,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,

    // Settings for 800x600:
    800,600,56,1024,800,824,896,1024,625,600,601,603,625,3600,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,
    800,600,60,1056,800,840,968,1056,628,600,601,605,628,4000,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,
    800,600,72,1040,800,856,976,1040,666,600,637,643,666,5000,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,
    800,600,75,1056,800,816,896,1056,625,600,601,604,625,4950,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,
    800,600,85,1048,800,832,896,1048,631,600,601,604,631,5625,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,

    // Settings for 1024x768:
    1024,768,60,1344,1024,1048,1184,1344,806,768,771,777,806,6500,
        BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_NEGATIVE,
    1024,768,70,1328,1024,1048,1184,1328,806,768,771,777,806,7500,
        BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_NEGATIVE,
    1024,768,75,1312,1024,1040,1136,1312,800,768,769,772,800,7875,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,
    1024,768,85,1376,1024,1072,1168,1376,808,768,769,772,808,9450,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,

    // Settings for 1152x864:
    1152,864,75,1600,1152,1216,1344,1600,900,864,865,868,900,10800,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,

    // Settings for 1280x960:
    1280,960,60,1800,1280,1376,1488,1800,1000,960,961,964,1000,10800,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,
    1280,960,85,1728,1280,1344,1504,1728,1011,960,961,964,1011,14850,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,

    // Settings for 1280x1024:
    1280,1024,60,1688,1280,1328,1440,1688,1066,1024,1025,1028,1066,10800,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,
    1280,1024,75,1688,1280,1296,1440,1688,1066,1024,1025,1028,1066,13500,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,
    1280,1024,85,1728,1280,1344,1504,1728,1072,1024,1025,1028,1072,15750,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,

    // Settings for 1600x1200:
    1600,1200,60,2160,1600,1664,1856,2160,1250,1200,1201,1204,1250,16200,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,
    1600,1200,65,2160,1600,1664,1856,2160,1250,1200,1201,1204,1250,17550,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,
    1600,1200,70,2160,1600,1664,1856,2160,1250,1200,1201,1204,1250,18900,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,
    1600,1200,75,2160,1600,1664,1856,2160,1250,1200,1201,1204,1250,20250,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,
    1600,1200,85,2160,1600,1664,1856,2160,1250,1200,1201,1204,1250,22950,
        BUFFER_HSYNC_POSITIVE,BUFFER_VSYNC_POSITIVE,

    // Settings for 1792x1344:
    1792,1344,60,2448,1792,1920,2120,2448,1394,1344,1345,1348,1394,20475,
        BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1792,1344,75,2456,1792,1888,2104,2456,1417,1344,1345,1348,1417,26100,
        BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    // Settings for 1856x1392:
    1856,1392,60,2528,1856,1952,2176,2528,1439,1392,1393,1396,1439,21825,
        BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1856,1392,75,2560,1856,1984,2208,2560,1500,1392,1393,1396,1500,28800,
        BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    // Settings for 1920x1440:
    1920,1440,60,2600,1920,2048,2256,2600,1500,1440,1441,1444,1500,23400,
        BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1920,1440,75,2640,1920,2064,2288,2640,1500,1440,1441,1444,1500,29700,
        BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    // Flag end of table
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00
};
                   
/*
    Function:   DmtFindMode

    Purpose:    This routine looks for a mode in the DMT list of modes.

    Arguments:  lpModeIn    ptr to MODEINFO structure
                lpModeOut   ptr to MODEOUT  structure

    Returns:    If an DMT mode is found which matches the dwXRes, dwYRes,
                dwBpp, and dwRefreshRate fields in the MODEINFO structure
                pointed to by lpModeIn, then fill in the timing parameters
                in the lpModeOut structure and return TRUE.

                If there is no DMT mode match found, then return FALSE.

    Preserve:   Do not change any fields in lpModeIn.
*/

int CFUNC
DmtFindMode (LPMODEINFO lpModeIn, LPMODEOUT lpModeOut)
{
    int     i;

    i = FindMode(DMTTimingTable, lpModeIn->dwXRes,
                    lpModeIn->dwYRes, lpModeIn->dwRefreshRate);

    // Was the mode found?
    if  (i < 0)
        return  (FALSE);

    // Yes! Convert to final output format
    SetupParamsForVidLutClass (DMTTimingTable + i, lpModeOut, lpModeIn);

    return  (TRUE);
}


/*
    Function:   DmtGetDmtTablePointer

    Purpose:    This routine returns a long pointer to the DMTTimingTable.

    Arguments:  lplpDmtTable    ptr to DMT Table pointer

    Returns:    Pointer to the DMT table.
*/

int CFUNC
DmtGetDmtTablePointer (LPMODESTRUC FAR *lplpDmtTable)
{

    *lplpDmtTable = DMTTimingTable;
    return  (TRUE);
}


/*
    DmtGetMaximumRefreshRate

    This routine calculates the maximum refresh rate for the target mode
    specified given the maimum XRes and YRes, and the maximum horizontal
    and vertical frequencies.

*/

int CFUNC
DmtGetMaximumRefreshRate (ULONG dwXRes, ULONG dwYRes, ULONG dwPixelClk,
                          ULONG *dwRefreshRate)
{
    ULONG   i;

    // Assume at least 60 Hz.
    *dwRefreshRate = 60;

    i = FindModeWithoutRR(DMTTimingTable, dwXRes, dwYRes);

    // Was the mode found?
    if  (i < 0)
        return  (FALSE);

    // Find the mode with the greatest refresh rate which has a pixel
    // clock less than or equal to dwPixelClock.
    for (;DMTTimingTable[i].wHorizVisible != 0x00; i++)
    {
        if ((DMTTimingTable[i].wHorizVisible == (USHORT) dwXRes) &&
            (DMTTimingTable[i].wVertVisible == (USHORT) dwYRes))
        {
            if ((ULONG)DMTTimingTable[i].wDotClock <= dwPixelClk)
                *dwRefreshRate = (ULONG) DMTTimingTable[i].wRefresh;
        }
    }

    return  (TRUE);
}


