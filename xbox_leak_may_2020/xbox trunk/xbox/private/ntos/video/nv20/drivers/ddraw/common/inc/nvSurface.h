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
// **************************************************************************

#ifndef _nvSurface_h
#define _nvSurface_h

#ifndef __cplusplus
#error This file requires a C++ compiler
#endif

extern DWORD g_adwSurfaceContextDMA[9];

//---------------------------------------------------------------------------

// class CSimpleSurface
//
// simple surface object
//  an instance will encapsulate a linear chunk of memory in any memory space.
//  it also contains HW specifics needed for HW access.
//  it is not very smart.

class CSimpleSurface
{
    // constants
public:
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
        HEAP_NOWHERE            = 0x0,

        FLAG_UPTODATE           = 0x00000100,   // surface data is up to date (can be used)
        FLAG_RENDERTARGET       = 0x00000200,   // surface is a render target
        FLAG_ZBUFFER            = 0x00000400,   // surface is a zbuffer
        FLAG_DONOTOWNMEMORY     = 0x00000800,   // memory management is done by somebody else (not self)
        FLAG_TILED              = 0x00001000,   // surface was allocated in tiled memory
        FLAG_SWIZZLED           = 0x00002000,   // surface is a swizzled (used by nvSetRenderTargetDX7)
        FLAG_available          = 0x00004000,
        FLAG_HASALPHA           = 0x00008000,   // surface format has alpha
        FLAG_ISDXT              = 0x00010000,   // surface format is DXT
#ifdef  STEREO_SUPPORT
        FLAG_STEREO             = 0x00020000,   // surface is stereo
        FLAG_NOTREAL            = 0x00040000,   // surface is stereo
#endif  //STEREO_SUPPORT

        // original lock parameters
        LOCK_NORMAL             = 0,            // normal access. do not rename
        LOCK_RENAME             = 1,            // rename if allowed
        LOCK_RENAME_COPY        = 3,            // rename if allowed and also copy contents forward

        LOCK_READ,                 //permissions
        LOCK_WRITE,

        LOCKED_NOT=0,               // for lock states
        LOCKED_READ=1,
        LOCKED_WRITE=2,

        // allocation flags
        ALLOCATE_SIMPLE         = 0x00000001,
        ALLOCATE_TILED          = 0x00000002,
        ALLOCATE_AS_ZBUFFER_UNC = 0x00000010,   // uncompressed z-buffer
        ALLOCATE_AS_ZBUFFER_CMP = 0x00000020,   // compressed z-buffer
        ALLOCATE_AS_ZBUFFER     = 0x00000030,   // any z format
		ALLOCATE_AS_STEREO		= 0x00000040	// stereo surface
    };

    // members
protected:
//bank
    DWORD      m_dwAddress;       // surface address [absolute]
    DWORD      m_dwOffset;        // surface offset [depends on dma context]
    DWORD      m_dwFormat;
    DWORD      m_dwBPP;           // bytes per pixel
    DWORD      m_dwBPPRequested;  // what did the app originally ask for? (not necessarily what was granted)
    DWORD      m_dwWidth;
    DWORD      m_dwHeight;        // height = 1 for vertex buffers, etc
    DWORD      m_dwDepth;         // depth = 1 for vertex buffers, etc
    DWORD      m_dwMultiSampleBits;   // number of samples per pixel
//bank
    DWORD      m_dwPitch;         // pitch = size for vertex buffers, etc.
    DWORD      m_dwSlicePitch;    // pitch of 2D slice for volume textures
    DWORD      m_dwFourCC;        // fourCC code (0 if not a fourCC surface)
    DWORD      m_dwFlags;         // flags
    DWORD      m_dwRetireDate;    // date that HW access retires
    CNvObject *m_pWrapperObject;
    DWORD      m_dwUsedMin;       // supertri only
    DWORD      m_dwUsedMax;       // supertri only
//bank
    DWORD      m_dwAllocFlags;    // our allocation flags (for debug purposes)
#ifdef  DEBUG_SURFACE_PLACEMENT
    DWORD      m_dwRMAllocFlags;  // Resman alloc flags (for debug purposes
#endif
#ifdef  STEREO_SUPPORT
    DWORD      m_dwAddressRight;  // right eye surface address [absolute]
    DWORD      m_dwOffsetRight;   // right eye surface offset [depends on dma context]
    DWORD      _dummy[5];
