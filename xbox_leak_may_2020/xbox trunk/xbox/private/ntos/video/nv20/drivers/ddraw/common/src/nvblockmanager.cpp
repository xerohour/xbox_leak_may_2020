/*
* Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
*/
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvBlockManager.cpp                                                *
*   block management/caching/renaming                                       *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Franck Diard/Michael Conrad  21Feb2001  rel20 development            *
*                                                                           *
\***************************************************************************/



#include "nvprecomp.h"

#ifdef MCFD

static CBlockManager g_BM; 
#ifdef DEBUG
BOOL g_renamingEnabled=FALSE;
#endif

// to be relocated

inline DWORD nvGetOffsetFromLinear(DWORD dwLinearAddress)
{
    NV_CFGEX_GET_AGP_OFFSET_PARAMS addr;
    
    addr.linaddr = (void*)dwLinearAddress;
    NvRmConfigGetEx(pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFGEX_GET_AGP_OFFSET, &addr, sizeof(addr));
    return addr.offset;
}








/////////////////////////// this code if very preliminary
///////////////m_gpuLocked is used for symmetry checking,
/// it does not mean at alll that the GPU is using something (IsBusy() does)

//// exported function:

BOOL GarbageCollect(DWORD Heap)
{
    return(g_BM.GarbageCollect(Heap));
}

////////////////



void  CVertexBuffer::LockForGPU(DWORD dwAccess)
{   
    m_blockList.LockForGPU(dwAccess);
}

void  CVertexBuffer::UnlockForGPU(void)                      
{    
    m_blockList.UnlockForGPU();
}

void  CVertexBuffer::LockForCPU(DWORD dwAccess, DWORD RenamingPolicy)            
{
    m_blockList.LockForCPU(dwAccess, RenamingPolicy);
    
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        nvglSetNv20KelvinInvalidateVertexCache (NV_DD_KELVIN);
    }
    else if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        nvglSetNv10CelsiusInvalidateVertexCache (NV_DD_CELSIUS);
    }
}

void  CVertexBuffer::UnlockForCPU(void)
{
    m_blockList.UnlockForCPU();
}

BOOL  CVertexBuffer::create(DWORD dwSize, DWORD dwAllowedHeaps, DWORD dwPreferredHeap)
{   
    BOOL bCreated=g_BM.CreateBlock(&m_blockList, dwSize, 1, 1, 1, dwAllowedHeaps, dwPreferredHeap, 0);
    
    return (bCreated);
}


BOOL  CVertexBuffer::destroy(void)                      
{   
    destroySuperTriLookAsideBuffer();

    g_BM.FreeList(&m_blockList);
    memset(&m_blockList, 0, sizeof(CBlockList));

    return(TRUE);
}


void  CVertexBuffer::own(DWORD dwAddress, DWORD dwSize, DWORD dwHeapLocation)
{
    BOOL bOwned=g_BM.OwnBlock(&m_blockList, dwAddress, dwSize, 1, dwHeapLocation, FALSE);       // equivalent to Create
}

void  CVertexBuffer::disown(void)
{
    BOOL bDisowned=g_BM.DisownBlock(&m_blockList);
}

void  CVertexBuffer::tagRenameEnable(DWORD dwRenameType, DWORD dwPreAllocCount)
{
    m_blockList.enableRenaming();
} 


