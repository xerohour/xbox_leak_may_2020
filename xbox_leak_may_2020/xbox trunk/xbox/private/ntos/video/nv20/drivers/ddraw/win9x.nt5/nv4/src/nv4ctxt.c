#ifdef  NV4
/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4CTXT.C                                                         *
*   Hardware specific context management routines.                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/31/97 - reorganzied.                 *
*                                                                           *
\***************************************************************************/
#include <windows.h>
#include "nvd3ddrv.h"
#include "nv32.h"
#include "ddrvmem.h"
#include "nvddobj.h"
#include "d3dinc.h"
#include "nv4dreg.h"
#include "nvd3dmac.h"
#include "nv3ddbg.h"
#include "nv4vxmac.h"
#include "nvheap.h"
#ifdef  MULTI_MON
#include "nvrmapi.h"
#include "nvrmarch.inc"
#include "nvos.h"
#endif  // MULTI_MON

#ifndef WINNT
FLATPTR EXTERN_DDAPI DDHAL32_VidMemAlloc (LPDDRAWI_DIRECTDRAW_GBL pdrv, int heap, DWORD dwXDim, DWORD dwYDim);
void EXTERN_DDAPI DDHAL32_VidMemFree (LPDDRAWI_DIRECTDRAW_GBL pdrv, int heap, FLATPTR ptr);
extern Nv4ControlDma * __stdcall NvOpenDma(char *, int, int, int);
#endif // #ifndef WINNT

/*
 * Setup the context surface.
 */
