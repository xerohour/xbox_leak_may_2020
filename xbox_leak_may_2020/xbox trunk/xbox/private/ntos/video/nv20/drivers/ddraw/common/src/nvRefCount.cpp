 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/

#include "nvprecomp.h"
#pragma hdrstop

/*
 * this is bad and it will fail inspection. dont know where to put them though
 */
void nvReset2DSurfaceFormat (void);

/*****************************************************************************
 *****************************************************************************
 *****************************************************************************
 ***  CReferenceCount  *******************************************************
 *****************************************************************************
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * CReferenceCount::nv4WriteCount
 *
 * writes refcount for NV4. we emulate by filling a single 32bpp pixel at a
 *  known location
 */
void CReferenceCount::nv4WriteCount
(
    void
)
{
    PNVD3DCONTEXT pContext;

    // Set proper surface info
    nvglSet2DSurfacesDestination (NV_DD_SURFACES, m_dwNV4SurfaceOffset);
    nvglSet2DSurfacesColorFormat (NV_DD_SURFACES, NV042_SET_COLOR_FORMAT_LE_Y32);
    //nvglSet2DSurfacesPitch       (NV_DD_SURFACES, 0x20,0x20);

    // Do solid rect fill (with argument as color)
    if (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_005E_SOLIDRECT)) {
        nvglSetObject                   (NV_DD_SPARE, D3D_GDI_RECTANGLE_TEXT_2);
        nvglDrawRopRectAndTextColor1A   (NV_DD_SPARE, m_dwRefCount);
        nvglDrawRopRectAndTextPointSize (NV_DD_SPARE, 0, 0, 1, 1);
        pDriverData->dDrawSpareSubchannelObject = D3D_GDI_RECTANGLE_TEXT_2;
    }
    else {
        nvglSetObject      (NV_DD_SPARE,   D3D_RENDER_SOLID_RECTANGLE_2);
        nvglSetStartMethod (0,NV_DD_SPARE, NV05E_COLOR,1);
        nvglSetData        (1,             m_dwRefCount);
        nvglSetStartMethod (2,NV_DD_SPARE, NV05E_RECTANGLE(0),2);
        nvglSetData        (3,             0);
        nvglSetData        (4,             0x00010001);
        nvglAdjust         (5);
        pDriverData->dDrawSpareSubchannelObject = D3D_RENDER_SOLID_RECTANGLE_2;
    }

    // We can't rely on pContext here.
    pContext = (PNVD3DCONTEXT)getDC()->dwContextListHead;
    while (pContext)
    {
        NV_FORCE_TRI_SETUP(pContext);
        pContext = pContext->pContextNext;
    }

    // reset when we are done
    nvReset2DSurfaceFormat();
}

/*****************************************************************************
 * CReferenceCount::create
 *
 * creates the infrastructure to support a reference count. for NV4/5 we emulate
 *  this behavior. NV10 supports this in HW.
 */
BOOL CReferenceCount::create
(
    DWORD dwTimeOut
)
{
#if ((NVARCH >= 0x020) && defined(KELVIN_SEMAPHORES2))
    if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
        (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
        (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
    {
        m_pSem = getDC()->nvSemaphoreManager.allocate (0);
    }

    else
#endif
    if ((!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN)) &&
        (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS))) {
        // We need to allocate a 16 byte chunk of memory that is guaranteed
        // to be 512 byte aligned. It doesn't matter too much where it is,
        // but don't let it fragment the heap. I specify TYPE_CURSOR because
        // that will place it at the end of the heap.
        DWORD dwStatus;
        NVHEAP_ALLOC (dwStatus, m_dwNV4SurfaceMemory, 511 + 16, TYPE_CURSOR);
        nvAssert (dwStatus == 0);
        DWORD dwMem = (m_dwNV4SurfaceMemory + 511) & (~511);
        m_dwNV4SurfaceOffset = dwMem - pDriverData->BaseAddress;
    }

    // init
    m_dwRefCount = m_dwHWRefCount = 0;
    if (getDC()->nvPusher.isValid()) {
        writeRefCount();
    }
#if (NVARCH >= 0x020)
    if (!(pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_CELSIUS | NVCLASS_FAMILY_KELVIN))) {
#else
    if (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS)) {
#endif
        *(DWORD*) (VIDMEM_ADDR(m_dwNV4SurfaceOffset + pDriverData->BaseAddress)) = 0;
    }

    // set timeout state
    m_dwTimeOutValue = m_dwTimeOutReload = dwTimeOut;

    return TRUE;
}

/*****************************************************************************
 * CReferenceCount::destroy
 *
 * destroys refcount related data
 */
