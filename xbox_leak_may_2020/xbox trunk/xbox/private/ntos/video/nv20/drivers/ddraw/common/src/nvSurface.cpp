//**************************************************************************
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
// **************************************************************************
#include "nvprecomp.h"

//****************************************************************************
//****************************************************************************
//****************************************************************************
//**  CSimpleSurface *********************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************

// convert from heap location to context dma
DWORD g_adwSurfaceContextDMA[9] =
{
    /* undefined       */ 0,
    /* HEAP_VID = 0x01 */ NV_CONTEXT_DMA_VID,
    /* HEAP_AGP = 0x02 */ NV_CONTEXT_DMA_AGP_OR_PCI,
    /* undefined       */ 0,
    /* HEAP_PCI = 0x04 */ NV_CONTEXT_DMA_AGP_OR_PCI,
    /* undefined       */ 0,
    /* undefined       */ 0,
    /* undefined       */ 0,
    /* HEAP_SYS = 0x08 */ NV_CONTEXT_DMA_NONE
};

// helper function
inline DWORD nvGetOffsetFromLinear(DWORD dwLinearAddress)
{
    NV_CFGEX_GET_AGP_OFFSET_PARAMS addr;

    addr.linaddr = (void*)dwLinearAddress;
    NvRmConfigGetEx(pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFGEX_GET_AGP_OFFSET, &addr, sizeof(addr));
    return addr.offset;
}

//****************************************************************************
// CSimpleSurface::isBusy
//

BOOL CSimpleSurface::isBusy (void) const
{
    dbgTracePush ("CSimpleSurface::isBusy");

    // check shadow
    if (getDC()->pRefCount->readShadow() >= m_dwRetireDate) {
        dbgTracePop();
        return FALSE;
    }

    // read HW and recheck
    if (getDC()->pRefCount->read() >= m_dwRetireDate) {
        dbgTracePop();
        return FALSE;
    }

    // return that the surface is busy
    dbgTracePop();
    return TRUE;
}

//****************************************************************************
// CSimpleSurface::reset
//
// surface is lost - tag as such

void CSimpleSurface::reset
(
    void
)
{
    dbgTracePush ("CSimpleSurface[%08x]::reset()",this);
    // disown memory
    disown();
    dbgTracePop();
}

//****************************************************************************
// CSimpleSurface::own
//
// let surface own a piece of memory that is allocated by someone else
//  unless explicitly tagged as a non-owner, the surface will be freed
//  at destroy time.

void CSimpleSurface::own
(
    DWORD dwAddress,
    DWORD dwPitch,
    DWORD dwHeight,
    DWORD dwHeapLocation,
    BOOL  bOwnMemory
)
{
    // tell world
    dbgTracePush ("CSimpleSurface[%08x]::own(dwAddress=%08x,dwPitch=%08x,dwH=%08x,dwHeapLoc=%x,bOwnMem=%d)",
                  this,dwAddress,dwPitch,dwHeight,dwHeapLocation,bOwnMemory);

    // assign
    m_dwAddress    = dwAddress;
    m_dwPitch      = dwPitch;
    m_dwHeight     = dwHeight;
    m_dwFlags     &= ~HEAP_LOCATION_MASK;
    m_dwFlags     |= dwHeapLocation;
    m_dwRetireDate = 0;
    if (bOwnMemory) m_dwFlags &= ~FLAG_DONOTOWNMEMORY;
               else m_dwFlags |= FLAG_DONOTOWNMEMORY;

    switch (getHeapLocation())
    {
        case HEAP_VID:
            m_dwOffset = m_dwAddress - VIDMEM_ADDR(pDriverData->BaseAddress);
            break;
        case HEAP_AGP:
#ifdef NV_AGP
            m_dwOffset = nvGetOffsetFromLinear(m_dwAddress);
            DPF_LEVEL(NVDBG_LEVEL_NV_AGP, "CSimpleSurface::Own: offset of AGP address %08x is %08x", m_dwAddress, m_dwOffset);
#else
            m_dwOffset = AGPMEM_OFFSET(m_dwAddress);
#endif
            break;
        case HEAP_PCI:
#ifdef WINNT // WINNT Temp Hack
        if (!getDC()->nvD3DTexHeapData.dwBase)
            m_dwOffset = m_dwAddress - getDC()->nvPusher.getBase();
        else
#endif // WINNT Temp Hack
            m_dwOffset = m_dwAddress - getDC()->nvD3DTexHeapData.dwBase;
            break;
        case HEAP_SYS:
            m_dwOffset = m_dwAddress;
            break;
        default:
            DPF ("unknown context");
            dbgD3DError();
            break;
    }

    dbgTracePop();
}

//****************************************************************************
// CSimpleSurface::disown
//
// remove surface ownership

void CSimpleSurface::disown
(
    void
)
{
    dbgTracePush ("CSimpleSurface[%08x]::disown()",this);
    // tag as invalid
    m_dwAddress = 0;
    m_dwFlags  &= ~HEAP_LOCATION_MASK;
    dbgTracePop();
}

//****************************************************************************
// CSimpleSurface::hwUnlock
//

void CSimpleSurface::hwUnlock
(
    void
)
{
    dbgTracePush ("CSimpleSurface::hwUnlock");
    // write reference count into push buffer and keep
    //  the value. HW has to reach this point before CPU can
    //  get lock access

    //added a check for surfaces that the HW could actually touch
    //should we actually check each surface instead of calling HWUnlock?
    //HMH

    if (hwCanRead() || hwCanWrite()) {
        m_dwRetireDate = getDC()->pRefCount->inc (CReferenceCount::INC_LAZY);
    }
    dbgTracePop();
}

//****************************************************************************
// CSimpleSurface::cpuLock
//

void CSimpleSurface::cpuLock
(
    DWORD //dwAccess
)
{
    DDSTARTTICK(SURF7_SPINLOCK);
    dbgTracePush ("CSimpleSurface::cpuLock");

    //The next check has been removed because looks like the only situation
    //we used it for was for a SystemMemorySurface artificially created in
    //CreateSurfaceEx. Ironically that is exactly the case where we need
    //synchronization now. Otherwise we pull a chair from under the blits
    //that are still in progress when the surface is being destroyed. Andrei O.
#if 1
    // if the surface is not touchable by HW, return immediately
    //
    //reenabled due to change in hwCanRead to reflect true value for Dynamic ContextDMAs
    //
    //note that hwCanWrite is currently redundant
    //
    //note also that this is simply an optimization.  surfaces that the HW can't touch
    //shouldn't be effected by HWLock/HWUnlock
    //
    // HMH
    if (!hwCanRead() && !hwCanWrite()) {
        DDENDTICK(SURF7_SPINLOCK);
        dbgTracePop();
        return;
    }
#endif

    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
        // if the channel's been destroyed, then everything is retired by definition
        DDENDTICK(SURF7_SPINLOCK);
        dbgTracePop();
        return;
    }

    // this is needed to prevent hangs when returning from a full-screen DOS box
    // waiting for a notifier with the FIFO in this state guarantees a hang
    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_D3D) {
        DPF("CSimpleSurface::cpuLock: Early exit due to DosOccurred flag");
        dbgTracePop();
        DDENDTICK(SURF7_SPINLOCK);
        return;
    }

    // spin until surface is unlocked by HW
    getDC()->pRefCount->wait (CReferenceCount::WAIT_PREEMPTIVE, m_dwRetireDate);

    dbgTracePop();
    DDENDTICK(SURF7_SPINLOCK);

 }

