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

#define RED_OUT_300MV   0x800001A0
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
    U032  hiVclk1, hiVclk2;
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
    hiVclk1 = 250000;
    hiVclk2 = 340000;
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
        CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock, Head);
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, Head);

        // put NV15 into NV15 VPLL VCO programming mode
        CRTC_RD(NV_CIO_CRE_LCD__INDEX, cr33, Head);
        cr33 |= (1 << DRF_SHIFT(NV_CIO_CRE_LCD_VPLL_VCO));
        CRTC_WR(NV_CIO_CRE_LCD__INDEX, cr33, Head);

        // restore lock, if previously locked
        if (lock == 0)
            CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, Head);
    }

    //
    // Determine the current crystal frequency
    //   
    if (pDev->Chip.HalInfo.CrystalFreq == 14318180)
    {
        // 14.3Khz
        crystalFreq = 14318;

        lowM = 1;
             
        if (vclk > hiVclk2)
            highM = 2;
        else if (vclk > hiVclk1)
            highM = 6;
        else
            highM = 14;
    }
    else
    {
        // 13.5Khz
        crystalFreq = 13500;

        lowM = 1;

        if (vclk > hiVclk2)
            highM = 2;
        else if (vclk > hiVclk1)
            highM = 6;
        else
            highM = 13;
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
                        if (((vclk/lwvs) > 200) || (lwvs == 0)) // zero error; rare, but would cause exception
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
RM_STATUS dacProgramMClk
(
    PHWINFO pDev
)
{
    U032 mNew;
    U032 nNew;
    U032 pNew;
    U032 Head = 0;  // MPLL is on head 0

    // Compute M, N, O, and P values
    dacCalcMNP( pDev, Head, pDev->Dac.HalInfo.MClk / 10000, &mNew, &nNew, &pNew );

    // Update Dac structure
    pDev->Dac.HalInfo.MPllM = mNew;
    pDev->Dac.HalInfo.MPllN = nNew;
    pDev->Dac.HalInfo.MPllO = 1;
    pDev->Dac.HalInfo.MPllP = pNew;

    return(nvHalDacProgramMClk(pDev));
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
    
    // Unlock CRTC extended regs
    lock = ReadCRTCLock(pDev, Head);
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, Head);
    
    CRTC_RD(NV_CIO_CRE_HCUR_ADDR1_INDEX, cr31, Head);           
    CRTC_WR(NV_CIO_CRE_HCUR_ADDR1_INDEX, (cr31 | (1 << DRF_SHIFT(NV_CIO_CRE_HCUR_ADDR1_ENABLE))), Head);

    // Relock CRTC extended regs
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, lock, Head);

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
    
    // Unlock CRTC extended regs
    lock = ReadCRTCLock(pDev, Head);
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, Head);
    
    CRTC_RD(NV_CIO_CRE_HCUR_ADDR1_INDEX, cr31, Head);           
    CRTC_WR(NV_CIO_CRE_HCUR_ADDR1_INDEX, (cr31 & ~(1 << DRF_SHIFT(NV_CIO_CRE_HCUR_ADDR1_ENABLE))), Head);

    // Relock CRTC extended regs
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, lock, Head);
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
    
    // Unlock CRTC extended regs
    lock = ReadCRTCLock(pDev, Head);
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, Head);
    
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

    CRTC_WR(NV_CIO_SR_LOCK_INDEX, lock, Head);
        
    return;
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

    // Unlock CRTC extended regs
    lock = ReadCRTCLock(pDev, Head);
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, Head);

    dacDisableCursor(pDev, Head);

    CRTC_WR(NV_CIO_SR_LOCK_INDEX, lock, Head);
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
    
    if (!IsNV11(pDev) && (Head == 1)) // if not dual-head
    {
        return;
    }
    
    // unlock CRTC extended regs
    CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock, Head);
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, Head);

    // enable vblank interrupt
    DAC_REG_WR32(NV_PCRTC_INTR_EN_0, 1, Head);
    pDev->Dac.CrtcInfo[Head].StateFlags |= DAC_CRTCINFO_VBLANK_ENABLED;
    
    switch (GETDISPLAYTYPE(pDev, Head))
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
                    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, _FPCLK, Head);     // FPCLK off only;leave TMDS going for secondary crtc
                else
                    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, _BOTH, Head);      // FPCLK/TMDS both off
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
                DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, _NONE, Head);
            }
            
            break;   
                                       
        case DISPLAY_TYPE_FLAT_PANEL:
        
            // restore sync states
            DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_TG_CONTROL, _VSYNC, pDev->Dac.CrtcInfo[Head].CurrentVsyncState, Head);
            DAC_FLD_WR_DRF_NUM(_PRAMDAC, _FP_TG_CONTROL, _HSYNC, pDev->Dac.CrtcInfo[Head].CurrentHsyncState, Head);
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, _NONE, Head);

            if (pDev->Power.MobileOperation)
    	   		dacPowerOnMobilePanel(pDev,Head);
            break;                              
            
        case DISPLAY_TYPE_TV:
            dacEnableTV(pDev, Head);
            break;
            
        default:
            break;
    }

    // relock extended regs
    if (lock == 0)
    {
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, Head);
    }

} // end of dacEnableDac()

