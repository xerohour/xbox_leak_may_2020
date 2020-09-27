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
#ifndef _nvTex_h
#define _nvTex_h

#ifndef __cplusplus
#error This file requires a C++ compiler
#endif

#include "nvUtil.h"

#define TOTAL_TEX_RENAME 8

#ifdef CNT_TEX
#define MAX_FRAME_CNT 5000
extern DWORD g_dwVidTexDL[], g_dwAgpTexDL[];
extern DWORD g_dwFrames;
#endif
//---------------------------------------------------------------------------

struct _def_nv_d3d_context;
typedef _def_nv_d3d_context *PNVD3DCONTEXT;

// class CTexture
//
// texture object

class CTexture
{
    // more flags
public:
    enum
    {
        FLAG_USERMIPLEVEL       = 0x80000000,   // this structure is part of a mip map chain (does not own surface)
        FLAG_PALETTIZED         = 0x40000000,   // texture is palettized
        FLAG_COMPRESSED         = 0x20000000,   // texture is compressed
        FLAG_CUBEMAP            = 0x10000000,   // texture is part of a cube map
        FLAG_VOLUMEMAP          = 0x08000000,   // texture is volumetric data
        FLAG_HASHANDLE          = 0x04000000,   // texture has a legacy handle (dx3,dx5)
        FLAG_MANAGED            = 0x02000000,   // app wants us to manage this texture
        FLAG_COLORKEY           = 0x01000000,   // has a color key
        FLAG_RENAMENOTCOPIED    = 0x00800000,   // set when a texture was just renamed and its contents is not copied over yet
        FLAG_SUBSURFDIRTY       = 0x00400000,   // a mip or cube map sub surface is dirty - force chain update in hwLock
        FLAG_AUTOPALETTIZED     = 0x00200000,   // texture is dynamically palettized by texture blit
        FLAG_UNPALETTIZED       = 0x00100000,   // flag that this serface was palettized and then converted back
    };

    // members
protected:
// bank
    CSimpleSurface                    m_surfLinear;
    CComplexSurface<TOTAL_TEX_RENAME> m_surfSwizzled;

// bank
    CTexture              *m_pBaseTexture;  // the root of the mipmap chain, or for cubemaps, the root of the +x chain. the root points to itself.
    DWORD                  m_dwLogWidth;
    DWORD                  m_dwLogHeight;
    DWORD                  m_dwLogDepth;
    DWORD                  m_dwMipMapCount;
    DWORD                  m_dwPriority;
    DWORD                  m_dwColorKey;
    DWORD                  m_dwFlags;

// bank
    DWORD                  m_dwAutoPaletteOffset; // offset to auto palette
    DWORD                  m_dwOriginalFormat;    // contains the original texture format (if auto palette converts it)

    // cube map data (common to all surfaces that are part of the same cube map)
    DWORD                  m_dwCubeMapSizeFacePadded;

    // we try to precompute and store as much HW state as possible
    DWORD                  m_dwNV056Control0;
    DWORD                  m_dwNV056Format;

#ifdef TEX_MANAGE
    // links in chains of textures used for texture management
    CTexture              *m_pNext;
    CTexture              *m_pPrev;
    PNVD3DCONTEXT          m_pContext; // the context in whose lists this texture is kept and with which it is
                                       // associated. we do our best to make these associations correctly, but
                                       // since textures exist outside of contexts, we're sometimes forced to
                                       // make arbitrary choices.
#endif

    // helpers
public:
    inline void            setBaseTexture      (CTexture *pTexture)       { m_pBaseTexture = pTexture; }
    inline CTexture*       getBaseTexture      (void)               const { return (m_pBaseTexture); }
    inline BOOL            isBaseTexture       (void)               const { return ((this==m_pBaseTexture) ? TRUE : FALSE); };

    inline void            setPriority         (DWORD dwPriority)         { m_dwPriority = dwPriority; }

    inline DWORD           getMipMapCount      (void)               const { return m_dwMipMapCount; }
    inline void            tagUserMipLevel     (void)                     { m_dwFlags |= FLAG_USERMIPLEVEL; }
    inline BOOL            isUserMipLevel      (void)               const { return ((m_dwFlags & FLAG_USERMIPLEVEL) == FLAG_USERMIPLEVEL); }

    inline void            tagPalettized       (void)                     { m_dwFlags |= FLAG_PALETTIZED; }
    inline BOOL            isPalettized        (void)               const { return ((m_dwFlags & FLAG_PALETTIZED) == FLAG_PALETTIZED); }

