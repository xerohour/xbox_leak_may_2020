 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/******************************** DAC Manager ******************************\
*                                                                           *
* Module: DAC.C                                                             *
*   The DAC control management is updated here.                             *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <state.h>
#include <class.h>
#include <vblank.h>
#include <dma.h>
#include <gr.h>
#include <dac.h>
#include <os.h>
#include <i2c.h>
#include "nvhw.h"
#include <vga.h>
#include <nvcm.h>
#include <edid.h>
#include <tv.h>
#include <devinit.h>
#include <smp.h>
#include <stddef.h>    // for size_t

/* This file simulates the NV arbitor */
#include <arb_fifo.h>


// Const strings for accessing the registry

extern U008    CH_Init[];
// The "leaf" subkeys are OS-independent

#ifdef NTRM    
char strDisplayType[] = "Display Type";
char strDisplay2Type[] = "Display2 Type";
char strTVtype[] = "TV Type";
char strTVOutType[] = "TV Out";
char    strFpMode[] = "FlatPanelMode";
#else
extern char strDisplayType[];
extern char strDisplay2Type[];
extern char strTVtype[];
extern char strTVOutType[];
extern char strFpMode[];
#endif // NTRM

extern char strDevNodeRM[];

#define RED_OUT_300MV	0x800001A0
#define MAXCRTRETRIES   5

// remove this after all drivers switch to the new devices based API.
static BOOL using_new_api = FALSE;

//
// Set Display Power Managememnt Level.
//
RM_STATUS dacSetDpmLevel
(
    PHWINFO      pDev
)
{
    switch (pDev->Dac.DpmLevel)
    {
        default:
            break;
    }
    return (RM_OK);
}

RM_STATUS dacLoadWidthDepth
(
   PHWINFO pDev,
   U032    Head
)
{
    U032 data;

    switch (pDev->Dac.HalInfo.Depth)
    {
        case 8:
            data = NV_CIO_CRE_PIXEL_FORMAT_8BPP;
            break;
        case 15:
        case 16:
            data = NV_CIO_CRE_PIXEL_FORMAT_16BPP;
            break;
        case 24:
        case 30:
        case 32:
        default:
            data = NV_CIO_CRE_PIXEL_FORMAT_32BPP;
            break;
    }
    //
    // Write the CRTC register
    //
    CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, data, Head);

    return (RM_OK);
}

//
// Find closest DAC PLL parameters for a given frequency.
//
RM_STATUS dacCalcPLL
(
    PHWINFO pDev 
)
{
    U032 DeltaNew;
    U032 DeltaOld;
    U032 VClk;
    U032 Freq;
    U032 M;
    U032 N;
    U032 P;

    //
    // Calc VPLL.
    //
    DeltaOld = 0xFFFFFFFF;
    VClk     = pDev->Dac.HalInfo.VClk / 1000;
    //
    // Calculate frequencies using KHz to keep the math precision inside 32 bits.
    //
    for (P = 1; P <= 8; P <<= 1)
    {
        Freq = VClk * P;
        //
        // Bound the parameters to the internal frequencies of the DAC.
        //
        if ((Freq >= 64000) && (Freq <= 170000))
        {
            for (M = 7; M <= 12; M++)
            {
                N    = VClk * P * M / 12096;
                Freq = N * 12096 / P / M;
                if (Freq > VClk)
                    DeltaNew = Freq - VClk;
                else
                    DeltaNew = VClk - Freq;
                if (DeltaNew < DeltaOld)
                {
                    //
                    // Closer match.
                    //
                    pDev->Dac.HalInfo.VPllM = M;
                    pDev->Dac.HalInfo.VPllN = N;
                    pDev->Dac.HalInfo.VPllO = 1;
                    if (P==8)
                        pDev->Dac.HalInfo.VPllP = 3;
                    else if (P==4)
                        pDev->Dac.HalInfo.VPllP = 2;
                    else if (P==2)
                        pDev->Dac.HalInfo.VPllP = 1;
                    else 
                        pDev->Dac.HalInfo.VPllP = 0;
                    DeltaOld = DeltaNew;
                }
            }
        }
    }
    return (RM_OK);
}


//
//       dacCalcMNP - Return the PLL values for a specified frequency
//
//          Clock frequency in MHz * 100
//
// The algorithm is constrained to keep within 0.5% error
// to remain inside the VBE spec.
// However, if you have a choice of what frequencies you
// request, try to avoid the following requested frequencies
// when you get above 300MHz, as they require M values > 3.
//
//  14.31818 MHz
// 29739 - 29918
// 30219 - 30393
// 31178 - 31343
// 31658 - 31818
// 32617 - 32767
// 33097 - 33243
// 34056 - 34193
// 34536 - 34668
//
//  13.5000 MHz
// 29397 - 29552
// 29849 - 30000
// 30754 - 30896
// 31206 - 31344
// 32110 - 32239
// 32563 - 32767
// 33467 - 33582
// 33919 - 34030
// 34824 - 34926
//
U032 dacCalcMNP(
    PHWINFO  pDev,
    U032     Head,
    U032     Clock,
    U032    *M,    
    U032    *N,    
    U032    *P    
)
{
    U016  crystalFreq;
    U032  vclk;
    U016  vclkClosest = 0;
    U016  deltaOld;
    U016  bestM = 0;
    U016  bestP = 0;
    U016  bestN = 0;
    U016  lowM;
    U016  highM;
    U016  lowP, highP;
    U032  hiVclk1, hiVclk2, hiVclk3;
    U032  FminVco, FmaxVco;
    
    U016  powerP;
    U016  m;
    U016  n;
    U016  i, j;
    U032  lwv, lwv2;
    
    U016  lwvs, vclks;
    U016  t;
    
    vclk = (U032)(Clock * 10);     // Make KHz
    
    deltaOld = 0xFFFF;

    //
    // NV11 needs four highM buckets due to
    // induced jitter into the PLL's
    //
    if (IsNV11(pDev))
    {
        hiVclk1 = 150000;
        hiVclk2 = 200000;
        hiVclk3 = 340000;
    }
    else
    {
        hiVclk1 = 250000;
        hiVclk2 = 340000;

    }

    lowP  = 0;
    highP = 4;

    // if initDac() has run then FminVco and FmaxVco will be set.
    // But we might be called during devinit processing prior to
    //    initDac(), so we need a fallback...
    // NOTE: these defaults duplicated in initDac()
    if ((pDev->Dac.FminVco == 0) || (pDev->Dac.FmaxVco == 0))
    {
        if (IsNV15orBetter(pDev)) {
            if (IsNV15MaskRevA01(pDev))
                pDev->Dac.FminVco = 200000;
            else
                pDev->Dac.FminVco = 250000;
            
            pDev->Dac.FmaxVco = pDev->Dac.FminVco * 2; 
        } else {
            pDev->Dac.FminVco = 128000;
            pDev->Dac.FmaxVco = 256000;
        }
    }
    
    FminVco = pDev->Dac.FminVco;
    FmaxVco = pDev->Dac.FmaxVco;

    //
    // NV11 requires a modified Vco range, and since the BIOS's are already 
    // out in the field, shortcircuit this nice feature to fix the noise issue.
    //
    // When we're sure all older BIOS's have been flushed out of the world
    // (need 3.11.01.25.00+), this code can be removed.  If we detect a newer
    // BIOS revision is in use, use those values.
    //
    // Mobile NV11 has always used these ranges, so this should not be a change
    // for them.
    //
    if (IsNV11(pDev) && (pDev->Chip.BiosRevision <= 0x03110125))
    {
        FminVco = 200000;
        FmaxVco = 400000;
    }

    // Apply any adjusts needed to FminVco/FmaxVco.
    //
    // 1). Make sure vclk is not so low that we'll miss the whole VCO range.
    //     Adjust FminVco to hit with the highest P (post divide).
    //
    if ((vclk << highP) < FminVco)
        FminVco = vclk << highP;

    //
    // 2). Also, check if two consecutive P values would cause us to miss
    //     the whole VCO range, where the first P is beneath FminVco and
    //     the next P is above FmaxVco.
    //
    for (powerP = lowP; powerP < highP; powerP++)
    {
        lwv = vclk << powerP;
        if ((lwv < FminVco) && ((lwv << 1) > FmaxVco))
        {
            // adjust FminVco or FmaxVco, whichever is closer
            if ((FminVco - lwv) < ((lwv << 1) - FmaxVco))
                FminVco = lwv;
            else
                FmaxVco = (lwv << 1);
        }
    }

    //
    // 3). Let a high vclk set the upper bound.
    //
    if (vclk > FmaxVco)
        FmaxVco = vclk;

    //
    // For NV15, we've got a bit in CR33 that indicates we're setting the
    // PLL in the "upper" VCO range.
    //
    if (IsNV15(pDev))
    {
        U008 lock, cr33;

        // unlock the extended regs
        lock = UnlockCRTC(pDev, Head);

        // put NV15 into NV15 VPLL VCO programming mode
        CRTC_RD(NV_CIO_CRE_LCD__INDEX, cr33, Head);
        cr33 |= (1 << DRF_SHIFT(NV_CIO_CRE_LCD_VPLL_VCO));
        CRTC_WR(NV_CIO_CRE_LCD__INDEX, cr33, Head);

        RestoreLock(pDev, Head, lock);
    }

    //
    // Determine the current crystal frequency
    //   
    if (pDev->Chip.HalInfo.CrystalFreq == 14318180)
    {
        // 14.3Khz
        crystalFreq = 14318;

        lowM = 1;
             
        if (IsNV11(pDev))
        {
            //
            // NV11 needs four highM buckets due to
            // induced jitter into the PLL's
            //
            if (vclk > hiVclk3)
                highM = 2;
            else if (vclk > hiVclk2)
                highM = 4;
            else if (vclk > hiVclk1)
                highM = 6;
            else
                highM = 14;

        }
        else
        {
            //
            // All chips except for NV11 only need three
            // highM buckets
            //
            if (vclk > hiVclk2)
                highM = 2;
            else if (vclk > hiVclk1)
                highM = 6;
            else
                highM = 14;
        }
    }
    else
    {
        // 13.5Khz
        crystalFreq = 13500;

        lowM = 1;

        //
        // NV11 needs four highM buckets due to
        // induced jitter into the PLL's
        //
        if (IsNV11(pDev))
        {
            if (vclk > hiVclk3)
                highM = 2;
            else if (vclk > hiVclk2)
                highM = 4;
            else if (vclk > hiVclk1)
                highM = 6;
            else
                highM = 13;
        }
        else
        {
            //
            // All chips except for NV11 only need three
            // highM buckets
            //
            if (vclk > hiVclk2)
                highM = 2;
            else if (vclk > hiVclk1)
                highM = 6;
            else
                highM = 13;
        }
    }                      
    
    //
    // P Loop
    //
    for (powerP = lowP; powerP <= highP; powerP++) // Init Power of P
    {                             
        // M Loop
        for (m = lowM; m <= highM; m++)
        {                          
            lwv = vclk << powerP;   // vclk * P
            
            // Compare with internal range of DAC
            if ((lwv >= FminVco) && (lwv <= FmaxVco))
            {
                lwv *= (U032)m;    // vclk * P * M
                
                // Add crystal Frequency /2 - will round better
                lwv += (crystalFreq >> 1);
                
                lwv /= crystalFreq; // N = vclk*P*M/crystal
                n = (U016)(lwv & 0xFFFF);
                
                if (n > 0xFF)
                    break;
                    
                lwv *= crystalFreq; // N * crystal
                   
                i = 1 << powerP;    // for rounding
                i >>= 1;            // /2
                
                lwv += i;           // Add 1/2 of P
                lwv >>= powerP;     // N*cyrstal/P
                
                i = m;
                j = m;
                
                j >>= 1;            // M/2
                lwv += j;           // For rounding
                lwv /= m;           // Freq = N*crystal/P/M
                                       
                lwv2 = lwv;          // Save a copy

                // Because the only divide in computing N is by the
                // crystal frequency, the remainder will always be
                // less than the crystal frequency which fits into
                // a word, so we can do the delta math on words
                //
                // Compute delta
                
                lwvs = (U016)(lwv & 0xFFFF);
                vclks = (U016)(vclk & 0xFFFF);
                
                if (lwvs < vclks)
                {               
                    // Exchange the values
                    t = lwvs;
                    lwvs = vclks;
                    vclks = t;
                }            
                    
                lwvs -= vclks;      // ABS(Freq-Vclk);
                
                //
                // If this is an NV5 or better, allow the higher PCLK settings to use a lower M
                // if the error is small enough.  Just looking for the lowest overall error
                // may result in an M that is too high, possibly introducing PLL jitter.
                //
                if (IsNV5orBetter(pDev))
                {                
                    //
                    // If VCLK > 250MHz and error is less than 0.2%, drop out
                    //
                    if (vclk > hiVclk1)
                    {
                        if (lwvs == 0) // zero error; rare, but would cause exception
                        {
                            *P = powerP;
                            *M = m;
                            *N = n;
                            lwv2 /= 10;
                            return ((U016)(lwv2 & 0xFFFF));
                        } 
                        else if ((vclk/lwvs) > 200) // ok, now safe to div by lwvs (it's not zero)
                        {
                            *P = powerP;
                            *M = m;
                            *N = n;
                            lwv2 /= 10;
                            return ((U016)(lwv2 & 0xFFFF));
                        } 
                    }
                    
                    if (lwvs < deltaOld) // go to closest match with M < highM
                    {
                        // Closer match
                        deltaOld = lwvs;
                        
                        bestP = powerP;
                        bestM = m;
                        bestN = n;
                        
                        lwv2 /= 10;
                        
                        vclkClosest = (U016)(lwv2 & 0xFFFF);
                    }
                                            
                }
                else // !NV5
                {
                    if (lwvs < deltaOld)
                    {
                        // Closer match
                        deltaOld = lwvs;
                        
                        bestP = powerP;
                        bestM = m;
                        bestN = n;
                        
                        lwv2 /= 10;
                        
                        vclkClosest = (U016)(lwv2 & 0xFFFF);
                    }
                }
            }
        }
    }                             
    
    // Verify we've established M, N, P values to use
    if (!bestM && !bestN && !bestP)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: dacCalcMNP didn't set any M, N, P values!!!\n");
        DBG_BREAKPOINT();
    }

    //
    // Return the results
    //
    *M = bestM;
    *N = bestN;
    *P = bestP;
    
    return (vclkClosest);
} 


//
//       dacCalcMNP_MaxM - Return the PLL values for a specified frequency while
//                          abiding by the given maximum M value
//
//          Clock frequency in MHz * 100
//
// The algorithm is constrained to keep within 0.5% error
// to remain inside the VBE spec.
// However, if you have a choice of what frequencies you
// request, try to avoid the following requested frequencies
// when you get above 300MHz, as they require M values > 3.
//
//  14.31818 MHz
// 29739 - 29918
// 30219 - 30393
// 31178 - 31343
// 31658 - 31818
// 32617 - 32767
// 33097 - 33243
// 34056 - 34193
// 34536 - 34668
//
//  13.5000 MHz
// 29397 - 29552
// 29849 - 30000
// 30754 - 30896
// 31206 - 31344
// 32110 - 32239
// 32563 - 32767
// 33467 - 33582
// 33919 - 34030
// 34824 - 34926
//
U032 dacCalcMNP_MaxM(
    PHWINFO  pDev,
    U032     Head,
    U032     Clock,
    U032    *M,    
    U032    *N,    
    U032    *P,
    U032    maxM
)
{
    U016  crystalFreq;
    U032  vclk;
    U016  vclkClosest = 0;
    U016  deltaOld;
    U016  bestM = 0;
    U016  bestP = 0;
    U016  bestN = 0;
    U016  lowM;
    U016  highM;
    U016  lowP, highP;
    U032  hiVclk1, hiVclk2, hiVclk3;
    U032  FminVco, FmaxVco;

    U016  powerP;
    U016  m;
    U016  n;
    U016  i, j;
    U032  lwv, lwv2;
    
    U016  lwvs, vclks;
    U016  t;
    
    vclk = (U032)(Clock * 10);     // Make KHz
    
    deltaOld = 0xFFFF;
    
    //
    // NV11 needs four highM buckets due to
    // induced jitter into the PLL's
    //
    if (IsNV11(pDev))
    {
        hiVclk1 = 150000;
        hiVclk2 = 200000;
        hiVclk3 = 340000;
    }
    else
    {
        hiVclk1 = 250000;
        hiVclk2 = 340000;

    }
    
    lowP  = 0;
    highP = 4;

    // if initDac() has run then FminVco and FmaxVco will be set.
    // But we might be called during devinit processing prior to
    //    initDac(), so we need a fallback...
    // NOTE: these defaults duplicated in initDac()
    if ((pDev->Dac.FminVco == 0) || (pDev->Dac.FmaxVco == 0))
    {
        if (IsNV15orBetter(pDev)) {
            if (IsNV15MaskRevA01(pDev))
                pDev->Dac.FminVco = 200000;
            else
                pDev->Dac.FminVco = 250000;
            
            pDev->Dac.FmaxVco = pDev->Dac.FminVco * 2; 
        } else {
            pDev->Dac.FminVco = 128000;
            pDev->Dac.FmaxVco = 256000;
        }
    }
    
    FminVco = pDev->Dac.FminVco;
    FmaxVco = pDev->Dac.FmaxVco;

    //
    // NV11 requires a modified Vco range, and since the BIOS's are already 
    // out in the field, shortcircuit this nice feature to fix the noise issue.
    //
    // When we're sure all older BIOS's have been flushed out of the world
    // (need 3.11.01.25.00+), this code can be removed.  If we detect a newer
    // BIOS version is in use, go ahead and use those values
    //
    // Mobile NV11 has always used these ranges, so this should not be a change
    // for them.
    //
    if (IsNV11(pDev) && (pDev->Chip.BiosRevision <= 0x03110125))
    {
        FminVco = 200000;
        FmaxVco = 400000;
    }

    // Apply any adjusts needed to FminVco/FmaxVco.
    //
    // 1). Make sure vclk is not so low that we'll miss the whole VCO range.
    //     Adjust FminVco to hit with the highest P (post divide).
    //
    if ((vclk << highP) < FminVco)
        FminVco = vclk << highP;

    //
    // 2). Also, check if two consecutive P values would cause us to miss
    //     the whole VCO range, where the first P is beneath FminVco and
    //     the next P is above FmaxVco.
    //
    for (powerP = lowP; powerP < highP; powerP++)
    {
        lwv = vclk << powerP;
        if ((lwv < FminVco) && ((lwv << 1) > FmaxVco))
        {
            // adjust FminVco or FmaxVco, whichever is closer
            if ((FminVco - lwv) < ((lwv << 1) - FmaxVco))
                FminVco = lwv;
            else
                FmaxVco = (lwv << 1);
        }
    }

    //
    // 3). Let a high vclk set the upper bound.
    //
    if (vclk > FmaxVco)
        FmaxVco = vclk;

    //
    // For NV15, we've got a bit in CR33 that indicates we're setting the
    // PLL in the "upper" VCO range.
    //
    if (IsNV15(pDev))
    {
        U008 lock, cr33;

        // unlock the extended regs
        lock = UnlockCRTC(pDev, Head);

        // put NV15 into NV15 VPLL VCO programming mode
        CRTC_RD(NV_CIO_CRE_LCD__INDEX, cr33, Head);
        cr33 |= (1 << DRF_SHIFT(NV_CIO_CRE_LCD_VPLL_VCO));
        CRTC_WR(NV_CIO_CRE_LCD__INDEX, cr33, Head);

        RestoreLock(pDev, Head, lock);
    }

    //
    // Determine the current crystal frequency
    //   
    if (pDev->Chip.HalInfo.CrystalFreq == 14318180)
    {
        // 14.3Khz
        crystalFreq = 14318;

        lowM = 1;
             
        if (IsNV11(pDev))
        {
            //
            // NV11 needs four highM buckets due to
            // induced jitter into the PLL's
            //
            if (vclk > hiVclk3)
                highM = 2;
            else if (vclk > hiVclk2)
                highM = 4;
            else if (vclk > hiVclk1)
                highM = 6;
            else
                highM = 14;

        }
        else
        {
            //
            // All chips except for NV11 only need three
            // highM buckets
            //
            if (vclk > hiVclk2)
                highM = 2;
            else if (vclk > hiVclk1)
                highM = 6;
            else
                highM = 14;
          
        }
        
    }
    else
    {
        // 13.5Khz
        crystalFreq = 13500;

        lowM = 1;

        //
        // NV11 needs four highM buckets due to
        // induced jitter into the PLL's
        //
        if (IsNV11(pDev))
        {
            if (vclk > hiVclk3)
                highM = 2;
            else if (vclk > hiVclk2)
                highM = 4;
            else if (vclk > hiVclk1)
                highM = 6;
            else
                highM = 13;
        }
        else
        {
            //
            // All chips except for NV11 only need three
            // highM buckets
            //
            if (vclk > hiVclk2)
                highM = 2;
            else if (vclk > hiVclk1)
                highM = 6;
            else
                highM = 13;
        }

    }                      
    

    // Make sure we haven't gone past the maximum M
    if (highM > (U016)maxM) {
      highM = (U016)maxM;
    }


    //
    // P Loop
    //
    for (powerP = lowP; powerP <= highP; powerP++) // Init Power of P
    {                             
        // M Loop
        for (m = lowM; m <= highM; m++)
        {                          
            lwv = vclk << powerP;   // vclk * P
            
            // Compare with internal range of DAC
            if ((lwv >= FminVco) && (lwv <= FmaxVco))
            {
                lwv *= (U032)m;    // vclk * P * M
                
                //
                // In case of normal incoming clock, 
                // want to select a clock that is close
                // but can be greater or less than the target
                // In case of SpreadSpectrum want to be 
                // EQUAL or LOWER than the target frequency
                //
                // !! always skip this when searching with a limited M value
                //
                // Add crystal Frequency /2 - will round better
                //  lwv += (crystalFreq >> 1);
                
                lwv /= crystalFreq; // N = vclk*P*M/crystal
                n = (U016)(lwv & 0xFFFF);
                
                if (n > 0xFF)
                    break;
                    
                lwv *= crystalFreq; // N * crystal
                   
                i = 1 << powerP;    // for rounding
                i >>= 1;            // /2
                
                lwv += i;           // Add 1/2 of P
                lwv >>= powerP;     // N*cyrstal/P
                
                i = m;
                j = m;
                
                j >>= 1;            // M/2
                lwv += j;           // For rounding
                lwv /= m;           // Freq = N*crystal/P/M
                                       
                lwv2 = lwv;          // Save a copy

                // Because the only divide in computing N is by the
                // crystal frequency, the remainder will always be
                // less than the crystal frequency which fits into
                // a word, so we can do the delta math on words
                //
                // Compute delta
                
                lwvs = (U016)(lwv & 0xFFFF);
                vclks = (U016)(vclk & 0xFFFF);
                
                if (lwvs < vclks)
                {               
                    // Exchange the values
                    t = lwvs;
                    lwvs = vclks;
                    vclks = t;
                }            
                    
                lwvs -= vclks;      // ABS(Freq-Vclk);
                
                //
                // If this is an NV5 or better, allow the higher PCLK settings to use a lower M
                // if the error is small enough.  Just looking for the lowest overall error
                // may result in an M that is too high, possibly introducing PLL jitter.
                //
                if (IsNV5orBetter(pDev))
                {                
                    //
                    // If VCLK > 250MHz and error is less than 0.2%, drop out
                    //
                    if (vclk > hiVclk1)
                    {
                        if (lwvs == 0) // zero error; rare, but would cause exception
                        {
                            *P = powerP;
                            *M = m;
                            *N = n;
                            lwv2 /= 10;
                            return ((U016)(lwv2 & 0xFFFF));
                        } 
                        else if ((vclk/lwvs) > 200) // ok, now safe to div by lwvs (it's not zero)
                        {
                            *P = powerP;
                            *M = m;
                            *N = n;
                            lwv2 /= 10;
                            return ((U016)(lwv2 & 0xFFFF));
                        } 
                    }
                    
                    if (lwvs < deltaOld) // go to closest match with M < highM
                    {
                        // Closer match
                        deltaOld = lwvs;
                        
                        bestP = powerP;
                        bestM = m;
                        bestN = n;
                        
                        lwv2 /= 10;
                        
                        vclkClosest = (U016)(lwv2 & 0xFFFF);
                    }
                                            
                }
                else // !NV5
                {
                    if (lwvs < deltaOld)
                    {
                        // Closer match
                        deltaOld = lwvs;
                        
                        bestP = powerP;
                        bestM = m;
                        bestN = n;
                        
                        lwv2 /= 10;
                        
                        vclkClosest = (U016)(lwv2 & 0xFFFF);
                    }
                }
            }
        }
    }                             
    
    // Verify we've established M, N, P values to use
    if (!bestM && !bestN && !bestP)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: dacCalcMNP didn't set any M, N, P values!!!\n");
        DBG_BREAKPOINT();
    }

    //
    // Return the results
    //
    *M = bestM;
    *N = bestN;
    *P = bestP;
    
    return (vclkClosest);
} 


static U032 CalculateClockFrequency
(
   PHWINFO pDev,
   U032 M,
   U032 N,
   U032 P
)
{
    U032 Fout;
    // Fout = Fin * (N/M) / 2**P.
    U032 Fin = pDev->Chip.HalInfo.CrystalFreq;

    if ((M * (1 << P)) == 0)
       return 0;

    Fout = (Fin * N) / (M * (1 << P));

    return Fout;
}

//
// Clock programming methods
//

//
// dacProgramMClk - Program MCLK based on the value in pDev->Dac.HalInfo.MClk
//
// This routine uses the value in pDev->Dac.HalInfo.MClk (in Hz) to calculate
// M, N, and P values.  Those values are then stored in the pDev->Dac
// structure.  The hardware is then programmed with the new M, N, and P
// values in a manner that is safe for the PLL.
//
// If Ponly is TRUE, then we will only raise and lower P get as close to the desired
// clock speed as possible without going above or below it.
//
RM_STATUS dacProgramMClk
(
    PHWINFO pDev,
    U032    DisableDLLs
)
{
    U032 mNew;
    U032 nNew;
    U032 pNew;
    U032 Head = 0;  // MPLL is on head 0
    RM_STATUS status;

    // Compute M, N, O, and P values
    dacCalcMNP( pDev, Head, pDev->Dac.HalInfo.MClk / 10000, &mNew, &nNew, &pNew );

    // Update Dac structure
    pDev->Dac.HalInfo.MPllM = mNew;
    pDev->Dac.HalInfo.MPllN = nNew;
    pDev->Dac.HalInfo.MPllO = 1;
    pDev->Dac.HalInfo.MPllP = pNew;

    status = nvHalDacProgramMClk(pDev, DisableDLLs);

    return status;
}

//
// dacProgramNVClk - Program NVCLK based on the value in
// pDev->Dac.HalInfo.NVClk
//
// This routine uses the value in pDev->Dac.HalInfo.NVClk (in Hz) to calculate
// M, N, and P values.  Those values are then stored in the pDev->Dac
// structure.  The hardware is then programmed with the new M, N, and P
// values in a manner that is safe for the PLL.
//
RM_STATUS dacProgramNVClk
(
    PHWINFO pDev 
)
{
    U032 mNew;
    U032 nNew;
    U032 pNew;
    U032 Head = 0; // NVPLL is on head 0

    // Compute M, N, O, and P values
    dacCalcMNP(pDev, Head, pDev->Dac.HalInfo.NVClk / 10000, &mNew, &nNew, &pNew);

    // Update Dac structure
    pDev->Dac.HalInfo.NVPllM = mNew;
    pDev->Dac.HalInfo.NVPllN = nNew;
    pDev->Dac.HalInfo.NVPllO = 1;
    pDev->Dac.HalInfo.NVPllP = pNew;

    return(nvHalDacProgramNVClk(pDev));
}

