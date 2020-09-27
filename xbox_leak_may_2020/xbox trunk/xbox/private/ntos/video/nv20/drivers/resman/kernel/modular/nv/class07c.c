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
* Module: class07C.C                                                        *
*   This module implements the NV15_VIDEO_LUT_CURSOR_DAC object class and   *
*   its corresponding methods.                                              *
*                                                                           *
*   For now, support for NV20_VIDEO_LUT_CURSOR_DAC is in here as well.      *
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

#define IS_2D_BASED_FLIP(pDev, Object)    \
    ((pDev)->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].pVidLutCurDac == (VOID_PTR)(Object))

static VOID class07CSetScreenDeltaXY(PHWINFO, PVIDEO_LUT_CURSOR_DAC_OBJECT, U032);

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

RM_STATUS class07CSetNotifyCtxDma
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

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07CSetNotifyCtxDma\r\n");
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
        return NV07C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

    //
    // Connect to dma buffer.
    //
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &(pVidLutCurDac->CBase.NotifyXlate));
    if (status)
        return (NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);

    //
    // Validate notification parameters.
    //
    status = dmaValidateXlate(pVidLutCurDac->CBase.NotifyXlate,
                              0,
                              (9 * 0x0010));
    if (status)
    {
        pVidLutCurDac->CBase.NotifyXlate = NULL;
        return (NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    }

    return (status);
} // end of class07CSetNotifyCtxDma

RM_STATUS class07CSetNotify
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07CSetNotify\r\n");
    //
    // Set the notification style (note that these will be s/w notifies!)
    //
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;

    pVidLutCurDac->CBase.NotifyAction  = Data;
    pVidLutCurDac->CBase.NotifyTrigger = TRUE;

    return (NV07C_NOTIFICATION_STATUS_DONE_SUCCESS);
} // end of class07CSetNotify

RM_STATUS class07CStopImage
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT) Object;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07CStopImage\r\n");

    dacDisableImage(pDev, DACGETHEAD(pVidLutCurDac));

    return (NV07C_NOTIFICATION_STATUS_DONE_SUCCESS);
} // end of class07CStopImage

RM_STATUS class07CStopLUT
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07CStopLUT\r\n");
    return (NV07C_NOTIFICATION_STATUS_DONE_SUCCESS);
} // end of class07CStopLUT

RM_STATUS class07CStopCursorImage
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT) Object;
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07CStopCursorImage\r\n");

    //
    // Disable the current cursor.  Note this will currently not preempt any
    // pending notifies via our vblank callback mechanism.  Should probably
    // add that at some point.
    //
    dacDisableCursor(pDev, DACGETHEAD(pVidLutCurDac));

    return (NV07C_NOTIFICATION_STATUS_DONE_SUCCESS);
} // end of class07CStopCursorImage

RM_STATUS class07CStopDAC
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT) Object;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07CStopDAC\r\n");

    // only allow this method if this object was the last one to set a mode on this head
    if (pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].pVidLutCurDac == (VOID_PTR)pVidLutCurDac)
    {
        dacDisableDac(pDev, DACGETHEAD(pVidLutCurDac));

        // reset display type
        SETDISPLAYTYPE(pDev, DACGETHEAD(pVidLutCurDac), 0xffffffff);
    }
    
    return (NV07C_NOTIFICATION_STATUS_DONE_SUCCESS);
    
} // end of class07CStopDAC

RM_STATUS class07CSetImageCtxDma
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

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07CSetImageCtxDma\r\n");
    //
    // Set the Image DMA context.
    //
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;
    buffNum = (Offset == NV07C_SET_CONTEXT_DMA_IMAGE(0)) ? 0 : 1;

    if (pVidLutCurDac->Image[buffNum].NotifyTrigger)
        return NV07C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

    if (Data == NV01_NULL_OBJECT)
    {
        pVidLutCurDac->Image[buffNum].ImageCtxDma = NULL;
        return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
    }
    //
    // Connect to dma buffer.
    //
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &(pVidLutCurDac->Image[buffNum].ImageCtxDma));
    if (status)
        return (NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);

    //
    // Validate the dma context.
    // The image context dma must point to the frame buffer.
    if (pVidLutCurDac->Image[buffNum].ImageCtxDma->HalInfo.AddressSpace != ADDR_FBMEM) 
    {
        pVidLutCurDac->Image[buffNum].ImageCtxDma = NULL;
        return (NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    }

    return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
} // end of class07CSetImageCtxDma

RM_STATUS class07CSetLUTCtxDma
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

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07CSetLUTCtxDma\r\n");
    //
    // Set the Image DMA context.
    //
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;
    buffNum = (Offset == NV07C_SET_CONTEXT_DMA_LUT(0)) ? 0 : 1;

    if (pVidLutCurDac->LUT[buffNum].NotifyTrigger)
        return NV07C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

    if (Data == NV01_NULL_OBJECT)
    {
        pVidLutCurDac->LUT[buffNum].LUTCtxDma = NULL;
        return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
    }
    //
    // Connect to dma buffer.
    //
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &(pVidLutCurDac->LUT[buffNum].LUTCtxDma));
    if (status)
        return (NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);

    //
    // Validate the dma context.
    //
    status = dmaValidateXlate(pVidLutCurDac->LUT[buffNum].LUTCtxDma,
                              0,
                              0x400);
    if (status)
    {
        pVidLutCurDac->LUT[buffNum].LUTCtxDma = NULL;
        return (NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    }

    return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
} // end of  class07CSetLUTCtxDma

RM_STATUS class07CSetCursorCtxDma
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

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07CSetCursorCtxDma\r\n");
    //
    // Set the Cursor DMA context.
    //
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;
    buffNum = (Offset == NV07C_SET_CONTEXT_DMA_CURSOR(0)) ? 0 : 1;

    if (pVidLutCurDac->CursorImage[buffNum].NotifyTrigger)
        return NV07C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

    if (Data == NV01_NULL_OBJECT)
    {
        pVidLutCurDac->CursorImage[buffNum].CursorCtxDma = NULL;
        return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
    }
    //
    // Connect to dma buffer.
    //
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &(pVidLutCurDac->CursorImage[buffNum].CursorCtxDma));
    if (status)
        return (NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);

    //
    // Validate the dma context.
    //
    status = dmaValidateXlate(pVidLutCurDac->CursorImage[buffNum].CursorCtxDma,
                              0,
                              0x800);
    if (status)
    {
        pVidLutCurDac->CursorImage[buffNum].CursorCtxDma = NULL;
        return (NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    }
    // The cursor image context dma must point to the frame buffer.
    if (pVidLutCurDac->CursorImage[buffNum].CursorCtxDma->HalInfo.AddressSpace != ADDR_FBMEM) 
    {
        pVidLutCurDac->CursorImage[buffNum].CursorCtxDma = NULL;
        return (NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    }

    return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;

} // end of class07CSetCursorCtxDma


RM_STATUS class07CSetPanOffset
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

    // only 2D panning flips should occur here
    RM_ASSERT(IS_2D_BASED_FLIP(pDev, Object));

    // determine screen delta x,y from the previous/new pan offset
    class07CSetScreenDeltaXY(pDev, pVidLutCurDac, Data);

    // For this head, set the current pan offset
    pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].CurrentPanOffset = Data;

    // 
    // Set Image[].Offset (like the NV07C_SET_IMAGE_OFFSET method) to this
    // heads CurrentImageOffset. Note, we're assuming the display driver is
    // doing its flips through buffNum0 (similar to the mode set).
    //
    pVidLutCurDac->Image[0x0].Offset =
        pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].CurrentImageOffset;

    return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
} // end of class07CSetPanOffset

