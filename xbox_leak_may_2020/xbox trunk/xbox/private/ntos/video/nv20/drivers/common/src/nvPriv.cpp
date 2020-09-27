 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/

#if (NVARCH >= 0x04)

#include "nvprecomp.h"
#include "wincommon.h"
#include "nvPriv.h"
#include "nvProcMan.h"
#pragma hdrstop


//extern BOOL pmAddProcess(GLOBALDATA *pDriverData, DWORD processID);
//extern BOOL pmDeleteProcess(DWORD processID);

// this is really global, do not move to pDriverData
// each element contains a pDriverData associated with an adapter
extern "C" 
{
	NV_ADAPTER_TYPE g_adapterData[MAX_ADAPTERS];
}

#if (IS_WIN9X || IS_WINNT5)
// -----------------------------------------------------------------------------
DWORD nvGetTextureFromSystemMemAddress(GLOBALDATA *pDriverData, FLATPTR fpVidMem)
{
    CNvObject *pNvObject = global.pNvObjectHead;
    while (pNvObject)
    {
        CTexture *pTexture = pNvObject->getTexture();
        if (pTexture)
        {
            if (pTexture->getLinear()->isValid())
            {
                if (pTexture->getLinear()->getAddress() == fpVidMem)
                {
                    CSimpleSurface *pSurf = pNvObject->getTexture()->getSwizzled();
                    return pSurf->getOffset();
                }
            }
        }
        pNvObject = pNvObject->getNext();
    }
    return -1;
}
#endif

static GLOBALDATA *getGlobalDataPtr(NvU32 dwDevice)
{
#if (IS_WINNT5)
    return g_adapterData[dwDevice].pDriverData;
#elif (IS_WINNT4)
    // device is always zero under winnt4
    return g_adapterData[0].pDriverData;
#else
    int i;
    if (dwDevice > MAX_ADAPTERS)
    {
        return NULL;
    }
    if (dwDevice != 0)
    {
        dwDevice -= 1;
    }
    for (i=0; i < MAX_ADAPTERS; i++)
    {
        if (g_adapterData[i].pDriverData &&
           (g_adapterData[i].pDriverData->dwDeviceIDNum == (dwDevice + 1)))
        {
            return g_adapterData[i].pDriverData;
        }
    }
    return NULL;
#endif
}


#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// nvOverlayControl
//      Controls various overlay features
//      Commands:
//          GETCAPS - Returns overlay capabilities
//          SETCOLORCONTROL - Sets the colour controls
//          GETCOLORCONTROL - Returns colour control state
//          SETFEATURE - Sets overlay features
//          GETFEATURE - Gets overlay features

