 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1997 NVIDIA, Corp.  All rights reserved.        *|
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

/********************************* DMA Manager *****************************\
*                                                                           *
* Module: NOTIFY.C                                                          *
*   Method notifications are handled in this module.  DMA report and OS     *
*   action are dealt with on a per-object basis.                            *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <class.h>
#include <fifo.h>
#include <dma.h>   
#include <os.h>
#include <nv32.h>
#include "nvhw.h"
//
// Method notifications.
//
RM_STATUS mthdSetNotifyCtxDma
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    RM_STATUS status;
    
    if (Object->NotifyTrigger)
        return (RM_ERR_DMA_IN_USE);
    //
    // Check for previous dma buffer.
    //
	/* KJK
    if (Object->NotifyXlate)
    {
        dmaEndXfer(Object->NotifyXlate, NULL);
        dmaDetach(Object->NotifyXlate);
    }
	*/

    //
    // Connect to dma buffer.
    //    
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &(Object->NotifyXlate));
    if (status)
    {
        osError(Object, Offset, Data, status);
        return (status);
    }
	/* KJK
    status = dmaAttach(Object->NotifyXlate);
    if (status)
    {
        osError(Object, Offset, Data, status);
        Object->NotifyXlate   = NULL;
        return (status);
    }
	*/
    //
    // Validate notification parameters.
    //
    status = dmaValidateXlate(Object->NotifyXlate, 0, 0x000F);
    if (status)
    {
        Object->NotifyXlate   = NULL;
        osError(Object, Offset, Data, status);
        return (status);
    }
	/* KJK
    status = dmaBeginXfer(Object->NotifyXlate, NULL);
    if (status)
    {
        Object->NotifyXlate   = NULL;
        osError(Object, Offset, Data, status);
    }
	*/
    return (status);
}
RM_STATUS mthdSetNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    Object->NotifyAction  = Data;
    Object->NotifyTrigger = TRUE;
    return (RM_OK);
}
//
// Dual buffer notifications.
//
RM_STATUS mthdSetBufferNotifyCtxDma
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PBUFFEROBJECT BuffObject;
    U032          BuffNum;

    BuffObject = (PBUFFEROBJECT)Object;
    BuffNum    = (Offset - Method->Low) >> 2;
#ifdef DEBUG
    if (BuffNum > 1)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Invalid buffer number!", BuffNum);
        DBG_BREAKPOINT();
    }
#endif
    return(notifySetBufferNotifyCtxDma(pDev, BuffObject, &BuffObject->Buffer[BuffNum], Offset, Data));
}
RM_STATUS notifySetBufferNotifyCtxDma
(
	PHWINFO       pDev,
    PBUFFEROBJECT BuffObject,
    PBUFFER       pBuffer,
    U032          Offset,
    U032          Data
)
{
    RM_STATUS status;

    //
    // Check for busy buffer.
    //
    if (pBuffer->State == BUFFER_BUSY)
    {
        osError(&BuffObject->Base, Offset, Data, RM_ERR_DMA_IN_USE);
        return (RM_ERR_DMA_IN_USE);
    }
    //
    // Check for previous dma buffer.
    //
	/* KJK
    if (pBuffer->NotifyXlate)
        dmaDetach(pBuffer->NotifyXlate);
	*/

    //
    // Connect to dma buffer.
    //    
    status = dmaValidateObjectName(pDev, Data,
                                   BuffObject->Base.ChID,
                                   &(pBuffer->NotifyXlate));
    if (status)
    {
        pBuffer->NotifyXlate = NULL;
        return (status);
    }
	/* KJK
    status = dmaAttach(pBuffer->NotifyXlate);
    if (status)
    {
        pBuffer->NotifyXlate = NULL;
        return (status);
    }
	*/
    return (status);
}
RM_STATUS mthdSetBufferNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PBUFFEROBJECT BuffObject;
    U032          BuffNum;

    BuffObject = (PBUFFEROBJECT)Object;
    BuffNum    = (Offset - Method->Low) >> 2;
#ifdef DEBUG
    if (BuffNum > 1)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Invalid buffer number!", BuffNum);
        DBG_BREAKPOINT();
    }
