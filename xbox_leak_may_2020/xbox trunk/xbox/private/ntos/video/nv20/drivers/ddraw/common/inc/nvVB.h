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
#ifndef _nvVB_h
#define _nvVB_h

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * constants
 */
#define VB_CACHE_SIZE               32              // # of software cache entries (no more than 256)
#define TOTAL_VB_RENAME             16

/*****************************************************************************
 * CVertexBuffer
 *
 * d3d vertex buffer internal structure
 */
class CVertexBuffer
{
    /*
     * constants
     */
public:
    enum
    {
        FLAG_D3D_LOCKED         = 1,    // d3d has lock (affects renaming policy)
        FLAG_D3D_IMPLICIT       = 2,    // d3d implicit VB
        FLAG_DX6                = 4,    // dx6 vertex buffer
        FLAG_MODIFIED           = 8,    // set when we believe the contents of the buffer has changed
        FLAG_RENAME2SYSTEM      = 16,   // when set, we will attempt to move the VB to system memory (used to CM hack performance)
        FLAG_DYNAMIC            = 32,   // dynamic buffer
        FLAG_SUPERTRIOWNSMEM    = 64,   // supertri owns the look aside buffer
        FLAG_ALIASLOCKED        = 128   // NV_AGP alias lock
    };

public:
    struct STVERTEX
    {
        float x,y,z,reserved;
    };

    /*
     * members
     */
protected:
// bank
#ifdef MCFD

#else 
    DWORD            m_Cache_CSS_dwFlags;             // Copy of CSimpleSurface flags
    CSimpleSurface  *m_Cache_CSS_pActiveSurface;      // Pointer to the currently active surface
#endif //MCFD
    CNvObject       *m_pWrapperObject;
    DWORD            m_dwFlags;
    DWORD            m_dwVertexStride;
    void            *m_pSuperTriLookAsideBuffer;
    DWORD            m_dwSuperTriLookAsideBufferSize;
    DWORD            m_dwLockCount;
// bank
    DWORD          __pad[8];
// bank

#ifdef MCFD
	void * m_data;  //pointer to the whole bitmap
public:
	CBlockList m_blockList;   // list header of the blocks belonging to the renaming chain

    inline DWORD           getContextDMA     (void)   { return m_blockList.m_pActive->getContextDMA(); }
    inline DWORD           getHeapLocation   (void)   { return m_blockList.m_pActive->getHeapLocation(); }
    inline DWORD           getOffset         (void)   { return m_blockList.m_pActive->getOffset();  }
    DWORD                  getAddress        (void);
    inline DWORD           getfpVidMem       (void)   { return m_blockList.m_pActive->getAddress(); }
    inline DWORD           getSize           (void)   { return m_blockList.m_pActive->getSize();    }

    inline BOOL            hwCanRead(void)            { return m_blockList.m_pActive->hwCanRead(); }

    void  LockForGPU             (DWORD dwAcess);
    void  UnlockForGPU           (void);

    void  LockForCPU            (DWORD dwAcess, DWORD RenamingPolicy=RENAME_POLICY_NO_COPY);
    void  UnlockForCPU          (void);


#else
    CComplexSurface<TOTAL_VB_RENAME> m_Surface;

    /*
     * helpers
     */
public:
    inline CSimpleSurface* getSurface        (void)   { return m_Surface.getActiveSurface(); }
    inline DWORD           getContextDMA     (void)   { return m_Surface.getActiveSurface()->getContextDMA(); }
    inline DWORD           getHeapLocation   (void)   { return m_Surface.getActiveSurface()->getHeapLocation(); }
    inline DWORD           getOffset         (void)   { return m_Surface.getActiveSurface()->getOffset();  }
    inline DWORD           getAddress        (void)   { return m_Surface.getActiveSurface()->getAddress(); }
    inline DWORD           getfpVidMem       (void)   { return m_Surface.getActiveSurface()->getfpVidMem(); }
    inline DWORD           getSize           (void)   { return m_Surface.getActiveSurface()->getSize();    }
#endif
    inline CNvObject*      getWrapper        (void)   { return (m_pWrapperObject); }

    /*
     * methods
     */
public:
           BOOL  prepareSuperTriLookAsideBuffer (void);
           void  destroySuperTriLookAsideBuffer (void);
    inline void* getSuperTriLookAsideBuffer     (void)    const {   return m_pSuperTriLookAsideBuffer;   }
    inline void  setSuperTriLookAsideBuffer     (void *p)       {   m_pSuperTriLookAsideBuffer = p;
                                                                    // this should never assert, but if it does, this simply means we should free the look aside buffer we already own
                                                                    nvAssert (!(m_dwFlags & FLAG_SUPERTRIOWNSMEM));
                                                                }

#ifdef MCFD
    BOOL  create             (DWORD dwSize, DWORD dwAllowedHeaps, DWORD dwPreferredHeap);
    BOOL  destroy            (void);
    void  own                (DWORD dwAddress, DWORD dwSize, DWORD dwHeapLocation);
    void  disown             (void) ;
    void  tagRenameEnable    (DWORD dwRenameType, DWORD dwPreAllocCount=1);
#else
    inline BOOL  create             (DWORD dwSize, DWORD dwAllowedHeaps, DWORD dwPreferredHeap)
                                                                {   BOOL bCreated =  m_Surface.create (m_pWrapperObject,
                                                                                                       dwSize,
                                                                                                       dwAllowedHeaps,
                                                                                                       dwPreferredHeap
#ifdef CAPTURE
                                                                                                      ,CAPTURE_SURFACE_KIND_VERTEX
#endif
                                                                                                       );
                                                                    updateCachedValues();
                                                                    return bCreated;
                                                                }


