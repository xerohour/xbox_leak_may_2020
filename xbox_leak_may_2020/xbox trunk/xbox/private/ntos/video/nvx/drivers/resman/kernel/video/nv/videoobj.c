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
* Module: videoobj.c                                                        *
*   Video engine objects are managed here.                                  *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#include <nv_ref.h>
#include <nvrm.h>
#include <class.h>
#include <smp.h>
#include "nvhw.h"

//
// Forwards.
//

// NV_PATCHCORD_VIDEO (class60)
static RM_STATUS videoCreatePatchcordObj(PHWINFO, PCLASSOBJECT, U032, POBJECT *);
static RM_STATUS videoDestroyPatchcordObj(PHWINFO, POBJECT);

// NV_VIDEO_SINK (class61)
static RM_STATUS videoCreateSinkObj(PHWINFO, PCLASSOBJECT, U032, POBJECT *);
static RM_STATUS videoDestroySinkObj(PHWINFO, POBJECT);

// NV_VIDEO_COLORMAP (class62)
static RM_STATUS videoCreateColormapObj(PHWINFO, PCLASSOBJECT, U032, POBJECT *);
static RM_STATUS videoDestroyColormapObj(PHWINFO, POBJECT);

// NV_VIDEO_FROM_MEMORY (class63)
static RM_STATUS videoCreateFromMemoryObj(PHWINFO, PCLASSOBJECT, U032, POBJECT *);
static RM_STATUS videoDestroyFromMemoryObj(PHWINFO, POBJECT);

// NV_VIDEO_SCALER (class64)
static RM_STATUS videoCreateScalerObj(PHWINFO, PCLASSOBJECT, U032, POBJECT *);
static RM_STATUS videoDestroyScalerObj(PHWINFO, POBJECT);

// NV_VIDEO_COLOR_KEY (class65)
static RM_STATUS videoCreateColorKeyObj(PHWINFO, PCLASSOBJECT, U032, POBJECT *);
static RM_STATUS videoDestroyColorKeyObj(PHWINFO, POBJECT);

// NVXX_VIDEO_OVERLAY
static RM_STATUS videoCreateOverlayObj(PHWINFO, PCLASSOBJECT, U032, POBJECT *, VOID *);
static RM_STATUS videoDestroyOverlayObj(PHWINFO, POBJECT);

//
// Class lock macros (video engine allocations are per-device).
//
#define VIDEOCLASSLOCK_ISSET(pdev,bit)  (pDev->Video.ClassLocks & (1 << bit))
#define VIDEOCLASSLOCK_SET(pdev,bit)    (pDev->Video.ClassLocks |= (1 << bit))
#define VIDEOCLASSLOCK_CLEAR(pdev,bit)  (pDev->Video.ClassLocks &= ~(1 << bit))

//
// videoCreateObj
//
// Top-level create routine for video engine classes.
//
RM_STATUS videoCreateObj
(
    VOID*   pDevHandle,
    PCLASSOBJECT ClassObject,
    U032    Name,
    POBJECT *Object,
    VOID*   pCreateParms
)
{
    PHWINFO         pDev = (PHWINFO) pDevHandle;
    RM_STATUS       status = RM_OK;
    PCLASS          pClass = ClassObject->Base.ThisClass;
    U032            lockBit;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: videoCreateObj\r\n");

    //
    // Check allocation restrictions.
    //
    lockBit = CLASSDECL_FIELD(*pClass->ClassDecl, _LOCK);
    if (lockBit != NO_LOCK)
    {
        if (VIDEOCLASSLOCK_ISSET(pDev, lockBit))
            return (RM_ERR_ILLEGAL_OBJECT);
        VIDEOCLASSLOCK_SET(pDev, lockBit);
    }

    //
    // Invoke class-specific create routine.    
    //
    switch (pClass->Type)
    {
        case NV_PATCHCORD_VIDEO:
            status = videoCreatePatchcordObj(pDev, ClassObject, Name, Object);
            break;
        case NV_VIDEO_SINK:
            status = videoCreateSinkObj(pDev, ClassObject, Name, Object);
            break;
        case NV_VIDEO_COLORMAP:
            status = videoCreateColormapObj(pDev, ClassObject, Name, Object);
            break;
        case NV_VIDEO_FROM_MEMORY:
            status = videoCreateFromMemoryObj(pDev, ClassObject, Name, Object);
            break;
        case NV_VIDEO_SCALER:
            status = videoCreateScalerObj(pDev, ClassObject, Name, Object);
            break;
        case NV_VIDEO_COLOR_KEY:
            status = videoCreateColorKeyObj(pDev, ClassObject, Name, Object);
            break;
        case NV04_VIDEO_OVERLAY:
        case NV10_VIDEO_OVERLAY:
            status = videoCreateOverlayObj(pDev, ClassObject, Name, Object, pCreateParms);
            break;
        default:
            //
            // Should *never* happen because this create entry point was
            // already bound to this class type in class.c.
            //
            status = RM_ERR_ILLEGAL_OBJECT;
            break;
    }

    if (status != RM_OK)
    {
        if (lockBit != NO_LOCK)
            VIDEOCLASSLOCK_CLEAR(pDev, lockBit);
        return (status); 
    }

    return (RM_OK);
}