//****************************************************************************
// CSimpleSurface::swap
//
// swap surface pointers

void CSimpleSurface::swap
(
    CSimpleSurface* pSurface
)
{
    dbgTracePush ("CSimpleSurface[%08x]::swap(pSurface=%08x)",this,pSurface);

    // sanity check
    assert (this);
    assert (isValid());
    assert (pSurface);
    assert (pSurface->isValid());

    NV_SWAP_1 (m_dwAddress,      pSurface->m_dwAddress);
    NV_SWAP_1 (m_dwOffset,       pSurface->m_dwOffset);
    NV_SWAP_1 (m_dwFormat,       pSurface->m_dwFormat);
    NV_SWAP_1 (m_dwBPP,          pSurface->m_dwBPP);
    NV_SWAP_1 (m_dwBPPRequested, pSurface->m_dwBPPRequested);
    NV_SWAP_1 (m_dwWidth,        pSurface->m_dwWidth);
    NV_SWAP_1 (m_dwHeight,       pSurface->m_dwHeight);
    NV_SWAP_1 (m_dwDepth,        pSurface->m_dwDepth);
    NV_SWAP_1 (m_dwPitch,        pSurface->m_dwPitch);
    NV_SWAP_1 (m_dwFourCC,       pSurface->m_dwFourCC);
    NV_SWAP_1 (m_dwFlags,        pSurface->m_dwFlags);
    NV_SWAP_1 (m_dwRetireDate,   pSurface->m_dwRetireDate);

    dbgTracePop();
}

//****************************************************************************
// CSimpleSurface::create
//
// creates a surface in one of the the allowed heaps based on preference.

BOOL CSimpleSurface::create
(
    DWORD dwPitch,
    DWORD dwHeight,
    DWORD dwDepth,
    DWORD dwBPPGranted,
    DWORD dwAllowedHeaps,
    DWORD dwPreferredHeap,
    DWORD dwAllocFlags
#ifdef CAPTURE
   ,DWORD dwUsageKind  // CAPTURE_SURFACE_KIND_xxx
#endif
)
{
#ifdef DEBUG
    // just so we can see what was asked for when debugging
    DWORD dwOriginalAllowedHeaps  = dwAllowedHeaps;
    DWORD dwOriginalPreferredHeap = dwPreferredHeap;
#endif

    // tell world
    dbgTracePush ("CSimpleSurface[%08x]::create(dwPitch=%08x,dwHt=%08x,dwDp=%08x,dwAllHeaps=%x,dwPrefHeap=%x,dwFlags=%x)",
                 this, dwPitch, dwHeight, dwDepth, dwAllowedHeaps, dwPreferredHeap, dwAllocFlags);

    DWORD dwSize = dwPitch * dwHeight * dwDepth;

    // Allow for a possible ZERO sized surface
    if (!dwSize) {
        // complain
        DPF ("CSimpleSurface::create: dwSize == 0");
        dbgD3DError();
        // tag surface as invalid
        m_dwAddress = 0;
        dbgTracePop();
        return (FALSE);
    }

    // retire surface (since it is about to exist)
    m_dwRetireDate = 0;

    // no specific preference would start with video memory
    if (dwPreferredHeap == HEAP_NOWHERE) {
        dwPreferredHeap = HEAP_VID;
    }

    // if we require tiled memory - override to only allow vid heap
    if (dwAllocFlags & ALLOCATE_TILED) {
        dwAllowedHeaps  = HEAP_VID;
        dwPreferredHeap = HEAP_VID;
    }

    // suppress video memory allocation all together if it is overidden
    //  by the registy
    if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_VIDEOTEXTUREENABLE_MASK) == D3D_REG_VIDEOTEXTUREENABLE_DISABLE)
    {
        dwAllowedHeaps &= ~HEAP_VID;
        if (!dwAllowedHeaps) dwAllowedHeaps = HEAP_AGP | HEAP_PCI;
    }
#if 0 // moved
    else
    {
        // override the preference to the AGP heap if
        //  - it would be faster
        //  - it is allowed
        //  - and the preference indicated the slower option
        if ((global.dwPerformanceStrategy & PS_TEXTURE_PREFER_AGP)
         && ((dwAllowedHeaps & (HEAP_VID | HEAP_AGP)) == (HEAP_VID | HEAP_AGP))
         && (dwPreferredHeap == HEAP_VID))
        {
            // override to agp
            dwPreferredHeap = HEAP_AGP;
        }
    }
