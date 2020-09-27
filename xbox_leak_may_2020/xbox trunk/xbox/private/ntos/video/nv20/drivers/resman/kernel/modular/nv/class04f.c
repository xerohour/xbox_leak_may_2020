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
#include <nvrm.h>
#include <nvhw.h>
#include <nv32.h>

//---------------------------------------------------------------------------
//
//  Create/Destroy object routines.
//
//---------------------------------------------------------------------------

//
// See kernel/mp/mpobj.c.
//

//---------------------------------------------------------------------------
//
//  Class method routines.
//
//---------------------------------------------------------------------------

RM_STATUS class04FSetContextDmaNotifies
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04FSetContextDmaNotifies\r\n");
    //
    // Set the notify DMA context.
    //
    return (mthdSetNotifyCtxDma(pDev, Object, Method, Offset, Data));
}

RM_STATUS class04FNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04FNotify\r\n");
    //
    // Set the notification style.
    //
    return (mthdSetNotify(pDev, Object, Method, Offset, Data));
}

RM_STATUS class04FStopTransfer
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PPARALLELBUSOBJECT pBus;
    RM_STATUS status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04FStopTransfer\r\n");

    pBus = (PPARALLELBUSOBJECT)Object;

    //
    // Probably should error check this data again NV04F_STOP_TRANSFER_VALUE.
    //
    (void) nvHalMpMethod(pDev, NV1_EXTERNAL_PARALLEL_BUS, &pBus->HalObject, NV04F_STOP_TRANSFER, Data, &status);

    return (status);
}

RM_STATUS class04FWrite
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    PPARALLELBUSOBJECT      pBus;
    RM_STATUS status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04FWrite\r\n");

    //
    // A nice cast to make the code more readable.
    //
    pBus = (PPARALLELBUSOBJECT)Object;

    (void) nvHalMpMethod(pDev, NV1_EXTERNAL_PARALLEL_BUS, &pBus->HalObject, Offset, Data, &status);

    return (status);
}


RM_STATUS class04FRead
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    PPARALLELBUSOBJECT      pBus;
    PPARALLELBUSHALOBJECT   pBusHalObj;
    RM_STATUS               status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04FRead\r\n");

    //
    // A nice cast to make the code more readable.
    //
    pBus = (PPARALLELBUSOBJECT)Object;
    pBusHalObj = (PPARALLELBUSHALOBJECT)&pBus->HalObject;

    (void) nvHalMpMethod(pDev, NV1_EXTERNAL_PARALLEL_BUS, &pBus->HalObject, Offset, Data, &status);

    //
    // If this is the transfer method, handle posting of the notify.
    //
    switch (Offset - Method->Low)
    {
        //
        // Get Data
        //
        case 0x08:

            //
            // Send back the data.
            // XXX (scottl):  should 'status' be written to notify
            // area in this case?
            //
            notifyFillNotifierArray(pDev,  pBus->CBase.NotifyXlate, 
                                    pBusHalObj->Read.ReadData,
                                    0, 
                                    RM_OK, 
                                    NV04F_NOTIFICATION_READ_GET_DATA_NOTIFY );
            
            //DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "r");
            
            //tmrDelay(100000);         // 500ns delay    
            
            break;
        default:
            break;            
    }

    return (status);
}

RM_STATUS class04FSetInterruptNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    PPARALLELBUSOBJECT   pBus;
    RM_STATUS status;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04FSetInterruptNotify\r\n");
    //
    // A nice cast to make the code more readable.
    //
    pBus = (PPARALLELBUSOBJECT)Object;

    //
    // Set the notify data
    //
    pBus->InterruptNotifyPending = Data;

    (void) nvHalMpMethod(pDev, NV1_EXTERNAL_PARALLEL_BUS, &pBus->HalObject, Offset, Data, &status);

    return (status);
}    


//---------------------------------------------------------------------------
//
// Exception Handling
//
//---------------------------------------------------------------------------

//
// Handle parallel bus portion of MediaPort exceptions.
//
V032 class04FService
(
    PHWINFO pDev
)
{
    PPARALLELBUSOBJECT pParallelBus;
    VOID *pMpHalObj;
    U032 eventPending = CLASS04F_PBUS_EVENT;
    V032 intrStatus = 0;

    //
    // Handle parallel bus exceptions.
    //
    pParallelBus = (PPARALLELBUSOBJECT)(pDev->MediaPort.CurrentParallelBusObj);
    if (pParallelBus)
        pMpHalObj = (VOID *)&pParallelBus->HalObject;
    else
        pMpHalObj = NULL;

    //
    // Call into the HAL to get pending status.
    //
    (void) nvHalMpGetEventStatus(pDev, NV01_EXTERNAL_PARALLEL_BUS, pMpHalObj, &eventPending, &intrStatus);

    //
    // Now process any pending events.
    //
    if (pParallelBus && eventPending == CLASS04F_PBUS_EVENT)
    {    
         notifyFillNotifierArray(pDev, pParallelBus->CBase.NotifyXlate, 
                                 0, 
                                 0, 
                                 RM_OK, 
                                 NV04F_NOTIFICATION_SET_INTERRUPT_NOTIFY );

        if (pParallelBus->InterruptNotifyPending)
            osNotifyEvent(pDev, (POBJECT)pParallelBus,
                          NV04F_NOTIFICATION_SET_INTERRUPT_NOTIFY,
                          0, 
                          1, 
                          RM_OK, 
                          pParallelBus->InterruptNotifyPending);

        //
        // This is a single-shot notify.  Clear the notify type.
        //
        pParallelBus->InterruptNotifyPending = 0;            
    }

    //
    // Call into HAL to finish processing exception (will clear
    // pending bit in interrupt register).
    //
    (void) nvHalMpServiceEvent(pDev, NV01_EXTERNAL_PARALLEL_BUS, pMpHalObj, eventPending, &intrStatus);

    return (intrStatus);
}
