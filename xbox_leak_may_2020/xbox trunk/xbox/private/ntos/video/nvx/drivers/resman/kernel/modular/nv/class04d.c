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

/****************************************************************************\
*                                                                           *
* Module: CLASS04D.C                                                        *
*   This module implements the NV03_EXTERNAL_VIDEO_DECODER methods.         *
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

// See kernel/mp/mpobj.c.

//---------------------------------------------------------------------------
//
//  Class method routines.
//
//---------------------------------------------------------------------------

RM_STATUS class04DNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04DNotify\r\n");
    //
    // Set the notify DMA context
    //
    return (mthdSetNotify(pDev, Object, Method, Offset, Data));
}
RM_STATUS class04DStopTransferVbi
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEODECODEROBJECT pDecoder;
    RM_STATUS status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04DStopTransferVbi\r\n");

    pDecoder = (PVIDEODECODEROBJECT)Object;

    //
    // Stop the current vbi data transfer immediately.
    //

    //
    // Probably should error check this data again
    // NV04F_STOP_TRANSFER_VBI_VALUE.
    //
    (void) nvHalMpMethod(pDev, NV03_EXTERNAL_VIDEO_DECODER, &pDecoder->HalObject, NV04D_STOP_TRANSFER_VBI, Data, &status);

	 return (status);
}

RM_STATUS class04DStopTransferImage
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEODECODEROBJECT pDecoder;
    RM_STATUS status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04DStopTransferImage\r\n");

    //
    // A nice cast to make the code more readable.
    //
    pDecoder = (PVIDEODECODEROBJECT)Object;

    //
    // Probably should error check this data again
    // NV04F_STOP_TRANSFER_IMAGE_VALUE.
    //
    (void) nvHalMpMethod(pDev, NV03_EXTERNAL_VIDEO_DECODER, &pDecoder->HalObject, NV04D_STOP_TRANSFER_IMAGE, Data, &status);

    return (status);
}

RM_STATUS class04DSetContextDmaNotifies
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04DSetContextDmaNotifies\r\n");
    //
    // Set the notify DMA context
    //
    return (mthdSetNotifyCtxDma(pDev, Object, Method, Offset, Data));
}

RM_STATUS class04DSetContextDmaImage
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEODECODEROBJECT         pDecoder;
    U032                        Buffer;
    RM_STATUS                   status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04DSetContextDmaImage\r\n");
    
    //
    // A nice cast to make the code more readable.
    //
    pDecoder = (PVIDEODECODEROBJECT)Object;

    Buffer = (Offset - Method->Low) >> 2;

	if (Data == NV01_NULL_OBJECT)
    {
       	pDecoder->ContextDmaImage[Buffer] = NULL;
        pDecoder->HalObject.SetImage[Buffer].CtxDma = NULL;
        return NV04D_NOTIFICATION_STATUS_DONE_SUCCESS;
    }

    //
    // Set the image DMA contexts
    //
    // Connect to dma buffer.
    //    
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &(pDecoder->ContextDmaImage[Buffer]));
    if (status)
        return (status);

    //
    // Give HAL access to context dma.
    //
    pDecoder->HalObject.SetImage[Buffer].CtxDma = &pDecoder->ContextDmaImage[Buffer]->HalInfo;

    return NV04D_NOTIFICATION_STATUS_DONE_SUCCESS;    
}

RM_STATUS class04DSetContextDmaVbi
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEODECODEROBJECT         pDecoder;
    U032                        Buffer;
    RM_STATUS                   status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04DSetContextDmaVbi\r\n");
    
    //
    // A nice cast to make the code more readable.
    //
    pDecoder = (PVIDEODECODEROBJECT)Object;

    Buffer = (Offset - Method->Low) >> 2;

	if (Data == NV01_NULL_OBJECT)
    {
       	pDecoder->ContextDmaVbi[Buffer] = NULL;
        pDecoder->HalObject.SetVbi[Buffer].CtxDma = NULL;
        return NV04D_NOTIFICATION_STATUS_DONE_SUCCESS;
    }

    //
    // Set the data DMA contexts
    //
    // Connect to dma buffer.
    //    
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &(pDecoder->ContextDmaVbi[Buffer]));
    if (status)
        return (status);

    //
    // Give HAL access to context dma.
    //
    pDecoder->HalObject.SetVbi[Buffer].CtxDma = &pDecoder->ContextDmaVbi[Buffer]->HalInfo;
    
    return (RM_OK);
}

RM_STATUS class04DSetImageConfig
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    PVIDEODECODEROBJECT         pDecoder;
    RM_STATUS status;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04DSetImageStartLine\r\n");

    pDecoder = (PVIDEODECODEROBJECT)Object;

    (void) nvHalMpMethod(pDev, NV03_EXTERNAL_VIDEO_DECODER, &pDecoder->HalObject, NV04D_SET_IMAGE_CONFIG, Data, &status);
    
    return (status);
    
}

RM_STATUS class04DSetImageStartLine
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    PVIDEODECODEROBJECT pDecoder;
    RM_STATUS status;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04DSetImageStartLine\r\n");
    
    //
    // A nice cast to make the code more readable.
    //
    pDecoder = (PVIDEODECODEROBJECT)Object;

    (void) nvHalMpMethod(pDev, NV03_EXTERNAL_VIDEO_DECODER, &pDecoder->HalObject, NV04D_SET_IMAGE_START_LINE, Data, &status);

    return (status);
    
}

RM_STATUS class04DSetVbi
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    PVIDEODECODEROBJECT pDecoder;
    RM_STATUS status;


    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04DSetVbi\r\n");
    //
    // A nice cast to make the code more readable.
    //
    pDecoder = (PVIDEODECODEROBJECT)Object;

    (void) nvHalMpMethod(pDev, NV03_EXTERNAL_VIDEO_DECODER, &pDecoder->HalObject, Offset, Data, &status);

    return (status);
}

RM_STATUS class04DSetImage
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    PVIDEODECODEROBJECT pDecoder;
    RM_STATUS status;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04DSetImage\r\n");
    //
    // A nice cast to make the code more readable.
    //
    pDecoder = (PVIDEODECODEROBJECT)Object;

    (void) nvHalMpMethod(pDev, NV03_EXTERNAL_VIDEO_DECODER, &pDecoder->HalObject, Offset, Data, &status);

    return (status);
}

RM_STATUS class04DGetVbiOffsetNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    PVIDEODECODEROBJECT pDecoder;
    U032                        Buffer;
    RM_STATUS status;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04DGetVbiOffsetNotify\r\n");
    //
    // A nice cast to make the code more readable.
    //
    pDecoder = (PVIDEODECODEROBJECT)Object;

    Buffer = ((Offset - Method->Low) <= 0x03) ? 0 : 1;

    //
    // Get current position from HAL.
    //
    (void) nvHalMpMethod(pDev, NV03_EXTERNAL_VIDEO_DECODER, &pDecoder->HalObject, Offset, Data, &status);

    //
    // Send back the data
    //
    notifyFillNotifierArray(pDev, pDecoder->CBase.NotifyXlate,
                            pDecoder->HalObject.SetVbi[Buffer].GetOffsetData,
                            0,
                            RM_OK,
                            NV04D_NOTIFIERS_GET_VBI_OFFSET_NOTIFY(Buffer) );

    if (Data)
        osNotifyEvent(pDev, (POBJECT)pDecoder, 
                      NV04D_NOTIFIERS_GET_VBI_OFFSET_NOTIFY(Buffer),
                      0, 
                      1, 
                      RM_OK, 
                      Data);

    return (status);
    
}

RM_STATUS class04DGetImageOffsetNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    PVIDEODECODEROBJECT         pDecoder;
    U032                        Buffer;
    RM_STATUS                   status;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04DGetImageOffsetNotify\r\n");
    //
    // A nice cast to make the code more readable.
    //
    pDecoder = (PVIDEODECODEROBJECT)Object;

    Buffer = ((Offset - Method->Low) <= 0x03) ? 0 : 1;

    //
    // Get current position from HAL.
    //
    (void) nvHalMpMethod(pDev, NV03_EXTERNAL_VIDEO_DECODER, &pDecoder->HalObject, Offset, Data, &status);

    //
    // Send back the data
    //
    notifyFillNotifierArray(pDev, pDecoder->CBase.NotifyXlate,
                            pDecoder->HalObject.SetImage[Buffer].GetOffsetData,
                            0,
                            RM_OK,
                            NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(Buffer));

    if (Data)
        osNotifyEvent(pDev, (POBJECT)pDecoder,
                      NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(Buffer),
                      0, 
                      1, 
                      RM_OK, 
                      Data);

    return (status);
}

//---------------------------------------------------------------------------
//
// Exception Handling
//
//---------------------------------------------------------------------------

// This is a helper proc for VbiEvent.
// This procedure actually does the notification for the given buffer.
static void NotifyVbiBuffer
(
	PHWINFO				pDev,
    PVIDEODECODEROBJECT pDecoder,
    U032                BuffNum
)
{
    notifyFillNotifierArray(pDev, pDecoder->CBase.NotifyXlate, 
                             0, 
                             0, 
                             RM_OK, 
                             NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(BuffNum));

    if (pDecoder->HalObject.SetVbi[BuffNum].Notify)
        osNotifyEvent(pDev, (POBJECT)pDecoder, 
                      NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(BuffNum),
                      0, 
                      0, 
                      RM_OK, 
                      pDecoder->HalObject.SetVbi[BuffNum].Notify);
}

// This is a helper proc for ImageEvent.
// This procedure actually does the notification for the given buffer.
static VOID NotifyImageBuffer
(
	PHWINFO				pDev,
    PVIDEODECODEROBJECT pDecoder,
    U032                BuffNum
)
{
    //
    // Include 32bit info value.
    //
    notifyFillNotifierArray(pDev, pDecoder->CBase.NotifyXlate, 
                             pDecoder->HalObject.SetImage[BuffNum].Info32,
                             0,
                             RM_OK, 
                             NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(BuffNum));

    if (pDecoder->HalObject.SetImage[BuffNum].Notify)
        osNotifyEvent(pDev, (POBJECT)pDecoder, 
                      NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(BuffNum),
                      0, 
                      1, 
                      RM_OK, 
                      pDecoder->HalObject.SetImage[BuffNum].Notify);
}

static VOID VbiEvents
(
    PHWINFO pDev,
    PVIDEODECODEROBJECT pDecoder,
    U032 *pIntrStatus
)
{
    VOID *pMpHalObj = &pDecoder->HalObject;
    U032 events = CLASS04D_VBI_EVENT(0)|CLASS04D_VBI_EVENT(1);

    //
    // Call into HAL and get status on VBI events.
    //
    (void) nvHalMpGetEventStatus(pDev, NV03_EXTERNAL_VIDEO_DECODER, pMpHalObj, &events, pIntrStatus);

    //
    // If the pending bit comes back set, then the event needs servicing.
    //
    if (pDecoder->LastVbiBufferProcessed == 1)
    {
        // Check buffer 0 first.
        if (events & CLASS04D_VBI_EVENT(0))
        {
            NotifyVbiBuffer(pDev, pDecoder, 0);
            (void) nvHalMpServiceEvent(pDev, NV03_EXTERNAL_VIDEO_DECODER,
                                       pMpHalObj,
                                       CLASS04D_VBI_EVENT(0),
                                       pIntrStatus);
            pDecoder->LastVbiBufferProcessed = 0;
            pDecoder->LastEventProcessed = EVENT_PROCESSED_VBI;
        }
        // Now buffer 1.
        if (events & CLASS04D_VBI_EVENT(1))
        {
            NotifyVbiBuffer(pDev, pDecoder, 1);
            (void) nvHalMpServiceEvent(pDev, NV03_EXTERNAL_VIDEO_DECODER,
                                       pMpHalObj,
                                       CLASS04D_VBI_EVENT(1),
                                       pIntrStatus);
            pDecoder->LastVbiBufferProcessed = 1;
            pDecoder->LastEventProcessed = EVENT_PROCESSED_VBI;
        }
    }
    else
    {
        // Check buffer 1 first.
        if (events & CLASS04D_VBI_EVENT(1))
        {
            NotifyVbiBuffer(pDev, pDecoder, 1);
            (void) nvHalMpServiceEvent(pDev, NV03_EXTERNAL_VIDEO_DECODER,
                                       pMpHalObj,
                                       CLASS04D_VBI_EVENT(1),
                                       pIntrStatus);
            pDecoder->LastVbiBufferProcessed = 1;
            pDecoder->LastEventProcessed = EVENT_PROCESSED_VBI;
        }
        // Now buffer 0.
        if (events & CLASS04D_VBI_EVENT(0))
        {
            NotifyVbiBuffer(pDev, pDecoder, 0);
            (void) nvHalMpServiceEvent(pDev, NV03_EXTERNAL_VIDEO_DECODER,
                                       pMpHalObj,
                                       CLASS04D_VBI_EVENT(0),
                                       pIntrStatus);
            pDecoder->LastVbiBufferProcessed = 0;
            pDecoder->LastEventProcessed = EVENT_PROCESSED_VBI;
        }

    }

    return;
}

static VOID ImageEvents
(
    PHWINFO pDev,
    PVIDEODECODEROBJECT pDecoder,
    U032 *pIntrStatus
)
{
    VOID *pMpHalObj = &pDecoder->HalObject;
    U032 events = CLASS04D_IMAGE_EVENT(0)|CLASS04D_IMAGE_EVENT(1);

    //
    // Call into HAL and get status on Image events.
    //
    (void) nvHalMpGetEventStatus(pDev, NV03_EXTERNAL_VIDEO_DECODER, pMpHalObj, &events, pIntrStatus);

    //
    // If the pending bit comes back set, then the event needs servicing.
    //
    if (pDecoder->LastImageBufferProcessed == 1)
    {
        // Check buffer 0 first.
        if (events & CLASS04D_IMAGE_EVENT(0))
        {
            NotifyImageBuffer(pDev, pDecoder, 0);
            (void) nvHalMpServiceEvent(pDev, NV03_EXTERNAL_VIDEO_DECODER,
                                       pMpHalObj,
                                       CLASS04D_IMAGE_EVENT(0),
                                       pIntrStatus);
            pDecoder->LastImageBufferProcessed = 0;
            pDecoder->LastEventProcessed = EVENT_PROCESSED_IMAGE;
        }
        // Now buffer 1.
        if (events & CLASS04D_IMAGE_EVENT(1))
        {
            NotifyImageBuffer(pDev, pDecoder, 1);
            (void) nvHalMpServiceEvent(pDev, NV03_EXTERNAL_VIDEO_DECODER,
                                       pMpHalObj,
                                       CLASS04D_IMAGE_EVENT(1),
                                       pIntrStatus);
            pDecoder->LastImageBufferProcessed = 1;
            pDecoder->LastEventProcessed = EVENT_PROCESSED_IMAGE;
        }
    }
    else
    {
        // Check buffer 1 first.
        if (events & CLASS04D_IMAGE_EVENT(1))
        {
            NotifyImageBuffer(pDev, pDecoder, 1);
            (void) nvHalMpServiceEvent(pDev, NV03_EXTERNAL_VIDEO_DECODER,
                                       pMpHalObj,
                                       CLASS04D_IMAGE_EVENT(1),
                                       pIntrStatus);
            pDecoder->LastImageBufferProcessed = 1;
            pDecoder->LastEventProcessed = EVENT_PROCESSED_IMAGE;
        }
        // Now buffer 0.
        if (events & CLASS04D_IMAGE_EVENT(0))
        {
            NotifyImageBuffer(pDev, pDecoder, 0);
            (void) nvHalMpServiceEvent(pDev, NV03_EXTERNAL_VIDEO_DECODER,
                                       pMpHalObj,
                                       CLASS04D_IMAGE_EVENT(0),
                                       pIntrStatus);
            pDecoder->LastImageBufferProcessed = 0;
            pDecoder->LastEventProcessed = EVENT_PROCESSED_IMAGE;
        }

    }

    //
    // If we had any pending image events, issue os-dependent callback.
    //
    if (events)
        osmpCheckCallbacks(pDev, 1);

    return;
}

//
// class04DService
//
V032 class04DService
(
    PHWINFO pDev
)
{
    PVIDEODECODEROBJECT pDecoder;
    U032 eventsPending, intrStatus;

    //
    // Handle case where we don't have a current decoder object.
    //
    pDecoder = (PVIDEODECODEROBJECT)(pDev->MediaPort.CurrentDecoder);
    if (pDecoder == NULL)
    {
        //
        // This exception is spurious.  Call into HAL to give it a
        // chance to reset the pending condition and then we're done.
        //
        eventsPending = 0xffffffff;
        (void) nvHalMpGetEventStatus(pDev, NV03_EXTERNAL_VIDEO_DECODER, NULL, &eventsPending, &intrStatus);
        return (intrStatus);
    }

    //
    // The order in which we process this interrupt is dependent on
    // the last event/buffer processed.
    //
    if (pDecoder->LastEventProcessed == EVENT_PROCESSED_IMAGE) 
    {
        VbiEvents(pDev, pDecoder, &intrStatus);
        ImageEvents(pDev, pDecoder, &intrStatus);
    }
    else
    {
        ImageEvents(pDev, pDecoder, &intrStatus);
        VbiEvents(pDev, pDecoder, &intrStatus);
    }

    return (intrStatus);
}