#endif

    m_dwAllocFlags = dwAllocFlags;

    // try to allocate in all allowed memory heaps
    while (dwAllowedHeaps)
    {
        // video
        if (dwAllowedHeaps & dwPreferredHeap & HEAP_VID)
        {
            DWORD dwStatus, dwType;

            if (dwAllocFlags & ALLOCATE_TILED)
            {
                dwType = (dwAllocFlags & ALLOCATE_AS_ZBUFFER_UNC) ?
                             TYPE_DEPTH :
                             ((dwAllocFlags & ALLOCATE_AS_ZBUFFER_CMP) ?
                                 ((dwBPPGranted == 2) ?
                                     TYPE_DEPTH_COMPR16 :
                                     TYPE_DEPTH_COMPR32) :
                                 TYPE_IMAGE);
#ifdef WINNT
                NVHEAP_ALLOC_TILED (dwStatus, m_dwOffset, dwPitch, dwHeight, dwType);
                m_dwAddress = VIDMEM_ADDR(m_dwOffset);
#else  // !WINNT
                NVHEAP_ALLOC_TILED (dwStatus, m_dwAddress, dwPitch, dwHeight, dwType);
#endif // !WINNT
#ifdef DEBUG_SURFACE_PLACEMENT
                m_dwRMAllocFlags = dwType;
#endif
                nvAssert (dwStatus == 0);
            }

            else
            {
                dwType = (dwAllocFlags & ALLOCATE_AS_ZBUFFER_UNC) ?
                             TYPE_DEPTH :
                             ((dwAllocFlags & ALLOCATE_AS_ZBUFFER_CMP) ?
                                 ((dwBPPGranted == 2) ?
                                     TYPE_DEPTH_COMPR16 :
                                     TYPE_DEPTH_COMPR32) :
                                 TYPE_TEXTURE);
#ifdef WINNT
                NVHEAP_ALLOC (dwStatus, m_dwOffset, dwSize, dwType);
                m_dwAddress = VIDMEM_ADDR(m_dwOffset);
#else
                NVHEAP_ALLOC (dwStatus, m_dwAddress, dwSize, dwType);
#endif // WINNT
#ifdef DEBUG_SURFACE_PLACEMENT
                m_dwRMAllocFlags = dwType;
#endif
            }

            if (dwStatus == 0) {
                // increment count of video memory surfaces allocated.
                pDriverData->DDrawVideoSurfaceCount++;
                m_dwPitch     = dwPitch;
                m_dwHeight    = dwHeight;
                m_dwDepth     = dwDepth;
#ifndef WINNT
                m_dwOffset    = m_dwAddress - pDriverData->BaseAddress;
#endif // WINNT
                m_dwFlags    &= ~HEAP_LOCATION_MASK;
                m_dwFlags    |= HEAP_VID;
                m_dwFlags    &= ~FLAG_TILED;
                m_dwFlags    |= (dwAllocFlags & ALLOCATE_TILED)      ? FLAG_TILED   : 0;
                m_dwFlags    |= (dwAllocFlags & ALLOCATE_AS_ZBUFFER) ? FLAG_ZBUFFER : 0;

#ifdef DEBUG
                DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"Vid Surface Alloc: Address:%08x Size:%08x\n",m_dwAddress,dwSize);
#endif

#ifdef CAPTURE
                if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
                    CAPTURE_SURFACE_ALLOC surf;
                    surf.dwJmpCommand         = CAPTURE_JMP_COMMAND;
                    surf.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
                    surf.dwExtensionID        = CAPTURE_XID_SURFACE_ALLOC;
                    surf.dwCtxDMAHandle       = NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY;
                    surf.dwOffset             = m_dwOffset;
                    surf.dwSize               = getSize();
                    surf.dwAlignment          = 0;
                    surf.dwType               = (dwAllocFlags & ALLOCATE_AS_ZBUFFER) ? CAPTURE_SURFACE_TYPE_ZETA : CAPTURE_SURFACE_TYPE_NORMAL;
                    surf.dwKind               = dwUsageKind;
                    surf.dwSurfaceType        = getFormat();
                    surf.dwIntendedPitch      = dwPitch;
                    surf.dwIntendedHeight     = dwHeight;
                    captureLog (&surf,sizeof(surf));
                }
#endif
                dbgTracePop();
                return (TRUE);
            }

#ifdef DEBUG
                DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"Vid Surface Alloc Failed: Size:%08x\n",dwSize);
#endif
            // fail - don't try again
            DPF_LEVEL(NVDBG_LEVEL_INFO,"CSimpleSurface::create: Surface in VID failed");
            dwAllowedHeaps &= ~HEAP_VID;
        }

        // agp
        if (dwAllowedHeaps & dwPreferredHeap & HEAP_AGP)
        {
            if (pDriverData->GARTLinearBase)
            {
                // On AGP, allocate call DDRAW to allocate the AGP memory.
                m_dwAddress = (DWORD)nvAGPAlloc(dwSize);
                if (m_dwAddress != ERR_DXALLOC_FAILED)
                {
#ifdef NV_AGP
                    m_dwOffset   = nvGetOffsetFromLinear(m_dwAddress);
                    DPF_LEVEL(NVDBG_LEVEL_NV_AGP, "CSimpleSurface::Create: offset of AGP address %08x is %08x", m_dwAddress, m_dwOffset);
#else
                    // at this point m_dwAddress is actually an offset
                    m_dwAddress  = AGPMEM_ADDR(m_dwAddress);
                    m_dwOffset   = AGPMEM_OFFSET(m_dwAddress);
#endif
                    m_dwPitch    = dwPitch;
                    m_dwHeight   = dwHeight;
                    m_dwDepth    = dwDepth;
                    m_dwFlags   &= ~HEAP_LOCATION_MASK;
                    m_dwFlags   |= HEAP_AGP;
                    m_dwFlags   &= ~FLAG_TILED;
#ifdef DEBUG
                    DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"AGP Surface Alloc: Address:%08x Size:%08x\n",m_dwAddress,dwSize);
#endif

#ifdef CAPTURE
                    if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
                        CAPTURE_SURFACE_ALLOC surf;
                        surf.dwJmpCommand         = CAPTURE_JMP_COMMAND;
                        surf.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
                        surf.dwExtensionID        = CAPTURE_XID_SURFACE_ALLOC;
                        surf.dwCtxDMAHandle       = D3D_CONTEXT_DMA_HOST_MEMORY;
                        surf.dwOffset             = m_dwOffset;
                        surf.dwSize               = getSize();
                        surf.dwAlignment          = 0;
                        surf.dwType               = CAPTURE_SURFACE_TYPE_NORMAL;
                        surf.dwKind               = dwUsageKind;
                        surf.dwSurfaceType        = getFormat();
                        surf.dwIntendedPitch      = dwPitch;
                        surf.dwIntendedHeight     = dwHeight;
                        captureLog (&surf,sizeof(surf));
                    }
#endif
                    dbgTracePop();
                    return (TRUE);
                }
            }
            // fail - don't try again
#ifdef DEBUG
                DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"AGP Surface Alloc Failed: Size:%08x\n",dwSize);
#endif
            DPF_LEVEL(NVDBG_LEVEL_INFO,"CSimpleSurface::create: Surface in AGP failed");
            dwAllowedHeaps &= ~HEAP_AGP;
        }

        // pci
        if (dwAllowedHeaps & dwPreferredHeap & HEAP_PCI)
        {
            DWORD dwOffset;

            // On PCI, allocate from our internal texture heap.
            dwOffset = nvPCIAlloc(dwSize);
            if (dwOffset)
            {
                m_dwPitch     = dwPitch;
                m_dwHeight    = dwHeight;
                m_dwDepth     = dwDepth;
                m_dwOffset    = dwOffset;
                m_dwAddress   = getDC()->nvD3DTexHeapData.dwBase + dwOffset;
                m_dwFlags    &= ~HEAP_LOCATION_MASK;
                m_dwFlags    |= HEAP_PCI;
                m_dwFlags    &= ~FLAG_TILED;

#ifdef DEBUG
                DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"PCI Surface Alloc: Address:%08x Size:%08x\n",m_dwAddress,dwSize);
#endif

#ifdef CAPTURE
                if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
                    CAPTURE_SURFACE_ALLOC surf;
                    surf.dwJmpCommand         = CAPTURE_JMP_COMMAND;
                    surf.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
                    surf.dwExtensionID        = CAPTURE_XID_SURFACE_ALLOC;
                    surf.dwCtxDMAHandle       = D3D_CONTEXT_DMA_HOST_MEMORY;
                    surf.dwOffset             = m_dwOffset;
                    surf.dwSize               = getSize();
                    surf.dwAlignment          = 0;
                    surf.dwType               = CAPTURE_SURFACE_TYPE_NORMAL;
                    surf.dwKind               = dwUsageKind;
                    surf.dwSurfaceType        = getFormat();
                    surf.dwIntendedPitch      = dwPitch;
                    surf.dwIntendedHeight     = dwHeight;
                    captureLog (&surf,sizeof(surf));
                }
