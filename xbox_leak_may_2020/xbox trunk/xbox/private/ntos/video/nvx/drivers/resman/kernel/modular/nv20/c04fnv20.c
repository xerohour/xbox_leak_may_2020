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
* Module: CLASS04F.C                                                        *
*   This module implements the NV_EXTERNAL_PARALLEL_BUS object              *
*   class and its corresponding methods.                                    *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nv20_ref.h>
#include <nvrm.h>
#include <nv20_hal.h>
#include "nvhw.h"
#include "nvhalcpp.h"

//
// The following routines are used by the HAL mediaport engine
// manager in kernel/mp/nv20/mpnv20.c.
//
RM_STATUS class04FMethod_NV20(PMPMETHODARG_000);
RM_STATUS class04FGetEventStatus_NV20(PMPGETEVENTSTATUSARG_000);
RM_STATUS class04FServiceEvent_NV20(PMPSERVICEEVENTARG_000);

#define	VIPREG_TIMEOUT_CNT	1000		// XXX arbitrary

//---------------------------------------------------------------------------
//
//  Create/Destroy object routines.
//
//---------------------------------------------------------------------------

//
// Class instantiation/destruction is handled by mp/mpobj.c and
// mp/nv20/mpnv20.c.
//

//---------------------------------------------------------------------------
//
//  Class method routines.
//
//---------------------------------------------------------------------------

