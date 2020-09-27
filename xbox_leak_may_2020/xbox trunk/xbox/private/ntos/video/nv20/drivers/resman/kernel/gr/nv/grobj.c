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
* Module: GROBJ.C                                                           *
*   The graphics engine is managed in this module.  Graphics objects are    *
*   created and deleted here.  Graphics object methods are implemented      *
*   also contained here.                                                    *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <btree.h>
#include <state.h>
#include <class.h>
#include <vblank.h>
#include <fifo.h>
#include <dma.h>
#include <dac.h>
#include <gr.h>
#include <modular.h>   
#include <os.h>
#include <nv32.h>
#include "nvhw.h"
#include "smp.h"

//
// Degubbing macros.
//
#ifdef DEBUG
#define VALIDATE_GROBJ(go)  \
if (((POBJECT)go)->Name < 4096) \
{                           \
    GR_PRINTF((DBG_LEVEL_ERRORS, "NVRM: Invalid graphics object name!\n\r")); \
    DBG_BREAKPOINT();       \
}
#else
#define VALIDATE_GROBJ(go)
#endif //  DEBUG
//
// Externs.
//
// extern PGRAPHICSCHANNEL grTable;
//
// The canvas tree.
//
// PNODE grCanvasTree;

//
// Class lock macros.
//

#define GRCLASSLOCK_ISSET(pdev,chid,bit)    (((U032 *)(pDev->Graphics.ClassLocks))[chid] & (1 << bit))
#define GRCLASSLOCK_SET(pdev,chid,bit)      (((U032 *)(pDev->Graphics.ClassLocks))[chid] |= (1 << bit))
#define GRCLASSLOCK_CLEAR(pdev,chid,bit)    (((U032 *)(pDev->Graphics.ClassLocks))[chid] &= ~(1 << bit))

//---------------------------------------------------------------------------
//
//  Create/Delete graphics objects.
//
//---------------------------------------------------------------------------

//
// Ordinal to video switch.
//
RM_STATUS grCreateOrdinal
(
    PHWINFO      pDev,
    PCLASSOBJECT ClassObject,
    U032         UserName,
    POBJECT     *GrObject
)
{
    RM_STATUS status;
    U032      i;
    
    status = osAllocMem((VOID **)GrObject, sizeof(ORDINALOBJECT));
    if (status)
        return (status);
    ((PORDINALOBJECT)*GrObject)->Value               = 0;
    ((PORDINALOBJECT)*GrObject)->FanOut              = 0;
    for (i = 0; i < MAX_GRPATCH_FANOUT; i++)
        ((PORDINALOBJECT)*GrObject)->Switch[i]       = 0;
    //
    // Increment the number of ordinals (and thus double buffered apps) in
    // the system.
    //
    pDev->Framebuffer.FlipUsageCount &= 0x7FFFFFFF;
    pDev->Framebuffer.FlipUsageCount++;
    return (RM_OK);
}
RM_STATUS grDeleteOrdinal
(
    POBJECT GrObject
)
{
    return osFreeMem(GrObject);
}

//---------------------------------------------------------------------------
//
//  Canvas routines.
//
//---------------------------------------------------------------------------

VOID grInitCommon(
    PCOMMONOBJECT pCommon,
	U032 Device
)
{
    //
    // Load up the default settings for a new common hardware object
    //
    pCommon->Valid         = FALSE;    
    pCommon->NotifyPending = FALSE;
    pCommon->Instance      = 0;
    pCommon->NotifyObject  = NULL;
    pCommon->Dma0Object    = NULL;
    pCommon->Dma1Object    = NULL;
    pCommon->Next          = NULL;
}

VOID grInitRenderCommon(
    PRENDERCOMMON pRenderCommon
)
{
    //
    // Load up the default settings for a new common render object
    //
    pRenderCommon->Xlate        = NULL;

    //
    // Default out the full patch context
    //
    pRenderCommon->PatchContext.ColorKey    = NULL;
    pRenderCommon->PatchContext.Pattern     = NULL;
    pRenderCommon->PatchContext.Clip        = NULL;
    pRenderCommon->PatchContext.Rop         = NULL;
    pRenderCommon->PatchContext.Beta1       = NULL;
    pRenderCommon->PatchContext.Beta4       = NULL;
    pRenderCommon->PatchContext.Surface     = NULL;
    pRenderCommon->PatchContext.SurfaceSource = NULL;
    pRenderCommon->PatchContext.Operation   = NV_PGRAPH_CTX_SWITCH1_PATCH_CONFIG_SRCCOPY_AND;
    
}

