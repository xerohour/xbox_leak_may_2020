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

/********************************* DMA Manager *****************************\
*                                                                           *
* Module: DMA.C                                                             *
*   The DMA engine is managed in this module.  All priviledged state and    *
*   object interaction is handled here.                                     *
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
#include <fifo.h>
#include <dma.h>
#include <gr.h>   
#include <fb.h>
#include <os.h>
#include <nv32.h>
#include "nvhw.h"
#include "smp.h"

RM_STATUS dmaCreate
(
    VOID*               pDevHandle,
    PCLASSOBJECT        ClassObject,
    U032                UserName,
    POBJECT             *NewObject,
    VOID*               pCreateParms
)
{
    PHWINFO     pDev = (PHWINFO) pDevHandle;
    RM_STATUS   status = RM_OK;
    PDMAOBJECT  DmaObject;

    status = osAllocMem((VOID **)&DmaObject, sizeof(DMAOBJECT));
    if (status)
        return (status);
    DmaObject->State      = 0;
    DmaObject->Valid      = FALSE;
    DmaObject->LockHandle = 0;

    //
    // Initialize HAL info data.
    //
    DmaObject->HalInfo.PteArray     = NULL;
    DmaObject->HalInfo.Instance     = NULL;
    DmaObject->HalInfo.classNum     = ClassObject->Base.ThisClass->Type;
    DmaObject->HalInfo.MemoryClass  = 0;
    DmaObject->HalInfo.Flags        = 0;

    //
    // Add to dma context table.
    //
    DmaObject->Next       = pDev->DBDmaList;
    pDev->DBDmaList       = DmaObject;
    *NewObject            = (POBJECT)DmaObject;
    
#ifdef RM20
    //
    // Initialize the DMA object's client handle
    //
    CliGetClientHandle(&DmaObject->ClientHandle);
#endif    
    //
    // Get the address space associated with this DMA object
    //
    // Assume this is a system memory context first.  We may
    // reassign it later when we're looking at the physical
    // page locations.
    //
    DmaObject->HalInfo.AddressSpace = ADDR_SYSMEM;
    
#ifdef LEGACY_ARCH    
    //
    // Since we're about to put this new (unallocated) dma context
    // in the hash table, we need to point it at some PTE.  Point it
    // at an empty one so if anyone attempts to use this context
    // before its allocated, we'll get an error.  The PTE will be
    // exchanged with a valid entry once the dma context gets
    // allocated (locked).
    //
    // This step will disappear once we have the all-in-one
    // NvAllocDma() routine.
    //
    switch (ClassObject->Base.ThisClass->Type)  //KJK
    {
        case NV1_CONTEXT_DMA_TO_MEMORY:     
        case NV_CONTEXT_DMA_TO_MEMORY:
            DmaObject->HalInfo.Instance = pDev->Dma.DmaToMemEmptyInstance;
            break;
        case NV1_CONTEXT_DMA_FROM_MEMORY:     
        case NV_CONTEXT_DMA_FROM_MEMORY:
            DmaObject->HalInfo.Instance = pDev->Dma.DmaFromMemEmptyInstance;
            break;
        case NV1_CONTEXT_DMA_IN_MEMORY:     
        case NV_CONTEXT_DMA_IN_MEMORY:
            DmaObject->HalInfo.Instance = pDev->Dma.DmaInMemEmptyInstance;
            break;
    }            
#endif // LEGACY_ARCH
        
    DmaObject->Base.Name  = UserName;
    DmaObject->Base.ChID  = ClassObject->Base.ChID;
    
#ifdef LEGACY_ARCH    
    //
    // Build a FIFO/Hash context for this object
    //
    status = fifoUpdateObjectInstance(pDev,
                                      &DmaObject->Base, 
                                      DmaObject->Base.ChID, 
                                      DmaObject->HalInfo.Instance);

    if (status)
        return (status);                               
#endif
    
    return (status);
    
}
//
// Delete NV_CONTEXT_DMA_??_MEMORY object.
//
RM_STATUS dmaDelete
(
    VOID*   pDevHandle,
    POBJECT DelObject
)
{
    PHWINFO     pDev = (PHWINFO) pDevHandle;
    RM_STATUS   status;
    PDMAOBJECT  DmaObject;
    PDMAOBJECT  DmaDelete;
    
    DmaObject = (PDMAOBJECT)DelObject;
    status = RM_OK;
    
#ifdef LEGACY_ARCH
    //
    // If there are objects that are currently using this context, we are not allowed
    // to remove the dma context from the chip (hash).  We can unlock it (leaving it 
    // at a NULL PTE), but we can't just yank the whole thing.
    //
    // We probably need to scan all of instance memory and all on-chip context caches
    // and clear out any usage of this object.
    //
    // KJK How do we handle this correctly????  For now, just yank it.
    //
    status = fifoDeleteObjectInstance(pDev, DelObject, DelObject->ChID);
    if (status)
        return (status);
#endif        
    //
    // NOTE: In the new architecture, it is assumed that this object has 
    //       been removed from the hash table (if necessary).
    
    //
    // Are we locked somewhere?
    //
    if (DmaObject->Valid)
    {
        //
        // Free all memory allocated to this object.
        //
        if (DmaObject->HalInfo.PteArray)
        {
            //DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: DMA buffer resoures still allocated at delete time!\n\r");
            //DBG_BREAKPOINT();
            dmaFreeInstance(pDev, DmaObject);
            osUnlockUserMem(
						    pDev,
						    DmaObject->Base.ChID,
                            (VOID*)DmaObject->HalInfo.BufferBase,
                            DmaObject->DescAddr,
                            DmaObject->HalInfo.AddressSpace,
                            DmaObject->DescLimit,
                            DmaObject->HalInfo.PteCount,
                            DmaObject->LockHandle,
                            (DmaObject->Base.Class->Type == NV1_CONTEXT_DMA_TO_MEMORY) ||
                            (DmaObject->Base.Class->Type == NV_CONTEXT_DMA_TO_MEMORY) ? TRUE : FALSE);
            osFreeMem(DmaObject->HalInfo.PteArray);
            DmaObject->HalInfo.PteArray = NULL;
        }
        if (pDev->DBDmaList == NULL)        
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: DMA list NULL in DMADELETE!\n\n");
            DBG_BREAKPOINT();
        }

        //
        // Detach from DMA list.
        //
        if (pDev->DBDmaList == DmaObject)
            pDev->DBDmaList = DmaObject->Next;
        else
        {
            DmaDelete = pDev->DBDmaList;
            while (DmaDelete->Next && DmaDelete->Next != DmaObject)
            {
                DBG_VAL_PTR(DmaDelete);
                DmaDelete = DmaDelete->Next;
            }
            if (DmaDelete->Next)
                DmaDelete->Next = DmaObject->Next;
        }    
        status = osFreeMem(DmaObject);
    }
    
    return (status);
}

//
// Find a specific DMA object based upon its instance
//
RM_STATUS dmaContextInstanceToObject
(
    PHWINFO  pDev,
    U032     ChID,
    U032     Instance,
    PDMAOBJECT *DmaObject
)
{
    PDMAOBJECT TempDmaList;
    U032 hClient;
    RM_STATUS status;
    
    //
    // Are there any dma objects available?
    //
    if (!pDev->DBDmaList)
    {    
        *DmaObject = NULL;    
        return (RM_ERR_BAD_OBJECT);
    }        
        
#ifdef RM20  
    // First we must establish a client via the Channel ID.
    status = CliGetClientHandleFromChID(pDev, ChID, &hClient);
    if (status)
    {
        *DmaObject = NULL;    
        return (status);
    }
      
    //
    // Walk the list looking for this object by matching the client and the instance
    //
    TempDmaList = pDev->DBDmaList;
    while (TempDmaList)        
    {
        if ((TempDmaList->ClientHandle == hClient) && (TempDmaList->HalInfo.Instance == Instance))
        {
            //
            // Found it!
            //
            *DmaObject = TempDmaList;
            return (RM_OK);
        }
        TempDmaList = TempDmaList->Next;
    }
#else
    //
    // Walk the list looking for this object by matching the channel and the instance
    //
    TempDmaList = pDev->DBDmaList;
    while (TempDmaList)        
    {
        if ((TempDmaList->Base.ChID == ChID) && (TempDmaList->HalInfo.Instance == Instance))
        {
            //
            // Found it!
            //
            *DmaObject = TempDmaList;
            return (RM_OK);
        }
        TempDmaList = TempDmaList->Next;
    }
#endif
    *DmaObject = NULL;    
    return (RM_ERR_BAD_OBJECT);        
}

//
// Validate DMA object name.
//
RM_STATUS dmaValidateObjectName
(
    PHWINFO     pDev,
    V032        ObjectName,
    U032        ChID,
    PDMAOBJECT *DmaObject
)
{
    U032 hClient;
    RM_STATUS status;

    //
    // Check for NULL object.
    //
    if (ObjectName == NV1_NULL_OBJECT)
    {
        *DmaObject = NULL;
        return (RM_WARN_NULL_OBJECT);
    }
    status = fifoSearchObject(pDev, ObjectName, ChID, (POBJECT *)DmaObject);
    if (status)
    {
#ifdef RM20
        // attempt to find the object in the global DMA list
        // the client handle is also needed for this lookup
        status = CliGetClientHandleFromChID(pDev, ChID, &hClient);
        if (status)
        {
            *DmaObject = NULL;
            return (status);
        }
        status = dmaFindContext(pDev, ObjectName, hClient, DmaObject);
        if (status)
        {
            *DmaObject = NULL;
            return (status);
        }
        
#ifdef LEGACY_ARCH
        // CWJ: DMA objects don't exist in the FIFO tree with the new arch.
        // add the DMA context to the FIFO's object tree if it is unused
        if ((*DmaObject)->Base.ChID == INVALID_CHID)
        {
            (*DmaObject)->Base.ChID = ChID;
            status = fifoAddObject(pDev, (POBJECT)*DmaObject);
            if (status)
            {
                return (status);
            }
        }
        else
        {
            // return RM_ERR_BAD_DMA_SPECIFIER;
        }
#endif
#else
        // not finding the object in the tree is an error
        *DmaObject = NULL;
        return (status);
#endif // RM20
    }
    if (((*DmaObject)->Base.Class->Type != NV1_CONTEXT_DMA_FROM_MEMORY)
    &&  ((*DmaObject)->Base.Class->Type != NV1_CONTEXT_DMA_TO_MEMORY)
    &&  ((*DmaObject)->Base.Class->Type != NV1_CONTEXT_DMA_IN_MEMORY)
    &&  ((*DmaObject)->Base.Class->Type != NV_CONTEXT_DMA_FROM_MEMORY)
    &&  ((*DmaObject)->Base.Class->Type != NV_CONTEXT_DMA_TO_MEMORY)
    &&  ((*DmaObject)->Base.Class->Type != NV_CONTEXT_DMA_IN_MEMORY))
        return (RM_ERR_OBJECT_TYPE_MISMATCH);
    return (RM_OK);
}

#ifdef RM20
// find a DMA context in the list
RM_STATUS dmaFindContext(PHWINFO pDev, U032 id, U032 hClient, PDMAOBJECT *ppDmaObject)
{
    for (*ppDmaObject = pDev->DBDmaList; *ppDmaObject; *ppDmaObject = (*ppDmaObject)->Next)
        if (((*ppDmaObject)->Base.Name == id) &&
                ((*ppDmaObject)->ClientHandle == hClient))
            break;

    return (*ppDmaObject) ? RM_OK : 1;

} // end of dmaFindContext()
#endif // RM20

RM_STATUS dmaValidateXlate
(
    PDMAOBJECT DmaObject,
    U032       Start,
    U032       Length
)
{
    if (DmaObject == NULL)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Invalid DMA context in dmaValidateXlate\n\r");
        DBG_BREAKPOINT();
        return (RM_ERR_INVALID_XLATE);
    }
    DBG_VAL_PTR(DmaObject);
#if DEBUG
    if ((DmaObject->Base.Class->Type != NV1_CONTEXT_DMA_TO_MEMORY)
    &&  (DmaObject->Base.Class->Type != NV1_CONTEXT_DMA_FROM_MEMORY)
    &&  (DmaObject->Base.Class->Type != NV1_CONTEXT_DMA_IN_MEMORY)
    &&  (DmaObject->Base.Class->Type != NV_CONTEXT_DMA_TO_MEMORY)
    &&  (DmaObject->Base.Class->Type != NV_CONTEXT_DMA_FROM_MEMORY)
    &&  (DmaObject->Base.Class->Type != NV_CONTEXT_DMA_IN_MEMORY))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Bad type in dmaValidateXlate\n\r");
        DBG_BREAKPOINT();
        return (RM_ERROR);
    }
#endif // DEBUG
    if (!DmaObject->Valid)
        return (RM_ERR_INVALID_XLATE);
    if ((Start + Length - 1) > DmaObject->DescLimit)
        return (RM_ERR_INVALID_START_LENGTH);
    return (RM_OK);
}

//
// Allocate and lock memory associated with this DMA object
//
RM_STATUS dmaAllocate
(
    PHWINFO    pDev,
    PDMAOBJECT DmaObject
)
{
    RM_STATUS status;
    
    //DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: DMA Allocate\n\r");

    DBG_VAL_PTR(DmaObject);
    //
    // Make sure its valid.
    //
    if (!DmaObject->Valid)
        return (RM_ERR_INVALID_XLATE);
    //
    // Get memory translation.
    //
    if (!DmaObject->HalInfo.PteArray)
    {
        //
        // Save off class of this dma context object.
        //
        DmaObject->HalInfo.classNum      = DmaObject->Base.ThisClass->Type;

        //
        // These members are for future use to make sure we pass
        // any hardware error checking involving new context dma
        // class parameters.
        // 
        DmaObject->HalInfo.Flags      = 0;
        DmaObject->HalInfo.MemoryClass = 0;

        //
        // Calculate page table values.
        //    
        DmaObject->HalInfo.BufferSize = DmaObject->DescLimit  + 1;
        osCalculatePteAdjust(pDev, DmaObject->HalInfo.AddressSpace, DmaObject->DescAddr, &DmaObject->HalInfo.PteAdjust);
        DmaObject->HalInfo.PteLimit   = DmaObject->DescLimit; //DmaObject->BufferSize + DmaObject->PteAdjust;
        DmaObject->HalInfo.PteCount   =
            (DmaObject->HalInfo.PteLimit + DmaObject->HalInfo.PteAdjust + RM_PAGE_SIZE - 1) >> RM_PAGE_SHIFT;
        status = osAllocMem((VOID **)&(DmaObject->HalInfo.PteArray), DmaObject->HalInfo.PteCount * 4);
        if (status)
        {
            DmaObject->HalInfo.BufferBase = NULL;
            DmaObject->HalInfo.PteArray   = NULL;
            return (RM_ERR_NO_FREE_MEM);
        }

        //
        // Lock and map DMA buffer.
        //    
        if (osLockUserMem(
						  pDev,
						  DmaObject->Base.ChID,
                          DmaObject->DescSelector,
                          DmaObject->DescOffset,
                          DmaObject->DescAddr,
                          &(DmaObject->HalInfo.AddressSpace),
                          DmaObject->DescLimit,
                          DmaObject->HalInfo.PteCount,
                          DmaObject->HalInfo.PteArray,
                          (VOID**)&(DmaObject->HalInfo.BufferBase),
                          &(DmaObject->LockHandle)) != RM_OK)
        {
            osFreeMem(DmaObject->HalInfo.PteArray);
            DmaObject->HalInfo.BufferBase = NULL;
            DmaObject->HalInfo.PteArray   = NULL;
            return (RM_ERR_DMA_MEM_NOT_LOCKED);
        }
        //
        // Get DMA instance for page table mapping.
        //
        status = dmaAllocInstance(pDev, DmaObject);
        if (status)
        {
            DmaObject->Valid = FALSE;
            osUnlockUserMem(
							pDev,
							DmaObject->Base.ChID,
                            (VOID*)DmaObject->HalInfo.BufferBase,
                            DmaObject->DescAddr,
                            DmaObject->HalInfo.AddressSpace,
                            DmaObject->DescLimit,
                            DmaObject->HalInfo.PteCount,
                            DmaObject->LockHandle,
                            (DmaObject->Base.Class->Type == NV1_CONTEXT_DMA_TO_MEMORY) ||
                            (DmaObject->Base.Class->Type == NV_CONTEXT_DMA_TO_MEMORY) ? TRUE : FALSE);
            osFreeMem(DmaObject->HalInfo.PteArray);
            DmaObject->HalInfo.BufferBase = NULL;
            DmaObject->HalInfo.PteArray   = NULL;
            return (status);
        }

        //
        // Call into HAL to handle h/w-dependent initialization.
        //
        status = nvHalDmaAlloc(pDev, &DmaObject->HalInfo);
        if (status)
        {
            DmaObject->Valid = FALSE;
            osUnlockUserMem(
							pDev,
							DmaObject->Base.ChID,
                            (VOID*)DmaObject->HalInfo.BufferBase,
                            DmaObject->DescAddr,
                            DmaObject->HalInfo.AddressSpace,
                            DmaObject->DescLimit,
                            DmaObject->HalInfo.PteCount,
                            DmaObject->LockHandle,
                            (DmaObject->Base.Class->Type == NV1_CONTEXT_DMA_TO_MEMORY) ||
                            (DmaObject->Base.Class->Type == NV_CONTEXT_DMA_TO_MEMORY) ? TRUE : FALSE);
            osFreeMem(DmaObject->HalInfo.PteArray);
            DmaObject->HalInfo.BufferBase = NULL;
            DmaObject->HalInfo.PteArray   = NULL;
            return (RM_ERR_BAD_DMA_SPECIFIER);      // ???
        }
    }

    return (RM_OK);
}

RM_STATUS dmaDeallocate
(
    PHWINFO    pDev,
    PDMAOBJECT DmaObject
)
{
    PNODE             node;
    U032              ChID;
    U032              Instance;
    U032              EngTag;

    //DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Dma Dealloc\n\r");

    DBG_VAL_PTR(DmaObject);
    //
    // Check for resources to free up.
    //
    if (DmaObject->HalInfo.PteArray)
    {
        //
        // If there are objects that are currently using this context, we cannot allow any
        // modifications!!
        //
        for (ChID = 0 ; ChID < NUM_FIFOS ; ChID++)
        {
            if (pDev->DBfifoTable[ChID].InUse)
            {

                // 
                // Go find the minimum value (>0) and start there.  That way we're
                // sure to visit every node.
                //
                btreeEnumStart(0, &node, pDev->DBfifoTable[ChID].ObjectTree);

                while (node != NULL)
                {
                    //
                    // If this is a hardware object
                    //
                    EngTag = ENGDECL_FIELD(*((POBJECT)(node->Data))->Class->EngineDecl, _TAG);
                    if (EngTag == GR_ENGINE_TAG)
                    {
                        //
                        // Find the dma context field in this object's context instance
                        //
                        Instance = ((PCOMMONOBJECT)(node->Data))->Instance;
                        
                        //
                        // If it is currently being used by any object, don't allow
                        // any adjustments
                        //
                        if (Instance)
                        {
                            if (RAM_RD_ISF(Instance, _PRAMIN_CTX_1, _NOTIFY_INSTANCE) == DmaObject->HalInfo.Instance)
                                return(NVFFE_NOTIFICATION_STATUS_ERROR_STATE_IN_USE);
                            if (RAM_RD_ISF(Instance, _PRAMIN_CTX_2, _DMA_0_INSTANCE) == DmaObject->HalInfo.Instance)
                                return(NVFFE_NOTIFICATION_STATUS_ERROR_STATE_IN_USE);
                            if (RAM_RD_ISF(Instance, _PRAMIN_CTX_2, _DMA_1_INSTANCE) == DmaObject->HalInfo.Instance)
                                return(NVFFE_NOTIFICATION_STATUS_ERROR_STATE_IN_USE);
                        }                            

                    }

                    btreeEnumNext(&node, pDev->DBfifoTable[ChID].ObjectTree);
                }
            }
        }

        //
        // Now free the DMA object
        //
        dmaFreeInstance(pDev, DmaObject);
        osUnlockUserMem(
						pDev,
						DmaObject->Base.ChID,
                        (VOID*)DmaObject->HalInfo.BufferBase,
                        DmaObject->DescAddr,
                        DmaObject->HalInfo.AddressSpace,
                        DmaObject->DescLimit,
                        DmaObject->HalInfo.PteCount,
                        DmaObject->LockHandle,
                        (DmaObject->Base.Class->Type == NV1_CONTEXT_DMA_TO_MEMORY) ||
                        (DmaObject->Base.Class->Type == NV_CONTEXT_DMA_TO_MEMORY) ? TRUE : FALSE);
        osFreeMem(DmaObject->HalInfo.PteArray);
        DmaObject->HalInfo.BufferBase = NULL;
        DmaObject->HalInfo.PteArray   = NULL;
    }
    
#ifdef LEGACY_ARCH
    //
    // Point back to the NULL PTE
    //
    switch (DmaObject->Base.Class->Type)
    {
        case NV1_CONTEXT_DMA_TO_MEMORY:     
        case NV_CONTEXT_DMA_TO_MEMORY:
            DmaObject->HalInfo.Instance = pDev->Dma.DmaToMemEmptyInstance;
            break;
        case NV1_CONTEXT_DMA_FROM_MEMORY:     
        case NV_CONTEXT_DMA_FROM_MEMORY:
            DmaObject->HalInfo.Instance = pDev->Dma.DmaFromMemEmptyInstance;
            break;
        case NV1_CONTEXT_DMA_IN_MEMORY:     
        case NV_CONTEXT_DMA_IN_MEMORY:
            DmaObject->HalInfo.Instance = pDev->Dma.DmaInMemEmptyInstance;
            break;
    }            
#endif LEGACY_ARCH
    

    return (RM_OK);
}

//
// Return the mapped address of a DMA buffer.
// We need to special case an ADDR_FBMEM mapping, since RmAllocContextDma converted
// the BufferBase field to an FB offset. So to return the correct mapping, we use our
// RM mapping to the FB memory.
//
RM_STATUS dmaGetMappedAddress
(
    PHWINFO    pDev,
    PDMAOBJECT DmaObject,
    U032       Start,
    U032       Length,
    VOID     **Address
)
{
    RM_STATUS status;

    if ((DmaObject == NULL) || (!DmaObject->Valid))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Invalid DMA context in dmaGetMappedAddress\n\r");
        DBG_BREAKPOINT();
        return (RM_ERR_INVALID_XLATE);
    }
    DBG_VAL_PTR(DmaObject);
    status = dmaValidateXlate(DmaObject, Start, Length);
    if (status)
        return (status);
    if (!DmaObject->HalInfo.PteArray ||
        (!DmaObject->HalInfo.BufferBase && DmaObject->HalInfo.AddressSpace != ADDR_FBMEM))
        return (RM_ERR_DMA_MEM_NOT_LOCKED);
	if (DmaObject->HalInfo.AddressSpace == ADDR_FBMEM)
        *Address = (U008*)fbAddr + (SIZE_PTR)DmaObject->HalInfo.BufferBase + Start;      // BufferBase is an FB offset
    else
	    *Address = (U008*)DmaObject->HalInfo.BufferBase + Start;
    return (RM_OK);
}

//
// Allocate a DMA instance in offscreen memory.  This involves locking
// the physical pages in memory, getting the translation table, and
// adding the object to the instance list.
//
RM_STATUS dmaAllocInstance
(
    PHWINFO pDev,
    PDMAOBJECT DmaObject
)
{
    RM_STATUS    status;

    DBG_VAL_PTR(DmaObject);
    //
    // Only alloc valid buffers.
    //    
    if ((DmaObject == NULL) || (!DmaObject->Valid))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Invalid DMA context in dmaAllocInstance\n\r");
        DBG_BREAKPOINT();
        return (RM_ERR_INVALID_XLATE);
    }
    //
    // Allocate some instance memory.  Allocate an extra PTE so when the 
    // DMA engine prefetches, it won't fault on a last access to a page.
    //
    //
    status = nvHalDmaGetInstSize(pDev, &DmaObject->HalInfo);
    if (status)
        return (RM_ERR_BAD_DMA_SPECIFIER);      // ???
    status = fbAllocInstMem(pDev, &(DmaObject->HalInfo.Instance), DmaObject->HalInfo.NumUnits);
    if (status)
        return (status);
    
    //
    // Is this supposed to be a real AGP context?  We need to verify the addresses
    // fall within the GART space that DirectDraw has already told us about.  If not,
    // we must assume this is normal PCI memory and treat it as such.
    //
    // This check is not for the NV-specific AGP implementation.  That approach
    // uses our own GART.
    //
    // NOTE: This isn't needed anymore now that osGetAddressInfo does its own internal
    // scan for AGP memory.  But keep it around in case we need it back in the future.
    //
    /*
    if ((DmaObject->AddressSpace == ADDR_AGPMEM) && !pDev->Chip.NVAGPEnabled)
    {
        if (!((DmaObject->DescOffset >= pDev->Mapping.AGPLinearStart) 
            && (DmaObject->DescOffset <= (pDev->Mapping.AGPLinearStart + pDev->Mapping.AGPLimit))))
                // not really AGP memory
                DmaObject->AddressSpace = ADDR_SYSMEM;
    }
    */

    return (RM_OK);
}
//
// Free DAM instance memory associated with a DMA context and unlock the pages.
//
RM_STATUS dmaFreeInstance
(
    PHWINFO    pDev,
    PDMAOBJECT DmaObject
)
{
    DBG_VAL_PTR(DmaObject);
    if (DmaObject == NULL)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: NULL object in dmaFreeInstance!\n\r");
        return (RM_OK);
    }
    //
    // Free instance page table plus guard page.
    //
    fbFreeInstMem(pDev, DmaObject->HalInfo.Instance, DmaObject->HalInfo.NumUnits);
    DmaObject->HalInfo.Instance   = 0;

    return (RM_OK);    
}