VOID dacDisableDac
(
    PHWINFO pDev,
    U032 Head
)
{
    if (!IsNV11(pDev) && (Head == 1)) // if not dual-head
    {
        return;
    }

    // disable vblank interrupt
    DAC_REG_WR_DRF_DEF(_PCRTC, _INTR_0, _VBLANK, _RESET, Head);
    DAC_REG_WR32(NV_PCRTC_INTR_EN_0, 0, Head);
    pDev->Dac.CrtcInfo[Head].StateFlags &= ~DAC_CRTCINFO_VBLANK_ENABLED;

    // device-specific disabling...
    switch (GETDISPLAYTYPE(pDev, Head))
    {
        case DISPLAY_TYPE_MONITOR:
        
            // TO DO: this decision must be made by the HAL
            // head 0 -- treat as CRT
            if (Head == 0)
            {
                // disable sync
                dacSetSyncState(pDev, Head, 0, 0);
                //
                // BIOS WORKAROUND
                //
                // For now the Mobile BIOS does not reenable the dac power when performing
                // a modeset, therefore after we disable the power upon windows exit,
                // the CRT remains blank (even after warm restart).  Until the BIOS is
                // updated to account for this, I need to make sure we don't power
                // off the dac.
                //
                // The only issue with not doing the powerdown is with monitors that
                // don't abide by the blank-on-disabled-syncs rule, but that exposure
                // is small.
                //
                // Only do this for mobile
                //
                if (!pDev->Power.MobileOperation) {
                    dacDisableImage(pDev, Head);
                    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _OFF, Head);
                }
            }
            
            // head 1 -- treat as DFP
            else
            {    
                // disable sync
                DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _VSYNC, _DISABLE, Head);
                DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _HSYNC, _DISABLE, Head);
                DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, _BOTH, Head);
            }
            break;
            
        case DISPLAY_TYPE_FLAT_PANEL:
        
            // disable sync
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _VSYNC, _DISABLE, Head);
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _HSYNC, _DISABLE, Head);
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_DEBUG_0, _PWRDOWN, _BOTH, Head);

            // if we're mobile, turn the power off as well
            if (pDev->Power.MobileOperation)
                dacPowerOffMobilePanel(pDev, Head);
            
            break;
            
        case DISPLAY_TYPE_TV:
        
            // this has to be encoder dependent
            dacDisableTV(pDev, Head);
            break;
            
        default:
            break;
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

            // Unlock CRTC extended regs
            lock = ReadCRTCLock(pDev, Head);
            CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, Head);

            // Write the alternative pitch to the CRTC Offset Register
            // The value written to CR13/CR19 is the number of 8-byte chunks
            CRTC_WR(NV_CIO_CR_OFFSET_INDEX, (Pitch >> 3) & 0xFF, Head);

            // Modify the bits in the extended register as well
            CRTC_RD(NV_CIO_CRE_RPC0_INDEX, i, Head);
            i &= 0x1F;
            val32 = ((Pitch >> 3) & 0x700) >> 3;
            i |= (U008)val32; 
            CRTC_WR(NV_CIO_CRE_RPC0_INDEX, i, Head);

            // Relock extended regs
            CRTC_WR(NV_CIO_SR_LOCK_INDEX, lock, Head);

            // Restore index
            if (REG_RD08(NV_PRMVIO_MISC__READ) & 0x01)
                REG_WR08(NV_PRMCIO_CRX__COLOR, crtc_index); // restore crtc index
            else
                REG_WR08(NV_PRMCIO_CRX__MONO, crtc_index);  // restore crtc index

        }
//#endif

        // Program the new start address; this will latch at the end of blank
#ifdef  STEREO_SUPPORT
        if (pDev->pStereoParams)
        {
            pDev->pStereoParams->FlipOffsets[3][0] = StartAddr;
            if (pDev->pStereoParams->Flags & STEREOCFG_STEREOACTIVATED)
            {
                return RM_OK;
            }
        }