//
// grIsRenderObject
//
// This routine returns TRUE if the given class is a rendering
// object, FALSE otherwise.
//
static BOOL
grIsRenderObject(U032 class)
{
    switch (class)
    {
        case NV1_BETA_SOLID:
        case NV1_IMAGE_SOLID:
        case NV1_IMAGE_PATTERN:
        case NV1_IMAGE_BLACK_RECTANGLE:
        case NV3_CONTEXT_ROP:
        case NV3_CONTEXT_SURFACE_0:
        case NV3_CONTEXT_SURFACE_1:
        case NV3_CONTEXT_SURFACE_2:
        case NV3_CONTEXT_SURFACE_3:
        case NV4_CONTEXT_SURFACES_2D:
        case NV4_CONTEXT_PATTERN:
        case NV4_CONTEXT_SURFACE_SWIZZLED:
        case NV4_CONTEXT_SURFACES_ARGB_ZS:
        case NV4_CONTEXT_BETA:
        case NV10_CONTEXT_SURFACES_2D:
        case NV10_CONTEXT_SURFACES_3D:
            return FALSE;
        //default:
            // default action is to fall thru
    }
    return TRUE;
}

//
// grCreateObj
//
// Generic graphics engine object creation routine.
//
RM_STATUS
grCreateObj(VOID* pDevHandle, PCLASSOBJECT ClassObject, U032 Name, POBJECT *Object, VOID* pCreateParms)
{
    PHWINFO pDev = (PHWINFO) pDevHandle;
    RM_STATUS status = RM_OK;
    PCOMMONOBJECT pCommonObject;
    PCLASS pClass = ClassObject->Base.ThisClass;
    U032 lockBit;
    U032 instSize, instAlign, size;
    U032 allocFlags;
    BOOL isRenderObj;

    GR_PRINTF((DBG_LEVEL_INFO, "NVRM: grCreateObj class: 0x%x\n", pClass->Type));

    //
    // copy in any allocation parameters from user space
    //

    allocFlags = 0;
    if (pCreateParms)
    {
        NV_GR_ALLOCATION_PARAMETERS grParams;
        status = osCopyIn(pCreateParms, (U008*) &grParams, sizeof(grParams));
        if (status != RM_OK)
            return status;
        allocFlags = grParams.flags;
    }

    //
    // Check per-channel allocation restrictions if applicable.
    //
    lockBit = CLASSDECL_FIELD(*pClass->ClassDecl, _LOCK);
    if (lockBit != NO_LOCK) {
        if (GRCLASSLOCK_ISSET(pDev, ClassObject->Base.ChID, lockBit))
        {
            GR_PRINTF((DBG_LEVEL_INFO, "NVRM: class already exists in channel: 0x%x\n", ClassObject->Base.ChID));
            return (RM_ERR_ILLEGAL_OBJECT);
        }
        GRCLASSLOCK_SET(pDev, ClassObject->Base.ChID, lockBit);
    }

    //
    // Determine if this class is a rendering class (and therefore
    // represented by a RENDERCOMMONOBJECT structure).
    //
    isRenderObj = grIsRenderObject(pClass->Type);

    //
    // Allocate space for object.
    //
    size = (isRenderObj) ? sizeof (RENDERCOMMONOBJECT) : sizeof (COMMONOBJECT);
    status = osAllocMem((VOID **)Object, size);
    if (status)
    {
        GRCLASSLOCK_CLEAR(pDev, ClassObject->Base.ChID, lockBit);
        return (status);
    }

    //
    // A nice cast to make the code more readable.
    //
    pCommonObject = (PCOMMONOBJECT)*Object;

    //
    // Init common (software) state.
    //
    classInitCommonObject(*Object, ClassObject, Name);

    //
    // Init render object data.
    //
    if (isRenderObj)
        grInitRenderCommon(&((PRENDERCOMMONOBJECT)pCommonObject)->RenderCommon);

    //
    // Allocate instance memory.  The amount allocated is based
    // on two values:  the default size from the engine declaration
    // entry plus any additional space that might be specified via
    // the class instance memory alloc entry.
    //
    // Note that graphics objects currently only use the default, but
    // on NV20 the Kelvin class is going to require more than a
    // single paragraph of instance memory.
    //
    // Also factor in any alignment requirements if necessary.
    //
    instSize = ENGDECL_FIELD(*pClass->EngineDecl, _OBJINSTSZ);
    instAlign = 0;
    if (pClass->ClassInstMalloc) {
        instSize += CLASSINSTMALLOC_FIELD(*pClass->ClassInstMalloc, _SIZE);
        instAlign = CLASSINSTMALLOC_FIELD(*pClass->ClassInstMalloc, _ALIGN);
    }

    if (instAlign)
        status = fbAllocInstMemAlign(pDev, &(pCommonObject->Instance), instSize, instAlign);
    else
        status = fbAllocInstMem(pDev, &(pCommonObject->Instance), instSize);

    if (status)
    {
        if (lockBit != NO_LOCK)
            GRCLASSLOCK_CLEAR(pDev, ClassObject->Base.ChID, lockBit);
        osFreeMem(*Object);
        return (status);
    }

    //
    // Let the HAL setup instance memory for the object.
    //
    status = nvHalGrAlloc(pDev, pCommonObject->Base.ChID, pCommonObject->Instance, pClass->Type, allocFlags);
    if (status == RM_OK)
    {
        //
        // Override any HAL initialization here.
        //

        //
        // Build a FIFO/Hash context for this object
        //
        status = fifoUpdateObjectInstance(pDev,
                                          &pCommonObject->Base, 
                                          pCommonObject->Base.ChID, 
                                          pCommonObject->Instance);
    }

    if (status != RM_OK)
    {
        fbFreeInstMem(pDev, pCommonObject->Instance, instSize);
        osFreeMem(*Object);
        GRCLASSLOCK_CLEAR(pDev, ClassObject->Base.ChID, lockBit);
        return (status);
    }
    
    //
    // Object allocation is a possible power mgmt trigger.  Let's go check.
    //
    status = mcPowerStateTrigger(pDev);     // no error recover required

    return (RM_OK);
}