RM_STATUS dmaRegisterToDevice
(
    PHWINFO pDev, 
    PCLI_DMA_INFO pDmaInfo, 
    PDMAOBJECT *ppDmaObject
)
{
    RM_STATUS rmStatus = RM_OK;
    PCLASS      DmaClass;
    CLASSOBJECT DmaClassObject;
    PDMAOBJECT  pDmaObject;

    rmStatus = classGetClass(&pDev->classDB, pDmaInfo->Access, &DmaClass);
    if (rmStatus)
        return rmStatus;

    // initialize parts of DmaClassObject for use in dmaCreate
    DmaClassObject.Base.ChID = INVALID_CHID;
    DmaClassObject.Base.ThisClass = DmaClass;
    rmStatus = dmaCreate((VOID*)pDev, &DmaClassObject, 0, (POBJECT *)&pDmaObject, NULL);

    if (rmStatus)
        return rmStatus;
    pDmaObject->Base.Name               = pDmaInfo->Handle;
    pDmaObject->Base.ChID               = INVALID_CHID;
    pDmaObject->Base.Class              = DmaClass;
    pDmaObject->Base.Subchannel         = 0;
    pDmaObject->Base.Next               = 0;
    pDmaObject->Base.NotifyXlate        = 0;
    pDmaObject->Base.NotifyAction       = 0;
    pDmaObject->Base.NotifyTrigger      = 0;
    pDmaObject->DescSelector            = pDmaInfo->DescSelector;
    pDmaObject->DescOffset              = pDmaInfo->DescOffset;
    pDmaObject->DescLimit               = pDmaInfo->DescLimit;
    pDmaObject->DescAddr                = pDmaInfo->DescAddr;
    pDmaObject->LockCount               = 0;
    pDmaObject->LockHandle              = pDmaInfo->LockHandle;
    pDmaObject->Valid                   = TRUE;
    pDmaObject->NotifyActionHandle      = 0;
    pDmaObject->HalInfo.BufferBase      = (VOID_PTR)pDmaInfo->BufferBase;
    pDmaObject->HalInfo.BufferSize      = pDmaInfo->BufferSize;
    pDmaObject->HalInfo.PteCount        = pDmaInfo->PteCount;

    pDmaObject->HalInfo.PteAdjust       = pDmaInfo->PteAdjust;
    pDmaObject->HalInfo.PteLimit        = pDmaInfo->PteLimit;
    pDmaObject->HalInfo.PteArray        = pDmaInfo->PteArray;
    pDmaObject->HalInfo.AddressSpace    = pDmaInfo->AddressSpace;

    // get DMA instance for page table mapping in the given device
    rmStatus = dmaAllocInstance(pDev, pDmaObject);
    if (rmStatus != RM_OK)
    {
        dmaDelete((VOID*)pDev, (POBJECT)pDmaObject);
        return rmStatus;
    }

    //
    // Call into HAL to handle h/w-dependent initialization.
    //
    if (nvHalDmaAlloc(pDev, &pDmaObject->HalInfo) != RM_OK)
    {
        return (RM_ERR_BAD_DMA_SPECIFIER);      // ???
    }

    // Success.
    *ppDmaObject = pDmaObject;
    return RM_OK;
}