#else
    DWORD      _dummy[7];
#endif  //STEREO_SUPPORT
//bank

    // inline helper methods
public:
    inline BOOL  isValid           (void)         const { return m_dwAddress != 0; }
    inline BOOL  hwCanRead         (void)         const { return (m_dwFlags & HEAP_LOCATION_MASK) != HEAP_SYS; }
    inline BOOL  hwCanWrite        (void)         const { return (m_dwFlags & HEAP_LOCATION_MASK) == HEAP_VID; }
    inline BOOL  isReadCached      (void)         const { return (m_dwFlags & HEAP_LOCATION_MASK) >= HEAP_PCI; }
    inline BOOL  isWriteCached     (void)         const { return (m_dwFlags & HEAP_LOCATION_MASK) >= HEAP_PCI; }
    inline BOOL  isWriteCombined   (void)         const { return (m_dwFlags & HEAP_LOCATION_MASK) <= HEAP_AGP; }

    inline DWORD getSize           (void)         const { return (m_dwPitch * m_dwHeight); }
    inline DWORD getContextDMA     (void)         const { return g_adwSurfaceContextDMA[m_dwFlags & HEAP_LOCATION_MASK]; }
    inline DWORD getHeapLocation   (void)         const { return m_dwFlags & HEAP_LOCATION_MASK; }
    inline DWORD getOffset         (void)         const { return m_dwOffset;  }
    // Used by updateCachedValues in CVertexBuffer to mainatin a cached copy of the flags.
    // It's only added in the right places for Vertex Buffers, as well.
    inline DWORD getFlags          (void)         const { return m_dwFlags; }

    // this should only be used in DX6 DestroySurface32 to restore the surface address of the render target
    // the surface must be in video memory only
    inline void  setAddress        (DWORD dwAddress)    { m_dwAddress = dwAddress; m_dwOffset = dwAddress - pDriverData->BaseAddress; }

    inline DWORD getAddress        (void)         const { return m_dwAddress; }
#ifdef WINNT
    inline DWORD getfpVidMem       (void)         const { return ((getHeapLocation() & (HEAP_VID | HEAP_AGP)) ? m_dwOffset : m_dwAddress); }
#else
    inline DWORD getfpVidMem       (void)         const { return m_dwAddress; }