    inline BOOL  destroy            (void)                      {   destroySuperTriLookAsideBuffer();
                                                                    return (m_Surface.destroy());
                                                                }

    inline void  own                (DWORD dwAddress, DWORD dwSize, DWORD dwHeapLocation)
                                                                {   m_Surface.own(dwAddress,dwSize,1,dwHeapLocation,FALSE);
                                                                    updateCachedValues();
                                                                }

    inline void  disown             (void)                      {   m_Surface.disown();   }

    inline void  tagRenameEnable    (DWORD dwRenameType, DWORD dwPreAllocCount=1)
                                                                {   m_Surface.tagRenameEnable (dwRenameType, dwPreAllocCount);   }
#endif

#ifndef MCFD
    inline void  hwLock             (DWORD dwAccess)            {   m_Surface.hwLock (dwAccess);   }
    inline void  hwUnlock           (void)                      {   m_Surface.hwUnlock();   }

    inline void  cpuLock            (DWORD dwAccess)            {   m_Surface.cpuLock (dwAccess);
                                                                    updateCachedValues();
                                                                    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                                                                        nvglSetNv20KelvinInvalidateVertexCache (NV_DD_KELVIN);
                                                                    }
                                                                    else if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
                                                                        nvglSetNv10CelsiusInvalidateVertexCache (NV_DD_CELSIUS);
                                                                    }
                                                                }

    inline void  cpuUnlock          (void)                      {   m_Surface.cpuUnlock();   }
#endif

    inline void  tagD3DLocked       (void)                      {   m_dwFlags |= FLAG_D3D_LOCKED;         }
    inline void  tagD3DUnlocked     (void)                      {   m_dwFlags &= ~FLAG_D3D_LOCKED;        }
    inline BOOL  isD3DLocked        (void) const                {   return (m_dwFlags & FLAG_D3D_LOCKED); }

    inline void  tagD3DImplicit     (void)                      {   m_dwFlags |= FLAG_D3D_IMPLICIT;         }
    inline BOOL  isD3DImplicit      (void) const                {   return (m_dwFlags & FLAG_D3D_IMPLICIT); }

    inline void  tagDX6             (void)                      {   m_dwFlags |= FLAG_DX6;         }
    inline BOOL  isDX6              (void) const                {   return (m_dwFlags & FLAG_DX6); }

    inline void  tagModified        (void)                      {   m_dwFlags |=  FLAG_MODIFIED;        }
    inline void  tagNotModified     (void)                      {   m_dwFlags &= ~FLAG_MODIFIED;        }
    inline BOOL  isModified         (void) const                {   return (m_dwFlags & FLAG_MODIFIED); }

    inline void  tagAliasLocked     (void)                      {   m_dwFlags |=  FLAG_ALIASLOCKED;        }
    inline void  tagNotAliasLocked  (void)                      {   m_dwFlags &= ~FLAG_ALIASLOCKED;        }
    inline BOOL  isAliasLocked      (void) const                {   return (m_dwFlags & FLAG_ALIASLOCKED); }

    inline void  tagRenameToSystem  (void)                      {   m_dwFlags |= FLAG_RENAME2SYSTEM;         }
    inline void  tagRenamedToSystem (void)                      {   m_dwFlags &= ~FLAG_RENAME2SYSTEM;        }
    inline BOOL  mustRenameToSystem (void) const                {   return (m_dwFlags & FLAG_RENAME2SYSTEM); }

    inline void  tagDynamic         (void)                      {   m_dwFlags |= FLAG_DYNAMIC;         }
    inline BOOL  isDynamic          (void) const                {   return (m_dwFlags & FLAG_DYNAMIC); }

    inline void  setVertexStride    (DWORD dw)                  {   m_dwVertexStride = dw;     }
    inline DWORD getVertexStride    (void) const                {   return (m_dwVertexStride); }

    inline void  bumpLockCount      (void)                      {   m_dwLockCount++;        }
    inline DWORD getLockCount       (void) const                {   return (m_dwLockCount); }

#ifdef MCFD

    inline void  updateCachedValues (void)                      {   
                                                                }

    inline bool  cachedhwCanRead    (void)                      {   return (m_blockList.m_pActive->getFlags() & CSimpleSurface::HEAP_LOCATION_MASK) != CSimpleSurface::HEAP_SYS;   }
    inline void  prefetchSurface() {};