RM_STATUS class07CSetSemaphoreCtxDma
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

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07CSetSemaphoreCtxDma\r\n");

    //
    // Set the semaphore DMA context.
    //
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;

    if (pVidLutCurDac->Semaphore.ReleaseTrigger)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: SetSemaCtxdma: trigger active!\n");
        DBG_BREAKPOINT();
        return NV07C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
    }

    if (Data == NV01_NULL_OBJECT)
    {
        pVidLutCurDac->Semaphore.SemaphoreCtxDma = NULL;
        return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
    }
    //
    // Connect to dma buffer.
    //
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &(pVidLutCurDac->Semaphore.SemaphoreCtxDma));
    if (status)
        return (NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);

    //
    // There are some rules on alignment of both base/offset.
    // The hardware expects a single page of memory.
    //

    //
    // Validate the dma context.
    // Needs to be one page big at least.
    //
    status = dmaValidateXlate(pVidLutCurDac->Semaphore.SemaphoreCtxDma,
                              0,
                              0x1000);
    if (status)
    {
        pVidLutCurDac->Semaphore.SemaphoreCtxDma = NULL;
        return (NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    }

    return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
} // end of class07CSetSemaphoreCtxDma

RM_STATUS class07CSetSemaphoreOffset
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    RM_STATUS status = RM_OK;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: class07CSetSemaphoreOffset: ", Data);

    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;


    if (pVidLutCurDac->Semaphore.ReleaseTrigger)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: SetSemaOffset: trigger active!\n");
        DBG_BREAKPOINT();
        return NV07C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;
    }

    //
    // Better have the semaphore ctxdma plugged in.
    //
    if (pVidLutCurDac->Semaphore.SemaphoreCtxDma == NULL)
        return (NV07C_NOTIFICATION_STATUS_ERROR_INVALID_STATE);

    //
    // Alignment check...for the host, 4byte aligned is OK.
    //
    // Note that for nv20 graphics (kelvin) release methods, the
    // alignment must be 16byte to workaround a hw bug.
    //
    if (Data & 0x3)
        return(NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);

    pVidLutCurDac->Semaphore.Offset = Data;

    return status;

} // end of class07CSetSemaphoreOffset

RM_STATUS class07CSetSemaphoreRelease
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
    U032 *ReleaseAddr;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: class07CSetSemaphoreRelease: ", Data);

    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;

    //
    // Better have the semaphore ctxdma plugged in.
    //
    if (pVidLutCurDac->Semaphore.SemaphoreCtxDma == NULL)
        return (NV07C_NOTIFICATION_STATUS_ERROR_INVALID_STATE);

    if (pVidLutCurDac->Semaphore.ReleaseTrigger == TRUE)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: semaphore trigger active!\n");
        DBG_BREAKPOINT();
        return (NV07C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE);
    }

    status = dmaGetMappedAddress(pDev, pVidLutCurDac->Semaphore.SemaphoreCtxDma, pVidLutCurDac->Semaphore.Offset, 0x0003, (VOID **)&(ReleaseAddr));
    if (status)
        return (status);

    //
    // The actual release will occur after the next flip...
    //
    pVidLutCurDac->Semaphore.ReleaseAddr = ReleaseAddr;
    pVidLutCurDac->Semaphore.ReleaseValue = Data;
    pVidLutCurDac->Semaphore.ReleaseTrigger = TRUE;

    return status;

} // end of class07CSetSemaphoreOffset

RM_STATUS class07CSetOffsetRange
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
    U032                         start, stop, polarity;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07CSetOffsetRange\r\n");

    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;

    pVidLutCurDac->Polarity   = Data & 0x80000000;  // top bit is polarity (in/out of range)
    pVidLutCurDac->ScanRegion = Data & 0x7FFFFFFF;  // remaining bits are start/stop offsets

    // Extract the data (polarity indicates if the stall occurs inside or outside the start/stop range)
    start    = (Data >> DRF_SHIFT(NV07C_SET_OFFSET_RANGE_START)) & DRF_MASK(NV07C_SET_OFFSET_RANGE_START);
    stop     = (Data >> DRF_SHIFT(NV07C_SET_OFFSET_RANGE_STOP)) & DRF_MASK(NV07C_SET_OFFSET_RANGE_STOP);
    polarity = (Data >> DRF_SHIFT(NV07C_SET_OFFSET_RANGE_POLARITY)) & DRF_MASK(NV07C_SET_OFFSET_RANGE_POLARITY);

    // Program the hardware
    status = dacProgramScanRegion(pDev, DACGETHEAD(pVidLutCurDac), start, stop, polarity);

    if (status)
        return(NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT);
    else
        return (NV07C_NOTIFICATION_STATUS_DONE_SUCCESS);

} // end of class07CSetOffsetRange


RM_STATUS class07CGetOffset
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

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07CGetOffset\r\n");

    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;
    if (!pVidLutCurDac->CBase.NotifyTrigger)
        // nothing to do.
        return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;

    // The caller is expecting a notification.
    // Write the image offset in the INFO32 of the notification.
    if (pVidLutCurDac->CBase.NotifyXlate)
    {
        status = notifyFillNotifierArray(pDev, pVidLutCurDac->CBase.NotifyXlate,
                                         pVidLutCurDac->Image[Data].Offset,
                                         NV07C_NOTIFICATION_INFO16_VALID_OFFSET,
                                         NV07C_NOTIFICATION_STATUS_DONE_SUCCESS,
                                         NV07C_NOTIFIERS_NOTIFY);
        //
        // Do any OS specified action related to this notification.
        //
        if (pVidLutCurDac->CBase.NotifyAction)
        {
            osNotifyEvent(pDev, (POBJECT)pVidLutCurDac,
                          NV07C_NOTIFIERS_NOTIFY,
                          0 /* Method */,
                          0 /* Data */,
                          status,
                          pVidLutCurDac->CBase.NotifyAction);
        }
    }
    pVidLutCurDac->CBase.NotifyTrigger = FALSE;
    return (NV07C_NOTIFICATION_STATUS_DONE_SUCCESS);
} // end of class07CGetOffset

#ifdef DEBUG
//
// Print out the current read/write values and ASSERT that incrementing
// the read pointer will not put us into the stall condition
//
static U032 debug_hwflip = 0;

// these PGRAPH_SURFACE fields are common to nv15/nv11/nv20
#define NV_PGRAPH_SURFACE                                0x00400710 /* RW-4R */
#define NV_PGRAPH_SURFACE_WRITE_3D                            22:20 /* RWIVF */
#define NV_PGRAPH_SURFACE_READ_3D                             26:24 /* RWIVF */
#define NV_PGRAPH_SURFACE_MODULO_3D                           30:28 /* RWIVF */

#define DBG_PRINT_READ_WRITE_3D_STATE()                                            \
    if (debug_hwflip) {                                                            \
        U032 m,r,w;                                                                \
        U032 surface = REG_RD32(NV_PGRAPH_SURFACE);                                \
                                                                                   \
        m = DRF_VAL(_PGRAPH, _SURFACE, _MODULO_3D, surface);                       \
        r = DRF_VAL(_PGRAPH, _SURFACE, _READ_3D,   surface);                       \
        w = DRF_VAL(_PGRAPH, _SURFACE, _WRITE_3D,  surface);                       \
                                                                                   \
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_WARNINGS, "NVRM: hwflip write = ", w);   \
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_WARNINGS, "NVRM:         read = ", r);   \
        RM_ASSERT(((r + 1) % m) != w);                                             \
    }
#define DBG_ASSERT_START_PROPERLY_LATCHED()                                            \
    if (debug_hwflip) {                                                                \
        U032 raster = REG_RD32(NV_PCRTC_RASTER);                                       \
        switch (raster & 0x3000)                                                       \
        {                                                                              \
            case 0x0000: DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"NVRM: latch 'display'\n");\
                         break;                                                        \
            case 0x1000: DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"NVRM: latch 'before'\n"); \
                         break;                                                        \
            case 0x2000: DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"NVRM: latch 'after'\n");  \
                         break;                                                        \
        }                                                                              \
    }
