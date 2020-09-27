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
* Module: class63.C                                                         *
*   This module implements the NV_VIDEO_FROM_MEMORY methods.                *
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

// statics
static RM_STATUS class63XferComplete(PHWINFO, POBJECT, U032, V032, RM_STATUS);

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
static RM_STATUS class63WaitVBlank
(
    PHWINFO       pDev,
    PVBLANKNOTIFY VBlankNotify,
    U032          buffer,
    U032          waitCount
)
{   
    //
    // Check for in-use.
    //
    if (VBlankNotify->Next)
        return (RM_ERR_NOTIFY_IN_USE);
    //
    // Stick this object on the appropriate queue waiting for the next VBlank
    //
    VBlankNotify->Next   = pDev->DBclass63VBlankList[buffer];
    pDev->DBclass63VBlankList[buffer] = VBlankNotify;

    pDev->Video.class63VBlankCount[buffer] = waitCount;

    return (RM_OK);
}

RM_STATUS class63VBlank
(
    PHWINFO pDev
)
{
    PVBLANKNOTIFY VBlankNotify;
    PVBLANKNOTIFY VBlankNext;

    //
    // Pull all notifies annd complete them.
    //
    if (--pDev->Video.class63VBlankCount[pDev->Framebuffer.Current] == 0)
    {
    VBlankNotify = pDev->DBclass63VBlankList[pDev->Framebuffer.Current];
    while (VBlankNotify)
    {
        VBlankNext         = VBlankNotify->Next;
        VBlankNotify->Next = NULL;
        VBlankNotify->Proc(pDev,
                           VBlankNotify->Object,
                           VBlankNotify->Param1,
                           VBlankNotify->Param2,
                           RM_OK);
        VBlankNotify = VBlankNext;

        //DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "n");
        
    }
    pDev->DBclass63VBlankList[pDev->Framebuffer.Current] = NULL;
    pDev->Video.class63VBlankCount[pDev->Framebuffer.Current] = 0;
    }
    return (RM_OK);
}

//---------------------------------------------------------------------------
//
//  Class method routines.
//
//---------------------------------------------------------------------------
RM_STATUS class63SetNotifyCtxDma
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    RM_STATUS status;
    PVIDEOFROMMEMOBJECT pVidFromMem;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class63SetNotifyCtxDma\r\n");

    //
    // A nice cast to make the code more readable.
    //
    pVidFromMem = (PVIDEOFROMMEMOBJECT)Object;
    
    if (pVidFromMem->BufferObj.Base.NotifyTrigger)
        return (NVFF8_NOTIFICATION_STATUS_ERROR_STATE_IN_USE);

    //
    // Connect to dma buffer.
    //    
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &(pVidFromMem->BufferObj.Base.NotifyXlate));
    if (status)
        return (NVFF8_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT);
    
    //
    // Validate notification parameters.
    //
    status = dmaValidateXlate(pVidFromMem->BufferObj.Base.NotifyXlate, 0, (5 * 0x0010));
    if (status)
    {
        pVidFromMem->BufferObj.Base.NotifyXlate   = NULL;
        return (NVFF8_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT);
    }
    
    //
    // Since NV4 uses the same notify buffer for all buffers, plug them into everywhere
    //
    pVidFromMem->BufferObj.Buffer[0].NotifyXlate = pVidFromMem->BufferObj.Base.NotifyXlate;
    pVidFromMem->BufferObj.Buffer[1].NotifyXlate = pVidFromMem->BufferObj.Base.NotifyXlate;
    
    return (status);
}

RM_STATUS class63SetNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEOFROMMEMOBJECT pVidFromMem;

    //
    // A nice cast to make the code more readable.
    //
    pVidFromMem = (PVIDEOFROMMEMOBJECT)Object;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class63SetNotify\r\n");
    //
    // Set the notification style (note that these will be s/w notifies!)
    //
    pVidFromMem->BufferObj.Base.NotifyAction  = Data;
    pVidFromMem->BufferObj.Base.NotifyTrigger = TRUE;
    
    return (RM_OK);
}

