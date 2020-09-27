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

/******************************* Video Manager *****************************\
*                                                                           *
* Module: class07a.C                                                        *
*   This module implements the NV10_VIDEO_OVERLAY methods.                  *
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
#include <gr.h>   
#include <dma.h>   
#include <modular.h>
#include <os.h>
#include <nv32.h>
#include "nvhw.h"
#include "dac.h"
#include "smp.h"

#if 0
static RM_STATUS class07AOverlayService(PHWINFO, POBJECT);
#endif

BOOL   HwOwnsBuffer(PHWINFO, U032, POBJECT);
extern VOID NV10_AdjustScalarForTV(PHWINFO, U032);

//---------------------------------------------------------------------------
//
//  Create/Destroy object routines.
//
//---------------------------------------------------------------------------

//
// See kernel/video/videoobj.c
//

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

RM_STATUS class07ASetNotifyCtxDma
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    RM_STATUS                    status;
    PVIDEO_OVERLAY_OBJECT        pOverlayObject;
    PVIDEO_OVERLAY_HAL_OBJECT   pOverlayHalObject;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07ASetNotifyCtxDma\r\n");
    //
    // Set the notify DMA context.
    //
    pOverlayObject = (PVIDEO_OVERLAY_OBJECT)Object;
    pOverlayHalObject = &pOverlayObject->HalObject;
    
    if (pOverlayObject->CBase.NotifyTrigger)
        return NV07A_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

    //
    // Give HAL a chance to reject this method.
    //
    (void) nvHalVideoMethod(pDev, NV10_VIDEO_OVERLAY, &pOverlayObject->HalObject, Offset, Data, &status);
    if (status)
        return (status);

    //
    // Connect to dma buffer.
    //    
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &(pOverlayObject->CBase.NotifyXlate));
    if (status)
        return (NV07A_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    
    //
    // Validate notification parameters.
    //
    status = dmaValidateXlate(pOverlayObject->CBase.NotifyXlate, 
                              0, 
                              (3 * 0x0010));
    if (status)
    {
        pOverlayObject->CBase.NotifyXlate = NULL;
        return (NV07A_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    }
    
    return (status);
} // end of class07ASetNotifyCtxDma

RM_STATUS class07ASetNotify
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEO_OVERLAY_OBJECT        pOverlayObject;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07ASetNotify\r\n");
    //
    // Set the notification style (note that these will be s/w notifies!)
    //

    pOverlayObject = (PVIDEO_OVERLAY_OBJECT)Object;

    pOverlayObject->CBase.NotifyAction  = Data;
    pOverlayObject->CBase.NotifyTrigger = TRUE;
    
    return (NV07A_NOTIFICATION_STATUS_DONE_SUCCESS);
} // end of class07ASetNotify

RM_STATUS class07AStopOverlay
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEO_OVERLAY_OBJECT        pOverlayObject;
    RM_STATUS                    status;
    pOverlayObject = (PVIDEO_OVERLAY_OBJECT)Object;


    (void) nvHalVideoMethod(pDev, NV10_VIDEO_OVERLAY, &pOverlayObject->HalObject, Offset, Data, &status);

    return status;
} // end of class07AStopOverlay

RM_STATUS class07ASetOverlayCtxDma
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    RM_STATUS                    status;
    PVIDEO_OVERLAY_OBJECT        pOverlayObject;
    U032                         buffNum;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07ASetOverlayCtxDma\r\n");
    //
    // Set the Image DMA context.
    //
    pOverlayObject = (PVIDEO_OVERLAY_OBJECT)Object;
    buffNum = (Offset == NV07A_SET_CONTEXT_DMA_OVERLAY(0)) ? 0 : 1;

    //
    // Give HAL a chance to reject this method.
    //
    (void) nvHalVideoMethod(pDev, NV10_VIDEO_OVERLAY, &pOverlayObject->HalObject, Offset, Data, &status);
    if (status)
        return (status);

    if (Data == NV01_NULL_OBJECT)
    {
        pOverlayObject->Overlay[buffNum].OverlayCtxDma = NULL;
        pOverlayObject->HalObject.Overlay[buffNum].OverlayCtxDma = NULL;
        return NV07A_NOTIFICATION_STATUS_DONE_SUCCESS;
    }
    //
    // Connect to dma buffer.
    //    
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &(pOverlayObject->Overlay[buffNum].OverlayCtxDma));
    if (status)
        return (NV07A_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    
    // The overlay context dma MUST point to the frame buffer.
    if (pOverlayObject->Overlay[buffNum].OverlayCtxDma->HalInfo.AddressSpace != ADDR_FBMEM)
    {
        return NV07A_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
    }

    // Give HAL access to context dma.
    pOverlayObject->HalObject.Overlay[buffNum].OverlayCtxDma = &pOverlayObject->Overlay[buffNum].OverlayCtxDma->HalInfo;

    return NV07A_NOTIFICATION_STATUS_DONE_SUCCESS;
} // end of class07ASetOverlayCtxDma

RM_STATUS class07AGetOffset
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    RM_STATUS                    status;
    PVIDEO_OVERLAY_OBJECT        pOverlayObject;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07AGetOffset\r\n");
    
    pOverlayObject = (PVIDEO_OVERLAY_OBJECT)Object;
    if (!pOverlayObject->CBase.NotifyTrigger)
        // nothing to do.
        return NV07A_NOTIFICATION_STATUS_DONE_SUCCESS;

    // The caller is expecting a notification. 
    // Write the image offset in the INFO32 of the notification.
    if (pOverlayObject->CBase.NotifyXlate) 
    {
        //
        // Give HAL chance to setup offset if necessary.
        //
        (void) nvHalVideoMethod(pDev, NV10_VIDEO_OVERLAY, &pOverlayObject->HalObject, Offset, Data, &status);
        if (status)
            return status;
        status = notifyFillNotifierArray(pDev, pOverlayObject->CBase.NotifyXlate, 
                                         pOverlayObject->HalObject.Overlay[Data].Offset,   // info32
                                         NV07A_NOTIFICATION_INFO16_VALID_OFFSET, // info16
                                         NV07A_NOTIFICATION_STATUS_DONE_SUCCESS,
                                         NV07A_NOTIFIERS_NOTIFY);
        //
        // Do any OS specified action related to this notification.
        //
        if (pOverlayObject->CBase.NotifyAction)
        {
            osNotifyEvent(pDev, (POBJECT)pOverlayObject, 
                          NV07A_NOTIFIERS_NOTIFY,
                          0 /* Method */, 
                          0 /* Data */, 
                          status, 
                          pOverlayObject->CBase.NotifyAction);
        }
    }

    pOverlayObject->CBase.NotifyTrigger = FALSE; 
    return (NV07A_NOTIFICATION_STATUS_DONE_SUCCESS);
} // end of class07AGetOffset