#else
#define DBG_PRINT_READ_WRITE_3D_STATE()
#define DBG_ASSERT_START_PROPERLY_LATCHED()
#endif

// This proc writes the notification for an image buffer.
RM_STATUS class07CImageNotify
(
    PHWINFO   pDev,
    POBJECT   Object,
    U032      param1,
    V032      BuffNum,
    RM_STATUS Status
)
{
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    U032 startAddr;

    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;

    // assert we're on the expected vblank
    RM_ASSERT(pVidLutCurDac->Image[BuffNum].ImageCallback.VBlankCount ==
              pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].VBlankCounter);

    // Program the new start address
    startAddr = (U032)((size_t)(pVidLutCurDac->Image[BuffNum].ImageCtxDma->DescAddr) +
                pVidLutCurDac->Image[BuffNum].Offset);
    startAddr += pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].CurrentPanOffset;

    if (dacProgramVideoStart(pDev, DACGETHEAD(pVidLutCurDac), startAddr, pVidLutCurDac->Image[BuffNum].Pitch))
    {
        pVidLutCurDac->Image[BuffNum].NotifyTrigger = FALSE;
        return NV07C_NOTIFICATION_STATUS_ERROR_INVALID_STATE;
    }

    // increment the flip count and send it back through the notifier
    pVidLutCurDac->CompletedFlipCount++;

    // Send all notifications
    if (pVidLutCurDac->CBase.NotifyXlate)
    {
        notifyFillNotifierArray(pDev, pVidLutCurDac->CBase.NotifyXlate,
                                pVidLutCurDac->CompletedFlipCount, // Info32
                                0,
                                Status,
                                NV07C_NOTIFIERS_SET_IMAGE(BuffNum));
        //
        // Do any OS specified action related to this notification.
        //
        if (pVidLutCurDac->Image[BuffNum].NotifyAction)
        {
            osNotifyEvent(pDev, (POBJECT)pVidLutCurDac,
                          NV07C_NOTIFIERS_SET_IMAGE(BuffNum),
                          0 /* Method */,
                          0 /* Data */,
                          Status,
                          pVidLutCurDac->Image[BuffNum].NotifyAction);
        }
    }

    // Check if this is a 2D or 3D flip
    if (IS_2D_BASED_FLIP(pDev, Object)) {
        //
        // For 2D panning flips, on a head with an active video overlay, update
        // the window start since DDraw won't see this as windows move event.
        //
        if (pDev->Video.HalInfo.ActiveVideoOverlayObject &&
            (pDev->Video.HalInfo.Head == DACGETHEAD(pVidLutCurDac))) {

            videoUpdateWindowStart(pDev,
                                   pVidLutCurDac->Image[BuffNum].DeltaX,
                                   pVidLutCurDac->Image[BuffNum].DeltaY);
        }
    } else {
        //
        // For 3D flips, if this is a synchronized multihead flip, only
        // the last head that completes their flip will increment READ_3D.
        //
        if (pVidLutCurDac->Image[BuffNum].MultiSync) {
            pDev->Dac.MultiSyncCounter--;
            if (pDev->Dac.MultiSyncCounter == 0) {
                DBG_PRINT_READ_WRITE_3D_STATE();
                FLD_WR_DRF_DEF(_PGRAPH, _INCREMENT, _READ_3D, _TRIGGER);
                pDev->Dac.MultiSyncCounter = pDev->Dac.HalInfo.NumCrtcs;
            }
        } else {
            DBG_PRINT_READ_WRITE_3D_STATE();
            FLD_WR_DRF_DEF(_PGRAPH, _INCREMENT, _READ_3D, _TRIGGER);
        }
    }

    DBG_ASSERT_START_PROPERLY_LATCHED();

    //
    // Handle semaphore release.
    //
    if (pVidLutCurDac->Semaphore.ReleaseTrigger == TRUE)
    {
        *pVidLutCurDac->Semaphore.ReleaseAddr = pVidLutCurDac->Semaphore.ReleaseValue;
        pVidLutCurDac->Semaphore.ReleaseTrigger = FALSE;
    }

    pVidLutCurDac->Image[BuffNum].NotifyTrigger = FALSE;

    return (NV07C_NOTIFICATION_STATUS_DONE_SUCCESS);
}

static
VOID class07CSetScreenDeltaXY
(
    PHWINFO pDev,
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac,
    U032 newPanOffset
)
{
    S032 pitch, bpp;
    S016 deltaX, deltaY;
    S016 oldDeltaX, oldDeltaY;
    S016 newDeltaX, newDeltaY;

    // set the pitch and bytes per pixel
    pitch = pVidLutCurDac->Image[0].Pitch;
    bpp   = (pVidLutCurDac->HalObject.Dac[0].PixelDepth + 1) >> 3; // bytes per pixel

    // if either is uninitialized, return avoiding a divide by 0
    if (!bpp || !pitch)
    {
        pVidLutCurDac->Image[0].DeltaX = 0;
        pVidLutCurDac->Image[0].DeltaY = 0;
        return;
    }

    // get x,y info from new pan offset
    newDeltaY = (S016)(newPanOffset / pitch);
    newDeltaX = (S016)(((newPanOffset) % pitch) / bpp);

    // get x,y info from current pan offset
    oldDeltaY = (S016)(pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].CurrentPanOffset / pitch);
    oldDeltaX = (S016)(((pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].CurrentPanOffset) % pitch) / bpp);

    deltaY = (S016)(newDeltaY - oldDeltaY);
    deltaX = (S016)(newDeltaX - oldDeltaX);

    pVidLutCurDac->Image[0].DeltaX = deltaX;
    pVidLutCurDac->Image[0].DeltaY = deltaY;

    //
    // Keep a running tab of pan offset deltas for overlay.
    //
    pDev->Video.HalInfo.PanOffsetDeltaX[DACGETHEAD(pVidLutCurDac)] += deltaX;
    pDev->Video.HalInfo.PanOffsetDeltaY[DACGETHEAD(pVidLutCurDac)] += deltaY;
}

