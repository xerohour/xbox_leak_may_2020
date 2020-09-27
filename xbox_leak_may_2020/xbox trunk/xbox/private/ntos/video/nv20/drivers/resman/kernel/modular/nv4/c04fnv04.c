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

/************************ Parallel Bus Manager ******************************\
*                                                                           *
* Module: C04FNV04.C                                                        *
*   This module implements the nv4-specific portions of the                 *
*   NV_EXTERNAL_PARALLEL_BUS class (methods, etc.).                         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nv4_ref.h>
#include <nvrm.h>
#include <nv4_hal.h>
#include "nvhw.h"
#include "nvhalcpp.h"

//
// The following routines are used by the HAL mediaport engine
// manager in kernel/mp/nv4/mpnv04.c.
//
RM_STATUS class04FMethod_NV04(PMPMETHODARG_000);
RM_STATUS class04FGetEventStatus_NV04(PMPGETEVENTSTATUSARG_000);
RM_STATUS class04FServiceEvent_NV04(PMPSERVICEEVENTARG_000);

//---------------------------------------------------------------------------
//
//  Create/Destroy object routines.
//
//---------------------------------------------------------------------------

//
// Class instantiation/destruction is handled by mp/mpobj.c and
// mp/nv4/mpnv04.c.
//

//---------------------------------------------------------------------------
//
//  Class method routines.
//
//---------------------------------------------------------------------------

RM_STATUS class04FMethod_NV04
(
    PMPMETHODARG_000 pMpMethodArg
)
{
    PPARALLELBUSHALOBJECT pPBusHalObj;
    PHALHWINFO pHalHwInfo;
    U032 offset, data, data32;
    S032 saddr;

    pPBusHalObj = (PPARALLELBUSHALOBJECT)pMpMethodArg->pHalObjInfo;
    offset = pMpMethodArg->offset;
    data = pMpMethodArg->data;
    pHalHwInfo = pMpMethodArg->pHalHwInfo;

    switch (offset)
    {
        case NV04F_WRITE_SIZE:
            pPBusHalObj->Write.DataSize = (U016)(data & 0x0000FFFF);
            pPBusHalObj->Write.AddressSize = (U016)((data >> 16) & 0x0000FFFF);
            break;
        case NV04F_WRITE_ADDRESS:
            pPBusHalObj->Write.Address = data;
            break;
        case NV04F_WRITE_DATA:
            pPBusHalObj->Write.Data = data;

            //
            // XXX allow for negative addresses to be passed in so we
            // can access the MPC (temporary)
            //
            saddr = (S032)(pPBusHalObj->Write.Address) + 0x200;

            //
            // PME_EXTERNAL_BUS is an 8bit bus, so we must break
            // up larger transfers
            //
            switch (pPBusHalObj->Write.DataSize)
            {
                // 8bit access
                case 0x0001:
                    REG_WR32(NV_PME_EXTERNAL(saddr), pPBusHalObj->Write.Data);
                    break;

                    // 16bit access
                case 0x0002:
                    REG_WR32(NV_PME_EXTERNAL(saddr), pPBusHalObj->Write.Data & 0xFF);
                    REG_WR32(NV_PME_EXTERNAL(saddr+1), (pPBusHalObj->Write.Data >> 8) & 0xFF);
                    break;

                    // 24bit access
                case 0x0003:
                    /* ???
                       REG_WR32(NV_PME_EXTERNAL(0x200+pBus->Write.Address)+0x200, pBus->Write.Data & 0xFF);
                       REG_WR32(NV_PME_EXTERNAL(0x200+pBus->Write.Address+1)+0x200, (pBus->Write.Data >> 8) & 0xFF);
                       REG_WR32(NV_PME_EXTERNAL(0x200+pBus->Write.Address+2)+0x200, (pBus->Write.Data >> 16) & 0xFF);
                       */
                    REG_WR32(NV_PME_EXTERNAL(saddr), pPBusHalObj->Write.Data & 0xFF);
                    REG_WR32(NV_PME_EXTERNAL(saddr+1), (pPBusHalObj->Write.Data >> 8) & 0xFF);
                    REG_WR32(NV_PME_EXTERNAL(saddr+2), (pPBusHalObj->Write.Data >> 16) & 0xFF);
                    break;

                    // 32bit access
                case 0x0004:
                    REG_WR32(NV_PME_EXTERNAL(saddr), pPBusHalObj->Write.Data & 0xFF);
                    REG_WR32(NV_PME_EXTERNAL(saddr+1), (pPBusHalObj->Write.Data >> 8) & 0xFF);
                    REG_WR32(NV_PME_EXTERNAL(saddr+2), (pPBusHalObj->Write.Data >> 16) & 0xFF);
                    REG_WR32(NV_PME_EXTERNAL(saddr+3), (pPBusHalObj->Write.Data >> 24) & 0xFF);

                    /* stuff specific to CCube
                       REG_WR32(NV_PME_EXTERNAL(0x06)+0x200, (pBus->Write.Address >> 16) & 0xFF);
                       REG_WR32(NV_PME_EXTERNAL(0x05)+0x200, (pBus->Write.Address >> 8)  & 0xFF);
                       REG_WR32(NV_PME_EXTERNAL(0x04)+0x200, (pBus->Write.Address)       & 0xFF);
                    
                       REG_WR32(NV_PME_EXTERNAL(0x03)+0x200, (pBus->Write.Data >> 24) & 0xFF);
                       REG_WR32(NV_PME_EXTERNAL(0x02)+0x200, (pBus->Write.Data >> 16) & 0xFF);
                       REG_WR32(NV_PME_EXTERNAL(0x01)+0x200, (pBus->Write.Data >> 8)  & 0xFF);
                       REG_WR32(NV_PME_EXTERNAL(0x00)+0x200, (pBus->Write.Data)       & 0xFF);
                       */
                    
                    //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: ClassNV04F_NOTIFICATION_SET_INTERRUPT_NOTIFY04F Write (index): ", pBus->Write.Address);
                    break;
            }
            
            //DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "w");
            
            // Impose a bit of a break
            //tmrDelay(100000);         // 500ns delay    
            break;
        case NV04F_READ_SIZE:
            pPBusHalObj->Read.DataSize = (U016)(data & 0x0000FFFF);
            pPBusHalObj->Read.AddressSize = (U016)((data >> 16) & 0x0000FFFF);
            break;
        case NV04F_READ_ADDRESS:
            pPBusHalObj->Read.Address = data;
            break;
        case NV04F_READ_GET_DATA_NOTIFY:
            pPBusHalObj->Read.Notify = data;

            //
            // XXX allow for negative addresses to be passed in so
            // we can access the MPC (temporary)
            //
            saddr = (S032)(pPBusHalObj->Read.Address) + 0x200;

            //
            // PME_EXTERNAL_BUS is an 8bit bus, so we must break
            // up larger transfers
            //
            switch (pPBusHalObj->Read.DataSize)
            {
                // 8bit access
                case 0x0001:
                    data32 = REG_RD32(NV_PME_EXTERNAL(saddr));
                    break;

                    // 16bit access
                case 0x0002:
                    data32 = REG_RD32(NV_PME_EXTERNAL(saddr+1));
                    data32 <<= 8;
                    data32 |= REG_RD32(NV_PME_EXTERNAL(saddr));
                    break;

                    // 24bit access
                case 0x0003:
                    data32 = REG_RD32(NV_PME_EXTERNAL(saddr+2));
                    data32 <<= 8;
                    data32 |= REG_RD32(NV_PME_EXTERNAL(saddr+1));
                    data32 <<= 8;
                    data32 |= REG_RD32(NV_PME_EXTERNAL(saddr));
                    break;

                    // 32bit access
                case 0x0004:
                    data32 = REG_RD32(NV_PME_EXTERNAL(saddr+3));
                    data32 <<= 8;
                    data32 |= REG_RD32(NV_PME_EXTERNAL(saddr+2));
                    data32 <<= 8;
                    data32 |= REG_RD32(NV_PME_EXTERNAL(saddr+1));
                    data32 <<= 8;
                    data32 |= REG_RD32(NV_PME_EXTERNAL(saddr));
                    
                    /* stuff specific to CCube
                       REG_WR32(NV_PME_EXTERNAL(0x06)+0x200, (pBus->Read.Address >> 16) & 0xFF);
                       REG_WR32(NV_PME_EXTERNAL(0x05)+0x200, (pBus->Read.Address >> 8)  & 0xFF);
                       REG_WR32(NV_PME_EXTERNAL(0x04)+0x200, (pBus->Read.Address)       & 0xFF);
                    
                       data32 = REG_RD32(NV_PME_EXTERNAL(0x03)+0x200);
                       data32 <<= 8;
                       data32 |= REG_RD32(NV_PME_EXTERNAL(0x02)+0x200);
                       data32 <<= 8;
                       data32 |= REG_RD32(NV_PME_EXTERNAL(0x01)+0x200);
                       data32 <<= 8;
                       data32 |= REG_RD32(NV_PME_EXTERNAL(0x00)+0x200);
                       */

                    break;
            }

            //
            // Save data off so that RM can return it via the notify.
            //
            pPBusHalObj->Read.ReadData = data32;
            break;
        case NV04F_SET_INTERRUPT_NOTIFY:
            //
            // Make sure the interrupt is enabled
            //
            FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _VMI, _ENABLED);
            break;
        default:
            // HAL doesn't have anything to do for this particular method
            ;
    }
    return NV04F_NOTIFICATION_STATUS_DONE_SUCCESS;
}