RM_STATUS dacGetDisplayInfo
(
    PHWINFO pDev,
    U032 DisplayMap,
    U032 *Head,
    U032 *DisplayType,
    U032 *I2CWritePort,
    U032 *I2CReadPort
)
{
    U032 flag = 0;
    PDACDCBDEVDESC pDevDesc;
    pDACDCBI2CRECORD pDCBI2c;

    pDevDesc = pDev->Dac.DCBDesc;
    pDCBI2c  = pDev->Dac.DCBI2c;

    while (pDevDesc->DCBRecType != DCB_DEV_REC_TYPE_EOL)
    {
        if (pDevDesc->DevTypeUnit == DisplayMap)
        {
            flag = 1;
            break;
        }
        pDevDesc++;
        pDCBI2c++;
    }

    if (!flag) return RM_ERROR;

    if (Head)         *Head = pDevDesc->DCBRecHead;
    if (DisplayType)  *DisplayType = pDevDesc->DCBRecType;
    if (I2CWritePort) *I2CWritePort = pDCBI2c->WritePort;
    if (I2CReadPort)  *I2CReadPort = pDCBI2c->ReadPort;

    return RM_OK;
}

//
// dacProgramPClk - Program PCLK based on the value in pDev->Dac.PClk
//
// This routine uses the value in pDev->Dac.PClk (in Hz) to calculate
// M, N, and P values.  Those values are then stored in the pDev->Dac
// structure.  The hardware is then programmed with the new M, N, and P
// values in a manner that is safe for the PLL.
//
RM_STATUS dacProgramPClk
(
    PHWINFO pDev, 
    U032    Head,
    U032    PixelClock
)
{
    VIDEO_LUT_CURSOR_DAC_HAL_OBJECT vidLutCurDacHalObj;
    U032 mNew;
    U032 nNew;
    U032 pNew;

    // Compute M, N, O, and P values
    dacCalcMNP( pDev, Head, PixelClock, &mNew, &nNew, &pNew );

    // Update Dac structure
    pDev->halHwInfo.pDacHalInfo->VPllM = mNew;
    pDev->halHwInfo.pDacHalInfo->VPllN = nNew;
    pDev->halHwInfo.pDacHalInfo->VPllO = 1;
    pDev->halHwInfo.pDacHalInfo->VPllP = pNew;

    vidLutCurDacHalObj.Head = Head;

    return(nvHalDacProgramPClk(pDev, PixelClock, (VOID*) &vidLutCurDacHalObj));
}

// This function just enables the cursor by setting bit 0 of config register 0x31 to 1.
// The cursor image is not changed in any way.
VOID dacEnableCursor
(
    PHWINFO pDev,
    U032    Head
)
{
    U008    cr31;
    U008    lock;
    
    lock = UnlockCRTC(pDev, Head);
    
    CRTC_RD(NV_CIO_CRE_HCUR_ADDR1_INDEX, cr31, Head);           
    CRTC_WR(NV_CIO_CRE_HCUR_ADDR1_INDEX, (cr31 | (1 << DRF_SHIFT(NV_CIO_CRE_HCUR_ADDR1_ENABLE))), Head);

    RestoreLock(pDev, Head, lock);
}

// This function just disables the cursor by setting bit 0 of config register 0x31 to 0.
// The cursor image is not changed in any way.
VOID dacDisableCursor
(
    PHWINFO pDev,
    U032    Head
)
{
    U008    cr31;
    U008    lock;
    
    lock = UnlockCRTC(pDev, Head);
    
    CRTC_RD(NV_CIO_CRE_HCUR_ADDR1_INDEX, cr31, Head);           
    CRTC_WR(NV_CIO_CRE_HCUR_ADDR1_INDEX, (cr31 & ~(1 << DRF_SHIFT(NV_CIO_CRE_HCUR_ADDR1_ENABLE))), Head);

    RestoreLock(pDev, Head, lock);
}

/*  This function is mis-named. It also loads the cursor image offset in addition to enabling the
    cursor. This is used by the old style of cursor manipulation. 
*/
VOID enableCursor
(
    PHWINFO pDev,
    U032    Head
)
{
    U008    lock;
    
    // unlock the extended regs
    lock = UnlockCRTC(pDev, Head);
    
#ifdef NTRM    
    // Tell the DAC where to load the image from
    // The address is in pages, so shift 4 bits.
    if (REG_RD08(NV_PRMVIO_MISC__READ) & 0x00000001)    // color or mono?
    {
        CRTC_WR(NV_CIO_CRE_HCUR_ADDR0_INDEX, (pDev->Dac.CursorImageInstance >> 12) & 0x7f, Head);
        CRTC_WR(NV_CIO_CRE_HCUR_ADDR1_INDEX, ((pDev->Dac.CursorImageInstance >> 4) & 0xf8), Head);
    }
    else
    {
        // call crtc_wrm: need to add this routine
        CRTC_WRM(NV_CIO_CRE_HCUR_ADDR0_INDEX, (pDev->Dac.CursorImageInstance >> 12) & 0x7f);
        CRTC_WRM(NV_CIO_CRE_HCUR_ADDR1_INDEX, ((pDev->Dac.CursorImageInstance >> 4) & 0xf8) | 1);
    }
#else
    // Tell the DAC where to load the image from. The address is in pages, so shift 4 bits.
#ifdef WIN31
    // On NV4 and later HCUR_ADDR0 is 23:17, HCUR_ADDR1 is 16:11 (on NV3 its 22:16)
    CRTC_WR(NV_CIO_CRE_HCUR_ADDR0_INDEX, (pDev->Dac.CursorImageInstance >> 13) & 0x7f);
    CRTC_WR(NV_CIO_CRE_HCUR_ADDR1_INDEX, ((pDev->Dac.CursorImageInstance >> 5) & 0xfc) | 1);
//JohnH The Win9x code below is incorrect for NV4 and up, but the correct address
//JohnH gets set in dacProgramCursorImage.
//JohnH CRTC_WR(NV_CIO_CRE_HCUR_ADDR0_INDEX, (pDev->Dac.CursorImageInstance >> 12) & 0x7f);
//JohnH CRTC_WR(NV_CIO_CRE_HCUR_ADDR1_INDEX, (pDev->Dac.CursorImageInstance >> 4) & 0xf8);
#endif // WIN31
#endif // NTRM
    
    // JJV - I am removing this call. Vblank will call this later if needed.
    //       This will fix a multimonitor cursor problem.  
    // Enable the cursor.
    //dacEnableCursor(pDev);

    RestoreLock(pDev, Head, lock);
}

/*  Turn off the enable bit in CRTC reg
    This function unlocks the CRTC extended registers and calls dacDisableCursor.
    This is used by the old style cursor manipulation functions. */
VOID disableCursor
(
    PHWINFO pDev,
    U032    Head
)
{
    U008 lock;

#ifndef NTRM
    pDev->Dac.CrtcInfo[Head].CursorType = DAC_CURSOR_DISABLED;    // disable updates in vblank
#endif

    // unlock the extended regs
    lock = UnlockCRTC(pDev, Head);

    dacDisableCursor(pDev, Head);

    RestoreLock(pDev, Head, lock);
}

RM_STATUS dacProgramCursorPosition
(
    PHWINFO pDev, 
    U032    Head,
    U032    cursorX, 
    U032    cursorY 
)
{
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;

    // on modes below 400 lines, scanline doubling is used. we need to adjust cursor y postion
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)pDev->Dac.CrtcInfo[Head].pVidLutCurDac;
    if (pVidLutCurDac && pVidLutCurDac->HalObject.Dac[0].Format.DoubleScanMode)
        cursorY *= 2;

//    if (CRTCHALINFO(pDev, Head, DisplayType) == TV)
        cursorX += pDev->Framebuffer.CursorAdjust;  // HW bug in NV4, position is off a little on TV

    DAC_REG_WR32(NV_PRAMDAC_CU_START_POS, cursorX | (cursorY << 16), Head);

    return RM_OK;
}

VOID dacDisableImage
(
    PHWINFO pDev,
    U032 Head
)
{
    U016 data;
    U032 prevHead = (pDev->Dac.HalInfo.Head2RegOwner & 0x1);

    EnableHead(pDev, Head);        // sequencer updates require setting CR44 

    // turn on blank
    data = ReadIndexed(NV_PRMVIO_SRX, 0x01);
    data |= 0x2000;
    WriteIndexed(NV_PRMVIO_SRX, data);

    EnableHead(pDev, prevHead);    // restore previous CR44 value
}

VOID dacEnableImage
(
    PHWINFO pDev,
    U032 Head
)
{
    U016 data;
    U032 prevHead = (pDev->Dac.HalInfo.Head2RegOwner & 0x1);

    EnableHead(pDev, Head);        // sequencer updates require setting CR44 

    // turn off blank
    data = ReadIndexed(NV_PRMVIO_SRX, 0x01);
    data &= ~0x2000;
    WriteIndexed(NV_PRMVIO_SRX, data);

    EnableHead(pDev, prevHead);    // restore previous CR44 value
}


VOID dacEnableDac
(
    PHWINFO pDev,
    U032 Head
)
{
    U008 lock, data08;    
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    
    if (!IsNV11(pDev) && (Head == 1)) // if not dual-head
    {
        return;
    }
    
    // unlock the extended regs
    lock = UnlockCRTC(pDev, Head);

    // enable vblank interrupt
    DAC_REG_WR32(NV_PCRTC_INTR_EN_0, 1, Head);
    pDev->Dac.CrtcInfo[Head].StateFlags |= DAC_CRTCINFO_VBLANK_ENABLED;
    
    // device-specific disabling...
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)pDev->Dac.CrtcInfo[Head].pVidLutCurDac;
    if (pVidLutCurDac)
    {
        switch (pVidLutCurDac->DisplayType)
        {
            case DISPLAY_TYPE_MONITOR:
        
                // TO DO: this decision must be made by the HAL
                // if normal VGA (internal DAC)
                if (Head == 0) 
                {
                    dacEnableImage(pDev, Head);
                    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _ON, Head);  // CRT on
                    //
                    // We need to be careful about powering down the TMDS
                    // transmitter on NV11 because we can lose a flat
                    // panel connected to the second crtc (head 1).
                    //
                    if (IsNV11(pDev))
                        dacWriteLinkPLL(pDev, Head, NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_FPCLK);     // FPCLK off only;leave TMDS going for secondary crtc
                    else
                        dacWriteLinkPLL(pDev, Head, NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_BOTH);      // FPCLK/TMDS both off
                    CRTC_RD(0x33, data08, Head);
                    CRTC_WR(0x33, (data08 & ~0x02), Head);  // clear CR33[1] (BIOS flag)
                    CRTC_RD(NV_CIO_CRE_RPC1_INDEX, data08, Head);
                    CRTC_WR(NV_CIO_CRE_RPC1_INDEX, data08 & 0x3F, Head);
                    dacSetCursorBlink(pDev, Head, 0); // set cursor blink rate for DOS
                }
        
                // PRAMDAC_FP is driving analog
                else
                {
                    // restore sync states
                    DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_TG_CONTROL, _VSYNC, pDev->Dac.CrtcInfo[Head].CurrentVsyncState, Head);
                    DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_TG_CONTROL, _HSYNC, pDev->Dac.CrtcInfo[Head].CurrentHsyncState, Head);
                    dacWriteLinkPLL(pDev, Head, NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_NONE);
                }
            
                break;   
                                       
            case DISPLAY_TYPE_FLAT_PANEL:
        
                // restore sync states
                DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_TG_CONTROL, _VSYNC, pDev->Dac.CrtcInfo[Head].CurrentVsyncState, Head);
                DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_TG_CONTROL, _HSYNC, pDev->Dac.CrtcInfo[Head].CurrentHsyncState, Head);
                dacWriteLinkPLL(pDev, Head, NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_NONE);

                // Turn Hsync off and Vsync on, or leave both on, depending on what we found in registry
                CRTC_RD(NV_CIO_CRE_RPC1_INDEX, data08, Head);
                data08 &= ~0xC0;
                if (pDev->Dac.HsyncOff) 
                {
                    CRTC_WR(NV_CIO_CRE_RPC1_INDEX, data08 |= 0x80, Head); // disable horiz sync to put CRT in suspend
                }
                else
                {
                    CRTC_WR(NV_CIO_CRE_RPC1_INDEX, data08, Head);    // leave both syncs on
                }

                if (pDev->Power.MobileOperation)
                    dacPowerOnMobilePanel(pDev,Head);

                //
                // Slave the CRTC again off the FP (this complements what was done
                // in dacDisableDac for the FP).
                //
                CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, data08, Head);
                data08 |= 0x80;        // set bit 7
                CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, data08, Head);    
                break;                              
            
            case DISPLAY_TYPE_TV:
                // Turn Hsync off and Vsync on, or leave both on, depending on what we found in registry
                CRTC_RD(NV_CIO_CRE_RPC1_INDEX, data08, Head);
                data08 &= ~0xC0;
                if (pDev->Dac.HsyncOff) 
                {
                    CRTC_WR(NV_CIO_CRE_RPC1_INDEX, data08 |= 0x80, Head); // disable horiz sync to put CRT in suspend
                }
                else
                {
                    CRTC_WR(NV_CIO_CRE_RPC1_INDEX, data08, Head);    // leave both syncs on
                }
                dacEnableTV(pDev, Head);
                break;
            
            default:
                break;
        }
    }

    RestoreLock(pDev, Head, lock);

} // end of dacEnableDac()

VOID dacDisableDac
(
    PHWINFO pDev,
    U032 Head
)
{
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;

    if (!IsNV11(pDev) && (Head == 1)) // if not dual-head
    {
        return;
    }

    if (MC_POWER_LEVEL_0 != pDev->Power.State) {
        return;
        }

    // disable vblank interrupt, if there are no callbacks pending
    if (pDev->Dac.CrtcInfo[Head].VBlankCallbackList == NULL)
    {
        DAC_REG_WR_DRF_DEF(_PCRTC, _INTR_0, _VBLANK, _RESET, Head);
        DAC_REG_WR32(NV_PCRTC_INTR_EN_0, 0, Head);
        pDev->Dac.CrtcInfo[Head].StateFlags &= ~DAC_CRTCINFO_VBLANK_ENABLED;
    }

    // device-specific disabling...
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)pDev->Dac.CrtcInfo[Head].pVidLutCurDac;
    if (pVidLutCurDac)
    {
        pDev->Dac.CrtcInfo[Head].MonitorPowerSetMode = FALSE;
        switch (pVidLutCurDac->DisplayType)
        {
            case DISPLAY_TYPE_MONITOR:
        
                // TO DO: this decision must be made by the HAL
                // head 0 -- treat as CRT
                if (Head == 0)
                {
                    // disable hsync
                    if (pDev->Dac.HsyncOff) 
                        dacSetSyncState(pDev, Head, 0, 1); // H off, V on
                    else
                        dacSetSyncState(pDev, Head, 0, 0); // H off, V off
                    dacDisableImage(pDev, Head);
                    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _OFF, Head);
                }

                // head 1 -- treat as DFP
                else
                {    
                    // disable hsync
                    if (pDev->Dac.HsyncOff) 
                        dacSetSyncState(pDev, Head, 0, 1); // H off, V on
                    else
                        dacSetSyncState(pDev, Head, 0, 0); // H off, V off
                    dacWriteLinkPLL(pDev, Head, NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_BOTH);
                }
                break;
            
            case DISPLAY_TYPE_FLAT_PANEL:
            {
                U008 lock, data;

                //Don't mess with the clocks on a Toshiba system unless we know that the
                //SBIOS has been informed of the switch.  They will turn off the panel
                //power for us, an event that should occur within a not-too-large time
                //window of turning off the clocks to spare the display.
                if ((pDev->Power.MobileOperation == 2) && !pDev->Dac.DisplayChangeInProgress)
                    break;

                // if we're mobile, turn the power off as well
                if (pDev->Power.MobileOperation)
                    dacPowerOffMobilePanel(pDev, Head);

                //
                // Unslave the CRTC from the FP (the BIOS uses the state of CR28
                // and CR33 to determine if a FP is active on this head). 
                //
                lock = UnlockCRTC(pDev, Head);

                CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, data, Head);
                data &= ~0x80;        // clear bit 7
                CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, data, Head);    
           
                RestoreLock(pDev, Head, lock);
                
                // NOTE: do not turn off FPCLK before unslaving CRTC
                dacWriteLinkPLL(pDev, Head, NV_PRAMDAC_FP_DEBUG_0_PWRDOWN_BOTH);
                break;
            }            

            case DISPLAY_TYPE_TV:
            {
                dacSetSyncState(pDev, Head, 0, 0); // H off, V off (TV is disabled, CRT is powered down)
                // this has to be encoder dependent
                dacDisableTV(pDev, Head);
                break;
            }
            
            default:
                break;
        }
    }
    else
    {
        U008 lock, data;

        // Since pVidLutCurDac is NULL, pDev->Dac.DevicesEnabled will tell us if the flat panel is on.
        // If we're in a mobile environment on Head 1 and the flat panel is on,
        // go ahead and turn it off. This fixes the stand-by problem with 32095.
        if((Head == 1) && (pDev->Power.MobileOperation) && (pDev->Dac.DevicesEnabled & DAC_PANEL_POWER_ENABLED))
            dacPowerOffMobilePanel(pDev, Head);

        //
        // Disable in a display device independent way (i.e. drop the monitor
        // syncs, powerdown the DAC and turnoff sequencer memory fetches).
        //
        dacSetSyncState(pDev, Head, 0, 0); // H off, V off (no device is being driven off this head)
        dacDisableImage(pDev, Head);
        DAC_FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _OFF, Head);
        //
        // Unslave the CRTC from the FP (the BIOS uses the state of CR28
        // and CR33 to determine if a FP is active on this head). 
        // This catches cases where we declined to disable the hardware
        // when we originally deallocated the object.  Solves problems
        // with Toshiba mobile.
        lock = UnlockCRTC(pDev, Head);

        CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, data, Head);
        data &= ~0x80;        // clear bit 7
        CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, data, Head);    

        RestoreLock(pDev, Head, lock);
    }

} // end of dacDisableDac()

RM_STATUS dacProgramVideoStart
(
    PHWINFO pDev, 
    U032    Head,
    U032    StartAddr, 
    U032    Pitch 
)
{
    if (pDev->pStereoParams && (pDev->pStereoParams->Flags & (STEREOCFG_STEREOACTIVATED|STEREOCFG_INTERLACED)) 
        == (STEREOCFG_STEREOACTIVATED|STEREOCFG_INTERLACED))
    {
        // Asus stereo uses interlaced video modes.
        Pitch >>= 1;
    }
    if (!pDev->Vga.Enabled)
    {
        VIDEO_LUT_CURSOR_DAC_HAL_OBJECT vidLutCurDacHalObj;

        vidLutCurDacHalObj.Head = Head;

        //
        // If the pitch of the RGB surface does not match the current display width, we
        // need to tweak the CRTC to a different pitch adjust.
        //
        // Compare Pitch against pDev->Framebuffer.DisplayPitch rather than trying to
        // calculate it, since DisplayPitch has been aligned for the HW already (e.g. NV10).
        //
        // Checking against DisplayPitch actually doesn't save us in the case where the 2
        // surfaces have different pitch values. This really needs to be checking against
        // the last loaded pitch ... but then we'd be programming more than just the start
        // addr 1/2 the time.
        //
        // For now, print out a message, so the driver can be changed to flip between
        // surfaces of the same pitch making our job easier and giving us the best perf.
        //        
//#if 0 // This was used for video class63, but later classes take care of this case
//        if (Pitch != pDev->Framebuffer.DisplayPitch)
        {

            U008      i, lock, crtc_index;
            U032      val32;

//            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Flipping between different pitched surfaces!!!\n\r");

            //
            // Save the current CRTC index
            //
            if (REG_RD08(NV_PRMVIO_MISC__READ) & 0x01)    // color or mono?
                crtc_index = REG_RD08(NV_PRMCIO_CRX__COLOR);    // save crtc index
            else
                crtc_index = REG_RD08(NV_PRMCIO_CRX__MONO); // save crtc index

            // unlock the extended regs
            lock = UnlockCRTC(pDev, Head);

            // Write the alternative pitch to the CRTC Offset Register
            // The value written to CR13/CR19 is the number of 8-byte chunks
            CRTC_WR(NV_CIO_CR_OFFSET_INDEX, (Pitch >> 3) & 0xFF, Head);

            // Modify the bits in the extended register as well
            CRTC_RD(NV_CIO_CRE_RPC0_INDEX, i, Head);
            i &= 0x1F;
            val32 = ((Pitch >> 3) & 0x700) >> 3;
            i |= (U008)val32; 
            CRTC_WR(NV_CIO_CRE_RPC0_INDEX, i, Head);

            RestoreLock(pDev, Head, lock);

            // Restore index
            if (REG_RD08(NV_PRMVIO_MISC__READ) & 0x01)
                REG_WR08(NV_PRMCIO_CRX__COLOR, crtc_index); // restore crtc index
            else
                REG_WR08(NV_PRMCIO_CRX__MONO, crtc_index);	// restore crtc index
        }
//#endif

        // Program the new start address; this will latch at the end of blank
        if (pDev->pStereoParams)
        {
            pDev->pStereoParams->FlipOffsets[3][0] = StartAddr;
            if ((pDev->pStereoParams->Flags & (STEREOCFG_STEREOACTIVATED|STEREOCFG_INTERLACED)) == STEREOCFG_STEREOACTIVATED)
            {
                return RM_OK;
            }
        }
        nvHalDacSetStartAddr(pDev, StartAddr, &vidLutCurDacHalObj);
    }
    return RM_OK;
}

RM_STATUS dacProgramLUT
(
    PHWINFO pDev, 
    U032    Head,
    U032*   Palette, 
    U032    numEntries 
)
{
    U032    i, rgb;

    // start at palette 0 and autoincrement
    DAC_REG_WR08(NV_USER_DAC_WRITE_MODE_ADDRESS, 0, Head);
    for (i = 0; i < numEntries; i++)
    {
        rgb = Palette[i];
        DAC_REG_WR08(NV_USER_DAC_PALETTE_DATA, (U008)(rgb >> 16), Head);    // b
        DAC_REG_WR08(NV_USER_DAC_PALETTE_DATA, (U008)(rgb >> 8), Head);     // g
        DAC_REG_WR08(NV_USER_DAC_PALETTE_DATA, (U008)rgb, Head);            // r
    }
    return RM_OK;
}

RM_STATUS dacProgramScanRegion
(
    PHWINFO pDev,
    U032    Head,
    U032    startLine,
    U032    stopLine,
    U032    polarity
)
{
    if (IsNV15(pDev) || IsNV11(pDev) || IsNV1A(pDev) || IsNV20(pDev))
    {
        DAC_REG_WR32(NV_PCRTC_RASTER_START, startLine | DRF_NUM(_PCRTC, _RASTER_START, _POL, polarity), Head);
        DAC_REG_WR32(NV_PCRTC_RASTER_STOP,  stopLine, Head);
        return (RM_OK);    
    }
    else
        return (RM_ERROR);
}

/************************************************************
* Here is how the H & V blank and retrace values map to the config registers:
    Hbs<0:7> = CR02<0:7>
    Hbe<0:4> = CR03<0:4>
    Hbe<5>   = CR05<7>
    Hbe<6>   = CR25<4>
    Vbs<0:7> = CR15<0:7>
    Vbs<8>   = CR07<3>
    Vbs<9>   = CR09<5>
    Vbs<10>  = CR25<3>
    Vbe<0:7> = CR16<0:7>
    
    Hrs<0:7> = CR04<0:7>
    Hre<0:4> = CR05<0:4>
    Vrs<0:7> = CR10<0:7>
    Vrs<8>   = CR07<2>
    Vrs<9>   = CR07<7>
    Vrs<10>  = CR25<2>
    Vre<0:3> = CR11<0:3>
***********************************************************/

// Position the screen on the display device (CRT or TV).
// DeviceMap: bit 0..7 = CRT0-7, bit 8..15 = TV 8-15
// Position:  bit 0..3 = position, bit 4 = default, bit 5 = last committed positon, bit 6 = commit
RM_STATUS dacSetDesktopPosition
(
    PHWINFO pDev, 
    U032    DeviceMap, 
    U032    Position
)
{

    // Add CRT positioning here

    // Position TV
    return dacPositionTV(pDev, 0, Position);
}
    
//  Position TV display in a device-independent way (via CRTC).
RM_STATUS dacPositionTV
(
    PHWINFO pDev, 
    U032    Head, 
    U032    Position
)
{
    RM_STATUS status = RM_OK;
    U008 lock;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    U032 VTotal, Vbs, Vt, Vrs = 0;
    U008 Skew, Cr01, Cr04, Cr05, Cr07, Cr21, Cr28;
    
#if 0
    // If no positioning info is given, then return whether this is the default position
    if (Position == 0) 
    {
        if (Vt == pDev->Dac.CrtcInfo[Head].MonitorPosition.Vt) // need to add Vt to MonitorPosition
            if (Hrs == pDev->Dac.CrtcInfo[Head].MonitorPosition.Hrs) 
                return TV_DEFAULT_POSITION;
        return TV_CUSTOM_POSITION;
    }
    if (Position & POS_DEFAULT) 
    {
        // Set default position
    }
    if (Position & POS_COMMIT) 
    {
        // Commit the current position to the registry
    }
    if (Position & POS_CANCEL) 
    {
        // Set the last committed position (saved in registry)
    }
#endif    
    
    // unlock the extended regs
    lock = UnlockCRTC(pDev, Head);
    // save the shadow lock
    CRTC_RD(0x21, Cr21, Head);
    CRTC_WR(0x21, Cr21 | 0x10, Head); // enable loading VT

    // The only adjustment we can do is the position, and that is done thru CRTC adjustment,
    // so this is actually generic TV positioning code. Should work on any encoder.

    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT) pDev->Dac.CrtcInfo[Head].pVidLutCurDac;
    VTotal = pVidLutCurDac->HalObject.Dac[0].TotalHeight;    
    Vbs = pVidLutCurDac->HalObject.Dac[0].VerticalBlankStart;
        
    // Horizontal position is controlled at pixel granularity by Cr28, 
    // and at character granularity by Cr4 (HRS)
    // Vertical position is controlled by Vertical Total.
    // Increasing the H retrace start value shortens the time between the sync and the start of display,
    // moving the screen to the left; decreasing retrace starts moves right.
    // Increasing vertical total increases time between sync and start of the display, moving the screen
    // down; decreasing VT moves the screen up.
    if (Position & POS_RIGHT) // right
    {
            CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, Cr28, Head);
            CRTC_RD(NV_CIO_CR_HRS_INDEX, Cr04, Head);
            CRTC_RD(NV_CIO_CR_HRE_INDEX, Cr01, Head);
            Skew = Cr28 & 0x38;
            if (Skew == 0x38) // at max?
            {
                if ((Cr04) > Cr01) // HRS must be > display end
                {
                    Cr04--;            // move right one char
                    Cr28 &= ~0x38;     // and set skew back to 0
                }
                else
                    status = RM_ERROR;
            }
            else
            {
                Skew >>= 3;
                Skew++;   // increment one pixel
                Skew <<=3;
            }
            CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, Cr28, Head);
            CRTC_WR(NV_CIO_CR_HRS_INDEX, Cr04, Head);
        
     }
     if (Position & POS_LEFT) // left
     {
            CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, Cr28, Head);
            CRTC_RD(NV_CIO_CR_HRS_INDEX, Cr04, Head);
            CRTC_RD(NV_CIO_CR_HRE_INDEX, Cr05, Head);
            Skew = Cr28 & 0x38;
            if (Skew == 0x00) 
            {
                if (((Cr04 + 1) & BITS0_4) < Cr05) // start must be < end
                {
                    Cr04++;            // move left one char
                    Cr28 |= 0x38;     // and set skew to max
                }
                else
                    status = RM_ERROR;
            }
            else
            {
                Skew >>= 3;
                Skew--;   // decrement one pixel
                Skew <<= 3;
            }
            CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, Cr28, Head);
            CRTC_WR(NV_CIO_CR_HRS_INDEX, Cr04, Head);
    }
    if (Position & POS_DOWN) // down
    {
            // get current VT
            CRTC_RD(NV_CIO_CR_VDT_INDEX, Vt, Head);
            CRTC_RD(NV_CIO_CR_OVL_INDEX, Cr07, Head);
            if (Cr07 & BIT0)
                Vt |= BIT8;
            if (Cr07 & BIT5)
                Vt |= BIT9;
            if (Vt < VTotal + 10) // put some limit on how far we increment
            {
                Vt++;
                Cr07 &= ~(BIT5 | BIT0);
                if (Vt & BIT8) Cr07 |= BIT0;
                if (Vt & BIT9) Cr07 |= BIT5;
                CRTC_WR(NV_CIO_CR_VDT_INDEX, Vt & 0xff, Head);
                CRTC_WR(NV_CIO_CR_OVL_INDEX, Cr07, Head);
            }
            else
                status = RM_ERROR;
    }
    if (Position & POS_UP)  // up
    {
            // get current VT
            CRTC_RD(NV_CIO_CR_VDT_INDEX, Vt, Head);
            CRTC_RD(NV_CIO_CR_OVL_INDEX, Cr07, Head);
            if (Cr07 & BIT0)
                Vt |= BIT8;
            if (Cr07 & BIT5)
                Vt |= BIT9;
            // get VRS too
            CRTC_RD(NV_CIO_CR_VRS_INDEX, Vrs, Head);
            if (Cr07 & BIT2) 
                Vrs |= BIT8;
            if (Cr07 & BIT7)
                Vrs |= BIT9;
            if (Vt > Vrs) // total must be > sync start
            {
                Vt--;
                Cr07 &= ~(BIT5 | BIT0);
                if (Vt & BIT8) Cr07 |= BIT0;
                if (Vt & BIT9) Cr07 |= BIT5;
                CRTC_WR(NV_CIO_CR_VDT_INDEX, Vt & 0xff, Head);
                CRTC_WR(NV_CIO_CR_OVL_INDEX, Cr07, Head);
            }
            else
                status = RM_ERROR;
    }
    CRTC_WR(0x21, Cr21, Head); // restore shadow locks
    RestoreLock(pDev, Head, lock);
    
    return status;
}