BOOL nvSetContextSurface
(
    PNVD3DCONTEXT   pContext
)
{
    DWORD   dwRenderAlphaMask;
    DWORD   dwRenderRedMask;
    DWORD   dwRenderGreenMask;
    DWORD   dwRenderBlueMask;

    DWORD   dwRenderFormat;
    DWORD   dwRenderBitCount;
    DWORD   dwZetaBitCount;
    DWORD   dwAdjustedZetaBitCount;
    DWORD  *pdwZetaBitCount;

    if (pContext->lpLcl)
    {
        /*
         * Store the width and height of the surface in the context.
         */
        pContext->surfaceViewport.clipHorizontal.wX      = 0;
        pContext->surfaceViewport.clipHorizontal.wWidth  = pContext->lpLcl->lpGbl->wWidth;
        pContext->surfaceViewport.clipVertical.wY        = 0;
        pContext->surfaceViewport.clipVertical.wHeight   = pContext->lpLcl->lpGbl->wHeight;

        /*
         * Initialize current surface addresses.
         */
        pContext->dwSurfaceAddr            = VIDMEM_ADDR(pContext->lpLcl->lpGbl->fpVidMem);
        pContext->dwSurfaceOffset          = VIDMEM_OFFSET(pContext->lpLcl->lpGbl->fpVidMem);
        pContext->surfacePitch.wColorPitch = (WORD)pContext->lpLcl->lpGbl->lPitch;

        /*
         * Get the Render Target color format.
         */
        if (pContext->lpLcl->dwFlags & DDRAWISURF_HASPIXELFORMAT)
        {
            DPF_LEVEL(NVDBG_LEVEL_INFO, "nvSetContextSurface: Render Target Pixel Format");
            dwRenderBitCount  = pContext->lpLcl->lpGbl->ddpfSurface.dwRGBBitCount;
            dwRenderAlphaMask = pContext->lpLcl->lpGbl->ddpfSurface.dwRGBAlphaBitMask;
            dwRenderRedMask   = pContext->lpLcl->lpGbl->ddpfSurface.dwRBitMask;
            dwRenderGreenMask = pContext->lpLcl->lpGbl->ddpfSurface.dwGBitMask;
            dwRenderBlueMask  = pContext->lpLcl->lpGbl->ddpfSurface.dwBBitMask;
        }
        else
        {
            DPF_LEVEL(NVDBG_LEVEL_INFO, "nvSetContextSurface: Render Format same as desktop!");
            dwRenderBitCount = pDriverData->bi.biBitCount;
            if (dwRenderBitCount == 16)
            {
                dwRenderAlphaMask = 0x00000000;
                dwRenderRedMask   = 0x0000F800;
                dwRenderGreenMask = 0x000007E0;
                dwRenderBlueMask  = 0X0000001F;
            }
            else if ((dwRenderBitCount == 24) || (dwRenderBitCount == 32))
            {
                dwRenderAlphaMask = 0x00000000;
                dwRenderRedMask   = 0x00FF0000;
                dwRenderGreenMask = 0x0000FF00;
                dwRenderBlueMask  = 0X000000FF;
            }
            else
            {
                DPF_LEVEL(NVDBG_LEVEL_ERROR, "Invalid 3D rendering format!!");
                dbgD3DError();
                return (FALSE);
            }
        }
        DPF_LEVEL(NVDBG_LEVEL_INFO, "    BitCount = %02ld", dwRenderBitCount);
        DPF_LEVEL(NVDBG_LEVEL_INFO, "    RMask=%08lx, GMask=%08lx, BMask=%08lx, AMask=%08lx",
             dwRenderRedMask, dwRenderGreenMask, dwRenderBlueMask, dwRenderAlphaMask);
        DPF_LEVEL(NVDBG_LEVEL_INFO, "\r\n");
        if (dwRenderBitCount == 16)
        {
            switch (dwRenderAlphaMask)
            {
                case 0x00000000:
                    if (dwRenderRedMask == 0x7C00)
                        dwRenderFormat = 0x00007FFF;
                    else
                        dwRenderFormat = 0x0000FFFF;
                    break;
                default:
                    DPF_LEVEL(NVDBG_LEVEL_ERROR, "Invalid 16 bpp Render Target Format!!");
                    break;
            }
        }
        else if ((dwRenderBitCount == 24)
              || (dwRenderBitCount == 32))
        {
            switch (dwRenderAlphaMask)
            {
                case 0x00000000:
                    dwRenderFormat = 0x00FFFFFF;
                    break;
                case 0x7F000000:
                    dwRenderFormat = 0x7FFFFFFF;
                    break;
                case 0xFF000000:
                    dwRenderFormat = 0xFFFFFFFF;
                    break;
                default:
                    DPF_LEVEL(NVDBG_LEVEL_ERROR, "Invalid %02ld Bpp Render Target Format!!", dwRenderBitCount);
            }
        }
        else
            DPF_LEVEL(NVDBG_LEVEL_ERROR, "Invalid %02ld Bpp Render Target Format!!", dwRenderBitCount);
    }
    else
    {
        pContext->surfaceViewport.clipHorizontal.wX     = 0;
        pContext->surfaceViewport.clipHorizontal.wWidth = 0;
        pContext->surfaceViewport.clipVertical.wY       = 0;
        pContext->surfaceViewport.clipVertical.wHeight  = 0;
        pContext->dwSurfaceAddr                         = 0;
        pContext->dwSurfaceOffset                       = 0;
        pContext->surfacePitch.wColorPitch              = 0;

        /*
         * No Render Target color format.
         */
        dwRenderFormat   = 0x00000000;
        dwRenderBitCount = 0;
    }

    /*
     * Enable/Disable the Z-Buffer based on whether one is supplied.
     */
    if (pContext->lpLclZ)
    {
        /*
         * Initialize Z-Buffer location.
         */
        pContext->ZBufferAddr             = pContext->lpLclZ->lpGbl->fpVidMem;
#ifdef WINNT
        pContext->ZBufferAddr += (ULONG) pDriverData->ppdev->pjScreen;
#endif // #ifndef WINNT
        pContext->ZBufferOffset           = pContext->lpLclZ->lpGbl->fpVidMem - pDriverData->BaseAddress;
        pContext->surfacePitch.wZetaPitch = (WORD)pContext->lpLclZ->lpGbl->lPitch;

        /*
         * Get the Zeta Buffer bit depth.
         * The bit depth of the render target and the zeta buffer MUST match.
         */
        if (pContext->lpLclZ->dwFlags & DDRAWISURF_HASPIXELFORMAT)
        {
            DPF_LEVEL(NVDBG_LEVEL_INFO, "D3D:nvSetContext Surface: Zeta Buffer Format");
            DPF_LEVEL(NVDBG_LEVEL_INFO, "    BitCount = %02ld", pContext->lpLclZ->lpGbl->ddpfSurface.dwRGBBitCount);
            DPF_LEVEL(NVDBG_LEVEL_INFO, "    RMask=%08lx, GMask=%08lx, BMask=%08lx, AMask=%08lx",
                      pContext->lpLclZ->lpGbl->ddpfSurface.dwRBitMask,
                      pContext->lpLclZ->lpGbl->ddpfSurface.dwGBitMask,
                      pContext->lpLclZ->lpGbl->ddpfSurface.dwBBitMask,
                      pContext->lpLclZ->lpGbl->ddpfSurface.dwRGBAlphaBitMask);
            DPF_LEVEL(NVDBG_LEVEL_INFO, "\r\n");
            dwZetaBitCount = pContext->lpLclZ->lpGbl->ddpfSurface.dwRGBBitCount;
            pdwZetaBitCount = (DWORD *)&pContext->lpLclZ->lpGbl->ddpfSurface.dwRGBBitCount;
            /*
             * Can't do stencil buffer in 16bpp rendering modes.
             */
            if ((pContext->lpLclZ->lpGbl->ddpfSurface.dwFlags & DDPF_STENCILBUFFER)
             && (dwRenderBitCount == 16))
                pContext->dwContextFlags |= NV_CONTEXT_ZETA_BUFFER_MISMATCH;
        }
        else
        {
            DPF_LEVEL(NVDBG_LEVEL_INFO, "No Zeta Format specified.  Make it same as render surface.");
            dwZetaBitCount = pContext->lpLcl->lpGbl->ddpfSurface.dwRGBBitCount;
            pdwZetaBitCount = NULL;
        }
    }
    else
    {
        /*
         * Disable the Z buffer.
         */
        pContext->dwRenderState[D3DRENDERSTATE_ZENABLE]      = FALSE;
        pContext->dwRenderState[D3DRENDERSTATE_ZWRITEENABLE] = FALSE;

        /*
         * There's no Z-Buffer addresses.
         */
        pContext->ZBufferAddr             = 0;
        pContext->ZBufferOffset           = 0;

        /*
         * When there is no z-buffer, the pitch still has to be non-zero. (I hate that!)
         */
        pContext->surfacePitch.wZetaPitch = 32;

        /*
         * No Zeta Buffer color format.
         */
        dwZetaBitCount  = 0;
        pdwZetaBitCount = NULL;
    }

    /*
     * Set the surface format.
     */
    pContext->dwSurfaceFormat = DRF_DEF(053, _SET_FORMAT, _TYPE, _PITCH);

    if ((pDriverData->regValidateZMethod == REG_VZM_FLEXIBLE)
     || ((pDriverData->regValidateZMethod == REG_VZM_MIXED) && (pContext->dwDXVersionLevel < APP_VERSION_DX6)))
    {
        /*
         * Match Z-Buffer Depth to Render Depth if the VALIDATEZMETHOD registry setting is:
         * REG_VZM_FLEXIBLE
         * OR
         * REG_VZM_MIXED and this is a legacy (DX5 or earlier) application.
         */
        switch (dwRenderFormat)
        {
            case 0x00007FFF:
                pContext->dwSurfaceFormat |= DRF_DEF(053, _SET_FORMAT, _COLOR, _LE_X1R5G5B5_Z1R5G5B5);
                dwAdjustedZetaBitCount = 16;
                break;
            case 0x0000FFFF:
                pContext->dwSurfaceFormat |= DRF_DEF(053, _SET_FORMAT, _COLOR, _LE_R5G6B5);
                dwAdjustedZetaBitCount = 16;
                break;
            case 0x00FFFFFF:
                pContext->dwSurfaceFormat |= DRF_DEF(053, _SET_FORMAT, _COLOR, _LE_X8R8G8B8_Z8R8G8B8);
                dwAdjustedZetaBitCount = 24;
                break;
            case 0x7FFFFFFF:
                pContext->dwSurfaceFormat |= DRF_DEF(053, _SET_FORMAT, _COLOR, _LE_X1A7R8G8B8_Z1A7R8G8B8);
                dwAdjustedZetaBitCount = 24;
                break;
            case 0xFFFFFFFF:
                pContext->dwSurfaceFormat |= DRF_DEF(053, _SET_FORMAT, _COLOR, _LE_A8R8G8B8);
                dwAdjustedZetaBitCount = 24;
                break;
        }
        if (pdwZetaBitCount)
            *pdwZetaBitCount = dwAdjustedZetaBitCount;

        /*
         * Keep track of whether or not the z-buffer depth matches the rendering depth.  This information
         * will be used by SetRenderTarget.
         */
        if (((dwZetaBitCount == 16) && (dwRenderBitCount != 16))
         || ((dwZetaBitCount == 24) && (dwRenderBitCount == 16))
         || ((dwZetaBitCount == 32) && (dwRenderBitCount == 16)))
        {
            pContext->dwContextFlags |= NV_CONTEXT_ZETA_BUFFER_MISMATCH;
        }

    }
    else
    {
        /*
         * Make sure Z-Buffer Depth matches the Rendering Depth if the VALIDATEZMETHOD registry setting is:
         * REG_VZM_STRICT
         * OR
         * REG_VZM_MIXED and this is a DX6 or later application.
         */
        if (((dwZetaBitCount == 0) && (dwRenderBitCount != 0))
         || ((dwZetaBitCount == dwRenderBitCount)
         ||  ((dwZetaBitCount == 24) && (dwRenderBitCount == 32))
         ||  ((dwZetaBitCount == 32) && (dwRenderBitCount == 24))))
        {
            switch (dwRenderFormat)
            {
                case 0x00007FFF:
                    pContext->dwSurfaceFormat |= DRF_DEF(053, _SET_FORMAT, _COLOR, _LE_X1R5G5B5_Z1R5G5B5);
                    break;
                case 0x0000FFFF:
                    pContext->dwSurfaceFormat |= DRF_DEF(053, _SET_FORMAT, _COLOR, _LE_R5G6B5);
                    break;
                case 0x00FFFFFF:
                    pContext->dwSurfaceFormat |= DRF_DEF(053, _SET_FORMAT, _COLOR, _LE_X8R8G8B8_Z8R8G8B8);
                    break;
                case 0x7FFFFFFF:
                    pContext->dwSurfaceFormat |= DRF_DEF(053, _SET_FORMAT, _COLOR, _LE_X1A7R8G8B8_Z1A7R8G8B8);
                    break;
                case 0xFFFFFFFF:
                    pContext->dwSurfaceFormat |= DRF_DEF(053, _SET_FORMAT, _COLOR, _LE_A8R8G8B8);
                    break;
            }
        }
        else
        {
            DPF_LEVEL(NVDBG_LEVEL_ERROR, "Render Target Bit Depth and Zeta Bit Depth Don't match");
            return (FALSE);
        }
    }
    /*
     * If the WFar value is 0.0, then set a default based on the z-buffer precision.
     */
    if (pCurrentContext->dvWFar == 0.0f)
    {
        pCurrentContext->dvWNear = 0.0f;
        if ((pCurrentContext->dwSurfaceFormat == (DRF_DEF(053, _SET_FORMAT, _TYPE, _PITCH) | DRF_DEF(053, _SET_FORMAT, _COLOR, _LE_X1R5G5B5_Z1R5G5B5)))
         || (pCurrentContext->dwSurfaceFormat == (DRF_DEF(053, _SET_FORMAT, _TYPE, _PITCH) | DRF_DEF(053, _SET_FORMAT, _COLOR, _LE_R5G6B5))))
        {
            pCurrentContext->dvWFar  = (D3DVALUE)(1 << 16);
        }
        else
        {
            pCurrentContext->dvWFar  = (D3DVALUE)(1 << 24);
        }
        pCurrentContext->dvRWFar = pCurrentContext->dvWFar;
    }
    return (TRUE);
}
BOOL nvSetupContext
(
    PNVD3DCONTEXT   pContext
)
{
#ifdef  NVD3D_DX6
    DWORD   i;
#endif  // NVD3D_DX6

    /*
     * Clear all context flags to start out.
     */
    pContext->dwContextFlags = 0;

    /*
     * First setup the context surface.
     */
    if (!nvSetContextSurface(pContext))
        return (FALSE);

    /*
     * Set the default rendering state for the context.
     * The defaults come from the final DX5 D3D DDK Documentation.
     */
    pContext->dwRenderState[D3DRENDERSTATE_TEXTUREHANDLE]      = 0;
    pContext->dwRenderState[D3DRENDERSTATE_ANTIALIAS]          = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_TEXTUREADDRESS]     = D3DTADDRESS_WRAP;
    pContext->dwRenderState[D3DRENDERSTATE_TEXTUREPERSPECTIVE] = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_WRAPU]              = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_WRAPV]              = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_FILLMODE]           = D3DFILL_SOLID;
    pContext->dwRenderState[D3DRENDERSTATE_SHADEMODE]          = D3DSHADE_GOURAUD;
    pContext->dwRenderState[D3DRENDERSTATE_LINEPATTERN]        = 0;
    pContext->dwRenderState[D3DRENDERSTATE_MONOENABLE]         = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_ROP2]               = R2_COPYPEN;
    pContext->dwRenderState[D3DRENDERSTATE_PLANEMASK]          = 0xFFFFFFFF;
    pContext->dwRenderState[D3DRENDERSTATE_ALPHATESTENABLE]    = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_LASTPIXEL]          = TRUE;
    pContext->dwRenderState[D3DRENDERSTATE_TEXTUREMAG]         = D3DFILTER_NEAREST;
    pContext->dwRenderState[D3DRENDERSTATE_TEXTUREMIN]         = D3DFILTER_NEAREST;
    pContext->dwRenderState[D3DRENDERSTATE_SRCBLEND]           = D3DBLEND_ONE;
    pContext->dwRenderState[D3DRENDERSTATE_DESTBLEND]          = D3DBLEND_ZERO;
    pContext->dwRenderState[D3DRENDERSTATE_TEXTUREMAPBLEND]    = D3DTBLEND_MODULATE;
    pContext->dwRenderState[D3DRENDERSTATE_CULLMODE]           = D3DCULL_CCW;
    pContext->dwRenderState[D3DRENDERSTATE_ZFUNC]              = D3DCMP_LESSEQUAL;
    pContext->dwRenderState[D3DRENDERSTATE_ALPHAREF]           = 0;
    pContext->dwRenderState[D3DRENDERSTATE_ALPHAFUNC]          = D3DCMP_ALWAYS;
    pContext->dwRenderState[D3DRENDERSTATE_DITHERENABLE]       = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_ALPHABLENDENABLE]   = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_FOGENABLE]          = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_SPECULARENABLE]     = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_ZVISIBLE]           = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_SUBPIXEL]           = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_SUBPIXELX]          = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_STIPPLEDALPHA]      = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_FOGCOLOR]           = 0;
    pContext->dwRenderState[D3DRENDERSTATE_FOGTABLEMODE]       = D3DFOG_NONE;
    pContext->dwRenderState[D3DRENDERSTATE_FOGTABLESTART]      = 1;
    pContext->dwRenderState[D3DRENDERSTATE_FOGTABLEEND]        = 100;
    pContext->dwRenderState[D3DRENDERSTATE_FOGTABLEDENSITY]    = 1;
    pContext->dwRenderState[D3DRENDERSTATE_STIPPLEENABLE]      = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_EDGEANTIALIAS]      = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_COLORKEYENABLE]     = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_BORDERCOLOR]        = 0;
    pContext->dwRenderState[D3DRENDERSTATE_TEXTUREADDRESSU]    = D3DTADDRESS_WRAP;
    pContext->dwRenderState[D3DRENDERSTATE_TEXTUREADDRESSV]    = D3DTADDRESS_WRAP;
    pContext->dwRenderState[D3DRENDERSTATE_MIPMAPLODBIAS]      = 0;
    pContext->dwRenderState[D3DRENDERSTATE_ZBIAS]              = 0;
    pContext->dwRenderState[D3DRENDERSTATE_RANGEFOGENABLE]     = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_ANISOTROPY]         = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_FLUSHBATCH]         = FALSE;

    /*
     * Enable the Z-Buffer if there is one attached.
     */
    if (pContext->lpLclZ)
    {
        pContext->dwRenderState[D3DRENDERSTATE_ZENABLE]      = TRUE;
        pContext->dwRenderState[D3DRENDERSTATE_ZWRITEENABLE] = TRUE;
    }
    else
    {
        pContext->dwRenderState[D3DRENDERSTATE_ZENABLE]      = FALSE;
        pContext->dwRenderState[D3DRENDERSTATE_ZWRITEENABLE] = FALSE;
    }

    /*
     * Initialize the hardware LOD bias as the value specified in the registry.
     */
    pContext->dwMipMapLODBias = pDriverData->regLODBiasAdjust;