RM_STATUS videoDestroyObj
(
    VOID*   pDevHandle,
    POBJECT Object
)
{
    PHWINFO             pDev = (PHWINFO) pDevHandle;
    RM_STATUS           status = RM_OK;
    PCOMMONOBJECT       pCommonObject;
    PCLASS              pClass;
    U032                lockBit;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: videoDestroyObj\r\n");

    //
    // Just in case...
    //
    if (Object == NULL)
        return (RM_OK);

    //
    // A nice cast to make the code more readable.
    //
    pCommonObject = (PCOMMONOBJECT)Object;

    //
    // Clear per-device allocation lock bit if applicable.
    //
    pClass = Object->ThisClass;
    lockBit = CLASSDECL_FIELD(*pClass->ClassDecl, _LOCK);
    if (lockBit != NO_LOCK)
    {
        // ASSERT(VIDEOCLASSLOCK_ISSET(pDev, lockBit));
        VIDEOCLASSLOCK_CLEAR(pDev, lockBit);
    }

    //
    // Clear the subChannel ptrs for this object 
    //
	osEnterCriticalCode(pDev);
	fifoDeleteSubchannelContextEntry(pDev, Object);
	osExitCriticalCode(pDev);

    //
    // And finally delete the object itself    
    //
    switch (pClass->Type)
    {
        case NV_PATCHCORD_VIDEO:
            status = videoDestroyPatchcordObj(pDev, Object);
            break;
        case NV_VIDEO_SINK:
            status = videoDestroySinkObj(pDev, Object);
            break;
        case NV_VIDEO_COLORMAP:
            status = videoDestroyColormapObj(pDev, Object);
            break;
        case NV_VIDEO_FROM_MEMORY:
            status = videoDestroyFromMemoryObj(pDev, Object);
            break;
        case NV_VIDEO_SCALER:
            status = videoDestroyScalerObj(pDev, Object);
            break;
        case NV_VIDEO_COLOR_KEY:
            status = videoDestroyColorKeyObj(pDev, Object);
            break;
        case NV04_VIDEO_OVERLAY:
        case NV10_VIDEO_OVERLAY:
            status = videoDestroyOverlayObj(pDev, Object);
            break;
        default:
            //
            // Should *never* happen because this create entry point was
            // already bound to this class type in class.c.
            //
            status = RM_ERR_ILLEGAL_OBJECT;
    }
    return (status);
}

//----------------------------------------------------------------------
// Video patchcord create/destroy routines.
//----------------------------------------------------------------------
static RM_STATUS videoCreatePatchcordObj
(
    PHWINFO pDev,
    PCLASSOBJECT ClassObject,
    U032 Name,
    POBJECT *Object
)
{
    RM_STATUS status;
    U032 i;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: videoCreatePatchcordObj\r\n");

    //
    // NV_PATCHCORD_VIDEO requires no HAL support.
    //

    //
    // Allocate space for the object.
    //
    status = osAllocMem((VOID **)Object, sizeof (VIDEOPATCHCORD));
    if (status)
        return (status);

    //
    // Init common (software) state.
    //
    classInitCommonObject(*Object, ClassObject, Name);

    ((PVIDEOPATCHCORD)*Object)->FanOut      = 0;
    ((PVIDEOPATCHCORD)*Object)->Source      = NULL;
    for (i = 0; i < MAX_GRPATCH_FANOUT; i++)
        ((PVIDEOPATCHCORD)*Object)->Destination[i] = NULL;

    //
    // Build a FIFO/Hash context for this object
    //
    status = fifoUpdateObjectInstance(pDev,
                                      &((PVIDEOPATCHCORD)*Object)->CBase, 
                                      ((PVIDEOPATCHCORD)*Object)->CBase.ChID, 
                                      ((PVIDEOPATCHCORD)*Object)->CInstance);
    if (status != RM_OK)
        osFreeMem(*Object);

    return (status);
}

