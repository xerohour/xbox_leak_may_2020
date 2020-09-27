/*==========================================================================;
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *  Portions Copyright (C) 1995, 1998 NVidia Corporation.  All Rights Reserved.
 *  Portions Copyright (C) 1997 SGS-THOMSON Microelectronics  All Rights Reserved.
 *
 *  File:       nvd3ddrv.c
 *  Content:    master D3D include file - OS specific includes
 *
 ***************************************************************************/

#ifndef WINNT
#include <conio.h>
#include "ddrawi.h"
#include "ddmini.h"
#include <d3dhal.h>
#include "nvwin32.h"
#else

#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <d3d.h>

#include <winerror.h>
#include <wingdi.h>
#include <winddi.h>
#include <mmsystem.h>
#include <devioctl.h>

#include <ioaccess.h>
#include <math.h>
#include <dmemmgr.h>
#include <dx95type.h>
#include "nvapi.h"

//
// Type redefs excluded from dx95type.h.
//

typedef PDD_DIRECTDRAW_LOCAL            LPDDRAWI_DIRECTDRAW_LCL; //HMH
typedef D3DNTHAL_DP2CLEAR               *LPD3DNTHAL_DP2CLEAR;  //HMH
typedef LPD3DNTHAL_DP2CLEAR             LPD3DHAL_DP2CLEAR;     //HMH
typedef D3DNTHAL_DP2SETRENDERTARGET     *LPD3DNTHAL_DP2SETRENDERTARGET;  //HMH
typedef LPD3DNTHAL_DP2SETRENDERTARGET   LPD3DHAL_DP2SETRENDERTARGET;     //HMH
typedef D3DNTHAL_DP2TEXBLT     *LPD3DNTHAL_DP2TEXBLT;  //HMH
typedef LPD3DNTHAL_DP2TEXBLT   LPD3DHAL_DP2TEXBLT;     //HMH
typedef D3DNTHAL_DP2STATESET            *LPD3DNTHAL_DP2STATESET; //HMH
typedef LPD3DNTHAL_DP2STATESET          LPD3DHAL_DP2STATESET; //HMH

typedef D3DNTHAL_D3DEXTENDEDCAPS D3DHAL_D3DEXTENDEDCAPS;
typedef PDD_SURFACE_INT LPDDRAWI_DDRAWSURFACE_INT;
typedef D3DNTHAL_DP2POINTS *LPD3DNTHAL_DP2POINTS;
typedef D3DNTHAL_DP2RENDERSTATE *LPD3DNTHAL_DP2RENDERSTATE;
typedef D3DNTHAL_DP2LINELIST *LPD3DNTHAL_DP2LINELIST;
typedef D3DNTHAL_DP2INDEXEDLINELIST *LPD3DNTHAL_DP2INDEXEDLINELIST;
typedef D3DNTHAL_DP2LINESTRIP *LPD3DNTHAL_DP2LINESTRIP;
typedef D3DNTHAL_DP2TRIANGLELIST *LPD3DNTHAL_DP2TRIANGLELIST;
typedef D3DNTHAL_DP2TRIANGLESTRIP *LPD3DNTHAL_DP2TRIANGLESTRIP;
typedef D3DNTHAL_DP2TRIANGLEFAN *LPD3DNTHAL_DP2TRIANGLEFAN;
typedef D3DNTHAL_DP2TEXTURESTAGESTATE *LPD3DNTHAL_DP2TEXTURESTAGESTATE;

typedef LPD3DNTHAL_DP2POINTS LPD3DHAL_DP2POINTS;
typedef LPD3DNTHAL_DP2RENDERSTATE LPD3DHAL_DP2RENDERSTATE;
typedef LPD3DNTHAL_DP2LINELIST LPD3DHAL_DP2LINELIST;
typedef LPD3DNTHAL_DP2INDEXEDLINELIST LPD3DHAL_DP2INDEXEDLINELIST;
typedef LPD3DNTHAL_DP2LINESTRIP LPD3DHAL_DP2LINESTRIP;
typedef LPD3DNTHAL_DP2STARTVERTEX LPD3DHAL_DP2STARTVERTEX;
typedef LPD3DNTHAL_DP2TRIANGLELIST LPD3DHAL_DP2TRIANGLELIST;
typedef LPD3DNTHAL_DP2TRIANGLESTRIP LPD3DHAL_DP2TRIANGLESTRIP;
typedef LPD3DNTHAL_DP2TRIANGLEFAN LPD3DHAL_DP2TRIANGLEFAN;
typedef LPD3DNTHAL_DP2TEXTURESTAGESTATE LPD3DHAL_DP2TEXTURESTAGESTATE;
typedef LPD3DNTHAL_DP2VIEWPORTINFO LPD3DHAL_DP2VIEWPORTINFO;
typedef LPD3DNTHAL_DP2WINFO LPD3DHAL_DP2WINFO;
typedef LPD3DNTHAL_DP2TRIANGLEFAN_IMM LPD3DHAL_DP2TRIANGLEFAN_IMM;
#define NvGetFlatDataSelector() (0xffffffff)

/*
 * NT doesn't define the DX5 Clear routine's data structure, but the DX5 driver
 * routine can still be called from the DX6 clear routine to do fast buffer
 * clears, so need to define this ourselves (taken from d3d.h)
 */