#ifdef  NVD3D_DX6
    pContext->dwRenderState[D3DRENDERSTATE_TRANSLUCENTSORTINDEPENDENT] = TRUE;
    pContext->dwRenderState[D3DRENDERSTATE_STENCILENABLE]              = FALSE;
    pContext->dwRenderState[D3DRENDERSTATE_STENCILFAIL]                = D3DSTENCILOP_KEEP;
    pContext->dwRenderState[D3DRENDERSTATE_STENCILZFAIL]               = D3DSTENCILOP_KEEP;
    pContext->dwRenderState[D3DRENDERSTATE_STENCILPASS]                = D3DSTENCILOP_KEEP;
    pContext->dwRenderState[D3DRENDERSTATE_STENCILFUNC]                = D3DCMP_ALWAYS;
    pContext->dwRenderState[D3DRENDERSTATE_STENCILREF]                 = 0;
    pContext->dwRenderState[D3DRENDERSTATE_STENCILMASK]                = 0xFFFFFFFF;
    pContext->dwRenderState[D3DRENDERSTATE_STENCILWRITEMASK]           = 0xFFFFFFFF;
    pContext->dwRenderState[D3DRENDERSTATE_TEXTUREFACTOR]              = 0xFFFFFFFF;
    pContext->dwRenderState[D3DRENDERSTATE_WRAP0]                      = 0;
    pContext->dwRenderState[D3DRENDERSTATE_WRAP1]                      = 0;
    pContext->dwRenderState[D3DRENDERSTATE_WRAP2]                      = 0;
    pContext->dwRenderState[D3DRENDERSTATE_WRAP3]                      = 0;
    pContext->dwRenderState[D3DRENDERSTATE_WRAP4]                      = 0;
    pContext->dwRenderState[D3DRENDERSTATE_WRAP5]                      = 0;
    pContext->dwRenderState[D3DRENDERSTATE_WRAP6]                      = 0;
    pContext->dwRenderState[D3DRENDERSTATE_WRAP7]                      = 0;


    /*
     * Initialize the texture stages.  Stage 1-7 are all defaulted to disabled.
     * Stage 0 is defaulted for no texturing.
     */
    pContext->tssState[0].dwTextureMap      = 0;
    pContext->tssState[0].dwAddressU        = D3DTADDRESS_WRAP;
    pContext->tssState[0].dwAddressV        = D3DTADDRESS_WRAP;
    pContext->tssState[0].dwMagFilter       = D3DTFG_POINT;
    pContext->tssState[0].dwMinFilter       = D3DTFN_POINT;
    pContext->tssState[0].dwMipFilter       = D3DTFP_POINT;
    pContext->tssState[0].dwColorOp         = D3DTOP_MODULATE;
    pContext->tssState[0].dwColorArg1       = D3DTA_TEXTURE;
    pContext->tssState[0].dwColorArg2       = D3DTA_CURRENT;
    pContext->tssState[0].dwAlphaOp         = D3DTOP_SELECTARG1;
    pContext->tssState[0].dwAlphaArg1       = D3DTA_TEXTURE;
    pContext->tssState[0].dwAlphaArg2       = D3DTA_CURRENT;
    pContext->tssState[0].dwBumpEnvMat00    = 0;
    pContext->tssState[0].dwBumpEnvMat01    = 0;
    pContext->tssState[0].dwBumpEnvMat10    = 0;
    pContext->tssState[0].dwBumpEnvMat11    = 0;
    pContext->tssState[0].dwTexCoordIndex   = 0;
    pContext->tssState[0].dwBorderColor     = 0x00000000;
    pContext->tssState[0].dwMipMapLODBias   = 0;
    pContext->tssState[0].dwMaxMipLevel     = 0;
    pContext->tssState[0].dwMaxAnisotropy   = 1;
    pContext->tssState[0].dwBumpEnvlScale   = 0;
    pContext->tssState[0].dwBumpEnvlOffset  = 0;
    pContext->tssState[0].bStageNumber      = 0;
    pContext->tssState[0].bWrapU            = 0;
    pContext->tssState[0].bWrapV            = 0;
    pContext->tssState[0].bLODBias          = (BYTE)(pDriverData->regLODBiasAdjust & 0x000000FF);

    for (i = 1; i < 8; i++)
    {
        pContext->tssState[i].dwTextureMap      = 0;
        pContext->tssState[i].dwAddressU        = D3DTADDRESS_WRAP;
        pContext->tssState[i].dwAddressV        = D3DTADDRESS_WRAP;
        pContext->tssState[i].dwMagFilter       = D3DTFG_POINT;
        pContext->tssState[i].dwMinFilter       = D3DTFN_POINT;
        pContext->tssState[i].dwMipFilter       = D3DTFP_POINT;
        pContext->tssState[i].dwColorOp         = D3DTOP_DISABLE;
        pContext->tssState[i].dwColorArg1       = D3DTA_TEXTURE;
        pContext->tssState[i].dwColorArg2       = D3DTA_CURRENT;
        pContext->tssState[i].dwAlphaOp         = D3DTOP_DISABLE;
        pContext->tssState[i].dwAlphaArg1       = D3DTA_TEXTURE;
        pContext->tssState[i].dwAlphaArg2       = D3DTA_CURRENT;
        pContext->tssState[i].dwBumpEnvMat00    = 0;
        pContext->tssState[i].dwBumpEnvMat01    = 0;
        pContext->tssState[i].dwBumpEnvMat10    = 0;
        pContext->tssState[i].dwBumpEnvMat11    = 0;
        pContext->tssState[i].dwTexCoordIndex   = 0;
        pContext->tssState[i].dwBorderColor     = 0x00000000;
        pContext->tssState[i].dwMipMapLODBias   = 0;
        pContext->tssState[i].dwMaxMipLevel     = 0;
        pContext->tssState[i].dwMaxAnisotropy   = 1;
        pContext->tssState[i].dwBumpEnvlScale   = 0;
        pContext->tssState[i].dwBumpEnvlOffset  = 0;
        pContext->tssState[i].bStageNumber      = (BYTE)i;
        pContext->tssState[i].bWrapU            = 0;
        pContext->tssState[i].bWrapV            = 0;
        pContext->tssState[i].bLODBias          = (BYTE)(pDriverData->regLODBiasAdjust & 0x000000FF);
    }
    pContext->dwStageCount = 1;
    pContext->bUseDX6Class = FALSE;
    pContext->dwContextFlags |= NV_CONTEXT_TBLEND_UNINITIALIZED;
    pContext->bUseTBlendSettings = FALSE;