static RM_STATUS videoDestroyPatchcordObj
(
    PHWINFO pDev,
    POBJECT Object
)
{
    RM_STATUS status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: videoDestroyPatchcordObj\r\n");

    //
    // NV_PATCHCORD_VIDEO requires no HAL support.
    //

    //
    // Delete the FIFO/Hash context for this object
    //
    status = fifoDeleteObjectInstance(pDev, Object, Object->ChID);
    if (status)
        return (status);

    return (osFreeMem(Object));
}

//----------------------------------------------------------------------
// Video sink create/destroy routines.
//----------------------------------------------------------------------
static RM_STATUS videoCreateSinkObj
(
    PHWINFO pDev,
    PCLASSOBJECT ClassObject,
    U032 Name,
    POBJECT *Object
)
{
    U032 i;
    RM_STATUS status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: videoCreateSinkObj\r\n");

    //
    // NV_VIDEO_SINK requires no HAL support.
    //

    //
    // Allocate space for the object.
    //
    status = osAllocMem((VOID **)Object, sizeof (VIDEOSINKOBJECT));
    if (status)
        return (status);

    //
    // Init common (software) state.
    //
    classInitCommonObject(*Object, ClassObject, Name);

    for (i = 0; i < MAX_GRPATCH_INPUT; i++)
        ((PVIDEOSINKOBJECT)*Object)->VideoInput[i] = NULL;

    //
    // Build a FIFO/Hash context for this object
    //
    status = fifoUpdateObjectInstance(pDev,
                                      &((PVIDEOSINKOBJECT)*Object)->CBase, 
                                      ((PVIDEOSINKOBJECT)*Object)->CBase.ChID, 
                                      ((PVIDEOSINKOBJECT)*Object)->CInstance);
    if (status != RM_OK)
        osFreeMem(*Object);

    return (status);
}

static RM_STATUS videoDestroySinkObj
(
    PHWINFO pDev,
    POBJECT Object
)
{
    RM_STATUS status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: videoDestroySinkObj\r\n");

    //
    // NV_VIDEO_SINK requires no HAL support.
    //

    //
    // Delete the FIFO/Hash context for this object
    //
    status = fifoDeleteObjectInstance(pDev, Object, Object->ChID);
    if (status)
        return (status);

    return (osFreeMem(Object));
}

//----------------------------------------------------------------------
// Video colormap create/destroy routines.
//----------------------------------------------------------------------
static RM_STATUS videoCreateColormapObj
(
    PHWINFO pDev,
    PCLASSOBJECT ClassObject,
    U032 Name,
    POBJECT *Object
)
{
    U032 i;
    RM_STATUS status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: videoCreateColormapObj\r\n");

    //
    // NV_VIDEO_COLORMAP requires no HAL support.
    //

    //
    // Allocate space for the object.
    //
    status = osAllocMem((VOID **)Object, sizeof (VIDEOCOLORMAPOBJECT));
    if (status)
        return (status);

    //
    // Init common (software) state.
    //
    classInitCommonObject(*Object, ClassObject, Name);

    ((PVIDEOCOLORMAPOBJECT)*Object)->VideoInput  = NULL;
    ((PVIDEOCOLORMAPOBJECT)*Object)->VideoOutput = NULL;
    ((PVIDEOCOLORMAPOBJECT)*Object)->ColorFormat = INVALID_COLOR_FORMAT;
    ((PVIDEOCOLORMAPOBJECT)*Object)->Xlate       = NULL;
    ((PVIDEOCOLORMAPOBJECT)*Object)->Start       = 0;
    ((PVIDEOCOLORMAPOBJECT)*Object)->Length      = 0;
    ((PVIDEOCOLORMAPOBJECT)*Object)->DirtyStart  = 0;
    ((PVIDEOCOLORMAPOBJECT)*Object)->DirtyLength = 0;
    for (i = 0; i < 256; i++)
        ((PVIDEOCOLORMAPOBJECT)*Object)->ColorMap[i] = (i << 16) | (i << 8) | i;

    //
    // Build a FIFO/Hash context for this object
    //
    status = fifoUpdateObjectInstance(pDev,
                                      &((PVIDEOCOLORMAPOBJECT)*Object)->CBase, 
                                      ((PVIDEOCOLORMAPOBJECT)*Object)->CBase.ChID, 
                                      ((PVIDEOCOLORMAPOBJECT)*Object)->CInstance);
    if (status != RM_OK)
        osFreeMem(*Object);

    return (status);
}