    // for auto-palette use
    inline void            tagAutoPalettized   (void)                     { m_dwFlags |= FLAG_AUTOPALETTIZED; }
    inline void            untagAutoPalettized (void)                     { m_dwFlags &= ~FLAG_AUTOPALETTIZED; }
    inline void            tagUnpalettized     (void)                     { m_dwFlags |= FLAG_UNPALETTIZED; }
    inline BOOL            isReused            (void)               const { return m_dwFlags & FLAG_UNPALETTIZED; }
    inline void            setOriginalFormat   (DWORD dwFormat)           { m_dwOriginalFormat = dwFormat; }
    inline DWORD           getOriginalFormat   (void)               const { return m_dwOriginalFormat; }
    inline void            resetOriginalFormat (void)                     { setFormat(m_dwOriginalFormat); }
    inline void            setAutoPaletteOffset(DWORD dwPalette)          { m_dwAutoPaletteOffset = dwPalette; }
    inline DWORD           getAutoPaletteOffset(void)               const { return m_dwAutoPaletteOffset; }
    inline BOOL            hasAutoPalette      (void)               const { return m_dwFlags & FLAG_AUTOPALETTIZED; }

    inline void            tagCubeMap          (void)                     { m_dwFlags |= FLAG_CUBEMAP; }
    inline BOOL            isCubeMap           (void)               const { return ((m_dwFlags & FLAG_CUBEMAP) == FLAG_CUBEMAP); }

    inline void            tagVolumeMap          (void)                     { m_dwFlags |= FLAG_VOLUMEMAP; }
    inline BOOL            isVolumeMap           (void)               const { return ((m_dwFlags & FLAG_VOLUMEMAP) == FLAG_VOLUMEMAP); }

    inline void            tagHasHandle        (void)                     { m_dwFlags |= FLAG_HASHANDLE; }
    inline BOOL            hasHandle           (void)               const { return ((m_dwFlags & FLAG_HASHANDLE) == FLAG_HASHANDLE); }

    inline void            tagManaged          (void)                     { m_dwFlags |= FLAG_MANAGED; }
    inline BOOL            isManaged           (void)               const { return ((m_dwFlags & FLAG_MANAGED) == FLAG_MANAGED); }

    inline void            tagRenamedNotCopied (void)                     { m_dwFlags |=  FLAG_RENAMENOTCOPIED; }
    inline void            tagRenamedAndCopied (void)                     { m_dwFlags &= ~FLAG_RENAMENOTCOPIED; }
    inline BOOL            isRenamedNotCopied  (void)               const { return ((m_dwFlags & FLAG_RENAMENOTCOPIED) == FLAG_RENAMENOTCOPIED); }

    inline void            tagSubSurfacesDirty (void)                     { m_dwFlags |=  FLAG_SUBSURFDIRTY; }
    inline void            tagSubSurfacesClean (void)                     { m_dwFlags &= ~FLAG_SUBSURFDIRTY; }
    inline BOOL            isSubSurfacesDirty  (void)               const { return ((m_dwFlags & FLAG_SUBSURFDIRTY) == FLAG_SUBSURFDIRTY); }

    inline CSimpleSurface* getSwizzled         (void)                     { return (m_surfSwizzled.getActiveSurface()); }
    inline CSimpleSurface* getPreviousSwizzled (void)                     { return (m_surfSwizzled.getPreviousSurface()); }
    inline CSimpleSurface* getLinear           (void)                     { return (&m_surfLinear); }
    inline CTexture*       getBaseTexture      (void)                     { return m_pBaseTexture; }

    inline CNvObject*      getWrapper          (void)               const { return (m_surfLinear.getWrapper()); }
    inline BOOL            hwCanWrite          (void)               const { return (m_surfLinear.hwCanWrite()); }

    inline DWORD           getSize             (void)               const { return (m_surfLinear.getSize()); }

