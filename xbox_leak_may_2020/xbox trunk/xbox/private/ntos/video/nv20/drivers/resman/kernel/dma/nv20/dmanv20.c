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

/****************************** DMA Manager ********************************\
*                                                                           *
* Module: dmanv20.c                                                         *
*   The NV20 HAL DMA engine implementation is provided in this module.      *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#include <nv_ref.h>
#include <nv20_ref.h>
#include <nvrm.h>
#include <nv20_hal.h>
#include "nvhw.h"
#include "nvhalcpp.h"

// forwards
RM_STATUS nvHalDmaControl_NV20(VOID *);
RM_STATUS nvHalDmaAlloc_NV20(VOID *);
RM_STATUS nvHalDmaFree_NV20(VOID *);
RM_STATUS nvHalDmaGetInstSize_NV20(VOID *);

//
// nvHalDmaControl
//
// Nothing to do here.
//
RM_STATUS
nvHalDmaControl_NV20(VOID *arg)
{
    PDMACONTROLARG_000 pDmaControlArg = (PDMACONTROLARG_000)arg;

    //
    // Verify interface revision.
    //
    if (pDmaControlArg->id != DMA_CONTROL_000)
        return (RM_ERR_VERSION_MISMATCH);

    switch (pDmaControlArg->cmd)
    {
        case DMA_CONTROL_UPDATE:
        case DMA_CONTROL_LOAD:
        case DMA_CONTROL_UNLOAD:
        case DMA_CONTROL_DESTROY:
        case DMA_CONTROL_INIT:
            break;
        default:
            return (RM_ERR_BAD_ARGUMENT);
    }

    return (RM_OK);
}

//
// nvHalDmaAlloc
//
// Initialize dma instance.
//
// For nv20, we have to workaround a bug in the scaled_image hw:
//
//  [a] The hw generates a limit violation exception if
//      the programmed transfer size is within 64bytes of
//      the surface context dma limit; in practice, this happens
//      with system memory contexts in ddraw where the context dma
//      maps the underlying image buffer byte for byte (as opposed
//      to vidmem and agp context dmas that span the entire memory
//      range).
//
//  [b] The hw reads an extra 64bytes beyond the programmed
//      transfer size.
//
// To workaround this bug, we'll do two things here:
//
//  [1] Pad the specified context dma limit to prevent [a]
//      for _all_ context dmas.
//
//  [2] Make sure there's a guard page for the system memory
//      context dma's so that there's real memory behind
//      the 64byte prefetch done by the hw (helps with [b]).
//
// For [2], we can handle the PCI context dmas in the RM by
// putting the pte for the first page of the memory at the
// end of pte table.  For NVM context dmas, we know that
// there will always be valid memory in place because instance
// memory will protect us should the prefetch cross the last byte of
// allocatable video memory.  For AGP context dmas, we are
// relying on the driver to always pad the allocations THEMSELVES!!!!
//
RM_STATUS
nvHalDmaAlloc_NV20(VOID *arg)
{
    PDMAALLOCARG_000 pDmaAllocArg = (PDMAALLOCARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PDMAHALOBJINFO pDmaHalObjInfo;
    PDMAHALINFO pDmaHalInfo;
    U032 adjust, i;

    //
    // Verify interface revision.
    //
    if (pDmaAllocArg->id != DMA_ALLOC_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pDmaAllocArg->pHalHwInfo;
    pDmaHalObjInfo = pDmaAllocArg->pHalObjInfo;
    pDmaHalInfo = pHalHwInfo->pDmaHalInfo;

    adjust = 0;
    switch (pDmaHalObjInfo->classNum)
    {
        case NV1_CONTEXT_DMA_TO_MEMORY:     
        case NV_CONTEXT_DMA_TO_MEMORY:
            adjust |= SF_NUM(_DMA, _CLASS, NV1_CONTEXT_DMA_TO_MEMORY);
            break;
        case NV1_CONTEXT_DMA_FROM_MEMORY:     
        case NV_CONTEXT_DMA_FROM_MEMORY:
            adjust |= SF_NUM(_DMA, _CLASS, NV1_CONTEXT_DMA_FROM_MEMORY);
            break;
        case NV1_CONTEXT_DMA_IN_MEMORY:     
        case NV_CONTEXT_DMA_IN_MEMORY:
            adjust |= SF_NUM(_DMA, _CLASS, NV1_CONTEXT_DMA_IN_MEMORY);
            break;
    }
    adjust |= SF_NUM(_DMA, _ADJUST, pDmaHalObjInfo->PteAdjust);
    if ((pDmaHalObjInfo->AddressSpace == ADDR_FBMEM) || (pDmaHalObjInfo->AddressSpace == ADDR_AGPMEM))
        adjust |= SF_DEF(_DMA, _PAGE_ENTRY, _LINEAR);
    adjust |= SF_DEF(_DMA, _PAGE_TABLE, _PRESENT);
    switch (pDmaHalObjInfo->AddressSpace)
    {
        case ADDR_SYSMEM:
            adjust |= SF_DEF(_DMA, _TARGET_NODE, _PCI);
            break;
        case ADDR_AGPMEM:
#ifdef IKOS
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: AGP on IKOS!!!!\n");
            DBG_BREAKPOINT();
#endif
            adjust |= SF_DEF(_DMA, _TARGET_NODE, _AGP);
            break;
        case ADDR_FBMEM:
            adjust |= SF_DEF(_DMA, _TARGET_NODE, _NVM);
            break;
        default:
#ifdef DEBUG
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Invalid address space type\n\r");
            DBG_BREAKPOINT();
#endif
            return (RM_ERR_BAD_ARGUMENT);
    }

    //
    // Workaround a bug in the HOST's semaphore context dma checks.
    // It only accepts the SetContextDmaSemaphore method if the
    // ctxdma is marked COHERENCY_CACHED (even though the underlying
    // memory is NODE_NVM).  Since no other engine checks the
    // coherency flags (yet), this should be OK for now.
    //
    adjust |= SF_DEF(_DMA, _FLAGS_MAPPING_COHERENCY, _CACHED);

    //
    // Fill the page translation array.
    //
    if ((pDmaHalObjInfo->AddressSpace == ADDR_FBMEM) || (pDmaHalObjInfo->AddressSpace == ADDR_AGPMEM))
    {    
        INST_WR32(pDmaHalObjInfo->Instance, 8, pDmaHalObjInfo->PteArray[0]);
        INST_WR32(pDmaHalObjInfo->Instance, 12, pDmaHalObjInfo->PteArray[0]); // better guard
    }        
    else 
    {   
        for (i = 0; i < pDmaHalObjInfo->PteCount; i++)
        {
            INST_WR32(pDmaHalObjInfo->Instance, 8 + i*4, pDmaHalObjInfo->PteArray[i]);
        }

        //
        // Load in guard page pte.  Needed especially for scaled_image war.
        //
        INST_WR32(pDmaHalObjInfo->Instance, 8 + i*4, pDmaHalObjInfo->PteArray[0]);
    }
    //
    // Fill in rest of page table values.
    // Bump offset by 256bytes to cover scaled_image bug (we really only
    // need 64bytes of additional NV_DMA_LIMIT, but we need to make
    // sure that we align things for any possible use.
    //
    INST_WR32(pDmaHalObjInfo->Instance, SF_OFFSET(NV_DMA_CLASS), adjust);
    INST_WR32(pDmaHalObjInfo->Instance, SF_OFFSET(NV_DMA_LIMIT), (pDmaHalObjInfo->PteLimit + NV20_SCALED_IMAGE_WAR_OFFSET));

    //
    // Add this dma context to the HAL table.
    //
    pDmaHalObjInfo->Next = (PDMAHALOBJINFO)pDmaHalInfo->pDmaHalObjInfos;
    pDmaHalInfo->pDmaHalObjInfos = (VOID_PTR)pDmaHalObjInfo;

    return (RM_OK);
}

//
// nvHalDmaFree
//
RM_STATUS
nvHalDmaFree_NV20(VOID *arg)
{
    PDMAFREEARG_000 pDmaFreeArg = (PDMAFREEARG_000)arg;
    PHALHWINFO pHalHwInfo;
    PDMAHALINFO pDmaHalInfo;
    PDMAHALOBJINFO pDmaHalObjInfo, pDmaHalObjInfoDelete;

    //
    // Verify interface revision.
    //
    if (pDmaFreeArg->id != DMA_FREE_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pDmaFreeArg->pHalHwInfo;
    pDmaHalInfo = pHalHwInfo->pDmaHalInfo;
    pDmaHalObjInfo = pDmaFreeArg->pHalObjInfo;

    //
    // Detach from DMA list.
    //
    if (pDmaHalInfo->pDmaHalObjInfos == (VOID_PTR)pDmaHalObjInfo)
        pDmaHalInfo->pDmaHalObjInfos = (VOID_PTR)pDmaHalObjInfo->Next;
    else
    {
        pDmaHalObjInfoDelete = (PDMAHALOBJINFO)pDmaHalInfo->pDmaHalObjInfos;
        while (pDmaHalObjInfoDelete->Next && pDmaHalObjInfoDelete->Next != pDmaHalObjInfo)
        {
            pDmaHalObjInfoDelete = pDmaHalObjInfoDelete->Next;
        }
        if (pDmaHalObjInfoDelete->Next)
            pDmaHalObjInfoDelete->Next = pDmaHalObjInfo->Next;
    }

    return (RM_OK);
}

//
// nvHalDmaGetInstSize
//
// Return number of *paragraphs* required for dma context in
// instance memory.
//
RM_STATUS
nvHalDmaGetInstSize_NV20(VOID *arg)
{
    PDMAGETINSTSIZEARG_000 pDmaGetInstSizeArg = (PDMAGETINSTSIZEARG_000)arg;
    PDMAHALOBJINFO pDmaHalObjInfo;
    PHALHWINFO pHalHwInfo;

    //
    // Verify interface revision.
    //
    if (pDmaGetInstSizeArg->id != DMA_GET_INSTSIZE_000)
        return (RM_ERR_VERSION_MISMATCH);

    pHalHwInfo = pDmaGetInstSizeArg->pHalHwInfo;
    pDmaHalObjInfo = pDmaGetInstSizeArg->pHalObjInfo;

    //
    // Factor in an extra PTE so when the DMA engine prefetches, it
    // won't fault on a last access to a page.
    //
    
    //
    // Local memory contexts require only 1 paragraph (they're linear).
    // So do single page contexts in other address spaces (i.e. system memory).
    //
    if ((pDmaHalObjInfo->AddressSpace == ADDR_FBMEM) || ((pDmaHalObjInfo->AddressSpace == ADDR_AGPMEM)))
        pDmaHalObjInfo->NumUnits = 1;
    else
    {
        if (pDmaHalObjInfo->PteCount == 1)
            pDmaHalObjInfo->NumUnits = 1;
        else
            pDmaHalObjInfo->NumUnits = 2 + (pDmaHalObjInfo->PteCount - 1) / 4;
    }
    
    return (RM_OK);
}

//
// dmaHalInstanceToHalInfo
//
RM_STATUS
dmaHalInstanceToHalInfo_NV20(U032 chid, U032 instance, PDMAHALOBJINFO *ppDmaHalObjInfo, PHALHWINFO pHalHwInfo)
{
    PDMAHALOBJINFO pDmaHalObjInfo;

    pDmaHalObjInfo = (PDMAHALOBJINFO)pHalHwInfo->pDmaHalInfo->pDmaHalObjInfos;
    while (pDmaHalObjInfo)
    {
        if (pDmaHalObjInfo->Instance == instance)
        {
            //
            // Found it!
            //
    	    *ppDmaHalObjInfo = pDmaHalObjInfo;
            return RM_OK;
        }
        pDmaHalObjInfo = pDmaHalObjInfo->Next;
    }

    *ppDmaHalObjInfo = NULL;  
    return RM_ERR_BAD_ARGUMENT;
}