#endif // WINNT
    inline void  setPitch          (DWORD dwP)          { m_dwPitch = dwP; }
    inline DWORD getPitch          (void)         const { return (m_dwPitch); }

    inline void  setSlicePitch     (DWORD dwSP)         { m_dwSlicePitch = dwSP; }
    inline DWORD getSlicePitch     (void)         const { return (m_dwSlicePitch); }

    inline void  setWidth          (DWORD dwW)          { m_dwWidth = dwW; }
    inline DWORD getWidth          (void)         const { return (m_dwWidth); }

    inline void  setHeight         (DWORD dwH)          { m_dwHeight = dwH; }
    inline DWORD getHeight         (void)         const { return (m_dwHeight); }

    inline void  setDepth          (DWORD dwD)          { m_dwDepth = dwD; }
    inline DWORD getDepth          (void)         const { return (m_dwDepth); }

    inline void  setMultiSampleBits(DWORD dwM)          { m_dwMultiSampleBits = dwM; }
    inline DWORD getMultiSampleBits(void)         const { return (m_dwMultiSampleBits); }

    inline void  tagUpToDate       (void)               { m_dwFlags |=  FLAG_UPTODATE; }
    inline void  tagOutOfDate      (void)               { m_dwFlags &= ~FLAG_UPTODATE; }
    inline BOOL  isUpToDate        (void)         const { return m_dwFlags & FLAG_UPTODATE; }

    inline void  tagAsLinear       (void)               { m_dwFlags &= ~FLAG_SWIZZLED; }
    inline void  tagAsSwizzled     (void)               { m_dwFlags |=  FLAG_SWIZZLED; } // used to tag render targets only
    inline BOOL  isSwizzled        (void)         const { return m_dwFlags & FLAG_SWIZZLED; }

    inline void  tagAsZBuffer      (void)               { m_dwFlags |=  FLAG_ZBUFFER; }
    inline BOOL  isZBuffer         (void)         const { return m_dwFlags & FLAG_ZBUFFER; }

    inline void  tagAsOwner        (void)               { m_dwFlags &= ~FLAG_DONOTOWNMEMORY; }
    inline void  tagNotAsOwner     (void)               { m_dwFlags |=  FLAG_DONOTOWNMEMORY; }

    inline void  tagAsDynSys       (void)               { m_dwFlags &= ~HEAP_DYNSYSBIT; }
    inline void  tagNotAsDynSys    (void)               { m_dwFlags |=  HEAP_DYNSYSBIT; }

    inline BOOL  isOwner           (void)         const { return (m_dwFlags & FLAG_DONOTOWNMEMORY) == 0; }


    inline DWORD getRetireDate     (void)         const { return m_dwRetireDate; }
           BOOL  isBusy            (void)         const;
           void  reset             (void);

           void  own               (DWORD dwAddress, DWORD dwPitch, DWORD dwHeight,
                                    DWORD dwHeapLocation, BOOL bOwnMemory);
           void  disown            (void);

    // formatted surface helpers
    inline void       setWrapper       (CNvObject *nvO)         { m_pWrapperObject = nvO; }
    inline CNvObject* getWrapper       (void)             const { return (m_pWrapperObject); }

    inline void       setBPP           (DWORD dwBPP)            { m_dwBPP = dwBPP; }
    inline DWORD      getBPP           (void)             const { return (m_dwBPP); }

    inline void       setBPPRequested  (DWORD dwBPPReq)         { m_dwBPPRequested = dwBPPReq; }
    inline DWORD      getBPPRequested  (void)             const { return (m_dwBPPRequested); }

    inline void       setFormat        (DWORD dwFmt)            { m_dwFormat = dwFmt;
                                                                  // note: determination of alpha's existence in palettized
                                                                  // textures is deferred until a palette is set
                                                                  if ((dwFmt == NV_SURFACE_FORMAT_A1R5G5B5)       ||
                                                                      (dwFmt == NV_SURFACE_FORMAT_A4R4G4B4)       ||
                                                                      (dwFmt == NV_SURFACE_FORMAT_A8R8G8B8)       ||
                                                                      (dwFmt == NV_SURFACE_FORMAT_X1A7R8G8B8)     ||
                                                                      (dwFmt == NV_SURFACE_FORMAT_DXT1_A1R5G5B5)  ||
                                                                      (dwFmt == NV_SURFACE_FORMAT_DXT23_A8R8G8B8) ||
                                                                      (dwFmt == NV_SURFACE_FORMAT_DXT45_A8R8G8B8))
                                                                  {
                                                                      m_dwFlags |= FLAG_HASALPHA;
                                                                  }
                                                                  else
                                                                  {
                                                                      m_dwFlags &= ~FLAG_HASALPHA;
                                                                  }
                                                                }
    inline DWORD      getFormat        (void)             const { return (m_dwFormat); }

    inline void       setFourCC        (DWORD dw4cc)            { m_dwFourCC = dw4cc;
                                                                  if (((dw4cc == FOURCC_DXT1) ||
                                                                       (dw4cc == FOURCC_DXT2) ||
                                                                       (dw4cc == FOURCC_DXT3) ||
                                                                       (dw4cc == FOURCC_DXT4) ||
                                                                       (dw4cc == FOURCC_DXT5)))
                                                                  {
                                                                      m_dwFlags |= FLAG_ISDXT;
                                                                  }
                                                                  else
                                                                  {
                                                                      m_dwFlags &= ~FLAG_ISDXT;
                                                                  }
                                                                }
    inline DWORD      getFourCC        (void)             const { return m_dwFourCC; }

    inline BOOL       isFourCC         (void)             const { return m_dwFourCC != 0; }
    inline BOOL       isDXT            (void)             const { return (m_dwFlags & FLAG_ISDXT) == FLAG_ISDXT; }

    inline void       tagHasAlpha      (void)                   { m_dwFlags |=  FLAG_HASALPHA; }
    inline void       tagHasNoAlpha    (void)                   { m_dwFlags &= ~FLAG_HASALPHA; }
    inline BOOL       hasAlpha         (void)             const { return (m_dwFlags & FLAG_HASALPHA) == FLAG_HASALPHA; }

    inline BOOL       hasStencil       (void)             const { return m_dwFormat == NV_SURFACE_FORMAT_Z24S8; }
    
    inline DWORD      getAllocFlags    (void)             const { return m_dwAllocFlags; }