    inline void            setWidth            (DWORD dwW)                { m_surfLinear.setWidth(dwW);
                                                                            ASM_LOG2(dwW);
                                                                            m_dwLogWidth = dwW;
                                                                            #if (NVARCH >= 0x010)
                                                                            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) calculateNV056ControlWords();
                                                                            #endif  // NVARCH >= 0x010
                                                                          }

    inline DWORD           getWidth            (void)               const { return (m_surfLinear.getWidth()); }
    inline DWORD           getLogWidth         (void)               const { return (m_dwLogWidth); }  // for DXTs, this is the UNEXPANDED width
    inline DWORD           getLogicalWidth     (void)               const { return isDXT() ? m_surfLinear.getWidth() * 4 : m_surfLinear.getWidth(); }
    inline DWORD           getLogicalLogWidth  (void)               const { return isDXT() ? (m_dwLogWidth + 2) : m_dwLogWidth; }

    inline void            setHeight           (DWORD dwH)                { m_surfLinear.setHeight(dwH);
                                                                            ASM_LOG2(dwH);
                                                                            m_dwLogHeight = dwH;
                                                                            #if (NVARCH >= 0x010)
                                                                            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) calculateNV056ControlWords();
                                                                            #endif  // NVARCH >= 0x010
                                                                          }

    inline DWORD           getHeight           (void)               const { return (m_surfLinear.getHeight()); }
    inline DWORD           getLogHeight        (void)               const { return (m_dwLogHeight); }  // for DXTs, this is the UNEXPANDED height
    inline DWORD           getLogicalHeight    (void)               const { return isDXT() ? m_surfLinear.getHeight() * 4 : m_surfLinear.getHeight(); }
    inline DWORD           getLogicalLogHeight (void)               const { return isDXT() ? (m_dwLogHeight + 2) : m_dwLogHeight; }

    inline void            setDepth            (DWORD dwD)                { m_surfLinear.setDepth(dwD);
                                                                            ASM_LOG2(dwD);
                                                                            m_dwLogDepth = dwD; }
    inline DWORD           getDepth            (void)               const { return (m_surfLinear.getDepth()); }
    inline DWORD           getLogDepth         (void)               const { return (m_dwLogDepth); }  // What does this mean for 3D DXT???
    inline DWORD           getLogicalDepth     (void)               const { return (m_surfLinear.getDepth()); }
    inline DWORD           getLogicalLogDepth  (void)               const { return (m_dwLogDepth); }

    inline DWORD           getNumTexCoords     (void)               const { return ((isVolumeMap() || isCubeMap()) ? 3 : 2); }

    inline void            setBPP              (DWORD dwBPP)              { m_surfLinear.setBPP(dwBPP); }
    inline DWORD           getBPP              (void)               const { return (m_surfLinear.getBPP()); }

    inline void            setBPPRequested     (DWORD dwBPP)              { m_surfLinear.setBPPRequested(dwBPP); }
    inline DWORD           getBPPRequested     (void)               const { return (m_surfLinear.getBPPRequested()); }

    inline void            setPitch            (DWORD dwPitch)            { m_surfLinear.setPitch(dwPitch); }
    inline DWORD           getPitch            (void)               const { return (m_surfLinear.getPitch()); }

    inline void            setSlicePitch       (DWORD dwSlicePitch)       { m_surfLinear.setSlicePitch(dwSlicePitch); }
    inline DWORD           getSlicePitch       (void)               const { return (m_surfLinear.getSlicePitch()); }

    inline void            setFormat           (DWORD dwFmt)              { m_surfLinear.setFormat(dwFmt);
                                                                            #if (NVARCH >= 0x010)
                                                                            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) calculateNV056ControlWords();
                                                                            #endif  // NVARCH >= 0x010
                                                                          }

    inline DWORD           getFormat           (void)               const { return (m_surfLinear.getFormat()); }

    inline void            setFourCC           (DWORD dw4cc)              { m_surfLinear.setFourCC(dw4cc); }
    inline BOOL            getFourCC           (void)               const { return (m_surfLinear.getFourCC()); }
    inline BOOL            isFourCC            (void)               const { return (m_surfLinear.isFourCC()); }
    inline BOOL            isDXT               (void)               const { return (m_surfLinear.isDXT()); }
    inline BOOL            isBumpMap           (void)               const { return ((m_surfLinear.getFormat() == NV_SURFACE_FORMAT_DV8DU8)   ||
                                                                                    (m_surfLinear.getFormat() == NV_SURFACE_FORMAT_L6DV5DU5) ||
                                                                                    (m_surfLinear.getFormat() == NV_SURFACE_FORMAT_X8L8DV8DU8)); }

    inline void            tagHasAlpha         (void)                     { m_surfLinear.tagHasAlpha(); }
    inline void            tagHasNoAlpha       (void)                     { m_surfLinear.tagHasNoAlpha(); }
    inline BOOL            hasAlpha            (void)               const { return (m_surfLinear.hasAlpha()); }

#ifdef TEX_MANAGE
    inline void            setNext             (CTexture *pNext)          { m_pNext = pNext; }
    inline CTexture*       getNext             (void)               const { return (m_pNext); }
    inline void            setPrev             (CTexture *pPrev)          { m_pPrev = pPrev; }
    inline CTexture*       getPrev             (void)               const { return (m_pPrev); }
    inline void            setContext          (PNVD3DCONTEXT pCtx)       { m_pContext = pCtx; }
    inline PNVD3DCONTEXT   getContext          (void)               const { return (m_pContext); }

    inline BOOL            isResident          (void)                     { return ((getSwizzled()->getHeapLocation() != CSimpleSurface::HEAP_SYS) ? TRUE : FALSE); }
    inline BOOL            isEvicted           (void)                     { return ((getSwizzled()->getHeapLocation() == CSimpleSurface::HEAP_SYS) ? TRUE : FALSE); }