RM_STATUS dmaUnregisterFromDevice
(
   PHWINFO     pDev,
   PDMAOBJECT  pDmaObject
)
{
    RM_STATUS rmStatus;

    //
    // Call into HAL to release h/w-dependent resources.
    //
    if (nvHalDmaFree(pDev, &pDmaObject->HalInfo) != RM_OK)
    {
        return (RM_ERR_BAD_DMA_SPECIFIER);      // ???
    }
    
    rmStatus = dmaFreeInstance(pDev, pDmaObject);
    if (rmStatus)
    {
        return rmStatus;
    }

    // delete DMA object for the device in the context of the device
    rmStatus = dmaDelete((VOID*)pDev, (POBJECT)pDmaObject);
    return rmStatus;
}

RM_STATUS dmaBindToChannel
(
    PHWINFO    pDev, 
    PDMAOBJECT pDmaObject, 
    U032       ChID
)
{
    RM_STATUS rmStatus = RM_OK;
    
    //
    // CWJ: don't add the DMA object to the pDev->DBfifoTable[ChID] in NV4. We don't
    // use the pDev->DBfifoTable[] for look-ups in NV4 and since there's a single
    // Node field it's unable to exist on more than a one fifoTable, just
    // skip it.
    //
    // rmStatus = fifoAddDmaObject(pDev, (POBJECT)pDmaObject, pFifoInfo->ChID);
    // if (rmStatus != RM_OK)
    // {
    //  return rmStatus;
    // }

    // update the hash table
    rmStatus = fifoUpdateObjectInstance(pDev,
                                        &pDmaObject->Base,
                                        ChID,
                                        pDmaObject->HalInfo.Instance);

    return rmStatus;
}

