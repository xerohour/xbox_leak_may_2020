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
//  Module: nvFlipper.cpp
//      a flipping class, for use with kelvin-style semaphores
//      (possibly to be expanded to handle flipping more generally)
//
// **************************************************************************
//
//  History:
//      Craig Duttweiler        26Jun2000         created, NV20 development
//
// **************************************************************************

#include "nvprecomp.h"
#pragma hdrstop

#if (NVARCH >= 0x020)
#ifdef KELVIN_SEMAPHORES

//---------------------------------------------------------------------------

BOOL CFlipper::create (void)
{
    return (TRUE);
}

//---------------------------------------------------------------------------

BOOL CFlipper::init (DWORD dwChainSize)
{
    m_dwFlipChainSize = dwChainSize;
    m_dwIndex         = 0;

    // initialize semaphores
    for (DWORD i=0; i<(dwChainSize-1); i++) {
        m_ppSemaphores[i] = getDC()->nvSemaphoreManager.allocate (SEMAPHORE_READY_FOR_RENDER);
    }
    m_ppSemaphores[dwChainSize-1] = getDC()->nvSemaphoreManager.allocate (SEMAPHORE_READY_FOR_SCANOUT);

    return (TRUE);
}

//---------------------------------------------------------------------------

BOOL CFlipper::destroy (void)
{
    // free semaphores
    for (DWORD i=0; i<m_dwFlipChainSize; i++) {
        getDC()->nvSemaphoreManager.free (m_ppSemaphores[i]);
    }

    return (TRUE);
}

//---------------------------------------------------------------------------

BOOL CFlipper::flip (CPushBuffer *pPusher3D,
                     CPushBuffer *pPusherDAC,
                     LPDDRAWI_DDRAWSURFACE_LCL pSurfCurr,
                     LPDDRAWI_DDRAWSURFACE_LCL pSurfTarg,
                     BOOL bNoVSync)
{
    CSemaphore *pCurrSem, *pNextSem, *pLastSem;
    DWORD       dwNextIndex, dwLastIndex;
    DWORD       dwDAC, dwNumHeads;
    DWORD       dwFormat;
    BOOL        bMultiHead;

    dwNextIndex = (m_dwIndex + 1) % m_dwFlipChainSize;
    dwLastIndex = (m_dwIndex + m_dwFlipChainSize - 1) % m_dwFlipChainSize;

    pCurrSem = m_ppSemaphores[m_dwIndex];
    pNextSem = m_ppSemaphores[dwNextIndex];
    pLastSem = m_ppSemaphores[dwLastIndex];

    m_dwIndex = dwNextIndex;

#ifdef WINNT
    // on Win2K we don't support real NV11 multimon, so anything with more than 1 DAC is treated like clone mode
    bMultiHead = pDriverData->ppdev->ulNumberDacsActive > 1;
    dwDAC      = NV_DD_VIDEO_LUT_CURSOR_DAC;
#else  // !WINNT
    bMultiHead = pDriverData->dwDesktopState == NVTWINVIEW_STATE_CLONE;
    dwDAC      = NV_DD_VIDEO_LUT_CURSOR_DAC + pDXShare->dwHeadNumber;
#endif  // !WINNT

    // we don't handle multi-head stuff yet
    nvAssert (!bMultiHead);

    PRIMARY_BUFFER_INDEX ^= 0x1;

    dwNumHeads = bMultiHead ? pDriverData->dwHeads : 1;
    dwFormat   = DRF_DEF (07C, _SET_IMAGE_FORMAT, _NOTIFY, _WRITE_ONLY) |
                 DRF_NUM (07C, _SET_IMAGE_FORMAT, _PITCH,  pSurfTarg->lpGbl->lPitch);
    dwFormat  |= bNoVSync   ? DRF_DEF (07C, _SET_IMAGE_FORMAT, _WHEN, _IMMEDIATELY) :
                              DRF_DEF (07C, _SET_IMAGE_FORMAT, _WHEN, _NOT_WITHIN_BUFFER);
    dwFormat  |= bMultiHead ? DRF_DEF (07C, _SET_IMAGE_FORMAT, _MULTIHEAD_SYNC, _ENABLED) :
                              DRF_DEF (07C, _SET_IMAGE_FORMAT, _MULTIHEAD_SYNC, _DISABLED);

    for (DWORD dwLogicalHead = 0; dwLogicalHead < dwNumHeads; dwLogicalHead ++) {
        // calculate offset, the RM will add the pan & scan adjustment from the display driver if needed
        DWORD dwOffset = pSurfTarg->lpGbl->fpVidMem - pDriverData->BaseAddress;
#ifdef WINNT
        DWORD dwPhysicalHead = pDriverData->ppdev->ulDeviceDisplay[dwLogicalHead];
        // add the span mode offset
        dwOffset += pDriverData->ppdev->ulHeadDisplayOffset[dwPhysicalHead];
#else // !WINNT
        DWORD dwPhysicalHead = dwLogicalHead;
#endif // !WINNT
        // BUGBUG eventually this needs to be per DAC object!!

        // the 3D rendering pipe releases its current target and waits to acquire the next
        pCurrSem->release (SEMAPHORE_READY_FOR_SCANOUT, pPusher3D, NV_DD_KELVIN, CSemaphore::SEMAPHORE_RELEASE_KELVIN_BACKEND);
        pNextSem->acquire (SEMAPHORE_READY_FOR_RENDER,  pPusher3D);

        // the DAC acquires the new scanout source, and posts a flip
        // (note the release method gets issued before the flip itself because it really just
        // sets a trigger that gets executed _after_ the subsequent flip has actually occurred)
        pCurrSem->acquire (SEMAPHORE_READY_FOR_SCANOUT, pPusherDAC);
        pLastSem->release (SEMAPHORE_FLIP_COMPLETE, pPusherDAC, NV_DD_SUBCH_DAC, CSemaphore::SEMAPHORE_RELEASE_DAC);
        // program the flip
        // BUGBUG eventually set the correct DAC object
        //pPusherDAC->push (x, (0x00040000 | (NV_DD_SUBCH_DAC << 13)));
        //pPusherDAC->push (x, dwDAC + dwPhysicalHead);
        pPusherDAC->push (0, (0x00080000 | (NV_DD_SUBCH_DAC << 13) | NV07C_SET_IMAGE_OFFSET(PRIMARY_BUFFER_INDEX)));
        pPusherDAC->push (1, dwOffset);
        pPusherDAC->push (2, dwFormat);
        pPusherDAC->adjust (3);

        // the DAC channel waits for the flip to come back, after which it can
        // finally release this source back to the 3D channel and itself proceed.
        // important: this ensures vital serialization of flips and methods on the DAC channel
        pLastSem->acquire (SEMAPHORE_FLIP_COMPLETE, pPusherDAC);
        pLastSem->release (SEMAPHORE_READY_FOR_RENDER, pPusherDAC);

    }  // for...

    #ifdef SPOOF_KELVIN
    // delay to make winbench happy
    NV_SLEEP1;
    #endif

    return (TRUE);
}

#endif  // KELVIN_SEMAPHORES
#endif  // NVARCH >= 0x020