#endif
                dbgTracePop();
                return (TRUE);
            }

#ifdef DEBUG
            DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"PCI Surface Alloc Failed: Size:%08x\n",dwSize);
#endif

            // fail - don't try again
            DPF_LEVEL(NVDBG_LEVEL_INFO,"CSimpleSurface::create: Surface in PCI failed");
            dwAllowedHeaps &= ~HEAP_PCI;
        }

        // system memory
        if (dwAllowedHeaps & dwPreferredHeap & HEAP_SYS)
        {
            // For system memory, allocate from global heap.
            // m_dwOffset keeps the original ptr while
            // m_dwAddress keeps the aligned ptr
#ifdef WINNT
            PDD_SURFACE_LOCAL pDDSLcl = getWrapper() ? getWrapper()->getDDSLcl() : NULL;
            if (pDDSLcl)
            {
                m_dwOffset = (DWORD)EngAllocPrivateUserMem(pDDSLcl, dwSize + 31, 'x_VN');
//                getWrapper()->setDDSLcl(NULL);  We shouldn't keep Lcl around, but we have to for texture
                //management    HH
            }
            else
#endif // WINNT
            {
                m_dwOffset = (DWORD) AllocIPM (dwSize);
                nvAssert (((m_dwOffset + 31) & ~31) == m_dwOffset);
                //GlobalAlloc(GMEM_FIXED, dwSize + 31);
            }

            if (m_dwOffset)
            {
                m_dwPitch     = dwPitch;
                m_dwHeight    = dwHeight;
                m_dwDepth     = dwDepth;
                m_dwAddress   = (m_dwOffset + 31) & ~31;
                m_dwFlags    &= ~HEAP_LOCATION_MASK;
                m_dwFlags    |= HEAP_SYS;
                m_dwFlags    &= ~FLAG_TILED;

#ifdef DEBUG
                DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"SYS Surface Alloc: Address:%08x Size:%08x\n",m_dwAddress,dwSize);
#endif

                dbgTracePop();
                return (TRUE);
            }

            // fail - don't try again
#ifdef DEBUG
            DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"SYS Surface Alloc Failed: Size:%08x\n",dwSize);
#endif
            DPF_LEVEL(NVDBG_LEVEL_INFO,"CSimpleSurface::create: Surface in SYS failed");
            dwAllowedHeaps &= ~HEAP_SYS;
        }

        // next
        dwPreferredHeap = (dwPreferredHeap << 1) & HEAP_ANYWHERE;
        if (!dwPreferredHeap) dwPreferredHeap = HEAP_VID;
    }

    // not allocated
    //  tag as invalid and fail
    DPF_LEVEL(NVDBG_LEVEL_INFO,"CSimpleSurface::create: Surface Not allocated");
    m_dwAddress = 0;
    dbgTracePop();
    return (FALSE);
}

//****************************************************************************

BOOL CSimpleSurface::create
(
    const CSimpleSurface* pTemplateSurface
#ifdef CAPTURE
   ,DWORD dwUsageKind
#endif
)
{
    DWORD dwHeap, dwAllocFlags;
    BOOL  bRV;

    dbgTracePush ("CSimpleSurface[%08x]::create(pTemplateSurface=%08x)",this,pTemplateSurface);
    // create in same heap as original
    dwHeap = pTemplateSurface->getHeapLocation();
    dwAllocFlags  = CSimpleSurface::ALLOCATE_SIMPLE;
    dwAllocFlags |= pTemplateSurface->isZBuffer() ? CSimpleSurface::ALLOCATE_AS_ZBUFFER_CMP : 0;

    bRV = create (pTemplateSurface->getSize(),
                  1,
                  1,
                  pTemplateSurface->getBPP(),
                  dwHeap,
                  dwHeap,
                  dwAllocFlags
#ifdef CAPTURE
                 ,dwUsageKind
#endif
                  );
    dbgTracePop();
    return (bRV);
}

//****************************************************************************
// CSimpleSurface::create for formatted surfaces
//
// creates a surface with format information

BOOL CSimpleSurface::create (CNvObject *pWrapperObject,
                             DWORD      dwFormat,
                             DWORD      dwWidth,
                             DWORD      dwHeight,
                             DWORD      dwDepth,
                             DWORD      dwMultiSampleBits,
                             DWORD      dwBPPRequested,
                             DWORD      dwBPPGranted,
                             DWORD      dwPitch,
                             DWORD      dwAllowedHeaps,
                             DWORD      dwPreferredHeap,
                             DWORD      dwAllocFlags
#ifdef CAPTURE
                            ,DWORD      dwUsageKind
#endif
                             )
{
    dbgTracePush ("CSimpleSurface::create");

    setWrapper      (pWrapperObject);
    setFormat       (dwFormat);
    setWidth        (dwWidth);
    setHeight       (dwHeight);
    setDepth        (dwDepth);
    setMultiSampleBits(dwMultiSampleBits);
    setBPPRequested (dwBPPRequested);
    setBPP          (dwBPPGranted);

#ifdef  STEREO_SUPPORT
	if (dwAllocFlags & ALLOCATE_AS_STEREO && STEREODATA(dwStereoHWType) == STEREOCFG_INTERLACED) 
		dwPitch = GetModePitch(dwWidth, dwHeight, dwBPPGranted << 3) * 2;
#endif  //STEREO_SUPPORT

    BOOL bRV = create (dwPitch, dwHeight, dwDepth, dwBPPGranted, dwAllowedHeaps, dwPreferredHeap, dwAllocFlags
#ifdef CAPTURE
                                      ,dwUsageKind
#endif
                                       );
#ifdef  STEREO_SUPPORT
	if (bRV && dwAllocFlags & ALLOCATE_AS_STEREO) 
		bRV = createStereo ();
#endif  //STEREO_SUPPORT

    dbgTracePop();
    return (bRV);
}

//****************************************************************************
// CSimpleSurface::recreate for formatted surfaces
//
// recreate a surface with a different bit depth

