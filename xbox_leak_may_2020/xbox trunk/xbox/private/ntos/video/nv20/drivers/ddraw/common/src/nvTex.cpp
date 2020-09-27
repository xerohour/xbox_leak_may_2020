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
//
//  Module: nvTex.cpp
//   HW Specific Texture routines.
//
//  History:
//        Charles Inman (chasi)       01/31/98 - re-wrote and cleaned up.
//        Ben de Waal                 10/12/98 - overhaul
//
// **************************************************************************
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

//****************************************************************************
//****************************************************************************
//****************************************************************************
//**  CTexture  **************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************

//****************************************************************************
// invalidateTextureCache
//
// invalidates the texture cache by forcing at least a write to offset

__inline void invalidateTextureCache
(
    PNVD3DCONTEXT pContext   // may be NULL!!
)
{
    dbgTracePush ("invalidateTextureCache");

    if (pContext) {
#if (NVARCH >= 0x20)
        if (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_KELVIN | NVCLASS_FAMILY_CELSIUS)) {
            pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TEXTURE_STATE;
            pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TEXTURE_STATE;
#ifdef STATE_CULL
            // there's no explicit method to clear nv10's texture cache,
            // but re-sending the offset will do it implicitly
            pContext->hwState.celsius.valid [NV056_SET_TEXTURE_OFFSET(0) >> 2] = 0;
            pContext->hwState.celsius.valid [NV056_SET_TEXTURE_OFFSET(1) >> 2] = 0;
#endif
        }
        else
#endif // NVARCH >= 0x10
        {
            NV_FORCE_TRI_SETUP (pContext);
        }
    }

    dbgTracePop();
}

//****************************************************************************
// updateMipMapChain
//
// what does it do?

__forceinline void updateMipMapChain
(
    PNVD3DCONTEXT pContext,  // may be NULL!!
    CNvObject    *pNvObj
)
{
    dbgTracePush ("updateMipMapChain");

    while (pNvObj) {
        // get texture
        CTexture *pTexture = pNvObj->getTexture();

        // if this level has been renamed, copy data forward
        if (pTexture->isRenamedNotCopied())
        {
            DWORD dwSize = pTexture->getBPP() << (pTexture->getLogWidth() + pTexture->getLogHeight());
            DWORD dwSrc  = pTexture->getPreviousSwizzled()->getAddress();
            DWORD dwDst  = pTexture->getSwizzled()->getAddress();
                    #ifdef NV_PROFILE_COPIES
                    NVP_START(NVP_T_RENAME_FWD);
                    #endif
            nvMemCopy (dwDst,dwSrc,dwSize);
                    #ifdef NV_PROFILE_COPIES
                    NVP_STOP(NVP_T_RENAME_FWD);
                    nvpLogTime (NVP_T_RENAME_FWD,nvpTime[NVP_T_RENAME_FWD]);
                    #endif
            pTexture->tagRenamedAndCopied();
        }

        // handle this texture
        if (!pTexture->getSwizzled()->isUpToDate())
        {
            pTexture->updateSwizzleSurface (pContext);
        }

        // get the next one in the chain
        pNvObj = pNvObj->getAttachedA();
    }

    dbgTracePop();
}

//****************************************************************************
// updateTextureSurfaces
//
// update all out-of-date swizzled surfaces in a complex texture.
// traverses all mipmap levels and all faces of a cubemap.

__forceinline void updateSwizzledSurfaces
(
    PNVD3DCONTEXT pContext,  // may be NULL!!
    CTexture     *pTexture
)
{
    // get this object
    CNvObject *pNvObj = pTexture->getLinear()->getWrapper();

    if (!pTexture->getSwizzled()->isUpToDate() || pTexture->isSubSurfacesDirty())
    {
        if (pTexture->isCubeMap())
        {
            // cube map
            while (pNvObj)
            {
                updateMipMapChain (pContext, pNvObj);
                pNvObj = pNvObj->getAttachedB();
            }
        }
        else
        {
            // simple texture
            updateMipMapChain (pContext, pNvObj);
        }
        pTexture->tagSubSurfacesClean();
    }
}

//****************************************************************************
// CTexture::hwLock
//
// aquire hw access to swizzled surface
//  if not swizzled yet, do it now.
// NOTE: if the surface to be locked is a mipmap, or a subsurface of a cube map,
//  this routine will automatically lock the whole chain (logically one memory region)

void CTexture::hwLock
(
    PNVD3DCONTEXT pContext,  // may be NULL!!
    DWORD         dwAccess
)
{
    dbgTracePush ("CTexture::hwLock");

    // if this texture is not the base, call lock with a base
    //  yes - we can honestly say we use recursion in our driver now!
    nvAssert (getBaseTexture());
    if (!isBaseTexture()) {
        getBaseTexture()->hwLock (pContext,dwAccess);
        dbgTracePop();
        return;
    }

#ifdef TEX_MANAGE
    if (isManaged()) {
        // if the texture is in system memory, we have to bring it into video or AGP
        if (getSwizzled()->getHeapLocation() == CSimpleSurface::HEAP_SYS) {
            BOOL bSuccess = nvTexManageFetch (pContext, this);
            nvAssert (bSuccess);
        }
        else {
            // just make a note of the fact that we've touched this texture
            nvTexManageTouch (pContext, this);
        }
    }
#endif

    // update only if dirty
#if defined(NV_NULL_DRIVER) || defined(NV_NULL_TEXTURE_DRIVER)
    // NOP
#else
    updateSwizzledSurfaces (pContext, this);

    // lock surface explicitly
    m_surfSwizzled.hwLock (dwAccess);
#endif // NULL

    dbgTracePop();
}

//****************************************************************************
// CTexture::internalCanRename
//
// tell surface renamer if it is OK to rename a this texture now

BOOL CTexture::internalCanRename
(
    void
)
{
    //
    // cannot rename cube maps or individual mipmap levels (for now)
    //
    dbgTracePush ("CTexture::internalCanRename");
    BOOL b = (!isCubeMap() && !isUserMipLevel());
    dbgTracePop();
    return b;
}

// helper wrapper

BOOL __stdcall _CTexture_canRename
(
    void *pContext
)
{
    dbgTracePush ("_CTexture_canRename");
    BOOL b = ((CTexture*)pContext)->internalCanRename();
    dbgTracePop();
    return b;
}

//****************************************************************************
// CTexture::enableRenaming
//
// enable texture renaming

void CTexture::internalRename
(
    DWORD dwOldIndex,
    DWORD dwNewIndex
)
{
    dbgTracePush ("CTexture::internalRename");

    // make sure that the 'old' surface does not have any pending
    //  copies. (i.e. we only rename one level ahead without copying).
/*todo*/ //hwLock (NULL,);

    // since we can only rename on a BLT to the top-level, we have to
    // tell all the mipmaps that they should take care of themselves
    // if they do not get updated properly (i.e. copy from old surface)
    DWORD dwAddr = m_surfSwizzled.getSurface(dwNewIndex)->getAddress();
    DWORD dwBPP  = getBPP();
    DWORD dwAdd  = dwBPP << (getLogWidth() + getLogHeight());

    CNvObject *pNvObj = getWrapper();


    if (pNvObj)
    {
        // skip the parent level
        dwAddr  += dwAdd;
        dwAdd  >>= 2;
        if (dwAdd < dwBPP) dwAdd = dwBPP;
        pNvObj = pNvObj->getAttachedA();
    }

    while (pNvObj)
    {
        // get texture
        CTexture *pTexture = pNvObj->getTexture();
        pTexture->tagRenamedNotCopied();
        pTexture->m_surfSwizzled.forceRename(dwNewIndex,dwAddr);

        // update (allow DXT to work down to 1x1 (which is stored as 4x4))
        dwAddr  += dwAdd;
        dwAdd  >>= 2;
        if (dwAdd < dwBPP) dwAdd = dwBPP;

        // get the next one in the chain
        pNvObj = pNvObj->getAttachedA();
    }

    tagSubSurfacesDirty();
    dbgTracePop();
}

// helper wrapper

void __stdcall _CTexture_rename
(
    void         *pTexture,
    DWORD         dwOldIndex,
    DWORD         dwNewIndex
)
{
    dbgTracePush ("_CTexture_rename");
    if(dbgShowState & NVDBG_SHOW_TEXTURE_RENAME)
        DPF("CTexture Renaming Handle: %08x TexAddr: %08x INDEX: %08x->%08x\n",
            (((CTexture*)pTexture)->getWrapper())->getHandle(), pTexture,
            dwOldIndex,dwNewIndex);

    ((CTexture*)pTexture)->internalRename(dwOldIndex,dwNewIndex);
    dbgTracePop();
}

//****************************************************************************
// CTexture::enableRenaming
//
// enable texture renaming

void CTexture::enableRenaming
(
    void
)
{
    dbgTracePush ("CTexture::enableRenaming");
    //
    // enable renaming with special callbacks
    //
    m_surfSwizzled.tagRenameEnable (CComplexSurface<TOTAL_TEX_RENAME>::RENAME_EXTERNAL,
                                    1, // todo: same here
                                    _CTexture_canRename,
                                    _CTexture_rename,
                                    this);
    dbgTracePop();
}

//****************************************************************************
// CTexture::calcMipMapSize
//
// calculates the amount of bytes needed with a given number of mipmaps

DWORD CTexture::calcMipMapSize
(
    DWORD dwLogU,
    DWORD dwLogV,
    DWORD dwLogP,
    DWORD dwMipMapLevels
)
{
    dbgTracePush ("CTexture::calcMipMapSize");

    DWORD dwSize = 0;
    while (dwMipMapLevels)
    {
        dwSize += 1U << (dwLogU + dwLogV + dwLogP);
        if (dwLogU) dwLogU --;
        if (dwLogV) dwLogV --;
        if (dwLogP) dwLogP --;
        dwMipMapLevels --;
    }

    dbgTracePop();
    return dwSize;

/*
    neat code that works for mipmaps that got to Nx1 or 1xN, not 1x1 (if non-square)

    DWORD dwBits  = ((dwLogU + dwLogV) & 1) ? 0xaaaaaaaa : 0x55555555;
    DWORD dwTotal =  ((1 << (dwLogU + dwLogV + 1)) - 1) & dwBits;
    DWORD dwMask  = ~((1 << (dwLogU + dwLogV + 2 - dwMipMapLevels * 2)) - 1);
    return dwTotal & dwMask;
*/
}

//****************************************************************************
// CTexture::getCubeMapFaceNum
//
// get the face number from the face flag

DWORD CTexture::getCubeMapFaceNum
(
    DWORD dwFace              // cubemap face flag
)
{
    DWORD dwFaceNum;

    dbgTracePush ("CTexture::getCubeMapFaceNum");

    switch (dwFace) {
        case DDSCAPS2_CUBEMAP_POSITIVEX:
            dwFaceNum = 0;
            break;
        case DDSCAPS2_CUBEMAP_NEGATIVEX:
            dwFaceNum = 1;
            break;
        case DDSCAPS2_CUBEMAP_POSITIVEY:
            dwFaceNum = 2;
            break;
        case DDSCAPS2_CUBEMAP_NEGATIVEY:
            dwFaceNum = 3;
            break;
        case DDSCAPS2_CUBEMAP_POSITIVEZ:
            dwFaceNum = 4;
            break;
        case DDSCAPS2_CUBEMAP_NEGATIVEZ:
            dwFaceNum = 5;
            break;
    }

    dbgTracePop();
    return dwFaceNum;
}

//****************************************************************************
// CTexture::prepareLinearAsRenderTarget
//
// checks that the linear copy of the texture is in video memory. if it does
//  not exist, or if it is in a different heap, relocate it

BOOL CTexture::prepareLinearAsRenderTarget
(
    void
)
{
    dbgTracePush ("CTexture[%08x]::prepareLinearAsRenderTarget()",this);

    // sanity checks
    assert (this);

    // we are going to use the linear surface, make sure it's up to date
    if (!updateLinearSurface())
    {
        dbgTracePop();
        return FALSE;
    }

    // is it in the right place? (vidmem)
    if (!(getLinear()->getHeapLocation() == CSimpleSurface::HEAP_VID))
    {
        // nope, allocate a version in video memory
        CSimpleSurface surfTemp;
        if (!surfTemp.create (getLinear()->getWrapper(), getLinear()->getFormat(),
                              getLinear()->getWidth(), getLinear()->getHeight(), getLinear()->getDepth(),
                              getLinear()->getMultiSampleBits(), getLinear()->getBPPRequested(), getLinear()->getBPP(),
                              getLinear()->getPitch(), CSimpleSurface::HEAP_VID,
                              CSimpleSurface::HEAP_VID, CSimpleSurface::ALLOCATE_TILED
#ifdef CAPTURE
                             ,CAPTURE_SURFACE_KIND_TEXTURE
#endif
                              ))
        {
            dbgTracePop();
            return FALSE;
        }

        // copy contents from original surface to the new one
        nvMemCopy (surfTemp.getAddress(),
                   getLinear()->getAddress(),
                   getLinear()->getSize());

        // swap surfaces
        //  neat trick to make the temp surface own the old non-vidmem surface
        getLinear()->swap (&surfTemp);

        // destroy old surface
        surfTemp.destroy();
    }

    // tag swizzled out-of-date, so that it gets reswizzled from the linear
    //  surface when it is used as a texture
    getSwizzled()->tagOutOfDate();

    // if this is part of a cube map/mipmap, tag the base as dirty
    //  this will trigger a proper update of all subsurfaces before HW touches them
    if (isUserMipLevel() || isCubeMap())
    {
        getBaseTexture()->tagSubSurfacesDirty();
    }

    // tag linear as up-to-date
    getLinear()->tagUpToDate();

    // done
    dbgTracePop();
    return TRUE;
}

//****************************************************************************
// CTexture::updateLinearSurface
//