//  After every mode set, we save the H & V retrace in nvinfo, so we can restore the default screen position
VOID dacSaveMonitorDefaultPosition
(
    PHWINFO pDev,
    U032    Head
)
{
    PDACMONITORPOSITION pDacMonPos;
    U016    Hrs, Hre, Vrs, Vre;
    U016    Hbs, Hbe, Vbs, Vbe;
    U008    Cr03, Cr05, Cr07, Cr09, Cr25;
    short   temp;

// debug 
    CRTC_RD(NV_CIO_CR_HBE_INDEX, Cr03, Head);

    // read all the necessary config registers.
    CRTC_RD(NV_CIO_CR_HBE_INDEX, Cr03, Head);
    CRTC_RD(NV_CIO_CR_HRE_INDEX, Cr05, Head);
    CRTC_RD(NV_CIO_CR_OVL_INDEX, Cr07, Head);
    CRTC_RD(NV_CIO_CR_CELL_HT_INDEX, Cr09, Head);
    CRTC_RD(NV_CIO_CRE_LSR_INDEX, Cr25, Head);

    // Construct Hbs
    CRTC_RD(NV_CIO_CR_HBS_INDEX, Hbs, Head);
    // Construct Hbe
    Hbe = Cr03 & BITS0_4;
    if (Cr05 & BIT7)
        Hbe |= BIT5;
    if (Cr25 & BIT4)
        Hbe |= BIT6;
    // Construct Vbs
    CRTC_RD(NV_CIO_CR_VBS_INDEX, Vbs, Head);
    if (Cr07 & BIT3)
        Vbs |= BIT8;
    if (Cr09 & BIT5)
        Vbs |= BIT9;
    if (Cr25 & BIT3)
        Vbs |= BIT10;
    // Construct Vbe
    CRTC_RD(NV_CIO_CR_VBE_INDEX, Vbe, Head);

    // Construct Hrs
    CRTC_RD(NV_CIO_CR_HRS_INDEX, Hrs, Head);
    // Construct Hre
    Hre = Cr05 & BITS0_4;
    // Construct Vrs
    CRTC_RD(NV_CIO_CR_VRS_INDEX, Vrs, Head);
    if (Cr25 & BIT2)
        Vrs |= BIT10;
    if (Cr07 & BIT7)
        Vrs |= BIT9;
    if (Cr07 & BIT2)
        Vrs |= BIT8;
    // Construct Vre
    CRTC_RD(NV_CIO_CR_VRE_INDEX, Vre, Head);
    Vre &= BITS0_3;

    // The horizontal/vertical, blank/retrace end values are really the width of the respective signal.
    // Calculate the absolute end values. It is a bit cumbursome because the number of bits of information
    // is not uniform.
    // The formula is: <Absolute end value> = <start-value> + ((<width> - <start-value>) & (# of bits))
    temp = (short)Hbe - (short)Hbs; 
    temp &= 0x7f; // just take the low 7 bits
    Hbe = Hbs + temp;
    
    temp = (short)Hre - (short)Hrs; 
    temp &= 0x1f; // just take the lower 5 bits
    Hre = Hrs + temp;

    temp = (short)Vbe - (short)Vbs; 
    temp &= 0x7f; // just take the low 7 bits
    Vbe = Vbs + temp;
    
    temp = (short)Vre - (short)Vrs; 
    temp &= 0xf; // just take the lower 4 bits
    Vre = Vrs + temp;

    // save it off in nvinfo
    pDacMonPos = &pDev->Dac.CrtcInfo[Head].MonitorPosition;
    pDacMonPos->Hbs = Hbs;
    pDacMonPos->Hbe = Hbe;
    pDacMonPos->Vbs = Vbs;
    pDacMonPos->Vbe = Vbe;
    pDacMonPos->Hrs = Hrs;
    pDacMonPos->Hre = Hre;
    pDacMonPos->Vrs = Vrs;
    pDacMonPos->Vre = Vre;
}

//****************************************************************************************
/*  Adjust monitor horizontal position
    This is accomplished by increasing/decreasing the retrace start/end values
    The adjustment is character granularity
*/
static VOID SetRetraceH
(   PHWINFO pDev, 
    U032    Head,
    U032    Hrs, 
    U032    Hre
)
{
    U008    Cr05;

    // Get HRE
    CRTC_RD(NV_CIO_CR_HRE_INDEX, Cr05, Head);
    Cr05 &= ~BITS0_4;        // Clear bits 0:4
    Cr05 |= (Hre & BITS0_4); // Combine new HRE bits 0:4 with other bits

    // Write out adjustment
    CRTC_WR(NV_CIO_CR_HRS_INDEX, Hrs, Head);
    CRTC_WR(NV_CIO_CR_HRE_INDEX, Cr05, Head);
}

//****************************************************************************************
/*  Adjust monitor vertical position
    This is accomplished by increasing/decreasing the retrace start/end values
    The adjustment is scanline granularity
*/
static VOID SetRetraceV
(   PHWINFO pDev, 
    U032    Head,
    U032    Vrs, 
    U032    Vre
)
{
    U008    Cr10, Cr11, Cr07, Cr25;

    // Get CR25, CR7, CR11, mask VR bits
    CRTC_RD(NV_CIO_CR_OVL_INDEX, Cr07, Head);
    Cr07 &= ~(BIT7 | BIT2);
    CRTC_RD(NV_CIO_CRE_LSR_INDEX, Cr25, Head);
    Cr25 &= ~BIT2;
    CRTC_RD(NV_CIO_CR_VRE_INDEX, Cr11, Head);
    Cr11 &= ~BITS0_3;
        
    Cr10 = (U008)(Vrs & BITS0_7);
    // Combine new VRS value with other bits in CR07, CR25.
    if (Vrs & BIT10)
        Cr25 |= BIT2;
    if (Vrs & BIT9)
        Cr07 |= BIT7;
    if (Vrs & BIT8)
        Cr07 |= BIT2;

    // Combine VRE with other bits in CR11
    Cr11 |= (Vre & BITS0_3);
    
    // Write out adjustment
    CRTC_WR(NV_CIO_CR_VRS_INDEX, Cr10, Head);  
    CRTC_WR(NV_CIO_CR_OVL_INDEX, Cr07, Head);
    CRTC_WR(NV_CIO_CRE_LSR_INDEX, Cr25, Head);
    CRTC_WR(NV_CIO_CR_VRE_INDEX, Cr11, Head);
}

//****************************************************************************************
/*  Adjust monitor postion
    A CRT is positioned by moving the sync pulses relative to the display data.
*/
RM_STATUS dacSetMonitorPosition
(
    PHWINFO pDev,
    U032    Head,
    U032    Hrs,
    U032    Hre,
    U032    Vrs,
    U032    Vre
)
{
    U032      Vde, Vt;
    U008      Cr01, Cr07, Cr11, Cr21, Cr25, val;
    U008      lock;
    U008      UpdatedCr11;
    RM_STATUS rmStatus = RM_OK;

    // A CRT driven by a DAC is positioned by CRTC timing;
    // a CRT driven by FP timing is positioned by FP timing
    if (Head == 0) // TODO: don't assume Head B = FP; use DCB
    {
        // unlock the extended regs
        lock = UnlockCRTC(pDev, Head);

        CRTC_RD(NV_CIO_CR_VRE_INDEX, Cr11, Head);
        val = Cr11 & 0x7f;                          // clear write protection bit
        CRTC_WR(NV_CIO_CR_VRE_INDEX, val, Head);

        CRTC_RD(0x21, Cr21, Head);
        CRTC_WR(0x21, 0xf8, Head);       // disable shadowing for TV

        CRTC_RD(NV_CIO_CR_HDE_INDEX, Cr01, Head);     // read H display end

        // Check that the sync width doesn't go to 0 and that the sync pulse is beyond display end
        if ((Hre <= (Hrs & 0x1f)) || (Hrs <= Cr01))
        {
            rmStatus = RM_ERROR;
            goto done;
        }

        // Get overflow bits
        CRTC_RD(NV_CIO_CR_OVL_INDEX, Cr07, Head);
        CRTC_RD(NV_CIO_CRE_LSR_INDEX, Cr25, Head);    

        // Check that the sync width doesn't go to 0 and that the sync pulse is beyond display end
        CRTC_RD(NV_CIO_CR_VDE_INDEX, Vde, Head);    // read V display end (CR12)
        if (Cr07 & BIT(1)) Vde |= 0x100;
        if (Cr07 & BIT(6)) Vde |= 0x200;
        if (Cr25 & BIT(1)) Vde |= 0x400;

        // Check also that the sync pulse doesn't enter the display start (vertical total)
        CRTC_RD(NV_CIO_CR_VDT_INDEX, Vt, Head);
        if (Cr07 & BIT(0)) Vt |= 0x100;
        if (Cr07 & BIT(5)) Vt |= 0x200;    
        if (Cr25 & BIT(0)) Vt |= 0x400; 

        // Check for error conditions
        if ((Vre <= Vrs) || (Vrs < Vde) || (Vre > Vt))
        {
            rmStatus = RM_ERROR;
            goto done;
        }

        /*
        // Make sure we don't change these values during the vblank interval.
        while (REG_RD_DRF(_PCRTC, _RASTER, _VERT_BLANK) != NV_PCRTC_RASTER_VERT_BLANK_ACTIVE)
            ;
        while (REG_RD_DRF(_PCRTC, _RASTER, _VERT_BLANK) != NV_PCRTC_RASTER_VERT_BLANK_INACTIVE)
            ;
        */

        SetRetraceH(pDev, Head, Hrs, Hre);
        SetRetraceV(pDev, Head, Vrs, Vre);

        CRTC_WR(0x21, Cr21, Head);

        // the lower 4 bits in CR11 were updated by SetRetraceV(), don't mess it up
        CRTC_RD(NV_CIO_CR_VRE_INDEX, UpdatedCr11, Head);
        Cr11 = (Cr11 & ~BITS0_3) | (UpdatedCr11 & BITS0_3);
        CRTC_WR(NV_CIO_CR_VRE_INDEX, Cr11, Head);

     done:
        RestoreLock(pDev, Head, lock);
    }
    else // FP timing
    {
        PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
        // The default timing is in the dac object. Only the sync timings change.
        pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)pDev->Dac.CrtcInfo[Head].pVidLutCurDac;
        
        // Check that the sync pulse is between display end and total
        if ((Hrs <= pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth) || (Hre >= pVidLutCurDac->HalObject.Dac[0].TotalWidth))
        {
            rmStatus = RM_ERROR;
        }
        // 
        if ((Vrs <= pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight) || (Vre >= pVidLutCurDac->HalObject.Dac[0].TotalHeight))
        {
            rmStatus = RM_ERROR;
        }
        if (rmStatus == RM_OK) 
        {
            // The timings are reasonable, write them to HW        
            DAC_REG_WR32(NV_PRAMDAC_FP_HSYNC_START, Hrs, Head);
            DAC_REG_WR32(NV_PRAMDAC_FP_HSYNC_END, Hre, Head);
            DAC_REG_WR32(NV_PRAMDAC_FP_VSYNC_START, Vrs, Head);
            DAC_REG_WR32(NV_PRAMDAC_FP_VSYNC_END, Vre, Head);
        }
    }

    return rmStatus;
}
 
//  Get the H & V Blank and retrace values from nvinfo.
VOID dacGetMonitorDefaultPosition
(
    PHWINFO pDev,
    U032    Head,
    U032    *pHbs,
    U032    *pHbe,
    U032    *pVbs,
    U032    *pVbe,
    U032    *pHrs,
    U032    *pHre,
    U032    *pVrs,
    U032    *pVre
)
{
    PDACMONITORPOSITION pDacMonPos;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;


    if (Head == 0) // TODO: don't assume Head B = FP; use DCB
    {
        // We saved the timings when did the modeset (NOTE: they are also in the DAC object, 
        // so it is no longer necessary to have this structure)
        pDacMonPos = &pDev->Dac.CrtcInfo[Head].MonitorPosition;

        *pHbs = pDacMonPos->Hbs;
        *pHbe = pDacMonPos->Hbe;
        *pVbs = pDacMonPos->Vbs;
        *pVbe = pDacMonPos->Vbe;
        *pHrs = pDacMonPos->Hrs;
        *pHre = pDacMonPos->Hre;
        *pVrs = pDacMonPos->Vrs;
        *pVre = pDacMonPos->Vre;
    }
    else
    {
        // The timings are in the DAC object
        pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)pDev->Dac.CrtcInfo[Head].pVidLutCurDac;
        
        *pHbs = pVidLutCurDac->HalObject.Dac[0].HorizontalBlankStart;
        *pHbe = *pHbs + pVidLutCurDac->HalObject.Dac[0].HorizontalBlankWidth;
        *pVbs = pVidLutCurDac->HalObject.Dac[0].VerticalBlankStart;
        *pVbe = *pVbs + pVidLutCurDac->HalObject.Dac[0].VerticalBlankHeight;
        *pHrs = pVidLutCurDac->HalObject.Dac[0].HorizontalSyncStart;
        *pHre = *pHrs + pVidLutCurDac->HalObject.Dac[0].HorizontalSyncWidth;
        *pVrs = pVidLutCurDac->HalObject.Dac[0].VerticalSyncStart;
        *pVre = *pVrs + pVidLutCurDac->HalObject.Dac[0].VerticalSyncHeight;

    }
}

//  Get a string pointing to a key in the Display section
//  The string is of the type hresxyres, e.g. "1024x768"
//  Return 0 if its not a desktop mode
void dacFindRegistryKeyDisplayPosition
(
    PHWINFO pDev,
    U032    Head,
    BOOL    isMonitor,
    char    *strDisplayMode
)
{
    U032    hres, vres;
    U032    strIndex;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;

    // Get resolution
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT) pDev->Dac.CrtcInfo[Head].pVidLutCurDac;
    hres = pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth;
    vres = pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight;

    //
    // For non-TV modes, convert the resolution into a string
    //
    // Oh, to have a sscanf() handy...
    //
    strIndex = 0;   // start at the beginning

    //
    // Prepend the TV format if necessary
    //
    if (!isMonitor)
    {
        switch (pDev->Dac.TVStandard)
        {
            case NTSC_M:
            case NTSC_J:
                strDisplayMode = osStringCopy(strDisplayMode, "NTSC");
                strIndex = 4;
                break;
            case PAL_M: // unique case; does not use normal PAL timing
                strDisplayMode = osStringCopy(strDisplayMode, "PALM");
                strIndex = 4;
                break;
            default:    // rest are PAL modes
                strDisplayMode = osStringCopy(strDisplayMode, "PAL");
                strIndex = 3;
                break;     
        }
    }

    //
    // Assume the resolution is always below 9999x9999 and above 99x99
    // (this makes it a bit easier, especially with floating 0's)
    //
    // Start with the horizontal resolution first
    //
    if (hres / 1000)
    {
        strDisplayMode[strIndex++] = (U008)(hres/1000)+'0';
        hres -= (hres/1000)*1000;
    }

    if (hres / 100)
    {
        strDisplayMode[strIndex++] = (U008)(hres/100)+'0';
        hres -= (hres/100)*100;
    }
    else
        strDisplayMode[strIndex++] = '0';
    
    if (hres / 10)
    {
        strDisplayMode[strIndex++] = (U008)(hres/10)+'0';
        hres -= (hres/10)*10;
    }
    else
        strDisplayMode[strIndex++] = '0';
    
    strDisplayMode[strIndex++] = (U008)hres + '0';

    //
    // Append a lower case 'x'
    //
    strDisplayMode[strIndex++] = 'x';

    // 
    // Now generate the vertical value
    //
    if (vres / 1000)
    {
        strDisplayMode[strIndex++] = (U008)(vres/1000)+'0';
        vres -= (vres/1000)*1000;
    }
    
    if (vres / 100)
    {
        strDisplayMode[strIndex++] = (U008)(vres/100)+'0';
        vres -= (vres/100)*100;
    }
    else
        strDisplayMode[strIndex++] = '0';
    
    if (vres / 10)
    {
        strDisplayMode[strIndex++] = (U008)(vres/10)+'0';
        vres -= (vres/10)*10;
    }
    else
        strDisplayMode[strIndex++] = '0';
    
    strDisplayMode[strIndex++] = (U008)vres + '0';

    //
    // Zero append to the end
    //
    strDisplayMode[strIndex] = 0;

    return;

}

//  Save the screen position for the current mode in the registry
//  Find the registry key for this device and this mode
RM_STATUS dacWriteDesktopPositionToRegistry
(
    PHWINFO pDev,
    U032    Head,
    BOOL    isMonitor,
    U008 *  position, 
    U032    numBytes
)
{
    U032    i;
    char    strDisplayMode[] = "12345678901234567890";
    //
    // Generate the path to the registry key--last character is the display number
    //
    dacFindRegistryKeyDisplayPosition(pDev, Head, isMonitor, strDisplayMode);
    i = osStringLength(strDisplayMode);
    strDisplayMode[i++] = '_';
    strDisplayMode[i++] = ((U008)Head + '0');
    strDisplayMode[i] = 0;

    return osWriteRegistryBinary(pDev,pDev->Registry.DBstrDevNodeDisplayNumber, strDisplayMode, position, numBytes);
}

//  Read the screen position for the current mode from the registry
RM_STATUS dacReadDesktopPositionFromRegistry
(
    PHWINFO pDev,
    U032    Head,
    BOOL    isMonitor,
    U008 *  position, 
    U032 *  numBytes
)
{
    U032    i;
    char    strDisplayMode[] = "12345678901234567890";

    //
    // Generate the path to the registry key--last character is the display number
    //
    dacFindRegistryKeyDisplayPosition(pDev, Head, isMonitor, strDisplayMode);
    i = osStringLength(strDisplayMode);
    strDisplayMode[i++] = '_';
    strDisplayMode[i++] = ((U008)Head + '0');
    strDisplayMode[i] = 0;

    return osReadRegistryBinary(pDev,pDev->Registry.DBstrDevNodeDisplayNumber, strDisplayMode, position, numBytes);
}

//  Return the registry values for TV position, brightness, contrast and flicker filter.
RM_STATUS dacReadTVDesktopPositionFromRegistry
(
    PHWINFO pDev,
    U032 Head,
    U008 *Params,
    U032 *numBytes
)
{
    U032    i;
    char    strDisplayMode[] = "12345678901234567890";

    //
    // Generate the path to the registry key--last character is the display number
    //
    dacFindRegistryKeyDisplayPosition(pDev, Head, 0, strDisplayMode);
    i = osStringLength(strDisplayMode);
    strDisplayMode[i++] = '_';
    strDisplayMode[i++] = ((U008)Head + '0');
    strDisplayMode[i] = 0;

    return osReadRegistryBinary(pDev, pDev->Registry.DBstrDevNodeDisplayNumber, strDisplayMode, Params, numBytes);
}

//  Read the TV type (NTSC_, PAL_) from the registry
//  The registry key is DeviceX, X= 0,1,2,... under the Display key.
RM_STATUS dacReadTVStandardFromRegistry
(
    PHWINFO pDev,
	U032 *	type
)
{

    return OS_READ_REGISTRY_DWORD(pDev, pDev->Registry.DBstrDevNodeDisplayNumber, strTVtype, type);
}

//  Write the TV type (NTSC_, PAL_) from the registry
//  The registry key is DeviceX, X= 0,1,2,... under the Display key.
RM_STATUS dacWriteTVStandardToRegistry
(
    PHWINFO pDev,
	U032 	type
)
{

    return osWriteRegistryDword(pDev, pDev->Registry.DBstrDevNodeDisplayNumber, strTVtype, type);
}

//  Read the TV Out format (Composite on S-video, S-Video, Auto select)
//  The registry key is DeviceX, X= 0,1,2,... under the Display key.
RM_STATUS dacReadTVOutFromRegistry
(
    PHWINFO pDev,
    U032    Head,
    U032   *type
)
{
    return OS_READ_REGISTRY_DWORD(pDev, pDev->Registry.DBstrDevNodeDisplayNumber, strTVOutType, type);
}

//  Check Registry for Philips 7108B special setting
RM_STATUS dacReadTVPhilipsBFromRegistry
(
    PHWINFO pDev,
    U032    Head,
    U032   *type
)
{
char strTVPhilipsB[] = "Philips7108B";

    return OS_READ_REGISTRY_DWORD(pDev, pDev->Registry.DBstrDevNodeDisplayNumber, strTVPhilipsB, type);
}

//  Write the TV Out format (Composite on S-video, S-Video, Auto select)
//  The registry key is DeviceX, X= 0,1,2,... under the Display key.
RM_STATUS dacWriteTVOutToRegistry
(
    PHWINFO pDev,
    U032    Head,
    U032    type
)
{
    pDev->Dac.TVoutput = type;  // save in pDev, because NT cannot read registry at modeset time.
    return osWriteRegistryDword(pDev,pDev->Registry.DBstrDevNodeDisplayNumber, strTVOutType, type);
}

//  Look for registry entry telling us how to treat monitor syncs when monitor is not in use.
//  If we find "PowerSaverHsyncOn", we'll turn on Hsync & leave Vsync on when we want to blank the CRT
//  but driver DFP or TV on the same head. Otherwise, we'll turn off HSync, which is the legacy
//  method for communicating power mgmt requests to displays.
RM_STATUS dacReadMonitorSyncsFromRegistry
(
    PHWINFO pDev,
    U032   *type
)
{
    char strMonitorSyncs[] = "PowerSaverHsyncOn";

    return OS_READ_REGISTRY_DWORD(pDev, pDev->Registry.DBstrDevNodeDisplayNumber, strMonitorSyncs, type);
}


RM_STATUS dacGetMonitorInfo
(
    PHWINFO pDev,
    U032    Head,
    U032    *MonitorConnected
)
{
    *MonitorConnected = 0;

    if (dacMonitorConnectStatus(pDev, Head))
        *MonitorConnected = 1;
    return RM_OK;
}
// Write a CRTC timing parameter into the CRTC registers
VOID dacSetCRTC
(
    PHWINFO pDev, 
    U032 Head, 
    U032 Param, 
    U032 Val
)
{
    U008 Crtc;
    U032 Val2;

    switch (Param) 
    {
    case H_TOTAL:
        Val2 = (Val >> 3) - 5;
        Crtc = (U008)(Val2 & 0xff);
        CRTC_WR(0, Crtc, Head);
        CRTC_RD(0x2D, Crtc, Head);
        Crtc &= ~BIT(0);
        if (Val2 & 0x100)               
            Crtc |= BIT(0);
        CRTC_WR(0x2D, Crtc, Head);
        break;
    case H_DISPLAY_END:
        Crtc = (U008) ((Val >> 3) - 1);
        CRTC_WR(1, Crtc, Head);
        break;
    case H_BLANK_START:
        Val2 = (Val >> 3) - 1;
        Crtc = (U008)(Val2 & 0xff);
        CRTC_WR(2, Crtc, Head);
        CRTC_RD(0x2D, Crtc, Head);
        Crtc &= ~BIT(2);
        if (Val2 & 0x100)               
            Crtc |= BIT(2);
        CRTC_WR(0x2D, Crtc, Head);
        break;
    case H_BLANK_END:
        Val2 = (Val >> 3) - 1;
        CRTC_RD(3, Crtc, Head);     // HBE bits 4:0
        Crtc &= 0xe0;
        Crtc |= (U008)(Val2 & 0x1f);
        CRTC_WR(3, Crtc, Head);
        CRTC_RD(5, Crtc, Head);
        Crtc &= ~BIT(7);
        if (Val2 & 0x20)               // HBE bit 5
            Crtc |= BIT(7);
        CRTC_WR(5, Crtc, Head);
        CRTC_RD(0x25, Crtc, Head);
        Crtc &= ~BIT(4);
        if (Val2 & 0x40)
            Crtc |= BIT(4);             // HBE bit 6
        CRTC_WR(0x25, Crtc, Head);
        break;
    case H_RETRACE_START:
        Val2 = (Val >> 3) + 1;
        Crtc = (U008) (Val2 & 0xFF);
        CRTC_WR(4, Crtc, Head);
        CRTC_RD(0x2D, Crtc, Head);
        Crtc &= ~BIT(3);
        if (Val2 & 0x100)
            Crtc |= BIT(3);
        CRTC_WR(0x2D, Crtc, Head);
        break;
    case H_RETRACE_END:
        Val2 = (Val >> 3) + 1;
        CRTC_RD(5, Crtc, Head);     // HRE bits 4:0
        Crtc &= 0xe0;
        Crtc |= (U008)(Val2 & 0x1f);
        CRTC_WR(5, Crtc, Head);
        break;
    case V_TOTAL:
        Val2 = Val - 2;
        Crtc = (U008) Val2;
        CRTC_WR(6, Crtc, Head);
        CRTC_RD(7, Crtc, Head);
        Crtc &= ~(0x21);
        if (Val2 & 0x100) Crtc |= BIT(0);
        if (Val2 & 0x200) Crtc |= BIT(5);
        CRTC_WR(7, Crtc, Head);
        CRTC_RD(0x25, Crtc, Head);
        Crtc &= ~BIT(0);
        if (Val2 & 0x400) Crtc |= BIT(0);
        CRTC_WR(0x25, Crtc, Head);
        CRTC_RD(0x41, Crtc, Head);
        Crtc &= ~BIT(0);
        if (Val2 & 0x800) Crtc |= BIT(0);
        CRTC_WR(0x41, Crtc, Head);
        break;
    case V_DISPLAY_END:
        Val2 = Val - 1;
        Crtc = (U008) Val2;
        CRTC_WR(0x12, Crtc, Head);    // VDE 7:0
        CRTC_RD(7, Crtc, Head);
        Crtc &= ~(0x42);
        if (Val2 & 0x100) Crtc |= BIT(1);  // VDE 8
        if (Val2 & 0x200) Crtc |= BIT(6);  // VDE 9
        CRTC_WR(7, Crtc, Head);
        CRTC_RD(0x25, Crtc, Head);
        Crtc &= ~BIT(1);
        if (Val2 & 0x400) Crtc |= BIT(1);  // VDE 10
        CRTC_WR(0x25, Crtc, Head);
        break;
    case V_RETRACE_START:
        Val2 = Val - 1;
        Crtc = (U008) Val2;
        CRTC_WR(0x10, Crtc, Head);
        CRTC_RD(7, Crtc, Head);
        Crtc &= ~(BIT(2) | BIT(7));
        if (Val2 & 0x100) Crtc |= BIT(2);  // VRS 8
        if (Val2 & 0x200) Crtc |= BIT(7);  // VRS 9
        CRTC_WR(7, Crtc, Head);
        CRTC_RD(0x25, Crtc, Head);
        Crtc &= ~BIT(2);
        if (Val2 & 0x400) Crtc |= BIT(2);  // VRS 10
        CRTC_WR(0x25, Crtc, Head);
        CRTC_RD(0x41, Crtc, Head);
        Crtc &= ~BIT(4);
        if (Val2 & 0x800) Crtc |= BIT(4);  // VRS 11
        CRTC_WR(0x41, Crtc, Head);
        break;
    case V_RETRACE_END:
        Val2 = Val - 1;
        CRTC_RD(0x11, Crtc, Head);
        Crtc &= 0xF0;
        Crtc |= (U008) (Val2 & 0x0F);
        CRTC_WR(0x11, Crtc, Head);
        break;
    case V_BLANK_START:
        Val2 = Val - 1;
        Crtc = (U008) Val2;
        CRTC_WR(0x15, Crtc, Head);
        CRTC_RD(7, Crtc, Head);
        Crtc &= ~BIT(3);
        if (Val2 & 0x100) Crtc |= BIT(3);  // VBS 8
        CRTC_WR(7, Crtc, Head);
        CRTC_RD(9, Crtc, Head);
        Crtc &= ~BIT(5);
        if (Val2 & 0x200) Crtc |= BIT(5);  // VBS 9
        CRTC_WR(9, Crtc, Head);
        CRTC_RD(0x25, Crtc, Head);
        Crtc &= ~BIT(3);
        if (Val2 & 0x400) Crtc |= BIT(3);  // VBS 10
        CRTC_WR(0x25, Crtc, Head);
        CRTC_RD(0x41, Crtc, Head);
        Crtc &= ~BIT(6);
        if (Val2 & 0x800) Crtc |= BIT(6);  // VBS 11
        CRTC_WR(0x41, Crtc, Head);
        break;
    case V_BLANK_END:
        Crtc = (U008) Val - 1;
        CRTC_WR(0x16, Crtc, Head);
        break;
    }
}

