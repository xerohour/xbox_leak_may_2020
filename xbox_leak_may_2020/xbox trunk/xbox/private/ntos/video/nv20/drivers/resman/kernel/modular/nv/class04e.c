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
* Module: CLASS04E.C                                                        *
*   This module implements the NV_EXTERNAL_VIDEO_DECOMPRESSOR object class  *
*   and its corresponding methods.                                          *
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
//  Miscellaneous class support routines.
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//  Class method routines.
//
//---------------------------------------------------------------------------

RM_STATUS class04ENotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04ENotify\r\n");
    //
    // Set the notify DMA context
    //
    return (mthdSetNotify(pDev, Object, Method, Offset, Data));
}
RM_STATUS class04EStopTransfer
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEODECOMPRESSOROBJECT    pDecompress;
    RM_STATUS                   status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04EStopTransfer\r\n");

    //
    // A nice cast to make the code more readable.
    //
    pDecompress = (PVIDEODECOMPRESSOROBJECT)Object;

    //
    // Stop the current data transfer immediately.
    //
    (void) nvHalMpMethod(pDev, NV03_EXTERNAL_VIDEO_DECOMPRESSOR, &pDecompress->HalObject, NV04E_STOP_TRANSFER, Data, &status);

    return (RM_OK);
}
RM_STATUS class04ESetContextDmaNotifies
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04ESetContextNotifies\r\n");
    //
    // Set the notify DMA context
    //
    return (mthdSetNotifyCtxDma(pDev, Object, Method, Offset, Data));
}

RM_STATUS class04ESetContextDmaData
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEODECOMPRESSOROBJECT    pDecompress;
    U032                        Buffer;
    RM_STATUS                   status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04ESetContextDmaDma\r\n");
    
    //
    // A nice cast to make the code more readable.
    //
    pDecompress = (PVIDEODECOMPRESSOROBJECT)Object;

    Buffer = (Offset - Method->Low) >> 2;

	if (Data == NV01_NULL_OBJECT)
    {
       	pDecompress->ContextDmaData[Buffer] = NULL;
        pDecompress->HalObject.ReadData[Buffer].CtxDma = NULL;
        return NV04E_NOTIFICATION_STATUS_DONE_SUCCESS;
    }

    //
    // Set the data DMA contexts
    //
    // Connect to dma buffer.
    //    
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &(pDecompress->ContextDmaData[Buffer]));
    if (status)
    {
        osError(Object, Offset, Data, status);
        return (status);
    }

    //
    // Give the HAL access to the context dma.
    //
    pDecompress->HalObject.ReadData[Buffer].CtxDma = &pDecompress->ContextDmaData[Buffer]->HalInfo;

    return (RM_OK);
}
RM_STATUS class04ESetContextDmaImage
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEODECOMPRESSOROBJECT    pDecompress;
    U032                        Buffer;
    RM_STATUS                   status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04ESetContextDmaImage\r\n");
    
    //
    // A nice cast to make the code more readable.
    //
    pDecompress = (PVIDEODECOMPRESSOROBJECT)Object;

    Buffer = (Offset - Method->Low) >> 2;

	if (Data == NV01_NULL_OBJECT)
    {
       	pDecompress->ContextDmaImage[Buffer] = NULL;
        pDecompress->HalObject.WriteImage[Buffer].CtxDma = NULL;
        return NV04E_NOTIFICATION_STATUS_DONE_SUCCESS;
    }

    //
    // Set the data DMA contexts
    //
    // Connect to dma buffer.
    //    
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &(pDecompress->ContextDmaImage[Buffer]));
    if (status)
    {
        osError(Object, Offset, Data, status);
        return (status);
    }

    //
    // Give the HAL access to the context dma.
    //
    pDecompress->HalObject.WriteImage[Buffer].CtxDma = &pDecompress->ContextDmaImage[Buffer]->HalInfo;
    
    return (RM_OK);
}