RM_STATUS class07CSetImageValues
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

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07CSetImageValues\r\n");

    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;
    //
    // Initialize the image buffer description.
    //
    switch (Offset)
    {
        //
        // Set Image Offset
        //
        case NV07C_SET_IMAGE_OFFSET(0):
        case NV07C_SET_IMAGE_OFFSET(1):
        {
            buffNum = (Offset == NV07C_SET_IMAGE_OFFSET(0)) ? 0 : 1;
            if (pVidLutCurDac->Image[buffNum].NotifyTrigger)
                return NV07C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            if ((Data & 0x03) != 0)
                // The image offset must be a multiple of 4 bytes.
                return NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;

            // Save the CurrentImageOffset for this head
            pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].CurrentImageOffset = Data;
            pVidLutCurDac->Image[buffNum].Offset = Data;

            return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
        }
        //
        // Set Image Format
        //
        case NV07C_SET_IMAGE_FORMAT(0):
        case NV07C_SET_IMAGE_FORMAT(1):
        {
            U016  pitch;
            U032  when, multisync, flags;

            buffNum = (Offset == NV07C_SET_IMAGE_FORMAT(0)) ? 0 : 1;
            if (pVidLutCurDac->Image[buffNum].NotifyTrigger)
                return NV07C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            if (pVidLutCurDac->Image[buffNum].ImageCtxDma == NULL)
                return NV07C_NOTIFICATION_STATUS_ERROR_INVALID_STATE;

            // pitch is specified by bits 15:0.
            pitch = (U016) (Data & 0x0000ffff);
            if ((pitch & 0x3F) != 0)
                // the pitch must be a multiple of 64 bytes.
                return NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;

            pVidLutCurDac->Image[buffNum].Pitch = pitch;
            pDev->Dac.DisplayPitch = pitch;

            when = DRF_VAL(07C, _SET_IMAGE_FORMAT, _WHEN, Data);
            multisync = DRF_VAL(07C, _SET_IMAGE_FORMAT, _MULTIHEAD_SYNC, Data);
            flags = DRF_VAL(07C, _SET_IMAGE_FORMAT, _FLAGS, Data);

            pVidLutCurDac->Image[buffNum].NotifyAction = (Data & BIT(DEVICE_BASE(NV07C_SET_IMAGE_FORMAT_NOTIFY))) ? 1 : 0;
            pVidLutCurDac->Image[buffNum].NotifyTrigger = TRUE;
            pVidLutCurDac->Image[buffNum].MultiSync = multisync;

            //
            // Always latch the startAddr at hsync, just control when it's written
            // (either now, or during vsync). XXX shouldn't be done on every flip,
            // but once somewhere during dacCreateObj or nvHalDacControl.
            //
            DAC_FLD_WR_DRF_DEF(_PCRTC, _CONFIG, _START_ADDRESS, _HSYNC_NV10, DACGETHEAD(pVidLutCurDac));

            //
            // Programming the new PCRTC_START occurs during class07CImageNotify,
            // which is called immediately in the case of a flip on hsync or as
            // part of a vblank callback on vsync flips.
            //
            if (when == NV07C_SET_IMAGE_FORMAT_WHEN_IMMEDIATELY) {

                // This hsync flip happens on the current VBlankCounter
                pVidLutCurDac->Image[buffNum].ImageCallback.VBlankCount =
                    pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].VBlankCounter;

                if (pDev->pStereoParams && pDev->pStereoParams->Flags & STEREOCFG_STEREOACTIVATED)
                {
                    //Shouldn't ever happen because we always force waiting for VSync for stereo.
                    RM_ASSERT(FALSE);
                } else
                    // Do the flip/notify right here.
                    class07CImageNotify(pDev, (POBJECT)pVidLutCurDac,
                                        0 /* param 1*/, buffNum,
                                        NV07C_NOTIFICATION_STATUS_DONE_SUCCESS);
            } else {
                U032  startAddr;

                if (pDev->pStereoParams)
                {
                    // DescAddr is really just an offset in this case.

                    startAddr = (U032)((NV_UINTPTR_T)(pVidLutCurDac->Image[buffNum].ImageCtxDma->DescAddr)) +
                                pVidLutCurDac->Image[buffNum].Offset;
                    pDev->pStereoParams->FlipOffsets[3][0] = startAddr;
                }

                pVidLutCurDac->Image[buffNum].ImageCallback.Proc   = class07CImageNotify;
                pVidLutCurDac->Image[buffNum].ImageCallback.Object = (POBJECT) pVidLutCurDac;
                pVidLutCurDac->Image[buffNum].ImageCallback.Next   = NULL;
                pVidLutCurDac->Image[buffNum].ImageCallback.Param1 = 0;
                pVidLutCurDac->Image[buffNum].ImageCallback.Param2 = buffNum;
                pVidLutCurDac->Image[buffNum].ImageCallback.Status = NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
                pVidLutCurDac->Image[buffNum].ImageCallback.Flags = 0;

                if (flags & NV07C_SET_IMAGE_FORMAT_FLAGS_COMPLETE_ON_OBJECT_CLEANUP)
                    pVidLutCurDac->Image[buffNum].ImageCallback.Flags |= CALLBACK_FLAG_COMPLETE_ON_OBJECT_CLEANUP;

                // Check if the other buffer is already pending ...
                if (pVidLutCurDac->Image[buffNum ^ 1].NotifyTrigger == FALSE) {
                    // No, program/notify on the next vblank
                    pVidLutCurDac->Image[buffNum].ImageCallback.VBlankCount = pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].VBlankCounter + 1;
                } else {
                    // Yes, program/notify on the next + 1 vblank
                    pVidLutCurDac->Image[buffNum].ImageCallback.VBlankCount = pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].VBlankCounter + 2;
                    pVidLutCurDac->Image[buffNum].ImageCallback.Flags |= CALLBACK_FLAG_SPECIFIED_VBLANK_COUNT;
                }

                VBlankAddCallback(pDev, DACGETHEAD(pVidLutCurDac), &(pVidLutCurDac->Image[buffNum].ImageCallback));
            }
            return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
            break;
        }

        default:
            return NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            break;
     }

    return RM_OK;

} // end of class07CSetImageValues


// This proc is called from VBlank to program the LUT.
RM_STATUS class07CProgramLUT
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
    U032     *startAddr;
    U032      numEntries;

    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;

    // Program the LUT.
    numEntries = 256;
    myStatus = dmaGetMappedAddress(pDev, pVidLutCurDac->LUT[BuffNum].LUTCtxDma, pVidLutCurDac->LUT[BuffNum].Offset,
                                   numEntries * 4, (void **)(&startAddr));
    if (!myStatus)
    {
        myStatus = dacProgramLUT(pDev, DACGETHEAD(pVidLutCurDac), startAddr, numEntries);
    }


    // Notify.
    if (pVidLutCurDac->CBase.NotifyXlate)
    {
        notifyFillNotifierArray(pDev, pVidLutCurDac->CBase.NotifyXlate, 0, 0,
                                myStatus,
                                NV07C_NOTIFIERS_SET_LUT(BuffNum));
        //
        // Do any OS specified action related to this notification.
        //
        if (pVidLutCurDac->LUT[BuffNum].NotifyAction)
        {
            osNotifyEvent(pDev, (POBJECT)pVidLutCurDac,
                          NV07C_NOTIFIERS_SET_LUT(BuffNum),
                          0 /* Method */,
                          0 /* Data */,
                          myStatus,
                          pVidLutCurDac->LUT[BuffNum].NotifyAction);
        }
    }
    pVidLutCurDac->LUT[BuffNum].NotifyTrigger = FALSE;
    return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
}

RM_STATUS class07CSetLUTValues
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

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07CSetLUTValues\r\n");

    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;
    //
    // Initialize the LUT buffer description.
    //
    switch (Offset)
    {
        //
        // Set LUT Offset
        //
        case NV07C_SET_LUT_OFFSET(0):
        case NV07C_SET_LUT_OFFSET(1):
        {
            buffNum = (Offset == NV07C_SET_LUT_OFFSET(0)) ? 0 : 1;
            if (pVidLutCurDac->LUT[buffNum].NotifyTrigger)
                return NV07C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            if ((Data & 0x3FF) != 0)
                // The LUT offset must be a multiple of 1024 bytes.
                return NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;

            pVidLutCurDac->LUT[buffNum].Offset = Data;
            return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
        }
        //
        // Set LUT Format
        //
        case NV07C_SET_LUT_FORMAT(0):
        case NV07C_SET_LUT_FORMAT(1):
        {
            buffNum = (Offset == NV07C_SET_LUT_FORMAT(0)) ? 0 : 1;
            if (pVidLutCurDac->LUT[buffNum].NotifyTrigger)
                return NV07C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            pVidLutCurDac->LUT[buffNum].NotifyAction = Data;
            pVidLutCurDac->LUT[buffNum].NotifyTrigger = TRUE;
            // We can only program the LUT during VBlank.
            // Setup a callback.
            pVidLutCurDac->LUT[buffNum].LUTCallback.Proc   = class07CProgramLUT;
            pVidLutCurDac->LUT[buffNum].LUTCallback.Object = (POBJECT) pVidLutCurDac;
            pVidLutCurDac->LUT[buffNum].LUTCallback.Next    = NULL;
            pVidLutCurDac->LUT[buffNum].LUTCallback.Param1  = 0;
            // Set param2 to the buffer number so we know which notifier to write.
            pVidLutCurDac->LUT[buffNum].LUTCallback.Param2  = buffNum;
            pVidLutCurDac->LUT[buffNum].LUTCallback.Status  = NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;

            VBlankAddCallback(pDev, DACGETHEAD(pVidLutCurDac), &(pVidLutCurDac->LUT[buffNum].LUTCallback));
            return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
            break;
        }

        default:
            return NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            break;
     }
} // end of class07CSetLUTValues

