/*
 * Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvKelvinAA.cpp                                                    *
*     prototypes for aa routines                                            *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Scott Cutler                     11Oct2000   created                *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x020)

#ifdef DEBUG_SURFACE_PLACEMENT
extern CSurfaceDebug csd;
#endif

//#define SCALED_IMAGE
//#define SUPERSAMPLE
#define AAFORCE_16_32
#define SUBDIVIDE_AA
//#define _16_BIT_FIX
//#define TEST_SUB

#define UPSAMPLE_SEMANTICS_FRAME_COUNT 5

#ifdef SUBDIVIDE_AA
#ifdef TEST_SUB
DWORD __stripsize = 768;
DWORD __swath = NV097_SET_SWATH_WIDTH_V_32;
#endif
#endif


extern HRESULT nvSetKelvinD3DDefaults(PNVD3DCONTEXT pContext);
__forceinline DWORD F2D(float f) { return *(DWORD *)&f; }

const char * CKelvinAAState::BufferTypeNames[] = {
    "BUFFER_SRT",
    "BUFFER_RT",
    "BUFFER_SZB",
    "BUFFER_ZB",
    "BUFFER_SCRATCH"
};

const char * CKelvinAAState::AccessTypeNames[] = {
    "ACCESS_READ",
    "ACCESS_WRITE",
    "ACCESS_WRITE_DISCARD"
};

const CKelvinAAState::MethodInfoStruct CKelvinAAState::MethodInfo[METHOD_MAX] = {
    { // METHOD_NONE
        false, 
        1.0,  0.0, 
        SIMPLEMETHOD_1X1, 
        DRF_DEF(097, _SET_SURFACE_FORMAT, _ANTI_ALIASING, _CENTER_1),        
        0,
        METHOD_NONE,
        METHOD_NONE
    }, { // METHOD_SUPERSAMPLE_2X_H                                                     
        false, 
        1.41, 0.0, 
        SIMPLEMETHOD_2X1, 
        DRF_DEF(097, _SET_SURFACE_FORMAT, _ANTI_ALIASING, _CENTER_1),        
        0,
        METHOD_SUPERSAMPLE_2X_H,
        METHOD_SUPERSAMPLE_2X_H
    }, { // METHOD_SUPERSAMPLE_2X_V                                                     
        false, 
        1.41, 0.0, 
        SIMPLEMETHOD_1X2, 
        DRF_DEF(097, _SET_SURFACE_FORMAT, _ANTI_ALIASING, _CENTER_1),        
        0,
        METHOD_SUPERSAMPLE_2X_V,
        METHOD_SUPERSAMPLE_2X_V
    }, { // METHOD_MULTISAMPLE_2X                                                     
        true,  
        0.0,  0.0, 
        SIMPLEMETHOD_2X1, 
        DRF_DEF(097, _SET_SURFACE_FORMAT, _ANTI_ALIASING, _CENTER_CORNER_2), 
        0,
        METHOD_MULTISAMPLE_2X,
        METHOD_SUPERSAMPLE_2X_V
    }, { // METHOD_MULTISAMPLE_2X_QUINCUNX                                                     
        true,  
        0.0,  0.0, 
        SIMPLEMETHOD_2X1, 
        DRF_DEF(097, _SET_SURFACE_FORMAT, _ANTI_ALIASING, _CENTER_CORNER_2), 
        NV097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL_QUINCUNX,
        METHOD_MULTISAMPLE_2X,
        METHOD_SUPERSAMPLE_2X_V
    }, { // METHOD_SUPERSAMPLE_4X_BIAS                                                     
        false, 
        2.0,  1.0, 
        SIMPLEMETHOD_2X2, 
        DRF_DEF(097, _SET_SURFACE_FORMAT, _ANTI_ALIASING, _CENTER_1),        
        0,
        METHOD_SUPERSAMPLE_4X_BIAS,
        METHOD_SUPERSAMPLE_4X_BIAS
    }, { // METHOD_SUPERSAMPLE_4X                                                     
        false, 
        2.0,  0.0, 
        SIMPLEMETHOD_2X2, 
        DRF_DEF(097, _SET_SURFACE_FORMAT, _ANTI_ALIASING, _CENTER_1),        
        0,
        METHOD_SUPERSAMPLE_4X,
        METHOD_SUPERSAMPLE_4X
    }, { // METHOD_SUPERSAMPLE_4X_GAUSSIAN                                                    
        false, 
        2.0,  0.0, 
        SIMPLEMETHOD_2X2, 
        DRF_DEF(097, _SET_SURFACE_FORMAT, _ANTI_ALIASING, _CENTER_1),        
        NV097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL_GAUSSIAN_3,
        METHOD_SUPERSAMPLE_4X,
        METHOD_SUPERSAMPLE_4X
    }, { // METHOD_MULTISAMPLE_4X                                                     
        true,
        0.0,  0.0,
        SIMPLEMETHOD_2X2,
        DRF_DEF(097, _SET_SURFACE_FORMAT, _ANTI_ALIASING, _SQUARE_OFFSET_4),
        0,
        METHOD_MULTISAMPLE_4X,
        METHOD_SUPERSAMPLE_4X
    }, { // METHOD_MULTISAMPLE_4X_GAUSSIAN                                                     
        true,  
        0.0,  0.0, 
        SIMPLEMETHOD_2X2, 
        DRF_DEF(097, _SET_SURFACE_FORMAT, _ANTI_ALIASING, _SQUARE_OFFSET_4), 
        NV097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL_GAUSSIAN_3,
        METHOD_MULTISAMPLE_4X,
        METHOD_SUPERSAMPLE_4X
    }, { // METHOD_MULTISAMPLE_4X_ROTATED_4TAP                                                     
        true,  
        0.0,  0.0, 
        SIMPLEMETHOD_2X2, 
        DRF_DEF(097, _SET_SURFACE_FORMAT, _ANTI_ALIASING,_CENTER_CORNER_2),
        0,
        METHOD_MULTISAMPLE_4X,
        METHOD_SUPERSAMPLE_4X
    }, { // METHOD_MULTISAMPLE_4X_ROTATED_8TAP                                                     
        true,  
        0.0,  0.0, 
        SIMPLEMETHOD_2X2, 
        DRF_DEF(097, _SET_SURFACE_FORMAT, _ANTI_ALIASING,_CENTER_CORNER_2),
        0,
        METHOD_MULTISAMPLE_4X,
        METHOD_SUPERSAMPLE_4X
    }, { // METHOD_SUPERSAMPLE_9X_BIAS                                                     
        false, 
        3.0,  1.5, 
        SIMPLEMETHOD_3X3, 
        DRF_DEF(097, _SET_SURFACE_FORMAT, _ANTI_ALIASING, _CENTER_1),
        0,
        METHOD_SUPERSAMPLE_9X_BIAS,
        METHOD_SUPERSAMPLE_9X_BIAS
    }, { // METHOD_SUPERSAMPLE_9X                                                     
        false, 
        3.0,  0.0, 
        SIMPLEMETHOD_3X3, 
        DRF_DEF(097, _SET_SURFACE_FORMAT, _ANTI_ALIASING, _CENTER_1),
        0,
        METHOD_SUPERSAMPLE_9X,
        METHOD_SUPERSAMPLE_9X
    }, { // METHOD_SUPERSAMPLE_16X_BIAS                                                     
        false, 
        4.0,  2.0, 
        SIMPLEMETHOD_4X4, 
        DRF_DEF(097, _SET_SURFACE_FORMAT, _ANTI_ALIASING, _CENTER_1),
        0,
        METHOD_SUPERSAMPLE_16X_BIAS,
        METHOD_SUPERSAMPLE_16X_BIAS
    }, { // METHOD_SUPERSAMPLE_16X                                                     
        false, 
        4.0,  0.0, 
        SIMPLEMETHOD_4X4, 
        DRF_DEF(097, _SET_SURFACE_FORMAT, _ANTI_ALIASING, _CENTER_1),
        0,
        METHOD_SUPERSAMPLE_16X,
        METHOD_SUPERSAMPLE_16X
    }
};

const CKelvinAAState::BufferBltInfoStruct CKelvinAAState::BufferBltInfo[SIMPLEMETHOD_MAX] = {
    {1, 1, 0, 0, 0.00, 0.00, 0.00, 0.00,   0.5000, 0.5000},    // SIMPLEMETHOD_1X1
    {1, 2, 0, 0, 0.00, 0.50, 0.00, 0.00,   0.5000, 0.2500},    // SIMPLEMETHOD_1X2
    {2, 1, 0, 0, 0.50, 0.00, 0.00, 0.00,   0.2500, 0.5000},    // SIMPLEMETHOD_2X1
    {2, 2, 0, 0, 0.50, 0.50, 0.00, 0.00,   0.2500, 0.2500},    // SIMPLEMETHOD_2X2
    {3, 3, 2, 2, 0.25, 0.25, 0.50, 0.50,   0.1666, 0.1666},    // SIMPLEMETHOD_3X3
    {4, 4, 2, 2, 0.50, 0.50, 0.50, 0.50,   0.1250, 0.1250}     // SIMPLEMETHOD_4X4
};


const CKelvinAAState::MethodType CKelvinAAState::RegQualityToAATypeMapping[10] = {
    METHOD_NONE,
    METHOD_MULTISAMPLE_2X,
    METHOD_MULTISAMPLE_2X_QUINCUNX,
    METHOD_MULTISAMPLE_4X,
    METHOD_MULTISAMPLE_4X_GAUSSIAN,
// Following methods are not implemented yet.  Punt to working modes.
//    METHOD_SUPERSAMPLE_9X_BIAS,
//    METHOD_SUPERSAMPLE_9X,
//    METHOD_SUPERSAMPLE_16X_BIAS,
//    METHOD_SUPERSAMPLE_16X
    METHOD_MULTISAMPLE_4X_GAUSSIAN,
    METHOD_MULTISAMPLE_4X_GAUSSIAN,
    METHOD_MULTISAMPLE_4X_GAUSSIAN,
    METHOD_MULTISAMPLE_4X_GAUSSIAN,
    METHOD_MULTISAMPLE_4X_GAUSSIAN
};


CKelvinAAState::CKelvinAAState() { 
    _pContext = 0;
    _enabled = 0;
    _numZLocks = 0;
    _didUpsample = true;
    _lastDownsampleUsedConvolution = false;
    _badSemantics = false;
    _seen3D = 0;

    memset(_bufferInfo, 0, sizeof(_bufferInfo));
}





void CKelvinAAState::Create(PNVD3DCONTEXT pContext) {
    if (_pContext) {
        // Make sure we don't try to create things twice
        // We want one Destroy for every Create
        nvAssert(0);
        return;
    }
        
    nvKelvinInitProgram(&_program);
    nvKelvinParseProgram(&_program, "%!VP1.0\nMOV o[HPOS], v[0];\nMOV o[TEX0], v[9];\nMOV o[TEX1], v[10];\nMOV o[TEX2], v[11];\nMOV o[TEX3], v[12];");

    InitAppCompatibility();
    if (_appCompatibility.dwFlags & APPFLAGS_DISABLE_AA) return;

    //on endscene we give read access to the RT, and if it's invalid, we minify the SRT.
    //after a refresh call, the SRT is invalidated and usually a subsequent write access
    //(like a clear) to the SRT tags the RT as invalid (and the SRT as valid). however, if
    //the app doesn't write to the SRT before the read access to the RT, the RT is never
    //invalidated which means the minification never happens -> eek!
    //this remebers if the SRT was valid at refresh time and does not set the RT as valid
    //in that case. FS
    
    if (_invalidRT)
      _invalidRT = 0;
    else 
      _bufferInfo[BUFFER_RT].flags |= FLAGS_VALID;
      


    // Well, this isn't quite correct, since we haven't copied
    // the contents of the Z buffer.  Fortunately, it shouldn't cause many
    // problems, and it needed because we destroy the normal Z buffer before
    // we create the new one.
    _bufferInfo[BUFFER_SZB].flags |= FLAGS_VALID;

    _bufferInfo[BUFFER_RT].other = BUFFER_SRT;
    _bufferInfo[BUFFER_SRT].other = BUFFER_RT;
    _bufferInfo[BUFFER_ZB].other = BUFFER_SZB;
    _bufferInfo[BUFFER_SZB].other = BUFFER_ZB;

    nvAssert(getDC()->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN);


    if ((!pContext->pRenderTarget) ||
        (pContext->pRenderTarget->isSwizzled())) {
        DPF_LEVEL (NVDBG_LEVEL_ERROR,"CKelvinAAState::Create: AA failed because render target either does not exist or it is swizzled.");
        memset(_bufferInfo, 0, sizeof(_bufferInfo));
        return;
    }

    _pContext = pContext;


    DWORD dwMSBits = _pContext->pRenderTarget->getMultiSampleBits();

    if (dwMSBits >= 2) {
        // App-controlled multisampling.  We support 2 and 4 sample modes.
        nvAssert(!_pContext->pZetaBuffer || (dwMSBits == _pContext->pZetaBuffer->getMultiSampleBits()));

        switch (dwMSBits) {
        case 2:  SetMethod(METHOD_MULTISAMPLE_2X);           break;
        case 3:  SetMethod(METHOD_MULTISAMPLE_2X_QUINCUNX);  break;
        case 4:  SetMethod(METHOD_MULTISAMPLE_4X);           break;
        case 5:  SetMethod(METHOD_MULTISAMPLE_4X_GAUSSIAN);  break;
        default:
            // Invalid number of bits
            DPF_LEVEL(NVDBG_LEVEL_ERROR,"CKelvinAAState::Create: App set invalid number of multisampling bits (%d).", dwMSBits);
            _pContext = 0;
            return;
        }
    } else if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_ANTIALIASFORCEENABLE_MASK) ||
               (pContext->dwRenderState[D3DRENDERSTATE_ANTIALIAS] != D3DANTIALIAS_NONE)) {
        // FSAA forced on through registry or render state.
        nvAssert(getDC()->nvD3DRegistryData.regAntiAliasQuality < 9);
        SetMethod(RegQualityToAATypeMapping[getDC()->nvD3DRegistryData.regAntiAliasQuality]);
    } else {
        // We shouldn't be in here
        nvAssert(0);
    }

#ifdef AAFORCE_16_32
    _MixedModeAA = (getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_ANTIALIASCONVOLUTIONFASTMODE_MASK) == D3D_REG_ANTIALIASCONVOLUTIONFASTMODE_ENABLE ;
#endif

    const MethodInfoStruct *info = &MethodInfo[_method];
    const BufferBltInfoStruct *bltInfo;

    DWORD dwSimpleMethod = info->type;

    // Just for the convenience factor
    CSimpleSurface *&pRT = _bufferInfo[BUFFER_RT].pSurface;
    CSimpleSurface *&pZB = _bufferInfo[BUFFER_ZB].pSurface;
    CSimpleSurface *&pSRT = _bufferInfo[BUFFER_SRT].pSurface;
    CSimpleSurface *&pSZB = _bufferInfo[BUFFER_SZB].pSurface;

    pRT = _pContext->pRenderTarget;
    pZB = _pContext->pZetaBuffer;
    pSRT = 0;
    pSZB = 0;

    // Kill the normal Z buffer.  There's no need for it unless the application
    // later locks the Z buffer (in which case we'll recreate it).  Otherwise it 
    // just takes up memory and Z tag bit space.
    if (pZB) {
#ifdef DEBUG_SURFACE_PLACEMENT
        csd.DeleteSurface(pZB);
#endif
        pZB->destroy();
    }

    // A "simple method" is a unique buffer scale + filter type combination.
    // We start at the top, iterating down, until we find one that works.
    while (dwSimpleMethod > SIMPLEMETHOD_1X1) {
        DWORD dwMem = 0;
        DWORD dwPitch = 0;

        bltInfo = &BufferBltInfo[dwSimpleMethod];
        
        // 4k is the max.
        if ((pRT->getWidth() * bltInfo->dwSuperBufferScaleX) > 4096) goto creationFail;
        if ((pRT->getHeight() * bltInfo->dwSuperBufferScaleY) > 4096) goto creationFail;

        // Calculate the framebuffer space requirements
        if (pRT) dwMem += pRT->getBPP() * pRT->getWidth() * pRT->getHeight();
        if (pZB) dwMem += pZB->getBPP() * pZB->getWidth() * pZB->getHeight();

        dwMem *= DWORD((bltInfo->dwSuperBufferScaleX * bltInfo->dwSuperBufferScaleY) +
                        (bltInfo->dwScratchBufferScaleX * bltInfo->dwScratchBufferScaleY));

        // Ensure that we still have 1/4 of frame buffer memory left after all is said and done
        if ((int(getDC()->VideoHeapFree) - int(dwMem)) < int(getDC()->VideoHeapTotal / 4)) goto creationFail;

        
        // Create the super render target if the app has one (it better!)
        if (pRT) {
            if (!(pSRT = new CSimpleSurface())) goto creationFail;

#ifndef AAFORCE_16_32
            dwPitch = pRT->getWidth() * bltInfo->dwSuperBufferScaleX * pRT->getBPP();
#else
            if (IsMixedModeAA() && pRT->getFormat() == NV_SURFACE_FORMAT_X8R8G8B8 && (_method == METHOD_MULTISAMPLE_2X_QUINCUNX || _method ==  METHOD_MULTISAMPLE_4X_GAUSSIAN)) 
                dwPitch = pRT->getWidth() * bltInfo->dwSuperBufferScaleX * 2;
            else 
                dwPitch = pRT->getWidth() * bltInfo->dwSuperBufferScaleX * pRT->getBPP();
#endif
            dwPitch = (dwPitch + getDC()->dwSurfaceAlignPad) & ~getDC()->dwSurfaceAlignPad;


#ifndef AAFORCE_16_32
            if (!pSRT->create(NULL,
                              pRT->getFormat(),
                              pRT->getWidth()  * bltInfo->dwSuperBufferScaleX,
                              pRT->getHeight() * bltInfo->dwSuperBufferScaleY,
                              pRT->getDepth(),
                              1,
                              pRT->getBPPRequested(),
                              pRT->getBPP(),
                              dwPitch,
                              CSimpleSurface::HEAP_VID,
                              CSimpleSurface::HEAP_VID,
                              CSimpleSurface::ALLOCATE_TILED
#ifdef CAPTURE
                             ,CAPTURE_SURFACE_KIND_TARGET
#endif
                        )) goto creationFail;
#else   //AAFORCE_16_32

            if (IsMixedModeAA() && pRT->getFormat() == NV_SURFACE_FORMAT_X8R8G8B8 && (_method == METHOD_MULTISAMPLE_2X_QUINCUNX || _method ==  METHOD_MULTISAMPLE_4X_GAUSSIAN)) 
            {
                if (!pSRT->create(NULL,
                                  NV_SURFACE_FORMAT_R5G6B5,
                                  pRT->getWidth()  * bltInfo->dwSuperBufferScaleX,
                                  pRT->getHeight() * bltInfo->dwSuperBufferScaleY,
                                  pRT->getDepth(),
                                  1,
                                  2,
                                  2,
                                  dwPitch,
                                  CSimpleSurface::HEAP_VID,
                                  CSimpleSurface::HEAP_VID,
                                  CSimpleSurface::ALLOCATE_TILED
#ifdef CAPTURE
                                 ,CAPTURE_SURFACE_KIND_TARGET
#endif
                            )) goto creationFail;
                
            }
            else 
            {
                
                if (!pSRT->create(NULL,
                                  pRT->getFormat(),
                                  pRT->getWidth()  * bltInfo->dwSuperBufferScaleX,
                                  pRT->getHeight() * bltInfo->dwSuperBufferScaleY,
                                  pRT->getDepth(),
                                  1,
                                  pRT->getBPPRequested(),
                                  pRT->getBPP(),
                                  dwPitch,
                                  CSimpleSurface::HEAP_VID,
                                  CSimpleSurface::HEAP_VID,
                                  CSimpleSurface::ALLOCATE_TILED
#ifdef CAPTURE
                                 ,CAPTURE_SURFACE_KIND_TARGET
#endif
                            )) goto creationFail;

            }
#endif //AAFORCE_16_32
        } else {
            DPF("Error in CKelvinAAState::Create: No render target");
            nvAssert(0);
            goto creationFail;
        }


#ifdef DEBUG_SURFACE_PLACEMENT
        csd.SetAARenderBuffer(pSRT);
#endif 


        if (pZB) {
            if (!(pSZB = new CSimpleSurface())) goto creationFail;

            dwPitch = pZB->getWidth() * bltInfo->dwSuperBufferScaleX * pZB->getBPP();
            dwPitch = (dwPitch + getDC()->dwSurfaceAlignPad) & ~getDC()->dwSurfaceAlignPad;

            DWORD dwZCmpFlags = (_appCompatibility.dwFlags & APPFLAGS_FORCE_UNCOMPRESSED_Z) ? 
                CSimpleSurface::ALLOCATE_AS_ZBUFFER_UNC : 
                CSimpleSurface::ALLOCATE_AS_ZBUFFER_CMP;

            if (!pSZB->create(NULL,
                              pZB->getFormat(),
                              pZB->getWidth()  * bltInfo->dwSuperBufferScaleX,
                              pZB->getHeight() * bltInfo->dwSuperBufferScaleY,
                              pZB->getDepth(),
                              1,
                              pZB->getBPPRequested(),
                              pZB->getBPP(),
                              dwPitch,
                              CSimpleSurface::HEAP_VID,
                              CSimpleSurface::HEAP_VID,
                              CSimpleSurface::ALLOCATE_TILED | dwZCmpFlags
#ifdef CAPTURE
                             ,CAPTURE_SURFACE_KIND_ZETA
#endif
                        )) goto creationFail;
        }

#ifdef DEBUG_SURFACE_PLACEMENT
        csd.SetAAZBuffer(pSZB);
#endif 

#ifdef  STEREO_SUPPORT
        if (STEREO_ENABLED)
        {
            if (pSRT) {
                pSRT->tagNotReal();
                pSRT->createStereo();
            }
            if (pSZB) {
                pSZB->tagNotReal();
                pSZB->createStereo();
            }
        }
#endif  //STEREO_SUPPORT

        // TODO: scratch buffer stuff


        // The buffers were created properly.
        break;

creationFail:
        if (pSRT) {
            pSRT->destroy();
            delete pSRT;
            pSRT = 0;
        }

        if (pSZB) {
            pSZB->destroy();
            delete pSZB;
            pSZB = 0;
        }

        dwSimpleMethod--;
    }


    CNvObject *pWrapper;
    
    if (pSRT) {
        pWrapper = new CNvObject(0);
        pWrapper->setObject(CNvObject::NVOBJ_SIMPLESURFACE, (void *)pSRT);
        pSRT->setWrapper(pWrapper);
    }
    
    if (pSZB) {
        pWrapper = new CNvObject(0);
        pWrapper->setObject(CNvObject::NVOBJ_SIMPLESURFACE, (void *)pSZB);
        pSZB->setWrapper(pWrapper);
    }
    

    if ((DWORD)info->type != dwSimpleMethod) {
        // We changed the method from what the user requested for one reason or
        // another.  Find an ordinary method that corresponds with this.

        _method = METHOD_SUPERSAMPLE_16X;
        while (
            (_method > METHOD_NONE) && 
            (((DWORD)MethodInfo[_method].type != dwSimpleMethod) ||
            MethodInfo[_method].dwKernel)
            ) _method--;

        info = &MethodInfo[_method];
        bltInfo = &BufferBltInfo[dwSimpleMethod];
    }

    if (_method == METHOD_NONE) {
        ReCreateZBuffer();
    
        pSRT = pRT;
        pSZB = pZB;
        bltInfo = &BufferBltInfo[METHOD_NONE];
    }

    
    if (MethodInfo[_method].bMultisample) {
        // Multisampling does the viewport scaling automatically.
        if (_method == METHOD_MULTISAMPLE_4X_ROTATED_4TAP || _method == METHOD_MULTISAMPLE_4X_ROTATED_8TAP ) 
        {
            _pContext->aa.fWidthAmplifier = 1;
            _pContext->aa.fHeightAmplifier = 2;
        }
        else {
            _pContext->aa.fWidthAmplifier  = 1;
            _pContext->aa.fHeightAmplifier = 1;
        }
        
    } else {
        _pContext->aa.fWidthAmplifier = bltInfo->dwSuperBufferScaleX;
        _pContext->aa.fHeightAmplifier = bltInfo->dwSuperBufferScaleY;
    }
    _pContext->aa.dwDepthAmplifier = 1;
    _pContext->aa.fLODBias = info->fBias;
    _pContext->aa.dwLinePointSize = (DWORD)(info->fLinePointSize * 0x8);

/////////////////////////////////////////////////////
    
    // Should we stripdivide?
    // output 0, dsty, strip._x0, srcy-strip.y0
    //        strip.dstWidth, dsty, strip.srcWidth_x0, srcy-strip.y0
    //        strip.dstWidth, dsty+strip.dstyinc, strip.srcWidth_x0, srcy+strip.srcyinc-strip.y0
    //        0, dsty+strip.dstyinc, strip._x0, srcy+strip.srcyinc-strip.y0

    // This logic actually works whether we're upscaling or downscaling
    if (MethodInfo[_method].dwKernel) {
        _strip.x0.f = 0.25 * BufferBltInfo[dwSimpleMethod].dwSuperBufferScaleX;
        _strip.y0.f = 0.25 * BufferBltInfo[dwSimpleMethod].dwSuperBufferScaleY;
    } else {
        _strip.x0.f = 0;
        _strip.y0.f = 0;
    }
    _strip._x0.f = -_strip.x0.f;

    _bSubdivideAA = true;
    _UpsampleFlip = CURRENT_FLIP;
    if (IsMixedModeAA()) 
    {
        if (_method == METHOD_MULTISAMPLE_2X_QUINCUNX)
            _strip.stripsize = 32;        
        else if (_method == METHOD_MULTISAMPLE_4X_GAUSSIAN)
            _strip.stripsize = 16;
        else  
        {
            _strip.stripsize = pRT->getHeight();
            _bSubdivideAA = false;            
        }
    }
    else 
    {
        if (_method == METHOD_MULTISAMPLE_2X_QUINCUNX)
            _strip.stripsize = 16;
        else 
        {
            _strip.stripsize = pRT->getHeight();
            _bSubdivideAA = false;            
        }
        
    }
    
#ifdef TEST_SUB
    _strip.stripsize = __stripsize;
#endif

    _strip.num_strips =  pRT->getHeight() / _strip.stripsize;
    _strip.remainder = pRT->getHeight() % _strip.stripsize;

    if (_method <= METHOD_MULTISAMPLE_2X_QUINCUNX)
        _strip.swath = NV097_SET_SWATH_WIDTH_V_32;
    else if (_method < METHOD_MULTISAMPLE_4X_GAUSSIAN)
        _strip.swath = NV097_SET_SWATH_WIDTH_V_128;
    else 
        _strip.swath = NV097_SET_SWATH_WIDTH_V_OFF;

    if (_strip.num_strips > 1) 
    {
        if (_method == METHOD_MULTISAMPLE_2X_QUINCUNX)
            _strip.swath = IsMixedModeAA() ? NV097_SET_SWATH_WIDTH_V_16 : NV097_SET_SWATH_WIDTH_V_8;
        else if (IsMixedModeAA()  && (_method == METHOD_MULTISAMPLE_4X_GAUSSIAN))
            _strip.swath = NV097_SET_SWATH_WIDTH_V_16;
    }

#ifdef TEST_SUB
    _strip.swath = __swath;
#endif

    _strip.srcyinc.f = static_cast<float>(_strip.stripsize * BufferBltInfo[dwSimpleMethod].dwSuperBufferScaleY);
    _strip.dstyinc.f = static_cast<float>(_strip.stripsize);
    if (_strip.remainder) 
    {
        _strip.srcyrem.f = static_cast<float>(_strip.remainder * BufferBltInfo[dwSimpleMethod].dwSuperBufferScaleY);         
        _strip.dstyrem.f = static_cast<float>(_strip.remainder);
    }
    float tmp = static_cast<float>(pSRT->getWidth());
    _strip.srcWidth.f = tmp;
    _strip.srcWidth_x0.f = tmp - _strip.x0.f;
    _strip.dstWidth.f = static_cast<float>(pRT->getWidth());

///////////////////////////////////////////////

    // Finally, turn on AA
    SetSampleMask();

    Save();
    
    // start off suspended
    _enabled = 1;

    // for good luck
    if (_pContext) _pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_MISC_STATE | KELVIN_DIRTY_SURFACE | KELVIN_DIRTY_TRANSFORM;

    getDC()->dwAAContextCount++;
}

void CKelvinAAState::Refresh() {
    if (!_enabled || !_pContext) return;

    PNVD3DCONTEXT pContext = _pContext;
    //don't optimize this! FS
    if (IsValid(CKelvinAAState::BUFFER_SRT)) {
        Destroy();
        _invalidRT = true;
    } else {
        Destroy();
    }
    Create(pContext);
}

void CKelvinAAState::Destroy() {
    if (!_pContext) return;
    
    if (getDC()->nvPusher.isValid()) {
        getDC()->nvPusher.flush (TRUE, CPushBuffer::FLUSH_WITH_DELAY);
    }

    //dirty the surface state at least -- do we need to dirty anything else?
    _pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_SURFACE;

    if (_method != SIMPLEMETHOD_1X1) {
        // Eh, we better not destroy these if we don't own them
        if (_bufferInfo[BUFFER_RT].pSurface != _bufferInfo[BUFFER_SRT].pSurface) _bufferInfo[BUFFER_SRT].pSurface->getWrapper()->release();
        if (_bufferInfo[BUFFER_ZB].pSurface != _bufferInfo[BUFFER_SZB].pSurface) _bufferInfo[BUFFER_SZB].pSurface->getWrapper()->release();
    }

    ReCreateZBuffer();

    getDC()->dwAAContextCount--;

    bool badSemantics = _badSemantics; // save this bit of info across destroy calls
    memset(this, 0, sizeof(CKelvinAAState));
    _badSemantics = badSemantics;
}

CSimpleSurface * CKelvinAAState::GetCurrentRT(PNVD3DCONTEXT pContext) {
    _seen3D = true;
    return IsEnabled() ? GetSRT() : pContext->pRenderTarget; 
}

CSimpleSurface * CKelvinAAState::GetCurrentZB(PNVD3DCONTEXT pContext) { 
    _seen3D = true;
    return IsEnabled() ? GetSZB() : pContext->pZetaBuffer; 
}


bool CKelvinAAState::CheckBufferMemoryConfig(PNVD3DCONTEXT pContext) {
    // quick check to see if we have enough memory
    DWORD dwMem = 0;

    if (pContext->pRenderTarget) {
        // 3 bits is really 2 with a filter
        DWORD dwMultiSampleBits = (pContext->pRenderTarget->getMultiSampleBits() == 3) ? 2 : pContext->pRenderTarget->getMultiSampleBits();

        if (dwMultiSampleBits < 2) return true; // no AA

        dwMem += 
            pContext->pRenderTarget->getBPP() * 
            pContext->pRenderTarget->getWidth() * 
            pContext->pRenderTarget->getHeight() *
            dwMultiSampleBits;
    }

    if (pContext->pZetaBuffer) {
        DWORD dwMultiSampleBits = (pContext->pZetaBuffer->getMultiSampleBits() == 3) ? 2 : pContext->pZetaBuffer->getMultiSampleBits();

        if (dwMultiSampleBits < 2) return true; // no AA

        dwMem += 
            pContext->pZetaBuffer->getBPP() * 
            pContext->pZetaBuffer->getWidth() * 
            pContext->pZetaBuffer->getHeight() *
            dwMultiSampleBits;
    }

    // Ensure that we still have 1/4 of frame buffer memory left after all is said and done
    if ((int(getDC()->VideoHeapFree) - int(dwMem)) < int(getDC()->VideoHeapTotal / 4)) return false;

    // multisample settings must match
    if (pContext->pRenderTarget && pContext->pZetaBuffer && 
       (pContext->pRenderTarget->getMultiSampleBits() != pContext->pZetaBuffer->getMultiSampleBits())) return false;

    return true;
}

void CKelvinAAState::TestCreation(PNVD3DCONTEXT pContext) {
    if (!(getDC()->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN)) return;
    
    nvAssert(pContext && getDC());

    if (_pContext) {
        // Already created; destroy if needed
        if (_enabled &&
           ((!(getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_ANTIALIASFORCEENABLE_MASK) &&
             !(pContext->dwRenderState[D3DRENDERSTATE_ANTIALIAS] == D3DANTIALIAS_SORTDEPENDENT) &&
             !(pContext->dwRenderState[D3DRENDERSTATE_ANTIALIAS] == D3DANTIALIAS_SORTINDEPENDENT) &&
             !(_bufferInfo[BUFFER_RT].pSurface->getMultiSampleBits() > 1)  &&
             !(_bufferInfo[BUFFER_ZB].pSurface && (_bufferInfo[BUFFER_ZB].pSurface->getMultiSampleBits() > 1))) ||
             _badSemantics ||
             (_appCompatibility.dwFlags & APPFLAGS_DISABLE_AA))) {
            Destroy();
        }
    } else {
        // Not created; check if we need to
        if (_enabled &&
            !(_appCompatibility.dwFlags & APPFLAGS_DISABLE_AA) &&
            !_badSemantics &&
           ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_ANTIALIASFORCEENABLE_MASK &&
             getDC()->nvD3DRegistryData.regAntiAliasQuality > 0) ||
            (pContext->dwRenderState[D3DRENDERSTATE_ANTIALIAS] == D3DANTIALIAS_SORTDEPENDENT) ||
            (pContext->dwRenderState[D3DRENDERSTATE_ANTIALIAS] == D3DANTIALIAS_SORTINDEPENDENT) ||
            (pContext->pRenderTarget && pContext->pRenderTarget->getMultiSampleBits() > 1))) {
                Create(pContext);
        }
    }

    if (IsEnabled()) {
        CSimpleSurface *pCRT = pContext->pRenderTarget;
        CSimpleSurface *pRT  = _bufferInfo[BUFFER_RT].pSurface;
        CSimpleSurface *pCZB = pContext->pZetaBuffer;
        CSimpleSurface *pZB  = _bufferInfo[BUFFER_ZB].pSurface;
        
        if ((!pCRT != !pRT) || (!pCZB != !pZB) ||
            (pCRT && pRT && (pCRT->getWidth() != pRT->getWidth()) && (pCRT->getHeight() != pRT->getHeight())) ||
            (pCZB && pZB && (pCZB->getWidth() != pZB->getWidth()) && (pCZB->getHeight() != pZB->getHeight()))) {
            Refresh();
        }
    }
}

void CKelvinAAState::Save() 
{
    _bSubdivideAASave = _bSubdivideAA;
    _methodSave = _method;
    _bAAMethodChanged = false;
}

void CKelvinAAState::Restore()
{
    if (_bAAMethodChanged) 
    {
        if ((CURRENT_FLIP - _UpsampleFlip) > 5)
        {
            _bSubdivideAA = _bSubdivideAASave;
            _method = _methodSave;
        }
    
    }
}    


void CKelvinAAState::SetMethod(MethodType method) {
    if (!_pContext) return;

#ifndef SUPERSAMPLE
    if (!(_appCompatibility.dwFlags & APPFLAGS_FORCE_SUPERSAMPLE)) {
        _method = method;
    } else 
#endif
    {
        _method = MethodInfo[method].dwSuperSampledAlternative;
    }

}

DWORD CKelvinAAState::GetAAFormat() {
    return IsEnabled() ? MethodInfo[_method].AASurfaceType : 0; 
}

void CKelvinAAState::Suspend() {
    _enabled = 0;
    if (_pContext) _pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_MISC_STATE | KELVIN_DIRTY_SURFACE | KELVIN_DIRTY_TRANSFORM;
}

void CKelvinAAState::Resume() {
    _enabled = 1;
    if (_pContext) _pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_MISC_STATE | KELVIN_DIRTY_SURFACE | KELVIN_DIRTY_TRANSFORM;
}

void CKelvinAAState::Notify(NotificationType type) {
    if (!_pContext) return;
    // not used yet
    nvAssert(0);
}

void CKelvinAAState::Flip() {
    if (!_pContext) return;

    Resume();

    if (_appCompatibility.dwFlags & APPFLAGS_MAGNIFY_AFTER_FLIP) {
        GrantAccess(BUFFER_RT, ACCESS_WRITE, true);
    } else {
        GrantAccess(BUFFER_RT, ACCESS_READ, true);
    }
}

int CKelvinAAState::IsEnabled() {
    return (_enabled && _pContext && (_method != METHOD_NONE));
}

int CKelvinAAState::IsCreated() {
    return (int)_pContext; 
}

int CKelvinAAState::IsMultisampling() { 
    return (IsEnabled() && MethodInfo[_method].bMultisample);
}

int CKelvinAAState::IsSupersampling() { 
    return (IsEnabled() && MethodInfo[_method].bMultisample);
}

void CKelvinAAState::SetSampleMask() {
    if (!_pContext) return;

    if (_pContext->pRenderTarget->getMultiSampleBits() <= 1) {
        _sampleMask = 0xffff;
    } else {
        _sampleMask = _pContext->dwRenderState[D3DRS_MULTISAMPLEMASK] & 0xf;
        _sampleMask = (_sampleMask << 12) | (_sampleMask << 8) | (_sampleMask << 4) | _sampleMask; // Replicate across four pixels
    }

    DWORD aaControl = _enabled ?
        DRF_DEF(097, _SET_ANTI_ALIASING_CONTROL, _ENABLE, _TRUE) |
        DRF_DEF(097, _SET_ANTI_ALIASING_CONTROL, _ALPHA_TO_COVERAGE, _DISABLE) |
        DRF_DEF(097, _SET_ANTI_ALIASING_CONTROL, _ALPHA_TO_ONE, _DISABLE) |
        DRF_NUM(097, _SET_ANTI_ALIASING_CONTROL, _SAMPLE_MASK, _sampleMask)
        :
        DRF_DEF(097, _SET_ANTI_ALIASING_CONTROL, _ENABLE, _FALSE) |
        DRF_DEF(097, _SET_ANTI_ALIASING_CONTROL, _ALPHA_TO_COVERAGE, _DISABLE) |
        DRF_DEF(097, _SET_ANTI_ALIASING_CONTROL, _ALPHA_TO_ONE, _DISABLE) |
        DRF_NUM(097, _SET_ANTI_ALIASING_CONTROL, _SAMPLE_MASK, 0xffff);

    _pContext->hwState.kelvin.set1(NV097_SET_ANTI_ALIASING_CONTROL, aaControl);
}

void CKelvinAAState::ReCreateZBuffer() {
    
    CSimpleSurface *pZB = _bufferInfo[BUFFER_ZB].pSurface;
    
    if (pZB && !pZB->isValid()) {
//        pZB->recreate(pZB->getBPP());
        pZB->create(pZB->getWrapper(),
            pZB->getFormat(),
            pZB->getWidth(),
            pZB->getHeight(),
            1,
            pZB->getMultiSampleBits(),
            pZB->getBPPRequested(),
            pZB->getBPP(),
            pZB->getBPP() * pZB->getWidth(),
            CSimpleSurface::HEAP_VID,
            CSimpleSurface::HEAP_VID,
            pZB->getAllocFlags()
#ifdef CAPTURE
            ,CAPTURE_SURFACE_KIND_ZETA
#endif          
            );
#ifdef DEBUG_SURFACE_PLACEMENT
        csd.ReCreateZBuffer(pZB);
#endif
#ifdef  STEREO_SUPPORT
            pZB->createStereo();
#endif  //STEREO_SUPPORT
    }
}

PNVD3DCONTEXT CKelvinAAState::GetContext(CSimpleSurface *pSurf) {
    int i;

    PNVD3DCONTEXT pContext = (PNVD3DCONTEXT)getDC()->dwContextListHead;

    while (pContext) {
        if (pSurf == pContext->pZetaBuffer) return pContext;

        for (i=0; i<KELVIN_AA_NUM_FLIP_CHAIN_SLOTS; i++) {
            if (pSurf == pContext->kelvinAA._flipChain[i]) return pContext;
        }

        pContext = pContext->pContextNext;
    }

    return 0;
}


void CKelvinAAState::EndScene() {
#ifdef WINNT
    if (_appCompatibility.dwFlags & APPFLAGS_WRITE_ACCESS_ON_ENDSCENE) {
        GrantAccess(CKelvinAAState::BUFFER_RT, CKelvinAAState::ACCESS_WRITE, TRUE);
    } else
#endif
    {
        GrantAccess(CKelvinAAState::BUFFER_RT, CKelvinAAState::ACCESS_READ, TRUE);
    }
}

void CKelvinAAState::UpdateZBuffer() {
    if (!IsEnabled()) return;

    _bufferInfo[BUFFER_ZB].pSurface = _pContext->pZetaBuffer;
}

void CKelvinAAState::UpdateRenderTarget() {
    if (!IsEnabled()) return;

    CSimpleSurface *pRT = _bufferInfo[BUFFER_RT].pSurface;
    CSimpleSurface *pCRT = _pContext->pRenderTarget;

    if (pCRT && pRT && (pCRT->getWidth() != pRT->getWidth()) && (pCRT->getHeight() != pRT->getHeight())) {
        Refresh();
    }

    _bufferInfo[BUFFER_RT].pSurface = _pContext->pRenderTarget;

    int i;

    for (i=0; i<KELVIN_AA_NUM_FLIP_CHAIN_SLOTS; i++) {
        if (_flipChain[i] == _pContext->pRenderTarget) break;
    }

    if (i == KELVIN_AA_NUM_FLIP_CHAIN_SLOTS) {
        for (i=0; i<KELVIN_AA_NUM_FLIP_CHAIN_SLOTS; i++) if (!_flipChain[i]) break;
        nvAssert(i < KELVIN_AA_NUM_FLIP_CHAIN_SLOTS);
        _flipChain[i] = _pContext->pRenderTarget;
    }
}

void CKelvinAAState::GrantAccess(BufferType buffer, AccessType access, bool bConvolve, bool bFlush) {
#ifdef DEBUG
    nvAssert(buffer < BUFFER_MAX);
    nvAssert(access < ACCESS_MAX);
    DPF_LEVEL(NVDBG_LEVEL_AA_INFO, 
        "CKelvinAAState::GrantAccess(%s, %s), address: %x   %d%d %d%d", 
        BufferTypeNames[buffer], 
        AccessTypeNames[access],
	_bufferInfo[buffer].pSurface,
        !!(_bufferInfo[BUFFER_RT].flags & FLAGS_VALID),
        !!(_bufferInfo[BUFFER_SRT].flags & FLAGS_VALID),
        !!(_bufferInfo[BUFFER_ZB].flags & FLAGS_VALID),
        !!(_bufferInfo[BUFFER_SZB].flags & FLAGS_VALID));
#endif

    // Return if AA is not on
    if (!IsEnabled()) return;
    
    //exit GrantAccess if we have recursed here.
    if (InGrantAccess()) return;
    EnterGrantAccess();

    nvAssert(buffer < BUFFER_MAX);
    nvAssert(access < ACCESS_MAX);

    UpdateRenderTarget();

    BufferType other = _bufferInfo[buffer].other;

    if ((access == ACCESS_WRITE) &&
        _lastDownsampleUsedConvolution &&
        (buffer == BUFFER_RT)) {
        // superbuffer better be valid
        nvAssert(_bufferInfo[other].flags & FLAGS_VALID);
        _bufferInfo[buffer].flags &= ~FLAGS_VALID;
        _appCompatibility.dwFlags |= APPFLAGS_DISABLE_DITHER;
    }

    // We deallocated the normal Z buffer for performance reasons
    // If we desire access, then we have to reallocate it.
    CSimpleSurface *pZB = _bufferInfo[BUFFER_ZB].pSurface;
    if (buffer == BUFFER_ZB) ReCreateZBuffer();

    //at aa creation time, the z buffer was disabled. later, the app
    //enabled it and now we have to write to the super z, so create one.
    if (_bufferInfo[BUFFER_ZB].pSurface && !_bufferInfo[BUFFER_SZB].pSurface && buffer == BUFFER_SZB)
    {
        const BufferBltInfoStruct *bltInfo = &BufferBltInfo[MethodInfo[_method].type];

        if (!(_bufferInfo[BUFFER_SZB].pSurface = new CSimpleSurface())) {
            Destroy();
            ExitGrantAccess();
            return;
        }

        DWORD dwPitch = pZB->getWidth() * bltInfo->dwSuperBufferScaleX * pZB->getBPP();
        dwPitch = (dwPitch + getDC()->dwSurfaceAlignPad) & ~getDC()->dwSurfaceAlignPad;
        
        if (!_bufferInfo[BUFFER_SZB].pSurface->create(NULL,
            pZB->getFormat(),
            pZB->getWidth()  * bltInfo->dwSuperBufferScaleX,
            pZB->getHeight() * bltInfo->dwSuperBufferScaleY,
            pZB->getDepth(),
            1,
            pZB->getBPPRequested(),
            pZB->getBPP(),
            dwPitch,
            CSimpleSurface::HEAP_VID,
            CSimpleSurface::HEAP_VID,
            CSimpleSurface::ALLOCATE_TILED | CSimpleSurface::ALLOCATE_AS_ZBUFFER_CMP
#ifdef CAPTURE
            ,CAPTURE_SURFACE_KIND_ZETA
#endif
            ))
        {
            Destroy();
            ExitGrantAccess();
            return;
        }
#ifdef  STEREO_SUPPORT
        _bufferInfo[BUFFER_SZB].pSurface->createStereo();
#endif  //STEREO_SUPPORT
        CNvObject *pWrapper = new CNvObject(0);
        pWrapper->setObject(CNvObject::NVOBJ_SIMPLESURFACE, (void *)_bufferInfo[BUFFER_SZB].pSurface);
        _bufferInfo[BUFFER_SZB].pSurface->setWrapper(pWrapper);
    }
    
    switch (access) {
    case ACCESS_READ:
        if (!(_bufferInfo[buffer].flags & FLAGS_VALID)) {
            ScaledBlt(buffer, other, bConvolve, bFlush);
            _bufferInfo[buffer].flags |= FLAGS_VALID;
        }

        break;
    case ACCESS_WRITE:
        if (!(_bufferInfo[buffer].flags & FLAGS_VALID)) {
            // we never want to give write access to a blur filtered buffer
            ScaledBlt(buffer, other, bConvolve, bFlush);
            _bufferInfo[buffer].flags |= FLAGS_VALID;
        }
        _bufferInfo[other].flags &= ~FLAGS_VALID;

        break;
    case ACCESS_WRITE_DISCARD:
        // Don't have to perform any blts; we don't care about the results
        _bufferInfo[buffer].flags |= FLAGS_VALID;
        _bufferInfo[other].flags &= ~FLAGS_VALID;

        break;
    default:
        DPF("Invalid access type in CKelvinAAState::GrantAccess()");
        nvAssert(0);

        break;
    };

    ExitGrantAccess();
    return;
}

void CKelvinAAState::ScaledBlt(BufferType dst, BufferType src, bool bConvolve, bool bFlush) {
    DPF_LEVEL(NVDBG_LEVEL_AA_INFO, "CKelvinAAState::ScaledBlt(%s, %s)", BufferTypeNames[dst], BufferTypeNames[src]);
    nvAssert(_enabled && _pContext);

    BufferInfo *dstInfo = &_bufferInfo[dst];
    BufferInfo *srcInfo = &_bufferInfo[src];
    const BufferBltInfoStruct *bltInfo = &BufferBltInfo[MethodInfo[src].type];

    BufferInfo *tempInfo = (bltInfo->dwScratchBufferScaleX) ? &_bufferInfo[BUFFER_SCRATCH] : 0;

    bConvolve = bConvolve && MethodInfo[_method].dwKernel; // and out the filter if we're not using convolution

    if (!srcInfo->pSurface || !dstInfo->pSurface) return;
    if (_method == SIMPLEMETHOD_1X1) return;

    nvAssert(dstInfo->pSurface);
    nvAssert(srcInfo->pSurface);

    DWORD dwMethod = _method;
    bool bSubdivideAA = (bool)_bSubdivideAA;
    bool bDither = !(_appCompatibility.dwFlags & APPFLAGS_DISABLE_DITHER);
    bool bZBuffer = (src == BUFFER_ZB) || (src == BUFFER_SZB);

    if ((dstInfo->pSurface->getWidth() < srcInfo->pSurface->getWidth()) ||
        (dstInfo->pSurface->getHeight() < srcInfo->pSurface->getHeight())) {
        if (!bZBuffer) {
            if (_didUpsample) {
                bDither = false;
                _didUpsample = false;
            }
    
            if (bConvolve) {
                _lastDownsampleUsedConvolution = true;
            } else {
                _lastDownsampleUsedConvolution = false;
                dwMethod = MethodInfo[_method].dwBoxFilteredAlternative;
            }
        }
    } else {
        // upsampling
        bSubdivideAA = false;
        bDither = false;
        _didUpsample = true;
        dwMethod = MethodInfo[_method].dwBoxFilteredAlternative;
    }

    if (!tempInfo) {
#ifndef SCALED_IMAGE
#ifdef  STEREO_SUPPORT
        if (STEREO_ACTIVATED)
        {
			//We can't mess up the pStereoData->dwLastEye or use SetupStereoContext
			//for that matter. Those are higher level primitives.
			DWORD dwLastEye = pStereoData->dwLastEye;
			pStereoData->dwLastEye = EYE_LEFT;
            ImageModeTexBlt(dstInfo->pSurface, srcInfo->pSurface, dwMethod, bSubdivideAA, bDither, bFlush);
            pStereoData->dwLastEye = EYE_RIGHT;
            ImageModeTexBlt(dstInfo->pSurface, srcInfo->pSurface, dwMethod, bSubdivideAA, bDither, bFlush);
			pStereoData->dwLastEye = dwLastEye;
        } else
            ImageModeTexBlt(dstInfo->pSurface, srcInfo->pSurface, dwMethod, bSubdivideAA, bDither, bFlush);
#else   //STEREO_SUPPORT==0
        ImageModeTexBlt(dstInfo->pSurface, srcInfo->pSurface, dwMethod, bSubdivideAA, bDither, bFlush);
#endif  //STEREO_SUPPORT
#else
        DWORD dstWidth  = dstInfo->pSurface->getWidth();
        DWORD dstHeight = dstInfo->pSurface->getHeight();
        DWORD srcWidth  = srcInfo->pSurface->getWidth();
        DWORD srcHeight = srcInfo->pSurface->getHeight();
        
        if (dstWidth >= 1024) {
            nvAssert(!(dstWidth & 0x1));
            
            ScaledBlt(
                dstInfo->pSurface, 0, 0, dstWidth/2, dstInfo->pSurface->getHeight(), 
                srcInfo->pSurface, bltInfo->fDownscaleOffsetX1, bltInfo->fDownscaleOffsetY1, srcWidth/2, srcInfo->pSurface->getHeight() 
                );
            ScaledBlt(
                dstInfo->pSurface, dstWidth/2, 0, dstWidth/2, dstInfo->pSurface->getHeight(), 
                srcInfo->pSurface, bltInfo->fDownscaleOffsetX1+(srcWidth/2), bltInfo->fDownscaleOffsetY1, srcWidth/2, srcInfo->pSurface->getHeight() 
                );
        } else {
            ScaledBlt(
                dstInfo->pSurface, 0, 0, dstInfo->pSurface->getWidth(), dstInfo->pSurface->getHeight(), 
                srcInfo->pSurface, bltInfo->fDownscaleOffsetX1, bltInfo->fDownscaleOffsetY1, srcInfo->pSurface->getWidth(), srcInfo->pSurface->getHeight() 
                );
        }
#endif
    } else {
        nvAssert(0);
        // We don't handle this case yet
    }
}

void CKelvinAAState::ImageModeTexBlt(
    CSimpleSurface *pDst, 
    CSimpleSurface *pSrc,
    DWORD dwMethod,
    bool bSubdivideAA,
    bool bDither,
    bool bFlush
) {
    DWORD i;
    DWORD dwSimpleMethod = MethodInfo[dwMethod].type;
    DWORD dwSrcFormat = pSrc->getFormat();
    DWORD dwDstFormat = pDst->getFormat();
    bool bZBuffer = false;

    if (!_bInitializedState) {
        _pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_REALLY_FILTHY;
        nvSetKelvinState(_pContext);
	    // THIS IS KIND OF UGLY NOW because we moved the GrantAccess call into nvSetKelvinState
	    // thus ImageModeTexBlt could call itself recursively.  Make sure you set _bInitializedState=true
	    // BEFORE calling nvSetKelvinState here or you will fall into an infinite loop
        _bInitializedState = true;	//<------------------- DO NOT DELETE
    }

    bool bUpscaling = (pDst->getWidth() > pSrc->getWidth()) || (pDst->getHeight() > pSrc->getHeight());


    // Spoof the src format
    switch (dwSrcFormat) {
    case NV_SURFACE_FORMAT_Z16:
        bZBuffer = true;
        bDither = false;
    case NV_SURFACE_FORMAT_R5G6B5:
    case NV_SURFACE_FORMAT_IMAGE_R5G6B5:
        dwSrcFormat = NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G6B5;
        break;
    case NV_SURFACE_FORMAT_Z24S8:
    case NV_SURFACE_FORMAT_Z24X8:
        bZBuffer = true;
        bDither = false;
    case NV_SURFACE_FORMAT_X8R8G8B8:
    case NV_SURFACE_FORMAT_A8R8G8B8:
    case NV_SURFACE_FORMAT_IMAGE_A8R8G8B8:
        dwSrcFormat = NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8;
        break;
    default:
        // Invalid surface format
        nvAssert(0);
        return;
    }

    // Spoof the dst format
    switch (dwDstFormat) {
    case NV_SURFACE_FORMAT_X1R5G5B5:
        dwDstFormat = NV097_SET_SURFACE_FORMAT_COLOR_LE_X1R5G5B5_Z1R5G5B5;
        break;
    case NV_SURFACE_FORMAT_Z16:
    case NV_SURFACE_FORMAT_R5G6B5:
    case NV_SURFACE_FORMAT_IMAGE_R5G6B5:
        dwDstFormat = NV097_SET_SURFACE_FORMAT_COLOR_LE_R5G6B5;
        break;
    case NV_SURFACE_FORMAT_Z24S8:
    case NV_SURFACE_FORMAT_Z24X8:
    case NV_SURFACE_FORMAT_X8R8G8B8:
    case NV_SURFACE_FORMAT_A8R8G8B8:
    case NV_SURFACE_FORMAT_IMAGE_A8R8G8B8:
        dwDstFormat = NV097_SET_SURFACE_FORMAT_COLOR_LE_A8R8G8B8;
        break;
    case NV_SURFACE_FORMAT_X1A7R8G8B8:
        dwDstFormat = NV097_SET_SURFACE_FORMAT_COLOR_LE_X1A7R8G8B8_Z1A7R8G8B8;
        break;
    default:
        // don't worry, we're not catching all the cases
        break;
    }


    struct filterTypeStruct {
        bool bEmulateFilter;
        DWORD dwNumTextures;

        struct {
            float fXOffset;
            float fYOffset;
        } texStageInfo[4];

        DWORD dwNumCombinerStages;

        struct {
            DWORD dwICW;
            DWORD dwOCW;
            DWORD dwFactor0;
            DWORD dwFactor1;
        } combinerInfo[4];
    };

    float fCenterContrib = 0.25; // Contribution of center texel in Gaussian filter.
    float fSqrtCenterContrib = float(sqrt(fCenterContrib));

    DWORD dwCombinerCountInc = 0;

#ifdef _16_BIT_FIX
    if ((pSrc->getBPP() == 2) && !bUpscaling && bDither) dwCombinerCountInc = 1;
#endif

    static const filterTypeStruct filterType[] = {
        {   // ordinary filter - box, standard gaussian, standard quincunx
            false,
            1, 
            {{0.0, 0.0},
             {0.0, 0.0},
             {0.0, 0.0},
             {0.0, 0.0}},
            1+dwCombinerCountInc, 
            {{0x08200000, 0x00000c00, 0x00000000, 0x00000000},
             {0x0c010000, 0x00000c00, 0x00f7fbf7, 0x00000000},
             {0x00000000, 0x00000000, 0x00000000, 0x00000000},
             {0x00000000, 0x00000000, 0x00000000, 0x00000000}}
        }, { // emulated gaussian
            true,
            4, 
            {{-1+fSqrtCenterContrib, -1+fSqrtCenterContrib},
             { 1-fSqrtCenterContrib, -1+fSqrtCenterContrib},
             {-1+fSqrtCenterContrib,  1-fSqrtCenterContrib},
             { 1-fSqrtCenterContrib,  1-fSqrtCenterContrib}},
            3+dwCombinerCountInc, 
            {{0x08010902, 0x00000C00, 0x7f7f7f7f, 0x7f7f7f7f},
             {0x0c010a02, 0x00000C00, 0x7f7f7f7f, 0x3f3f3f3f},
             {0x0c010b02, 0x00000C00, 0xffffffff, 0x3f3f3f3f},
             {0x0c010000, 0x00000c00, 0x00f7fbf7, 0x00000000}}
        
        },  { //9 tap emulated gaussian 
            true,
            4, 
            {{0.66+0.5, 0.5+0.5},  //.75 8
             {1.33+0.5, 0.5+0.5},  //.75 9
             {0+0.5   , 1.5+0.5},  //.25 a
             {2+0.5   , 1.5+0.5}}, //.25 b
            4, 
            {{0x48014a02, 0x00000c00, 0xBFBFBFBF, 0x3f3f3f3f},
             {0x49014b02, 0x00000d00, 0xBFBFBFBF, 0x3f3f3f3f},
             {0xcc01cd02, 0x00000C00, 0x7f7f7f7f, 0x7f7f7f7f},
             {0xcc01c102, 0x00000C00, 0x7f7f7f7f, 0xFFFFFFFF}}
        },  { //
            true,
            2, 
            {{0.5,      0.0},
             {-0.5,      0.5},
             {0.0,      0.0},
             {0.0,      0.0}},
            1+dwCombinerCountInc, 
            {{0x08010902, 0x00000c00, 0x80808080, 0x7f7f7f7f},
             {0x0c010000, 0x00000c00, 0x00f7fbf7, 0x00000000},
             {0x00000000, 0x00000000, 0x00000000, 0x00000000},
             {0x00000000, 0x00000000, 0x00000000, 0x00000000}}
        }
    };
    
    DWORD dwFilterType;

    switch (dwMethod) {
//    case METHOD_MULTISAMPLE_2X_QUINCUNX:       dwFilterType = 3;   break;
    case METHOD_MULTISAMPLE_4X_ROTATED_8TAP:   dwFilterType = 2;   break;
    default:                                   dwFilterType = 0;   break; 
    }

    DWORD dwShaderStageProgram = 0;
    for (i=0; i<filterType[dwFilterType].dwNumTextures; i++) {
        dwShaderStageProgram |= DRF_DEF(097, _SET_SHADER_STAGE_PROGRAM, _STAGE0, _2D_PROJECTIVE) << (5*i);
    }

    _pContext->hwState.kelvin.set1(NV097_SET_SHADER_STAGE_PROGRAM, dwShaderStageProgram);

    _pContext->hwState.kelvin.set1(NV097_SET_DOT_RGBMAPPING,
        DRF_DEF(097, _SET_DOT_RGBMAPPING, _STAGE1, _ZERO_TO_1) |
        DRF_DEF(097, _SET_DOT_RGBMAPPING, _STAGE2, _ZERO_TO_1) |
        DRF_DEF(097, _SET_DOT_RGBMAPPING, _STAGE3, _ZERO_TO_1));

#ifdef SUBDIVIDE_AA
    _pContext->hwState.kelvin.set1(NV097_SET_SWATH_WIDTH, _strip.swath);
#else
    if (dwMethod <= METHOD_MULTISAMPLE_2X_QUINCUNX)
        _pContext->hwState.kelvin.set1(NV097_SET_SWATH_WIDTH, NV097_SET_SWATH_WIDTH_V_32);
    else if (dwMethod < METHOD_MULTISAMPLE_4X_GAUSSIAN)
        _pContext->hwState.kelvin.set1(NV097_SET_SWATH_WIDTH, NV097_SET_SWATH_WIDTH_V_128);
    else 
        _pContext->hwState.kelvin.set1(NV097_SET_SWATH_WIDTH, NV097_SET_SWATH_WIDTH_V_OFF);
#endif

    // dithering
    _pContext->hwState.kelvin.set1(NV097_SET_DITHER_ENABLE, bDither ? NV097_SET_DITHER_ENABLE_V_TRUE : NV097_SET_DITHER_ENABLE_V_FALSE);

    _pContext->hwState.kelvin.set1(NV097_SET_SHADER_CLIP_PLANE_MODE, 0);
    _pContext->hwState.kelvin.set1(NV097_SET_SHADER_OTHER_STAGE_INPUT, 0);
    _pContext->hwState.kelvin.set1(NV097_SET_FLAT_SHADE_OP, NV097_SET_FLAT_SHADE_OP_V_LAST_VTX);
    _pContext->hwState.kelvin.set1(NV097_SET_LOGIC_OP_ENABLE, NV097_SET_LOGIC_OP_ENABLE_V_FALSE);
    _pContext->hwState.kelvin.set1(NV097_SET_ZMIN_MAX_CONTROL,
        DRF_DEF(097, _SET_ZMIN_MAX_CONTROL, _CULL_IGNORE_W, _TRUE)   |
        DRF_DEF(097, _SET_ZMIN_MAX_CONTROL, _ZCLAMP_EN, _CLAMP)      |
        DRF_DEF(097, _SET_ZMIN_MAX_CONTROL, _CULL_NEAR_FAR_EN, _FALSE));

    _pContext->hwState.kelvin.set1(NV097_SET_ANTI_ALIASING_CONTROL, (0xFFFF0000) | NV097_SET_ANTI_ALIASING_CONTROL_ENABLE_FALSE);
    _pContext->hwState.kelvin.set1(NV097_SET_COMPRESS_ZBUFFER_EN, NV097_SET_COMPRESS_ZBUFFER_EN_V_DISABLE);
    _pContext->hwState.kelvin.set1(NV097_SET_OCCLUDE_ZSTENCIL_EN, NV097_SET_OCCLUDE_ZSTENCIL_EN_OCCLUDE_ZEN_DISABLE);
    _pContext->hwState.kelvin.set1(NV097_SET_TWO_SIDE_LIGHT_EN, NV097_SET_TWO_SIDE_LIGHT_EN_V_FALSE);
    _pContext->hwState.kelvin.set1(NV097_SET_ZPASS_PIXEL_COUNT_ENABLE, NV097_SET_ZPASS_PIXEL_COUNT_ENABLE_V_FALSE);

    // deactivate all but the first clip rectangle
    for (DWORD dwWindow=1; dwWindow < KELVIN_NUM_WINDOWS; dwWindow++) {
        _pContext->hwState.kelvin.set1 (NV097_SET_WINDOW_CLIP_HORIZONTAL(dwWindow), 0);
        _pContext->hwState.kelvin.set1 (NV097_SET_WINDOW_CLIP_VERTICAL(dwWindow), 0);
    }


    _pContext->hwState.kelvin.set1 (NV097_SET_WINDOW_CLIP_HORIZONTAL(0), 
        DRF_NUM(097, _SET_WINDOW_CLIP_HORIZONTAL, _XMIN, 0) |
        DRF_NUM(097, _SET_WINDOW_CLIP_HORIZONTAL, _XMAX, pDst->getWidth()-1));

    _pContext->hwState.kelvin.set1 (NV097_SET_WINDOW_CLIP_VERTICAL(0), 
        DRF_NUM(097, _SET_WINDOW_CLIP_VERTICAL, _YMIN, 0) |
        DRF_NUM(097, _SET_WINDOW_CLIP_VERTICAL, _YMAX, pDst->getHeight()-1));

    ////////////////////////////////////////

    _pContext->hwState.kelvin.set8(NV097_SET_VERTEX_DATA_ARRAY_OFFSET(0), 0, 0, 0, 0, 0, 0, 0, 0);
    _pContext->hwState.kelvin.set8(NV097_SET_VERTEX_DATA_ARRAY_OFFSET(8), 0, 8, 16, 24, 32, 0, 0, 0);

    _pContext->hwState.kelvin.set8(NV097_SET_VERTEX_DATA_ARRAY_FORMAT(0),
        DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _2)        | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F),
        DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _DISABLED) | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F),
        DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _DISABLED) | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F),
        DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _DISABLED) | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F),
        DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _DISABLED) | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F),
        DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _DISABLED) | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F),
        DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _DISABLED) | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F),
        DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _DISABLED) | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F));

    _pContext->hwState.kelvin.set8(NV097_SET_VERTEX_DATA_ARRAY_FORMAT(8),
        DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _DISABLED) | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F),
        DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _2)        | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F),
        DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _2)        | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F),
        DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _2)        | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F),
        DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _2)        | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F),
        DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _DISABLED) | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F),
        DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _DISABLED) | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F),
        DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _DISABLED) | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F));

    _pContext->hwState.kelvin.set1(NV097_SET_CULL_FACE_ENABLE, NV097_SET_CULL_FACE_ENABLE_V_FALSE);

    /////////////////////////////////////////////////////

    DWORD dwFormat = DRF_NUM(097, _SET_SURFACE_FORMAT, _COLOR, dwDstFormat);

    if (pDst->isSwizzled()) {
        CTexture* pTexture = pDst->getWrapper()->getTexture();
        nvAssert(pTexture);
        dwFormat |= DRF_DEF(097, _SET_SURFACE_FORMAT, _TYPE, _SWIZZLE);
        dwFormat |= DRF_NUM(097, _SET_SURFACE_FORMAT, _WIDTH,  pTexture->getLogWidth());
        dwFormat |= DRF_NUM(097, _SET_SURFACE_FORMAT, _HEIGHT, pTexture->getLogHeight());
    } else {
        dwFormat |= DRF_DEF(097, _SET_SURFACE_FORMAT, _TYPE, _PITCH);
    }

    dwFormat |= DRF_NUM(097, _SET_SURFACE_FORMAT, _ZETA, 
        (pDst->getBPP() == 2) ? NV097_SET_SURFACE_FORMAT_ZETA_Z16 : NV097_SET_SURFACE_FORMAT_ZETA_Z24S8);

    _pContext->hwState.kelvin.set3(NV097_SET_SURFACE_PITCH,
                                 (DRF_NUM (097, _SET_SURFACE_PITCH, _COLOR, pDst->getPitch()) |
                                  DRF_NUM (097, _SET_SURFACE_PITCH, _ZETA, KELVIN_ALIGNMENT_MIN)),
#ifdef  STEREO_SUPPORT
                                  GetStereoOffset(pDst),
#else   //STEREO_SUPPORT==0
                                  pDst->getOffset(),
#endif  //STEREO_SUPPORT
                                  0);

    _pContext->hwState.kelvin.set1 (NV097_SET_SURFACE_FORMAT, dwFormat);

    _pContext->hwState.kelvin.set2 (NV097_SET_SURFACE_CLIP_HORIZONTAL,
        DRF_NUM(097, _SET_SURFACE_CLIP_HORIZONTAL, _X, 0) |
        DRF_NUM(097, _SET_SURFACE_CLIP_HORIZONTAL, _WIDTH, pDst->getWidth()),
        DRF_NUM(097, _SET_SURFACE_CLIP_VERTICAL, _Y, 0) |
        DRF_NUM(097, _SET_SURFACE_CLIP_VERTICAL, _HEIGHT, pDst->getHeight()));

    ///////////////////////////////////////////////////////

    for (i=0; i<filterType[dwFilterType].dwNumTextures; i++) {
        _pContext->hwState.kelvin.set1(NV097_SET_TEXTURE_FORMAT(i),
            DRF_NUM(097, _SET_TEXTURE_FORMAT, _MIPMAP_LEVELS, 1)                               |
            DRF_NUM(097, _SET_TEXTURE_FORMAT, _COLOR, dwSrcFormat)                             |
            DRF_DEF(097, _SET_TEXTURE_FORMAT, _DIMENSIONALITY, _TWO)                           |
            DRF_DEF(097, _SET_TEXTURE_FORMAT, _BORDER_SOURCE, _COLOR)                          |
            DRF_DEF(097, _SET_TEXTURE_FORMAT, _CUBEMAP_ENABLE, _FALSE)                         |
            DRF_NUM(097, _SET_TEXTURE_FORMAT, _CONTEXT_DMA, nv097TextureContextDma[pSrc->getContextDMA()]));

        _pContext->hwState.kelvin.set1(NV097_SET_TEXTURE_CONTROL0(i),
            DRF_DEF(097, _SET_TEXTURE_CONTROL0, _ENABLE, _TRUE)              |
            DRF_NUM(097, _SET_TEXTURE_CONTROL0, _MIN_LOD_CLAMP, 0)           |
            DRF_NUM(097, _SET_TEXTURE_CONTROL0, _MAX_LOD_CLAMP, 0)           |
            DRF_DEF(097, _SET_TEXTURE_CONTROL0, _LOG_MAX_ANISO, _0)          |
            DRF_DEF(097, _SET_TEXTURE_CONTROL0, _IMAGE_FIELD_ENABLE, _FALSE) |
            DRF_DEF(097, _SET_TEXTURE_CONTROL0, _ALPHA_KILL_ENABLE, _FALSE)  |
            DRF_DEF(097, _SET_TEXTURE_CONTROL0, _COLOR_KEY_OPERATION, _FALSE));

        _pContext->hwState.kelvin.set1(NV097_SET_TEXTURE_CONTROL1(i),
            DRF_NUM(097, _SET_TEXTURE_CONTROL1, _IMAGE_PITCH, pSrc->getPitch()));

        _pContext->hwState.kelvin.set1(NV097_SET_TEXTURE_ADDRESS(i),
            DRF_DEF(097, _SET_TEXTURE_ADDRESS, _CYLWRAP_Q, _FALSE) |
            DRF_DEF(097, _SET_TEXTURE_ADDRESS, _CYLWRAP_P, _FALSE) |
            DRF_DEF(097, _SET_TEXTURE_ADDRESS, _CYLWRAP_V, _FALSE) |
            DRF_DEF(097, _SET_TEXTURE_ADDRESS, _CYLWRAP_U, _FALSE) |
            DRF_DEF(097, _SET_TEXTURE_ADDRESS, _U, _CLAMP_TO_EDGE) |
            DRF_DEF(097, _SET_TEXTURE_ADDRESS, _V, _CLAMP_TO_EDGE) |
            DRF_DEF(097, _SET_TEXTURE_ADDRESS, _P, _CLAMP_TO_EDGE));
        
        if (bUpscaling || bZBuffer) {
            // We're upscaling
            _pContext->hwState.kelvin.set1(NV097_SET_TEXTURE_FILTER(i),
                DRF_DEF(097, _SET_TEXTURE_FILTER, _BSIGNED, _BIT_DISABLED) |
                DRF_DEF(097, _SET_TEXTURE_FILTER, _GSIGNED, _BIT_DISABLED) |
                DRF_DEF(097, _SET_TEXTURE_FILTER, _RSIGNED, _BIT_DISABLED) |
                DRF_DEF(097, _SET_TEXTURE_FILTER, _ASIGNED, _BIT_DISABLED) |
                DRF_DEF(097, _SET_TEXTURE_FILTER, _MAG, _BOX_LOD0)         |
                DRF_DEF(097, _SET_TEXTURE_FILTER, _MIN, _BOX_LOD0)         |
                DRF_DEF(097, _SET_TEXTURE_FILTER, _CONVOLUTION_KERNEL, _GAUSSIAN_3));
        } else if (MethodInfo[dwMethod].dwKernel && !filterType[dwFilterType].bEmulateFilter) {
            _pContext->hwState.kelvin.set1(NV097_SET_TEXTURE_FILTER(i),
                DRF_DEF(097, _SET_TEXTURE_FILTER, _BSIGNED, _BIT_DISABLED)    |
                DRF_DEF(097, _SET_TEXTURE_FILTER, _GSIGNED, _BIT_DISABLED)    |
                DRF_DEF(097, _SET_TEXTURE_FILTER, _RSIGNED, _BIT_DISABLED)    |
                DRF_DEF(097, _SET_TEXTURE_FILTER, _ASIGNED, _BIT_DISABLED)    |
                DRF_DEF(097, _SET_TEXTURE_FILTER, _MAG, _CONVOLUTION_2D_LOD0) |
                DRF_DEF(097, _SET_TEXTURE_FILTER, _MIN, _CONVOLUTION_2D_LOD0) |
                DRF_NUM(097, _SET_TEXTURE_FILTER, _CONVOLUTION_KERNEL, MethodInfo[dwMethod].dwKernel));
        } else {
            // We're downscaling
            _pContext->hwState.kelvin.set1(NV097_SET_TEXTURE_FILTER(i),
                DRF_DEF(097, _SET_TEXTURE_FILTER, _BSIGNED, _BIT_DISABLED) |
                DRF_DEF(097, _SET_TEXTURE_FILTER, _GSIGNED, _BIT_DISABLED) |
                DRF_DEF(097, _SET_TEXTURE_FILTER, _RSIGNED, _BIT_DISABLED) |
                DRF_DEF(097, _SET_TEXTURE_FILTER, _ASIGNED, _BIT_DISABLED) |
                DRF_DEF(097, _SET_TEXTURE_FILTER, _MAG, _TENT_LOD0)        |
                DRF_DEF(097, _SET_TEXTURE_FILTER, _MIN, _TENT_LOD0)        |
                DRF_DEF(097, _SET_TEXTURE_FILTER, _CONVOLUTION_KERNEL, _GAUSSIAN_3));
        }
    
#ifdef  STEREO_SUPPORT
        _pContext->hwState.kelvin.set1(NV097_SET_TEXTURE_OFFSET(i), GetStereoOffset(pSrc));
#else   //STEREO_SUPPORT==0
        _pContext->hwState.kelvin.set1(NV097_SET_TEXTURE_OFFSET(i), pSrc->getOffset());
#endif  //STEREO_SUPPORT

        // we subtract the X and Y scaling factors here, then c
//        _pContext->hwState.kelvin.set1(NV097_SET_TEXTURE_IMAGE_RECT(i), 
//            DRF_NUM(097, _SET_TEXTURE_IMAGE_RECT, _WIDTH, pSrc->getWidth()-BufferBltInfo[dwSimpleMethod].dwSuperBufferScaleX) |
//            DRF_NUM(097, _SET_TEXTURE_IMAGE_RECT, _HEIGHT, pSrc->getHeight()-BufferBltInfo[dwSimpleMethod].dwSuperBufferScaleY));
        _pContext->hwState.kelvin.set1(NV097_SET_TEXTURE_IMAGE_RECT(i), 
            DRF_NUM(097, _SET_TEXTURE_IMAGE_RECT, _WIDTH, pSrc->getWidth()) |
            DRF_NUM(097, _SET_TEXTURE_IMAGE_RECT, _HEIGHT, pSrc->getHeight()));

        _pContext->hwState.kelvin.set4(NV097_SET_TEXGEN_S(i),
            NV097_SET_TEXGEN_S_V_DISABLE,
            NV097_SET_TEXGEN_T_V_DISABLE,
            NV097_SET_TEXGEN_R_V_DISABLE,
            NV097_SET_TEXGEN_Q_V_DISABLE);

        _pContext->hwState.kelvin.set1(NV097_SET_TEXTURE_MATRIX_ENABLE(i), 
            DRF_DEF(097, _SET_TEXTURE_MATRIX_ENABLE, _V, _FALSE));
    }


    ///////////////////////////////////////////////////////

    _pContext->hwState.kelvin.set1(NV097_SET_LIGHTING_ENABLE, NV097_SET_LIGHTING_ENABLE_V_FALSE);
    _pContext->hwState.kelvin.set1(NV097_SET_DEPTH_TEST_ENABLE, NV097_SET_DEPTH_TEST_ENABLE_V_FALSE);
    _pContext->hwState.kelvin.set2(NV097_SET_FRONT_POLYGON_MODE, NV097_SET_FRONT_POLYGON_MODE_V_FILL, NV097_SET_BACK_POLYGON_MODE_V_FILL);
    _pContext->hwState.kelvin.set1(NV097_SET_BLEND_ENABLE, NV097_SET_BLEND_ENABLE_V_FALSE);
    _pContext->hwState.kelvin.set1(NV097_SET_ALPHA_TEST_ENABLE, NV097_SET_ALPHA_TEST_ENABLE_V_FALSE);
    _pContext->hwState.kelvin.set1(NV097_SET_STENCIL_TEST_ENABLE, NV097_SET_STENCIL_TEST_ENABLE_V_FALSE);
    _pContext->hwState.kelvin.set1(NV097_SET_FOG_ENABLE, NV097_SET_FOG_ENABLE_V_FALSE);
#ifdef KPFS
    getDC()->nvKelvinPM.download_special(_pContext,&_program,0xFFFFFFFE);
#else
    _pContext->hwState.kelvin.set1(NV097_SET_TRANSFORM_EXECUTION_MODE,
         DRF_DEF(097, _SET_TRANSFORM_EXECUTION_MODE, _MODE, _PROGRAM) |
         DRF_DEF(097, _SET_TRANSFORM_EXECUTION_MODE, _RANGE_MODE, _PRIV));

    _pContext->hwState.kelvin.set1(NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN,
         DRF_DEF(097, _SET_TRANSFORM_PROGRAM_CXT_WRITE_EN, _V, _READ_ONLY));

    nvKelvinDownloadProgram(_pContext, &_program,0);

    _pContext->hwState.kelvin.set1 (NV097_SET_TRANSFORM_PROGRAM_START, 0);
#endif
    _pContext->hwState.kelvin.set1(NV097_INVALIDATE_VERTEX_CACHE_FILE, 0);

    /////////////////////////////////////////////////////////

    _pContext->hwState.kelvin.set1 (NV097_SET_COMBINER_CONTROL, 
        DRF_NUM(097, _SET_COMBINER_CONTROL, _ITERATION_COUNT, filterType[dwFilterType].dwNumCombinerStages)         |
        DRF_DEF(097, _SET_COMBINER_CONTROL, _MUX_SELECT, _LSB)           |
        DRF_DEF(097, _SET_COMBINER_CONTROL, _FACTOR0, _EACH_STAGE)       |
        DRF_DEF(097, _SET_COMBINER_CONTROL, _FACTOR1, _EACH_STAGE));

    for (i=0; i<filterType[dwFilterType].dwNumCombinerStages; i++) {
        _pContext->hwState.kelvin.set1 (NV097_SET_COMBINER_COLOR_ICW(i), filterType[dwFilterType].combinerInfo[i].dwICW);
        _pContext->hwState.kelvin.set1 (NV097_SET_COMBINER_COLOR_OCW(i), filterType[dwFilterType].combinerInfo[i].dwOCW);
        _pContext->hwState.kelvin.set1 (NV097_SET_COMBINER_ALPHA_ICW(i), filterType[dwFilterType].combinerInfo[i].dwICW |
            DRF_DEF(097, _SET_COMBINER_ALPHA, _ICW_A_ALPHA, _TRUE) |
            DRF_DEF(097, _SET_COMBINER_ALPHA, _ICW_B_ALPHA, _TRUE) |
            DRF_DEF(097, _SET_COMBINER_ALPHA, _ICW_C_ALPHA, _TRUE) |
            DRF_DEF(097, _SET_COMBINER_ALPHA, _ICW_D_ALPHA, _TRUE));
        _pContext->hwState.kelvin.set1 (NV097_SET_COMBINER_ALPHA_OCW(i), filterType[dwFilterType].combinerInfo[i].dwOCW);
        _pContext->hwState.kelvin.set1 (NV097_SET_COMBINER_FACTOR0(i), filterType[dwFilterType].combinerInfo[i].dwFactor0);
        _pContext->hwState.kelvin.set1 (NV097_SET_COMBINER_FACTOR1(i), filterType[dwFilterType].combinerInfo[i].dwFactor1);
    }

    /////////////////////////////////////////////////////

    float x0, y0;
    
    // This logic actually works whether we're upscaling or downscaling
    if (MethodInfo[dwMethod].dwKernel && !filterType[dwFilterType].bEmulateFilter) {
        x0 = 0.25 * BufferBltInfo[dwSimpleMethod].dwSuperBufferScaleX;
        y0 = 0.25 * BufferBltInfo[dwSimpleMethod].dwSuperBufferScaleY;
    } else {
        x0 = 0;
        y0 = 0;
    }

    if (IsMultisampling()) {
        if (bUpscaling) {
            x0 -= _appCompatibility.fXOffset;
            y0 -= _appCompatibility.fYOffset;
        } else {
            // match the offset we already added in nvSetKelvinTransform
            x0 += _appCompatibility.fXOffset * BufferBltInfo[dwSimpleMethod].dwSuperBufferScaleX;
            y0 += _appCompatibility.fYOffset * BufferBltInfo[dwSimpleMethod].dwSuperBufferScaleY;
        }
    } 

    float dx0 = filterType[dwFilterType].texStageInfo[0].fXOffset;
    float dy0 = filterType[dwFilterType].texStageInfo[0].fYOffset;
    float dx1 = filterType[dwFilterType].texStageInfo[1].fXOffset;
    float dy1 = filterType[dwFilterType].texStageInfo[1].fYOffset;
    float dx2 = filterType[dwFilterType].texStageInfo[2].fXOffset;
    float dy2 = filterType[dwFilterType].texStageInfo[2].fYOffset;
    float dx3 = filterType[dwFilterType].texStageInfo[3].fXOffset;
    float dy3 = filterType[dwFilterType].texStageInfo[3].fYOffset;
    
    if (bSubdivideAA) {
        float srcy0, srcy1, dsty0, dsty1;
        srcy0 = 0;
        dsty0 = 0;
        srcy1 = _strip.srcyinc.f;
        dsty1 = _strip.dstyinc.f;

        for (DWORD j=0; j<=_strip.num_strips; j++) {
            _pContext->hwState.kelvin.set1(NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_QUADS);
            getDC()->nvPusher.makeSpace (sizeSetStartMethod + 40);
            nvglSetStartMethod (0, NV_DD_KELVIN, NVPUSHER_NOINC(NV097_INLINE_ARRAY), 40);
            getDC()->nvPusher.inc (sizeSetStartMethod);

            if (j == _strip.num_strips) {
                srcy1 = srcy0 + _strip.srcyrem.f;
                dsty1 = dsty0 + _strip.dstyrem.f;
            }
            
            const float verts[4][10] = {
                {
                    0,                       dsty0,                 
                    x0+dx0,                  srcy0+y0+dy0, 
                    x0+dx1,                  srcy0+y0+dy1, 
                    x0+dx2,                  srcy0+y0+dy2, 
                    x0+dx3,                  srcy0+y0+dy3 
                }, {
                    pDst->getWidth(),        dsty0,                 
                    pSrc->getWidth()+x0+dx0, srcy0+y0+dy0, 
                    pSrc->getWidth()+x0+dx1, srcy0+y0+dy1, 
                    pSrc->getWidth()+x0+dx2, srcy0+y0+dy2, 
                    pSrc->getWidth()+x0+dx3, srcy0+y0+dy3 
                }, {
                    pDst->getWidth(),        dsty1, 
                    pSrc->getWidth()+x0+dx0, srcy1+y0+dy0, 
                    pSrc->getWidth()+x0+dx1, srcy1+y0+dy1, 
                    pSrc->getWidth()+x0+dx2, srcy1+y0+dy2, 
                    pSrc->getWidth()+x0+dx3, srcy1+y0+dy3
                }, {
                    0,                       dsty1, 
                    x0+dx0,                  srcy1+y0+dy0,
                    x0+dx1,                  srcy1+y0+dy1,
                    x0+dx2,                  srcy1+y0+dy2,
                    x0+dx3,                  srcy1+y0+dy3,
                }
            };
            
            
            for (i=0; i<40; i++) nvglSetData(i, *(DWORD *)&(((float *)verts)[i]));
            getDC()->nvPusher.inc(40);

            _pContext->hwState.kelvin.set1(NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END);
    
            srcy0 = srcy1;
            dsty0 = dsty1;
            srcy1 += _strip.srcyinc.f;
            dsty1 += _strip.dstyinc.f;
        }
    } else {
        _pContext->hwState.kelvin.set1(NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_QUADS);
        
        getDC()->nvPusher.makeSpace (sizeSetStartMethod + 40);
        nvglSetStartMethod (0, NV_DD_KELVIN, NVPUSHER_NOINC(NV097_INLINE_ARRAY), 40);
        getDC()->nvPusher.inc (sizeSetStartMethod);
        
        const float verts[4][10] = {
            {
                0,                       0,
                x0+dx0,                  y0+dy0,
                x0+dx1,                  y0+dy1,
                x0+dx2,                  y0+dy2,
                x0+dx3,                  y0+dy3
            }, {
                pDst->getWidth(),        0,                 
                pSrc->getWidth()+x0+dx0, y0+dy0, 
                pSrc->getWidth()+x0+dx1, y0+dy1, 
                pSrc->getWidth()+x0+dx2, y0+dy2, 
                pSrc->getWidth()+x0+dx3, y0+dy3 
            }, {
                pDst->getWidth(),        pDst->getHeight(), 
                pSrc->getWidth()+x0+dx0, pSrc->getHeight()+y0+dy0, 
                pSrc->getWidth()+x0+dx1, pSrc->getHeight()+y0+dy1, 
                pSrc->getWidth()+x0+dx2, pSrc->getHeight()+y0+dy2, 
                pSrc->getWidth()+x0+dx3, pSrc->getHeight()+y0+dy3
            }, {
                0,                       pDst->getHeight(), 
                x0+dx0,                  pSrc->getHeight()+y0+dy0,
                x0+dx1,                  pSrc->getHeight()+y0+dy1,
                x0+dx2,                  pSrc->getHeight()+y0+dy2,
                x0+dx3,                  pSrc->getHeight()+y0+dy3,
            }
        };
        
        
        for (i=0; i<40; i++) nvglSetData(i, *(DWORD *)&(((float *)verts)[i]));
        getDC()->nvPusher.inc(40);
        
        _pContext->hwState.kelvin.set1(NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END);
    }

    //////////////////////////////////////////////////
    // Gotta reset the whole shebang
    _pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_REALLY_FILTHY;
    if (bFlush) getDC()->nvPusher.flush (TRUE, CPushBuffer::FLUSH_WITH_DELAY);

    // Turn AA back on
    SetSampleMask();
    nvPusherStart(FALSE);
}


void CKelvinAAState::ScaledBlt(
    CSimpleSurface *pDst, 
    DWORD dwDstX,
    DWORD dwDstY,
    DWORD dwDstWidth, 
    DWORD dwDstHeight, 
    CSimpleSurface *pSrc, 
    double fSrcX,
    double fSrcY,
    DWORD dwSrcWidth,
    DWORD dwSrcHeight
    ) {

    nvAssert(_pContext);

    DWORD dwDstFormat = pDst->getFormat();
    DWORD dwSrcFormat = pSrc->getFormat();

    // Spoof the dst format
    switch (dwDstFormat) {
    case NV_SURFACE_FORMAT_Z16:
        dwDstFormat = NV_SURFACE_FORMAT_R5G6B5;
        break;
    case NV_SURFACE_FORMAT_Z24S8:
    case NV_SURFACE_FORMAT_Z24X8:
        dwDstFormat = NV_SURFACE_FORMAT_A8R8G8B8;
        break;
    }

    // Spoof the src format
    switch (dwSrcFormat) {
    case NV_SURFACE_FORMAT_Z16:
        dwSrcFormat = NV_SURFACE_FORMAT_R5G6B5;
        break;
    case NV_SURFACE_FORMAT_Z24S8:
    case NV_SURFACE_FORMAT_Z24X8:
        dwSrcFormat = NV_SURFACE_FORMAT_A8R8G8B8;
        break;
    }

    DWORD bDither = 1;

/////////////////////////////////////////////////////

    DWORD dwDstBPP = nvSurfaceFormatToBPP[dwDstFormat];
    DWORD dwSrcBPP = nvSurfaceFormatToBPP[dwSrcFormat];

/////////////////////////////////////////////////////

    nvPushData(0, dDrawSubchannelOffset(NV_DD_ROP) | SET_ROP_OFFSET | 0x40000);
    nvPushData(1, (SRCCOPY >> 16));
    nvPusherAdjust(2);
    getDC()->bltData.dwLastRop = (SRCCOPY >> 16);

/////////////////////////////////////////////////////

    DWORD dwSrcOffset, dwDstOffset;
    DWORD dwSrcPitch, dwDstPitch;
    DWORD dwSrcPoint, dwDstPoint;
    DWORD dwDsDx, dwDtDy;
    DWORD dwSrcSize, dwDstSize;
    DWORD dwSrcX, dwSrcY;
    DWORD bFOH, bCenter;


    dwSrcPitch = pSrc->getPitch();
    dwDstPitch = pDst->getPitch();

    dwDstOffset  = pDst->getOffset() + (dwDstBPP * dwDstX + dwDstPitch * dwDstY);
    dwDstX       = 0;
    dwDstY       = 0;
    if (dwDstOffset & NV_BYTE_ALIGNMENT_PAD) {
        dwDstX      += (dwDstOffset & NV_BYTE_ALIGNMENT_PAD) / dwDstBPP;
        dwDstOffset &= ~NV_BYTE_ALIGNMENT_PAD;
    }

    dwSrcX = (DWORD)(fSrcX * 16);
    dwSrcY = (DWORD)(fSrcY * 16);
    dwSrcOffset = pSrc->getOffset() + (dwSrcBPP * (dwSrcX >> 4) + dwSrcPitch * (dwSrcY >> 4));
    dwSrcX     &= 0xf;
    dwSrcY     &= 0xf;
    if (dwSrcOffset & NV_BYTE_ALIGNMENT_PAD) {
        dwSrcX      += ((dwSrcOffset & NV_BYTE_ALIGNMENT_PAD) / dwSrcBPP) << 4;
        dwSrcOffset &= ~NV_BYTE_ALIGNMENT_PAD;
    }

    dwDstSize = (dwDstHeight << 16) | dwDstWidth;
    dwSrcSize = (dwSrcHeight << 16) | dwSrcWidth;

    dwDstPoint = (dwDstY << 16) | dwDstX;
    dwSrcPoint = (dwSrcY << 20) | (dwSrcX << 4);

    dwDsDx = DWORD((__int64(dwSrcWidth) << 20) / __int64(dwDstWidth));
    dwDtDy = DWORD((__int64(dwSrcHeight) << 20) / __int64(dwDstHeight));

    bFOH = 1;
    bCenter = 0;

///////////////////////////////////////////////

//    nvAssert(!(dwSrcSize & 0xfffefffe));
//    nvAssert(!(dwDstSize & 0xfffefffe));

///////////////////////////////////////////////

    nvPushData ( 0, dDrawSubchannelOffset(NV_DD_SURFACES) | SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x100000);
    nvPushData ( 1, nv062SurfaceFormat[dwSrcFormat]);    // SetColorFormat
    nvPushData ( 2, (dwDstPitch << 16) | dwSrcPitch); // SetPitch
    nvPushData ( 3, dwSrcOffset);                     // SetOffset
    nvPushData ( 4, dwDstOffset);                     // SetOffset
    nvPusherAdjust (5);
    getDC()->bltData.dwLastColourFormat = nv062SurfaceFormat[dwSrcFormat];
    getDC()->bltData.dwLastCombinedPitch = (dwDstPitch << 16) | dwSrcPitch;
    getDC()->bltData.dwLastSrcOffset = dwSrcOffset;
    getDC()->bltData.dwLastDstOffset = dwDstOffset;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
    nvPushData(1, NV_DD_CONTEXT_COLOR_KEY);
    nvPushData(2, dDrawSubchannelOffset(NV_DD_SPARE) | SET_TRANSCOLOR_OFFSET | 0x40000);
    nvPushData(3, 0);
    nvPusherAdjust(4);
    getDC()->bltData.dwLastColourKey = 0xFFFFFFFF;
    getDC()->dDrawSpareSubchannelObject = NV_DD_CONTEXT_COLOR_KEY;


    nvPushData ( 0, dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000);
    nvPushData ( 1, NV_DD_SCALED_IMAGE_IID);
    nvPushData ( 2, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_CONTEXT_DMA_IMAGE_OFFSET | 0x40000);
    nvPushData ( 3, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY); // SetContextDmaImage
    nvPushData ( 4, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_SET_COLOR_FORMAT | 0x40000);
    nvPushData ( 5, nv089SurfaceFormat[dwDstFormat]); // SetColorFormat
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
        nvPusherAdjust (18);
    }

    nvPusherStart (TRUE);

    getDC()->TwoDRenderingOccurred   = TRUE;
    getDC()->ThreeDRenderingOccurred = 0;

    _pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_REALLY_FILTHY;
    
} 

extern "C" void __CxxFrameHandler() {
    // WARNING: This frame handler does absolutely nothing.  It's only
    // defined because it's required by __try/__except blocks, and apparently
    // we can't link in the library that has the real function.  If you hit this
    // int 3, make absolutely sure that the behavior is what you want.
#ifdef DEBUG
    __asm int 3;
#endif
}

char * nvGetCommandLine() {
#ifdef WINNT
    if (getDC()->nvD3DRegistryData.regAACompatibilityBits) {
        // the address is defined in the registry; load it
        char **pCmdLinePtrPtr = (char **)(~getDC()->nvD3DRegistryData.regAACompatibilityBits);

        if (*pCmdLinePtrPtr) {
            return *pCmdLinePtrPtr;
        } else {
            // somebody input the wrong registry value, and we were lucky to trap the null pointer
            dbgD3DError();
            return "";
        }
    } else {
        // nothing in the registry; iterate through our list of known possible 
        // locations, trying to determine which one will work, and using exception
        // handlers to gracefully recover.

#define MAX_GCL_PTRS 3
        // To add a new OS:
        // - Write a small program to return the address of the GetCommandLine function (say, 0x77e871cc)
        // - While still in the program (so that the DLL doesn't get unmapped), break into SoftICE and
        //   disassemble starting at that address ("u 77e871cc").
        // - There should be a line looking something like "mov eax, [77ee0694]".  The number in the 
        //   brackets is the one you want
        // - Add it to the list below, making sure to comment the OS, service pack, and build number
        // - I suggest adding the item to the bottom of the list and testing, to ensure that when it
        //   runs through the existing pointers, the excpetion handling works correctly.

        static const char **ppGCL[MAX_GCL_PTRS] = {
            (const char **)0x77edf694,    // Win2k SP1
            (const char **)0x77ee0694,    // Win2k, no SP
            (const char **)0x77e48fdc,    // Whistler build 2416
        };

        static char pCmdLine[256];
        int i, j, len;
    
        for (i=0; i<MAX_GCL_PTRS; i++) {
            __try {
                // This memcpy will throw an exception if the pointer is bad, though try/except
                // only works for user-space pointers.  Check that the pointer we look at is <2 GB,
                // and attempt the copy.

                nvAssert(ppGCL[i] < (const char **)0x80000000);      // no kernel-space pointers should be put in ppGCL!
                if (*ppGCL[i] >= (const char *)0x80000000) continue; // check that the dereferenced pointer is also <2 GB

                memcpy(pCmdLine, *ppGCL[i], 256);
    
                for (j=0; j<256; j++) if (!pCmdLine[j]) break;
                
                if (j == 256) continue; // no null character in string; we must be looking at the wrong memory
    
                len = j;
                if (len < 5) continue; // the filename better have at least one character, a dot, and an extension

                for (j=0; j<len; j++) if ((pCmdLine[j] > 0) && (pCmdLine[j] < 32)) continue; // hmmm, hit some control characters; abort.

                break; // looks like we found one that works
            } __except (EXCEPTION_EXECUTE_HANDLER) {
                continue;
            }
        }

        if (i == MAX_GCL_PTRS) {
            // we tried all the pointers in ppGCL, and they all failed
            return "";
        } else {
            return (char *)pCmdLine;
        }
    }

#else
    // under Win9x, we don't have to go through this mess
    return GetCommandLine();
#endif
}


void CKelvinAAState::InitAppCompatibility() {
    char *pCmdLine = nvGetCommandLine();

    static const AppCompatibilityStruct AppCompatibilitySettings[] = {
        {0x7ce92a196eb3e4a8, APPFLAGS_FORCE_SUPERSAMPLE, 0.04, 0.04},     // "monkey4.exe"          Escape From Monkey Island
        {0x97b0a9f590eb304f, APPFLAGS_FORCE_SUPERSAMPLE, 0, 0},           // "gpolice.exe"          G-Police
        {0x61053382419baf37, 0, 0.04, 0.04},                              // "hh2002_demo.exe"      High Heat Baseball 2002
        {0xd528be9ef82890fa, APPFLAGS_FORCE_SUPERSAMPLE, 0, 0},           // "monsterx.exe"         Monster Truck Madness 1/2
        {0x7814373c4b0b7201, APPFLAGS_WRITE_ACCESS_ON_ENDSCENE, 0, 0},    // "mrball.exe"           Mr Ball
        {0x8e039f4e0d3c4a75, APPFLAGS_FORCE_SUPERSAMPLE, 0, 0},           // "newhaas.exe"          Newman Hass Racing
        {0x925ac5084c34c71f, 0, -0.25, -0.25},                            // "lithtech.exe"         No One Lives Forever demo
        {0xbe568edf15a298b0, APPFLAGS_DOWNSAMPLE_ON_BLIT, 0, 0},          // "pop3ddemo.exe"        Prince of Persia demo
        {0x1ed8331bc713a59c, APPFLAGS_DISABLE_AA, 0, 0},                  // "revenant.exe"         Revenant
        {0xc9360a7086bac23c, APPFLAGS_FORCE_UNCOMPRESSED_Z, 0, 0},        // "kamdemo.exe"          Star Trek: Klingon Academy
        {0x8c02d8d6d094cf9a, 0, -1.0, -1.0},                              // "unrealtournament.exe" Unreal Tournament
        {0x849ce96f91d533d9, APPFLAGS_DOWNSAMPLE_ON_BLIT | APPFLAGS_FORCE_SUPERSAMPLE, 0, 0},        // "armada.exe" Star Trek Armada
        {0x0fb32fa59c3762d6, APPFLAGS_MAGNIFY_AFTER_FLIP | APPFLAGS_DOWNSAMPLE_ON_BLIT, 0, 0},     // "lemmings revolution.exe" Lemmings Revolution
        {0xb7c39ae78e70f3d3, 0,0.25,0.25},                                //"aquanox.exe"           Aquanox
        {0xfffe65aaac391e60, APPFLAGS_DISABLE_DITHER,0.25,0.0},           //"linksmmi.exe"          Msft Links2001
        {0x34b3962ff618545d, 0,0,0.25},                                   //"fs2.exe"               Descent Freepace 2, partial fix
        {0xd9e267694cee1255, APPFLAGS_FORCE_SUPERSAMPLE, 0.2501,0.25},    //"game.exe"              Driver, also matches Diablo II (.25->.2501 for Quincunx AA on Diablo)
        {0x0000000000000000, 0, 0, 0}   
    };

    
    char pLCCmdLine[256];
    strncpy(pLCCmdLine, pCmdLine, 256);
    pLCCmdLine[255] = 0;
    _strlwr(pLCCmdLine);

    char *pPos = strstr(pLCCmdLine, ".exe");
    char *pEnd = pPos + 4;
    char pExeName[256];

    
    if (pPos) {
        while ((pPos >= pLCCmdLine) && (*pPos != '\\') && (*pPos != '"')) pPos--;
        pPos++;

        int len = pEnd - pPos;

        strncpy(pExeName, pPos, len);
        pExeName[len] = 0;
    } else {
        pExeName[0] = 0;
    }
    


    _appCompatibility.qwHashVal = nvHash64((unsigned char *)pExeName, strlen(pExeName));
    DPF("Program name/hash value: \"%s\" = 0x%08x%08x", pExeName, (DWORD)(_appCompatibility.qwHashVal>>32), (DWORD)(_appCompatibility.qwHashVal&0xffffffff));  

    for (int i=0; AppCompatibilitySettings[i].qwHashVal; i++) {
        if (_appCompatibility.qwHashVal == AppCompatibilitySettings[i].qwHashVal) {
            _appCompatibility = AppCompatibilitySettings[i];
            //DPF (">>>>>>>>>>>>> matched");
            break;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////


/*
--------------------------------------------------------------------
hash() -- hash a variable-length key into a 64-bit value
  k     : the key (the unaligned variable-length array of bytes)
  len   : the length of the key, counting by bytes
  level : can be any 8-byte value
Returns a 64-bit value.  Every bit of the key affects every bit of
the return value.  No funnels.  Every 1-bit and 2-bit delta achieves
avalanche.  About 41+5len instructions.

The best hash table sizes are powers of 2.  There is no need to do
mod a prime (mod is sooo slow!).  If you need less than 64 bits,
use a bitmask.  For example, if you need only 10 bits, do
  h = (h & hashmask(10));
In which case, the hash table should have hashsize(10) elements.

If you are hashing n strings (ub1 **)k, do it like this:
  for (i=0, h=0; i<n; ++i) h = hash( k[i], len[i], h);

By Bob Jenkins, Jan 4 1997.  bob_jenkins@burtleburtle.net.  You may
use this code any way you wish, private, educational, or commercial,
as long as this whole comment accompanies it.

See http://burtleburtle.net/bob/hash/evahash.html
Use for hash table lookup, or anything where one collision in 2^^64
is acceptable.  Do NOT use for cryptographic purposes.
--------------------------------------------------------------------
*/