#ifdef  DEBUG_SURFACE_PLACEMENT
    inline DWORD      getRMAllocFlags  (void)             const { return m_dwRMAllocFlags; }
#endif

    // helpers
public:
    inline void hwLock    (DWORD dwAccess) {}   // aquire hw access
           void hwUnlock  (void);               // release hw access
           void cpuLock   (DWORD dwAccess);     // aquire cpu access
    inline void cpuUnlock (void) {}             // release cpu access

    void swap (CSimpleSurface* pSurface);         // swap surface pointers

    inline DWORD modifyDDCaps (DWORD dwCaps) const
    {
        // clear all flags
        dwCaps &= ~(DDSCAPS_VIDEOMEMORY    |
                    DDSCAPS_LOCALVIDMEM    |
                    DDSCAPS_NONLOCALVIDMEM |
                    DDSCAPS_SYSTEMMEMORY);
        // put back the flags we want
        switch (m_dwFlags & HEAP_LOCATION_MASK)
        {
            case HEAP_VID:
                dwCaps |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
                break;
            case HEAP_AGP:
            case HEAP_PCI:
                dwCaps |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_NONLOCALVIDMEM);
                break;
            // DDRAW just hates it if you specify the DDSCAPS_SYSTEMMEMORY flag. basically it will
            //  never call back to free surfaces. Ugghh.
            // believe it or not, it actually want the VidMem flag set!
            default:
                dwCaps |= (DDSCAPS_VIDEOMEMORY);
                break;
        }
        // return the new flags
        return (dwCaps);
    }

    // construction
public:
    BOOL create   (DWORD dwPitch, DWORD dwHeight, DWORD dwDepth, DWORD dwBPPGranted,
                   DWORD dwAllowedHeaps, DWORD dwPreferredHeap,
                   DWORD dwAllocFlags
#ifdef CAPTURE
                  ,DWORD dwUsageKind  // CAPTURE_SURFACE_KIND_xxx
#endif
                   );

    BOOL create   (const CSimpleSurface* pTemplateSurface
#ifdef CAPTURE
                  ,DWORD dwUsageKind  // CAPTURE_SURFACE_KIND_xxx
#endif
                   );

    // formatted surface create
    BOOL create   (CNvObject *pWrapperObject, DWORD dwFormat,
                   DWORD dwWidth, DWORD dwHeight, DWORD dwDepth, DWORD dwMultiSampleBits,
                   DWORD dwBPPRequested, DWORD dwBPPGranted,
                   DWORD dwPitch, DWORD dwAllowedHeaps, DWORD dwPreferredHeap,
                   DWORD dwAllocFlags
#ifdef CAPTURE
                  ,DWORD dwUsageKind  // CAPTURE_SURFACE_KIND_xxx
#endif
                   );

    BOOL recreate (DWORD dwBPP);

    BOOL growBy   (DWORD dwDelta, BOOL bPreserveContents);

    BOOL destroy  (void);


#ifdef  STEREO_SUPPORT
    inline void     tagAsStereo     (void)       { m_dwFlags |=  FLAG_STEREO; }
    inline void     tagNotAsStereo  (void)       { m_dwFlags &= ~FLAG_STEREO; }
    inline void     tagNotReal      (void)       { m_dwFlags |=  FLAG_NOTREAL; }
    inline DWORD    getAddressRight (void) const { return m_dwAddressRight;  }
    inline DWORD    getOffsetRight  (void) const { return m_dwOffsetRight;  }
    inline BOOL     isStereo        (void) const { return m_dwFlags & FLAG_STEREO; }
    BOOL            createStereo    (void);
    BOOL            destroyStereo   (void);