// This proc writes the notification for an Cursor buffer.
RM_STATUS class07CCursorImageNotify
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
                                NV07C_NOTIFIERS_SET_CURSOR_IMAGE(BuffNum));
        //
        // Do any OS specified action related to this notification.
        //
        if (pVidLutCurDac->CursorImage[BuffNum].NotifyAction)
        {
            osNotifyEvent(pDev, (POBJECT)pVidLutCurDac,
                          NV07C_NOTIFIERS_SET_CURSOR_IMAGE(BuffNum),
                          0 /* Method */,
                          0 /* Data */,
                          Status,
                          pVidLutCurDac->CursorImage[BuffNum].NotifyAction);
        }
    }
    pVidLutCurDac->CursorImage[BuffNum].NotifyTrigger = FALSE;
    return (NV07C_NOTIFICATION_STATUS_DONE_SUCCESS);
}

RM_STATUS class07CSetCursorImageValues
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

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07CSetCursorImageValues\r\n");

    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;
    //
    // Initialize the image buffer description.
    //
    switch (Offset)
    {
        //
        // Set Cursor Image Offset
        //
        case NV07C_SET_CURSOR_IMAGE_OFFSET(0):
        case NV07C_SET_CURSOR_IMAGE_OFFSET(1):
        {
            buffNum = (Offset == NV07C_SET_CURSOR_IMAGE_OFFSET(0)) ? 0 : 1;
            if (pVidLutCurDac->CursorImage[buffNum].NotifyTrigger)
                return NV07C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            if ((Data & 0x3ff) != 0)
                // The cursor image offset must be a multiple of 1024 bytes.
                return NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;

            pVidLutCurDac->CursorImage[buffNum].Offset = Data;
            return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
        }
        //
        // Set Cursor Image Format
        //
        case NV07C_SET_CURSOR_IMAGE_FORMAT(0):
        case NV07C_SET_CURSOR_IMAGE_FORMAT(1):
        {
            U032 startAddr;
            BOOL queue_callback = TRUE;

            // Unlike the other IMAGE_FORMAT routines, we do allow multiple calls
            // during a single frame time. But, we still have only one callback struct.
            // So allow the args to be updated and the HW to be reprogrammed for the
            // new cursor. Just don't requeue the callback struct, since this could
            // lose an existing callback already on the list.

            buffNum = (Offset == NV07C_SET_CURSOR_IMAGE_FORMAT(0)) ? 0 : 1;
            if (pVidLutCurDac->CursorImage[buffNum].NotifyTrigger)
                queue_callback = FALSE;

            pVidLutCurDac->CursorImage[buffNum].Width =
                (Data >> DRF_SHIFT(NV07C_SET_CURSOR_IMAGE_FORMAT_WIDTH)) & DRF_MASK(NV07C_SET_CURSOR_IMAGE_FORMAT_WIDTH);
            pVidLutCurDac->CursorImage[buffNum].Height =
                (Data >> DRF_SHIFT(NV07C_SET_CURSOR_IMAGE_FORMAT_HEIGHT)) & DRF_MASK(NV07C_SET_CURSOR_IMAGE_FORMAT_HEIGHT);
            pVidLutCurDac->CursorImage[buffNum].ColorFormat =
                (Data >> DRF_SHIFT(NV07C_SET_CURSOR_IMAGE_FORMAT_COLOR)) & DRF_MASK(NV07C_SET_CURSOR_IMAGE_FORMAT_COLOR);

            pVidLutCurDac->CursorImage[buffNum].NotifyAction =
                (Data >> DRF_SHIFT(NV07C_SET_CURSOR_IMAGE_FORMAT_NOTIFY)) & DRF_MASK(NV07C_SET_CURSOR_IMAGE_FORMAT_NOTIFY);

            // Kick off the transfer at this time.
            pVidLutCurDac->CursorImage[buffNum].NotifyTrigger = TRUE;

            // Program the hardware. It will take effect at the next VBlank.
            // The cursorImageCtxDma points to the frame buffer. DescAddr has the offset of this buffer
            // from the start of the frame buffer.
            startAddr = (U032)((size_t)(pVidLutCurDac->CursorImage[buffNum].CursorCtxDma->DescAddr) +
                               pVidLutCurDac->CursorImage[buffNum].Offset);
            
            status = nvHalDacProgramCursorImage(pDev, 
                                           startAddr, NV_CIO_CRE_HCUR_ASI_FRAMEBUFFER,
                                           pVidLutCurDac->CursorImage[buffNum].Width,
                                           pVidLutCurDac->CursorImage[buffNum].Height,
                                           pVidLutCurDac->CursorImage[buffNum].ColorFormat,
                                           (VOID *)&pVidLutCurDac->HalObject);
            if (status)
            {
                pVidLutCurDac->CursorImage[buffNum].NotifyTrigger = FALSE;
                return NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            }

            // Program was successful, so enable the cursor
            dacEnableCursor(pDev, DACGETHEAD(pVidLutCurDac));

            if (queue_callback)
            {
                // setup so we get a callback to do the notify at the next VBlank.
                pVidLutCurDac->CursorImage[buffNum].CursorImageCallback.Proc   = class07CCursorImageNotify;
                pVidLutCurDac->CursorImage[buffNum].CursorImageCallback.Object = (POBJECT) pVidLutCurDac;
                pVidLutCurDac->CursorImage[buffNum].CursorImageCallback.Next    = NULL;
                pVidLutCurDac->CursorImage[buffNum].CursorImageCallback.Param1  = 0;
                // Set param2 to the buffer number so we know which notifier to write.
                pVidLutCurDac->CursorImage[buffNum].CursorImageCallback.Param2  = buffNum;
                pVidLutCurDac->CursorImage[buffNum].CursorImageCallback.Status  = NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;

                VBlankAddCallback(pDev, DACGETHEAD(pVidLutCurDac), &(pVidLutCurDac->CursorImage[buffNum].CursorImageCallback));
            }
            return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
        }

        default:
            return NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
     }

} // end of class07CSetCursorImageValues

// This proc is called from VBlank to program the cursor position.
RM_STATUS class07CProgramCursorPoint
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

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07CProgramCursorPoint\r\n");

    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;

    pVidLutCurDac->CursorPoint = Data;
    status = dacProgramCursorPosition(pDev, DACGETHEAD(pVidLutCurDac),
                                      (Data & 0xffff), /* cursorX */
                                      (Data >> 16));   /* cursorY */
                                      
    // signal that the callback is done
    pVidLutCurDac->CursorPointCallback.Proc = NULL;
    
    return (NV07C_NOTIFICATION_STATUS_DONE_SUCCESS);

} // end of class07CProgramCursorPoint()

RM_STATUS class07CSetCursorPoint
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07CSetCursorPoint\r\n");
    
    // update the cursor position
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;
    pVidLutCurDac->CursorPointCallback.Param1      = Data;

    if (pVidLutCurDac->CursorPointCallback.Proc == NULL)
    {
        // add the cursor position callback
        pVidLutCurDac->CursorPointCallback.Proc    = class07CProgramCursorPoint;
        pVidLutCurDac->CursorPointCallback.Object  = (POBJECT)pVidLutCurDac;
        pVidLutCurDac->CursorPointCallback.Next    = NULL;
        pVidLutCurDac->CursorPointCallback.Param2  = 0;
        pVidLutCurDac->CursorPointCallback.Status  = NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
        VBlankAddCallback(pDev, DACGETHEAD(pVidLutCurDac), &(pVidLutCurDac->CursorPointCallback));
    }
    
    return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;

} // end of class07CSetCursorPoint