static RM_STATUS videoDestroyColormapObj
(
    PHWINFO pDev,
    POBJECT Object
)
{
    RM_STATUS status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: videoDestroyColormapObj\r\n");

    //
    // NV_VIDEO_COLORMAP requires no HAL support.
    //

    //
    // Delete the FIFO/Hash context for this object
    //
    status = fifoDeleteObjectInstance(pDev, Object, Object->ChID);
    if (status)
        return (status);
    
    // null out the color map object cached in pDev upon destruction
    if ((POBJECT)pDev->colormapObjectToNotify == Object)
    {
        pDev->colormapObjectToNotify = NULL;
    }

    return (osFreeMem(Object));
}

//----------------------------------------------------------------------
// Video from memory create/destroy routines.
//----------------------------------------------------------------------
static RM_STATUS videoCreateFromMemoryObj
(
    PHWINFO      pDev,
    PCLASSOBJECT ClassObject,
    U032         UserName,
    POBJECT     *Object
)
{
    RM_STATUS           status;
    PCLASS              pClass = ClassObject->Base.ThisClass;
    PVIDEOFROMMEMOBJECT pVidFromMem;
    U008                *bytePtr;
    U032                i;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: videoCreateFromMemoryObj\r\n");

    //
    // Create the NV_VIDEO_FROM_MEMORY object.
    //
    status = osAllocMem((VOID **)Object, sizeof (VIDEOFROMMEMOBJECT));
    if (status)
        return (status);

    //
    // A nice cast to make the code more readable.
    //
    pVidFromMem = (PVIDEOFROMMEMOBJECT)*Object;

    //
    // Initialize the new object structure.
    //
    bytePtr = (U008 *)pVidFromMem;
    for (i = 0; i < sizeof (VIDEOFROMMEMOBJECT); i++)
       *bytePtr++ = 0;

    //
    // Setup any specific initialization values.
    //
    pVidFromMem->InitState               = 0;
    pVidFromMem->BBuffer[0].State        = BUFFER_IDLE;
    pVidFromMem->BBuffer[1].State        = BUFFER_IDLE;
    pVidFromMem->VBlankNotify[0].Pending = FALSE;
    pVidFromMem->VBlankNotify[1].Pending = FALSE;

    //
    // Be sure to clear pending vblank notifies
    //
    for (i=0;i<2;i++)
    {
        pDev->Video.class63VBlankCount[i] = 0;
        pDev->DBclass63VBlankList[i] = NULL;
    }

    //
    // Init common (software) state.
    //
    classInitCommonObject(&pVidFromMem->CBase, ClassObject, UserName);

    //
    // Let the HAL setup any chip-dependent resources.
    //
    status = nvHalVideoAlloc(pDev, pVidFromMem->CBase.ChID, pVidFromMem->CInstance, pClass->Type, &pVidFromMem->HalObject);
    if (status)
        return (status);

    //
    // HACK (scottl): There are several places in the RM that assume
    // the struct _def_common_object is at the head of class-specific
    // data structures like struct _def_video_from_memory_object
    // (most notably the call to fifoAddObject after we return from
    // here).  In order to allow these to continue to work, we do
    // the following completely bogus initialization.
    //
    (*Object)->Name          = UserName;
    (*Object)->Class         = ClassObject->Base.ThisClass;
    (*Object)->ThisClass     = ClassObject->Base.ThisClass;
    (*Object)->ChID          = ClassObject->Base.ChID;
    (*Object)->NotifyXlate   = NULL;
    (*Object)->NotifyAction  = 0;
    (*Object)->NotifyTrigger = FALSE;
    (*Object)->NotifyEvent   = NULL;

    //
    // Build a FIFO/Hash context for this object
    //
    status = fifoUpdateObjectInstance(pDev,
                                      &pVidFromMem->CBase, 
                                      pVidFromMem->CBase.ChID, 
                                      pVidFromMem->CInstance);
    if (status)
        return (status); 
        
    return (RM_OK);
}

static RM_STATUS videoDestroyFromMemoryObj
(
    PHWINFO pDev,
    POBJECT Object
)
{
    PVIDEOFROMMEMOBJECT pVidFromMem;
    U032                i;
    RM_STATUS           status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: videoDestroyFromMemoryObj\r\n");

    //
    // A nice cast to make the code more readable.
    //
    pVidFromMem = (PVIDEOFROMMEMOBJECT)Object;
    
    //
    // Destroy the NV_VIDEO_FROM_MEMORY object.
    //

    //
    // Halt transfers.
    //
    class63StopTransfer(pDev, Object, 0, 0, 0);

    //
    // Let the HAL do it's cleanup thing.
    //
    nvHalVideoFree(pDev, pVidFromMem->CBase.ChID, pVidFromMem->CInstance, Object->ThisClass->Type, &pVidFromMem->HalObject);

    //
    // Be sure to clear pending vblank notifies
    //
    for (i=0;i<2;i++)
    {
        pDev->Video.class63VBlankCount[i] = 0;
        pDev->DBclass63VBlankList[i] = NULL;
    }

    if ((POBJECT) pDev->Video.HalInfo.ActiveVideoOverlayObject == (POBJECT) pVidFromMem)
        pDev->Video.HalInfo.ActiveVideoOverlayObject = NULL;

    //
    // Delete the FIFO/Hash context for this object
    //
    status = fifoDeleteObjectInstance(pDev, &pVidFromMem->CBase, pVidFromMem->CBase.ChID); 
    if (status)
        return (status);

    return (osFreeMem(Object));
}

