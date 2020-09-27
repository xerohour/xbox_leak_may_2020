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
/******************************* DMA Manager *******************************\
*                                                                           *
* Module: DMASTATE.C                                                        *
*   The DMA state is managed in this module.                                *
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
#include <dma.h>
#include <os.h>
#include <nv32.h>
#include "nvhw.h"


//
// Initialize DMA hardware.
//
RM_STATUS stateDma
(
    PHWINFO pDev,
    U032    msg
)
{
#ifdef KJK
    PDMAOBJECT DmaObject;
#endif
#ifdef LEGACY_ARCH
    RM_STATUS  status;
#endif
    
    switch (msg)
    {
        case STATE_UPDATE:
            break;
        case STATE_LOAD:

#ifdef LEGACY_ARCH
            //
            // Fill class-specific NULL DMA instances with something invalid
            //
            if (!pDev->Dma.DmaInMemEmptyInstance)
            {            
                status = fbAllocInstMem(pDev, &(pDev->Dma.DmaInMemEmptyInstance), 1);
                if (status == RM_OK)
                {
//                    INST_WR32(pDev->Dma.DmaInMemEmptyInstance, SF_OFFSET(NV_DMA_CLASS), NV1_CONTEXT_DMA_IN_MEMORY);
//                    INST_WR32(pDev->Dma.DmaInMemEmptyInstance, SF_OFFSET(NV_DMA_LIMIT),  0);
                    INST_WR32(pDev->Dma.DmaInMemEmptyInstance, SF_OFFSET(NV_DMA_CLASS), NV01_NULL);
                    INST_WR32(pDev->Dma.DmaInMemEmptyInstance, SF_OFFSET(NV_DMA_LIMIT), 0xFFFFFFFF);
                }
            }                
            if (!pDev->Dma.DmaToMemEmptyInstance)
            {            
                status = fbAllocInstMem(pDev, &(pDev->Dma.DmaToMemEmptyInstance), 1);
                if (status == RM_OK)
                {
//                    INST_WR32(pDev->Dma.DmaToMemEmptyInstance, SF_OFFSET(NV_DMA_CLASS), NV1_CONTEXT_DMA_TO_MEMORY);
//                    INST_WR32(pDev->Dma.DmaToMemEmptyInstance, SF_OFFSET(NV_DMA_LIMIT),  0);
                    INST_WR32(pDev->Dma.DmaToMemEmptyInstance, SF_OFFSET(NV_DMA_CLASS), NV01_NULL);
                    INST_WR32(pDev->Dma.DmaToMemEmptyInstance, SF_OFFSET(NV_DMA_LIMIT), 0xFFFFFFFF);
                }
            }                
            if (!pDev->Dma.DmaFromMemEmptyInstance)
            {            
                status = fbAllocInstMem(pDev, &(pDev->Dma.DmaFromMemEmptyInstance), 1);
                if (status == RM_OK)
                {
//                    INST_WR32(pDev->Dma.DmaFromMemEmptyInstance, SF_OFFSET(NV_DMA_CLASS), NV1_CONTEXT_DMA_FROM_MEMORY);
//                    INST_WR32(pDev->Dma.DmaFromMemEmptyInstance, SF_OFFSET(NV_DMA_LIMIT),  0);
                    INST_WR32(pDev->Dma.DmaFromMemEmptyInstance, SF_OFFSET(NV_DMA_CLASS), NV01_NULL);
                    INST_WR32(pDev->Dma.DmaFromMemEmptyInstance, SF_OFFSET(NV_DMA_LIMIT), 0xFFFFFFFF);
                }
            }                
#endif // LEGACY_ARCH

#ifdef KJK            
            //
            // Reload any DMA instances.
            //
            DmaObject = pDev->DBDmaList;
            while (DmaObject)
            {
                DBG_VAL_PTR(DmaObject);
                //
                // Load instance.
                //    
                if (DmaObject->State & DMA_RELOAD)
                {
                    //
                    // Get DMA instance for page table mapping.
                    //
                    if (dmaAllocInstance(DmaObject))
                    {
                        DmaObject->Valid = FALSE;
                        osUnlockUserMem(DmaObject->Base.ChID,
                                        DmaObject->BufferBase,
                                        DmaObject->DescAddr,
                                        DmaObject->AddressSpace,
                                        DmaObject->DescLimit,
                                        DmaObject->PteCount,
                                        DmaObject->LockHandle,
                                        DmaObject->Base.Class->Type == NV1_CONTEXT_DMA_TO_MEMORY ? TRUE : FALSE);
                        osFreeMem(DmaObject->PteArray);
                        DmaObject->PteArray = NULL;
                    }
                    DmaObject->State &= ~DMA_RELOAD;
                }
                DmaObject = DmaObject->Next;
            }
#endif //KJK
            break;
        case STATE_UNLOAD:
            //
            // Unload all DMA instances.
            //
#ifdef KJK            
            DmaObject = pDev->DBDmaList;
            while (DmaObject)
            {
                //
                // Unload instance.
                //    
                DBG_VAL_PTR(DmaObject);
                if (DmaObject->PteArray)
                {
                    dmaFreeInstance(pDev, DmaObject);
                    DmaObject->State |= DMA_RELOAD;
                }
                DmaObject = DmaObject->Next;
            }

            pDev->Dma.DmaInMemEmptyInstance = 0;
            pDev->Dma.DmaToMemEmptyInstance = 0;
            pDev->Dma.DmaFromMemEmptyInstance = 0;
#endif //KJK
            break;
        case STATE_INIT:
            pDev->DBDmaList = NULL;

#ifdef LEGACY_ARCH    
            pDev->Dma.DmaInMemEmptyInstance = 0;
            pDev->Dma.DmaToMemEmptyInstance = 0;
            pDev->Dma.DmaFromMemEmptyInstance = 0;
#endif // LEGACY_ARCH
            
            break;
        case STATE_DESTROY:
            break;
    }    
    return (RM_OK);
}
