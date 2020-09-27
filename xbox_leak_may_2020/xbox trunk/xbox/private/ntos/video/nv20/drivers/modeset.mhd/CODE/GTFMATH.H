/*==========================================================================
;
; Copyright (C) 1999, Nvidia Corporation
;
; File:     gtf.h
;
; Purpose:  This file implements the GTF algorithm
==========================================================================*/

typedef struct  tagGTFIN
{
    ULONG   dwHPixels;
    ULONG   dwVLines;
    ULONG   dwMarginsRqd;
    ULONG   dwIntRqd;
    ULONG   dwIPParmType;
    ULONG   dwIPFreqRqd;
} GTFIN;

typedef GTFIN   FAR *LPGTFIN;

// The entire GTF based computation is based upon the variables provided
// in the GTFIN structure. They are defined as follows:
// HPixels      - the horizontal resolution in Pixels
// VLines       - the vertical resolution in scanlines
// MarginsRqd   - this is a flag -- TRUE if borders should be present
//                FALSE if they shouldn't be. Borders are also referred
//                to as the overscan area.
// IntRqd       - this is a flag -- TRUE if interlace mode is desired
//                FALSE if non-interlace mode is desired.
// IPParmType   - this indicates what the dwIPFreqRqd variables means.
//                assording to the following table.
//  If IPParm is this           IPFreqRqd is this
//  1                           vertical refresh rate in hz
//  2                           horizontal refresh rate in Khz
//  3                           pixel clock rate in Mhz

// These are the tree values that can be assumed by the
// dwIPParmType file din the GTFIN structure.
#define IPTYPE_VERTICAL_REFRESH_RATE    1
#define IPTYPE_HORIZONTAL_REFRESH_RATE  2
#define IPTYPE_PIXEL_CLOCK_RATE         3


typedef struct  tagGTFOUT
{
    ULONG   dwHTotalChars;
    ULONG   dwVTotalScans;
    ULONG   dwHActiveChars;
    ULONG   dwVActiveScans;
    ULONG   dwHBlankStartChar;
    ULONG   dwHFrontPorchChars;
    ULONG   dwHSyncChars;
    ULONG   dwHBackPorchChars;
    ULONG   dwHBlankEndChar;
    ULONG   dwVBlankStartScan;
    ULONG   dwVFrontPorchScans;
    ULONG   dwVSyncScans;
    ULONG   dwVBackPorchScans;
    ULONG   dwVBlankEndScan;
    ULONG   dwPixelClockIn10KHertz;
} GTFOUT;

typedef GTFOUT  FAR *LPGTFOUT;

// For those display devices which don't really need front and back
// porches, those fields can be ignored but they are always valid.


extern ULONG WINAPI ComputeGTFParameters(LPGTFIN lpGtfIn, LPGTFOUT lpGtfOut);
extern ULONG WINAPI Divide32By32(ULONG dwDividend, ULONG dwDivisor);
extern ULONG WINAPI Multiply32By32(ULONG dwMult1, ULONG dwMult2);