#endif  // NVD3D_DX6

    /*
     * Allocate a default 1x1 texture.
     */
    nvAllocDefaultTexture(pContext);

    /*
     * Set the hardware state.
     */
    pCurrentContext = pContext;

    /*
     * Force an initial state load.
     */
    pCurrentContext->dwStateChange = TRUE;
    NV_FORCE_TRI_SETUP(pCurrentContext);
    return(TRUE);
}
BOOL nvFinalContextCleanup
(
    void
)
{
    return (TRUE);
}
void nvAllocDefaultTexture
(
    PNVD3DCONTEXT   pContext
)
{
    unsigned char  *pData = 0;
#ifndef NVHEAPMGR
    LPDDRAWI_DIRECTDRAW_GBL pdrv     = pContext->lpLcl->lpGbl->lpDD;
#endif

    pContext->dwDefaultTextureContextDma  = NV054_FORMAT_CONTEXT_DMA_B;
#ifndef WINNT
    NVHEAP_ALLOC(((DWORD)pData), 2 + NV_TEXTURE_PAD, TYPE_TEXTURE);
#else
    NVHEAP_ALLOC(((DWORD)pData), ((2 + NV_TEXTURE_PAD + NV_TEXTURE_OFFSET_ALIGN) & ~pDriverData->dwSurfaceAlignPad), TYPE_TEXTURE);
#endif // #ifdef WINNT
    //pData = (unsigned char *)DDHAL32_VidMemAlloc(pContext->lpLcl->lpGbl->lpDD, 0, ((2 + NV_TEXTURE_PAD + NV_TEXTURE_OFFSET_ALIGN) & ~pDriverData->dwSurfaceAlignPad), 1);
    if (pData)
    {
        pContext->dwDefaultTextureBase   = (DWORD)pData;
        pContext->dwDefaultTextureOffset = VIDMEM_OFFSET((((DWORD)pData + NV_TEXTURE_OFFSET_ALIGN) & ~NV_TEXTURE_OFFSET_ALIGN) - pDriverData->BaseAddress);
        pData = (unsigned char *)VIDMEM_ADDR((pContext->dwDefaultTextureOffset));

        pData[0] = 0xFF;
        pData[1] = 0xFF;

        /*
         * Increment count of video memory surfaces allocated.
         */
        pDriverData->DDrawVideoSurfaceCount++;
    }
    else
    {
        pContext->dwDefaultTextureBase   = (DWORD)0;
        pContext->dwDefaultTextureOffset = 0;
    }
    pContext->dwDefaultTextureColorFormat = NV054_FORMAT_COLOR_LE_A1R5G5B5;


    /*
     * init psuedo-notifier memory
     */
    {
        DWORD size = 16;
#ifndef WINNT
        memset ((void*)(pDriverData->dwTMPseudoNotifierOffset + pDriverData->BaseAddress),0,size);
        *(DWORD*)(pDriverData->dwTMPseudoNotifierOffset + pDriverData->BaseAddress) = pDriverData->dwTMStageAddr;
#else
        memset ((void*)(pDriverData->dwTMPseudoNotifierMem),0,size);
        *(DWORD*)(pDriverData->dwTMPseudoNotifierMem) = pDriverData->dwTMStageAddr;
#endif
        pDriverData->dwTMHead = pDriverData->dwTMStageAddr;
        pDriverData->dwTMTail = pDriverData->dwTMStageAddr;
        pDriverData->dwRenderedFrame = pDriverData->dwCurrentFrame = 0;
        global.dwHWTextureDate = global.dwSWTextureDate;
    }

    /*
     * invalidate fvfData cache
     */
    fvfData.dwVertexType = 0;

    return;
}
void nvFreeDefaultTexture
(
    PNVD3DCONTEXT   pContext
)
{
#ifndef NVHEAPMGR
#ifndef WINNT // BUGBUG where do we get this from under NT?
    LPDDRAWI_DIRECTDRAW_GBL pdrv     = pContext->lpLcl->lpGbl->lpDD;
#else
    LPDDRAWI_DIRECTDRAW_GBL pdrv     = pContext->lpLcl->lpGbl->dwReserved1;
#endif // #ifndef WINNT
#endif
    if (!pContext) return;
    if ((pContext->lpLcl) &&
#ifndef WINNT
    (pContext->lpLcl->lpGbl) &&
    (pContext->lpLcl->lpGbl->lpDD) &&
#else
    (pContext->lpDD) &&
#endif // #ifndef WINNT
            (pContext->dwDefaultTextureBase))

    {
        NVHEAP_FREE (pContext->dwDefaultTextureBase);

        //DDHAL32_VidMemFree(pContext->lpLcl->lpGbl->lpDD, 0, pContext->dwDefaultTextureBase);
        pContext->dwDefaultTextureBase   = 0;
        pContext->dwDefaultTextureOffset = 0;

        /*
         * Increment count of video memory surfaces allocated.
         */
        pDriverData->DDrawVideoSurfaceCount--;
    }

    return;
}
#define NV4_PUSH_BUFFER_PAD     64