DWORD  CVertexBuffer::getAddress()
{ 
    return m_blockList.m_pActive->getAddress(); 
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////


BOOL CBlockDescriptor::IsBusy() 
{
    dbgTracePush ("CSimpleSurface::isBusy");
    
    if (m_dwRetireDate)
    {
        
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
        
        return TRUE;

    }
    else                // that means that gpulock has not been called, so that's not busy
    {
        dbgTracePop();
        return FALSE;
    }
}


/////////////////////////////////// block list helpers

void  CBlockList::LockForGPU(DWORD dwAccess)                // aka hwlock
{   
    nvAssert(m_pActive); 
    nvAssert(dwAccess);
    nvAssert(!m_gpuLocked);
    nvAssert(m_bOwnMemory);
    
    dbgTracePush ("CBlockList::LockForGPU");
    
    m_gpuLocked=dwAccess;
    
    dbgTracePop();
    
}

void  CBlockList::UnlockForGPU(void)                        // aka hwunlock            
{   
    
    dbgTracePush ("CBlockList::UnlockForGPU");
    // write reference count into push buffer and keep
    //  the value. HW has to reach this point before CPU can
    //  get lock access
    
    //added a check for surfaces that the HW could actually touch
    //should we actually check each surface instead of calling HWUnlock?
    //HMH
    
    nvAssert(m_pActive);
    nvAssert(m_gpuLocked);
    nvAssert(m_bOwnMemory);
    
    if (m_pActive->hwCanRead() || m_pActive->hwCanWrite()) 
    {
        m_pActive->m_dwRetireDate = getDC()->pRefCount->inc (CReferenceCount::INC_LAZY);
    }
    
    m_gpuLocked=0;
    
    dbgTracePop();
}


void CBlockList::LockForCPU(DWORD dwAccess, DWORD dwRenamingCopyPolicy)  // renaming can allow different size
{
    nvAssert(dwAccess);
    nvAssert(m_pActive);
    nvAssert(!m_cpuLocked);
    nvAssert(!m_gpuLocked);
    
    
    m_cpuLocked=dwAccess;
    
    DDSTARTTICK(SURF7_SPINLOCK);
    dbgTracePush ("CBlockList::LockForCPU");
    
    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
        // if the channel's been destroyed, then everything is retired by definition
        DDENDTICK(SURF7_SPINLOCK);
        dbgTracePop();
        return;
    }
    
    // this is needed to prevent hangs when returning from a full-screen DOS box
    // waiting for a notifier with the FIFO in this state guarantees a hang
    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_D3D) {
        DPF("CSimpleSurface::destroy: Early exit due to DosOccurred flag");
        dbgTracePop();
        DDENDTICK(SURF7_SPINLOCK);
        return;
    }
    
    if ((dwAccess==CSimpleSurface::LOCK_WRITE))
    {
        if (m_pActive->IsBusy())
        {
            if (m_renaming 
                //&& (m_dwNb <=m_dwMax) 
#ifdef DEBUG
                && g_renamingEnabled
#endif
                )
            {
                Rename(dwRenamingCopyPolicy);      // with copy or not
            }
            else
            {
                getDC()->pRefCount->wait (CReferenceCount::WAIT_PREEMPTIVE, m_pActive->m_dwRetireDate);
                m_pActive->m_dwRetireDate=0;
            }
        }
    }
    
    dbgTracePop();
    DDENDTICK(SURF7_SPINLOCK);
}

void  CBlockList::UnlockForCPU(void)
{
    // nothing to be done, just a symmetric
    dbgTracePush ("CBlockList::UnlockForCPU");
    
    nvAssert(m_pActive);
    nvAssert(m_cpuLocked);
    nvAssert(!m_gpuLocked);
    
    m_cpuLocked=0;
    dbgTracePop();
}


//////////////// Rename should check if some memory is available