#endif  //STEREO_SUPPORT
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
    //
    // Note that this routine should only be called for NV15, the only
    // chip with this support.  Since no other chips will have this
    // feature (past or future), no need to overload a chip func.
    //
    if (IsNV15(pDev) || IsNV11(pDev) || IsNV20(pDev))
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
U032    Vde, Vt;
U008    Cr01, Cr07, Cr11, Cr21, Cr25, val;
U008    lock;
U008    UpdatedCr11;

    lock = ReadCRTCLock(pDev, Head);
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, Head);

    CRTC_RD(NV_CIO_CR_VRE_INDEX, Cr11, Head);
    val = Cr11 & 0x7f;                          // clear write protection bit
    CRTC_WR(NV_CIO_CR_VRE_INDEX, val, Head);

    CRTC_RD(0x21, Cr21, Head);
    CRTC_WR(0x21, 0xf8, Head);       // disable shadowing for TV
    
    CRTC_RD(NV_CIO_CR_HDE_INDEX, Cr01, Head);     // read H display end

    // Check that the sync width doesn't go to 0 and that the sync pulse is beyond display end
    if ((Hre <= (Hrs & 0x1f)) || (Hrs <= Cr01))
    {
        return RM_ERROR;
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
    if ((Vre <= Vrs) || (Vrs <= Vde) || (Vre >= Vt))
    {
        return RM_ERROR;
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

    CRTC_WR(NV_CIO_SR_LOCK_INDEX, lock, Head);

    return RM_OK;
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
    strDisplayMode[strIndex] = NULL;

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
    strDisplayMode[i] = NULL;

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
    strDisplayMode[i] = NULL;

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
    strDisplayMode[i] = NULL;

    return osReadRegistryBinary(pDev, pDev->Registry.DBstrDevNodeDisplayNumber, strDisplayMode, Params, numBytes);
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
        // Unlock extended registers
        lock = ReadCRTCLock(pDev, Head);
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, Head);
        
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
        
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, lock, Head);
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
        
        for (strptr = strDisplayMode; *strptr != NULL; strptr++) 
            ;
        *strptr++ = '_';
        *strptr++ = (char)(Head | 0x30);
        *strptr++ = 'T';                    // make sure to use different key than desktop position
        *strptr   = NULL;
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
        
        // Unlock extended registers
        lock = ReadCRTCLock(pDev, Head);
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, Head);

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
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, lock, Head);

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
        for (strptr = strDisplayMode; *strptr != NULL; strptr++) 
            ;
        *strptr++ = '_';
        *strptr++ = (char)(Head | 0x30);
        *strptr++ = 'T';                    // make sure to use different key than desktop position
        *strptr   = NULL;
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
    U008    rc;
    U032    displayType;
    U008    lock;               

    lock = ReadCRTCLock(pDev, Head);
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, Head);

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

    CRTC_WR(NV_CIO_SR_LOCK_INDEX, lock, Head);
    return displayType;
}

//  Read analog out comparator 
//  If the output is loaded, it will be below threshold
//  If no CRT is attached, it will trip
BOOL dacDetectTrigger(
    PHWINFO pDev
)
{
    U008 blank, data8;
    U032 retry;

#define MAXRETRIES 500000
    
    // wait for active
    retry = 0;
    do {
        blank = REG_RD08(NV_PRMCIO_INP0__COLOR) & 0x1;
        retry++;
    } while ((retry < MAXRETRIES) && blank);
    if (retry >= MAXRETRIES)
        goto timedout;

    // wait for blank
    retry = 0;
    do {
        blank = REG_RD08(NV_PRMCIO_INP0__COLOR) & 0x1;
        retry++;
    } while ((retry < MAXRETRIES) && !blank);
    if (retry >= MAXRETRIES)
        goto timedout;

    // wait for active
    retry = 0;
    do {
        blank = REG_RD08(NV_PRMCIO_INP0__COLOR) & 0x1;
        retry++;
    } while ((retry < MAXRETRIES) && blank);
    if (retry >= MAXRETRIES)
        goto timedout;

    // get 2 samples which are the same to debounce
    retry = 0;
    do {
        data8 = (REG_RD08(NV_PRMCIO_INP0) & 0x10);
        retry++;
    } while ((retry < MAXRETRIES) && ((REG_RD08(NV_PRMCIO_INP0) & 0x10) != data8));
    if (retry >= MAXRETRIES)
    {
 timedout:
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: trigger detect timed out\n");
        return FALSE;
    }

    // sense bit in NV_PRMCIO_INP0 is inverted (0x1 = no trigger).
    if (data8)
        return FALSE;    // no trigger
    else
        return TRUE;     // trigger
}



// Check whether monitor is currently connected
BOOL dacMonitorConnectStatus
(
    PHWINFO pDev,
    U032    Head
)
{
    RM_STATUS status;
    U008    lock;
    U032    Current;

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
    // Attempt to read EDID and check for analog display. On head 1 there is no way to do the analog out test below.
        lock = ReadCRTCLock(pDev, Head);    // Unlock the registers

        CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, Head);

        // read EDID 
        status = EDIDRead(pDev, Head, DISPLAY_TYPE_MONITOR);        // Try to read EDID

        CRTC_WR(NV_CIO_SR_LOCK_INDEX, lock, Head);

        if (status == RM_OK)        
        {
            if (edidIsAnalogDisplay(pDev, Head))
                return TRUE;
            else    
                return FALSE;
        }
        else
            return FALSE;   // the analog test (below) will not work on head 1--it always returns TRUE.
    }
//#endif

    status = EDIDDetect(pDev, Head, DISPLAY_TYPE_MONITOR);      // Try to read EDID
    if (status) 
    {
        if (Head != 1) // Head 1 does not have an internal DAC, so we cannot do the analog check.
        {
            // EDID did not confirm that a CRT is connected, but it may be an older non-DDC CRT,
            // so procede with analog out test.
            return (dacVGAConnectStatus(pDev, Head));
        }
        return FALSE;       // EDID read failed on Head1, so no CRT
    }

    return TRUE;
}

#ifdef MACOS
// XXX Enable old style on the Mac
//     This one can timeout 5 times in dacDetectTrigger(), the new one 
//     can time out about 255 times!