void nvD3DReenable()
{
    PNVD3DCONTEXT   pContext = 0;

    NV_D3D_GLOBAL_SETUP();

    /*
     * Run through all valid contexts and update push buffer information.
     */
    pContext = (PNVD3DCONTEXT)pDriverData->dwContextListHead;
    while (pContext)
    {
        pContext->dwDefaultTextureBase = 0;
        pContext->dwContextReset       = TRUE;
        pContext = pContext->pContextNext;
    }

    /*
     * Force reset of D3D rendering target and z-buffer.
     */
    pDriverData->fFullScreenDosOccurred  = 0;
    pDriverData->lpLast3DSurfaceRendered = 0;
    pDriverData->dwCurrentContextHandle  = 0;
    NV_D3D_GLOBAL_SAVE();
}

void nvResetContext
(
    PNVD3DTEXTURE   pTexture
)
{
    if (!pCurrentContext->dwDefaultTextureBase)
        nvAllocDefaultTexture(pCurrentContext);
    return;
}

/*
 * I'm just going to create my own objects for now....
 */
void nvD3DCreateObjects
(
    void
)
{
#ifdef HW_PAL8
    DWORD dwCount;
#endif

    if (!pDriverData->NvDevFlatDma)
        return;

    nvDmaPushOffsetMax        = pDriverData->dwDmaPushOffsetMax = pDriverData->NvDmaPusherBufferSize - NV4_PUSH_BUFFER_PAD;
    nvDmaPushBase             = pDriverData->NvDmaPusherBufferBase;
    nvFifo                    = pDriverData->NvDmaPusherPutAddress;
    nvFreeCount               = 0;
    pCurrentContext           = NULL; // no contexes created yet

    if (!nvFifo)
        return;

    /*
     * Get fifo address and the current free count.
     */
#ifdef  CACHE_FREECOUNT
    nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
    NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#else   // CACHE_FREECOUNT
    nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE,
                     (sizeSetObject + sizeCreateObject));