//---------------------------------------------------------------------------
//
//  Exception handling routines
//
//---------------------------------------------------------------------------

RM_STATUS class04FGetEventStatus_NV04
(
    PMPGETEVENTSTATUSARG_000 pMpGetEventStatusArg
)
{
    PHALHWINFO pHalHwInfo;
    U032 intr0;

    pHalHwInfo = pMpGetEventStatusArg->pHalHwInfo;

    intr0 = REG_RD32(NV_PME_INTR_0);

    //
    // If there's a VMI interrupt pending, then we need
    // to let the RM know so it can post notifies.
    //
    if (intr0 & DRF_DEF(_PME, _INTR_0, _VMI, _PENDING))
    {
        pMpGetEventStatusArg->events = CLASS04F_PBUS_EVENT;
    }
    else
    {
        pMpGetEventStatusArg->events = 0;
    }

    //
    // This value is given back in the service routine so we know
    // which exceptions we are to clear.
    //
    pMpGetEventStatusArg->intrStatus = intr0;

    return RM_OK;
}

RM_STATUS class04FServiceEvent_NV04
(
    PMPSERVICEEVENTARG_000 pMpServiceEventArg
)
{
    PHALHWINFO pHalHwInfo;

    pHalHwInfo = pMpServiceEventArg->pHalHwInfo;

    //
    // The intrStatus value was saved off in GetEventStatus.
    //
    if (pMpServiceEventArg->event != CLASS04F_PBUS_EVENT)
    {
        pMpServiceEventArg->intrStatus = REG_RD32(NV_PME_INTR_0);
        return RM_ERR_BAD_ARGUMENT;
    }

    //
    // Clear the interrupt
    //
    REG_WR32(NV_PME_INTR_0, DRF_DEF(_PME, _INTR_0, _VMI, _RESET));

    //
    // This is a single-shot notify.  Disable the interrupt.
    //
    FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _VMI, _DISABLED);

    pMpServiceEventArg->intrStatus = REG_RD32(NV_PME_INTR_0);

    return RM_OK;
}