//----------------------------------------------------------------------
// Video scaler create/destroy routines.
//----------------------------------------------------------------------
static RM_STATUS videoCreateScalerObj
(
    PHWINFO      pDev,
    PCLASSOBJECT ClassObject,
    U032         UserName,
    POBJECT     *Object
)
{
    PCLASS              pClass = ClassObject->Base.ThisClass;
    RM_STATUS           status;
    PVIDEOSCALEROBJECT  pVidScaler;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: videoCreateScalerObj\r\n");

    //
    // Create the NV_VIDEO_SCALER object.
    //
    status = osAllocMem((VOID **)Object, sizeof (VIDEOSCALEROBJECT));
    if (status)
        return (status);

    //
    // A nice cast to make the code more readable.
    //
    pVidScaler = (PVIDEOSCALEROBJECT)*Object;

    //
    // Init common (software) state.
    //
    classInitCommonObject(&pVidScaler->CBase, ClassObject, UserName);

    //
    // Let the HAL setup any chip-dependent resources.
    //
    status = nvHalVideoAlloc(pDev, pVidScaler->CBase.ChID, pVidScaler->CInstance, pClass->Type, &pVidScaler->HalObject);
    if (status)
        return (status);

    //
    // Build a FIFO/Hash context for this object
    //
    status = fifoUpdateObjectInstance(pDev,
                                      &pVidScaler->CBase, 
                                      pVidScaler->CBase.ChID, 
                                      pVidScaler->CInstance);
    if (status)
        return (status); 
        
    return (RM_OK);
}

static RM_STATUS videoDestroyScalerObj
(
    PHWINFO pDev,
    POBJECT Object
)
{
    PVIDEOSCALEROBJECT  pVidScaler;
    RM_STATUS           status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: videoDestroyScalerObj\r\n");

    //
    // A nice cast to make the code more readable.
    //
    pVidScaler = (PVIDEOSCALEROBJECT)Object;
    
    //
    // Destroy the NV_VIDEO_SCALER object.
    //

    //
    // Let the HAL do it's cleanup thing.
    //
    nvHalVideoFree(pDev, pVidScaler->CBase.ChID, pVidScaler->CInstance, Object->ThisClass->Type, &pVidScaler->HalObject);

    // Delete the FIFO/Hash context for this object
    //
    status = fifoDeleteObjectInstance(pDev, &pVidScaler->CBase, pVidScaler->CBase.ChID); 
    if (status)
        return (status);

    return (osFreeMem(Object));
}


//----------------------------------------------------------------------
// Video colorkey create/destroy routines.
//----------------------------------------------------------------------
static RM_STATUS videoCreateColorKeyObj
(
    PHWINFO      pDev,
    PCLASSOBJECT ClassObject,
    U032         UserName,
    POBJECT     *Object
)
{
    PCLASS                  pClass = ClassObject->Base.ThisClass;
    RM_STATUS               status;
    PVIDEOCOLORKEYOBJECT    pVidColorKey;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: videoCreateColorKeyObj\r\n");

    //
    // Create the NV_VIDEO_COLOR_KEY object.
    //
    status = osAllocMem((VOID **)Object, sizeof (VIDEOCOLORKEYOBJECT));
    if (status)
        return (status);

    //
    // A nice cast to make the code more readable.
    //
    pVidColorKey = (PVIDEOCOLORKEYOBJECT)*Object;

    //
    // Init common (software) state.
    //
    classInitCommonObject(&pVidColorKey->CBase, ClassObject, UserName);

    //
    // Let the HAL setup any chip-dependent resources.
    //
    status = nvHalVideoAlloc(pDev, pVidColorKey->CBase.ChID, pVidColorKey->CInstance, pClass->Type, &pVidColorKey->HalObject);
    if (status)
        return (status);

    //
    // Build a FIFO/Hash context for this object
    //
    status = fifoUpdateObjectInstance(pDev,
                                      &pVidColorKey->CBase, 
                                      pVidColorKey->CBase.ChID, 
                                      pVidColorKey->CInstance);
    if (status)
        return (status); 
        
    return (RM_OK);
}