BOOL CBlockList::Rename(DWORD dwRenamingCopyPolicy)  // renaming can allow different size
{
    CBlockDescriptor * pNext;
    CBlockDescriptor * pSave;
    BOOL bCreate=FALSE;
    
    nvAssert(m_pActive);
    
    pSave=m_pActive;
    
    if (m_dwNb==1)       //   if we are here, the acitive was busy, so we need to create a new one
        bCreate=TRUE;
    else
    { 
        pNext=m_pActive->m_pNextRenamed;
        
        if (pNext->IsBusy())    // if it is busy, no need to continue
            bCreate=TRUE;
        else
        {
            m_pActive=pNext;
            m_pActive->m_dwRetireDate=0;
            
            if (dwRenamingCopyPolicy==RENAME_POLICY_COPY)
                nvMemCopy((void *)m_pActive->getAddress(), (void *)pSave->getAddress(), m_pActive->getSize());
            
            return(TRUE);
        }
    }
    if (bCreate)
    {
        g_BM.CreateBlock(this,  m_pActive->m_dwPitch, m_pActive->m_dwHeight,        // will set the active to the new one
            m_pActive->m_dwDepth, m_dwBPPGranted, 
            m_dwAllowedHeaps, m_dwPreferredHeap, m_pActive->m_dwAllocFlags);
        
        if (dwRenamingCopyPolicy==RENAME_POLICY_COPY)
            nvMemCopy((void *)m_pActive->getAddress(), (void *)pSave->getAddress(), m_pActive->getSize());
        
    }
    return(TRUE);
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

CBlockManager::CBlockManager()
{
    memset(this, 0, sizeof(CBlockManager));
    
}

BOOL CBlockManager::GarbageCollect(DWORD dwHeap)
{
    DWORD dwDevice=0;
    
    switch (dwHeap)
    {
        
    case CSimpleSurface::HEAP_VID:
        {
            break;
        }
    case CSimpleSurface::HEAP_AGP:
        {
            break;
        }
    case CSimpleSurface::HEAP_PCI:
        {
            break;
        }
    case CSimpleSurface::HEAP_SYS:
        {
            break;
        }
    default:
        nvAssert(0);
        break;
    }    
    return(TRUE);
}

BOOL CBlockManager::FreeList(CBlockList * BlockList)
{
    return(TRUE);
}

BOOL CBlockManager::AddToCachedBlockList(CBlockDescriptor * ToAdd)
{
    
    DWORD dwDevice=0;           // see later
    
    switch (ToAdd->m_dwHeap)
    {
        
    case CSimpleSurface::HEAP_VID:
        {
            ToAdd->m_pNextGlobal=m_apVIDFirst[dwDevice];
            m_apVIDFirst[dwDevice]=ToAdd;
            break;
        }
    case CSimpleSurface::HEAP_AGP:
        {
            ToAdd->m_pNextGlobal=m_pAGPFirst;
            m_pAGPFirst=ToAdd;
            break;
        }
    case CSimpleSurface::HEAP_PCI:
        {
            ToAdd->m_pNextGlobal=m_pPCIFirst;
            m_pPCIFirst=ToAdd;
            break;
        }
    case CSimpleSurface::HEAP_SYS:
        {
            ToAdd->m_pNextGlobal=m_pSYSFirst;
            m_pSYSFirst=ToAdd;
            break;
        }
    default:
        nvAssert(0);
        break;
    }
    
    m_dwCachedBlocks++;
    
    return(TRUE);
}


//adds a block right after the active one
BOOL CBlockManager::RegisterBlock(CBlockList * BlockList, CBlockDescriptor * ToAdd)
{
    //private list
    
    if (BlockList->m_pActive) 
    {
        ToAdd->m_pNextRenamed=BlockList->m_pActive->m_pNextRenamed;    //forward link
        BlockList->m_pActive->m_pNextRenamed=ToAdd;
        
        ToAdd->m_pPreviousRenamed=BlockList->m_pActive->m_pPreviousRenamed;    //backward link
        BlockList->m_pActive->m_pPreviousRenamed=ToAdd;
    }
    else
    {
        ToAdd->m_pNextRenamed=ToAdd;        // yes it points to itself
        ToAdd->m_pPreviousRenamed=ToAdd;        // yes it points to itself
    }
    
    BlockList->m_pActive=ToAdd;
    BlockList->m_dwNb++;
    
    
    DWORD dwDevice=0;           // see later
    
    //memory accounting and sorting
    
    switch (ToAdd->m_dwHeap)
    {
    case CSimpleSurface::HEAP_VID:
        {
            m_dwAllocatedVIDMemory+=ToAdd->m_dwSize;
            m_adwAllocatedVIDMemoryPerDevice[dwDevice]+=ToAdd->m_dwSize;
            
            ToAdd->m_pNextGlobal=m_apVIDFirst[dwDevice];
            m_apVIDFirst[dwDevice]=ToAdd;
            
            break;
        }
    case CSimpleSurface::HEAP_AGP:
        {
            m_dwAllocatedAGPMemory+=ToAdd->m_dwSize;
            
            ToAdd->m_pNextGlobal=m_pAGPFirst;
            m_pAGPFirst=ToAdd;
            
            break;
        }
    case CSimpleSurface::HEAP_PCI:
        {
            m_dwAllocatedPCIMemory+=ToAdd->m_dwSize;
            
            ToAdd->m_pNextGlobal=m_pPCIFirst;
            m_pPCIFirst=ToAdd;
            
            break;
        }
    case CSimpleSurface::HEAP_SYS:
        {
            m_dwAllocatedSYSMemory+=ToAdd->m_dwSize;
            
            ToAdd->m_pNextGlobal=m_pSYSFirst;
            m_pSYSFirst=ToAdd;
            
            break;
        }
    default:
        nvAssert(0);
        break;
    }
    
    ToAdd->m_pOwner=BlockList;
    
    m_dwActiveBlocks++;
    
    return(TRUE);
}


BOOL CBlockManager::CreateBlock(
                                CBlockList * BlockList,
                                DWORD dwPitch,
                                DWORD dwHeight,
                                DWORD dwDepth,
                                DWORD dwBPPGranted,
                                DWORD dwAllowedHeaps,
                                DWORD dwPreferredHeap,
                                DWORD dwAllocFlags
                                )
{
    
    CBlockDescriptor * NewBlockDesc= new CBlockDescriptor;
    
    BlockList->m_bOwnMemory=TRUE;

    if (AllocateBlock(NewBlockDesc, dwPitch, dwHeight, dwDepth, dwBPPGranted, dwAllowedHeaps, dwPreferredHeap, dwAllocFlags))
    {        
        if (RegisterBlock(BlockList, NewBlockDesc))
        {
            BlockList->m_dwAllowedHeaps=dwAllowedHeaps;             // to create new ones 
            BlockList->m_dwPreferredHeap=dwPreferredHeap;
            BlockList->m_dwBPPGranted=1;
            return(TRUE);           // ideal code path
        }
    }
    else
    {
        nvAssert(0);
    }
    
    delete NewBlockDesc;
    return(FALSE);
    
}

BOOL CBlockManager::OwnBlock(   CBlockList * BlockList,

    DWORD dwAddress,
    DWORD dwPitch,
    DWORD dwHeight,
    DWORD dwHeapLocation,
    BOOL  bOwnMemory )

{

    nvAssert(!bOwnMemory);

    BlockList->m_bOwnMemory=FALSE;

    // tell world
    dbgTracePush ("CBlockManager::OwnBlock[%08x]::own(dwAddress=%08x,dwPitch=%08x,dwH=%08x,dwHeapLoc=%x,bOwnMem=%d)",
                  this, dwAddress, dwPitch, dwHeight, dwHeapLocation, bOwnMemory);
    if (BlockList->m_dwNb)          
        nvAssert(0);             // oula! should reuse the blocklist to own a block
    
    CBlockDescriptor * NewBlockDesc= new CBlockDescriptor;
    
    // assign
    NewBlockDesc->m_dwAddress    = dwAddress;
    NewBlockDesc->m_dwPitch      = dwPitch;
    NewBlockDesc->m_dwHeight     = dwHeight;
    NewBlockDesc->m_dwFlags     &= ~CSimpleSurface::HEAP_LOCATION_MASK;
    NewBlockDesc->m_dwFlags     |= dwHeapLocation;

    NewBlockDesc->m_dwFlags |= CSimpleSurface::FLAG_DONOTOWNMEMORY;

    switch (NewBlockDesc->getHeapLocation())
    {
        case CSimpleSurface::HEAP_VID:
            NewBlockDesc->m_dwOffset = NewBlockDesc->m_dwAddress - VIDMEM_ADDR(pDriverData->BaseAddress);
            break;
        case CSimpleSurface::HEAP_AGP:
#ifdef NV_AGP
            NewBlockDesc->m_dwOffset = nvGetOffsetFromLinear(NewBlockDesc->m_dwAddress);
            DPF_LEVEL(NVDBG_LEVEL_NV_AGP, "CSimpleSurface::Own: offset of AGP address %08x is %08x", NewBlockDesc->m_dwAddress, NewBlockDesc->m_dwOffset);
#else
            NewBlockDesc->m_dwOffset = AGPMEM_OFFSET(m_dwAddress);
#endif
            break;
        case CSimpleSurface::HEAP_PCI:
#ifdef WINNT // WINNT Temp Hack
        if (!getDC()->nvD3DTexHeapData.dwBase)
            NewBlockDesc->m_dwOffset = NewBlockDesc->m_dwAddress - getDC()->nvPusher.getBase();
        else
#endif // WINNT Temp Hack
            NewBlockDesc->m_dwOffset = NewBlockDesc->m_dwAddress - getDC()->nvD3DTexHeapData.dwBase;
            break;
        case CSimpleSurface::HEAP_SYS:
            NewBlockDesc->m_dwOffset = NewBlockDesc->m_dwAddress;
            break;
        default:
            DPF ("unknown context");
            dbgD3DError();
            break;
    }

    BlockList->m_pActive=NewBlockDesc;             // make it active
    BlockList->m_dwNb++;

    dbgTracePop();
    return(TRUE);           // ideal code path
}


BOOL CBlockManager::DisownBlock( CBlockList * BlockList)
{
    dbgTracePush ("CSimpleSurface[%08x]::disown()",this);

    nvAssert(0);        // should not happen now

    dbgTracePop();

    return(TRUE);
}


BOOL CBlockManager::AllocateBlock
(
 CBlockDescriptor * BlockDesc,
 DWORD dwPitch,
 DWORD dwHeight,
 DWORD dwDepth,
 DWORD dwBPPGranted,
 DWORD dwAllowedHeaps,
 DWORD dwPreferredHeap,
 DWORD dwAllocFlags
 )
{
#ifdef DEBUG
    // just so we can see what was asked for when debugging
    DWORD dwOriginalAllowedHeaps  = dwAllowedHeaps;
    DWORD dwOriginalPreferredHeap = dwPreferredHeap;
#endif
    
    // tell world
    dbgTracePush ("CBlockManager::CreateBlock[%08x]::create(dwPitch=%08x,dwHt=%08x,dwDp=%08x,dwAllHeaps=%x,dwPrefHeap=%x,dwFlags=%x)",
        this, dwPitch, dwHeight, dwDepth, dwAllowedHeaps, dwPreferredHeap, dwAllocFlags);
    
    
    DWORD dwSize = dwPitch * dwHeight * dwDepth;

    // Allow for a possible ZERO sized surface
    if (!dwSize) {
        // complain
        DPF ("CSimpleSurface::create: dwSize == 0");
        dbgD3DError();
        // tag surface as invalid
        BlockDesc->m_dwAddress = 0;
        dbgTracePop();
        return (FALSE);
    }
    
    // retire surface (since it is about to exist)
    BlockDesc->m_dwRetireDate = 0;
    
    // no specific preference would start with video memory
    if (dwPreferredHeap == CSimpleSurface::HEAP_NOWHERE) {
        dwPreferredHeap = CSimpleSurface::HEAP_VID;
    }
    
    // if we require tiled memory - override to only allow vid heap
    if (dwAllocFlags & CSimpleSurface::ALLOCATE_TILED) {
        dwAllowedHeaps  = CSimpleSurface::HEAP_VID;
        dwPreferredHeap = CSimpleSurface::HEAP_VID;
    }
    
    // suppress video memory allocation all together if it is overidden
    //  by the registy
    if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_VIDEOTEXTUREENABLE_MASK) == D3D_REG_VIDEOTEXTUREENABLE_DISABLE)
    {
        dwAllowedHeaps &= ~CSimpleSurface::HEAP_VID;
        if (!dwAllowedHeaps) dwAllowedHeaps = CSimpleSurface::HEAP_AGP | CSimpleSurface::HEAP_PCI;
    }
    
    BlockDesc->m_dwAllocFlags = dwAllocFlags;
    
    // try to allocate in all allowed memory heaps
    while (dwAllowedHeaps)
    {
        // video
        if (dwAllowedHeaps & dwPreferredHeap & CSimpleSurface::HEAP_VID)
        {
            DWORD dwStatus, dwType;
            
            if (dwAllocFlags & CSimpleSurface::ALLOCATE_TILED)
            {
                dwType = (dwAllocFlags & CSimpleSurface::ALLOCATE_AS_ZBUFFER_UNC) ?TYPE_DEPTH :
                        ((dwAllocFlags & CSimpleSurface::ALLOCATE_AS_ZBUFFER_CMP) ?
                    ((dwBPPGranted == 2) ? TYPE_DEPTH_COMPR16 : TYPE_DEPTH_COMPR32) : TYPE_IMAGE);
#ifdef WINNT
                NVHEAP_ALLOC_TILED (dwStatus, BlockDesc->m_dwOffset, dwPitch, dwHeight, dwType);
                BlockDesc->m_dwAddress = VIDMEM_ADDR(BlockDesc->m_dwOffset);
#else  // !WINNT
                NVHEAP_ALLOC_TILED (dwStatus, BlockDesc->m_dwAddress, dwPitch, dwHeight, dwType);
#endif // !WINNT
#ifdef DEBUG_SURFACE_PLACEMENT
                m_dwRMAllocFlags = dwType;
#endif
                nvAssert (dwStatus == 0);
            }
            
            else
            {
                dwType = (dwAllocFlags & CSimpleSurface::ALLOCATE_AS_ZBUFFER_UNC) ? TYPE_DEPTH :
                        ((dwAllocFlags & CSimpleSurface::ALLOCATE_AS_ZBUFFER_CMP) ? ((dwBPPGranted == 2) ?
                        TYPE_DEPTH_COMPR16 : TYPE_DEPTH_COMPR32) : TYPE_TEXTURE);
#ifdef WINNT
                NVHEAP_ALLOC (dwStatus, BlockDesc->m_dwOffset, dwSize, dwType);
                BlockDesc->m_dwAddress = VIDMEM_ADDR(BlockDesc->m_dwOffset);
#else
                NVHEAP_ALLOC (dwStatus, BlockDesc->m_dwAddress, dwSize, dwType);
#endif // WINNT
#ifdef DEBUG_SURFACE_PLACEMENT
                m_dwRMAllocFlags = dwType;
#endif
            }
            
            if (dwStatus == 0) {
                
                // increment count of video memory surfaces allocated.
                pDriverData->DDrawVideoSurfaceCount++;
                BlockDesc->m_dwPitch     = dwPitch;
                BlockDesc->m_dwHeight    = dwHeight;
                BlockDesc->m_dwDepth     = dwDepth;
#ifndef WINNT
                BlockDesc->m_dwOffset    = BlockDesc->m_dwAddress - pDriverData->BaseAddress;
#endif // WINNT
                BlockDesc->m_dwFlags    &= ~CSimpleSurface::HEAP_LOCATION_MASK;
                BlockDesc->m_dwFlags    |= CSimpleSurface::HEAP_VID;
                BlockDesc->m_dwFlags    &= ~CSimpleSurface::FLAG_TILED;
                BlockDesc->m_dwFlags    |= (dwAllocFlags & CSimpleSurface::ALLOCATE_TILED)      ? CSimpleSurface::FLAG_TILED   : 0;
                BlockDesc->m_dwFlags    |= (dwAllocFlags & CSimpleSurface::ALLOCATE_AS_ZBUFFER) ? CSimpleSurface::FLAG_ZBUFFER : 0;
                
#ifdef DEBUG
                DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"Vid Surface Alloc: Address:%08x Size:%08x\n", BlockDesc->m_dwAddress,dwSize);
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
                    surf.dwType               = (dwAllocFlags & CSimpleSurface::ALLOCATE_AS_ZBUFFER) ? CAPTURE_SURFACE_TYPE_ZETA : CAPTURE_SURFACE_TYPE_NORMAL;
                    surf.dwKind               = dwUsageKind;
                    surf.dwSurfaceType        = getFormat();
                    surf.dwIntendedPitch      = dwPitch;
                    surf.dwIntendedHeight     = dwHeight;
                    captureLog (&surf,sizeof(surf));
                }
#endif
                
                BlockDesc->m_dwHeap=CSimpleSurface::HEAP_VID;
                BlockDesc->m_dwSize=dwSize;
                
                dbgTracePop();
                
                return (TRUE);
            }
            
