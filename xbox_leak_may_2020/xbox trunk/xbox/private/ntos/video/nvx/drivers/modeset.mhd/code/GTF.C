/*
    FILE:   gtf.c
    DATE:   4/8/99

    This file is the generic entry point for the gtf modeset code.
*/

#include "cmntypes.h"
#include "modeext.h"
#include "modeset.h"
#include "gtf.h"
#include "gtfmath.h"
#include "utils.h"

/*
    Function:   GtfFindMode

    Purpose:    This routine looks for a mode in the GTF list of modes.

    Arguments:  lpModeIn    ptr to MODEINFO structure
                lpModeOut   ptr to MODEOUT structure

    Returns:    Gtf is a formula based mode computation. Given a set
                of input parameters which describe the mode, the
                set of timing parameters to display the mode is
                computed.

                Hence, this routine always returns TRUE and lpModeOut
                is filled in with the corret timing parameters.

    Preserve:   Do not change any fields in lpModeIn.
*/

int CFUNC
GtfFindMode (LPMODEINFO lpModeIn, LPMODEOUT lpModeOut)
{
    GTFIN       GTFIn;
    GTFOUT      GTFOut;
    MODESTRUC   sGTF;

    GTFIn.dwHPixels      = lpModeIn->dwXRes;
    GTFIn.dwVLines       = lpModeIn->dwYRes;
    GTFIn.dwMarginsRqd   = FALSE;
    GTFIn.dwIntRqd       = FALSE;
    GTFIn.dwIPParmType   = IPTYPE_VERTICAL_REFRESH_RATE;
    GTFIn.dwIPFreqRqd    = lpModeIn->dwRefreshRate;

    // Be careful of double scan modes. 512x384 needs to be calculated
    // as if it is 1024x768.
    if (lpModeIn->dwYRes < 400)
    {
        GTFIn.dwHPixels = (GTFIn.dwHPixels * 2);
        GTFIn.dwVLines  = (GTFIn.dwVLines  * 2);
    }

    // Get the timing parameters.
    ComputeGTFParameters(&GTFIn, &GTFOut);

    // Store all parameters in temporary MODESTRUC buffer. Note that
    // horizontal GTF parameters are returned in character clocks and
    // need to be converted to pixels.
    sGTF.wHorizVisible    = (USHORT) (GTFOut.dwHActiveChars << 3);
    sGTF.wVertVisible     = (USHORT) GTFOut.dwVActiveScans;
    sGTF.wRefresh         = (USHORT) lpModeIn->dwRefreshRate;
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

    // Be careful of double scan modes. Need to reduce the horizontals and
    // pixel clock by half since the GTF calculations were done with
    // GTFIn.dwHPixels and GTFIn.dwVLines doubled. All verticals except
    // VertVisible remain the same.
    if (lpModeIn->dwYRes < 400)
    {
        // Add 1 to account for rounding off--otherwise values will
        // be truncated.
        sGTF.wHorizVisible    = ((sGTF.wHorizVisible + 1)/2);
        sGTF.wVertVisible     = ((sGTF.wVertVisible + 1)/2);
        sGTF.wHorizTotal      = ((sGTF.wHorizTotal + 1)/2);
        sGTF.wHorizBlankStart = ((sGTF.wHorizBlankStart + 1)/2);
        sGTF.wHorizSyncStart  = ((sGTF.wHorizSyncStart + 1)/2);
        sGTF.wHorizSyncEnd    = ((sGTF.wHorizSyncEnd + 1)/2);
        sGTF.wHorizBlankEnd   = ((sGTF.wHorizBlankEnd + 1)/2);
        sGTF.wDotClock        = ((sGTF.wDotClock + 1)/2);
    }

    // Convert to final output format
    SetupParamsForVidLutClass (&sGTF, lpModeOut, lpModeIn);

    return  (TRUE);
}

