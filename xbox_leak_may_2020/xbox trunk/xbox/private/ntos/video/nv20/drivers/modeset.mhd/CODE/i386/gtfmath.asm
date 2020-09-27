;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gtf.asm
;
; Purpose:      This file implements the GTF algorithm
;
;       Just come important comments and definitions I extracted from
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
.586
include ..\include\macros.dat
include gtfmath.inc

.listall

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, USE32, CODE

fZeroPointFour                  REAL4   0.4
fZeroPointFive                  REAL4   0.5
fOne                            REAL4   1.0
fOnePointEight                  REAL4   1.8
fTwo                            REAL4   2.0
fThree                          REAL4   3.0
fEight                          REAL4   8.0
fTwenty                         REAL4   20.0
fForty                          REAL4   40.0
fOneHundred                     REAL4   100.0
fOneHundredTwentyEight          REAL4   128.0
fTwoHundredFiftySix             REAL4   256.0
fFiveHundredFifty               REAL4   550.0
fSixHundred                     REAL4   600.0
fOneThousand                    REAL4   1000.0
fOneMillion                     REAL4   1000000.0

;==============================================================================
;
; Function:     ComputeGTFParameters
;
; Purpose:      This function computes the data for the GTFOut structure
;               from the arguments in the GTFIn structure provided
;               according to the GTF formula.
;
; Arguments:    lpGTFIn         FAR ptr to GTFIN structure
;               lpGTFOut        FAR ptr to GTFOUT structure
;
; Returns:      eax     0       failure and lpGTFOut was not filled in
;                       non-0   success and lpGTFOut was filled in
;
; Preserve:     All registers
;==============================================================================
IFDEF  _WIN32
DECPROC ComputeGTFParameters, STANDARD, FRAME, NEAR
ELSE
DECPROC ComputeGTFParameters, PASCAL, FRAME, FAR16
ENDIF
PARMD   lpGTFIn
PARMD   lpGTFOut
OPENPROC
        PUSHR   ds,ebx,esi,edi
IFDEF _WIN32
        mov     ebx,lpGTFIn
        mov     esi,lpGTFOut
ELSE
        sub     ebx,ebx
        lds     bx,lpGTFIn
        sub     esi,esi
        les     si,lpGTFOut
ENDIF
        call    ComputeGTFParameters1
        POPR    ds,ebx,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     ComputeGTFParameters1
;
; Purpose:      This function computes the data for the GTFOut structure
;               from teh arguments in the GTFIn structure provided
;               according to the GTF formula.
;
; Arguments:    ds:ebx  GTFIN ptr
;               es:esi  GTFOUT ptr
;
; Returns:      eax     0       failure and lpGTFOut was not filled in
;                       non-0   success and lpGTFOut was filled in
;
; Preserve:     All registers
;==============================================================================
DECPROC ComputeGTFParameters1, PASCAL, FRAME, NEAR
LOCALD  dwTemp
LOCALR4 fVLinesRnd
LOCALR4 fAddrLinesPerFrame
LOCALR4 fCellGran
LOCALR4 fPixelFreq
LOCALR4 fCharTime
LOCALR4 fTopMarginLines
LOCALR4 fBottomMarginLines
LOCALR4 fVSyncPlusBackPorch
LOCALR4 fMinPorch
LOCALR4 fInterlace
LOCALR4 fTotalLinesPerFrame
LOCALR4 fTotalPixels
LOCALR4 fTotalHTimeChars
LOCALR4 fHPixelsRnd
LOCALR4 fHAddrTime
LOCALR4 fHAddrTimeChars
LOCALR4 fHBlankPixels
LOCALR4 fHBlank
LOCALR4 fHBlankChars
LOCALR4 fLeftMarginPixels
LOCALR4 fRightMarginPixels
LOCALR4 fHBlankPlusMargin
LOCALR4 fHBlankPlusMarginChars
LOCALR4 fActualDutyCycle
LOCALR4 fBlankPlusMarginDutyCycle
LOCALR4 fLeftMargin
LOCALR4 fLeftMarginChars
LOCALR4 fRightMargin
LOCALR4 fRightMarginChars
LOCALR4 fHSyncPercent
LOCALR4 fHSyncPixels
LOCALR4 fHFrontPorchPixels
LOCALR4 fHBackPorchPixels
LOCALR4 fHSyncChars
LOCALR4 fHSync
LOCALR4 fHFrontPorchChars
LOCALR4 fHFrontPorch
LOCALR4 fHBackPorchChars
LOCALR4 fHBackPorch
LOCALR4 fTotalVLines
LOCALR4 fHPeriod
LOCALR4 fVFramePeriod
LOCALR4 fVFieldPeriod
LOCALR4 fVAddrTimePerFrame
LOCALR4 fVAddrTimePerField
LOCALR4 fVOddBlankingLines
LOCALR4 fVOddBlanking
LOCALR4 fVEvenBlankingLines
LOCALR4 fVEvenBlanking
LOCALR4 fTopMargin
LOCALR4 fVOddFrontPorch
LOCALR4 fVOddFrontPorchLines
LOCALR4 fVEvenFrontPorch
LOCALR4 fVSyncRqd
LOCALR4 fVSync
LOCALR4 fVBackPorch
LOCALR4 fVEvenBackPorch
LOCALR4 fVBackPorch
LOCALR4 fVEvenBackPorchLines
LOCALR4 fVOddBackPorch
LOCALR4 fBottomMargin
LOCALR4 fMarginPercent
LOCALR4 fMinVSyncPlusBackPorch
LOCALR4 fGTF_M
LOCALR4 fGTF_C
LOCALR4 fGTF_J
LOCALR4 fGTF_K
LOCALR4 fGTF_CPrime
LOCALR4 fGTF_MPrime
LOCALR4 fVFieldRateRqd
LOCALR4 fHPeriodEst
LOCALR4 fVFieldRateEst
LOCALR4 fVFieldRate
LOCALR4 fVFrameRate
LOCALR4 fTotalActivePixels
LOCALR4 fIdealDutyCycle
LOCALR4 fHFreq
LOCALR4 fIdealHPeriod
OPENPROC
        PUSHR   ds,ebx,esi,edi

        ; Reset the floating point unit
        fninit

        ; Set up all the constants we need
        call    SetupGTFConstants

        ; Now we compute the stage 1 parameters. There are three cases
        ; depending upon the IPParm which gives the type of the IPFreqRqd.
        push    OFFSET DoneStage1Parms
        cmp     ds:[ebx].GTFIN.dwIPParmType,IPTYPE_VERTICAL_REFRESH_RATE
        je      VerticalRefreshToStage1Parameters
        cmp     ds:[ebx].GTFIN.dwIPParmType,IPTYPE_HORIZONTAL_REFRESH_RATE
        je      HorizontalRefreshToStage1Parameters
        cmp     ds:[ebx].GTFIN.dwIPParmType,IPTYPE_PIXEL_CLOCK_RATE
        je      PixelClockToStage1Parameters
        pop     eax
        sub     eax,eax
        jmp     Done