BOOL CTexture::updateLinearSurface
(
    void
)
{
    dbgTracePush ("CTexture[%08x]::updateLinearSurface()",this);

    // texture valid
    assert (this);

    // cache active swizzled surface
    CSimpleSurface *pActive = getSwizzled();

    // create a linear surface if it does not exist already
    //  note that the linear surface for render targets get allocated in
    //   CTexture::prepareLinearAsRenderTarget(), and not here
    if (m_surfLinear.isValid())
    {
        // we have a linear surface, is it already up to date?
        if (m_surfLinear.isUpToDate())
        {
            // yes, we have no work!
            dbgTracePop();
            return TRUE;
        }
    }
    else
    {
        // allocate surface
        DWORD dwAllowedHeaps  = pDriverData->GARTLinearBase ? (CSimpleSurface::HEAP_VID | CSimpleSurface::HEAP_AGP | CSimpleSurface::HEAP_SYS)
                                                            : (CSimpleSurface::HEAP_VID | CSimpleSurface::HEAP_PCI | CSimpleSurface::HEAP_SYS);

        DWORD dwPreferredHeap = CSimpleSurface::HEAP_SYS;

        if (!((CSimpleSurface*)&m_surfLinear)->create (getPitch(), getHeight(), getDepth(), getBPP(),
                                                       dwAllowedHeaps, dwPreferredHeap,
                                                       CSimpleSurface::ALLOCATE_SIMPLE
#ifdef CAPTURE
                                                      ,CAPTURE_SURFACE_KIND_TARGET
#endif
                                                       ))
        {
            // no memory - ouch
            DPF ("CTexture::updateLinearSurface: cannot allocate linear surface");
            dbgD3DError();
            dbgTracePop();
            return FALSE;
        }
    }

    // Is the swizzle surface valid?
    //  if so then we deswizzle here. This is skipped usually when the app wants to
    //  lock the texture and hasn't touched a pixel yet
    if (pActive->isValid() && pActive->isUpToDate())
    {
        // deswizzle
                #ifdef NV_PROFILE_CPULOCK
                NVP_START (NVP_T_TEX_ULS);
                #endif
        DDLOCKINDEX(NVSTAT_LOCK_UPDATE_LINEAR);
        m_surfLinear.cpuLock (CSimpleSurface::LOCK_NORMAL);
        pActive->cpuLock (CSimpleSurface::LOCK_NORMAL);
                #ifdef NV_PROFILE_CPULOCK
                NVP_STOP (NVP_T_TEX_ULS);
                nvpLogTime (NVP_T_TEX_ULS,nvpTime[NVP_T_TEX_ULS]);
                #endif
        if (isDXT())
        {
            nvSwizBlt_cpu_lin2lin (pActive->getAddress(),getPitch(),0,0,
                                   m_surfLinear.getAddress(),getPitch(),0,0,
                                   getWidth(),getHeight(),getBPP());
        }
        else
        {
            nvSwizBlt_cpu_swz2lin (pActive->getAddress(),0,0,getLogWidth(),getLogHeight(),
                                   m_surfLinear.getAddress(),getPitch(),0,0,
                                   getWidth(),getHeight(),getBPP());
        }
        pActive->cpuUnlock();
        m_surfLinear.cpuUnlock();
    }

    // update flags & reset dirty rectangle
    m_surfLinear.tagUpToDate();

    // done
    dbgTracePop();
    return TRUE;
}

//****************************************************************************
// CTexture::updateSwizzleSurface
//

BOOL CTexture::updateSwizzleSurface
(
    PNVD3DCONTEXT pContext  // may be NULL!
)
{
    dbgTracePush ("CTexture[%08x]::updateSwizzleSurface()",this);

    // texture valid
    assert (this);

    // get active swizzled surface
    CSimpleSurface *pActive = getSwizzled();

    // create swizzled surface if it does not exist already
    // note that the linear surface for render targets get allocated in
    // CTexture::prepareLinearAsRenderTarget(), and not here

    if (!pActive->isValid()) {
        // allocate surface
        DWORD dwAllowedHeaps  = pDriverData->GARTLinearBase ? (CSimpleSurface::HEAP_VID | CSimpleSurface::HEAP_AGP)
                                                            : (CSimpleSurface::HEAP_VID | CSimpleSurface::HEAP_PCI);

        DWORD dwPreferredHeap = (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_TEXTURE_PREFER_AGP) ? ( CSimpleSurface::HEAP_AGP | CSimpleSurface::HEAP_PCI)
                                                                                                           : CSimpleSurface::HEAP_VID;

        if (!pActive->create (getPitch(), getHeight(), getDepth(), getBPP(),
                              dwAllowedHeaps, dwPreferredHeap,
                              CSimpleSurface::ALLOCATE_SIMPLE
#ifdef CAPTURE
                             ,CAPTURE_SURFACE_KIND_TEXTURE
#endif
                              )) {
            // no memory - ouch
            DPF ("CTexture::updateSwizzleSurface: cannot allocate swizzled surface");
            dbgD3DError();
            dbgTracePop();
            return FALSE;
        }
    }
    else {
        // we have a swizzled surface, is it already up to date?
        if (pActive->isUpToDate()) {
            // yes, we have no work!
            dbgTracePop();
            return TRUE;
        }
    }

    // if the source is up to date, we can now swizzle it
    //  if not, texture corruption will occur because the app failed to init the texture

    if (m_surfLinear.isValid() && m_surfLinear.isUpToDate()) {
        // swizzle
        BOOL bUseHW = (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_TEXTURE_USEHW)
                   && m_surfLinear.hwCanRead()
                   && pActive->hwCanWrite();

        if (bUseHW)
        {
            m_surfLinear.hwLock(CSimpleSurface::LOCK_NORMAL);
            pActive->hwLock(CSimpleSurface::LOCK_NORMAL);
            nvSwizBlt_gpu_lin2swz (m_surfLinear.getOffset(), m_surfLinear.getHeapLocation(),
                                   getPitch(), 0, 0,
                                   pActive->getOffset(), 0, 0,
                                   getLogWidth(), getLogHeight(),
                                   getWidth(), getHeight(),
                                   getBPP());
            pActive->hwUnlock();
            m_surfLinear.hwUnlock();
        }
        else
        {
                    #ifdef NV_PROFILE_CPULOCK
                    NVP_START (NVP_T_TEX_USS);
                    #endif
            DDLOCKINDEX(NVSTAT_LOCK_UPDATE_OPTIMIZED);
            m_surfLinear.cpuLock (CSimpleSurface::LOCK_NORMAL);
            pActive->cpuLock (CSimpleSurface::LOCK_RENAME); // allow renaming
                    #ifdef NV_PROFILE_CPULOCK
                    NVP_STOP (NVP_T_TEX_USS);
                    nvpLogTime (NVP_T_TEX_USS,nvpTime[NVP_T_TEX_USS]);
                    #endif
            if (isDXT())
            {
                nvSwizBlt_cpu_lin2lin (m_surfLinear.getAddress(),getPitch(),0,0,
                                       pActive->getAddress(),getPitch(),0,0,
                                       getWidth(),getHeight(),getBPP());
            }
            else
            {
                nvSwizBlt_cpu_lin2swz (m_surfLinear.getAddress(),getPitch(),0,0,
                                       pActive->getAddress(),0,0,getLogWidth(),getLogHeight(),
                                       getWidth(),getHeight(),getBPP());
            }

            // capture
#ifdef CAPTURE
            if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
                CAPTURE_MEMORY_WRITE memwr;
                memwr.dwJmpCommand         = CAPTURE_JMP_COMMAND;
                memwr.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
                memwr.dwExtensionID        = CAPTURE_XID_MEMORY_WRITE;
                memwr.dwCtxDMAHandle       = (pActive->getHeapLocation() == CSimpleSurface::HEAP_VID)
                                           ? NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY
                                           : D3D_CONTEXT_DMA_HOST_MEMORY;
                memwr.dwOffset             = pActive->getOffset();
                memwr.dwSize               = getWidth() * getHeight() * getBPP();
                captureLog (&memwr,sizeof(memwr));
                captureLog ((void*)pActive->getAddress(),memwr.dwSize);
            }
#endif

            //
            // BUGBUG this should like below (test with powerslide)
            //
            // problem is that hwLock is called in inner loops after it read the dvb values. if
            //  the lock modifies them, it does not detect that case.
            //
            // can help perf if fixed.
            //
/*            nvSwizBlt_lin2swz (m_surfLinear.getAddress(),getPitch(),0,0,
                                    this,pActive->getAddress(),0,0,getLogWidth(),getLogHeight(),
                                    getWidth(),getHeight(),getBPP());*/
            pActive->cpuUnlock();
            m_surfLinear.cpuUnlock();
            invalidateTextureCache (pContext);
        }
    }

    // update state
    pActive->tagUpToDate();

    // done
    dbgTracePop();
    return TRUE;
}

//****************************************************************************

#if (NVARCH >= 0x010)

// pre-calculate HW-specific control words

void CTexture::calculateNV056ControlWords
(
    void
)
{
    m_dwNV056Control0  = DRF_DEF(056, _SET_TEXTURE_CONTROL0, _ENABLE, _TRUE);
    // these are backward from openGL. d3d can only set the maximum mipmap level, by which they mean
    // the openGL minimum level (d3d maximum = largest sized level to use, not largest level number).
    // the d3d minimum level (the openGL / celsius maximum) is always just the end of the chain
    m_dwNV056Control0 |= DRF_NUM(056, _SET_TEXTURE_CONTROL0, _MAX_LOD_CLAMP, ((m_dwMipMapCount-1) << 8));  // 4.8 fixed point
    // Let's assume these defaults are cool for D3D. Alpha kill works
    // when color key is set to kill (I think).
    m_dwNV056Control0 |= DRF_DEF(056, _SET_TEXTURE_CONTROL0, _ALPHA_KILL_ENABLE, _FALSE);

    m_dwNV056Format  = DRF_NUM(056, _SET_TEXTURE_FORMAT, _CONTEXT_DMA, nv056TextureContextDma[getSwizzled()->getContextDMA()]);
    m_dwNV056Format |= DRF_NUM(056, _SET_TEXTURE_FORMAT, _COLOR, nv056TextureFormat[m_surfLinear.getFormat()]);

    // texel alignment
    m_dwNV056Format |= ((getDC()->nvD3DRegistryData.regTexelAlignment & D3D_REG_TEXELALIGNMENT_ZOH_MASK) == D3D_REG_TEXELALIGNMENT_ZOH_CENTER)
                    ? DRF_DEF(056, _SET_TEXTURE_FORMAT, _ORIGIN_ZOH, _CENTER)
                    : DRF_DEF(056, _SET_TEXTURE_FORMAT, _ORIGIN_ZOH, _CORNER);
    m_dwNV056Format |= ((getDC()->nvD3DRegistryData.regTexelAlignment & D3D_REG_TEXELALIGNMENT_FOH_MASK) == D3D_REG_TEXELALIGNMENT_FOH_CENTER)
                    ? DRF_DEF(056, _SET_TEXTURE_FORMAT, _ORIGIN_FOH, _CENTER)
                    : DRF_DEF(056, _SET_TEXTURE_FORMAT, _ORIGIN_FOH, _CORNER);

    // levels should already be clamped to 1-15
    nvAssert ((m_dwMipMapCount > 0) && (m_dwMipMapCount < 16));
    m_dwNV056Format |= DRF_NUM(056, _SET_TEXTURE_FORMAT, _MIPMAP_LEVELS, m_dwMipMapCount);
    m_dwNV056Format |= DRF_NUM(056, _SET_TEXTURE_FORMAT, _BASE_SIZE_U, getLogicalLogWidth());
    m_dwNV056Format |= DRF_NUM(056, _SET_TEXTURE_FORMAT, _BASE_SIZE_V, getLogicalLogHeight());
}

#endif  // NVARCH >= 0x010

//****************************************************************************
// CTexture::create
//
// create a base level texture

BOOL CTexture::create
(
    CNvObject *pWrapperObject,   // NvObject wrapping this texture
    DWORD      dwWidth,          // width
    DWORD      dwHeight,         // height
    DWORD      dwDepth,          // depth
    DWORD      dwBPPRequested,   // bytes per pixel requested
    DWORD      dwBPPGranted,     // bytes per pixel granted
    DWORD      dwMipMapCount,    // # of mipmaps (including self)
    DWORD      dwFormat,         // texture format NV_TEXTURE_FORMAT_xxx
    DWORD      dwAllowedHeaps,   // allowed heaps to allocate in
    DWORD      dwPreferredHeap   // preferred heap
)
{
    dbgTracePush ("CTexture[%08x]::create(dwWidth=%d,dwHeight=%d,dwBPP=%d,dwMipMapCount=%d,dwFormat=%x,dwAllowedHeaps=%x,dwPreferredHeap=%x)",
                  this,dwWidth,dwHeight,dwBPPGranted,dwMipMapCount,dwFormat,dwAllowedHeaps,dwPreferredHeap);

    // valid?
    assert (this);

    DWORD dwLogWidth  = dwWidth;
    DWORD dwLogHeight = dwHeight;
    DWORD dwLogDepth  = dwDepth;
    ASM_LOG2 (dwLogWidth);
    ASM_LOG2 (dwLogHeight);
    ASM_LOG2 (dwLogDepth);

    // determine size of texture
    //  - include mipmaps
    DWORD dwSize = calcMipMapSize(dwLogWidth, dwLogHeight, dwLogDepth, dwMipMapCount) * dwBPPGranted;

    // cubemap
    if (isCubeMap())
    {
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
            m_dwCubeMapSizeFacePadded = (dwSize + NV_KELVIN_CUBEMAP_FACE_ALIGN) & ~NV_KELVIN_CUBEMAP_FACE_ALIGN;
        } else {
            m_dwCubeMapSizeFacePadded = (isDXT() || isPalettized()) ?
                                        (dwSize + NV_CELSIUS_CUBEMAP_FACE_ALIGN_COMPRESSED) & ~NV_CELSIUS_CUBEMAP_FACE_ALIGN_COMPRESSED :
                                        (dwSize + NV_CELSIUS_CUBEMAP_FACE_ALIGN) & ~NV_CELSIUS_CUBEMAP_FACE_ALIGN;
        }
        dwSize = 6 * m_dwCubeMapSizeFacePadded;
    }

    // if we prefer AGP memory over VID memory
    if (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_TEXTURE_PREFER_AGP)
    {
        // if we are allowed agp and video memory to allocate in
        if ((dwPreferredHeap & (CSimpleSurface::HEAP_VID | CSimpleSurface::HEAP_AGP)) == (CSimpleSurface::HEAP_VID | CSimpleSurface::HEAP_AGP))
        {
            // if we want video
            if (dwPreferredHeap == CSimpleSurface::HEAP_VID)
            {
                // override to agp
                dwPreferredHeap = CSimpleSurface::HEAP_AGP;
            }
        }
    }

    // allocate surface
    if (!m_surfSwizzled.create (pWrapperObject, dwSize, dwAllowedHeaps, dwPreferredHeap
#ifdef CAPTURE
                               ,CAPTURE_SURFACE_KIND_TEXTURE
#endif
                                ))
    {
        DPF ("CTexture::create: failed to alloc surface");
        dbgTracePop();
        return FALSE;
    }

    m_surfLinear.setWrapper      (pWrapperObject);
    m_surfLinear.setWidth        (dwWidth);
    m_surfLinear.setHeight       (dwHeight);
    m_surfLinear.setDepth        (dwDepth);
    m_surfLinear.setBPP          (dwBPPGranted);
    m_surfLinear.setBPPRequested (dwBPPRequested);
    m_surfLinear.setFormat       (dwFormat);
    m_surfLinear.setPitch        (dwBPPGranted << dwLogWidth);
    m_surfLinear.setSlicePitch   (dwBPPGranted << (dwLogWidth+dwLogDepth));

    m_dwLogWidth    = dwLogWidth;
    m_dwLogHeight   = dwLogHeight;
    m_dwLogDepth    = dwLogDepth;
    m_dwMipMapCount = dwMipMapCount;

    // pre-calculate HW-specific control words