BOOL CSimpleSurface::recreate (DWORD dwBPP)
{
    // cache off current info
    CNvObject *pNvObj   = getWrapper();
    DWORD      dwFormat = getFormat();
    DWORD      dwWidth  = getWidth();
    DWORD      dwHeight = getHeight();
    DWORD      dwMSBits = getMultiSampleBits();
    DWORD      dwBPPReq = getBPPRequested();
    DWORD      dwPitch  = dwBPP * dwWidth;
    DWORD      dwAFlags = getAllocFlags();
#ifdef  STEREO_SUPPORT
    BOOL       bStereo = isStereo();
    BOOL       bRetVal; 
    // destroy and reallocate
    destroy();
	if (bStereo && (m_dwFlags &  FLAG_NOTREAL) == 0 && STEREODATA(dwStereoHWType) == STEREOCFG_INTERLACED) 
		dwPitch = GetModePitch(dwWidth, dwHeight, dwBPP << 3) * 2;
    bRetVal = create (pNvObj, dwFormat, dwWidth, dwHeight, 1,
                    dwMSBits, dwBPPReq, dwBPP, dwPitch,
                    CSimpleSurface::HEAP_VID, CSimpleSurface::HEAP_VID, dwAFlags
            #ifdef CAPTURE
                   ,CAPTURE_SURFACE_KIND_ZETA
            #endif
                     );
    if (bStereo)
        createStereo();
    return bRetVal;
#else   //STEREO_SUPPORT
    // destroy and reallocate
    destroy();
    return (create (pNvObj, dwFormat, dwWidth, dwHeight, 1,
                    dwMSBits, dwBPPReq, dwBPP, dwPitch,
                    CSimpleSurface::HEAP_VID, CSimpleSurface::HEAP_VID, dwAFlags
            #ifdef CAPTURE
                   ,CAPTURE_SURFACE_KIND_ZETA
            #endif
                   )
           );
#endif  //STEREO_SUPPORT
}

//****************************************************************************
// CSimpleSurface::growTo
//
BOOL CSimpleSurface::growBy
(
    DWORD dwDelta,
    BOOL  bPreserveContents
)
{
    dbgTracePush ("CSimpleSurface::create");

    // currently we only grow sysmem surfaces (sorry)
    if (getHeapLocation() != HEAP_SYS)
    {
        nvAssert(0);
        dbgTracePop();
        return FALSE;
    }

    DWORD dwSize = getSize() + dwDelta + m_dwPitch - 1;
    dwSize /= m_dwPitch;
    dwSize *= m_dwPitch;

    // For system memory, allocate from global heap.
    // m_dwOffset keeps the original ptr while
    // m_dwAddress keeps the aligned ptr
#ifdef WINNT
    PDD_SURFACE_LOCAL pDDSLcl = getWrapper() ? getWrapper()->getDDSLcl() : NULL;
    if (pDDSLcl)
    {
        void *ptr = EngAllocPrivateUserMem(pDDSLcl, dwSize+31, 'x_VN');
        if (ptr)
        {
            if (bPreserveContents)
            {
                DWORD dw = (DWORD(ptr) + 31) & ~31;
                nvMemCopy (dw,m_dwAddress,getSize());
            }
            EngFreePrivateUserMem (pDDSLcl, (void*)m_dwOffset);
            m_dwOffset = (DWORD)ptr;
        }
        else
        {
            // fail safely
            dbgTracePop();
            return FALSE;
        }
    }
    else
#endif // WINNT
    {
        void *ptr = AllocIPM(dwSize);
        if (ptr)
        {
            if (bPreserveContents)
            {
                DWORD dw = (DWORD(ptr) + 31) & ~31;
                nvMemCopy (dw,m_dwAddress,getSize());
            }
            FreeIPM ((void*)m_dwOffset);
            m_dwOffset = (DWORD)ptr;
        }
        else
        {
            // fail safely
            dbgTracePop();
            return FALSE;
        }
    }

    // update state
    m_dwHeight  = dwSize / m_dwPitch;
    m_dwAddress = (m_dwOffset + 31) & ~31;

    DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"SYS Surface Realloc: Address:%08x Size:%08x\n",m_dwAddress,dwSize);

    dbgTracePop();
    return TRUE;
}

//****************************************************************************
// CSimpleSurface::destroy
//
// frees a surface

BOOL CSimpleSurface::destroy
(
    void
)
{
    dbgTracePush ("CSimpleSurface[%08x]::destroy()",this);

    // validate surface
    //  destroying a invalid surface is technically a bad thing, but unfortunately mode
    //  switches will disown memory that is still considered allocated. we just return.
    if (!isValid())
    {
        DPF ("CSimpleSurface::destroy: Attempting to free an invalid surface (may not be serious)");
        dbgTracePop();
        return TRUE;
    }

    // cannot free memory until it is actually not in use any more
    cpuLock (LOCK_NORMAL);

    // capture surface free
#ifdef CAPTURE
    if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
        CAPTURE_SURFACE_FREE surf;
        surf.dwJmpCommand         = CAPTURE_JMP_COMMAND;
        surf.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
        surf.dwExtensionID        = CAPTURE_XID_SURFACE_FREE;
        surf.dwCtxDMAHandle       = (getHeapLocation() == CSimpleSurface::HEAP_VID)
                                    ? NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY
                                    : D3D_CONTEXT_DMA_HOST_MEMORY;
        surf.dwOffset             = m_dwOffset;
        captureLog (&surf,sizeof(surf));
    }
#endif

    // do we own the memory?
    if (m_dwFlags & FLAG_DONOTOWNMEMORY)
    {
        // disown this surface
        disown();
        dbgTracePop();
        return TRUE;
    }

    // free memory
    switch (getHeapLocation())
    {
        // video
        case HEAP_VID:
        {
            // Call DDRAW to de-allocate memory from the video memory heap.
#ifdef WINNT
            NVHEAP_FREE (m_dwOffset);
#else
            // we should only be trying to free this memory from 16 bit land when a mode switch is about
            // to happen. in this case the free isn't required since the RM will destroy the heap
            if (!global.b16BitCode)
            {
                NVHEAP_FREE (m_dwAddress);
            }
#endif

            // Decrement count of video memory surfaces allocated.
            pDriverData->DDrawVideoSurfaceCount--;

#ifdef DEBUG
            DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"VID Surface Free: Address:%08x\n",m_dwAddress);
#endif

            // done
            m_dwAddress = 0;
            m_dwFlags  &= ~HEAP_LOCATION_MASK;

#ifdef  STEREO_SUPPORT
            destroyStereo();
#endif  //STEREO_SUPPORT

            dbgTracePop();
            return TRUE;
        }
        // agp
        case HEAP_AGP:
        {
            // de-allocate memory from AGP heap
#ifdef NV_AGP
            nvAGPFree((void*)m_dwAddress);
#else
#ifdef WINNT
            nvAGPFree((void*)m_dwOffset);
#else
            nvAGPFree((void*)m_dwAddress);
#endif
#endif

#ifdef DEBUG
            DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"AGP Surface Free: Address:%08x\n",m_dwAddress);
