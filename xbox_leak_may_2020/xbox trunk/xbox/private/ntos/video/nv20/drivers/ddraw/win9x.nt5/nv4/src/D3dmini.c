/*
 * Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: D3DMINI.C                                                         *
*   The direct 3d HAL driver routines are implemented in this module.       *
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
#include <windows.h>
#include "nvd3ddrv.h"
#include "ddrvmem.h"
#include "nv32.h"
#include "d3dinc.h"
#include "nvd3dcap.h"
#include "nvtexfmt.h"
#include "nv3ddbg.h"

/*
 * Miscellaneous Driver Data.
 */
static D3DHAL_GLOBALDRIVERDATA  NvGlobal;

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
#ifndef WINNT
    nvRenderState,              /* Required if no Execute */
    nvRenderPrimitive,          /* Required if no Execute */
#else
    0,
    0,
#endif // #else
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
#ifndef WINNT
    nvGetState,                 /* Required if implementing Execute. */
#else
    0,
#endif // #else
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
 * Define the NV3 device capabilities structure.
 */
D3DDEVICEDESC_V1 NvCaps =
{
    sizeof(D3DDEVICEDESC_V1),
    NV_DEVDESC_FLAGS,
    NV_COLORMODEL,
    NV_DEVCAPS_FLAGS,
    NV_TRANSFORMCAPS,
    NV_3DCLIPPINGCAPS,
    NV_LIGHTINGCAPS,
    NV_LINECAPS,
    NV_TRICAPS,
    NV_RENDERBITDEPTHCAPS,
    NV_ZBUFFERBITDEPTHCAPS,
    NV_MAXBUFFERSIZE,
    NV_MAXVERTEXCOUNT
};

/*
 * Define supported texture formats.
 */
static DDSURFACEDESC    NvTextureFormats[] =
{
    NV_TEXTURE_FORMATS      /* Texture formats defined in NVTEXFMTS.H */
};

/*
 * Developer Identification String
 * Before sending a pre-release driver to a developer, this data can be modified in the binary to identify
 * the developer it is being sent to.  This will give us an identification mechanism if the driver mysteriously
 * finds it's way on the the net.
 */
BYTE bDeveloperID[32 + 32] =
{
    0x0F, 0xAD, 0xCA, 0xFE, 0xDE, 0xAF, 0xFE, 0xED,
    0xDE, 0xAF, 0xBA, 0xBE, 0xDE, 0xAD, 0xF0, 0x0D,
    0x0B, 0xAD, 0xCA, 0xFE, 0x0D, 0xEA, 0xDF, 0xED,
    0xDE, 0xAD, 0xBA, 0xBE, 0x0B, 0xAD, 0xF0, 0x0D,

    0x61, 0x34, 0x42, 0x34, 0xf6, 0xf4, 0x82, 0xe1,
    0xb7, 0xec, 0x5c, 0x23, 0x82, 0x00, 0x60, 0x84,
    0x67, 0x8c, 0x45, 0x2c, 0x47, 0x31, 0x85, 0x32,
    0x30, 0x05, 0x06, 0x20, 0x93, 0x10, 0x40, 0x92,
};

/*
 * --------------------------------------------------------------------------
 *  Direct 3D HAL driver creation routines.
 *  Functions used to instantiate the 3D portion of the DirectDraw HAL
 *  Will be called (for example) from invokeCreateDriver() from ddsamp.
 *
 *  D3DHALCreateDriver
 *
 *  LPD3DHAL_GLOBALDRIVERDATA* lplpGlobal
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
    LPD3DHAL_GLOBALDRIVERDATA  *lplpGlobal,
    LPD3DHAL_CALLBACKS         *lplpHALCallbacks
)
{
    if (!pDriverData->hContexts_gbl)
        if (!CreateItemArrays())
        {
            DPF_LEVEL (NVDBG_LEVEL_ERROR, "Failed to initialize arrays");
            return FALSE;
        }

    /*
     * check if driver is valid
     */
    {
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

    /*
     * Get the current registry settings from the hardware specific routine.
     */
    D3DReadRegistry();

    /*
     * Here we fill in the supplied structures.
     */
    memset(&NvGlobal, 0, sizeof(D3DHAL_GLOBALDRIVERDATA));
    NvGlobal.dwSize              = sizeof(D3DHAL_GLOBALDRIVERDATA);
    NvGlobal.hwCaps              = NvCaps;
    NvGlobal.dwNumVertices       = 0;
    NvGlobal.dwNumClipVertices   = 0;
    NvGlobal.dwNumTextureFormats = sizeof(NvTextureFormats)
                                 / sizeof(DDSURFACEDESC);
    NvGlobal.lpTextureFormats    = &NvTextureFormats[0];

    /*
     * Modify the capabilities as neccessary for the registry settings.
     */
    D3DModifyCapabilities(&NvGlobal);

    /*
     * Return the HAL table.
     */
    *lplpGlobal       = &NvGlobal;
    *lplpHALCallbacks = &NvD3DHALCallbacks;
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
    void
)
{
#ifndef OLDSTUFF
#else
    /*
     * Initialize memory system.
     */
    if (!DDrvItemArrayInit())
    {
        DPF_LEVEL (NVDBG_LEVEL_ERROR, "CreateItemArrays - Failed to initialize memory subsystem");
        return (FALSE);
    }
#endif

    /*
     * Create the D3D context heap.
     */
    if (!nvCreateContextListHeap())
        return (FALSE);

    /*
     * Create the D3D texture heap.
     */
    if (!nvCreateTextureListHeap())
    {
        /*
         * Could not allocate the Texture heap! Cleanup.
         */
        nvDestroyContextListHeap();
#ifndef OLDSTUFF
#else
        DDrvItemArrayFini();
#endif
        return (FALSE);
    }
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
    /*
     * Cleanup texture array.
     */
    nvDestroyTextureListHeap();

    /*
     * Cleanup the D3D context heap.
     */
    nvDestroyContextListHeap();

    /*
     * Set all the heap handles back to null.
     */
    pDriverData->hContexts_gbl  = 0;
    pDriverData->hTextures_gbl  = 0;
    pDriverData->hAGPList_gbl   = 0;

#ifndef OLDSTUFF
    return (TRUE);
#else
    /*
     * Cleanup memory.
     */
    return (DDrvItemArrayFini());
#endif
}
