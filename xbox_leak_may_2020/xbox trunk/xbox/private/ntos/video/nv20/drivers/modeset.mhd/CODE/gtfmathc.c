#if !defined(_WIN32) || defined(_WIN64)
/*
;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gtfmath.c (ported from gtfmath.asm)
;
; Purpose:      This file implements the GTF algorithm
;
;       Just some important comments and definitions extracted from
;       the GTF document.
;
;       Horizontal - negative
;       Vertical - positive
;       composite - negative
;
;       Blanking Duty Cycle = (1 - tA / T) * 100%
;                     = (       1 - (F * n) / p) * 100%
;               tA = active video time
;               T  = Horizontal period
;               F  = horizontal frequency
;               n  = number of active pixels (Xresolution)
;               p = pixel clock frequency
;
;       Basic form of equation is:
;       Blanking Duty Cycle = C' - (M' / F)
;
;       C' = ((C - J) * (K / 256) + J
;       M' = (K / 256) * M
;
;       Margin% = 1.8%
;       size of top and bottom overscan margin as % of active vertical
;       size of left and right overscan margin as % of active vertical
;
;       CellGran = 8 pixels
;       character cell granularity
;
;       MinPorch = 1 line / 1 char cell
;       Minimum front porch in lines and character cells
;
;       VsyncRqd = 3 lines
;       width of the vsync pulse in scanlines
;
;       HSyncPercent = 8%
;       The width of HSync as a percentage of the total line period
;
;       MinVSyncPlusBackPorch  550us
;       minimum time for vertical sync plus back porch
;
;       M       600% / Khz
;       blanking formula gradient
;
;       C       40%
;       blanking formula offset
;
;       K       128
;       blanking formula scaling factor
;
;       J       20%
;       blanking formula scaling factor weighting
;
;       HPixels    - the horizontal resolution
;       VLines     - the vertical resolution
;       MarginsRqd - this is a flag -- TRUE if borders, FLASE if none
;       IntRqd     - this is a flag -- TRUE if interlace, FALSE otherwise
;       IPParm     - this indicates what the last specifying parm will be
;               1  - VFrameRateRqd
;               2  - HFreq
;               3  - PixelClock
;       IPFreqRqd  - Depends upon VPParm
;               If IPParm is this       IPFreqRqd is this
;               1                       vertical refresh rate in hz
;               2                       horizontal refresh rate in Kilohz
;               3                       pixel clock rate in Megahz
;
;       Interestingly enough, the GTF spec says that any monitor that
;       has the GTF bit set, MUST have a Monitor Description Block in
;       the EDID that tells the monitor frequency limits. If this block
;       does not exist, then the monitor should be trated as non-GTF
;       compliant. Byte 10, bit 0 = 1 means that overscan margins are
;       required.
;==============================================================================
*/
#include "cmntypes.h"
#include "gtfmath.h"

/*
 * These math routines need to be defined in the OS specific code.
 */
float calcROUND(float);
float calcFLOOR(float);
float calcSQRT(float);

#define const_fZero                     0.0f
#define const_fZeroPointFour            0.4f
#define const_fZeroPointFive            0.5f
#define const_fOne                      1.0f
#define const_fOnePointEight            1.8f
#define const_fTwo                      2.0f
#define const_fThree                    3.0f
#define const_fEight                    8.0f
#define const_fTwenty                   20.0f
#define const_fForty                    40.0f
#define const_fOneHundred               100.0f
#define const_fOneHundredTwentyEight    128.0f
#define const_fTwoHundredFiftySix       256.0f
#define const_fFiveHundredFifty         550.0f
#define const_fSixHundred               600.0f
#define const_fOneThousand              1000.0f
#define const_fOneMillion               1000000.0f