NVOCERROR __stdcall nvOverlayControl(LPNVOCDATATYPE lpOCData)
{
GLOBALDATA* pDriverData = NULL;          // override the global pDriverData
NV_CFGEX_GET_DISPLAY_TYPE_MULTI_PARAMS displayParams[NV_MAX_HEADS];
int i;

    if (lpOCData->dwSize < sizeof(NVOCDATATYPE) || lpOCData->dwSize > 4096) {
        return NVOCERR_INVALID_PARMS;
    }

    pDriverData = getGlobalDataPtr(lpOCData->dwDevice);
    if (pDriverData == NULL) {
        return NVOCERR_BAD_DEVICE;
    }
    Vpp_t &vpp = pDriverData->vpp;

    // If the VPP constructor hasn't been called, don't return any info.
    if (!VppIsConstructed(&pDriverData->vpp))
    {
        return NVOCERR_BAD_DEVICE;
    }

    switch (lpOCData->cmd) {
    case NVOCCMD_NOOP:
        return NVOCERR_OK;

    case NVOCCMD_IDENTIFY:
        vpp.dwDecoderIdentity = lpOCData->caps.dwCaps1;
        vpp.dwDecoderCaps     = lpOCData->caps.dwCaps2;
        return NVOCERR_OK;

    case NVOCCMD_GETCAPS:
        lpOCData->caps.dwCaps1 = 0;
        lpOCData->caps.dwCaps2 = 0;
        lpOCData->caps.dwCaps3 = 0;
        lpOCData->caps.dwCaps4 = 0;
        lpOCData->caps.dwCaps5 = 0;
        if (vpp.regOverlayColourControlEnable ||
            pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
            lpOCData->caps.dwCaps1 = NVOCCAPS1_BRIGHTNESS |
                                     NVOCCAPS1_CONTRAST |
                                     NVOCCAPS1_HUE |
                                     NVOCCAPS1_SATURATION;
        }

        lpOCData->caps.dwCaps2 |= NVOCCAPS2_TFILTER |
                                  NVOCCAPS2_OVLZOOM;
        if (pDriverData->NvDeviceVersion < NV_DEVICE_VERSION_10) {
            lpOCData->caps.dwCaps2 |= NVOCCAPS2_HQVUPSCALE;
        } else {
            lpOCData->caps.dwCaps2 |= NVOCCAPS2_DFILTER;
        }
        if (pDriverData->dwHeads > 1) {
            lpOCData->caps.dwCaps2 |= NVOCCAPS2_FSMIRROR;
        }

        lpOCData->caps.dwCaps3 |= NVOCCAPS3_SUBPICTURE;
        lpOCData->caps.dwCaps3 |= NVOCCAPS3_LATEFLIPSYNC;
        if (vpp.pSysInfo->dwNVClasses & NVCLASS_0004_TIMER) {
            lpOCData->caps.dwCaps3 |= NVOCCAPS3_DELIVERYCONTROL;
        }
        lpOCData->caps.dwCaps3 |= NVOCCAPS3_BLTCAPS;

        // TODO: figure out whether this is an SMA configuration
        lpOCData->caps.dwCaps4 |= NVOCCAPS4_IMB;
        if (vpp.dwOverlaySurfaces) {
            // set if this device has an open overlay
            lpOCData->caps.dwCaps4 |= NVOCCAPS4_DEVICEHASOVL;
        }
        lpOCData->caps.dwCaps4 |= (pDriverData->dwHeads & 0xF) << 16;
        lpOCData->caps.dwCaps4 |= (pDriverData->dwDesktopState & 0xF) << 20;

        for (i = 0; i < (int)pDriverData->dwHeads; i++) {
            displayParams[i].Head = i;
            NvRmConfigGetEx(pDriverData->dwRootHandle, vpp.pDriverData->dwDeviceHandle,
                            NV_CFGEX_GET_DISPLAY_TYPE_MULTI,
                            &displayParams[i], sizeof(displayParams));
        }
        lpOCData->caps.dwCaps4 |= (displayParams[0].Type & 0xF) << 24;
        if (pDriverData->dwHeads > 1) {
            lpOCData->caps.dwCaps4 |= (displayParams[1].Type & 0xF) << 28;
        }

        lpOCData->caps.dwCaps5 |= NVOCCAPS5_BLT_YUV9_2_YUV422
                               |  NVOCCAPS5_BLT_YUV12_2_YUV422
                               |  NVOCCAPS5_BLT_YC12_2_YUV422
                               |  NVOCCAPS5_BLT_YUV422_2_YUV422
                               |  NVOCCAPS5_BLT_YUV422_2_RGB16
                               |  NVOCCAPS5_BLT_YUV422_2_RGB32
                               |  NVOCCAPS5_BLT_RGB16_2_RGB32
                               |  NVOCCAPS5_BLT_RGB32_2_RGB32
                               |  NVOCCAPS5_BLT_SYUV2VYUV
                               |  NVOCCAPS5_BLT_VYUV2SYUV
                               |  NVOCCAPS5_BLT_SYUV2VRGB
                               |  NVOCCAPS5_BLT_VYUV2VRGB
                               |  NVOCCAPS5_BLT_SRGB2VRGB
                               |  NVOCCAPS5_BLT_VRGB2VRGB
                               |  NVOCCAPS5_BLT_2RGBTEX;
        return NVOCERR_OK;

    case NVOCCMD_SETCOLORCONTROL:
        if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
            // NV10 ranges
            if (lpOCData->ccData.dwCCFlags & NVOCCC_CONTRAST) {
                vpp.colorCtrl.lContrast = min(max(lpOCData->ccData.lContrast * 20000 / 200, 0), 20000);
            }
            if (lpOCData->ccData.dwCCFlags & NVOCCC_BRIGHTNESS) {
                vpp.colorCtrl.lBrightness = min(max(lpOCData->ccData.lBrightness * 10000 / 255, -10000), 10000);
            }
            if (lpOCData->ccData.dwCCFlags & NVOCCC_HUE) {
                vpp.colorCtrl.lHue = lpOCData->ccData.lHue;
                while (vpp.colorCtrl.lHue < -180) vpp.colorCtrl.lHue += 360;
                while (vpp.colorCtrl.lHue >= 180) vpp.colorCtrl.lHue -= 360;
            }
            if (lpOCData->ccData.dwCCFlags & NVOCCC_SATURATION) {
                vpp.colorCtrl.lSaturation = min(max(lpOCData->ccData.lSaturation * 20000 / 200, 0), 20000);
            }
            VppSetOverlayColourControl(&vpp);
        } else {
            // NV4/5 ranges
            if (lpOCData->ccData.dwCCFlags & NVOCCC_CONTRAST) {
                vpp.colorCtrl.lContrast = min(max(lpOCData->ccData.lContrast * 511 / 200, 0), 511);
            }
            if (lpOCData->ccData.dwCCFlags & NVOCCC_BRIGHTNESS) {
                vpp.colorCtrl.lBrightness = min(max(lpOCData->ccData.lBrightness, -256), 255);
            }
            if (lpOCData->ccData.dwCCFlags & NVOCCC_HUE) {
                vpp.colorCtrl.lHue = lpOCData->ccData.lHue;
                while (vpp.colorCtrl.lHue < 0)    vpp.colorCtrl.lHue += 360;
                while (vpp.colorCtrl.lHue >= 360) vpp.colorCtrl.lHue -= 360;
            }
            if (lpOCData->ccData.dwCCFlags & NVOCCC_SATURATION) {
                vpp.colorCtrl.lSaturation = min(max(lpOCData->ccData.lSaturation * 512 / 200, 0), 512);
            }
            vpp.regOverlayColourControlEnable    = lpOCData->ccData.dwEnable;
        }
        return NVOCERR_OK;

    case NVOCCMD_GETCOLORCONTROL:
        lpOCData->ccData.dwCCFlags = NVOCCC_CONTRAST |
                                     NVOCCC_BRIGHTNESS |
                                     NVOCCC_HUE |
                                     NVOCCC_SATURATION;
        if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
            // NV10 ranges
            lpOCData->ccData.lContrast = vpp.colorCtrl.lContrast / 100;
            lpOCData->ccData.lBrightness = vpp.colorCtrl.lBrightness * 255 / 10000;
            lpOCData->ccData.lHue = vpp.colorCtrl.lHue;
            lpOCData->ccData.lSaturation = vpp.colorCtrl.lSaturation / 100;
            lpOCData->ccData.dwEnable = 3;
        } else {
            // NV4/5 ranges
            lpOCData->ccData.lContrast = vpp.colorCtrl.lContrast * 200 / 511;
            lpOCData->ccData.lBrightness = vpp.colorCtrl.lBrightness;
            lpOCData->ccData.lHue = vpp.colorCtrl.lHue;
            lpOCData->ccData.lSaturation = vpp.colorCtrl.lSaturation * 200 / 512;
            lpOCData->ccData.dwEnable = vpp.regOverlayColourControlEnable;
        }
        return NVOCERR_OK;

    case NVOCCMD_SETFEATURE:
        // turn on high quality vertical upscaling
        if (lpOCData->featureData.dwFeature1 & NVOCF1_HQVUPSCALE) {
            if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
                return NVOCERR_NOT_SUPPORTED;
            }
            /*
            if (pDriverData->dwOverlaySurfaces > 0 &&
               (pDriverData->regOverlayMode & NV4_REG_OVL_MODE_VUPSCALE)==0) {
                // if an overlay app is already running, and we are going from off to on,
                // no can do, must quit app first
                return NVOCERR_ALREADY_ACTIVE;
            }
            */
            vpp.regOverlayMode |= NV4_REG_OVL_MODE_VUPSCALE;
        }
        // turn on temporal filtering
        if (lpOCData->featureData.dwFeature1 & NVOCF1_TFILTER) {
            if (lpOCData->featureData.lCoefficient[0] < 0 ||
                lpOCData->featureData.lCoefficient[0] > 255) {
                return NVOCERR_INVALID_PARMS;
            }
            vpp.regOverlayMode |= NV4_REG_OVL_MODE_TFILTER;
            vpp.regOverlayMode &= ~0xFF00;
            vpp.regOverlayMode |= lpOCData->featureData.lCoefficient[0] << 8;
            vpp.regOverlayMode &= ~NV4_REG_OVL_MODE_NOTFIRST;  // first frame
            if (lpOCData->dwCmdFlags & NVOCF_TF_PRECOPY) {
                vpp.regOverlayMode |= NV4_REG_OVL_MODE_TF_PRECOPY;
            } else {
                vpp.regOverlayMode &= ~NV4_REG_OVL_MODE_TF_PRECOPY;
            }
        }
        // turn on deinterlace filter
        if (lpOCData->featureData.dwFeature1 & NVOCF1_DFILTER) {
            if (lpOCData->featureData.lCoefficient[1] < 0 ||
                lpOCData->featureData.lCoefficient[1] > 255) {
                return NVOCERR_INVALID_PARMS;
            }
            vpp.regOverlayMode |= NV4_REG_OVL_MODE_DFILTER;
            vpp.regOverlayMode &= ~0xFF0000;
            vpp.regOverlayMode |= lpOCData->featureData.lCoefficient[1] << 16;
            if (lpOCData->dwCmdFlags & NVOCF_DF_PRECOPY) {
                vpp.regOverlayMode |= NV4_REG_OVL_MODE_DF_PRECOPY;
            } else {
                vpp.regOverlayMode &= ~NV4_REG_OVL_MODE_DF_PRECOPY;
            }
        }
        // turn on fullscreen mirror
        if (lpOCData->featureData.dwFeature1 & NVOCF1_FSMIRROR) {
            DWORD vppInvMaskSave;
            BOOL bNewDevice = (vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSDEVICEMASK) != (DWORD)(lpOCData->featureData.lCoefficient[4] & NV4_REG_OVL_MODE2_FSDEVICEMASK);
            if (vpp.dwOverlayFSNumSurfaces == 0 || bNewDevice ||
               (vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSASPECTMASK) != (DWORD)(lpOCData->featureData.lCoefficient[4] & NV4_REG_OVL_MODE2_FSASPECTMASK)) {
                // only reenable if already disabled, or aspect, or device change
                vpp.regOverlayMode2 = (vpp.regOverlayMode2 & ~NV4_REG_OVL_MODE2_FSMASK) |
                                               (lpOCData->featureData.lCoefficient[4] & NV4_REG_OVL_MODE2_FSMASK);
                if (vpp.dwOverlaySurfaces) {
                    vppInvMaskSave = vpp.regVPPInvMask;
                    vpp.regVPPInvMask = 0xFFFFFFFF;    // shut off VPP pipe
#if IS_WINNT4
                    nvSpin(vpp.pSysInfo->dwSpinLoopCount * 3);
#else
                    Sleep(VPP_TIMEOUT_TIME * 3);                // make sure all flips are flushed
#endif
                    VppDestroyFSMirror(&vpp);
                    if (bNewDevice) {
                        // device changed, we need to reinstantiate the overlay too
                        VppDestroyOverlay(&vpp);
                        VppCreateOverlay(&vpp);
#ifndef WINNT
                        pmDeleteAllProcesses(pDriverData);
                        pmAddProcess(pDriverData, GetCurrentProcessId());
#endif
                    }
                    VppCreateFSMirror(&vpp, vpp.dwOverlayFSSrcWidth, vpp.dwOverlayFSSrcHeight);
                    vpp.regVPPInvMask = vppInvMaskSave;    // turn on VPP pipe again
                }
                if (bNewDevice && pDriverData->dwDesktopState == NVTWINVIEW_STATE_CLONE &&
                   (vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSDEVICEMASK) == 0) {
                    // user has just disabled video mirror in clone mode, we must now tag all overlays as lost
                    vpp.dwOverlayFSOvlLost = TRUE;
                }
            } else {
                vpp.regOverlayMode2 = (vpp.regOverlayMode2 & ~NV4_REG_OVL_MODE2_FSMASK) |
                                               (lpOCData->featureData.lCoefficient[4] & NV4_REG_OVL_MODE2_FSMASK);
            }
        }
        // turn on overlay zoom
        if (lpOCData->featureData.dwFeature1 & NVOCF1_OVLZOOM) {
            vpp.regOverlayMode3 = (vpp.regOverlayMode3 & ~NV4_REG_OVL_MODE3_OVLZOOMMASK) |
                                           (lpOCData->featureData.lCoefficient[5] & NV4_REG_OVL_MODE3_OVLZOOMMASK);
        }
        // turn on frame delivery control
        if (lpOCData->featureData.dwFeature1 & NVOCF1_DELIVERYCONTROL) {
            NvNotification *pPioFlipOverlayNotifier = vpp.m_obj_a[OVERLAY_NDX].notifier_a;
            volatile NvNotification *pTimerNotifier = vpp.m_obj_a[TIMER_NDX].notifier_a;
            
            nvAssert(pTimerNotifier != NULL);

            vpp.llDeliveryPeriod = (__int64)lpOCData->featureData.lCoefficient[6] | 
                                              ((__int64)lpOCData->featureData.lCoefficient[7] << 32);

            // stop current timer
            vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push(1, NV_VPP_TIMER_IID);
            vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | NV004_STOP_TRANSFER | 0x40000);
            vpp.pPusher->push(3, NV004_STOP_TRANSFER_VALUE);
            vpp.pPusher->adjust(4);
            vpp.pPusher->start(TRUE);

            // get current time
            while (pTimerNotifier->status == NV_IN_PROGRESS);
            pTimerNotifier->status = NV_IN_PROGRESS;
            vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push(1, NV_VPP_TIMER_IID);
            vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | NV004_SET_ALARM_TIME_RELATIVE | 0x80000);
            vpp.pPusher->push(3, 1); // wait a token nanosecond
            vpp.pPusher->push(4, 0);
            vpp.pPusher->push(5, SUB_CHANNEL(vpp.spareSubCh) | NV004_SET_ALARM_NOTIFY | 0x40000);
            vpp.pPusher->push(6, NV004_SET_ALARM_NOTIFY_WRITE_ONLY);
            vpp.pPusher->adjust(7);
            vpp.pPusher->start(TRUE);
            pDriverData->dDrawSpareSubchannelObject = NV_VPP_TIMER_IID;
            while (pTimerNotifier->status == NV_IN_PROGRESS);
            vpp.llDeliveryStart = (__int64)pTimerNotifier->timeStamp.nanoseconds[0] |
                                             ((__int64)pTimerNotifier->timeStamp.nanoseconds[1] << 32);

            // next frame will be delivered at this time
            vpp.llDeliveryTime = vpp.llDeliveryStart + vpp.llDeliveryPeriod;

            // return pointers to overlay flip timecodes
            lpOCData->featureData.lCoefficient[6] = (long) &(pPioFlipOverlayNotifier[1].timeStamp.nanoseconds[0]);
            lpOCData->featureData.lCoefficient[7] = (long) &(pPioFlipOverlayNotifier[2].timeStamp.nanoseconds[0]);
        }
        // turn on late flip sync
        if (lpOCData->featureData.dwFeature1 & NVOCF1_LATEFLIPSYNC) {
            vpp.regOverlayMode |= NV4_REG_OVL_MODE_LATEFLIPSYNC;
        }
        // set maximum number of VPP work surfaces
        if (lpOCData->featureData.dwFeature2 & NVOCF2_VPPMAXSURFACES) {
            vpp.regVPPMaxSurfaces = lpOCData->featureData.lCoefficient[2];
        }
        return NVOCERR_OK;

    case NVOCCMD_RESETFEATURE:
        if (lpOCData->featureData.dwFeature1 & NVOCF1_HQVUPSCALE) {
            vpp.regOverlayMode &= ~NV4_REG_OVL_MODE_VUPSCALE;
        }
        if (lpOCData->featureData.dwFeature1 & NVOCF1_TFILTER) {
            vpp.regOverlayMode &= ~NV4_REG_OVL_MODE_TFILTER;
            vpp.regOverlayMode |= 0xFF00;
        }
        if (lpOCData->featureData.dwFeature1 & NVOCF1_DFILTER) {
            vpp.regOverlayMode &= ~NV4_REG_OVL_MODE_DFILTER;
            vpp.regOverlayMode |= 0xFF0000;
        }
        if (lpOCData->featureData.dwFeature1 & NVOCF1_FSMIRROR) {
            DWORD vppInvMaskSave = vpp.regVPPInvMask;
            vpp.regVPPInvMask = 0xFFFFFFFF;    // shut off VPP pipe
#if IS_WINNT4
            nvSpin(vpp.pSysInfo->dwSpinLoopCount * 3);
#else
            Sleep(VPP_TIMEOUT_TIME * 3);                // make sure all flips are flushed
#endif
            vpp.regOverlayMode2 &= ~NV4_REG_OVL_MODE2_FSMASK;
            VppDestroyFSMirror(&vpp);
            vpp.regVPPInvMask = vppInvMaskSave;    // turn on VPP pipe again
            if (pDriverData->dwDesktopState == NVTWINVIEW_STATE_CLONE) {
                // user has just disabled video mirror in clone mode, we must now tag all overlays as lost
                vpp.dwOverlayFSOvlLost = TRUE;
            }
        }
        if (lpOCData->featureData.dwFeature1 & NVOCF1_OVLZOOM) {
            vpp.regOverlayMode3 &= ~NV4_REG_OVL_MODE3_OVLZOOMMASK;
        }
        if (lpOCData->featureData.dwFeature1 & NVOCF1_DELIVERYCONTROL) {
            vpp.llDeliveryPeriod = 0;
        }
        if (lpOCData->featureData.dwFeature1 & NVOCF1_LATEFLIPSYNC) {
            vpp.regOverlayMode &= NV4_REG_OVL_MODE_LATEFLIPSYNC;
        }
        return NVOCERR_OK;

    case NVOCCMD_GETFEATURE:
        lpOCData->featureData.dwFeature1 = 0;
        lpOCData->featureData.dwFeature2 = 0;
        lpOCData->featureData.dwFeature3 = 0;
        lpOCData->featureData.dwFeature4 = 0;
        if (vpp.regOverlayMode & NV4_REG_OVL_MODE_VUPSCALE) {
            lpOCData->featureData.dwFeature1 |= NVOCF1_HQVUPSCALE;
        }
        if (vpp.regOverlayMode & NV4_REG_OVL_MODE_TFILTER) {
            lpOCData->featureData.dwFeature1 |= NVOCF1_TFILTER;
            lpOCData->featureData.lCoefficient[0] = (vpp.regOverlayMode >> 8) & 0xFF;
        }
        if (vpp.regOverlayMode & NV4_REG_OVL_MODE_DFILTER) {
            lpOCData->featureData.dwFeature1 |= NVOCF1_DFILTER;
            lpOCData->featureData.lCoefficient[1] = (vpp.regOverlayMode >> 16) & 0xFF;
        }
        if (vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSDEVICEMASK) {
            lpOCData->featureData.dwFeature1 |= NVOCF1_FSMIRROR;
            lpOCData->featureData.lCoefficient[4] = vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSMASK;
        }
        if (vpp.regOverlayMode2 & NV4_REG_OVL_MODE3_OVLZOOMQUADMASK) {
            lpOCData->featureData.dwFeature1 |= NVOCF1_OVLZOOM;
            lpOCData->featureData.lCoefficient[5] = vpp.regOverlayMode3 & NV4_REG_OVL_MODE3_OVLZOOMMASK;
        }
        if (vpp.llDeliveryPeriod != 0) {
            NvNotification *pPioFlipOverlayNotifier = vpp.m_obj_a[OVERLAY_NDX].notifier_a;
            lpOCData->featureData.dwFeature1 |= NVOCF1_DELIVERYCONTROL;
            lpOCData->featureData.lCoefficient[6] = (DWORD)vpp.llDeliveryPeriod;
            lpOCData->featureData.lCoefficient[7] = (DWORD)(vpp.llDeliveryPeriod >> 32);
            lpOCData->featureData.lCoefficient[8]  = pPioFlipOverlayNotifier[1].timeStamp.nanoseconds[0];
            lpOCData->featureData.lCoefficient[9]  = pPioFlipOverlayNotifier[1].timeStamp.nanoseconds[1];
            lpOCData->featureData.lCoefficient[10] = pPioFlipOverlayNotifier[2].timeStamp.nanoseconds[0];
            lpOCData->featureData.lCoefficient[11] = pPioFlipOverlayNotifier[2].timeStamp.nanoseconds[1];
        }
        if (vpp.regOverlayMode & NV4_REG_OVL_MODE_LATEFLIPSYNC) {
            lpOCData->featureData.dwFeature1 |= NVOCF1_LATEFLIPSYNC;
        }
        lpOCData->featureData.dwFeature2 = NVOCF2_VPPMAXSURFACES;
        lpOCData->featureData.lCoefficient[2] = vpp.regVPPMaxSurfaces;
        lpOCData->featureData.lCoefficient[3] = vpp.extraNumSurfaces;
        return NVOCERR_OK;

    default:
        return NVOCERR_BAD_COMMAND;
    }
}