static RM_STATUS videoDestroyColorKeyObj
(
    PHWINFO pDev,
    POBJECT Object
)
{
    PVIDEOCOLORKEYOBJECT    pVidColorKey;
    RM_STATUS               status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: videoDestroyColorKeyObj\r\n");

    //
    // A nice cast to make the code more readable.
    //
    pVidColorKey = (PVIDEOCOLORKEYOBJECT)Object;
    
    //
    // Destroy the NV_VIDEO_COLOR_KEY object.
    //

    //
    // Let the HAL do it's cleanup thing.
    //
    nvHalVideoFree(pDev, pVidColorKey->CBase.ChID, pVidColorKey->CInstance, Object->ThisClass->Type, &pVidColorKey->HalObject);

    //
    // Delete the FIFO/Hash context for this object
    //
    status = fifoDeleteObjectInstance(pDev, &pVidColorKey->CBase, pVidColorKey->CBase.ChID); 
    if (status)
        return (status);

    return (osFreeMem(Object));
}

//----------------------------------------------------------------------
// Video overlay create/destroy routines.
//----------------------------------------------------------------------

//
// Return head number specified in alloc request.
//
static U032 videoGetHead
(
    PCLASS pClass,
    VOID *pCreateParms
)
{
	RM_STATUS	status = RM_OK;
    U032 Head = 0;

    if (pCreateParms == NULL)
        return Head;
        
    switch (pClass->Type)
    {
        case NV10_VIDEO_OVERLAY:
        {
			NV07C_ALLOCATION_PARAMETERS Class07cParams;
			status = osCopyIn(pCreateParms, (U008*) &Class07cParams, sizeof (NV07C_ALLOCATION_PARAMETERS));
			if (status != RM_OK)
				return 0;

			Head = Class07cParams.logicalHeadId;
            break;
        }
        default:
            // for everything else, it's head 0
            break;
    }

    return Head;
}

static RM_STATUS videoCreateOverlayObj
(
    PHWINFO pDev,
    PCLASSOBJECT ClassObject,
    U032 Name,
    POBJECT *Object,
    VOID* pCreateParms
)
{
    RM_STATUS                   status;
    PCLASS                      pClass = ClassObject->Base.ThisClass;
    PVIDEO_OVERLAY_OBJECT        pOverlayObject;
    U008                        *bytePtr;
    int                         i;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: videoCreateOverlayObj\r\n");

    //
    // Allocate space for the overlay object.
    //
    status = osAllocMem((VOID **)Object, sizeof (VIDEO_OVERLAY_OBJECT));
    if (status)
        return (status);
        
    pOverlayObject = (PVIDEO_OVERLAY_OBJECT)*Object;

    //
    // Associate the head for the Video Scalar
    //
    pDev->Video.HalInfo.Head = videoGetHead(pClass, pCreateParms);
    if (pDev->Video.HalInfo.Head >= pDev->Dac.HalInfo.NumCrtcs)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: videoCreateOverlayObj: invalid logical head number ", pDev->Video.HalInfo.Head);
        return RM_ERR_INSUFFICIENT_RESOURCES;
    }


    if (IsNV11(pDev)) {
        AssocVideoScalar(pDev, pDev->Video.HalInfo.Head);
    }

    //
    // Initialize the new object structure.
    //
    bytePtr = (U008 *)pOverlayObject;
    for (i = 0; i < sizeof (VIDEO_OVERLAY_OBJECT); i++)
       *bytePtr++ = 0;

    //
    // Init common (software) state.
    //
    classInitCommonObject(*Object, ClassObject, Name);

    //
    // Let the HAL setup any chip-dependent resources.
    //
    status = nvHalVideoAlloc(pDev, pOverlayObject->CBase.ChID, pOverlayObject->CInstance, pClass->Type, &pOverlayObject->HalObject);
    if (status)
        return (status);

    //
    // Build a FIFO/Hash context for this object
    //
    status = fifoUpdateObjectInstance(pDev,
                                      &pOverlayObject->CBase, 
                                      pOverlayObject->CBase.ChID, 
                                      pOverlayObject->CInstance);
    if (status != RM_OK)
    {
        nvHalVideoFree(pDev, pOverlayObject->CBase.ChID, pOverlayObject->CInstance, pClass->Type, (VOID *)&pOverlayObject->HalObject);
        osFreeMem(*Object);
    }

    return (status);
}