static float    fTemp;
static float    fVLinesRnd;
static float    fAddrLinesPerFrame;
static float    fCellGran;
static float    fPixelFreq;
static float    fCharTime;
static float    fTopMarginLines;
static float    fBottomMarginLines;
static float    fVSyncPlusBackPorch;
static float    fMinPorch;
static float    fInterlace;
static float    fTotalLinesPerFrame;
static float    fTotalPixels;
static float    fTotalHTimeChars;
static float    fHPixelsRnd;
static float    fHAddrTime;
static float    fHAddrTimeChars;
static float    fHBlankPixels;
static float    fHBlank;
static float    fHBlankChars;
static float    fLeftMarginPixels;
static float    fRightMarginPixels;
static float    fHBlankPlusMargin;
static float    fHBlankPlusMarginChars;
static float    fActualDutyCycle;
static float    fBlankPlusMarginDutyCycle;
static float    fLeftMargin;
static float    fLeftMarginChars;
static float    fRightMargin;
static float    fRightMarginChars;
static float    fHSyncPercent;
static float    fHSyncPixels;
static float    fHFrontPorchPixels;
static float    fHBackPorchPixels;
static float    fHSyncChars;
static float    fHSync;
static float    fHFrontPorchChars;
static float    fHFrontPorch;
static float    fHBackPorchChars;
static float    fHBackPorch;
static float    fTotalVLines;
static float    fHPeriod;
static float    fVFramePeriod;
static float    fVFieldPeriod;
static float    fVAddrTimePerFrame;
static float    fVAddrTimePerField;
static float    fVOddBlankingLines;
static float    fVOddBlanking;
static float    fVEvenBlankingLines;
static float    fVEvenBlanking;
static float    fTopMargin;
static float    fVOddFrontPorch;
static float    fVOddFrontPorchLines;
static float    fVEvenFrontPorch;
static float    fVSyncRqd;
static float    fVSync;
static float    fVEvenBackPorch;
static float    fVBackPorch;
static float    fVEvenBackPorchLines;
static float    fVOddBackPorch;
static float    fBottomMargin;
static float    fMarginPercent;
static float    fMinVSyncPlusBackPorch;
static float    fGTF_M;
static float    fGTF_C;
static float    fGTF_J;
static float    fGTF_K;
static float    fGTF_CPrime;
static float    fGTF_MPrime;
static float    fVFieldRateRqd;
static float    fHPeriodEst;
static float    fVFieldRateEst;
static float    fVFieldRate;
static float    fVFrameRate;
static float    fTotalActivePixels;
static float    fIdealDutyCycle;
static float    fHFreq;
static float    fIdealHPeriod;
/*
;==============================================================================
;
; Function:     SetupGTFConstants
;
; Purpose:      This function sets up all the "constants" used by
;               the GTF calculation. I make these constants into
;               variables because the spec says to -- at some point
;               in the future the GTF spec may be extended and they
;               want the flexibility to adjust these "constants" to
;               new values. By making the constants into variables
;               I just make the upgrade path easy.
;
; Returns:      None
;
;==============================================================================
*/
static void SetupGTFConstants
(
    void
)
{
    /*
     * This is the size of the top (or bottom -- they are the same)
     * overscan area as a percentage of the total vertcical time.
     */
    fMarginPercent = const_fOnePointEight;
    /*
     * Number of pixels per character cell
     */
    fCellGran = const_fEight;
    /*
     * Minimum horizontal front porch in character cells and
     * minimum vertical front porch in scanlines (they are the same.)
     */
    fMinPorch = const_fOne;
    /*
     * Width of vsync pulse in scanlines
     */
    fVSyncRqd = const_fThree;
    /*
     * Width of the hsync pulse as a percentage of horizontal line time
     */
    fHSyncPercent = const_fEight;
    /*
     * minimum time for vertical sync plus back porch in microseconds
     */
    fMinVSyncPlusBackPorch = const_fFiveHundredFifty;
    /*
     * These apply to the Basic Blanking Duty Cycle Equation.
     * See the top of the file.
     */
    fGTF_M = const_fSixHundred;
    fGTF_C = const_fForty;
    fGTF_K = const_fOneHundredTwentyEight;
    fGTF_J = const_fTwenty;
    /*
     * This is forced by GTF definitions
     * C' = ((C - J) * (K / 256) + J
     *         fld     fGTF_K
     *         fdiv    fTwoHundredFiftySix
     *         fld     fGTF_C
     *         fsub    fGTF_J
     *         fmulp   ST(1),ST(0)
     *         fadd    fGTF_J
     *         fstp    fGTF_CPrime
     */
    fGTF_CPrime = ((fGTF_C - fGTF_J) 
                *  (fGTF_K / const_fTwoHundredFiftySix))
                + fGTF_J;
    /*
     * This is forced by GTF definitions
     * M' = (K / 256) * M
     */
    fGTF_MPrime = (fGTF_K / const_fTwoHundredFiftySix) * fGTF_M;
    return;
}
/*
;==============================================================================
;
; Function:     VerticalRefreshToStage1Parameters
;
; Purpose:      This function compautes GTF stage 1 parameters assuming
;               that the following parameters are given in the GTFIn struct.
;
;               These are always required by GTF:
;               HPixels    - the horizontal resolution
;               VLines     - the vertical resolution
;               MarginsRqd - this is a flag -- TRUE if borders, FLASE if none
;               IntRqd     - this is a flag -- TRUE if interlace, else FALSE
;
;               For this computation these parameters are:
;               IPParm     - This value is IPTYPE_VERTICAL_REFRESH_RATE
;               IPFreqRqd  - vertical refresh rate in hertz
;
; Returns:      None
;
;==============================================================================
*/
static void VerticalRefreshToStage1Parameters
(
    LPGTFIN  lpGtfIn
)
{
    /*
     * 1) Make sure that the XResolution is a multiple of char cell size
     *
     * [H PIXELS RND] = ROUND([H PIXELS] / [CELL GRAN RND], 0) * [CELL GRAN RND]
     */
    fHPixelsRnd = calcROUND((float)lpGtfIn->dwHPixels / fCellGran) * fCellGran;
    /*
     * 2) If interlace is requested, half the number of vertical lines
     *    since the calculation is really vertical lines per field
     *
     * [V LINES RND] = IF([INT RQD?]="y", ROUND([V LINES] / 2, 0),
     *                                    ROUND([V LINES], 0))
     */
    fVLinesRnd = (float)lpGtfIn->dwVLines;
    if (lpGtfIn->dwIntRqd != 0)
        fVLinesRnd /= const_fTwo;
    fVLinesRnd = calcROUND(fVLinesRnd);
    /*
     * 3) Find the frame rate required
     *
     * [V FIELD RATE RQD] = IF([INT RQD?]="y", [I/P FREQ RQD] * 2,
     *                                         [I/P FREQ RQD])
     */
    fVFieldRateRqd = (float)lpGtfIn->dwIPFreqRqd;
    if (lpGtfIn->dwIntRqd != 0)
        fVFieldRateRqd += fVFieldRateRqd;
    /*
     * 4) Find the number of lines in the top margin
     *
     * [TOP MARGIN (LINES)] = IF([MARGINS RQD?]="Y", ROUND(([MARGIN%] / 100 * [V LINES RND]), 0),
     *                                               0)
     */
    if (lpGtfIn->dwMarginsRqd != 0)
        fTopMarginLines = calcROUND(fMarginPercent / const_fOneHundred * fVLinesRnd);
    else
        fTopMarginLines = const_fZero;
    /*
     * 5) Find the number of lines in the bottom margin
     *
     * [BOT MARGIN (LINES)] = IF([MARGINS RQD?]="Y", ROUND(([MARGIN%] / 100 * [V LINES RND]), 0),
     *                                               0)
     */
    if (lpGtfIn->dwMarginsRqd != 0)
        fBottomMarginLines = calcROUND(fMarginPercent / const_fOneHundred * fVLinesRnd);
    else
        fBottomMarginLines = const_fZero;
    /*
     * 6) If interlace is required, set dwInterlace to 0.5
     *
     * [INTERLACE]=IF([INT RQD?] = "y", 0.5,
     *                                  0)
     */
    if (lpGtfIn->dwIntRqd != 0)
        fInterlace = const_fZeroPointFive;
    else
        fInterlace = const_fZero;
    /*
     * 7) Estimate the horizontal period
     *
     * [H PERIOD EST] = ((1 / [V FIELD RATE RQD]) - [MIN VSYNC+BP] / 1000000)
     *                / ([V LINES RND] + (2 * [TOP MARGIN (LINES)]) + [MIN PORCH RND] + [INTERLACE])
     *                * 1000000
     */
    fHPeriodEst = ((const_fOne / fVFieldRateRqd) - fMinVSyncPlusBackPorch / const_fOneMillion)
                / (fVLinesRnd + (const_fTwo * fTopMarginLines) + fMinPorch + fInterlace)
                * const_fOneMillion;
    /*
     * 8) Find the number of lines in fMinVSyncPlusBackPorch
     *
     * [V SYNC+BP] = ROUND(([MIN VSYNC+BP] / [H PERIOD EST]) ,0)
     */
    fVSyncPlusBackPorch = calcROUND(fMinVSyncPlusBackPorch / fHPeriodEst);
    /*
     * 9) Find the number of lines in VBackPorch alone
     *
     * [V BACK PORCH] = [V SYNC+BP] - [V SYNC RND]
     */
    fVBackPorch = fVSyncPlusBackPorch - fVSyncRqd;
    /*
     * 10) Find the total number of lines in the vertical field
     *
     * [TOTAL V LINES] = [V LINES RND] + [TOP MARGIN (LINES)] + [BOT MARGIN (LINES)]
     *                 + [V SYNC+BP] + [INTERLACE] + [MIN PORCH RND]
     */
    fTotalVLines = fVLinesRnd
                 + fTopMarginLines
                 + fBottomMarginLines
                 + fVSyncPlusBackPorch
                 + fInterlace
                 + fMinPorch;
    /*
     * 11) Estimate the vertical field frequency
     *
     * [V FIELD RATE EST] = 1 / [H PERIOD EST] / [TOTAL V LINES] * 1000000
     */
    fVFieldRateEst = const_fOne / fHPeriodEst / fTotalVLines * const_fOneMillion;
    /*
     * 12) Find the actual horizontal period
     *
     * [H PERIOD] = [H PERIOD EST] / ([V FIELD RATE RQD] / [V FIELD RATE EST])
     */
    fHPeriod = fHPeriodEst / (fVFieldRateRqd / fVFieldRateEst);
    /*
     * 13) Find the actual vertical field frequency
     *
     * [V FIELD RATE] = 1 / [H PERIOD] / [TOTAL V LINES] * 1000000
     */
    fVFieldRate = const_fOne / fHPeriod / fTotalVLines * const_fOneMillion;
    /*
     * 14) Find the vertical frame frequency
     *
     * [V FRAME RATE] = IF([INT RQD?]="y", [V FIELD RATE] / 2,
     *                                     [V FIELD RATE])
     */
    fVFrameRate = fVFieldRate;
    if (lpGtfIn->dwIntRqd != 0)
        fVFrameRate /= const_fTwo;
    /*
     * 15) Find the number of pixels in the left margin
     *
     * [LEFT MARGIN (PIXELS)] = IF([MARGINS RQD?]="Y", ROUND([H PIXELS RND] * [MARGIN%] / 100 / [CELL GRAN RND], 0) * [CELL GRAN RND],
     *                                                 0)
     */
    if (lpGtfIn->dwMarginsRqd)
    {
        fLeftMarginPixels = calcROUND(fHPixelsRnd * fMarginPercent / const_fOneHundred / fCellGran)
                          * fCellGran;
    }
    else
        fLeftMarginPixels = const_fZero;
    /*
     * 16) Find the number of pixels in the right margin
     *
     * [RIGHT MARGIN (PIXELS)] = IF([MARGINS RQD?]="Y", ROUND([H PIXELS RND] * [MARGIN%] / 100 / [CELL GRAN RND], 0) * [CELL GRAN RND],
     *                                                  0)
     */
    if (lpGtfIn->dwMarginsRqd != 0)
    {
        fRightMarginPixels = calcROUND(fHPixelsRnd * fMarginPercent / const_fOneHundred / fCellGran)
                           * fCellGran;
    }
    else
        fRightMarginPixels = const_fZero;
    /*
     * 17) Find total number of active pixels in image+left+right margin
     *
     * [TOTAL ACTIVE PIXELS] = [H PIXELS RND] + [LEFT MARGIN (PIXELS)] + [RIGHT MARGIN (PIXELS)]
     */
    fTotalActivePixels = fHPixelsRnd
                       + fLeftMarginPixels
                       + fRightMarginPixels;
    /*
     * 18) Find the ideal Blanking duty cycle
     *
     * [IDEAL DUTY CYCLE] = [C'] - ([M'] * [H PERIOD] / 1000)
     */
    fIdealDutyCycle = fGTF_CPrime
                    - (fGTF_MPrime * fHPeriod / const_fOneThousand);
    /*
     * 19) Find the number of pixels in the blanking time to the
     * nearest double character cell
     *
     * [H BLANK (PIXELS)] = ROUND([TOTAL ACTIVE PIXELS] * [IDEAL DUTY CYCLE]
     *                    /       (100 - [IDEAL DUTY CYCLE])
     *                    /       (2 * [CELL GRAN RND]), 0)
     *                    * (2 * [CELL GRAN RND])
     */
    fHBlankPixels = calcROUND((fTotalActivePixels * fIdealDutyCycle
                  /            (const_fOneHundred - fIdealDutyCycle)
                  /            (const_fTwo * fCellGran)))
                  * (const_fTwo * fCellGran);
    /*
     * 20) Find the total number of pixels
     *
     * [TOTAL PIXELS] = [TOTAL ACTIVE PIXELS] + [H BLANK (PIXELS)]
     */
    fTotalPixels = fTotalActivePixels + fHBlankPixels;
    /*
     * 21) Find the pixel clock frequency
     *
     * [PIXEL FREQ] = [TOTAL PIXELS] / [H PERIOD]
     */
    fPixelFreq = fTotalPixels / fHPeriod;
    /*
     * 22) Find the horizontal frequency
     *
     * [H FREQ] = 1000 / [H PERIOD]
     */
    fHFreq = const_fOneThousand / fHPeriod;
    return;
}
/*
;==============================================================================
;
; Function:     HorizontalRefreshToStage1Parameters
;
; Purpose:      This function compautes GTF stage 1 parameters assuming
;               that the following parameters are given:
;
;               These are always required by GTF:
;               HPixels    - the horizontal resolution
;               VLines     - the vertical resolution
;               MarginsRqd - this is a flag -- TRUE if borders, FLASE if none
;               IntRqd     - this is a flag -- TRUE if interlace, else FALSE
;
;               For this computation these parameters are:
;               IPParm     - IPTYPE_HORIZONTAL_REFRESH_RATE
;               IPFreqRqd  - horizontal refresh rate in Kilohertz
;
; Returns:      None
;
;==============================================================================
*/
static void HorizontalRefreshToStage1Parameters
(
    LPGTFIN  lpGtfIn
)
{
    /*
     * 1) Make sure that the XResolution is a multiple of char cell size
     *         fild    ds:[ebx].GTFIN.dwHPixels
     *         fdiv    fCellGran
     *         fistp   dwTemp
     *         fild    dwTemp
     *         fmul    fCellGran
     *         fstp    fHPixelsRnd
     */
    fHPixelsRnd = calcROUND((float)lpGtfIn->dwHPixels / fCellGran)
                * fCellGran;
    /*
     * 2) If interlace is requested, half the number of vertical lines
     *    since the calculation is really vertical lines per field
     *         fild    ds:[ebx].GTFIN.dwVLines
     *         cmp     ds:[ebx].GTFIN.dwIntRqd,0
     *         je      @F
     *         fdiv    fTwo
     * @@:     fstp    fVLinesRnd
     */
    fVLinesRnd = (float)lpGtfIn->dwVLines;
    if (lpGtfIn->dwIntRqd != 0)
        fVLinesRnd /= const_fTwo;
    fVLinesRnd = calcROUND(fVLinesRnd);
    /*
     * 3) Find the horizontal freqency required
     *         fild    ds:[ebx].GTFIN.dwIPFreqRqd
     *         fstp    fHFreq
     */
    fHFreq = (float)lpGtfIn->dwIPFreqRqd;
    /*
     * 4) Find the number of lines in the top margin
     *         sub     eax,eax
     *         cmp     ds:[ebx].GTFIN.dwMarginsRqd,0
     *         je      @F
     *         fld     fMarginPercent
     *         fmul    fVLinesRnd
     *         fdiv    fOneHundred
     * @@:     fstp    fTopMarginLines
     */
    if (lpGtfIn->dwMarginsRqd != 0)
        fTopMarginLines = calcROUND(fMarginPercent / const_fOneHundred * fVLinesRnd);
    else
        fTopMarginLines = const_fZero;
    /*
     * 5) Find the number of lines in the bottom margin
     *         sub     eax,eax
     *         cmp     ds:[ebx].GTFIN.dwMarginsRqd,0
     *         je      @F
     *         fld     fMarginPercent
     *         fmul    fVLinesRnd
     *         fdiv    fOneHundred
     * @@:     fstp    fBottomMarginLines
     */
    if (lpGtfIn->dwMarginsRqd != 0)
        fBottomMarginLines = fMarginPercent / const_fOneHundred * fVLinesRnd;
    else
        fBottomMarginLines = const_fZero;
    /*
     * 6) If interlace is required, set dwInterlace to 0.5
     *         fldz
     *         cmp     ds:[ebx].GTFIN.dwIntRqd,0
     *         je      @F
     *         faddp   ST(0),ST(0)             ; pop the stack
     *         fld     fZeroPointFive
     * @@:     fstp    fInterlace
     */
    if (lpGtfIn->dwIntRqd != 0)
        fInterlace = const_fZeroPointFive;
    else
        fInterlace = const_fZero;
    /*
     * 7) Find the number of lines in fMinVSyncPlusBackPorch
     *         fld     fMinVSyncPlusBackPorch
     *         fmul    fHFreq
     *         fdiv    fOneThousand
     *         fistp   dwTemp
     *         fild    dwTemp
     *         fstp    fVSyncPlusBackPorch
     */
    fVSyncPlusBackPorch = calcROUND(fMinVSyncPlusBackPorch * fHFreq / const_fOneThousand);
    /*
     * 8) Find the number of lines in VBackPorch alone
     *         fld     fVSyncPlusBackPorch
     *         fsub    fVSyncRqd
     *         fstp    fVBackPorch
     */
    fVBackPorch = fVSyncPlusBackPorch - fVSyncRqd;
    /*
     * 9) Find the total number of lines in the vertical field
     *         fld     fVLinesRnd
     *         fadd    fTopMarginLines
     *         fadd    fBottomMarginLines
     *         fadd    fVSyncPlusBackPorch
     *         fadd    fMinPorch
     *         fadd    fInterlace
     *         fstp    fTotalVLines
     */
    fTotalVLines = fVLinesRnd
                 + fTopMarginLines
                 + fBottomMarginLines
                 + fInterlace
                 + fVSyncPlusBackPorch
                 + fMinPorch;
    /*
     * 10) Find the actual vertical field frequency
     *         fld     fHFreq
     *         fdiv    fTotalVLines
     *         fmul    fOneThousand
     *         fstp    fVFieldRate
     */
    fVFieldRate = fHFreq / fTotalVLines * const_fOneThousand;
    /*
     * 11) Find the vertical frame frequency
     *         fld     fVFieldRate
     *         cmp     ds:[ebx].GTFIN.dwIntRqd,0
     *         je      @F
     *         fdiv    fTwo
     * @@:     fstp    fVFrameRate
     */
    fVFrameRate = fVFieldRate;
    if (lpGtfIn->dwIntRqd != 0)
        fVFrameRate /= const_fTwo;
    /*
     * 12) Find the number of pixels in the left margin
     *         fldz
     *         cmp     ds:[ebx].GTFIN.dwMarginsRqd,0
     *         je      @F
     *         faddp   ST(0),ST(0)             ; pop the stack
     *         fld     fHPixelsRnd
     *         fmul    fMarginPercent
     *         fdiv    fOneHundred
     *         fdiv    fCellGran
     *         fistp   dwTemp
     *         fild    dwTemp
     *         fmul    fCellGran
     * @@:     fstp    fLeftMarginPixels
     */
    if (lpGtfIn->dwMarginsRqd != 0)
    {
        fLeftMarginPixels = calcROUND(fHPixelsRnd * fMarginPercent / const_fOneHundred / fCellGran)
                          * fCellGran;
    }
    else
        fLeftMarginPixels = const_fZero;
    /*
     * 13) Find the number of pixels in the right margin
     *         fldz
     *         cmp     ds:[ebx].GTFIN.dwMarginsRqd,0
     *         je      @F
     *         faddp   ST(0),ST(0)             ; pop the stack
     *         fld     fHPixelsRnd
     *         fmul    fMarginPercent
     *         fdiv    fOneHundred
     *         fdiv    fCellGran
     *         fistp   dwTemp
     *         fild    dwTemp
     *         fmul    fCellGran
     * @@:     fstp    fRightMarginPixels
     */
    if (lpGtfIn->dwMarginsRqd != 0)
    {
        fRightMarginPixels = calcROUND(fHPixelsRnd * fMarginPercent / const_fOneHundred / fCellGran)
                           * fCellGran;
    }
    else
        fRightMarginPixels = const_fZero;
    /*
     * 14) Find total number of active pixels in image+left+right margin
     *         fld     fHPixelsRnd
     *         fadd    fLeftMarginPixels
     *         fadd    fRightMarginPixels
     *         fstp    fTotalActivePixels
     */
    fTotalActivePixels = fHPixelsRnd
                       + fLeftMarginPixels
                       + fRightMarginPixels;
    /*
     * 15) Find the ideal Blanking duty cycle
     *         fld     fGTF_CPrime
     *         fld     fGTF_MPrime
     *         fdiv    fHFreq
     *         fsubp   ST(1),ST(0)
     *         fstp    fIdealDutyCycle
     */
    fIdealDutyCycle = fGTF_CPrime - (fGTF_MPrime / fHFreq);
    /*
     * 16) Find the number of pixels in the blanking time to the
     * nearest double character cell
     *         fld     fTotalActivePixels
     *         fmul    fIdealDutyCycle
     *         fld     fOneHundred
     *         fsub    fIdealDutyCycle
     *         fdivp   ST(1),ST(0)
     *         fld     fCellGran
     *         fadd    ST(0),ST(0)
     *         fdivp   ST(1),ST(0)
     *         fistp   dwTemp
     *         fild    dwTemp
     *         fadd    ST(0),ST(0)
     *         fmul    fCellGran
     *         fstp    fHBlankPixels
     */
    fHBlankPixels = calcROUND((fTotalActivePixels * fIdealDutyCycle
                  /            (const_fOneHundred - fIdealDutyCycle)
                  /            (const_fTwo * fCellGran)))
                  * (const_fTwo *fCellGran);
    /*
     * 17) Find the total number of pixels
     *         fld     fTotalActivePixels
     *         fadd    fHBlankPixels
     *         fstp    fTotalPixels
     */
    fTotalPixels = fTotalActivePixels + fHBlankPixels;
    /*
     * 18) Find the horizontal frequency
     *         fld     fOneThousand
     *         fdiv    fHFreq
     *         fstp    fHPeriod
     */
    fHPeriod = const_fOneThousand / fHFreq;
    /*
     * 19) Find the pixel clock frequency
     *         fld     fTotalPixels
     *         fmul    fHFreq
     *         fdiv    fOneThousand
     *         fstp    fPixelFreq
     */
    fPixelFreq = fTotalPixels * fHFreq / const_fOneThousand;
    return;
}
/*
;==============================================================================
;
; Function:     PixelClockToStage1Parameters
;
; Purpose:      This function compautes GTF stage 1 parameters assuming
;               that the following parameters are given:
;
;               These are always required by GTF:
;               HPixels    - the horizontal resolution
;               VLines     - the vertical resolution
;               MarginsRqd - this is a flag -- TRUE if borders, FLASE if none
;               IntRqd     - this is a flag -- TRUE if interlace, else FALSE
;
;               For this computation these parameters are:
;               IPParm     - IPTYPE_PIXEL_CLOCK_RATE
;               IPFreqRqd  - pixel clock rate in Megahertz
;
; Returns:      None
;
;==============================================================================
*/
static void PixelClockToStage1Parameters
(
    LPGTFIN  lpGtfIn
)
{
    /*
     * 1) Make sure that the XResolution is a multiple of char cell size
     *         fild    ds:[ebx].GTFIN.dwHPixels
     *         fdiv    fCellGran
     *         fistp   dwTemp
     *         fild    dwTemp
     *         fmul    fCellGran
     *         fstp    fHPixelsRnd
     */
    fHPixelsRnd = calcROUND((float)lpGtfIn->dwHPixels / fCellGran)
                * fCellGran;
    /*
     * 2) If interlace is requested, half the number of vertical lines
     *    since the calculation is really vertical lines per field
     *         fild    ds:[ebx].GTFIN.dwVLines
     *         cmp     ds:[ebx].GTFIN.dwIntRqd,0
     *         je      @F
     *         fdiv    fTwo
     * @@:     fstp    fVLinesRnd
     */
    fVLinesRnd = (float)lpGtfIn->dwVLines;
    if (lpGtfIn->dwIntRqd != 0)
        fVLinesRnd /= const_fTwo;
    fVLinesRnd = calcROUND(fVLinesRnd);
    /*
     * 3) Find the horizontal freqency required
     *         fild    ds:[ebx].GTFIN.dwIPFreqRqd
     *         fstp    fPixelFreq
     */
    fPixelFreq = (float)lpGtfIn->dwIPFreqRqd;
    /*
     * 4) Find the number of pixels in the left margin
     *         fldz
     *         cmp     ds:[ebx].GTFIN.dwMarginsRqd,0
     *         je      @F
     *         faddp   ST(0),ST(0)             ; pop the stack
     *         fld     fHPixelsRnd
     *         fmul    fMarginPercent
     *         fdiv    fOneHundred
     *         fdiv    fCellGran
     *         fistp   dwTemp
     *         fild    dwTemp
     *         fmul    fCellGran
     * @@:     fstp    fLeftMarginPixels
     */
    if (lpGtfIn->dwMarginsRqd != 0)
    {
        fLeftMarginPixels = calcROUND(fHPixelsRnd * fMarginPercent / const_fOneHundred / fCellGran)
                          * fCellGran;
    }
    else
        fLeftMarginPixels = const_fZero;
    /*
     * 5) Find the number of pixels in the right margin
     *         fldz
     *         cmp     ds:[ebx].GTFIN.dwMarginsRqd,0
     *         je      @F
     *         faddp   ST(0),ST(0)             ; pop the stack
     *         fld     fHPixelsRnd
     *         fmul    fMarginPercent
     *         fdiv    fOneHundred
     *         fdiv    fCellGran
     *         fistp   dwTemp
     *         fild    dwTemp
     *         fmul    fCellGran
     * @@:     fstp    fRightMarginPixels
     */
    if (lpGtfIn->dwMarginsRqd != 0)
    {
        fRightMarginPixels = calcROUND(fHPixelsRnd * fMarginPercent / const_fOneHundred / fCellGran)
                           * fCellGran;
    }
    else
        fRightMarginPixels = const_fZero;
    /*
     * 6) Find total number of active pixels in image+left+right margin
     *         fld     fHPixelsRnd
     *         fadd    fLeftMarginPixels
     *         fadd    fRightMarginPixels
     *         fstp    fTotalActivePixels
     */
    fTotalActivePixels = fHPixelsRnd
                       + fLeftMarginPixels
                       + fRightMarginPixels;
    /*
     * 7) Find the ideal horizontal period from blanking duty cycle Eq
     *         fld     fOneHundred
     *         fsub    fGTF_CPrime
     *         fmul    ST(0),ST(0)
     *         fld     fTotalActivePixels
     *         fadd    fLeftMarginPixels
     *         fadd    fRightMarginPixels
     *         fmul    fGTF_MPrime
     *         fmul    fZeroPointFour
     *         fdiv    fPixelFreq
     *         faddp   ST(1),ST(0)
     *         fsqrt
     *         fadd    fGTF_CPrime
     *         fsub    fOneHundred
     *         fdiv    fTwo
     *         fmul    fGTF_MPrime
     *         fmul    fOneThousand
     *         fstp    fIdealHPeriod
     */
    fTemp         = const_fOneHundred - fGTF_CPrime;
    fTemp        *= fTemp;
    fTemp         = fTemp
                  + (const_fZeroPointFour * fGTF_MPrime 
                  *  (fTotalActivePixels + fLeftMarginPixels + fRightMarginPixels)
                  /  fPixelFreq);
    fIdealHPeriod = ((fGTF_CPrime - const_fOneHundred)
                  +  (float)calcSQRT(fTemp))
                  / const_fTwo
                  / fGTF_MPrime
                  * const_fOneThousand;
    /*
     * 8) Find the ideal Blanking duty cycle
     *         fld     fGTF_CPrime
     *         fld     fGTF_MPrime
     *         fmul    fIdealHPeriod
     *         fdiv    fOneThousand
     *         fsubp   ST(1),ST(0)
     *         fstp    fIdealDutyCycle
     */

    fIdealDutyCycle = fGTF_CPrime - 
                      (fGTF_MPrime * fIdealHPeriod / const_fOneThousand);
    /*
     * 9) Find the number of pixels in the blanking time to the
     * nearest double character cell
     *         fld     fTotalActivePixels
     *         fmul    fIdealDutyCycle
     *         fld     fOneHundred
     *         fsub    fIdealDutyCycle
     *         fdivp   ST(1),ST(0)
     *         fld     fCellGran
     *         fadd    ST(0),ST(0)
     *         fdivp   ST(1),ST(0)
     *         fistp   dwTemp
     *         fild    dwTemp
     *         fadd    ST(0),ST(0)
     *         fmul    fCellGran
     *         fstp    fHBlankPixels
     */
    fHBlankPixels = calcROUND((fTotalActivePixels * fIdealDutyCycle
                  /            (const_fOneHundred - fIdealDutyCycle)
                  /            (const_fTwo * fCellGran)))
                  * (const_fTwo * fCellGran);
    /*
     * 10) Find the total number of pixels
     *         fld     fTotalActivePixels
     *         fadd    fHBlankPixels
     *         fstp    fTotalPixels
     */
    fTotalPixels = fTotalActivePixels + fHBlankPixels;
    /*
     * 11) Find the horizontal frequency
     *         fld     fPixelFreq
     *         fdiv    fTotalPixels
     *         fmul    fOneThousand
     *         fstp    fHFreq
     */
    fHFreq = fPixelFreq / fTotalPixels * const_fOneThousand;
    /*
     * 12) Find the horizontal period
     *         fld     fOneThousand
     *         fdiv    fHFreq
     *         fstp    fHPeriod
     */
    fHPeriod = const_fOneThousand / fHFreq;
    /*
     * 13) Find the number of lines in the top margin
     *         sub     eax,eax
     *         cmp     ds:[ebx].GTFIN.dwMarginsRqd,0
     *         je      @F
     *         fld     fMarginPercent
     *         fmul    fVLinesRnd
     *         fdiv    fOneHundred
     * @@:     fstp    fTopMarginLines
     */
    if (lpGtfIn->dwMarginsRqd != 0)
        fTopMarginLines = calcROUND(fMarginPercent / const_fOneHundred * fVLinesRnd);
    else
        fTopMarginLines = const_fZero;
    /*
     * 14) Find the number of lines in the bottom margin
     *         sub     eax,eax
     *         cmp     ds:[ebx].GTFIN.dwMarginsRqd,0
     *         je      @F
     *         fld     fMarginPercent
     *         fmul    fVLinesRnd
     *         fdiv    fOneHundred
     * @@:     fstp    fBottomMarginLines
     */
    if (lpGtfIn->dwMarginsRqd != 0)
        fBottomMarginLines = calcROUND(fMarginPercent / const_fOneHundred * fVLinesRnd);
    else
        fBottomMarginLines = const_fZero;
    /*
     * 15) If interlace is required, set dwInterlace to 0.5
     *         fldz
     *         cmp     ds:[ebx].GTFIN.dwIntRqd,0
     *         je      @F
     *         faddp   ST(0),ST(0)             ; pop the stack
     *         fld     fZeroPointFive
     * @@:     fstp    fInterlace
     */
    if (lpGtfIn->dwIntRqd != 0)
        fInterlace = const_fZeroPointFive;
    else
        fInterlace = const_fZero;
    /*
     * 16) Find the number of lines in fMinVSyncPlusBackPorch
     *         fld     fMinVSyncPlusBackPorch
     *         fmul    fHFreq
     *         fdiv    fOneThousand
     *         fstp    fVSyncPlusBackPorch
     */
    fVSyncPlusBackPorch = calcROUND(fMinVSyncPlusBackPorch * fHFreq / const_fOneThousand);
    /*
     * 17) Find the number of lines in VBackPorch alone
     *         fld     fVSyncPlusBackPorch
     *         fsub    fVSyncRqd
     *         fstp    fVBackPorch
     */
    fVBackPorch = fVSyncPlusBackPorch - fVSyncRqd;
    /*
     * 18) Find the total number of lines in the vertical field
     *         fld     fVLinesRnd
     *         fadd    fTopMarginLines
     *         fadd    fBottomMarginLines
     *         fadd    fVSyncPlusBackPorch
     *         fadd    fMinPorch
     *         fadd    fInterlace
     *         fstp    fTotalVLines
     */
    fTotalVLines = fVLinesRnd
                 + fTopMarginLines
                 + fBottomMarginLines
                 + fVSyncPlusBackPorch
                 + fMinPorch
                 + fInterlace;
    /*
     * 19) Find the actual vertical field frequency
     *         fld     fHFreq
     *         fdiv    fTotalVLines
     *         fmul    fOneThousand
     *         fstp    fVFieldRate
     */
    fVFieldRate = fHFreq /fTotalVLines * const_fOneThousand;
    /*
     * 20) Find the vertical frame frequency
     *         fld     fVFieldRate
     *         cmp     ds:[ebx].GTFIN.dwIntRqd,0
     *         je      @F
     *         fdiv    fTwo
     * @@:     fstp    fVFrameRate
     */
    fVFrameRate = fVFieldRate;
    if (lpGtfIn->dwIntRqd != 0)
        fVFrameRate /= const_fTwo;
    return;
}
/*
;==============================================================================
;
; Function:     ComputeGTFParameters
;
; Purpose:      This function computes the data for the GTFOut structure
;               from the arguments in the GTFIn structure provided
;               according to the GTF formula.
;
; Returns:      0       failure and lpGTFOut was not filled in
;               non-0   success and lpGTFOut was filled in
;
;==============================================================================
*/
ULONG WINAPI ComputeGTFParameters
(
        LPGTFIN  lpGtfIn,
        LPGTFOUT lpGtfOut
)
{
    /*
     * Set up all the constants we need
     */
    SetupGTFConstants();
    /*
     * Now we compute the stage 1 parameters. There are three cases
     * depending upon the IPParm which gives the type of the IPFreqRqd.
     */
    switch (lpGtfIn->dwIPParmType)
    {
        case IPTYPE_VERTICAL_REFRESH_RATE:
            VerticalRefreshToStage1Parameters(lpGtfIn);
            break;
        case IPTYPE_HORIZONTAL_REFRESH_RATE:
            HorizontalRefreshToStage1Parameters(lpGtfIn);
            break;
        case IPTYPE_PIXEL_CLOCK_RATE:
            PixelClockToStage1Parameters(lpGtfIn);
            break;
    }
    /*
     * Convert stage 1 to stage 2 parameters
     *
     * 1) Find the adressable lines per frame
     *
     * [ADDR LINES PER FRAME] = IF([INT RQD?]="y", [V LINES RND] * 2,
     *                                             [V LINES RND])
     */
    fAddrLinesPerFrame = fVLinesRnd;
    if (lpGtfIn->dwIntRqd != 0)
        fAddrLinesPerFrame += fVLinesRnd;
    /*
     * 2) Find the character time in nanoseconds
     *
     * [CHAR TIME] = [CELL GRAN RND] / [PIXEL FREQ] * 1000
     */
    fCharTime = fCellGran / fPixelFreq * const_fOneThousand;
    /*
     * 3) Find total number of lines in a frame
     *
     * [TOTAL LINES PER FRAME] = IF([INT RQD?]="y", 2 * ([V LINES RND] + [TOP MARGIN (LINES)] + [BOT MARGIN (LINES)] + [V SYNC+BP] + [INTERLACE] + [MIN PORCH RND]),
     *                                              ([V LINES RND] + [TOP MARGIN (LINES)] + [BOT MARGIN (LINES)] + [V SYNC+BP] + [INTERLACE] + [MIN PORCH RND]))
     */
    fTotalLinesPerFrame = fVLinesRnd
                        + fTopMarginLines
                        + fBottomMarginLines
                        + fVSyncPlusBackPorch
                        + fInterlace
                        + fMinPorch;
    if (lpGtfIn->dwIntRqd != 0)
        fTotalLinesPerFrame += fTotalLinesPerFrame;
    /*
     * 4) Find total number of chars in a horizontal line
     *
     * [TOTAL H TIME (CHARS)] = ROUND([TOTAL PIXELS] / [CELL GRAN RND], 0)
     */
    fTotalHTimeChars = calcROUND(fTotalPixels / fCellGran);
    /*
     * 5) Find the horizontal addressable time in microseconds
     *
     * [H ADDR TIME] = [H PIXELS RND] / [PIXEL FREQ]
     */
    fHAddrTime = fHPixelsRnd / fPixelFreq;
    /*
     * 6) Find the horizontal addressable time in chars
     *
     * [H ADDR TIME (CHARS)] = ROUND([H PIXELS RND] / [CELL GRAN RND], 0)
     */
    fHAddrTimeChars = calcROUND(fHPixelsRnd / fCellGran);
    /*
     * 7) Find horizontal blanking time in microseconds
     *
     * [H BLANK] = [H BLANK (PIXELS)] / [PIXEL FREQ]
     */
    fHBlank = fHBlankPixels / fPixelFreq;
    /*
     * 8) Find horizontal blanking time in chars
     *
     * [H BLANK (CHARS)] = ROUND([H BLANK (PIXELS)] / [CELL GRAN RND], 0)
     */
    fHBlankChars = calcROUND(fHBlankPixels / fCellGran);
    /*
     * 9) Find the horizontal blanking plus margin time in microseconds
     *
     * [H BLANK + MARGIN] = ([H BLANK (PIXELS)] + [RIGHT MARGIN (PIXELS)] + [LEFT MARGIN (PIXELS)])
     *                    / [PIXEL FREQ]
     */
    fHBlankPlusMargin = (fHBlankPixels + fRightMarginPixels + fLeftMarginPixels)
                      / fPixelFreq;
    /*
     * 10) Find the horizontal blanking plus margin time in chars
     *
     * [H BLANK + MARGIN(CHARS)] = ROUND(([H BLANK (PIXELS)] + [RIGHT MARGIN (PIXELS)] + [LEFT MARGIN (PIXELS)]) / [CELL GRAN RND], 0)
     */
    fHBlankPlusMarginChars = calcROUND((fHBlankPixels + fRightMarginPixels + fLeftMarginPixels) / fCellGran);
    /*
     * 11) Find the actual horizontal active video duty cycle in %
     *
     * [ACTUAL DUTY CYCLE] = [H BLANK (CHARS)] / [TOTAL H TIME (CHARS)] * 100
     */
    fActualDutyCycle = fHBlankChars / fTotalHTimeChars * const_fOneHundred;
    /*
     * 12) Find the image video duty cycle in %
     *
     * [BLANK + MARGIN DUTY CYCLE] = [H BLANK + MARGIN(CHARS)] / [TOTAL H TIME (CHARS)] * 100
     */
    fBlankPlusMarginDutyCycle = fHBlankPlusMarginChars / fTotalHTimeChars * const_fOneHundred;
    /*
     * 13) Find the left margin time in microseconds
     *
     * [LEFT MARGIN] = [LEFT MARGIN (PIXELS)] / [PIXEL FREQ] * 1000
     */
    fLeftMargin = fLeftMarginPixels * fPixelFreq * const_fOneThousand;
    /*
     * 14) Find the number of chars in the left margin
     *
     * [LEFT MARGIN (CHARS)] = [LEFT MARGIN (PIXELS)] / [CELL GRAN RND]
     */
    fLeftMarginChars = fLeftMarginPixels / fCellGran;
    /*
     * 15) Find the right margin time in microseconds
     *
     * [RIGHT MARGIN] = [RIGHT MARGIN (PIXELS)] / [PIXEL FREQ] * 1000
     */
    fRightMargin = fRightMarginPixels * fPixelFreq * const_fOneThousand;
    /*
     * 16) Find the number of chars in the rightt margin
     *
     * [RIGHT MARGIN (CHARS)] = [RIGHT MARGIN (PIXELS)] / [CELL GRAN RND]
     */
    fRightMarginChars = fRightMarginPixels / fCellGran;
    /*
     * 17) Find the number of pixels inteh horizontal sync period
     *
     * [H SYNC (PIXELS)] = ROUND(([H SYNC%] / 100 * [TOTAL PIXELS] / [CELL GRAN RND]), 0)
     *                   * [CELL GRAN RND]
     */
    fHSyncPixels = calcROUND(fHSyncPercent / const_fOneHundred * fTotalPixels / fCellGran)
                 * fCellGran;
    /*
     * 18) Find number of pixels in horizontal front porch period
     *
     * [H FRONT PORCH (PIXELS)] = ([H BLANK (PIXELS)] / 2) - [H SYNC (PIXELS)]
     */
    fHFrontPorchPixels = (fHBlankPixels / const_fTwo) - fHSyncPixels;
    /*
     * 19) Find number of pixels in horizontal front porch period
     *
     * [H BACK PORCH (PIXELS)] = [H FRONT PORCH (PIXELS)] + [H SYNC (PIXELS)]
     */
    fHBackPorchPixels = fHFrontPorchPixels + fHSyncPixels;
    /*
     * 20) Find number of characters in the horizontal sync period
     *
     * [H SYNC (CHARS)] = [H SYNC (PIXELS)] / [CELL GRAN RND]
     */
    fHSyncChars = fHSyncPixels / fCellGran;
    /*
     * 21) Find the horizontal sync period in microseconds
     *
     * [H SYNC] = [H SYNC (PIXELS)] / [PIXEL FREQ]
     */
    fHSync = fHSyncPixels / fPixelFreq;
    /*
     * 22) Find the number of chars in the horizontal front porch
     *
     * [H FRONT PORCH (CHARS)] = [H FRONT PORCH (PIXELS)] / [CELL GRAN RND]
     */
    fHFrontPorchChars = fHFrontPorchPixels / fCellGran;
    /*
     * 23) Find the horizontal front porch period in microseconds
     *
     * [H FRONT PORCH] = [H FRONT PORCH (PIXELS)] / [PIXEL FREQ]
     */
    fHFrontPorch = fHFrontPorchPixels / fPixelFreq;
    /*
     * 24) Find the number of chars in the horizontal back porch
     *
     * [H BACK PORCH (CHARS)] = [H BACK PORCH (PIXELS)] / [CELL GRAN RND]
     */
    fHBackPorchChars = fHBackPorchPixels / fCellGran;
    /*
     * 25) Find the horizontal front back period in microseconds
     *
     * [H BACK PORCH] = [H BACK PORCH (PIXELS)] / [PIXEL FREQ]
     */
    fHBackPorch = fHBackPorchPixels / fPixelFreq;
    /*
     * 26) Find the vertical frame period in milliseconds
     *
     * [V FRAME PERIOD] = IF([INT RQD?]="y", [TOTAL V LINES] * [H PERIOD] / 1000 * 2,
     *                                       [TOTAL V LINES] * [H PERIOD] / 1000)
     */
    fVFramePeriod = fTotalVLines * fHPeriod / const_fOneThousand;
    if (lpGtfIn->dwIntRqd != 0)
        fVFramePeriod += fVFramePeriod;
    /*
     * 27) Find the vertical field period in milliseconds
     *
     * [V FIELD PERIOD] = [TOTAL V LINES] * [H PERIOD] / 1000
     */
    fVFieldPeriod = fTotalVLines * fHPeriod / const_fOneThousand;
    /*
     * 28) Find the addressable vertical period per frame in milliseconds
     *
     * [V ADDR TIME PER FRAME] = IF([INT RQD?]="y", [V LINES RND] * [H PERIOD] / 1000 * 2,
     *                                              [V LINES RND] * [H PERIOD] / 1000)
     */
    fVAddrTimePerFrame = fVLinesRnd * fHPeriod / const_fOneThousand;
    if (lpGtfIn->dwIntRqd != 0)
        fVAddrTimePerFrame += fVAddrTimePerFrame;
    /*
     * 29) Find the addressable vertical period per field in milliseconds
     *
     * [V ADDR TIME PER FIELD] = [V LINES RND] * [H PERIOD] / 1000
     */
    fVAddrTimePerField = fVLinesRnd * fHPeriod / const_fOneThousand;
    /*
     * 30) Find the number of lines in teh odd blanking period
     *
     * [V ODD BLANKING (LINES)] = [V SYNC+BP] + [MIN PORCH RND]
     */
    fVOddBlankingLines = fVSyncPlusBackPorch + fMinPorch;
    /*
     * 31) Find the odd blanking period in milliseconds
     *
     * [V ODD BLANKING] = ([V SYNC+BP] + [MIN PORCH RND]) * [H PERIOD] / 1000
     */
    fVOddBlanking = (fVSyncPlusBackPorch + fMinPorch) * fHPeriod / const_fOneThousand;
    /*
     * 32) Find the number of lines in the even blanking period
     *
     * [V EVEN BLANKING (LINES)] = [V SYNC+BP] + (2 * [INTERLACE]) + [MIN PORCH RND]
     */
    fVEvenBlankingLines = fVSyncPlusBackPorch
                        + (const_fTwo * fInterlace)
                        + fMinPorch;
    /*
     * 33) Find the even blanking period in milliseconds
     *
     * [V EVEN BLANKING] = ([V SYNC+BP] + (2 * [INTERLACE]) + [MIN PORCH RND])
     *                   / 1000 * [H PERIOD]
     */
    fVEvenBlanking = (fVSyncPlusBackPorch + (const_fTwo * fInterlace) + fMinPorch)
                   / const_fOneThousand
                   *  fHPeriod;
    /*
     * 34) Find the top margin period in microseconds
     *
     * [TOP MARGIN ] = [TOP MARGIN (LINES)] * [H PERIOD]
     */
    fTopMargin = fTopMarginLines * fHPeriod;
    /*
     * 35) Find the odd front porch in microseconds
     *
     * [V ODD FRONT PORCH] = ([MIN PORCH RND] + [INTERLACE]) * [H PERIOD]
     */
    fVOddFrontPorch = (fMinPorch + fInterlace) * fHPeriod;
    /*
     * 36) Find the number of lines in the odd front porch period
     *
     * [V ODD FRONT PORCH(LINES)] = [MIN PORCH RND] + [INTERLACE]
     */
    fVOddFrontPorchLines = fMinPorch + fInterlace;
    /*
     * 37) Find the even front porch period
     *
     * [V EVEN FRONT PORCH] = [MIN PORCH RND] * [H PERIOD]
     */
    fVEvenFrontPorch = fMinPorch * fHPeriod;
    /*
     * 38) Find the vertcial sync period in microseconds
     *
     * [V SYNC] = [V SYNC RND] * [H PERIOD]
     */
    fVSync = fVSyncRqd * fHPeriod;
    /*
     * 39) Find the even front porch period in microseconds
     *
     * [V EVEN BACK PORCH] = ([V BACK PORCH] + [INTERLACE]) * [H PERIOD]
     */
    fVEvenBackPorch = (fVBackPorch + fInterlace) * fHPeriod;
    /*
     * 40) Find the number of lines in the even fron tporch period
     *
     * [V EVEN BACK PORCH (LINES)] = [V BACK PORCH] + [INTERLACE]
     */
    fVEvenBackPorchLines = fVBackPorch + fInterlace;
    /*
     * 41) Find the odd back porch period in microseconds
     *
     * [V ODD BACK PORCH] = [V BACK PORCH] * [H PERIOD]
     */
    fVOddBackPorch = fVBackPorch * fHPeriod;
    /*
     * 42) Find the bottom margin period in microseconds
     *
     * [BOT MARGIN] = [BOT MARGIN (LINES)] * [H PERIOD]
     */
    fBottomMargin = fBottomMarginLines * fHPeriod;
    /*
     * Cram all the data into GTFOut
     */
    lpGtfOut->dwHTotalChars          = (ULONG)fTotalHTimeChars;
    lpGtfOut->dwVTotalScans          = (ULONG)fTotalLinesPerFrame;
    lpGtfOut->dwHActiveChars         = (ULONG)fHAddrTimeChars;
    lpGtfOut->dwVActiveScans         = (ULONG)fVLinesRnd;
    lpGtfOut->dwHBlankStartChar      = (ULONG)(fHAddrTimeChars + fRightMarginChars);
    lpGtfOut->dwHFrontPorchChars     = (ULONG)fHFrontPorchChars;
    lpGtfOut->dwHSyncChars           = (ULONG)fHSyncChars;
    lpGtfOut->dwHBackPorchChars      = (ULONG)fHBackPorchChars;
    lpGtfOut->dwHBlankEndChar        = (ULONG)(fTotalHTimeChars - fLeftMarginChars);
    lpGtfOut->dwVBlankStartScan      = (ULONG)(fVLinesRnd + fBottomMarginLines);
    lpGtfOut->dwVFrontPorchScans     = (ULONG)fVOddFrontPorchLines;
    lpGtfOut->dwVSyncScans           = (ULONG)fVSyncRqd;
    lpGtfOut->dwVBackPorchScans      = (ULONG)fVEvenBackPorchLines;
    lpGtfOut->dwVBlankEndScan        = (ULONG)(fTotalLinesPerFrame - fTopMarginLines);
    lpGtfOut->dwPixelClockIn10KHertz = (ULONG)(fPixelFreq * const_fOneHundred);
    /*
     * Return success
     */
    return(1);
}
ULONG WINAPI Divide32By32
(
        ULONG dwDividend,
        ULONG dwDivisor
)
{
    return (dwDividend / dwDivisor);
}
ULONG WINAPI Multiply32By32
(
        ULONG dwMult1,
        ULONG dwMult2
)
{
    return (dwMult1 * dwMult2);
}
#endif /* !defined(_WIN32)  || defined(_WIN64) */