#if (IS_WIN9X || IS_WINNT5)

// -----------------------------------------------------------------------------
// nvMoComp
//      Copies system YC12 data to video yuv422.  Returns error code
//      Event is shared with OCC.  Could affect QUERY_IDLE or WAIT_FOR_IDLE
//      Commands:
//          NOOP - Does nothing
//          INIT - Creates system memory DMA context and attaches it to necessary objects
//          RELEASE - Kills DMA context
//          BLIT - Does the system to video blit
//          QUERY_IDLE - Returns BUSY or OK
//          WAIT_FOR_IDLE - Waits for event to trigger

NVS2VERROR __stdcall nvMoComp(LPNVS2VDATATYPE lpS2VData)
{
NvNotification *dmaPusherSyncNotifier;
DWORD dwDstYOffset, dwDstCOffset, dwSrcYOffset, dwSrcCOffset, dwSrcUOffset, dwSrcVOffset, dwSurfSize;
DWORD dwFourCC, dwVPPFlags;
LPDDRAWI_DDRAWSURFACE_INT lpDstSurf;
FLATPTR fpDstSys;
static DWORD dwYPhase = 0;
static FLATPTR fpSrcSurf[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static FLATPTR fpDstSurf[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
LPPROCESSINFO lpProcInfo;
GLOBALDATA* pDriverData = NULL;          // override global pDriverData
DWORD dwSrcContextDma, dwDstContextDma;

    if (lpS2VData->dwSize < sizeof(NVS2VDATATYPE) || lpS2VData->dwSize > 4096)
        return NVS2VERR_INVALID_PARMS;

    pDriverData = getGlobalDataPtr(lpS2VData->dwDevice);
    if (pDriverData == NULL) {
        return NVS2VERR_BAD_DEVICE;
    }
    Vpp_t &vpp = pDriverData->vpp;

    lpProcInfo = pmGetProcess(pDriverData, GetCurrentProcessId());

    dmaPusherSyncNotifier = pDriverData->pDmaPusherSyncNotifierFlat;

    switch (lpS2VData->dwCmd) {
// ************************************************************************
    case NVS2VCMD_BLIT:
// ************************************************************************

		// This sometimes uses the VPP_DMABLT_TO_VID object... which is only ready for use if 
		// we've gotten vppEnable()
		nvAssert(vpp.dwFlags & VPP_FLAG_ENABLED);

		if (!(vpp.dwFlags & VPP_FLAG_ENABLED))
		{
			return NVS2VERR_NOT_READY;
		}

        if (lpS2VData->dwIndex >= 16)
            return NVS2VERR_INVALID_PARMS;

        if (vpp.regVPPInvMask & (NV4_REG_VPP_DMABLIT_DISABLE | NV4_REG_VPP_MASTER_DISABLE)) {
            return NVS2VERR_OK;
        }

        switch (lpS2VData->dwDstType) {
        case NVS2V_RGB32:
        case NVS2V_RGB16:
            dwSurfSize = lpS2VData->dwDstPitch * lpS2VData->dwHeight;
            fpDstSys = (FLATPTR) lpS2VData->fpDst;
            dwYPhase = 0;
            break;

        case NVS2V_YUYV:
            dwSurfSize = lpS2VData->dwDstPitch * lpS2VData->dwHeight;
            fpDstSys = (FLATPTR) lpS2VData->fpDst;
            dwYPhase = 0;
            break;

        case NVS2V_UYVY:
            dwSurfSize = lpS2VData->dwDstPitch * lpS2VData->dwHeight;
            fpDstSys = (FLATPTR) lpS2VData->fpDst;
            dwYPhase = 1;
            break;

        case NVS2V_DDSURF:
            dwSurfSize = lpS2VData->dwDstPitch * lpS2VData->dwHeight;
            lpDstSurf = (LPDDRAWI_DDRAWSURFACE_INT) lpS2VData->lpDstDirectDrawSurface;
            if (lpDstSurf->lpLcl->lpGbl->ddpfSurface.dwFourCC == FOURCC_UYVY ||
                lpDstSurf->lpLcl->lpGbl->ddpfSurface.dwFourCC == FOURCC_UYNV) {
                dwYPhase = 1;
            } else {
                dwYPhase = 0;
            }
            fpDstSys = lpDstSurf->lpLcl->lpGbl->fpVidMem;
            break;

        case NVS2V_INDEXED:
            dwSurfSize = lpS2VData->dwDstSize;
            fpDstSys = fpDstSurf[lpS2VData->dwIndex];
            dwYPhase = 0;
            break;

        default:
            return NVS2VERR_INVALID_PARMS;
        }

        if (lpS2VData->dwDstFlags & NVS2VF_SYSTEM) {
            FLATPTR fpSrcSys = (FLATPTR)lpS2VData->fpSrc;

            if (lpS2VData->dwSrcFlags & NVS2VF_OFFSET)
                fpSrcSys += pDriverData->BaseAddress;

            if (fpSrcSys < pDriverData->BaseAddress ||
               (fpSrcSys + dwSurfSize) >= (pDriverData->BaseAddress + pDriverData->TotalVRAM))
                return NVS2VERR_INVALID_PARMS;

            dwSrcContextDma = NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY;
            dwDstContextDma = NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V + lpS2VData->dwIndex;
            dwDstYOffset = dwDstCOffset = fpDstSys - fpSrcSurf[lpS2VData->dwIndex];
            dwDstYOffset += dwYPhase;
            dwDstCOffset += dwYPhase ^ 1;
            if (lpS2VData->dwSrcFlags & NVS2VF_OFFSET) {
                dwSrcYOffset = (FLATPTR)lpS2VData->SrcYC12.fpY;
                dwSrcCOffset = (FLATPTR)lpS2VData->SrcYC12.fpC;
                dwSrcUOffset = (FLATPTR)lpS2VData->SrcYUV12.fpU;
                dwSrcVOffset = (FLATPTR)lpS2VData->SrcYUV12.fpV;
            } else {
                dwSrcYOffset = (FLATPTR)lpS2VData->SrcYC12.fpY - pDriverData->BaseAddress;
                dwSrcCOffset = (FLATPTR)lpS2VData->SrcYC12.fpC - pDriverData->BaseAddress;
                dwSrcUOffset = (FLATPTR)lpS2VData->SrcYUV12.fpU - pDriverData->BaseAddress;
                dwSrcVOffset = (FLATPTR)lpS2VData->SrcYUV12.fpV - pDriverData->BaseAddress;
            }
        } else {
            if (lpS2VData->dwDstFlags & NVS2VF_OFFSET)
                fpDstSys += pDriverData->BaseAddress;

            if (fpDstSys < pDriverData->BaseAddress ||
               (fpDstSys + dwSurfSize) >= (pDriverData->BaseAddress + pDriverData->TotalVRAM))
                return NVS2VERR_INVALID_PARMS;

            dwSrcContextDma = NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V + lpS2VData->dwIndex;
            dwDstContextDma = NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY;
            dwDstYOffset = dwDstCOffset = fpDstSys - pDriverData->BaseAddress;
            dwDstYOffset += dwYPhase;
            dwDstCOffset += dwYPhase ^ 1;
            dwSrcYOffset = (FLATPTR)lpS2VData->SrcYC12.fpY - fpSrcSurf[lpS2VData->dwIndex];
            dwSrcCOffset = (FLATPTR)lpS2VData->SrcYC12.fpC - fpSrcSurf[lpS2VData->dwIndex];
            dwSrcUOffset = (FLATPTR)lpS2VData->SrcYUV12.fpU - fpSrcSurf[lpS2VData->dwIndex];
            dwSrcVOffset = (FLATPTR)lpS2VData->SrcYUV12.fpV - fpSrcSurf[lpS2VData->dwIndex];
        }

        switch (lpS2VData->dwSrcType) {
        case NVS2V_YC12:
        case NVS2V_YUV12:
        case NVS2V_YV12:
            // move luma
            vpp.pPusher->push (0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push (1, NV_DD_SYSTEM_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT);
            vpp.pPusher->push (2, SUB_CHANNEL(vpp.spareSubCh) | NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000);
            vpp.pPusher->push (3, NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY);    // SetContextDmaNotifies
            vpp.pPusher->push (4, dwSrcContextDma);                     // SetContextDmaBufferIn
            vpp.pPusher->push (5, dwDstContextDma);                     // SetContextDmaBufferOut
            vpp.pPusher->push (6, SUB_CHANNEL(vpp.spareSubCh) | NV039_OFFSET_IN | 0x200000);
            vpp.pPusher->push (7, dwSrcYOffset);                        // OffsetIn
            vpp.pPusher->push (8, dwDstYOffset);                        // OffsetOut
            vpp.pPusher->push (9, lpS2VData->dwSrcPitch);               // PitchIn
            vpp.pPusher->push (10, lpS2VData->dwDstPitch);              // PitchOut
            vpp.pPusher->push (11, lpS2VData->dwWidth);                 // LineLengthIn
            vpp.pPusher->push (12, lpS2VData->dwHeight);                // LineCount
            vpp.pPusher->push (13, 0x201);                              // Format
            vpp.pPusher->push (14, NV039_BUFFER_NOTIFY_WRITE_ONLY);     // BufferNotify
            vpp.pPusher->adjust (15);
            vpp.pPusher->start (TRUE);
            break;

        case NVS2V_YUYV:
        case NVS2V_UYVY:
        case NVS2V_RGB32:
        case NVS2V_RGB16:
            // move all

            VppResetNotification(
                &vpp.m_obj_a[DMABLT_TO_VID_NDX].notifier_a[1],
                lpProcInfo->hDMABlitToVid3
                );

            // NvResetEvent(lpProcInfo->hDMABlitToVid3);
            // pPioColourControlEventNotifier[1].status = NV_IN_PROGRESS;

            vpp.pPusher->push (0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push (1, vpp.m_obj_a[DMABLT_TO_VID_NDX].classIID);
            vpp.pPusher->push (2, SUB_CHANNEL(vpp.spareSubCh) | NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000);
            vpp.pPusher->push (3, vpp.m_obj_a[DMABLT_TO_VID_NDX].contextIID); // SetContextDmaNotifies
            vpp.pPusher->push (4, dwSrcContextDma);                         // SetContextDmaBufferIn
            vpp.pPusher->push (5, dwDstContextDma);                         // SetContextDmaBufferOut
            vpp.pPusher->push (6, SUB_CHANNEL(vpp.spareSubCh) | NV039_OFFSET_IN | 0x200000);
            vpp.pPusher->push (7, dwSrcYOffset);                            // OffsetIn
            vpp.pPusher->push (8, dwDstYOffset);                            // OffsetOut
            vpp.pPusher->push (9, lpS2VData->dwSrcPitch);                   // PitchIn
            vpp.pPusher->push (10, lpS2VData->dwDstPitch);                  // PitchOut
            if (lpS2VData->dwSrcType == NVS2V_RGB32) {
                vpp.pPusher->push (11, lpS2VData->dwWidth << 2);            // LineLengthIn
            } else {
                vpp.pPusher->push (11, lpS2VData->dwWidth << 1);            // LineLengthIn
            }
            vpp.pPusher->push (12, lpS2VData->dwHeight);                    // LineCount
            vpp.pPusher->push (13, 0x101);                                  // Format
            vpp.pPusher->push (14, VPP_NOTIFY_TYPE);                        // BufferNotify
            vpp.pPusher->adjust (15);
            vpp.pPusher->start (TRUE);

            pDriverData->dDrawSpareSubchannelObject = vpp.m_obj_a[DMABLT_TO_VID_NDX].classIID;
            break;

        default:
            return NVS2VERR_INVALID_PARMS;
        }

        // move chroma
        switch (lpS2VData->dwSrcType) {
        case NVS2V_YUYV:
        case NVS2V_UYVY:
        case NVS2V_RGB32:
        case NVS2V_RGB16:
            // already moved everything
            break;

        case NVS2V_YC12:
            // move chroma, even lines
            vpp.pPusher->push (0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push (1, NV_DD_SYSTEM_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT);
            vpp.pPusher->push (2, SUB_CHANNEL(vpp.spareSubCh) | NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000);
            vpp.pPusher->push (3, NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY); // SetContextDmaNotifies
            vpp.pPusher->push (4, dwSrcContextDma);                     // SetContextDmaBufferIn
            vpp.pPusher->push (5, dwDstContextDma);                     // SetContextDmaBufferOut
            vpp.pPusher->push (6, SUB_CHANNEL(vpp.spareSubCh) | NV039_OFFSET_IN | 0x200000);
            vpp.pPusher->push (7, dwSrcCOffset);                        // OffsetIn
            vpp.pPusher->push (8, dwDstCOffset);                        // OffsetOut
            vpp.pPusher->push (9, lpS2VData->dwSrcPitch);               // PitchIn
            vpp.pPusher->push (10, lpS2VData->dwDstPitch << 1);         // PitchOut
            vpp.pPusher->push (11, lpS2VData->dwWidth);                 // LineLengthIn
            vpp.pPusher->push (12, lpS2VData->dwHeight >> 1);           // LineCount
            vpp.pPusher->push (13, 0x201);                              // Format
            vpp.pPusher->push (14, NV039_BUFFER_NOTIFY_WRITE_ONLY);     // BufferNotify
            vpp.pPusher->adjust (15);
            vpp.pPusher->start (TRUE);

//            NvResetEvent(lpProcInfo->hDMABlitToVid3);
//            pPioColourControlEventNotifier[1].status = NV_IN_PROGRESS;
            VppResetNotification(
                &vpp.m_obj_a[DMABLT_TO_VID_NDX].notifier_a[1],
                lpProcInfo->hDMABlitToVid3
                );

            // move chroma, odd lines
            vpp.pPusher->push (0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push (1, vpp.m_obj_a[DMABLT_TO_VID_NDX].classIID);
            vpp.pPusher->push (2, SUB_CHANNEL(vpp.spareSubCh) | NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000);
            vpp.pPusher->push (3, vpp.m_obj_a[DMABLT_TO_VID_NDX].contextIID); // SetContextDmaNotifies
            vpp.pPusher->push (4, dwSrcContextDma);                         // SetContextDmaBufferIn
            vpp.pPusher->push (5, dwDstContextDma);                         // SetContextDmaBufferOut
            vpp.pPusher->push (6, SUB_CHANNEL(vpp.spareSubCh) | NV039_OFFSET_IN | 0x200000);
            vpp.pPusher->push (7, dwSrcCOffset);                            // OffsetIn
            vpp.pPusher->push (8, dwDstCOffset + lpS2VData->dwDstPitch);    // OffsetOut
            vpp.pPusher->push (9, lpS2VData->dwSrcPitch);                   // PitchIn
            vpp.pPusher->push (10, lpS2VData->dwDstPitch << 1);             // PitchOut
            vpp.pPusher->push (11, lpS2VData->dwWidth);                     // LineLengthIn
            vpp.pPusher->push (12, lpS2VData->dwHeight >> 1);               // LineCount
            vpp.pPusher->push (13, 0x201);                                  // Format
            vpp.pPusher->push (14, VPP_NOTIFY_TYPE);                        // BufferNotify
            vpp.pPusher->adjust (15);
            vpp.pPusher->start (TRUE);

            pDriverData->dDrawSpareSubchannelObject = vpp.m_obj_a[DMABLT_TO_VID_NDX].classIID;
            break;

        case NVS2V_YUV12:
        case NVS2V_YV12:
            // move U, even lines
            vpp.pPusher->push (0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push (1, NV_DD_SYSTEM_MEMORY_U_TO_VIDEO_MEMORY_FORMAT);
            vpp.pPusher->push (2, SUB_CHANNEL(vpp.spareSubCh) | NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000);
            vpp.pPusher->push (3, NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY);    // SetContextDmaNotifies
            vpp.pPusher->push (4, dwSrcContextDma);                     // SetContextDmaBufferIn
            vpp.pPusher->push (5, dwDstContextDma);                     // SetContextDmaBufferOut
            vpp.pPusher->push (6, SUB_CHANNEL(vpp.spareSubCh) | NV039_OFFSET_IN | 0x200000);
            vpp.pPusher->push (7, dwSrcUOffset);                        // OffsetIn
            vpp.pPusher->push (8, dwDstCOffset);                        // OffsetOut
            if (lpS2VData->dwSrcType == NVS2V_YUV12) {
                vpp.pPusher->push (9, lpS2VData->dwSrcPitch >> 1);      // PitchIn
            } else {
                vpp.pPusher->push (9, lpS2VData->dwSrcYV12ChromaPitch); // PitchIn
            }
            vpp.pPusher->push (10, lpS2VData->dwDstPitch << 1);         // PitchOut
            vpp.pPusher->push (11, lpS2VData->dwWidth >> 1);            // LineLengthIn
            vpp.pPusher->push (12, lpS2VData->dwHeight >> 1);           // LineCount
            vpp.pPusher->push (13, 0x401);                              // Format
            vpp.pPusher->push (14, NV039_BUFFER_NOTIFY_WRITE_ONLY);     // BufferNotify
            vpp.pPusher->adjust (15);
            vpp.pPusher->start (TRUE);

            // move V, even lines
            vpp.pPusher->push (0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push (1, NV_DD_SYSTEM_MEMORY_V_TO_VIDEO_MEMORY_FORMAT);
            vpp.pPusher->push (2, SUB_CHANNEL(vpp.spareSubCh) | NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000);
            vpp.pPusher->push (3, NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY);    // SetContextDmaNotifies
            vpp.pPusher->push (4, dwSrcContextDma);                     // SetContextDmaBufferIn
            vpp.pPusher->push (5, dwDstContextDma);                     // SetContextDmaBufferOut
            vpp.pPusher->push (6, SUB_CHANNEL(vpp.spareSubCh) | NV039_OFFSET_IN | 0x200000);
            vpp.pPusher->push (7, dwSrcVOffset);                        // OffsetIn
            vpp.pPusher->push (8, dwDstCOffset + 2);                    // OffsetOut
            if (lpS2VData->dwSrcType == NVS2V_YUV12) {
                vpp.pPusher->push (9, lpS2VData->dwSrcPitch >> 1);      // PitchIn
            } else {
                vpp.pPusher->push (9, lpS2VData->dwSrcYV12ChromaPitch); // PitchIn
            }
            vpp.pPusher->push (10, lpS2VData->dwDstPitch << 1);         // PitchOut
            vpp.pPusher->push (11, lpS2VData->dwWidth >> 1);            // LineLengthIn
            vpp.pPusher->push (12, lpS2VData->dwHeight >> 1);           // LineCount
            vpp.pPusher->push (13, 0x401);                              // Format
            vpp.pPusher->push (14, NV039_BUFFER_NOTIFY_WRITE_ONLY);     // BufferNotify
            vpp.pPusher->adjust (15);
            vpp.pPusher->start (TRUE);

            // move U, odd lines
            vpp.pPusher->push (0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push (1, NV_DD_SYSTEM_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT);
            vpp.pPusher->push (2, SUB_CHANNEL(vpp.spareSubCh) | NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000);
            vpp.pPusher->push (3, NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY);    // SetContextDmaNotifies
            vpp.pPusher->push (4, dwSrcContextDma);                     // SetContextDmaBufferIn
            vpp.pPusher->push (5, dwDstContextDma);                     // SetContextDmaBufferOut
            vpp.pPusher->push (6, SUB_CHANNEL(vpp.spareSubCh) | NV039_OFFSET_IN | 0x200000);
            vpp.pPusher->push (7, dwSrcUOffset);                        // OffsetIn
            vpp.pPusher->push (8, dwDstCOffset + lpS2VData->dwDstPitch);// OffsetOut
            if (lpS2VData->dwSrcType == NVS2V_YUV12) {
                vpp.pPusher->push (9, lpS2VData->dwSrcPitch >> 1);      // PitchIn
            } else {
                vpp.pPusher->push (9, lpS2VData->dwSrcYV12ChromaPitch); // PitchIn
            }
            vpp.pPusher->push (10, lpS2VData->dwDstPitch << 1);         // PitchOut
            vpp.pPusher->push (11, lpS2VData->dwWidth >> 1);            // LineLengthIn
            vpp.pPusher->push (12, lpS2VData->dwHeight >> 1);           // LineCount
            vpp.pPusher->push (13, 0x401);                              // Format
            vpp.pPusher->push (14, NV039_BUFFER_NOTIFY_WRITE_ONLY);     // BufferNotify
            vpp.pPusher->adjust (15);
            vpp.pPusher->start (TRUE);

//            NvResetEvent(lpProcInfo->hDMABlitToVid3);
//            pPioColourControlEventNotifier[1].status = NV_IN_PROGRESS;
            VppResetNotification(
                &vpp.m_obj_a[DMABLT_TO_VID_NDX].notifier_a[1],
                lpProcInfo->hDMABlitToVid3
                );

            // move V, odd lines
            vpp.pPusher->push (0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push (1, vpp.m_obj_a[DMABLT_TO_VID_NDX].classIID);
            vpp.pPusher->push (2, SUB_CHANNEL(vpp.spareSubCh) | NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000);
            vpp.pPusher->push (3, vpp.m_obj_a[DMABLT_TO_VID_NDX].contextIID); // SetContextDmaNotifies
            vpp.pPusher->push (4, dwSrcContextDma);                         // SetContextDmaBufferIn
            vpp.pPusher->push (5, dwDstContextDma);                         // SetContextDmaBufferOut
            vpp.pPusher->push (6, SUB_CHANNEL(vpp.spareSubCh) | NV039_OFFSET_IN | 0x200000);
            vpp.pPusher->push (7, dwSrcVOffset);                            // OffsetIn
            vpp.pPusher->push (8, dwDstCOffset + lpS2VData->dwDstPitch + 2);// OffsetOut
            if (lpS2VData->dwSrcType == NVS2V_YUV12) {
                vpp.pPusher->push (9, lpS2VData->dwSrcPitch >> 1);          // PitchIn
            } else {
                vpp.pPusher->push (9, lpS2VData->dwSrcYV12ChromaPitch);     // PitchIn
            }
            vpp.pPusher->push (10, lpS2VData->dwDstPitch << 1);             // PitchOut
            vpp.pPusher->push (11, lpS2VData->dwWidth >> 1);                // LineLengthIn
            vpp.pPusher->push (12, lpS2VData->dwHeight >> 1);               // LineCount
            vpp.pPusher->push (13, 0x401);                                  // Format
            vpp.pPusher->push (14, VPP_NOTIFY_TYPE);                       // BufferNotify
            vpp.pPusher->adjust (15);
            vpp.pPusher->start (TRUE);

            pDriverData->dDrawSpareSubchannelObject = vpp.m_obj_a[DMABLT_TO_VID_NDX].classIID;
            break;

        default:
            return NVS2VERR_INVALID_PARMS;
        }

        // TBD: how do we get here with no event?  Is that normal?  How 
        // do I distinguish the case where we have no events period.
        if (lpS2VData->bWaitForCompletion) {

#ifdef VPP_USE_EVENT_NOTIFIERS
            if (lpProcInfo && lpProcInfo->hDMABlitToVid3) {

                if (IS_EVENT_ERROR(NvWaitEvent(lpProcInfo->hDMABlitToVid3, lpS2VData->dwTimeoutTime))) {
                    EVENT_ERROR;
                    return NVS2VERR_TIMEOUT;
                }
            } else {
                while (vpp.m_obj_a[DMABLT_TO_VID_NDX].notifier_a[1].status == NV_IN_PROGRESS) 
                {
#if IS_WINNT4
                    nvSpin(vpp.pSysInfo->dwSpinLoopCount); // TBD: is this right? @mjl@
#else
                    Sleep(0);
#endif
                }
                return NVS2VERR_NO_EVENT;
            }
#else
            if (VppWaitForNotification(&vpp.m_obj_a[DMABLT_TO_VID_NDX].notifier_a[1], lpProcInfo->hDMABlitToVid3,lpS2VData->dwTimeoutTime))
            {
                EVENT_ERROR;
                return NVS2VERR_TIMEOUT;
            }

#endif // USE EVENT NOTIFIERS
        }
        return NVS2VERR_OK;
// ************************************************************************
    case NVS2VCMD_INIT:
// ************************************************************************
        DWORD dwPitch;
        PVOID fpSys;
        NVS2VSURFTYPE surfType;

        if (lpS2VData->dwIndex >= 16)
            return NVS2VERR_INVALID_PARMS;

        if (lpS2VData->dwDstFlags & NVS2VF_SYSTEM) {
            // system surface they want to initialize is in destination
            dwPitch = lpS2VData->dwDstPitch;
            fpSys = lpS2VData->DstYC12.fpY;
            surfType = lpS2VData->dwDstType;
        } else {
            // system surface they want to initialize is in source
            dwPitch = lpS2VData->dwSrcPitch;
            fpSys = lpS2VData->SrcYC12.fpY;
            surfType = lpS2VData->dwSrcType;
        }

        switch (surfType) {
        case NVS2V_YC12:
        case NVS2V_YUV12:
        case NVS2V_YV12:
            dwSurfSize = dwPitch * lpS2VData->dwHeight * 3 / 2;
            break;
        case NVS2V_YUYV:
        case NVS2V_UYVY:
        case NVS2V_RGB16:
        case NVS2V_RGB32:
            dwSurfSize = dwPitch * lpS2VData->dwHeight;
            break;
        default:
            return NVS2VERR_INVALID_PARMS;
        }
        dwSurfSize += 4096;     // to prevent MTMF fault

        if (NvRmAllocContextDma(pDriverData->dwRootHandle,
                                NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V + lpS2VData->dwIndex,
                                NV01_CONTEXT_DMA,
                                DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_WRITE) |
                                DRF_DEF(OS03, _FLAGS, _LOCKED, _ALWAYS) |
                                DRF_DEF(OS03, _FLAGS, _COHERENCY, _CACHED),
                                fpSys,
                                (dwSurfSize - 1)) == 0) {

            // cache surface pointer
            fpSrcSurf[lpS2VData->dwIndex] = (FLATPTR) fpSys;

            // cache destination surface pointer (only used for indexed blits)
            switch (lpS2VData->dwDstType) {
            case NVS2V_YUYV:
                fpDstSurf[lpS2VData->dwIndex] = (FLATPTR) lpS2VData->fpDst;
                dwYPhase = 0;
                break;

            case NVS2V_UYVY:
                fpDstSurf[lpS2VData->dwIndex] = (FLATPTR) lpS2VData->fpDst;
                dwYPhase = 1;
                break;

            case NVS2V_DDSURF:
                lpDstSurf = (LPDDRAWI_DDRAWSURFACE_INT) lpS2VData->lpDstDirectDrawSurface;
                if (lpDstSurf->lpLcl->lpGbl->ddpfSurface.dwFourCC == FOURCC_UYVY ||
                    lpDstSurf->lpLcl->lpGbl->ddpfSurface.dwFourCC == FOURCC_UYNV) {
                    dwYPhase = 1;
                } else {
                    dwYPhase = 0;
                }
                fpDstSurf[lpS2VData->dwIndex] = lpDstSurf->lpLcl->lpGbl->fpVidMem;
                break;
            }
            if (lpS2VData->dwDstFlags & NVS2VF_OFFSET)
                fpDstSurf[lpS2VData->dwIndex] += pDriverData->BaseAddress;

            if (!lpProcInfo || lpProcInfo->hDMABlitToVid3 == 0 || lpProcInfo->hDMABlitToVid0 == 0)
                return NVS2VERR_NO_EVENT;
            return NVS2VERR_OK;
        } else
            return NVS2VERR_INIT_FAILED;

// ************************************************************************
    case NVS2VCMD_RELEASE:
// ************************************************************************
        if (lpS2VData->dwIndex >= 16)
            return NVS2VERR_INVALID_PARMS;

        fpSrcSurf[lpS2VData->dwIndex] = 0;
        fpDstSurf[lpS2VData->dwIndex] = 0;

        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
                 NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V + lpS2VData->dwIndex);
        return NVS2VERR_OK;

// ************************************************************************
    case NVS2VCMD_QUERY_IDLE:
// ************************************************************************
        if (vpp.m_obj_a[DMABLT_TO_VID_NDX].notifier_a[1].status == NV_IN_PROGRESS)
            return NVS2VERR_BUSY;
        else
            return NVS2VERR_OK;

// ************************************************************************
    case NVS2VCMD_WAIT_FOR_IDLE:
// ************************************************************************
        if (lpProcInfo && lpProcInfo->hDMABlitToVid3) {
            if (!IS_EVENT_ERROR(NvWaitEvent(lpProcInfo->hDMABlitToVid3, lpS2VData->dwTimeoutTime))) {
                return NVS2VERR_OK;
            } else {
                EVENT_ERROR;
                return NVS2VERR_TIMEOUT;
            }
        } else {
            while (vpp.m_obj_a[DMABLT_TO_VID_NDX].notifier_a[1].status == NV_IN_PROGRESS)
            {
#if IS_WINNT4
                    nvSpin(vpp.pSysInfo->dwSpinLoopCount); // TBD: is this right? @mjl@
#else
                    Sleep(0);
#endif
            };
            return NVS2VERR_NO_EVENT;
        }

// ************************************************************************
    case NVS2VCMD_SET_SUBPICTURE:
// ************************************************************************
        if (lpS2VData->fpSP == 0) {
            // disable subpicture
            vpp.subPicture.offset = 0;
            if (pDriverData->NvDeviceVersion < NV_DEVICE_VERSION_10) {
                vpp.subPicture.format = NV038_OVERLAY_FORMAT_COLOR_LE_TRANSPARENT;
            } else {
                vpp.subPicture.format = NV088_OVERLAY_FORMAT_COLOR_LE_TRANSPARENT;
            }
            return NVS2VERR_OK;
        }
        if (lpS2VData->dwSPType == NVS2V_YUVA16) {
            if (pDriverData->NvDeviceVersion < NV_DEVICE_VERSION_10) {
                vpp.subPicture.format = NV038_OVERLAY_FORMAT_COLOR_LE_A4V6YB6A4U6YA6;
            } else {
                vpp.subPicture.format = NV088_OVERLAY_FORMAT_COLOR_LE_A4CR6YB6A4CB6YA6;
            }
        } else if (lpS2VData->dwSPType == NVS2V_YUVA32) {
            if (pDriverData->NvDeviceVersion < NV_DEVICE_VERSION_10) {
                vpp.subPicture.format = NV038_OVERLAY_FORMAT_COLOR_LE_A8V8U8Y8;
            } else {
                vpp.subPicture.format = NV088_OVERLAY_FORMAT_COLOR_LE_A8CR8CB8Y8;
            }
        } else {
            return NVS2VERR_INVALID_PARMS;
        }
        if (lpS2VData->dwSPFlags & NVS2VF_OFFSET) {
            vpp.subPicture.offset = lpS2VData->fpSP;
        } else {
            vpp.subPicture.offset = lpS2VData->fpSP - pDriverData->BaseAddress;
        }
        vpp.subPicture.pitch = lpS2VData->dwSPPitch;
        if (lpS2VData->dwSPFlags & NVS2VF_SP_SUBRECT) {
            vpp.subPicture.pitch  |= NVS2VF_SP_SUBRECT;
            vpp.subPicture.originX = lpS2VData->dwSubRectX;
            vpp.subPicture.originY = lpS2VData->dwSubRectY;
            vpp.subPicture.width   = lpS2VData->dwSubRectWidth;
            vpp.subPicture.height  = lpS2VData->dwSubRectHeight;
        } else {
            vpp.subPicture.originX = 0xFFFFFFFF;
            vpp.subPicture.originY = 0xFFFFFFFF;
            vpp.subPicture.width   = 0xFFFFFFFF;
            vpp.subPicture.height  = 0xFFFFFFFF;
        }
        if (lpS2VData->dwSPFlags & NVS2VF_SP_ODD) {
            vpp.subPicture.pitch |= NVS2VF_SP_ODD;
        }
        if (lpS2VData->dwSPFlags & NVS2VF_SP_EVEN) {
            vpp.subPicture.pitch |= NVS2VF_SP_EVEN;
        }
        if ((lpS2VData->dwSPFlags & (NVS2VF_SP_ODD | NVS2VF_SP_EVEN)) == 0) {
            vpp.subPicture.pitch |= NVS2VF_SP_ODD | NVS2VF_SP_EVEN;
        }
        return NVS2VERR_OK;

// ************************************************************************
    case NVS2VCMD_VPP_START:
// ************************************************************************
        if (lpS2VData->dwSrcFlags & NVS2VF_OFFSET) {
            dwSrcYOffset = (DWORD) lpS2VData->fpSrc;
        } else {
            dwSrcYOffset = (DWORD) lpS2VData->fpSrc - pDriverData->BaseAddress;
        }
        switch (lpS2VData->dwSrcType) {
        case NVS2V_YUYV: dwFourCC = FOURCC_YUY2; break;
        case NVS2V_UYVY: dwFourCC = FOURCC_UYVY; break;
        default: return NVS2VERR_INVALID_PARMS;
        }
        dwVPPFlags = VPP_ALL | VPP_SAVE_STATE;
        if (lpS2VData->dwSrcFlags & NVS2VF_VPP_ODD) {
            dwVPPFlags |= VPP_ODD;
        }
        if (lpS2VData->dwSrcFlags & NVS2VF_VPP_EVEN) {
            dwVPPFlags |= VPP_EVEN;
        }
        if (lpS2VData->dwSrcFlags & NVS2VF_VPP_BOB) {
            dwVPPFlags |= VPP_BOB;
        }
        if (lpS2VData->dwSrcFlags & NVS2VF_VPP_INTERLEAVED) {
            dwVPPFlags |= VPP_INTERLEAVED;
        }
        VppDoFlip(&(vpp),dwSrcYOffset, lpS2VData->dwSrcPitch, lpS2VData->dwWidth, lpS2VData->dwHeight, dwFourCC, dwVPPFlags);
        return NVS2VERR_OK;

// ************************************************************************
    case NVS2VCMD_VPP_QUERY:
// ************************************************************************
        if (vpp.pipeState.dwOpCount > 0 && vpp.pipeState.hLastStage) {
            if (IS_EVENT_ERROR(NvWaitEvent(vpp.pipeState.hLastStage, 0))) {
                return NVS2VERR_BUSY;
            }
        } else if (vpp.m_obj_a[DMABLT_TO_VID_NDX].notifier_a[1].status == NV_IN_PROGRESS) {
            // wait for any DMA blits to flush out before processing
            if (IS_EVENT_ERROR(NvWaitEvent(lpProcInfo->hDMABlitToVid3, 0))) {
                return NVS2VERR_BUSY;
            }
        }
        return NVS2VERR_OK;

// ************************************************************************
    case NVS2VCMD_VPP_FLIP:
// ************************************************************************
        VppDoFlip(&(vpp),0, 0, 0, 0, 0, VPP_RESTORE_STATE);
        return NVS2VERR_OK;

// ************************************************************************
    case NVS2VCMD_4CC_TO_VRGB:
// ************************************************************************
        DWORD dwSrcOffset, dwDstOffset;
        DWORD dwSrcFormat, dwDstFormat;
        DWORD dwSrcSize, dwDstSize;
        DWORD dwDeltaX, dwDeltaY;
        DWORD dwCombinedPitch;
        DWORD dwSrcContextDma;

        if ((lpS2VData->dwDstFlags & NVS2VF_SYSTEM) ||
            (lpS2VData->dwSrcFlags & NVS2VF_SWIZZLED)) {
            return NVS2VERR_INVALID_PARMS;
        }

        pDriverData->blitCalled = TRUE;
        pDriverData->TwoDRenderingOccurred = 1;

        dwDeltaX = ((lpS2VData->dwWidth - 1) << 20)  / (lpS2VData->DstRGB.dwDstWidth - 1);    // 12.20 fixed point
        dwDeltaY = ((lpS2VData->dwHeight - 1) << 20)  / (lpS2VData->DstRGB.dwDstHeight - 1);    // 12.20 fixed point
        dwSrcSize = (lpS2VData->dwHeight << 16) | lpS2VData->dwWidth;
        dwDstSize = (lpS2VData->DstRGB.dwDstHeight << 16) | lpS2VData->DstRGB.dwDstWidth;

        // get surfaces info
        if (lpS2VData->dwIndex == 0xFFFFFFFF) {
            dwSrcContextDma = vpp.hInVideoMemContextDma;
            if (lpS2VData->dwSrcFlags & NVS2VF_OFFSET) {
                dwSrcOffset = (DWORD)lpS2VData->fpSrc;
            } else {
                dwSrcOffset = VIDMEM_OFFSET((DWORD)lpS2VData->fpSrc);
            }
        } else {
            dwSrcContextDma = NV_DD_DMA_FLOATING_CONTEXT_DMA_IN_SYSTEM_S2V + lpS2VData->dwIndex;
            dwSrcOffset = 0;
        }
        if (lpS2VData->dwDstFlags & NVS2VF_OFFSET) {
            dwDstOffset = (DWORD)lpS2VData->fpDst;
        } else {
            dwDstOffset = VIDMEM_OFFSET((DWORD)lpS2VData->fpDst);
        }
        switch (lpS2VData->dwSrcType) {
        case NVS2V_YUYV:  dwSrcFormat = NV089_SET_COLOR_FORMAT_LE_CR8YB8CB8YA8; break;
        case NVS2V_UYVY:  dwSrcFormat = NV089_SET_COLOR_FORMAT_LE_YB8CR8YA8CB8; break;
        case NVS2V_RGB16: dwSrcFormat = NV089_SET_COLOR_FORMAT_LE_R5G6B5;       break;
        case NVS2V_RGB32: dwSrcFormat = NV089_SET_COLOR_FORMAT_LE_X8R8G8B8;     break;
        default:         return NVS2VERR_INVALID_PARMS;
        }

        if (lpS2VData->dwDstFlags & NVS2VF_SWIZZLED) {
            // swizzled
            DWORD dwLSBIndex, dwMSBIndex, dwLogWidth, dwLogHeight;

            // find log width
            dwLSBIndex = dwMSBIndex = lpS2VData->DstRGB.dwDstWidth;
            ASM_LOG2(dwLSBIndex);
            ASM_LOG2_R(dwMSBIndex);
            if (dwMSBIndex != dwLSBIndex) { dwMSBIndex++; } // this wasn't an even power of 2. round up.
            dwLogWidth = dwMSBIndex;

            // find log height
            dwLSBIndex = dwMSBIndex = lpS2VData->DstRGB.dwDstHeight;
            ASM_LOG2(dwLSBIndex);
            ASM_LOG2_R(dwMSBIndex);
            if (dwMSBIndex != dwLSBIndex) { dwMSBIndex++; } // this wasn't an even power of 2. round up.
            dwLogHeight = dwMSBIndex;

            // find surface format
            switch (lpS2VData->dwDstType) {
                case NVS2V_RGB16:  dwDstFormat = NV052_SET_FORMAT_COLOR_LE_R5G6B5;    break;
                case NVS2V_RGB32:  dwDstFormat = NV052_SET_FORMAT_COLOR_LE_A8R8G8B8;  break;
                default:           return NVS2VERR_INVALID_PARMS;
            }

            // set up swizzled surface
            // for now, assume destination is always in video memory
            vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push(1, D3D_CONTEXT_SURFACE_SWIZZLED);
            vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | NV052_SET_FORMAT | 0x80000);
            vpp.pPusher->push(3, dwDstFormat     |                     // SetFormat
                                  (dwLogWidth << 16) |
                                  (dwLogHeight << 24));
            vpp.pPusher->push(4, dwDstOffset);                         // SetOffset
            vpp.pPusher->adjust(5);

            // set mono pattern mask
            vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push(1, NV_DD_CONTEXT_PATTERN);
            vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) + NV044_SET_PATTERN_SELECT | 0xC0000);
            vpp.pPusher->push(3, NV044_SET_PATTERN_SELECT_MONOCHROME);
            switch (lpS2VData->dwDstType) {
            case NVS2V_RGB32:
                vpp.pPusher->push(4,NV_ALPHA_1_032);
                vpp.pPusher->push(5,NV_ALPHA_1_032);
                break;
            case NVS2V_RGB16:
                vpp.pPusher->push(4,NV_ALPHA_1_016);
                vpp.pPusher->push(5,NV_ALPHA_1_016);
                break;
            default: return NVS2VERR_INVALID_PARMS;
            }
            vpp.pPusher->adjust(6);
            pDriverData->dDrawSpareSubchannelObject = NV_DD_CONTEXT_PATTERN;
        } else {
            // not swizzled
            switch (lpS2VData->dwDstType) {
            case NVS2V_RGB16: dwDstFormat = NV063_SET_COLOR_FORMAT_LE_R5G6B5;   break;
            case NVS2V_RGB32: dwDstFormat = NV063_SET_COLOR_FORMAT_LE_X8R8G8B8; break;
            default:          return NVS2VERR_INVALID_PARMS;
            }
            dwCombinedPitch = (lpS2VData->dwDstPitch << 16) | lpS2VData->dwSrcPitch;
            if (lpS2VData->dwDstFlags & NVS2VF_SYSTEM) {
                dwCombinedPitch &= 0x0000FFFF;
                dwCombinedPitch |= dwCombinedPitch << 16;
            }
            if (lpS2VData->dwSrcFlags & NVS2VF_SYSTEM) {
                dwCombinedPitch &= 0xFFFF0000;
                dwCombinedPitch |= dwCombinedPitch >> 16;
            }
            if (pDriverData->bltData.dwLastColourFormat != dwDstFormat ||
                pDriverData->bltData.dwLastCombinedPitch != dwCombinedPitch ||
                pDriverData->bltData.dwLastSrcOffset != dwSrcOffset ||
                pDriverData->bltData.dwLastDstOffset != dwDstOffset) {
                vpp.pPusher->push(0, dDrawSubchannelOffset(NV_DD_SURFACES) | SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x100000);
                vpp.pPusher->push(1, dwDstFormat);     // SetColorFormat
                vpp.pPusher->push(2, dwCombinedPitch); // SetPitch
                vpp.pPusher->push(3, dwSrcOffset);     // SetSrcOffset
                vpp.pPusher->push(4, dwDstOffset);     // SetDstOffset
                vpp.pPusher->adjust(5);

                if (pDriverData->bltData.dwLastColourFormat != dwDstFormat) {
                    // set mono pattern mask
                    vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
                    vpp.pPusher->push(1, NV_DD_CONTEXT_PATTERN);
                    vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) + NV044_SET_PATTERN_SELECT | 0xC0000);
                    vpp.pPusher->push(3, NV044_SET_PATTERN_SELECT_MONOCHROME);
                    switch (lpS2VData->dwDstType) {
                    case NVS2V_RGB32:
                        vpp.pPusher->push(4,NV_ALPHA_1_032);
                        vpp.pPusher->push(5,NV_ALPHA_1_032);
                        break;
                    case NVS2V_RGB16:
                        vpp.pPusher->push(4,NV_ALPHA_1_016);
                        vpp.pPusher->push(5,NV_ALPHA_1_016);
                        break;
                    default: return NVS2VERR_INVALID_PARMS;
                    }
                    vpp.pPusher->adjust(6);
                    pDriverData->dDrawSpareSubchannelObject = NV_DD_CONTEXT_PATTERN;
                }

                pDriverData->bltData.dwLastColourFormat = dwDstFormat;
                pDriverData->bltData.dwLastCombinedPitch = dwCombinedPitch;
                pDriverData->bltData.dwLastSrcOffset = dwSrcOffset;
                pDriverData->bltData.dwLastDstOffset = dwDstOffset;
            }
        }

        // update clip
        bltUpdateClip(pDriverData);

        // set blt state
        if (SRCCOPYINDEX != pDriverData->bltData.dwLastRop) {
            vpp.pPusher->push(0, dDrawSubchannelOffset(NV_DD_ROP) | SET_ROP_OFFSET | 0x40000);
            vpp.pPusher->push(1, SRCCOPYINDEX);
            vpp.pPusher->adjust(2);
            pDriverData->bltData.dwLastRop = SRCCOPYINDEX;
        }
        // source key is disabled
        if (pDriverData->bltData.dwLastColourKey != 0xFFFFFFFF) {
            vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push(1, vpp.hContextColorKey);
            vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | SET_TRANSCOLOR_OFFSET | 0x40000);
            vpp.pPusher->push(3, 0);
            vpp.pPusher->adjust(4);
            pDriverData->bltData.dwLastColourKey = 0xFFFFFFFF;
            pDriverData->dDrawSpareSubchannelObject = vpp.hContextColorKey;
        }

        NvResetEvent(lpProcInfo->hFourCCBlt3);
        if (!lpProcInfo || !lpProcInfo->hFourCCBlt3) {
            vpp.pPusher->flush (TRUE, CPushBuffer::FLUSH_WITH_DELAY);
            dmaPusherSyncNotifier->status = NV_IN_PROGRESS;
        }

        // frame delivery control
        if ((lpS2VData->dwCmdFlags & NVS2VF_USE_DELIVERY_CTL) &&
             vpp.llDeliveryPeriod != 0) {
            NvResetEvent(lpProcInfo->hTimer3);
            vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push(1, NV_VPP_TIMER_IID);
            vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | NV004_SET_ALARM_TIME | 0x80000);
            vpp.pPusher->push(3, (DWORD)(vpp.llDeliveryTime & 0x00000000FFFFFFFF));
            vpp.pPusher->push(4, (DWORD)(vpp.llDeliveryTime >> 32));
            vpp.pPusher->push(5, SUB_CHANNEL(vpp.spareSubCh) | NV004_SET_ALARM_NOTIFY | 0x40000);
            vpp.pPusher->push(6, NV004_SET_ALARM_NOTIFY_WRITE_THEN_AWAKEN);
            vpp.pPusher->adjust(7);
            vpp.pPusher->start(TRUE);
            pDriverData->dDrawSpareSubchannelObject = NV_VPP_TIMER_IID;
            vpp.llDeliveryTime += vpp.llDeliveryPeriod;
            if (IS_EVENT_ERROR(NvWaitEvent(lpProcInfo->hTimer3, lpS2VData->dwTimeoutTime))) {
                DPF(" *** TIMER event failure ***");
                EVENT_ERROR;
            }
        }

        // wait for the scanline to hit the right place
        if ((lpS2VData->dwCmdFlags & NVS2VF_CRTC_SYNC) && 
             getDC()->flipPrimaryRecord.nvTimingInfo[lpS2VData->dwHead].Head != 0xFFFFFFFF &&
            (vpp.pSysInfo->dwNVClasses & NVCLASS_0004_TIMER)) {
            NV_CFGEX_CURRENT_SCANLINE_MULTI_PARAMS cfgParams;
            DWORD dwScanDelta;
            __int64 llWaitTime;

            NvResetEvent(lpProcInfo->hTimer3);

            // get current scanline
            cfgParams.Head = lpS2VData->dwHead;
            NvRmConfigGetEx (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
                             NV_CFGEX_CURRENT_SCANLINE_MULTI, &cfgParams, sizeof(cfgParams));

            // calculate how many scanlines we have to wait for
            if (cfgParams.RasterPosition > lpS2VData->dwScanline) {
                dwScanDelta = lpS2VData->dwScanline + getDC()->flipPrimaryRecord.nvTimingInfo[lpS2VData->dwHead].VTotal - cfgParams.RasterPosition;
            } else {
                dwScanDelta = lpS2VData->dwScanline - cfgParams.RasterPosition;
            }

            // calculate how long that is in ns
            llWaitTime = (LONGLONG)dwScanDelta * getDC()->flipPrimaryRecord.llFlipDurationNs[lpS2VData->dwHead] / getDC()->flipPrimaryRecord.nvTimingInfo[lpS2VData->dwHead].VTotal;

            // Sleep and don't wake up until that time has elapsed
            vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push(1, NV_VPP_TIMER_IID);
            vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | NV004_SET_ALARM_TIME_RELATIVE | 0x80000);
            vpp.pPusher->push(3, (DWORD)(llWaitTime & 0x00000000FFFFFFFF));
            vpp.pPusher->push(4, (DWORD)(llWaitTime >> 32));
            vpp.pPusher->push(5, SUB_CHANNEL(vpp.spareSubCh) | NV004_SET_ALARM_NOTIFY | 0x40000);
            vpp.pPusher->push(6, NV004_SET_ALARM_NOTIFY_WRITE_THEN_AWAKEN);
            vpp.pPusher->adjust(7);
            vpp.pPusher->start(TRUE);
            pDriverData->dDrawSpareSubchannelObject = NV_VPP_TIMER_IID;
            if (IS_EVENT_ERROR(NvWaitEvent(lpProcInfo->hTimer3, lpS2VData->dwTimeoutTime))) {
                DPF(" *** TIMER event failure ***");
                EVENT_ERROR;
            }
        }

        // do the fourcc blit
        vpp.pPusher->push(0, dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000);
        vpp.pPusher->push(1, NV_VPP_SCALED_IMAGE2_IID);
        vpp.pPusher->push(2, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_CONTEXT_DMA_IMAGE_OFFSET | 0x40000);
        vpp.pPusher->push(3, dwSrcContextDma);     // SetContextDmaImage
        vpp.pPusher->push(4, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_CONTEXT_SURFACE_OFFSET | 0x40000);
        if (lpS2VData->dwDstFlags & NVS2VF_SWIZZLED) {
            vpp.pPusher->push(5, D3D_CONTEXT_SURFACE_SWIZZLED);    // SetContextSurface
        } else {
            vpp.pPusher->push(5, NV_DD_SURFACES_2D);               // SetContextSurface
        }
        vpp.pPusher->push(6, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_SET_COLOR_FORMAT | 0x40000);
        vpp.pPusher->push(7, dwSrcFormat);         // SetColorFormat
        vpp.pPusher->push(8, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_CLIPPOINT_OFFSET | 0x180000);
        vpp.pPusher->push(9, 0);              // ClipPoint
        vpp.pPusher->push(10, dwDstSize);     // ClipSize
        vpp.pPusher->push(11, 0);             // ImageOutPoint
        vpp.pPusher->push(12, dwDstSize);     // ImageOutSize
        vpp.pPusher->push(13, dwDeltaX);      // DsDx
        vpp.pPusher->push(14, dwDeltaY);      // DtDy
        vpp.pPusher->push(15, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_IMAGE_IN_SIZE_OFFSET | 0x100000);
        vpp.pPusher->push(16, dwSrcSize);     // ImageInSize
        vpp.pPusher->push(17, (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_FOH << 24) |     // ImageInFormat
                                   (NV077_IMAGE_IN_FORMAT_ORIGIN_CENTER << 16) |
                                    lpS2VData->dwSrcPitch);
        vpp.pPusher->push(18, dwSrcOffset);   // ImageInOffset
        vpp.pPusher->push(19, lpS2VData->dwSrcPixelBias);  // ImageInPoint
        vpp.pPusher->push(20, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_NOTIFY_OFFSET | 0x40000);
        vpp.pPusher->push(21, NV089_NOTIFY_WRITE_THEN_AWAKEN);
        vpp.pPusher->push(22, dDrawSubchannelOffset(NV_DD_STRETCH) | NV089_NO_OPERATION | 0x40000);
        vpp.pPusher->push(23, 0);
        vpp.pPusher->adjust(24);
        vpp.pPusher->start(TRUE);

        if (lpS2VData->bWaitForCompletion) {
            if (lpProcInfo && lpProcInfo->hFourCCBlt3) {
                if (IS_EVENT_ERROR(NvWaitEvent(lpProcInfo->hFourCCBlt3, lpS2VData->dwTimeoutTime))) {
                    EVENT_ERROR;
                    return NVS2VERR_TIMEOUT;
                }
            } else {
                vpp.pPusher->flush (TRUE, CPushBuffer::FLUSH_WITH_DELAY);
                return NVS2VERR_NO_EVENT;
            }
        }
        return NVS2VERR_OK;

// ************************************************************************
    case NVS2VCMD_GET_TEXTURE_OFFSET:
// ************************************************************************
        lpS2VData->fpTexture = (LPVOID) nvGetTextureFromSystemMemAddress(pDriverData, (FLATPTR)lpS2VData->fpTexture);
        if ((DWORD)lpS2VData->fpTexture == 0xFFFFFFFF) {
            return NVS2VERR_GENERIC_FATAL;
        }
        return NVS2VERR_OK;

// ************************************************************************
    case NVS2VCMD_NOOP:
// ************************************************************************
        return NVS2VERR_OK;
    }

    return NVS2VERR_BAD_COMMAND;
}