#if (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        calculateNV056ControlWords();
    }
#endif  // NVARCH >= 0x010

    dbgTracePop();
    return TRUE;
}

//****************************************************************************
// CTexture::create
//
// create a mipmap level texture

BOOL CTexture::create
(
    CNvObject *pWrapperObject,   // NvObject wrapping this texture
    DWORD      dwThisFace,
    CTexture  *pBaseTexture,   // create a mipmap from this base
    DWORD      dwMipMapLevel   // this is the nth mipmap (base = 0)
)
{
    dbgTracePush ("CTexture[%08x]::create(dwThisFace=%d,pBaseTexture=%08x,dwMipMapLevel=%d)",
                  this,dwThisFace,pBaseTexture,dwMipMapLevel);

    // valid?
    assert (this);

    // prepare information for this mipmap level
    DWORD dwBaseLogWidth  = pBaseTexture->getLogWidth();
    DWORD dwBaseLogHeight = pBaseTexture->getLogHeight();
    DWORD dwBaseLogDepth  = pBaseTexture->getLogDepth();
    DWORD dwLogWidth      = DWORD(max(0,int(dwBaseLogWidth  - dwMipMapLevel)));
    DWORD dwLogHeight     = DWORD(max(0,int(dwBaseLogHeight - dwMipMapLevel)));
    DWORD dwLogDepth      = DWORD(max(0,int(dwBaseLogDepth  - dwMipMapLevel)));
    DWORD dwWidth         = 1 << dwLogWidth;
    DWORD dwHeight        = 1 << dwLogHeight;
    DWORD dwDepth         = 1 << dwLogDepth;
    DWORD dwBPP           = pBaseTexture->getBPP();
    DWORD dwBPPRequested  = pBaseTexture->getBPPRequested();
    DWORD dwFormat        = pBaseTexture->getFormat();
    DWORD dwMipMapCount   = pBaseTexture->getMipMapCount() - dwMipMapLevel;
    DWORD dwPitch         = dwBPP << dwLogWidth;
    DWORD dwSize          = calcMipMapSize(dwLogWidth,dwLogHeight,dwLogDepth,dwMipMapCount) * dwBPP;
    DWORD dwAddress       = pBaseTexture->getSwizzled()->getAddress()
                          + pBaseTexture->getSwizzled()->getSize()
                          - dwSize;
    DWORD dwHeapLocation  = pBaseTexture->getSwizzled()->getHeapLocation();

    // cubemap
    if (isCubeMap())
    {
        // copy cube map info from base
        m_dwCubeMapSizeFacePadded = pBaseTexture->m_dwCubeMapSizeFacePadded;

        // get offset to this face
        dwAddress = pBaseTexture->getSwizzled()->getAddress()
                  + getCubeMapFaceNum(dwThisFace) * m_dwCubeMapSizeFacePadded
                  + calcMipMapSize(dwBaseLogWidth,dwBaseLogHeight,dwBaseLogDepth,dwMipMapLevel) * dwBPP;
    }

    // point texture to proper place off of base texture
    if (!m_surfSwizzled.own (dwAddress, dwPitch, dwLogHeight, dwHeapLocation, FALSE))
    {
        DPF ("CTexture::create: failed to claim surface");
        dbgTracePop();
        return FALSE;
    }

    // assign variables
    m_surfLinear.setWrapper      (pWrapperObject);
    m_surfLinear.setWidth        (dwWidth);
    m_surfLinear.setHeight       (dwHeight);
    m_surfLinear.setDepth        (dwDepth);
    m_surfLinear.setBPP          (dwBPP);
    m_surfLinear.setBPPRequested (dwBPPRequested);
    m_surfLinear.setFormat       (dwFormat);
    m_surfLinear.setPitch        (dwBPP << dwLogWidth);
    m_surfLinear.setSlicePitch   (dwBPP << (dwLogWidth+dwLogDepth));

    m_dwLogWidth    = dwLogWidth;
    m_dwLogHeight   = dwLogHeight;
    m_dwLogDepth    = dwLogDepth;
    m_dwMipMapCount = dwMipMapCount;
    m_pBaseTexture  = pBaseTexture;

    // update state
    tagUserMipLevel();

    // pre-calculate HW-specific control words
#if (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        calculateNV056ControlWords();
    }
#endif  // NVARCH >= 0x010

    dbgTracePop();
    return TRUE;
}

#ifdef TEX_MANAGE

//****************************************************************************

// CTexture::fetch
//
// bring a texture from system memory back into HW-readable memory (ie. video or AGP)
// returns TRUE on success, FALSE on failure

BOOL CTexture::fetch (void)
{
    CComplexSurface<TOTAL_TEX_RENAME> newSurfSwizzled;

    dbgTracePush ("CTexture::fetch");

    // this should be the base texture
    nvAssert (isBaseTexture());

    if (getSwizzled()->getHeapLocation() != CSimpleSurface::HEAP_SYS) {
        // texture is already in video or AGP memory. this shouldn't really have been called
        DPF ("attempt to fetch an unevicted texture");
        dbgD3DError();
        dbgTracePop();
        return (TRUE);
    }

    // determine the size of the texture
    DWORD dwSize;
    if (isCubeMap()) {
        dwSize = 6 * m_dwCubeMapSizeFacePadded;
    }
    else {
        dwSize = getBPP() * calcMipMapSize (m_dwLogWidth, m_dwLogHeight, m_dwLogDepth, m_dwMipMapCount);
    }

    // alternately, i think this should work...
    DWORD dwSize2 = m_surfSwizzled.getActiveSurface()->getPitch() * m_surfSwizzled.getActiveSurface()->getHeight();
    nvAssert (dwSize == dwSize2);

    // allocate a new surface in system memory
    DWORD dwPreferredHeap = CSimpleSurface::HEAP_VID;
    DWORD dwAllowedHeaps  = CSimpleSurface::HEAP_VID |
                            ((pDriverData->GARTLinearBase) ?
                                CSimpleSurface::HEAP_AGP :
                                CSimpleSurface::HEAP_PCI);

#ifdef TEX_MANAGE_TEST
    dwAllowedHeaps = dwPreferredHeap = CSimpleSurface::HEAP_VID;
#endif

    if (!newSurfSwizzled.create (m_surfLinear.getWrapper(), dwSize, dwAllowedHeaps, dwPreferredHeap
#ifdef CAPTURE
                                ,CAPTURE_SURFACE_KIND_TEXTURE
#endif
                                ))
    {
        dbgTracePop();
        return (FALSE);
    }

    // copy texture data to the new surface
    nvMemCopy (newSurfSwizzled.getActiveSurface()->getAddress(), m_surfSwizzled.getActiveSurface()->getAddress(), dwSize);

    // swap the new into the old, and destroy the new, which is now the old  :)
    m_surfSwizzled.swap (&newSurfSwizzled);
    newSurfSwizzled.destroy();

    // re-fresh HW control words (heap may have changed)
#if (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        calculateNV056ControlWords();
    }
#endif  // NVARCH >= 0x010

    // run down the attached textures and reset their pointers into the new surface
    CNvObject *pNvObjA, *pNvObjB;
    DWORD     dwFace, dwMMLevel;

    dwFace     = DDSCAPS2_CUBEMAP_POSITIVEX;
    dwMMLevel  = 0;

    // this surface's mipmaps
    pNvObjA = m_surfLinear.getWrapper()->getAttachedA();
    while (pNvObjA) {
        dwMMLevel++;
        pNvObjA->getTexture()->create (pNvObjA, dwFace, this, dwMMLevel);
        pNvObjA = pNvObjA->getAttachedA();
    }

    // run down the cubemap faces (if any)
    pNvObjB = m_surfLinear.getWrapper()->getAttachedB();
    while (pNvObjB) {
        dwFace <<= 1;
        dwMMLevel = 0;
        pNvObjB->getTexture()->create (pNvObjB, dwFace, this, dwMMLevel);
        pNvObjA = pNvObjB->getAttachedA();
        while (pNvObjA) {
            dwMMLevel++;
            pNvObjA->getTexture()->create (pNvObjA, dwFace, this, dwMMLevel);
            pNvObjA = pNvObjA->getAttachedA();
        }
        pNvObjB = pNvObjB->getAttachedB();
    }

    dbgTracePop();
    return (TRUE);
}

//****************************************************************************

// CTexture::evict
//
// evict a texture from video or AGP memory into system memory
// returns TRUE on success, FALSE on failure

BOOL CTexture::evict (void)
{
    CComplexSurface<TOTAL_TEX_RENAME> newSurfSwizzled;

    dbgTracePush ("CTexture::evict");

    // make sure this is a base texture
    nvAssert (isBaseTexture());

    if (isEvicted()) {
        // texture is already evicted. this shouldn't really have been called
        DPF ("ack. attempt to evict texture that's already been evicted");
        dbgD3DError();
        dbgTracePop();
        return (TRUE);
    }

    // if the texture is in use, it can't be evicted
    if (getSwizzled()->isBusy()) {
        dbgTracePop();
        return (FALSE);
    }

    // determine the size of the texture
    DWORD dwSize;
    if (isCubeMap()) {
        dwSize = 6 * m_dwCubeMapSizeFacePadded;
    }
    else {
        dwSize = getBPP() * calcMipMapSize (m_dwLogWidth, m_dwLogHeight, m_dwLogDepth, m_dwMipMapCount);
    }

    // alternately, i think this should work...
    DWORD dwSize2 = m_surfSwizzled.getActiveSurface()->getPitch() * m_surfSwizzled.getActiveSurface()->getHeight();
    nvAssert (dwSize == dwSize2);

    // allocate a new surface in system memory
    if (!newSurfSwizzled.create (m_surfLinear.getWrapper(), dwSize, CSimpleSurface::HEAP_SYS, CSimpleSurface::HEAP_SYS
#ifdef CAPTURE
                                ,CAPTURE_SURFACE_KIND_TEXTURE
#endif
                                 ))
    {
        DPF ("CTexture::evict: failed to alloc system memory surface");
        dbgD3DError();
        dbgTracePop();
        return (FALSE);
    }

    // copy texture data to the new surface
    nvMemCopy (newSurfSwizzled.getActiveSurface()->getAddress(), m_surfSwizzled.getActiveSurface()->getAddress(), dwSize);

    // swap the new into the old, and destroy the new, which is now the old  :)
    m_surfSwizzled.swap (&newSurfSwizzled);
    newSurfSwizzled.destroy();

    // re-fresh HW control words (heap may have changed)
#if (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        calculateNV056ControlWords();
    }
#endif  // NVARCH >= 0x010

    // run down the attached textures and reset their pointers into the new surface
    CNvObject *pNvObjA, *pNvObjB;
    DWORD     dwFace, dwMMLevel;

    dwFace     = DDSCAPS2_CUBEMAP_POSITIVEX;
    dwMMLevel  = 0;

    // this surface's mipmaps
    pNvObjA = m_surfLinear.getWrapper()->getAttachedA();
    while (pNvObjA) {
        dwMMLevel++;
        pNvObjA->getTexture()->create (pNvObjA, dwFace, this, dwMMLevel);
        pNvObjA = pNvObjA->getAttachedA();
    }

    // run down the cubemap faces (if any)
    pNvObjB = m_surfLinear.getWrapper()->getAttachedB();
    while (pNvObjB) {
        dwFace <<= 1;
        dwMMLevel = 0;
        pNvObjB->getTexture()->create (pNvObjB, dwFace, this, dwMMLevel);
        pNvObjA = pNvObjB->getAttachedA();
        while (pNvObjA) {
            dwMMLevel++;
            pNvObjA->getTexture()->create (pNvObjA, dwFace, this, dwMMLevel);
            pNvObjA = pNvObjA->getAttachedA();
        }
        pNvObjB = pNvObjB->getAttachedB();
    }

    dbgTracePop();
    return (TRUE);
}

#endif  // TEX_MANAGE

//****************************************************************************
// CTexture::destroy
//

BOOL CTexture::destroy
(
    void
)
{
    dbgTracePush ("CTexture[%08x]::destroy()",this);

    // valid?
    assert (this);

    // destroy surface(s)
    if (m_surfLinear.isValid())
    {
        m_surfLinear.destroy();
    }
    m_surfSwizzled.destroy();

    dbgTracePop();
    return TRUE;
}

//****************************************************************************
// CTexture::CTexture
//

CTexture::CTexture
(
    void
)
{
    dbgTracePush ("CTexture[%08x]::CTexture()",this);

    // alloc succeeded?
    assert (this);

    // init
    m_surfLinear.setWidth (0);
    m_surfLinear.setHeight (0);
    m_surfLinear.setDepth (1);
    m_surfLinear.setBPP (0);
    m_surfLinear.setBPPRequested (0);
    m_surfLinear.setFormat (0);

    m_dwFlags         = 0;
    m_pBaseTexture    = this;
    m_dwMipMapCount   = 0;
    m_dwPriority      = 0;

    m_dwNV056Control0 = 0;
    m_dwNV056Format   = 0;

#ifdef TEX_MANAGE
    m_pPrev           = NULL;
    m_pNext           = NULL;
    m_pContext        = NULL;
#endif

    dbgTracePop();
}

//****************************************************************************
// CTexture::~CTexture
//

