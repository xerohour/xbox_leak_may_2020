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
//  Module: nvDefVB.cpp
//      default vertex buffer code
//
// **************************************************************************
//
//  History:
//      Craig Duttweiler        15Jun00         creation
//
// **************************************************************************

#include "nvprecomp.h"
#pragma hdrstop

//---------------------------------------------------------------------------

BOOL CDefaultVB::create
(
    DWORD dwHeap,
    DWORD dwSize
)
{
    dbgTracePush ("CDefaultVB::create");

    BOOL bRes = m_pVertexBuffer->create (dwSize, dwHeap, dwHeap);
    m_pRefCountMap->create (0, TRUE);

    m_dwCurrentOffset = 0;
    m_dwTotalVerticesSincePut = 0;

    dbgTracePop();
    return bRes;
}

//---------------------------------------------------------------------------

BOOL CDefaultVB::destroy
(
    void
)
{
    dbgTracePush ("CDefaultVB::destroy");

    m_pRefCountMap->destroy();
    m_pVertexBuffer->destroy();

    dbgTracePop();
    return (TRUE);
}

//---------------------------------------------------------------------------

DWORD CDefaultVB::getHeap (void)
{
    return (m_pVertexBuffer->getHeapLocation());
}

//---------------------------------------------------------------------------

DWORD CDefaultVB::getAddress (void)
{
    return (m_pVertexBuffer->getAddress());
}

//---------------------------------------------------------------------------

DWORD CDefaultVB::getSize (void)
{
    return (m_pVertexBuffer->getSize());
}

//---------------------------------------------------------------------------

DWORD CDefaultVB::getOffset (void)
{
    return (m_pVertexBuffer->getOffset());
}

//---------------------------------------------------------------------------

DWORD CDefaultVB::getCachedOffset (void)
{
    return (m_pRefCountMap->getCached());
}

//---------------------------------------------------------------------------

void CDefaultVB::updateOffset
(
    DWORD dwOffset
)
{
    m_dwCurrentOffset = dwOffset;
}

//---------------------------------------------------------------------------

void CDefaultVB::updateMap
(
    void
)
{
    m_pRefCountMap->put (m_dwCurrentOffset);
}

//---------------------------------------------------------------------------

// wait for space in the default vertex buffer
//
// int  isize - size of the memory block required (should not exceed VBSize / 4)
// BOOL bWait - if FALSE, return immediately when there is no space (doesn't work yet - must be TRUE)
//
// return value - offset to the requested memory block
//              - ~0 if bWait is FALSE and hardware is busy

DWORD CDefaultVB::waitForSpace
(
    int iSize, BOOL bWait
)
{
    // prepare some variables
    int iVBSize      = m_pVertexBuffer->getSize();
    int iVBOffset    = m_dwCurrentOffset;
    int iVBGetOffset = m_pRefCountMap->get();
    int iVBThreshold = (iVBOffset < iVBGetOffset) ? (iVBGetOffset - iSize)
                                                  : (iVBSize      - iSize);

        #ifdef NV_PROFILE_DEFVB
            int iGet = getDC()->pDefaultVBRefCountMap->get();
            nvpLogEvent (NVP_E_PRIM);
            nvpLogCount (NVP_C_PUT,iVBOffset);
            nvpLogCount (NVP_C_GET,iGet);
            nvpLogCount (NVP_C_FREECOUNT,iVBThreshold);
        #endif

    // wait for space
    BOOL bFirst = TRUE;
    while (iVBOffset > iVBThreshold)
    {
        // do we need to wrap around?
        if (iVBOffset >= int(iVBSize - iSize))
        {
            nvPusherStart(TRUE);
            iVBGetOffset = dispatchWrapAround (iVBOffset);
            iVBOffset    = 0;
        }
        else
        {
            // if we are not wrapping, write put now. this will ensure that there is a put
            // after the last batch of vertices which will prevent unnecessary delays when
            // waiting for free space later
            if (bFirst)
            {
                m_pRefCountMap->put(iVBOffset);
                m_dwTotalVerticesSincePut = 0;
                nvPusherStart (TRUE);
            }
        }

        // read HW get
        if (bFirst)
        {
            bFirst = FALSE;
        }
        else
        {
            if (!bWait) return ~0;
            nvDelay();
        }

        // get new threshold
        iVBGetOffset = m_pRefCountMap->get();
        iVBThreshold = (iVBOffset < iVBGetOffset) ? (iVBGetOffset - iSize)
                                                  : (iVBSize      - iSize);
    }

    // return offset into default vertex buffer
    return (iVBOffset);
}

//---------------------------------------------------------------------------

DWORD CDefaultVB::dispatchWrapAround
(
    int iVBOffset
)
{
    // read get pointer
    int  iVBGetOffset  = m_pRefCountMap->get();
    BOOL bPutChasesGet = iVBOffset < iVBGetOffset;

    // make certain get has advanced far enough such that we do not
    //  scribble over batched data
    if (bPutChasesGet)
    {
        // if put is already chasing get, then get must wrap around
        //  and advance away from zero so that we do not discard
        //  a whole buffer of vertices. when we are done here, get
        //  will be chasing put
        for (; (iVBOffset < iVBGetOffset) && m_pRefCountMap->isBusy();)
        {
            nvDelay();
            iVBGetOffset = m_pRefCountMap->get();
        }
    }

    // we are about to go from running away from get, to put chasing get.
    //  in order not to discard a full buffer, we have to make certain
    //  that get is not zero. we may have to help get become not zero
    //  in case nothing else was pipelined since.
    if (!iVBGetOffset)
    {
        m_pRefCountMap->put (iVBOffset);
        nvPusherStart (TRUE);
        iVBGetOffset = m_pRefCountMap->get();
        for (; (!iVBGetOffset) && m_pRefCountMap->isBusy();)
        {
            nvDelay();
            iVBGetOffset = m_pRefCountMap->get();
        }
    }

    // write wrap around
    m_pRefCountMap->put (0);
    m_dwTotalVerticesSincePut = 0;
    nvPusherStart (TRUE);

#if (NVARCH >= 0x020)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        nvglSetNv20KelvinInvalidateVertexCache (NV_DD_KELVIN);
    }
    else
#endif
#if (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        nvglSetNv10CelsiusInvalidateVertexCache (NV_DD_CELSIUS);
    }
#endif

    // done
    return (iVBGetOffset);
}

//---------------------------------------------------------------------------

CDefaultVB::CDefaultVB
(
    void
)
{
    dbgTracePush ("CDefaultVB::CDefaultVB");

    m_pVertexBuffer = new CVertexBuffer;
    m_pRefCountMap  = new CReferenceCountMap<10>;

    dbgTracePop();
}

//---------------------------------------------------------------------------

CDefaultVB::~CDefaultVB
(
    void
)
{
    dbgTracePush ("CDefaultVB::~CDefaultVB");

    if (m_pVertexBuffer) delete m_pVertexBuffer;
    if (m_pRefCountMap)  delete m_pRefCountMap;

    dbgTracePop();
}