#endif

            // done
            m_dwAddress = 0;
            m_dwFlags  &= ~HEAP_LOCATION_MASK;
            dbgTracePop();
            return TRUE;
        }
        // pci
        case HEAP_PCI:
        {
            // On PCI, de-allocate from our internal texture heap.
            nvPCIFree(m_dwAddress - getDC()->nvD3DTexHeapData.dwBase);

#ifdef DEBUG
            DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"PCI Surface Free: Address:%08x\n",m_dwAddress);
#endif
            // done
            m_dwAddress = 0;
            m_dwFlags  &= ~HEAP_LOCATION_MASK;
            dbgTracePop();
            return TRUE;
        }
        // sys
        case HEAP_SYS:
        {
            //  m_dwOffset has the original unaligned ptr
#ifdef WINNT
            PDD_SURFACE_LOCAL pDDSLcl = getWrapper() ? getWrapper()->getDDSLcl() : NULL;
            if (pDDSLcl)
            {
                EngFreePrivateUserMem(pDDSLcl, (void*)m_dwOffset);
                getWrapper()->setDDSLcl(NULL);
            }
            else
#endif // WINNT
            {
                FreeIPM ((void*) m_dwOffset);
                // GlobalFree ((HGLOBAL)m_dwOffset);
            }

#ifdef DEBUG
            DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"SYS Surface Free: Address:%08x\n",m_dwAddress);
#endif

            // done
            m_dwAddress = 0;
            m_dwFlags  &= ~HEAP_LOCATION_MASK;
            dbgTracePop();
            return TRUE;
        }
    }

    // failed (may be default push buffer - not allowed to be destroyed)
    DPF ("CSimpleSurface::destroy: Invalid heap specified. this surface is probably corrupt.");
    dbgD3DError();
    dbgTracePop();
    return FALSE;
}

#ifdef  STEREO_SUPPORT

BOOL CSimpleSurface::createStereo ()
    {
        BOOL    bRetVal = TRUE;
        DWORD   dwAllocFlags = 0;
        if (!STEREO_ENABLED)
            return TRUE;
        nvAssert (m_dwAddressRight == 0);
        if (isZBuffer())
            dwAllocFlags |= ALLOCATE_AS_ZBUFFER_UNC;
        if (m_dwFlags & FLAG_TILED)
            dwAllocFlags |= ALLOCATE_TILED;
        if (STEREODATA(dwStereoHWType) != STEREOCFG_INTERLACED || m_dwFlags & FLAG_NOTREAL)
        {
            //Save what will be overwritten.
            DWORD   dwSaveAddress = m_dwAddress;
            DWORD   dwSaveOffset  = m_dwOffset;
            if (bRetVal = create(m_dwPitch, m_dwHeight, 1, m_dwBPP,
                                 m_dwFlags & HEAP_LOCATION_MASK,
                                 m_dwFlags & HEAP_LOCATION_MASK, dwAllocFlags
#ifdef CAPTURE
                                 ,CAPTURE_SURFACE_KIND_UNKNOWN
#endif
                                 ))
            {
                m_dwAddressRight = m_dwAddress;
                m_dwOffsetRight  = m_dwOffset;
            } else
	            LOG("Can't create a stereo rendering surface");
            m_dwAddress = dwSaveAddress;
            m_dwOffset  = dwSaveOffset; 
        } else           
        {
			DWORD dwRightEyeOffset;
			dwRightEyeOffset = m_dwPitch >> 1;
			m_dwAddressRight = m_dwAddress + dwRightEyeOffset;
			m_dwOffsetRight  = m_dwOffset + dwRightEyeOffset;
        }
        tagAsStereo();
        if (m_dwFlags & FLAG_NOTREAL)
            return bRetVal;
        if (!isZBuffer())
        {
            nvAssert (STEREODATA(dwSurfaceCount) < 3);
            for (DWORD i = 0; i < 3; i++)
            {
                if (STEREODATA(RmStereoParams.FlipOffsets[i][0]) == 0xFFFFFFFF)
                {
                    //It is a free element
                    STEREODATA(RmStereoParams.FlipOffsets[i][0]) = m_dwOffset;
                    STEREODATA(RmStereoParams.FlipOffsets[i][1]) = m_dwOffsetRight;
                    STEREODATA(pRenderTargets[i]) = this;
                    STEREODATA(dwSurfaceCount)++;
                    return bRetVal;
                }
            }
            nvAssert (0);
        }
        return bRetVal;
    }

BOOL CSimpleSurface::destroyStereo (void)
    {
        if (isStereo())
        {
            m_dwFlags  &= ~FLAG_STEREO;
            if (STEREODATA(dwStereoHWType) != STEREOCFG_INTERLACED)
            {
#ifdef WINNT
                NVHEAP_FREE (m_dwOffsetRight);
#else
                NVHEAP_FREE (m_dwAddressRight);
#endif
                pDriverData->DDrawVideoSurfaceCount--;
            }
            m_dwAddressRight = 0;
            m_dwOffsetRight  = 0;
            if (m_dwFlags & FLAG_NOTREAL)
                return TRUE;
            if (!STEREO_ENABLED)
                return TRUE;
            if (!isZBuffer())
            {
                nvAssert (STEREODATA(dwSurfaceCount) > 0);
                STEREODATA(dwSurfaceCount)--;
                for (DWORD i = 0; i < 3; i++)
                {
                    if (STEREODATA(pRenderTargets[i]) == this)
                    {
                        //Found itself. Now clean it up.
                        STEREODATA(RmStereoParams.FlipOffsets[i][0]) = 0xFFFFFFFF;
                        STEREODATA(pRenderTargets[i]) = NULL;
                        return TRUE;
                    }
                }
            }
        }
        return TRUE;
    }
#endif  //STEREO_SUPPORT

//****************************************************************************
//****************************************************************************
//****************************************************************************
//**  CComplexSurface ********************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************

//****************************************************************************
// CComplexSurface::tagRenameEnable
//
// enable renaming and the renaming strategy to follow
template <int cMax>
void CComplexSurface<cMax>::tagRenameEnable
(
    DWORD           dwRenameType,
    DWORD           dwPreAllocCount,
    PCANRENAMEPROC  pfnCanRenameProc,
    PRENAMEPROC     pfnRenameProc,
    void           *pContext
)
{
    dbgTracePush ("CComplexSurface::tagRenameEnable");

    // update flags
    m_dwFlags &= ~FLAG_RENAME_CALL;
    m_dwFlags |=  FLAG_RENAME;

    // set new policy
    if (dwRenameType == RENAME_EXTERNAL)
    {
        m_pfnCanRenameProc     = pfnCanRenameProc;
        m_pfnRenameProc        = pfnRenameProc;
        m_pRenameContext       = pContext;
        m_dwFlags             |= FLAG_RENAME_CALL;
    }

    // allocate some of the surfaces already
    while (dwPreAllocCount > 1)
    {
        // alloc a surface
        dwPreAllocCount--;
        if (!m_aSurface[dwPreAllocCount].create(&m_aSurface[0]
#ifdef CAPTURE
            ,CAPTURE_SURFACE_KIND_TEXTURE
#endif
            )){
            DPF("tagRenameEnable: Failed to pre allocate surfaces\n");
            break;
        }
        m_dwTotalSurfaces++;
    }

    dbgTracePop();
}