CTexture::~CTexture
(
    void
)
{
    dbgTracePush ("CTexture[%08x]::~CTexture()",this);

    // valid?
    assert (this);

    // automatically destroy this texture
    destroy();

    // zero out (debug builds)
#ifdef DEBUG
    m_surfLinear.setWidth (0);
    m_surfLinear.setHeight (0);
    m_surfLinear.setDepth (0);
    m_surfLinear.setBPP (0);
    m_surfLinear.setBPPRequested (0);
    m_surfLinear.setFormat (0);
    m_surfLinear.setWrapper (NULL);

    m_dwFlags         = 0;
    m_pBaseTexture    = NULL;
    m_dwMipMapCount   = 0;
    m_dwPriority      = 0;

    m_dwNV056Control0 = 0;
    m_dwNV056Format   = 0;

#ifdef TEX_MANAGE
    m_pPrev           = (CTexture *) 0xCCCCCCCC;
    m_pNext           = (CTexture *) 0xCCCCCCCC;
    m_pContext        = (PNVD3DCONTEXT) 0xCCCCCCCC;
#endif

    m_dwCubeMapSizeFacePadded    = 0;
    // m_pCubeMap        = NULL;
#endif

    dbgTracePop();
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//**  exports  ***************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************

// nvTextureCreateSurface
//
// creates a texture surface with the passed in specicifations
//
// called from only one place in CreateSurface32()

DWORD nvTextureCreateSurface
(
    LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl,
    LPDDRAWI_DIRECTDRAW_GBL   pDDGbl,
    LPDDSURFACEDESC           pDDSurfDesc
)
{
    dbgTracePush ("nvTextureCreateSurface");

    assert (pDDSLcl);
    assert (pDDGbl);

    LPDDRAWI_DDRAWSURFACE_GBL pGbl = pDDSLcl->lpGbl;
    assert (pGbl);

    // init
    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_D3D) {
        if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE) {
            nvD3DRecover();
        }
        else {
            dbgTracePop();
            return (DD_OK);
        }
    }

    // calculate surface parameters
    DWORD dwFormat, dwWidth, dwHeight, dwDepth, dwBPPRequested, dwBPPGranted, dwPitch, dwSlicePitch, dwMultiSampleBits;
    BOOL  bFourCC;
    if (nvCalculateSurfaceParams (pDDSLcl, &dwFormat, &bFourCC, &dwWidth, &dwHeight, &dwDepth, &dwMultiSampleBits,
        &dwBPPRequested, &dwBPPGranted, /* pitch is ignored */&dwPitch, &dwSlicePitch, TRUE) != DD_OK) {
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvCreateTextureSurface - Invalid pixel format!");
        //dbgD3DError();  -- not an err: CanCreateSurface32() doesn't do checks, so we need them here
        dbgTracePop();
        return (DDERR_INVALIDPARAMS);
    }

    // dimensions: dwLogWidth, dwLogHeight
    if (!dwWidth                     // zero?
     || !dwHeight                    // zero?
     || !dwDepth
     || (dwWidth  & (dwWidth - 1))   // power of 2?
     || (dwHeight & (dwHeight - 1))
     || (dwDepth & (dwDepth - 1))) // power of 2?
    {
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvCreateTextureSurface - Invalid texture size!");
        dbgTracePop();
        return (DDERR_INVALIDPARAMS);
    }

    // get log2 of width & height
    DWORD dwLogWidth  = dwWidth;
    DWORD dwLogHeight = dwHeight;
    DWORD dwLogDepth = dwDepth;
    ASM_LOG2(dwLogWidth);
    ASM_LOG2(dwLogHeight);
    ASM_LOG2(dwLogDepth);

    DWORD dwLogLimit =
        (pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_VOLUME) ?
        pDriverData->nvD3DPerfData.dwMaxVolumeTextureSize :
        pDriverData->nvD3DPerfData.dwMaxTextureSize;

    ASM_LOG2(dwLogLimit);

    if ((dwLogWidth  > dwLogLimit) ||
        (dwLogHeight > dwLogLimit) ||
        ((pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_VOLUME) && (dwLogDepth > dwLogDepth)))
    {
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvCreateTextureSurface - Invalid texture size!");
        dbgD3DError();
        dbgTracePop();
        return (DDERR_INVALIDPARAMS);
    }

    // dwMipMapCount
    CTexture *pBaseTexture       = NULL; // if defined, current texture is a user mip level
    CTexture *pCubeMapParentFace = NULL; // if defined, current cube map +x face
    DWORD     dwMipMapCount      = 1;
    DWORD     dwMipMapLevel      = 0;

    // handle cube maps
    if (pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_CUBEMAP)
    {
        // the following check is necessary to get around Microsoft's cubemap topology bug
        // for the +X face mipmaps we will actually be counting the -Z face and its mipmaps
        // this check makes sure that we don't count the -Z parent face. Since all cubemap
        // faces must have the same number of miplevels, this will work in all cases
        LPDDRAWI_DDRAWSURFACE_LCL pMipLcl = pDDSLcl;
        if ( (pMipLcl->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_CUBEMAP_POSITIVEX)
         && !(pMipLcl->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_MIPMAPSUBLEVEL))
        {
            pMipLcl = pDDSLcl->lpAttachList->lpAttached;
        }
        // count the number of mip map levels
        for (; pMipLcl->lpAttachList; pMipLcl = pMipLcl->lpAttachList->lpAttached) {
            dwMipMapCount++;
        }
        // Check if this is the base level of the first face or some subordinate level.
        if (pDDSLcl->lpAttachListFrom)
        {
            // count the number of levels back to the parent of this face
            for (pMipLcl = pDDSLcl; pMipLcl->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_MIPMAPSUBLEVEL; pMipLcl = pMipLcl->lpAttachListFrom->lpAttached) {
                dwMipMapLevel++;
            }
            // continue back to the parent +X face
            for (; pMipLcl->lpAttachListFrom; pMipLcl = pMipLcl->lpAttachListFrom->lpAttached);
            // get the base texture
            pBaseTexture = GET_PNVOBJ(pMipLcl)->getTexture();
        }
    }
    else
    {
        // has mipmaps?
        if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_MIPMAP)
        {
            // registry allows?
            if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_USERMIPMAPENABLE_MASK) == D3D_REG_USERMIPMAPENABLE_ENABLE)
            {
                // count the number of mip levels from this texture down
                LPDDRAWI_DDRAWSURFACE_LCL pMipLcl;
                for (pMipLcl = pDDSLcl; pMipLcl->lpAttachList; pMipLcl = pMipLcl->lpAttachList->lpAttached) {
                    dwMipMapCount ++;
                }

                // Check if this is the base level or one of the mip levels.
                if (pDDSLcl->lpAttachListFrom != 0)
                {
                    // figure out what mip level this is, and also at the
                    //  same time, find the base texture
                    for (pMipLcl = pDDSLcl; pMipLcl->lpAttachListFrom; pMipLcl = pMipLcl->lpAttachListFrom->lpAttached) {
                        dwMipMapLevel ++;
                    }

                    // assign base texture
                    pBaseTexture = GET_PNVOBJ(pMipLcl)->getTexture();
                    assert (IS_VALID_PNVOBJ(pBaseTexture));
                }
            }
        }
    }

    // create the texture
    CNvObject *pNvObj   = GET_PNVOBJ (pDDSLcl);
    CTexture  *pTexture = IS_VALID_PNVOBJ(pNvObj) ? pNvObj->getTexture() : NULL;

    if (!pTexture)
    {
        // texture structure not created yet, do it now
#ifndef WINNT
        pNvObj   = new CNvObject (pDDSLcl->dwProcessId);
#else   // WINNT don't have pid
        pNvObj   = new CNvObject (0);
#endif
        pTexture = new CTexture;

        if (!pNvObj || !pTexture) {
            if (pNvObj) pNvObj->release();
            pGbl->fpVidMem = 0;
            SET_PNVOBJ (pDDSLcl, NULL);
            dbgTracePop();
            return (DDERR_OUTOFMEMORY);
        }

        // tag as part of a cube map if needed
        if (pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_CUBEMAP) {
            pTexture->tagCubeMap();
            //we need to augment the renaming logic to allow for
            //traversing renaming in the cube map direction as well
            //as the mipmap case.
        }
        else if (pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_VOLUME) {
            pTexture->tagVolumeMap();
            //probably shouldn't enable renaming with volumemaps because
            //n^3 size would have serious impacts on memory usage
        }
        else{
            // apply renaming policy
            if (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_TEXTURE_RENAME) {
                pTexture->enableRenaming();
            }
        }

        // store in surface
        pNvObj->setObject (CNvObject::NVOBJ_TEXTURE, pTexture);
        SET_PNVOBJ (pDDSLcl, pNvObj);
    }

#ifdef WINNT
        if (pNvObj)
        {
            // we need this pointer to release memory when the PID differs from the one used at creation time
            pNvObj->setDDSLcl(pDDSLcl);
        }
#endif

    // tag as fourCC (will also automatically select DXT)
    if (bFourCC) {
        pTexture->setFourCC (pGbl->ddpfSurface.dwFourCC);
    }
    else {
        pTexture->setFourCC (0);
    }

    // tag as color keyed
    DWORD dwCK;
    if (nvCalculateColorKey (&dwCK, pDDSLcl, dwFormat)) {
        pTexture->tagColorKey();
        pTexture->setColorKey(dwCK);
    }

    // tag as palletized
    // What should we REALLY be checking here... this is kind of a loose
    // match should the ddraw user HAVE to export both caps or just one???
    if ((pDDSLcl->ddsCaps.dwCaps & DDSCAPS_PALETTE) ||
        (pDDSLcl->lpGbl->ddpfSurface.dwFlags & DDPF_PALETTEINDEXED8))
    {
        DPF_LEVEL(NVDBG_LEVEL_PALETTE,"Surface marked palettized: pddslcl=%08x pnvobj=%08x ptexture=%08x\n",pDDSLcl, pNvObj, pTexture);
        DPF_LEVEL(NVDBG_LEVEL_PALETTE,"                           %d x %d x %d\n", dwWidth, dwHeight, dwBPPRequested<<2);
        pTexture->tagPalettized();
    }

    // tag as managed
    if ((pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_TEXTUREMANAGE) ||
        (pBaseTexture && pBaseTexture->isManaged()))
    {
        pTexture->tagManaged();
        DPF_LEVEL(NVDBG_LEVEL_TEXMAN,"Texture %08x marked as managed\n",pTexture);
    }

#ifdef TEX_MANAGE_TEST
    pTexture->tagManaged();
#endif

    // determine DX app version
    DWORD dwDXAppVersion = 0x0700;
    PNVD3DCONTEXT pContext = (PNVD3DCONTEXT)getDC()->dwContextListHead;
    while (pContext)
    {
        if (pContext->pDriverData == pDriverData)
        {
            dwDXAppVersion = pContext->dwDXAppVersion;
            break;
        }
        pContext = pContext->pContextNext;
    }

    // figure out which heap to allocate from
    DWORD dwAllowedHeaps, dwPreferredHeap;

    if (pTexture->isPalettized()) {
        // palettized textures can only go in video memory due to memory bandwidth limitations
        dwAllowedHeaps  =
        dwPreferredHeap = CSimpleSurface::HEAP_VID;
    }

    else if ((dwDXAppVersion < 0x0700) ||
             (pDriverData->GARTLinearBase == NULL)) {
        // original scheme - used for pre-DX7 apps and PCI cards
        // treat their request as a preference, but allow all allocations to fall back into AGP or PCI
        dwAllowedHeaps = CSimpleSurface::HEAP_VID |
                         (pDriverData->GARTLinearBase ? CSimpleSurface::HEAP_AGP : CSimpleSurface::HEAP_PCI);
        if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM) {
            dwPreferredHeap = (pDriverData->GARTLinearBase) ? CSimpleSurface::HEAP_AGP : CSimpleSurface::HEAP_PCI;
        }
        else {
            dwPreferredHeap = CSimpleSurface::HEAP_VID;
        }
    }

    else {
        // new scheme - used for AGP cards on DX7 and beyond
        // rigorously honor app's requests for particular types of memory
        nvAssert (pDriverData->GARTLinearBase);
        if (pDDSLcl->ddsCaps.dwCaps & (DDSCAPS_LOCALVIDMEM | DDSCAPS_3DDEVICE)) {
            // they've explicitly requested video
            nvAssert ((pDDSLcl->ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM) == 0); // these are supposed to be mutually exclusive
            dwAllowedHeaps  =
            dwPreferredHeap = CSimpleSurface::HEAP_VID;
        }
        else if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM) {
            // they've explicitly requested AGP
            nvAssert ((pDDSLcl->ddsCaps.dwCaps & DDSCAPS_LOCALVIDMEM) == 0); // these are supposed to be mutually exclusive
            dwAllowedHeaps  =
            dwPreferredHeap = CSimpleSurface::HEAP_AGP;
        }
        else {
            // they explicitly requested neither video nor AGP, so either is OK, but video is preferred
            dwAllowedHeaps  = CSimpleSurface::HEAP_VID | CSimpleSurface::HEAP_AGP;
            dwPreferredHeap = CSimpleSurface::HEAP_VID;
        }
    }

    // initially create the texture in system memory if it's managed
    // and we're using the lazy texture management strategy -- ala microsoft.
    if ((pTexture->isManaged()) &&
        (getDC()->nvD3DRegistryData.regTextureManageStrategy == D3D_REG_TEXTUREMANAGESTRATEGY_LAZY)) {
        dwAllowedHeaps  |=  CSimpleSurface::HEAP_SYS;
        dwPreferredHeap |=  CSimpleSurface::HEAP_SYS;
        dwPreferredHeap &= ~CSimpleSurface::HEAP_VID;
    }

    // create the underlying swizzled surface
    if (pBaseTexture)
    {
        // this is a mipmap level.
        // user mip levels do not own their own memory. they point to memory
        //  off of the base texture. the base texture owns the memory
        DWORD dwThisFace = pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_CUBEMAP_ALLFACES;

        if (!pTexture->create (pNvObj, dwThisFace, pBaseTexture, dwMipMapLevel))
        {
            pNvObj->release();
            pGbl->fpVidMem = 0;
            SET_PNVOBJ (pDDSLcl, NULL);
            dbgTracePop();
            return (DDERR_OUTOFVIDEOMEMORY);
        }

        // update state
        pTexture->tagUserMipLevel();

        /*
         * For DX6 runtime we need to attach this texture to the chain steming
         * from the base texture. For DX7 it gets done upon CreateTextureEx call
         */
        if (global.dwDXRuntimeVersion < 0x0700) {
            CNvObject *pNvObjFrom;
            LPDDRAWI_DDRAWSURFACE_LCL pFromLcl;
            // Go 1 level up.
            pFromLcl = pDDSLcl->lpAttachListFrom->lpAttached;
            pNvObjFrom = GET_PNVOBJ (pFromLcl);
            assert (IS_VALID_PNVOBJ(pNvObjFrom));
            if (pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_CUBEMAP)
                pNvObjFrom->setAttachedB (pNvObj);
            else
                pNvObjFrom->setAttachedA (pNvObj);
        }
    }
    else
    {
        // this is a base texture. it owns its own memory and also those of all
        //  possible mipmaps

#ifdef TEX_MANAGE

#ifdef TEX_MANAGE_TEST

        dwAllowedHeaps = dwPreferredHeap = CSimpleSurface::HEAP_VID;
#endif

        BOOL bCreationSuccess, bEvictionSuccess;

        bCreationSuccess = pTexture->create (pNvObj, dwWidth, dwHeight, dwDepth,
                                             dwBPPRequested, dwBPPGranted, dwMipMapCount, dwFormat,
                                             dwAllowedHeaps, dwPreferredHeap);

        if (!bCreationSuccess && pTexture->isManaged()) {

            PNVD3DCONTEXT pContext = (PNVD3DCONTEXT) getDC()->dwContextListHead;

            while (pContext && !bCreationSuccess) {

                nvTexManageResetPtr (pContext);

                do {
                    // evict something and try again
                    bEvictionSuccess = nvTexManageEvict (pContext, NV_TEXMAN_EVICTION_PROTOCOL_ANY);
                    if (bEvictionSuccess) {
                        bCreationSuccess = pTexture->create (pNvObj, dwWidth, dwHeight, dwDepth,
                                                             dwBPPRequested, dwBPPGranted, dwMipMapCount, dwFormat,
                                                             dwAllowedHeaps, dwPreferredHeap);
                    }
                } while (bEvictionSuccess && !bCreationSuccess);

                pContext = pContext->pContextNext;

            }

        }  // !bSuccess

        // if we still haven't managed to create the thing, we have a serious problem
        if (!bCreationSuccess) {
            pNvObj->release();
            pGbl->fpVidMem = 0;
            SET_PNVOBJ (pDDSLcl, NULL);
            dbgTracePop();
            return (DDERR_OUTOFVIDEOMEMORY);
        }

        if (pTexture->isManaged() && !pTexture->isEvicted()) {
            nvTexManageAddTexture ((PNVD3DCONTEXT) getDC()->dwContextListHead,
                                   pTexture, NV_TEXMAN_DEFAULT_CLASS);
        }

#else  // !TEX_MANAGE

        if (!pTexture->create (pNvObj, dwWidth, dwHeight, dwDepth,
                               dwBPPRequested, dwBPPGranted, dwMipMapCount, dwFormat,
                               dwAllowedHeaps, dwPreferredHeap))
        {
            pNvObj->release();
            pGbl->fpVidMem = 0;
            SET_PNVOBJ (pDDSLcl, NULL);
            dbgTracePop();
            return (DDERR_OUTOFVIDEOMEMORY);
        }
#endif  // !TEX_MANAGE
    }

    // update DDraw