#else

    // SK - The following are used to prefetch the current, active vertex buffer
    inline void  updateCachedValues (void)                      {   CSimpleSurface *p = m_Surface.getActiveSurface();
                                                                    m_Cache_CSS_dwFlags = p->getFlags();
                                                                    m_Cache_CSS_pActiveSurface = p;
                                                                }

    inline bool  cachedhwCanRead    (void)                      {   return (m_Cache_CSS_dwFlags & CSimpleSurface::HEAP_LOCATION_MASK) != CSimpleSurface::HEAP_SYS;   }

    #define rEAX                    0
    #define rEDX                    2
    #define mREG(x)                 ((x) << 3)
    #define rmIND8(x)               (0x40 | (x))
    #define prefetch_rm8(h,rm,ofs)  __asm _emit 0x0f __asm _emit 0x18 __asm _emit (mREG(h) | (rm)) __asm _emit (ofs)

    inline void  prefetchSurface    (void) const                {   if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI) {
                                                                        __asm
                                                                        {
                                                                            mov eax, this
                                                                            lea edx, [eax].m_Surface
                                                                            prefetch_rm8(1, rmIND8(rEAX), 0)
                                                                            prefetch_rm8(0, rmIND8(rEAX), 32)
                                                                            mov edx, [eax]CVertexBuffer.m_Cache_CSS_pActiveSurface
                                                                            prefetch_rm8(1, rmIND8(rEDX), 0)
                                                                            prefetch_rm8(1, rmIND8(rEDX), 32)
                                                                        }
                                                                    }

                                                                }
    #undef rEAX
    #undef rEDX
    #undef mREG
    #undef rmIND8
    #undef prefetch_rm8
#endif
    /*
     * construction
     */
public:
    CVertexBuffer  (CNvObject *pWrapperObject = NULL);
    ~CVertexBuffer (void);

};

/*****************************************************************************
 * CCommandBuffer
 *
 * d3d system memory wrapper (command and execute buffers)
 */
class CCommandBuffer
{
    /*
     * members
     */
protected:
// bank
    CSimpleSurface             m_Surface;
// bank
    CNvObject                 *m_pWrapperObject;
    LPDDRAWI_DDRAWSURFACE_LCL  m_pDDSurfaceLcl;
    DWORD                      _pad0[6];

    /*
     * helpers
     */
public:
    inline LPDDRAWI_DDRAWSURFACE_LCL getDDSurfaceLcl (void) const { return m_pDDSurfaceLcl; }

    inline CSimpleSurface* getSurface (void)                 { return &m_Surface; }
    inline DWORD           getAddress (void)           const { return m_Surface.getAddress(); }
    inline DWORD           getSize    (void)           const { return m_Surface.getSize();    }

    inline void            cpuLock    (DWORD dwAccess)       { m_Surface.cpuLock (dwAccess); }
    inline void            cpuUnlock  (void)                 { m_Surface.cpuUnlock(); }

    /*
     * methods
     */
public:
    inline BOOL create  (DWORD dwSize)  { return m_Surface.create (dwSize, 1, 1, 0,
                                                                   CSimpleSurface::HEAP_SYS,
                                                                   CSimpleSurface::HEAP_SYS,
                                                                   CSimpleSurface::ALLOCATE_SIMPLE
#ifdef CAPTURE
                                                                  ,CAPTURE_SURFACE_KIND_COMMAND
#endif
                                                                   ); }
    inline BOOL destroy (void)          { return m_Surface.destroy(); }

    inline void own     (DWORD dwAddress, DWORD dwSize, DWORD dwHeapLocation)
                                                               { m_Surface.own(dwAddress, dwSize, 1, dwHeapLocation, FALSE); }

    inline BOOL isOwner (void)    const { return m_Surface.isOwner(); }


    inline BOOL growBy  (DWORD dwSize, BOOL bPreserveContents) { return m_Surface.growBy(dwSize, bPreserveContents); }

    /*
     * construction
     */
public:
    CCommandBuffer (CNvObject                 *pWrapperObject,
                    LPDDRAWI_DDRAWSURFACE_LCL  pDDSurfaceLcl);
    ~CCommandBuffer (void);
};

/*****************************************************************************
 * D3D callbacks
 */
DWORD __stdcall D3DCanCreateExecuteBuffer32 (LPDDHAL_CANCREATESURFACEDATA pCanCreateSurfaceData);
DWORD __stdcall D3DCreateExecuteBuffer32    (LPDDHAL_CREATESURFACEDATA    pCreateSurfaceData);
DWORD __stdcall D3DDestroyExecuteBuffer32   (LPDDHAL_DESTROYSURFACEDATA   pDestroySurfaceData);
DWORD __stdcall D3DLockExecuteBuffer32      (LPDDHAL_LOCKDATA             pLockData);
DWORD __stdcall D3DUnlockExecuteBuffer32    (LPDDHAL_UNLOCKDATA           pUnlockData);

#ifdef __cplusplus
}
#endif

#endif // _nvVB_h