#endif
    return(notifySetBufferNotify(pDev, BuffObject, BuffNum, Offset, Data));
}
RM_STATUS notifySetBufferNotify
(
	PHWINFO       pDev,
    PBUFFEROBJECT BuffObject,
    U032          BuffNum,
    U032          Offset,
    U032          Data
)
{
    RM_STATUS status;
    PBUFFER   pBuffer = &BuffObject->Buffer[BuffNum];

    //
    // Check for busy buffer.
    //
    if (pBuffer->State == BUFFER_BUSY)
    {
        osError(&BuffObject->Base, Offset, Data, RM_ERR_DMA_IN_USE);
        return (RM_ERR_DMA_IN_USE);
    }
    pBuffer->NotifyAction = Data;
    //
    // Validate notification parameters.
    //    
    status = dmaValidateXlate(pBuffer->NotifyXlate, 0, 0x000F);
    if (status)
    {
        osError(&BuffObject->Base, Offset, Data, status);
        return (status);
    }
	/* KJK
    status = dmaBeginXfer(pBuffer->NotifyXlate, NULL);
    if (status)
    {
        osError(&BuffObject->Base, Offset, Data, status);
        return (status);
    }
	*/
    //
    // Validate buffer parameters.
    //    
    status = dmaValidateXlate(pBuffer->Xlate,
                              pBuffer->Start,
                              pBuffer->Length);
    if (status)
    {
        notifyFillNotifier(pDev, pBuffer->NotifyXlate, 0, 0, status);
        //KJK dmaEndXfer(pBuffer->NotifyXlate, NULL);
        if (pBuffer->NotifyAction)
        {
            osNotifyEvent(pDev,
                          (POBJECT)BuffObject,
            			  0,
                          Offset,
                          BuffObject->Base.Name,
                          status,
                          pBuffer->NotifyAction);
        }
        return (status);
    }
	/* KJK
    status = dmaBeginXfer(pBuffer->Xlate, NULL);
    if (status)
    {
        notifyFillNotifier(pDev, pBuffer->NotifyXlate, 0, 0, status);
        dmaEndXfer(pBuffer->NotifyXlate, NULL);
        if (pBuffer->NotifyAction)
        {
            osNotifyEvent((POBJECT)BuffObject,
            			  0,
                          Offset,
                          BuffObject->Base.Name,
                          status,
                          pBuffer->NotifyAction);
        }
        return (status);
    }
	*/
    //
    // Call the object specific routine to begin the transfer.
    //
    BuffObject->BufferComplete                     = notifyBufferComplete;
    pBuffer->State                                 = BUFFER_BUSY;
    pBuffer->NotifyCurrentCount                    = pBuffer->NotifyCount;
    if (pBuffer->Length == 0)
    {
        pBuffer->NotifyCurrentCount = 0;
        notifyBufferComplete(pDev, BuffObject, BuffNum, BuffObject->Base.Name, RM_OK);
    }
    else
    {
        status = BuffObject->BufferXfer(pDev, BuffObject, BuffNum);
        if (status)
            notifyBufferComplete(pDev, BuffObject, BuffNum, BuffObject->Base.Name, status);
    }
    return (status);
}
RM_STATUS mthdSetBufferNotifyCount
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PBUFFEROBJECT BuffObject;
    U032          BuffNum;

    //
    // Update the count even if buffer is busy.
    //
    BuffObject = (PBUFFEROBJECT)Object;
    BuffNum    = (Offset - Method->Low) >> 2;
#ifdef DEBUG
    if (BuffNum > 1)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Invalid buffer number!", BuffNum);
        DBG_BREAKPOINT();
    }