RM_STATUS class07CSetDACValues
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

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class07CSetDACValues\r\n");

    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;
    //
    // Initialize the image buffer description.
    //
    switch (Offset)
    {
        //
        // Set DAC image size
        //
        case NV07C_SET_DAC_IMAGE_SIZE(0):
        case NV07C_SET_DAC_IMAGE_SIZE(1):
        {
            buffNum = (Offset == NV07C_SET_DAC_IMAGE_SIZE(0)) ? 0 : 1;
            if (pVidLutCurDac->Dac[buffNum].NotifyTrigger)
                return NV07C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            // Bits 15:0 contain the width and bits 31:16 contain the height in pixels.
            // Width must be a multiple of 8.
            // Width and height must be less than or equal 2048.
            if (((Data & 0x3) != 0)                 || // width not a multiple of 8
                ((Data & 0x0000ffff) > 2048)        || // width greater than 2048
                (((Data & 0xffff0000) >> 16) > 2048))  // height greater than 2048
            {
                // Something is wrong, but the display driver won't do anything with the error, so use the closest acceptable value.
                pVidLutCurDac->HalObject.Dac[buffNum].VisibleImageWidth  = Data & 0x0000fffe;
                pVidLutCurDac->HalObject.Dac[buffNum].VisibleImageHeight = (Data & 0xfffe0000) >> 16;
                
                return NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            }

            pVidLutCurDac->HalObject.Dac[buffNum].VisibleImageWidth  = Data & 0x0000ffff;
            pVidLutCurDac->HalObject.Dac[buffNum].VisibleImageHeight = (Data & 0xffff0000) >> 16;
            return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
        }
        //
        // Set Horizontal Blank
        //
        case NV07C_SET_DAC_HORIZONTAL_BLANK(0):
        case NV07C_SET_DAC_HORIZONTAL_BLANK(1):
        {
            buffNum = (Offset == NV07C_SET_DAC_HORIZONTAL_BLANK(0)) ? 0 : 1;
            if (pVidLutCurDac->Dac[buffNum].NotifyTrigger)
                return NV07C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            // Bits 15:0 contain the start and bits 31:16 contain the width in pixels.
            // Start must be a multiple of 8 and less than or equal to 4088.
            // Width must be a multiple of 8 and less than or equal to 1024.
            if (((Data & 0x3) != 0)                        || // start not a multiple of 8
                ((Data & 0x0000ffff) > 4088)               || // start greater than 4088
                ((((Data & 0xffff0000) >> 16) & 0x3) != 0) || // width not a multiple of 8
                (((Data & 0xffff0000) >> 16) > 1024))          // width greater than 1024
            {
                // Something is wrong, but the display driver won't do anything with the error, so we use the closest acceptable value.
                pVidLutCurDac->HalObject.Dac[buffNum].HorizontalBlankStart = Data & 0x0000fffe;
                pVidLutCurDac->HalObject.Dac[buffNum].HorizontalBlankWidth = (Data & 0xfffe0000) >> 16;
                return NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            }

            pVidLutCurDac->HalObject.Dac[buffNum].HorizontalBlankStart = Data & 0x0000ffff;
            pVidLutCurDac->HalObject.Dac[buffNum].HorizontalBlankWidth = (Data & 0xffff0000) >> 16;
            return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
        }
        //
        // Set Horizontal sync
        //
        case NV07C_SET_DAC_HORIZONTAL_SYNC(0):
        case NV07C_SET_DAC_HORIZONTAL_SYNC(1):
        {
            buffNum = (Offset == NV07C_SET_DAC_HORIZONTAL_SYNC(0)) ? 0 : 1;
            if (pVidLutCurDac->Dac[buffNum].NotifyTrigger)
                return NV07C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            // Bits 15:0 contain the start and bits 31:16 contain the width in pixels.
            // Start must be a multiple of 8 and less than or equal to 4088.
            // Width must be a multiple of 8 and less than or equal to 256.
            if (((Data & 0x3) != 0)                        || // start not a multiple of 8
                ((Data & 0x0000ffff) > 4088)               || // start greater than 4088
                ((((Data & 0xffff0000) >> 16) & 0x3) != 0) || // width not a multiple of 8
                (((Data & 0xffff0000) >> 16) > 256))          // width greater than 256
            {
                // Something is wrong, but the display driver won't do anything with the error, so use the closest acceptable value.
                pVidLutCurDac->HalObject.Dac[buffNum].HorizontalSyncStart = Data & 0x0000fffe;
                pVidLutCurDac->HalObject.Dac[buffNum].HorizontalSyncWidth = (Data & 0xfffe0000) >> 16;
                return NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            }

            pVidLutCurDac->HalObject.Dac[buffNum].HorizontalSyncStart = Data & 0x0000ffff;
            pVidLutCurDac->HalObject.Dac[buffNum].HorizontalSyncWidth = (Data & 0xffff0000) >> 16;
            return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
        }

        //
        // Set Vertical Blank
        //
        case NV07C_SET_DAC_VERTICAL_BLANK(0):
        case NV07C_SET_DAC_VERTICAL_BLANK(1):
        {
            buffNum = (Offset == NV07C_SET_DAC_VERTICAL_BLANK(0)) ? 0 : 1;
            if (pVidLutCurDac->Dac[buffNum].NotifyTrigger)
                return NV07C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            // Bits 15:0 contain the start and bits 31:16 contain the width in pixels.
            // Start must be less than or equal to 2047.
            // Width must be less than or equal to 128.
            if (((Data & 0x0000ffff) > 2047)       || // start greater than 2047
                (((Data & 0xffff0000) >> 16) > 128))   // width greater than 128
            {
                // Something is wrong, but the display driver won't do anything with the error, so use the closest acceptable value.
                if ((Data & 0x0000ffff) > 2047)
                {
                    pVidLutCurDac->HalObject.Dac[buffNum].VerticalBlankStart  = 2047;
                }
                if (((Data & 0xffff0000) >> 16) > 128)
                {
                    pVidLutCurDac->HalObject.Dac[buffNum].VerticalBlankHeight = 128;
                }
                return NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            }

            pVidLutCurDac->HalObject.Dac[buffNum].VerticalBlankStart  = Data & 0x0000ffff;
            pVidLutCurDac->HalObject.Dac[buffNum].VerticalBlankHeight = (Data & 0xffff0000) >> 16;
            return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
        }
        //
        // Set Vertical sync
        //
        case NV07C_SET_DAC_VERTICAL_SYNC(0):
        case NV07C_SET_DAC_VERTICAL_SYNC(1):
        {
            buffNum = (Offset == NV07C_SET_DAC_VERTICAL_SYNC(0)) ? 0 : 1;
            if (pVidLutCurDac->Dac[buffNum].NotifyTrigger)
                return NV07C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            // Bits 15:0 contain the start and bits 31:16 contain the width in pixels.
            // Start must be less than or equal to 2047.
            // Width must be less than or equal to 16.
            if (((Data & 0x0000ffff) > 2047)       || // start greater than 2047
                (((Data & 0xffff0000) >> 16) > 16))   // width greater than 16
            {
                // Something is wrong, but the display driver won't do anything with the error, so use the closest acceptable value.
                if ((Data & 0x0000ffff) > 2047)
                    pVidLutCurDac->HalObject.Dac[buffNum].VerticalSyncStart  = 2047;
                if (((Data & 0xffff0000) >> 16) > 16)
                    pVidLutCurDac->HalObject.Dac[buffNum].VerticalSyncHeight = 16;
                return NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            }

            pVidLutCurDac->HalObject.Dac[buffNum].VerticalSyncStart  = Data & 0x0000ffff;
            pVidLutCurDac->HalObject.Dac[buffNum].VerticalSyncHeight = (Data & 0xffff0000) >> 16;
            return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
        }

        //
        // Set total size.
        //
        case NV07C_SET_DAC_TOTAL_SIZE(0):
        case NV07C_SET_DAC_TOTAL_SIZE(1):
        {
            buffNum = (Offset == NV07C_SET_DAC_TOTAL_SIZE(0)) ? 0 : 1;
            if (pVidLutCurDac->Dac[buffNum].NotifyTrigger)
                return NV07C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            // Bits 15:0 contain the width in pixels and bits 31:16 contain the height in lines.
            // Width must be a multiple of 8, less than or equal to 4128 and greater than or
            // equal to 40
            // Height must be less than or equal to 2049 and greater than or equal to 2.
            if (((Data & 0x3) != 0)                  || // width not a multiple of 8
                ((Data & 0x0000ffff) > 4128)         || // width greater than 4128
                ((Data & 0x0000ffff) < 40)           || // width less than 40
                (((Data & 0xffff0000) >> 16) > 2049) || // height greater than 2049
                (((Data & 0xffff0000) >> 16) < 2))      // height less than 2
            {
                // Something is wrong, but the display driver won't do anything with the error, so use the closest acceptable value.
                pVidLutCurDac->HalObject.Dac[buffNum].TotalWidth  = Data & 0x0000fffe;
                pVidLutCurDac->HalObject.Dac[buffNum].TotalHeight = (Data & 0xfffe0000) >> 16;
                return NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            }

            pVidLutCurDac->HalObject.Dac[buffNum].TotalWidth  = Data & 0x0000ffff;
            pVidLutCurDac->HalObject.Dac[buffNum].TotalHeight = (Data & 0xffff0000) >> 16;
            return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
        }

        // Set pixel clock.
        case NV07C_SET_DAC_PIXEL_CLOCK(0):
        case NV07C_SET_DAC_PIXEL_CLOCK(1):
        {
            buffNum = (Offset == NV07C_SET_DAC_PIXEL_CLOCK(0)) ? 0 : 1;
            if (pVidLutCurDac->Dac[buffNum].NotifyTrigger)
                return NV07C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            pVidLutCurDac->HalObject.Dac[buffNum].PixelClock = Data;
            return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
        }

        //
        // Set format
        //
        case NV07C_SET_DAC_FORMAT(0):
        case NV07C_SET_DAC_FORMAT(1):
        {
            RM_STATUS modeSetStatus;

            buffNum = (Offset == NV07C_SET_DAC_FORMAT(0)) ? 0 : 1;
            if (pVidLutCurDac->Dac[buffNum].NotifyTrigger)
                return NV07C_NOTIFICATION_STATUS_ERROR_STATE_IN_USE;

            //Copy to a head-specific pointer.  In addition to allowing reference
            // to dac properties on a per-display basis, this pointer can be used
            // to determine if the head is enabled (non-NULL ptr) or disabled (NULL ptr)
            pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].pVidLutCurDac = (VOID_PTR)pVidLutCurDac;
            pDev->Dac.HalInfo.CrtcInfo[DACGETHEAD(pVidLutCurDac)].pVidLutCurDac = &(pVidLutCurDac->HalObject);

            // Bit 0 contains double scan mode setting.
            // Bit 1 contains interlaced mode setting.
            // Bit 2 contains horizontal sync polarity setting.
            // Bit 3 contains vertical sync polarity setting.
            pVidLutCurDac->HalObject.Dac[buffNum].Format.DoubleScanMode          = (Data & BIT(DEVICE_BASE(NV07C_SET_DAC_FORMAT_DOUBLE_SCAN))) ? 1 : 0;
            pVidLutCurDac->HalObject.Dac[buffNum].Format.InterlacedMode          = (Data & BIT(DEVICE_BASE(NV07C_SET_DAC_FORMAT_INTERLACE))) ? 1 : 0;
            pVidLutCurDac->HalObject.Dac[buffNum].Format.HorizontalSyncPolarity  = (Data & BIT(DEVICE_BASE(NV07C_SET_DAC_FORMAT_HORIZONTAL_SYNC))) ? 1 : 0;
            pVidLutCurDac->HalObject.Dac[buffNum].Format.VerticalSyncPolarity    = (Data & BIT(DEVICE_BASE(NV07C_SET_DAC_FORMAT_VERTICAL_SYNC))) ? 1 : 0;

            // color is specified by bits 19:16.
            pVidLutCurDac->HalObject.Dac[buffNum].ColorFormat =
                (Data >> DRF_SHIFT(NV07C_SET_DAC_FORMAT_COLOR)) & DRF_MASK(NV07C_SET_DAC_FORMAT_COLOR);

            pVidLutCurDac->Dac[buffNum].NotifyAction = (Data & BIT(DEVICE_BASE(NV07C_SET_DAC_FORMAT_NOTIFY))) ? 1 : 0;

            pVidLutCurDac->Dac[buffNum].NotifyTrigger = TRUE;

            // NOTE: the modeset parameters are copied into pDev. This will be changed when we pass the pobject as a parameter to
            // the modeset function.

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
                                case NV07C_SET_DAC_FORMAT_COLOR_LUT_LE_Y8:
                                        pDev->Dac.HalInfo.Depth = 8;
                                        //pDev->Dac.DisplayPitch =  pDev->Framebuffer.HalInfo.HorizDisplayWidth * 1;
                                        break;
                                case NV07C_SET_DAC_FORMAT_COLOR_LE_BYPASS1R5G5B5:
                                        pDev->Dac.HalInfo.Depth = 15;
                                        //pDev->Dac.DisplayPitch =  pDev->Framebuffer.HalInfo.HorizDisplayWidth * 2;
                                        break;
                                case NV07C_SET_DAC_FORMAT_COLOR_BYPASS_LE_R5G6B5:
                                        pDev->Dac.HalInfo.Depth = 16;
                                        //pDev->Dac.DisplayPitch =  pDev->Framebuffer.HalInfo.HorizDisplayWidth * 2;
                                        break;
                                case NV07C_SET_DAC_FORMAT_COLOR_LE_BYPASS1X7R8G8B8:
                                        pDev->Dac.HalInfo.Depth = 32;
                                        //pDev->Dac.DisplayPitch =  pDev->Framebuffer.HalInfo.HorizDisplayWidth * 4;
                                        break;
                        }

            pDev->Dac.PixelDepth              = pDev->Dac.HalInfo.Depth;
            pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].PrimaryDevice =
                DRF_VAL(07C, _SET_DAC_FORMAT, _PRIMARY_DEVICE, Data);

            //LPL: this is a per-buffer, not per-head property -- are there times it
            // will differ between buffers?  I'm assuming not.