#ifdef WINNT
    if (pTexture->getSwizzled()->getHeapLocation() == CSimpleSurface::HEAP_AGP) {
#ifdef NV_AGP
        if (pBaseTexture) { // texture is a user mip level
            // BUGBUG tidy this up with "chunk per process AGP heap manager"
            DWORD dwBaseAddress = pBaseTexture->getSwizzled()->getAddress();
            DWORD dwMipAddress  = pTexture->getSwizzled()->getAddress();
            pGbl->fpVidMem = nvAGPGetUserAddr(dwBaseAddress) + (dwMipAddress - dwBaseAddress);
        } else {
            pGbl->fpVidMem = nvAGPGetUserAddr(pTexture->getSwizzled()->getAddress());
        }
#else
        pGbl->lpVidMemHeap = ppdev->AgpHeap;
        pGbl->fpHeapOffset = pTexture->getSwizzled()->getfpVidMem() + pDriverData->GARTLinearHeapOffset;
#endif
        // set the bits to let Microsoft know we put it in AGP
        pDDSLcl->ddsCaps.dwCaps |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_NONLOCALVIDMEM);
    }
    else
#endif
    {
        pGbl->fpVidMem = pTexture->getSwizzled()->getfpVidMem();
    }

    // set size if linear, else set pitch
    // the correct conditional would be: (pDDSurfDesc->dwFlags & DDSD_LINEARSIZE)
    // but MS apparently hasn't set this flag yet, even though they will do so later. nice.
    if (pTexture->isDXT()) {  // for now these are the only "linear" texture surfaces
        pGbl->dwLinearSize = pTexture->getSize();
    }
    else {
        pGbl->lPitch = pTexture->getPitch();
        pGbl->lSlicePitch = pTexture->getSlicePitch();
    }

    // telling MS that we have an AGP surface causes BLTs not to come our way. Just say nothing...

    //pDDSLcl->ddsCaps.dwCaps = pTexture->getSwizzled()->modifyDDCaps(pDDSLcl->ddsCaps.dwCaps);

    // force DXT surfaces to pre-allocate sysmem copies since WB locks them
    //  all the time. This will evade costly GlobalAlloc calls during run
    if (pTexture->isDXT()) {
        pTexture->updateLinearSurface();
    }

    // done
    dbgTracePop();
    return (DD_OK);
}

//****************************************************************************
// nvTextureDestroySurface
//

BOOL nvTextureDestroySurface
(
    LPDDHAL_DESTROYSURFACEDATA pDestorySurfaceData,
    LPDDRAWI_DDRAWSURFACE_LCL  pDDSLcl
)
{
    dbgTracePush ("nvTextureDestroySurface");

    // sanity checks
    assert (pDestorySurfaceData);
    assert (pDDSLcl);

    LPDDRAWI_DDRAWSURFACE_GBL pGbl = pDDSLcl->lpGbl;
    assert (pGbl);

    // get driver context
    nvSetDriverDataPtrFromDDGbl (pDestorySurfaceData->lpDD);

    // get the texture
    CNvObject *pNvObj   = GET_PNVOBJ (pDDSLcl);
    CTexture  *pTexture = IS_VALID_PNVOBJ(pNvObj) ? pNvObj->getTexture() : NULL;

    if (pTexture)
    {
        BOOL bDestroyTexture = TRUE;
        // make sure that we do not have this texture referenced anywhere
        PNVD3DCONTEXT pContext = (PNVD3DCONTEXT)getDC()->dwContextListHead;
        while (pContext)
        {

            if (pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP] == (DWORD)pNvObj) {
                pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP] = 0;
#if (NVARCH >= 0x010)
                pContext->hwState.dwDirtyFlags |= celsiusDirtyBitsFromTextureStageState[D3DTSS_TEXTUREMAP];
                pContext->hwState.dwDirtyFlags |= kelvinDirtyBitsFromTextureStageState[D3DTSS_TEXTUREMAP];
#endif
                // for legacy apps that use the setContextState calls (e.g. tunnel)
                pContext->dwRenderState[D3DRENDERSTATE_TEXTUREHANDLE] = 0;
            }
            if (pContext->tssState[1].dwValue[D3DTSS_TEXTUREMAP] == (DWORD)pNvObj) {
                pContext->tssState[1].dwValue[D3DTSS_TEXTUREMAP] = 0;
#if (NVARCH >= 0x010)
                pContext->hwState.dwDirtyFlags |= celsiusDirtyBitsFromTextureStageState[D3DTSS_TEXTUREMAP];
                pContext->hwState.dwDirtyFlags |= kelvinDirtyBitsFromTextureStageState[D3DTSS_TEXTUREMAP];
#endif
            }
            pContext = pContext->pContextNext;
        }
        pGbl->fpVidMem = 0;

        if (global.dwDXRuntimeVersion >= 0x0700) {
            bDestroyTexture = (pDDSLcl->dwFlags & DDRAWISURF_INVALID) ? FALSE : TRUE;
        }
        else {
            bDestroyTexture = pTexture->hasHandle();
        }

        // destroy texture object if handle is freed already (or allocated at all)
        if (bDestroyTexture)
        {
            CNvObject *pNvObj = pTexture->getWrapper();

#ifdef WINNT
            // we need this pointer to release memory when the PID differs from the one used at creation time
            pNvObj->setDDSLcl(pDDSLcl);
#endif
#ifdef TEX_MANAGE
            if (pTexture->isManaged() && pTexture->isBaseTexture() && pTexture->isResident()) {
                nvTexManageRemoveTexture (pTexture);
            }
#endif

            // clean up ddraw
            SET_PNVOBJ (pDDSLcl, NULL);
            // BUGBUG what about the object list entry?

            // object release will delete the texture object and surface memory
            pNvObj->release();
        }
    }

    else
    {
        // not our surface - punt
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return FALSE;
    }

    // done
    NvReleaseSemaphore(pDriverData);
    pDestorySurfaceData->ddRVal = DD_OK;
    dbgTracePop();
    return TRUE;
}

//---------------------------------------------------------------------------

// nvTextureBlt
// performs blts to and/or from a texture