#endif
    BuffObject->Buffer[BuffNum].NotifyCount        = Data;
    BuffObject->Buffer[BuffNum].NotifyCurrentCount = Data;
    return (RM_OK);
}
RM_STATUS mthdSetBufferPosNotifyCtxDma
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    RM_STATUS     status;
    PBUFFEROBJECT BuffObject;
    U032          BuffNum;

    BuffObject = (PBUFFEROBJECT)Object;
    BuffNum    = (Offset - Method->Low) >> 2;
    //
    // Check for previous dma buffer.
    //
	/* KJK
    if (BuffObject->Buffer[BuffNum].PosXlate)
    {
        dmaEndXfer(BuffObject->Buffer[BuffNum].PosXlate, NULL);
        dmaDetach(BuffObject->Buffer[BuffNum].PosXlate);
    }
	*/

    //
    // Connect to dma buffer.
    //    
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &(BuffObject->Buffer[BuffNum].PosXlate));
    if (status)
    {
        BuffObject->Buffer[BuffNum].PosXlate = NULL;
        return (status);
    }
	/* KJK
    status = dmaAttach(BuffObject->Buffer[BuffNum].PosXlate);
    if (status)
    {
        BuffObject->Buffer[BuffNum].PosXlate = NULL;
        return (status);
    }
	*/
    //
    // Validate notification parameters.
    //
    status = dmaValidateXlate(BuffObject->Buffer[BuffNum].PosXlate, 0, 0x000F);
    if (status)
    {
        BuffObject->Buffer[BuffNum].PosXlate = NULL;
        osError(Object, Offset, Data, status);
        return (status);
    }
	/* KJK
    status = dmaBeginXfer(BuffObject->Buffer[BuffNum].PosXlate, NULL);
    if (status)
    {
        BuffObject->Buffer[BuffNum].PosXlate = NULL;
        osError(Object, Offset, Data, status);
    }
	*/
    return (status);
}
RM_STATUS mthdSetBufferPosNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    RM_STATUS     status;
    PBUFFEROBJECT BuffObject;
    U032          BuffNum;
    U032          Position;

    BuffObject = (PBUFFEROBJECT)Object;
    BuffNum    = (Offset - Method->Low) >> 2;
    status     = BuffObject->BufferPos(pDev, BuffObject, BuffNum, &Position);
    if (status)
        return (status);
    notifyFillNotifier(pDev, BuffObject->Buffer[BuffNum].PosXlate, Position, 0, 0);
    return (RM_OK);
}

//---------------------------------------------------------------------------
//
//  Notification completion.
//
//---------------------------------------------------------------------------