#define USE_TESTPOINT_DATA
#endif

BOOL dacVGAConnectStatus
(
    PHWINFO pDev, 
    U032    Head
)
{
    U032	dacPowerDown;
    U016	Sr1;
    U008	retries;
    BOOL    result = 0;
#ifndef USE_TESTPOINT_DATA
    U032 bgr, cur_palette[256], red_palette[256];
    U032 i, red_trigger, dacControl;
#endif

    // This test drives a selected amount of current out the DAC Red
    // output. This currect causes a voltage drop over the terminating
    // resistor (no CRT connected) or the terminating resistor in parallel
    // with the CRT load.  There is a voltage comparator which is sampled at
    // port 3C2. (Actually 3, on for each color.)  This indicates whether the
    // voltage at the output resistor has crossed a reference level.  If a CRT
    // is connected, the amount of current required to reach this level will
    // be twice (this can vary depending on the board) that required if there
    // is not a CRT.  (e.g. 75 ohms on the board, in parallel with 75 ohms at
    // the monitor = 37.5 ohms).  At the selected current level, we check to
    // see whether the comparator has flipped.  If so, there must be no CRT
    // connected (75 ohm load). If not, there is additional loading, i.e.  a
    // CRT is connected.

#ifdef USE_TESTPOINT_DATA
    // This shortened test replaces the traditional method of ramping up the
    // palette. We set the DAC into a test mode which bypasses the palette,
    // and set one level of analog out which we know will differentiate
    // between CRT and no CRT.
#endif
    
    Sr1 = ReadIndexed(NV_PRMVIO_SRX, 0x01);      
    if (Sr1 & 0x2000)
        WriteIndexed(NV_PRMVIO_SRX, (U016)(Sr1 & ~0x2000)); // turn off blank

    // make sure the DAC is driving RGB out
    dacPowerDown = REG_RD32(NV_PRAMDAC_TEST_CONTROL);
    FLD_WR_DRF_DEF( _PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _ON);

#ifdef USE_TESTPOINT_DATA
    // set to test mode--bypass palette
    FLD_WR_DRF_DEF( _PRAMDAC, _TEST_CONTROL, _TP_INS_EN, _ASSERTED);
    // set to level corresponding to roughly 300 mv
    REG_WR32(NV_PRAMDAC_TESTPOINT_DATA, RED_OUT_300MV);
#endif

    tmrDelay(pDev, 10000000);	// delay 10 ms to allow DAC to power up
    
#ifdef USE_TESTPOINT_DATA
    // The function dacDetectTrigger debounces the samples.
    // We will call dacDetectTrigger multiple times to insure
    // stability on a longer time interval as well.
    retries = 0;
    result = dacDetectTrigger(pDev);
    while (retries < MAXCRTRETRIES) 
    {
        BOOL result2;	

        result2 = dacDetectTrigger(pDev);
        if (result == result2) 
            break;
        result = result2;
        retries++;
    }
    FLD_WR_DRF_DEF( _PRAMDAC, _TEST_CONTROL, _TP_INS_EN, _DEASSERTED); // test mode off

    if (GETDISPLAYTYPE(pDev, Head) != DISPLAY_TYPE_MONITOR)
        DAC_FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _OFF, Head);   // CRT off

    if (retries == MAXCRTRETRIES)
        result = TRUE;  // if the test is not working, assume CRT is present        
    
    return result;
#else
    //
    // Here's the "ramping up the palette" method for detecting if a CRT is
    // present, reimplemented from what the BIOS had. The USE_TESTPOINT_DATA
    // version seemed not very reliable and frequently gave false CRT
    // detections. 
    // 
    // The palette version in testing gave no false detections for many test
    // loops and did correctly detect the monitor when it was present. The
    // red_trigger value used was tested on NV5/NV10/NV15 and NV11.
    //

    // save the current palette 
    DAC_REG_WR08(NV_USER_DAC_READ_MODE_ADDRESS, 0, Head);
    for (i = 0; i < 256; i++)
    {
        bgr = DAC_REG_RD08(NV_USER_DAC_PALETTE_DATA, Head);     // b
        bgr <<= 8;
        bgr |= DAC_REG_RD08(NV_USER_DAC_PALETTE_DATA, Head);    // g
        bgr <<= 8;
        bgr |= DAC_REG_RD08(NV_USER_DAC_PALETTE_DATA, Head);    // r
        cur_palette[i] = bgr;
    }

    // these red values rely on being interpreted as 6 bits per color
    dacControl = DAC_REG_RD32(NV_PRAMDAC_GENERAL_CONTROL, Head);
    DAC_FLD_WR_DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _BPC, _6BITS, Head);

    for (red_trigger = 0x8; red_trigger < 0x3F; red_trigger++)
    {
        // Load the red palette with the new values
        for (i = 0; i < 256; i++)
            red_palette[i] = (0x8 << 16) | (0x8 << 8) | red_trigger;

        dacProgramLUT(pDev, Head, red_palette, 256);

        for (retries = 0; retries < MAXCRTRETRIES; retries++) {
            result = dacDetectTrigger(pDev);
            if (result == dacDetectTrigger(pDev))
                break;
        }

        if (retries == MAXCRTRETRIES)
            return TRUE;    // no consistent read (assume something's connected)

        if (result == TRUE)
            break;          // triggered, so check the value
    }

    // restore the previous dac control
    DAC_REG_WR32(NV_PRAMDAC_GENERAL_CONTROL, dacControl, Head);

    // restore the previous palette
    dacProgramLUT(pDev, Head, cur_palette, 256);

    if (GETDISPLAYTYPE(pDev, Head) != DISPLAY_TYPE_MONITOR)
        DAC_FLD_WR_DRF_DEF(_PRAMDAC, _TEST_CONTROL, _PWRDWN_DAC, _OFF, Head);	// CRT off

    if (red_trigger < 0x18)
        return FALSE;
    else
        return TRUE;
