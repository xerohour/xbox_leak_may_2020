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
* Module: class046.C                                                        *
*   This module implements the NV04_VIDEO_LUT_CURSOR_DAC object class and   *
*   its corresponding methods.                                              *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <stddef.h>    // for size_t
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

RM_STATUS class046SetNotifyCtxDma
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    RM_STATUS                    status;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class046SetNotifyCtxDma\r\n");
    //
    // Set the notify DMA context.
    //
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;
    
    if (pVidLutCurDac->CBase.NotifyTrigger          ||
        pVidLutCurDac->Image[0].NotifyTrigger       ||
        pVidLutCurDac->Image[1].NotifyTrigger       ||
        pVidLutCurDac->LUT[0].NotifyTrigger       ||
        pVidLutCurDac->LUT[1].NotifyTrigger       ||
        pVidLutCurDac->CursorImage[0].NotifyTrigger ||
        pVidLutCurDac->CursorImage[1].NotifyTrigger ||
        pVidLutCurDac->Dac[0].NotifyTrigger         ||
        pVidLutCurDac->Dac[1].NotifyTrigger)
        return NV046_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

    //
    // Connect to dma buffer.
    //    
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &(pVidLutCurDac->CBase.NotifyXlate));
    if (status)
        return (NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    
    //
    // Validate notification parameters.
    //
    status = dmaValidateXlate(pVidLutCurDac->CBase.NotifyXlate, 
                              0, 
                              (9 * 0x0010));
    if (status)
    {
        pVidLutCurDac->CBase.NotifyXlate = NULL;
        return (NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    }
    
    return (status);
} // end of class046SetNotifyCtxDma

RM_STATUS class046SetNotify
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class046SetNotify\r\n");
    //
    // Set the notification style (note that these will be s/w notifies!)
    //

    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;

    pVidLutCurDac->CBase.NotifyAction  = Data;
    pVidLutCurDac->CBase.NotifyTrigger = TRUE;
    
    return (NV046_NOTIFICATION_STATUS_DONE_SUCCESS);
} // end of class046SetNotify

RM_STATUS class046StopImage
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class046StopImage\r\n");

    // class046 supports only one head (head 0)
    dacDisableImage(pDev, 0);

    return (NV046_NOTIFICATION_STATUS_DONE_SUCCESS);
} // end of class046StopImage

RM_STATUS class046StopLUT
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class046StopLUT\r\n");
    return (NV046_NOTIFICATION_STATUS_DONE_SUCCESS);
} // end of class046StopLUT

RM_STATUS class046StopCursorImage
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    U032    Head = 0;   // single-head class
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class046StopCursorImage\r\n");

    //
    // Disable the current cursor.  Note this will currently not preempt any
    // pending notifies via our vblank callback mechanism.  Should probably
    // add that at some point.
    //
    dacDisableCursor(pDev, Head);

    return (NV046_NOTIFICATION_STATUS_DONE_SUCCESS);
} // end of class046StopCursorImage

RM_STATUS class046StopDAC
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class046StopDAC\r\n");

    // class046 supports only one head (head 0).
    dacDisableDac(pDev, 0);

    // reset display type
    SETDISPLAYTYPE(pDev, 0, 0xffffffff);
    
    return (NV046_NOTIFICATION_STATUS_DONE_SUCCESS);
} // end of class046StopDAC