RM_STATUS class63StopTransfer
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    VIDEO_LUT_CURSOR_DAC_HAL_OBJECT VidLutCurDac;
    PVIDEOFROMMEMOBJECT pVidFromMem;
    RM_STATUS           status;
    U032                Head = 0; // single head class
    //
    // A nice cast to make the code more readable.
    //
    pVidFromMem = (PVIDEOFROMMEMOBJECT)Object;

    VidLutCurDac.Head = Head;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class63StopTransfer\r\n");

    //
    // Stop the current data transfer immediately.
    //

    if (pVidFromMem->HalObject.Buffer[0].State != OVERLAY_BUFFER_IDLE)
    {
        notifyFillNotifierArray( pDev, pVidFromMem->BufferObj.Base.NotifyXlate, 
                                 0, 
                                 0, 
                                 RM_OK, 
                                 NVFF8_NOTIFIERS_IMAGE_SCAN(0) );
        pVidFromMem->HalObject.Buffer[0].State = OVERLAY_BUFFER_IDLE;
    }
    if (pVidFromMem->HalObject.Buffer[1].State != OVERLAY_BUFFER_IDLE)
    {
        notifyFillNotifierArray( pDev, pVidFromMem->BufferObj.Base.NotifyXlate, 
                                 0, 
                                 0, 
                                 RM_OK, 
                                 NVFF8_NOTIFIERS_IMAGE_SCAN(1) );
        pVidFromMem->HalObject.Buffer[1].State = OVERLAY_BUFFER_IDLE;
    }

    //
    // Notify this one
    //
    notifyFillNotifierArray( pDev, pVidFromMem->BufferObj.Base.NotifyXlate, 
                             0, 
                             0, 
                             RM_OK, 
                             NVFF8_NOTIFIERS_IMAGE_SCAN(0) );
                    
    //
    // And the other one
    //
    notifyFillNotifierArray( pDev, pVidFromMem->BufferObj.Base.NotifyXlate, 
                             0, 
                             0, 
                             RM_OK, 
                             NVFF8_NOTIFIERS_IMAGE_SCAN(1) );
    //
    // Issue stop-immediately call to HAL.
    //
    (void) nvHalVideoMethod(pDev, NV_VIDEO_FROM_MEMORY, &pVidFromMem->HalObject, NVFF8_STOP_TRANSFER(0), NVFF8_STOP_TRANSFER_VALUE, &status);
    
    //
    // Transition state of object back to uninitialized.
    //
    pVidFromMem->InitState = 0;

    //
    // Video has been disabled, so refresh arb settings.
    //
    nvHalDacUpdateArbSettings(pDev, (VOID*) &VidLutCurDac);

    // Clear the active overlay object.
    if ((POBJECT) pDev->Video.HalInfo.ActiveVideoOverlayObject == (POBJECT) pVidFromMem)
        pDev->Video.HalInfo.ActiveVideoOverlayObject = (VOID_PTR) NULL;

    return status;
}

RM_STATUS class63SetVideoOutput
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class63SetVideoOutput\r\n");

    return (RM_OK);
}

RM_STATUS class63SetImageCtxDma
(
	PHWINFO pDev,
    POBJECT Object, 
    PMETHOD Method, 
    U032    Offset, 
    V032    Data
)
{
    PVIDEOFROMMEMOBJECT pVidFromMem;
    U032 buffNum;
    RM_STATUS status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class63SetImageCtxDma\r\n");

    pVidFromMem = (PVIDEOFROMMEMOBJECT)Object;
    buffNum = (Offset - Method->Low) >> 2;

    //
    // Set the image buffer DMA context.
    //
    status = mthdSetBufferCtxDma(pDev, Object, Method, Offset, Data);

    //
    // Give HAL access to context dma.
    //
    pVidFromMem->HalObject.Buffer[buffNum].bufferCtxDma = &pVidFromMem->BufferObj.Buffer[buffNum].Xlate->HalInfo;

    return status;
}