// Write a CRTC timing parameter into the CRTC registers
U032 dacGetCRTC
(
    PHWINFO pDev, 
    U032 Head, 
    U032 Param
)
{
    U008 Crtc;
    U032 Val=0, Val2;

    switch (Param) 
    {
    case H_TOTAL:
        CRTC_RD(0, Crtc, Head);
        Val = (U032)Crtc;
        CRTC_RD(0x2D, Crtc, Head);
        if (Crtc & BIT(0)) Val |= 0x100;
        Val = (Val + 5) * 8;
        break;
    case H_DISPLAY_END:
        CRTC_RD(1, Crtc, Head);
        Val = (U032)(Crtc + 1) * 8;
        break;
    case H_BLANK_START:
        CRTC_RD(2, Crtc, Head);
        Val = (U032)Crtc;
        CRTC_RD(0x2D, Crtc, Head);
        if (Crtc & BIT(2)) Val |= 0x100;
        Val = (Val + 1) * 8;
        break;
    case H_BLANK_END:
        CRTC_RD(3, Crtc, Head);     // HBE bits 4:0
        Val = (U032)(Crtc & 0x1f);
        CRTC_RD(5, Crtc, Head);
        if (Crtc & 0x80) Val |= 0x20;
        CRTC_RD(0x25, Crtc, Head);
        if (Crtc & BIT(4)) Val |= 0x40;
        Val++;
        Val2 = dacGetCRTC(pDev, Head, H_BLANK_START);
        Val2 >>= 3;                     // convert HRS back to real chars
        Val2++;
        if (Val >= (Val2 & 0x7f))       // if end > start, just add upper bits of start 
        {
            Val |= (Val2 & 0x180);
        }
        else
        {
            Val |= ((Val2 + 0x80) & 0x180);    // add Start + carry
        }
        Val *= 8;
        break;
    case H_RETRACE_START:
        CRTC_RD(4, Crtc, Head);
        Val = (U032)Crtc;
        CRTC_RD(0x2D, Crtc, Head);
        if (Crtc & BIT(3)) Val |= 0x100;
        Val--;
        Val *= 8;
        break;
    case H_RETRACE_END:
        CRTC_RD(5, Crtc, Head);     // HRE bits 4:0
        Val = (U032)(Crtc & 0x1f);
        Val--;
        Val2 = dacGetCRTC(pDev, Head, H_RETRACE_START);
        Val2 >>= 3;                     // convert HRS back to real chars
        Val2--;
        if (Val >= (Val2 & 0x1f))       // if end > start, just add upper bits of start 
        {
            Val |= (Val2 & 0x1E0);
        }
        else
        {
            Val |= ((Val2 + 0x20) & 0x1E0);    // add Start + carry
        }
        Val *= 8;
        break;
    case V_TOTAL:
        CRTC_RD(6, Crtc, Head);
        Val = (U032)Crtc;
        CRTC_RD(7, Crtc, Head);
        if (Crtc & BIT(0)) Val |= 0x100;
        if (Crtc & BIT(5)) Val |= 0x200;
        CRTC_RD(0x25, Crtc, Head);
        if (Crtc & BIT(0)) Val |= 0x400;
        CRTC_RD(0x41, Crtc, Head);
        if (Crtc & BIT(0)) Val |= 0x800;
        Val += 2;
        break;
    case V_DISPLAY_END:
        CRTC_RD(0x12, Crtc, Head);    // VDE 7:0
        Val = (U032)Crtc;
        CRTC_RD(7, Crtc, Head);
        if (Crtc & BIT(1)) Val |= 0x100;  // VDE 8
        if (Crtc & BIT(6)) Val |= 0x200;  // VDE 9
        CRTC_RD(0x25, Crtc, Head);
        if (Crtc & BIT(1)) Val |= 0x400;  // VDE 10
        Val++;
        break;
    case V_RETRACE_START:
        CRTC_RD(0x10, Crtc, Head);
        Val = (U032)Crtc;
        CRTC_RD(7, Crtc, Head);
        if (Crtc & BIT(2)) Val |= 0x100;  // VRS 8
        if (Crtc & BIT(7)) Val |= 0x200;  // VRS 9
        CRTC_RD(0x25, Crtc, Head);
        if (Crtc & BIT(2)) Val |= 0x400;  // VRS 10
        CRTC_RD(0x41, Crtc, Head);
        if (Crtc & BIT(4)) Val |= 0x800;  // VRS 11
        Val++;
        break;
    case V_RETRACE_END:
        CRTC_RD(0x11, Crtc, Head);
        Val = (U032)(Crtc & 0x0f);
        Val2 = dacGetCRTC(pDev, Head, V_RETRACE_START);
        if (Val >= (Val2 & 0x0f))       // if end > start, just add upper bits of start 
        {
            Val |= (Val2 & 0xFF0);
        }
        else
        {
            Val |= ((Val2 + 0x10) & 0xFF0);    // add Start + carry
        }
        Val++;
        break;
    case V_BLANK_START:
        CRTC_RD(0x15, Crtc, Head);
        Val = (U032)Crtc;
        CRTC_RD(7, Crtc, Head);
        if (Crtc & BIT(3)) Val |= 0x100;  // VBS 8
        CRTC_RD(9, Crtc, Head);
        if (Crtc & BIT(5)) Val |= 0x200;  // VBS 9
        CRTC_RD(0x25, Crtc, Head);
        if (Crtc & BIT(3)) Val |= 0x400;  // VBS 10
        CRTC_RD(0x41, Crtc, Head);
        if (Crtc & BIT(6)) Val |= 0x800;  // VBS 11
        Val++;
        break;
    case V_BLANK_END:
        CRTC_RD(0x16, Crtc, Head);
        Val = (U032)Crtc + 1;
        Val2 = dacGetCRTC(pDev, Head, V_RETRACE_START); // End = Start + width of sync
        if (Val >= (Val2 & 0xff))    // if low byte of end > low byte of start 
        {
            Val |= (Val2 & 0xf00);  // combine with upper bits of V_BLANK_START
        }
        else
        {
            Val |= ((Val2 + 0x100) & 0xf00);    // combine with upper bits of V_BLANK_START plus carry
        }
        break;
    }
    return Val;
}
//  Get CRTC timing values from either CRTC registers or Windows registry
RM_STATUS dacGetCRTCTiming
(
    PHWINFO pDev,
    U032    Head,
    NV_CFGEX_CRTC_TIMING_PARAMS    *Params
)
{
    U008    lock;

    if (Params->Reg & NV_CFGEX_CRTC_TIMING_REGISTER) // Get currect CRTC values
    {
        // unlock the extended regs
        lock = UnlockCRTC(pDev, Head);
        
        Params->HTotal = dacGetCRTC(pDev, Head, H_TOTAL);    
        Params->HDisplayEnd = dacGetCRTC(pDev, Head, H_DISPLAY_END);    
        Params->HBlankStart = dacGetCRTC(pDev, Head, H_BLANK_START);    
        Params->HBlankEnd = dacGetCRTC(pDev, Head, H_BLANK_END);    
        Params->HRetraceStart = dacGetCRTC(pDev, Head, H_RETRACE_START);    
        Params->HRetraceEnd = dacGetCRTC(pDev, Head, H_RETRACE_END);    
        Params->VTotal = dacGetCRTC(pDev, Head, V_TOTAL);    
        Params->VDisplayEnd = dacGetCRTC(pDev, Head, V_DISPLAY_END);    
        Params->VRetraceStart = dacGetCRTC(pDev, Head, V_RETRACE_START);    
        Params->VRetraceEnd = dacGetCRTC(pDev, Head, V_RETRACE_END);    
        Params->VBlankStart = dacGetCRTC(pDev, Head, V_BLANK_START);    
        Params->VBlankEnd = dacGetCRTC(pDev, Head, V_BLANK_END);    
        
        RestoreLock(pDev, Head, lock);
        return RM_OK;
    }
    else if (Params->Reg & NV_CFGEX_CRTC_TIMING_REGISTRY)   // Get values from registry
    {

        char    *strptr;
        U032    numBytes;
        char    strDisplayMode[] = "12345678901234567890";

        // fill in the path to the registry key--last character is the display number
        //JJV-New registry Process size = osStringLength(strDevNodeDisplayNumber);
        //JJV-New registry Process strDevNodeDisplayNumber[size - 1] = ('0' | (U008)devInstance);    // fill in ascii device #
        // now get the specific entry we need
        dacFindRegistryKeyDisplayPosition(pDev, Head, TRUE, strDisplayMode);
        // Copy the string and append the head number
        
        for (strptr = strDisplayMode; *strptr != 0; strptr++) 
            ;
        *strptr++ = '_';
        *strptr++ = (char)(Head | 0x30);
        *strptr++ = 'T';                    // make sure to use different key than desktop position
        *strptr   = 0;
        return osReadRegistryBinary(pDev,pDev->Registry.DBstrDevNodeDisplayNumber, strDisplayMode, (U008 *)&Params->HTotal, &numBytes);
    }
    else // return default values for current mode
    {

        PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;

        //
        // Grab the current dac settings (last modeset to come through on this head)
        //
        pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)pDev->Dac.CrtcInfo[Head].pVidLutCurDac;


        Params->HTotal = pVidLutCurDac->HalObject.Dac[0].TotalWidth;    
        Params->HDisplayEnd = pVidLutCurDac->HalObject.Dac[0].VisibleImageWidth;
        Params->HBlankStart = pVidLutCurDac->HalObject.Dac[0].HorizontalBlankStart;
        Params->HBlankEnd = (pVidLutCurDac->HalObject.Dac[0].HorizontalBlankStart +
                                            pVidLutCurDac->HalObject.Dac[0].HorizontalBlankWidth);
        Params->HRetraceStart = pVidLutCurDac->HalObject.Dac[0].HorizontalSyncStart;
        Params->HRetraceEnd = (pVidLutCurDac->HalObject.Dac[0].HorizontalSyncStart +
                                            pVidLutCurDac->HalObject.Dac[0].HorizontalSyncWidth);
        Params->VTotal = pVidLutCurDac->HalObject.Dac[0].TotalHeight;
        Params->VDisplayEnd = pVidLutCurDac->HalObject.Dac[0].VisibleImageHeight;
        Params->VRetraceStart = pVidLutCurDac->HalObject.Dac[0].VerticalSyncStart;
        Params->VRetraceEnd = (pVidLutCurDac->HalObject.Dac[0].VerticalSyncStart +
                                           pVidLutCurDac->HalObject.Dac[0].VerticalSyncHeight);
        Params->VBlankStart = pVidLutCurDac->HalObject.Dac[0].VerticalBlankStart;
        Params->VBlankEnd = (pVidLutCurDac->HalObject.Dac[0].VerticalBlankStart +
                                            pVidLutCurDac->HalObject.Dac[0].VerticalBlankHeight);

        //
        // There is a bug in NV hardware where the hardware cursor will not clip correctly
        // if a border is defined in the display timings (blank start != visible end).
        //
        // Borders are only defined in two specific DMT modes, so generally this is
        // not a problem.
        //
        // To fix this, set the blank starts to match the visible totals and remove the 
        // implicit borders.
        //
        // Remove this code when the hardware gets fixed.  Since this bug has been in the 
        // hardware since 1994, don't expect a fix soon.
        //
        //HorizontalBlankStart = HorizontalVisible;
        //HorizontalBlankEnd   = HorizontalTotal;
        //VerticalBlankStart   = VerticalVisible;
        //VerticalBlankEnd     = VerticalTotal;
        //
        // We want to return "default" values that match what the hardware really wants
        // us to program, not the true borders that exist in the DMT.  Otherwise a
        // client that uses this call to get hardware defaults, and then turns around
        // and programs the hardware with them, will get incorrect timings.
        //
        Params->HBlankStart = Params->HDisplayEnd;
        Params->HBlankEnd = Params->HTotal;
        Params->VBlankStart = Params->VDisplayEnd;
        Params->VBlankEnd = Params->VTotal;

        return RM_OK;
    }
}

//  Write caller's CRTC timing values into CRTC register and/or Windows Registry
RM_STATUS dacSetCRTCTiming
(
    PHWINFO pDev,
    U032    Head,
    NV_CFGEX_CRTC_TIMING_PARAMS    *Params
)
{

    U008    lock, data08;

    if (Params->Reg & NV_CFGEX_CRTC_TIMING_REGISTER) // Set CRTC values
    {
        // unlock the extended regs
        lock = UnlockCRTC(pDev, Head);

        CRTC_RD(NV_CIO_CR_VRE_INDEX, data08, Head);
        CRTC_WR(NV_CIO_CR_VRE_INDEX, data08 & ~0x80, Head);    // unlock CR0-7


        // Write the CRTC's        
        dacSetCRTC(pDev, Head, H_TOTAL, Params->HTotal);
        dacSetCRTC(pDev, Head, H_DISPLAY_END, Params->HDisplayEnd);
        dacSetCRTC(pDev, Head, H_BLANK_START, Params->HBlankStart);
        dacSetCRTC(pDev, Head, H_BLANK_END, Params->HBlankEnd);       
        dacSetCRTC(pDev, Head, H_RETRACE_START, Params->HRetraceStart);
        dacSetCRTC(pDev, Head, H_RETRACE_END, Params->HRetraceEnd);     
        dacSetCRTC(pDev, Head, V_TOTAL, Params->VTotal);
        dacSetCRTC(pDev, Head, V_DISPLAY_END, Params->VDisplayEnd);
        dacSetCRTC(pDev, Head, V_BLANK_START, Params->VBlankStart);
        dacSetCRTC(pDev, Head, V_BLANK_END, Params->VBlankEnd);
        dacSetCRTC(pDev, Head, V_RETRACE_START, Params->VRetraceStart);
        dacSetCRTC(pDev, Head, V_RETRACE_END, Params->VRetraceEnd);

        //
        // Lock everything back up again
        //
        CRTC_RD(NV_CIO_CR_VRE_INDEX, data08, Head);
        CRTC_WR(NV_CIO_CR_VRE_INDEX, data08 | 0x80, Head);    // lock CR0-7
        RestoreLock(pDev, Head, lock);
    }
    if (Params->Reg & NV_CFGEX_CRTC_TIMING_REGISTRY) // Set Register values
    {
        char    *strptr;
        U032    numBytes = (NV_CFGEX_CRTC_PARAMS * 4) - 8;  // HTotal is 8 bytes in
        char    strDisplayMode[] = "12345678901234567890";

        // fill in the path to the registry key--last character is the display number
        //JJV-New registry Process size = osStringLength(strDevNodeDisplayNumber);
        //JJV-New registry Process strDevNodeDisplayNumber[size - 1] = ('0' | (U008)devInstance);    // fill in ascii device #
        // now get the specific entry we need
        dacFindRegistryKeyDisplayPosition(pDev, Head, TRUE, strDisplayMode);
        // append the head number
        for (strptr = strDisplayMode; *strptr != 0; strptr++) 
            ;
        *strptr++ = '_';
        *strptr++ = (char)(Head | 0x30);
        *strptr++ = 'T';                    // make sure to use different key than desktop position
        *strptr   = 0;
        return osWriteRegistryBinary(pDev,pDev->Registry.DBstrDevNodeDisplayNumber, strDisplayMode, (U008 *)&Params->HTotal, numBytes);
    }
    return RM_OK;
}

//	See what is currently being used as a display
U032 dacGetBiosDisplayType
(
    PHWINFO pDev,
    U032    Head
)
{
    U008  	rc;
    U032 	displayType;
    U008	lock;               

    // unlock the extended regs
    lock = UnlockCRTC(pDev, Head);

    CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, rc, Head);
    if (rc & 0x80)    // is VGA slaved to something?
    {
        CRTC_RD(NV_CIO_CRE_LCD__INDEX, rc, Head);
        if (rc & 0x01)
            displayType = DISPLAY_TYPE_FLAT_PANEL;   
        else
            displayType = DISPLAY_TYPE_TV;      
    }
    else
    {
        displayType = DISPLAY_TYPE_MONITOR;
    }

    RestoreLock(pDev, Head, lock);

    return displayType;
}

//
// Determine which is the boot head by checking which head has an active
// display set (this mechanism breaks with simul-mode). This should only
// be called from initDac.
//
U032 dacGetBiosDisplayHead
(
    PHWINFO pDev
)
{
    U008 lock, cr4b_A, cr3b_A, cr3b_B;

    // headA for non-NV11
    if (IsNV11(pDev) == FALSE)
        return 0;

    lock = UnlockCRTC(pDev, 0);
    CRTC_RD(NV_CIO_CRE_SCRATCH6__INDEX, cr4b_A, 0);
    RestoreLock(pDev, 0, lock);

    // headA if this is not a Mobile BIOS
    if ((cr4b_A & NV_IS_MOBILE_BIOS) == 0) 
        return 0;

    // read out the current display (7:4 of CR3B) for both heads
    lock = UnlockCRTC(pDev, 0);
    CRTC_RD(NV_CIO_CRE_SCRATCH3__INDEX, cr3b_A, 0);
    RestoreLock(pDev, 0, lock);

    lock = UnlockCRTC(pDev, 1);
    CRTC_RD(NV_CIO_CRE_SCRATCH3__INDEX, cr3b_B, 1);
    RestoreLock(pDev, 1, lock);

    // error check if both heads have non-zero current displays
    if ((cr3b_A & 0xF0) && (cr3b_B & 0xF0))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "dacGetBiosDisplayHead - both heads have current displays\n");
        return 0;
    }

    if (cr3b_B & 0xF0)
        return 1;
    else
        return 0;
}

//
// This routine is ugly!
// It is used to clear the bits in the bios scratch register CR3B on the non-primary
// head when more than one head is active.
// One instance where this is needed is on portables when going into hibernation
// under win2k.  Normally, the monitors are supposed to be left on and a hibernation
// progress screen is displayed in vga mode.  But since broadcast mode is broken,
// if we don't clear the bits in the scratch register on the non-primary head, then
// one screen will display the hibernation progress screen and the other will display
// garbage.
// This routine will return TRUE if the other head is active.
//
BOOL dacDisableBroadcast
(
    PHWINFO pDev,
    U032    headSecondary,
    U008   *reg3B
)
{
    BOOL otherHeadActive;
    U008 lock[2];
    U008 cr3B[2];
    U032 headPrimary;

    //
    // Only need to execute on NV11's that don't support broadcast.
    //
    otherHeadActive = FALSE;
    if (IsNV11(pDev))
    {
        headPrimary = 1 - headSecondary;
        if (pDev->Power.MobileOperation)
        {
            //
            // MOBILE
            //
            // Unlock the registers.
            //
            lock[0] = UnlockCRTC(pDev, 0);
            lock[1] = UnlockCRTC(pDev, 1);
            //
            // Get current register values.
            //
            CRTC_RD(0x3B, cr3B[0], 0);
            CRTC_RD(0x3B, cr3B[1], 1);
            //
            // Return the original value of reg cr3B for the head being disabled.
            //
            if (reg3B)
                *reg3B = cr3B[headSecondary];
            //
            // Check if the other head is active.
            //
            if (cr3B[headPrimary] & 0xF0)
            {
                //
                // Mark the specified head as disabled.
                //
                cr3B[headSecondary] &= 0x0F;
                CRTC_WR(0x3B, cr3B[headSecondary], headSecondary);
                otherHeadActive = TRUE;
                //
                // Enable the primary head so any subsequent vga modesets will be be done on the
                // correct head by the bios.
                //
                EnableHead(pDev, headPrimary);
            }
            //
            // Restore the register locks.
            //
            RestoreLock(pDev, 0, lock[0]);
            RestoreLock(pDev, 1, lock[1]);
        }
        else
        {
            //
            // DESKTOP
            //
            // If the display on head 1 is on, turn it off to avoid
            // seeing garbage on it while in vga mode.
            if (pDev->Power.DisplayState[1] != MC_POWER_LEVEL_6)
            {
                //
                // Use the power management routines to turn the display on head 1 off.
                //
                mcPowerDisplayState(pDev, MC_POWER_LEVEL_6, 1);
                //
                // not really in MC_POWER_LEVEL_6 power management state, so leave
                // the power level state set to MC_POWER_LEVEL_5.
                //
                pDev->Power.DisplayState[1] = MC_POWER_LEVEL_5;
            }
            //
            // Whatever is the the active display on head a is the display that we
            // want to enable.  That's what the bios expects to be programming when
            // doing a modeset.
            // One negative side effect of this is that in clone mode with the DFP as the
            // primary display, opening a full screen dos box will not display on the DFP.
            // it will display on whatever display is on head a.
            //
            EnableHead(pDev, 0);
        }
    }
    return (otherHeadActive);
}
BOOL dacRestoreBroadcast
(
    PHWINFO pDev,
    U032    head,
    U008    reg3B
)
{
    BOOL regRestored = FALSE;
    U008 lock;

    if (IsNV11(pDev) && pDev->Power.MobileOperation)
    {
        lock = UnlockCRTC(pDev, head);
        CRTC_WR(0x3B, reg3B, head);
        RestoreLock(pDev, head, lock);
        regRestored = TRUE;
    }
    return (regRestored);
}

//  Read analog out comparator 
//  If the output is loaded, it will be below threshold
//  If no CRT is attached, it will trip
U032 dacDetectTrigger(
    PHWINFO pDev
)
{
    U008 blank, data8;
    U032 retry;
    U032 LastFunc;

//define maxretries in 100us increments
#define MAXRETRIES 334
    
    // wait for active
    retry = 0;
    LastFunc=0;
    do {
        blank = REG_RD08(NV_PRMCIO_INP0__COLOR) & 0x1;
        retry++; 
        tmrDelay(pDev,100000); //delay 100us
    } while((retry<MAXRETRIES)&&(blank));
    if (retry >= MAXRETRIES)
        goto timedout;

    
    // wait for blank
    retry = 0;
    LastFunc=1;
    do {
        blank = REG_RD08(NV_PRMCIO_INP0__COLOR) & 0x1;
        retry++;
        tmrDelay(pDev,100000); //delay 100us
    } while ((retry < MAXRETRIES) && !blank);
    if (retry >= MAXRETRIES)
        goto timedout;

    // wait for active
    retry = 0;
    LastFunc=2;
    do {
        blank = REG_RD08(NV_PRMCIO_INP0__COLOR) & 0x1;
        retry++;
        tmrDelay(pDev,100000); //delay 100us
    } while ((retry < MAXRETRIES) && blank);
    if (retry >= MAXRETRIES)
        goto timedout;

    // get 2 samples which are the same to debounce
    retry = 0;
    LastFunc=3;
    do {
        data8 = (REG_RD08(NV_PRMCIO_INP0) & 0x10);
        retry++;
        tmrDelay(pDev,100000); //delay 100us
    } while ((retry < MAXRETRIES) && ((REG_RD08(NV_PRMCIO_INP0) & 0x10) != data8));
    if (retry >= MAXRETRIES)
    {
 timedout:
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: trigger detect timed out: ",LastFunc);
        return 2;
    }

    // sense bit in NV_PRMCIO_INP0 is inverted (0x1 = no trigger).
    if (data8)
    {
        // DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"NVRM: DacDetectTrigger: False\n");
        return 0;    // no trigger
    }
    else
    {
        // DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"NVRM: DacDetectTrigger: True\n");
        return 1;     // trigger
    }
}



