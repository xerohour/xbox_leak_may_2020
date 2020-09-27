/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvBlit.h                                                          *
*    Definition file for:                                                   *
*       Blt832.c, Blt1632.c and Blt3232                                     *
*    Suggested new name for combined file:                                  *
*       nvBlit.c                                                            *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Paul van der Kouwe 06/16/99 - created.                              *
*                                                                           *
\***************************************************************************/

#ifndef _NVBLIT_H_
#define _NVBLIT_H_

#ifdef __cplusplus
extern "C" {
#endif

// workspace allocation flags
#define BLTWS_UNINITIALIZED     0x0
#define BLTWS_VIDEO             0x1
#define BLTWS_AGP               0x2
#define BLTWS_SYSTEM            0x4
#define BLTWS_PREFERRED_VIDEO   0x10
#define BLTWS_PREFERRED_AGP     0x20
#define BLTWS_PREFERRED_SYSTEM  0x40
#define BLTWS_REQUIRED_VIDEO    0x100
#define BLTWS_REQUIRED_AGP      0x200
#define BLTWS_REQUIRED_SYSTEM   0x400

// other defines (lore)
#define NV4_BLT_READ_AHEAD_BUFFER_SIZE  4096
#define NV10_BLT_READ_AHEAD_BUFFER_SIZE 8192

// abbreviated surface info
typedef struct tagSurfInfo {
    LPDDRAWI_DDRAWSURFACE_LCL pLcl;
    LPDDRAWI_DDRAWSURFACE_GBL pGbl;
    CNvObject *pObj;
    DWORD     dwFourCC;
    BOOL      bUseSwizzle;
    BOOL      bIsLocked;
    DWORD     dwCaps;
    DWORD     dwContextDma;
    FLATPTR   fpVidMem;
    DWORD     dwOffset;
    DWORD     dwPitch;
    DWORD     dwWidth;
    DWORD     dwHeight;
    DWORD     dwSubRectX;
    DWORD     dwSubRectY;
    DWORD     dwSubRectWidth;
    DWORD     dwSubRectHeight;
    BOOL      isPotentialFault;
    DWORD     dwBytesPerPixel;
    BOOL      bDefaultBlitChanged;
} SURFINFO, *LPSURFINFO;

// exports
DWORD __stdcall GetBltStatus32(LPDDHAL_GETBLTSTATUSDATA lpGetBltStatus);
DWORD __stdcall Blit32(LPDDHAL_BLTDATA pbd);

// utility functions
void  __stdcall bltInit(void);
DWORD __stdcall bltEarlyErrorCheck(LPDDHAL_BLTDATA pbd, LPSURFINFO pSrc, LPSURFINFO pDst);
DWORD __stdcall bltUpdateClip(GLOBALDATA *pDriverData);
DWORD __stdcall bltDestroyAllContextDMAs(LPDDRAWI_DIRECTDRAW_GBL lpDD);
DWORD __stdcall bltCreateContextDMA(LPDDHAL_BLTDATA pbd, LPSURFINFO pSurf);
DWORD __stdcall bltControlTraffic(LPSURFINFO pSrc, LPSURFINFO pDst);
DWORD __stdcall bltAllocWorkSpace(LPDDHAL_BLTDATA pbd, LPSURFINFO pSurf, DWORD dwFlags, LPDDRAWI_DIRECTDRAW_GBL lpDD);
DWORD __stdcall bltSetSurfaces2D(LPSURFINFO pSrc, LPSURFINFO pDst);
DWORD __stdcall bltSetBltState(LPDDHAL_BLTDATA pbd, LPSURFINFO src, LPSURFINFO dst);

// blt functions
DWORD __stdcall bltPotentialFaultHandler(LPDDHAL_BLTDATA pbd, LPSURFINFO pSrc, LPSURFINFO pDst);
DWORD __stdcall bltTexture(LPDDHAL_BLTDATA pbd, LPSURFINFO pSrc, LPSURFINFO pDst, BOOL bStretch);
DWORD __stdcall ScaleZFillDepth(LPDDHAL_BLTDATA pbd, LPSURFINFO pDst);
DWORD __stdcall bltColourFill(LPDDHAL_BLTDATA pbd, LPSURFINFO pDst, DWORD dwFillColour);
DWORD __stdcall blt4CCto4CC(LPDDHAL_BLTDATA pbd, LPSURFINFO pSrc, LPSURFINFO pDst);
DWORD __stdcall bltSimpleCopy(LPDDHAL_BLTDATA pbd, LPSURFINFO pSrc, LPSURFINFO pDst);
DWORD __stdcall bltSlowVtoSBlt(LPDDHAL_BLTDATA pbd, LPSURFINFO pSrc, LPSURFINFO pDst);
DWORD __stdcall bltSlowStoVBlt(LPDDHAL_BLTDATA pbd, LPSURFINFO pSrc, LPSURFINFO pDst);
DWORD __stdcall bltSimpleVtoVBlt(LPDDHAL_BLTDATA pbd, LPSURFINFO pSrc, LPSURFINFO pDst);
DWORD __stdcall blt8bppStrBlt(LPDDHAL_BLTDATA pbd, LPSURFINFO pSrc, LPSURFINFO pDst);
DWORD __stdcall bltStrBlt(LPDDHAL_BLTDATA pbd, LPSURFINFO pSrc, LPSURFINFO pDst, BOOL bFilter);

// internal use blit functions
DWORD __stdcall bltWriteDword(DWORD dwOffset, DWORD dwIndex, DWORD dwData);
DWORD __stdcall bltFillBlock(GLOBALDATA *pDriverData, DWORD dwColour, DWORD dwOffset, DWORD dwPitch, DWORD dwWidth, DWORD dwHeight, DWORD dwBytesPerPel);

// old fallback blt code
DWORD __stdcall Blt8bpp32  (LPDDHAL_BLTDATA pbd);
DWORD __stdcall Blt16bpp32 (LPDDHAL_BLTDATA pbd);
DWORD __stdcall Blt32bpp32 (LPDDHAL_BLTDATA pbd);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _NVBLIT_H_