DoneStage1Parms:
        ; Convert stage 1 to stage 2 parameters

        ; 1) Find the adressable lines per frame
        fld     fVLinesRnd
        cmp     ds:[ebx].GTFIN.dwIntRqd,0
        je      @F
        fadd    ST(0),ST(0)
@@:     fstp    fAddrLinesPerFrame

        ; 2) Find the character time in nanoseconds
        fld     fCellGran
        fdiv    fPixelFreq
        fmul    fOneThousand
        fstp    fCharTime

        ; 3) Find total number of lines in a frame
        fld     fVLinesRnd
        fadd    fTopMarginLines
        fadd    fBottomMarginLines
        fadd    fVSyncPlusBackPorch
        fadd    fMinPorch
        fadd    fInterlace
        cmp     ds:[ebx].GTFIN.dwIntRqd,0
        je      @F
        fadd    ST(0),ST(0)
@@:     fstp    fTotalLinesPerFrame

        ; 4) Find total number of chars in a horizontal line
        fld     fTotalPixels
        fdiv    fCellGran
        fistp   dwTemp
        fild    dwTemp
        fstp    fTotalHTimeChars

        ; 5) Find the horizontal addressable time in microseconds
        fld     fHPixelsRnd
        fdiv    fPixelFreq
        fstp    fHAddrTime

        ; 6) Find the horizontal addressable time in chars
        fld     fHPixelsRnd
        fdiv    fCellGran
        fistp   dwTemp
        fild    dwTemp
        fstp    fHAddrTimeChars

        ; 7) Find horizontal blanking time in microseconds
        fld     fHBlankPixels
        fdiv    fPixelFreq
        fstp    fHBlank

        ; 8) Find horizontal blanking time in chars
        fld     fHBlankPixels
        fdiv    fCellGran
        fistp   dwTemp
        fild    dwTemp
        fstp    fHBlankChars

        ; 9) Find the horizontal blanking plus margin time in microseconds
        fld     fHBlankPixels
        fadd    fLeftMarginPixels
        fadd    fRightMarginPixels
        fdiv    fPixelFreq
        fstp    fHBlankPlusMargin

        ; 10) Find the horizontal blanking plus margin time in chars
        fld     fHBlankPixels
        fadd    fLeftMarginPixels
        fadd    fRightMarginPixels
        fdiv    fCellGran
        fistp   dwTemp
        fild    dwTemp
        fstp    fHBlankPlusMarginChars

        ; 11) Find the actual horizontal active video duty cycle in %
        fld     fHBlankChars
        fdiv    fTotalHTimeChars
        fmul    fOneHundred
        fstp    fActualDutyCycle

        ; 12) Find the image video duty cycle in %
        fld     fHBlankPlusMarginChars
        fdiv    fTotalHTimeChars
        fmul    fOneHundred
        fstp    fBlankPlusMarginDutyCycle

        ; 13) Find the left margin time in microseconds
        fld     fLeftMarginPixels
        fmul    fPixelFreq
        fmul    fOneThousand
        fstp    fLeftMargin

        ; 14) Find the number of chars in the left margin
        fld     fLeftMarginPixels
        fdiv    fCellGran
        fstp    fLeftMarginChars

        ; 15) Find the right margin time in microseconds
        fld     fRightMarginPixels
        fmul    fPixelFreq
        fmul    fOneThousand
        fstp    fRightMargin

        ; 16) Find the number of chars in the rightt margin
        fld     fRightMarginPixels
        fdiv    fCellGran
        fstp    fRightMarginChars

        ; 17) Find the number of pixels inteh horizontal sync period
        fld     fHSyncPercent
        fdiv    fOneHundred
        fmul    fTotalPixels
        fdiv    fCellGran
        fistp   dwTemp
        fild    dwTemp
        fmul    fCellGran
        fstp    fHSyncPixels

        ; 18) Find number of pixels in horizontal front porch period
        fld     fHBlankPixels
        fdiv    fTwo
        fsub    fHSyncPixels
        fstp    fHFrontPorchPixels

        ; 19) Find number of pixels in horizontal front porch period
        fld     fHFrontPorchPixels
        fadd    fHSyncPixels
        fstp    fHBackPorchPixels

        ; 20) Find number of characters in the horizontal sync period
        fld     fHSyncPixels
        fdiv    fCellGran
        fstp    fHSyncChars

        ; 21) Find the horizontal sync period in microseconds
        fld     fHSyncPixels
        fdiv    fPixelFreq
        fstp    fHSync

        ; 22) Find the number of chars in the horizontal front porch
        fld     fHFrontPorchPixels
        fdiv    fCellGran
        fstp    fHFrontPorchChars

        ; 23) Find the horizontal front porch period in microseconds
        fld     fHFrontPorchPixels
        fdiv    fPixelFreq
        fstp    fHFrontPorch

        ; 24) Find the number of chars in the horizontal back porch
        fld     fHBackPorchPixels
        fdiv    fCellGran
        fstp    fHBackPorchChars

        ; 25) Find the horizontal front back period in microseconds
        fld     fHBackPorchPixels
        fdiv    fPixelFreq
        fstp    fHBackPorch

        ; 26) Find the vertical frame period in milliseconds
        fld     fTotalVLines
        fmul    fHPeriod
        fdiv    fOneThousand
        cmp     ds:[ebx].GTFIN.dwIntRqd,0
        je      @F
        fmul    fTwo
