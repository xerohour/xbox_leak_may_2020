/*
 * Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: d3dMini.cpp                                                       *
*   The Direct3D HAL driver routines are implemented in this module.        *
*   This file is a hardware independant file.                               *
*   DO NOT PUT HARDWARE SPECIFIC CODE IN THIS FILE                          *
*   Hardware specific code can be found in:                                 *
*   nv3mini.c, nv4mini.c, ...                                               *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/02/97 - re-wrote and cleaned up.     *
*       Charles Inman               01/31/98 - Ported for NV4               *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

/*
 * Direct3D HAL Callback Table.
 */
static D3DHAL_CALLBACKS NvD3DHALCallbacks =
{
    sizeof(D3DHAL_CALLBACKS),
    /*
     * Device context
     */
    nvContextCreate,            /* Required. */
    nvContextDestroy,           /* Required. */
    nvContextDestroyAll,        /* Required. */
    /*
     * Scene capture
     */
    nvSceneCapture,             /* Optional. */
    /*
     * Execution
     */
    0,                          /* Optional. Not needed if only rasterization */
    0,
#ifdef WINNT                    // must be at least DX7 and these calls are therefore obsolete
    0L,
    0L,
#else
    nvRenderState,              /* Required if no Execute */
    nvRenderPrimitive,          /* Required if no Execute */
#endif
    0L,                         /* Reserved, must be zero */
    /*
     * Textures
     */
    nvTextureCreate,            /* If any of these calls are supported, */
    nvTextureDestroy,           /* they must all be. */
    nvTextureSwap,              /* ditto - but can always fail. */
    nvTextureGetSurf,           /* ditto - but can always fail. */
    /*
     * Transform - must be supported if lighting is supported.
     */
    0,                          /* If any of these calls are supported, */
    0,                          /* they must all be. */
    0,                          /* ditto */
    0,                          /* ditto */
    0,                          /* ditto */
    /*
     * Lighting
     */
    0,                          /* If any of these calls are supported, */
    0,                          /* they must all be. */
    0,                          /* ditto */
    0,                          /* ditto */
    0,                          /* ditto */
    /*
     * Pipeline state
     */
#ifdef WINNT                    // must be at least DX7 and these calls are therefore obsolete
    0,
#else
    nvGetState,                 /* Required if implementing Execute. */
#endif // !WINNT
    /*
     * Reserved
     */
    0L,                         /* Reserved, must be zero */
    0L,                         /* Reserved, must be zero */
    0L,                         /* Reserved, must be zero */
    0L,                         /* Reserved, must be zero */
    0L,                         /* Reserved, must be zero */
    0L,                         /* Reserved, must be zero */
    0L,                         /* Reserved, must be zero */
    0L,                         /* Reserved, must be zero */
    0L,                         /* Reserved, must be zero */
    0L,                         /* Reserved, must be zero */
};

/*
 * Define supported vertex buffer callbacks
 */
static DDHAL_DDEXEBUFCALLBACKS NvD3DBufCallbacks =
{
    sizeof(DDHAL_DDEXEBUFCALLBACKS),
    0x1F,       // hardcoded to all, runtime doesn't even check, so it doesn't REALLY matter
    D3DCanCreateExecuteBuffer32,
    D3DCreateExecuteBuffer32,
    D3DDestroyExecuteBuffer32,
    D3DLockExecuteBuffer32,
    D3DUnlockExecuteBuffer32,
};

#ifndef TEXFORMAT_CRD

/*
 * Define supported texture formats.
 */
static DDSURFACEDESC Nv4TextureFormats[] =
{
    NV4_TEXTURE_FORMATS             // Texture formats defined in NVTEXFMTS.H
};

static DDSURFACEDESC celsiusTextureFormats[] =
{
    CELSIUS_TEXTURE_FORMATS         // Texture formats defined in NVTEXFMTS.H
};
static DDSURFACEDESC celsiusTextureFormatsNoDXT[] =
{
    CELSIUS_TEXTURE_FORMATS_NO_DXT  // Texture formats defined in NVTEXFMTS.H
};

#if (NVARCH >= 0x020)
static DDSURFACEDESC kelvinTextureFormats[] =
{
    KELVIN_TEXTURE_FORMATS         // Texture formats defined in NVTEXFMTS.H
};

static DDSURFACEDESC kelvinTextureFormatsNoDXT[] =
{
    KELVIN_TEXTURE_FORMATS_NO_DXT  // Texture formats defined in NVTEXFMTS.H
};
#endif // (NVARCH >= 0x020)

#endif  // !TEXFORMAT_CRD

/*
 * --------------------------------------------------------------------------
 *  Direct 3D HAL driver creation routines.
 *  Functions used to instantiate the 3D portion of the DirectDraw HAL
 *  Will be called (for example) from invokeCreateDriver() from ddsamp.
 *
 *  D3DHALCreateDriver
 *
 *  LPD3DHAL_GLOBALDRIVERDATA lpGlobal
 *      - A pointer to a structure containing alot of global information about
 *        our driver.
 *
 *  LPD3DHAL_CALLBACKS* lplpHALCallbacks
 *      - A pointer to a structure we fill in with the callbacks that this
 *        driver supports.
 * --------------------------------------------------------------------------
 */
