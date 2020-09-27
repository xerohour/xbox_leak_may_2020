// **************************************************************************
//
//       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.
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
// **************************************************************************
//
//  Module: nvPusher_link.cpp
//      a pointer to a shared file
//
// **************************************************************************

#define PDEV_PTR() m_ppdev

// redefine pusher aliases to local environment (NT4)

//#define _pdwHWGet      These reference internal pusher vars and are therefore okay
//#define _pdwHWPut      
//#define _pdwHWRef
#define _pbHWBusy       (*(volatile DWORD *)(m_ppdev->pDriverData->NvBaseFlat + 0x400700))
#define _ffCache1Push1  (*(volatile DWORD *)(m_ppdev->pDriverData->NvBaseFlat + 0x003204))
#define _ffCache1Status (*(volatile DWORD *)(m_ppdev->pDriverData->NvBaseFlat + 0x003214))
#define _hasKNI         (m_ppdev->pDriverData->sysInfo.dwCPUFeatureSet & FS_KATMAI)
#define _has3DNOW       (m_ppdev->pDriverData->sysInfo.dwCPUFeatureSet & FS_3DNOW)
#define _hasSFENCE      (m_ppdev->pDriverData->sysInfo.dwCPUFeatureSet & FS_SFENCE)
#define _hasFullScreenDosOccurred (m_ppdev->pDriverData->fFullScreenDosOccurred)
#define _systemInfo (m_ppdev->pDriverData->sysInfo)
#define _dmaPusherChannelIndex (m_ppdev->vppChannelNdx)
#define _dwRootHandle (m_ppdev->pDriverData->dwRootHandle)
#define _nvBaseFlat ((volatile DWORD*)m_ppdev->pDriverData->NvBaseFlat)
#define _pCurrentChannelID ((DWORD*)(m_ppdev->pDriverData->pCurrentChannelID))
#define _pDriverData (m_ppdev->pDriverData)
#define _hDevice (m_ppdev->hDevice)

// By redefining this class to nothing, and copying these consts, we effectively rewrite some code in
// the pusher class that depends on the CSimpleSurface class which I'm not ready to port. @mjl@
#define CSimpleSurface
enum
{
    // used to specify where we want to allocate a surface (or'ed together)
    // or where it's currently allocated
    HEAP_LOCATION_MASK      = 0x0000001f,   // surface location - preserve order
    HEAP_VID                = 0x01,
    HEAP_AGP                = 0x02,
    HEAP_PCI                = 0x04,
    HEAP_SYS                = 0x08,
    HEAP_DYNSYS             = 0x18,
    HEAP_DYNSYSBIT          = 0x10,
    HEAP_ANYWHERE           = HEAP_VID | HEAP_AGP | HEAP_PCI | HEAP_SYS | HEAP_DYNSYS,
    HEAP_NOWHERE            = 0x0
};

#define HEAP_SHARED                 0x04000000UL    // put heap in shared memory

#include "..\..\..\..\common\src\nvPusher.cpp"

