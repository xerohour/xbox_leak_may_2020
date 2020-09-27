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

/*************************** MP Engine Manager *****************************\
*                                                                           *
* Module: mpobj.c                                                           *
*   Media port engine objects are managed here.                             *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#include <nv_ref.h>
#include <nvrm.h>
#include <class.h>
#include <mp.h>
#include <i2c.h>
#include <smp.h>
#include "nvhw.h"

//
// Forwards.
//

// NV03_EXTERNAL_VIDEO_DECODER (class04d)
static RM_STATUS mpCreateDecoderObj(PHWINFO, PCLASSOBJECT, U032, POBJECT *);
static RM_STATUS mpDestroyDecoderObj(PHWINFO, POBJECT);

// NV03_EXTERNAL_VIDEO_DECOMPRESSOR (class04e)
static RM_STATUS mpCreateDecompressorObj(PHWINFO, PCLASSOBJECT, U032, POBJECT *);
static RM_STATUS mpDestroyDecompressorObj(PHWINFO, POBJECT);

// NV01_EXTERNAL_PARALLEL_BUS
static RM_STATUS mpCreateParallelBusObj(PHWINFO, PCLASSOBJECT, U032, POBJECT *);
static RM_STATUS mpDestroyParallelBusObj(PHWINFO, POBJECT);

//
// Class lock macros (MediaPort class allocations are per-device).
//
#define MPCLASSLOCK_ISSET(pdev,bit)    (pDev->MediaPort.ClassLocks & (1 << bit))
#define MPCLASSLOCK_SET(pdev,bit)      (pDev->MediaPort.ClassLocks |= (1 << bit))
#define MPCLASSLOCK_CLEAR(pdev,bit)    (pDev->MediaPort.ClassLocks &= ~(1 << bit))

//
// mpCreateObj
//
// Top-level create routine for mediaport classes.
//
RM_STATUS mpCreateObj
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

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: mpCreateObj\r\n");

    //
    // Check allocation restrictions.
    //
    lockBit = CLASSDECL_FIELD(*pClass->ClassDecl, _LOCK);
    if (lockBit != NO_LOCK)
    {
        if (MPCLASSLOCK_ISSET(pDev, lockBit))
            return (RM_ERR_ILLEGAL_OBJECT);
        MPCLASSLOCK_SET(pDev, lockBit);
    }

    //
    // Invoke class-specific create routine.    
    //
    switch (pClass->Type)
    {
        case NV03_EXTERNAL_VIDEO_DECODER:
            status = mpCreateDecoderObj(pDev, ClassObject, Name, Object);
            break;
        case NV03_EXTERNAL_VIDEO_DECOMPRESSOR:
            status = mpCreateDecompressorObj(pDev, ClassObject, Name, Object);
            break;
        case NV01_EXTERNAL_PARALLEL_BUS:
            status = mpCreateParallelBusObj(pDev, ClassObject, Name, Object);
            break;
        case NV03_EXTERNAL_MONITOR_BUS:
            status = class050Create(pDev, ClassObject, Name, Object);
            break;
        case NV04_EXTERNAL_SERIAL_BUS:
            status = class051Create(pDev, ClassObject, Name, Object);
            break;
        default:
            //
            // Should *never* happen because this create entry point was
            // already bound to this class type in class.c.
            //
            status = RM_ERR_ILLEGAL_OBJECT;
    }

    if (status != RM_OK)
    {
        if (lockBit != NO_LOCK)
            MPCLASSLOCK_CLEAR(pDev, lockBit);
        return (status);
    }

    return (RM_OK);
}

RM_STATUS mpDestroyObj
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

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: mpDestroyObj\r\n");

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
        // ASSERT(MPCLASSLOCK_ISSET(pDev, lockBit));
        MPCLASSLOCK_CLEAR(pDev, lockBit);
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
        case NV03_EXTERNAL_VIDEO_DECODER:
            status = mpDestroyDecoderObj(pDev, Object);
            break;
        case NV03_EXTERNAL_VIDEO_DECOMPRESSOR:
            status = mpDestroyDecompressorObj(pDev, Object);
            break;
        case NV01_EXTERNAL_PARALLEL_BUS:
            status = mpDestroyParallelBusObj(pDev, Object);
            break;
        case NV03_EXTERNAL_MONITOR_BUS:
            status = class050Destroy(pDev, Object);
            break;
        case NV04_EXTERNAL_SERIAL_BUS:
            status = class051Destroy(pDev, Object);
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
// Video decoder create/destroy routines.
//----------------------------------------------------------------------
static RM_STATUS mpCreateDecoderObj
(
    PHWINFO pDev,
    PCLASSOBJECT ClassObject,
    U032 Name,
    POBJECT *Object
)
{
    RM_STATUS                   status = RM_OK;
    PCLASS                      pClass = ClassObject->Base.ThisClass;
    PVIDEODECODEROBJECT         pDecoderObject;
    U008                        *bytePtr;
    int                         i;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: mpCreateDecoderObj\r\n");

    //
    // Allocate space for the overlay object.
    //
    status = osAllocMem((VOID **)Object, sizeof (VIDEODECODEROBJECT));
    if (status)
        return (status);
        
    pDecoderObject = (PVIDEODECODEROBJECT)*Object;

    //
    // Initialize the new object structure.
    //
    bytePtr = (U008 *)pDecoderObject;
    for (i = 0; i < sizeof (VIDEODECODEROBJECT); i++)
       *bytePtr++ = 0;

    //
    // Init common (software) state.
    //
    classInitCommonObject(*Object, ClassObject, Name);

    //
    // Let the HAL setup any chip-dependent resources.
    //
    status = nvHalMpAlloc(pDev, pDecoderObject->CBase.ChID, pDecoderObject->CInstance, pClass->Type, &pDecoderObject->HalObject);
    if (status)
        return (status);

    //
    // Initialize the event processing order.
    // We always start with VBI and buffer number 0. 
    // So assume the last event processed was an image and the
    // last buffer processed was 1.
    //
    pDecoderObject->LastEventProcessed = EVENT_PROCESSED_IMAGE;
    pDecoderObject->LastImageBufferProcessed = 1;
    pDecoderObject->LastVbiBufferProcessed = 1;

    //
    // Build a FIFO/Hash context for this object
    //
    status = fifoUpdateObjectInstance(pDev,
                                      &pDecoderObject->CBase, 
                                      pDecoderObject->CBase.ChID, 
                                      pDecoderObject->CInstance);
    if (status != RM_OK)
    {
        nvHalMpFree(pDev, pDecoderObject->CBase.ChID, pDecoderObject->CInstance, pClass->Type, (VOID *)&pDecoderObject->HalObject);
        osFreeMem(*Object);
    }

    //
    // Plug this object into the device structure
    //
    pDev->MediaPort.CurrentDecoder = (VOID_PTR)pDecoderObject;

    return (status);
}

static RM_STATUS mpDestroyDecoderObj
(
    PHWINFO pDev,
    POBJECT Object
)
{
    RM_STATUS                   status;
    PVIDEODECODEROBJECT         pDecoderObject;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: mpDestroyDecoderObj\r\n");

    pDecoderObject = (PVIDEODECODEROBJECT)Object;

    //
    // Let the HAL do it's cleanup thing.
    //
    nvHalMpFree(pDev, pDecoderObject->CBase.ChID, pDecoderObject->CInstance, Object->ThisClass->Type, &pDecoderObject->HalObject);

    //
    // Delete the FIFO/Hash context for this object
    //
    status = fifoDeleteObjectInstance(pDev, &pDecoderObject->CBase, pDecoderObject->CBase.ChID); 
    if (status)
        return (status);

    //
    // Remove this object from the device structure
    //
    pDev->MediaPort.CurrentDecoder = NULL;

    return (osFreeMem(Object));
}


//----------------------------------------------------------------------
// Video decompressor create/destroy routines.
//----------------------------------------------------------------------
static RM_STATUS mpCreateDecompressorObj
(
    PHWINFO pDev,
    PCLASSOBJECT ClassObject,
    U032 Name,
    POBJECT *Object
)
{
    RM_STATUS                   status = RM_OK;
    PCLASS                      pClass = ClassObject->Base.ThisClass;
    PVIDEODECOMPRESSOROBJECT    pDecompressorObject;
    U008                        *bytePtr;
    int                         i;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: mpCreateDecompressorObj\r\n");

    //
    // Allocate space for the overlay object.
    //
    status = osAllocMem((VOID **)Object, sizeof (VIDEODECOMPRESSOROBJECT));
    if (status)
        return (status);
        
    pDecompressorObject = (PVIDEODECOMPRESSOROBJECT)*Object;

    //
    // Initialize the new object structure.
    //
    bytePtr = (U008 *)pDecompressorObject;
    for (i = 0; i < sizeof (VIDEODECOMPRESSOROBJECT); i++)
       *bytePtr++ = 0;

    //
    // Init common (software) state.
    //
    classInitCommonObject(*Object, ClassObject, Name);

    //
    // Let the HAL setup any chip-dependent resources.
    //
    status = nvHalMpAlloc(pDev, pDecompressorObject->CBase.ChID, pDecompressorObject->CInstance, pClass->Type, &pDecompressorObject->HalObject);
    if (status)
        return (status);

    //
    // Build a FIFO/Hash context for this object
    //
    status = fifoUpdateObjectInstance(pDev,
                                      &pDecompressorObject->CBase, 
                                      pDecompressorObject->CBase.ChID, 
                                      pDecompressorObject->CInstance);
    if (status != RM_OK)
    {
        nvHalMpFree(pDev, pDecompressorObject->CBase.ChID, pDecompressorObject->CInstance, pClass->Type, (VOID *)&pDecompressorObject->HalObject);
        osFreeMem(*Object);
    }

    //
    // Plug this object into the device structure
    //
    pDev->MediaPort.CurrentDecompressor = (VOID_PTR)pDecompressorObject;

    return (status);
}

static RM_STATUS mpDestroyDecompressorObj
(
    PHWINFO pDev,
    POBJECT Object
)
{
    RM_STATUS                   status;
    PVIDEODECOMPRESSOROBJECT    pDecompressorObject;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: mpDestroyDecompressorObj\r\n");

    pDecompressorObject = (PVIDEODECOMPRESSOROBJECT)Object;

    //
    // Let the HAL do it's cleanup thing.
    //
    nvHalMpFree(pDev, pDecompressorObject->CBase.ChID, pDecompressorObject->CInstance, Object->ThisClass->Type, &pDecompressorObject->HalObject);

    //
    // Delete the FIFO/Hash context for this object
    //
    status = fifoDeleteObjectInstance(pDev, &pDecompressorObject->CBase, pDecompressorObject->CBase.ChID); 
    if (status)
        return (status);

    //
    // Remove this object from the device structure
    //
    pDev->MediaPort.CurrentDecompressor = NULL;

    return (osFreeMem(Object));
}

//----------------------------------------------------------------------
// Parallel bus create/destroy routines.
//----------------------------------------------------------------------
static RM_STATUS mpCreateParallelBusObj
(
    PHWINFO pDev,
    PCLASSOBJECT ClassObject,
    U032 Name,
    POBJECT *Object
)
{
    RM_STATUS                   status = RM_OK;
    PCLASS                      pClass = ClassObject->Base.ThisClass;
    PPARALLELBUSOBJECT          pParallelBusObject;
    U008                        *bytePtr;
    int                         i;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: mpCreateParallelBusObj\r\n");

    //
    // Allocate space for the overlay object.
    //
    status = osAllocMem((VOID **)Object, sizeof (PARALLELBUSOBJECT));
    if (status)
        return (status);
        
    pParallelBusObject = (PPARALLELBUSOBJECT)*Object;

    //
    // Initialize the new object structure.
    //
    bytePtr = (U008 *)pParallelBusObject;
    for (i = 0; i < sizeof (PARALLELBUSOBJECT); i++)
       *bytePtr++ = 0;

    //
    // Init common (software) state.
    //
    classInitCommonObject(*Object, ClassObject, Name);

    //
    // Let the HAL setup any chip-dependent resources.
    //
    status = nvHalMpAlloc(pDev, pParallelBusObject->CBase.ChID, pParallelBusObject->CInstance, pClass->Type, &pParallelBusObject->HalObject);
    if (status)
        return (status);

    //
    // Build a FIFO/Hash context for this object
    //
    status = fifoUpdateObjectInstance(pDev,
                                      &pParallelBusObject->CBase, 
                                      pParallelBusObject->CBase.ChID, 
                                      pParallelBusObject->CInstance);
    if (status != RM_OK)
    {
        nvHalMpFree(pDev, pParallelBusObject->CBase.ChID, pParallelBusObject->CInstance, pClass->Type, (VOID *)&pParallelBusObject->HalObject);
        osFreeMem(*Object);
    }

    //
    // Plug this object into the device structure
    //
    pDev->MediaPort.CurrentParallelBusObj = (VOID_PTR)pParallelBusObject;

    return (status);
}

static RM_STATUS mpDestroyParallelBusObj
(
    PHWINFO pDev,
    POBJECT Object
)
{
    RM_STATUS                   status;
    PPARALLELBUSOBJECT          pParallelBusObject;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: mpDestroyParallelBusObj\r\n");

    pParallelBusObject = (PPARALLELBUSOBJECT)Object;

    //
    // Let the HAL do it's cleanup thing.
    //
    nvHalMpFree(pDev, pParallelBusObject->CBase.ChID, pParallelBusObject->CInstance, Object->ThisClass->Type, &pParallelBusObject->HalObject);

    //
    // Delete the FIFO/Hash context for this object
    //
    status = fifoDeleteObjectInstance(pDev, &pParallelBusObject->CBase, pParallelBusObject->CBase.ChID); 
    if (status)
        return (status);

    //
    // Remove this object from the device structure
    //
    pDev->MediaPort.CurrentParallelBusObj = NULL;


    return (osFreeMem(Object));
}
