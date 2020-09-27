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

/**************************** Graphics Manager *****************************\
*                                                                           *
* Module: GR.C                                                              *
*   The graphics engine is managed in this module.  All priviledged state   *
*   relating to the canvas and datapath is managed here.  Context switching *
*   is also contained here.                                                 *
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
#include <fifo.h>
#include <dma.h>
#include <gr.h>
#include <os.h>
#include "nvhw.h"
#ifdef WIN32
#include "nvwin32.h"   // XXX this shouldn't be needed
#else
#define NV_OS_WRITE_THEN_AWAKEN	   0x00000001
#endif

//
// Externs.
//
// extern PGRAPHICSCHANNEL grTable;
//
// Globals.
//
// PVBLANKNOTIFY grVBlankList[2];

//---------------------------------------------------------------------------
//
//  Graphics VBlank servicing.
//
//---------------------------------------------------------------------------

RM_STATUS grWaitVBlank
(
    PHWINFO pDev,
    PVBLANKNOTIFY VBlankNotify,
    U032          buffer
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
    VBlankNotify->Next   = pDev->DBgrVBlankList[buffer];
    pDev->DBgrVBlankList[buffer] = VBlankNotify;
    return (RM_OK);
}
RM_STATUS grVBlank
(
    PHWINFO pDev
)
{
    PVBLANKNOTIFY VBlankNotify;
    PVBLANKNOTIFY VBlankNext;

    //
    // Pull all notifies annd complete them.
    //
    VBlankNotify = pDev->DBgrVBlankList[pDev->Framebuffer.FlipFrom];
    while (VBlankNotify)
    {
        VBlankNext         = VBlankNotify->Next;
        VBlankNotify->Next = NULL;
        VBlankNotify->Proc(pDev, VBlankNotify->Object,
                           VBlankNotify->Param1,
                           VBlankNotify->Param2,
                           RM_OK);
        VBlankNotify = VBlankNext;
    }
    pDev->DBgrVBlankList[pDev->Framebuffer.FlipFrom] = NULL;
    pDev->Framebuffer.FinishFlags = 0;
    return (RM_OK);
}
RM_STATUS grCancelVBlank
(
    PHWINFO pDev,
    PVBLANKNOTIFY VBlankNotify
)
{
    U032          i;
    PVBLANKNOTIFY VBlankMatch;

    for (i = 0; i < 2; i++)
    {
        if (VBlankNotify == pDev->DBgrVBlankList[i])
        {
            pDev->DBgrVBlankList[i] = VBlankNotify->Next;
            VBlankNotify->Next = NULL;
            return (RM_OK);
        }
        else
        {
            VBlankMatch = pDev->DBgrVBlankList[i];
            while (VBlankMatch && VBlankMatch->Next != VBlankNotify)
                VBlankMatch = VBlankMatch->Next;
            if (VBlankMatch)
            {
                VBlankMatch->Next  = VBlankNotify->Next;
                VBlankNotify->Next = NULL;
                return (RM_OK);
            }
        }
    }
    return (RM_OK);
}

//---------------------------------------------------------------------------
//
//  Helper functions.
//
//---------------------------------------------------------------------------

static RM_STATUS grGetLastIllegalMthdObject
(
    PHWINFO pDev,
    U032    ChID,
    U032    Instance,
    POBJECT *cachedObject
)
{
    POBJECT lastObject = pDev->DBfifoTable[ChID].LastIllegalMthdObject;

    *cachedObject = (POBJECT)NULL;
    if (lastObject != (POBJECT)NULL)
    {
        if (ENGDECL_FIELD(*lastObject->Class->EngineDecl, _TAG) == GR_ENGINE_TAG)
        {
            if ((((PCOMMONOBJECT)(lastObject))->Instance & 0xFFFF) == (Instance & 0xFFFF))
            {
                // found it.
                *cachedObject = lastObject;
                return RM_OK;
            }
        }
    }

    // did not find a match.
    return RM_ERR_BAD_OBJECT; 
}

static VOID grSetLastIllegalMthdObject
(
    PHWINFO pDev,
    U032    ChID,
    POBJECT Object
)
{
    pDev->DBfifoTable[ChID].LastIllegalMthdObject = Object;
}
//---------------------------------------------------------------------------
//
//  Graphics engine service.
//
//---------------------------------------------------------------------------

#define BENSWORK

V032 grService
(
    PHWINFO pDev
)
{
    RM_STATUS   status = RM_OK;
    POBJECT     Object = NULL;
    POBJECT     Object2 = NULL;
    PDMAOBJECT  DmaObject;
    V032        Intr;
    U032        Instance, NotifyInstance;
    U032        ChID;
    U032        Class;
    U032        Offset;
    U032        Data;
    U032        NotifySource;
    GREXCEPTIONDATA ExceptionData;
#ifdef BENSWORK
    U032      isDebuggerPresent;
    U032      mustNotifyDebugger;

    isDebuggerPresent  = ((PRMINFO)pDev->pRmInfo)->Debugger.object != NULL;
    mustNotifyDebugger = 0;
#endif

    //
    // Disable FIFO from writing to GE and wait for GE idle.
    //
    REG_WR_DRF_DEF(_PGRAPH, _FIFO, _ACCESS, _DISABLED);

    //
    // Waiting for everything in the graphics engine to go idle is too restrictive.  Some
    // exceptions are raised while the offending function has stalled part of the engine,
    // therefore making it impossible to wait for idle.
    //
    // Only wait for idle in the specific interrupt handlers.
    //
    //GR_DONE();

    Intr = REG_RD32(NV_PGRAPH_INTR);

    if (Intr == 0)
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics empty interrupt!!\n\r");
    
    //
    // Query HAL for exception data.
    // any engine-based exceptions.
    //
    nvHalGrGetExceptionData(pDev, &ExceptionData);

    //
    // Here's where we'd bypass HAL gr exception handling if needed.
    // 

    //
    // Give HAL chance to service selected exceptions.
    //
    nvHalGrService(pDev, Intr, &ExceptionData);

    //
    // Refresh interrupt status since HAL may have resolved
    // some (or all) pending conditions.
    //
    if ((Intr = REG_RD32(NV_PGRAPH_INTR)) == 0)
    {
        //
        // Reset FIFO.
        //
        REG_WR_DRF_DEF(_PGRAPH, _FIFO, _ACCESS, _ENABLED);
        return (Intr);
    }

    //
    // Decode exception data.
    //
    Class = ExceptionData.classNum;
    NotifyInstance = ExceptionData.NotifyInstance;
    Instance = ExceptionData.Instance;
    Offset = ExceptionData.Offset;
    Data = ExceptionData.Data;
    ChID = ExceptionData.ChID;
    NotifySource = ExceptionData.Nsource;
    
    //
    // Reset interrupt registers before servicing them.
    //
    REG_WR32(NV_PGRAPH_INTR, Intr);

    //
    // If that was the only reason we're in here, let's leave now.  Most of the object
    // status updates that follow aren't valid when we just context switching.
    //
    if ((Intr == DRF_DEF(_PGRAPH, _INTR, _CONTEXT_SWITCH, _PENDING)) || 
        (Intr == DRF_DEF(_PGRAPH, _INTR, _SINGLE_STEP, _PENDING)))
    {    
        //
        // Reset FIFO.
        //
        REG_WR_DRF_DEF(_PGRAPH, _FIFO, _ACCESS, _ENABLED);
        return (REG_RD32(NV_PGRAPH_INTR));
    }
    
    //
    // The rest of the exceptions require that we know what object we're currently using.
    // Convert the object instance into the actual object pointer so we can route everything
    // appropriately.
    // 
    if (!Instance)
    {    
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics object instance is zero!!\n\r");
    }            
    else        
    {        
        status = grGetLastIllegalMthdObject(pDev, ChID, Instance, &Object);
        if (status != RM_OK)
        {
            status = grContextInstanceToObject(pDev, ChID, Instance, &Object);
            if (status)
            {
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics exception with NO valid object\n\r");
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Status:", status);
            }
        }
        //
        // Let's take this opportunity to update this object's notify information (ctx and request).
        // This is important information if we (the resmgr) need to perform the notification that
        // wasn't completed by the hardware due to this error condition.
        //
        // We need to convert the instance into a dma object, then plug the dma object
        // into this graphics object.  Also see if a notify is currently pending.
        //
        if (Object && NotifyInstance)
        {        
            status = dmaContextInstanceToObject(pDev, ChID, NotifyInstance, &DmaObject);
            if (status)
                Object->NotifyXlate = NULL;
            else
                Object->NotifyXlate = DmaObject;            
        }                

        //
        // Is there a notify pending in the hardware?
        //
        if ((NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _NOTIFICATION, _PENDING)) && Object)
        {
            GRNOTIFYDATA grNotifyData;

            nvHalGrGetNotifyData(pDev, &grNotifyData);
            if (grNotifyData.trigger == TRUE)
            {
                Object->NotifyTrigger = TRUE;
                Object->NotifyAction = grNotifyData.action;
                if (Object->NotifyXlate == NULL)
                {
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Pending notification with NO valid notifier\n\r");
                    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Object:", Object->Name);
                } else
                    status = notifyMethodComplete(pDev, Object, Offset, Data, RM_OK);

                // we've handled the notify here, so clear the PENDING bit
                NotifySource &= ~DRF_DEF(_PGRAPH, _NSOURCE, _NOTIFICATION, _PENDING);                
            }
        }
    }
    
    //
    // Missing hardware?  Must be a software method.  Route accordingly.
    //
    if ((Intr & DRF_DEF(_PGRAPH, _INTR, _MISSING_HW, _PENDING)) && Object)
    {
    
        //
        // Wait for idle
        //
        GR_DONE();
    
        //
        // Check per-class valid method.
        //
        status = classSoftwareMethod(pDev, Object, Offset, Data);
        if (status)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics software method.\n\r");
            if (Object)
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Graphics software Object:  ", Object->Name);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Graphics software ChID:    ", ChID);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Graphics software Class:   ", Class);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Graphics software Offset:  ", Offset);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Graphics software Data:    ", Data);
        }
    }

    //
    // Software assisted notification?
    //
    // Note: only NV10 has a PGRAPH_INTR_ERROR_PENDING bit which also flags
    // illegal method exceptions, but it should be safe to check this on NV4
    // since this'll read back as 0.
    //
    if (((Intr & DRF_DEF(_PGRAPH, _INTR, _NOTIFY, _PENDING)) ||
        (Intr & DRF_DEF(_PGRAPH, _INTR, _ERROR, _PENDING))) && Object)
    {
        //
        // Notification.
        //
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Graphics exception handler.\n\r");
        
        // KJK!! Missing hardware as a protection fault!!
        //
        // Check per-class valid method.
        //
        if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _ILLEGAL_MTHD, _PENDING))
        {        
        
            //
            // Wait for idle
            //
            GR_DONE();
        
            //
            // Convert the incoming data into the object id so the method can handle it.  Remember
            // that the FIFO has done a hash on the incoming data and passed the actual instance
            // pointer to the graphics engine.  Since we need to process based on the original data,
            // we need to convert it back to the original user data.
            //
            if (Offset < 0x200)
            {
                if (!Data)
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics object context data is zero!!\n\r");
                status = grContextInstanceToObject(pDev, ChID, Data, &Object2);
                if (status)
                {
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics exception with NO valid context\n\r");
                    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Status:", status);
                }
                if (status == RM_OK && Object2 != NULL)    
                {
                    Data = Object2->Name;
                }
            }                
        
            // Cache this object as the lastIllegalMthdObject.
            grSetLastIllegalMthdObject(pDev, ChID, Object);
            status = classSoftwareMethod(pDev, Object, Offset, Data);
            if (status)
            {
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics software method error!\n\r");
                if (Object)
                    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Graphics software Object:  ", Object->Name);
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Graphics software ChID:    ", ChID);
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Graphics software Class:   ", Class);
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Graphics software Offset:  ", Offset);
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Graphics software Data:    ", Data);

#ifdef BENSWORK // case 1
                // external debugger registered?
                if (isDebuggerPresent)
                {
                    NvdeAppendEventForDebugger ((PNVDEOBJECT)((PRMINFO)pDev->pRmInfo)->Debugger.object,1,3,Object ? Object->Name : ~0,ChID,Class,Offset,Data,0);
                    mustNotifyDebugger = 1;
                }
#endif
            }
        }
        else if (NotifySource)
        {
            // 
            // Determine the notification type.  The hardware will only track the cause, not the
            // type of notification the client has requested.  What is actually be done with this
            // notify (action) is determined by per-object data.
            // 
            //Object->NotifyAction = REG_RD_DRF(_PGRAPH, _NOTIFY, _TYPE);
            
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _NOTIFICATION, _PENDING))
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics software notification pending!\n\r");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _DATA_ERROR, _PENDING))
            {
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics invalid data error!\n\r");
            }
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _PROTECTION_ERROR, _PENDING))
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics surface protection error!\n\r");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _RANGE_EXCEPTION, _PENDING))
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics range exception error!\n\r");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _LIMIT_COLOR, _PENDING))
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics color buffer limit error!\n\r");
            //if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _LIMIT_ZETA, _PENDING))
            //    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics zeta buffer limit error!\n\r");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _DMA_R_PROTECTION, _PENDING))
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics dma read protection error!\n\r");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _DMA_W_PROTECTION, _PENDING))
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics dma write protection error!\n\r");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _FORMAT_EXCEPTION, _PENDING))
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics format exception error!\n\r");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _PATCH_EXCEPTION, _PENDING))
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics patch exception error!\n\r");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _STATE_INVALID, _PENDING))
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics object state invalid error!\n\r");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _DOUBLE_NOTIFY, _PENDING))
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics double notify error!\n\r");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _NOTIFY_IN_USE, _PENDING))
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics notify in use error!\n\r");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _METHOD_CNT, _PENDING))
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics method count error!\n\r");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _BFR_NOTIFICATION, _PENDING))
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics buffer notification error!\n\r");
            
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM:          ERROR:   ", NotifySource);
            if (Object)
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM:         Object:   ", Object->Name);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM:           ChID:   ", ChID);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM:          Class:   ", Class);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM:         Offset:   ", Offset);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM:           Data:   ", Data);
            DBG_BREAKPOINT();
#ifdef BENSWORK // case 2
            // external debugger registered?
            if (isDebuggerPresent)
            {
                NvdeAppendEventForDebugger ((PNVDEOBJECT)((PRMINFO)pDev->pRmInfo)->Debugger.object,2,3,0,0,0,0,0,0);
                mustNotifyDebugger = 1;
            }
            else
            {
                DBG_BREAKPOINT();
            }
#else
            DBG_BREAKPOINT();
#endif
            status = notifyMethodComplete(pDev, Object, Offset, Data, RM_OK);
        }            
    }
    
    //
    // Software assisted buffer notification?
    //    
    if ((Intr & DRF_DEF(_PGRAPH, _INTR, _BUFFER_NOTIFY, _PENDING)) && Object)
    {
    
        //
        // Wait for idle
        //
        GR_DONE();
    
        //
        // Notification.
        //
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Graphics buffer software notification\n\r");
        Object->NotifyTrigger = TRUE;
        // 
        // Determine the notification type.  The hardware will only track the cause, not the
        // type of notification the client has requested.  What is actually be done with this
        // notify (action) is determined by per-object data.
        // 
        //Object->NotifyAction = REG_RD_DRF(_PGRAPH, _NOTIFY, _TYPE);
        //
        // If the notify status is zero (no error), then this must have occurred due to
        // a WRITE_THEN_AWAKEN request
        //
        //if (!REG_RD32(NV_PGRAPH_NSTATUS))
            Object->NotifyAction = NV_OS_WRITE_THEN_AWAKEN;
        status = notifyBufferHWComplete(pDev, Object, Offset, Data, 1, RM_OK);
    }

#ifdef BENSWORK // case 3
    // external debugger registered?
    if (isDebuggerPresent)
    {
        // check for DMA faults on channel A & B
        if ((Intr & DRF_DEF(_PGRAPH, _INTR, _TLB_PRESENT_A, _PENDING))
         || (Intr & DRF_DEF(_PGRAPH, _INTR, _TLB_PRESENT_B, _PENDING)))
        {
            NvdeAppendEventForDebugger ((PNVDEOBJECT)((PRMINFO)pDev->pRmInfo)->Debugger.object,3,3,0,0,0,0,0,0);
            mustNotifyDebugger = 1;
        }
    }
#endif
#ifdef DEBUG
    //
    // DMA Fault on channel A?
    //
    if (Intr & DRF_DEF(_PGRAPH, _INTR, _TLB_PRESENT_A, _PENDING))
    {
    
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics DMA TLB not present or valid (A)\n\r");
    
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DMA_CONTROL    = ", REG_RD32(NV_PGRAPH_DMA_A_CONTROL));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DMA_LIMIT      = ", REG_RD32(NV_PGRAPH_DMA_A_LIMIT));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DMA_TLB_PTE    = ", REG_RD32(NV_PGRAPH_DMA_A_TLB_PTE));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DMA_TLB_TAG    = ", REG_RD32(NV_PGRAPH_DMA_A_TLB_TAG));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DMA_ADJ_OFFSET = ", REG_RD32(NV_PGRAPH_DMA_A_ADJ_OFFSET));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DMA_OFFSET     = ", REG_RD32(NV_PGRAPH_DMA_A_OFFSET));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DMA_SIZE       = ", REG_RD32(NV_PGRAPH_DMA_A_SIZE));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DMA_Y_SIZE     = ", REG_RD32(NV_PGRAPH_DMA_A_Y_SIZE));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DMA_INSTANCE   = ", REG_RD32(NV_PGRAPH_DMA_A_XLATE_INST));
    }

    //
    // DMA Fault on channel B?
    //
    if (Intr & DRF_DEF(_PGRAPH, _INTR, _TLB_PRESENT_B, _PENDING))
    {
    
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Graphics DMA TLB not present or valid (B)\n\r");
    
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DMA_CONTROL    = ", REG_RD32(NV_PGRAPH_DMA_B_CONTROL));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DMA_LIMIT      = ", REG_RD32(NV_PGRAPH_DMA_B_LIMIT));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DMA_TLB_PTE    = ", REG_RD32(NV_PGRAPH_DMA_B_TLB_PTE));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DMA_TLB_TAG    = ", REG_RD32(NV_PGRAPH_DMA_B_TLB_TAG));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DMA_ADJ_OFFSET = ", REG_RD32(NV_PGRAPH_DMA_B_ADJ_OFFSET));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DMA_OFFSET     = ", REG_RD32(NV_PGRAPH_DMA_B_OFFSET));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DMA_SIZE       = ", REG_RD32(NV_PGRAPH_DMA_B_SIZE));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DMA_Y_SIZE     = ", REG_RD32(NV_PGRAPH_DMA_B_Y_SIZE));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DMA_INSTANCE   = ", REG_RD32(NV_PGRAPH_DMA_B_XLATE_INST));
    }
    
    if (REG_RD32(NV_PGRAPH_INTR) != 0)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Graphics interrupt not completely serviced = ", REG_RD32(NV_PGRAPH_INTR));
    }
#endif // DEBUG

    //
    // Reset FIFO.
    //
#ifdef BENSWORK
    if (mustNotifyDebugger)
    {
        osNotifyEvent(pDev, ((PRMINFO)pDev->pRmInfo)->Debugger.object, 0, 0, 0, RM_OK, NV_OS_WRITE_THEN_AWAKEN);
    }
    else
    {
#endif
    REG_WR_DRF_DEF(_PGRAPH, _FIFO, _ACCESS, _ENABLED);
#ifdef BENSWORK
    }
#endif

    return (REG_RD32(NV_PGRAPH_INTR));
}

//
//  Search for the object matching a given context instance.
//
RM_STATUS grContextInstanceToObject
(
    PHWINFO  pDev,
    U032     ChID,
    U032     Instance,
    POBJECT *GrObject
)
{
    PNODE node;
    U032  engineTag;
    
    *GrObject = NULL;
    
    //
    // Do this brute force.  Search the whole fifo tree for the object.
    // 
    // Go find the minimum value (>0) and start there.  That way we're
    // sure to visit every node.
    //
    btreeEnumStart(0, &node, pDev->DBfifoTable[ChID].ObjectTree);

    while (node != NULL)
    {
        //
        // Get engine tag associated with this object (see nvhal.h for
        // list of valid tags).
        //
        engineTag = ENGDECL_FIELD(*(((POBJECT)(node->Data))->Class)->EngineDecl, _TAG);

        //
        // We only want graphics/software objects, so prune out
        // everything else.
        //
        if (engineTag != CLASS_PSEUDO_ENGINE_TAG && engineTag != NO_ENGINE)
        {
            //
            // Verify the instance
            //
            if ((((PCOMMONOBJECT)(node->Data))->Instance & 0xFFFF) == (Instance & 0xFFFF))
            {
                // FOUND IT!!
                *GrObject = (POBJECT)(node->Data);
                return (RM_OK);
            }
        }

        btreeEnumNext(&node, pDev->DBfifoTable[ChID].ObjectTree);
    }
    
    return (RM_ERR_BAD_OBJECT);
}

//
// Delete the graphics context instance for a given object
//
RM_STATUS grDeleteObjectInstance
(
    PHWINFO       pDev,
    PCOMMONOBJECT Object
)
{
    RM_STATUS status = RM_OK;

    //
    // Free the instance
    //
    if (Object->Instance) {
        status = fbFreeInstMem(pDev, Object->Instance, 1);    // one paragraph
        if (status)
            return(status);

        //
        // Zero the fields, just in case
        //
        INST_WR32(Object->Instance, SF_OFFSET(NV_PRAMIN_CONTEXT_0), 0);
        INST_WR32(Object->Instance, SF_OFFSET(NV_PRAMIN_CONTEXT_1), 0);
        INST_WR32(Object->Instance, SF_OFFSET(NV_PRAMIN_CONTEXT_2), 0);
        
        Object->Instance = 0;
    }

    return (status);
}