// Check whether monitor is currently connected
BOOL dacMonitorConnectStatus
(
    PHWINFO pDev,
    U032    Head
)
{
    RM_STATUS status = RM_OK;
    U008      lock;
    U032      Current;
    U032      data;
    U032      force_flag = 0;

    if (using_new_api == FALSE) {
        //
        // Make sure monitor is supported on this head.
        //
        if ((GETCRTCHALINFO(pDev, Head, DisplayTypesAllowed) & DISPLAYTYPEBIT(DISPLAY_TYPE_MONITOR)) == 0)
            return FALSE;

        //
        // For NV11 TwinView, make sure head isn't in use by another display device
        //
        if (!pDev->Power.MobileOperation)
            if (IsNV11(pDev))
            {
                if (GETDISPLAYTYPE(pDev, Head ^ 1) != 0xFFFFFFFF) // if both heads in use
                {
                    Current = GETDISPLAYTYPE(pDev, Head);
                    if (Current == DISPLAY_TYPE_FLAT_PANEL) // then only the current type is allowed (except TV)
                        return FALSE;
                }
            }

        // Mobile devices should never have CRT on Head 1
        if (pDev->Power.MobileOperation && (Head == 1)) 
            return FALSE;
    }

//#ifdef USE_EDID_TO_TEST_CRT_CONNECT
    if ((IsNV11(pDev)) && (Head == 1))
    {
        U032 prevHead = (pDev->Dac.HalInfo.Head2RegOwner & 0x1);
        PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
        U032 M,N,P;
        U032 CoeffSelect = 0;
        U032 Vpll2 = 0;
        U008 Cr28  = 0;
        U008 Cr33  = 0;
        // Attempt to read EDID and check for analog display. On head 1 there is no way to do the analog out test below.

        if (IsNV11(pDev))
           EnableHead(pDev, Head);

        // unlock the extended regs
        lock = UnlockCRTC(pDev, Head);

        pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT) pDev->Dac.CrtcInfo[Head].pVidLutCurDac;
        
        // There are monitors which do not perform DDC unless they see valid sync timing. If we are on the DVI connector
        // (and on Head 1 we are), we must toggle syncs long enough to wake the monitor up so we can do DDC.
        if (pVidLutCurDac == NULL) // Currently in use?
        {
            if (pDev->Dac.HalInfo.InitialBootHead != 1) // And if not the boot device (this may be called before VidLutCurDac is created)
            {
                VIDEO_LUT_CURSOR_DAC_OBJECT VidLutCurDac;

                // Write 800x600 timing in the FP registers.
                DAC_REG_WR32(NV_PRAMDAC_FP_HCRTC, 800, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_HDISPLAY_END, 799, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_HVALID_START, 800, Head);    // START = END + 1 so no data goes out
                DAC_REG_WR32(NV_PRAMDAC_FP_HVALID_END, 799, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_HSYNC_START, 799+16, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_HSYNC_END, 839, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_HTOTAL, 1120, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_VCRTC, 600, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_VDISPLAY_END, 599, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_VVALID_START, 600, Head);    // START = END + 1 so no data goes out
                DAC_REG_WR32(NV_PRAMDAC_FP_VVALID_END, 599, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_VSYNC_START, 601, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_VSYNC_END, 625, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_VTOTAL, 638, Head);
                DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _VSYNC, _POS, Head);
                DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _HSYNC, _POS, Head);
                // Give it a 40 MHz clock
                dacCalcMNP(pDev, Head, 4000, &M, &N, &P);   // 40 MHz
                FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _V2SOURCE, _PROG); 
                REG_WR32(NV_PRAMDAC_VPLL2_COEFF, DRF_NUM(_PRAMDAC, _VPLL_COEFF, _MDIV, M) |
                                                DRF_NUM(_PRAMDAC, _VPLL_COEFF, _NDIV, N) |
                                                DRF_NUM(_PRAMDAC, _VPLL_COEFF, _PDIV, P));
                DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, _NONE, Head);     // FPCLK ON

                // dacProgramCRTC need a couple of values set in addition to the arguments passed.
                pDev->Framebuffer.HalInfo.Start[Head] = 0;
                // need one field in a lut object, not other fields used
                VidLutCurDac.Image[0].Pitch = 0;
                // Set 800x600 60 timing to make sure CRT is alive
                dacProgramCRTC(pDev, Head, &VidLutCurDac, 800, 800, 840, 840+128, 800+256, 1056,
                                     600, 600, 601, 605, 628, 628, 8, 4000, 1, 1, 0);

                // Slave the CRTC to timing generator
                CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, Cr28, Head);
                Cr28 |= BIT(7);
                CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, Cr28, Head); 
                CRTC_RD(0x33, Cr33, Head);   // NV_CIO_CRE_LCD__INDEX
                CRTC_WR(0x33, Cr33 | 0x11, Head); // bits 0 & 4 are HW
                tmrDelay(pDev, 200000000); // delay 200 ms for monitor to see syncs
                
                // read EDID 
                status = EDIDRead(pDev, Head, DISPLAY_TYPE_MONITOR);		// Try to read EDID
                
                // restore 
                CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, Cr28 & 0x7f, Head); // unslave CRTC
                DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, _BOTH, Head);     // FPCLK OFF
                CRTC_WR(0x33, Cr33, Head);
            }
        }
        else
        {

            // In use, if used by TV, we must preserve timings.
            // On NV11, it can only be TV.
            if (pVidLutCurDac->DisplayType == DISPLAY_TYPE_TV)
            {
                // Write 800x600 timing in the FP registers.
                // CRTC timing may not match FP timing. This is OK; we only need valid sync timing.
                DAC_REG_WR32(NV_PRAMDAC_FP_HCRTC, 800, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_HDISPLAY_END, 799, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_HVALID_START, 800, Head);    // START = END + 1 so no data goes out
                DAC_REG_WR32(NV_PRAMDAC_FP_HVALID_END, 799, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_HSYNC_START, 799+16, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_HSYNC_END, 839, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_HTOTAL, 1120, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_VCRTC, 600, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_VDISPLAY_END, 599, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_VVALID_START, 600, Head);    // START = END + 1 so no data goes out
                DAC_REG_WR32(NV_PRAMDAC_FP_VVALID_END, 599, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_VSYNC_START, 601, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_VSYNC_END, 625, Head);
                DAC_REG_WR32(NV_PRAMDAC_FP_VTOTAL, 638, Head);
                DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _VSYNC, _POS, Head);
                DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _HSYNC, _POS, Head);
                // Give it a 40 MHz clock
                Vpll2 = REG_RD32(NV_PRAMDAC_VPLL2_COEFF);
                dacCalcMNP(pDev, Head, 4000, &M, &N, &P);   // 40 MHz
                FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _V2SOURCE, _PROG); 
                REG_WR32(NV_PRAMDAC_VPLL2_COEFF, DRF_NUM(_PRAMDAC, _VPLL_COEFF, _MDIV, M) |
                                                DRF_NUM(_PRAMDAC, _VPLL_COEFF, _NDIV, N) |
                                                DRF_NUM(_PRAMDAC, _VPLL_COEFF, _PDIV, P));
                DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, _NONE, Head);     // FPCLK ON
                
                CoeffSelect = REG_RD32(NV_PRAMDAC_PLL_COEFF_SELECT);
                FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _VS_PCLK2_TV, _NONE);   // clock source is not TV
                
                // Slave the CRTC to timing generator
                CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, Cr28, Head);
                Cr28 |= BIT(7);
                CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, Cr28, Head); 
                CRTC_RD(0x33, Cr33, Head);   // NV_CIO_CRE_LCD__INDEX
                CRTC_WR(0x33, Cr33 | 0x11, Head); // bits 0 & 4 are HW
                tmrDelay(pDev, 200000000); // delay 200 ms for monitor to see syncs
                
                // read EDID 
                status = EDIDRead(pDev, Head, DISPLAY_TYPE_MONITOR);		// Try to read EDID
                
                // restore
                REG_WR32(NV_PRAMDAC_VPLL2_COEFF, Vpll2); 
                REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT, CoeffSelect);
                CRTC_WR(0x33, Cr33, Head);
            }
        }
        
        RestoreLock(pDev, Head, lock);
        if (IsNV11(pDev))
           EnableHead(pDev, prevHead);


        if (OS_READ_REGISTRY_DWORD(pDev, pDev->Registry.DBstrDevNodeDisplayNumber, "ForceCRTDetect", &data) == RM_OK)
        {
            if (data == 1)
            {
                force_flag = 1;
            }
        }

        if (status == RM_OK)		
    	{
            if (edidIsAnalogDisplay(pDev, Head))
                return TRUE;
            else    
            {
                if (force_flag == 1)
                {
                    return TRUE;
                }
                return FALSE;
            }
    	}
        else
        {
            if (force_flag == 1)
            {
                return TRUE;
            }
            return FALSE;   // the analog test (below) will not work on head 1--it always returns TRUE.
        }
    }
//#endif

    // NOTE: for cards with 1 head (NV15/20/...) that can have a CRT plugged into the DVI-I port
    // EDIDDetect will always fail, relying on the call to dacVGAConnectStatus below to properly
    // detect the CRT.  This works because on NV15/20 and other cards with 1 head, the RGB/sync lines
    // are physically connected, so a dacVGAConnectStatus(pDev,0) on head 0 will detect a CRT regardless
    // of which I2C it is actually on.
    status = EDIDDetect(pDev, Head, DISPLAY_TYPE_MONITOR);      // Try to read EDID
    if (status) 
    {
        if (Head != 1) // Head 1 does not have an internal DAC, so we cannot do the analog check.
        {
            // EDID did not confirm that a CRT is connected, but it may be an older non-DDC CRT,
            // so procede with analog out test.
            return (dacVGAConnectStatus(pDev, Head));
        }
        return FALSE;       // EDID read failed on head1, so not CRT
    }

    return TRUE;
}

BOOL dacVGAConnectStatus
(
    PHWINFO pDev, 
    U032    Head
)
{
    U032    dacPowerDown;
    U016    Sr1;
    U032    retries;
    U032    result = 0;
    U032    prevHead = (pDev->Dac.HalInfo.Head2RegOwner & 0x1);
    U032    rgb, trigger, testColor, dacControl;
    U032    PramdacCoeffSelect, CurrentVClk;
    U008    Cr28, Cr1A, PixMask, lock;
    U032    done = 0;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: VGA connection detection on head ", Head);

    // This test drives a selected amount of current out the DAC output.
    // This currect causes a voltage drop over the terminating
    // resistor (no CRT connected) or the terminating resistor in parallel
    // with the CRT load.  There is a voltage comparator which is sampled at
    // port 3C2. (Actually 3, one for each color.)  This indicates whether the
    // voltage at the output resistor has crossed a reference level.  If a CRT
    // is connected, the amount of current required to reach this level will
    // be twice (this can vary depending on the board) that required if there
    // is not a CRT.  (e.g. 75 ohms on the board, in parallel with 75 ohms at
    // the monitor = 37.5 ohms).  At the selected current level, we check to
    // see whether the comparator has flipped.  If so, there must be no CRT
    // connected (75 ohm load). If not, there is additional loading, i.e.  a
    // CRT is connected.
    // NOTE: we could possibly improve this in terms of speed by using the palette mask register and only changing
    // one byte of framebuffer and one palette location. But then again, since there's RC delay to ramp up voltage, there may 
    // be no advantage.
    lock = UnlockCRTC(pDev, Head);

    // Sequencer updates require pointing at the appropriate head
    EnableHead(pDev, Head);

    Sr1 = ReadIndexed(NV_PRMVIO_SRX, 0x01);
    if (Sr1 & 0x2000)
        WriteIndexed(NV_PRMVIO_SRX, (U016)(Sr1 & ~0x2000));	// turn off blank

    // make sure the DAC is driving RGB out
    dacPowerDown = DAC_REG_RD32(NV_PRAMDAC_TEST_CONTROL, Head);
    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _ON, Head);

    tmrDelay(pDev, 10000000);	// delay 10 ms to allow DAC to power up

    CurrentVClk = REG_RD32(NV_PRAMDAC_VPLL_COEFF);
    
    // We need to make sure that a mode is set up.  If there's no object and this is not the head the BIOS is using,
    // we should just go ahead and set up a dummy mode for the test.
    if((pDev->Dac.CrtcInfo[Head].pVidLutCurDac == NULL) && (Head != pDev->Dac.HalInfo.InitialBootHead))

    {
        VIDEO_LUT_CURSOR_DAC_OBJECT VidLutCurDac;
        U032 M,N,P;
        
        // Give it a 40 MHz clock
        dacCalcMNP(pDev, Head, 4000, &M, &N, &P);   // 40 MHz
        FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _V1SOURCE, _PROG); 
        REG_WR32(NV_PRAMDAC_VPLL_COEFF, DRF_NUM(_PRAMDAC, _VPLL_COEFF, _MDIV, M) |
                                        DRF_NUM(_PRAMDAC, _VPLL_COEFF, _NDIV, N) |
                                        DRF_NUM(_PRAMDAC, _VPLL_COEFF, _PDIV, P));
        DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, _NONE, Head);     // FPCLK ON

        // dacProgramCRTC need a couple of values set in addition to the arguments passed.
        pDev->Framebuffer.HalInfo.Start[Head] = 0;
        // need one field in a lut object, not other fields used
        VidLutCurDac.Image[0].Pitch = 0;
        // Set 800x600 60 timing to make sure CRT is alive
        dacProgramCRTC(pDev, Head, &VidLutCurDac, 800, 800, 840, 840+128, 800+256, 1056,
                             600, 600, 601, 605, 628, 628, 8, 4000, 1, 1, 0);

    }
    else
    {
        // If FP is output, it may be scaling, and so pixel clock is wrong for this mode. Get native clock.
        // NOTE for debugging: it's good to verify the output is changing color as we go thru the loop, but the
        // monitor may not be synced when the flat panel is the output device, because of the CRTC adjustments (esp. CR4).
        // NOTE 2: It is possible that pVidLutCurDac is NULL at this point.  This happens on Win2k when drivers are being
        // installed on the system for the very first time. In this case, the system will be running in vga mode.
        //
        if ((pDev->Dac.CrtcInfo[Head].pVidLutCurDac)
         && (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_FLAT_PANEL))
        {
            VIDEO_LUT_CURSOR_DAC_OBJECT * pVidLutCurDac;
            U032 M,N,P;

            pVidLutCurDac = (VIDEO_LUT_CURSOR_DAC_OBJECT *)pDev->Dac.CrtcInfo[Head].pVidLutCurDac;
            dacCalcMNP(pDev, Head, pVidLutCurDac->HalObject.Dac[0].PixelClock, &M, &N, &P);   // get native VCLK
            FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _V1SOURCE, _PROG); 
            REG_WR32(NV_PRAMDAC_VPLL_COEFF, DRF_NUM(_PRAMDAC, _VPLL_COEFF, _MDIV, M) |
                                            DRF_NUM(_PRAMDAC, _VPLL_COEFF, _NDIV, N) |
                                            DRF_NUM(_PRAMDAC, _VPLL_COEFF, _PDIV, P));
        }
        // If TV, should be OK as is, if CRT, definitely OK
    }
    // Here's the "ramping up the palette" method for detecting if a CRT is
    // present, reimplemented from what the BIOS had. The USE_TESTPOINT_DATA
    // version seemed not very reliable and frequently gave false CRT
    // detections. 
    // 
    // The palette version in testing gave no false detections for many test
    // loops and did correctly detect the monitor when it was present. The
    // trigger value used was tested on NV5/NV10/NV15 and NV11.
    //
    
    // Make sure we have a PCLK before accessing the palette
    PramdacCoeffSelect = REG_RD32(NV_PRAMDAC_PLL_COEFF_SELECT);
    // Make PLL the source
    if (Head == 0) 
    {
        FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _VS_PCLK_TV, _NONE);
    }
    else
    {
        FLD_WR_DRF_DEF(_PRAMDAC, _PLL_COEFF_SELECT, _VS_PCLK2_TV, _NONE);
    }
    // Not slaved, 8 bpp
    CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, Cr28, Head);
    CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, Cr28 & ~0x83, Head);    

    // Enable syncs
    CRTC_RD(NV_CIO_CRE_RPC1_INDEX, Cr1A, Head);
    CRTC_WR(NV_CIO_CRE_RPC1_INDEX, Cr1A & ~0xC0, Head);    

    // save the current palette (we only need one entry)
    DAC_REG_WR08(NV_USER_DAC_READ_MODE_ADDRESS, 0, Head);
    rgb = DAC_REG_RD08(NV_USER_DAC_PALETTE_DATA, Head);     // b
    rgb <<= 8;
    rgb |= DAC_REG_RD08(NV_USER_DAC_PALETTE_DATA, Head);    // g
    rgb <<= 8;
    rgb |= DAC_REG_RD08(NV_USER_DAC_PALETTE_DATA, Head);    // r
    
    // use the mask register to limit DAC lookup to the first entry
    PixMask = DAC_REG_RD08(NV_USER_DAC_PIXEL_MASK, Head);
    DAC_REG_WR08(NV_USER_DAC_PIXEL_MASK, 0, Head);
    
    // these values rely on being interpreted as 6 bits per color
    dacControl = DAC_REG_RD32(NV_PRAMDAC_GENERAL_CONTROL, Head);
    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _BPC, _6BITS, Head);
    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _PIXMIX, _ON, Head); // set PIXMIX so that LUT is used

    // Ramp up the color until the comparator flips or we hit the top of our range
    for (trigger = 0x8; (trigger < 0x18) && ! done; trigger++)
    {
        testColor = trigger; // one color should do it. The BIOS uses all three. 
        // testColor |= trigger << 8;
        // testcolor |= trigger << 16;

        // write RGB of the first entry in the palette
        DAC_REG_WR08(NV_USER_DAC_WRITE_MODE_ADDRESS, 0, Head);
        DAC_REG_WR08(NV_USER_DAC_PALETTE_DATA, testColor >> 16, Head); // R
        DAC_REG_WR08(NV_USER_DAC_PALETTE_DATA, testColor >> 8, Head);  // G
        DAC_REG_WR08(NV_USER_DAC_PALETTE_DATA, testColor, Head);       // B
        
        retries = 0;
        
        do
        {
            result = dacDetectTrigger(pDev);
            if (result==2)
            {
                done = 1;    // exit the test now
                DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"NVRM: vga connect timeout\n");
            }
        } while((result != dacDetectTrigger(pDev)) && (retries++ < 5));
        
        if (result == 1)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Trigger Detected\n");
            done = 1;
        }
    }
    // restore the mask register 
    DAC_REG_WR08(NV_USER_DAC_PIXEL_MASK, PixMask, Head);
    
    // restore the previous dac control
    DAC_REG_WR32(NV_PRAMDAC_GENERAL_CONTROL, dacControl, Head);

    // restore the previous palette value
    DAC_REG_WR08(NV_USER_DAC_WRITE_MODE_ADDRESS, 0, Head);
    DAC_REG_WR08(NV_USER_DAC_PALETTE_DATA, rgb >> 16, Head); // b
    DAC_REG_WR08(NV_USER_DAC_PALETTE_DATA, rgb >> 8, Head);  // g
    DAC_REG_WR08(NV_USER_DAC_PALETTE_DATA, rgb, Head);       // r

    // restore VCLK
    REG_WR32(NV_PRAMDAC_VPLL_COEFF, CurrentVClk);

    // restore the previous dac powerdown
    DAC_REG_WR32(NV_PRAMDAC_TEST_CONTROL, dacPowerDown, Head);

    // restore the previous SR1 value
    // sequencer updates require pointing at the appropriate head
    EnableHead(pDev, Head);
    WriteIndexed(NV_PRMVIO_SRX, (U016)Sr1);

    // restore previous CR44 value
    EnableHead(pDev, prevHead);
    // restore Cr28
    CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, Cr28, Head);    
    // restore syncs
    CRTC_WR(NV_CIO_CRE_RPC1_INDEX, Cr1A, Head);    
    // restore clock source
    REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT, PramdacCoeffSelect);

    RestoreLock(pDev, Head, lock);

    if (trigger < 0x18) // If trigger, there is less DC load, therefore no monitor
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"NVRM: Monitor detect: FALSE\n");
        
        return FALSE;
    }
    else
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"NVRM: Monitor detect: TRUE -- red trigger is: ", trigger);
        
        return TRUE;
    }
}


// Check whether flat panel is currently connected
BOOL dacFlatPanelConnectStatus
(
    PHWINFO pDev,
    U032    Head
)
{
    RM_STATUS status;
    U008 lock;

    if (using_new_api == FALSE) {
        //
        // Make sure flatpanel allowed on this head.
        //
        if ((GETCRTCHALINFO(pDev, Head, DisplayTypesAllowed) & DISPLAYTYPEBIT(DISPLAY_TYPE_FLAT_PANEL)) == 0)
            return FALSE;

        //
        // If the other head is TV, we can't do it (P38 with external TMDS, P41 with internal is OK)
        // TODO: Need to change this to a dynamic check based on feedback from the BIOS topology table
        // If we're using the 12bit wide FP interface, then FP/TV can share and are allowed.
        //
        if ((GETDISPLAYTYPE(pDev, Head^1) == DISPLAY_TYPE_TV) && !pDev->Power.MobileOperation &&
            (REG_RD_DRF(_PEXTDEV, _BOOT_0, _STRAP_FP_IFACE) == NV_PEXTDEV_BOOT_0_STRAP_FP_IFACE_24BIT))
            return FALSE;
    }

    lock = UnlockCRTC(pDev, Head);

    // read EDID 
    status = EDIDRead(pDev, Head, DISPLAY_TYPE_FLAT_PANEL);	// Try to read EDID

    RestoreLock(pDev, Head, lock);

    if (status != RM_OK)
        return FALSE;

    if (edidIsDigitalDisplay(pDev, Head)) 		// Check the display technology byte
    {
        pDev->Dac.CrtcInfo[Head].EdidDisplayType = DISPLAY_TYPE_FLAT_PANEL;
        return TRUE;
    }
    return FALSE;
}

U032 dacGetFlatPanelConfig
(
    PHWINFO pDev,
    U032    Head
)
{
    U032 mode;
    
    mode = DAC_REG_RD32(NV_PRAMDAC_FP_TG_CONTROL, Head); 
    return(DRF_VAL(_PRAMDAC, _FP_TG_CONTROL, _MODE, mode)); 
}
    
RM_STATUS dacGetFlatPanelInfo
(
    PHWINFO pDev,
    U032    Head,
    U032    *FpMode,
    U032    *FpMaxX,
    U032    *FpMaxY,
    BOOL    *FpConnected,
    BOOL    *FlatPanelNativeSupported,
    BOOL    fromRegistry
)
{
    U032      mode;
    RM_STATUS status;
    U008    i, chksum2 = 0xff, chksum = 0xff;
    BOOL    EdidOk = FALSE;

    // Not getting an EDID for the flat panel is catasrophic, so we'll do some retries if we fail.
    for (i=0; i<3; i++) 
    {
        // Get info from EDID
        if (!dacFlatPanelConnectStatus(pDev, Head))	// FP connected? (we got a header)
        {
            *FpConnected = NV_CFGEX_GET_FLATPANEL_INFO_NOT_CONNECTED;
            return RM_OK;						// then no more to do
        }
        // If we got an EDID, make sure it's a good one
        if (fpParseEDID(pDev, Head) == RM_OK) // parse EDID to get timing info (fail if bad header, or (?) bad timing)
        {
            // Checksum to verify this is a valid EDID
            if (EDIDCheckSumOk(pDev, Head, pDev->Dac.CrtcInfo[Head].EDID, 128, &chksum)) // expect checksum = 0
            {
                EdidOk = TRUE;
                break; // stop iteration if checksum is good
            }
            else
            {
                // some "good" panels have bad checksums, so if we get the same checksum twice, accept it
                if (chksum == chksum2) 
                {
                    EdidOk = TRUE;
                    break; // stop iteration if we get same checksum twice
                }
                chksum2 = chksum;
            }
            // If we fail, print checksum
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Flat Panel EDID checksum =  ", chksum);
        }
    } // if we still fail, we have no choice but to go with it

    *FpConnected = NV_CFGEX_GET_FLATPANEL_INFO_CONNECTED;
    *FpMaxX = pDev->Dac.HalInfo.fpHMax;
    *FpMaxY = pDev->Dac.HalInfo.fpVMax;
    *FlatPanelNativeSupported = FALSE;
    if (pDev->Dac.fpNative)
        *FlatPanelNativeSupported = TRUE;

    if (fromRegistry)
    {
        //JJV-New registry Process   char * strDisplayMode;
        //JJV-New registry Process   U032   size;
        // read it from the registry
        // fill in the path to the registry key--last character is the display number
        //JJV-New registry Process size = osStringLength(pDev->Registry.DBstrDevNodeDisplayNumber);
        //JJV-New registry Process pDev->Registry.DBstrDevNodeDisplayNumber[size - 1] = ('0' | (U008)devInstance);    // fill in ascii device #
        status = OS_READ_REGISTRY_DWORD(pDev, pDev->Registry.DBstrDevNodeDisplayNumber, strFpMode, &mode);
    	if (status == RM_OK)
	        *FpMode = mode;
    	else
	    	*FpMode = NV_CFGEX_GET_FLATPANEL_INFO_NATIVE;
    }
    else
    {
        mode = dacGetFlatPanelConfig(pDev, Head); // This just reads the HW to find the current scaling mode
        *FpMode = mode;
        status = RM_OK;
    }
    if (!EdidOk) 
        return RM_ERROR; // failed to get a good EDID
    else
        return status;   // error if requested registry, but none found
}

