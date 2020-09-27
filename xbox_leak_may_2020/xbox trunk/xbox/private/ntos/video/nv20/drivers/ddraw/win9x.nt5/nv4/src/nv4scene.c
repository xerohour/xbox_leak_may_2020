#ifdef  NV4
/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4SCENE.C                                                        *
*   The direct 3d HAL scene capture routines is implemented in this module. *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       03/24/97 - wrote and cleaned up.        *
*                                                                           *
\***************************************************************************/
#include <windows.h>
#include "nvd3ddrv.h"
#include "ddrvmem.h"
#include "nv32.h"
#include "d3dinc.h"
#include "nv4dmac.h"
#include "nv4dreg.h"
#include "nv3ddbg.h"
#include "nv4vxmac.h"

#ifdef NV_MARKER
DWORD dwMarkerCount = 0;
#endif

#ifdef NV_STATS
DWORD dwSceneCount;
DWORD dwSceneTime;
DWORD dwDP2CallCount;
DWORD dwPrimCount;
DWORD dwPrimLegacyCount;
DWORD dwPrimListCount;
DWORD dwPrimStripCount;
DWORD dwPrimFanCount;
DWORD dwHWPCount;
#endif

/*
 * Scene Capture start and end points
 */
DWORD __stdcall nvSceneCapture
(
    LPD3DHAL_SCENECAPTUREDATA   pscd
)
{
    NvNotification *pPusherSyncNotifier;

    /*
     * Get the pointer to the context.
     */
    pCurrentContext = (PNVD3DCONTEXT)pscd->dwhContext;
    if (!pCurrentContext)
    {
        DPF_LEVEL(NVDBG_LEVEL_ERROR, "nvSceneCapture: - Bad Context");
        pscd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }

    /*
     * Get pointer to global driver. data structure.
     */
    NvSetDriverDataPtrFromContext(pCurrentContext);

    /*
     * Need to make sure that an unfriendly mode switch didn't sneak and not cause
     * us to get re-enabled properly.
     */
    if (pDriverData->fFullScreenDosOccurred)
    {
        if (pDriverData->fFullScreenDosOccurred & 0x01)
            nvD3DReenable();
        else
        {
            pscd->ddrval = DD_OK;
            return (DDHAL_DRIVER_HANDLED);
        }
    }

    /*
     * Setup frequently accessed globals.
     */
    NV_D3D_GLOBAL_SETUP();

    pPusherSyncNotifier = (NvNotification *)pDriverData->NvDmaPusherSyncNotifierFlat;
    switch (pscd->dwFlag)
    {
//////////////////////////////////////////////////////////////////////////////
        case D3DHAL_SCENE_CAPTURE_START:
#ifdef NV_PROFILE
            /*
             * Scene Timer Start & log
             */
            //nvpLogEvent (NVP_T_CLOCK);
            nvpLogEvent (NVP_E_BEGINSCENE);
            NVP_START(NVP_T_SCENE);
#endif
#ifdef NV_MARKER
            if (1)
            {
                extern DWORD dwMarkerCount;
                dwMarkerCount ++;
                *(DWORD*)(pDriverData->BaseAddress) = (dwMarkerCount << 16) | dwMarkerCount;
            }
            else
                *(DWORD*)(pDriverData->BaseAddress) = 0xaaaaaaaa;
#endif
#ifdef NV_STATS
            dwSceneTime       = (DWORD)-(int)GetTickCount();
            dwDP2CallCount    = 0;
            dwPrimCount       = 0;
            dwPrimLegacyCount = 0;
            dwPrimListCount   = 0;
            dwPrimStripCount  = 0;
            dwPrimFanCount    = 0;
            dwHWPCount        = 0;
#endif

            DPF_LEVEL(NVDBG_LEVEL_DDI_ENTRY, "nvSceneCapture - Begin Scene");

            /*
             * bump # of frames render
             */
            pCurrentContext->dwSceneCount ++;
            pCurrentContext->dwMipMapsInThisScene = 0;

            /*
             * Make sure the context has the correct suface information in it.
             */
            if (dbgFrontRender)
            {
                pCurrentContext->dwSurfaceAddr            = pDriverData->CurrentVisibleSurfaceAddress;
                pCurrentContext->dwSurfaceOffset          = VIDMEM_OFFSET(pDriverData->CurrentVisibleSurfaceAddress);
                pCurrentContext->surfacePitch.wColorPitch = (WORD)dbgFrontRenderPitch;
                pDriverData->lpLast3DSurfaceRendered      = 0;
            }
            else
            {
                pCurrentContext->dwSurfaceAddr            = VIDMEM_ADDR(pCurrentContext->lpLcl->lpGbl->fpVidMem);
                pCurrentContext->dwSurfaceOffset          = VIDMEM_OFFSET(pCurrentContext->lpLcl->lpGbl->fpVidMem);
                pCurrentContext->surfacePitch.wColorPitch = (WORD)pCurrentContext->lpLcl->lpGbl->lPitch;
            }
            if (pCurrentContext->lpLclZ)
            {
                /*
                 * I've seen it happen where the location of the z-buffer will actually
                 * change without without the context being destroyed or the set render
                 * target call happening. So it's neccessary to make sure that we have the
                 * correct z-buffer address and offset here.
                 */
                pCurrentContext->ZBufferAddr             = VIDMEM_ADDR(pCurrentContext->lpLclZ->lpGbl->fpVidMem);
                pCurrentContext->ZBufferOffset           = VIDMEM_OFFSET(pCurrentContext->lpLclZ->lpGbl->fpVidMem);
                pCurrentContext->surfacePitch.wZetaPitch = (WORD)pCurrentContext->lpLclZ->lpGbl->lPitch;
            }

            /*
             * anti-aliasing semantics check
             */
            pCurrentContext->dwAntiAliasFlags |= AA_IN_SCENE;
            NV_AA_SEMANTICS_SETUP (pCurrentContext);

            /*
             * Get the current freecount and try kicking off any outstanding data in the buffer
             * if the hardware is idle.
             */
            nvglIdleKickoff(sizeSetHeaderDmaPush);
            break;

//////////////////////////////////////////////////////////////////////////////
        case D3DHAL_SCENE_CAPTURE_END:
//            if ((!(pCurrentContext->lpLcl->dwReserved1 & NV_D3D_HAS_FLIPPED))
//             && (!nvglDmaFifoIdle()))
//            {
//                NV_D3D_GLOBAL_SAVE();
//                pscd->ddrval = DD_OK;
//                return (DDHAL_DRIVER_HANDLED);
//            }

            DPF_LEVEL(NVDBG_LEVEL_DDI_ENTRY, "nvSceneCapture - End Scene");

            /*
             * Anti-Alias post processing
             */
            if (pCurrentContext->dwAntiAliasFlags & AA_ENABLED_MASK)
            {
                /*
                 * AA will use the push buffer. Get proper freecount here.
                 */
#ifdef  CACHE_FREECOUNT
                nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#else   // CACHE_FREECOUNT
                nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetRenderTargetContexts + sizeSetRenderTarget));
#endif  // CACHE_FREECOUNT

                /*
                 * aa post-processing
                 */
                nvAARenderPrimitive();
            }

            /*
             * aa - check for bad semantics
             */
            if ((pCurrentContext->dwAntiAliasFlags & AA_MIX_MASK) == AA_MIX_MASK)
            {
                DPF_LEVEL(NVDBG_LEVEL_AA_INFO,"    Invalid Semantics Tagged");
                pCurrentContext->dwAntiAliasFlags &= ~AA_ENABLED_MASK;
                pCurrentContext->dwAntiAliasFlags |=  AA_INVALID_SEMANTICS;
                NV_FORCE_TRI_SETUP(pCurrentContext);
                NV_D3D_GLOBAL_SAVE();
                nvFlushDmaBuffers();
                NV_D3D_GLOBAL_SETUP();
                pDriverData->lpLast3DSurfaceRendered = 0;
                nvSetD3DSurfaceState (pCurrentContext);
            }

            pCurrentContext->dwAntiAliasFlags &= ~AA_IN_SCENE;