#ifdef DEBUG
            DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"Vid Surface Alloc Failed: Size:%08x\n",dwSize);
#endif
            // fail - don't try again
            DPF_LEVEL(NVDBG_LEVEL_INFO,"CSimpleSurface::create: Surface in VID failed");
            dwAllowedHeaps &= ~CSimpleSurface::HEAP_VID;
        }
        
        // agp
        if (dwAllowedHeaps & dwPreferredHeap & CSimpleSurface::HEAP_AGP)
        {
            if (pDriverData->GARTLinearBase)
            {
                // On AGP, allocate call DDRAW to allocate the AGP memory.
                BlockDesc->m_dwAddress = (DWORD)nvAGPAlloc(dwSize);
                if (BlockDesc->m_dwAddress != ERR_DXALLOC_FAILED)
                {
#ifdef NV_AGP
                    BlockDesc->m_dwOffset   = nvGetOffsetFromLinear(BlockDesc->m_dwAddress);
                    DPF_LEVEL(NVDBG_LEVEL_NV_AGP, "CSimpleSurface::Create: offset of AGP address %08x is %08x", BlockDesc->m_dwAddress, BlockDesc->m_dwOffset);
#else
                    // at this point m_dwAddress is actually an offset
                    BlockDesc->m_dwAddress  = AGPMEM_ADDR(BlockDesc->m_dwAddress);
                    BlockDesc->m_dwOffset   = AGPMEM_OFFSET(BlockDesc->m_dwAddress);
#endif
                    BlockDesc->m_dwPitch    = dwPitch;
                    BlockDesc->m_dwHeight   = dwHeight;
                    BlockDesc->m_dwDepth    = dwDepth;
                    BlockDesc->m_dwFlags   &= ~CSimpleSurface::HEAP_LOCATION_MASK;
                    BlockDesc->m_dwFlags   |= CSimpleSurface::HEAP_AGP;
                    BlockDesc->m_dwFlags   &= ~CSimpleSurface::FLAG_TILED;
#ifdef DEBUG
                    DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"AGP Surface Alloc: Address:%08x Size:%08x\n",BlockDesc->m_dwAddress,dwSize);
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
                    
                    BlockDesc->m_dwHeap=CSimpleSurface::HEAP_AGP;
                    BlockDesc->m_dwSize=dwSize;
                    
                    dbgTracePop();
                    
                    return (TRUE);
                }
            }
            // fail - don't try again
