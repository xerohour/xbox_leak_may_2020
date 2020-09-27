//
// (C) Copyright NVIDIA Corporation Inc., 1997-2000. All rights reserved.
//
/******************************** MP Manager ********************************\
*                                                                           *
* Module: MPSTATE.C                                                         *
*   The MediaPort state is changed in this module.                          *
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
#include <dma.h>
#include <modular.h>
#include <mp.h>
#include <os.h>
#include "nvhw.h"

//
// Registry data.
//
extern char strDevNodeRM[];
char strDisableMPC[] = "DisableMPCDetect";

//
// Change MediaPort hardware state.
//
RM_STATUS stateMp
(
    PHWINFO pDev,
    U032    msg
)
{
    switch (msg)
    {
        case STATE_UPDATE:
            nvHalMpControl(pDev, MP_CONTROL_UPDATE);
            break;
        case STATE_LOAD:
            nvHalMpControl(pDev, MP_CONTROL_LOAD);
            break;
        case STATE_UNLOAD:
            nvHalMpControl(pDev, MP_CONTROL_UNLOAD);
            break;
        case STATE_INIT:
        {
            U032 data32;

            //
            // Check if MPC Detection has been disabled.
            // It would be nice if this were made into a chip-independent
            // registry setting.
            //
            if (OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, strDisableMPC, &data32) == RM_OK)
            {
                pDev->MediaPort.HalInfo.DetectEnable = 1;
            }
            else
            {
                pDev->MediaPort.HalInfo.DetectEnable = 0;
            }

            //
            // Set initial state to unused.
            //
            pDev->MediaPort.CurrentDecoder = NULL;
            pDev->MediaPort.CurrentDecompressor = NULL;
            pDev->MediaPort.CurrentParallelBusObj = NULL;
            nvHalMpControl(pDev, MP_CONTROL_INIT);
            break;
        }
        case STATE_DESTROY:
            nvHalMpControl(pDev, MP_CONTROL_DESTROY);
            break;
    }
    return (RM_OK);
}


