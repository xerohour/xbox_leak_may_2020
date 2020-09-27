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

/******************************** Time Manager *****************************\
*                                                                           *
* Module: TMRINIT.C                                                         *
*   Time state is initialized here.                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <class.h>
#include <state.h>
#include <dma.h>
#include <tmr.h>
#include <os.h>
#include "nvhw.h"


//
//  Initialize timer.
//
RM_STATUS  initTmr
(
    PHWINFO pDev
)
{
    RM_STATUS status;
    S032      num;
    S032      den;
    U032      i;

    //
    // Save previous values.
    //
    pDev->Timer.tmrSave[0] = REG_RD32(NV_PTIMER_NUMERATOR);
    pDev->Timer.tmrSave[1] = REG_RD32(NV_PTIMER_DENOMINATOR);
    //
    // Init timer callback list.
    //
    pDev->DBtmrCallbackList = NULL;
    status = osAllocMem((VOID **)&pDev->DBtmrCallbackTable, sizeof(TIMERCALLBACK) * NUM_TIMERCALLBACKS);
    if (status)
        return (status);
    pDev->DBtmrCallbackFreeList = pDev->DBtmrCallbackTable;
    for (i = 0; i < NUM_TIMERCALLBACKS - 1; i++)
    {
        pDev->DBtmrCallbackFreeList[i].Next = &(pDev->DBtmrCallbackFreeList[i + 1]);
    }
    pDev->DBtmrCallbackFreeList[i].Next = NULL;
    //
    // Calc numerator and denomenator from DAC NVCLK parameters.
    // From Curtis.
    //
    // num = pDev->Dac.MClk; // The manual says MClk. That is wrong.
    num = pDev->Dac.HalInfo.NVClk;
    den = 31250000;
    while ((num % 2 == 0) && (den % 2 == 0))
    {
        num /= 2;
        den /= 2;
    }
    while ((num % 5 == 0) && (den % 5 == 0))
    {
        num /= 5;
        den /= 5;
    }
    while ((num > 2 * DRF_MASK(NV_PTIMER_NUMERATOR_VALUE))
        || (den > 2 * DRF_MASK(NV_PTIMER_DENOMINATOR_VALUE)))
    {
        num /= 2;
        den /= 2;
    }
    num = (num + 1) / 2;
    den = (den + 1) / 2;
    pDev->Timer.Numerator   = num;
    pDev->Timer.Denominator = den;
    
    REG_WR32(NV_PTIMER_NUMERATOR,   pDev->Timer.Numerator);
    REG_WR32(NV_PTIMER_DENOMINATOR, pDev->Timer.Denominator);
    REG_WR32(NV_PTIMER_ALARM_0,     0xFFFFFFFF);
#ifdef RM_STATS
    //
    // Init performance timer to come in every second.
    //
    pDev->Statistics.UpdateLo = 1000000000;
    pDev->Statistics.UpdateHi = 0;
    
    //
    // Zero exception and service counts.
    //
    pDev->Statistics.FifoExceptionCount        = 0;
    pDev->Statistics.FramebufferExceptionCount = 0;
    pDev->Statistics.GraphicsExceptionCount    = 0;
    pDev->Statistics.TotalExceptionCount       = 0;
    pDev->Fifo.ServiceCount                    = 0;
    pDev->Framebuffer.ServiceCount             = 0;
    pDev->Graphics.ServiceCount                = 0;
    pDev->Chip.ServiceCount                    = 0;
    pDev->MediaPort.ServiceCount               = 0;
    pDev->Video.ServiceCount                   = 0;
#endif
    return (RM_OK);
}
