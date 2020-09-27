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
* Module: class047.C                                                        *
*   This module implements the NV04_VIDEO_OVERLAY methods.                  *
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

RM_STATUS class047StopOverlayNow
(
    PHWINFO   pDev,
    POBJECT   Object,
    U032      param1,
    V032      BuffNum,
    RM_STATUS Status
)
{
    VIDEO_LUT_CURSOR_DAC_HAL_OBJECT VidLutCurDac;
    PVIDEO_OVERLAY_OBJECT           pOverlayObject;
    PVIDEO_OVERLAY_HAL_OBJECT       pOverlayHalObject;
    RM_STATUS                       status;
    U032                            Head = 0; // this is a single-head class

    pOverlayObject = (PVIDEO_OVERLAY_OBJECT)Object;
    pOverlayHalObject = &pOverlayObject->HalObject;
    VidLutCurDac.Head = Head;

    //
    // Stop the current data transfer immediately.
    //
    if (pOverlayHalObject->Overlay[0].State != OVERLAY_BUFFER_IDLE)
    {
        notifyFillNotifierArray(pDev, pOverlayObject->Common.Base.NotifyXlate, 
                                0, // info32
                                0, // info16
                                RM_OK, 
                                NV047_NOTIFIERS_SET_OVERLAY(0));
        pOverlayHalObject->Overlay[0].State = OVERLAY_BUFFER_IDLE;
    }
    if (pOverlayHalObject->Overlay[1].State != OVERLAY_BUFFER_IDLE)
    {
        notifyFillNotifierArray(pDev, pOverlayObject->Common.Base.NotifyXlate, 
                                0,  // info32
                                0,  // info16
                                RM_OK, 
                                NV047_NOTIFIERS_SET_OVERLAY(1));
        pOverlayHalObject->Overlay[1].State = OVERLAY_BUFFER_IDLE;
    }

    //
    // Notify this one
    //
    if (pOverlayObject->Common.Base.NotifyXlate)
    {
        notifyFillNotifierArray(pDev, pOverlayObject->Common.Base.NotifyXlate, 
                                0, // info32
                                0, // info16
                                RM_OK, 
                                NV047_NOTIFIERS_SET_OVERLAY(BuffNum));
    }     

    //
    // Issue stop-immediately call to HAL.
    //
    (void) nvHalVideoMethod(pDev, NV04_VIDEO_OVERLAY, &pOverlayObject->HalObject, NV047_STOP_OVERLAY(BuffNum), NV047_STOP_OVERLAY_AS_SOON_AS_POSSIBLE, &status);

    //
    // Transition state of object back to uninitialized.
    //
    pOverlayObject->InitState = 0;

    //
    // Video has been disabled, so refresh arb settings.
    //
    nvHalDacUpdateArbSettings(pDev, (VOID*) &VidLutCurDac);

    //
    // Clear the active overlay object.
    //
    if ((POBJECT) pDev->Video.HalInfo.ActiveVideoOverlayObject == (POBJECT) pOverlayObject)
    {
        pDev->Video.HalInfo.ActiveVideoOverlayObject = NULL;
        pDev->Video.HalInfo.Enabled = 0;
    }

    return status;
}

//---------------------------------------------------------------------------
//
//  Class method routines.
//
//---------------------------------------------------------------------------

RM_STATUS class047SetNotifyCtxDma
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
	RM_STATUS                   status;
    PVIDEO_OVERLAY_OBJECT       pOverlayObject;
    PVIDEO_OVERLAY_HAL_OBJECT   pOverlayHalObject;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class047SetNotifyCtxDma\r\n");
    //
    // Set the notify DMA context.
    //
    pOverlayObject = (PVIDEO_OVERLAY_OBJECT)Object;
    pOverlayHalObject = &pOverlayObject->HalObject;

    if (pOverlayObject->CBase.NotifyTrigger)
        return NV047_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

    //
    // Give HAL a chance to reject this method.
    //
    (void) nvHalVideoMethod(pDev, NV04_VIDEO_OVERLAY, &pOverlayObject->HalObject, Offset, Data, &status);
    if (status)
        return (status);

    //
    // Connect to dma buffer.
    //    
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &(pOverlayObject->CBase.NotifyXlate));
    if (status)
        return (NV047_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    
    //
    // Validate notification parameters.
    //
    status = dmaValidateXlate(pOverlayObject->CBase.NotifyXlate, 
                              0, 
                              (3 * 0x0010));
    if (status)
    {
        pOverlayObject->CBase.NotifyXlate = NULL;
        return (NV047_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    }
    
    return (status);
} // end of class047SetNotifyCtxDma

RM_STATUS class047SetNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEO_OVERLAY_OBJECT        pOverlayObject;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class047SetNotify\r\n");
    //
    // Set the notification style (note that these will be s/w notifies!)
    //

    pOverlayObject = (PVIDEO_OVERLAY_OBJECT)Object;

    pOverlayObject->CBase.NotifyAction  = Data;
    pOverlayObject->CBase.NotifyTrigger = TRUE;
    
    return (NV047_NOTIFICATION_STATUS_DONE_SUCCESS);
} // end of class047SetNotify