#endif  // CACHE_FREECOUNT

    /*
     * Make sure there aren't any dangling DMA context object.
     */
#ifdef MULTI_MON
    D3DDestroyTextureContexts();
#endif

#if 0 // BUGBUG -- Object creation needs to be moved and converted
#ifdef HW_PAL8
    /*
     * Create Indexed Image From CPU object for
     * palettized texture swizzling.                    NV 0x060
     */
    while (nvFreeCount < (sizeSetObject + sizeCreateObject))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeCreateObject));
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, NV04_INDEXED_IMAGE_FROM_CPU);
    nvglCreateObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_INDEXED_IMAGE_FROM_CPU);

    /*
     * Create an Image From CPU object for moving
     * palettes into video memory                       NV 0x061
     */
    while (nvFreeCount < (sizeSetObject + sizeCreateObject))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeCreateObject));
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, NV04_IMAGE_FROM_CPU);
    nvglCreateObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_IMAGE_FROM_CPU);
#endif // HW_PAL8
#endif // BUGBUG

#ifndef MULTI_MON
    /*
     * Create Context Dma To Memory for notifiers.      NV 0xFFD
     */
    while (nvFreeCount < (sizeSetObject + sizeCreateObject))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeCreateObject));
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, NV_CONTEXT_DMA_TO_MEMORY);
    nvglCreateObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_NOTIFY_CONTEXT_DMA_TO_MEMORY);

    /*
     * Hook up DMA contexts.
     */
    while (nvFreeCount < (sizeSetObject + sizeCreateObject))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetDmaToMemory));
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_NOTIFY_CONTEXT_DMA_TO_MEMORY);
    nvglSetDmaToMemory(nvFifo, nvFreeCount, NV_DD_SPARE, pDriverData->NvDmaPusherSyncNotifierFlat, NvGetFlatDataSelector(), (sizeof(NvNotification) - 1));
#endif  // !MULTI_MON

    nvStartDmaBuffer (FALSE);
    if (nvTMCreate())
    {
#ifdef  CACHE_FREECOUNT
        pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#ifdef  DEBUG
        NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  // CACHE_FREECOUNT
        /*
         * Creation failed
         */
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "Texture Manager Creation failed");
        return;
    }

    /*
     * Point system memory texture context at video memory for now.  This will get changed later.
     * This is just to make sure that it is pointing at memory of some sort.
     */
#ifdef  MULTI_MON
    nvStartDmaBuffer (FALSE);
#ifndef WINNT
    if (NvRmAllocContextDma(pDriverData->dwRootHandle,
                        D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY,
                        NV01_CONTEXT_DMA,
                        (ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY |
                         ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                        (PVOID)pDriverData->BaseAddress,
                        0xFF) != ALLOC_CTX_DMA_STATUS_SUCCESS)
                            return(FALSE);
#else
    if (NvRmAllocContextDma(pDriverData->dwRootHandle,
                        D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY,
                        NV01_CONTEXT_DMA,
                        (ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY |
                         ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                        pDriverData->ppdev->pjScreen,
                        0xFF) != ALLOC_CTX_DMA_STATUS_SUCCESS)
                            return(FALSE);
#endif // #ifdef WINNT

    pDriverData->dwTextureContextValid = TRUE;
#else   // !MULTI_MON
    while (nvFreeCount < (sizeSetObject + sizeSetDmaFromMemoryNotify))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetDmaFromMemoryNotify));
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY);
    nvglSetDmaFromMemory(nvFifo, nvFreeCount, NV_DD_SPARE,
                         pDriverData->BaseAddress, pDriverData->flatSelector, 0xFF);

    /*
     * Set the notify context.
     */
    while (nvFreeCount < (sizeSetObject + sizeSetDmaFromMemoryContextNotify))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetDmaFromMemoryContextNotify));
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_CONTEXT_LINEAR_TEXTURE_FROM_MEMORY);
    nvglSetDmaFromMemoryContextNotify(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_NOTIFY_CONTEXT_DMA_TO_MEMORY);

    while (nvFreeCount < (sizeSetObject + sizeSetDmaFromMemoryContextNotify))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetDmaFromMemoryContextNotify));
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY);
    nvglSetDmaFromMemoryContextNotify(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_NOTIFY_CONTEXT_DMA_TO_MEMORY);