RM_STATUS dmaUnbindFromChannel
(
   PHWINFO    pDev,
   PDMAOBJECT pDmaObject,
   U032       ChID
)
{
    RM_STATUS rmStatus;
    
    // Delete this dma context object for this ChID from the hash table.
    rmStatus = fifoDeleteObjectInstance(pDev, (POBJECT)pDmaObject, ChID);
    return rmStatus;
}

//---------------------------------------------------------------------------
//
//  DMA object methods.
//
//---------------------------------------------------------------------------

//
// Validate and calculate new specifier parameters.
//
RM_STATUS mthdSetDmaSpecifier
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    PDMAOBJECT DmaObject;
    PCLASS pClass;
    RM_STATUS  status;
    
    //DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: mthdDmaSpecifier\n\r");

    DmaObject = (PDMAOBJECT)Object;
    pClass = Object->Class;

    //
    // Make sure the graphics engine is idle before we steal the DMA context
    //
    GR_IDLE();

    //
    // If already in use...
    //
    if (DmaObject->Valid)
    {
        //
        // Unallocate the previous lock
        //
        status = dmaDeallocate(pDev, DmaObject);
        if (status)
            return (status);
        //
        // And update the instance
        //
        status = fifoUpdateObjectInstance(pDev, 
                                          &DmaObject->Base, 
                                          DmaObject->Base.ChID, 
                                          DmaObject->HalInfo.Instance);
        if (status)
            return (status);                               
            
    }    
    //
    // Validate specifier.
    //
    DmaObject->Valid = FALSE;
    switch (Offset)
    {
        case 0x0300:
            DmaObject->State        = DMA_SET_DESC_0;
            // TO DO: this is a known truncation for IA64
            DmaObject->DescOffset   = (VOID*)Data;
            break;
        case 0x0304:
            if (DmaObject->State != DMA_SET_DESC_0)
                return (RM_ERR_METHOD_COUNT);
            DmaObject->State        = DMA_SET_DESC_1;
            DmaObject->DescSelector = Data;
            break;
        case 0x0308:
            if (DmaObject->State != DMA_SET_DESC_1)
                return (RM_ERR_METHOD_COUNT);
            DmaObject->State     = DMA_SET_DESC_2;
            DmaObject->DescLimit = Data;

            //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: SetDesc1 = ", Data);
            //
            // If the user sets a 0 limit on the DMA object, then
            // he is not using the buffer. Don't return an error
            // but also don't validate the buffer.
            //
            if (DmaObject->DescLimit != 0)
            {
               //
               // Validate DMA buffer address.
               //
               if (osGetAddressInfo(pDev,
                                    DmaObject->Base.ChID, 
                                    DmaObject->DescSelector,
                                    &(DmaObject->DescOffset),
                                    DmaObject->DescLimit,
                                    &(DmaObject->DescAddr), 
                                    &(DmaObject->HalInfo.AddressSpace)) != RM_OK)
                   return (RM_ERR_BAD_DMA_SPECIFIER);
                //
                // Now attempt to lock the context
                //                   
                DmaObject->Valid = TRUE;
                status = dmaAllocate(pDev, DmaObject);
                if (status)
                    return (status);
                //
                // And update the instance
                //
                status = fifoUpdateObjectInstance(pDev, 
                                                  &DmaObject->Base, 
                                                  DmaObject->Base.ChID, 
                                                  DmaObject->HalInfo.Instance);
                if (status)
                    return (status);                               
                                    
            }
    }
    return (RM_OK);
}