#if 0
typedef struct _D3DHAL_CLEARDATA
{
    DWORD               dwhContext;     // in:  Context handle

    // dwFlags can contain D3DCLEAR_TARGET or D3DCLEAR_ZBUFFER
    DWORD               dwFlags;        // in:  surfaces to clear

    DWORD               dwFillColor;    // in:  Color value for rtarget
    DWORD               dwFillDepth;    // in:  Depth value for Z buffer

    LPD3DRECT           lpRects;        // in:  Rectangles to clear
    DWORD               dwNumRects;     // in:  Number of rectangles

    HRESULT             ddrval;         // out: Return value
} D3DHAL_CLEARDATA;
typedef D3DHAL_CLEARDATA FAR *LPD3DHAL_CLEARDATA;
#endif
/*
 * These represent the "OS Independent" RM entry points. For NT they are functions
 * in the display driver which package the parms into a IOCTL packet and make
 * an IOCTL call to the miniport where the RM lives.
 */

#define NvRmOpen() \
    NvOpen(pDriverData->ppdev->hDriver)
#define NvRmAllocContextDma(hClient, hDma, hClass, flags, base, limit) \
    NvAllocContextDma(pDriverData->ppdev->hDriver, hClient, hDma, hClass, flags, base, limit)
#define NvRmFree(hClient, hParent, hObj) \
    NvFree(pDriverData->ppdev->hDriver, hClient, hParent, hObj)
#define NvRmAllocChannelPio(hClient, hDev, hChannel, hClass, hErrorCtx, ppChannel, flags) \
    NvAllocChannelPio(pDriverData->ppdev->hDriver, (hClient), hDev, (hChannel), (hClass), (hErrorCtx), (ppChannel), (flags))
#define NvRmAllocChannelDma(hClient, hDev, hChan, hClass, hErrorCtx, hDataCtx, offset, ppChan) \
    NvAllocChannelDma(pDriverData->ppdev->hDriver, (hClient), hDev, (hChan), (hClass), (hErrorCtx), (hDataCtx), (offset), (ppChan))
#define NvRmAllocMemory(hClient, hPar, hMem, hClass, fl, ppAddr, pLim) \
    NvAllocMemory(pDriverData->ppdev->hDriver, hClient, hPar, hMem, hClass, fl, ppAddress, pLim)
#define NvRmConfigGet(hClient, hDevice, index, pValue) \
    NvConfigGet(pDriverData->ppdev->hDriver, hClient, hDevice, index, pValue)
#define NvRmConfigSet(hClient, hDevice, index, newValue, pOldValue) \
    NvConfigSet(pDriverData->ppdev->hDriver, hClient, hDevice, index, newValue, pOldValue)
#define NvRmAllocObject(hCli, hChan, hObj, hCls) \
    NvAllocObject(pDriverData->ppdev->hDriver, hCli, hChan, hObj, hCls)
#define NvRmAlloc(hCli, hChan, hObj, hCls, parms) \
    NvAlloc(pDriverData->ppdev->hDriver, hCli, hChan, hObj, hCls, parms)

/*
 * This macro is used to force the driver to check the PDEV flag to
 * determine if overlay is supported in the current video mode. Eventually
 * we want to change this to use the internal driver routine, but we'll
 * need to redefine the NT RM entry point name to do it.
 */
#define GetVideoScalerBandwidthStatus32(surfaceFlags) \
    (pDriverData->ppdev->bHwVidOvl)

/*
 * Macros for GDI functions missing from Win2K.
 */
#define IntersectRect(pIntersect, pSrc, pDst) \
    bIntersect((PRECTL) (pSrc), (PRECTL) (pDst), (PRECTL) (pIntersect))

/*
 * Macros for DirectX heap manager functions.
 */
#define DDHAL32_VidMemAlloc(pdrv, heap, x, y) \
    HeapVidMemAllocAligned((LPVIDMEM)pDriverData->ppdev->AgpHeap, x, y, \
        &(pDriverData->ppdev->AgpHeapAlignment), &(pDriverData->ppdev->AgpDummyPitch))

#define DDHAL32_VidMemFree(pdrv, heap, surface) \
    { \
    if (pDriverData->ppdev->AgpHeap->lpHeap) \
        { \
        VidMemFree((pDriverData->ppdev->AgpHeap->lpHeap), \
          ((ULONG) (surface) - (ULONG) (pDriverData->ppdev->AgpHeapBase))); \
        } \
    }

#define NV_SLEEP
#define NV_SLEEPFLIP
#define SetPriorityClass(a, b)
#define SetThreadPriority(a, b)
#define GetCurrentThread()
#define GetCurrentProcess()
#define GetSystemInfo(a)

//********************************************************************
// Global defs.
//********************************************************************
extern SURFACEALIGNMENT    AgpHeapAlignment;
extern LONG                AgpDummyPitch;

#include "basetsd.h"             // for compiling 64-bit Merced codes.
#include <ntddvdeo.h>
#include "memstruc.h"
#include "ddmini.h"
#include "nvEscDef.h"
#include "driver.h"
#include "nvntioctl.h"
#include "nvos.h"
#endif // #ifdef WINNT

