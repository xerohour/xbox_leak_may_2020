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

/**************************** Graphics Manager *****************************\
*                                                                           *
* Module: GRSTATE.C                                                         *
*   The graphics state is changed in this module.                           *
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
#include <gr.h>
#include <os.h>
#include "nvhw.h"
//
// Externs.
//
extern char strDevNodeRM[];

//
// Change graphics hardware state.
//
RM_STATUS stateGr
(
    PHWINFO pDev,
    U032    msg
)
{
    U032    data32;

    switch (msg)
    {
        case STATE_UPDATE:
            break;
        case STATE_LOAD:
            //
            // Disable data from fifo.
            //
            REG_WR32(NV_PGRAPH_FIFO, 0x0);

            // Ignore return code.
            (void) nvHalGrControl(pDev, GR_CONTROL_LOAD);

            // save off this registry value (to be loaded later)
            if (IsNV15orBetter(pDev) &&
                (OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, "SwathMultWidth", &data32) == RM_OK))
                nvHalGrLoadOverride(pDev, GR_LOAD_OVERRIDE_SWATHCTL, data32 | SWATH_CONTROL_VALID_BIT);
            else
                nvHalGrLoadOverride(pDev, GR_LOAD_OVERRIDE_SWATHCTL, 0);

            //
            // Reset and enabled the interrupts.
            //
            REG_WR32(NV_PGRAPH_INTR,    0xFFFFFFFF);
            REG_WR32(NV_PGRAPH_INTR_EN, 0xFFFFFFFF);
            
            pDev->Graphics.Enabled = TRUE;
            
            break;
        case STATE_UNLOAD:
            if (pDev->Graphics.Enabled)
            {
                //
                // Set disabled flag.
                //
                pDev->Graphics.Enabled = FALSE;
                
                //
                // Idle the graphics engine.
                //
                // XXXscottl - Potential calls to any of the engine
                // interrupt service routines need to be mutexed.
                // We have a problem here because we can be called
                // as a result of a win9x class-driven modeset (in which
                // case the higher-level mcService routine will have
                // grabbed the spinlock) or because of a winnt-type
                // modeset (in which case no lock is held).
                //
                GR_IDLE();

                //
                // Pull out current graphics state.
                //
                (void) nvHalGrControl(pDev, GR_CONTROL_UNLOAD);
                
                //
                // Disable data access from fifo.
                //
                REG_WR32(NV_PGRAPH_FIFO,    0x00000000);
                REG_WR32(NV_PGRAPH_INTR_EN, 0x00000000);
            }
            break;
        case STATE_INIT:
        {
            U032 i;
            RM_STATUS status;

            //
            // Allocate/initialize the graphics engine class allocation
            // locks.
            //
            status = osAllocMem((VOID **)&pDev->Graphics.ClassLocks, sizeof (U032) * NUM_FIFOS);
            if (status)
                return (status);

            for (i = 0; i < NUM_FIFOS; i++)
            {
                ((U032 *)(pDev->Graphics.ClassLocks))[i] = 0;
            }

            pDev->Graphics.Enabled = FALSE;
            
            //
            // Handle any hardware dependent initialization now.
            //
            nvHalGrControl(pDev, GR_CONTROL_INIT);

            //
            // Clear VBlank list.
            //
            pDev->DBgrVBlankList[0] = NULL;
            pDev->DBgrVBlankList[1] = NULL;
            //
            // Load any overrides.
            //
            initGrEx(pDev);
            break;
        }
        case STATE_DESTROY:
            osFreeMem((VOID *)pDev->Graphics.ClassLocks);
            break;
    }    
    return (RM_OK);
}