//---------------------------------------------------------------------------
//
//  DMA buffer object methods.
//
//---------------------------------------------------------------------------

RM_STATUS mthdSetBufferCtxDma
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    PBUFFEROBJECT BuffObject;
    U032          BuffNum;

    BuffObject = (PBUFFEROBJECT)Object;
    BuffNum    = (Offset - Method->Low) >> 2;
#ifdef DEBUG
    if (BuffNum > 1)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Invalid buffer number!", BuffNum);
        DBG_BREAKPOINT();
    }
#endif
    return(dmaSetBufferCtxDma(pDev, BuffObject, &BuffObject->Buffer[BuffNum], Offset, Data));
}

RM_STATUS dmaSetBufferCtxDma
(
    PHWINFO       pDev, 
    PBUFFEROBJECT BuffObject,
    PBUFFER       pBuffer,
    U032          Offset,
    V032          Data
)
{
    RM_STATUS     status;

    //
    // Check for busy buffer.
    //
/*    
    if (pBuffer->State != BUFFER_IDLE)
    {
        osError(&BuffObject->Base, Offset, Data, status);
        return (RM_ERR_DMA_IN_USE);
    }
*/    
    //
    // Check for a previous translation.
    //
    //if (pBuffer->Xlate)
    //    dmaDetach(pBuffer->Xlate);
    //
    // Check for NULL object.
    //
    if (Data == NV1_NULL_OBJECT)
    {
        pBuffer->Xlate = NULL;
        return (RM_OK);
    }
    //
    // Validate dma buffer.
    //
    status = dmaValidateObjectName(pDev, Data,
                                   BuffObject->Base.ChID,
                                   &pBuffer->Xlate);
    if (status)
    {
        osError(&BuffObject->Base, Offset, Data, status);
        return (status);
    }
    //
    // Set the method to be used for notification on this buffer
    //
    pBuffer->NotifyMethod = Offset;
    return (status);
}