RM_STATUS class046SetImageCtxDma
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    RM_STATUS                    status;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    U032                         buffNum;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class046SetImageCtxDma\r\n");
    //
    // Set the Image DMA context.
    //
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;
    buffNum = (Offset == NV046_SET_CONTEXT_DMA_IMAGE(0)) ? 0 : 1;
    
    if (pVidLutCurDac->Image[buffNum].NotifyTrigger)
        return NV046_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

        if (Data == NV01_NULL_OBJECT)
    {
        pVidLutCurDac->Image[buffNum].ImageCtxDma = NULL;
        return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
    }
    //
    // Connect to dma buffer.
    //    
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &(pVidLutCurDac->Image[buffNum].ImageCtxDma));
    if (status)
        return (NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    
    //
    // Validate the dma context.
    // The image context dma must point to the frame buffer.
    if (pVidLutCurDac->Image[buffNum].ImageCtxDma->HalInfo.AddressSpace != ADDR_FBMEM) 
    {
        pVidLutCurDac->Image[buffNum].ImageCtxDma = NULL;
        return (NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    }

    return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
} // end of class046SetImageCtxDma

RM_STATUS class046SetLUTCtxDma
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    RM_STATUS                    status;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    U032                         buffNum;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class046SetLUTCtxDma\r\n");
    //
    // Set the Image DMA context.
    //
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;
    buffNum = (Offset == NV046_SET_CONTEXT_DMA_LUT(0)) ? 0 : 1;
    
    if (pVidLutCurDac->LUT[buffNum].NotifyTrigger)
        return NV046_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

        if (Data == NV01_NULL_OBJECT)
    {
        pVidLutCurDac->LUT[buffNum].LUTCtxDma = NULL;
        return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
    }
    //
    // Connect to dma buffer.
    //    
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &(pVidLutCurDac->LUT[buffNum].LUTCtxDma));
    if (status)
        return (NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    
    //
    // Validate the dma context.
    //
    status = dmaValidateXlate(pVidLutCurDac->LUT[buffNum].LUTCtxDma, 
                              0, 
                              0x400);
    if (status)
    {
        pVidLutCurDac->LUT[buffNum].LUTCtxDma = NULL;
        return (NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    }
    
    return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
} // end of  class046SetLUTCtxDma

RM_STATUS class046SetCursorCtxDma
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    RM_STATUS                    status;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    U032                         buffNum;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class046SetCursorCtxDma\r\n");
    //
    // Set the Cursor DMA context.
    //
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;
    buffNum = (Offset == NV046_SET_CONTEXT_DMA_CURSOR(0)) ? 0 : 1;
    
    if (pVidLutCurDac->CursorImage[buffNum].NotifyTrigger)
        return NV046_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

        if (Data == NV01_NULL_OBJECT)
    {
        pVidLutCurDac->CursorImage[buffNum].CursorCtxDma = NULL;
        return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
    }
    //
    // Connect to dma buffer.
    //    
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &(pVidLutCurDac->CursorImage[buffNum].CursorCtxDma));
    if (status)
        return (NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    
    //
    // Validate the dma context.
    //
    status = dmaValidateXlate(pVidLutCurDac->CursorImage[buffNum].CursorCtxDma, 
                              0, 
                              0x800);
    if (status)
    {
        pVidLutCurDac->CursorImage[buffNum].CursorCtxDma = NULL;
        return (NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    }
    // The cursor image context dma must point to the frame buffer.
    if (pVidLutCurDac->CursorImage[buffNum].CursorCtxDma->HalInfo.AddressSpace != ADDR_FBMEM) 
    {
        pVidLutCurDac->CursorImage[buffNum].CursorCtxDma = NULL;
        return (NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    }

    return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;

} // end of class046SetCursorCtxDma

RM_STATUS class046SetPanOffset
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;

    // For this head, this becomes the current pan offset
    pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].CurrentPanOffset = Data;

    // 
    // Set Image[].Offset (like the NV046_SET_IMAGE_OFFSET method) to this
    // heads CurrentImageOffset. Note, we're assuming the display driver is
    // doing its flips through buffNum0 (similar to the mode set).
    //
    pVidLutCurDac->Image[0x0].Offset =
        pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].CurrentImageOffset;

    return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
} // end of class046SetPanOffset

RM_STATUS class046GetOffset
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    RM_STATUS                    status;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class046GetOffset\r\n");
    
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;
    if (!pVidLutCurDac->CBase.NotifyTrigger)
        // nothing to do.
        return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;

    // The caller is expecting a notification. 
    // Write the image offset in the INFO32 of the notification.
    if (pVidLutCurDac->CBase.NotifyXlate) 
    {
        status = notifyFillNotifierArray(pDev, pVidLutCurDac->CBase.NotifyXlate, 
                                         pVidLutCurDac->Image[Data].Offset, 
                                         NV046_NOTIFICATION_INFO16_VALID_OFFSET, 
                                         NV046_NOTIFICATION_STATUS_DONE_SUCCESS,
                                         NV046_NOTIFIERS_NOTIFY);
        //
        // Do any OS specified action related to this notification.
        //
        if (pVidLutCurDac->CBase.NotifyAction)
        {
            osNotifyEvent(pDev, (POBJECT)pVidLutCurDac, 
                          NV046_NOTIFIERS_NOTIFY,
                          0 /* Method */, 
                          0 /* Data */, 
                          status, 
                          pVidLutCurDac->CBase.NotifyAction);
        }
    }
    pVidLutCurDac->CBase.NotifyTrigger = FALSE; 
    return (NV046_NOTIFICATION_STATUS_DONE_SUCCESS);
} // end of class046GetOffset

// This proc writes the notification for an image buffer.
RM_STATUS class046ImageNotify
(
    PHWINFO   pDev,
    POBJECT   Object,
    U032      param1,
    V032      BuffNum,
    RM_STATUS Status
)
{
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;

    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;

    // increment the flip count and send it back through the notifier
    pVidLutCurDac->CompletedFlipCount++;

    if (pVidLutCurDac->CBase.NotifyXlate) 
    {
        notifyFillNotifierArray(pDev, pVidLutCurDac->CBase.NotifyXlate,
                                pVidLutCurDac->CompletedFlipCount, // Info32
                                0, 
                                Status,
                                NV046_NOTIFIERS_SET_IMAGE(BuffNum));
        //
        // Do any OS specified action related to this notification.
        //
        if (pVidLutCurDac->Image[BuffNum].NotifyAction)
        {
            osNotifyEvent(pDev, (POBJECT)pVidLutCurDac, 
                          NV046_NOTIFIERS_SET_IMAGE(BuffNum),
                          0 /* Method */, 
                          0 /* Data */, 
                          Status, 
                          pVidLutCurDac->Image[BuffNum].NotifyAction);
        }
    }
    pVidLutCurDac->Image[BuffNum].NotifyTrigger = FALSE;   
    return (NV046_NOTIFICATION_STATUS_DONE_SUCCESS);
}

