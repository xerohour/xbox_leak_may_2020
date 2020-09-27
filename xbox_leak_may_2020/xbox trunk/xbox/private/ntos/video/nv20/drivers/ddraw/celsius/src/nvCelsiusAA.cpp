/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvCelsiusAA.cpp                                                   *
*   Celsius antialiasing                                                       *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal             11Jan2000       NV15 development            *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"


#if (NVARCH >= 0x10)

//////////////////////////////////////////////////////////////////////////////
// super buffer info
//
struct AAMETHODINFO
{
    float fXScale;
    float fYScale;
    DWORD dwLODBias;
    DWORD dwLinePointSize; 
};

static AAMETHODINFO g_aAAMethodInfo[] =
{
   // X     Y     bias(4.4) line/point size (6.3)
    { 1.0f, 2.0f, 0x00,          0x0C           },// 1x2
    { 2.0f, 2.0f, 0x10,          0x10           },// 2x2 lod
    { 2.0f, 2.0f, 0x00,          0x10           },// 2x2
    { 2.0f, 2.0f, 0x00,          0x10           },// 2x2 spec
    { 3.0f, 3.0f, 0x19,          0x18           },// 3x3 lod
    { 3.0f, 3.0f, 0x00,          0x18           },// 3x3
    { 4.0f, 4.0f, 0x20,          0x20           },// 4x4 lod
    { 4.0f, 4.0f, 0x00,          0x20           },// 4x4
};

// Methods starting with this one require a temporary work space for blits.
#define FIRST_TEMP_SPACE_METHOD 3

__declspec(dllexport) void nvCelsiusAAInitCompatibilitySettings();
__declspec(dllexport) void nvCelsiusAAGetCompatibilitySettings(PNVD3DCONTEXT);

extern char* nvGetCommandLine();

//////////////////////////////////////////////////////////////////////////////
// nvCelsiusAAFindContext
//
PNVD3DCONTEXT nvCelsiusAAFindContext
(
    CSimpleSurface *pSurface
)
{

    //if the surface is NULL (i.e. we tried to get a CSimpleSurface from
    //a CTexture object this will be null.  Combine that with a NULL zbuffer
    //and we'll return a context that matches.  BAD.

    if(!pSurface) return NULL;

    // AA is enabled for some context. Now we have to find out which one
    PNVD3DCONTEXT pContext = (PNVD3DCONTEXT)getDC()->dwContextListHead;
    while (pContext)
    {
        // is this the one?
        if (pContext->pZetaBuffer == pSurface)
        {
            return pContext;
        }
        if (pContext->pRenderTarget == pSurface)
        {
            return pContext;
        }

        // next
        pContext = pContext->pContextNext;
    }

    // not found
    return NULL;
}