RM_STATUS mthdSetBufferStart
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    U032          BuffNum;
    PBUFFEROBJECT BuffObject;

    BuffObject = (PBUFFEROBJECT)Object;
    BuffNum = (Offset - Method->Low) >> 2;
#ifdef DEBUG
    if (BuffNum > 1)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Invalid buffer number!", BuffNum);
        DBG_BREAKPOINT();
    }
#endif
    return(dmaSetBufferStart(BuffObject, &BuffObject->Buffer[BuffNum], Offset, Data));
}

RM_STATUS dmaSetBufferStart
(
    PBUFFEROBJECT BuffObject,
    PBUFFER       pBuffer,
    U032          Offset,
    V032          Data
)
{
    //
    // Check for busy buffer.
    //
/*    
    if (pBuffer->State != BUFFER_IDLE)
    {
        osError(&BuffObject->Base, Offset, Data, RM_ERR_DMA_IN_USE);
        return (RM_ERR_DMA_IN_USE);
    }
*/    
    pBuffer->Start = Data;
    return (RM_OK);
}

RM_STATUS mthdSetBufferLength
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    U032 BuffNum;
    PBUFFEROBJECT BuffObject;

    BuffObject = (PBUFFEROBJECT)Object;
    BuffNum = (Offset - Method->Low) >> 2;
