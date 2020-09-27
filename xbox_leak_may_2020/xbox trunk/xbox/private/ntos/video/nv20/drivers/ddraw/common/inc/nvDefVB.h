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
//  Module: nvDefVB.h
//      default vertex buffer header file
//
// **************************************************************************
//
//  History:
//      Craig Duttweiler        15Jun00         creation
//
// **************************************************************************

#ifndef __NVDEFVB_H
#define __NVDEFVB_H

// forward declarations

class CVertexBuffer;
template <int cdwLogMax> class CReferenceCountMap;

//---------------------------------------------------------------------------

class CDefaultVB
{

private:
    CVertexBuffer           *m_pVertexBuffer;
    CReferenceCountMap<10>  *m_pRefCountMap;

public:
    DWORD                    m_dwCurrentOffset;
    DWORD                    m_dwTotalVerticesSincePut;

public:
    // inline functions
    inline CVertexBuffer* getVB (void)      {   return (m_pVertexBuffer);   }

    // prototypes
    BOOL  create             (DWORD dwHeap, DWORD dwSize);
    BOOL  destroy            (void);
    DWORD getHeap            (void);
    DWORD getAddress         (void);
    DWORD getSize            (void);
    DWORD getOffset          (void);
    DWORD getCachedOffset    (void);
    void  updateOffset       (DWORD dwOffset);
    void  updateMap          (void);
    DWORD waitForSpace       (int iSize, BOOL bWait);

private:
    DWORD dispatchWrapAround (int iVBOffset);

    // construction
public:
    CDefaultVB  (void);
    ~CDefaultVB (void);

    // friends
    friend void nvCelsiusILCompile_computeIndexAndOffset (DWORD dwVBStride, DWORD dwLogStride);

};

#endif  // __NVDEFVB_H
