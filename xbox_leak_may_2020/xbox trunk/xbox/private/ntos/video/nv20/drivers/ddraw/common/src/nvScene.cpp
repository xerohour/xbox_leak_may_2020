/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvScene.cpp                                                       *
*   The Direct3D HAL scene capture routines is implemented in this module.  *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       03/24/97 - wrote and cleaned up.        *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

#ifdef NV_MARKER
DWORD dwMarkerCount = 0;
#endif

/*
 * Scene Capture start and end points
 */
DWORD __stdcall nvSceneCapture
(
    LPD3DHAL_SCENECAPTUREDATA pscd
)
{
    NvNotification *pPusherSyncNotifier;

    dbgTracePush ("nvSceneCapture");

    // get the context
    PNVD3DCONTEXT pContext;
    NV_SET_CONTEXT (pContext, pscd);

    // Get pointer to global driver. data structure.
    nvSetDriverDataPtrFromContext(pContext);

    // Need to make sure that an unfriendly mode switch didn't sneak and not cause
    // us to get re-enabled properly.
    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_D3D) {
        if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE) {
            nvD3DRecover();
        }
        else {
            NvReleaseSemaphore(pDriverData);
            pscd->ddrval = DD_OK;
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }
    }

    pPusherSyncNotifier = pDriverData->pDmaPusherSyncNotifierFlat;

    switch (pscd->dwFlag) {

        // BeginScene -----------------------------------------------------------------

        case D3DHAL_SCENE_CAPTURE_START:

#ifdef NV_PROFILE
            // Scene Timer Start & log
            //nvpLogEvent (NVP_T_CLOCK);
            nvpLogEvent (NVP_E_BEGINSCENE);
            NVP_START(NVP_T_SCENE);
#endif
#ifdef NV_MARKER
            if (1) {
                extern DWORD dwMarkerCount;
                dwMarkerCount ++;
                *(DWORD*)(pDriverData->BaseAddress) = (dwMarkerCount << 16) | dwMarkerCount;
            }
            else {
                *(DWORD*)(pDriverData->BaseAddress) = 0xaaaaaaaa;
            }
#endif
#ifdef CAPTURE
            if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
                CAPTURE_SCENE scene;
                scene.dwJmpCommand         = CAPTURE_JMP_COMMAND;
                scene.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
                scene.dwExtensionID        = CAPTURE_XID_SCENE;
                captureLog (&scene,sizeof(scene));
            }
#endif

            pContext->dwClearCount = 0; // number of full clears this frame, used to check for bad CT semantics

            // clear the area behind the logo if enabled in the registry
            if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_LOGOENABLE_MASK) == D3D_REG_LOGOENABLE_ENABLE)
            {
                nvDrawLogo(pContext, TRUE);
            }

            if (pContext->dwRTHandle)
            {
                CNvObject *pRTObj = nvGetObjectFromHandle(pContext->dwDDLclID, pContext->dwRTHandle, &global.pNvSurfaceLists);
                if (pRTObj && pRTObj->isDirty())
                {
                    // reset the render target
                    nvSetRenderTargetDX7(pContext, pContext->dwRTHandle, pContext->dwZBHandle);
#if (NVARCH >= 0x010)
                    // make sure the AA info is also updated
                    if (pContext->aa.isEnabled()) nvCelsiusAADestroy(pContext);
#endif  // NVARCH >= 0x010
                }
            }

#if (NVARCH >= 0x010)
            // Let AASTATE know that we have a BeginScene
            pContext->aa.BeginScene(pContext);
#endif  // NVARCH >= 0x010
            pContext->kelvinAA.TagSeen3D();

            if (dbgFrontRender) pDriverData->bDirtyRenderTarget = TRUE;

            break;

        // EndScene -------------------------------------------------------------------

        case D3DHAL_SCENE_CAPTURE_END:

            DDFLIPTICKS(FEBEGINEND);