RM_STATUS class04EReadData
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    PVIDEODECOMPRESSOROBJECT    pDecompress;
    RM_STATUS                   status;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04EReadData\r\n");
    //
    // A nice cast to make the code more readable.
    //
    pDecompress = (PVIDEODECOMPRESSOROBJECT)Object;

    (void) nvHalMpMethod(pDev, NV03_EXTERNAL_VIDEO_DECOMPRESSOR, &pDecompress->HalObject, Offset, Data, &status);

    return status;
}

RM_STATUS class04EWriteImage
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    PVIDEODECOMPRESSOROBJECT    pDecompress;
    RM_STATUS                   status;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class04EWriteImage\r\n");
    //
    // A nice cast to make the code more readable.
    //
    pDecompress = (PVIDEODECOMPRESSOROBJECT)Object;

    (void) nvHalMpMethod(pDev, NV03_EXTERNAL_VIDEO_DECOMPRESSOR, &pDecompress->HalObject, Offset, Data, &status);

    return status;
}

//---------------------------------------------------------------------------
//
// Exception Handling
//
//---------------------------------------------------------------------------

static void NotifyDataBuffer
(
	PHWINFO				        pDev,
    PVIDEODECOMPRESSOROBJECT    pDecompress,
    U032                        BuffNum
)
{
    notifyFillNotifierArray(pDev,  pDecompress->CBase.NotifyXlate, 
                            0, 
                            0, 
                            RM_OK, 
                            NV04E_NOTIFIERS_SET_DATA_NOTIFY(BuffNum) );

    if (pDecompress->HalObject.ReadData[BuffNum].Notify)
        osNotifyEvent(pDev, (POBJECT)pDecompress, 
                      NV04E_NOTIFIERS_SET_DATA_NOTIFY(BuffNum),
                      0, 
                      0, 
                      RM_OK, 
                      pDecompress->HalObject.ReadData[BuffNum].Notify);
}

static void NotifyImageBuffer
(
	PHWINFO				        pDev,
    PVIDEODECOMPRESSOROBJECT    pDecompress,
    U032                        BuffNum
)
{
    //
    // Only call the completion routine if one was expected
    //
    notifyFillNotifierArray(pDev,  pDecompress->CBase.NotifyXlate, 
                            0, 
                            0, 
                            RM_OK, 
                            NV04E_NOTIFIERS_SET_IMAGE_NOTIFY(BuffNum) );
}

static VOID VideoEvents
(
    PHWINFO pDev,
    PVIDEODECOMPRESSOROBJECT pDecompress,
    U032 *pIntrStatus
)
{
    VOID *pMpHalObj = &pDecompress->HalObject;
    U032 events = CLASS04E_VIDEO_EVENT(0)|CLASS04E_VIDEO_EVENT(1);

    //
    // Call into HAL and get status on Video events.
    //
    (void) nvHalMpGetEventStatus(pDev, NV03_EXTERNAL_VIDEO_DECOMPRESSOR, pMpHalObj, &events, pIntrStatus);

    //
    // If the pending bit comes back set, then the event needs servicing.
    //
    if (events & CLASS04E_VIDEO_EVENT(0))
    {
        NotifyDataBuffer(pDev, pDecompress, 0);
        (void) nvHalMpServiceEvent(pDev, NV03_EXTERNAL_VIDEO_DECOMPRESSOR,
                                   pMpHalObj,
                                   CLASS04E_VIDEO_EVENT(0),
                                   pIntrStatus);
    }

    if (events & CLASS04E_VIDEO_EVENT(1))
    {
        NotifyDataBuffer(pDev, pDecompress, 1);
        (void) nvHalMpServiceEvent(pDev, NV03_EXTERNAL_VIDEO_DECOMPRESSOR,
                                   pMpHalObj,
                                   CLASS04E_VIDEO_EVENT(1),
                                   pIntrStatus);
    }

    return;
}

