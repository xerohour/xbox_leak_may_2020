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

/***************************** Object Manager *****************************\
*                                                                           *
* Module: CLASSOBJ.C                                                        *
*     NV_CLASS objects are defined and managed in this module.              *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <class.h>
#include <vblank.h>
#include <dma.h>
#include <gr.h>
#include <dac.h>
#include <fifo.h>
#include <os.h>
#include <modular.h>


//
// Class object table.
//
extern CLASS classTable[];


//---------------------------------------------------------------------------
//
//  Class object method functions.
//
//---------------------------------------------------------------------------

RM_STATUS mthdCreateLegacy
(
	PHWINFO pDev,
    POBJECT ClassObject,
    PMETHOD Method,
    U032    Offset,
    U032    NewName
)
{
    RM_STATUS status;
    POBJECT   NewObject;

    if (NewName < RESERVED_NAMES)
        return (RM_ERR_CREATE_BAD_CLASS);

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Creating Object ", NewName);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "               Class  ", ClassObject->Name);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "               ChID   ", ClassObject->ChID);
    
    //
    // Class specific creation routine.
    //
    status = classCreate((VOID*)pDev, (PCLASSOBJECT)ClassObject, NewName, &NewObject, NULL);
    if (status == RM_OK)
    {
        //
        // Init global state.
        //
        DBG_VAL_PTR(NewObject);

        //
        // Stick new object into FIFO database.
        //
        if ((status = fifoAddObject(pDev, NewObject)) != RM_OK)
            classDestroy((VOID*)pDev, NewObject);
    }
    return (status);
}
RM_STATUS mthdCreate
(
	PHWINFO pDev,
    POBJECT ClassObject,
    PMETHOD Method,
    U032    Offset,
    U032    NewName,
    VOID*   pCreateParms
)
{
    RM_STATUS status;
    POBJECT   NewObject;

    if (NewName < RESERVED_NAMES)
        return (RM_ERR_CREATE_BAD_CLASS);

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Creating Object ", NewName);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "               Class  ", ClassObject->Name);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "               ChID   ", ClassObject->ChID);
    
    //
    // Class specific creation routine.
    //
    status = classCreate((VOID*)pDev, (PCLASSOBJECT)ClassObject, NewName, &NewObject, pCreateParms);
    if (status == RM_OK)
    {
        //
        // Init global state.
        //
        DBG_VAL_PTR(NewObject);

        //
        // Stick new object into FIFO database.
        //
        if ((status = fifoAddObject(pDev, NewObject)) != RM_OK)
            classDestroy((VOID*)pDev, NewObject);
    }
    return (status);
}
RM_STATUS mthdDestroy
(
	PHWINFO pDev,
    POBJECT ClassObject,
    PMETHOD Method,
    U032    Offset,
    U032    DelName
)
{
    RM_STATUS status;
    POBJECT   DelObject;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Destroying Object ", DelName);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "                 Class  ", ClassObject->Name);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "                 ChID   ", ClassObject->ChID);
    //
    // Retrieve and delete object from FIFO database.
    //    
    status = fifoSearchObject(pDev, DelName, ClassObject->ChID, &DelObject);
    if (status == RM_OK)
    {
        //
        // Make sure to free up any events in this object
        //
        if (DelObject->NotifyEvent)
        {
            PEVENTNOTIFICATION thisEvent = DelObject->NotifyEvent;
            PEVENTNOTIFICATION nextEvent = DelObject->NotifyEvent->Next;
            
            //
            // Walk the event chain and delete
            //
            osFreeMem((VOID *)thisEvent);
            while (nextEvent)
            {
                thisEvent = nextEvent;
                nextEvent = thisEvent->Next;
                osFreeMem((VOID *)thisEvent);
            }
        }
        
        //
        // Class specific delete procedure.
        //
        status  = fifoDelObject(pDev, DelObject);
        status |= classDestroy((VOID*)pDev, DelObject);
    }
    return (status);
}