@@:     fstp    fVFramePeriod

        ; 27) Find the vertical field period in milliseconds
        fld     fTotalVLines
        fmul    fHPeriod
        fdiv    fOneThousand
        fstp    fVFieldPeriod

        ; 28) Find the addressable vertical period per frame in milliseconds
        fld     fVLinesRnd
        fmul    fHPeriod
        fdiv    fOneThousand
        cmp     ds:[ebx].GTFIN.dwIntRqd,0
        je      @F
        fmul    fTwo
@@:     fstp    fVAddrTimePerFrame

        ; 29) Find the addressable vertical period per field in milliseconds
        fld     fVLinesRnd
        fmul    fHPeriod
        fdiv    fOneThousand
        fstp    fVAddrTimePerField

        ; 30) Find the number of lines in teh odd blanking period
        fld     fVSyncPlusBackPorch
        fadd    fMinPorch
        fstp    fVOddBlankingLines

        ; 31) Find the odd blanking period in milliseconds
        fld     fVSyncPlusBackPorch
        fadd    fMinPorch
        fmul    fHPeriod
        fdiv    fOneThousand
        fstp    fVOddBlanking

        ; 32) Find the number of lines in the even blanking period
        fld     fVSyncPlusBackPorch
        fadd    fMinPorch
        fadd    fInterlace
        fadd    fInterlace
        fstp    fVEvenBlankingLines

        ; 33) Find the even blanking period in milliseconds
        fld     fVSyncPlusBackPorch
        fadd    fMinPorch
        fadd    fInterlace
        fadd    fInterlace
        fmul    fHPeriod
        fdiv    fOneThousand
        fstp    fVEvenBlanking

        ; 34) Find the top margin period in microseconds
        fld     fTopMarginLines
        fmul    fHPeriod
        fstp    fTopMargin

        ; 35) Find the odd front porch in microseconds
        fld     fMinPorch
        fadd    fInterlace
        fmul    fHPeriod
        fstp    fVOddFrontPorch

        ; 36) Find the number of lines in the odd front porch period
        fld     fMinPorch
        fadd    fInterlace
        fstp    fVOddFrontPorchLines

        ; 37) Find the even front porch period
        fld     fMinPorch
        fmul    fHPeriod
        fstp    fVEvenFrontPorch

        ; 38) Find the vertcial sync period in microseconds
        fld     fVSyncRqd
        fmul    fHPeriod
        fstp    fVSync

        ; 39) Find the even front porch period in microseconds
        fld     fVBackPorch
        fadd    fInterlace
        fmul    fHPeriod
        fstp    fVEvenBackPorch

        ; 40) Find the number of lines in the even fron tporch period
        fld     fVBackPorch
        fadd    fInterlace
        fstp    fVEvenBackPorchLines

        ; 41) Find the odd back porch period in microseconds
        fld     fVBackPorch
        fmul    fHPeriod
        fstp    fVOddBackPorch

        ; 42) Find the bottom margin period in microseconds
        fld     fBottomMarginLines
        fmul    fHPeriod
        fstp    fBottomMargin

        ; Cram all the data into GTFOut
        fld     fTotalHTimeChars
        fistp   es:[esi].GTFOUT.dwHTotalChars
        fld     fTotalLinesPerFrame
        fistp   es:[esi].GTFOUT.dwVTotalScans
        fld     fHAddrTimeChars
        fistp   es:[esi].GTFOUT.dwHActiveChars
        fld     fVLinesRnd
        fistp   es:[esi].GTFOUT.dwVActiveScans
        fld     fHAddrTimeChars
        fadd    fRightMarginChars
        fistp   es:[esi].GTFOUT.dwHBlankStartChar
        fld     fHFrontPorchChars
        fistp   es:[esi].GTFOUT.dwHFrontPorchChars
        fld     fHSyncChars
        fistp   es:[esi].GTFOUT.dwHSyncChars
        fld     fHBackPorchChars
        fistp   es:[esi].GTFOUT.dwHBackPorchChars
        fld     fTotalHTimeChars
        fsub    fLeftMarginChars
        fistp   es:[esi].GTFOUT.dwHBlankEndChar
        fld     fVLinesRnd
        fadd    fBottomMarginLines
        fistp   es:[esi].GTFOUT.dwVBlankStartScan
        fld     fVOddFrontPorchLines
        fistp   es:[esi].GTFOUT.dwVFrontPorchScans
        fld     fVSyncRqd
        fistp   es:[esi].GTFOUT.dwVSyncScans
        fld     fVEvenBackPorchLines
        fistp   es:[esi].GTFOUT.dwVBackPorchScans
        fld     fTotalLinesPerFrame
        fsub    fTopMarginLines
        fistp   es:[esi].GTFOUT.dwVBlankEndScan
        fld     fPixelFreq