RM_STATUS class63ImageScan
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    VIDEO_LUT_CURSOR_DAC_HAL_OBJECT VidLutCurDac;
    PVIDEOFROMMEMOBJECT     pVidFromMem;
    PVIDEOFROMMEMHALOBJECT  pVidFromMemHalObj;
    PBUFFEROBJECT           pBuffObject;
    U032                    Buffer;
    RM_STATUS               status;
    U032                    Size;
    U032                    Head = 0; // single-head class

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class63ImageScan\r\n");

    VidLutCurDac.Head = Head;

    //
    // A nice cast to make the code more readable.
    //
    pVidFromMem = (PVIDEOFROMMEMOBJECT)Object;
    pVidFromMemHalObj = &pVidFromMem->HalObject;
    pBuffObject = (PBUFFEROBJECT)Object;
    Buffer = ((Offset - Method->Low) <= 0x10) ? 0 : 1;

    //
    // Initialize the image buffer description.
    //
    switch (Offset - Method->Low)
    {
        //
        // Set Offset
        //
        case 0x00:
        case 0x14:
            dmaSetBufferStart(pBuffObject, &(pBuffObject->Buffer[Buffer]), Offset, Data);
            pVidFromMemHalObj->Buffer[Buffer].Start = pBuffObject->Buffer[Buffer].Start;
            break;
        //
        // Set Pitch
        //
        case 0x04:
        case 0x18:
            dmaSetBufferPitch(pBuffObject, &(pBuffObject->Buffer[Buffer]), Offset, Data);
            pVidFromMemHalObj->Buffer[Buffer].Pitch = pBuffObject->Buffer[Buffer].Pitch;
            break;
        //
        // Set Size
        //
        case 0x08:
        case 0x1C:

            //
            // Save away the width separately
            //
            pBuffObject->Buffer[Buffer].Width = (Data & 0xFFFF);
            pVidFromMemHalObj->Buffer[Buffer].Width = (Data & 0xFFFF);

            // Cheat for now to get the length
            Size = (Data & 0xFFFF) * ((Data & 0xFFFF0000)>>16);
            dmaSetBufferLength(pBuffObject, &(pBuffObject->Buffer[Buffer]), Offset, Size);
            pVidFromMemHalObj->Buffer[Buffer].Length = pBuffObject->Buffer[Buffer].Length;
            break;
        //
        // Set Format.
        //
        case 0x0C:
        case 0x20:
			//
			// For now, we must make the assumption that both buffers
			// have the same format.  Makes sense -- but if we want
			// different formats in the future, just move the format
			// field down from the BufferObj into the individual buffers.
			//
            pBuffObject->Sample.Format = Data;
            pVidFromMemHalObj->Format = Data;

            // Q: How do we know that this object is the one pointing to the h/w?
            // If this is a s/w only object then need to disable any writes to h/w!
			
            break;
        //
        // Set Notify
        //
        case 0x10:
        case 0x24:
#ifdef DEBUG_TIMING            
            if ((Offset - Method->Low) == 0x10)
            {    
                //DBG_PRINT_TIME("VideoKickoff0: ",REG_RD32(NV_PTIMER_TIME_1) - time0hi);
                DBG_PRINT_TIME("VideoKickoff0: ",REG_RD32(NV_PTIMER_TIME_0) - time0lo);
                    
                time0hi = REG_RD32(NV_PTIMER_TIME_1);
                time0lo = REG_RD32(NV_PTIMER_TIME_0);
            } 
            else
            {    
                //DBG_PRINT_TIME("VideoKickoff1: ",REG_RD32(NV_PTIMER_TIME_1) - time1hi);
                DBG_PRINT_TIME("VideoKickoff1: ",REG_RD32(NV_PTIMER_TIME_0) - time1lo);
             
                time1hi = REG_RD32(NV_PTIMER_TIME_1);
                time1lo = REG_RD32(NV_PTIMER_TIME_0);
            }
#endif // DEBUG_TIMING            
            //
            // Verify all required data has been sent
            //  verify rest of imagescan

            //
            // YUV Buffers will be handled normally; RGB buffer will be
            // special cased, since they really aren't double-buffered.
            //
            if (((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_Y8) ||
                ((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_GY1R5G5B5) ||
                ((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_R5G6B5) ||
                ((pVidFromMemHalObj->Format & 0x0FFFFFFF) == NVFF8_IMAGE_SCAN_FORMAT_COLOR_LE_GY1X7R8G8B8))
            {
                U032 when;
                U032 Head = 0;  // this is a single-head class
                //
                // We assume the format, pitch, and length are the same as the 
                // framebuffer.
                //
                // RGB video is not handled as a true double-buffered object,
                // but instead acts as a single-shot transfer with notify.
                // When the app wants a transfer, it sets a notify.  Nothing
                // really moves automatically (unlike back-end Video).
                //

                //
                // !!!!! SIMPLE VALIDATATION NEEDED !!!!!
                //  verify contexts

                //
                // Make sure outstanding notification doesn't exist.
                //
                if (pVidFromMem->VBlankNotify[Buffer].Pending)
                    return (NVFF8_NOTIFICATION_STATUS_IN_PROGRESS);

                //
                // This class does not have support for flipping on HSync.
                // That support is only in class 049 (NV05_VIDEO_LUT_CURSOR_DAC).
                // Simulate it with a registry setting.
                //
                if (IsNV5orBetter(pDev) && pDev->Video.FlipOnHsync == 0x1) 
                {
                    // Flip on HSync instead of VSync.
                    when = NV049_SET_IMAGE_FORMAT_WHEN_IMMEDIATELY; // just use the class 49 define.
                }
                else
                {
                    when = NV049_SET_IMAGE_FORMAT_WHEN_NOT_WITHIN_BUFFER;
                }

                //
                // Validate notification.
                //
                status = dmaValidateXlate(pVidFromMem->BufferObj.Base.NotifyXlate,
                                          0,
                                          0x000F);

                dacProgramVideoStart(pDev, Head,
                                     pBuffObject->Buffer[Buffer].Start,
                                     pBuffObject->Buffer[Buffer].Pitch);

                if (!IsNV10orBetter(pDev) && when == NV049_SET_IMAGE_FORMAT_WHEN_IMMEDIATELY)
                {
                    // Change the default flip functionality to end of hsync.
                    FLD_WR_DRF_DEF(_PCRTC, _CONFIG, _START_ADDRESS, _HSYNC_NV5);

                    // Don't wait for VSync for the notify. Just do it right here. The assumption is that
                    // HSync will happen before the driver pays attention to the notification.
                    // Do everything that would have normally been done in VBlank.
                    pDev->Framebuffer.FlipFrom = pDev->Framebuffer.Current;
                    pDev->Framebuffer.FlipTo   = pDev->Framebuffer.Current ^ 1;
                    pDev->Framebuffer.Current  ^= 1;

                    class63XferComplete(pDev, Object, Offset, Buffer, RM_OK);
                }
                else
                {
                    //
                    // We want to get a notify when done transferring to Buffer.  The
                    // VBlank notify algorithm works on which buffer you're transferring
                    // from, so we need to flip from the current buffer.  This dictates
                    // that the next vblank will cause this callback.
                    //
                    pDev->Framebuffer.HalInfo.Start[pDev->Framebuffer.Current ^ 1] = pBuffObject->Buffer[Buffer].Start;
                    pDev->Framebuffer.FlipTo = pDev->Framebuffer.Current ^ 1;
                    pDev->Framebuffer.UpdateFlags |= UPDATE_HWINFO_BUFFER_FLIP;
                    pVidFromMem->VBlankNotify[Buffer].Object  = Object;
                    pVidFromMem->VBlankNotify[Buffer].Proc    = class63XferComplete;
                    pVidFromMem->VBlankNotify[Buffer].Next    = NULL;
                    pVidFromMem->VBlankNotify[Buffer].Pending = FALSE;
                    pVidFromMem->VBlankNotify[Buffer].Xlate   = pVidFromMem->BufferObj.Base.NotifyXlate;
                    pVidFromMem->VBlankNotify[Buffer].Action  = Data;
                    pVidFromMem->VBlankNotify[Buffer].Param1  = Offset;
                    pVidFromMem->VBlankNotify[Buffer].Param2  = Buffer;

                    status = class63WaitVBlank(pDev, &(pVidFromMem->VBlankNotify[Buffer]), 
                                               pDev->Framebuffer.Current ^ 1, 1);
                }
            } // RGB Video
            else
            {
                //
                // YUV
                //
				// Here is an overview of how hardware overlay works:
                // VideoFroMem, VideoScaler and VideoColorKey objects are setup by DDraw before calling
                // the VideoFromMem->Notify method (which is where we are now).
                // 
                // The Notify method on VideoFromMemory class tells the RM that all the values for one
                // frame are setup and we should program the hardware. The RM then marks the appropriate
                // buffer as BUFFER_BUSY. 
                //
                // class63VideoService is the interrupt handler that handles the "scanout complete"
                // interrupt from the video engine. The video engine is double buffered. So the video
                // engine is constantly toggling the "current buffer". The RM tries to keep feeding
                // the video engine with new values.
                //
                // After handling the notification (if any) of a buffer completion, VideoService tries
                // to kickoff the other buffer. (For example, at the completion of buffer 0, we will try
                // to kickoff buffer 1 if it is in state BUFFER_BUSY). When a buffer is actually kicked off
                // (i.e loaded into the hardware registers), its state is marked BUFFER_NOTIFY_PENDING.
                //
                // If the other buffer is not BUFFER_BUSY at the completion of a buffer, we will restart
                // the same buffer by forcing the hardware to flip to the same buffer again. This is so
                // that we will always get an interrupt even when there are no new values to be loaded.
                // It is necessary to get an interrupt because the interrupt handler is the only place
                // (other than the first time) we actually kickoff a buffer by programming the hardware
                // registers.
                // 
                // State transitions of a buffer:
                // A buffer goes from state BUFFER_IDLE to BUFFER_BUSY when the notify method is invoked.
                // It goes from BUFFER_BUSY to BUFFER_NOTIFY_PENDING when its values are actually programmed
                // into the appropriate hardware registers (in class63InitXfer the very first time and in
                // class63VideoService the rest of the time).
                // It goes from BUFFER_NOTIFY_PENDING to BUFFER_IDLE when the hardware is finished scanning out
                // that buffer and a notification is written for it (in class63VideoService).
                //

                //
                // This will also kick off a call to the bufferXfer proc for this buffer.
                // The very first time the bufferXfer proc is setup to call class63InitXfer.
                //
                if (pBuffObject->Buffer[Buffer].Length) // KJK quick hack so we don't start running immediately!!
                {
                    if (pVidFromMemHalObj->Buffer[Buffer].State != OVERLAY_BUFFER_IDLE)
                    {
                        // Trying to kickoff a busy buffer.
#ifdef DEBUG_TIMING
                        DBG_PRINT_STRING_VALUE(0x10, "******TRYING TO KICKOFF BUSY BUFFER********: ",Buffer);
#endif
                        return NVFF8_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
                    }

                    //
                    // Check for DMA translation instances.
                    //
                    if (!pBuffObject->Buffer[Buffer].Xlate->HalInfo.Instance ||
                        !pBuffObject->Buffer[Buffer].NotifyXlate->HalInfo.Instance)
                        return (RM_ERR_INVALID_XLATE);
                    
                    pBuffObject->Buffer[Buffer].NotifyAction = Data;

                    //
                    // If this is the init stage on this object,
                    // make sure the video overlay hardware is available.
                    //
                    if (pVidFromMem->InitState == 0)
                    {
                        if ((POBJECT) pDev->Video.HalInfo.ActiveVideoOverlayObject != (POBJECT) NULL)
                        {
                            return NV_OUT_OF_RESOURCES;
                        }

                        //
                        // Validate patch
                        // XXX (scottl): do we need this anymore?!?!?
                        if (!pBuffObject->Valid)
                        {
                            pVidFromMem->CBase.ChID = pBuffObject->Base.ChID;
                            pBuffObject->Valid = TRUE;
                        }
                    }


                    //
                    // Call into HAL to program up any hw.
                    //
                    (void) nvHalVideoMethod(pDev, NV_VIDEO_FROM_MEMORY, &pVidFromMem->HalObject, NVFF8_IMAGE_SCAN_NOTIFY(0, Buffer), Data, &status);
                    if (status)
                    {
                        notifyFillNotifierArray( pDev, pVidFromMem->BufferObj.Base.NotifyXlate, 
                                                 0, 
                                                 0, 
                                                 status, 
                                                 NVFF8_NOTIFIERS_IMAGE_SCAN(Buffer) );
                    }
                    else
                    {
                        //
                        // If video has just been enabled, then refresh arb settings.
                        //
                        if (pVidFromMem->InitState == 0)
                        {
                            nvHalDacUpdateArbSettings(pDev, (VOID*) &VidLutCurDac);
                            pVidFromMem->InitState = 1;
                        }

                        // Make this the active overlay object.
                        pDev->Video.HalInfo.ActiveVideoOverlayObject = (VOID_PTR) pVidFromMem;
                        pDev->pOverlayServiceHandler   = videoFromMemService;
                    }
                }
                else
                {
                	// Length is 0. Assume we want to stop the overlay.
                    class63StopTransfer(pDev, (POBJECT)pVidFromMem, 
                                        0 /*Method */, 0 /* Offset */, 0 /* data */);
                }
            }
            break;
    }
    return (RM_OK);
}

RM_STATUS class63GetOffsetNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    return (RM_OK);
}    

//---------------------------------------------------------------------------
//
//  Buffer transfer routines. 
//
//---------------------------------------------------------------------------

static RM_STATUS class63XferComplete
(
    PHWINFO   pDev,
    POBJECT   Object,
    U032      Method,
    V032      Data,
    RM_STATUS Status
)
{
    PVIDEOFROMMEMOBJECT pVidFromMem;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class63XferComplete\r\n");

    //
    // A nice cast to make the code more readable.
    //
    pVidFromMem = (PVIDEOFROMMEMOBJECT)Object;

    notifyFillNotifierArray( pDev, pVidFromMem->BufferObj.Base.NotifyXlate, 
                             0, 
                             0, 
                             Status, 
                             NVFF8_NOTIFIERS_IMAGE_SCAN(Data) );
                             
    //KJK dmaEndXfer(pVidFromMem->BBuffer[Data].NotifyXlate, NULL);
    //
    // Do any OS specified action related to this notification.
    //
    if (pVidFromMem->BBuffer[Data].NotifyAction)
    {
        osNotifyEvent(pDev, Object, 
                      NVFF8_NOTIFIERS_IMAGE_SCAN(Data),
                      0, 
                      1, 
                      RM_OK, 
                      pVidFromMem->BBuffer[Data].NotifyAction);
                       
    }
    pVidFromMem->VBlankNotify[Data].Pending = FALSE;
    return (RM_OK);
}