#endif
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
    
        // if the other head is already a DFP, we can't do it (P38).
        //
        // TODO: Need to change this to a dynamic check based on feedback from the BIOS topology table
        //
        // Check dac object for the other head to see if the driver is using the flat panel. Previous check in HAL for 
        // display type is not correct because the boot device may be flat panel, but this should not limit us from changing
        // to a new configuration when the driver loads.
        if (pDev->Dac.DFP2PortID == NV_I2C_PORT_SECONDARY)   // is P38? 
        {
#if 0
            PVIDEO_LUT_CURSOR_DAC_OBJECT AltDacObj = (PVIDEO_LUT_CURSOR_DAC_OBJECT) pDev->Dac.CrtcInfo[Head^1].pVidLutCurDac;
            if (AltDacObj != NULL) 
            {
                if (AltDacObj->DisplayType == DISPLAY_TYPE_FLAT_PANEL)
        //        if (GETDISPLAYTYPE(pDev, Head^1) == DISPLAY_TYPE_FLAT_PANEL)
                    return FALSE;
            }
#endif    
            if ((pDev->Dac.DesktopState == NV_DAC_DESKTOP_STATE_MULTI) && (Head == 0))  
                            // In multi head mode, limit DFP to head 1 only, since we can't know what the driver is going to do
                            // i.e., this gets called for both heads before the driver does a modeset on either head,
                            // so the driver could attempt to use DFP on both.
            {
                return FALSE;
            }
        }
    }

    lock = UnlockCRTC(pDev, Head);

	// read EDID 
    status = EDIDRead(pDev, Head, DISPLAY_TYPE_FLAT_PANEL);     // Try to read EDID

    RestoreLock(pDev, Head, lock);

    if (status != RM_OK)
        return FALSE;

	if (edidIsDigitalDisplay(pDev, Head)) 				// Check the display technology byte
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

    // Get info from EDID
	if (!dacFlatPanelConnectStatus(pDev, Head))	// FP connected?
	{
		*FpConnected = NV_CFGEX_GET_FLATPANEL_INFO_NOT_CONNECTED;
		return RM_OK;						// then no more to do
	}
	if ((status = fpParseEDID(pDev, Head)) != RM_OK) // parse EDID to get timing info
        return status;
    
    *FpConnected = NV_CFGEX_GET_FLATPANEL_INFO_CONNECTED;
    *FpMaxX = pDev->Dac.fpHMax;
    *FpMaxY = pDev->Dac.fpVMax;
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
        mode = dacGetFlatPanelConfig(pDev, Head);
        *FpMode = mode;
        status = RM_OK;
    }

    return status;
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
                dacDisableTV(pDev, Head);
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

            // If no head is using TV, blank image.
            for (i = 0; i < pDev->Dac.HalInfo.NumCrtcs; i++)
                if (GETDISPLAYTYPE(pDev, i) == DISPLAY_TYPE_TV)
                    break;
            if (i == pDev->Dac.HalInfo.NumCrtcs)
                dacDisableTV(pDev, Head);
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

    if(IsNV10orBetter(pDev))
    {
        lock = ReadCRTCLock(pDev, Head);
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, Head);
        CRTC_RD(0x35, data, Head);
        data &= ~0x02;      // bit 1 = output enable (active low) 
        if (state == 0)
        {
            // OFF
            CRTC_WR(0x35, data & ~0x01, Head);  // bit 0 = fan control (active high)
        }
        else
        {
            // ON
            CRTC_WR(0x35, data | 0x01, Head);       // bit 0 = fan control (active high)
        }
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, lock, Head);
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

    if (IsNV11(pDev) == FALSE)
        return CONFIG_GETEX_BAD_READ;    // not supported pre-NV11

    // unlock extended regs
    CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock, Head);
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, Head);

    // get the color boost value
    CRTC_RD(NV_CIO_CRE_CSB, *boostValue, Head);

    // restore extended regs lock state
    if (lock == 0) {
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, Head);
    } else if (lock == 1) {
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RO_VALUE, Head);
    }
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

    if (IsNV11(pDev) == FALSE)
        return CONFIG_GETEX_BAD_READ;    // not support pre-NV11

    if (boostValue > NV_CFGEX_PREDAC_COLOR_SATURATION_BOOST_750)
        return CONFIG_SETEX_BAD_PARAM;    // 75% is the largest value

    // unlock extended regs
    CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock, Head);
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, Head);

    // set the color saturation boost value
    CRTC_WR(NV_CIO_CRE_CSB, boostValue, Head);

    // restore extended regs lock state
    if (lock == 0) {
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, Head);
    } else if (lock == 1) {
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RO_VALUE, Head);
    }
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
    U032 head, i2cport, tmdslink, displaytype;
    char primary_displaytype_string[16];
    U032 reglen;

    displayA = ~0;
    displayB = ~0;

    osReadRegistryDword(pDev, (char *) 0, "displayA", &displayA);
    osReadRegistryDword(pDev, (char *) 0, "displayB", &displayB);
    if ((displayA == ~0) && (displayB == ~0))
        return RM_ERROR;

    // what is 

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
        i2cport =     (displayA >> 16) & 0xFF;
        tmdslink =    (displayA >> 8)  & 0xFF;
        displaytype = (displayA >> 0)  & 0xFF;

        if (i2cport == 0xff)
            i2cport = 0x36;
        pDev->Dac.I2CPrimaryStatusIndex = i2cport;
        pDev->Dac.I2CPrimaryWriteIndex = i2cport + 1;
    }
    
    if ((displayB & 0x00FFFFFF) != 0x00FFFFFF)
    {
        head =        (displayB >> 24) & 0xFF;
        i2cport =     (displayB >> 16) & 0xFF;
        tmdslink =    (displayB >> 8)  & 0xFF;
        displaytype = (displayB >> 0)  & 0xFF;

        pDev->Dac.I2CSecondaryStatusIndex = i2cport;
        pDev->Dac.I2CSecondaryWriteIndex = i2cport + 1;
    }

    // is CRT on primary or secondary
    pDev->Dac.CRTPortID  = NV_I2C_PORT_PRIMARY;
    pDev->Dac.DFPPortID  = NV_I2C_PORT_PRIMARY;
    pDev->Dac.CRT2PortID = NV_I2C_PORT_SECONDARY;
    pDev->Dac.DFP2PortID = NV_I2C_PORT_SECONDARY;
 
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

        // This device has two DDC ports, set defaults

        pDev->Dac.I2CPrimaryWriteIndex = NV_CIO_CRE_DDC_WR__INDEX;
        pDev->Dac.I2CPrimaryStatusIndex = NV_CIO_CRE_DDC_STATUS__INDEX;
        pDev->Dac.I2CSecondaryWriteIndex = NV_CIO_CRE_DDC0_WR__INDEX;
        pDev->Dac.I2CSecondaryStatusIndex = NV_CIO_CRE_DDC0_STATUS__INDEX;
        pDev->Dac.I2CTertiaryWriteIndex = NV_CIO_CRE_DDC2_WR__INDEX;
        pDev->Dac.I2CTertiaryStatusIndex = NV_CIO_CRE_DDC2_STATUS__INDEX;
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
    // If we're in a multimon mode, and this is the first dac object
    // to be instantiated for a given head, be sure to swing the
    // VGA space over the *other* head.  By doing this, we can
    // be sure that any VGA accesses done between the time the
    // first and second heads are accessible will safely go out
    // to the second head.  This won't always necessarily be what
    // we want (e.g. blue screens), but it's better than having
    // Windows start routing VGA accesses to the primary head (while
    // it's got a desktop going) in response to detection of
    // the second head.
    //
    if (pDev->Dac.DesktopState == NV_DAC_DESKTOP_STATE_MULTI)
    {
        if (pDev->Dac.CrtcInfo[head].RefCount == 0)
            EnableHead(pDev, head^1);
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

#if !defined(NTRM)
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
        if (pVidLutCurDac->HalObject.Head != 0)
            dacDisableDac(pDev, pVidLutCurDac->HalObject.Head);
    }

    //Remove the pointer to this object -- whether this ptr is NULL is used
    // to determine whether this head is disabled.
    if (pVidLutCurDac == (PVIDEO_LUT_CURSOR_DAC_OBJECT)(pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].pVidLutCurDac)) 
    {
#if !defined(NTRM)
        //Runtime clone mode switch support -- turn off display when associated object it is destroyed.
        if (!pDev->Vga.Enabled) {
            dacDisableDac(pDev, pVidLutCurDac->HalObject.Head); 

            //Also, if mobile BIOS, let the BIOS know we've done this.
            lock = ReadCRTCLock(pDev, pVidLutCurDac->HalObject.Head);           //Back up lock state
            CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, pVidLutCurDac->HalObject.Head);    //Unlock CRTC extended regs
            if (pDev->Power.MobileOperation &&  pDev->HotKeys.enabled) {
                CRTC_RD(0x3B, data08, pVidLutCurDac->HalObject.Head);
                CRTC_WR(0x3B, (data08 & 0x0F), pVidLutCurDac->HalObject.Head);
            }

            CRTC_WR(NV_CIO_SR_LOCK_INDEX, lock, pVidLutCurDac->HalObject.Head); // Restore lock state
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
    U008 data08, temp;

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

    //We've determined experimentally that leaving 3d4 pointing at CR44
    // gives rise to many complications.  So we do a dummy access here
    // to avoid that problem.
    CRTC_WR(NV_CIO_CRE_H2OWNS, data08, 0);
    CRTC_RD(NV_CIO_SR_LOCK_INDEX, temp, 0);
    pDev->Dac.HalInfo.Head2RegOwner = (U032)data08;
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

        lock = ReadCRTCLock(pDev, 0);           //Back up lock state
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, 0);    //Unlock CRTC extended regs

        //Clear any pending events
        CRTC_RD(NV_CIO_CRE_SCRATCH5__INDEX, data, 0);
        data &= ~(NV_MOBILE_FLAG_SCALE_MASK | NV_MOBILE_FLAG_DISPLAY_MASK);
        CRTC_WR(NV_CIO_CRE_SCRATCH5__INDEX, data, 0);

        //Notify BIOS we're enabling driver hotkey handling.
        CRTC_RD(NV_CIO_CRE_SCRATCH6__INDEX, data, 0);
        CRTC_WR(NV_CIO_CRE_SCRATCH6__INDEX, data | NV_DRIVER_HANDLES_HOTKEYS, 0);

        CRTC_WR(NV_CIO_SR_LOCK_INDEX, lock, 0); // Restore lock state

        //Workaround: head 1's bit, too.
        lock = ReadCRTCLock(pDev, 1);           //Back up lock state
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, 1);    //Unlock CRTC extended regs

        //Clear any pending events
        CRTC_RD(NV_CIO_CRE_SCRATCH5__INDEX, data, 1);
        data &= ~(NV_MOBILE_FLAG_SCALE_MASK | NV_MOBILE_FLAG_DISPLAY_MASK);
        CRTC_WR(NV_CIO_CRE_SCRATCH5__INDEX, data, 1);

        //Notify BIOS we're enabling driver hotkey handling.
        CRTC_RD(NV_CIO_CRE_SCRATCH6__INDEX, data, 1);
        CRTC_WR(NV_CIO_CRE_SCRATCH6__INDEX, data | NV_DRIVER_HANDLES_HOTKEYS, 1);

        CRTC_WR(NV_CIO_SR_LOCK_INDEX, lock, 1); // Restore lock state

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

        lock = ReadCRTCLock(pDev, 0);           //Back up lock state
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, 0);    //Unlock CRTC extended regs

        CRTC_RD(NV_CIO_CRE_SCRATCH6__INDEX, data, 0);
        CRTC_WR(NV_CIO_CRE_SCRATCH6__INDEX, data & ~NV_DRIVER_HANDLES_HOTKEYS, 0);

        CRTC_WR(NV_CIO_SR_LOCK_INDEX, lock, 0); // Restore lock state

        //Workaround: head 1's bit, too.
        lock = ReadCRTCLock(pDev, 1);           //Back up lock state
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, 1);    //Unlock CRTC extended regs

        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Driver-based hotkey handling disabled\n\r");
        CRTC_RD(NV_CIO_CRE_SCRATCH6__INDEX, data, 1);
        CRTC_WR(NV_CIO_CRE_SCRATCH6__INDEX, data & ~NV_DRIVER_HANDLES_HOTKEYS, 1);

        CRTC_WR(NV_CIO_SR_LOCK_INDEX, lock, 1); // Restore lock state

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