#ifdef DEBUG
            DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"AGP Surface Alloc Failed: Size:%08x\n",dwSize);
#endif
            DPF_LEVEL(NVDBG_LEVEL_INFO,"CSimpleSurface::create: Surface in AGP failed");
            dwAllowedHeaps &= ~CSimpleSurface::HEAP_AGP;
        }
        
        // pci
        if (dwAllowedHeaps & dwPreferredHeap & CSimpleSurface::HEAP_PCI)
        {
            DWORD dwOffset;
            
            // On PCI, allocate from our internal texture heap.
            dwOffset = nvPCIAlloc(dwSize);
            if (dwOffset)
            {
                BlockDesc->m_dwPitch     = dwPitch;
                BlockDesc->m_dwHeight    = dwHeight;
                BlockDesc->m_dwDepth     = dwDepth;
                BlockDesc->m_dwOffset    = dwOffset;
                BlockDesc->m_dwAddress   = getDC()->nvD3DTexHeapData.dwBase + dwOffset;
                BlockDesc->m_dwFlags    &= ~CSimpleSurface::HEAP_LOCATION_MASK;
                BlockDesc->m_dwFlags    |= CSimpleSurface::HEAP_PCI;
                BlockDesc->m_dwFlags    &= ~CSimpleSurface::FLAG_TILED;
                
#ifdef DEBUG
                DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"PCI Surface Alloc: Address:%08x Size:%08x\n", BlockDesc->m_dwAddress,dwSize);
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
                
                BlockDesc->m_dwHeap=CSimpleSurface::HEAP_PCI;
                BlockDesc->m_dwSize=dwSize;
                
                dbgTracePop();
                
                return (TRUE);
            }
            
