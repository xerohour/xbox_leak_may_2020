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

/****************************** FB Manager *********************************\
*                                                                           *
* Module: FBINIT.C                                                          *
*   The FrameBuffer is initialized in this module.  This is a one time      *
* occurance and then the module is unloaded.                                *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include "nvhw.h"
#include <state.h>
#include <class.h>
#include <fb.h>
#include <vga.h>
#include <os.h>
#include <tv.h>
#include <devinit.h>

// Registry access
extern char strDevNodeRM[];

// forwards
RM_STATUS initFbBiosData(PHWINFO, PBIOSDRVSLWPARMS);

//
// One time framebuffer initialization.
//
RM_STATUS initFb
(
    PHWINFO pDev
)
{
    BIOSDRVSLWPARMS biosDrvSlwParms;
    U032 status;
    U032 RmRamSizeMB;
    U032 AdjustDriveSlew;

    //
    // Hook up any bios BIP3 table data.
    //
    pDev->Framebuffer.HalInfo.pBiosDrvSlwParms = NULL;
    if (initFbBiosData(pDev, &biosDrvSlwParms) == RM_OK)
        pDev->Framebuffer.HalInfo.pBiosDrvSlwParms = (VOID_PTR)&biosDrvSlwParms;

    //
    // Account for BIOS_IMAGE_PAD.  This could be made os-indepedendent
    // so we don't waste 64K on platforms that don't need the bios.
    //
    pDev->Pram.HalInfo.InstPadSize = BIOS_IMAGE_PAD;

    //
    // By default adjust the drive slew parameters.
    // Check the registry for drive slew override.
    //
    pDev->Framebuffer.HalInfo.AdjustDriveSlew = 1;
    if (RM_OK == OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, "AdjustDriveSlew", &AdjustDriveSlew))
        pDev->Framebuffer.HalInfo.AdjustDriveSlew = AdjustDriveSlew;

    //
    // Let HAL establish FB parameters:
    //   - dram sizes/bank counts
    //   - fifo context/hash table/run out sizes
    //
    status = nvHalFbControl(pDev, FB_CONTROL_INIT);

    //
    // Clear the bip3 table data pointer.
    //
    pDev->Framebuffer.HalInfo.pBiosDrvSlwParms = (VOID_PTR)NULL;

    //Did we config the frame buffer correctly? 
    if (status != RM_OK)
    {
        return (RM_ERROR);
    }

    // Make sure the values are valid
    if (pDev->Framebuffer.HalInfo.RamSize == 0 ||
        pDev->Framebuffer.HalInfo.RamSizeMb == 0)
    {
        return (RM_ERROR);
    }

    //
    // We may want to override the ram size with our own values from the registry
    //
    status = osReadRegistryDword(pDev, strDevNodeRM, "RmRamSizeMB", &RmRamSizeMB);
    if (status == RM_OK)
    {
        if (RmRamSizeMB < pDev->Framebuffer.HalInfo.RamSizeMb)
        {
            pDev->Framebuffer.HalInfo.RamSizeMb = RmRamSizeMB;
            pDev->Framebuffer.HalInfo.RamSize = RmRamSizeMB * 1024 * 1024;
        }
        else
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: RmRamSizeMB was set greater than actual physical ram size\n");             
            DBG_BREAKPOINT(); 
        }
    }

    fbLoadOverrides(pDev);

    //
    // Locate and size the free instance area. This is the base where
    // allocations should start and size of the allocatable inst mem.
    //
    pDev->Pram.FreeInstSize = pDev->Pram.HalInfo.TotalInstSize - BIOS_IMAGE_PAD - pDev->Pram.HalInfo.CurrentSize;
    pDev->Pram.FreeInstBase = (pDev->Pram.HalInfo.CurrentSize + BIOS_IMAGE_PAD) >> 4;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: FreeInstSize ", pDev->Pram.FreeInstSize);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: FreeInstBase ", pDev->Pram.FreeInstBase);

    //
    // Default to 640X480X16X1 framebuffer w/ VGA timing.
    //
    pDev->Framebuffer.DpmLevel          = 0;
    pDev->Framebuffer.HalInfo.HorizDisplayWidth = 640;
    pDev->Framebuffer.HalInfo.VertDisplayWidth  = 480;
    pDev->Dac.HalInfo.Depth     = 16;
    pDev->Framebuffer.HalInfo.RefreshRate       = 60;
    pDev->Framebuffer.Count             = 1;
    pDev->Framebuffer.Current           = 0;
    pDev->Framebuffer.FlipUsageCount    = 0;
    pDev->Framebuffer.FlipTo            = 0;
    pDev->Framebuffer.FlipFrom          = 0;
    pDev->Framebuffer.UpdateFlags       = 0;
    pDev->Framebuffer.HorizFrontPorch   = 0;
    pDev->Framebuffer.HorizSyncWidth    = 0;
    pDev->Framebuffer.HorizBackPorch    = 0;
    pDev->Framebuffer.VertFrontPorch    = 0;
    pDev->Framebuffer.VertSyncWidth     = 0;
    pDev->Framebuffer.VertBackPorch     = 0;
    pDev->Framebuffer.HSyncPolarity     = 0;
    pDev->Framebuffer.VSyncPolarity     = 0;
    pDev->Framebuffer.CSync             = 0;

    pDev->Framebuffer.HalInfo.Start[0] = 0x00000000;
    pDev->Framebuffer.HalInfo.Limit[0] = pDev->Framebuffer.HalInfo.RamSize - pDev->Pram.HalInfo.TotalInstSize - 1;
    pDev->Framebuffer.HalInfo.Start[1] = 0x00000000;
    pDev->Framebuffer.HalInfo.Limit[1] = 0x00000000;

    //
    // Check for os-specific mode overrides.
    //
    initFbEx(pDev);

    //
    // Make sure our write protection is disabled for now
    //
    nvHalFbControl(pDev, FB_CONTROL_INSTWR_ENABLE);

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: RamSize ", pDev->Framebuffer.HalInfo.RamSize);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: TotalInstSize ", pDev->Pram.HalInfo.TotalInstSize);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Limit[0] ", pDev->Framebuffer.HalInfo.Limit[0]);

    return (fbCheckConfig(pDev));
}

//
// Get BIOS bip3 table data if it exists.
//
RM_STATUS initFbBiosData
(
    PHWINFO pDev,
    PBIOSDRVSLWPARMS pDrvSlwParms
)
{
    BMP_Control_Block bmpCtrlBlk;
    U032 offset;
    RM_STATUS rmStatus = RM_OK;

    //
    // Attempt to find the Drive/Slew parms in the BIOS' BMP tables.
    //
    rmStatus = DevinitGetBMPControlBlock(pDev, &bmpCtrlBlk, &offset);
    if (rmStatus != RM_OK)
        return rmStatus;

    //
    // Only available in rev 5/ver 0x13 tables.
    //
    if ((bmpCtrlBlk.CTL_Version == BMP_INIT_TABLE_VER_5) && (bmpCtrlBlk.CTL_Flags >= 0x13))
    {
        BMP_Struc_Rev5_0x13 bmpInitTable;
        rmStatus = BiosReadStructure(pDev, &bmpInitTable, offset,
                                     (U032 *)0, BMP_INIT5_0x13_FMT);
        if (rmStatus == RM_OK)
        {
            rmStatus = BiosReadStructure(pDev, pDrvSlwParms,
                                         bmpInitTable.BMP_DrvSlwParms_Block,
                                         (U032 *)0, BMP_DRVSLW_PARMS);
        }
    }
    else
    {
        rmStatus = RM_ERROR;
    }

    return rmStatus;
}