#ifdef NV_TEX2
            /*
             * set block point for textures
             */
            nvTextureSetBlockPoint();
#endif // NV_TEX2

            /*
             * Start pushing buffer.
             */
            nvStartDmaBuffer (FALSE);

#ifdef NV_STATS
            dwSceneTime  += GetTickCount();
            dwSceneCount ++;
            {
                char sz[512];
                wsprintf (sz,"Scene %d ********************************************\n",dwSceneCount);
                OutputDebugString (sz);
                wsprintf (sz,"Scene Time                = %dms\n",dwSceneTime);
                OutputDebugString (sz);
                wsprintf (sz,"DP2 Call Count            = %d\n",dwDP2CallCount);
                OutputDebugString (sz);
                wsprintf (sz,"Primitive Count           = %d\n",dwPrimCount);
                OutputDebugString (sz);
                wsprintf (sz,"Primitive Legacy Count    = %d\n",dwPrimLegacyCount);
                OutputDebugString (sz);
                wsprintf (sz,"Primitive List Count      = %d\n",dwPrimListCount);
                OutputDebugString (sz);
                wsprintf (sz,"Primitive Strip Count     = %d\n",dwPrimStripCount);
                OutputDebugString (sz);
                wsprintf (sz,"Primitive Fan Count       = %d\n",dwPrimFanCount);
                OutputDebugString (sz);
                wsprintf (sz,"HW Program Count          = %d\n",dwHWPCount);
                OutputDebugString (sz);
                if (dwSceneTime)
                {
                    wsprintf (sz,"Primitives/second  = %d\n",(dwPrimCount*1000)/dwSceneTime);
                    OutputDebugString (sz);
                }
            }
#endif
#ifdef NV_MARKER
            *(DWORD*)(pDriverData->BaseAddress) = 0xbbbbbbbb;
#endif
#ifdef NV_PROFILE
            /*
             * Scene Timer Stop and log
             */
            NVP_STOP(NVP_T_SCENE);
            nvpLogTime (NVP_T_SCENE,nvpTime[NVP_T_SCENE]);
#endif
            break;
        default:
            break;
    }
    /*
     * Return driver not handled since we want D3D to call our RenderPrimitive
     * routine.
     */
    NV_D3D_GLOBAL_SAVE();
    pscd->ddrval = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
}
#endif  // NV4