RM_STATUS class046SetImageValues
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    RM_STATUS                    status;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    U032                         buffNum;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class046SetImageValues\r\n");

    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;
        //
    // Initialize the image buffer description.
    //
    switch (Offset)
    {
        //
        // Set Image Offset
        //
        case NV046_SET_IMAGE_OFFSET(0):
        case NV046_SET_IMAGE_OFFSET(1):
        {
            buffNum = (Offset == NV046_SET_IMAGE_OFFSET(0)) ? 0 : 1;
            if (pVidLutCurDac->Image[buffNum].NotifyTrigger)
                return NV046_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
                
            if ((Data & 0x03) != 0)
                // The image offset must be a multiple of 4 bytes.
                return NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
                
            // Save the CurrentImageOffset for this head
            pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].CurrentImageOffset = Data;
            pVidLutCurDac->Image[buffNum].Offset = Data;
            return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
        }
        //
        // Set Image Format
        //
        case NV046_SET_IMAGE_FORMAT(0):
        case NV046_SET_IMAGE_FORMAT(1):
        {
            U016  pitch;
            U032  startAddr;
            U032  Head = 0;   // single-head class
            U032  flags;
            
            buffNum = (Offset == NV046_SET_IMAGE_FORMAT(0)) ? 0 : 1;
            if (pVidLutCurDac->Image[buffNum].NotifyTrigger)
                return NV046_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            if (pVidLutCurDac->Image[buffNum].ImageCtxDma == NULL)
                return NV046_NOTIFICATION_STATUS_ERROR_INVALID_STATE;

            // pitch is specified by bits 15:0.
            pitch = (U016) (Data & 0x0000ffff);
            if ((pitch & 0x1F) != 0)
                // the minimum pitch must be a multiple of 32 bytes.
                return NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
                
            flags = DRF_VAL(046, _SET_IMAGE_FORMAT, _FLAGS, Data);

            pVidLutCurDac->Image[buffNum].Pitch = pitch;
            pDev->Dac.DisplayPitch = pitch;

            pVidLutCurDac->Image[buffNum].NotifyAction = (Data & BIT(DEVICE_BASE(NV046_SET_IMAGE_FORMAT_NOTIFY))) ? 1 : 0;
            pVidLutCurDac->Image[buffNum].NotifyTrigger = TRUE;

            // Program the hardware. It will take effect at the next VBlank.
            // The imageCtxDma points to the frame buffer. DescAddr has the offset of this buffer
            // from the start of the frame buffer.
            startAddr = (U032)((size_t)(pVidLutCurDac->Image[buffNum].ImageCtxDma->DescAddr) + 
                               pVidLutCurDac->Image[buffNum].Offset);
            // add in the panning offset
            startAddr += pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].CurrentPanOffset;

            status = dacProgramVideoStart(pDev, Head, startAddr, pitch);
            if (status) 
                return NV046_NOTIFICATION_STATUS_ERROR_INVALID_STATE;

            // setup so we get a callback to do the notify at the next VBlank.
            pVidLutCurDac->Image[buffNum].ImageCallback.Proc   = class046ImageNotify;
            pVidLutCurDac->Image[buffNum].ImageCallback.Object = (POBJECT) pVidLutCurDac;
            pVidLutCurDac->Image[buffNum].ImageCallback.Next    = NULL;
            pVidLutCurDac->Image[buffNum].ImageCallback.Param1  = 0;
            // Set param2 to the buffer number so we know which notifier to write.
            pVidLutCurDac->Image[buffNum].ImageCallback.Param2  = buffNum;
            pVidLutCurDac->Image[buffNum].ImageCallback.Status  = NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
            pVidLutCurDac->Image[buffNum].ImageCallback.Flags   = 0;

            if (flags & NV046_SET_IMAGE_FORMAT_FLAGS_COMPLETE_ON_OBJECT_CLEANUP)
                pVidLutCurDac->Image[buffNum].ImageCallback.Flags |= CALLBACK_FLAG_COMPLETE_ON_OBJECT_CLEANUP;

            // Defaults to head 0.
            VBlankAddCallback(pDev, 0, &(pVidLutCurDac->Image[buffNum].ImageCallback));
            return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
            break;
        }
            
        default:
            return NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            break;
     }

    return RM_OK;

} // end of class046SetImageValues

// This proc is called from VBlank to program the LUT.
RM_STATUS class046ProgramLUT
(
    PHWINFO   pDev,
    POBJECT   Object,
    U032      param1,
    V032      BuffNum,
    RM_STATUS Status
)
{
    RM_STATUS myStatus;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    U032*     startAddr;
    U032      numEntries;
    U032    Head = 0;   // single-head class

    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;

    // Program the LUT.
    numEntries = 256;
    myStatus = dmaGetMappedAddress(pDev, pVidLutCurDac->LUT[BuffNum].LUTCtxDma, pVidLutCurDac->LUT[BuffNum].Offset, 
                                   numEntries * 4, (void **)(&startAddr));
    if (!myStatus)
    {
        myStatus = dacProgramLUT(pDev, Head, startAddr, numEntries);
    }


    // Notify.
    if (pVidLutCurDac->CBase.NotifyXlate) 
    {
        notifyFillNotifierArray(pDev, pVidLutCurDac->CBase.NotifyXlate, 0, 0, 
                                myStatus,
                                NV046_NOTIFIERS_SET_LUT(BuffNum));
        //
        // Do any OS specified action related to this notification.
        //
        if (pVidLutCurDac->LUT[BuffNum].NotifyAction)
        {
            osNotifyEvent(pDev, (POBJECT)pVidLutCurDac, 
                          NV046_NOTIFIERS_SET_LUT(BuffNum),
                          0 /* Method */, 
                          0 /* Data */, 
                          myStatus, 
                          pVidLutCurDac->LUT[BuffNum].NotifyAction);
        }
    }
    pVidLutCurDac->LUT[BuffNum].NotifyTrigger = FALSE;   
    return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
}