;;;        fmul    fOneMillion
        fmul    fOneHundred
        fistp   es:[esi].GTFOUT.dwPixelClockIn10KHertz

        ; Return success
        mov     eax,1

Done:
        POPR    ds,ebx,esi,edi
CLOSEPROC


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
;
; Arguments:
;
; Returns:      None
;
; Preserve:     ds,ebx,es,esi
;==============================================================================
DECPROC SetupGTFConstants, PASCAL, NOFRAME, NEAR
OPENPROC
        PUSHR   ds,ebx,es,esi

        ; This is the size of the top (or bottom -- they are the same)
        ; overscan area as a percentage of the total vertcical time.
        fld     fOnePointEight
        fstp    fMarginPercent

        ; Number of pixels per character cell
        fld     fEight
        fstp    fCellGran

        ; Minimum horizontal front porch in character cells and
        ; minimum vertical front porch in scanlines (they are the same.)
        fld     fOne
        fstp    fMinPorch

        ; Width of vsync pulse in scanlines
        fld     fThree
        fstp    fVSyncRqd

        ; Width of the hsync pulse as a percentage of horizontal line time
        fld     fEight
        fstp    fHSyncPercent

        ; minimum time for vertical sync plus back porch in microseconds
        fld     fFiveHundredFifty
        fstp    fMinVSyncPlusBackPorch

        ; These apply to the Basic Blanking Duty Cycle Equation.
        ; See the top of the file.
        fld     fSixHundred
        fstp    fGTF_M
        fld     fForty
        fstp    fGTF_C
        fld     fOneHundredTwentyEight
        fstp    fGTF_K
        fld     fTwenty
        fstp    fGTF_J

        ; This is forced by GTF definitions
        ; C' = ((C - J) * (K / 256) + J
        fld     fGTF_K
        fdiv    fTwoHundredFiftySix
        fld     fGTF_C
        fsub    fGTF_J
        fmulp   ST(1),ST(0)
        fadd    fGTF_J
        fstp    fGTF_CPrime

        ; This is forced by GTF definitions
        ; M' = (K / 256) * M
        fld     fGTF_K
        fdiv    fTwoHundredFiftySix
        fmul    fGTF_M
        fstp    fGTF_MPrime

        POPR    ds,ebx,es,esi
