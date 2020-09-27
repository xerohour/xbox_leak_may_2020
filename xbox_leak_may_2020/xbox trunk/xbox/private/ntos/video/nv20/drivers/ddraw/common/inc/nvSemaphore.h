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

#ifndef __NVSEMAPHORE_H
#define __NVSEMAPHORE_H

#if (NVARCH >= 0x020)
#ifdef KELVIN_SEMAPHORES

//---------------------------------------------------------------------------

// forward definitions

class CSimpleSurface;
class CSemaphoreManager;

//---------------------------------------------------------------------------

class CSemaphore
{

public:
    const enum {
        SEMAPHORE_RELEASE_KELVIN_BACKEND = 1,
        SEMAPHORE_RELEASE_KELVIN_TEXTURE = 2,
        SEMAPHORE_RELEASE_DAC            = 3
    };

private:
    DWORD   m_dwValue;   // must be first
    DWORD   m_dwOffset;  // offset of this semaphore within the semphore context DMA
    BOOL    m_bInUse;
    DWORD   m_dwPad;     // semaphores must be 16-byte aligned

public:
    BOOL    acquire (DWORD dwValue, CPushBuffer *pPusher);
    BOOL    release (DWORD dwValue, CPushBuffer *pPusher);
    BOOL    release (DWORD dwValue, CPushBuffer *pPusher, DWORD dwSubCh, DWORD dwType);

    DWORD   read    (void)      { return (m_dwValue); }

    CSemaphore()    {}
    ~CSemaphore()   {}

    friend CSemaphoreManager;

};

//---------------------------------------------------------------------------

class CSemaphoreManager
{

public:

    const enum {
        SEMAPHORE_CONTEXT_DMA_IN_MEMORY   = 0xDD001E00,
        SEMAPHORE_CONTEXT_DMA_FROM_MEMORY = 0xDD001E01
    };

private:

    CSimpleSurface *m_pSemaphoreSurface;
    BYTE           *m_pSemaphoreMemory;

public:

    BOOL            create    (void);
    BOOL            destroy   (void);
    CSemaphore     *allocate  (DWORD dwInitialValue);
    BOOL            free      (CSemaphore *pSem);

    CSemaphoreManager()       {   m_pSemaphoreSurface = (CSimpleSurface *)NULL;
                                  m_pSemaphoreMemory  = NULL;
                              }

    ~CSemaphoreManager()      {}

};

#endif  // KELVIN_SEMAPHORES
#endif  // NVARCH >= 0x020

#endif  // __NVSEMAPHORE_H