static RM_STATUS videoDestroyOverlayObj
(
    PHWINFO pDev,
    POBJECT Object
)
{
    VIDEO_LUT_CURSOR_DAC_HAL_OBJECT VidLutCurDac;
    RM_STATUS                    status;
    PVIDEO_OVERLAY_OBJECT        pOverlayObject;
    U032                         Head = 0;  // TO DO: add parameter or extract from object

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: videoDestroyOverlayObj\r\n");

    VidLutCurDac.Head = Head;

    pOverlayObject = (PVIDEO_OVERLAY_OBJECT)Object;

    //
    // Let the HAL do it's cleanup thing.
    //
    nvHalVideoFree(pDev, pOverlayObject->CBase.ChID, pOverlayObject->CInstance, Object->ThisClass->Type, &pOverlayObject->HalObject);

    //
    // Video has been disabled, so refresh arb settings.
    //
    nvHalDacUpdateArbSettings(pDev, (VOID*) &VidLutCurDac);

    //
    // Clear active overlay object field is applicable.
    //
    if ((POBJECT) pDev->Video.HalInfo.ActiveVideoOverlayObject == (POBJECT) pOverlayObject)
    {
        pDev->Video.HalInfo.ActiveVideoOverlayObject = NULL;
        pDev->Video.HalInfo.Enabled = 0;
    }

    //
    // Delete the FIFO/Hash context for this object
    //
    status = fifoDeleteObjectInstance(pDev, &pOverlayObject->CBase, pOverlayObject->CBase.ChID); 
    if (status)
        return (status);

    //
    // Cleanup any pending VBlank callbacks if chip requires it.
    //
    if (!IsNV10orBetter(pDev)) {
        VBlankDeleteCallback(pDev, 0, &(pOverlayObject->Overlay[0].OverlayCallback));
        VBlankDeleteCallback(pDev, 0, &(pOverlayObject->Overlay[1].OverlayCallback));
    }

    return (osFreeMem(Object));
}

//-----------------------------------------------------------------------
// Video exception handling.
//-----------------------------------------------------------------------

//
// Service exception on class63 (NV_VIDEO_FROM_MEMORY) object transfer.
//
V032 videoFromMemService
(
    PHWINFO pDev,
    POBJECT pObject
)
{
    PVIDEOFROMMEMOBJECT pVidFromMemObj;
    VOID *pVidHalObj;
    U032 buffsPending;
    V032 intrStatus = 0;
    U032 i;

    //
    // Allow HAL chance to quiesce the video HW even if we don't
    // have an active overlay object.
    //
    pVidFromMemObj = (PVIDEOFROMMEMOBJECT)pObject;
    if (pVidFromMemObj)
        pVidHalObj = (VOID *)&pVidFromMemObj->HalObject;
    else
        pVidHalObj = NULL;

    //
    // Get buffer status first.
    //
    (void) nvHalVideoGetEventStatus(pDev, NV_VIDEO_FROM_MEMORY, pVidHalObj, &buffsPending, &intrStatus);

    //
    // If video isn't enabled, then this interrupt was spurious.
    //
    if (!pDev->Video.HalInfo.Enabled)
        return intrStatus;

    //
    // Handle notify requests if a buffer needs servicing.
    //
    for (i = 0; i < MAX_OVERLAY_BUFFERS; i++)
    {
        if (buffsPending & (1 << i))
        {
            // Hardware buffer 0 completed. 
            notifyFillNotifierArray( pDev, pVidFromMemObj->BufferObj.Base.NotifyXlate, 
                                     0, 
                                     0, 
                                     0, 
                                     NVFF8_NOTIFIERS_IMAGE_SCAN(i) );
                    
#ifdef DEBUG_TIMING            
            DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "Buffer 0 Completed. \n");
            vmmOutDebugString("0N");
#endif // DEBUG_TIMING                
                    
            //
            // Do any OS specified action related to this notification.
            //
            if (pVidFromMemObj->BBuffer[i].NotifyAction)
            {
                osNotifyEvent(pDev, (POBJECT)pVidFromMemObj,
                              NVFF8_NOTIFIERS_IMAGE_SCAN(i),
                              0, 
                              1, 
                              RM_OK, 
                              pVidFromMemObj->BBuffer[i].NotifyAction);
            }
        }
    }

    //
    // Now call into HAL to finish processing exception (in this case,
    // queue up next buffer for processing).
    //
    (void) nvHalVideoServiceEvent(pDev, NV_VIDEO_FROM_MEMORY, pVidHalObj, buffsPending, &intrStatus);

    return intrStatus;
}

