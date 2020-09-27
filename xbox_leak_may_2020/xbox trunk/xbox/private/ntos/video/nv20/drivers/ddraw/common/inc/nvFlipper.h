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
//  Module: nvFlipper.h
//      a flipping class, for use with kelvin-style semaphores
//      (possibly to be expanded to handle flipping more generally)
//
// **************************************************************************
//
//  History:
//      Craig Duttweiler        26Jun2000         created, NV20 development
//
// **************************************************************************

#ifndef __NVFLIPPER_H
#define __NVFLIPPER_H

#if (NVARCH >= 0x020)
#ifdef KELVIN_SEMAPHORES

#define FLIPPER_MAX_CHAIN_SIZE 16

//---------------------------------------------------------------------------

class CFlipper
{

private:

    const enum {
        SEMAPHORE_READY_FOR_RENDER  = 0,
        SEMAPHORE_READY_FOR_SCANOUT = 1,
        SEMAPHORE_FLIP_COMPLETE     = 2
    };

    CSemaphore      *m_ppSemaphores[FLIPPER_MAX_CHAIN_SIZE];
    DWORD            m_dwFlipChainSize;
    DWORD            m_dwIndex;

public:

    BOOL create     (void);
    BOOL destroy    (void);
    BOOL init       (DWORD dwChainSize);
    BOOL flip       (CPushBuffer *pPusher3D,
                     CPushBuffer *pPusherDAC,
                     LPDDRAWI_DDRAWSURFACE_LCL pSurfCurr,
                     LPDDRAWI_DDRAWSURFACE_LCL pSurfTarg,
                     BOOL bNoVSync);

    CFlipper()      {   m_dwFlipChainSize = 0;
                        m_dwIndex         = 0;    }

    ~CFlipper()     {}
};

#endif  // KELVIN_SEMAPHORES
#endif  // NVARCH >= 0x020
#endif  // __NVFLIPPER_H