BOOL __stdcall D3DHALCreateDriver
(
    LPD3DHAL_GLOBALDRIVERDATA  lpGlobal,
    LPD3DHAL_CALLBACKS         *lplpHALCallbacks,
    LPDDHAL_DDEXEBUFCALLBACKS  *lplpBufCallbacks,
    GLOBALDATA                 *pDriverData
)
{
    /*
     * check if driver is valid
     */
    {
        extern BYTE bDeveloperID[32 + 32];
        DWORD i,c;

        for (i = 32,c = 0; i < 64; i++)
        {
            if ((i & 7) == 7)
            {
                c ^= 0x42;
                if ((c ^ bDeveloperID[i]) & 0xfe)
                {
                    DPF_LEVEL (NVDBG_LEVEL_ERROR, "Invalid Driver ID");
                    return FALSE;
                }
                c = 0;
            }
            else
            {
                c ^= bDeveloperID[i];
            }
        }
    }

    // create context tables
    if (!CreateItemArrays(pDriverData))
    {
        DPF_LEVEL (NVDBG_LEVEL_ERROR, "Failed to initialize arrays");
        return FALSE;
    }

    //
    // Here we fill in the supplied structures.
    //
    memset(lpGlobal, 0, sizeof(D3DHAL_GLOBALDRIVERDATA));
    lpGlobal->dwSize              = sizeof(D3DHAL_GLOBALDRIVERDATA);
    lpGlobal->hwCaps              = getDC()->nvD3DDevCaps.dd1Caps;
    lpGlobal->dwNumVertices       = 32768;
    lpGlobal->dwNumClipVertices   = 32768;

#ifdef TEXFORMAT_CRD // --------------------------------

    lpGlobal->dwNumTextureFormats = getDC()->dwSurfaceDescriptorCount;
    lpGlobal->lpTextureFormats    = getDC()->pSurfaceDescriptors;

#else  // !TEXFORMAT_CRD // --------------------------------

#if (NVARCH >= 0x020)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN)
    {
#ifdef DXT_SUPPORT
        if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_TEXTURECOMPRESSIONENABLE_MASK) == D3D_REG_TEXTURECOMPRESSIONENABLE_ENABLE) {
            lpGlobal->dwNumTextureFormats = sizeof(kelvinTextureFormats) / sizeof(DDSURFACEDESC);
            lpGlobal->lpTextureFormats    = &kelvinTextureFormats[0];
        }
        else
#endif  // DXT_SUPPORT
        {
            lpGlobal->dwNumTextureFormats = sizeof(kelvinTextureFormatsNoDXT) / sizeof(DDSURFACEDESC);
            lpGlobal->lpTextureFormats    = &kelvinTextureFormatsNoDXT[0];
        }
    }
    else
#endif  // NVARCH >= 0x020
#if (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS)
    {
#ifdef DXT_SUPPORT
        if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_TEXTURECOMPRESSIONENABLE_MASK) == D3D_REG_TEXTURECOMPRESSIONENABLE_ENABLE) {
            lpGlobal->dwNumTextureFormats = sizeof(celsiusTextureFormats) / sizeof(DDSURFACEDESC);
            lpGlobal->lpTextureFormats    = &celsiusTextureFormats[0];
        }
        else
#endif  // DXT_SUPPORT
        {
            lpGlobal->dwNumTextureFormats = sizeof(celsiusTextureFormatsNoDXT) / sizeof(DDSURFACEDESC);
            lpGlobal->lpTextureFormats    = &celsiusTextureFormatsNoDXT[0];
        }
    }
    else
#endif (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_DXTRI) {
        lpGlobal->dwNumTextureFormats = sizeof(Nv4TextureFormats) / sizeof(DDSURFACEDESC);
        lpGlobal->lpTextureFormats    = &Nv4TextureFormats[0];
    }
    else {
        DPF("Unknown architecture");
        dbgD3DError();
        lpGlobal->dwNumTextureFormats = sizeof(Nv4TextureFormats) / sizeof(DDSURFACEDESC);
        lpGlobal->lpTextureFormats    = &Nv4TextureFormats[0];
    }

#endif  // !TEXFORMAT_CRD --------------------------------

    /*
     * Modify the capabilities as neccessary for the registry settings.
     */
    D3DModifyCapabilities(lpGlobal, pDriverData);

    /*
     * Return the HAL table.
     */
    *lplpHALCallbacks = &NvD3DHALCallbacks;
#ifndef DO_NOT_SUPPORT_VERTEX_BUFFERS // see nvprecomp.h
    if (
#if (NVARCH >= 0x020)
        (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) ||
#endif // (NVARCH >= 0x020)
        (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS)
       )
    {

        *lplpBufCallbacks = &NvD3DBufCallbacks;
    }
    else
#endif
    {
        *lplpBufCallbacks = NULL;
    }
    return (TRUE);
}

/*
 * --------------------------------------------------------------------------
 * Initialization routines.
 * --------------------------------------------------------------------------
 */

/*
 * This routine creates the various dynamic item arrays used by the Direct3D
 * driver for managing contexts and textures. This routine should be called
 * at DLL_PROCESS_ATTACH time from DllMain.
 */
BOOL CreateItemArrays
(
    GLOBALDATA *pDriverData
)
{
    /*
     * Initialize memory system.
     */
    if (!DDrvItemArrayInit())
    {
        DPF_LEVEL (NVDBG_LEVEL_ERROR, "CreateItemArrays - Failed to initialize memory subsystem");
        return (FALSE);
    }

    /*
     * Create the D3D context heap.
     */
    if (!nvCreateContextListHeap(pDriverData))
        return (FALSE);

    /*
     * done
     */
    return (TRUE);
}

/*
 * --------------------------------------------------------------------------
 * Termination routines.
 * --------------------------------------------------------------------------
 */

/*
 * This routine destroys the various dynamic item arrays used by the
 * Direct3D driver for managing contexts and textures. This routine should
 * be called at DLL_PROCESS_DETACH time from DllMain.
 */
BOOL DestroyItemArrays
(
    void
)
{
    // Cleanup the D3D context heap.
    nvDestroyContextListHeap();

    // Cleanup memory.
    return (DDrvItemArrayFini());
}

#endif  // NVARCH >= 0x04
