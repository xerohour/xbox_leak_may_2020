//******************************************************************************
//
// Copyright (c) 1992  Microsoft Corporation
//
// Module Name:
//
//     NV10.C
//
// Abstract:
//
//     This is code specific to NV10
//
// Environment:
//
//     kernel mode only
//
// Notes:
//
// Revision History:
//
//******************************************************************************

//******************************************************************************
//
// Copyright (c) 1996,1997  NVidia Corporation. All Rights Reserved
//
//******************************************************************************


#include "dderror.h"
#include "devioctl.h"
#include "miniport.h"

#include "ntddvdeo.h"
#include "video.h"

#include "nv.h"

#include "nv4_ref.h"

#include "nv32.h"
#include "nvsubch.h"
#include "nvalpha.h"

#include "cmdcnst.h"
#include <arb_fifo.h>
#include <nvntioctl.h>

#include "modes.h"
#include "tv.h"

#include "monitor.h"
#include "modedefs.h"
#include "vesadata.h"

#include "nvos.h"
#include "rm.h"

#include "nvreg.h"
#include "nvcm.h"

//******************************************************************************
// Forward declarations
//******************************************************************************

VOID NV10ChipFBPowerSave(PHW_DEVICE_EXTENSION);
VOID NV10ChipFBPowerRestore(PHW_DEVICE_EXTENSION);

//******************************************************************************
// External Declarations
//******************************************************************************

//******************************************************************************
// Hack -> Eventually, we want to include the official
//         NVREF.H file, located in \kernel\resman\inc.
//         However, since this miniport already has an nvref.h file,
//         we'll have to pick and choose.  This will all get
//         overhauled when the miniport is re-architected.
//******************************************************************************

#define NV_PGRAPH_TILE(i)                       (0x00400b00+(i)*16) /* RW-4A */
#define NV_PGRAPH_TLIMIT(i)                     (0x00400b04+(i)*16) /* RW-4A */
#define NV_PGRAPH_TSIZE(i)                      (0x00400b08+(i)*16) /* RW-4A */
#define NV_PBUS_DEBUG_1                                  0x00001084

#define NV_PRAMIN_DATA032(i)                     (0x00700000+(i)*4) /* RW-4A */
#define NV_PRAMIN_DATA032__SIZE_1                            524288 /*       */
#define NV_PRAMIN_DATA032_VALUE                                31:0 /* RWXVF */


#define NV_PFB_TIMING2                                   0x00100228 /* RW-4R */
#define NV_PFB_TIMING2_REFRESH                                 15:5 /* RW-VF */
#define NV_PFB_CFG                                       0x00100200 /* RW-4R */
#define NV_PFB_CSTATUS                                   0x0010020C /* R--4R */
#define NV_PFB_REFCTRL                                   0x00100210 /* RW-4R */
#define NV_PFB_MRS                                       0x001002C0 /* RW-4R */
#define NV_PFB_EMRS                                      0x001002C4 /* RW-4R */
#define NV_PFB_REF                                       0x001002D0 /* -W-4R */

//******************************************************************************
//
// Function:    NV10ChipFBPowerSave()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NV10ChipFBPowerSave (PHW_DEVICE_EXTENSION HwDeviceExtension)
    {

    // power down mode - max out memory refresh value
    HwDeviceExtension->Power_refresh = REG_RD32(NV_PFB_TIMING2);   // refresh
    FLD_WR_DRF_NUM(_PFB, _TIMING2, _REFRESH, 0xFFFFFFFF);       // max refresh
    // Chip has been init'd with the BIOS init tables, unless the BIOS
    // has an incompatible structure and the table was not found, in which case
    // we used a default table, which may not have the correct RAM config, so
    // we save away the current config for restoration above.
    HwDeviceExtension->Power_PfbBoot0     = REG_RD32(NV_PFB_CFG);           // RAM config
    HwDeviceExtension->Power_PfbConfig0   = REG_RD32(NV_PFB_CSTATUS);       // RAM config
    HwDeviceExtension->Power_PfbConfig1   = REG_RD32(NV_PFB_REFCTRL);       // RAM config
    HwDeviceExtension->Power_PfbMrs       = REG_RD32(NV_PFB_MRS);           // RAM config
    HwDeviceExtension->Power_PfbEmrs      = REG_RD32(NV_PFB_EMRS);          // RAM config
    HwDeviceExtension->Power_PfbRef       = REG_RD32(NV_PFB_REF);           // RAM config
    HwDeviceExtension->Power_PbusDebug1   = REG_RD32(NV_PBUS_DEBUG_1);      // RAM config
    HwDeviceExtension->Power_PextdevBoot0 = REG_RD32(NV_PEXTDEV_BOOT_0);    // READ STRAPS
    HwDeviceExtension->Power_PfifoRamHT   = REG_RD32(NV_PFIFO_RAMHT);
    HwDeviceExtension->Power_PfifoRamRO   = REG_RD32(NV_PFIFO_RAMRO);
    HwDeviceExtension->Power_PfifoRamFC   = REG_RD32(NV_PFIFO_RAMFC);

    }



//******************************************************************************
//
// Function:    NV10ChipFBPowerRestore()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NV10ChipFBPowerRestore (PHW_DEVICE_EXTENSION HwDeviceExtension)
    {
    U032 D0_DELAY_RETRIES = 10; 
    U032 retry = 0;             
    U032 i;                     

    // power up mode - restore previous memory config
    REG_WR32(NV_PBUS_DEBUG_1, HwDeviceExtension->Power_PbusDebug1);   // must set FBI differential
    REG_WR32(NV_PFB_TIMING2, HwDeviceExtension->Power_refresh);       // refresh
    REG_WR32(NV_PFB_CFG, HwDeviceExtension->Power_PfbBoot0);          // RAM config
    REG_WR32(NV_PFB_CSTATUS, HwDeviceExtension->Power_PfbConfig0);    // RAM config
    REG_WR32(NV_PFB_REFCTRL, HwDeviceExtension->Power_PfbConfig1);    // RAM config
    REG_WR32(NV_PFB_MRS, HwDeviceExtension->Power_PfbMrs);    		 // RAM config
//  REG_WR32(NV_PFB_EMRS, HwDeviceExtension->Power_PfbEmrs);    		 // RAM config - reset
    REG_WR32(NV_PFB_REF, HwDeviceExtension->Power_PfbRef);    		 // RAM config

//  A delay seems to be necessary for i820 for win9x.
//  Couldn't demonstrate a problem with W2K, so its here for safety.
//  Reading NV_PMC_ENABLE is simply a convenient delay    
    while (REG_RD32(NV_PEXTDEV_BOOT_0) != HwDeviceExtension->Power_PextdevBoot0)
        {
        for (i=0; i<2000; i++)
            {
            REG_RD32(NV_PMC_ENABLE);
            }
        REG_WR32(NV_PEXTDEV_BOOT_0, HwDeviceExtension->Power_PextdevBoot0);    // WRITE STRAP REG
        retry++;
        if (retry == D0_DELAY_RETRIES)
            break;
        }
    REG_WR32(NV_PFIFO_RAMHT, HwDeviceExtension->Power_PfifoRamHT);      
    REG_WR32(NV_PFIFO_RAMRO, HwDeviceExtension->Power_PfifoRamRO);      
    REG_WR32(NV_PFIFO_RAMFC, HwDeviceExtension->Power_PfifoRamFC);      

    }