RM_STATUS notifyDefaultError
(
	PHWINFO   pDev,
    POBJECT   Object,
    U032      Offset,
    V032      Data,
    U032      ChID,
    RM_STATUS CompletionStatus
)
{
    V032       ObjectName;
    PDMAOBJECT ErrNotifier;
    
    //
    // Check for error.
    //
    if (CompletionStatus & 0xFFFF)
    {
        //
        // Fill NV_ERROR_NOTIFIER if defined.
        //
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_USERERRORS, "NVRM: method error ", CompletionStatus);
        if (Object)
        {
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_USERERRORS, "            object ", (U032)Object->Name);
            ObjectName = Object->Name;
        }
        else    
        {
            DBG_PRINT_STRING(DEBUGLEVEL_USERERRORS, "            object NULL");
            ObjectName = 0;
        }
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_USERERRORS, "            offset ", Offset);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_USERERRORS, "              data ", Data);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_USERERRORS, "              chID ", ChID);
        if (fifoSearchObject(pDev, NV_CONTEXT_ERROR_TO_MEMORY, ChID, (POBJECT *)&ErrNotifier) == RM_OK)
        {
            if (ErrNotifier->Valid)
            {
                if (!dmaValidateXlate(ErrNotifier, 0, 0x000F))
                {
                    //KJK dmaAttach(ErrNotifier);
                    //KJK dmaBeginXfer(ErrNotifier, NULL);
                    notifyFillNotifier(pDev, ErrNotifier, ObjectName, (V016)Offset, CompletionStatus);
					/* KJK
                    dmaEndXfer(ErrNotifier, NULL);
                    dmaDetach(ErrNotifier);
					*/
                }
            }
        }
        //
        // Do any OS specific action.
        //
        if (Object)
            osError(Object, Offset, Data, CompletionStatus);
        else    
            osError(Object, Offset, ChID, CompletionStatus);
    }
    return (RM_OK);
}
RM_STATUS notifyMethodComplete
(
	PHWINFO	  pDev,
    POBJECT   Object,
    U032      Offset,
    V032      Data,
    RM_STATUS CompletionStatus
)
{
    if ((Offset == 0x100) || (Offset >= 0x0200))
    {
        if (Object->NotifyTrigger)
        {
            Object->NotifyTrigger = FALSE;
            if (Object->NotifyXlate)
                notifyFillNotifier(pDev, Object->NotifyXlate, Object->Name, (V016)Offset, CompletionStatus);
            //
            // Do any OS specified action related to this notification.
            //
            if (Object->NotifyAction)
                osNotifyEvent(pDev, Object, 0, Offset, Data, CompletionStatus, Object->NotifyAction);
        }
    }
    return (RM_OK);
}
RM_STATUS notifyBufferHWComplete
(
	PHWINFO	  pDev, 
    POBJECT   Object,
    U032      Offset,
    V032      Data,
    U032      Buffer,    
    RM_STATUS CompletionStatus
)
{
    if (Offset >= 0x0200)
    {
        if (Object->NotifyTrigger)
        {
            Object->NotifyTrigger = FALSE;
            if (Object->NotifyXlate)
                notifyFillNotifierArray(pDev, Object->NotifyXlate, Object->Name, (V016)Offset, CompletionStatus, Buffer);
            //
            // Do any OS specified action related to this notification.
            //
            if (Object->NotifyAction)
                osNotifyEvent(pDev, Object, Buffer, Offset, Data, CompletionStatus, Object->NotifyAction);
        }
    }
    return (RM_OK);
}
RM_STATUS notifyBufferComplete
(
	PHWINFO       pDev,
    PBUFFEROBJECT BuffObject,
    U032          BuffNum,
    V032          OtherInfo,
    RM_STATUS     CompletionStatus
)
{
    RM_STATUS status;
    PBUFFER   pBuffer = &BuffObject->Buffer[BuffNum];

    if ((OtherInfo == NOTIFY_INFO16_BUFFER_YANK)
    ||  (pBuffer->NotifyCurrentCount == 0))
    {
        //
        // Complete buffer with any notify action requested.
        //
        if (pBuffer->NotifyXlate)
            notifyFillNotifier(pDev, pBuffer->NotifyXlate, 0, (V016)OtherInfo, CompletionStatus);
        pBuffer->State = BUFFER_IDLE;
        //
        // Do any OS specified action related to this notification.
        //
        if (pBuffer->NotifyAction)
        {
            osNotifyEvent(pDev,
                          (POBJECT)BuffObject,
            			  0,
                          pBuffer->NotifyMethod,
                          BuffObject->Base.Name,
                          CompletionStatus,
                          pBuffer->NotifyAction);
        }
    }
    else
    {
        //
        // Auto notify buffer again.
        //
        if (pBuffer->NotifyXlate)
            notifyFillNotifier(pDev, pBuffer->NotifyXlate,
                               pBuffer->NotifyCurrentCount,
                               NOTIFY_INFO16_BUFFER_LOOP,
                               CompletionStatus);
        if (pBuffer->NotifyCount != 0xFFFFFFFF)
            pBuffer->NotifyCurrentCount--;
        status = BuffObject->BufferXfer(pDev, BuffObject, BuffNum);
        if (status)
        {
            //
            // Stop looping and return error.
            //
            pBuffer->NotifyCurrentCount = 0;
            notifyBufferComplete(pDev, BuffObject, BuffNum, OtherInfo, status);
        }
    }
    return (RM_OK);
}
RM_STATUS notifyFillNotifier
(
	PHWINFO	   pDev,
    PDMAOBJECT NotifyXlate,
    V032       Info32,
    V016       Info16,
    RM_STATUS  CompletionStatus
)
{
    RM_STATUS     status;
    PNOTIFICATION NotifyBuffer;

    //
    // Fill in the notification structure.
    //
    status = dmaGetMappedAddress(pDev, NotifyXlate, 0, 0x000F, (VOID **)&(NotifyBuffer));
    if (status)
        return (status);
    NotifyBuffer->OtherInfo32 = Info32;
    NotifyBuffer->OtherInfo16 = Info16;
    tmrGetCurrentTime(pDev, &NotifyBuffer->TimeHi, &NotifyBuffer->TimeLo);
    NotifyBuffer->Status = (V016)CompletionStatus;
    return (RM_OK);
}

RM_STATUS notifyFillNotifierArray
(
	PHWINFO	   pDev,
    PDMAOBJECT NotifyXlate,
    V032       Info32,
    V016       Info16,
    RM_STATUS  CompletionStatus,
    U032       Index
)
{
    RM_STATUS     status;
    PNOTIFICATION NotifyBuffer;

    //
    // Fill in the notification structure.
    //
    status = dmaGetMappedAddress(pDev, NotifyXlate, (Index*0x10), 0x000F, (VOID **)&(NotifyBuffer));
    if (status)
        return (status);
    NotifyBuffer->OtherInfo32 = Info32;
    NotifyBuffer->OtherInfo16 = Info16;
    tmrGetCurrentTime(pDev, &NotifyBuffer->TimeHi, &NotifyBuffer->TimeLo);
    NotifyBuffer->Status = (V016)CompletionStatus;
    return (RM_OK);
}
