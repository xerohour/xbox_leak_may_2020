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

/***************************** NVDE ****************************************\
*                                                                           *
* Module: NVDE.C                                                            *
*     NVDE methods                                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <class.h>
#include <modular.h>
#include <event.h>

RM_STATUS NvdeAlloc
(
    U032            Name,
    VOID*           pCreateParms
)
{
    PRMINFO pRmInfo = &NvRmInfo;
    RM_STATUS status;
    POBJECT pObject;
    PNVDEOBJECT pNvdeObject;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvdeAlloc\r\n");

    //
    // Make sure an NVDE object isn't already in use
    //
    if (pRmInfo->Debugger.object)
        return (NVDE_STATUS_ERROR_STATE_IN_USE);

    //
    // Allocate space for the object.
    //
    status = osAllocMem((VOID **)&pObject, sizeof(NVDEOBJECT));
    if (status)
        return (status);

    //
    // Set some default state.
    //
    pObject->Name = Name;
    pObject->ChID = 0;
    pObject->Class = NULL;
    pObject->ThisClass = NULL;
    pObject->Subchannel = NULL;
    pObject->Next = NULL;
    pObject->NotifyXlate = NULL;
    pObject->NotifyAction = 0;
    pObject->NotifyTrigger = 0;
    pObject->NotifyEvent = 0;

    //
    // Save off the event parameters.
    //
    pNvdeObject = (PNVDEOBJECT)pObject;
    pNvdeObject->EventHandle = ((NVDE_ALLOCATION_PARAMETERS*)pCreateParms)->EventHandle;
    pNvdeObject->EventClass = ((NVDE_ALLOCATION_PARAMETERS*)pCreateParms)->EventClass;
    pNvdeObject->EventBufferCount = 0;

    //
    // Queue up event...
    //
    status = eventSetObjectEventNotify(pObject, 0, 0, pNvdeObject->EventClass,  pNvdeObject->EventHandle);

    //
    // Set the pointer so we know a NVDE object is in use
    //
    if (status == RM_OK)
        pRmInfo->Debugger.object = (POBJECT) pObject;

    return status;
}


RM_STATUS NvdeFree
(
    U032 Client,
    U032 Object
)
{
    PRMINFO pRmInfo = &NvRmInfo;
    PNVDEOBJECT pNvdeObject = (PNVDEOBJECT)pRmInfo->Debugger.object;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvdeFree\r\n");

    if (pNvdeObject == NULL)
        return RM_OK;

    //
    // Assert that the handles match.
    //
    if (pNvdeObject->Common.Base.Name != Object)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: bad debugger object handle ", Object);
        return RM_ERROR;
    }

    //
    // Unplug event.  Handle is 0 for now since we only support
    // a single debugger event.
    //
    eventRemoveObjectEventNotify((POBJECT)pNvdeObject, 0);

    //
    // Clear out the NVDE object pointer so we know it's not in use
    //
    pRmInfo->Debugger.object = (POBJECT)NULL;

    return (osFreeMem(pNvdeObject));
}

V032 NvdeAppendEventForDebugger
(
    PNVDEOBJECT pDebugger,
    U032        code,
    U032        flags,
    U032        param1,
    U032        param2,
    U032        param3,
    U032        param4,
    U032        param5,
    U032        param6
)
{
    PNVDEEVENT event;

    if (pDebugger->EventBufferCount >= NVDE_MAX_EVENTS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Too many NVDE events to buffer.\n\r");
        return 0;
    }
    
    event           = pDebugger->EventBuffer + pDebugger->EventBufferCount;
    event->code     = code;
    event->flags    = flags;
    event->param[0] = param1;
    event->param[1] = param2;
    event->param[2] = param3;
    event->param[3] = param4;
    event->param[4] = param5;
    event->param[5] = param6;

    pDebugger->EventBufferCount++;
    return 0;
}