CLOSEPROC


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
; Arguments:    ds:ebx  ptr to lpGTFIn structure
;               You have the stack frame available to ComputeGTFParameters
;               but be careful about ptrs, because they are flat 32bit ptrs
;               if you are running under WinNT and 16:16 ptrs if you are
;               running under Win9X. You shouldn't need to load any ptrs
;               since lpGTFIn is already passed into this routine in the
;               correct format.
;
; Returns:      None
;
; Preserve:     ds,esi,edi,ebx
;==============================================================================
DECPROC VerticalRefreshToStage1Parameters, PASCAL, NOFRAME, NEAR
OPENPROC
        PUSHR   ds,ebx,es,esi

        ; 1) Make sure that the XResolution is a multiple of char cell size
        fild    ds:[ebx].GTFIN.dwHPixels
        fdiv    fCellGran
        fistp   dwTemp
        fild    dwTemp
        fmul    fCellGran
        fstp    fHPixelsRnd

        ; 2) If interlace is requested, half the number of vertical lines
        ;    since the calculation is really vertical lines per field
        fild    ds:[ebx].GTFIN.dwVLines
        cmp     ds:[ebx].GTFIN.dwIntRqd,0
        je      @F
        fdiv    fTwo
@@:     fstp    fVLinesRnd

        ; 3) Find the frame rate required
        fild    ds:[ebx].GTFIN.dwIPFreqRqd
        cmp     ds:[ebx].GTFIN.dwIntRqd,0
        je      @F
        fadd    ST(0),ST(0)
@@:     fstp    fVFieldRateRqd

        ; 4) Find the number of lines in the top margin
        fldz
        cmp     ds:[ebx].GTFIN.dwMarginsRqd,0
        je      @F
        faddp   ST(0),ST(0)             ; pop the stack
        fld     fMarginPercent
        fdiv    fOneHundred
        fmul    fVLinesRnd
@@:     fstp    fTopMarginLines

        ; 5) Find the number of lines in the bottom margin
        fldz
        cmp     ds:[ebx].GTFIN.dwMarginsRqd,0
        je      @F
        faddp   ST(0),ST(0)             ; pop the stack
        fld     fMarginPercent
        fdiv    fOneHundred
        fmul    fVLinesRnd
@@:     fstp    fBottomMarginLines

        ; 6) If interlace is required, set dwInterlace to 0.5
        fldz
        cmp     ds:[ebx].GTFIN.dwIntRqd,0
        je      @F
        faddp   ST(0),ST(0)             ; pop the stack
        fld     fZeroPointFive
@@:     fstp    fInterlace

        ; 7) Estimate the horizontal period
        fld1
        fdiv    fVFieldRateRqd
        fld     fMinVSyncPlusBackPorch
        fdiv    fOneMillion
        fsubp   ST(1),ST(0)
        fld     fVLinesRnd
        fadd    fTopMarginLines
        fadd    fTopMarginLines
        fadd    fMinPorch
        fadd    fInterlace
        fdivp   ST(1),ST(0)
        fmul    fOneMillion
        fstp    fHPeriodEst

        ; 8) Find the number of lines in fMinVSyncPlusBackPorch
        fld     fMinVSyncPlusBackPorch
        fdiv    fHPeriodEst
        fistp   dwTemp
        fild    dwTemp
        fstp    fVSyncPlusBackPorch

        ; 9) Find the number of lines in VBackPorch alone
        fld     fVSyncPlusBackPorch
        fsub    fVSyncRqd
        fstp    fVBackPorch

        ; 10) Find the total number of lines in the vertical field
        fld     fVLinesRnd
        fadd    fTopMarginLines
        fadd    fBottomMarginLines
        fadd    fVSyncPlusBackPorch
        fadd    fMinPorch
        fadd    fInterlace
        fstp    fTotalVLines

        ; 11) Estimate the vertical field frequency
        fld1
        fdiv    fHPeriodEst
        fdiv    fTotalVLines
        fmul    fOneMillion
        fstp    fVFieldRateEst

        ; 12) Find the actual horizontal period
        fld     fHPeriodEst
        fmul    fVFieldRateEst
        fdiv    fVFieldRateRqd
        fstp    fHPeriod

        ; 13) Find the actual vertical field frequency
        fld1
        fdiv    fHPeriod
        fdiv    fTotalVLines
        fmul    fOneMillion
        fstp    fVFieldRate

        ; 14) Find the vertical frame frequency
        fld     fVFieldRate
        cmp     ds:[ebx].GTFIN.dwIntRqd,0
        je      @F
        fdiv    fTwo
@@:     fstp    fVFrameRate

        ; 15) Find the number of pixels in the left margin
        fldz
        cmp     ds:[ebx].GTFIN.dwMarginsRqd,0
        je      @F
        faddp   ST(0),ST(0)             ; pop the stack
        fld     fHPixelsRnd
        fmul    fMarginPercent
        fdiv    fOneHundred
        fdiv    fCellGran
        fistp   dwTemp
        fild    dwTemp
        fmul    fCellGran
@@:     fstp    fLeftMarginPixels

        ; 16) Find the number of pixels in the right margin
        fldz
        cmp     ds:[ebx].GTFIN.dwMarginsRqd,0
        je      @F
        faddp   ST(0),ST(0)             ; pop the stack
        fld     fHPixelsRnd
        fmul    fMarginPercent
        fdiv    fOneHundred
        fdiv    fCellGran
        fistp   dwTemp
        fild    dwTemp
        fmul    fCellGran