//////////////////////////////////////////////////////////////////////////////
// nvCelsiusAACanCreate
//
BOOL nvCelsiusAACanCreate
(
    LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl
)
{
    //
    // figure out if we have enough memory to create AA buffers.
    //  AA may still fail if the memory is not available at nvCelsiusAACreate() time which
    //  is invoked when AA renderstate is set (i.e. this function may give false positives).
    //

    //
    // does registry allow it and do we have to right AA bits set
    //
    DWORD dwCaps  = pDDSLcl->ddsCaps.dwCaps;
    DWORD dwCaps2 = pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2;
    if (!(dwCaps & DDSCAPS_3DDEVICE)
     || !(dwCaps2 & DDSCAPS2_HINTANTIALIASING)
     || !(getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_ANTIALIASENABLE_MASK))
    {
        return TRUE; // since AA is not enabled, allow creation to continue
    }

    //
    // extract surface info
    //
    DWORD dwBufferWidth  = pDDSLcl->lpGbl->wWidth;
    DWORD dwBufferHeight = pDDSLcl->lpGbl->wHeight;

    DWORD dwBufferBPP    = (pDDSLcl->dwFlags & DDRAWISURF_HASPIXELFORMAT)
                         ? (pDDSLcl->lpGbl->ddpfSurface.dwRGBBitCount / 8)
                         : (GET_MODE_BPP() / 8);

    //
    // try all combinations
    //
    DWORD dwAAMethodMax = getDC()->nvD3DRegistryData.regAntiAliasQuality - 1;
    for (DWORD dwAAMethod = dwAAMethodMax; !(dwAAMethod & 0x80000000); dwAAMethod--)
    {
        //
        // set up data
        //
        float fWidthAmplifier  = g_aAAMethodInfo[dwAAMethod].fXScale;
        float fHeightAmplifier = g_aAAMethodInfo[dwAAMethod].fYScale;

        /*
        //
        // check again if we can AA this buffer
        //
        if (((dwBufferWidth  * fWidthAmplifier)  > 2048.0f)
         || ((dwBufferHeight * fHeightAmplifier) > 2048.0f))
        {
            // next
            continue;
        }
        */


        //
        // see if we can allocate all the memory required by computation
        //   we will have 2 super buffers and usually 3 normal buffers
        //
        DWORD dwMemoryRequired  = DWORD(2 * dwBufferWidth * fWidthAmplifier * dwBufferHeight * fHeightAmplifier * dwBufferBPP
                                      + 3 * dwBufferWidth *                   dwBufferHeight *                    dwBufferBPP);
              dwMemoryRequired += dwMemoryRequired / 20; // add 5% wastage

        if (dwMemoryRequired > (DWORD)pDriverData->VideoHeapTotal)
        {
            continue;
        }

        //
        // this looks like a good combination - accept
        //
        return TRUE;
    }

    //
    // none found - fail
    //
    return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// nvCelsiusAACreate
//
BOOL nvCelsiusAACreate
(
    PNVD3DCONTEXT pContext
)
{    
    dbgTracePush ("nvCelsiusAACreate");
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {   // Kelvin has it's own AA functions
        dbgTracePop();
        return FALSE;
    }

    if (pContext->aa.mAppCompat.bNoAA){
        dbgTracePop();
        return FALSE;
    }

    //
    // check if this is really a celsius machine
    //
    if (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS))
    {
        // no, clear the reg bit and bye bye
        getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_ANTIALIASENABLE_MASK;
        dbgTracePop();
        return FALSE;
    }

    //
    // check if we can AA this buffer
    //
    if (!(getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_ANTIALIASENABLE_MASK) // aa enabled
     || !pContext->pRenderTarget                        // must exist
     || pContext->pRenderTarget->isSwizzled())          // cannot be swizzled
    {
        DPF_LEVEL (NVDBG_LEVEL_ERROR,"nvCelsiusAACreate: AA failed because render target either does not exist or it is swizzled.");
        dbgTracePop();
        return FALSE;
    }

    // do not AA when rendering to texture
    if (pContext->pRenderTarget->getWrapper()->getTexture()) {
        DPF_LEVEL (NVDBG_LEVEL_ERROR,"nvCelsiusAACreate: AA failed because render target is a texture.");
        dbgTracePop();
        return FALSE;
    }

    // extract current render buffer information
    //
    DWORD           dwBufferWidth  = pContext->pRenderTarget->getWidth();
    DWORD           dwBufferHeight = pContext->pRenderTarget->getHeight();
    CSimpleSurface *pRenderTarget  = pContext->pRenderTarget;
    CSimpleSurface *pZetaBuffer    = pContext->pZetaBuffer;

    //objects created already?
    if (pContext->aa.isInit())
    {
        //check whether the target dimension changed
        //if not, save some work, keep the old AA buffer
        if ((dwBufferWidth        == pContext->aa.dwOriginalRenderTargetWidth)
         && (dwBufferHeight       == pContext->aa.dwOriginalRenderTargetHeight)
         && (pContext->dwZBHandle == pContext->aa.dwOriginalZBHandle))
        {
            dbgTracePop();
            return TRUE;
        }
        //if the dimension changed, destroy the old aa buffer and continue to build a new one
        else
            nvCelsiusAADestroy(pContext);
    }

    nvCelsiusAAInitCompatibilitySettings();
    nvCelsiusAAGetCompatibilitySettings(pContext);

    // This one is needed to catch the first pass through
    if (pContext->aa.mAppCompat.bNoAA) return FALSE;

    //initialize the original RenderTarget dimension
    pContext->aa.dwOriginalRenderTargetWidth  = dwBufferWidth;
    pContext->aa.dwOriginalRenderTargetHeight = dwBufferHeight;
    pContext->aa.dwOriginalZBHandle           = pContext->dwZBHandle;

    DWORD dwAAMethodMax;

    // attempt to find the highest quality super buffer
    if (pContext->pRenderTarget && (pContext->pRenderTarget->getMultiSampleBits() > 1)) {
        if (pContext->pRenderTarget->getMultiSampleBits() == 2) {
            dwAAMethodMax = 0;
        } else if (pContext->pRenderTarget->getMultiSampleBits() == 4) {
            dwAAMethodMax = 2;
        } else {
            DPF("Bad number of multisample bits: %d", pContext->pRenderTarget->getMultiSampleBits());
            dbgD3DError();
        }
    } else {
        dwAAMethodMax = getDC()->nvD3DRegistryData.regAntiAliasQuality - 1;
    }

    for (DWORD dwAAMethod = dwAAMethodMax; !(dwAAMethod & 0x80000000); dwAAMethod--)
    {
        //
        // set up data
        //

        //Smetimes we dont want to allow special setting
        if ((dwAAMethod  == AASTATE::METHOD_2x2spec) && !(pContext->aa.mAppCompat.bAllowSpecial))
            continue;

        // never allow us to drop down to 2x2 special.
        // Only allow it if it is explicitly requested.
        if ((dwAAMethod == AASTATE::METHOD_2x2spec) && (dwAAMethodMax > AASTATE::METHOD_2x2spec))
            continue;

        pContext->aa.fWidthAmplifier  = g_aAAMethodInfo[dwAAMethod].fXScale;
        pContext->aa.fHeightAmplifier = g_aAAMethodInfo[dwAAMethod].fYScale;
        pContext->aa.fLODBias         = float(g_aAAMethodInfo[dwAAMethod].dwLODBias) / 16.0f;
        pContext->aa.dwDepthAmplifier = 1;
        pContext->aa.dwLinePointSize  = g_aAAMethodInfo[dwAAMethod].dwLinePointSize;
        pContext->aa.modFlags (~AASTATE::MASK_METHOD,dwAAMethod);
        DPF_LEVEL (NVDBG_LEVEL_INFO,"Attempting %fx%f...",pContext->aa.fWidthAmplifier,pContext->aa.fHeightAmplifier);

        /*
        //
        // check again if we can AA this buffer
        //
        if (((dwBufferWidth  * pContext->aa.fWidthAmplifier)  > 2048.0f)
         || ((dwBufferHeight * pContext->aa.fHeightAmplifier) > 2048.0f))
        {
            // next
            DPF_LEVEL (NVDBG_LEVEL_INFO,"    ...not addressable.");
            continue;
            
        }
        */

        DWORD dwMem = (DWORD)(2*pContext->aa.fWidthAmplifier*pContext->aa.fHeightAmplifier*pRenderTarget->getBPP()*pRenderTarget->getWidth()*pRenderTarget->getHeight());
        if (dwMem > (DWORD)pDriverData->VideoHeapFree) {
            // next
            continue;
        }

        // create super frame buffer
        //
        pContext->aa.pSuperRenderTarget = new CSimpleSurface;
        if (!pContext->aa.pSuperRenderTarget)
        {
            // failed
            return FALSE;
        }

        // Ask to be created as Tiled memory in the Zbuffer area (Bank 1) -- this reduces memory bank collisions.
        DWORD dwPitch = DWORD(0.5f + pRenderTarget->getWidth() * pContext->aa.fWidthAmplifier * pRenderTarget->getBPP());
              dwPitch = (dwPitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;

        if (!pContext->aa.pSuperRenderTarget->create(NULL,
                                                     pRenderTarget->getFormat(),
                                                     DWORD(0.5f + pRenderTarget->getWidth()  * pContext->aa.fWidthAmplifier),
                                                     DWORD(0.5f + pRenderTarget->getHeight() * pContext->aa.fHeightAmplifier),
                                                     pRenderTarget->getDepth()               * pContext->aa.dwDepthAmplifier,
                                                     1,
                                                     pRenderTarget->getBPPRequested(),
                                                     pRenderTarget->getBPP(),
                                                     dwPitch,
                                                     pRenderTarget->getHeapLocation(),
                                                     pRenderTarget->getHeapLocation(),
                                                     (CSimpleSurface::ALLOCATE_TILED | CSimpleSurface::ALLOCATE_AS_ZBUFFER_UNC)
#ifdef CAPTURE
                                                    ,CAPTURE_SURFACE_KIND_TARGET
#endif
                                                     ))
        {
            // failed - clean up
            delete pContext->aa.pSuperRenderTarget;
            pContext->aa.pSuperRenderTarget = NULL;
            // next
            DPF_LEVEL (NVDBG_LEVEL_INFO,"    ...not enough frame buffer space.");
            continue;
        }

        //
        // create super z buffer
        //
        // Ask to be created as Tiled memory to force it to go on Bank 0 --- reduces memory collisions
        if (pZetaBuffer)
        {
            pContext->aa.pSuperZetaBuffer = new CSimpleSurface;
            if (!pContext->aa.pSuperZetaBuffer)
            {
                // failed
                pContext->aa.pSuperRenderTarget->destroy();
                delete pContext->aa.pSuperRenderTarget;
                pContext->aa.pSuperRenderTarget = NULL;
                return FALSE;
            }
            DWORD dwPitch = DWORD(0.5f + pZetaBuffer->getWidth() * pContext->aa.fWidthAmplifier * pZetaBuffer->getBPP());
                  dwPitch = (dwPitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
            if (!pContext->aa.pSuperZetaBuffer->create(NULL,
                                                         pZetaBuffer->getFormat(),
                                                         DWORD(0.5f + pZetaBuffer->getWidth()  * pContext->aa.fWidthAmplifier),
                                                         DWORD(0.5f + pZetaBuffer->getHeight() * pContext->aa.fHeightAmplifier),
                                                         pZetaBuffer->getDepth()               * pContext->aa.dwDepthAmplifier,
                                                         1,
                                                         pZetaBuffer->getBPPRequested(),
                                                         pZetaBuffer->getBPP(),
                                                         dwPitch,
                                                         pZetaBuffer->getHeapLocation(),
                                                         pZetaBuffer->getHeapLocation(),
                                                         CSimpleSurface::ALLOCATE_TILED
#ifdef CAPTURE
                                                        ,CAPTURE_SURFACE_KIND_ZETA
#endif
                                                         ))

            {
                // failed - clean up
                delete pContext->aa.pSuperZetaBuffer;
                pContext->aa.pSuperZetaBuffer = NULL;
                pContext->aa.pSuperRenderTarget->destroy();
                delete pContext->aa.pSuperRenderTarget;
                pContext->aa.pSuperRenderTarget = NULL;
                // next
                DPF_LEVEL (NVDBG_LEVEL_INFO,"    ...not enough z buffer space.");
                continue;
            }
        }

#ifdef  STEREO_SUPPORT
        if (STEREO_ENABLED)
        {
            pContext->aa.pSuperRenderTarget->tagNotReal();
            pContext->aa.pSuperRenderTarget->createStereo();
            pContext->aa.pSuperZetaBuffer->tagNotReal();
            pContext->aa.pSuperZetaBuffer->createStereo();
        }
#endif  //STEREO_SUPPORT

        //
        // if necessary, create blit temporary space
        //
        if (dwAAMethod >= FIRST_TEMP_SPACE_METHOD) {
            pContext->aa.pBlitTempSpace = new CSimpleSurface;
            if (!pContext->aa.pBlitTempSpace)
            {
                // failed - clean up
                if (pContext->aa.pSuperZetaBuffer) {
                    pContext->aa.pSuperZetaBuffer->destroy();
                    delete pContext->aa.pSuperZetaBuffer;
                    pContext->aa.pSuperZetaBuffer = NULL;
                }
                pContext->aa.pSuperRenderTarget->destroy();
                delete pContext->aa.pSuperRenderTarget;
                pContext->aa.pSuperRenderTarget = NULL;
                return FALSE;
            }
            DWORD dwPitch = 2 * pRenderTarget->getWidth() * pRenderTarget->getBPP();
                  dwPitch = (dwPitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
            if (!pContext->aa.pBlitTempSpace->create(NULL,
                                                     pRenderTarget->getFormat(),
                                                     pRenderTarget->getWidth()  * 2,
                                                     pRenderTarget->getHeight() * 2,
                                                     pRenderTarget->getDepth()  * pContext->aa.dwDepthAmplifier,
                                                     1,
                                                     pRenderTarget->getBPPRequested(),
                                                     pRenderTarget->getBPP(),
                                                     dwPitch,
                                                     pRenderTarget->getHeapLocation(),
                                                     pRenderTarget->getHeapLocation(),
                                                     CSimpleSurface::ALLOCATE_TILED
#ifdef CAPTURE
                                                    ,CAPTURE_SURFACE_KIND_TARGET
#endif
                                                     ))
            {
                // failed - clean up
                delete pContext->aa.pBlitTempSpace;
                pContext->aa.pBlitTempSpace = NULL;
                if (pContext->aa.pSuperZetaBuffer) {
                    pContext->aa.pSuperZetaBuffer->destroy();
                    delete pContext->aa.pSuperZetaBuffer;
                    pContext->aa.pSuperZetaBuffer = NULL;
                }
                pContext->aa.pSuperRenderTarget->destroy();
                delete pContext->aa.pSuperRenderTarget;
                pContext->aa.pSuperRenderTarget = NULL;
                // next
                DPF_LEVEL (NVDBG_LEVEL_INFO,"    ...not enough blit temporary buffer space.");
                continue;
            }
        }

        // success
        break;
    }
    if (dwAAMethod & 0x80000000)
    {
        pContext->aa.fWidthAmplifier  = 1.0;
        pContext->aa.fHeightAmplifier = 1.0;
        pContext->aa.fLODBias         = 0.0;
        pContext->aa.dwLinePointSize  = 1;
        DPF_LEVEL (NVDBG_LEVEL_ERROR,"nvCelsiusAACreate: AA failed due to lack of memory or limited drawable area.");
        dbgTracePop();
        return FALSE;
    }

    //
    // update flags
    //
    pContext->aa.modFlags (~(AASTATE::FLAG_ENABLED),
                            (AASTATE::FLAG_INIT));
    pContext->aa.bsReset();
    getDC()->dwAAContextCount ++;

    //walk the pcontext list and update anyone else's pcontext->aa data
    //if they point to the same targets here
    PNVD3DCONTEXT pTempContext = (PNVD3DCONTEXT) getDC()->dwContextListHead;
    while (pTempContext) {
        if (pTempContext != pContext && pTempContext->pRenderTarget == pContext->pRenderTarget) {
            pTempContext->aa.pSuperRenderTarget = pContext->aa.pSuperRenderTarget;
            pTempContext->aa.pBlitTempSpace = pContext->aa.pBlitTempSpace;
        }
        if (pTempContext != pContext && pTempContext->pZetaBuffer == pContext->pZetaBuffer) {
            pTempContext->aa.pSuperZetaBuffer = pContext->aa.pSuperZetaBuffer;
        }
        pTempContext = pTempContext->pContextNext;
    }


    // Make wrapper objects
    CNvObject *pWrapper;

    if (pContext->aa.pSuperRenderTarget) {
        pWrapper = new CNvObject(0);
        pWrapper->setObject(CNvObject::NVOBJ_SIMPLESURFACE, (void *)pContext->aa.pSuperRenderTarget);
        pContext->aa.pSuperRenderTarget->setWrapper(pWrapper);
    }

    if (pContext->aa.pSuperZetaBuffer) {
        pWrapper = new CNvObject(0);
        pWrapper->setObject(CNvObject::NVOBJ_SIMPLESURFACE, (void *)pContext->aa.pSuperZetaBuffer);
        pContext->aa.pSuperZetaBuffer->setWrapper(pWrapper);
    }

    pContext->aa.modFlags(~AASTATE::FLAG_SRT_VALID & ~AASTATE::FLAG_SZB_VALID, AASTATE::FLAG_RT_VALID | AASTATE::FLAG_ZB_VALID);

    // Always select super buffers
    nvCelsiusAASelectSuperBuffers(pContext);

    //
    // done
    //
    dbgTracePop();
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// nvCelsiusAADestroy
//
BOOL nvCelsiusAADestroy
(
    PNVD3DCONTEXT pContext
)
{
    dbgTracePush ("nvCelsiusAADestroy");

    //walk the pcontext list and update anyone else's pcontext->aa data
    //if they point to the same targets here

    CSimpleSurface *pRenderTarget = pContext->pRenderTarget;
    CSimpleSurface *pZetaBuffer = pContext->pZetaBuffer;

    PNVD3DCONTEXT pTempContext = pContext;
    pContext = (PNVD3DCONTEXT)getDC()->dwContextListHead;
    while (pContext) {

        //
        // do we have AA objects?
        //
        if (pContext->aa.isInit())
        {
            BOOL bDestroyed = FALSE;
            //
            // destroy objects
            //
            if (getDC()->nvPusher.getSize() > 0 &&
                getDC()->nvPusher.isValid()){
                // only do this if push buffer is valid.  This
                // additional check is due to the fact we can get
                // an nvCelsiusAADestroy calling into here AFTER
                // the pushbuffer has been destroyed on Win2K, in
                // this case you can't minify.
                nvCelsiusAASelectNormalBuffers (pContext,TRUE);
            }

            if (pContext == pTempContext) {
                CNvObject *pWrapper = pContext->aa.pSuperRenderTarget->getWrapper();
                if (pWrapper) {
                    pWrapper->release();
                } else {
                    // SRT has no wrapper object!
                    nvAssert(0);
                }
                if (pContext->aa.pSuperZetaBuffer) {
                    pWrapper = pContext->aa.pSuperZetaBuffer->getWrapper();
                    if (pWrapper) {
                        pWrapper->release();
                    } else {
                        // SZB has no wrapper object!
                        nvAssert(0);
                    }
                    bDestroyed = TRUE;
                }
                if (pContext->aa.pBlitTempSpace) {
                    pContext->aa.pBlitTempSpace->destroy();
                    delete pContext->aa.pBlitTempSpace;
                    bDestroyed = TRUE;
                }
            }

            if (pContext->pRenderTarget == pRenderTarget) {
                pContext->aa.pSuperRenderTarget = NULL;
                pContext->aa.pBlitTempSpace = NULL;
                bDestroyed = TRUE;
            }
            if (pContext->pZetaBuffer == pZetaBuffer) {
                pContext->aa.pSuperZetaBuffer = NULL;
                bDestroyed = TRUE;
            }
            if (bDestroyed) {
                //
                // update flags
                //
                pContext->aa.modFlags (~(AASTATE::FLAG_INIT | AASTATE::FLAG_ENABLED),0);
                if(getDC()->dwAAContextCount > 0)
                    getDC()->dwAAContextCount --;
            }
        }
        pContext = pContext->pContextNext;
    }

    dbgTracePop();
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// nvCelsiusAASelectSuperBuffers
//
void nvCelsiusAASelectSuperBuffers
(
    PNVD3DCONTEXT pContext
)
{
    dbgTracePush ("nvCelsiusAASelectSuperBuffers");
/*
    if (!pContext->aa.isSuperCurrent())
    {
        //
        // update state
        //
        pContext->aa.setFlags (AASTATE::FLAG_SUPERCURRENT);
    }
*/
    //
    // done
    //
    dbgTracePop();
}

//////////////////////////////////////////////////////////////////////////////
// nvCelsiusAASelectNormalBuffers
//
void nvCelsiusAASelectNormalBuffers
(
    PNVD3DCONTEXT pContext,
    BOOL          bPreserveBufferContents
)
{
    dbgTracePush ("nvCelsiusAASelectNormalBuffers");
/*
    if (pContext->aa.isSuperCurrent())
    {
        //
        // minify buffer if it has valuable information
        //
        if (bPreserveBufferContents)
        {

            nvCelsiusAAMinify (pContext);
        }

        //
        // update state
        //
        pContext->aa.modFlags (~AASTATE::FLAG_SUPERCURRENT,0);
    }
*/
    //
    // done
    //
    dbgTracePop();
}

//////////////////////////////////////////////////////////////////////////////
// nvCelsiusAAScaledBltHW
//
inline void nvCelsiusAAScaledBltHW
(
    PNVD3DCONTEXT pContext,

    DWORD         dwSrcOffset,
    DWORD         dwSrcPitch,
    DWORD         dwSrcPoint,       // 12.4:12.4
    DWORD         dwSrcSize,        // 16:16                2 <= x <= 2046

    DWORD         dwDstOffset,
    DWORD         dwDstPitch,
    DWORD         dwDstPoint,       // 16:16
    DWORD         dwDstSize,        // 16:16

    DWORD         dwDsDx,           // 12.20
    DWORD         dwDtDy,           // 12.20

    DWORD         dwFormat,
    BOOL          bFOH,
    BOOL          bCenter,
    BOOL          bDither
)
{
    dbgTracePush ("nvCelsiusAAScaledBltHW");

    assert (~nv062SurfaceFormat[dwFormat]);
    assert (~nv089SurfaceFormat[dwFormat]);

    dwSrcSize += 0x00010001;
    dwSrcSize &= 0xfffefffe;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_ROP) | SET_ROP_OFFSET | 0x40000);
    nvPushData(1, (SRCCOPY >> 16));
    nvPusherAdjust(2);
    pDriverData->bltData.dwLastRop = (SRCCOPY >> 16);

    /*
    nvPushData ( 0, dDrawSubchannelOffset(NV_DD_SURFACES) | NV062_SET_CONTEXT_DMA_IMAGE_SOURCE | 0x080000);
    nvPushData ( 1, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY); // SetContextDMAImageSource
    nvPushData ( 2, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY); // SetContextDMAImageDestin
    nvPusherAdjust (3);
    */


    nvPushData ( 0, dDrawSubchannelOffset(NV_DD_SURFACES) | SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x100000);
    nvPushData ( 1, nv062SurfaceFormat[dwFormat]);    // SetColorFormat
    nvPushData ( 2, (dwDstPitch << 16) | dwSrcPitch); // SetPitch
    nvPushData ( 3, dwSrcOffset);                     // SetOffset
    nvPushData ( 4, dwDstOffset);                     // SetOffset
    nvPusherAdjust (5);
    pDriverData->bltData.dwLastColourFormat = nv062SurfaceFormat[dwFormat];
    pDriverData->bltData.dwLastCombinedPitch = (dwDstPitch << 16) | dwSrcPitch;
    pDriverData->bltData.dwLastSrcOffset = dwSrcOffset;
    pDriverData->bltData.dwLastDstOffset = dwDstOffset;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
    nvPushData(1, NV_DD_CONTEXT_COLOR_KEY);
    nvPushData(2, dDrawSubchannelOffset(NV_DD_SPARE) | SET_TRANSCOLOR_OFFSET | 0x40000);
    nvPushData(3, 0);
    nvPusherAdjust(4);
    pDriverData->bltData.dwLastColourKey = 0xFFFFFFFF;
    pDriverData->dDrawSpareSubchannelObject = NV_DD_CONTEXT_COLOR_KEY;


    nvPushData ( 0, dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000);
    nvPushData ( 1, NV_DD_SCALED_IMAGE_IID);
    nvPushData ( 2, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_CONTEXT_DMA_IMAGE_OFFSET | 0x40000);
    nvPushData ( 3, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY); // SetContextDmaImage
    nvPushData ( 4, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_SET_COLOR_FORMAT | 0x40000);
    nvPushData ( 5, nv089SurfaceFormat[dwFormat]); // SetColorFormat
    nvPushData ( 6, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_CLIPPOINT_OFFSET | 0x180000);
    nvPushData ( 7, dwDstPoint);             // ClipPoint
    nvPushData ( 8, dwDstSize);              // ClipSize
    nvPushData ( 9, dwDstPoint);             // ImageOutPoint
    nvPushData (10, dwDstSize);              // ImageOutSize
    nvPushData (11, dwDsDx);                 // DsDx
    nvPushData (12, dwDtDy);                 // DtDy
    nvPushData (13, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_IMAGE_IN_SIZE_OFFSET | 0x100000);
    nvPushData (14, dwSrcSize);              // ImageInSize
    nvPushData (15, (bFOH ? (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_FOH << 24)
                          : (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_ZOH << 24)) |     // ImageInFormat
                    (bCenter ? (NV077_IMAGE_IN_FORMAT_ORIGIN_CENTER << 16)
                             : (NV077_IMAGE_IN_FORMAT_ORIGIN_CORNER << 16)) |
                    dwSrcPitch);
    nvPushData (16, dwSrcOffset);            // ImageInOffset
    nvPushData (17, dwSrcPoint);             // ImageInPoint

    if (bDither) {
        nvPushData (18, dDrawSubchannelOffset(NV_DD_STRETCH) | NV089_SET_COLOR_CONVERSION | 0x40000);
        nvPushData (19, NV089_SET_COLOR_CONVERSION_DITHER); // SetColorFormat
        nvPusherAdjust (20);
    } else {
        nvPushData (18, dDrawSubchannelOffset(NV_DD_STRETCH) | NV089_SET_COLOR_CONVERSION | 0x40000);
        nvPushData (19, NV089_SET_COLOR_CONVERSION_TRUNCATE); // SetColorFormat
        nvPusherAdjust (20);
//        nvPusherAdjust (18);
    }

    nvPusherStart (TRUE);

    pDriverData->TwoDRenderingOccurred   = TRUE;
    pDriverData->ThreeDRenderingOccurred = 0;

    dbgTracePop();
}

//////////////////////////////////////////////////////////////////////////////
// nvCelsiusAAScaledBlt
//
void nvCelsiusAAScaledBlt
(
    PNVD3DCONTEXT pContext,

    DWORD         dwSrcOffset,
    DWORD         dwSrcPitch,
    DWORD         dwSrcX,           // 12.4
    DWORD         dwSrcY,           // 16
    DWORD         dwSrcWidth,
    DWORD         dwSrcHeight,

    DWORD         dwDstOffset,
    DWORD         dwDstPitch,
    DWORD         dwDstX,           // 16
    DWORD         dwDstY,           // 16
    DWORD         dwDstWidth,
    DWORD         dwDstHeight,

    DWORD         dwDsDx,           // 12.20     assume integer scaling
    DWORD         dwDtDy,           // 12.20

    DWORD         dwFormat,
    BOOL          bFOH,
    BOOL          bCenter,
    BOOL          bDither
)
{
    dbgTracePush ("nvCelsiusAAScaledBlt");

    // limit width
    if (dwSrcWidth > 2046)
    {
        DWORD dwDstSplit = 1024 / (dwDsDx >> 20);
        DWORD dwSrcSplit = dwDstSplit * (dwDsDx >> 20);

        nvCelsiusAAScaledBlt (pContext,
                              dwSrcOffset,dwSrcPitch,dwSrcX,dwSrcY,dwSrcSplit,dwSrcHeight,
                              dwDstOffset,dwDstPitch,dwDstX,dwDstY,dwDstSplit,dwDstHeight,
                              dwDsDx,dwDtDy,dwFormat,bFOH,bCenter,bDither);

        nvCelsiusAAScaledBlt (pContext,
                              dwSrcOffset,dwSrcPitch,dwSrcX + (dwSrcSplit << 4),dwSrcY,dwSrcWidth - dwSrcSplit,dwSrcHeight,
                              dwDstOffset,dwDstPitch,dwDstX + dwDstSplit,dwDstY,dwDstWidth - dwDstSplit,dwDstHeight,
                              dwDsDx,dwDtDy,dwFormat,bFOH,bCenter,bDither);

        return;
    }

    // limit height
    if (dwSrcHeight > 2047)
    {
        DWORD dwDstSplit = 1024 / (dwDtDy >> 20);
        DWORD dwSrcSplit = dwDstSplit * (dwDtDy >> 20);

        nvCelsiusAAScaledBlt (pContext,
                              dwSrcOffset,dwSrcPitch,dwSrcX,dwSrcY,dwSrcWidth,dwSrcSplit,
                              dwDstOffset,dwDstPitch,dwDstX,dwDstY,dwDstWidth,dwDstSplit,
                              dwDsDx,dwDtDy,dwFormat,bFOH,bCenter,bDither);

        nvCelsiusAAScaledBlt (pContext,
                              dwSrcOffset,dwSrcPitch,dwSrcX,dwSrcY + (dwSrcSplit << 4),dwSrcWidth,dwSrcHeight - dwSrcSplit,
                              dwDstOffset,dwDstPitch,dwDstX,dwDstY + dwDstSplit,dwDstWidth,dwDstHeight - dwDstSplit,
                              dwDsDx,dwDtDy,dwFormat,bFOH,bCenter,bDither);

        return;
    }

    // Lie about format for Z buffers
    switch (dwFormat) {
    case NV_SURFACE_FORMAT_Z16:
        dwFormat = NV_SURFACE_FORMAT_R5G6B5;
        break;
    case NV_SURFACE_FORMAT_Z24S8:
    case NV_SURFACE_FORMAT_Z24X8:
        dwFormat = NV_SURFACE_FORMAT_A8R8G8B8;
        break;
    }

    // attempt to minimize the subrect offset (nvCelsius is not too kosher with large subrects)
    DWORD dwBPP = nvSurfaceFormatToBPP[dwFormat];

    dwSrcOffset += dwBPP * (dwSrcX >> 4) + dwSrcPitch * (dwSrcY >> 4);
    dwSrcX      &= 0xf;
    dwSrcY      &= 0xf;
    if (dwSrcOffset & NV_BYTE_ALIGNMENT_PAD)
    {
        dwSrcX      += ((dwSrcOffset & NV_BYTE_ALIGNMENT_PAD) / dwBPP) << 4;
        dwSrcOffset &= ~NV_BYTE_ALIGNMENT_PAD;
    }

    dwDstOffset += dwBPP * dwDstX + dwDstPitch * dwDstY;;
    dwDstX       = 0;
    dwDstY       = 0;
    if (dwDstOffset & NV_BYTE_ALIGNMENT_PAD)
    {
        dwDstX      += (dwDstOffset & NV_BYTE_ALIGNMENT_PAD) / dwBPP;
        dwDstOffset &= ~NV_BYTE_ALIGNMENT_PAD;
    }

    // send to hardware
    nvCelsiusAAScaledBltHW (pContext,
                            dwSrcOffset,dwSrcPitch,(dwSrcY << 16) | dwSrcX,(dwSrcHeight << 16) | dwSrcWidth,
                            dwDstOffset,dwDstPitch,(dwDstY << 16) | dwDstX,(dwDstHeight << 16) | dwDstWidth,
                            dwDsDx,dwDtDy,dwFormat,bFOH,bCenter,bDither);

    dbgTracePop();
}


//////////////////////////////////////////////////////////////////////////////
// nvCelsiusAAMagnify
//
static void nvCelsiusAAMagnifyInternal
(
    PNVD3DCONTEXT pContext,
    CSimpleSurface *pSrc,
    CSimpleSurface *pDst
)
{
    dbgTracePush ("nvCelsiusAAMagnifyInternal");

    switch (pContext->aa.getMethod())
    {
        case AASTATE::METHOD_1x2:
        {
            nvCelsiusAAScaledBlt (pContext,

                                  pSrc->getOffset(),
                                  pSrc->getPitch(),
                                  8,
                                  4,
                                  pSrc->getWidth(),
                                  pSrc->getHeight(),

                                  pDst->getOffset(),
                                  pDst->getPitch(),
                                  0,
                                  0,
                                  pDst->getWidth(),
                                  pDst->getHeight(),

                                  0x00100000,
                                  0x00080000,

                                  pSrc->getFormat(),
                                  FALSE,
                                  FALSE,
                                  FALSE);
            break;
        }
        case AASTATE::METHOD_2x2lod:
        case AASTATE::METHOD_2x2:
        {
            nvCelsiusAAScaledBlt (pContext,

                                  pSrc->getOffset(),
                                  pSrc->getPitch(),
                                  4,
                                  4,
                                  pSrc->getWidth(),
                                  pSrc->getHeight(),

                                  pDst->getOffset(),
                                  pDst->getPitch(),
                                  0,
                                  0,
                                  pDst->getWidth(),
                                  pDst->getHeight(),

                                  0x00080000,
                                  0x00080000,

                                  pSrc->getFormat(),
                                  FALSE,
                                  FALSE,
                                  FALSE);
            break;
        }
        
        case AASTATE::METHOD_2x2spec:
        {
           //change 2x2 special into 2x2
           pContext->aa.modFlags(~1,0); 
           
           nvCelsiusAAScaledBlt  (pContext,
                                  pSrc->getOffset(),
                                  pSrc->getPitch(),
                                  4,
                                  4,
                                  pSrc->getWidth(),
                                  pSrc->getHeight(),

                                  pDst->getOffset(),
                                  pDst->getPitch(),
                                  0,
                                  0,
                                  pDst->getWidth(),
                                  pDst->getHeight(),

                                  0x00080000,
                                  0x00080000,

                                  pSrc->getFormat(),
                                  FALSE,
                                  FALSE,
                                  FALSE);
            break;
        }

        case AASTATE::METHOD_3x3lod:
        case AASTATE::METHOD_3x3:
        {
            nvCelsiusAAScaledBlt (pContext,

                                  pSrc->getOffset(),
                                  pSrc->getPitch(),
                                  3,
                                  3,
                                  pSrc->getWidth(),
                                  pSrc->getHeight(),

                                  pDst->getOffset(),
                                  pDst->getPitch(),
                                  0,
                                  0,
                                  pDst->getWidth(),
                                  pDst->getHeight(),

                                  0x00055555,
                                  0x00055555,

                                  pSrc->getFormat(),
                                  FALSE,
                                  FALSE,
                                  FALSE);
            break;
        }
        case AASTATE::METHOD_4x4lod:
        case AASTATE::METHOD_4x4:
        {
            nvCelsiusAAScaledBlt (pContext,

                                  pSrc->getOffset(),
                                  pSrc->getPitch(),
                                  2,
                                  2,
                                  pSrc->getWidth(),
                                  pSrc->getHeight(),

                                  pDst->getOffset(),
                                  pDst->getPitch(),
                                  0,
                                  0,
                                  pDst->getWidth(),
                                  pDst->getHeight(),

                                  0x00040000,
                                  0x00040000,

                                  pSrc->getFormat(),
                                  FALSE,
                                  FALSE,
                                  FALSE);
            break;
        }
    }
    dbgTracePop();
}

//////////////////////////////////////////////////////////////////////////////
// nvCelsiusAAMinifyInternal
//
static void nvCelsiusAAMinifyInternal
(
    PNVD3DCONTEXT pContext,
    BOOL bFOH,
    CSimpleSurface *pSrc,
    CSimpleSurface *pDst
)
{
    dbgTracePush ("nvCelsiusAAMinifyInternal");

    // Note: some of the source width/height parameters have 2
    // subtracted from them.  Some apps that were not designed for AA
    // have artifacts on the edge if we do not clamp the bottom and right
    // edges.

    switch (pContext->aa.getMethod())
    {
        case AASTATE::METHOD_1x2:
        {
            nvCelsiusAAScaledBlt (pContext,

#ifdef  STEREO_SUPPORT
                                  GetStereoOffset(pSrc),
#else   //STEREO_SUPPORT==0
                                  pSrc->getOffset(),
#endif  //STEREO_SUPPORT
                                  pSrc->getPitch(),
                                  0,
                                  bFOH ? 8 : 0,
                                  pSrc->getWidth(),
                                  pSrc->getHeight()-2,

#ifdef  STEREO_SUPPORT
                                  GetStereoOffset(pDst),
#else   //STEREO_SUPPORT==0
                                  pDst->getOffset(),
#endif  //STEREO_SUPPORT
                                  pDst->getPitch(),
                                  0,
                                  0,
                                  pDst->getWidth(),
                                  pDst->getHeight(),

                                  0x00100000,
                                  0x00200000,

                                  pDst->getFormat(),
                                  bFOH,
                                  TRUE,
                                  pContext->aa.mAppCompat.bDither);
            break;
        }
        case AASTATE::METHOD_2x2lod:
        case AASTATE::METHOD_2x2:
        {
            nvCelsiusAAScaledBlt (pContext,

#ifdef  STEREO_SUPPORT
                                  GetStereoOffset(pSrc),
#else   //STEREO_SUPPORT==0
                                  pSrc->getOffset(),
#endif  //STEREO_SUPPORT
                                  pSrc->getPitch(),
                                  bFOH ? 8 : 0,
                                  bFOH ? 8 : 0,
                                  pSrc->getWidth()-2,
                                  pSrc->getHeight()-2,

#ifdef  STEREO_SUPPORT
                                  GetStereoOffset(pDst),
#else   //STEREO_SUPPORT==0
                                  pDst->getOffset(),
#endif  //STEREO_SUPPORT
                                  pDst->getPitch(),
                                  0,
                                  0,
                                  pDst->getWidth(),
                                  pDst->getHeight(),

                                  0x00200000,
                                  0x00200000,

                                  pDst->getFormat(),
                                  bFOH,
                                  TRUE,
                                  pContext->aa.mAppCompat.bDither);
            break;
        }
        case AASTATE::METHOD_2x2spec:
        {
            if (bFOH) {
                nvCelsiusAAScaledBlt (pContext,

#ifdef  STEREO_SUPPORT
                                      GetStereoOffset(pSrc),
#else   //STEREO_SUPPORT==0
                                      pSrc->getOffset(),
#endif  //STEREO_SUPPORT
                                      pSrc->getPitch(),
                                      8,
                                      8,
                                      pSrc->getWidth()-2,
                                      pSrc->getHeight()-2,

                                      pContext->aa.pBlitTempSpace->getOffset(),
                                      pContext->aa.pBlitTempSpace->getPitch(),
                                      0,
                                      0,
                                      pSrc->getWidth(),
                                      pSrc->getHeight(),

                                      0x00100000,
                                      0x00100000,

                                      pDst->getFormat(),
                                      bFOH,
                                      TRUE,
                                      FALSE);

                nvCelsiusAAScaledBlt (pContext,

                                      pContext->aa.pBlitTempSpace->getOffset(),
                                      pContext->aa.pBlitTempSpace->getPitch(),
                                      8,
                                      8,
                                      pSrc->getWidth(),
                                      pSrc->getHeight(),

#ifdef  STEREO_SUPPORT
                                      GetStereoOffset(pDst),
#else   //STEREO_SUPPORT==0
                                      pDst->getOffset(),
#endif  //STEREO_SUPPORT
                                      pDst->getPitch(),
                                      0,
                                      0,
                                      pDst->getWidth(),
                                      pDst->getHeight(),

                                      0x00200000,
                                      0x00200000,

                                      pDst->getFormat(),
                                      bFOH,
                                      TRUE,
                                      FALSE);
            }
            else {
                nvCelsiusAAScaledBlt (pContext,

#ifdef  STEREO_SUPPORT
                                      GetStereoOffset(pSrc),
#else   //STEREO_SUPPORT==0
                                      pSrc->getOffset(),
#endif  //STEREO_SUPPORT
                                      pSrc->getPitch(),
                                      0,
                                      0,
                                      pSrc->getWidth()-2,
                                      pSrc->getHeight()-2,

#ifdef  STEREO_SUPPORT
                                      GetStereoOffset(pDst),
#else   //STEREO_SUPPORT==0
                                      pDst->getOffset(),
#endif  //STEREO_SUPPORT
                                      pDst->getPitch(),
                                      0,
                                      0,
                                      pDst->getWidth(),
                                      pDst->getHeight(),

                                      0x00200000,
                                      0x00200000,

                                      pDst->getFormat(),
                                      bFOH,
                                      TRUE,
                                      FALSE);
            }
            break;
        }
        case AASTATE::METHOD_3x3lod:
        case AASTATE::METHOD_3x3:
        {
            if (bFOH) {
                nvCelsiusAAScaledBlt (pContext,

#ifdef  STEREO_SUPPORT
                                      GetStereoOffset(pSrc),
#else   //STEREO_SUPPORT==0
                                      pSrc->getOffset(),
#endif  //STEREO_SUPPORT
                                      pSrc->getPitch(),
                                      4,
                                      4,
                                      pSrc->getWidth()-2,
                                      pSrc->getHeight()-2,

                                      pContext->aa.pBlitTempSpace->getOffset(),
                                      pContext->aa.pBlitTempSpace->getPitch(),
                                      0,
                                      0,
                                      (pSrc->getWidth() * 2) / 3,
                                      (pSrc->getHeight() * 2) / 3,

                                      0x00180000,
                                      0x00180000,

                                      pDst->getFormat(),
                                      bFOH,
                                      TRUE,
                                      FALSE);

                nvCelsiusAAScaledBlt (pContext,

                                      pContext->aa.pBlitTempSpace->getOffset(),
                                      pContext->aa.pBlitTempSpace->getPitch(),
                                      8,
                                      8,
                                      (pSrc->getWidth() * 2) / 3,
                                      (pSrc->getHeight() * 2) / 3,

#ifdef  STEREO_SUPPORT
                                      GetStereoOffset(pDst),
#else   //STEREO_SUPPORT==0
                                      pDst->getOffset(),
#endif  //STEREO_SUPPORT
                                      pDst->getPitch(),
                                      0,
                                      0,
                                      pDst->getWidth(),
                                      pDst->getHeight(),

                                      0x00200000,
                                      0x00200000,

                                      pDst->getFormat(),
                                      bFOH,
                                      TRUE,
                                      FALSE);
            }
            else {
                nvCelsiusAAScaledBlt (pContext,

#ifdef  STEREO_SUPPORT
                                      GetStereoOffset(pSrc),
#else   //STEREO_SUPPORT==0
                                      pSrc->getOffset(),
#endif  //STEREO_SUPPORT
                                      pSrc->getPitch(),
                                      0,
                                      0,
                                      pSrc->getWidth()-2,
                                      pSrc->getHeight()-2,

#ifdef  STEREO_SUPPORT
                                      GetStereoOffset(pDst),
#else   //STEREO_SUPPORT==0
                                      pDst->getOffset(),
#endif  //STEREO_SUPPORT
                                      pDst->getPitch(),
                                      0,
                                      0,
                                      pDst->getWidth(),
                                      pDst->getHeight(),

                                      0x00300000,
                                      0x00300000,

                                      pDst->getFormat(),
                                      bFOH,
                                      TRUE,
                                      FALSE);
            }
            break;
        }
        case AASTATE::METHOD_4x4lod:
        case AASTATE::METHOD_4x4:
        {
            if (bFOH) {
                nvCelsiusAAScaledBlt (pContext,

#ifdef  STEREO_SUPPORT
                                      GetStereoOffset(pSrc),
#else   //STEREO_SUPPORT==0
                                      pSrc->getOffset(),
#endif  //STEREO_SUPPORT
                                      pSrc->getPitch(),
                                      8,
                                      8,
                                      pSrc->getWidth()-2,
                                      pSrc->getHeight()-2,

                                      pContext->aa.pBlitTempSpace->getOffset(),
                                      pContext->aa.pBlitTempSpace->getPitch(),
                                      0,
                                      0,
                                      pSrc->getWidth() / 2,
                                      pSrc->getHeight() / 2,

                                      0x00200000,
                                      0x00200000,

                                      pDst->getFormat(),
                                      bFOH,
                                      TRUE,
                                      pContext->aa.mAppCompat.bDither);

                nvCelsiusAAScaledBlt (pContext,

                                      pContext->aa.pBlitTempSpace->getOffset(),
                                      pContext->aa.pBlitTempSpace->getPitch(),
                                      8,
                                      8,
                                      pSrc->getWidth() / 2,
                                      pSrc->getHeight() / 2,

#ifdef  STEREO_SUPPORT
                                      GetStereoOffset(pDst),
#else   //STEREO_SUPPORT==0
                                      pDst->getOffset(),
#endif  //STEREO_SUPPORT
                                      pDst->getPitch(),
                                      0,
                                      0,
                                      pDst->getWidth(),
                                      pDst->getHeight(),

                                      0x00200000,
                                      0x00200000,

                                      pDst->getFormat(),
                                      bFOH,
                                      TRUE,
                                      pContext->aa.mAppCompat.bDither);
            }
            else {
                nvCelsiusAAScaledBlt (pContext,

#ifdef  STEREO_SUPPORT
                                      GetStereoOffset(pSrc),
#else   //STEREO_SUPPORT==0
                                      pSrc->getOffset(),
#endif  //STEREO_SUPPORT
                                      pSrc->getPitch(),
                                      0,
                                      0,
                                      pSrc->getWidth()-2,
                                      pSrc->getHeight()-2,

#ifdef  STEREO_SUPPORT
                                      GetStereoOffset(pDst),
#else   //STEREO_SUPPORT==0
                                      pDst->getOffset(),
#endif  //STEREO_SUPPORT
                                      pDst->getPitch(),
                                      0,
                                      0,
                                      pDst->getWidth(),
                                      pDst->getHeight(),

                                      0x00400000,
                                      0x00400000,

                                      pDst->getFormat(),
                                      bFOH,
                                      TRUE,
                                      pContext->aa.mAppCompat.bDither);
            }
            break;
        }
        default:
        {
            DPF ("INVALID AA METHOD - impossible condition");
            dbgD3DError();
            break;
        }
    }

    dbgTracePop();
}

//////////////////////////////////////////////////////////////////////////////
// nvCelsiusAAMagnify
//
void nvCelsiusAAMagnify
(
    PNVD3DCONTEXT pContext
)
{
    dbgTracePush ("nvCelsiusAAMagnify");
    nvCelsiusAAMagnifyInternal(pContext,
                               pContext->pRenderTarget,
                               pContext->aa.pSuperRenderTarget);
    dbgTracePop();
}

//////////////////////////////////////////////////////////////////////////////
// nvCelsiusAAMinify
//
void nvCelsiusAAMinify
(
    PNVD3DCONTEXT pContext
)
{
    dbgTracePush ("nvCelsiusAAMinify");

#ifdef  STEREO_SUPPORT
    if (STEREO_ACTIVATED)
    {
		//We can't mess up the pStereoData->dwLastEye or use SetupStereoContext
		//for that matter. Those are higher level primitives.
		DWORD dwLastEye = pStereoData->dwLastEye;
		pStereoData->dwLastEye = EYE_LEFT;
        nvCelsiusAAMinifyInternal(pContext,
                                  TRUE, // filter colors
                                  pContext->aa.pSuperRenderTarget,
                                  pContext->pRenderTarget);
        pStereoData->dwLastEye = EYE_RIGHT;
        nvCelsiusAAMinifyInternal(pContext,
                                  TRUE, // filter colors
                                  pContext->aa.pSuperRenderTarget,
                                  pContext->pRenderTarget);
		pStereoData->dwLastEye = dwLastEye;
		pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_SURFACE;
    } else
#endif  //STEREO_SUPPORT
        nvCelsiusAAMinifyInternal(pContext,
                                  TRUE, // filter colors
                                  pContext->aa.pSuperRenderTarget,
                                  pContext->pRenderTarget);

    dbgTracePop();
}

//////////////////////////////////////////////////////////////////////////////
// nvCelsiusAAMagnifyZ
//
void nvCelsiusAAMagnifyZ
(
    PNVD3DCONTEXT pContext
)
{
    dbgTracePush ("nvCelsiusAAMagnifyZ");
    if (pContext->pZetaBuffer && pContext->aa.pSuperZetaBuffer) {
        nvCelsiusAAMagnifyInternal(pContext,
                                 pContext->pZetaBuffer,
                                 pContext->aa.pSuperZetaBuffer);
    }
    dbgTracePop();
}

//////////////////////////////////////////////////////////////////////////////
// nvCelsiusAAMinifyZ
//
void nvCelsiusAAMinifyZ
(
    PNVD3DCONTEXT pContext
)
{
    dbgTracePush ("nvCelsiusAAMinifyZ");

    if (pContext->pZetaBuffer && pContext->aa.pSuperZetaBuffer) {
        nvCelsiusAAMinifyInternal(pContext,
                                  FALSE, // never filter Z's
                                  pContext->aa.pSuperZetaBuffer,
                                  pContext->pZetaBuffer);
    }

    dbgTracePop();
}



#ifdef WINNT
    // No way on God's green earth we should need more than 32k.
    #define MAX_AAFILE_SIZE 32768
#endif
#ifndef WINNT
    #define DYNAMIC_AA_COMPAT_SETTINGS
#endif



// Dynamically loaded settings

AAAppCompatState *AAAppSettings = 0;

struct AANameValue {
    char *lpName;
    BOOL bValue;
};

AANameValue AANamesToValues[] = {
    {"DISCARD_FB",               TRUE},
    {"DONT_DISCARD_FB",          FALSE},
    {"MAGNIFY_ON_UNLOCK",        TRUE},
    {"DONT_MAGNIFY_ON_UNLOCK",   FALSE},
    {"MINIFY_ON_ENDSCENE",       TRUE},
    {"DONT_MINIFY_ON_ENDSCENE",  FALSE},
    {"DITHER",                   TRUE},
    {"DONT_DITHER",              FALSE},
    {"NO_AA_ON",                 TRUE},
    {"AA_ON",                    FALSE},
    {"BLIT_SYNC",                TRUE},
    {"NO_BLIT_SYNC",             FALSE},
    {"DISCARD_SRT",              TRUE},
    {"DONT_DISCARD_SRT",         FALSE},
    {"ALLOW_SPECIAL",            TRUE},
    {"DONT_ALLOW_SPECIAL",       FALSE},
    {"FULL_PATH",                TRUE},
    {"NO_PATH",                  FALSE},
    {0,                          FALSE}
};


// Compiled-in settings

#define DISCARD_FB               TRUE
#define DONT_DISCARD_FB          FALSE
#define MAGNIFY_ON_UNLOCK        TRUE
#define DONT_MAGNIFY_ON_UNLOCK   FALSE
#define MINIFY_ON_ENDSCENE       TRUE
#define DONT_MINIFY_ON_ENDSCENE  FALSE
#define DITHER                   TRUE
#define DONT_DITHER              FALSE
#define NO_AA_ON                 TRUE
#define AA_ON                    FALSE
#define BLIT_SYNC                TRUE
#define NO_BLIT_SYNC             FALSE
#define DISCARD_SRT              TRUE
#define DONT_DISCARD_SRT         FALSE
#define ALLOW_SPECIAL            TRUE
#define DONT_ALLOW_SPECIAL       FALSE
#define FULL_PATH                TRUE
#define NO_PATH                  FALSE

// The macros in nvCelsiusAACompatibility.h get expanded out to array entries
#define appcompat(app, path, no_aa, discard_fb, magnify_on_unlock, minify_on_endscene, dither, blitsync, discard_srt, special) \
    {app, path, no_aa, discard_fb, magnify_on_unlock, minify_on_endscene, dither, blitsync, discard_srt, special},


AAAppCompatState AAAppSettingsCompiled[] = {
#include "nvCelsiusAACompatibility.h"
    {0, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE}
};


////////////////////////////////////
// Gets the unadorned (no path, no params) name 
// of the calling exe file.  Won't work in 
// Win2k until we figure out a better way than
// accessing the offset directly.

void GetExecutableName(LPTSTR pOut, DWORD dwSize, PNVD3DCONTEXT pContext) {

#ifndef WINNT
    LPTSTR pCmdLine = GetCommandLine();
#else
    LPTSTR pCmdLine = nvGetCommandLine(); 
#endif

    LPTSTR pStr, pExe;
    BOOL bInQuotes = 0;

    // quick check
    if (*pCmdLine == '\"') {
        pCmdLine++;
        bInQuotes = TRUE;
    }

    // strip path and leave the raw executable name
    for (pExe = pCmdLine; *pCmdLine != '\0'; pCmdLine++) {
        if (*pCmdLine == '"') bInQuotes = !bInQuotes;

        if (!bInQuotes && *pCmdLine <= ' ') {
            break;
        }
        if (*pCmdLine == '\\' || *pCmdLine == ':') {
            pExe = pCmdLine + 1;
        }
    }

    for (int i=0; pExe[i] != '\0'; i++) {
        if (pExe[i] >= 'A' && pExe[i] <= 'Z') pExe[i] += 'a' - 'A';
    }

    strncpy(pOut, pExe, dwSize);

    // early string termination if quote, space, or control character is detected
    for (pStr = pOut; *pStr != '\0'; pStr++) {
        if (*pStr == '\"' || *pStr <= ' ') {
            *pStr = '\0';
        }
    }

    pOut[dwSize-1] = '\0';       // Just to be safe
}


// Get Full name, including path
void GetFullExecutableName(LPTSTR pOut, DWORD dwSize, PNVD3DCONTEXT pContext) {

#ifndef WINNT
    LPTSTR pCmdLine = GetCommandLine();
#else
    LPTSTR pCmdLine = nvGetCommandLine(); 
#endif

    LPTSTR pExe;
    BOOL bInQuotes = 0;

    if(pCmdLine[0] != '\0'){
        pExe = pCmdLine+1;
        pExe[strlen(pExe)] = '\0';
    }
    else{
        pExe = pCmdLine;
    }   

    for (int i=0; pExe[i] != '\0'; i++) {
        if (pExe[i] >= 'A' && pExe[i] <= 'Z') pExe[i] += 'a' - 'A';
    }

    strncpy(pOut, pExe, dwSize);
    pOut[dwSize-1] = '\0';       // Just to be safe
}


#define WORD_CHAR(x) ((((x) >= 'a') && ((x) <= 'z')) || (((x) >= 'A') && ((x) <= 'Z')) || ((x) == '_'))

__declspec(dllexport) void nvCelsiusAAInitCompatibilitySettings()
{

#ifdef DYNAMIC_AA_COMPAT_SETTINGS
    DWORD i;
    DWORD dwCurSetting = 0;

    if (pDriverData->nvD3DRegistryData.regAAReadCompatibilityFile != 0xd3dbeef) {
        AAAppSettings = AAAppSettingsCompiled;
    }

    if (AAAppSettings) return;

    // Hard-coded filename.  If it's not found, use the
    HANDLE hFile = NvCreateFile("\\NvCelsiusAACompatibility.h",
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);


    // Note: this parsing is "quick-and dirty."  Fortunately, no one
    // outside of NVIDIA should care.
    if ((hFile != INVALID_HANDLE_VALUE) && (hFile != NULL)) {
        DWORD dwNumBytes, dwSize;
        LPTSTR lpStr;

#ifndef WINNT
        dwSize = GetFileSize(hFile, NULL);
        lpStr = new char[dwSize];
        NvReadFile(hFile, lpStr, dwSize, &dwNumBytes, NULL);
#else
        lpStr = new char[MAX_AAFILE_SIZE];
        for (i =0; i< MAX_AAFILE_SIZE; i++) {
            NvReadFile(hFile, lpStr+i,1,&dwNumBytes, NULL);
            if (dwNumBytes == 0) {
                lpStr[i] = '\0';
                break;
            }
        }
        dwSize = i;
#endif

        DWORD dwNumSettings = 1; // Start with 1 to make room for the default
        for (i=0; i<dwSize; i++) {
            // Skip comment lines
            if (lpStr[i] == '/') {
                while (lpStr[i] != '\n') i++;
            }

            // The number of () pairs should be equal to the number of
            // settings there are in the file
            if (lpStr[i] == '(') {
                dwNumSettings++;
            }
        }

        AAAppSettings = new AAAppCompatState[dwNumSettings];

        // Iterate over all characters in the file
        for (i=0; i<dwSize; i++) {
            // Skip comment lines
            if (lpStr[i] == '/') {
                while (lpStr[i] != '\n') i++;
            }

            if (lpStr[i] == '(') {
                char lpIdent[256];
                DWORD j;

                while (lpStr[i-1] != '"') i++;
                j = 0;
                while (lpStr[i] != '"') lpIdent[j++] = lpStr[i++];
                lpIdent[j] = '\0';


                AAAppSettings[dwCurSetting].lpApp = new char[strlen(lpIdent) +1];
                strcpy(AAAppSettings[dwCurSetting].lpApp, lpIdent);

                DWORD dwParam = 0;
                while (1) {
                    while (!WORD_CHAR(lpStr[i]) && lpStr[i] != ')') i++;
                    if (lpStr[i] == ')') break;

                    j = 0;
                    while (WORD_CHAR(lpStr[i])) lpIdent[j++] = lpStr[i++];
                    lpIdent[j] = '\0';

                    // Find the name to value matching of the identifier
                    for (DWORD k=0; AANamesToValues[k].lpName != 0; k++) {
                        if (strcmp(AANamesToValues[k].lpName, lpIdent) == 0) break;
                    }

                    *(&(AAAppSettings[dwCurSetting].bPath) + dwParam) = AANamesToValues[k].bValue;
                    dwParam++;
                }
                dwCurSetting++;
            }

        }

        delete [] lpStr;

        // Make default setting
        AAAppSettings[dwCurSetting].lpApp = 0;
        AAAppSettings[dwCurSetting].bNoAA = FALSE;
        AAAppSettings[dwCurSetting].bPath = FALSE;
        AAAppSettings[dwCurSetting].bDiscardFBContents = TRUE;
        AAAppSettings[dwCurSetting].bAlwaysMagnifyUponUnlock = FALSE;
        AAAppSettings[dwCurSetting].bMinifyUponEndScene = FALSE;
        AAAppSettings[dwCurSetting].bDither = FALSE;
        AAAppSettings[dwCurSetting].bBlitSync = FALSE;
        AAAppSettings[dwCurSetting].bDiscardSRT = TRUE;
        AAAppSettings[dwCurSetting].bAllowSpecial = TRUE;
    } else {
        AAAppSettings = AAAppSettingsCompiled;
    }
#else
    AAAppSettings = AAAppSettingsCompiled;
#endif
}



__declspec(dllexport) void nvCelsiusAAGetCompatibilitySettings
(
    PNVD3DCONTEXT pContext
)
{
    DWORD i;
    char pExe[256];

    // Should never happen, I think...
    nvAssert(pContext);
    nvAssert(AAAppSettings);

    // Get name of exe (sans path) and convert to lowercase
    //GetExecutableName(pExe, 256);
    //for (i=0; pExe[i] != '\0'; i++) {
    //   if (pExe[i] >= 'A' && pExe[i] <= 'Z') pExe[i] += 'a' - 'A';
    //}

    // Iterate through list, stopping on the last one (has null for the filename),
    // or on the one that matches.
    for (i=0; AAAppSettings[i].lpApp != 0; i++)
    {
        if (AAAppSettings[i].bPath) {
            GetFullExecutableName(pExe, 256, pContext);
        } else {
            GetExecutableName(pExe, 256, pContext);
        }

        if (strcmp(pExe, AAAppSettings[i].lpApp) == 0)
        {
            pContext->aa.mAppCompat = AAAppSettings[i];
            return;
        }
    }

    // App not found, use default...
    pContext->aa.mAppCompat = AAAppSettings[i];
}

// AASTATE functionality ----------------------------------------------------

inline void AASTATE::makeRenderBuffersMatch (PNVD3DCONTEXT pContext)
{
    if ((dwFlags & FLAG_RT_VALID) && (dwFlags & FLAG_SRT_VALID)) return;
    if (dwFlags & FLAG_RT_VALID) {
        nvCelsiusAAMagnify(pContext);
        dwFlags |= FLAG_SRT_VALID;
        return;
    }
    if (dwFlags & FLAG_SRT_VALID) {
        nvCelsiusAAMinify(pContext);
        dwFlags |= FLAG_RT_VALID;
        return;
    }
}

inline void AASTATE::makeZBuffersMatch (PNVD3DCONTEXT pContext)
{
    if ((dwFlags & FLAG_ZB_VALID) && (dwFlags & FLAG_SZB_VALID)) return;
    if (dwFlags & FLAG_ZB_VALID) {
        nvCelsiusAAMagnifyZ(pContext);
        dwFlags |= FLAG_SZB_VALID;
        return;
    }
    if (dwFlags & FLAG_SZB_VALID) {
        nvCelsiusAAMinifyZ(pContext);
        dwFlags |= FLAG_ZB_VALID;
        return;
    }
}

inline BOOL AASTATE::WantEnabled (PNVD3DCONTEXT pContext)
{
    return ((pDriverData->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_ANTIALIASENABLE_MASK)
        && (pContext->dwRenderState[D3DRENDERSTATE_ANTIALIAS] != D3DANTIALIAS_NONE)
        || (pDriverData->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_ANTIALIASFORCEENABLE_MASK)
        || (pContext->pRenderTarget && (pContext->pRenderTarget->getMultiSampleBits() > 1)));
}

void AASTATE::makeRenderTargetValid (PNVD3DCONTEXT pContext)
{
    if (!(dwFlags & FLAG_RT_VALID))
    {
        nvCelsiusAAMinify(pContext);
        dwFlags |= FLAG_RT_VALID;
    }
    dwFlags &= ~FLAG_SRT_VALID;
}

void AASTATE::makeZetaBufferValid (PNVD3DCONTEXT pContext)
{
    if (!pContext->aa.pSuperZetaBuffer)
        return;

    if (!(dwFlags & FLAG_ZB_VALID))
    {
        nvCelsiusAAMinifyZ(pContext);
        dwFlags |= FLAG_ZB_VALID;
    }
    dwFlags &= ~FLAG_SZB_VALID;
}

void AASTATE::makeSuperBufferValid (PNVD3DCONTEXT pContext)
{
    if (!(dwFlags & FLAG_SRT_VALID))
    {
        nvCelsiusAAMagnify(pContext);
        //flags can get modified in the function above, the optimizer uses the cached, unmodified value.
        //force loading through the pointer to work around it.
        pContext->aa.dwFlags |= FLAG_SRT_VALID;
    }
    pContext->aa.dwFlags &= ~FLAG_RT_VALID;
}

void AASTATE::makeSuperZBValid (PNVD3DCONTEXT pContext) {

    if (!pContext->aa.pSuperZetaBuffer)
        return;

    if (!(dwFlags & FLAG_SZB_VALID))
    {
        nvCelsiusAAMagnifyZ(pContext);
        pContext->aa.dwFlags |= FLAG_SZB_VALID;
    }
    pContext->aa.dwFlags &= ~FLAG_ZB_VALID;
}

void AASTATE::Create (PNVD3DCONTEXT pContext)
{
    if (WantEnabled(pContext))
    {
        if (nvCelsiusAACreate(pContext))
        {
            // enable AA
            setFlags (AASTATE::FLAG_ENABLED);
        }
    }
}

void AASTATE::invalidateState (void)
{
    PNVD3DCONTEXT pContext = (PNVD3DCONTEXT)(DWORD(this) - OFFSETOF(NVD3DCONTEXT,aa)); // we know aa lives in context (not good practice)
    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_SURFACE
                                   |  CELSIUS_DIRTY_TRANSFORM
                                   |  CELSIUS_DIRTY_TEXTURE_STATE
                                   |  CELSIUS_DIRTY_SPECFOG_COMBINER
                                   |  CELSIUS_DIRTY_MISC_STATE;
}

#endif  // NVARCH == 0x10