// Check whether TV is currently connected
// This only works for the Brooktree encoder
BOOL dacTVConnectStatus
(
    PHWINFO pDev,
    U032    Head
)
{
    U008    read, ack, lock, encoderPower, reg0e, reg10, reg3d, i;
    BOOL    connect;
    U032    retry;

    if (using_new_api == FALSE) {
        //
        // Make sure TV allowed on this head.
        //
        if ((GETCRTCHALINFO(pDev, Head, DisplayTypesAllowed) & DISPLAYTYPEBIT(DISPLAY_TYPE_TV)) == 0)
            return FALSE;
    }

    //
    // For NV11, make sure TV isn't in use by other head.
    // For P38 boards (external TMDS), we cannot run both TV and flat panel. This will change if we use internal TMDS,
    // but we'll have to get that info from the BIOS. Until then, TV + DFP is not allowed.
    //
    if (IsNV11(pDev))
    {
        if (using_new_api == FALSE) {
            // If the other head already has the TV, we can't use it.
            if (GETDISPLAYTYPE(pDev, Head^1) == DISPLAY_TYPE_TV)  
                return FALSE;

            //
            // If the other head is flat panel, we can't do it (only if this board is using external TMDS)
            // or if we're using the 12bit wide FP interface, then FP/TV can share and are allowed.
            //
            if ((GETDISPLAYTYPE(pDev, Head^1) == DISPLAY_TYPE_FLAT_PANEL) && !pDev->Power.MobileOperation &&
                (REG_RD_DRF(_PEXTDEV, _BOOT_0, _STRAP_FP_IFACE) == NV_PEXTDEV_BOOT_0_STRAP_FP_IFACE_24BIT))
                return FALSE;

            // If the other head is CRT on head 1, we can't do it (only if this board is using external TMDS)
            if ((GETDISPLAYTYPE(pDev, Head^1) == DISPLAY_TYPE_MONITOR) && (Head == 0))
                return FALSE;
        }
        
        //
        // Make sure that i2c/tv ownership is setup properly.
        //
        AssocDDC(pDev, Head);
        AssocTV(pDev, Head);
    }

    lock = UnlockCRTC(pDev, Head);
    switch (TV_ENCODER_FAMILY(pDev->Dac.EncoderType))
    {
        case TV_ENCODER_CHRONTEL:
            // Set power to "normal"
            // Chrontel 7007 wants bit 7 set on register address
            
            // Read Power Management Register to preserve contents
            for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
                i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   
                ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID));
            }
            if (ack == 0) {
                ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_0E); // 0x0e = PM register
                i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   // RESTART = start without previous end
                ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID | 1));  // prepare for read
                i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &reg0e,1);
            }
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
            
            // Powered up? No, then do so
            if ((reg0e & 3) != 3) {
                for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
                    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   
                    ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
                }
                if (ack == 0) {
                    ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_0E); // 0e = power register
                    ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x0b); // ON + RESET HIGH
                }
            }
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);


            // Some initialization is necessary on the 7007 to make the detection work.
            // Exactly what needs to be set, I don't know, but the following 2 register initializations do it.

            // Set data format
            for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
                i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   
                ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
            }
            if (ack == 0) {
                ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_04); // 04 = input data format reg
                ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x05);      // set to NTSC for now
            }
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
            
            // Set clocking
            for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
                i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   
                ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
            }
            if (ack == 0) {
                ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_06); // 06 = clock mode
                ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x40);      // 
            }
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
            
            // In the 7008, reg 3D bits 2:0 must be cleared for detection to work
            // Don't know what effect it has on others, so we'll special case it.
            if (pDev->Dac.EncoderType == NV_ENCODER_CHRONTEL_7008) {

                // Read Reg 3D to preserve contents
                for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
                    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   
                    ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
                }
                if (ack == 0) {
                    ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_3D); // 3D = ?
                    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   // RESTART = start without previous end
                    ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID | 1));  // prepare for read
                    i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &reg3d,1);
                }
                i2cStop(pDev, Head, pDev->Dac.TVOutPortID);

                if (ack == 0) {
                    // clear mystery bits 2:0 so TV connect detection will work
                    for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
                        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   
                        ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
                    }
                    if (ack == 0) {
                        ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_3D); // 
                        ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(reg3d & 0xf8)); 
                    }
                    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
                }
            } // 7008

            // Read Connection Detect Register to preserve contents
            for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
                i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   
                ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
            }
            if (ack == 0) {
                ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_10); // 10 = connect detect register
                i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   // RESTART = start without previous end
                ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID | 1));  // prepare for read
                i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &reg10,1);
            }
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);

            if (ack == 0) {
                // Set Sense bit to tell encoder to check for TV connected
                for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
                    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   
                    ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
                }
                if (ack == 0) {
                    ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_10); // 10 = connect detect register
                    reg10 &= 0x0f;  // clear bits 7:4
                    ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(reg10 | 1)); // sense bit <= 1
                }
                i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
            }

            if (ack == 0) {
                // Reset Sense bit 
                for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
                    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   
                    ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
                }
                if (ack == 0) {
                    ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_10); // 10 = connect detect register
                    ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x00); // sense bit <= 0
                }
                i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
            }

            if (ack == 0) {
                // Read Connect reg - restart read
                for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
                    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   
                    ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
                }
                if (ack == 0) {
                    ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, CH_REG_10); // 10 = connect detect register
                    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   // RESTART = start without previous end
                    ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID | 1));  // prepare for read
                    i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &read,1);
                }
                i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
            }
            // If no head is using TV, blank image.
            for (i = 0; i < pDev->Dac.HalInfo.NumCrtcs; i++)
                if (GETDISPLAYTYPE(pDev, i) == DISPLAY_TYPE_TV)
                    break;
            if (i == pDev->Dac.HalInfo.NumCrtcs)
            {
              // Say that the device is enabled just to disable it
              pDev->Dac.DevicesEnabled |= DAC_TV_ENABLED;
              dacDisableTV(pDev, Head);
            }

            if  ((ack == 0 ) &&     // If we failed some how, we fail this
                 ((read & 0x0e) ^ 0x0e))     // 0 indicates connection
                                    // bit 3: Y (luma), bit 2: C (chroma), bit 1: composite
                connect = TRUE;
            else
                connect =  FALSE;
            break;
   

        case TV_ENCODER_BROOKTREE:

            // Turn DAC on before checking status
            i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
            ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
            ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_DAC_CTL);   // subaddress
            ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_DAC_ON);    // DAC ON
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);

            // Give it some time for the outputs to stabilize
            tmrDelay(pDev, 20000000);         // 20 ms delay    

            // Set Check Status bit to tell encoder to check for TV connected.
            for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
                i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
                ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
                if (ack)
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Missed one ACK after first address byte while detecting TV connection\n\r");
            }
            if (ack == 0) {
                ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_DAC_CTL); // subaddress
                ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_CHK_STAT); // set check stat
            } else {
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Swell, no ACK after first address byte while detecting TV connection\n\r");
            }
            i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
            if (ack != 0) {
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Swell, no ACK during first transaction while detecting TV connection\n\r");
            }

            //870/871 supports direct register reads.  The older chips require a more involved procedure.
            if (pDev->Dac.EncoderType == NV_ENCODER_BROOKTREE_871) {
                
                // enable non-legacy readback mode for 870/871 by doing a reset - harmless for older parts
                // (this enables read of any register on 87x -- 868/869 has only one read subaddress)
                i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                       // start state for I2C
                i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
                i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x6C);     // subaddress
                i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x44);     // TIMING_RST low, set readback mode, EACTIVE
                i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                         
                tmrDelay(pDev, 10000000);
                i2cStart(pDev, Head, pDev->Dac.TVOutPortID);                       
                i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)pDev->Dac.EncoderID);   // send ID
                i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x6C);  
                i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0xC4);      // TIMING_RST high
                i2cStop(pDev, Head, pDev->Dac.TVOutPortID);                         

                if (ack == 0) {
                    for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
                        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
                        ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
                        if (ack)
                            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Missed one ACK after third address byte while detecting TV connection\n\r");
                    }
                    ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x06);
                }

                if (ack == 0) {
                    for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
                        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
                        ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID | 1));   // Add read bit
                        if (ack)
                            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Missed one ACK after third address byte while detecting TV connection\n\r");
                    }
                    if (ack == 0) {
                        i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &read, 1);
                    } else {
                        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Swell, no ACK after third address byte while detecting TV connection\n\r");
                    }
                    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
                }
            } else {
                if (ack == 0) {
                    // Set Estat to select status register 1
                    for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
                        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
                        ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
                        if (ack)
                            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Missed one ACK after second address byte while detecting TV connection\n\r");
                    }
                    if (ack == 0) {
                        ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0xC4);   // subaddress
                        ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0x41);   // set estat (estat 01 + en_out)
                    } else {
                        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Swell, no ACK after second address byte while detecting TV connection\n\r");
                    }
                    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
                    if (ack != 0) {
                        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Swell, no ACK during second transaction while detecting TV connection\n\r");
                    }
                }

                if (ack == 0) {
                    // Read Monitor Status bits
                    for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
                        i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
                        ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID | 1));   // Add read bit
                        if (ack)
                            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Missed one ACK after third address byte while detecting TV connection\n\r");
                    }
                    if (ack == 0) {
                        i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &read, 1);
                    } else {
                        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Swell, no ACK after third address byte while detecting TV connection\n\r");
                    }
                    i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
                } 
            }

            if (ack == 0) {
                // Reset Check Status -- not documented, but very important (can't program encoder if not done).
                for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
                    i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
                    ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
                    if (ack)
                        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Missed one ACK after last address byte while detecting TV connection\n\r");
                }
                if (ack == 0) {
                    ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_DAC_CTL); // subaddress
                    ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) BT_CHK_OFF); // set check stat
                } else {
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Swell, no ACK after last address byte while detecting TV connection\n\r");
                }
                i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
                if (ack != 0) {
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Swell, no ACK during last transaction while detecting TV connection\n\r");
                }
            }

            if (ack) // if any error occurred, go to monitor
                return (FALSE);

            if (read & BT_MONSTAT)  // any connection deteced?
                connect = TRUE;
            else
                connect =  FALSE;

            // If not TV, disable DAC
            if (GETDISPLAYTYPE(pDev, Head) != DISPLAY_TYPE_TV)
            {
                // If no head is using TV, blank image.
                for (i = 0; i < pDev->Dac.HalInfo.NumCrtcs; i++)
                    if (GETDISPLAYTYPE(pDev, i) == DISPLAY_TYPE_TV)
                        break;
                if (i == pDev->Dac.HalInfo.NumCrtcs)
                {
                  // Say that the device is enabled just to disable it
                  pDev->Dac.DevicesEnabled |= DAC_TV_ENABLED;
                  dacDisableTV(pDev, Head);
                }
            }
            break;
        
       case TV_ENCODER_PHILIPS:
           // Turn Power On -- Read/Modify/Write
           for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
               i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   
               ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
           }
           if (ack == 0) {
              ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x61); // 
              i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   // RESTART = start without previous end
              ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID | 1));  // prepare for read
              i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &read,1);
           }
           i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
           encoderPower = read;
           read &= 0x3f;   // power on
           // Write Power bits
           for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
               i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
               ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
               if (ack)
                   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: No ACK from TV encoder\n\r");
           }
           if (ack == 0) {
               ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0x61);   // subaddress
               ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) read);   // 
           } else {
               DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: No ACK from TV encoder\n\r");
           }
           i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
           
           // Set Monitor Sense Threshold
           for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
               i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
               ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
               if (ack)
                   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: No ACK from TV encoder\n\r");
           }
           if (ack == 0) {
               ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0x1a);   // subaddress
               ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0x46);   // MSM threshold
           } else {
               DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: No ACK from TV encoder\n\r");
           }
           i2cStop(pDev, Head, pDev->Dac.TVOutPortID);

           // Set Monitor Sense Threshold
           for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
               i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
               ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
               if (ack)
                   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: No ACK from TV encoder\n\r");
           }
           if (ack == 0) {
               ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0x1a);   // subaddress
               ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0x46);   // MSM threshold
           } else {
               DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: No ACK from TV encoder\n\r");
           }
           i2cStop(pDev, Head, pDev->Dac.TVOutPortID);

           // Set Monitor Sense Mode
           for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
               i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
               ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
               if (ack)
                   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: No ACK from TV encoder\n\r");
           }
           if (ack == 0) {
               ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0x1b);   // subaddress
               ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0x80);   // MSM
           } else {
               DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: No ACK from TV encoder\n\r");
           }
           i2cStop(pDev, Head, pDev->Dac.TVOutPortID);

           // Read sense bits
           for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
               i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   
               ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
           }
           if (ack == 0) {
              ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, 0x1b); // 
              i2cStart(pDev, Head, pDev->Dac.TVOutPortID);   // RESTART = start without previous end
              ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008)(pDev->Dac.EncoderID | 1));  // prepare for read
              i2cReceiveByte(pDev, Head, pDev->Dac.TVOutPortID, &read,1);
           }
           i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
           if ((read & 0x07) == 0x07)    // check C and Y inputs
               connect = FALSE;
           else
               connect = TRUE;                                    
           // Reset Monitor Sense Mode
           for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
               i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
               ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
               if (ack)
                   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: No ACK from TV encoder\n\r");
           }
           if (ack == 0) {
               ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0x1b);   // subaddress
               ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0x00);   // MSM
           } else {
               DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: No ACK from TV encoder\n\r");
           }
           i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
           // Restore power state
           for (ack = 1, retry = I2C_ADDRESS_RETRIES; (ack != 0) && (retry != 0); retry--) {
               i2cStart(pDev, Head, pDev->Dac.TVOutPortID);
               ack = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) (pDev->Dac.EncoderID));
               if (ack)
                   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: No ACK from TV encoder\n\r");
           }
           if (ack == 0) {
               ack  = i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) 0x61);   // subaddress
               ack |= i2cSendByte(pDev, Head, pDev->Dac.TVOutPortID, (U008) encoderPower);   // power
           } else {
               DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: No ACK from TV encoder\n\r");
           }
           i2cStop(pDev, Head, pDev->Dac.TVOutPortID);
           break;
        default:
                connect =  FALSE;
    }
    RestoreLock(pDev, Head, lock);

    // restore TV connection if TV is connected on the other head
    if (GETDISPLAYTYPE(pDev, Head ^ 0x1) == DISPLAY_TYPE_TV) {
        AssocDDC(pDev, Head ^ 0x1);
        AssocTV(pDev, Head ^ 0x1);
    }

    // restore macrovision mode if appropriate
    if (pDev->Dac.TVLastMacrovision.mode & 0x3)
        SetMacrovisionMode(pDev, Head, (RM_MACROVISION_CONTROL *)&(pDev->Dac.TVLastMacrovision));

    return connect;
}

//  Set the text cursor blink rate depending on the display type
//  The BIOS won't do this, so we have to.
VOID dacSetCursorBlink
(
    PHWINFO pDev,
    U032    Head,
    U008    Rate
)
{
U008    data;

    if (IsNV5(pDev)) // This is a problem in NV5 only
    {
        // another undocumented register
        CRTC_RD(NV_CIO_CRE_TREG__INDEX, data, Head);
        data &= ~0x80;
        if (Rate)   // CRT set bit 7 to 0, FP & TV set to 1
            data |= 0x80;
        CRTC_WR(NV_CIO_CRE_TREG__INDEX, data, Head);
    }
}

// Unlock extended CRTC and return former state of lock
U008 UnlockCRTC
(
    PHWINFO pDev,
    U032    Head
)
{
    U008 lock;
    
    lock = ReadCRTCLock(pDev, Head);
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, Head);
    return lock;
}
VOID RestoreLock
(
    PHWINFO pDev, 
    U032    Head, 
    U008    lock
)
{
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, lock, Head);
}
//  Read extended CRTC lock register and return corresponding write value
U008 ReadCRTCLock
(
    PHWINFO pDev,
    U032    Head
)
{
    U008 data;

    CRTC_RD(NV_CIO_SR_LOCK_INDEX, data, Head);
    switch (data)
    {
        case 0x03:
            data = 0x57;
            break;
        case 0x01:
            data = 0x75;
            break;
        case 0x00:
        default:
            data = 0x99;
            break;
    }
    return data;
}

VOID dacSetFan
(
    PHWINFO pDev,
    U008    state
)
{
    U008 data, lock;
    U032 Head = 0;

    if (IsNV10orBetter(pDev))
    {
        // unlock the extended regs
        lock = UnlockCRTC(pDev, Head);
        
        CRTC_RD(0x35, data, Head);
        data &= ~0x02;		// bit 1 = output enable (active low) 
        if (state == 0)
        {
            // OFF
            CRTC_WR(0x35, data & ~0x01, Head);	// bit 0 = fan control (active high)
        }
        else
        {
            // ON
            CRTC_WR(0x35, data | 0x01, Head);		// bit 0 = fan control (active high)
        }

        RestoreLock(pDev, Head, lock);
    }
}

RM_STATUS dacGetColorSaturationBoost
(
    PHWINFO pDev,
    U032    Head,
    U008    *boostValue
)
{
    U008 lock;

    if (IsNV11orBetter(pDev) == FALSE)
        return CONFIG_GETEX_BAD_READ;    // not supported pre-NV11

    // unlock the extended regs
    lock = UnlockCRTC(pDev, Head);

    // get the color boost value
    CRTC_RD(NV_CIO_CRE_CSB, *boostValue, Head);

    RestoreLock(pDev, Head, lock);

    return RM_OK;
}

RM_STATUS dacSetColorSaturationBoost
(
    PHWINFO pDev,
    U032    Head,
    U008    boostValue
)
{
    U008 lock;

    if (IsNV11orBetter(pDev) == FALSE)
        return CONFIG_GETEX_BAD_READ;    // not support pre-NV11

    if (boostValue > NV_CFGEX_PREDAC_COLOR_SATURATION_BOOST_750)
        return CONFIG_SETEX_BAD_PARAM;    // 75% is the largest value

    // unlock the extended regs
    lock = UnlockCRTC(pDev, Head);

    // set the color saturation boost value
    CRTC_WR(NV_CIO_CRE_CSB, boostValue, Head);

    RestoreLock(pDev, Head, lock);
    return RM_OK;
}

#ifdef MACOS
/*
 * The fcode on the mac implements Apple's monitor selection policy
 *  (what device to boot on, etc).  The fcode then publishes this information
 *  in the registry.
 *
 * Here we read that information.
 *
 * "displayA"      --  HHIILLTT
 * "displayB"      --  HHIILLTT
 * "display-type"  -- "CRT" or "LCD"  preferred boot device
 *
 * Decoder ring:
 *             HH -- head
 *             II -- i2c status port base
 *             LL -- TMDS link info
 *             TT -- display type (0 = crt, 3 = digital flat panel)
 *
 * If a display is not present, then the value will be 0x??FFFFFF.
 *
 * On the iMac / P11, displayA would be 00FFFF00 (Head A, no I2C port, no TMDS
 * link, CRT) and displayB would be 013E0103 (Head B, I2C 3E/3F, Link 2, DFP).
 *
 */

RM_STATUS dacMacReadBIOSDisplaySettings(
    PHWINFO pDev
)
{
    U032 displayA, displayB;
    U032 head, i2cportA=0, i2cportB=0, tmdslink;
    U032 displaytype = 0;
    char primary_displaytype_string[16];
    U032 reglen;

    displayA = ~0;
    displayB = ~0;
    osReadRegistryDword(pDev, (char *) 0, "displayA", &displayA);
    osReadRegistryDword(pDev, (char *) 0, "displayB", &displayB);
    if ((displayA == ~0) && (displayB == ~0))
        return RM_ERROR;

    // what is the display type that the fcode depended on?
    // We don't really use this right now...

    reglen = sizeof(primary_displaytype_string);
    osMemSet(primary_displaytype_string, 0, reglen);
    osReadRegistryBinary(pDev,
                         (char *) 0,
                         "display-type",
                         (U008 *) primary_displaytype_string,
                         &reglen);


    // parse out displayA settings

    if ((displayA & 0x00FFFFFF) != 0x00FFFFFF)
    {
        head =        (displayA >> 24) & 0xFF;
        i2cportA =    (displayA >> 16) & 0xFF;
        tmdslink =    (displayA >> 8)  & 0xFF;
        displaytype = (displayA >> 0)  & 0xFF;
    }
    
    if ((displayB & 0x00FFFFFF) != 0x00FFFFFF)
    {
        head =        (displayB >> 24) & 0xFF;
        i2cportB =    (displayB >> 16) & 0xFF;
        tmdslink =    (displayB >> 8)  & 0xFF;
        displaytype = (displayB >> 0)  & 0xFF;
    }
    
    //
    // NOTE: DFP2 means the DFP on head B!!
    //       DFP  means the DFP on head A.
    //
    // first mark them all unknown
    //
    pDev->Dac.DFPPortID  = NV_I2C_PORT_TERTIARY;
    pDev->Dac.DFP2PortID = NV_I2C_PORT_TERTIARY;
    pDev->Dac.CRTPortID  = NV_I2C_PORT_TERTIARY;
    pDev->Dac.CRT2PortID = NV_I2C_PORT_TERTIARY;

    // If we have anything on headB, then that will be our primary.
    // If not, then A will be primary
    if ((displayB & 0x00FFFFFF) != 0x00FFFFFF)
    {
        pDev->Dac.I2CPrimaryStatusIndex = i2cportB;
        pDev->Dac.I2CPrimaryWriteIndex = i2cportB + 1;
        
        if (displaytype == 3)
            pDev->Dac.DFP2PortID = NV_I2C_PORT_PRIMARY;
        else
            pDev->Dac.CRT2PortID = NV_I2C_PORT_PRIMARY;

        if ((displayA & 0x00FFFFFF) != 0x00FFFFFF)
        {
            pDev->Dac.I2CSecondaryStatusIndex = i2cportA;
            pDev->Dac.I2CSecondaryWriteIndex = i2cportA + 1;
            
            if (displaytype == 3)
                pDev->Dac.DFPPortID = NV_I2C_PORT_SECONDARY;
            else
                pDev->Dac.CRTPortID = NV_I2C_PORT_SECONDARY;
        }
    }
    else
    {
        pDev->Dac.I2CPrimaryStatusIndex = i2cportA;
        pDev->Dac.I2CPrimaryWriteIndex = i2cportA + 1;
        
        if (displaytype == 3)
            pDev->Dac.DFPPortID = NV_I2C_PORT_PRIMARY;
        else
            pDev->Dac.CRTPortID = NV_I2C_PORT_PRIMARY;
    }
    
    return  RM_OK;
}

#endif // MACOS



//
// Extract the relevant I2C values from any arbitrary version of BMP structure
// Update the nvInfo structure with the location of the TV and FP ports.
RM_STATUS dacReadBIOSI2CSettings(
    PHWINFO pDev
)
{
    RM_STATUS rmStatus = RM_OK;

#ifdef IKOS
    pDev->Dac.I2CPrimaryWriteIndex = NV_CIO_CRE_DDC_WR__INDEX;
    pDev->Dac.I2CPrimaryStatusIndex = NV_CIO_CRE_DDC_STATUS__INDEX;
    pDev->Dac.I2CSecondaryWriteIndex = NV_CIO_CRE_DDC0_WR__INDEX;
    pDev->Dac.I2CSecondaryStatusIndex = NV_CIO_CRE_DDC0_STATUS__INDEX;
    pDev->Dac.CRTPortID = NV_I2C_PORT_PRIMARY;
    pDev->Dac.TVOutPortID = NV_I2C_PORT_SECONDARY;
    pDev->Dac.DFPPortID = NV_I2C_PORT_SECONDARY;
#else

    if ( !IsNV5orBetter(pDev) || IsNV0A(pDev) )
    {
        // This is a one DDC port device
        pDev->Dac.I2CPrimaryWriteIndex = NV_CIO_CRE_DDC_WR__INDEX;
        pDev->Dac.I2CPrimaryStatusIndex = NV_CIO_CRE_DDC_STATUS__INDEX;
        pDev->Dac.I2CSecondaryWriteIndex = NV_CIO_CRE_DDC_WR__INDEX;
        pDev->Dac.I2CSecondaryStatusIndex = NV_CIO_CRE_DDC_STATUS__INDEX;
        pDev->Dac.CRTPortID = NV_I2C_PORT_PRIMARY;
        pDev->Dac.TVOutPortID = NV_I2C_PORT_PRIMARY;
        pDev->Dac.DFPPortID = NV_I2C_PORT_PRIMARY;
    }
    else
    {
        U032 offset;
        BMP_Control_Block bmpCtrlBlk;
        U032 RegistryData;

        // This device has two DDC ports, set defaults

        pDev->Dac.I2CPrimaryWriteIndex = NV_CIO_CRE_DDC_WR__INDEX;
        pDev->Dac.I2CPrimaryStatusIndex = NV_CIO_CRE_DDC_STATUS__INDEX;
        pDev->Dac.I2CSecondaryWriteIndex = NV_CIO_CRE_DDC0_WR__INDEX;
        pDev->Dac.I2CSecondaryStatusIndex = NV_CIO_CRE_DDC0_STATUS__INDEX;
        if (pDev->Power.MobileOperation)
        {
            // 
            // For mobile, CIO_CRE_DDC2 (CR50/CR51) is used to control the spread
            // spectrum chip (avoid using this pair for any DDC communication).
            //
            pDev->Dac.I2CTertiaryWriteIndex = NV_CIO_CRE_DDC0_WR__INDEX;
            pDev->Dac.I2CTertiaryStatusIndex = NV_CIO_CRE_DDC0_STATUS__INDEX;
        }
        else
        {
            pDev->Dac.I2CTertiaryWriteIndex = NV_CIO_CRE_DDC2_WR__INDEX;
            pDev->Dac.I2CTertiaryStatusIndex = NV_CIO_CRE_DDC2_STATUS__INDEX;
        }
        pDev->Dac.CRTPortID = NV_I2C_PORT_PRIMARY;
        pDev->Dac.TVOutPortID = NV_I2C_PORT_SECONDARY;
        pDev->Dac.DFPPortID = NV_I2C_PORT_SECONDARY;
        
        // NV11, NV20 have three DDC ports
        if (IsNV11(pDev))
        {
#if 0 // We can no longer assume that VGA is the boot device. We assume that we can talk to VGA over I2C 0 and DFP over I2C 1
      // unless we find a registry override.
            // determine if this is VGA + ? or DVI/DVI board
            if (dacVGAConnectStatus(pDev, 0))  // Is there VGA on I2C 0? (Uses analog comparator)
            {
                // VGA is mapped to I2C 0, so the secondary could be VGA or DFP on I2C 1
                pDev->Dac.CRT2PortID = NV_I2C_PORT_SECONDARY;   // second CRT uses I2C 1
                pDev->Dac.DFPPortID = NV_I2C_PORT_TERTIARY;     // No primary DFP--set to unused port
                pDev->Dac.DFP2PortID = NV_I2C_PORT_SECONDARY;   // second DFP uses I2C 1
            }
            else // no CRT, assume dual DFP
            {
                if (dacFlatPanelConnectStatus(pDev, 0)) // this will try to find EDID on I2C 1
                {
                    // DVI on I2C 1 and 2                
                    pDev->Dac.DFP2PortID = NV_I2C_PORT_TERTIARY;   // secondary DFP uses I2c 2
                }
                else // No DFP after all, assume CRT with no EDID
                {
                    pDev->Dac.CRT2PortID = NV_I2C_PORT_SECONDARY;   // second CRT uses I2C 1
                    pDev->Dac.DFPPortID = NV_I2C_PORT_TERTIARY;    // set to unused port
                    pDev->Dac.DFP2PortID = NV_I2C_PORT_TERTIARY;   // set to unused port
                }
            }
#else
            U032 val = 0;
            char strDualDVI[] = "Dual DVI";

            // VGA is mapped to I2C 0, so the secondary could be VGA or DFP on I2C 1
            pDev->Dac.CRT2PortID = NV_I2C_PORT_SECONDARY;   // second CRT uses I2C 1
            pDev->Dac.DFPPortID = NV_I2C_PORT_TERTIARY;     // No primary DFP--set to unused port
            pDev->Dac.DFP2PortID = NV_I2C_PORT_SECONDARY;   // second DFP uses I2C 1
            
            // registry override for dual DVI board -- Flat Panels connected to I2C 1 and 2
            OS_READ_REGISTRY_DWORD(pDev, pDev->Registry.DBstrDevNodeDisplayNumber, strDualDVI, &val);
            if (val) 
            {
                pDev->Dac.DFPPortID = NV_I2C_PORT_SECONDARY;
                pDev->Dac.DFP2PortID = NV_I2C_PORT_TERTIARY;   // secondary DFP uses I2c 2
            }
#endif        
        }

#ifdef MACOS
        // the Mac is *special*
        // The fcode sets registry properties to indicate i2c mappings
        //    If we find these registry settings, we'll use them,
        //    otherwise we'll look for the BMP bios table.
        if (RM_OK == dacMacReadBIOSDisplaySettings(pDev))
            return RM_OK;
#endif

        // Attempt to find the I2C info in the BIOS' BMP tables
        rmStatus = DevinitGetBMPControlBlock(pDev,
                                             &bmpCtrlBlk,
                                             &offset);
        if (rmStatus == RM_OK)
        {
            switch ( bmpCtrlBlk.CTL_Version )
            {
                // version 0 is for Nv3, no others exist
                case BMP_INIT_TABLE_VER_0:
                case BMP_INIT_TABLE_VER_1:
                {
                    // There is no I2C info in these BIOS's.
                    break;
                }
                    
                // BMP init table version 2 & 3
                case BMP_INIT_TABLE_VER_2:
                case BMP_INIT_TABLE_VER_3:
                {
                    //PBMP_Struc_Rev3 BMP_InitTableInfo = (PBMP_Struc_Rev3)pBmpCtrlBlk;
                    BIOSI2CTable biosI2CTable;
                    
                    rmStatus = BiosReadStructure(pDev,
                                                 (U008 *) &biosI2CTable,
                                                 0x40,
                                                 (U032 *) 0,
                                                 BMP_I2C_FMT);
                    if (rmStatus == RM_OK)
                    {
                        pDev->Dac.I2CPrimaryWriteIndex = biosI2CTable.I2CPort0IO & 0xFF;
                        pDev->Dac.I2CPrimaryStatusIndex = ( biosI2CTable.I2CPort0IO >> 8 ) & 0xFF;
                        pDev->Dac.I2CSecondaryWriteIndex = biosI2CTable.I2CPort1IO & 0xFF;;
                        pDev->Dac.I2CSecondaryStatusIndex = ( biosI2CTable.I2CPort1IO >> 8 ) & 0xFF;
                        pDev->Dac.CRTPortID = biosI2CTable.CRTI2CPort;
                        pDev->Dac.TVOutPortID = biosI2CTable.TVI2CPort;
                        pDev->Dac.DFPPortID = biosI2CTable.DFPI2CPort;
                    }
                    break;
                }
        
                // BMP init table version >= BMP_INIT_TABLE_VER_4 ( 4 )
                default:
                {
                    BMP_Struc_Rev4 bmp_InitTableInfo ;
                    
                    rmStatus = BiosReadStructure(pDev,
                                                 (U008 *) &bmp_InitTableInfo,
                                                 offset,
                                                 (U032 *) 0,
                                                 BMP_INIT4_FMT);

                    if (rmStatus == RM_OK)
                    {
                        pDev->Dac.I2CPrimaryWriteIndex = bmp_InitTableInfo.BMP_I2CPort0IO & 0xFF;
                        pDev->Dac.I2CPrimaryStatusIndex = ( bmp_InitTableInfo.BMP_I2CPort0IO >> 8 ) & 0xFF;
                        pDev->Dac.I2CSecondaryWriteIndex = bmp_InitTableInfo.BMP_I2CPort1IO & 0xFF;;
                        pDev->Dac.I2CSecondaryStatusIndex = ( bmp_InitTableInfo.BMP_I2CPort1IO >> 8 ) & 0xFF;
                        pDev->Dac.CRTPortID = bmp_InitTableInfo.BMP_CRTI2CPort;
                        pDev->Dac.TVOutPortID = bmp_InitTableInfo.BMP_TVI2CPort;
                        pDev->Dac.DFPPortID = bmp_InitTableInfo.BMP_DFPI2CPort;
                    }
                    break;
                }
            }
        }

        // Verify that there are no non-zero values in these reg indexes as these should never be 0       
        // This fixes a bug in which Diamond stuffs 0s into the bitmap
        if( (pDev->Dac.I2CPrimaryWriteIndex == 0x00)   ||
            (pDev->Dac.I2CPrimaryStatusIndex == 0x00)  ||
            (pDev->Dac.I2CSecondaryWriteIndex == 0x00) ||
            (pDev->Dac.I2CSecondaryStatusIndex == 0x00) ) {

            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Invalid I2C info in bios image\n");

            // Someone gave us bogus values, so set all of these to the defaults
            pDev->Dac.I2CPrimaryWriteIndex = NV_CIO_CRE_DDC_WR__INDEX;
            pDev->Dac.I2CPrimaryStatusIndex = NV_CIO_CRE_DDC_STATUS__INDEX;
            pDev->Dac.I2CSecondaryWriteIndex = NV_CIO_CRE_DDC0_WR__INDEX;
            pDev->Dac.I2CSecondaryStatusIndex = NV_CIO_CRE_DDC0_STATUS__INDEX;
            pDev->Dac.CRTPortID = NV_I2C_PORT_PRIMARY;
            pDev->Dac.TVOutPortID = NV_I2C_PORT_SECONDARY;
            pDev->Dac.DFPPortID = NV_I2C_PORT_SECONDARY;
        }

        if (OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, "Nv11DVIConnectorOnly", &RegistryData) == RM_OK) {
            pDev->Dac.CRTPortID = NV_I2C_PORT_SECONDARY;
            pDev->Dac.DFPPortID = NV_I2C_PORT_SECONDARY;

            pDev->Dac.TVOutPortID = NV_I2C_PORT_TERTIARY;
            pDev->Dac.CRT2PortID = NV_I2C_PORT_TERTIARY;
            pDev->Dac.DFP2PortID = NV_I2C_PORT_TERTIARY;
        }

        if (IsNV15(pDev))
            pDev->Dac.CRT2PortID = NV_I2C_PORT_SECONDARY;
    }