#ifdef DEBUG
            DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"PCI Surface Alloc Failed: Size:%08x\n",dwSize);
#endif
            
            // fail - don't try again
            DPF_LEVEL(NVDBG_LEVEL_INFO,"CSimpleSurface::create: Surface in PCI failed");
            dwAllowedHeaps &= ~CSimpleSurface::HEAP_PCI;
        }
        
        // system memory
        if (dwAllowedHeaps & dwPreferredHeap & CSimpleSurface::HEAP_SYS)
        {
#ifdef WINNT
            nvAssert(0);            // we should use this call to allcoate heap memory with win2k
#endif
            // For system memory, allocate from global heap.
            // m_dwOffset keeps the original ptr while
            // m_dwAddress keeps the aligned ptr
            
            
            BlockDesc->m_dwOffset = (DWORD) AllocIPM (dwSize);
            nvAssert (((BlockDesc->m_dwOffset + 31) & ~31) == BlockDesc->m_dwOffset);
            
            if (BlockDesc->m_dwOffset)
            {
                BlockDesc->m_dwPitch     = dwPitch;
                BlockDesc->m_dwHeight    = dwHeight;
                BlockDesc->m_dwDepth     = dwDepth;
                BlockDesc->m_dwAddress   = (BlockDesc->m_dwOffset + 31) & ~31;
                BlockDesc->m_dwFlags    &= ~CSimpleSurface::HEAP_LOCATION_MASK;
                BlockDesc->m_dwFlags    |= CSimpleSurface::HEAP_SYS;
                BlockDesc->m_dwFlags    &= ~CSimpleSurface::FLAG_TILED;
                
#ifdef DEBUG
                DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"SYS Surface Alloc: Address:%08x Size:%08x\n", BlockDesc->m_dwAddress,dwSize);
#endif
                
                BlockDesc->m_dwHeap=CSimpleSurface::HEAP_SYS;
                BlockDesc->m_dwSize=dwSize;
                
                dbgTracePop();
                
                return (TRUE);
            }
            
            // fail - don't try again
#ifdef DEBUG
            DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"SYS Surface Alloc Failed: Size:%08x\n",dwSize);
#endif
            DPF_LEVEL(NVDBG_LEVEL_INFO,"CSimpleSurface::create: Surface in SYS failed");
            dwAllowedHeaps &= ~CSimpleSurface::HEAP_SYS;
        }
        
        // next
        dwPreferredHeap = (dwPreferredHeap << 1) & CSimpleSurface::HEAP_ANYWHERE;
        if (!dwPreferredHeap) dwPreferredHeap = CSimpleSurface::HEAP_VID;
    }
    
    // not allocated
    //  tag as invalid and fail
    DPF_LEVEL(NVDBG_LEVEL_INFO,"CSimpleSurface::create: Surface Not allocated");
    BlockDesc->m_dwAddress = 0;
    dbgTracePop();
    return (FALSE);
}

#endif