RM_STATUS
grDestroyObj(VOID* pDevHandle, POBJECT Object)
{
    PHWINFO pDev = (PHWINFO) pDevHandle;
    RM_STATUS               status = RM_OK;
    PCOMMONOBJECT           pCommonObject;
    PCLASS                  pClass;
    U032                    lockBit;
    U032                    instSize;

    GR_PRINTF((DBG_LEVEL_INFO, "NVRM: grDestroyObj\r\n"));

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
    // Let the HAL do it's cleanup thing.
    //
    pClass = Object->ThisClass;
    nvHalGrFree(pDev, pCommonObject->Base.ChID, pCommonObject->Instance);

    //
    // Invalidate any objects attached to this context
    //
    // !!KJK Only need to do this if we link the contexts to the objects
    //
    // Delete the instance data structure for this object
    //
    instSize = ENGDECL_FIELD(*pClass->EngineDecl, _OBJINSTSZ);
    if (pClass->ClassInstMalloc) {
        instSize += CLASSINSTMALLOC_FIELD(*pClass->ClassInstMalloc, _SIZE);
    }
    
    status = fbFreeInstMem(pDev, pCommonObject->Instance, instSize);
    if (status)
        return (status);
        
    pCommonObject->Instance = 0;

    //
    // Delete the FIFO/Hash context for this object
    //
    status = fifoDeleteObjectInstance(pDev, &pCommonObject->Base, pCommonObject->Base.ChID); 
    if (status)
        return (status);
  
    //
    // Clear per-channel allocation restrictions if applicable.
    //
    lockBit = CLASSDECL_FIELD(*pClass->ClassDecl, _LOCK);
    if (lockBit != NO_LOCK) {
        //ASSERT(pDev->Graphics.ClassLocks[pCommonObject->Base.ChID] & (1 << lockBit))
        GRCLASSLOCK_CLEAR(pDev, pCommonObject->Base.ChID, lockBit);
    }
      
    //
    // Object deallocation is a possible power mgmt trigger.  Let's go check.
    //
    status = mcPowerStateTrigger(pDev);  // no error recovery required

    //
    // And finally delete the object itself.
    //    
    return (osFreeMem(Object));
}