#endif  // !MULTI_MON

    /*
     * Hook up the ARGB_ZS surface context.
     */
    while (nvFreeCount < (sizeSetObject + sizeSetRenderTargetContexts))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetRenderTargetContexts));
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_CONTEXT_SURFACES_ARGB_ZS);
    nvglSetRenderTargetContexts(nvFifo, nvFreeCount, NV_DD_SPARE,
                                NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);

    /*
     * Hook up the Render Solid Rectangle.
     */
    while (nvFreeCount < (sizeSetObject + sizeSetNv4SolidRectangleSurface))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetNv4SolidRectangleSurface));
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_RENDER_SOLID_RECTANGLE);
    nvglSetNv4SolidRectangleSurface(nvFifo, nvFreeCount, NV_DD_SPARE, NV_DD_SURFACES_2D);

    /*
     * Hook up Render Solid Rectangle 2.
     */
    while (nvFreeCount < (sizeSetObject + sizeSetNv4SolidRectangleSurface + 2*sizeSetStartMethod + 2*sizeSetData))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetNv4SolidRectangleSurface + 2*sizeSetStartMethod + 2*sizeSetData));
    nvglSetObject      (nvFifo, nvFreeCount, NV_DD_SPARE, D3D_RENDER_SOLID_RECTANGLE_2);
    nvglSetNv4SolidRectangleSurface(nvFifo, nvFreeCount, NV_DD_SPARE, NV_DD_SURFACES_2D);
    nvglSetStartMethod (nvFifo,nvFreeCount,NV_DD_SPARE, NV05E_SET_OPERATION,1);
    nvglSetData        (nvFifo,nvFreeCount,             NV05E_SET_OPERATION_SRCCOPY);
    nvglSetStartMethod (nvFifo,nvFreeCount,NV_DD_SPARE, NV05E_SET_COLOR_FORMAT,1);
    nvglSetData        (nvFifo,nvFreeCount,             NV05E_SET_COLOR_FORMAT_LE_X8R8G8B8);

    /*
     * Hook up the Dx5 Textured Triangle.
     * System/AGP goes to context A, Video Memory goes to context B.
     */
    while (nvFreeCount < (sizeSetObject + sizeSetDx5TriangleContexts + sizeSetDx5TriangleSurface))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE,
                         (sizeSetObject + sizeSetDx5TriangleContexts + sizeSetDx5TriangleSurface));
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_DX5_TEXTURED_TRIANGLE);

    /*
     * Both A and B must be valid.
     */
    nvglSetDx5TriangleContexts(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);
    nvglSetDx5TriangleSurface(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_CONTEXT_SURFACES_ARGB_ZS);


    /*
     * Plug in the notify context.
     */
#ifdef  MULTI_MON
    while (nvFreeCount < (sizeSetStartMethod + sizeSetData))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetStartMethod + sizeSetData))
    nvglSetStartMethod(nvFifo, nvFreeCount, NV_DD_SPARE, NV054_SET_CONTEXT_DMA_NOTIFIES, 1);
    nvglSetData(nvFifo, nvFreeCount, NV_DD_DMA_PUSHER_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
#else   // !MULTI_MON
    while (nvFreeCount < (sizeSetStartMethod + sizeSetData))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetStartMethod + sizeSetData))
    nvglSetStartMethod(nvFifo, nvFreeCount, NV_DD_SPARE, NV054_SET_CONTEXT_DMA_NOTIFIES, 1);
    nvglSetData(nvFifo, nvFreeCount, D3D_NOTIFY_CONTEXT_DMA_TO_MEMORY);
#endif  // !MULTI_MON

    /*
     * Hook up the Dx6 Multi Textured Triangle.
     * System/AGP goes to context A, Video Memory goes to context B.
     */
    while (nvFreeCount < (sizeSetObject + sizeSetDx6TriangleContexts + sizeSetDx6TriangleSurface))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE,
                         (sizeSetObject + sizeSetDx6TriangleContexts + sizeSetDx6TriangleSurface));
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_DX6_MULTI_TEXTURE_TRIANGLE);

    /*
     * Both A and B must be valid.
     */
    nvglSetDx6TriangleContexts(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);
    nvglSetDx6TriangleSurface(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_CONTEXT_SURFACES_ARGB_ZS);

    /*
     * Plug in the notify context.
     */
#ifdef  MULTI_MON
    while (nvFreeCount < (sizeSetStartMethod + sizeSetData))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetStartMethod + sizeSetData))
    nvglSetStartMethod(nvFifo, nvFreeCount, NV_DD_SPARE, NV055_SET_CONTEXT_DMA_NOTIFIES, 1);
    nvglSetData(nvFifo, nvFreeCount, NV_DD_DMA_PUSHER_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
#else   // !MULTI_MON
    while (nvFreeCount < (sizeSetStartMethod + sizeSetData))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetStartMethod + sizeSetData))
    nvglSetStartMethod(nvFifo, nvFreeCount, NV_DD_SPARE, NV055_SET_CONTEXT_DMA_NOTIFIES, 1);
    nvglSetData(nvFifo, nvFreeCount, D3D_NOTIFY_CONTEXT_DMA_TO_MEMORY);
#endif  // !MULTI_MON

    /*
     * Hook up the swizzled surface context.
     */
    while (nvFreeCount < (sizeSetObject + sizeSetSwizzledSurfaceContextImage))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetSwizzledSurfaceContextImage));
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_CONTEXT_SURFACE_SWIZZLED);
    nvglSetSwizzledSurfaceContextImage(nvFifo, nvFreeCount, NV_DD_SPARE, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);

    /*
     * Hook up the scaled image from memory object.
     * - Set the source image context.
     * - Set the destination surface context.
     * - Set operation
     */
    while (nvFreeCount < sizeSetObject)
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, sizeSetObject);
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_SCALED_IMAGE_FROM_MEMORY);

    while (nvFreeCount < sizeSetScaledImageContextImage)
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, sizeSetScaledImageContextImage);
    nvglSetScaledImageContextImage(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_CONTEXT_LINEAR_TEXTURE_FROM_MEMORY);

    while (nvFreeCount < sizeSetScaledImageContextSurface)
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, sizeSetScaledImageContextSurface);
#ifdef DIRECTSWIZZLE
    nvglSetScaledImageContextSurface(nvFifo, nvFreeCount, NV_DD_SPARE, NV_DD_SURFACES_2D);
#else
    nvglSetScaledImageContextSurface(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_CONTEXT_SURFACE_SWIZZLED);
#endif

    while (nvFreeCount < (sizeSetScaledImageOperation))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetScaledImageOperation));
    nvglSetScaledImageOperation (nvFifo,nvFreeCount,NV_DD_SPARE, NV077_SET_OPERATION_SRCCOPY);