#endif

    return rmStatus;
}

//
// Return head number specified in alloc request.
//
U032 dacGetHead
(
    PCLASS pClass,
    VOID *pCreateParms
)
{
    RM_STATUS   status = RM_OK;
    U032 Head = 0;

    if (pCreateParms == NULL)
        return Head;

    switch (pClass->Type)
    {
        case NV15_VIDEO_LUT_CURSOR_DAC:
        {
            NV07C_ALLOCATION_PARAMETERS Class07cParams;
            status = osCopyIn(pCreateParms, (U008*) &Class07cParams, sizeof (NV07C_ALLOCATION_PARAMETERS));
            if (status != RM_OK)
                return 0;
            Head = Class07cParams.logicalHeadId;
        }
        default:
            // for everything else, it's head 0
            break;
    }

    return Head;
}

RM_STATUS
dacCreateObj(VOID* pDevHandle, PCLASSOBJECT ClassObject, U032 Name, POBJECT *Object, VOID *pCreateParms)
{
    PHWINFO pDev = (PHWINFO) pDevHandle;
    RM_STATUS                    status = RM_OK;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    PCLASS                       pClass = ClassObject->Base.ThisClass;
    U008                         *bytePtr;
    U032                          i, head;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: dacCreateObj\r\n");
    
    //
    // Check allocation restrictions.
    //

    //
    // Check head assignment.
    //
    head = dacGetHead(pClass, pCreateParms);
    if (head >= pDev->Dac.HalInfo.NumCrtcs)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: dacCreateObj: invalid logical head number ", head);
        return RM_ERR_INSUFFICIENT_RESOURCES;
    }

    //
    // For synchronized multi-head flips, init the sync counter
    //
    pDev->Dac.MultiSyncCounter = pDev->Dac.HalInfo.NumCrtcs;

    //
    // Alloc memory for class dependent object state.
    // All DAC engine objects currently use the same structure.
    //
    status = osAllocMem((VOID **)Object, sizeof (VIDEO_LUT_CURSOR_DAC_OBJECT));
    if (status)
        return (status);

    //
    // Nice casts to make the code more readable.
    //
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)*Object;

    //
    // Initialize the new object structure.
    //
    bytePtr = (U008 *)pVidLutCurDac;
    for (i = 0; i < sizeof (VIDEO_LUT_CURSOR_DAC_OBJECT); i++)
       *bytePtr++ = 0;

    //
    // Init common (software) state.
    //
    classInitCommonObject(&pVidLutCurDac->CBase, ClassObject, Name);

    //
    // Let the HAL setup any chip-dependent resources.
    //
    pVidLutCurDac->HalObject.Head = head;
    status = nvHalDacAlloc(pDev, pVidLutCurDac->CBase.ChID, pVidLutCurDac->CInstance, pClass->Type, (VOID *)&pVidLutCurDac->HalObject);

    if (status == RM_OK)
    {
        //
        // Build a FIFO/Hash context for this object
        //
        status = fifoUpdateObjectInstance(pDev,
                                          &pVidLutCurDac->CBase, 
                                          pVidLutCurDac->CBase.ChID, 
                                          pVidLutCurDac->CInstance);
    }

    if (status)
    {
        osFreeMem(*Object);
        return (status); 
    }

    //
    // Bump up reference count on this.
    //
    pDev->Dac.CrtcInfo[head].RefCount++;

    return (RM_OK);
}

RM_STATUS
dacDestroyObj(VOID* pDevHandle, POBJECT Object)
{
    PHWINFO pDev = (PHWINFO) pDevHandle;
    RM_STATUS               status = RM_OK;
    PCLASS                  pClass;
    U032                    buffNum;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;

#if !defined(NTRM) && !defined(__DJGPP__)
    U008    data08, lock;
#endif

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: dacDestroyObj\r\n");

    //
    // Just in case...
    //
    if (Object == NULL)
        return (RM_OK);

    //
    // Clear the subChannel ptrs for this object 
    //
	osEnterCriticalCode(pDev);
	fifoDeleteSubchannelContextEntry(pDev, Object);
	osExitCriticalCode(pDev);

    //
    // Nice cast to make the code more readable.
    //
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;

    //
    // Let the HAL do it's cleanup thing.
    //
    pClass = Object->ThisClass;
    nvHalDacFree(pDev, pVidLutCurDac->CBase.ChID, pVidLutCurDac->CInstance, pClass->Type, (VOID *)&pVidLutCurDac->HalObject);

    //
    // Delete the FIFO/Hash context for this object
    //
    status = fifoDeleteObjectInstance(pDev, &pVidLutCurDac->CBase, pVidLutCurDac->CBase.ChID); 
    if (status)
        return (status);

    //
    // Cleanup any pending VBlank callbacks
    //
    VBlankDeleteCallback(pDev, pVidLutCurDac->HalObject.Head, &(pVidLutCurDac->CursorPointCallback));

    for (buffNum = 0; buffNum < 2; buffNum++) {
        VBlankDeleteCallback(pDev, pVidLutCurDac->HalObject.Head, &(pVidLutCurDac->Image[buffNum].ImageCallback));
        VBlankDeleteCallback(pDev, pVidLutCurDac->HalObject.Head, &(pVidLutCurDac->LUT[buffNum].LUTCallback));
        VBlankDeleteCallback(pDev, pVidLutCurDac->HalObject.Head, &(pVidLutCurDac->CursorImage[buffNum].CursorImageCallback));
    }

    //
    // Decrement reference count.
    //
    // Turn off DAC if this is last object instance associated
    // with this (non-primary) head
    //
    if (--pDev->Dac.CrtcInfo[pVidLutCurDac->HalObject.Head].RefCount == 0)
    {
#ifndef __DJGPP__
        // Assumes last pVidLutCurDac is the modesetting object (which holds this flag)
        if (   pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].PrimaryDevice == FALSE
       //comment out following line to see multimon nv11-pci modeswitch problem
            && pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].pVidLutCurDac != NULL 
           )
        {
            dacDisableDac(pDev, pVidLutCurDac->HalObject.Head);
        }
#endif
    }

    //Remove the pointer to this object -- whether this ptr is NULL is used
    // to determine whether this head is disabled.
    if (pVidLutCurDac == (PVIDEO_LUT_CURSOR_DAC_OBJECT)(pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].pVidLutCurDac)) 
    {
#if !defined(NTRM) && !defined(__DJGPP__)
        //Runtime clone mode switch support -- turn off display when associated object it is destroyed.
        if (!pDev->Vga.Enabled) {
            dacDisableDac(pDev, pVidLutCurDac->HalObject.Head); 

            if (MC_POWER_LEVEL_0 == pDev->Power.State) {
              lock = UnlockCRTC(pDev, pVidLutCurDac->HalObject.Head);
                if (pDev->Power.MobileOperation &&  pDev->HotKeys.enabled) {
                    CRTC_RD(0x3B, data08, pVidLutCurDac->HalObject.Head);
                    CRTC_WR(0x3B, (data08 & 0x0F), pVidLutCurDac->HalObject.Head);
                }

                RestoreLock(pDev, pVidLutCurDac->HalObject.Head, lock);
            }
        }
#endif                                                          
        SETDISPLAYTYPE(pDev, DACGETHEAD(pVidLutCurDac), DISPLAY_TYPE_NONE);
        pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].pVidLutCurDac = NULL;
        pDev->Dac.HalInfo.CrtcInfo[DACGETHEAD(pVidLutCurDac)].pVidLutCurDac = NULL;
    }

    //
    // And finally delete the object itself    
    //    
    return (osFreeMem(Object));
}

//
// EnableHead
//
// Set DAC address space mode in CR44 (H2OWNS)
// register so that CIO/VIO/PRMVIO addressing 
// goes to the specified head.
//
VOID EnableHead
(
    PHWINFO pDev,
    U032    Head
)
{
    U008 data08;
    U008 crtc_index, lock;

    if (!IsNV11(pDev))
        return;

    if (pDev->Dac.HalInfo.Head2RegOwner == HEAD2_OWNER_UNKNOWN) {
        // Set the initial values, if this is the first call.
        if (Head == 1)
            data08 = 3;
        else
            data08 = 0;
    } else {
        // CR44 has a bug; memory mapped reads don't work, so we use a shadow copy.
        data08 = (U008)pDev->Dac.HalInfo.Head2RegOwner;
        if (Head == 1) 
        {
            // Did CR44 already point to head 1
            if ((data08 & 0x3) == 3)
                return;

            data08 |= 3;
        }
        else
        {
            // Did CR44 already point to head 0
            if ((data08 & 0x3) == 0)
                return;

            data08 &= ~3;
        }
    }

    // save/unlock head0 (new head is always programmed from head 0).
    crtc_index = DAC_REG_RD08(NV_PRMCIO_CRX__COLOR, 0);
    lock = UnlockCRTC(pDev, 0);

    CRTC_WR(NV_CIO_CRE_H2OWNS, data08, 0);

    RestoreLock(pDev, 0, lock);    // not pointing at CR44 anymore
    DAC_REG_WR08(NV_PRMCIO_CRX__COLOR, crtc_index, 0);

    pDev->Dac.HalInfo.Head2RegOwner = (U032)data08;
}


//returns the current owner of DDC (ENGINE_CTRL_I2C)
U032 CurrentHeadDDC(PHWINFO pDev)
{
    if (DAC_REG_RD_DRF(_PCRTC, _ENGINE_CTRL, _I2C, 0))
        return 0;   //head A wins a tie

    if (DAC_REG_RD_DRF(_PCRTC, _ENGINE_CTRL, _I2C, 1))
        return 1;

    return 0;   //head A wins by default
}


// Multi-head DDC switching (ENGINE_CTRL update routine).
VOID AssocDDC(PHWINFO pDev, U032 Head)
{
    if (Head == 0)
    {
        DAC_FLD_WR_DRF_DEF(_PCRTC, _ENGINE_CTRL, _I2C, _ENABLE, 0);
        DAC_FLD_WR_DRF_DEF(_PCRTC, _ENGINE_CTRL, _I2C, _DEFAULT, 1);
    }
    else
    {
        DAC_FLD_WR_DRF_DEF(_PCRTC, _ENGINE_CTRL, _I2C, _ENABLE, 1);
        DAC_FLD_WR_DRF_DEF(_PCRTC, _ENGINE_CTRL, _I2C, _DEFAULT, 0);
    }
}

// Multi-head TV switching (ENGINE_CTRL update routine).
VOID AssocTV(PHWINFO pDev, U032 Head)
{
    if (Head == 0)
    {
        DAC_FLD_WR_DRF_DEF(_PCRTC, _ENGINE_CTRL, _TV, _ENABLE, 0);
        DAC_FLD_WR_DRF_DEF(_PCRTC, _ENGINE_CTRL, _TV, _DEFAULT, 1);
    }
    else
    {
        DAC_FLD_WR_DRF_DEF(_PCRTC, _ENGINE_CTRL, _TV, _ENABLE, 1);
        DAC_FLD_WR_DRF_DEF(_PCRTC, _ENGINE_CTRL, _TV, _DEFAULT, 0);
    }
}

// Multi-head VideoScalar switching (ENGINE_CTRL update routine).
VOID AssocVideoScalar(PHWINFO pDev, U032 Head)
{
    if (Head == 0)
    {
        DAC_FLD_WR_DRF_DEF(_PCRTC, _ENGINE_CTRL, _VS, _ENABLE, 0);
        DAC_FLD_WR_DRF_DEF(_PCRTC, _ENGINE_CTRL, _VS, _DEFAULT, 1);
    }
    else
    {
        DAC_FLD_WR_DRF_DEF(_PCRTC, _ENGINE_CTRL, _VS, _ENABLE, 1);
        DAC_FLD_WR_DRF_DEF(_PCRTC, _ENGINE_CTRL, _VS, _DEFAULT, 0);
    }
}


//Set a flag to let the BIOS know not to handle hotkey events itself.
//Set while in Windows, cleared in full-screen DOS.  Mobile only.
VOID EnableMobileHotkeyHandling(PHWINFO pDev)
{
    U008 lock, data;

    if (pDev->Power.MobileOperation) {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Driver-based hotkey handling enabled\n\r");

        // unlock the extended regs
        lock = UnlockCRTC(pDev, 0);

        //Clear any pending events
        CRTC_RD(NV_CIO_CRE_SCRATCH5__INDEX, data, 0);
        data &= ~(NV_MOBILE_FLAG_SCALE_MASK | NV_MOBILE_FLAG_DISPLAY_MASK);
        CRTC_WR(NV_CIO_CRE_SCRATCH5__INDEX, data, 0);

        //Notify BIOS we're enabling driver hotkey handling.
        CRTC_RD(NV_CIO_CRE_SCRATCH6__INDEX, data, 0);
        CRTC_WR(NV_CIO_CRE_SCRATCH6__INDEX, data | NV_DRIVER_HANDLES_HOTKEYS, 0);
        //Temporary: Toshiba's system BIOS clears that bit accidentally.
        //So also use another bit.  Our VGA BIOS checks both.
        CRTC_RD(NV_CIO_CRE_SCRATCH5__INDEX, data, 0);
        CRTC_WR(NV_CIO_CRE_SCRATCH5__INDEX, data | NV_DRIVER_HANDLES_HOTKEYS, 0);

        RestoreLock(pDev, 0, lock);

        //Workaround: head 1's bit, too.
        lock = UnlockCRTC(pDev, 1);

        //Clear any pending events
        CRTC_RD(NV_CIO_CRE_SCRATCH5__INDEX, data, 1);
        data &= ~(NV_MOBILE_FLAG_SCALE_MASK | NV_MOBILE_FLAG_DISPLAY_MASK);
        CRTC_WR(NV_CIO_CRE_SCRATCH5__INDEX, data, 1);

        //Notify BIOS we're enabling driver hotkey handling.
        CRTC_RD(NV_CIO_CRE_SCRATCH6__INDEX, data, 1);
        CRTC_WR(NV_CIO_CRE_SCRATCH6__INDEX, data | NV_DRIVER_HANDLES_HOTKEYS, 1);
        //Temporary: Toshiba's system BIOS clears that bit accidentally.
        //So also use another bit.  Our VGA BIOS checks both.
        CRTC_RD(NV_CIO_CRE_SCRATCH5__INDEX, data, 1);
        CRTC_WR(NV_CIO_CRE_SCRATCH5__INDEX, data | NV_DRIVER_HANDLES_HOTKEYS, 1);

        RestoreLock(pDev, 1, lock);

        pDev->HotKeys.enabled = TRUE;
    }
}


//Clear flag so BIOS knows to handle hotkey events itself.
//Set while in Windows, cleared in full-screen DOS.  Mobile only.
VOID DisableMobileHotkeyHandling(PHWINFO pDev)
{
    U008 lock, data;

    if (pDev->Power.MobileOperation) {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Driver-based hotkey handling disabled\n\r");

        // unlock the extended regs
        lock = UnlockCRTC(pDev, 0);

        CRTC_RD(NV_CIO_CRE_SCRATCH6__INDEX, data, 0);
        CRTC_WR(NV_CIO_CRE_SCRATCH6__INDEX, data & ~NV_DRIVER_HANDLES_HOTKEYS, 0);
        //Temporary: Toshiba's system BIOS clears that bit accidentally.
        //So also use another bit.  Our VGA BIOS checks both.
        CRTC_RD(NV_CIO_CRE_SCRATCH5__INDEX, data, 0);
        CRTC_WR(NV_CIO_CRE_SCRATCH5__INDEX, data & ~NV_DRIVER_HANDLES_HOTKEYS, 0);

        RestoreLock(pDev, 0, lock);

        //Workaround: head 1's bit, too.
        // unlock the extended regs
        lock = UnlockCRTC(pDev, 1);

        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Driver-based hotkey handling disabled\n\r");
        CRTC_RD(NV_CIO_CRE_SCRATCH6__INDEX, data, 1);
        CRTC_WR(NV_CIO_CRE_SCRATCH6__INDEX, data & ~NV_DRIVER_HANDLES_HOTKEYS, 1);
        //Temporary: Toshiba's system BIOS clears that bit accidentally.
        //So also use another bit.  Our VGA BIOS checks both.
        CRTC_RD(NV_CIO_CRE_SCRATCH5__INDEX, data, 1);
        CRTC_WR(NV_CIO_CRE_SCRATCH5__INDEX, data & ~NV_DRIVER_HANDLES_HOTKEYS, 1);

        RestoreLock(pDev, 1, lock);

        pDev->HotKeys.enabled = FALSE;
    }
}

//---------------------------------------------------------------------------
//
//  VBlank service handler.
//
//---------------------------------------------------------------------------
V032 dacService
(
    PHWINFO pDev
)
{
    U032 head, pmc, pending;
    RM_STATUS status;

    pending = 0;

    pmc = REG_RD32(NV_PMC_INTR_0);
    if (pmc & DRF_DEF(_PMC, _INTR_0, _PCRTC, _PENDING))
        pending |= 0x1;
    if (pmc & DRF_DEF(_PMC, _INTR_0, _PCRTC2, _PENDING))
        pending |= 0x2;

    //
    // Check for any VBlank actions. Service count was already updated in VBlank
    //
    status = RM_OK;
    if (pDev->Framebuffer.FinishFlags)
        status |= grVBlank(pDev);
#if DEBUG
    if (status)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Problems in dacService.\n\r");
        DBG_BREAKPOINT();
    }
#endif // DEBUG

    VBlank(pDev);

    // Clear 'em all...
    for (head = 0; head < MAX_CRTCS; head++)
    {
        if (pending & BIT(head))
            DAC_REG_WR_DRF_DEF(_PCRTC, _INTR_0, _VBLANK, _RESET, head);
    }
    return (0x00000000);
}

// read/parse the Display Config Block (DCB) from the BIOS.  only revisions 1.4 and up support
// this feature
RM_STATUS dacParseDCBFromBIOS
(
    PHWINFO pDev
)
{
    U016        offset;             // offset from NV BIOS base
    U016        DCBBase;            // offset of DCB from BIOS base
    U032        CRTs = 0;           // # of CRT connectors
    U032        DDs = 0;            // # of DD connectors
    U032        TVs = 0;            // # of TV connectors
    U032        DevDescs = 0;       // # of valid DCB device descriptors
    U032        CRTCs = 0;          // # of CRTCs
    U032        I2cPorts;           // # of I2C ports
    U032        i;
    U032        j;
    U032        *pI2cPort;
    U032        RegistryData;
    U032        BMPOffset;
    BOOL        BMPVersionOK;
    RM_STATUS   status;
    BMP_Control_Block BMPCtrlBlk;
    
    // The BMP version must be greater than or equal to 0x05.0x12 for us to
    // use the DCB table
    
    status = DevinitGetBMPControlBlock(pDev, &BMPCtrlBlk, &BMPOffset);
   
    BMPVersionOK = ((status == RM_OK) &&
		    (( BMPCtrlBlk.CTL_Version >  BMP_INIT_TABLE_VER_5) ||
                     ((BMPCtrlBlk.CTL_Version == BMP_INIT_TABLE_VER_5) &&
                      (BMPCtrlBlk.CTL_Flags   >= BMP_INIT_TABLE_SUBVER_0x12))));

    DCBBase = BIOS_RD16(DCB_PTR_OFFSET);
    
    if ((BMPVersionOK) &&
        (DCBBase != 0) &&
        (BIOS_RD08(DCBBase + DCB_REVISION_OFFSET) >= DCB_FIRST_REVISION))
    {   
        // This is Rev 1.4 or greater, so there exits a DCB; however, many Rev 1.4 BIOS' have  
        // one DEV_REC (device descriptor) in the DCB, making them not very useful. 
        // If Rev 1.4 and there is only one DEV_REC, then fall back to the RM default DCB above.

        // read DCB device descriptors. endianess ?
        offset = DCBBase + DCB_DEV_DESC_OFFSET;
        for (i = 0; i < DCB_MAX_NUM_DEVDESCS; i++) 
        {
            pDev->Dac.DCBDesc[i].DCBDevRec.DevRec_U032 = BIOS_RD32(offset);
            if (pDev->Dac.DCBDesc[i].DCBRecType == DCB_DEV_REC_TYPE_EOL)
                break;
            offset += sizeof(U032);
            pDev->Dac.DCBDesc[i].DataPtr = BIOS_RD16(offset);
            offset += sizeof(U016);

            pDev->Dac.DCBDesc[i].DCBDevData.DataDword = BIOS_RD32(offset);
            offset += sizeof(U032);

            // keep track of the highest head number
            if (pDev->Dac.DCBDesc[i].DCBRecHead > CRTCs)
                CRTCs = pDev->Dac.DCBDesc[i].DCBRecHead;
        }
        DevDescs = i;   // # of device descriptors, excluding EOL
        CRTCs++;        // head number is zero based

        // We parsed the DCB, but if it's Rev 1.4 with a single entry,
        if ((DevDescs == 1) && (BIOS_RD08(DCBBase + DCB_REVISION_OFFSET) == DCB_FIRST_REVISION))
        {
            goto RM_DCB;
        }
        else
        {
            // read the I2C Configuration Block (ICB). endianess ?
            offset = BIOS_RD16(DCBBase + DCB_ICB_OFFSET);
            pI2cPort = (U032 *)pDev->Dac.DCBI2c;
            for (i = 0; i < DCB_MAX_NUM_I2C_RECORDS; i++) 
            {
                *pI2cPort++ = BIOS_RD32(offset);

                if (pDev->Dac.DCBI2c[i].I2cAccess == I2C_PORT_ACCESS_UNUSED)
                    break;

                offset += sizeof(U032);
            }
            I2cPorts = i;
        }
    }
    else
    {
RM_DCB:        
        // fake DCB records for older board
#define SETUP_DCB_DESC(i,a,b,c,d,e,f,g)                                     \
        pDev->Dac.DCBDesc[i].DCBRecType = DCB_DEV_REC_TYPE_##a;             \
        pDev->Dac.DCBDesc[i].DCBRecSubtype = DCB_DEV_REC_SUBTYPE_##b;       \
        pDev->Dac.DCBDesc[i].DCBRecFmt = DCB_DEV_REC_FMT_##c;               \
        pDev->Dac.DCBDesc[i].DCBRecIdxToICB = DCB_DEV_REC_LOGICAL_PORT_##d; \
        pDev->Dac.DCBDesc[i].DCBRecHead = DCB_DEV_REC_HEAD_##e;             \
        pDev->Dac.DCBDesc[i].DCBRecLocation = DCB_DEV_REC_LOCATION_##f;     \
        pDev->Dac.DCBDesc[i].DCBRecBus = DCB_DEV_REC_BUS_##g;

        if (IsNV11(pDev)) 
        {
            pDev->Dac.DCBI2c[0].I2cAccess = I2C_PORT_ACCESS_CRTC_INDEXED;
            pDev->Dac.DCBI2c[0].WritePort = I2C_PHYSICAL_PORT_A_DATA;
            pDev->Dac.DCBI2c[0].ReadPort = I2C_PHYSICAL_PORT_A_STATUS;

            pDev->Dac.DCBI2c[1].I2cAccess = I2C_PORT_ACCESS_CRTC_INDEXED;
            pDev->Dac.DCBI2c[1].WritePort = I2C_PHYSICAL_PORT_B_DATA;
            pDev->Dac.DCBI2c[1].ReadPort = I2C_PHYSICAL_PORT_B_STATUS;

            pDev->Dac.DCBI2c[2].I2cAccess = I2C_PORT_ACCESS_CRTC_INDEXED;
            pDev->Dac.DCBI2c[2].WritePort = I2C_PHYSICAL_PORT_C_DATA;
            pDev->Dac.DCBI2c[2].ReadPort = I2C_PHYSICAL_PORT_C_STATUS;

            pDev->Dac.DCBI2c[3].I2cAccess = I2C_PORT_ACCESS_UNUSED;
            I2cPorts = 3;

            if (pDev->Power.MobileOperation) 
            {
                //                TYPE      SUBTYPE FMT     I2C_IDX HEAD    LOCATION BUS
                SETUP_DCB_DESC(0, DD_SLINK, UNUSED, UNUSED, UNUSED, B,      UNUSED, 0); 
                SETUP_DCB_DESC(1, DD_SLINK, UNUSED, UNUSED, UNUSED, A,      UNUSED, 0); 
                SETUP_DCB_DESC(2, CRT,      UNUSED, UNUSED, 0,      A,      UNUSED, 1); 
                SETUP_DCB_DESC(3, CRT,      UNUSED, UNUSED, 0,      B,      UNUSED, 1); 
                if ((1 == pDev->Power.MobileOperation) || (pDev->Power.MobileOperation == 2)) 
                {     // no CRT/TV combo on toshiba
                    SETUP_DCB_DESC(4, TV,       UNUSED, UNUSED, 1,      A,      UNUSED, 1); 
                    SETUP_DCB_DESC(5, TV,       UNUSED, UNUSED, 1,      B,      UNUSED, 1); 
                }
                else 
                {
                    SETUP_DCB_DESC(4, TV,       UNUSED, UNUSED, 1,      A,      UNUSED, 2); 
                    SETUP_DCB_DESC(5, TV,       UNUSED, UNUSED, 1,      B,      UNUSED, 2); 
                }
                SETUP_DCB_DESC(6, EOL,      UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED); 
                DevDescs = 6;
                CRTCs = 2;
            }
            else 
            {
                if (OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, "Nv11DVIConnectorOnly", &RegistryData) == RM_OK) 
                {
                    //                TYPE      SUBTYPE FMT     I2C_IDX HEAD    LOCATION BUS
                    SETUP_DCB_DESC(0, CRT,      UNUSED, UNUSED, 1,      A,      UNUSED, 0); 
                    SETUP_DCB_DESC(1, DD_SLINK, UNUSED, UNUSED, 1,      A,      UNUSED, 0); 
                    SETUP_DCB_DESC(2, EOL,      UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED); 

                    DevDescs = 2;
                    CRTCs = 1;
                }
                else 
                {
                    //                TYPE      SUBTYPE FMT     I2C_IDX HEAD    LOCATION BUS
                    SETUP_DCB_DESC(0, CRT,      UNUSED, UNUSED, 0,      A,      UNUSED, 0); 
                    SETUP_DCB_DESC(1, DD_SLINK, UNUSED, UNUSED, 1,      A,      UNUSED, 1); 
                    SETUP_DCB_DESC(2, DD_SLINK, UNUSED, UNUSED, 1,      B,      UNUSED, 1); 
                    SETUP_DCB_DESC(3, CRT,      UNUSED, UNUSED, 1,      B,      UNUSED, 2); 
                    // Originally this was set up as A, B for 24 bit, B, A for 12 bit.
                    // The problem with that is that we don't want to use B as the primary on a desktop system.
                    // The desktop BIOS is not head aware; it will always assume head A, so DOS boxes won't work on B.
                    if (REG_RD_DRF(_PEXTDEV, _BOOT_0, _STRAP_FP_IFACE) == NV_PEXTDEV_BOOT_0_STRAP_FP_IFACE_24BIT) 
                    {
                        SETUP_DCB_DESC(4, TV,       UNUSED, UNUSED, 1,      A,      UNUSED, 1); 
                        SETUP_DCB_DESC(5, TV,       UNUSED, UNUSED, 1,      B,      UNUSED, 1); 
                    }
                    else 
                    {
                        SETUP_DCB_DESC(4, TV,       UNUSED, UNUSED, 1,      A,      UNUSED, 3); 
                        SETUP_DCB_DESC(5, TV,       UNUSED, UNUSED, 1,      B,      UNUSED, 3); 
                    }
                    SETUP_DCB_DESC(6, EOL,      UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED); 

                    DevDescs = 6;
                    CRTCs = 2;
                }
            }
        }
        else 
        {  // non NV11 boards
            pDev->Dac.DCBI2c[0].I2cAccess = I2C_PORT_ACCESS_CRTC_INDEXED;
            pDev->Dac.DCBI2c[0].WritePort = I2C_PHYSICAL_PORT_A_DATA;
            pDev->Dac.DCBI2c[0].ReadPort = I2C_PHYSICAL_PORT_A_STATUS;
            pDev->Dac.DCBI2c[1].I2cAccess = I2C_PORT_ACCESS_CRTC_INDEXED;
            pDev->Dac.DCBI2c[1].WritePort = I2C_PHYSICAL_PORT_B_DATA;
            pDev->Dac.DCBI2c[1].ReadPort = I2C_PHYSICAL_PORT_B_STATUS;
            pDev->Dac.DCBI2c[2].I2cAccess = I2C_PORT_ACCESS_UNUSED;
            I2cPorts = 2;

            //                TYPE      SUBTYPE FMT     I2C_IDX HEAD    LOCATION BUS
            SETUP_DCB_DESC(0, CRT,      UNUSED, UNUSED, 0,      A,      UNUSED, 0); 
            SETUP_DCB_DESC(1, DD_SLINK, UNUSED, UNUSED, 1,      A,      UNUSED, 0); 
            SETUP_DCB_DESC(2, TV,       UNUSED, UNUSED, 1,      A,      UNUSED, 0); 
            SETUP_DCB_DESC(3, EOL,      UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED); 
            DevDescs = 3;
            CRTCs = 1;
        }
    }