#endif  //STEREO_SUPPORT

    inline CSimpleSurface (void)
    {
#ifdef DEBUG
        memset (this,0xcc,sizeof(*this));
#endif
        m_dwAddress = 0;
        m_dwWidth   = 0;
        m_dwHeight  = 0;
        m_dwDepth   = 0;
        m_dwMultiSampleBits = 0;
        m_dwPitch   = 0;
        m_dwFlags   = 0;
        m_dwFourCC  = 0;
        m_dwUsedMin = ~0;
        m_dwUsedMax = 0;
        m_pWrapperObject = 0; //HH
#ifdef  STEREO_SUPPORT
        m_dwAddressRight = 0;
        m_dwOffsetRight = 0;
#endif  //STEREO_SUPPORT
    }

    // assumed to be init with zeroes
    inline ~CSimpleSurface (void)
    {
        if (m_dwAddress)
        {
            destroy();
        }
    }
};

//---------------------------------------------------------------------------

// class CComplexSurface
//
// wrapper for simple surfaces that reduces CPU blocking when accessed.
//  potentially consumes tons of memory.
// only useful for textures and vertex buffers.
// cannot be instanced by itself - you have to inherit and implement
//  abstract methods.

template<int cMax> class CComplexSurface
{
    // contants
public:
    enum
    {
    FLAG_RENAME     = 0x00000001,
    FLAG_RENAME_CALL        = 0x00000002,   // rename will call user function

        RENAME_DO_NOTHING       = 0,            // rename options
        RENAME_EXTERNAL         = 1,
    };

    // types
public:
    typedef BOOL (__stdcall *PCANRENAMEPROC)(void *pContext);
    typedef void (__stdcall *PRENAMEPROC)   (void *pContext,DWORD dwOldIndex,DWORD dwNewIndex);

    // members
protected:
//bank
    DWORD           m_dwActiveSurface;      // active one
    DWORD           m_dwPreviousSurface;    // last one used
    DWORD           m_dwTotalSurfaces;      // # in use
    DWORD           m_dwFlags;
    PCANRENAMEPROC  m_pfnCanRenameProc;
    PRENAMEPROC     m_pfnRenameProc;
    void           *m_pRenameContext;
    DWORD           _pad[1];
//bank
    CSimpleSurface  m_aSurface[cMax];       // all surfaces

    // inline helpers
public:
    inline CSimpleSurface* getSurface         (DWORD dwIndex)       { return &m_aSurface[dwIndex]; }
    inline CSimpleSurface* getActiveSurface   (void)                { return &m_aSurface[m_dwActiveSurface]; }
    inline CSimpleSurface* getPreviousSurface (void)                { return &m_aSurface[m_dwPreviousSurface]; }
    inline DWORD           getTotalSurfaces   (void)          const { return m_dwTotalSurfaces; }

    inline void      reset              (void)                { for (DWORD i = getTotalSurfaces(),j = 0; i; j++,i--) m_aSurface[j].reset(); }

           void      tagRenameEnable    (DWORD           dwRenameType,
                                         DWORD           dwPreAllocCount  = 1,  // create MUST have been called before you can set this to more than 1
                                         PCANRENAMEPROC  pfnCanRenameProc = NULL,
                                         PRENAMEPROC     pfnRenameProc    = NULL,
                                         void           *pContext         = NULL);
    inline void      tagRenameDisable   (void)                { m_dwFlags &= ~FLAG_RENAME; }
    inline BOOL      canRename          (void)          const { return m_dwFlags & FLAG_RENAME; }

           void      forceRename        (DWORD dwNewSurface,DWORD dwNewAddress);

    // helpers
public:
    inline void hwLock    (DWORD dwAccess) { getActiveSurface()->hwLock (dwAccess); }
    inline void hwUnlock  (void)           { getActiveSurface()->hwUnlock(); }
           void cpuLock   (DWORD dwAccess);
    inline void cpuUnlock (void)           { getActiveSurface()->cpuUnlock(); }

    // construction
public:
    BOOL create  (CNvObject *pWrapper, DWORD dwSize, DWORD dwAllowedHeaps, DWORD dwPreferredHeap
#ifdef CAPTURE
                 ,DWORD dwUsageKind  // CAPTURE_SURFACE_KIND_xxx
#endif
                  );
    BOOL own     (DWORD dwAddress, DWORD dwPitch, DWORD dwLogHeight, DWORD dwHeapLocation, BOOL bOwnMemory);
    BOOL disown  (void);
    BOOL swap    (CComplexSurface *pCSurf);
    BOOL destroy (void);

    CComplexSurface (void) { m_dwActiveSurface = m_dwTotalSurfaces = m_dwFlags = 0; }
};

#endif // _nvSurface_h