//            pDev->Dac.CrtcInfo[DACGETHEAD(pVidLutCurDac)].PixelDepth = pDev->Dac.HalInfo.Depth;
            pVidLutCurDac->HalObject.Dac[buffNum].PixelDepth = pDev->Dac.HalInfo.Depth;

            // Get output device...
            pVidLutCurDac->DisplayType            = (Data >> DRF_SHIFT(NV07C_SET_DAC_FORMAT_DISPLAY_TYPE)) & DRF_MASK(NV07C_SET_DAC_FORMAT_DISPLAY_TYPE);
            if (pVidLutCurDac->DisplayType > 3)
            {
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: class07c: invalid display type ", pVidLutCurDac->DisplayType);
                return NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            }

            // Get TV output format...
            pVidLutCurDac->TVStandard             = (Data >> DRF_SHIFT(NV07C_SET_DAC_FORMAT_TV_STANDARD)) & DRF_MASK(NV07C_SET_DAC_FORMAT_TV_STANDARD);
            if (pVidLutCurDac->TVStandard > 5)
            {
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: class07c: invalid TV standard ", pVidLutCurDac->TVStandard);
                return NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            }

            // Do the mode set.
            modeSetStatus = stateSetModeMultiHead(pDev, pVidLutCurDac);
            
            if (pVidLutCurDac->CBase.NotifyXlate)
            {
                status = notifyFillNotifierArray(pDev, pVidLutCurDac->CBase.NotifyXlate, 0, 0,
                                                 modeSetStatus,
                                                 NV07C_NOTIFIERS_SET_DAC(buffNum));
                //
                // Do any OS specified action related to this notification.
                //
                if (pVidLutCurDac->Dac[buffNum].NotifyAction)
                {
                    osNotifyEvent(pDev, (POBJECT)pVidLutCurDac,
                                  NV07C_NOTIFIERS_SET_DAC(buffNum),
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
            return NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
            break;
     }
} // end of class07CSetDACValues

RM_STATUS class07CSetHotPlug
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    //this function will turn or off the hotplug detection on the NV11 board
    //functionality will be added to the HAL to control hotplug mechanism
    U032 Head;
    U032 HotPlugDetect=0;
    U032 HotUnplugDetect=0;
    
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac_rmVersion;
    
    DBG_PRINTF((DBG_MODULE_DACCLASS,DBG_LEVEL_INFO, "NVRM: class07CSetDACValues\r\n"));
    
    //extract the pVidLutCurDac and the Head number
    pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)Object;
    Head = DACGETHEAD(pVidLutCurDac);
    
    //verify we aren't bein' slipped a loaded value
    if((~(NV07C_SET_HOTPLUG_PLUG|NV07C_SET_HOTPLUG_UNPLUG|NV07C_NOTIFY_WRITE_THEN_AWAKEN))&Data)
    {
        DBG_PRINTF((DBG_MODULE_DACCLASS,DBG_LEVEL_ERRORS,"NVRM: Bad mask sent to 7c set hotplug method.\n"));
    
        return NV07C_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT;
    }
    
    //setup the notifier
    pVidLutCurDac->CBase.NotifyAction  = (Data&NV07C_NOTIFY_WRITE_THEN_AWAKEN)?NV07C_NOTIFY_WRITE_THEN_AWAKEN:NV07C_NOTIFY_WRITE_ONLY;
    pVidLutCurDac->CBase.NotifyTrigger = TRUE;
    
    //now we need to save our intelligence
    pVidLutCurDac->HotPlug.HotPlugEventDescriptor=Data;
    
    //another Mac spec workaround
    //special case for Macintosh -- if the pVidLutCurDac we have is a faked one,
    //copy the notifier data from the real one to the fake one
    //we consider the one passed to us the real one, and the RM
    //version to be fake if the pointers do not match (make sure RM version not NULL)
    //because the one passed to us is related to the handle that we joined to the object
    //that display driver uses. This is nasty, but necessary.
    if(pVidLutCurDac!=((PVIDEO_LUT_CURSOR_DAC_OBJECT) pDev->Dac.CrtcInfo[Head].pVidLutCurDac))
    {
        if(!pDev->Dac.CrtcInfo[Head].pVidLutCurDac)
        {
            //if the rm version's pointer is null, we can place this one into RM without consequence
            pDev->Dac.CrtcInfo[Head].pVidLutCurDac = (VOID_PTR)pVidLutCurDac;
        }
        else
        {
            pVidLutCurDac_rmVersion=(PVIDEO_LUT_CURSOR_DAC_OBJECT) pDev->Dac.CrtcInfo[Head].pVidLutCurDac;
        
            //set the hotplug event descriptor data on the fake one, too
            pVidLutCurDac_rmVersion->HotPlug.HotPlugEventDescriptor=Data;
            
            //copy notifier data from real one to fake one (for hotplug only)
            osMemCopy((U008 *)&(pVidLutCurDac_rmVersion->Common),(U008 *)&(pVidLutCurDac->Common),sizeof(COMMONOBJECT));
        }
    }
    
    HotPlugDetect=((pVidLutCurDac->HotPlug.HotPlugEventDescriptor)&NV07C_SET_HOTPLUG_PLUG)?1:0;
    HotUnplugDetect=((pVidLutCurDac->HotPlug.HotPlugEventDescriptor)&NV07C_SET_HOTPLUG_UNPLUG)?1:0;
    
    //enable the hotplug interrupts as perscribed
    dacSetHotPlugInterrupts(pDev, Head, HotPlugDetect, HotUnplugDetect);
    
    DBG_PRINTF((DBG_MODULE_DACCLASS,DBG_LEVEL_INFO,"NVRM: HotPlugInterrupt configured.\n"));
    
    return NV07C_NOTIFICATION_STATUS_DONE_SUCCESS;
}