#ifdef MACOS
    //special failure case for NV11/NV20 on Mac - seeds are broken or not present
    //DDLLPP - remove when new seed properly describes hardware
    pDev->Dac.DCBI2c[0].I2cAccess = I2C_PORT_ACCESS_CRTC_INDEXED;
    pDev->Dac.DCBI2c[0].WritePort = I2C_PHYSICAL_PORT_A_DATA;
    pDev->Dac.DCBI2c[0].ReadPort = I2C_PHYSICAL_PORT_A_STATUS;

    pDev->Dac.DCBI2c[1].I2cAccess = I2C_PORT_ACCESS_CRTC_INDEXED;
    pDev->Dac.DCBI2c[1].WritePort = I2C_PHYSICAL_PORT_B_DATA;
    pDev->Dac.DCBI2c[1].ReadPort = I2C_PHYSICAL_PORT_B_STATUS;

    pDev->Dac.DCBI2c[2].I2cAccess = I2C_PORT_ACCESS_CRTC_INDEXED;
    pDev->Dac.DCBI2c[2].WritePort = I2C_PHYSICAL_PORT_C_DATA;
    pDev->Dac.DCBI2c[2].ReadPort = I2C_PHYSICAL_PORT_C_STATUS;

    pDev->Dac.DCBI2c[3].I2cAccess = I2C_PORT_ACCESS_UNUSED;
    I2cPorts = 3;
    
    if(IsNV20(pDev))
    {
        SETUP_DCB_DESC(0, CRT,      UNUSED, UNUSED, 0,      A,      UNUSED, 0);
        SETUP_DCB_DESC(1, CRT,      UNUSED, UNUSED, 1,      A,      UNUSED, 0);
        SETUP_DCB_DESC(2, DD_SLINK, UNUSED, UNUSED, 1,      A,      UNUSED, 0);
        SETUP_DCB_DESC(3, EOL,      UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, 0);   
        
        DevDescs = 3;
        CRTCs = 1;
    }    
    if(IsNV11(pDev))
    {   
        if (osReadRegistryBoolean(pDev, NULL, "backlight-control"))
        {

            //                TYPE      SUBTYPE FMT     I2C_IDX HEAD    LOCATION BUS
            SETUP_DCB_DESC(0, CRT,      UNUSED, UNUSED, UNUSED, A,      UNUSED, 0     ); 
            SETUP_DCB_DESC(1, DD_SLINK, UNUSED, UNUSED, 1,      B,      UNUSED, 0     );
            SETUP_DCB_DESC(2, EOL,      UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED);
            
            DevDescs = 2;
            CRTCs = 2;
        }
        else
        {
            SETUP_DCB_DESC(0, CRT,      UNUSED, UNUSED, 0,      A,      UNUSED, 0);
            SETUP_DCB_DESC(1, CRT,      UNUSED, UNUSED, 1,      A,      UNUSED, 0);
            SETUP_DCB_DESC(2, DD_SLINK, UNUSED, UNUSED, 1,      B,      UNUSED, 0);
            SETUP_DCB_DESC(3, EOL,      UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, 0);   
            
            DevDescs = 3;
            CRTCs = 2;
        }
    }
#endif    
    
    // override nvHalmcControl_nv10 setting
    DACHALINFO(pDev, NumCrtcs) = CRTCs;
    if (IsNV11(pDev) && OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, "TwinViewDisabled", &RegistryData) == RM_OK)
        DACHALINFO(pDev, NumCrtcs) = 1;

    // find all devices with no I2C port.  these are the built in devices.
    // currently, only the flat panel on laptop doesn't have an I2C port
    // associate with it.  assume at most one device of each type.
    for (i = 0; i < DevDescs; i++) 
    {
        if (pDev->Dac.DCBDesc[i].DCBRecIdxToICB == DCB_DEV_REC_LOGICAL_PORT_UNUSED) 
        {
            switch (pDev->Dac.DCBDesc[i].DCBRecType) {
            case DCB_DEV_REC_TYPE_CRT:
                pDev->Dac.DCBDesc[i].DevTypeUnit = 0x1 << DAC_CRT_OFFSET_IN_MAP;
                CRTs = 1;
                break;

            case DCB_DEV_REC_TYPE_TV:
                pDev->Dac.DCBDesc[i].DevTypeUnit = 0x1 << DAC_TV_OFFSET_IN_MAP;
                TVs = 1;
                break;

            case DCB_DEV_REC_TYPE_DD_SLINK:
            case DCB_DEV_REC_TYPE_DD_DLINK:
            case DCB_DEV_REC_TYPE_DD:
                pDev->Dac.DCBDesc[i].DevTypeUnit = 0x1 << DAC_DD_OFFSET_IN_MAP;
                DDs = 1;
                break;
            }
        }
    }

    // assign device unit number according to the I2C port number.  devices
    // of the same type connected to the same I2C port have the same unit
    // number since they use the same connnector.
    for (i = 0; i < I2cPorts; i++) 
    {
        int     CRTFound;
        int     TVFound;
        int     DDFound;

        CRTFound = TVFound = DDFound = 0;
        for (j = 0; j < DevDescs; j++) 
        {
            if (pDev->Dac.DCBDesc[j].DCBRecIdxToICB != i)
                continue;

            switch (pDev->Dac.DCBDesc[j].DCBRecType) 
            {
            case DCB_DEV_REC_TYPE_CRT:
                pDev->Dac.DCBDesc[j].DevTypeUnit = 0x1 << (DAC_CRT_OFFSET_IN_MAP + CRTs);
                CRTFound = 1;
                break;

            case DCB_DEV_REC_TYPE_TV:
                pDev->Dac.DCBDesc[j].DevTypeUnit = 0x1 << (DAC_TV_OFFSET_IN_MAP + TVs);
                TVFound = 1;
                break;

            case DCB_DEV_REC_TYPE_DD_SLINK:
            case DCB_DEV_REC_TYPE_DD_DLINK:
            case DCB_DEV_REC_TYPE_DD:
                pDev->Dac.DCBDesc[j].DevTypeUnit = 0x1 << (DAC_DD_OFFSET_IN_MAP + DDs);
                DDFound = 1;
                break;

            }
        }

        CRTs += CRTFound;
        TVs += TVFound;
        DDs += DDFound;
    }

    // create devices bitmap
    pDev->Dac.DevicesBitMap = (((0x1 << CRTs) - 1) << DAC_CRT_OFFSET_IN_MAP) |
        (((0x1 << TVs) - 1) << DAC_TV_OFFSET_IN_MAP) |
        (((0x1 << DDs) - 1) << DAC_DD_OFFSET_IN_MAP);

    if (OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, "DevicesConnected", &RegistryData) == RM_OK)
        pDev->Dac.DevicesNoCheck = RegistryData;
    else
        pDev->Dac.DevicesNoCheck = 0;

    return RM_OK;
}

// Find the connect status of the specified devices.
// *pDevBitMap - on input, a map of devices to check the connect status of
//  at ouput, a map of the queried devices that are actually present
// DevForcePresentMap - a similarly formatted map that indicates devices
//  that should always be considered present, whether they are attached or not
VOID dacDevicesConnectStatus
(
    PHWINFO pDev,
    U032    *pDevBitMap,
    U032    DevForcePresentMap
)
{
    U032            dev = *pDevBitMap;
    PDACDCBDEVDESC  pDevDesc;
    U032            CurrDevTypeUnit = 0x1;
    BOOL            status;

    using_new_api = TRUE;
    while (dev) {
        if (dev & 0x1) {
            if (CurrDevTypeUnit & DevForcePresentMap)
                goto NextDev;

            // go through all entries in the DCB to find matching devices.
            pDevDesc = pDev->Dac.DCBDesc;
            while (pDevDesc->DCBRecType != DCB_DEV_REC_TYPE_EOL) {
                status = FALSE;
                if  (pDevDesc->DevTypeUnit == CurrDevTypeUnit) {
                    if (pDevDesc->DCBRecIdxToICB == DCB_DEV_REC_LOGICAL_PORT_UNUSED)
                        goto NextDev;

                    switch (pDevDesc->DCBRecType)  {
                    case DCB_DEV_REC_TYPE_CRT:
                        status = dacMonitorConnectStatus(pDev, pDevDesc->DCBRecHead);
                        break;

                    case DCB_DEV_REC_TYPE_TV:
                        status = dacTVConnectStatus(pDev, pDevDesc->DCBRecHead);
                        break;

                    case DCB_DEV_REC_TYPE_DD_SLINK:
                    case DCB_DEV_REC_TYPE_DD_DLINK:
                    case DCB_DEV_REC_TYPE_DD:
                        status = dacFlatPanelConnectStatus(pDev, pDevDesc->DCBRecHead);
                        break;
                    }

                    if (status == FALSE)
                        // clear the corresponding bit if no devices found.
                        *pDevBitMap &= ~CurrDevTypeUnit;

                    goto NextDev;
                }
                pDevDesc++;
            }
        }

NextDev:
        dev >>= 1;
        CurrDevTypeUnit <<= 1;
    }
    using_new_api = FALSE;
}

// this routine will try to find a configuration for the specified
// devices. return FALSE if a suitable configuration cannot be found.
// the configuration is returned in pAllocationMap with the first nibble
// corresponds to the first/rightmost device, second nibble corresponds
// to the second device, ... up to 8 devices can be specified. the nibble
// value is the head that should be connected to the device.
static BOOL dacMapDeviceToHead
(
    PHWINFO     pDev,
    U032        DevBitMap,          // devices in configuration
    U032        HeadsUsed,          // assigned heads
    U032        BusesUsed,          // assigned buses
    U032        CurrDev,            // current device
    U032        NibbleToUse,        // nibble to use for the next allocation
    U032        *pAllocationMap     // allocation map, result that will be passed
                                    // back to the calling routine
)
{
    PDACDCBDEVDESC  pDevDesc;

    // done, no more device
    if (!DevBitMap)
        return TRUE;

    // look for the rightmost set bit
    while (!(DevBitMap & 0x1)) {
        DevBitMap >>= 1;
        CurrDev++;
    }

    // use recursion to map device to head, one at a time.
    pDevDesc = pDev->Dac.DCBDesc;
    while (pDevDesc->DCBRecType != DCB_DEV_REC_TYPE_EOL) {
        if (pDevDesc->DevTypeUnit != ((U032)0x1 << CurrDev))
            goto NextDevDesc;

        // make sure we are not using resources assigned already.
        if (((0x1 << pDevDesc->DCBRecHead) & HeadsUsed) ||
            ((0x1 << pDevDesc->DCBRecBus) & BusesUsed))
            goto NextDevDesc;

        if (dacMapDeviceToHead(pDev,
                               DevBitMap >> 1,
                               HeadsUsed | (0x1 << pDevDesc->DCBRecHead),
                               BusesUsed | (0x1 << pDevDesc->DCBRecBus),
                               CurrDev + 1,
                               NibbleToUse + 1,
                               pAllocationMap) == TRUE) {
            *pAllocationMap |= pDevDesc->DCBRecHead << (NibbleToUse * 4);
            return TRUE;
        }

NextDevDesc:
        pDevDesc++;
    }

    return FALSE;
}

// find a suitable configuration for the specified devices.
RM_STATUS dacFindDevicesConfiguration
(
    PHWINFO pDev,
    U032    NewDevBitMap,
    U032    *pNewAllocationMap,
    U032    OldDevBitMap,
    U032    OldAllocationMap
)
{
    U032        DevBitMap;
    U032        CurrDev;
    U032        NibbleToUse;
    U032        HeadsUsed = 0x0;     // heads used
    U032        BusesUsed = 0x0;     // buses used
    U032        head;
    PDACDCBDEVDESC pDevDesc;
    U032        AdjustedOldAllocationMap = 0x0;
    U032        AdjustedNibbleToUse;
    U032        NewConfig;

    // find common devices between the old and new configurations. keep them
    // attaching to the same CRTCs if possible.
    DevBitMap = OldDevBitMap;
    CurrDev = NibbleToUse = AdjustedNibbleToUse = 0;
    while (DevBitMap) {
        if (!(DevBitMap & 0x1))
            goto NextDev_0;

        // device in the old but not in the new configuration ?
        if (!(OldDevBitMap & NewDevBitMap & (0x1 << CurrDev))) {
            NibbleToUse++;
            goto NextDev_0;
        }

        // device is in both the old and new configurations.
        // find CRTCs attached to the device and mark it as used.
        // adjust old allocation map such that we can merge it with the new
        // allocation map later.
        head = (OldAllocationMap >> (NibbleToUse++ * 4)) & 0xf;
        HeadsUsed |= 0x1 << head;
        AdjustedOldAllocationMap |= head << (AdjustedNibbleToUse++ * 4);

        // determine other non-shared resources used by this device.
        pDevDesc = pDev->Dac.DCBDesc;
        while (pDevDesc->DCBRecType != DCB_DEV_REC_TYPE_EOL) {
            if ((pDevDesc->DevTypeUnit != ((U032)0x1 << CurrDev)) ||
                (pDevDesc->DCBRecHead != head))
                pDevDesc++;
            else {
                BusesUsed |= 0x1 << pDevDesc->DCBRecBus;
                break;
            }
        }

NextDev_0:
        DevBitMap >>= 1;
        CurrDev++;
    }

    // find new allocation with common devices at fixed CRTCs.
    NewConfig = 0x0;
    if (dacMapDeviceToHead(pDev,
                           (NewDevBitMap ^ OldDevBitMap) & NewDevBitMap,
                           HeadsUsed,
                           BusesUsed,
                           0,
                           0,
                           &NewConfig) == TRUE) {

        // no merging necessary if there are no common devices.
        if (!(OldDevBitMap & NewDevBitMap)) {
            *pNewAllocationMap = NewConfig;
            return RM_OK;
        }

        // merge configuration.
        DevBitMap = NewDevBitMap;
        *pNewAllocationMap = 0x0;
        CurrDev = NibbleToUse = 0;
        while (DevBitMap) {
            if (!(DevBitMap & 0x1))
                goto NextDev_1;

            if ((0x1 << CurrDev) & OldDevBitMap) {
                *pNewAllocationMap |= (AdjustedOldAllocationMap & 0xf) << (NibbleToUse++ * 4);
                AdjustedOldAllocationMap >>= 4;
            }
            else {
                *pNewAllocationMap |= (NewConfig & 0xf) << (NibbleToUse++ * 4);
                NewConfig >>= 4;
            }

NextDev_1:
            DevBitMap >>= 1;
            CurrDev++;
        }

        return RM_OK;
    }

    // couldn't find allocation with common devices at fixed CRTCs.
    // try to get allocation with no CRTCs fixed.
    if (OldDevBitMap & NewDevBitMap) {
        *pNewAllocationMap = 0x0;
        if (dacMapDeviceToHead(pDev,
                               NewDevBitMap,
                               0x0,
                               0x0,
                               0,
                               0,
                               pNewAllocationMap) == TRUE)
            return RM_OK;
    }

    return RM_ERROR;
}

//
// This is a more complete way of implementating a modeset within the RM from
// VIDEO_LUT_CURSOR_DAC values. It's more than just calling dacSetModeMulti
// since other VIDEO_LUT_CURSOR_DAC methods also occur during a complete modeset.
//
RM_STATUS dacSetModeFromObjectInfo
(
    PHWINFO pDev,
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac
)
{
    // Do the initial basic modeset (uses a default startAddr and palette)
    dacSetModeMulti(pDev, pVidLutCurDac);

    // Program the correct CRTC startAddr/pitch (with panning offset).
    if (pVidLutCurDac->Image[0].ImageCtxDma)
    {
        U032 startAddr;
        startAddr = (U032)
                    ((size_t)(pVidLutCurDac->Image[0].ImageCtxDma->DescAddr) +
                     pVidLutCurDac->Image[0].Offset);
        startAddr += pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].CurrentPanOffset;

        dacProgramVideoStart(pDev,
                             DACGETHEAD(pVidLutCurDac),
                             startAddr,
                             pVidLutCurDac->Image[0].Pitch);
    }

    // Program the user-defined LUT
    if (pVidLutCurDac->LUT[0].LUTCtxDma)
    {
        RM_STATUS status;
        U032 numEntries = 256;
        U032 *pPaletteAddr;
        
        status = dmaGetMappedAddress(pDev,
                                     pVidLutCurDac->LUT[0].LUTCtxDma,
                                     pVidLutCurDac->LUT[0].Offset,
                                     numEntries * 4,
                                     (void **)(&pPaletteAddr));
        if (status == RM_OK)
            dacProgramLUT(pDev, DACGETHEAD(pVidLutCurDac), pPaletteAddr, numEntries);
    }

    return RM_OK;
}

static RM_STATUS GetCrcValuesCallback
(
    PHWINFO   pDev,
    POBJECT   pObject,
    U032      Head,
    U032      Channel,
    RM_STATUS Status
)
{
    switch (GETDISPLAYTYPE(pDev, Head))
    {
        case DISPLAY_TYPE_MONITOR:
            DAC_REG_WR32
            (
                NV_PRAMDAC_TEST_CONTROL,
                  DRF_DEF(_PRAMDAC, _TEST_CONTROL, _CRC_RESET,   _DEASSERTED)
                | DRF_DEF(_PRAMDAC, _TEST_CONTROL, _CRC_ENABLE,  _ASSERTED)
                | DRF_NUM(_PRAMDAC, _TEST_CONTROL, _CRC_CHANNEL, Channel)
                | DRF_DEF(_PRAMDAC, _TEST_CONTROL, _CRC_CAPTURE, _ONE),
                Head
            );
            break;

        case DISPLAY_TYPE_FLAT_PANEL:
            DAC_REG_WR32
            (
                NV_PRAMDAC_FP_TEST_CONTROL,
                  DRF_DEF(_PRAMDAC, _FP_TEST_CONTROL, _CRC_RESET,   _DEASSERTED)
                | DRF_DEF(_PRAMDAC, _FP_TEST_CONTROL, _CRC_ENABLE,  _ASSERTED)
                | DRF_NUM(_PRAMDAC, _FP_TEST_CONTROL, _CRC_CHANNEL, Channel)
                | DRF_DEF(_PRAMDAC, _FP_TEST_CONTROL, _CRC_CAPTURE, _ONE),
                Head
            );
            break;

        case DISPLAY_TYPE_TV:
            DAC_REG_WR32
            (
                NV_PRAMDAC_TV_TEST_CONTROL,
                  DRF_DEF(_PRAMDAC, _TV_TEST_CONTROL, _CRC_RESET,   _DEASSERTED)
                | DRF_DEF(_PRAMDAC, _TV_TEST_CONTROL, _CRC_ENABLE,  _ASSERTED)
                | DRF_NUM(_PRAMDAC, _TV_TEST_CONTROL, _CRC_CHANNEL, Channel)
                | DRF_DEF(_PRAMDAC, _TV_TEST_CONTROL, _CRC_CAPTURE, _ONE),
                Head
            );
            break;

        default:
            RM_ASSERT(0);
            return RM_ERROR;
    }

    return RM_OK;
}

RM_STATUS  dacGetCrcValues
(
    PHWINFO pDev,
    U032    Head,
    U032 *  pRedCrc,
    U032 *  pGreenCrc,
    U032 *  pBlueCrc
)
{
    VBLANKCALLBACK VBlankCallback;
    U032 Channel;
    U032 Crcs[3];
    U032 i;
    U032 Try;
    U032 DisplayType  = GETDISPLAYTYPE(pDev, Head);
    U032 MaxPollLoops = 100;
    U032 Retries      =  10;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: dacGetCrcValues:\n");

    VBlankCallback.Proc        = GetCrcValuesCallback;
    VBlankCallback.Object      = 0;
    VBlankCallback.Param1      = Head;
    VBlankCallback.Param2      = 0;     // Used to pass Channel to callback.
    VBlankCallback.VBlankCount = 0;
    VBlankCallback.Flags       = 0;
    VBlankCallback.Status      = RM_OK;
    VBlankCallback.Next        = 0;

    *pRedCrc   = 0;
    *pGreenCrc = 0;
    *pBlueCrc  = 0;

    for (Channel = 0; Channel < 3; ++Channel)
    {
        switch (DisplayType)
        {
            case DISPLAY_TYPE_MONITOR:
                for (Try = 0; Try < Retries; ++Try)
                {
                    // Reset CRC.
                    DAC_REG_WR32(NV_PRAMDAC_TEST_CONTROL,
                          DRF_DEF(_PRAMDAC, _TEST_CONTROL, _CRC_RESET,   _ASSERTED)
                        | DRF_DEF(_PRAMDAC, _TEST_CONTROL, _CRC_ENABLE,  _ASSERTED)
                        | DRF_DEF(_PRAMDAC, _TEST_CONTROL, _CRC_CAPTURE, _ONE),
                        Head);

                    // Capture the CRC.
                    VBlankCallback.Param2 = Channel;
                    VBlankAddCallback(pDev, Head, &VBlankCallback);
                    for (i = 0; (DAC_REG_RD_DRF(_PRAMDAC, _CHECKSUM, _STATUS, Head)
                                    != NV_PRAMDAC_CHECKSUM_STATUS_CAPTURED)
                                &&  (i < MaxPollLoops); ++i)
                    {
                        osDelay(1);
                    }
                    if (i < MaxPollLoops)
                    {
                        Crcs[Channel] = DAC_REG_RD_DRF(_PRAMDAC, _CHECKSUM, _VALUE, Head); 
                        break;
                    }
                }
                if (Try == Retries)
                {
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: failed to capture DAC CRC\n");
                    return FALSE;
                }

                break;
    
            case DISPLAY_TYPE_FLAT_PANEL:
                for (Try = 0; Try < Retries; ++Try)
                {
                    // Reset CRC.
                    DAC_REG_WR32(NV_PRAMDAC_FP_TEST_CONTROL,
                          DRF_DEF(_PRAMDAC, _FP_TEST_CONTROL, _CRC_RESET,   _ASSERTED)
                        | DRF_DEF(_PRAMDAC, _FP_TEST_CONTROL, _CRC_ENABLE,  _ASSERTED)
                        | DRF_DEF(_PRAMDAC, _FP_TEST_CONTROL, _CRC_CAPTURE, _ONE),
                        Head);

                    // Capture the CRC.
                    VBlankCallback.Param2 = Channel;
                    VBlankAddCallback(pDev, Head, &VBlankCallback);
                    for (i = 0; (DAC_REG_RD_DRF(_PRAMDAC, _FP_CHECKSUM, _STATUS, Head)
                                    != NV_PRAMDAC_FP_CHECKSUM_STATUS_CAPTURED)
                                &&  (i < MaxPollLoops); ++i)
                    {
                        osDelay(1);
                    }
                    if (i < MaxPollLoops)
                    {
                        Crcs[Channel] = DAC_REG_RD_DRF(_PRAMDAC, _FP_CHECKSUM, _VAL, Head); 
                        break;
                    }
                }
                if (Try == Retries)
                {
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: failed to capture FP CRC\n");
                    return FALSE;
                }

                break;
    
            case DISPLAY_TYPE_TV:
                for (Try = 0; Try < Retries; ++Try)
                {
                    // Reset CRC.
                    DAC_REG_WR32(NV_PRAMDAC_TV_TEST_CONTROL,
                          DRF_DEF(_PRAMDAC, _TV_TEST_CONTROL, _CRC_RESET,   _ASSERTED)
                        | DRF_DEF(_PRAMDAC, _TV_TEST_CONTROL, _CRC_ENABLE,  _ASSERTED)
                        | DRF_DEF(_PRAMDAC, _TV_TEST_CONTROL, _CRC_CAPTURE, _ONE),
                        Head);

                    // Capture the CRC.
                    VBlankCallback.Param2 = Channel;
                    VBlankAddCallback(pDev, Head, &VBlankCallback);
                    for (i = 0; (DAC_REG_RD_DRF(_PRAMDAC, _TV_CHECKSUM, _STATUS, Head)
                                    != NV_PRAMDAC_TV_CHECKSUM_STATUS_CAPTURED)
                                &&  (i < MaxPollLoops); ++i)
                    {
                        osDelay(1);
                    }
                    if (i < MaxPollLoops)
                    {
                        Crcs[Channel] = DAC_REG_RD_DRF(_PRAMDAC, _TV_CHECKSUM, _VAL, Head); 
                        break;
                    }
                }
                if (Try == Retries)
                {
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: failed to capture TV CRC\n");
                    return FALSE;
                }

                break;
    
            default:
                RM_ASSERT(0);
                return RM_ERROR;
        }

    } // for each channel

    *pRedCrc   = Crcs[2];
    *pGreenCrc = Crcs[1];
    *pBlueCrc  = Crcs[0];

    return RM_OK;
}

RM_STATUS dacSetHotPlugInterrupts(PHWINFO pDev, U032 Head, U032 HotPlugEnable, U032 HotUnplugEnable)
{
    U032 HotPlugEnableProxy,HotUnplugEnableProxy;
    
    //do the transfer safely since technically we should be passing pointers to writeable memory
    HotPlugEnableProxy = HotPlugEnable;
    HotUnplugEnableProxy = HotUnplugEnable;

    //do the set    
    return nvHalDacHotPlugInterrupts(
        pDev,
        (VOID *)&(((PVIDEO_LUT_CURSOR_DAC_OBJECT)(pDev->Dac.CrtcInfo[Head].pVidLutCurDac))->HalObject),
        &HotPlugEnableProxy,
        &HotUnplugEnableProxy,
        TRUE /* write mode */,
        TRUE /* reset */
    );
}

RM_STATUS dacGetHotPlugInterrupts(PHWINFO pDev, U032 Head, U032 * pHotPlugStatus, U032 * pHotUnplugStatus, BOOL Reset)
{
    return nvHalDacHotPlugInterrupts(
        pDev,
        (VOID *)&(((PVIDEO_LUT_CURSOR_DAC_OBJECT)(pDev->Dac.CrtcInfo[Head].pVidLutCurDac))->HalObject),
        pHotPlugStatus,
        pHotUnplugStatus,
        FALSE /* read mode */,
        Reset
    );
}