//****************************************************************************
// CComplexSurface::forceRename
//
// rename surface on external demand
template <int cMax>
void CComplexSurface<cMax>::forceRename
(
    DWORD dwNewSurface,
    DWORD dwNewAddress
)
{
    // new surface retain most of its attributes
    DWORD dwPitch  = m_aSurface[m_dwActiveSurface].getPitch();
    DWORD dwHeight = m_aSurface[m_dwActiveSurface].getHeight();
    DWORD dwHeap   = m_aSurface[m_dwActiveSurface].getHeapLocation();
    m_aSurface[dwNewSurface].own (dwNewAddress,dwPitch,dwHeight,dwHeap,TRUE);

    // update flags
    m_dwPreviousSurface = m_dwActiveSurface;
    m_dwActiveSurface   = dwNewSurface;
    m_dwTotalSurfaces   = max(m_dwTotalSurfaces, (dwNewSurface+1));
}

//****************************************************************************
// CComplexSurface::cpuLock
//
// rename busy textures on demand when enabled

template <int cMax>
void CComplexSurface<cMax>::cpuLock
(
    DWORD dwAccess
)
{
    dbgTracePush ("CComplexSurface::cpuLock");

                #ifdef NV_PROFILE_CPULOCK
                NVP_START (NVP_T_FLOAT0);
                #endif

    // get proper index
    CSimpleSurface *pActive = getActiveSurface();

    // if we are allowed to rename and the surface is busy...
    if ((dwAccess & CSimpleSurface::LOCK_RENAME)                                    // renaming was requested (or is allowed)
     && canRename()                                                                 // surface has renaming enabled
     && pActive->isBusy()                                                           // surface is busy
     && (!(m_dwFlags & FLAG_RENAME_CALL)                                            // internal rename policy always OK
      || ((m_dwFlags & FLAG_RENAME_CALL) && m_pfnCanRenameProc(m_pRenameContext)))) // external rename policy active & OK
    {
        // search through all remaining surfaces
        BOOL  bAvailable      = FALSE;
        DWORD dwOldestDate    = ~0;
        DWORD dwOldestSurface = m_dwActiveSurface;
        DWORD dwNew;
        for(DWORD i = 1; i < m_dwTotalSurfaces; i++)
        {
            dwNew = m_dwActiveSurface + i;
            if (dwNew >= m_dwTotalSurfaces)
                dwNew -= m_dwTotalSurfaces;

            // check if we have a surface that isn't busy
            if (!m_aSurface[dwNew].isBusy()) {
                bAvailable = TRUE;
                break;
            }

            // update oldest pointer to point to the olders guy in this list
            if (m_aSurface[dwNew].getRetireDate() < dwOldestDate)
            {
                dwOldestDate    = m_aSurface[dwNew].getRetireDate();
                dwOldestSurface = dwNew;
            }
        }

        if (bAvailable)
        {
            // adopt the new surface
            m_dwPreviousSurface = m_dwActiveSurface;
            m_dwActiveSurface   = dwNew;
            pActive = getActiveSurface();
            // signal external rename policy of state, if needed
            if (m_dwFlags & FLAG_RENAME_CALL)
            {
                 //this call should really be made, but causes thrashing on some system in WinBench 1024x768x16 (test 10)
                (m_pfnRenameProc)(m_pRenameContext, m_dwPreviousSurface,m_dwActiveSurface);
            }
            else if (dwAccess & (CSimpleSurface::LOCK_RENAME_COPY & ~CSimpleSurface::LOCK_RENAME))
            {
                        #ifdef NV_PROFILE_COPIES
                        NVP_START(NVP_T_RENAME_AUTO_COPY);
                        #endif
                nvMemCopy (getSurface(m_dwActiveSurface)->getAddress(),
                           getSurface(m_dwPreviousSurface)->getAddress(),
                           getSurface(m_dwPreviousSurface)->getSize());
                        #ifdef NV_PROFILE_COPIES
                        NVP_STOP(NVP_T_RENAME_AUTO_COPY);
                        nvpLogTime (NVP_T_RENAME_AUTO_COPY,nvpTime[NVP_T_RENAME_AUTO_COPY]);
                        #endif
            }
        }
        else if (m_dwTotalSurfaces < cMax)
        {
            // create a new surface if we can
            dwNew = m_dwTotalSurfaces;
            if (m_aSurface[dwNew].create(&m_aSurface[m_dwActiveSurface]
#ifdef CAPTURE
               ,((m_dwFlags & FLAG_RENAME_CALL) && m_pfnCanRenameProc(m_pRenameContext)) ? CAPTURE_SURFACE_KIND_TEXTURE : CAPTURE_SURFACE_KIND_VERTEX
#endif
                ))
            {
                // signal external rename policy of state, if needed
                if (m_dwFlags & FLAG_RENAME_CALL)
                {
                    (m_pfnRenameProc)(m_pRenameContext, m_dwActiveSurface, dwNew);
                }
                else if (dwAccess & (CSimpleSurface::LOCK_RENAME_COPY & ~CSimpleSurface::LOCK_RENAME))
                {
                        #ifdef NV_PROFILE_COPIES
                        NVP_START(NVP_T_RENAME_AUTO_COPY);
                        #endif
                    nvMemCopy (getSurface(dwNew)->getAddress(),
                               getSurface(m_dwActiveSurface)->getAddress(),
                               getSurface(m_dwActiveSurface)->getSize());
                        #ifdef NV_PROFILE_COPIES
                        NVP_STOP(NVP_T_RENAME_AUTO_COPY);
                        nvpLogTime (NVP_T_RENAME_AUTO_COPY,nvpTime[NVP_T_RENAME_AUTO_COPY]);
                        #endif
                }

                // bump total
                m_dwTotalSurfaces++;

                // we'll take it
                m_dwPreviousSurface = m_dwActiveSurface;
                m_dwActiveSurface   = dwNew;
                pActive = getActiveSurface();
            }
            else
            {
                // we are all out of memory and all surfaces are busy - wait on the one that will retire first
                m_dwPreviousSurface = m_dwActiveSurface;
                m_dwActiveSurface   = dwOldestSurface;
                pActive = getActiveSurface();
            }
        }
        else
        {
            // we are all out of surfaces and all are busy - wait on the one that will retire first
            m_dwPreviousSurface = m_dwActiveSurface;
            m_dwActiveSurface   = dwOldestSurface;
            pActive = getActiveSurface();
        }
    }

    //  lock this surface (which may still be a busy surface)
// NVP_START(255);
            #ifdef NV_PROFILE_CPULOCK
            NVP_STOP (NVP_T_FLOAT0);
            nvpLogTime (NVP_T_FLOAT0,nvpTime[NVP_T_FLOAT0]);
            #endif

            #ifdef NV_PROFILE_CPULOCK
            NVP_START (NVP_T_FLOAT1);
            #endif
    pActive->cpuLock (dwAccess);
            #ifdef NV_PROFILE_CPULOCK
            NVP_STOP (NVP_T_FLOAT1);
            nvpLogTime (NVP_T_FLOAT1,nvpTime[NVP_T_FLOAT1]);
            #endif
// NVP_STOP(255);
// if (dwAccess & CSimpleSurface::LOCK_RENAME)
// {
    // __int64 time = nvpTime[255] / global.dwProcessorSpeed;
    // if (time >= 1000)
    // {
        // DPF ("%dus", (DWORD)time);
    // }
// }

    dbgTracePop();
}