RM_STATUS class046SetLUTValues
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    U032                         buffNum;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class046SetLUTValues\r\n");

    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;
        //
    // Initialize the LUT buffer description.
    //
    switch (Offset)
    {
        //
        // Set LUT Offset
        //
        case NV046_SET_LUT_OFFSET(0):
        case NV046_SET_LUT_OFFSET(1):
        {
            buffNum = (Offset == NV046_SET_LUT_OFFSET(0)) ? 0 : 1;
            if (pVidLutCurDac->LUT[buffNum].NotifyTrigger)
                return NV046_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
                
            if ((Data & 0x3FF) != 0)
                // The LUT offset must be a multiple of 1024 bytes.
                return NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
                
            pVidLutCurDac->LUT[buffNum].Offset = Data;
            return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
        }
        //
        // Set LUT Format
        //
        case NV046_SET_LUT_FORMAT(0):
        case NV046_SET_LUT_FORMAT(1):
        {
            buffNum = (Offset == NV046_SET_LUT_FORMAT(0)) ? 0 : 1;
            if (pVidLutCurDac->LUT[buffNum].NotifyTrigger)
                return NV046_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
                
            pVidLutCurDac->LUT[buffNum].NotifyAction = Data;
            pVidLutCurDac->LUT[buffNum].NotifyTrigger = TRUE;
            // We can only program the LUT during VBlank.
            // Setup a callback.
            pVidLutCurDac->LUT[buffNum].LUTCallback.Proc   = class046ProgramLUT;
            pVidLutCurDac->LUT[buffNum].LUTCallback.Object = (POBJECT) pVidLutCurDac;
            pVidLutCurDac->LUT[buffNum].LUTCallback.Next    = NULL;
            pVidLutCurDac->LUT[buffNum].LUTCallback.Param1  = 0;
            // Set param2 to the buffer number so we know which notifier to write.
            pVidLutCurDac->LUT[buffNum].LUTCallback.Param2  = buffNum;
            pVidLutCurDac->LUT[buffNum].LUTCallback.Status  = NV046_NOTIFICATION_STATUS_DONE_SUCCESS;

            VBlankAddCallback(pDev, DACGETHEAD(pVidLutCurDac), &(pVidLutCurDac->LUT[buffNum].LUTCallback));
            return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
            break;
        }
            
        default:
            return NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            break;
     }
} // end of class046SetLUTValues

// This proc writes the notification for an Cursor buffer.
RM_STATUS class046CursorImageNotify
(
    PHWINFO   pDev,
    POBJECT   Object,
    U032      param1,
    V032      BuffNum,
    RM_STATUS Status
)
{
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;

    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;
    if (pVidLutCurDac->CBase.NotifyXlate) 
    {
        notifyFillNotifierArray(pDev, pVidLutCurDac->CBase.NotifyXlate, 0, 0, 
                                Status,
                                NV046_NOTIFIERS_SET_CURSOR_IMAGE(BuffNum));
        //
        // Do any OS specified action related to this notification.
        //
        if (pVidLutCurDac->CursorImage[BuffNum].NotifyAction)
        {
            osNotifyEvent(pDev, (POBJECT)pVidLutCurDac, 
                          NV046_NOTIFIERS_SET_CURSOR_IMAGE(BuffNum),
                          0 /* Method */, 
                          0 /* Data */, 
                          Status, 
                          pVidLutCurDac->CursorImage[BuffNum].NotifyAction);
        }
    }
    pVidLutCurDac->CursorImage[BuffNum].NotifyTrigger = FALSE;   
    return (NV046_NOTIFICATION_STATUS_DONE_SUCCESS);
}

