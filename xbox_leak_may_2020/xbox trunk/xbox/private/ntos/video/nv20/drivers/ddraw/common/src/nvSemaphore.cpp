// **************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// ********************************* Direct 3D ******************************
//
//  Module: nvSemaphore.cpp
//      a semaphore class, for use with kelvin-style semaphores
//      handles allocation and management
//
// **************************************************************************
//
//  History:
//      Craig Duttweiler        13Sep2000         created, NV20 development
//
// **************************************************************************

#include "nvprecomp.h"
#pragma hdrstop

#if (NVARCH >= 0x020)
#ifdef KELVIN_SEMAPHORES

//---------------------------------------------------------------------------

// acquire a sempahore

BOOL CSemaphore::acquire (DWORD dwValue, CPushBuffer *pPusher)
{
    pPusher->push (0, (0x00040000 | NV206E_SEMAPHORE_OFFSET));
    pPusher->push (1, m_dwOffset);
    pPusher->push (2, (0x00040000 | NV206E_SEMAPHORE_ACQUIRE));
    pPusher->push (3, dwValue);
    pPusher->adjust (4);
    return (TRUE);
}

//---------------------------------------------------------------------------

// release a semaphore via the channel

BOOL CSemaphore::release (DWORD dwValue, CPushBuffer *pPusher)
{
    pPusher->push (0, (0x00040000 | NV206E_SEMAPHORE_OFFSET));
    pPusher->push (1, m_dwOffset);
    pPusher->push (2, (0x00040000 | NV206E_SEMAPHORE_RELEASE));
    pPusher->push (3, dwValue);
    pPusher->adjust (4);
    return (TRUE);
}

//---------------------------------------------------------------------------

// release a semaphore through a class interface

BOOL CSemaphore::release (DWORD dwValue, CPushBuffer *pPusher, DWORD dwSubCh, DWORD dwType)
{
    switch (dwType) {
        case SEMAPHORE_RELEASE_KELVIN_BACKEND:
            pPusher->push (0, (0x00040000 | (dwSubCh << 13) | NV097_SET_SEMAPHORE_OFFSET));
            pPusher->push (1, m_dwOffset);
            pPusher->push (2, (0x00040000 | (dwSubCh << 13) | NV097_BACK_END_WRITE_SEMAPHORE_RELEASE));
            pPusher->push (3, dwValue);
            pPusher->adjust (4);
            break;
        case SEMAPHORE_RELEASE_KELVIN_TEXTURE:
            pPusher->push (0, (0x00040000 | (dwSubCh << 13) | NV097_SET_SEMAPHORE_OFFSET));
            pPusher->push (1, m_dwOffset);
            pPusher->push (2, (0x00040000 | (dwSubCh << 13) | NV097_TEXTURE_READ_SEMAPHORE_RELEASE));
            pPusher->push (3, dwValue);
            pPusher->adjust (4);
            break;
        case SEMAPHORE_RELEASE_DAC:
            pPusher->push (0, (0x00040000 | (dwSubCh << 13) | NV07C_SET_SEMAPHORE_OFFSET));
            pPusher->push (1, m_dwOffset);
            pPusher->push (2, (0x00040000 | (dwSubCh << 13) | NV07C_SET_SEMAPHORE_RELEASE));
            pPusher->push (3, dwValue);
            pPusher->adjust (4);
            break;
        default:
            DPF ("unknown semaphore release type");
            dbgD3DError();
            break;
    } // switch
    return (TRUE);
}

//---------------------------------------------------------------------------

// create a manager for a page of semaphores

BOOL CSemaphoreManager::create (void)
{
    DWORD dwSize = 2*PAGE_SIZE;  // allocate two pages to insure successful alignment

    // allocate the surface
    m_pSemaphoreSurface = new CSimpleSurface;
    if (!m_pSemaphoreSurface) {
        return (FALSE);
    }

    // create the semaphores
    if (!m_pSemaphoreSurface->create (dwSize, 1, 1, 0,
                                      CSimpleSurface::HEAP_VID,
                                      CSimpleSurface::HEAP_VID,
                                      CSimpleSurface::ALLOCATE_SIMPLE
                                      #ifdef CAPTURE
                                      ,CAPTURE_SURFACE_KIND_UNKNOWN
                                      #endif
                                     )) {
        return (FALSE);
    }

    // align to the beginning of the first page contained in our allocated memory
    m_pSemaphoreMemory = (BYTE*)((m_pSemaphoreSurface->getAddress() + PAGE_SIZE - 1) & PAGE_MASK);

    // map a context dma to the page of semaphores
    if (NvRmAllocContextDma (pDriverData->dwRootHandle,
                             SEMAPHORE_CONTEXT_DMA_IN_MEMORY,
                             NV01_CONTEXT_DMA,
                             DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_WRITE) |
                             DRF_DEF(OS03, _FLAGS, _COHERENCY, _CACHED),
                             (PVOID) m_pSemaphoreMemory,
                             PAGE_SIZE-1)) {
        return (FALSE);
    }
    if (NvRmAllocContextDma (pDriverData->dwRootHandle,
                             SEMAPHORE_CONTEXT_DMA_FROM_MEMORY,
                             NV01_CONTEXT_DMA,
                             DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_ONLY) |
                             DRF_DEF(OS03, _FLAGS, _COHERENCY, _CACHED),
                             (PVOID) m_pSemaphoreMemory,
                             PAGE_SIZE-1)) {
        return (FALSE);
    }

    // initialize the semaphores (in particular, the bInUse field)
    memset (m_pSemaphoreMemory, FALSE, PAGE_SIZE);

    // success
    return (TRUE);
}

//---------------------------------------------------------------------------

// destroy the semaphore manager

BOOL CSemaphoreManager::destroy (void)
{
    // free the context dmas
    NvRmFree (pDriverData->dwRootHandle, pDriverData->dwRootHandle, SEMAPHORE_CONTEXT_DMA_IN_MEMORY);
    NvRmFree (pDriverData->dwRootHandle, pDriverData->dwRootHandle, SEMAPHORE_CONTEXT_DMA_FROM_MEMORY);
    // free the semaphores and delete the surface
    if (m_pSemaphoreSurface) {
        m_pSemaphoreSurface->destroy();
        delete m_pSemaphoreSurface;
    }
    m_pSemaphoreSurface = (CSimpleSurface *)NULL;
    m_pSemaphoreMemory  = NULL;
    return (TRUE);
}

//---------------------------------------------------------------------------

// allocate a new semaphore

CSemaphore *CSemaphoreManager::allocate (DWORD dwInitialValue)
{
    CSemaphore *pSem = (CSemaphore *)m_pSemaphoreMemory;

    while (((PBYTE)pSem) < ((PBYTE)(m_pSemaphoreMemory + PAGE_SIZE))) {
        if (!pSem->m_bInUse) {
            pSem->m_dwValue = dwInitialValue;
            pSem->m_dwOffset = (DWORD)pSem - (DWORD)m_pSemaphoreMemory;
            pSem->m_bInUse = TRUE;
            return (pSem);
        }
        pSem ++;
    }

    return (NULL);
}

//---------------------------------------------------------------------------

// free a semaphore

BOOL CSemaphoreManager::free (CSemaphore *pSem)
{
    pSem->m_bInUse = FALSE;
    return (TRUE);
}

#endif  // KELVIN_SEMAPHORES
#endif  // NVARCH >= 0x020