#ifdef DEBUG
    if (BuffNum > 1)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Invalid buffer number!", BuffNum);
        DBG_BREAKPOINT();
    }
#endif
    return(dmaSetBufferLength(BuffObject, &BuffObject->Buffer[BuffNum], Offset, Data));
}

RM_STATUS dmaSetBufferLength
(
    PBUFFEROBJECT BuffObject,
    PBUFFER       pBuffer,
    U032          Offset,
    V032          Data
)
{
    //
    // Check for busy buffer.
    //
/*    
    if (pBuffer->State != BUFFER_IDLE)
    {
        osError(&BuffObject->Base, Offset, Data, RM_ERR_DMA_IN_USE);
        return (RM_ERR_DMA_IN_USE);
    }
*/    
    pBuffer->Length = Data;
    return (RM_OK);
}

RM_STATUS mthdSetBufferPitch
(
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    U032 BuffNum;
    PBUFFEROBJECT BuffObject;

    BuffObject = (PBUFFEROBJECT)Object;
    BuffNum = (Offset - Method->Low) >> 2;
#ifdef DEBUG
    if (BuffNum > 1)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Invalid buffer number!", BuffNum);
        DBG_BREAKPOINT();
    }
#endif
    return(dmaSetBufferPitch(BuffObject, &BuffObject->Buffer[BuffNum], Offset, Data));
}