#define mix64(a,b,c) do { \
    a -= b; a -= c; a ^= (c>>43); \
    b -= c; b -= a; b ^= (a<<9); \
    c -= a; c -= b; c ^= (b>>8); \
    a -= b; a -= c; a ^= (c>>38); \
    b -= c; b -= a; b ^= (a<<23); \
    c -= a; c -= b; c ^= (b>>5); \
    a -= b; a -= c; a ^= (c>>35); \
    b -= c; b -= a; b ^= (a<<49); \
    c -= a; c -= b; c ^= (b>>11); \
    a -= b; a -= c; a ^= (c>>12); \
    b -= c; b -= a; b ^= (a<<18); \
    c -= a; c -= b; c ^= (b>>22); \
} while (0)

uint64 nvHash64(unsigned char *k, unsigned int length) {
    uint64 a,b,c;
    unsigned int len;
    
    // Set up the internal state
    len = length;
    a = b = 0;
    c = 0x9e3779b97f4a7c13; // the golden ratio; an arbitrary value
    
    // handle most of the key
    while (len >= 24) {
        a += ((uint64)k[0]       + ((uint64)k[ 1]<< 8) + ((uint64)k[ 2]<<16) + ((uint64)k[ 3]<<24)
          +  ((uint64)k[4 ]<<32) + ((uint64)k[ 5]<<40) + ((uint64)k[ 6]<<48) + ((uint64)k[ 7]<<56));
        b += ((uint64)k[8]       + ((uint64)k[ 9]<< 8) + ((uint64)k[10]<<16) + ((uint64)k[11]<<24)
          +  ((uint64)k[12]<<32) + ((uint64)k[13]<<40) + ((uint64)k[14]<<48) + ((uint64)k[15]<<56));
        c += ((uint64)k[16]      + ((uint64)k[17]<< 8) + ((uint64)k[18]<<16) + ((uint64)k[19]<<24)
          +  ((uint64)k[20]<<32) + ((uint64)k[21]<<40) + ((uint64)k[22]<<48) + ((uint64)k[23]<<56));
        
        mix64(a,b,c);
        k += 24; len -= 24;
    }
    
    // handle the last 23 bytes
    c += length;
    switch (len) {
    case 23: c+=((uint64)k[22]<<56);
    case 22: c+=((uint64)k[21]<<48);
    case 21: c+=((uint64)k[20]<<40);
    case 20: c+=((uint64)k[19]<<32);
    case 19: c+=((uint64)k[18]<<24);
    case 18: c+=((uint64)k[17]<<16);
    case 17: c+=((uint64)k[16]<<8);
    // the first byte of c is reserved for the length
    case 16: b+=((uint64)k[15]<<56);
    case 15: b+=((uint64)k[14]<<48);
    case 14: b+=((uint64)k[13]<<40);
    case 13: b+=((uint64)k[12]<<32);
    case 12: b+=((uint64)k[11]<<24);
    case 11: b+=((uint64)k[10]<<16);
    case 10: b+=((uint64)k[ 9]<<8);
    case  9: b+=((uint64)k[ 8]);
    case  8: a+=((uint64)k[ 7]<<56);
    case  7: a+=((uint64)k[ 6]<<48);
    case  6: a+=((uint64)k[ 5]<<40);
    case  5: a+=((uint64)k[ 4]<<32);
    case  4: a+=((uint64)k[ 3]<<24);
    case  3: a+=((uint64)k[ 2]<<16);
    case  2: a+=((uint64)k[ 1]<<8);
    case  1: a+=((uint64)k[ 0]);
    // case 0: nothing left to add
    }
    
    mix64(a,b,c);
    
    return c;
}

#endif // NVARCH >= 0x020