//****************************************************************************
// CComplexSurface::create
//
// creates a surface in one of the the allowed heaps based on preference.
template <int cMax>
BOOL CComplexSurface<cMax>::create
(
    CNvObject *pWrapper,
    DWORD dwSize,
    DWORD dwAllowedHeaps,
    DWORD dwPreferredHeap
#ifdef CAPTURE
   ,DWORD dwUsageKind
#endif
)
{
    DWORD i;

    dbgTracePush ("CComplexSurface[%08x]::create(dwSize=%08x,dwAllowedHeaps=%x,dwPreferredHeap=%x)",
                 this,dwSize,dwAllowedHeaps,dwPreferredHeap);

    // set surface values
    m_dwTotalSurfaces = 1;
    m_dwActiveSurface = m_dwPreviousSurface = 0;
    //run down the simple surface list and make sure they all have the wrapper set BEFORE create.
    for (i=0;i<cMax;i++) m_aSurface[i].setWrapper(pWrapper);

    // create the first surface and return
    BOOL bRV = m_aSurface[0].create (dwSize, 1, 1, 0, dwAllowedHeaps, dwPreferredHeap,
                                     CSimpleSurface::ALLOCATE_SIMPLE
#ifdef CAPTURE
                                    ,dwUsageKind
#endif
                                     );

    dbgTracePop();
    return (bRV);
}

//****************************************************************************
// CComplexSurface::own
//
// claim an already allocated surface
template <int cMax>
BOOL CComplexSurface<cMax>::own
(
    DWORD dwAddress,
    DWORD dwPitch,
    DWORD dwLogHeight,
    DWORD dwHeapLocation,
    BOOL  bOwnMemory
)
{
    dbgTracePush ("CComplexSurface[%08x]::own(dwAddress=%08x,dwPitch=%08x,dwLogH=%08x,dwHeapLoc=%x,bOwnMem=%d)",
                  this,dwAddress,dwPitch,dwLogHeight,dwHeapLocation,bOwnMemory);

    // set surface values
    m_dwTotalSurfaces = 1;
    m_dwActiveSurface = m_dwPreviousSurface
                      = 0;

    // claim the first surface
    m_aSurface[0].own (dwAddress,dwPitch,dwLogHeight,dwHeapLocation,bOwnMemory);

    // done
    dbgTracePop();
    return TRUE;
}

//****************************************************************************
// CComplexSurface::disown
//
// claim an already allocated surface
template <int cMax>
BOOL CComplexSurface<cMax>::disown
(
    void
)
{
    dbgTracePush ("CComplexSurface::disown()");

    m_aSurface[0].disown();

    dbgTracePop();
    return TRUE;
}

//****************************************************************************
// CComplexSurface::swap
//
// interchange two complex surfaces

template <int cMax>
BOOL CComplexSurface<cMax>::swap
(
    CComplexSurface *pCSurf
)
{
    dbgTracePush ("CComplexSurface[%08x]::swap()", this);

    // get number of component surfaces
    DWORD dwCount = max (pCSurf->getTotalSurfaces(), getTotalSurfaces());

    // swap each component surface
    for (DWORD i=0; i<dwCount; i++) {
        m_aSurface[i].swap (&(pCSurf->m_aSurface[i]));
    }

    // done
    dbgTracePop();
    return (TRUE);
}

//****************************************************************************
// CComplexSurface::destroy
//
// frees a surface

template <int cMax>
BOOL CComplexSurface<cMax>::destroy
(
    void
)
{
    dbgTracePush ("CComplexSurface[%08x]::destroy()",this);

    // destroy and destruct all surfaces
    DWORD dwCount = getTotalSurfaces();
    for (DWORD i = 0; i < dwCount; i++)
    {
        // destroy
        if (m_aSurface[i].isValid())
        {
            m_aSurface[i].destroy();
        }

        // destruct
        m_aSurface[i].~CSimpleSurface();
    }

    // kill state
    m_dwTotalSurfaces = m_dwFlags
                      = 0;

    // done
    dbgTracePop();
    return TRUE;
}

#pragma warning (push, 1)
// NOTE: The following warning is issued incorrectly by the MS compiler. Ignore it.  <bdw>
template CComplexSurface<TOTAL_TEX_RENAME>;
template CComplexSurface<TOTAL_VB_RENAME>;
#pragma warning (pop)


//****************************************************************************
//****************************************************************************
//****************************************************************************
//**  CNvObject     **********************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************


//****************************************************************************
// CNvObject::dbgTestIntegrity
//
// tests integrity of an object

#ifdef DEBUG
void CNvObject::dbgTestIntegrity
(
    void
)
{
    dbgTracePush ("CNvObject::dbgTestIntegrity");

    // test self
    if (!isValid())
    {
        DPF ("CNvObject::dbgTestIntegrity: instance contains invalid THIS object");
        __asm int 3;
        dbgTracePop();
        return;
    }

    // test next
    if (m_pNext && !m_pNext->isValid())
    {
        DPF ("CNvObject::dbgTestIntegrity: instance contains invalid NEXT object");
        __asm int 3;
        dbgTracePop();
        return;
    }

    // test prev
    if (m_pPrev && !m_pPrev->isValid())
    {
        DPF ("CNvObject::dbgTestIntegrity: instance contains invalid PREV object");
        __asm int 3;
        dbgTracePop();
        return;
    }

    dbgTracePop();
}
#endif  // DEBUG
