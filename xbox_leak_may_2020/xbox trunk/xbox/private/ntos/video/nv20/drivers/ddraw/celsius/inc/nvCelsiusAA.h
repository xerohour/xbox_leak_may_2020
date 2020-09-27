#ifndef _NVCELSIUSAA_H
#define _NVCELSIUSAA_H

/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvCelsiusAA.h                                                     *
*     prototypes for aa routines                                            *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                      12Jan2000   created                *
*                                                                           *
\***************************************************************************/

#if (NVARCH >= 0x010)

struct AAAppCompatState
{
    LPTSTR lpApp;
    BOOL bNoAA;
    BOOL bPath;
    BOOL bDiscardFBContents;
    BOOL bAlwaysMagnifyUponUnlock;
    BOOL bMinifyUponEndScene;
    BOOL bDither;
    BOOL bBlitSync;
    BOOL bDiscardSRT;
    BOOL bAllowSpecial;
};

#define AA_ENABLED(pDriverData, pContext) \
    (((pDriverData)->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_ANTIALIASENABLE_MASK) \
     && (((pContext)->dwRenderState[D3DRENDERSTATE_ANTIALIAS] != D3DANTIALIAS_NONE) \
     || ((pDriverData)->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_ANTIALIASFORCEENABLE_MASK)))

struct AASTATE
{
    enum
    {
        FLAG_ENABLED        = 0x80000000,       // aa is enabled and in use
        FLAG_INIT           = 0x20000000,       // aa is ready (super buffers exist)
        FLAG_SUSPENDED      = 0x10000000,       // aa is enabled but suspended for render to texture


        FLAG_RT_VALID       = 0x00800000,       // render target valid
        FLAG_ZB_VALID       = 0x00400000,       // zeta buffer valid
        FLAG_SRT_VALID      = 0x00200000,       // super render target valid
        FLAG_SZB_VALID      = 0x00100000,       // super zeta buffer valid

        MASK_BSCOUNT        = 0x00000ff0,
            BSC_RESET           = 0x200,
            BSC_DEC             = 0x010,

        MASK_METHOD         = 0x00000007,
            METHOD_1x2          = 0,
            METHOD_2x2lod       = 1,
            METHOD_2x2          = 2,
            METHOD_2x2spec      = 3,
            METHOD_3x3lod       = 4,
            METHOD_3x3          = 5,
            METHOD_4x4lod       = 6,
            METHOD_4x4          = 7,
    };
// bank
protected:
    DWORD dwFlags;

public:
    AAAppCompatState mAppCompat;

    CSimpleSurface *pSuperRenderTarget;
    CSimpleSurface *pSuperZetaBuffer;
    CSimpleSurface *pBlitTempSpace;

    float fWidthAmplifier;
    float fHeightAmplifier;
    DWORD dwDepthAmplifier;

    DWORD dwOriginalRenderTargetWidth;
    DWORD dwOriginalRenderTargetHeight;
    DWORD dwOriginalZBHandle;

    float fLODBias;
    DWORD dwLinePointSize;

    // helpers
    inline void setFlags (DWORD dwMask)
    {
        dwFlags |= dwMask;
        invalidateState();
    }

    inline void modFlags (DWORD dwAndMask,DWORD dwOrMask)
    {
        dwFlags &= dwAndMask;
        dwFlags |= dwOrMask;
        invalidateState();
    }

    inline BOOL isInit (void) const
    {
        return dwFlags & AASTATE::FLAG_INIT;
    }

    inline BOOL isEnabled (void) const
    {
        return dwFlags & AASTATE::FLAG_ENABLED;
    }

    inline BOOL isSuspended (void) const
    {
        return dwFlags & AASTATE::FLAG_SUSPENDED;
    }

    inline BOOL isRenderTargetValid (void) const
    {
        return dwFlags & AASTATE::FLAG_RT_VALID;
    }

    inline BOOL isSuperBufferValid (void) const
    {
        return dwFlags & AASTATE::FLAG_SRT_VALID;
    }

    inline BOOL isZetaBufferValid (void) const
    {
        return dwFlags & AASTATE::FLAG_ZB_VALID;
    }

    inline BOOL isSuperZBValid (void) const
    {
        return dwFlags & AASTATE::FLAG_SZB_VALID;
    }

    inline void invalidateSRT() {
        dwFlags &= ~FLAG_SRT_VALID;
    }

    inline void makeRenderBuffersMatch (PNVD3DCONTEXT pContext);
    inline void makeZBuffersMatch (PNVD3DCONTEXT pContext);

