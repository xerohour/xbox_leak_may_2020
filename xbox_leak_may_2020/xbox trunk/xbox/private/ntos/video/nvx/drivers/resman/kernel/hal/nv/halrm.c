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

/************************** HAL/RM Interfaces ******************************\
*                                                                           *
* Module: halrm.c                                                           *
*   The Resource Manager service routines for the HAL are in this module.   *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

//
// halrm.c
//
// RM service entry points needed by HAL.
//

#include <nv_ref.h>
#include <nvrm.h>
#include <nvhal.h>
#include "nvhw.h"
#include <i2c.h>


//
// nvHalRmPrintString
//
//
VOID nvHalRmPrintString(VOID *pDeviceId, U032 debugLevel, char* str)
{
   	PHWINFO pDev = (PHWINFO)pDeviceId;
	DBG_PRINT_STRING(debugLevel, str);
}

//
// nvHalRmPrintValue
//
//
VOID nvHalRmPrintValue(VOID *pDeviceId, U032 debugLevel, U032 value)
{
   	PHWINFO pDev = (PHWINFO)pDeviceId;
  	DBG_PRINT_VALUE(debugLevel, value);
}

//
// nvHalRmPrintStringValue
//
//
VOID nvHalRmPrintStringValue(VOID *pDeviceId, U032 debugLevel, char* str, U032 value)
{
  	PHWINFO pDev = (PHWINFO)pDeviceId;
    DBG_PRINT_STRING_VALUE(debugLevel, str, value);
}

//
// nvHalRmPrintStringPtr
//
//
VOID nvHalRmPrintStringPtr(VOID *pDeviceId, U032 debugLevel, char* str, VOID* value)
{
   	PHWINFO pDev = (PHWINFO)pDeviceId;
	DBG_PRINT_STRING_PTR(debugLevel, str, value);
}

//
// nvHalRmPrintStringVal64
//
//
VOID nvHalRmPrintStringVal64(VOID *pDeviceId, U032 debugLevel, char* str, U064 value)
{
   	PHWINFO pDev = (PHWINFO)pDeviceId;
    DBG_PRINT_STRING_VAL64(debugLevel, str, value);
}

//
// nvHalRmRegWr08
//
//
VOID nvHalRmRegWr08(VOID *pDeviceId, U032 offset, U008 value)
{
	PHWINFO pDev = (PHWINFO)pDeviceId;
	REG_WR08(offset, value);
}

//
// nvHalRmRegRd08
//
//
U008 nvHalRmRegRd08(VOID *pDeviceId, U032 offset)
{
	PHWINFO pDev = (PHWINFO)pDeviceId;
	return (REG_RD08(offset));
}

//
// nvHalRmRegWr32
//
//
VOID nvHalRmRegWr32(VOID *pDeviceId, U032 offset, U032 value)
{
	PHWINFO pDev = (PHWINFO)pDeviceId;
	REG_WR32(offset, value);
}

//
// nvHalRmRegRd32
//
//
U032 nvHalRmRegRd32(VOID *pDeviceId, U032 offset)
{
	PHWINFO pDev = (PHWINFO)pDeviceId;
	return (REG_RD32(offset));
}

//
// nvHalRmBreakPoint
//
//
VOID nvHalRmBreakPoint(VOID *pDeviceId)
{
   	PHWINFO pDev = (PHWINFO)pDeviceId;
	DBG_BREAKPOINT();
}

//
// nvHalRmDelay
//
//
RM_STATUS nvHalRmDelay(VOID *pDeviceId, U032 milliSeconds)
{
    PHWINFO pDev = (PHWINFO)pDeviceId;
	return (osDelay(milliSeconds));
}

//
// nvHalRmServiceIntr
//
// Service a pending condition on the specified engine on the
// specified device.
//
RM_STATUS
nvHalRmServiceIntr(VOID *pDeviceId, U032 engineTag)
{
    PHWINFO pDev = (PHWINFO)pDeviceId;

    switch (engineTag)
    {
        case MC_ENGINE_TAG:
            (void) mcService(pDev);
            break;
        case GR_ENGINE_TAG:
            (void) grService(pDev);
            break;
        case FIFO_ENGINE_TAG:
            (void) fifoService(pDev);
            break;
        case DAC_ENGINE_TAG:
            (void) VBlank(pDev);
            break;
        default:
            break;
    };
    return (RM_OK);
}

//
// nvHalRmI2cRead
//
// Issue i2c bus read request on behalf of the HAL.
//
RM_STATUS nvHalRmI2cRead(VOID *pDeviceId, U032 portID, U008 ChipAdr, U016 AdrLen, U008 *Adr, U016 DataLen, U008 *Data)
{
    PHWINFO pDev = (PHWINFO)pDeviceId;
    U032    Head =0; // for now, always use head 0
    return (i2cRead(pDev, Head, portID, ChipAdr, AdrLen, Adr, DataLen, Data));
}

//
// nvHalRmI2cWrite
//
// Issue i2c bus write request on behalf of the HAL.
//
RM_STATUS nvHalRmI2cWrite(VOID *pDeviceId, U032 portID, U008 ChipAdr, U016 AdrLen, U008 *Adr, U016 DataLen, U008 *Data)
{
    PHWINFO pDev = (PHWINFO)pDeviceId;
    U032    Head =0; // for now, always use head 0

    return (i2cWrite(pDev, Head, portID, ChipAdr, AdrLen, Adr, DataLen, Data));
}

//
// RM/HAL services table.
//
HALRMFUNCS_000 HalRmFuncs_000 =
{
    // interrupt service entry point
    nvHalRmServiceIntr,

    // debug print routines
    nvHalRmPrintString,
    nvHalRmPrintValue,
    nvHalRmPrintStringValue,
    nvHalRmPrintStringPtr,
    nvHalRmPrintStringVal64,

    // register io entry points
    nvHalRmRegWr08,
    nvHalRmRegRd08,

    nvHalRmRegWr32,
    nvHalRmRegRd32,

    nvHalRmBreakPoint,
    nvHalRmDelay,

    nvHalRmI2cRead,
    nvHalRmI2cWrite,
};