// read/parse the Display Config Block (DCB) from the BIOS.  only revisions 1.2 and up support
// this feature
RM_STATUS dacParseDCBFromBIOS
(
    PHWINFO pDev
)
{
    extern char strDevNodeRM[];
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

    DCBBase = BIOS_RD16(DCB_PTR_OFFSET);
    //if (BIOS_RD08(DCBBase + DCB_REVISION_OFFSET) < DCB_FIRST_REVISION) {
    if (1) {
        // fake DCB records for older board
#define SETUP_DCB_DESC(i,a,b,c,d,e,f,g)                                     \
        pDev->Dac.DCBDesc[i].DCBRecType = DCB_DEV_REC_TYPE_##a;             \
        pDev->Dac.DCBDesc[i].DCBRecSubtype = DCB_DEV_REC_SUBTYPE_##b;       \
        pDev->Dac.DCBDesc[i].DCBRecFmt = DCB_DEV_REC_FMT_##c;               \
        pDev->Dac.DCBDesc[i].DCBRecIdxToICB = DCB_DEV_REC_LOGICAL_PORT_##d; \
        pDev->Dac.DCBDesc[i].DCBRecHead = DCB_DEV_REC_HEAD_##e;             \
        pDev->Dac.DCBDesc[i].DCBRecLocation = DCB_DEV_REC_LOCATION_##f;     \
        pDev->Dac.DCBDesc[i].DCBRecBus = DCB_DEV_REC_BUS_##g;

        if (IsNV11(pDev)) {
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

            if (pDev->Power.MobileOperation) {
                //                TYPE      SUBTYPE FMT     I2C_IDX HEAD    LOCATION BUS
                SETUP_DCB_DESC(0, DD_SLINK, UNUSED, UNUSED, UNUSED, B,      UNUSED, 0); 
                SETUP_DCB_DESC(1, DD_SLINK, UNUSED, UNUSED, UNUSED, A,      UNUSED, 0); 
                SETUP_DCB_DESC(2, CRT,      UNUSED, UNUSED, 0,      A,      UNUSED, 1); 
                SETUP_DCB_DESC(3, CRT,      UNUSED, UNUSED, 0,      B,      UNUSED, 1); 
                SETUP_DCB_DESC(4, TV,       UNUSED, UNUSED, 1,      A,      UNUSED, 2); 
                SETUP_DCB_DESC(5, TV,       UNUSED, UNUSED, 1,      B,      UNUSED, 2); 
                SETUP_DCB_DESC(6, EOL,      UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED); 
                DevDescs = 6;
                CRTCs = 2;
            }
            else {
                //                TYPE      SUBTYPE FMT     I2C_IDX HEAD    LOCATION BUS
                SETUP_DCB_DESC(0, CRT,      UNUSED, UNUSED, 0,      A,      UNUSED, 0); 
                SETUP_DCB_DESC(1, DD_SLINK, UNUSED, UNUSED, 1,      A,      UNUSED, 1); 
                SETUP_DCB_DESC(2, DD_SLINK, UNUSED, UNUSED, 1,      B,      UNUSED, 1); 
                SETUP_DCB_DESC(3, CRT,      UNUSED, UNUSED, 1,      B,      UNUSED, 2); 
                SETUP_DCB_DESC(4, TV,       UNUSED, UNUSED, 1,      A,      UNUSED, 1); 
                SETUP_DCB_DESC(5, TV,       UNUSED, UNUSED, 1,      B,      UNUSED, 1); 
                SETUP_DCB_DESC(6, EOL,      UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED); 
                DevDescs = 6;
                CRTCs = 2;
            }
        }
        else {  // non NV11 boards
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
    else {
        // read DCB device descriptors. endianess ?
        offset = DCBBase + DCB_DEV_DESC_OFFSET;
        for (i = 0; i < DCB_MAX_NUM_DEVDESCS; i++) {
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

        // read the I2C Configuration Block (ICB). endianess ?
        offset = BIOS_RD16(DCBBase + DCB_ICB_OFFSET);
        pI2cPort = (U032 *)pDev->Dac.DCBI2c;
        for (i = 0; i < DCB_MAX_NUM_I2C_RECORDS; i++) {
            *pI2cPort++ = BIOS_RD32(offset);
    
            if (pDev->Dac.DCBI2c[i].I2cAccess == I2C_PORT_ACCESS_UNUSED)
                break;

            offset += sizeof(U032);
        }
        I2cPorts = i;
    }
    
    // override nvHalmcControl_nv10 setting
    DACHALINFO(pDev, NumCrtcs) = CRTCs;

    // find all devices with no I2C port.  these are the built in devices.
    // currently, only the flat panel on laptop doesn't have an I2C port
    // associate with it.  assume at most one device of each type.
    for (i = 0; i < DevDescs; i++) {
        if (pDev->Dac.DCBDesc[i].DCBRecIdxToICB == DCB_DEV_REC_LOGICAL_PORT_UNUSED) {
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
                pDev->Dac.DCBDesc[i].DevTypeUnit = 0x1 << DAC_DD_OFFSET_IN_MAP;
                DDs = 1;
                break;
            }
        }
    }

    // assign device unit number according to the I2C port number.  devices
    // of the same type connected to the same I2C port have the same unit
    // number since they use the same connnector.
    for (i = 0; i < I2cPorts; i++) {
        int     CRTFound;
        int     TVFound;
        int     DDFound;

        CRTFound = TVFound = DDFound = 0;
        for (j = 0; j < DevDescs; j++) {
            if (pDev->Dac.DCBDesc[j].DCBRecIdxToICB != i)
                continue;

            switch (pDev->Dac.DCBDesc[j].DCBRecType) {
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

    if (OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, "DevicesConnected", &pDev->Dac.DevicesNoCheck) != RM_OK)
        pDev->Dac.DevicesNoCheck = 0x0;

    return RM_OK;
}

// find the connect status of the specified devices.  a set bit
// in the bit map mean the calling routine want to know the connect
// status of the corresponding device.  this routine will keep a set
// bit set if the corresponding device is connected, clear a set bit
// if the corresponding device is not connected.
VOID dacDevicesConnectStatus
(
    PHWINFO pDev,
    U032    *pDevBitMap
)
{
    U032            dev = *pDevBitMap;
    PDACDCBDEVDESC  pDevDesc;
    U032            CurrDevTypeUnit = 0x1;
    BOOL            status;

    using_new_api = TRUE;
    while (dev) {
        if (dev & 0x1) {
            if (CurrDevTypeUnit & pDev->Dac.DevicesNoCheck)
                goto NextDev;

            // go through all entries in the DCB to find matching devices.
            pDevDesc = pDev->Dac.DCBDesc;
            while (pDevDesc->DCBRecType != DCB_DEV_REC_TYPE_EOL) {
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