#endif

// -----------------------------------------------------------------------------
// nvSetOverlayColorControl
//      Sets the colour control for the hardware overlay.  This is a private
//      interface for changing the controls without owning the overlay.  The
//      colour controls will be applied to any flippable overlay to the target
//      surface. If the lpDDCC is NULL, then capabilities are returned,
//      otherwise returns TRUE if succeeded.

DWORD __stdcall nvSetOverlayColorControl(DWORD dwDevice, Nv_ColorControl_t *lpDDCC)
{
    GLOBALDATA* pDriverData = NULL; // override global pDriverData

    pDriverData = getGlobalDataPtr(dwDevice);
    if (pDriverData == NULL) {
        return NVS2VERR_INIT_FAILED;  // We cannot handle this -- we need a context ptr
    }
    Vpp_t &vpp = pDriverData->vpp;

    if (lpDDCC) {
        if (lpDDCC->dwSize != sizeof(Nv_ColorControl_t)) return FALSE;
        if ((lpDDCC->dwFlags & ~(NV_VPP_COLOR_CONTRAST | NV_VPP_COLOR_BRIGHTNESS | NV_VPP_COLOR_HUE | NV_VPP_COLOR_SATURATION)) != 0)
            return (FALSE);
        if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
            while (lpDDCC->lHue < -180) lpDDCC->lHue += 360;
            while (lpDDCC->lHue >= 180) lpDDCC->lHue -= 360;
            if (lpDDCC->dwFlags & NV_VPP_COLOR_CONTRAST)
                vpp.colorCtrl.lContrast   = min(max(lpDDCC->lContrast * 20000 / 200, 0), 20000);
            if (lpDDCC->dwFlags & NV_VPP_COLOR_BRIGHTNESS)
                vpp.colorCtrl.lBrightness = min(max(lpDDCC->lBrightness * 100, -10000), 10000);
            if (lpDDCC->dwFlags & NV_VPP_COLOR_HUE)
                vpp.colorCtrl.lHue        = lpDDCC->lHue;
            if (lpDDCC->dwFlags & NV_VPP_COLOR_SATURATION)
                vpp.colorCtrl.lSaturation = min(max(lpDDCC->lSaturation * 20000 / 200, 0), 20000);
        } else {
            while (lpDDCC->lHue < 0)    lpDDCC->lHue += 360;
            while (lpDDCC->lHue >= 360) lpDDCC->lHue -= 360;
            if (lpDDCC->dwFlags & NV_VPP_COLOR_CONTRAST)
                vpp.colorCtrl.lContrast   = min(max(lpDDCC->lContrast * 511 / 200, 0), 511);
            if (lpDDCC->dwFlags & NV_VPP_COLOR_BRIGHTNESS)
                vpp.colorCtrl.lBrightness = min(max(lpDDCC->lBrightness, -256), 255);
            if (lpDDCC->dwFlags & NV_VPP_COLOR_HUE)
                vpp.colorCtrl.lHue        = lpDDCC->lHue;
            if (lpDDCC->dwFlags & NV_VPP_COLOR_SATURATION)
                vpp.colorCtrl.lSaturation = min(max(lpDDCC->lSaturation * 512 / 200, 0), 512);
            vpp.regOverlayColourControlEnable    = lpDDCC->dwReserved1 & 0x0000FFFF;
            vpp.regOverlayMode                   = lpDDCC->dwReserved1 >> 16;
        }
        return (TRUE);
    }
    else {
        if (!vpp.regOverlayColourControlEnable) return 0;
        return (NV_VPP_COLOR_CONTRAST | NV_VPP_COLOR_BRIGHTNESS | NV_VPP_COLOR_HUE | NV_VPP_COLOR_SATURATION);
    }
}

#ifdef __cplusplus
}       // extern "C"
#endif

#endif  // NVARCH >= 0x04

