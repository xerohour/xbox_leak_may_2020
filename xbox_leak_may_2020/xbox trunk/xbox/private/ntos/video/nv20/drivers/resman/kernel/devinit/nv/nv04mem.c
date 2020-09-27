 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1999 NVIDIA, Corp.  All rights reserved.        *|
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
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
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

/**************************************************************************************************************
*
*	Module: nv04mem.c
*
*	Description:
*		Device-specific memory sizing code, for NV4.
*
*	Revision History:
*		Original -- 5/99 Jeff Westerinen
*
**************************************************************************************************************/

#ifdef NV_PORTABLE

/* "portable" form for use outside resman and by external source customers */
/* look in devinit/portable/ for more details */
#include <nv_portable.h>
#include <nv_ref.h>
#include <nv4_ref.h>
#include <devinit.h>

#else	/* not NV_PORTABLE --> Nvidia internal */

#include <nvrm.h>
#include <nvhw.h>
#include <nv4_ref.h>
#include <os.h>
#include <devinit.h>

#endif

// TO DO: the actual memory detection and sizing is device dependent
extern char strDevNodeRM[];

RM_STATUS Nv04MemResizeMemory
(
	PHWINFO pDev
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 memoryType;
    U008 sgram_mem[] = "SGRAM";
    U008 sdram_mem[] = "SDRAM";
    U032 offset;

    // check for registry override of memory type
    if (OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, "VideoMemoryTypeOverride", &memoryType) == RM_OK)
    {
        switch(memoryType)
        {
            case DEVINIT_SDRAM:
                rmStatus = Nv04MemResizeSdram(pDev);
                break;
                
            case DEVINIT_SGRAM:
                rmStatus = Nv04MemResizeSgram(pDev);
                break;
                
            default:
                rmStatus = RM_ERROR;
                break;
        }
    } else {
        rmStatus = BiosFindString(pDev, sgram_mem, &offset, sizeof(sgram_mem));
        if (rmStatus == RM_OK)
        {
            rmStatus = Nv04MemResizeSgram(pDev);
            goto done;
        }
 
        rmStatus = BiosFindString(pDev, sdram_mem, &offset, sizeof(sdram_mem));
        if (rmStatus == RM_OK)
        {
            rmStatus = Nv04MemResizeSdram(pDev);
            goto done;
        } 

        rmStatus = RM_ERROR;
    }

done:
    return rmStatus;
                
} // end of DevinitResizeMemory()

RM_STATUS Nv04MemResizeSdram
(
	PHWINFO pDev
)
{
	RM_STATUS rmStatus = RM_OK;
	U032 i;

	// TO DO: is this necessary?
    // turn on full CPU access
	//...
        
	// turn on refresh
    FLD_WR_DRF_DEF(_PFB,_DEBUG_0,_REFRESH,_ENABLED);

    // assume 16MB, 128-bit, 1024K 2-bank
    FLD_WR_DRF_DEF(_PFB,_BOOT_0,_RAM_AMOUNT,_16MB);
    FLD_WR_DRF_DEF(_PFB,_BOOT_0,_RAM_WIDTH_128,_ON);
    FLD_WR_DRF_DEF(_PFB,_BOOT_0,_RAM_TYPE,_1024K_2BANK);   
    Nv04MemLatchMemConfig(pDev);

    // write 128 bits of data for the test sequence
    for (i = 0; i < 4; i++)
    {
        FB_WR32(i * 4, 'NV4A');
    }    
                                                 
    // if the lower 16 bits of 128-bit word is invalid, change to 8M
    if (FB_RD16(0x0000000C) != '4A')
    {
        FLD_WR_DRF_DEF(_PFB,_BOOT_0,_RAM_AMOUNT,_8MB);
        FLD_WR_DRF_DEF(_PFB,_BOOT_0,_RAM_WIDTH_128,_OFF);
    }

    // insure the memory config gets set
    Nv04MemLatchMemConfig(pDev);

	// TO DO: is this necessary?
	// turn off full CPU access
	//...
    
    // output results
    switch (REG_RD_DRF(_PFB,_BOOT_0,_RAM_AMOUNT))
    {
        case NV_PFB_BOOT_0_RAM_AMOUNT_8MB:
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM:   8MB SDRAM\n");
            break;
            
        case NV_PFB_BOOT_0_RAM_AMOUNT_16MB:
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM:   16MB SDRAM\n");
            break;
            
        default:
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Unknown Size SDRAM\n");
            break;
    }

	return rmStatus;

} // end of DevinitResizeSdram()