@@:     fstp    fRightMarginPixels

        ; 17) Find total number of active pixels in image+left+right margin
        fld     fHPixelsRnd
        fadd    fLeftMarginPixels
        fadd    fRightMarginPixels
        fstp    fTotalActivePixels

        ; 18) Find the ideal Blanking duty cycle
        fld     fGTF_CPrime
        fld     fGTF_MPrime
        fmul    fHPeriod
        fdiv    fOneThousand
        fsubp   ST(1),ST(0)
        fstp    fIdealDutyCycle

        ; 19) Find the number of pixels in the blanking time to the
        ; nearest double character cell
        fld     fTotalActivePixels
        fmul    fIdealDutyCycle
        fld     fOneHundred
        fsub    fIdealDutyCycle
        fdivp   ST(1),ST(0)
        fld     fCellGran
        fadd    ST(0),ST(0)
        fdivp   ST(1),ST(0)
        fistp   dwTemp
        fild    dwTemp
        fadd    ST(0),ST(0)
        fmul    fCellGran
        fstp    fHBlankPixels

        ; 20) Find the total number of pixels
        fld     fTotalActivePixels
        fadd    fHBlankPixels
        fstp    fTotalPixels

        ; 21) Find the pixel clock frequency
        fld     fTotalPixels
        fdiv    fHPeriod
        fstp    fPixelFreq

        ; 22) Find the horizontal frequency
        fld     fOneThousand
        fdiv    fHPeriod
        fstp    fHFreq

        POPR    ds,ebx,es,esi
CLOSEPROC


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
; Arguments:
;
; Returns:      None
;
; Preserve:     ds,ebx,es,esi
;==============================================================================
DECPROC HorizontalRefreshToStage1Parameters, PASCAL, NOFRAME, NEAR
OPENPROC
        PUSHR   ds,ebx,es,esi

        ; 1) Make sure that the XResolution is a multiple of char cell size
        fild    ds:[ebx].GTFIN.dwHPixels
        fdiv    fCellGran
        fistp   dwTemp
        fild    dwTemp
        fmul    fCellGran
        fstp    fHPixelsRnd

        ; 2) If interlace is requested, half the number of vertical lines
        ;    since the calculation is really vertical lines per field
        fild    ds:[ebx].GTFIN.dwVLines
        cmp     ds:[ebx].GTFIN.dwIntRqd,0
        je      @F
        fdiv    fTwo
@@:     fstp    fVLinesRnd

        ; 3) Find the horizontal freqency required
        fild    ds:[ebx].GTFIN.dwIPFreqRqd
        fstp    fHFreq

        ; 4) Find the number of lines in the top margin
        sub     eax,eax
        cmp     ds:[ebx].GTFIN.dwMarginsRqd,0
        je      @F
        fld     fMarginPercent
        fmul    fVLinesRnd
        fdiv    fOneHundred
@@:     fstp    fTopMarginLines

        ; 5) Find the number of lines in the bottom margin
        sub     eax,eax
        cmp     ds:[ebx].GTFIN.dwMarginsRqd,0
        je      @F
        fld     fMarginPercent
        fmul    fVLinesRnd
        fdiv    fOneHundred
@@:     fstp    fBottomMarginLines

        ; 6) If interlace is required, set dwInterlace to 0.5
        fldz
        cmp     ds:[ebx].GTFIN.dwIntRqd,0
        je      @F
        faddp   ST(0),ST(0)             ; pop the stack
        fld     fZeroPointFive
@@:     fstp    fInterlace

        ; 7) Find the number of lines in fMinVSyncPlusBackPorch
        fld     fMinVSyncPlusBackPorch
        fmul    fHFreq
        fdiv    fOneThousand
        fistp   dwTemp
        fild    dwTemp
        fstp    fVSyncPlusBackPorch

        ; 8) Find the number of lines in VBackPorch alone
        fld     fVSyncPlusBackPorch
        fsub    fVSyncRqd
        fstp    fVBackPorch

        ; 9) Find the total number of lines in the vertical field
        fld     fVLinesRnd
        fadd    fTopMarginLines
        fadd    fBottomMarginLines
        fadd    fVSyncPlusBackPorch
        fadd    fMinPorch
        fadd    fInterlace
        fstp    fTotalVLines

        ; 10) Find the actual vertical field frequency
        fld     fHFreq
        fdiv    fTotalVLines
        fmul    fOneThousand
        fstp    fVFieldRate

        ; 11) Find the vertical frame frequency
        fld     fVFieldRate
        cmp     ds:[ebx].GTFIN.dwIntRqd,0
        je      @F
        fdiv    fTwo
@@:     fstp    fVFrameRate

        ; 12) Find the number of pixels in the left margin
        fldz
        cmp     ds:[ebx].GTFIN.dwMarginsRqd,0
        je      @F
        faddp   ST(0),ST(0)             ; pop the stack
        fld     fHPixelsRnd
        fmul    fMarginPercent
        fdiv    fOneHundred
        fdiv    fCellGran
        fistp   dwTemp
        fild    dwTemp
        fmul    fCellGran