DWORD nvTextureBltDX7
(
    PNVD3DCONTEXT   pContext,  // may be NULL!!
    CNvObject      *pSrcObj,   // source nv object
    CNvObject      *pDstObj,   // dest nv object
    D3DBOX         *boxSrc,      // src rect
    DWORD           dwDstX,
    DWORD           dwDstY,
    DWORD           dwDstZ,
    DWORD           dwFlags    // blt flags
)
{
    BOOL bForceTextureInvalidate = FALSE;
    dbgTracePush ("nvTextureBltDX7");

#ifdef NO_TEX_BLTS

    // done
    dbgTracePop();
    return (DD_OK);
#endif


#if defined(NV_NULL_DRIVER) || defined(NV_NULL_TEXTURE_DRIVER)
    dbgTracePop();
    return (DD_OK);
#endif //NV_NULL_DRIVER

    if ((pSrcObj == NULL) || (pDstObj == NULL)) {
        // this routine doesn't handle things without sources (like blitFX)
        // and obviously we always need a destination
        dbgTracePop();
        return (DDERR_UNSUPPORTED);
    }

    DWORD dwSrcClass = pSrcObj->getClass();
    DWORD dwDstClass = pDstObj->getClass();

    if ((dwSrcClass != CNvObject::NVOBJ_TEXTURE) && (dwSrcClass != CNvObject::NVOBJ_SIMPLESURFACE)) {
        dbgTracePop();
        return (DDERR_UNSUPPORTED);
    }
    if ((dwDstClass != CNvObject::NVOBJ_TEXTURE) && (dwDstClass != CNvObject::NVOBJ_SIMPLESURFACE)) {
        dbgTracePop();
        return (DDERR_UNSUPPORTED);
    }

    // extract useful data about the source and destination surfaces

    // src
    DWORD dwSrcAddr;
    DWORD dwSrcPitch;
    DWORD dwSrcSlicePitch;
    DWORD dwSrcBPP;
    DWORD dwSrcHeap;
    DWORD dwSrcFourCC;
    BOOL  bSrcDXT;
    DWORD dwSrcSurfaceWidth;
    DWORD dwSrcSurfaceHeight;
    DWORD dwSrcSurfaceDepth;
    DWORD dwSrcSurfaceLogWidth;
    DWORD dwSrcSurfaceLogHeight;
    DWORD dwSrcSurfaceLogDepth;

    DWORD dwSrcX0 = boxSrc->Left;
    DWORD dwSrcY0 = boxSrc->Top;
    DWORD dwSrcZ0 = boxSrc->Front;
    DWORD dwSrcX1 = boxSrc->Right;
    DWORD dwSrcY1 = boxSrc->Bottom;
    DWORD dwSrcZ1 = boxSrc->Back;

    DWORD bSrcIsVolumeMap;

    CTexture       *pSrcTexture;
    CSimpleSurface *pSrcSurface;

    if (dwSrcClass == CNvObject::NVOBJ_TEXTURE)
    {
        pSrcTexture = pSrcObj->getTexture();
        pSrcSurface = pSrcTexture->getLinear();
        bSrcIsVolumeMap = pSrcTexture->isVolumeMap();

        if (pSrcSurface->isUpToDate()) {
            dwSrcAddr  = pSrcSurface->getAddress();
            dwSrcHeap  = pSrcSurface->getHeapLocation();
        }
        else {
            dwSrcAddr  = pSrcTexture->getSwizzled()->getAddress();
            dwSrcHeap  = pSrcTexture->getSwizzled()->getHeapLocation();
        }
    }
    else  // dwClassSrc == CNvObject::FORMATTEDSURFACE
    {
        pSrcTexture = NULL;
        pSrcSurface = pSrcObj->getSimpleSurface();
        dwSrcAddr   = pSrcSurface->getAddress();
        dwSrcHeap   = pSrcSurface->getHeapLocation();
        bSrcIsVolumeMap = ((dwSrcZ1 - dwSrcZ0) != 1);
    }

    // check if source was previously auto-paletized
    if (pSrcTexture) {
        // if this is the source for a blit, we must restore the original contents
        nvAutoPaletteCheckAndRevert(pSrcTexture, FALSE);
    }

    dwSrcPitch  = pSrcSurface->getPitch();
    dwSrcSlicePitch = pSrcSurface->getSlicePitch();
    dwSrcBPP    = pSrcSurface->getBPP();
    dwSrcFourCC = pSrcSurface->getFourCC();
    bSrcDXT     = pSrcSurface->isDXT();
    dwSrcSurfaceWidth     = pSrcSurface->getWidth();
    dwSrcSurfaceHeight    = pSrcSurface->getHeight();
    dwSrcSurfaceDepth     = pSrcSurface->getDepth();
    dwSrcSurfaceLogWidth  = dwSrcSurfaceWidth;
    dwSrcSurfaceLogHeight = dwSrcSurfaceHeight;
    dwSrcSurfaceLogDepth  = dwSrcSurfaceDepth;
    ASM_LOG2(dwSrcSurfaceLogWidth);
    ASM_LOG2(dwSrcSurfaceLogHeight);
    ASM_LOG2(dwSrcSurfaceLogDepth);

    // dest
    DWORD     dwDstAddr;
    DWORD     dwDstPitch;
    DWORD     dwDstSlicePitch;
    DWORD     dwDstBPP;
    DWORD     dwDstHeap;
    DWORD     dwDstFourCC;
    BOOL      bDstDXT;
    DWORD     dwDstSurfaceWidth;
    DWORD     dwDstSurfaceHeight;
    DWORD     dwDstSurfaceDepth;
    DWORD     dwDstSurfaceLogWidth;
    DWORD     dwDstSurfaceLogHeight;
    DWORD     dwDstSurfaceLogDepth;

    DWORD     dwDstX0 = dwDstX;
    DWORD     dwDstY0 = dwDstY;
    DWORD     dwDstZ0 = dwDstZ;
    DWORD     dwDstX1 = dwDstX0 + (dwSrcX1 - dwSrcX0);
    DWORD     dwDstY1 = dwDstY0 + (dwSrcY1 - dwSrcY0);
    DWORD     dwDstZ1 = dwDstZ0 + (dwSrcZ1 - dwSrcZ0);

    DWORD     bDstIsVolumeMap;

    CTexture       *pDstTexture;
    CSimpleSurface *pDstSurface;

    if (dwDstClass == CNvObject::NVOBJ_TEXTURE)
    {
        pDstTexture = pDstObj->getTexture();
        pDstSurface = pDstTexture->getLinear();
        dwDstAddr   = pDstTexture->getSwizzled()->getAddress();
        dwDstHeap   = pDstTexture->getSwizzled()->getHeapLocation();
        bDstIsVolumeMap = pDstTexture->isVolumeMap();
    }
    else  // dwClassDst == CNvObject::FORMATTEDSURFACE
    {
        pDstTexture = NULL;
        pDstSurface = pDstObj->getSimpleSurface();
        dwDstAddr   = pDstSurface->getAddress();
        dwDstHeap   = pDstSurface->getHeapLocation();
        bDstIsVolumeMap = ((dwDstZ1 - dwDstZ0) != 1);
    }

    dwDstPitch  = pDstSurface->getPitch();
    dwDstSlicePitch = pDstSurface->getSlicePitch();
    dwDstBPP    = pDstSurface->getBPP();
    dwDstFourCC = pDstSurface->getFourCC();
    bDstDXT     = pDstSurface->isDXT();
    dwDstSurfaceWidth     = pDstSurface->getWidth();
    dwDstSurfaceHeight    = pDstSurface->getHeight();
    dwDstSurfaceDepth     = pDstSurface->getDepth();
    dwDstSurfaceLogWidth  = dwDstSurfaceWidth;
    dwDstSurfaceLogHeight = dwDstSurfaceHeight;
    dwDstSurfaceLogDepth  = dwDstSurfaceDepth;
    ASM_LOG2 (dwDstSurfaceLogWidth);
    ASM_LOG2 (dwDstSurfaceLogHeight);
    ASM_LOG2 (dwDstSurfaceLogDepth);

    // derive commonly used values from extracted data
    DWORD dwBlitWidth  = dwSrcX1 - dwSrcX0;
    DWORD dwBlitHeight = dwSrcY1 - dwSrcY0;
    DWORD dwBlitDepth  = dwSrcZ1 - dwSrcZ0;

    BOOL  bFullDstSurface = FALSE;
    if (pDstSurface->isDXT())
    {
        if ((dwBlitWidth  == dwDstSurfaceWidth * 4) &&
            (dwBlitHeight == dwDstSurfaceHeight * 4) &&
            (dwBlitDepth == dwDstSurfaceDepth)) {   // Volume DXT's don't get multiplied, right?
            bFullDstSurface = TRUE;
        }
    }
    else
    {
        if ((dwBlitWidth  == dwDstSurfaceWidth) &&
            (dwBlitHeight == dwDstSurfaceHeight) &&
            (dwBlitDepth == dwDstSurfaceDepth)) {
            bFullDstSurface = TRUE;
        }
    }

    // validate parameters
    if ((dwSrcBPP != dwDstBPP)                    // bit depth must match
     || (!dwBlitHeight)                           // at least something to do
     || (!dwBlitWidth)                            // at least something to do
     || (!dwBlitDepth)                            // at least something to do
     || (!pSrcTexture && !pDstTexture)            // at least one texture involved
     || (bSrcDXT != bDstDXT)                      // we can only blit from one DXT to another
     || (bSrcDXT && (dwSrcFourCC != dwDstFourCC))) // we can only blit between similar DXTs
    {
        dbgTracePop();
        return (DDERR_UNSUPPORTED);
    }

    // this check doesn't seem to be required
/*    if ((dwSrcSurfaceWidth  != dwDstSurfaceWidth) ||
        (dwSrcSurfaceHeight != dwDstSurfaceHeight))
    {
        dbgTracePop();
        return (DDERR_NOSTRETCHHW);
    }*/

    // determine swizzledness. 4cc's are special cases handled first

    BOOL  bSrcSwizzled;
    BOOL  bDstSwizzled;
    DWORD dwSwizzledness;

    if (dwDstFourCC) {
        switch (dwDstFourCC) {
            case FOURCC_NVT0:
            case FOURCC_NVT1:
            case FOURCC_NVT2:
            case FOURCC_NVT3:
            case FOURCC_NVT4:
            case FOURCC_NVT5:
                bSrcSwizzled = TRUE;
                bDstSwizzled = TRUE;
                break;
            case FOURCC_NVS0:
            case FOURCC_NVS1:
            case FOURCC_NVS2:
            case FOURCC_NVS3:
            case FOURCC_NVS4:
            case FOURCC_NVS5:
#ifdef TEXFORMAT_CRD // --------------------------------
            case FOURCC_NVHU:
            case FOURCC_NVHS:
#else // !TEXFORMAT_CRD  --------------------------------
#if defined(HILO_SUPPORT_DX7) || defined(HILO_SUPPORT_DX8)
            case FOURCC_NVHU:
            case FOURCC_NVHS:
#endif
#endif // !TEXFORMAT_CRD  --------------------------------
                bSrcSwizzled = FALSE;
                bDstSwizzled = TRUE;
                break;
            case D3DFMT_Q8W8V8U8:
                bSrcSwizzled = pSrcTexture ? (pSrcTexture->getLinear()->isUpToDate() ? FALSE : TRUE) : FALSE;
                bDstSwizzled = pDstTexture != NULL;
                break;
            case FOURCC_DXT1:
            case FOURCC_DXT2:
            case FOURCC_DXT3:
            case FOURCC_DXT4:
            case FOURCC_DXT5:
                bSrcSwizzled = TRUE;        // well, it's compressed, but we want to use
                bDstSwizzled = TRUE;        // swz to swz blit copy because it supports renaming
                break;
            default:
                DPF ("unhandled 4cc in texture blit");
                dbgD3DError();
                dbgTracePop();
                return (DDERR_UNSUPPORTED);
                break;
        }  // switch
    }

    else {
        bSrcSwizzled = pSrcTexture ? (pSrcTexture->getLinear()->isUpToDate() ? FALSE : TRUE) : FALSE;
        bDstSwizzled = pDstTexture != NULL;
    }

    dwSwizzledness = (bSrcSwizzled ? 0x10 : 0x00) | (bDstSwizzled ? 0x01 : 0x00);

    // determine if we can allow HW to blt the texture
    //  - must be faster
    //  - must not be 1 wide (NV5 cannot do)
    //  - must not be 2046 wide or wdire (NV5 cannot do)
    //  - source must be in a suitable location
    //  - dest must be in a suitable location
    BOOL bAllowHW = (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_TEXTURE_USEHW)
                 && ((dwSrcHeap == CSimpleSurface::HEAP_VID)
                  || (dwSrcHeap == CSimpleSurface::HEAP_AGP)
                  || (dwSrcHeap == CSimpleSurface::HEAP_PCI))
                 && (dwDstHeap == CSimpleSurface::HEAP_VID);
    // exception - vid to vid ALWAYS use HW
    if ((dwSrcHeap == CSimpleSurface::HEAP_VID)
     && (dwDstHeap == CSimpleSurface::HEAP_VID))
    {
        bAllowHW = TRUE;
    }

    // exception - certain dimensions cannot be swizzled (although this also kills lin2lin)
    if ((dwBlitWidth  <= 1)
     || (dwBlitWidth  >= 2046)
     || (dwBlitHeight >= 2046))
    {
        bAllowHW = FALSE;
    }

    // check if this was previously auto-paletized
    if (pDstTexture) {
        nvAutoPaletteCheckAndRevert(pDstTexture, bFullDstSurface);
    }

#if 0
    // Display the source texture on the screen. (DEBUG only)
    dbgDisplayLinearTexture (lpBltData->lpDDSrcSurface,
                             lpBltData->lpDDDestSurface,
                             pDstTexture ? pDstTexture->getFormat()
                                         : pSrcTexture->getFormat());
#endif

    // we've gathered / calculated all the information we need to actually do the blits.
    // there are just a couple of cases we support. Punt on others (ddraw will magically handle!)

#ifdef CNT_TEX
    if ((dwDstHeap == CSimpleSurface::HEAP_VID) && (dwSrcHeap != CSimpleSurface::HEAP_VID)) {
        g_dwVidTexDL[g_dwFrames]++;
    } else if ((dwSrcHeap != CSimpleSurface::HEAP_VID) && (dwSrcHeap != CSimpleSurface::HEAP_AGP) && (dwDstHeap == CSimpleSurface::HEAP_AGP)) {
        g_dwAgpTexDL[g_dwFrames]++;
    }
#endif // CNT_TEX

    if (0) { // TODO  (bAllowHW) {

        // get lock on textures
        if (pSrcTexture) {
            pSrcTexture->hwLock (pContext, CSimpleSurface::LOCK_NORMAL);
        }
        if (pDstTexture) {
            pDstTexture->hwLock (pContext, CSimpleSurface::LOCK_NORMAL);
        }

        // TODO: HW blits

        // release locks on textures
        if (pSrcTexture) {
            pSrcTexture->hwUnlock();
        }
        if (pDstTexture) {
            pDstTexture->hwUnlock();
        }

    }
    else {
        switch (dwSwizzledness)
        {

            case 0x00:  // L -> L
                        #ifdef NV_PROFILE_CPULOCK
                        NVP_START (NVP_T_TEX_BLT);
                        #endif
                DDLOCKINDEX(NVSTAT_LOCK_L2L_COPY);
                if (pSrcTexture) pSrcTexture->cpuLockLin (CSimpleSurface::LOCK_NORMAL);
                if (pDstTexture) pDstTexture->cpuLockLin (CSimpleSurface::LOCK_NORMAL);
                        #ifdef NV_PROFILE_CPULOCK
                        NVP_STOP (NVP_T_TEX_BLT);
                        nvpLogTime (NVP_T_TEX_BLT,nvpTime[NVP_T_TEX_BLT]);
                        #endif

                assert (!bDstDXT); // we dont want dxt blts here

                nvSwizBlt_cpu_lin2lin (dwSrcAddr, dwSrcPitch, dwSrcX0, dwSrcY0,
                                       dwDstAddr, dwDstPitch, dwDstX0, dwDstY0,
                                       dwBlitWidth, dwBlitHeight, dwSrcBPP);

                if (pSrcTexture) pSrcTexture->cpuUnlockLin();
                if (pDstTexture) pDstTexture->cpuUnlockLin();
                break;

            case 0x01:  // L -> S
                        #ifdef NV_PROFILE_CPULOCK
                        NVP_START (NVP_T_TEX_BLT);
                        #endif
                DDLOCKINDEX(NVSTAT_LOCK_L2O_COPY);

                if (pSrcTexture)
                {
                    pSrcTexture->cpuLockLin (CSimpleSurface::LOCK_NORMAL);
                }
                else
                {
                    pSrcSurface->cpuLock (CSimpleSurface::LOCK_NORMAL);
                }
                if (pDstTexture)
                {
                    if (bFullDstSurface)
                    {
                        pDstTexture->cpuLockSwz (CSimpleSurface::LOCK_RENAME);
                        pDstTexture->getSwizzled()->tagUpToDate();
                        dwDstAddr = pDstTexture->getSwizzled()->getAddress();
                    }
                    else
                    {
                        pDstTexture->cpuLockSwz (CSimpleSurface::LOCK_NORMAL);
                    }
                }
                else
                {
                    pDstSurface->cpuLock (CSimpleSurface::LOCK_NORMAL);
                }
                        #ifdef NV_PROFILE_CPULOCK
                        NVP_STOP (NVP_T_TEX_BLT);
                        nvpLogTime (NVP_T_TEX_BLT,nvpTime[NVP_T_TEX_BLT]);
                        #endif
                nvAssert (!bDstDXT); // we dont want dxt blts here

#ifndef CAPTURE
                if (pDstTexture)
                {
#ifndef NO_TEX_SWZ
                    nvSwizBlt_lin2swz (pContext, dwSrcAddr, dwSrcPitch, dwSrcSlicePitch, dwSrcX0, dwSrcY0, dwSrcZ0,
                                       pDstTexture, dwDstAddr, dwDstX0, dwDstY0, dwDstZ0,
                                       dwDstSurfaceLogWidth, dwDstSurfaceLogHeight, dwDstSurfaceLogDepth,
                                       dwBlitWidth, dwBlitHeight, dwBlitDepth, dwSrcBPP);
                }
                else
#else
                    nvSwizBlt_cpu_lin2lin (dwSrcAddr, dwSrcPitch, dwSrcX0, dwSrcY0,
                                           dwDstAddr, dwDstPitch, dwDstX0, dwDstY0,
                                           dwBlitWidth, dwBlitHeight, dwSrcBPP);
                }
                else
#endif // No_Tex_SWZ
#endif  // CAPTURE
                {
#ifndef NO_TEX_SWZ
                    nvSwizBlt_cpu_lin2swz (dwSrcAddr, dwSrcPitch, dwSrcX0, dwSrcY0,
                                           dwDstAddr, dwDstX0, dwDstY0, dwDstSurfaceLogWidth, dwDstSurfaceLogHeight,
                                           dwBlitWidth, dwBlitHeight, dwSrcBPP);
                }
#else
                    nvSwizBlt_cpu_lin2lin (dwSrcAddr, dwSrcPitch, dwSrcX0, dwSrcY0,
                                           dwDstAddr, dwDstPitch, dwDstX0, dwDstY0,
                                           dwBlitWidth, dwBlitHeight, dwSrcBPP);
                }
#endif // No_Tex_SWZ
                // capture
#ifdef CAPTURE
                if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
                    CAPTURE_MEMORY_WRITE memwr;
                    memwr.dwJmpCommand         = CAPTURE_JMP_COMMAND;
                    memwr.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
                    memwr.dwExtensionID        = CAPTURE_XID_MEMORY_WRITE;
                    memwr.dwCtxDMAHandle       = (pDstTexture->getSwizzled()->getHeapLocation() == CSimpleSurface::HEAP_VID)
                                               ? NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY
                                               : D3D_CONTEXT_DMA_HOST_MEMORY;
                    memwr.dwOffset             = pDstTexture->getSwizzled()->getOffset();
                    memwr.dwSize               = pDstTexture->getWidth() * pDstTexture->getHeight() * pDstTexture->getBPP();
                    captureLog (&memwr,sizeof(memwr));
                    captureLog ((void*)pDstTexture->getSwizzled()->getAddress(),memwr.dwSize);
                }