RM_STATUS class04FMethod_NV20
(
    PMPMETHODARG_000 pMpMethodArg
)
{
    PPARALLELBUSHALOBJECT pPBusHalObj;
    PHALHWINFO pHalHwInfo;
    U032 offset, data;
    PHWREG nvAddr;

    pPBusHalObj = (PPARALLELBUSHALOBJECT)pMpMethodArg->pHalObjInfo;
    offset = pMpMethodArg->offset;
    data = pMpMethodArg->data;
    pHalHwInfo = pMpMethodArg->pHalHwInfo;
    nvAddr = pHalHwInfo->nvBaseAddr;

    switch (offset)
    {
        case NV04F_STOP_TRANSFER:
            //
            // Stop either read or write
            //
            FLD_WR_DRF_DEF(_PME, _VIPREG_CTRL, _READ, _NOT_PENDING);
            FLD_WR_DRF_DEF(_PME, _VIPREG_CTRL, _WRITE, _NOT_PENDING);
            break;
        case NV04F_WRITE_SIZE:
            pPBusHalObj->Write.DataSize = (U016)(data & 0x0000FFFF);
            if (pPBusHalObj->Write.DataSize > 4)
            {
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: bad NV04F_WRITE_SIZE argument ", data);
                return NV04F_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            }
            pPBusHalObj->Write.AddressSize = (U016)((data >> 16) & 0x0000FFFF);
            if (pPBusHalObj->Write.AddressSize != 2)
            {
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: bad NV04F_WRITE_SIZE argument ", data);
                return NV04F_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            }
            break;
        case NV04F_WRITE_ADDRESS:
            pPBusHalObj->Write.Address = data;
            break;
        case NV04F_WRITE_DATA:
        {
            U032 vipCtrl, cnt;

            pPBusHalObj->Write.Data = data;
            
            //
            // Make sure there isn't a write pending
            //
            cnt = 0;
            do {
                vipCtrl = REG_RD32(NV_PME_VIPREG_CTRL);
                if ( ++cnt > VIPREG_TIMEOUT_CNT )
                {
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: NV04F_WRITE_DATA state in use\r\n");
                    return NV04F_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
                }
            } while ( vipCtrl & DRF_DEF(_PME, _VIPREG_CTRL, _WRITE, _PENDING) );
            
            //
            // Do the write now.
            //
            REG_WR32(NV_PME_VIPREG_NBYTES, pPBusHalObj->Write.DataSize);
            REG_WR32(NV_PME_VIPREG_ADDR, (pPBusHalObj->Write.Address & 0x0000FFFF));
            REG_WR32(NV_PME_VIPREG_DATA, pPBusHalObj->Write.Data);
            FLD_WR_DRF_DEF(_PME, _VIPREG_CTRL, _WRITE, _START);
            break;
        }
        case NV04F_READ_SIZE:
            pPBusHalObj->Read.DataSize = (U016)(data & 0x0000FFFF);
            if (pPBusHalObj->Read.DataSize > 4)
            {
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: bad NV04F_READ_SIZE argument ", data);
                return NV04F_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            }
            pPBusHalObj->Read.AddressSize = (U016)((data >> 16) & 0x0000FFFF);
            if (pPBusHalObj->Read.AddressSize != 2)
            {
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: bad NV04F_READ_SIZE argument ", data);
                return NV04F_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            }
            break;
        case NV04F_READ_ADDRESS:
            pPBusHalObj->Read.Address = data;
            break;
        case NV04F_READ_GET_DATA_NOTIFY:
        {
            U032 vipCtrl, cnt, data32;
            
            pPBusHalObj->Read.Notify = data;

            //
            // Make sure there isn't a read pending.
            //
            cnt = 0;
            do {
                vipCtrl = REG_RD32(NV_PME_VIPREG_CTRL);
                if ( ++cnt > VIPREG_TIMEOUT_CNT )
                {
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: NV04F_READ_GET_DATA_NOTIFY state in use\r\n");
                    return NV04F_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
                }
            } while ( vipCtrl & DRF_DEF(_PME, _VIPREG_CTRL, _READ, _PENDING) );
            
            //
            // Do the read now.
            //
            REG_WR32(NV_PME_VIPREG_NBYTES, pPBusHalObj->Read.DataSize);
            REG_WR32(NV_PME_VIPREG_ADDR, (pPBusHalObj->Read.Address & 0x0000FFFF));
            FLD_WR_DRF_DEF(_PME, _VIPREG_CTRL, _READ, _START);
            
            //
            // Wait for the read to complete.
            //
            cnt = 0;
            do {
                vipCtrl = REG_RD32(NV_PME_VIPREG_CTRL);
                if ( ++cnt > VIPREG_TIMEOUT_CNT )
                    return(RM_ERROR);
            } while ( vipCtrl & DRF_DEF(_PME, _VIPREG_CTRL, _READ, _PENDING) );
            
            data32 = REG_RD32(NV_PME_VIPREG_DATA);
            
            //
            // Save data off so that RM can return it via the notify.
            //
            pPBusHalObj->Read.ReadData = data32;
            break;
        }
        case NV04F_SET_INTERRUPT_NOTIFY:
            //
            // Make sure the interrupt is enabled
            //
            FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _REGTIMEOUT_NOTIFY, _ENABLED);
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

RM_STATUS class04FGetEventStatus_NV20
(
    PMPGETEVENTSTATUSARG_000 pMpGetEventStatusArg
)
{
    PHALHWINFO pHalHwInfo;
    PHWREG nvAddr;
    U032 intr0;

    pHalHwInfo = pMpGetEventStatusArg->pHalHwInfo;
    nvAddr = pHalHwInfo->nvBaseAddr;

    intr0 = REG_RD32(NV_PME_INTR_0);

    // XXX not final (need to finish porting class04F to NV20)

    //
    // Let RM know we've got a pbus event pending.
    //
    if (intr0 & DRF_DEF(_PME, _INTR_0, _REGTIMEOUT_NOTIFY, _PENDING))
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

RM_STATUS class04FServiceEvent_NV20
(
    PMPSERVICEEVENTARG_000 pMpServiceEventArg
)
{
    PHALHWINFO pHalHwInfo;
    PHWREG nvAddr;

    pHalHwInfo = pMpServiceEventArg->pHalHwInfo;
    nvAddr = pHalHwInfo->nvBaseAddr;

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
    REG_WR32(NV_PME_INTR_0, DRF_DEF(_PME, _INTR_0, _REGTIMEOUT_NOTIFY, _RESET));

    //
    // This is a single-shot notify.  Disable the interrupt.
    //
    FLD_WR_DRF_DEF(_PME, _INTR_EN_0, _REGTIMEOUT_NOTIFY, _DISABLED);

    //
    // Clear state in exception data structure.
    //
    pMpServiceEventArg->intrStatus = REG_RD32(NV_PME_INTR_0);

    return RM_OK;
}