@@:     fstp    fLeftMarginPixels

        ; 13) Find the number of pixels in the right margin
        fldz
        cmp     ds:[ebx].GTFIN.dwMarginsRqd,0
        je      @F
        faddp   ST(0),ST(0)             ; pop the stack
        fld     fHPixelsRnd
        fmul    fMarginPercent
        fdiv    fOneHundred
        fdiv    fCellGran
        fistp   dwTemp
        fild    dwTemp
        fmul    fCellGran
@@:     fstp    fRightMarginPixels

        ; 14) Find total number of active pixels in image+left+right margin
        fld     fHPixelsRnd
        fadd    fLeftMarginPixels
        fadd    fRightMarginPixels
        fstp    fTotalActivePixels

        ; 15) Find the ideal Blanking duty cycle
        fld     fGTF_CPrime
        fld     fGTF_MPrime
        fdiv    fHFreq
        fsubp   ST(1),ST(0)
        fstp    fIdealDutyCycle

        ; 16) Find the number of pixels in the blanking time to the
        ; nearest double character cell
        fld     fTotalActivePixels
        fmul    fIdealDutyCycle
        fld     fOneHundred
        fsub    fIdealDutyCycle
        fdivp   ST(1),ST(0)
        fld     fCellGran
        fadd    ST(0),ST(0)
        fdivp   ST(1),ST(0)
        fistp   dwTemp
        fild    dwTemp
        fadd    ST(0),ST(0)
        fmul    fCellGran
        fstp    fHBlankPixels

        ; 17) Find the total number of pixels
        fld     fTotalActivePixels
        fadd    fHBlankPixels
        fstp    fTotalPixels

        ; 18) Find the horizontal frequency
        fld     fOneThousand
        fdiv    fHFreq
        fstp    fHPeriod

        ; 19) Find the pixel clock frequency
        fld     fTotalPixels
        fmul    fHFreq
        fdiv    fOneThousand
        fstp    fPixelFreq

        POPR    ds,ebx,es,esi
CLOSEPROC


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
; Arguments:
;
; Returns:      None
;
; Preserve:     ds,ebx,es,esi
;==============================================================================
DECPROC PixelClockToStage1Parameters, PASCAL, NOFRAME, NEAR
OPENPROC
        PUSHR   ds,ebx,es,esi

        ; 1) Make sure that the XResolution is a multiple of char cell size
        fild    ds:[ebx].GTFIN.dwHPixels
        fdiv    fCellGran
        fistp   dwTemp
        fild    dwTemp
        fmul    fCellGran
        fstp    fHPixelsRnd

        ; 2) If interlace is requested, half the number of vertical lines
        ;    since the calculation is really vertical lines per field
        fild    ds:[ebx].GTFIN.dwVLines
        cmp     ds:[ebx].GTFIN.dwIntRqd,0
        je      @F
        fdiv    fTwo
@@:     fstp    fVLinesRnd

        ; 3) Find the horizontal freqency required
        fild    ds:[ebx].GTFIN.dwIPFreqRqd
        fstp    fPixelFreq

        ; 4) Find the number of pixels in the left margin
        fldz
        cmp     ds:[ebx].GTFIN.dwMarginsRqd,0
        je      @F
        faddp   ST(0),ST(0)             ; pop the stack
        fld     fHPixelsRnd
        fmul    fMarginPercent
        fdiv    fOneHundred
        fdiv    fCellGran
        fistp   dwTemp
        fild    dwTemp
        fmul    fCellGran
@@:     fstp    fLeftMarginPixels

        ; 5) Find the number of pixels in the right margin
        fldz
        cmp     ds:[ebx].GTFIN.dwMarginsRqd,0
        je      @F
        faddp   ST(0),ST(0)             ; pop the stack
        fld     fHPixelsRnd
        fmul    fMarginPercent
        fdiv    fOneHundred
        fdiv    fCellGran
        fistp   dwTemp
        fild    dwTemp
        fmul    fCellGran