//
// Service exceptions on class0XX (NVXX_VIDEO_OVERLAY) object transfers.
//
V032 videoOverlayService
(
    PHWINFO pDev,
    POBJECT pObject
)
{
    PVIDEO_OVERLAY_OBJECT pOverlayObject;
    VOID *pVidHalObj;
    U032 buffsPending, class;
    V032 intrStatus = 0;
    U032 i;

    //
    // Allow HAL chance to quiesce the video HW even if we don't
    // have an active overlay object.
    //
    pOverlayObject = (PVIDEO_OVERLAY_OBJECT)pObject;
    if (pOverlayObject)
    {
        class = pObject->ThisClass->Type;
        pVidHalObj = (VOID *)&pOverlayObject->HalObject;
    }
    else
    {
        //
        // If there is no active video object, the class will
        // be ignored anyway.   
        //
        class = 0;
        pVidHalObj = NULL;
    }

    //
    // Get buffer status first.
    //
    (void) nvHalVideoGetEventStatus(pDev, class, pVidHalObj, &buffsPending, &intrStatus);

    //
    // If video isn't enabled, then this interrupt was spurious.
    //
    if (!pDev->Video.HalInfo.Enabled)
        return intrStatus;

    //
    // Handle notify requests if a buffer needs servicing.
    //
    for (i = 0; i < MAX_OVERLAY_BUFFERS; i++)
    {
        if (buffsPending & (1 << i))
        {
            // Hardware buffer completed. 
            notifyFillNotifierArray(pDev, pOverlayObject->Common.Base.NotifyXlate, 
                                    0, // info32
                                    0, // info16
                                    RM_OK, 
                                    NV047_NOTIFIERS_SET_OVERLAY(i));

#ifdef DEBUG_TIMING            
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Buffer Completed. \n");
            vmmOutDebugString("1N");
#endif // DEBUG_TIMING

            //
            // Do any OS specified action related to this notification.
            //
            if (pOverlayObject->Overlay[i].NotifyAction)
            {
                osNotifyEvent(pDev, (POBJECT)pOverlayObject,
                              NV047_NOTIFIERS_SET_OVERLAY(i),
                              0, // Method
                              0, // Data
                              RM_OK, 
                              pOverlayObject->Overlay[i].NotifyAction);
            }
        }
    }

    //
    // Now call into HAL to finish processing exception (in this case,
    // queue up next buffer for processing).
    //
    (void) nvHalVideoServiceEvent(pDev, class, pVidHalObj, buffsPending, &intrStatus);

    return intrStatus;
}

//
// Update the POINT_OUT on behalf DDraw when panning the desktop
//
VOID videoUpdateWindowStart
(
    PHWINFO pDev,
    S016 deltaX,
    S016 deltaY
)
{
    PVIDEO_OVERLAY_OBJECT pOverlayObject;
    U016 pointoutX, pointoutY;
    U032 Data;
    RM_STATUS status;

    pOverlayObject = (PVIDEO_OVERLAY_OBJECT) pDev->Video.HalInfo.ActiveVideoOverlayObject;

    if (pOverlayObject->CBase.ThisClass->Type != NV10_VIDEO_OVERLAY)
        return;    // only applicable to class07a

    if (!deltaX && !deltaY)
        return;    // no delta to apply

    // We assume buffer0 and buffer1 are using the same POINT_OUT values
    RM_ASSERT(pOverlayObject->HalObject.Overlay[0].PointOut_x == 
              pOverlayObject->HalObject.Overlay[1].PointOut_x);
    RM_ASSERT(pOverlayObject->HalObject.Overlay[0].PointOut_y == 
              pOverlayObject->HalObject.Overlay[1].PointOut_y);

    // Apply delta x,y to class07a's current point out x,y
    pointoutX = (S016)pOverlayObject->HalObject.Overlay[0].PointOut_x + deltaX;
    pointoutY = (S016)pOverlayObject->HalObject.Overlay[0].PointOut_y + deltaY;

    Data = DRF_NUM(07A, _SET_OVERLAY_POINT_OUT, _X, pointoutX) |
           DRF_NUM(07A, _SET_OVERLAY_POINT_OUT, _Y, pointoutY);
    //
    // This method doesn't wait for the buffer to be released by the HW
    // and updates both POINT_OUT(0) and POINT_OUT(1) to the same value.
    //
    (VOID) nvHalVideoMethod(pDev,
                            NV10_VIDEO_OVERLAY,
                            (VOID *)&pOverlayObject->HalObject,
                            NV07A_SET_OVERLAY_POINT_OUT_A,
                            Data,
                            &status);
}