#ifdef CAPTURE
            if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_PLAY) {
                getDC()->nvPusher.flush (TRUE, CPushBuffer::FLUSH_WITH_DELAY);
                capturePlay ((void *)(pContext->pRenderTarget->getAddress()),
                             pContext->pRenderTarget->getPitch(),
                             pContext->pRenderTarget->getWidth(),
                             pContext->pRenderTarget->getHeight());
                capturePlayFileInc();
            }
#endif

            // draw the NVIDIA logo if it is enabled in the registry
            if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_LOGOENABLE_MASK) == D3D_REG_LOGOENABLE_ENABLE)
            {
                nvDrawLogo(pContext, 0);
            }


#ifdef  STEREO_SUPPORT
            StereoAdjustmentMonitor(pContext);
#endif  //STEREO_SUPPORT

#if (NVARCH >= 0x010)
            // Let AASTATE know that we have a BeginScene
            pContext->aa.EndScene(pContext);
#endif  // NVARCH >= 0x010

#if (NVARCH >= 0x020)
            pContext->kelvinAA.EndScene();
#endif  // NVARCH >= 0x020

            // Start pushing buffer.
            nvPusherStart (TRUE);

#ifdef NV_MARKER
            *(DWORD*)(pDriverData->BaseAddress) = 0xbbbbbbbb;
#endif
#ifdef NV_PROFILE
            // Scene Timer Stop and log
            NVP_STOP(NVP_T_SCENE);
            nvpLogTime (NVP_T_SCENE,nvpTime[NVP_T_SCENE]);
#endif

            /*
             * This is a major hack to work around input lag in stupid applications that
             * want to use blits instead of flips to do their screen updates but then don't
             * make any getblitstatus calls to see if the blit has completed before beginning
             * to render the next frame.
             * This is not something that you want to have enabled unless you absolutely need
             * to have it enabled.
             */
            if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_SSYNCENABLE_MASK) == D3D_REG_SSYNCENABLE_ENABLE)
            {
                getDC()->nvPusher.flush (TRUE, CPushBuffer::FLUSH_WITH_DELAY);
            }

#ifdef TEX_MANAGE
            // shuffle texture eviction lists around...
            nvTexManageSceneEnd (pContext);
#endif
            /*
             * signal pm
             */
            PM_SIGNAL (PM_REG_PMTRIGGER_ENDSCENE);

#if 0 // DO NOT REMOVE - bdw
            {
                DWORD dwBase = (U032)NvDeviceBaseGet (NV_DEV_BASE, pDriverData->dwDeviceIDNum);

                static int j = -1;
                if (j == -1)
                {
                    DWORD dwData = *(volatile DWORD*)(dwBase + 0x400084);
                          dwData |= 0x02000000;
                    *(volatile DWORD*)(dwBase + 0x400084) = dwData;
                    j = 100;
                }
                else
                {
                    j--;
                    if (!j)
                    {
                        j = 100;

                        getDC()->nvPusher.flush (1, CPushBuffer::FLUSH_WITH_DELAY);

                        PF ("------------------------------- %d",GetTickCount());
                        *(volatile DWORD*)(dwBase + 0x400728)  = 0x01000000;

                        float afData[256];
                        for (DWORD i = 0x20; i <= 0x31; i++)
                        {
                            *(volatile DWORD*)(dwBase + 0x400728) = i << 24;
                            afData[i] = float(*(volatile DWORD*)(dwBase + 0x40072c));
                            PF ("%02x = %11d",i,int(afData[i]));
                        }

                    #define FPF(str,y) { float x = (y); PF (str,int(x),int((x) * 1000000) % 1000000); }

                        PF ("");
                        FPF ("  pipe util: %8d.%06d%%",100.0f * (afData[0x22] / 4.0f) / afData[0x20]);
                        FPF ("       idle: %8d.%06d%%",100.0f * afData[0x21] / afData[0x20]);
                        FPF ("  l2t stall: %8d.%06d%%",100.0f * afData[0x28] / afData[0x20]);
                    }
                }
            }
#endif

            break;

        default:
            DPF ("unknown scenecapture token");
            dbgD3DError();
            break;

    }  // switch

    NvReleaseSemaphore(pDriverData);
    pscd->ddrval = DD_OK;

    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
}

#endif  // NVARCH >= 0x04