#endif

    inline void            tagColorKey         (void)                     { m_dwFlags |= FLAG_COLORKEY; }
    inline BOOL            hasColorKey         (void)               const { return ((m_dwFlags & FLAG_COLORKEY) == FLAG_COLORKEY); }
    inline void            setColorKey         (DWORD dwCK)               { m_dwColorKey = dwCK; }
    inline DWORD           getColorKey         (void)               const { return (m_dwColorKey); }

    inline DWORD           getNV056Control0    (void)               const { return (m_dwNV056Control0); }
    inline DWORD           getNV056Format      (void)               const { return (m_dwNV056Format); }

           void            hwLock              (PNVD3DCONTEXT pContext, DWORD dwAccess);
    inline void            hwUnlock            (void)                     { m_surfSwizzled.hwUnlock(); }
    inline void            cpuLockSwz          (DWORD dwAccess)           { m_surfSwizzled.cpuLock (dwAccess); }
    inline void            cpuUnlockSwz        (void)                     { m_surfSwizzled.cpuUnlock(); }
    inline void            cpuLockLin          (DWORD dwAccess)           { m_surfLinear.cpuLock (dwAccess); }
    inline void            cpuUnlockLin        (void)                     { m_surfLinear.cpuUnlock(); }

           BOOL            internalCanRename   (void);
           void            internalRename      (DWORD dwOldIndex, DWORD dwNewIndex);
           void            enableRenaming      (void);

    // methods
public:
    DWORD        calcMipMapSize              (DWORD dwLogU, DWORD dwLogV, DWORD dwLogP, DWORD dwMipMapLevels);
    BOOL         prepareLinearAsRenderTarget (void);
    BOOL         updateLinearSurface         (void);
    BOOL         updateSwizzleSurface        (PNVD3DCONTEXT pContext);
    void         calculateNV056ControlWords  (void);
    static DWORD getCubeMapFaceNum           (DWORD dwFace);
#ifdef TEX_MANAGE
    BOOL         evict                       (void);
    BOOL         fetch                       (void);
#endif

    // construction
public:
    // new creation
    BOOL create  (CNvObject *pWrapperObject,      // NvObject wrapping this texture
                  DWORD      dwWidth,             // width
                  DWORD      dwHeight,            // height
                  DWORD      dwDepth,             // depth
                  DWORD      dwBPPRequested,      // bytes per pixel requested
                  DWORD      dwBPPGranted,        // bytes per pixel granted
                  DWORD      dwMipMapCount,       // # of mipmaps (including self)
                  DWORD      dwFormat,            // texture format NV_TEXTURE_FORMAT_xxx
                  DWORD      dwAllowedHeaps,      // allowed heaps to allocate in
                  DWORD      dwPreferredHeap);    // preferred heap
    BOOL create  (CNvObject *pWrapperObject,      // NvObject wrapping this texture
                  DWORD      dwThisFace,          // cubemap face
                  CTexture  *pBaseTexture,        // create a mipmap from this base
                  DWORD      dwMipMapLevel);      // this is the nth mipmap (base = 0)
    // new destruction
    BOOL destroy (void);

     CTexture ();
    ~CTexture ();
};

//---------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif

// public functions
DWORD nvTextureCreateSurface    (LPDDRAWI_DDRAWSURFACE_LCL pLcl, LPDDRAWI_DIRECTDRAW_GBL pDDGbl, LPDDSURFACEDESC pDDSurfDesc);
BOOL  nvTextureDestroySurface   (LPDDHAL_DESTROYSURFACEDATA pDestorySurfaceData, LPDDRAWI_DDRAWSURFACE_LCL pLcl);
DWORD nvTextureBlt              (PNVD3DCONTEXT pContext, LPDDHAL_BLTDATA pBltData);
DWORD nvTextureBltDX7           (PNVD3DCONTEXT pContext, CNvObject *pSrcObj, CNvObject *pDstObj, D3DBOX *boxSrc, DWORD dwDstX, DWORD dwDstY, DWORD dwDstZ, DWORD dwFlags);
DWORD nvTextureColorFill        (CNvObject *pDstObj, DWORD dwX0, DWORD dwY0, DWORD dwFillWidth, DWORD dwFillHeight, DWORD dwColor);
DWORD nvTextureStretchBlit      (LPSURFINFO pSrcInfo, LPSURFINFO pDstInfo);
DWORD nvTextureLock             (CTexture *pTexture, LPDDHAL_LOCKDATA pLockData);
DWORD nvTextureUnlock           (CTexture *pTexture, LPDDHAL_UNLOCKDATA pUnlockData);

DWORD D3DCreateTextureContexts  (DWORD dwHeapBase, DWORD dwNewLimit);
void  D3DDestroyTextureContexts (GLOBALDATA *pDriverData);

BOOL  nvTextureDefaultAlloc     (void);
void  nvTextureDefaultFree      (void);

#ifdef __cplusplus
}
#endif

#endif // _nvTex_h

