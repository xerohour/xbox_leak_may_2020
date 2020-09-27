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

/******************************* MC Manager ********************************\
*                                                                           *
* Module: MC.C                                                              *
*   All global exceptions are dispatched through this module.               *
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
#include <fifo.h>
#include <fb.h>
#include <gr.h>
#include <tmr.h>
#include <mc.h>
#include <os.h>
#include <devinit.h>
#include "nvhw.h"
#include "nvVer.h"

//---------------------------------------------------------------------------
//
//  Create/Delete chip id object.
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//  Chip specific object method.
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//  Resource Manager exception service handlers entrypoint.
//
//---------------------------------------------------------------------------

VOID mcService
(
    PHWINFO pDev  // pointer to an nv device
)
{
    V032 pending;
    V032 pmc;
#ifdef RM_STATS
    U032 EnterTimeLo;
    U032 EnterTimeHi;
    U032 ExitTimeLo;
    U032 ExitTimeHi;
    U032 ServiceTimeLo;
    U032 ServiceTimeHi;
#endif
    U032 MirrorEnabled = 0;

    NVRM_TRACE('MISR');

#ifdef RM_STATS
    pDev->Chip.ServiceCount++;
    tmrGetCurrentTime(&EnterTimeHi, &EnterTimeLo);
#endif
    osEnterCriticalCode(pDev);
    //
    // Disable I/O broadcast mode.  There are bugs in NV11's use of broadcast
    // mode where we can't reach HeadB through privSpace with this enabled.
    // It's possible that we've gotten an Int10 interrupt that enables
    // Broadcast mode before we've received the rmEnableVGA call.  We need
    // to disable this when servicing the interrupts, because these functions
    // need to access both Head A and Head B through privSpace.
    //
    // Chips after NV11 have this fixed.
    //
    if (IsNV11(pDev) && !pDev->Vga.Enabled)
    {
        // store broadcast support
        MirrorEnabled = REG_RD_DRF(_PBUS, _DEBUG_1, _DISP_MIRROR);

        // disable broadcast support
        FLD_WR_DRF_DEF(_PBUS, _DEBUG_1, _DISP_MIRROR, _DISABLE);

    }

    do
    {
        //
        // Process all exceptions as required
        //
        pending = 0;
        pmc     = REG_RD32(NV_PMC_INTR_0);
        
        if (pmc & DRF_DEF(_PMC, _INTR_0, _PTIMER, _PENDING))
        {
#ifdef RM_STATS
            pDev->Timer.ServiceCount++;
#endif
            pending |= tmrService(pDev);
        }
        
        if (pmc & (DRF_DEF(_PMC, _INTR_0, _PCRTC,    _PENDING) |
                   DRF_DEF(_PMC, _INTR_0, _PCRTC2,    _PENDING)))
        {
#ifdef RM_STATS
            pDev->Dac.ServiceCount++;
#endif
            pending |= dacService(pDev);
        }
        
        if (pmc & DRF_DEF(_PMC, _INTR_0, _PGRAPH, _PENDING))
        {
#ifdef RM_STATS
            pDev->Graphics.ServiceCount++;
#endif
            pending |= grService(pDev);
        }
        
        if (pmc & DRF_DEF(_PMC, _INTR_0, _PMEDIA, _PENDING))
        {
#ifdef RM_STATS
            pDev->MediaPort.ServiceCount++;
#endif
            pending |= mpService(pDev); 
        }
        
        if ((pmc & DRF_DEF(_PMC, _INTR_0, _PFIFO, _PENDING))
         || (REG_RD_DRF(_PFIFO, _DEBUG_0, _CACHE_ERROR0) == NV_PFIFO_DEBUG_0_CACHE_ERROR0_PENDING))
        {
#ifdef RM_STATS
            pDev->Fifo.ServiceCount++;
#endif
            pending |= fifoService(pDev);
        }
        
        if (pmc & DRF_DEF(_PMC, _INTR_0, _PVIDEO, _PENDING))
        {
#ifdef RM_STATS
            pDev->Video.ServiceCount++;
#endif
            pending |= videoService(pDev); 
        }
               
        if(pmc & DRF_DEF(_PMC, _INTR_0, _PBUS, _PENDING))
        {
#ifdef RM_STATS        
            pDev->Bus.ServiceCount++;
#endif
            pending |= busService(pDev);
        }
    }
    while (pending);
    
    // Restore Broadcast support if needed
    if (IsNV11(pDev) && !(pDev->Vga.Enabled) && MirrorEnabled)
    {
        FLD_WR_DRF_DEF(_PBUS, _DEBUG_1, _DISP_MIRROR, _ENABLE);
    }

    osExitCriticalCode(pDev);

#ifdef RM_STATS
    //
    // Add time spent in service routines to accumulated time.
    //
    tmrGetCurrentTime(&ExitTimeHi, &ExitTimeLo);
    ServiceTimeLo = ExitTimeLo - EnterTimeLo;
    ServiceTimeHi = ExitTimeHi - EnterTimeHi;
    if (ServiceTimeLo > ExitTimeLo)
        ServiceTimeHi--;
    pDev->Chip.ServiceTimeLo += ServiceTimeLo;
    if (pDev->Chip.ServiceTimeLo < ServiceTimeLo)
        ServiceTimeHi++;
    pDev->Chip.ServiceTimeHi += ServiceTimeHi;    
#endif

    NVRM_TRACE('misr');
}

V032 busService(PHWINFO pDev)
{    
    HotPlugService(pDev);
    
    //reset the bus interrupt in master
    

    //no pending irqs here- all interrupts serviced at once at this point
    return 0;
}

#define BIOS_RM_DWORD_REV       0x02050100

//
// Determine the current BIOS revision.
//
// If we've got a BIOS that is later than rev 2.05.01, we'll also check the dword
// that's been added to the BIOS header which indicates the minimum driver revision
// required for this HW.
//
// This allows us to add support for future chips before they've been completely
// validated. If a previously shipped driver turns out to not be able to safely
// boot the HW, we'll change the BIOS dword to require a later driver to boot.
//
RM_STATUS
mcSetBiosRevision
(
    PHWINFO pDev
)
{
#ifndef IKOS
    U032 offset;
    BMP_Control_Block bmpCtrlBlk;
    U016 minDrvRev;
    RM_STATUS rmStatus;
#endif

#ifndef IKOS
    rmStatus = DevinitGetBMPControlBlock(pDev,
                                         &bmpCtrlBlk,
                                         &offset);
    if (rmStatus == RM_OK)
    {
        // set the BiosRevision and BiosRMDword (if it's available)
        switch (bmpCtrlBlk.CTL_Version)
        {
            case BMP_INIT_TABLE_VER_1:
            {
                BMP_Struc_Rev1 BmpStrucRev1;
                BiosReadStructure(pDev,
                                  (U008 *) &BmpStrucRev1,
                                  offset,
                                  (U032 *) 0,
                                  BMP_INIT1_FMT);
                pDev->Chip.BiosRevision       = BmpStrucRev1.BIOS_Version;
                pDev->Chip.BiosOEMRevision    = 0;
                pDev->Chip.BiosRMDword        = 0x0;      // not supported
                break;
            }

            case BMP_INIT_TABLE_VER_2:
            case BMP_INIT_TABLE_VER_3:
            case BMP_INIT_TABLE_VER_4:
            {
                BMP_Struc_Rev2 BmpStrucRev2;
                BiosReadStructure(pDev,
                                  (U008 *) &BmpStrucRev2,
                                  offset,
                                  (U032 *) 0,
                                  BMP_INIT2_FMT);
                pDev->Chip.BiosRevision       = BmpStrucRev2.BIOS_Version;
                pDev->Chip.BiosOEMRevision    = 0;
                pDev->Chip.BiosRMDword        = BmpStrucRev2.BIOS_DriverInfo;
                break;
            }

            // default is BMP init table version 5 base-level functionality
            default:
            {
                BMP_Struc_Rev5 BmpStrucRev5;
                BiosReadStructure(pDev,
                                  (U008 *) &BmpStrucRev5,
                                  offset,
                                  (U032 *) 0,
                                  BMP_INIT5_FMT);

                pDev->Chip.BiosRevision       = BmpStrucRev5.BIOS_Version;
                pDev->Chip.BiosOEMRevision    = BmpStrucRev5.BMP_OEMRevision;
                pDev->Chip.BiosRMDword        = BmpStrucRev5.BIOS_DriverInfo;
                break;
            }
        }
    
        minDrvRev = (U016)(pDev->Chip.BiosRMDword & 0xFFFF);
                    
        // Check if we're loading a driver that is at the required rev or higher
        if (minDrvRev > NV_DRIVER_VERSION_NUMBER)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Later driver revision required!\n");
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "          HW expects a revision >= ", minDrvRev);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "          RM driver version is ", NV_DRIVER_VERSION_NUMBER);
            return RM_ERROR;
        }
    
        return RM_OK;
    }
    else
#endif
    {
        U032 i = 0, found = 0;

        //
        // We have ourselves on old BIOS.     
        //
        // This is nasty, but we need to scan for the POST string
        //   I'm not sure this will work on the MAC, but it shouldn't matter.
        //
        while (!found && (i < 0x100))
        {
            if (REG_RD08(pDev->Pram.HalInfo.PraminOffset+i) == 'O')
                if (REG_RD08(pDev->Pram.HalInfo.PraminOffset+i+1) == 'S')
                    if (REG_RD08(pDev->Pram.HalInfo.PraminOffset+i+2) == ' ')
                        if (REG_RD08(pDev->Pram.HalInfo.PraminOffset+i+3) == 'V')
                        {
                            pDev->Chip.BiosRevision =
                                ((REG_RD08(pDev->Pram.HalInfo.PraminOffset+i+4) - '0') << 24)
                              | ((REG_RD08(pDev->Pram.HalInfo.PraminOffset+i+6) - '0') << 20)
                              | ((REG_RD08(pDev->Pram.HalInfo.PraminOffset+i+7) - '0') << 16);
                             
                            if (REG_RD08(pDev->Pram.HalInfo.PraminOffset+i+8) != '.')
                                pDev->Chip.BiosRevision |= (REG_RD08(pDev->Pram.HalInfo.PraminOffset+i+8) - 'A') & 0xFF;
                            else
                            {
                                pDev->Chip.BiosRevision |=
                                    ((REG_RD08(pDev->Pram.HalInfo.PraminOffset+i+9) - '0') << 4)
                                  | ((REG_RD08(pDev->Pram.HalInfo.PraminOffset+i+10) - '0'));
                            }
                        found = 1;
                        }                            
                            
            i++;  
        }                                  
        pDev->Chip.BiosOEMRevision = 0;
        return RM_OK;
    }
}


//
// A possible power state change is triggered by a variety of sources through
// the resman.  This routine will check the current state of the system and
// determine if a power-related change is required.
//
// All algorithms are localized in this routine as opposed to being sprinkled
// throughout the rest of the code.
//
RM_STATUS mcPowerStateTrigger
(
    PHWINFO pDev
)
{
    U032 NewNVClk;
    U032 NewMClk;
    U032 PMLevel = pDev->Power.PowerMizerLevel;

    //
    // Perform mobile-specific power management
    //

    if (pDev->Power.PostNVCLK == 0)
        pDev->Power.PostNVCLK = pDev->Dac.HalInfo.NVClk;
    if (pDev->Power.PostMCLK == 0)
        pDev->Power.PostMCLK = pDev->Dac.HalInfo.MClk;

    if (pDev->Power.MobileOperation)
    {

        if (pDev->Graphics.HalInfo.has3dObject)
        {
            if (PMLevel == 0)
            {
                NewNVClk = pDev->Power.PmNVCLK[0];
            }
            else
            {
                // 3D NVClk uses one PM level below the current one.
                NewNVClk = pDev->Power.PmNVCLK[PMLevel - 1];
            }
        }
        else
        {
            NewNVClk = pDev->Power.PmNVCLK[PMLevel];
        }

        // Both 2D and 3D shared the same MClk since it is difficult to
        // dynamically switch MClk
        NewMClk = pDev->Power.PmMCLK[PMLevel];

        if (!NewNVClk)
        {
            NewNVClk = pDev->Power.PostNVCLK;
        }
        if (!NewMClk)
        {
            NewMClk = pDev->Power.PostMCLK;
        }

        if (NewNVClk != pDev->Dac.HalInfo.NVClk)
        {
            pDev->Dac.HalInfo.NVClk = NewNVClk;
            dacProgramNVClk(pDev);
        }
        if (NewMClk != pDev->Dac.HalInfo.MClk)
        {
            pDev->Dac.HalInfo.MClk = NewMClk;
            dacProgramMClk(pDev, 1);
        }
    }
    return RM_OK;
}


//---------------------------------------------------------------------------
//
//  Chip ID functions.
//
//---------------------------------------------------------------------------


//-------------------------------------------------------------------
// NV4 and family
//-------------------------------------------------------------------
BOOL IsNV4(PHWINFO pDev)
{
    PMCHALINFO pMcHalInfo = pDev->halHwInfo.pMcHalInfo;

    // assert the architecture has been setup
    RM_ASSERT(pMcHalInfo->Architecture);

    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV04) &&
            (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV04));
}

BOOL IsNV5(PHWINFO pDev)
{
    PMCHALINFO pMcHalInfo = pDev->halHwInfo.pMcHalInfo;

    // assert the architecture has been setup
    RM_ASSERT(pMcHalInfo->Architecture);

    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV04) &&
            (pMcHalInfo->Implementation >= MC_IMPLEMENTATION_NV05));
}

BOOL IsNV0A(PHWINFO pDev)
{
    PMCHALINFO pMcHalInfo = pDev->halHwInfo.pMcHalInfo;

    // assert the architecture has been setup
    RM_ASSERT(pMcHalInfo->Architecture);

    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV04) &&
            (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV0A));
}

BOOL IsNV5orBetter(PHWINFO pDev)
{
    PMCHALINFO pMcHalInfo = pDev->halHwInfo.pMcHalInfo;

    // assert the architecture has been setup
    RM_ASSERT(pMcHalInfo->Architecture);

    return ((pMcHalInfo->Architecture > MC_ARCHITECTURE_NV04) ||
            ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV04) &&
             (pMcHalInfo->Implementation >= MC_IMPLEMENTATION_NV05)));
}

//-------------------------------------------------------------------
// NV10 and family.
//-------------------------------------------------------------------
BOOL IsNV10(PHWINFO pDev)
{
    PMCHALINFO pMcHalInfo = pDev->halHwInfo.pMcHalInfo;

    // assert the architecture has been setup
    RM_ASSERT(pMcHalInfo->Architecture);

    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV10) &&
            (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV10));
}

BOOL IsNV10MaskRevA02orBetter(PHWINFO pDev)
{
    PMCHALINFO pMcHalInfo = pDev->halHwInfo.pMcHalInfo;

    // assert the architecture has been setup
    RM_ASSERT(pMcHalInfo->Architecture);

    // Is this NV10 A02 or better.  Note that "better" also includes all 
    // NV1X parts above NV10.
    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV10) &&
            ((pMcHalInfo->MaskRevision >= MC_MASK_REVISION_A2) ||
             (pMcHalInfo->Implementation > MC_IMPLEMENTATION_NV10)));
}

BOOL IsNV10MaskRevA03orBetter(PHWINFO pDev)
{
    PMCHALINFO pMcHalInfo = pDev->halHwInfo.pMcHalInfo;

    // assert the architecture has been setup
    RM_ASSERT(pMcHalInfo->Architecture);

    // Is this NV10 A03 or better.  Note that "better" also includes all 
    // NV1X parts above NV10.
    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV10) &&
            (((pMcHalInfo->MaskRevision >= MC_MASK_REVISION_A3) &&
             (pMcHalInfo->MaskRevision != MC_MASK_REVISION_A6)) ||
             (pMcHalInfo->Implementation > MC_IMPLEMENTATION_NV10)));
}

BOOL IsNV11(PHWINFO pDev)
{
    PMCHALINFO pMcHalInfo = pDev->halHwInfo.pMcHalInfo;

    // assert the architecture has been setup
    RM_ASSERT(pMcHalInfo->Architecture);

    // Is this NV11?
    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV10) &&
            (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV11));
}

BOOL IsNV1A(PHWINFO pDev)
{
    PMCHALINFO pMcHalInfo = pDev->halHwInfo.pMcHalInfo;

    // assert the architecture has been setup
    RM_ASSERT(pMcHalInfo->Architecture);

    // Is this NV11?
    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV10) &&
            (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV1A));
}

BOOL IsNV15(PHWINFO pDev)
{
    PMCHALINFO pMcHalInfo = pDev->halHwInfo.pMcHalInfo;

    // assert the architecture has been setup
    RM_ASSERT(pMcHalInfo->Architecture);

    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV10) &&
            (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV15));
}

BOOL IsNV20(PHWINFO pDev)
{
    PMCHALINFO pMcHalInfo = pDev->halHwInfo.pMcHalInfo;

    // assert the architecture has been setup
    RM_ASSERT(pMcHalInfo->Architecture);

    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV20) &&
            (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV20));
}

#ifdef _WIN32
#pragma warning(disable: 4296)
#endif

BOOL IsNV10orBetter(PHWINFO pDev)
{
    PMCHALINFO pMcHalInfo = pDev->halHwInfo.pMcHalInfo;

    // assert the architecture has been setup
    RM_ASSERT(pMcHalInfo->Architecture);

    return ((pMcHalInfo->Architecture >= MC_ARCHITECTURE_NV10) &&
            (pMcHalInfo->Implementation >= MC_IMPLEMENTATION_NV10));
}

#ifdef _WIN32
#pragma warning(default: 4296)
#endif

BOOL IsNV15orBetter(PHWINFO pDev)
{
    PMCHALINFO pMcHalInfo = pDev->halHwInfo.pMcHalInfo;

    // assert the architecture has been setup
    RM_ASSERT(pMcHalInfo->Architecture);

    return (((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV10) &&
             (pMcHalInfo->Implementation >= MC_IMPLEMENTATION_NV11)) ||
             (pMcHalInfo->Architecture >= MC_ARCHITECTURE_NV20));
}

BOOL IsNV15MaskRevA01(PHWINFO pDev)
{
    PMCHALINFO pMcHalInfo = pDev->halHwInfo.pMcHalInfo;

    // assert the architecture has been setup
    RM_ASSERT(pMcHalInfo->Architecture);

    // Is this NV15 Mask Revision A01?
    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV10) &&
            (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV15) &&
            (pMcHalInfo->MaskRevision == MC_MASK_REVISION_A1));
}

BOOL IsNV15MaskRevA02(PHWINFO pDev)
{
    PMCHALINFO pMcHalInfo = pDev->halHwInfo.pMcHalInfo;

    // assert the architecture has been setup
    RM_ASSERT(pMcHalInfo->Architecture);

    // Is this NV15 Mask Revision A02?
    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV10) &&
            (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV15) &&
            (pMcHalInfo->MaskRevision == MC_MASK_REVISION_A2));
}

BOOL IsNV15MaskRevA03(PHWINFO pDev)
{
    PMCHALINFO pMcHalInfo = pDev->halHwInfo.pMcHalInfo;

    // assert the architecture has been setup
    RM_ASSERT(pMcHalInfo->Architecture);

    // Is this NV15 Mask Revision A03?
    return ((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV10) &&
            (pMcHalInfo->Implementation == MC_IMPLEMENTATION_NV15) &&
            (pMcHalInfo->MaskRevision == MC_MASK_REVISION_A3));
}

BOOL IsNV11orBetter(PHWINFO pDev)
{
    PMCHALINFO pMcHalInfo = pDev->halHwInfo.pMcHalInfo;

    // assert the architecture has been setup
    RM_ASSERT(pMcHalInfo->Architecture);

    // Is this NV11 or better?
    return (((pMcHalInfo->Architecture == MC_ARCHITECTURE_NV10) &&
             (pMcHalInfo->Implementation >= MC_IMPLEMENTATION_NV11) &&
             (pMcHalInfo->Implementation != MC_IMPLEMENTATION_NV15)) ||
             (pMcHalInfo->Architecture >= MC_ARCHITECTURE_NV20));
}

BOOL IsNV20orBetter(PHWINFO pDev)
{
    PMCHALINFO pMcHalInfo = pDev->halHwInfo.pMcHalInfo;

    // assert the architecture has been setup
    RM_ASSERT(pMcHalInfo->Architecture);

    return (pMcHalInfo->Architecture >= MC_ARCHITECTURE_NV20);
}