void CReferenceCount::destroy
(
    void
)
{
#if ((NVARCH >= 0x020) && defined(KELVIN_SEMAPHORES2))
    if (m_pSem == 0xffffffff) return;   // create() was not called

    if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
        (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
        (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
    {
        getDC()->nvSemaphoreManager.free (m_pSem);
    }

    else
#endif
    if (m_dwNV4SurfaceMemory == 0xffffffff) return;   // create() was not called

    // for now kelvin and nv4 behave the same
    if ( !(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) ||
        (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS))) {
        NVHEAP_FREE (m_dwNV4SurfaceMemory);
        m_dwNV4SurfaceMemory = 0xffffffff;
        m_dwNV4SurfaceOffset = 0;
    }
}

/*****************************************************************************
 *****************************************************************************
 *****************************************************************************
 ***  CReferenceCountMap  ****************************************************
 *****************************************************************************
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * CReferenceCountMap::put
 *
 * writes a value into the map
 */
template <int cdwLogMax>
void CReferenceCountMap<cdwLogMax>::put
(
    DWORD dwNumber
)
{
    m_dwLastPut = dwNumber;

#if ((NVARCH >= 0x020) && defined(KELVIN_SEMAPHORES2))
    if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
        (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
        (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
    {
        nvAssert (m_pSem);
        m_pSem->release (dwNumber, &(getDC()->nvPusher), NV_DD_KELVIN, CSemaphore::SEMAPHORE_RELEASE_KELVIN_BACKEND);
    }

    else
#endif
    {
        // next head position
        DWORD dwNewHead = (m_dwHead + 1) & ((1 << cdwLogMax) - 1);
        if (dwNewHead == m_dwTail)
        {
            /*
             * 'get' the latest value. even though we do not use this value, we rely on ::get's
             *  side effect that it advances the tail pointer to the last know HW position
             */
            get();

            /*
             * if we are still out of space, block HW until we do
             *  bad case - we really want to avoid doing this by increasing cdwLogMax
             */
            if (dwNewHead == m_dwTail)
            {
                // start pusher
                nvPusherStart (TRUE);

                // spin until we have space
                do
                {
                    nvDelay();
                    get();
                }
                while (dwNewHead == m_dwTail);
            }
        }

        // write record
        m_adwRefCount[m_dwHead] = getDC()->pRefCount->inc (CReferenceCount::INC_IMMEDIATE);
        m_adwNumber  [m_dwHead] = dwNumber;
        m_dwHead                = dwNewHead;
    }
}

/*****************************************************************************
 * CReferenceCountMap::get
 *
 * reads the last value HW has reached
 */
template <int cdwLogMax>
DWORD CReferenceCountMap<cdwLogMax>::get
(
    void
)
{
#if ((NVARCH >= 0x020) && defined(KELVIN_SEMAPHORES2))
    if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
        (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
        (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
    {
        nvAssert (m_pSem);
        m_dwLast = m_pSem->read();
        return (m_dwLast);
    }

    else
#endif
    {
        // read ref count
        DWORD dwRefCount = getDC()->pRefCount->read();

        BOOL bLast = TRUE;
        // find its location in the map
        for (; m_dwTail != m_dwHead; m_dwTail = (m_dwTail + 1) & ((1 << cdwLogMax) - 1))
        {
            bLast = FALSE;

            // compare tail against latest date
            if (dwRefCount < m_adwRefCount[m_dwTail])
            {
                // reached a date the HW has not done with yet
                return m_dwLast;
            }

            // get number
            if (m_bConservative)
            {
                m_dwLast  = m_dwLast2;
                m_dwLast2 = m_adwNumber[m_dwTail];
            }
            else
            {
                m_dwLast  = m_adwNumber[m_dwTail];
            }
        }

        if (m_bConservative && bLast && (m_dwLast != m_dwLast2))
        {
            m_dwLast = m_dwLast2;
        }

        // list is empty, return the last number we had
        return (m_dwLast);
    }
}

/*****************************************************************************
 * CReferenceCountMap::isBusy
 */
template <int cdwLogMax>
BOOL CReferenceCountMap<cdwLogMax>::isBusy
(
    void
)
{
#if ((NVARCH >= 0x020) && defined(KELVIN_SEMAPHORES2))
    if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
        (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
        (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
    {
        return (m_pSem->read() < m_dwLastPut);
    }

    else
#endif
    {
        get();
        return (m_dwHead != m_dwTail);
    }
}

/*****************************************************************************
 * CReferenceCountMap::create
 */
template <int cdwLogMax>
void CReferenceCountMap<cdwLogMax>::create
(
    DWORD dwInitialNumber, BOOL bConservative
)
{
#if ((NVARCH >= 0x020) && defined(KELVIN_SEMAPHORES2))
    if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
        (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
        (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
    {
        nvAssert (!m_pSem);
        m_pSem = getDC()->nvSemaphoreManager.allocate (dwInitialNumber);
    }

    else
#endif
    {
        // init
        m_bConservative = bConservative;
        m_dwHead  = m_dwTail = 0;
        m_dwLast2 = m_dwLast = dwInitialNumber;
        m_adwNumber[(1 << cdwLogMax) - 1] = dwInitialNumber;
    }
}

/*****************************************************************************
 * CReferenceCountMap::destroy
 */
template <int cdwLogMax>
void CReferenceCountMap<cdwLogMax>::destroy
(
    void
)
{
#if ((NVARCH >= 0x020) && defined(KELVIN_SEMAPHORES2))
    if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
        (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
        (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
    {
        nvAssert (m_pSem);
        getDC()->nvSemaphoreManager.free (m_pSem);
    }
#endif
}

#pragma warning (push, 1)
// NOTE: The following warning is issued incorrectly by the MS compiler. Ignore it.  <bdw>
template CReferenceCountMap<5>;
template CReferenceCountMap<10>;
#pragma warning (pop)