RM_STATUS Nv04MemResizeSgram
(
	PHWINFO pDev
)
{
	RM_STATUS rmStatus = RM_OK;
	U032 i, memDevSize;

	// TO DO: is this necessary?
    // turn on full CPU access
	//...
        
    // disable refresh for the entire routine 
    FLD_WR_DRF_DEF(_PFB,_DEBUG_0,_REFRESH,_DISABLED);

    // assume 16MB, 128-bit, 512K 2-bank
    FLD_WR_DRF_DEF(_PFB,_BOOT_0,_RAM_AMOUNT,_16MB);
    FLD_WR_DRF_DEF(_PFB,_BOOT_0,_RAM_WIDTH_128,_ON);
    FLD_WR_DRF_DEF(_PFB,_BOOT_0,_RAM_TYPE,_512K_2BANK);    
    Nv04MemLatchMemConfig(pDev);

    // write 128-bit test data word at the beginning of FB
    for (i = 0; i < 4; i++)
    {
        FB_WR32(i * 4, 'NV4A');
    }    
    
    // determine the size of the memory devices, i.e. 8 or 16 Mbit
    memDevSize = (FB_RD32(0x00000000) == 'NV4A') ? 16 : 8;

    // determine the interface width
    if (FB_RD16(0x0e) == 'NV')
    {
        // 128-bit width detected
        
        // detect 8mbit or 16mbit @ 128-bits
        if (memDevSize == 16)
        {
            // 16mbit device, 128-bit width detected
            
            // detect 8MB or 16MB total RAM amount
            FB_WR32(0x00800000, 'A4VN');
            if (FB_RD32(0x00800000) != 'A4VN')
            {
                // 8MB detected, so change config
                FLD_WR_DRF_DEF(_PFB,_BOOT_0,_RAM_AMOUNT,_8MB);
            }
        }
        else
        {
            // 8mbit device, 128-bit width detected
            
            // detect 4MB or 8MB total RAM amount
            // Note: unclear about 4/8M test -- write at 4Meg or 8Meg??? -- !!!!! TEST THIS !!!!!
            FB_WR32(0x00800000, 'A4VN');
            if (FB_RD32(0x00800000) == 'A4VN')
            {
                // 8MB detected, so change config
                FLD_WR_DRF_DEF(_PFB,_BOOT_0,_RAM_AMOUNT,_8MB);
            }
            else
            {
                // 4MB detected, so change config
                FLD_WR_DRF_DEF(_PFB,_BOOT_0,_RAM_AMOUNT,_4MB);
            }
        }
    }
    else
    {
        // 64-bit width detected
        
        // since 8MBit device is not valid at 64-bit width, 4MB detected, so change config
        FLD_WR_DRF_DEF(_PFB,_BOOT_0,_RAM_WIDTH_128,_OFF);
        FLD_WR_DRF_DEF(_PFB,_BOOT_0,_RAM_AMOUNT,_4MB);
    }

    // insure the memory config gets set
    Nv04MemLatchMemConfig(pDev);

	// reenable refresh
    FLD_WR_DRF_DEF(_PFB,_DEBUG_0,_REFRESH,_ENABLED);

	// TO DO: is this necessary?
	// turn off full CPU access
	//...

    // output results
    switch (REG_RD_DRF(_PFB,_BOOT_0,_RAM_AMOUNT))
    {
        case NV_PFB_BOOT_0_RAM_AMOUNT_4MB:
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM:   4MB SGRAM\n");
            break;
            
        case NV_PFB_BOOT_0_RAM_AMOUNT_8MB:
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM:   8MB SGRAM\n");
            break;
            
        case NV_PFB_BOOT_0_RAM_AMOUNT_16MB:
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM:   16MB SGRAM\n");
            break;
            
        default:
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Unknown Size SGRAM\n");
            break;
    }

	return rmStatus;

} // end of DevinitResizeSgram()

VOID Nv04MemLatchMemConfig
(
    PHWINFO pDev
)
{
    // save the original CAS latency
    U032 latency = REG_RD_DRF(_PFB,_CONFIG_1,_CAS_LATENCY);
    
    // toggle the LSB 1-0-1
    FLD_WR_DRF_DEF(_PFB,_CONFIG_1,_CAS_LATENCY,_3);
    FLD_WR_DRF_DEF(_PFB,_CONFIG_1,_CAS_LATENCY,_2);
    FLD_WR_DRF_DEF(_PFB,_CONFIG_1,_CAS_LATENCY,_3);
    
    // restore original CAS latency
    FLD_WR_DRF_NUM(_PFB,_CONFIG_1,_CAS_LATENCY,latency);
    
} // end of Nv04MemLatchMemConfig()


// end of nv04mem.c