@@:     fstp    fRightMarginPixels

        ; 6) Find total number of active pixels in image+left+right margin
        fld     fHPixelsRnd
        fadd    fLeftMarginPixels
        fadd    fRightMarginPixels
        fstp    fTotalActivePixels

        ; 7) Find the ideal horizontal period from blanking duty cycle Eq
        fld     fOneHundred
        fsub    fGTF_CPrime
        fmul    ST(0),ST(0)
        fld     fTotalActivePixels
        fadd    fLeftMarginPixels
        fadd    fRightMarginPixels
        fmul    fGTF_MPrime
        fmul    fZeroPointFour
        fdiv    fPixelFreq
        faddp   ST(1),ST(0)
        fsqrt
        fadd    fGTF_CPrime
        fsub    fOneHundred
        fdiv    fTwo
        fdiv    fGTF_MPrime
        fmul    fOneThousand
        fstp    fIdealHPeriod

        ; 8) Find the ideal Blanking duty cycle
        fld     fGTF_CPrime
        fld     fGTF_MPrime
        fmul    fIdealHPeriod
        fdiv    fOneThousand
        fsubp   ST(1),ST(0)
        fstp    fIdealDutyCycle

        ; 9) Find the number of pixels in the blanking time to the
        ; nearest double character cell
        fld     fTotalActivePixels
        fmul    fIdealDutyCycle
        fld     fOneHundred
        fsub    fIdealDutyCycle
        fdivp   ST(1),ST(0)
        fld     fCellGran
        fadd    ST(0),ST(0)
        fdivp   ST(1),ST(0)
        fistp   dwTemp
        fild    dwTemp
        fadd    ST(0),ST(0)
        fmul    fCellGran
        fstp    fHBlankPixels

        ; 10) Find the total number of pixels
        fld     fTotalActivePixels
        fadd    fHBlankPixels
        fstp    fTotalPixels

        ; 11) Find the horizontal frequency
        fld     fPixelFreq
        fdiv    fTotalPixels
        fmul    fOneThousand
        fstp    fHFreq

        ; 12) Find the horizontal period
        fld     fOneThousand
        fdiv    fHFreq
        fstp    fHPeriod

        ; 13) Find the number of lines in the top margin
        fldz    
        cmp     ds:[ebx].GTFIN.dwMarginsRqd,0
        je      @F
        fld     fMarginPercent
        fmul    fVLinesRnd
        fdiv    fOneHundred
@@:     fstp    fTopMarginLines

        ; 14) Find the number of lines in the bottom margin
        fldz
        cmp     ds:[ebx].GTFIN.dwMarginsRqd,0
        je      @F
        fld     fMarginPercent
        fmul    fVLinesRnd
        fdiv    fOneHundred
@@:     fstp    fBottomMarginLines

        ; 15) If interlace is required, set dwInterlace to 0.5
        fldz
        cmp     ds:[ebx].GTFIN.dwIntRqd,0
        je      @F
        faddp   ST(0),ST(0)             ; pop the stack
        fld     fZeroPointFive
@@:     fstp    fInterlace

        ; 16) Find the number of lines in fMinVSyncPlusBackPorch
        fld     fMinVSyncPlusBackPorch
        fmul    fHFreq
        fdiv    fOneThousand
        fstp    fVSyncPlusBackPorch

        ; 17) Find the number of lines in VBackPorch alone
        fld     fVSyncPlusBackPorch
        fsub    fVSyncRqd
        fstp    fVBackPorch

        ; 18) Find the total number of lines in the vertical field
        fld     fVLinesRnd
        fadd    fTopMarginLines
        fadd    fBottomMarginLines
        fadd    fVSyncPlusBackPorch
        fadd    fMinPorch
        fadd    fInterlace
        fstp    fTotalVLines

        ; 19) Find the actual vertical field frequency
        fld     fHFreq
        fdiv    fTotalVLines
        fmul    fOneThousand
        fstp    fVFieldRate

        ; 20) Find the vertical frame frequency
        fld     fVFieldRate
        cmp     ds:[ebx].GTFIN.dwIntRqd,0
        je      @F
        fdiv    fTwo
@@:     fstp    fVFrameRate

        POPR    ds,ebx,es,esi
CLOSEPROC


;==============================================================================
;
; Function:     Divide32By32
;
; Purpose:      This function divides a 32bit number by another 32bit
;               number. The problem is that the 16bit compiler can't do
;               this without linking in a library.
;
; Arguments:
;
; Returns:      quotient.
;
; Preserve:     All registers
;==============================================================================
IFDEF _WIN32
DECPROC Divide32By32, STANDARD, FRAME, NEAR
ELSE
DECPROC Divide32By32, PASCAL, FRAME, FAR16
ENDIF
PARMD   dwDividend
PARMD   dwDivisor
OPENPROC
        sub     edx,edx
        mov     eax,dwDividend
        cmp     dwDivisor,edx
        jne     valid
divbyzero:
        cmp     eax,edx                 ; is numerator zero?
        je      done                    ; yes - eax:edx = 0
        mov     eax,-1                  ; no - call it infinity
        mov     edx,-1                  ; eax:edx = -1
        jmp     done
valid:  
        div     dwDivisor
done:
IFNDEF _WIN32
        mov     edx,eax
        shr     edx,10H
ENDIF
CLOSEPROC


;==============================================================================
;
; Function:     Multiply32By32
;
; Purpose:      This function multiplies a 32bit number by another 32bit
;               number. The problem is that the 16bit compiler can't do
;               this without linking in a library.
;
; Arguments:
;
; Returns:      quotient.
;
; Preserve:     All registers
;==============================================================================
IFDEF _WIN32
DECPROC Multiply32By32, STANDARD, FRAME, NEAR
ELSE
DECPROC Multiply32By32, PASCAL, FRAME, FAR16
ENDIF
PARMD   dwMult1
PARMD   dwMult2
OPENPROC
        mov     eax,dwMult1
        imul    eax,dwMult2
IFNDEF _WIN32
        mov     edx,eax
        shr     edx,10H
ENDIF
CLOSEPROC

CLOSESEG _NVTEXT32

END