RM_STATUS dmaSetBufferPitch
(
    PBUFFEROBJECT BuffObject,
    PBUFFER       pBuffer,
    U032          Offset,
    V032          Data
)
{
    //
    // Check for busy buffer.
    //
/*    
    if (pBuffer->State != BUFFER_IDLE)
    {
        osError(&BuffObject->Base, Offset, Data, RM_ERR_DMA_IN_USE);
        return (RM_ERR_DMA_IN_USE);
    }
*/    
    pBuffer->Pitch = Data;
    return (RM_OK);
}

RM_STATUS mthdSetSampleRate
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    ((PBUFFEROBJECT)Object)->Sample.Rate = Data;
    ((PBUFFEROBJECT)Object)->Sample.RateUpdate((PBUFFEROBJECT)Object);
    return (RM_OK);
}
RM_STATUS mthdSetSampleRateAdjust
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    ((PBUFFEROBJECT)Object)->Sample.RateAdjust = Data;
    ((PBUFFEROBJECT)Object)->Sample.RateUpdate((PBUFFEROBJECT)Object);
    return (RM_OK);
}
RM_STATUS mthdSetNumberChannels
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    ((PBUFFEROBJECT)Object)->Sample.NumChannels = Data;
    return (RM_OK);
}

//---------------------------------------------------------------------------
//
//  DMA service routines.
//
//---------------------------------------------------------------------------