RM_STATUS class046SetCursorImageValues
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    RM_STATUS                    status;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    U032                         buffNum;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class046SetCursorImageValues\r\n");
    
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;
        //
    // Initialize the image buffer description.
    //
    switch (Offset)
    {
        //
        // Set Cursor Image Offset
        //
        case NV046_SET_CURSOR_IMAGE_OFFSET(0):
        case NV046_SET_CURSOR_IMAGE_OFFSET(1):
        {
            buffNum = (Offset == NV046_SET_CURSOR_IMAGE_OFFSET(0)) ? 0 : 1;
            if (pVidLutCurDac->CursorImage[buffNum].NotifyTrigger)
                return NV046_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
                
            if ((Data & 0x3ff) != 0)
                // The cursor image offset must be a multiple of 1024 bytes.
                return NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
                
            pVidLutCurDac->CursorImage[buffNum].Offset = Data;
            return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
        }
        //
        // Set Cursor Image Format
        //
        case NV046_SET_CURSOR_IMAGE_FORMAT(0):
        case NV046_SET_CURSOR_IMAGE_FORMAT(1):
        {
            U032 startAddr;
            U032    Head = 0;   // single-head class
            BOOL queue_callback = TRUE;

            // Unlike the other IMAGE_FORMAT routines, we do allow multiple calls
            // during a single frame time. But, we still have only one callback struct.
            // So allow the args to be updated and the HW to be reprogrammed for the
            // new cursor. Just don't requeue the callback struct, since this could
            // lose an existing callback already on the list.

            buffNum = (Offset == NV046_SET_CURSOR_IMAGE_FORMAT(0)) ? 0 : 1;
            if (pVidLutCurDac->CursorImage[buffNum].NotifyTrigger)
                queue_callback = FALSE;

            // The cursor is always 32x32 image with a color format LE_ROP1R5G5B5. 
            pVidLutCurDac->CursorImage[buffNum].Width       = 32; 
            pVidLutCurDac->CursorImage[buffNum].Height      = 32; 
            pVidLutCurDac->CursorImage[buffNum].ColorFormat = 0;  // LE_ROP1R5G5B5

            pVidLutCurDac->CursorImage[buffNum].NotifyAction = Data; 

            // Kick off the trasfer at this time.
            pVidLutCurDac->CursorImage[buffNum].NotifyTrigger = TRUE;

            // Program the hardware. It will take effect at the next VBlank.
            // The cursorImageCtxDma points to the frame buffer. DescAddr has the offset of this buffer
            // from the start of the frame buffer.
            startAddr = (U032)((size_t)(pVidLutCurDac->CursorImage[buffNum].CursorCtxDma->DescAddr) + 
                                pVidLutCurDac->CursorImage[buffNum].Offset);
            status = nvHalDacProgramCursorImage(pDev, startAddr, NV_CIO_CRE_HCUR_ASI_FRAMEBUFFER,
                                           32 /* width */, 
                                           32 /* height */, 0 /* colorFormat LE_ROP1R5G5B5 */,
                                           (VOID *)&pVidLutCurDac->HalObject);
            if (status) 
                return NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;

            // Program was successful, so enable the cursor
            dacEnableCursor(pDev, Head);

            if (queue_callback)
            {
                // setup so we get a callback to do the notify at the next VBlank.
                pVidLutCurDac->CursorImage[buffNum].CursorImageCallback.Proc   = class046CursorImageNotify;
                pVidLutCurDac->CursorImage[buffNum].CursorImageCallback.Object = (POBJECT) pVidLutCurDac;
                pVidLutCurDac->CursorImage[buffNum].CursorImageCallback.Next    = NULL;
                pVidLutCurDac->CursorImage[buffNum].CursorImageCallback.Param1  = 0;
                // Set param2 to the buffer number so we know which notifier to write.
                pVidLutCurDac->CursorImage[buffNum].CursorImageCallback.Param2  = buffNum;
                pVidLutCurDac->CursorImage[buffNum].CursorImageCallback.Status  = NV046_NOTIFICATION_STATUS_DONE_SUCCESS;

                VBlankAddCallback(pDev, DACGETHEAD(pVidLutCurDac), &(pVidLutCurDac->CursorImage[buffNum].CursorImageCallback));
            }
            return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
        }
            
        default:
            return NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
     }

} // end of class046SetCursorImageValues

// This proc is called from VBlank to program the cursor position.
RM_STATUS class046ProgramCursorPoint
(
    PHWINFO   pDev,
    POBJECT   Object,
    U032      Data,
    V032      DummyParm,
    RM_STATUS Status
)
{
    RM_STATUS                    status;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    U032 Head = 0;   // single-head class

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class046SetCursorPoint\r\n");

    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;

    pVidLutCurDac->CursorPoint = Data;
    status = dacProgramCursorPosition(pDev, Head,
                                      (Data & 0xffff), /* cursorX */
                                      (Data >> 16));   /* cursorY */
                                      
    // signal that the callback is done
    pVidLutCurDac->CursorPointCallback.Proc = NULL;
    
    return (NV046_NOTIFICATION_STATUS_DONE_SUCCESS);

} // end of class046ProgramCursorPoint()

RM_STATUS class046SetCursorPoint
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    U032 Head = 0;   // single-head class
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class046SetCursorPoint\r\n");
    
    // update the cursor position
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;
    pVidLutCurDac->CursorPointCallback.Param1      = Data;
    if (pVidLutCurDac->CursorPointCallback.Proc == NULL)
    {
        // add the cursor position callback
        pVidLutCurDac->CursorPointCallback.Proc    = class046ProgramCursorPoint;
        pVidLutCurDac->CursorPointCallback.Object  = (POBJECT)pVidLutCurDac;
        pVidLutCurDac->CursorPointCallback.Next    = NULL;
        pVidLutCurDac->CursorPointCallback.Param2  = 0;
        pVidLutCurDac->CursorPointCallback.Status  = NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
        VBlankAddCallback(pDev, Head, &(pVidLutCurDac->CursorPointCallback));
    }
    
    return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;

} // end of class046SetCursorPoint