RM_STATUS class047StopOverlay
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEO_OVERLAY_OBJECT pOverlayObject;
    U032                  buffNum;
  
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class047StopOverlay\r\n");

    pOverlayObject = (PVIDEO_OVERLAY_OBJECT)Object;

    buffNum = (Offset == NV047_STOP_OVERLAY(0)) ? 0 : 1;
    if (Data == NV047_STOP_OVERLAY_AS_SOON_AS_POSSIBLE) 
    {
        class047StopOverlayNow(pDev, (POBJECT)pOverlayObject, 
                               0 /* param1 */, buffNum, 
                               NV047_NOTIFICATION_STATUS_DONE_SUCCESS);
    }
    else
    {
        if (pOverlayObject->HalObject.Overlay[buffNum].State != OVERLAY_BUFFER_STOP_PENDING) {
            // setup so we get a callback to do the stop at the next VBlank.
            pOverlayObject->Overlay[buffNum].OverlayCallback.Proc   = class047StopOverlayNow;
            pOverlayObject->Overlay[buffNum].OverlayCallback.Object = (POBJECT) pOverlayObject;
            pOverlayObject->Overlay[buffNum].OverlayCallback.Next    = NULL;
            pOverlayObject->Overlay[buffNum].OverlayCallback.Param1  = 0;
            // Set param2 to the buffer number so we know which notifier to write.
            pOverlayObject->Overlay[buffNum].OverlayCallback.Param2  = buffNum;
            pOverlayObject->Overlay[buffNum].OverlayCallback.Status  = NV047_NOTIFICATION_STATUS_DONE_SUCCESS;

            // Defaults to head 0.
            VBlankAddCallback(pDev, 0, &(pOverlayObject->Overlay[buffNum].OverlayCallback));
            pOverlayObject->HalObject.Overlay[buffNum].State = OVERLAY_BUFFER_STOP_PENDING;
        }
    }

    return (NV047_NOTIFICATION_STATUS_DONE_SUCCESS);
} // end of class047StopOverlay

RM_STATUS class047SetOverlayCtxDma
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
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class047SetOverlayCtxDma\r\n");
    //
    // Set the Image DMA context.
    //
    pOverlayObject = (PVIDEO_OVERLAY_OBJECT)Object;
    buffNum = (Offset == NV047_SET_CONTEXT_DMA_OVERLAY(0)) ? 0 : 1;

    //
    // Give HAL a chance to reject this method.
    //
    (void) nvHalVideoMethod(pDev, NV04_VIDEO_OVERLAY, &pOverlayObject->HalObject, Offset, Data, &status);
    if (status)
        return (status);    

	if (Data == NV01_NULL_OBJECT)
    {
       	pOverlayObject->Overlay[buffNum].OverlayCtxDma = NULL;
        pOverlayObject->HalObject.Overlay[buffNum].OverlayCtxDma = NULL;
        return NV047_NOTIFICATION_STATUS_DONE_SUCCESS;
    }
    //
    // Connect to dma buffer.
    //    
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &(pOverlayObject->Overlay[buffNum].OverlayCtxDma));
    if (status)
        return (NV047_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    
    // The overlay context dma MUST point to the frame buffer.
    if (pOverlayObject->Overlay[buffNum].OverlayCtxDma->HalInfo.AddressSpace != ADDR_FBMEM)
    {
        return NV047_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
    }

    // Give HAL access to context dma.
    pOverlayObject->HalObject.Overlay[buffNum].OverlayCtxDma = &pOverlayObject->Overlay[buffNum].OverlayCtxDma->HalInfo;

    return NV047_NOTIFICATION_STATUS_DONE_SUCCESS;
} // end of class047SetOverlayCtxDma

