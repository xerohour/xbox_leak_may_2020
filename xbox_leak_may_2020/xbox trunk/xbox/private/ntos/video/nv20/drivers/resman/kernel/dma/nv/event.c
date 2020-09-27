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
* Module: EVENT.C                                                           *
*   Event notifications are handled in this module.  DMA report and OS      *
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


//---------------------------------------------------------------------------
//
//  Event Notification structure.
//
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//
//  Event Object support.
//
//---------------------------------------------------------------------------

// Given the Parent Object, add the notify event.
RM_STATUS eventSetObjectEventNotify
(
   POBJECT ParentObject,
   U032    hObject,
   U032    NotifyIndex,
   U032    NotifyType,
   U064    Data
)
{
    PEVENTNOTIFICATION EventNotify;
    RM_STATUS          status;
    
    //
    // Create the event notification object
    //
    status = osAllocMem((VOID **)&EventNotify, sizeof(EVENTNOTIFICATION));
    if (status)
        return (status);

    //
    // Fill in the fields
    //
    EventNotify->Handle      = hObject;
    EventNotify->NotifyIndex = NotifyIndex;
    EventNotify->NotifyType  = NotifyType;
    EventNotify->Data        = Data;

    //
    // Now insert the event into the event chain of this object.  
    // Order doesn't really matter.
    //
    EventNotify->Next = ParentObject->NotifyEvent;
    ParentObject->NotifyEvent = EventNotify;
    
    return (RM_OK);        
}

//---------------------------------------------------------------------------
//
//  Event Notification support.
//
//---------------------------------------------------------------------------

//
// Given an object's name and fifo, insert a new event notifier into this object.
//
RM_STATUS eventSetEventNotify
(
    PHWINFO     pDev, 
    U032        ObjectName,
    U032        ChID,
    U032        NotifyIndex,
    U032        NotifyType,
    U064        Data
)
{
#ifdef LEGACY_ARCH
    POBJECT            Object;
    RM_STATUS          status;
    
    //
    // Find the parent object
    //
    if (fifoSearchObject(pDev, ObjectName, ChID, &Object) == RM_OK)
    {
        // Found the parent.
		status = eventSetObjectEventNotify(Object, 0/* object handle*/, 
		                                   NotifyIndex, NotifyType, Data);
    }
    else
		// Did not find the parent
        status = RM_ERROR;
        
    return status;        
#else
	// For the new architecture, this proc is not called.
    return RM_ERROR;
#endif
}    

RM_STATUS eventRemoveObjectEventNotify
(
   POBJECT ParentObject,
   U032    hObject
)
{
    PEVENTNOTIFICATION nextEvent, lastEvent;
    BOOL found = FALSE;
    
    // check for null list
    nextEvent = (void *) 0;
    if (ParentObject->NotifyEvent != NULL)
    {
        // check for head of list
        nextEvent = lastEvent = ParentObject->NotifyEvent;
        if ((nextEvent->Handle = hObject))
        {
            ParentObject->NotifyEvent = nextEvent->Next;
            found = TRUE;
        }
        else
        {
            // check for internal nodes
            nextEvent = nextEvent->Next;
            while (nextEvent)
            {
                if ((nextEvent->Handle = hObject))
                {
                    lastEvent->Next = nextEvent->Next;
                    found = TRUE;
                    break;
                }
                lastEvent = nextEvent;
                nextEvent = nextEvent->Next;
            }
        }
    }
    
    // delete the event if it was found
    if (found)
    {
        osFreeMem(nextEvent);
    }
    
    return (found) ? RM_OK : RM_ERROR;
        
} // end of eventRemoveObjectEventNotify()