RM_STATUS class07ASetOverlayValues
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEO_OVERLAY_OBJECT       pOverlayObject;
    RM_STATUS                   status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07ASetOverlayValues\r\n");

    pOverlayObject = (PVIDEO_OVERLAY_OBJECT)Object;

    // Initialize the overlay description.
    //
    switch (Offset)
    {
        case NV07A_SET_OVERLAY_LUMINANCE(0):
        case NV07A_SET_OVERLAY_LUMINANCE(1):
        case NV07A_SET_OVERLAY_CHROMINANCE(0):
        case NV07A_SET_OVERLAY_CHROMINANCE(1):
        case NV07A_SET_OVERLAY_COLORKEY(0):
        case NV07A_SET_OVERLAY_COLORKEY(1):
        case NV07A_SET_OVERLAY_OFFSET(0):
        case NV07A_SET_OVERLAY_OFFSET(1):
        case NV07A_SET_OVERLAY_SIZE_IN(0):
        case NV07A_SET_OVERLAY_SIZE_IN(1):
        case NV07A_SET_OVERLAY_POINT_IN(0):
        case NV07A_SET_OVERLAY_POINT_IN(1):
        case NV07A_SET_OVERLAY_DU_DX(0):
        case NV07A_SET_OVERLAY_DU_DX(1):
        case NV07A_SET_OVERLAY_DV_DY(0):
        case NV07A_SET_OVERLAY_DV_DY(1):
        case NV07A_SET_OVERLAY_POINT_OUT(0):
        case NV07A_SET_OVERLAY_POINT_OUT(1):
        case NV07A_SET_OVERLAY_SIZE_OUT(0):
        case NV07A_SET_OVERLAY_SIZE_OUT(1):
            (void) nvHalVideoMethod(pDev, NV10_VIDEO_OVERLAY, &pOverlayObject->HalObject, Offset, Data, &status);
            return status;
        case NV07A_SET_OVERLAY_FORMAT(0):
        case NV07A_SET_OVERLAY_FORMAT(1):
        {
            RM_STATUS status;
            U032 buffNum;

            buffNum = (Offset == NV07A_SET_OVERLAY_FORMAT(0)) ? 0 : 1;

            // Check to see if we should awaken and notify 
            if (Data & 0x80000000 ) {
                // if so, set the appropriate action
                //pOverlayObject->Overlay[buffNum].NotifyAction = NV_OS_WRITE_THEN_AWAKEN;
                pOverlayObject->Overlay[buffNum].NotifyAction = 1;
            
            }

            (void) nvHalVideoMethod(pDev, NV10_VIDEO_OVERLAY, &pOverlayObject->HalObject, Offset, Data, &status);

            //
            // Ensure class-dependent video service handler is
            // setup properly.
            //
            if (status == RM_OK)
            {
                pDev->Video.HalInfo.ActiveVideoOverlayObject = (VOID_PTR) pOverlayObject;
                pDev->pOverlayServiceHandler = videoOverlayService;
            }
            return status;
        }
        default:
            return NV07A_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            break;
     }

    return RM_OK;

} // end of class07ASetOverlayValues

RM_STATUS class07ASetOverlayPointOutNow
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEO_OVERLAY_OBJECT       pOverlayObject;
    RM_STATUS                   status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07ASetOverlayPointOutNow\r\n");

    pOverlayObject = (PVIDEO_OVERLAY_OBJECT)Object;

    (void) nvHalVideoMethod(pDev, NV10_VIDEO_OVERLAY, (VOID *)&pOverlayObject->HalObject, Offset, Data, &status);
    
    return status;
}

RM_STATUS class07ASetOverlayLuminanceNow
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEO_OVERLAY_OBJECT       pOverlayObject;
    RM_STATUS                   status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07ASetOverlayLuminanceNow\r\n");

    pOverlayObject = (PVIDEO_OVERLAY_OBJECT)Object;

    (void) nvHalVideoMethod(pDev, NV10_VIDEO_OVERLAY, (VOID *)&pOverlayObject->HalObject, Offset, Data, &status);

    return status;
}

RM_STATUS class07ASetOverlayChrominanceNow
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEO_OVERLAY_OBJECT       pOverlayObject;
    RM_STATUS                   status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07ASetOverlayChrominanceNow\r\n");

    pOverlayObject = (PVIDEO_OVERLAY_OBJECT)Object;

    (void) nvHalVideoMethod(pDev, NV10_VIDEO_OVERLAY, (VOID *)&pOverlayObject->HalObject, Offset, Data, &status);

    return status;
}