RM_STATUS class046SetDACValues
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    RM_STATUS                    status;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    U032                         buffNum;
    
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class046SetDACValues\r\n");

    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;

    //
    // Initialize the image buffer description.
    //
    switch (Offset)
    {
        //
        // Set DAC image size
        //
        case NV046_SET_DAC_IMAGE_SIZE(0):
        case NV046_SET_DAC_IMAGE_SIZE(1):
        {
            buffNum = (Offset == NV046_SET_DAC_IMAGE_SIZE(0)) ? 0 : 1;
            if (pVidLutCurDac->Dac[buffNum].NotifyTrigger)
                return NV046_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
                
            // Bits 15:0 contain the width and bits 31:16 contain the height in pixels.    
            // Width must be a multiple of 8.
            // Width and height must be less than or equal 2048. 
            if (((Data & 0x3) != 0)                 || // width not a multiple of 8
                ((Data & 0x0000ffff) > 2048)        || // width greater than 2048
                (((Data & 0xffff0000) >> 16) > 2048))  // height greater than 2048
                return NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
                
            pVidLutCurDac->HalObject.Dac[buffNum].VisibleImageWidth  = Data & 0x0000ffff;
            pVidLutCurDac->HalObject.Dac[buffNum].VisibleImageHeight = (Data & 0xffff0000) >> 16;
            return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
        }

        //
        // Set Horizontal Blank
        //
        case NV046_SET_DAC_HORIZONTAL_BLANK(0):
        case NV046_SET_DAC_HORIZONTAL_BLANK(1):
        {
            buffNum = (Offset == NV046_SET_DAC_HORIZONTAL_BLANK(0)) ? 0 : 1;
            if (pVidLutCurDac->Dac[buffNum].NotifyTrigger)
                return NV046_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
                
            // Bits 15:0 contain the start and bits 31:16 contain the width in pixels.    
            // Start must be a multiple of 8 and less than or equal to 4088.
            // Width must be a multiple of 8 and less than or equal to 256. 
            if (((Data & 0x3) != 0)                        || // start not a multiple of 8
                ((Data & 0x0000ffff) > 4088)               || // start greater than 4088
                ((((Data & 0xffff0000) >> 16) & 0x3) != 0) || // width not a multiple of 8
                (((Data & 0xffff0000) >> 16) > 1024))       // width greater than 1024
                return NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
                
            pVidLutCurDac->HalObject.Dac[buffNum].HorizontalBlankStart = Data & 0x0000ffff;
            pVidLutCurDac->HalObject.Dac[buffNum].HorizontalBlankWidth = (Data & 0xffff0000) >> 16;
            return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
        }
        //
        // Set Horizontal sync
        //
        case NV046_SET_DAC_HORIZONTAL_SYNC(0):
        case NV046_SET_DAC_HORIZONTAL_SYNC(1):
        {
            buffNum = (Offset == NV046_SET_DAC_HORIZONTAL_SYNC(0)) ? 0 : 1;
            if (pVidLutCurDac->Dac[buffNum].NotifyTrigger)
                return NV046_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
                
            // Bits 15:0 contain the start and bits 31:16 contain the width in pixels.    
            // Start must be a multiple of 8 and less than or equal to 4088.
            // Width must be a multiple of 8 and less than or equal to 256. 
            if (((Data & 0x3) != 0)                        || // start not a multiple of 8
                ((Data & 0x0000ffff) > 4088)               || // start greater than 4088
                ((((Data & 0xffff0000) >> 16) & 0x3) != 0) || // width not a multiple of 8
                (((Data & 0xffff0000) >> 16) > 256))          // width greater than 256
                return NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
                
            pVidLutCurDac->HalObject.Dac[buffNum].HorizontalSyncStart = Data & 0x0000ffff;
            pVidLutCurDac->HalObject.Dac[buffNum].HorizontalSyncWidth = (Data & 0xffff0000) >> 16;
            return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
        }
            
        //
        // Set Vertical Blank
        //
        case NV046_SET_DAC_VERTICAL_BLANK(0):
        case NV046_SET_DAC_VERTICAL_BLANK(1):
        {
            buffNum = (Offset == NV046_SET_DAC_VERTICAL_BLANK(0)) ? 0 : 1;
            if (pVidLutCurDac->Dac[buffNum].NotifyTrigger)
                return NV046_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
                
            // Bits 15:0 contain the start and bits 31:16 contain the width in pixels.    
            // Start must be less than or equal to 2047.
            // Width must be less than or equal to 16. 
            if (((Data & 0x0000ffff) > 2047)       || // start greater than 2047
                (((Data & 0xffff0000) >> 16) > 128))  // width greater than 128
                return NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
                
            pVidLutCurDac->HalObject.Dac[buffNum].VerticalBlankStart  = Data & 0x0000ffff;
            pVidLutCurDac->HalObject.Dac[buffNum].VerticalBlankHeight = (Data & 0xffff0000) >> 16;
            return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
        }
        //
        // Set Vertical sync
        //
        case NV046_SET_DAC_VERTICAL_SYNC(0):
        case NV046_SET_DAC_VERTICAL_SYNC(1):
        {
            buffNum = (Offset == NV046_SET_DAC_VERTICAL_SYNC(0)) ? 0 : 1;
            if (pVidLutCurDac->Dac[buffNum].NotifyTrigger)
                return NV046_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
                
            // Bits 15:0 contain the start and bits 31:16 contain the width in pixels.    
            // Start must be less than or equal to 2047.
            // Width must be less than or equal to 16. 
            if (((Data & 0x0000ffff) > 2047)       || // start greater than 2047
                (((Data & 0xffff0000) >> 16) > 16))   // width greater than 16
                return NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
                
            pVidLutCurDac->HalObject.Dac[buffNum].VerticalSyncStart  = Data & 0x0000ffff;
            pVidLutCurDac->HalObject.Dac[buffNum].VerticalSyncHeight = (Data & 0xffff0000) >> 16;
            return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
        }
            
        //
        // Set total size.
        //
        case NV046_SET_DAC_TOTAL_SIZE(0):
        case NV046_SET_DAC_TOTAL_SIZE(1):
        {
            buffNum = (Offset == NV046_SET_DAC_TOTAL_SIZE(0)) ? 0 : 1;
            if (pVidLutCurDac->Dac[buffNum].NotifyTrigger)
                return NV046_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
                
            // Bits 15:0 contain the width in pixels and bits 31:16 contain the height in lines.    
            // Width must be a multiple of 8, less than or equal to 4128 and greater than or
            // equal to 40
            // Height must be less than or equal to 2049 and greater than or equal to 2. 
            if (((Data & 0x3) != 0)                  || // width not a multiple of 8
                ((Data & 0x0000ffff) > 4128)         || // width greater than 4128
                ((Data & 0x0000ffff) < 40)           || // width less than 40
                (((Data & 0xffff0000) >> 16) > 2049) || // height greater than 2049
                (((Data & 0xffff0000) >> 16) < 2))      // height less than 2
                return NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
                
            pVidLutCurDac->HalObject.Dac[buffNum].TotalWidth  = Data & 0x0000ffff;
            pVidLutCurDac->HalObject.Dac[buffNum].TotalHeight = (Data & 0xffff0000) >> 16;
            return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
        }
            
        // Set pixel clock.
        case NV046_SET_DAC_PIXEL_CLOCK(0):
        case NV046_SET_DAC_PIXEL_CLOCK(1):
        {
            buffNum = (Offset == NV046_SET_DAC_PIXEL_CLOCK(0)) ? 0 : 1;
            if (pVidLutCurDac->Dac[buffNum].NotifyTrigger)
                return NV046_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
                
            pVidLutCurDac->HalObject.Dac[buffNum].PixelClock = Data;
            return NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
        }

        //
        // Set format
        //
        case NV046_SET_DAC_FORMAT(0):
        case NV046_SET_DAC_FORMAT(1):
        {
            RM_STATUS modeSetStatus;

            buffNum = (Offset == NV046_SET_DAC_FORMAT(0)) ? 0 : 1;
            if (pVidLutCurDac->Dac[buffNum].NotifyTrigger)
                return NV046_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            // Copy to a head-specific pointer.  In addition to allowing reference
            // to dac properties on a per-display basis, this pointer can be used
            // to determine if the head is enabled (non-NULL ptr) or disabled (NULL ptr)
            pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].pVidLutCurDac = (VOID_PTR) pVidLutCurDac;
            pDev->Dac.HalInfo.CrtcInfo[DACGETHEAD(pVidLutCurDac)].pVidLutCurDac = &(pVidLutCurDac->HalObject);

            // Bit 0 contains double scan mode setting.
            // Bit 1 contains interlaced mode setting.
            // Bit 2 contains horizontal sync polarity setting.
            // Bit 3 contains vertical sync polarity setting.
            pVidLutCurDac->HalObject.Dac[buffNum].Format.DoubleScanMode          = (Data & BIT(DEVICE_BASE(NV046_SET_DAC_FORMAT_DOUBLE_SCAN))) ? 1 : 0;
            pVidLutCurDac->HalObject.Dac[buffNum].Format.InterlacedMode          = (Data & BIT(DEVICE_BASE(NV046_SET_DAC_FORMAT_INTERLACE))) ? 1 : 0;
            pVidLutCurDac->HalObject.Dac[buffNum].Format.HorizontalSyncPolarity  = (Data & BIT(DEVICE_BASE(NV046_SET_DAC_FORMAT_HORIZONTAL_SYNC))) ? 1 : 0;
            pVidLutCurDac->HalObject.Dac[buffNum].Format.VerticalSyncPolarity    = (Data & BIT(DEVICE_BASE(NV046_SET_DAC_FORMAT_VERTICAL_SYNC))) ? 1 : 0;
            // color is specified by bits 19:16.
            pVidLutCurDac->HalObject.Dac[buffNum].ColorFormat =
                (Data >> DRF_SHIFT(NV046_SET_DAC_FORMAT_COLOR)) & DRF_MASK(NV046_SET_DAC_FORMAT_COLOR);

            pVidLutCurDac->Dac[buffNum].NotifyAction = (Data & BIT(DEVICE_BASE(NV046_SET_DAC_FORMAT_NOTIFY))) ? 1 : 0;
            
            pVidLutCurDac->Dac[buffNum].NotifyTrigger = TRUE;   

            // Save off the timing values in the pDev.
            pDev->Dac.HorizontalVisible       = pVidLutCurDac->HalObject.Dac[buffNum].VisibleImageWidth;
            pDev->Dac.HorizontalBlankStart    = pVidLutCurDac->HalObject.Dac[buffNum].HorizontalBlankStart;
            pDev->Dac.HorizontalRetraceStart  = pVidLutCurDac->HalObject.Dac[buffNum].HorizontalSyncStart;
            pDev->Dac.HorizontalRetraceEnd    = (pVidLutCurDac->HalObject.Dac[buffNum].HorizontalSyncStart +
                                                 pVidLutCurDac->HalObject.Dac[buffNum].HorizontalSyncWidth);
            pDev->Dac.HorizontalBlankEnd      = (pVidLutCurDac->HalObject.Dac[buffNum].HorizontalBlankStart +
                                                 pVidLutCurDac->HalObject.Dac[buffNum].HorizontalBlankWidth);
            pDev->Dac.HorizontalTotal         = pVidLutCurDac->HalObject.Dac[buffNum].TotalWidth;
            pDev->Dac.VerticalVisible         = pVidLutCurDac->HalObject.Dac[buffNum].VisibleImageHeight;
            pDev->Dac.VerticalBlankStart      = pVidLutCurDac->HalObject.Dac[buffNum].VerticalBlankStart;
            pDev->Dac.VerticalRetraceStart    = pVidLutCurDac->HalObject.Dac[buffNum].VerticalSyncStart;
            pDev->Dac.VerticalRetraceEnd      = (pVidLutCurDac->HalObject.Dac[buffNum].VerticalSyncStart +
                                                 pVidLutCurDac->HalObject.Dac[buffNum].VerticalSyncHeight);
            pDev->Dac.VerticalBlankEnd        = (pVidLutCurDac->HalObject.Dac[buffNum].VerticalBlankStart +
                                                 pVidLutCurDac->HalObject.Dac[buffNum].VerticalBlankHeight);
            pDev->Dac.VerticalTotal           = pVidLutCurDac->HalObject.Dac[buffNum].TotalHeight;
            pDev->Dac.HalInfo.PixelClock      = pVidLutCurDac->HalObject.Dac[buffNum].PixelClock;
            pDev->Dac.HorizontalSyncPolarity  = pVidLutCurDac->HalObject.Dac[buffNum].Format.HorizontalSyncPolarity;
            pDev->Dac.VerticalSyncPolarity    = pVidLutCurDac->HalObject.Dac[buffNum].Format.VerticalSyncPolarity;
            pDev->Dac.DoubleScannedMode       = pVidLutCurDac->HalObject.Dac[buffNum].Format.DoubleScanMode;

            pDev->Framebuffer.HalInfo.HorizDisplayWidth = pDev->Dac.HorizontalVisible;
            pDev->Framebuffer.HalInfo.VertDisplayWidth = pDev->Dac.VerticalVisible;
			switch(pVidLutCurDac->HalObject.Dac[buffNum].ColorFormat)
            {
				case NV046_SET_DAC_FORMAT_COLOR_LUT_LE_Y8:
					pDev->Dac.HalInfo.Depth = 8;
					// pDev->Dac.DisplayPitch =  pDev->Framebuffer.HalInfo.HorizDisplayWidth * 1;
					break;
				case NV046_SET_DAC_FORMAT_COLOR_LE_BYPASS1R5G5B5:
					pDev->Dac.HalInfo.Depth = 15;
					// pDev->Dac.DisplayPitch =  pDev->Framebuffer.HalInfo.HorizDisplayWidth * 2;
					break;
				case NV046_SET_DAC_FORMAT_COLOR_BYPASS_LE_R5G6B5:
					pDev->Dac.HalInfo.Depth = 16;
					// pDev->Dac.DisplayPitch =  pDev->Framebuffer.HalInfo.HorizDisplayWidth * 2;
					break;
				case NV046_SET_DAC_FORMAT_COLOR_LE_BYPASS1X7R8G8B8:
					pDev->Dac.HalInfo.Depth = 32;
					// pDev->Dac.DisplayPitch =  pDev->Framebuffer.HalInfo.HorizDisplayWidth * 4;
					break;
			}

            pDev->Dac.PixelDepth              = pDev->Dac.HalInfo.Depth;
            pVidLutCurDac->HalObject.Dac[buffNum].PixelDepth     = pDev->Dac.HalInfo.Depth;
            pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].PrimaryDevice = 
                DRF_VAL(046, _SET_DAC_FORMAT, _PRIMARY_DEVICE, Data);

            // Get output device...
            pVidLutCurDac->DisplayType		  = (Data >> DRF_SHIFT(NV046_SET_DAC_FORMAT_DISPLAY_TYPE)) & DRF_MASK(NV046_SET_DAC_FORMAT_DISPLAY_TYPE);
            if (pVidLutCurDac->DisplayType > 3)
            {
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: class046: invalid display type ", pVidLutCurDac->DisplayType);
                return NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            }

            // Get TV output format...
            pVidLutCurDac->TVStandard		  = (Data >> DRF_SHIFT(NV046_SET_DAC_FORMAT_TV_STANDARD)) & DRF_MASK(NV046_SET_DAC_FORMAT_TV_STANDARD);
            if (pVidLutCurDac->TVStandard > 5)
            {
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: class046: invalid TV standard ", pVidLutCurDac->TVStandard);
                return NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            }
            
            // Do the mode set.
            modeSetStatus = stateSetModeMultiHead(pDev, pVidLutCurDac);

            if (pVidLutCurDac->CBase.NotifyXlate) 
            {
                status = notifyFillNotifierArray(pDev, pVidLutCurDac->CBase.NotifyXlate, 0, 0, 
                                                 modeSetStatus,
                                                 NV046_NOTIFIERS_SET_DAC(buffNum));
                //
                // Do any OS specified action related to this notification.
                //
                if (pVidLutCurDac->Dac[buffNum].NotifyAction)
                {
                    osNotifyEvent(pDev, (POBJECT)pVidLutCurDac, 
                                  NV046_NOTIFIERS_SET_DAC(buffNum),
                                  0 /* Method */, 
                                  0 /* Data */, 
                                  modeSetStatus, 
                                  pVidLutCurDac->Dac[buffNum].NotifyAction);
                }
            }
            pVidLutCurDac->Dac[buffNum].NotifyTrigger = FALSE;   

            return modeSetStatus;
        }
        
        default:
            return NV046_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            break;
     }
} // end of class046SetDACValues