V032 HotPlugService(PHWINFO pDev)
{
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    U032 Head, HotPlugStatus[MAX_CRTCS], HotUnplugStatus[MAX_CRTCS];
    U032 NotifierData=NV07C_SET_HOTPLUG_NOHOTSWAP;
    U032 TriggerOSCall=0;
    
    DBG_PRINTF((DBG_MODULE_DACCLASS,DBG_LEVEL_INFO,"NVRM: Processing PBUS event.\n"));
    
    //do service for each head that notes interest
    for(Head=0;Head<pDev->Dac.HalInfo.NumCrtcs;Head++)
    {
        pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT) pDev->Dac.CrtcInfo[Head].pVidLutCurDac;
        
        dacGetHotPlugInterrupts(pDev, Head, &HotPlugStatus[Head], &HotUnplugStatus[Head], (Head==(pDev->Dac.HalInfo.NumCrtcs-1))?TRUE:FALSE /* reset status */ );
        DBG_PRINTF((DBG_MODULE_DACCLASS,DBG_LEVEL_INFO,"NVRM: Hot   plug status - %d.\n",HotPlugStatus[Head]));
        DBG_PRINTF((DBG_MODULE_DACCLASS,DBG_LEVEL_INFO,"NVRM: Hot unplug status - %d.\n",HotUnplugStatus[Head]));
        
        if(pVidLutCurDac)
        {
            if(pVidLutCurDac->HotPlug.HotPlugEventDescriptor)
            {                
                //reset data that we pass to notifier
                NotifierData=0;
                
                if(HotPlugStatus[Head])
                {
                    NotifierData|=NV07C_SET_HOTPLUG_PLUG;
                }
                
                if(HotUnplugStatus[Head])
                {
                    NotifierData|=NV07C_SET_HOTPLUG_UNPLUG;
                }
                
                if(NotifierData)
                {
                    TriggerOSCall=1;
                }
                
                //now do the notifier
                if(pVidLutCurDac->CBase.NotifyXlate)
                {
                    notifyFillNotifierArray(
                        pDev,
                        pVidLutCurDac->CBase.NotifyXlate,
                        NotifierData, // Info32
                        0,
                        NV07C_NOTIFICATION_STATUS_DONE_SUCCESS,
                        NV07C_NOTIFIERS_SET_HOTPLUG
                    );
                    
                    if (pVidLutCurDac->HotPlug.NotifyAction)
                    {
                        //this was a nasty abstraction.... but it looks OK if I copy the common object, which I do
                        //when its the Mac and the bloody fake VIDEO_LUT_CURSOR_DAC_OBJECT.... much grief!
                        osNotifyEvent(pDev, (POBJECT)pVidLutCurDac,
                                      NV07C_NOTIFIERS_SET_HOTPLUG,
                                      0 /* Method */,
                                      0 /* Data */,
                                      NV07C_NOTIFICATION_STATUS_DONE_SUCCESS,
                                      pVidLutCurDac->HotPlug.NotifyAction);
                    }
                }
                
                //print msg
                DBG_PRINTF((DBG_MODULE_DACCLASS,DBG_LEVEL_INFO,"NVRM: Posted notifier for hotplug event with data value U032 = 0x%x.\n",NotifierData));
            }
        }
    }
    
    if(TriggerOSCall)
    {
#ifdef MACOS
        osTriggerOSHotplug(pDev);
#endif
    }
    
    return 0;
}
