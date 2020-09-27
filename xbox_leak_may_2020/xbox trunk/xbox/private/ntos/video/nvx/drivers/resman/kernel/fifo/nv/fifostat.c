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

/****************************** FIFO Manager *******************************\
*                                                                           *
* Module: FIFOSTATE.C                                                       *
*   The FIFO state is changed in this module.                               *
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
#include <fifo.h>
#include <dma.h>
#include <dac.h>
#include <gr.h>
#include <os.h>
#include <nv32.h>
#include "nvhw.h"


//
// Change FIFO state in hardware.
//
RM_STATUS stateFifo
(
    PHWINFO pDev,
    U032    msg
)
{
    U032 i;

    switch (msg)
    {
        case STATE_UPDATE:
            nvHalFifoControl(pDev, FIFO_CONTROL_UPDATE);
            break;
        case STATE_LOAD:
            nvHalFifoControl(pDev, FIFO_CONTROL_LOAD);

            //
            // Re-disable cache reassignment if we're in a full-screen DOS session.  In case
            // some driver goes crazy and starts writing its dma pointers before
            // we're back into hires, we don't want to render on the DOS screen.
            // By disabling the reassign, we lock the fifo at the unused channel
            // until we're ready to process more data.  They're welcome to fill their
            // buffer up -- it just won't go anywhere.
            //
            if (pDev->Vga.Enabled)
            {
                FIFOLOG(3, 1, 1);
                REG_WR_DRF_DEF(_PFIFO, _CACHES, _REASSIGN, _DISABLED);
            }

            //
            // Enable all PFIFO interrupt conditions using value
            // setup by HAL (see FIFO_CONTROL_INIT code for each chip).
            //
            REG_WR32(NV_PFIFO_INTR_0,     0xFFFFFFFF);
            REG_WR32(NV_PFIFO_INTR_EN_0,  pDev->Fifo.HalInfo.IntrEn0);

            break;
        case STATE_UNLOAD:
            nvHalFifoControl(pDev, FIFO_CONTROL_UNLOAD);
            break;
        case STATE_INIT:
            initFifo(pDev);
            initFifoEx(pDev);
            
            //
            // Allocate Hash Table structure.
            //
            if (osAllocMem((VOID **)&pDev->DBhashTable, HASH_DEPTH * sizeof(SW_HASH_TABLE_ENTRY)) != RM_OK)
                return (RM_ERR_NO_FREE_MEM);

            //
            // Initialize Hash Table.
            //    
            for (i = 0; i < HASH_DEPTH; i++)
            {
                pDev->DBhashTable[i].Object = NULL;
            }

            nvHalFifoControl(pDev, FIFO_CONTROL_INIT);
            break;
        case STATE_DESTROY:
            //
            // Free up allocated memory.
            //
            osFreeMem(pDev->DBfifoTable);
            osFreeMem(pDev->DBhashTable);
            break;
    }    
    return (RM_OK);
}