RM_STATUS class047GetOffset
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
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class047GetOffset\r\n");
    
    pOverlayObject = (PVIDEO_OVERLAY_OBJECT)Object;
    if (!pOverlayObject->CBase.NotifyTrigger)
        // nothing to do.
        return NV047_NOTIFICATION_STATUS_DONE_SUCCESS;

    //
    // The caller is expecting a notification. 
    //
    if (pOverlayObject->CBase.NotifyXlate) 
    {
        //
        // Give HAL chance to setup offset if necessary.
        //
        (void) nvHalVideoMethod(pDev, NV04_VIDEO_OVERLAY, &pOverlayObject->HalObject, Offset, Data, &status);
        if (status)
            return status;
        status = notifyFillNotifierArray(pDev, pOverlayObject->CBase.NotifyXlate, 
                                         pOverlayObject->HalObject.Overlay[Data].Offset,   // info32
                                         NV047_NOTIFICATION_INFO16_VALID_OFFSET, // info16
                                         NV047_NOTIFICATION_STATUS_DONE_SUCCESS,
                                         NV047_NOTIFIERS_NOTIFY);
        //
        // Do any OS specified action related to this notification.
        //
        if (pOverlayObject->CBase.NotifyAction)
        {
            osNotifyEvent(pDev, (POBJECT)pOverlayObject, 
                          NV047_NOTIFIERS_NOTIFY,
                          0 /* Method */, 
                          0 /* Data */, 
                          status, 
                          pOverlayObject->CBase.NotifyAction);
        }
    }
    pOverlayObject->CBase.NotifyTrigger = FALSE; 
    return (NV047_NOTIFICATION_STATUS_DONE_SUCCESS);
} // end of class047GetOffset

RM_STATUS class047SetOverlayValues
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    VIDEO_LUT_CURSOR_DAC_HAL_OBJECT VidLutCurDac;
    PVIDEO_OVERLAY_OBJECT        pOverlayObject;
    RM_STATUS status;
    U032                         Head = 0; // this is a single-head class

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class047SetOverlayValues\r\n");

    VidLutCurDac.Head = Head;

    pOverlayObject = (PVIDEO_OVERLAY_OBJECT)Object;
	//
    // Initialize the overlay description via the HAL video engine manager.
    //
    switch (Offset)
    {
        case NV047_SET_OVERLAY_COLORKEY(0):
        case NV047_SET_OVERLAY_COLORKEY(1):
        case NV047_SET_OVERLAY_OFFSET(0):
        case NV047_SET_OVERLAY_OFFSET(1):
        case NV047_SET_OVERLAY_SIZE_IN(0):
        case NV047_SET_OVERLAY_SIZE_IN(1):
        case NV047_SET_OVERLAY_POINT_IN(0):
        case NV047_SET_OVERLAY_POINT_IN(1):
        case NV047_SET_OVERLAY_DS_DX(0):
        case NV047_SET_OVERLAY_DS_DX(1):
        case NV047_SET_OVERLAY_DT_DY(0):
        case NV047_SET_OVERLAY_DT_DY(1):
        case NV047_SET_OVERLAY_POINT_OUT(0):
        case NV047_SET_OVERLAY_POINT_OUT(1):
        case NV047_SET_OVERLAY_SIZE_OUT(0):
        case NV047_SET_OVERLAY_SIZE_OUT(1):
            (void) nvHalVideoMethod(pDev, NV04_VIDEO_OVERLAY, &pOverlayObject->HalObject, Offset, Data, &status);
            return status;
        case NV047_SET_OVERLAY_FORMAT(0):
        case NV047_SET_OVERLAY_FORMAT(1):
        {
            U032 buffNum;

            buffNum = (Offset == NV047_SET_OVERLAY_FORMAT(0)) ? 0 : 1;
            pOverlayObject->Overlay[buffNum].NotifyAction = (Data & BIT(DEVICE_BASE(NV047_SET_OVERLAY_FORMAT_NOTIFY))) ? 1 : 0;


#if 0
            //
            // Below is commented out because of race condition problems when we have
            // an overlay object in a DMA channel and a PIO channel as is the case with 
            // VPE (jsun)
            //
            if (pOverlayObject->InitState == 0)
            {
                if (pDev->Video.HalInfo.ActiveVideoOverlayObject != NULL)
                {
                    return NV_OUT_OF_RESOURCES;
                }
            }
#endif

            (void) nvHalVideoMethod(pDev, NV04_VIDEO_OVERLAY, &pOverlayObject->HalObject, Offset, Data, &status);

            //
            // Ensure class-dependent video service handler is
            // setup properly.
            //
            if (status == RM_OK)
            {
                //
                // If video has just been enabled, then refresh arb settings.
                //
                if (pOverlayObject->InitState == 0)
                {
                    nvHalDacUpdateArbSettings(pDev, (VOID*) &VidLutCurDac);
                    pOverlayObject->InitState = 1;
                }

                pDev->Video.HalInfo.ActiveVideoOverlayObject = (VOID_PTR) pOverlayObject;
                pDev->pOverlayServiceHandler = videoOverlayService;
            }
            return status;
        }
        default:
            return NV047_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            break;
     }

    return RM_OK;

} // end of class047SetOverlayValues

RM_STATUS class047SetOverlayPointOutNow
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEO_OVERLAY_OBJECT        pOverlayObject;
    RM_STATUS status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class047SetOverlayPointOutNow\r\n");

    pOverlayObject = (PVIDEO_OVERLAY_OBJECT)Object;

    (void) nvHalVideoMethod(pDev, NV04_VIDEO_OVERLAY, (VOID *)&pOverlayObject->HalObject, Offset, Data, &status);

    return status;
}