#ifdef HW_PAL8
    /*
     * Initialize Indexed Image from CPU object:
     * - set destination surface
     * - set LUT color format
     * - set index format: 8-bit (256-color palette) or 4-bit (16-color palette))
     * - set operation
     * - set point
     */
    while (nvFreeCount < sizeSetObject)
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, sizeSetObject);
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_INDEXED_IMAGE_FROM_CPU);

    // nv05 will be able to write to a swizzled surface. alas nv04 cannot
    //    while (nvFreeCount < sizeSetIndexedImageContextSurface)
    //        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, sizeSetIndexedImageContextSurface);
    //    nvglSetIndexedImageContextSurface(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_CONTEXT_SURFACE_SWIZZLED);
    while (nvFreeCount < sizeSetIndexedImageContextSurface)
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, sizeSetIndexedImageContextSurface);
    nvglSetIndexedImageContextSurface(nvFifo, nvFreeCount, NV_DD_SPARE, NV_DD_SURFACES_2D);

    while (nvFreeCount < sizeSetIndexedImageColorFormat)
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, sizeSetIndexedImageColorFormat);
    nvglSetIndexedImageColorFormat(nvFifo,nvFreeCount,NV_DD_SPARE, NV060_SET_COLOR_FORMAT_LE_A8R8G8B8);

    // we only support 8-bit indices for now, so the default is OK here...
    //    while (nvFreeCount < sizeSetIndexedImageIndexFormat)
    //        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, sizeSetIndexedImageIndexFormat);
    //    nvglSetIndexedImageIndexFormat(nvFifo,nvFreeCount,NV_DD_SPARE, NV_060_blahblahblah);

    while (nvFreeCount < (sizeSetIndexedImageOperation))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetIndexedImageOperation));
    nvglSetIndexedImageOperation (nvFifo, nvFreeCount, NV_DD_SPARE, NV060_SET_OPERATION_SRCCOPY);

    while (nvFreeCount < (sizeSetIndexedImagePoint))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetIndexedImagePoint));
    nvglSetIndexedImagePoint (nvFifo, nvFreeCount, NV_DD_SPARE, 0, 0);

    /*
     * Initialize Image from CPU object for moving palettes to video memory:
     * - set destination surface
     * - set opertion
     * - set format
     * - set point
     * - set sizes
     */
    dwCount = sizeSetObject + sizeSetStartMethod + 1*sizeSetData;
    while (nvFreeCount < dwCount)
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, dwCount);
    nvglSetObject      (nvFifo, nvFreeCount, NV_DD_SPARE, D3D_IMAGE_FROM_CPU);
    nvglSetStartMethod (nvFifo, nvFreeCount, NV_DD_SPARE, NV061_SET_CONTEXT_SURFACE, 1);
    nvglSetData        (nvFifo, nvFreeCount, NV_DD_SURFACES_2D);  // context surface

    dwCount = sizeSetStartMethod + 2*sizeSetData;
    while (nvFreeCount < dwCount)
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, dwCount);
    nvglSetStartMethod (nvFifo, nvFreeCount, NV_DD_SPARE, NV061_SET_OPERATION, 2);
    nvglSetData        (nvFifo, nvFreeCount, NV061_SET_OPERATION_SRCCOPY);  // operation
    nvglSetData        (nvFifo, nvFreeCount, NV061_SET_COLOR_FORMAT_LE_A8R8G8B8);  // format

#endif

    /*
     * Only need to set the operation once. This is only used for clears and those are always srccopy.
     */
    while (nvFreeCount < (sizeSetObject + sizeSetNv4SolidRectangleOperation))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetNv4SolidRectangleOperation));
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_RENDER_SOLID_RECTANGLE);
    nvglSetNv4SolidRectangleOperation(nvFifo, nvFreeCount, NV_DD_SPARE, NV05E_SET_OPERATION_SRCCOPY);

    NV_D3D_GLOBAL_SAVE();
    nvFlushDmaBuffers();
    NV_D3D_GLOBAL_SETUP();
#ifdef  CACHE_FREECOUNT
#ifdef  DEBUG
    NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
    pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#endif  // CACHE_FREECOUNT
}

void nvD3DDestroyObjects
(
    void
)
{
    /*
     * Destroy Render Solid Rectangle for clearing the z-buffer and rendering surfaces.
     */
    if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        D3D_RENDER_SOLID_RECTANGLE) != NVOS00_STATUS_SUCCESS)
            return(0);

    /*
     * Destroy Render Solid Rectangle 2
     */
    if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        D3D_RENDER_SOLID_RECTANGLE_2) != NVOS00_STATUS_SUCCESS)
            return(0);

#ifndef MULTI_MON
    /*
     * Destroy Context Dma To Memory for notifiers.
     */
    while (nvFreeCount < (sizeSetObject + sizeDestroyObject))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeDestroyObject));
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, NV_CONTEXT_DMA_TO_MEMORY);
    nvglDestroyObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_NOTIFY_CONTEXT_DMA_TO_MEMORY);

    /*
     * Destroy Context Dma From Memory for the source linear texture.
     * Destroy Context Dma From Memory for swizzled texture heap (PCI/AGP).
     */
    while (nvFreeCount < (sizeSetObject + (sizeDestroyObject * 3)))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeDestroyObject));
    nvglSetObject(nvFifo, nvFreeCount, NV_DD_SPARE, NV_CONTEXT_DMA_FROM_MEMORY);
    nvglDestroyObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_CONTEXT_LINEAR_TEXTURE_FROM_MEMORY);
    nvglDestroyObject(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY);
#endif  // !MULTI_MON

    /*
     * Destroy Scaled Image From Memory object for texture swizzling.
     */
    if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        D3D_SCALED_IMAGE_FROM_MEMORY) != NVOS00_STATUS_SUCCESS)
            return(0);

    /*
     * Destroy the swizzled surface context.
     */
    if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        D3D_CONTEXT_SURFACE_SWIZZLED) != NVOS00_STATUS_SUCCESS)
            return(0);

    /*
     * Destroy the ARGB_ZS surface context.
     */
    if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        D3D_CONTEXT_SURFACES_ARGB_ZS) != NVOS00_STATUS_SUCCESS)
            return(0);

    /*
     * Destroy the DX6 Multi Textured Triangle.
     */
    if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        D3D_DX6_MULTI_TEXTURE_TRIANGLE) != NVOS00_STATUS_SUCCESS)
            return(0);

    /*
     * Destroy the DX5 Textured Triangle.
     */
    if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        D3D_DX5_TEXTURED_TRIANGLE) != NVOS00_STATUS_SUCCESS)
            return(0);

    nvTMDestroy();
}
#endif  // NV4