#endif

                if (pSrcTexture) pSrcTexture->cpuUnlockLin();
                if (pDstTexture)
                {
                    pDstTexture->cpuUnlockSwz();
                    pDstTexture->tagRenamedAndCopied();
                    invalidateTextureCache(pContext);
                    if (pContext)
                        pContext->bStateChange = TRUE;
                    else
                        bForceTextureInvalidate = TRUE;
                }
                break;

            case 0x10:  // S -> L
                        #ifdef NV_PROFILE_CPULOCK
                        NVP_START (NVP_T_TEX_BLT);
                        #endif
                DDLOCKINDEX(NVSTAT_LOCK_O2L_COPY);
                if (pSrcTexture) pSrcTexture->cpuLockSwz (CSimpleSurface::LOCK_NORMAL);
                if (pDstTexture) pDstTexture->cpuLockLin (CSimpleSurface::LOCK_NORMAL);
                        #ifdef NV_PROFILE_CPULOCK
                        NVP_STOP (NVP_T_TEX_BLT);
                        nvpLogTime (NVP_T_TEX_BLT,nvpTime[NVP_T_TEX_BLT]);
                        #endif

                assert (!bDstDXT); // we dont want dxt blts here
#ifndef NO_TEX_SWZ
                nvSwizBlt_cpu_swz2lin (dwSrcAddr, dwSrcX0, dwSrcY0, dwSrcSurfaceLogWidth, dwSrcSurfaceLogHeight,
                                       dwDstAddr, dwDstPitch, dwDstX0, dwDstY0,
                                       dwBlitWidth, dwBlitHeight, dwSrcBPP);
#else
                nvSwizBlt_cpu_lin2lin (dwSrcAddr, dwSrcPitch, dwSrcX0, dwSrcY0,
                                       dwDstAddr, dwDstPitch, dwDstX0, dwDstY0,
                                       dwBlitWidth, dwBlitHeight, dwSrcBPP);
#endif
                if (pSrcTexture) pSrcTexture->cpuUnlockSwz();
                if (pDstTexture) pDstTexture->cpuUnlockLin();
                break;

            case 0x11:  // S -> S
                        #ifdef NV_PROFILE_CPULOCK
                        NVP_START (NVP_T_TEX_BLT);
                        #endif
                DDLOCKINDEX(NVSTAT_LOCK_O2O_COPY);
                if (pSrcTexture) pSrcTexture->cpuLockSwz (CSimpleSurface::LOCK_NORMAL);
                if (pDstTexture)
                {
                    if (bDstDXT && bFullDstSurface)
                    {
                        // allow renaming of DXT surfaces
                        pDstTexture->cpuLockSwz (CSimpleSurface::LOCK_RENAME);
                        dwDstAddr = pDstTexture->getSwizzled()->getAddress();
                    }
                    else
                    {
                        pDstTexture->cpuLockSwz (CSimpleSurface::LOCK_NORMAL);
                    }
                }
                        #ifdef NV_PROFILE_CPULOCK
                        NVP_STOP (NVP_T_TEX_BLT);
                        nvpLogTime (NVP_T_TEX_BLT,nvpTime[NVP_T_TEX_BLT]);
                        #endif
                if (bDstDXT)
                {
                    // we assume that all dxt blts are 4 texel aligned.
                    // if not, we blt a little extra
                    nvSwizBlt_cpu_lin2lin (dwSrcAddr, dwSrcPitch, dwSrcX0 >> 2, dwSrcY0 >> 2,
                                           dwDstAddr, dwDstPitch, dwDstX0 >> 2, dwDstY0 >> 2,
                                           (dwBlitWidth + 3) >> 2, (dwBlitHeight + 3) >> 2, dwSrcBPP);
                }
                else
                {
#ifndef NO_TEX_SWZ
                    nvSwizBlt_cpu_swz2swz (dwSrcAddr, dwSrcX0, dwSrcY0, dwSrcSurfaceLogWidth, dwSrcSurfaceLogHeight,
                                           dwDstAddr, dwDstX0, dwDstY0, dwDstSurfaceLogWidth, dwDstSurfaceLogHeight,
                                           dwBlitWidth, dwBlitHeight, dwSrcBPP);
#else
                    nvSwizBlt_cpu_lin2lin (dwSrcAddr, dwSrcPitch, dwSrcX0, dwSrcY0,
                                           dwDstAddr, dwDstPitch, dwDstX0, dwDstY0,
                                           dwBlitWidth, dwBlitHeight, dwSrcBPP);
#endif
                }
                // capture
#ifdef CAPTURE
                if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
                    CAPTURE_MEMORY_WRITE memwr;
                    memwr.dwJmpCommand         = CAPTURE_JMP_COMMAND;
                    memwr.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
                    memwr.dwExtensionID        = CAPTURE_XID_MEMORY_WRITE;
                    memwr.dwCtxDMAHandle       = (pDstTexture->getSwizzled()->getHeapLocation() == CSimpleSurface::HEAP_VID)
                                               ? NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY
                                               : D3D_CONTEXT_DMA_HOST_MEMORY;
                    memwr.dwOffset             = pDstTexture->getSwizzled()->getOffset();
                    memwr.dwSize               = pDstTexture->getWidth() * pDstTexture->getHeight() * pDstTexture->getBPP();
                    captureLog (&memwr,sizeof(memwr));
                    captureLog ((void*)pDstTexture->getSwizzled()->getAddress(),memwr.dwSize);
                }
#endif

                if (pSrcTexture) pSrcTexture->cpuUnlockSwz();
                if (pDstTexture)
                {
                    pDstTexture->cpuUnlockSwz();
                    pDstTexture->tagRenamedAndCopied();
                    invalidateTextureCache (pContext);
                    if(!pContext) bForceTextureInvalidate=TRUE;
                }
                break;

            default:
                DPF ("uh oh. unknown swizzledness");
                dbgD3DError();
                break;

        }  // switch

    }

    // update flags
    if (pDstTexture)
    {
        if (dwSwizzledness & 0x01) // dest swizzled?
        {
            pDstTexture->getLinear()->tagOutOfDate();
            pDstTexture->getSwizzled()->tagUpToDate();
        }
        else
        {
            pDstTexture->getSwizzled()->tagOutOfDate();
            pDstTexture->getLinear()->tagUpToDate();
            // if this is part of a cube map/mipmap, tag the base as dirty
            //  this will trigger a proper update of all subsurfaces before HW touches them
            if (pDstTexture->isUserMipLevel() || pDstTexture->isCubeMap())
            {
                pDstTexture->getBaseTexture()->tagSubSurfacesDirty();
            }
        }
    }

    // todo: auto mipmaps

    // force a cache invalidation on all contexts because we might have forced
    // texture renaming and we haven't informed the hardware that we have moved
    // the texture.
    if ((pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_TEXTURE_RENAME) && bForceTextureInvalidate) {
        // run through the list of contexts and invalidate them all
        PNVD3DCONTEXT pCtxt = (PNVD3DCONTEXT) getDC()->dwContextListHead;
        while (pCtxt) {
            invalidateTextureCache (pCtxt);
            pCtxt->bStateChange = TRUE;
            pCtxt = pCtxt->pContextNext;
        }
    }

    // this was a 2D operation
    pDriverData->TwoDRenderingOccurred = TRUE;

    dbgFlushType (NVDBG_FLUSH_2D);

    // Display the swizzled texture on the screen. (DEBUG only)
    if (pDstTexture)
    {
        dbgTextureDisplay (pDstTexture);
    }

    // done
    dbgTracePop();
    return (DD_OK);
}

//---------------------------------------------------------------------------

// fill a sub-rect of a swizzled texture with a color

DWORD nvTextureColorFill
(
    CNvObject *pDstObj,
    DWORD dwX0,         // corner of fill region
    DWORD dwY0,
    DWORD dwFillWidth,  // size of fill region
    DWORD dwFillHeight,
    DWORD dwColor       // fill color
)
{
    DWORD dwX, dwXInterleaved;
    DWORD dwY, dwYInterleaved;
    DWORD dwIndex;

    dbgTracePush ("nvTextureColorFill");

    CTexture *pTex = pDstObj->getTexture();
    nvAssert (pTex);

    // lock the swizzled surface since we're going to mess with it
    DDLOCKINDEX(NVSTAT_LOCK_FILL_OPT);
    pTex->cpuLockSwz (CSimpleSurface::LOCK_NORMAL);

    DWORD dwAddr  = pTex->getSwizzled()->getAddress();
    DWORD dwPitch = pTex->getPitch();
    DWORD dwBPP   = pTex->getBPP();

    DWORD dwTexWidth  = pTex->getWidth();
    DWORD dwTexHeight = pTex->getHeight();

    switch (dwBPP) {

        case 2:
            for (dwY=dwY0; dwY < dwY0+dwFillHeight; dwY++) {
                V_INTERLEAVE (dwYInterleaved, dwY, dwTexWidth, dwTexHeight);
                for (dwX = dwX0; dwX < dwX0+dwFillWidth; dwX++) {
                    U_INTERLEAVE (dwXInterleaved, dwX, dwTexWidth, dwTexHeight);
                    dwIndex = dwXInterleaved | dwYInterleaved;
                    ((WORD*)dwAddr)[dwIndex] = (WORD)dwColor;
                }
            }
            break;

        case 4:
            for (dwY=dwY0; dwY < dwY0+dwFillHeight; dwY++) {
                V_INTERLEAVE (dwYInterleaved, dwY, dwTexWidth, dwTexHeight);
                for (dwX = dwX0; dwX < dwX0+dwFillWidth; dwX++) {
                    U_INTERLEAVE (dwXInterleaved, dwX, dwTexWidth, dwTexHeight);
                    dwIndex = dwXInterleaved | dwYInterleaved;
                    ((DWORD*)dwAddr)[dwIndex] = (DWORD)dwColor;
                }
            }
            break;

        case 1:
            for (dwY=dwY0; dwY < dwY0+dwFillHeight; dwY++) {
                V_INTERLEAVE (dwYInterleaved, dwY, dwTexWidth, dwTexHeight);
                for (dwX = dwX0; dwX < dwX0+dwFillWidth; dwX++) {
                    U_INTERLEAVE (dwXInterleaved, dwX, dwTexWidth, dwTexHeight);
                    dwIndex = dwXInterleaved | dwYInterleaved;
                    ((unsigned char *)dwAddr)[dwIndex] = (unsigned char)dwColor;
                }
            }
            break;
        default:
            DPF ("unknown bit depth in nvTextureColorFill");
            dbgD3DError();
            break;

    } // switch

    // update tags
    pTex->getSwizzled()->tagUpToDate();
    pTex->getLinear()->tagOutOfDate();

    // unlock the swizzled surface
    pTex->cpuUnlockSwz();

    dbgTextureDisplay (pTex);

    dbgTracePop();
    return (DD_OK);
}

//---------------------------------------------------------------------------

DWORD nvTextureStretchBlit
(
    LPSURFINFO pSrcInfo,
    LPSURFINFO pDstInfo
)
{
    dbgTracePush ("nvTextureStretchBlit");

    nvAssert (pDstInfo->pObj);
    CTexture *pTexDst = pDstInfo->pObj->getTexture();
    nvAssert (pTexDst);

    // lock the swizzled surface since we're going to mess with it
    DDLOCKINDEX(NVSTAT_LOCK_STR_BLT);
    pTexDst->cpuLockSwz (CSimpleSurface::LOCK_NORMAL);

    DWORD dwAddrSrc = pSrcInfo->fpVidMem;
    DWORD dwAddrDst = pDstInfo->fpVidMem;

    DWORD dwPitchSrc = pSrcInfo->dwPitch;
    DWORD dwBPP = pSrcInfo->dwBytesPerPixel;

    DWORD dwWidthDst  = pTexDst->getWidth();
    DWORD dwHeightDst = pTexDst->getHeight();

    DWORD dwXDst, dwXDstInterleaved;
    DWORD dwYDst, dwYDstInterleaved;
    DWORD dwOffsetSrc, dwIndexDst;

    float fXSrc, fYSrc;

    float fdxSrc_dxDst = float(pSrcInfo->dwSubRectWidth)  / float(pDstInfo->dwSubRectWidth);
    float fdySrc_dyDst = float(pSrcInfo->dwSubRectHeight) / float(pDstInfo->dwSubRectHeight);

    switch (dwBPP) {

        case 2:

            fYSrc = float (pSrcInfo->dwSubRectY);

            for (dwYDst=pDstInfo->dwSubRectY; dwYDst < pDstInfo->dwSubRectY + pDstInfo->dwSubRectHeight; dwYDst++) {

                V_INTERLEAVE (dwYDstInterleaved, dwYDst, dwWidthDst, dwHeightDst);

                fXSrc = float (pSrcInfo->dwSubRectX);

                for (dwXDst=pDstInfo->dwSubRectX; dwXDst < pDstInfo->dwSubRectX + pDstInfo->dwSubRectWidth; dwXDst++) {

                    U_INTERLEAVE (dwXDstInterleaved, dwXDst, dwWidthDst, dwHeightDst);

                    dwIndexDst  = dwXDstInterleaved | dwYDstInterleaved;
                    dwOffsetSrc = (DWORD(fYSrc) * dwPitchSrc) + (DWORD(fXSrc) * dwBPP);

                    ((WORD*)dwAddrDst)[dwIndexDst] = *(WORD*)(dwAddrSrc + dwOffsetSrc);

                    // increment src x coord
                    fXSrc += fdxSrc_dxDst;
                }

                // increment src y coord
                fYSrc += fdySrc_dyDst;
            }

            break;

        case 4:

            fYSrc = float (pSrcInfo->dwSubRectY);

            for (dwYDst=pDstInfo->dwSubRectY; dwYDst < pDstInfo->dwSubRectY + pDstInfo->dwSubRectHeight; dwYDst++) {

                V_INTERLEAVE (dwYDstInterleaved, dwYDst, dwWidthDst, dwHeightDst);

                fXSrc = float (pSrcInfo->dwSubRectX);

                for (dwXDst=pDstInfo->dwSubRectX; dwXDst < pDstInfo->dwSubRectX + pDstInfo->dwSubRectWidth; dwXDst++) {

                    U_INTERLEAVE (dwXDstInterleaved, dwXDst, dwWidthDst, dwHeightDst);

                    dwIndexDst  = dwXDstInterleaved | dwYDstInterleaved;
                    dwOffsetSrc = (DWORD(fYSrc) * dwPitchSrc) + (DWORD(fXSrc) * dwBPP);

                    ((DWORD*)dwAddrDst)[dwIndexDst] = *(DWORD*)(dwAddrSrc + dwOffsetSrc);

                    // increment src x coord
                    fXSrc += fdxSrc_dxDst;
                }

                // increment src y coord
                fYSrc += fdySrc_dyDst;
            }

            break;

        default:
            DPF ("unknown bit depth in nvTextureStretchBlit");
            dbgD3DError();
            break;

    } // switch

    // update tags
    pTexDst->getSwizzled()->tagUpToDate();
    pTexDst->getLinear()->tagOutOfDate();

    // unlock the swizzled surface
    pTexDst->cpuUnlockSwz();

    dbgTextureDisplay (pTexDst);

    dbgTracePop();
    return (DD_OK);

}