static VOID AudioEvents
(
    PHWINFO pDev,
    PVIDEODECOMPRESSOROBJECT pDecompress,
    U032 *pIntrStatus
)
{
    VOID *pMpHalObj = &pDecompress->HalObject;
    U032 events = CLASS04E_AUDIO_EVENT(0)|CLASS04E_AUDIO_EVENT(1);

    //
    // Call into HAL and get status on Video events.
    //
    (void) nvHalMpGetEventStatus(pDev, NV03_EXTERNAL_VIDEO_DECOMPRESSOR, pMpHalObj, &events, pIntrStatus);

    //
    // If the pending bit comes back set, then the event needs servicing.
    //
    if (events & CLASS04E_AUDIO_EVENT(0))
    {
        NotifyDataBuffer(pDev, pDecompress, 0);
        (void) nvHalMpServiceEvent(pDev, NV03_EXTERNAL_VIDEO_DECOMPRESSOR,
                                   pMpHalObj,
                                   CLASS04E_AUDIO_EVENT(0),
                                   pIntrStatus);
    }

    if (events & CLASS04E_AUDIO_EVENT(1))
    {
        NotifyDataBuffer(pDev, pDecompress, 1);
        (void) nvHalMpServiceEvent(pDev, NV03_EXTERNAL_VIDEO_DECOMPRESSOR,
                                   pMpHalObj,
                                   CLASS04E_AUDIO_EVENT(1),
                                   pIntrStatus);
    }

    return;
}

#ifdef NOTYET
static VOID ImageEvents
(
    PHWINFO pDev,
    PVIDEODECOMPRESSOROBJECT pDecompress,
    U032 *pIntrStatus
)
{
    VOID *pMpHalObj = &pDecompress->HalObject;
    U032 events = CLASS04E_IMAGE_EVENT(0)|CLASS04E_IMAGE_EVENT(1);

    //
    // Call into HAL and get status on Video events.
    //
    (void) nvHalMpGetEventStatus(pDev, NV03_EXTERNAL_VIDEO_DECOMPRESSOR, pMpHalObj, &events, pIntrStatus);

    //
    // If the pending bit comes back set, then the event needs servicing.
    //
    if (events & CLASS04E_IMAGE_EVENT(0))
    {
        NotifyImageBuffer(pDev, pDecompress, 0);
        (void) nvHalMpServiceEvent(pDev, NV03_EXTERNAL_VIDEO_DECOMPRESSOR,
                                   pMpHalObj,
                                   CLASS04E_IMAGE_EVENT(0),
                                   pIntrStatus);
    }

    if (events & CLASS04E_IMAGE_EVENT(1))
    {
        NotifyImageBuffer(pDev, pDecompress, 1);
        (void) nvHalMpServiceEvent(pDev, NV03_EXTERNAL_VIDEO_DECOMPRESSOR,
                                   pMpHalObj,
                                   CLASS04E_IMAGE_EVENT(1),
                                   pIntrStatus);
    }

    return;
}
#endif // NOTYET

//
// class04EService
//
V032 class04EService
(
    PHWINFO pDev
)
{
    PVIDEODECOMPRESSOROBJECT pDecompress;
    U032 eventsPending, intrStatus;

    //
    // Handle case where we don't have a current decompressor object.
    //
    pDecompress = (PVIDEODECOMPRESSOROBJECT)(pDev->MediaPort.CurrentDecompressor);
    if (pDecompress == NULL)
    {
        //
        // This exception is spurious.  Call into HAL to give it a
        // chance to reset the pending condition and then we're done.
        //
        eventsPending = 0xffffffff;
        (void) nvHalMpGetEventStatus(pDev, NV03_EXTERNAL_VIDEO_DECOMPRESSOR, NULL, &eventsPending, &intrStatus);
        return (intrStatus);
    }

    //
    // Process events.
    //
    VideoEvents(pDev, pDecompress, &intrStatus);
    AudioEvents(pDev, pDecompress, &intrStatus);
#ifdef NOTYET
    ImageEvents(pDev, pDecompress, &intrStatus);
#endif

    return (intrStatus);
}