    // In the next four functions, we assume that at least one of the
    // buffers (of each type, either color or Z), is valid.
    // Instantiated in nvCelsiusAA.cpp
    void makeRenderTargetValid (PNVD3DCONTEXT pContext);
    void makeZetaBufferValid (PNVD3DCONTEXT pContext);
    void makeSuperBufferValid (PNVD3DCONTEXT pContext);
    void makeSuperZBValid (PNVD3DCONTEXT pContext);

    inline DWORD getMethod (void) const
    {
        return dwFlags & AASTATE::MASK_METHOD;
    }

    inline void bsReset (void)
    {
        dwFlags &= ~MASK_BSCOUNT;
        dwFlags |=  BSC_RESET;
    }

    inline BOOL bsDec (void)
    {
        if (dwFlags & MASK_BSCOUNT)
        {
            dwFlags -= BSC_DEC;
            return 0;
        }
        return 1;
    }

    inline BOOL WantEnabled (PNVD3DCONTEXT pContext);

    // The next few parts basically parallel various parts fo the driver.
    // They are called just before the various D3D functions are executed
    // to prepare the AA buffers for proper use.

    inline void BeginScene (PNVD3DCONTEXT pContext)
    {
        if (!isEnabled()) Create(pContext);
    }

    inline void EndScene (PNVD3DCONTEXT pContext)
    {
        if (!isEnabled()) return;

#ifndef WINNT   // WinNT requires that we minify on endscene in windowed apps
        if (mAppCompat.bMinifyUponEndScene)
#endif
        {
            makeRenderTargetValid(pContext);
        }
    }

    inline void Flip (PNVD3DCONTEXT pContext)
    {
        if (!isEnabled()) return;

        if (!(dwFlags & AASTATE::FLAG_RT_VALID)) {
            // Don't set both buffers valid unless we are
            // actively making the render target valid.
            makeRenderTargetValid(pContext);
            if (mAppCompat.bDiscardFBContents) {
                // We don't care what happens to the superbuffer after
                // this, so either buffer can be valid
                setFlags(AASTATE::FLAG_SRT_VALID);
            }
        }
    }

    inline void Lock (PNVD3DCONTEXT pContext)
    {
        if (!isEnabled()) return;
        makeRenderTargetValid(pContext);
    }

    inline void LockZ (PNVD3DCONTEXT pContext)
    {
        if (!isEnabled()) return;
        makeZetaBufferValid(pContext);
    }

    inline void Unlock (PNVD3DCONTEXT pContext)
    {
        if (!isEnabled()) return;
        //invalidateSRT();
        if (mAppCompat.bAlwaysMagnifyUponUnlock) makeSuperBufferValid(pContext);
    }

    inline void UnlockZ (PNVD3DCONTEXT pContext)
    {
        if (!isEnabled()) return;
        if (mAppCompat.bAlwaysMagnifyUponUnlock) makeSuperZBValid(pContext);
    }

    inline void Rendering (PNVD3DCONTEXT pContext)
    {
        if (!isEnabled()) return;

        makeSuperBufferValid(pContext);
        makeSuperZBValid(pContext);
    }

    inline void Minify (PNVD3DCONTEXT pContext)
    {
        if (!isEnabled()) return;
        makeRenderTargetValid(pContext);
    }

    inline void MinifyZ (PNVD3DCONTEXT pContext)
    {
        if (!isEnabled()) return;
        makeZetaBufferValid(pContext);
    }

    void Create          (PNVD3DCONTEXT pContext);
    void invalidateState (void);

};

//
// prototypes
//
PNVD3DCONTEXT nvCelsiusAAFindContext         (CSimpleSurface *pSurface);
BOOL          nvCelsiusAACanCreate           (LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl);
BOOL          nvCelsiusAACreate              (PNVD3DCONTEXT pContext);
BOOL          nvCelsiusAADestroy             (PNVD3DCONTEXT pContext);
void          nvCelsiusAASelectSuperBuffers  (PNVD3DCONTEXT pContext);
void          nvCelsiusAASelectNormalBuffers (PNVD3DCONTEXT pContext,BOOL bPreserveBufferContents);
void          nvCelsiusAAMagnify             (PNVD3DCONTEXT pContext);
void          nvCelsiusAAMinify              (PNVD3DCONTEXT pContext);
void          nvCelsiusAAMagnifyZ            (PNVD3DCONTEXT pContext);
void          nvCelsiusAAMinifyZ             (PNVD3DCONTEXT pContext);


#endif  // NVARCH >= 0x010

#endif  // _NVCELSIUSAA_H