//---------------------------------------------------------------------------

// nvTextureLock
//
// locks a texture so the user can party on the texels

DWORD nvTextureLock
(
    CTexture         *pTexture,
    LPDDHAL_LOCKDATA  lpLockData
)
{
    DWORD retVal;
    retVal = DDHAL_DRIVER_NOTHANDLED;

    dbgTracePush ("nvTextureLock");

    assert (pTexture);

    // figure out what we need to do
    BOOL  bNoLinear   = FALSE;
    BOOL  bNoSysLock  = lpLockData->dwFlags & DDLOCK_NOSYSLOCK;
    DWORD dwOldLinear = pTexture->getLinear()->getAddress();

    if (pTexture->hasAutoPalette())
    {
        lpLockData->ddRVal = DD_OK;
        return DDHAL_DRIVER_NOTHANDLED;
    }

    // swizzled NVT format textures
    if ((pTexture->getFourCC() & FOURCC_NVT_MASK) == FOURCC_NVTx)
    {
        bNoLinear = TRUE;
    }
    else  // linear format
    {
        pTexture->updateLinearSurface();
        // tag swizzled surface as not up to date (since lock impicitly modifies the linear copy)
        // but NOT if the LOCK flags indicates they don't intend to change the contents
        if(!(lpLockData->dwFlags & DDLOCK_READONLY)){
            // only mark dirty if user intends to change linear contents.
            pTexture->getSwizzled()->tagOutOfDate();

            // if this is part of a cube map/mipmap, tag the base as dirty
            //  this will trigger a proper update of all subsurfaces before HW touches them
            if (pTexture->isUserMipLevel() || pTexture->isCubeMap())
            {
                pTexture->getBaseTexture()->tagSubSurfacesDirty();
            }
        }
    }

    // now that linear surface may exist, get its heap location
    DWORD dwHeapLocation = pTexture->getLinear()->getHeapLocation();

    // setup DDRAW with surface information. Under WINNT, DirectX uses
    // the offset in fpVidMem to calculate a per process surface address.

    if (bNoLinear) // compressed or swizzled format
    {
        dwHeapLocation = pTexture->getSwizzled()->getHeapLocation();

#ifdef NV_AGP
        if( dwHeapLocation & CSimpleSurface::HEAP_AGP )
        {
            DWORD dwBaseAddress = pTexture->getBaseTexture()->getSwizzled()->getAddress();
            DWORD dwMipAddress  = pTexture->getSwizzled()->getAddress();
            lpLockData->lpSurfData = (void *)(nvAGPGetUserAddr(dwBaseAddress) + (dwMipAddress - dwBaseAddress));
        }
        else
#endif // NV_AGP
        {
            lpLockData->lpSurfData                   = (void*) pTexture->getSwizzled()->getfpVidMem();
            // we already set this in nvTextureCreateSurface
            //lpLockData->lpDDSurface->lpGbl->lPitch   = pTexture->getPitch();
        }

        lpLockData->lpDDSurface->lpGbl->fpVidMem = bNoSysLock // renaming can happen at any time - force ddraw to comply
                                                 ? 0
                                                 : (DWORD)lpLockData->lpSurfData;
    }
    else
    {
        // DX6 DDRAW BUG:
        // for reasons totally unknown to humanity the lock will return an incorrect alias to
        // the locked surface when NOSYSLOCK flag is set and lpSurfData == fpVidMem.
        // it seems that ddraw thinks that the memory did not move so the internal alias is
        // still valid. Fact of the matter is that it is wrong.
        // attempt to spoof ddraw into doing the right thing here.
        if ((global.dwDXRuntimeVersion < 0x0700) && (bNoSysLock) && (!dwOldLinear)) // 1st time created - thus we moved the surface
        {
            lpLockData->lpSurfData                   = (void*)pTexture->getLinear()->getfpVidMem();
            lpLockData->lpDDSurface->lpGbl->fpVidMem = pTexture->getSwizzled()->getfpVidMem();
            // we already set this in nvTextureCreateSurface
            //lpLockData->lpDDSurface->lpGbl->lPitch   = pTexture->getPitch();
        }
        else
        {
            lpLockData->lpSurfData                   = (void*)pTexture->getLinear()->getfpVidMem();
            lpLockData->lpDDSurface->lpGbl->fpVidMem = (DWORD)lpLockData->lpSurfData;
            // we already set this in nvTextureCreateSurface
            //lpLockData->lpDDSurface->lpGbl->lPitch   = pTexture->getPitch();
        }
    }

    // tell texture that CPU wants access
            #ifdef NV_PROFILE_CPULOCK
            NVP_START (NVP_T_TEX_LOCK);
            #endif
    DDLOCKINDEX(NVSTAT_LOCK_TEX_NORMAL);
    if (bNoLinear) pTexture->getSwizzled()->cpuLock (CSimpleSurface::LOCK_NORMAL);
              else pTexture->getLinear()->cpuLock (CSimpleSurface::LOCK_NORMAL);
            #ifdef NV_PROFILE_CPULOCK
            NVP_STOP (NVP_T_TEX_LOCK);
            nvpLogTime (NVP_T_TEX_LOCK,nvpTime[NVP_T_TEX_LOCK]);
            #endif

    // done
    lpLockData->ddRVal = DD_OK;
    dbgTracePop();

#ifdef WINNT
    /* If the user's copy of the the texture is in system memory, must handle. */
    if ((dwHeapLocation & CSimpleSurface::HEAP_ANYWHERE) && (dwHeapLocation & CSimpleSurface::HEAP_SYS)) {
        retVal = DDHAL_DRIVER_HANDLED;
    }
#endif // WINNT
    // return NOTHANDLED because for Locks the return values are reversed from what
    // would make any sense whatsoever.
    return retVal;
}

//---------------------------------------------------------------------------

// nvTextureUnlock
//
// unlocks a texture after the user partied on the texels

DWORD nvTextureUnlock
(
    CTexture           *pTexture,
    LPDDHAL_UNLOCKDATA  lpUnlockData
)
{
    dbgTracePush ("nvTextureUnlock");

    assert (pTexture);

    // unlock texture
    if (pTexture->isFourCC()
     && !pTexture->isDXT())  // swizzled format
    {
        pTexture->cpuUnlockSwz();
        // force hw fetch
        PNVD3DCONTEXT pContext = (PNVD3DCONTEXT)getDC()->dwContextListHead;
        while (pContext)
        {
            invalidateTextureCache (pContext);
            pContext = pContext->pContextNext;
        }
    }
    else
    {
        pTexture->cpuUnlockLin();
    }


    // if this texture is in use, we must set the texture stage dirty bit
    // to force invalidation of the texture cache
    CNvObject *pNvObj = pTexture->getWrapper();
    PNVD3DCONTEXT pContext = (PNVD3DCONTEXT)getDC()->dwContextListHead;
    while (pContext)
    {
        if ((pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP] == (DWORD)pNvObj) ||
            (pContext->tssState[1].dwValue[D3DTSS_TEXTUREMAP] == (DWORD)pNvObj)) {
            invalidateTextureCache (pContext);
        }
        pContext = pContext->pContextNext;
    }

    // done
    lpUnlockData->ddRVal = DD_OK;

    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

//---------------------------------------------------------------------------

// D3DCreateTextureContexts
//
// allocate the system memory texture context DMA

DWORD D3DCreateTextureContexts
(
    DWORD   dwHeapBase,
    DWORD   dwNewLimit
)
{
    DWORD   dwStatus;

    dbgTracePush ("D3DCreateTextureContexts");

    dwStatus = NVOS03_STATUS_ERROR_BAD_LIMIT;

    if (dwHeapBase && dwNewLimit) {
        dwStatus = NvRmAllocContextDma(pDriverData->dwRootHandle,
                                       D3D_CONTEXT_DMA_HOST_MEMORY,
                                       NV01_CONTEXT_DMA,
                                       DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_ONLY) |
                                       DRF_DEF(OS03, _FLAGS, _LOCKED, _ALWAYS) |
                                       DRF_DEF(OS03, _FLAGS, _COHERENCY, _CACHED),
                                       (PVOID) dwHeapBase,
                                       dwNewLimit);
    }

    // Force both triangle states to be resent.
    pDriverData->dDrawSpareSubchannelObject = 0;
    // let ddraw know we messed with the celsius object
    pDriverData->dwMostRecentHWUser = MODULE_ID_D3D;

    DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"Vid Heap: Address:%08x Size:%08x\n",pDriverData->BaseAddress,pDriverData->TotalVRAM);

    dbgTracePop();
    return (dwStatus);
}

//---------------------------------------------------------------------------

// Destroy all texture DMA contexts and free all allocated system memory for shutdown.

void D3DDestroyTextureContexts
(
    GLOBALDATA *pDriverData
)
{
    dbgTracePush ("D3DDestroyTextureContexts");
    NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle, D3D_CONTEXT_DMA_HOST_MEMORY);
    dbgTracePop();
}

//---------------------------------------------------------------------------

BOOL nvTextureDefaultAlloc
(
    void
)
{
    dbgTracePush ("nvTextureDefaultAlloc");

    // create the white texture
    nvAssert (getDC()->pDefaultTexture == NULL);

    getDC()->pDefaultTexture = new CTexture;

    if (!getDC()->pDefaultTexture) {
        DPF ("nvTextureDefaultAlloc - out of memory");
        dbgTracePop();
        return (FALSE);
    }

    if (!getDC()->pDefaultTexture->create (NULL, 1, 1, 1, 2, 2, 1, NV_SURFACE_FORMAT_A1R5G5B5,
                                           CSimpleSurface::HEAP_VID, CSimpleSurface::HEAP_VID)) {
        DPF ("nvTextureDefaultAlloc - out of memory");
        dbgTracePop();
        return (FALSE);
    }

    // fill with a white pixel
    *(WORD*) (getDC()->pDefaultTexture->getSwizzled()->getAddress()) = 0xffff;

    // create the texture used for user clip planes
    nvAssert (getDC()->pClipPlaneTexture == NULL);

    getDC()->pClipPlaneTexture = new CTexture;

    if (!getDC()->pClipPlaneTexture) {
        DPF ("nvTextureDefaultAlloc - out of memory");
        dbgTracePop();
        return(FALSE);
    }

    if (!getDC()->pClipPlaneTexture->create (NULL, 2, 2, 1, 2, 2, 1, NV_SURFACE_FORMAT_A1R5G5B5,
                                             CSimpleSurface::HEAP_VID, CSimpleSurface::HEAP_VID)) {
        DPF ("nvTextureDefaultAlloc - out of memory");
        dbgTracePop();
        return (FALSE);
    }

    // fill with [0 0; 0 1]
    WORD *pData = (WORD*) (getDC()->pClipPlaneTexture->getSwizzled()->getAddress());
    pData[0] = 0x0000;   pData[1] = 0x0000;
    pData[2] = 0x0000;   pData[3] = 0xffff;

    // init spare texture memory
    DWORD dwStatus;
    for (DWORD ii=0; ii<2; ii++) {
        NVHEAP_ALLOC (dwStatus, getDC()->dwSpareTextureAddr[ii], 256 + NV_TEXTURE_PAD, TYPE_TEXTURE);
        if (dwStatus) {
            DPF ("nvTextureDefaultAlloc - failed to allocate spare texture space");
            dbgTracePop();
            return (FALSE);
        }
        getDC()->dwSpareTextureOffset[ii] = ((getDC()->dwSpareTextureAddr[ii] + NV_TEXTURE_OFFSET_ALIGN) & ~NV_TEXTURE_OFFSET_ALIGN) - pDriverData->BaseAddress;
        pDriverData->DDrawVideoSurfaceCount ++;
    }

    dbgTracePop();
    return (TRUE);
}

//-------------------------------------------------------------------------

void nvTextureDefaultFree
(
    void
)
{
    dbgTracePush ("nvTextureDefaultFree");

    // release the NVIDIA logo
    if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_LOGOENABLE_MASK) == D3D_REG_LOGOENABLE_ENABLE) {
        nvFreeLogo();
    }

    // release the default (white) texture
    if (getDC()->pDefaultTexture) {
        getDC()->pDefaultTexture->destroy();
        delete getDC()->pDefaultTexture;
        getDC()->pDefaultTexture = NULL;
    }

    // release the texture used for user clip planes
    if (getDC()->pClipPlaneTexture) {
        getDC()->pClipPlaneTexture->destroy();
        delete getDC()->pClipPlaneTexture;
        getDC()->pClipPlaneTexture = NULL;
    }

    // free the spare texture memory
    for (DWORD ii=0; ii<2; ii++) {
        if (getDC()->dwSpareTextureAddr[ii] != 0xffffffff) {
            NVHEAP_FREE (getDC()->dwSpareTextureAddr[ii]);
            getDC()->dwSpareTextureAddr[ii] = 0;
            getDC()->dwSpareTextureOffset[ii] = 0;
            pDriverData->DDrawVideoSurfaceCount --;
        }
    }

    dbgTracePop();
}

#endif  // NVARCH >= 0x04

