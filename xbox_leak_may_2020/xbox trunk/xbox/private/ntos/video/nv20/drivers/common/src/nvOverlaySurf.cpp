// **************************************************************************
//
//       Copyright 1993-2001 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// **************************************************************************

#include "nvprecomp.h"
#include "nvOverlaySurf.h"

// A cleaner solution to this would be nice.  Note too that we may have multiple heads
// in different modes; so this is a bit ambiguous. @mjl@
#if (IS_WINNT4 || IS_WINNT5)
#undef GET_MODE_BPP
#define GET_MODE_BPP()           (vpp.m_ppdev->cBitsPerPel)
#endif


static DWORD nvCSCreate4ccIF09(Vpp_t *pVpp, LPDDRAWI_DDRAWSURFACE_LCL pSurf, HANDLE hOverlayOwner, DWORD dwExtraSize, DWORD dwExtraNumSurfaces);
static DWORD nvCSCreate4ccRGB0(Vpp_t *pVpp, LPDDRAWI_DDRAWSURFACE_LCL pSurf);
static DWORD nvCSCreate4ccIF09(Vpp_t *pVpp, LPDDRAWI_DDRAWSURFACE_LCL pSurf, HANDLE hOverlayOwner, DWORD dwExtraSize, DWORD dwExtraNumSurfaces);
static DWORD nvCSCreate4ccYUY2 (Vpp_t *pVpp, LPDDRAWI_DDRAWSURFACE_LCL pSurf, HANDLE hOverlayOwner, DWORD dwExtraSize, DWORD dwExtraNumSurfaces, DWORD dwBlockHeight);
static DWORD nvCSCreate4ccYV12 (Vpp_t *pVpp, LPDDRAWI_DDRAWSURFACE_LCL pSurf, HANDLE hOverlayOwner, DWORD dwExtraSize, DWORD dwExtraNumSurfaces);
#if (IS_WINNT5 || IS_WIN9X)
static DWORD nvCSCreate4ccNVID (Vpp_t *pVpp, LPDDRAWI_DDRAWSURFACE_LCL pSurf, LPDDRAWI_DIRECTDRAW_GBL pDDGbl);
static DWORD nvCSCreate4ccNV12 (Vpp_t *pVpp, LPDDRAWI_DDRAWSURFACE_LCL pSurf, HANDLE hOverlayOwner, DWORD dwExtraSize, DWORD dwExtraNumSurfaces);
static DWORD nvCSCreate4ccNVMC (Vpp_t *pVpp, LPDDRAWI_DDRAWSURFACE_LCL pSurf);
static DWORD nvCSCreate4ccNVSP (Vpp_t *pVpp, LPDDRAWI_DDRAWSURFACE_LCL pSurf);
#endif

//---------------------------------------------------------------------------

static DWORD nvCSCreate4ccRGB0 (Vpp_t *pVpp,LPDDRAWI_DDRAWSURFACE_LCL pSurf)
{
    Vpp_t &vpp = *pVpp;
    DWORD dwPitch;

    DWORD dwModeBPP = GET_MODE_BPP();

    if (pSurf->lpGbl->ddpfSurface.dwFourCC == FOURCC_RAW8) {
        dwPitch = pSurf->lpGbl->wWidth;
    }
    else {
        dwPitch = pSurf->lpGbl->wWidth * (dwModeBPP >> 3);
    }

    dwPitch = (dwPitch + vpp.pDriverData->dwSurfaceAlignPad) & ~vpp.pDriverData->dwSurfaceAlignPad;
    DWORD dwBlockSize = (dwPitch * ((DWORD)pSurf->lpGbl->wHeight));
    if (pSurf->lpGbl->ddpfSurface.dwFourCC == FOURCC_RAW8) {
        pSurf->lpGbl->ddpfSurface.dwRGBBitCount = 8;
        pSurf->lpGbl->ddpfSurface.dwRBitMask = 0;
        pSurf->lpGbl->ddpfSurface.dwGBitMask = 0;
        pSurf->lpGbl->ddpfSurface.dwBBitMask = 0;
    }
    else {
        pSurf->lpGbl->ddpfSurface.dwRGBBitCount = dwModeBPP;
        if (dwModeBPP == 8) {
            pSurf->lpGbl->ddpfSurface.dwRBitMask = 0;
            pSurf->lpGbl->ddpfSurface.dwGBitMask = 0;
            pSurf->lpGbl->ddpfSurface.dwBBitMask = 0;
            pSurf->lpGbl->ddpfSurface.dwFlags |= DDPF_PALETTEINDEXED8;
        }
        else if (dwModeBPP == 16) {
            pSurf->lpGbl->ddpfSurface.dwRBitMask = 0x0000F800;
            pSurf->lpGbl->ddpfSurface.dwGBitMask = 0x000007E0;
            pSurf->lpGbl->ddpfSurface.dwBBitMask = 0x0000001F;

        }
        else if (dwModeBPP == 32) {
            pSurf->lpGbl->ddpfSurface.dwRBitMask = 0x00FF0000;
            pSurf->lpGbl->ddpfSurface.dwGBitMask = 0x0000FF00;
            pSurf->lpGbl->ddpfSurface.dwBBitMask = 0x000000FF;
        }
    }
    pSurf->lpGbl->ddpfSurface.dwRGBAlphaBitMask = 0;
    NV_DBG_SHOW_FOURCC();
    pSurf->lpGbl->lPitch = dwPitch;
    pSurf->lpGbl->fpVidMem = 0;
    pSurf->lpGbl->dwReserved1 = 0;
    pSurf->ddsCaps.dwCaps |= DDSCAPS_HWCODEC;

    if (pSurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) {
        DWORD dwStatus;
        NVHEAP_ALLOC (dwStatus, pSurf->lpGbl->fpVidMem, dwBlockSize, TYPE_IMAGE);
        if (dwStatus == 0) {
            vpp.pDriverData->DDrawVideoSurfaceCount++;
            pSurf->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
            pSurf->ddsCaps.dwCaps |= DDSCAPS_LOCALVIDMEM;
            return (DD_OK);
        }
    }

    return (DDERR_OUTOFMEMORY);
}

//---------------------------------------------------------------------------

static DWORD nvCSCreate4ccIF09 (Vpp_t *pVpp,LPDDRAWI_DDRAWSURFACE_LCL pSurf,
                                HANDLE hOverlayOwner,
                                DWORD dwExtraSize, DWORD dwExtraNumSurfaces)
{
    DWORD dwPitch;
    DWORD i;
    Vpp_t &vpp = *pVpp;

    if ((pSurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
        (vpp.dwOverlaySurfaces > 0)) {
        if (vpp.overlayRelaxOwnerCheck)
            vpp.dwOverlayOwner = (DWORD)hOverlayOwner;
        if ((vpp.dwOverlayOwner != (DWORD)hOverlayOwner) ||
            (vpp.dwOverlaySurfaces + 1 >= NV_VPP_MAX_OVERLAY_SURFACES)) {
            return (DDERR_OUTOFCAPS);
        }
    }

    dwPitch = ((pSurf->lpGbl->wWidth + 1) & 0xFFFE) << 1;
    dwPitch = (dwPitch + vpp.dwOverlayByteAlignmentPad) & ~vpp.dwOverlayByteAlignmentPad;
    // first alloc some extra space for YUV9 surfaces to format chroma
    DWORD dwBlockSize = (dwPitch >> 1) * (((DWORD)pSurf->lpGbl->wHeight + 3) >> 2);
    // now allocate enough video memory for the Indeo part of the surface
    dwPitch = pSurf->lpGbl->wWidth;
    // allow extra room for block skip bits between Indeo portion of surface and YUY2 portion
    dwBlockSize += ((dwPitch * ((DWORD)pSurf->lpGbl->wHeight * 10L)) >> 3);
    // Force block to be properly aligned
    dwBlockSize = (dwBlockSize + vpp.dwOverlayByteAlignmentPad) & ~vpp.dwOverlayByteAlignmentPad;

    if ((pSurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) && (vpp.pDriverData->TotalVRAM >> 20 > 4)) {
        // Remember offset to 2nd half YUY2 portion of surface
        pSurf->lpGbl->dwReserved1 = dwBlockSize;
        // reset dwPitch to actual Indeo surface pitch
        dwPitch = pSurf->lpGbl->wWidth;
    }
    else
        pSurf->lpGbl->dwReserved1 = 0;

    pSurf->lpGbl->ddpfSurface.dwYUVBitCount = 9;
    pSurf->lpGbl->ddpfSurface.dwYBitMask = 0;
    pSurf->lpGbl->ddpfSurface.dwUBitMask = 0;
    pSurf->lpGbl->ddpfSurface.dwVBitMask = 0;
    pSurf->lpGbl->ddpfSurface.dwYUVAlphaBitMask = 0;
    NV_DBG_SHOW_FOURCC();
    pSurf->lpGbl->lPitch = dwPitch;
    pSurf->lpGbl->fpVidMem = 0;
    pSurf->ddsCaps.dwCaps |= DDSCAPS_HWCODEC;

    if (pSurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) {

        DWORD dwStatus;

        if (vpp.extraOverlayOffset[0] == 0 && dwExtraSize > 0) {
            // first time through, allocate extra space for VPP
            NVHEAP_ALLOC (dwStatus, pSurf->lpGbl->fpVidMem, dwBlockSize + dwExtraSize * dwExtraNumSurfaces, TYPE_OVERLAY);
            if (dwExtraNumSurfaces > 0) {
                vpp.extraOverlayOffset[0] = pSurf->lpGbl->fpVidMem - vpp.pDriverData->BaseAddress + dwBlockSize;
                vpp.extraOverlayOffset[0] = (vpp.extraOverlayOffset[0] + NV_TEXTURE_OFFSET_ALIGN) & ~NV_TEXTURE_OFFSET_ALIGN;
            }
            for (i=1; i<dwExtraNumSurfaces; i++) {
                vpp.extraOverlayOffset[i] = vpp.extraOverlayOffset[i-1] + dwExtraSize;
            }
        }
        else {
            NVHEAP_ALLOC (dwStatus, pSurf->lpGbl->fpVidMem, dwBlockSize, TYPE_OVERLAY);
        }

        if (dwStatus == 0) {
            vpp.pDriverData->DDrawVideoSurfaceCount++;
            pSurf->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
            pSurf->ddsCaps.dwCaps |= DDSCAPS_LOCALVIDMEM;

            if (pSurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {
                DWORD index = 0;
                while ((vpp.dwOverlaySurfaceLCL[index] != 0) &&
                    (index < NV_VPP_MAX_OVERLAY_SURFACES))
                    index++;
                nvAssert(index < NV_VPP_MAX_OVERLAY_SURFACES);
                vpp.dwOverlaySurfaceLCL[index] = (DWORD) pSurf;
                vpp.dwOverlayOwner = (DWORD)hOverlayOwner;
                vpp.dwOverlaySurfaces++;
            }
            return (DD_OK);
        }

        else {
            for (i=0; i<NV_VPP_MAX_EXTRA_SURFACES; i++) {
                vpp.extraOverlayOffset[i] = 0;
            }
            // We can't support overlay surfaces in system memory
            if (pSurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {
                return (DDERR_OUTOFMEMORY);
            }
        }

    }

    if (pSurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {
        DWORD index = 0;
        while ((vpp.dwOverlaySurfaceLCL[index] != 0) &&
            (index < NV_VPP_MAX_OVERLAY_SURFACES))
            index++;
        nvAssert(index < NV_VPP_MAX_OVERLAY_SURFACES);
        vpp.dwOverlaySurfaceLCL[index] = (DWORD) pSurf;
        vpp.dwOverlayOwner = (DWORD)hOverlayOwner;
        vpp.dwOverlaySurfaces++;
    }

    return (DD_OK);
}

//---------------------------------------------------------------------------
#if (IS_WIN9X || IS_WINNT5)

static DWORD nvCSCreate4ccNV12 (Vpp_t *pVpp, LPDDRAWI_DDRAWSURFACE_LCL pSurf, HANDLE hOverlayOwner,
                         DWORD dwExtraSize, DWORD dwExtraNumSurfaces)
{
    DWORD dwPitch, dwStatus;
    DWORD dwBlockSize;
    DWORD dw422BlockSize;
    int i;
    Vpp_t &vpp = *pVpp;

    if ((pSurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
        (vpp.dwOverlaySurfaces > 0)) {
        if (vpp.overlayRelaxOwnerCheck)
            vpp.dwOverlayOwner = (DWORD)hOverlayOwner;
        if ((vpp.dwOverlayOwner != (DWORD)hOverlayOwner) ||
            (vpp.dwOverlaySurfaces + 1 >= NV_VPP_MAX_OVERLAY_SURFACES)) {
            return (DDERR_OUTOFCAPS);
        }
    }

    // Round up surface height to an integer multiple of 16 scanlines
    pSurf->lpGbl->wHeight = (pSurf->lpGbl->wHeight + 15) & 0xFFF0;

    dwPitch = (pSurf->lpGbl->wWidth + 3) & ~3;
    dwPitch = (dwPitch + 127) & ~127;                                   // These surfaces MUST be 128 byte aligned so that
    pSurf->lpGbl->lPitch = dwPitch;                                     // when odd field is used as a texture it will be 128 byte aligned
    dwBlockSize = (dwPitch * (DWORD)pSurf->lpGbl->wHeight);       // 420 LUMA portion
    dwBlockSize += (dwPitch * ((DWORD)pSurf->lpGbl->wHeight >> 1));     // 420 CHROMA portion
    dwBlockSize += (8 * dwPitch) + 256;                                 // 8 scanline safety buffer + 16x16 block of 0x80 for Intra Block processing
    dwPitch = (pSurf->lpGbl->wWidth + 3) & ~3;
    dwPitch = ((dwPitch << 1) + 127) & ~127;

//    if ((pSurf->lpGbl->wWidth > 1600) && (vpp.pDriverData->TotalVRAM <= 0x2000000))  // Try to fit 1080i with motion comp in a 32 MB frame buffer
    if (pSurf->lpGbl->wWidth > 1600) // Our hardware isn't fast enough or memory efficient enough yet, always run 1080i in crippled mode
    {
        if ((vpp.pSysInfo->dwNVClasses & NVCLASS_FAMILY_KELVIN) &&
            (vpp.pDriverData->TotalVRAM > 0x2000000)) {
            vpp.pDriverData->bMCOverlaySrcIsSingleField1080i = FALSE;
            dw422BlockSize = (dwPitch * (DWORD)pSurf->lpGbl->wHeight);    // 422 portion
        } else {   
            vpp.pDriverData->bMCOverlaySrcIsSingleField1080i = TRUE;
            dw422BlockSize = (dwPitch * (DWORD)((pSurf->lpGbl->wHeight + 2) >> 1));    // 422 portion
        }
        if ((vpp.pSysInfo->dwNVClasses & NVCLASS_1196_CELSIUS) && 
            (!(vpp.pSysInfo->dwNVClasses & NVCLASS_FAMILY_KELVIN)))
            vpp.pDriverData->bMCHorizontallyDownscale1080i = TRUE;
        else
            vpp.pDriverData->bMCHorizontallyDownscale1080i = FALSE;
    } else if ((pSurf->lpGbl->wWidth > 1024) && 
               (vpp.pSysInfo->dwNVClasses & NVCLASS_1196_CELSIUS) &&
               (!(vpp.pSysInfo->dwNVClasses & NVCLASS_FAMILY_KELVIN))) { // actually this is 720p
        vpp.pDriverData->bMCOverlaySrcIsSingleField1080i = TRUE;
        vpp.pDriverData->bMCHorizontallyDownscale1080i = TRUE;
        dw422BlockSize = (dwPitch * (DWORD)pSurf->lpGbl->wHeight);    // 422 portion
    } else {
        vpp.pDriverData->bMCOverlaySrcIsSingleField1080i = FALSE;
        vpp.pDriverData->bMCHorizontallyDownscale1080i = FALSE;
        dw422BlockSize = (dwPitch * (DWORD)pSurf->lpGbl->wHeight);    // 422 portion
    }

    // Only allocate extra space for temporal filtering if it's allowed.
    // This decision was made earlier when the NVMC surface was created.
    if (vpp.pDriverData->bMCTemporalFilterDisabled < 2) {
        if (pSurf->lpGbl->wWidth < 1024)
            dw422BlockSize += (dwPitch * (DWORD)pSurf->lpGbl->wHeight);     // another 422 portion for filtering
    }

    pSurf->lpGbl->ddpfSurface.dwYUVBitCount = 12;
    pSurf->lpGbl->ddpfSurface.dwYBitMask = 0;
    pSurf->lpGbl->ddpfSurface.dwUBitMask = 0;
    pSurf->lpGbl->ddpfSurface.dwVBitMask = 0;
    pSurf->lpGbl->ddpfSurface.dwYUVAlphaBitMask = 0;
    NV_DBG_SHOW_FOURCC();
    pSurf->lpGbl->dwReserved1 = 0;
    pSurf->lpGbl->fpVidMem = 0;
    pSurf->ddsCaps.dwCaps |= DDSCAPS_HWCODEC;

    // Let overlay code know that we haven't got enough extra video memory for front end downscaling
    if ((vpp.pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) &&  (dwExtraNumSurfaces >= 2)) {
        if (vpp.pDriverData->TotalVRAM <= 0x1000000) {
            vpp.extraNumSurfaces = 0;
            dwExtraNumSurfaces = 0;
        }
    }

    if ((pSurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) && (vpp.extraOverlayOffset[0] == 0) && (dwExtraSize > 0) && (dwExtraNumSurfaces > 0)) {
        // first time through, allocate extra space for VPP
        NVHEAP_ALLOC (dwStatus, pSurf->lpGbl->fpVidMem, dwBlockSize + dw422BlockSize + dwExtraSize * dwExtraNumSurfaces, TYPE_OVERLAY);
        if (dwExtraNumSurfaces > 0) {
            vpp.extraOverlayOffset[0] = pSurf->lpGbl->fpVidMem - vpp.pDriverData->BaseAddress + dwBlockSize + dw422BlockSize;
        }
        for (i=1; i<(int)dwExtraNumSurfaces; i++) {
            vpp.extraOverlayOffset[i] = vpp.extraOverlayOffset[i-1] + dwExtraSize;
        }
    } else {
        NVHEAP_ALLOC (dwStatus, pSurf->lpGbl->fpVidMem, dwBlockSize + dw422BlockSize, TYPE_OVERLAY);
    }

    if (dwStatus == 0) {
        vpp.pDriverData->DDrawVideoSurfaceCount++;
        pSurf->ddsCaps.dwCaps |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
        pSurf->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;

        // Clear Intra block area to 0x80 bias value 
        // Actually, clear entire 420 surface to gray which avoids prediction pixel blending problems at edges of video image
        memset((char *)(VIDMEM_ADDR(pSurf->lpGbl->fpVidMem)), 128, dwBlockSize);
        // <<<NOTE>>>: Uncomment the following line if line above is ever removed
        // memset((char *)(VIDMEM_ADDR(pSurf->lpGbl->fpVidMem) + dwBlockSize - 256), 128, 256);

        // find a free surface
        for (i=0; i<8; i++) {
            if (vpp.pDriverData->dwMCNV12Surface[i] == 0) {
                break;
            }
        }

        if (i == 8) {
            // no free surfaces
            NVHEAP_FREE(pSurf->lpGbl->fpVidMem);
            vpp.pDriverData->DDrawVideoSurfaceCount--;
            return (DDERR_OUTOFCAPS);
        }

        vpp.pDriverData->dwMCNV12Surface[i] = (DWORD)pSurf;

        if (i==0) {
            vpp.pDriverData->nvMCSurfaceFlags[8].dwMCSurfaceBase = 0; // End of list marker
            vpp.pDriverData->dwMCDestinationSurface = (DWORD)pSurf;
        }

        vpp.pDriverData->nvMCSurfaceFlags[i].dwMCSurfaceBase = pSurf->lpGbl->fpVidMem;
        vpp.pDriverData->nvMCSurfaceFlags[i].dwMCSurface422Offset = pSurf->lpGbl->fpVidMem + dwBlockSize - vpp.pDriverData->BaseAddress;

        vpp.pDriverData->nvMCSurfaceFlags[i].dwMCMotionCompReferenceCount =
        vpp.pDriverData->nvMCSurfaceFlags[i].dwMCFormatCnvReferenceCount =
        vpp.pDriverData->nvMCSurfaceFlags[i].dwMCCompositeReferenceCount =
        vpp.pDriverData->nvMCSurfaceFlags[i].dwMCTemporalFilterReferenceCount = vpp.pDriverData->pRefCount->inc (CReferenceCount::INC_IMMEDIATE);

    } else {
        i = 8;
        while (--i >= 0) {
            if (vpp.pDriverData->nvMCSurfaceFlags[i].dwMCSurfaceBase != 0) {
                NVHEAP_FREE(vpp.pDriverData->nvMCSurfaceFlags[i].dwMCSurfaceBase);
                vpp.pDriverData->nvMCSurfaceFlags[i].dwMCSurfaceBase = 0;
                if (vpp.pDriverData->DDrawVideoSurfaceCount > 0)
                    vpp.pDriverData->DDrawVideoSurfaceCount--;
                if (vpp.dwOverlaySurfaces > 0)
                    vpp.dwOverlaySurfaces--;
                pSurf = (LPDDRAWI_DDRAWSURFACE_LCL)vpp.pDriverData->dwMCNV12Surface[i];
                vpp.pDriverData->dwMCNV12Surface[i] = 0;
                pSurf->lpGbl->fpVidMem = 0;
            }
        }
        if (vpp.dwOverlaySurfaces == 0) {
            vpp.dwOverlayOwner = 0;
            for (i=0; i<NV_VPP_MAX_EXTRA_SURFACES; i++) {
                vpp.extraOverlayOffset[i] = 0;
            }
            vpp.extraNumSurfaces = 0;
        }
        // We can't support overlay surfaces in system memory
        return (DDERR_OUTOFMEMORY);
    }

    if (pSurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {
        DWORD index = 0;
        while ((vpp.dwOverlaySurfaceLCL[index] != 0) &&
            (index < NV_VPP_MAX_OVERLAY_SURFACES))
            index++;
        nvAssert(index < NV_VPP_MAX_OVERLAY_SURFACES);
        vpp.dwOverlaySurfaceLCL[index] = (DWORD) pSurf;
        vpp.dwOverlayOwner = (DWORD)hOverlayOwner;
        vpp.dwOverlaySurfaces++;
    }

    return (DD_OK);
}
#endif


//---------------------------------------------------------------------------

#if (IS_WIN9X || IS_WINNT5)
static DWORD nvCSCreate4ccNVMC (Vpp_t *pVpp, LPDDRAWI_DDRAWSURFACE_LCL pSurf)
{
    DWORD dwStatus, i;
    DWORD dwBlockSize = 256;
    Vpp_t &vpp = *pVpp;

    NV_DBG_SHOW_FOURCC();

    pSurf->lpGbl->lPitch = 16;
    pSurf->lpGbl->dwReserved1 = 0;
    pSurf->lpGbl->fpVidMem = 0;
    pSurf->ddsCaps.dwCaps |= DDSCAPS_HWCODEC;

    // Currently we only support one motion comp session only because the display function only works with our 1 overlay
    if (pSurf->lpGbl->ddpfSurface.dwFourCC == FOURCC_NVMC) {

        if (vpp.pDriverData->dwMCNVMCSurface == 0) {

            NVHEAP_ALLOC (dwStatus, pSurf->lpGbl->fpVidMem, dwBlockSize, TYPE_IMAGE);

            if (dwStatus == 0) {
                vpp.pDriverData->DDrawVideoSurfaceCount++;
                pSurf->ddsCaps.dwCaps |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
                pSurf->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
                vpp.pDriverData->dwMCNVMCSurface = (DWORD)pSurf;

                // If frame buffer is 16MB, don't allow temporal filtering, to save room
                // for higher resolutions and video mirror buffers.
                // Possible values for bMCTemporalFilterDisabled:
                // 0 = temporal filtering enabled.
                // 1 = temporal filtering disabled but allowed (temporal space in NV12 buffers.)
                // 2 = temporal filtering disabled and not allowed (no temporal space in NV12 buffers.)
                if (vpp.pDriverData->TotalVRAM < 0x2000000)
                    vpp.pDriverData->bMCTemporalFilterDisabled = 2;
                else
                    vpp.pDriverData->bMCTemporalFilterDisabled = FALSE; // Default mode is enabled

                vpp.pDriverData->bMCPercentCurrentField = 65;       // Default temporal filter combination factor
                vpp.pDriverData->bMCOverlaySrcIsSingleField1080i = FALSE; // This is only needed to support 1080i in 32MB
                vpp.pDriverData->bMCHorizontallyDownscale1080i = FALSE; // This is only needed to support 1080i on NV15 or lower
                vpp.pDriverData->dwMCMostRecentlyDecodedSurfaceBase = -1;
                for (i=0; i<8; i++) {
                    vpp.pDriverData->dwMCNV12Surface[i] = 0;
                }
                for (i=0; i<9; i++) {
                    vpp.pDriverData->nvMCSurfaceFlags[i].dwMCSurfaceBase = 0;
                }
            }
            else {
                return (DDERR_OUTOFMEMORY);
            }

        } else {
            return (DDERR_OUTOFCAPS);
        }

    }

    else {

        if (vpp.pDriverData->dwMCNVDSSurfaceBase == 0) {

            NVHEAP_ALLOC (dwStatus, pSurf->lpGbl->fpVidMem, dwBlockSize, TYPE_IMAGE);

            if (dwStatus == 0) {
                vpp.pDriverData->dwMCNVDSSurfaceBase = pSurf->lpGbl->fpVidMem;
                vpp.pDriverData->DDrawVideoSurfaceCount++;
                pSurf->ddsCaps.dwCaps |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
                pSurf->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
            }
            else {
                return (DDERR_OUTOFMEMORY);
            }
        } else {
            pSurf->lpGbl->fpVidMem = vpp.pDriverData->dwMCNVDSSurfaceBase;
        }
    }

    return (DD_OK);
}
#endif

//---------------------------------------------------------------------------
#if (IS_WIN9X || IS_WINNT5)
static DWORD nvCSCreate4ccNVSP (Vpp_t *pVpp, LPDDRAWI_DDRAWSURFACE_LCL pSurf)
{
    DWORD dwPitch, dwStatus;
    Vpp_t &vpp = *pVpp;

    pSurf->lpGbl->ddpfSurface.dwRGBBitCount = 8;
    pSurf->lpGbl->ddpfSurface.dwRBitMask = 0;
    pSurf->lpGbl->ddpfSurface.dwGBitMask = 0;
    pSurf->lpGbl->ddpfSurface.dwBBitMask = 0;
    pSurf->lpGbl->ddpfSurface.dwRGBAlphaBitMask = 8;

    // Round up surface height to an integer multiple of 16 scanlines
    pSurf->lpGbl->wHeight = (pSurf->lpGbl->wHeight + 15) & 0xFFF0;

    dwPitch = ((pSurf->lpGbl->wWidth + 1) & 0xFFFE);
    dwPitch = (dwPitch + 127) & ~127;
    pSurf->lpGbl->lPitch = dwPitch;
    DWORD dwBlockSize = dwPitch * (DWORD)pSurf->lpGbl->wHeight; // calculate size of 8 bit index per pixel plane
    dwBlockSize <<= 1;  // double size to include 8 bit alpha per pixel plane
    dwBlockSize += (dwBlockSize << 1); // increase to include fully decompressed 32 bit per pixel plane
    dwBlockSize += 1024; // include room for indexed image look up table
    dwPitch = (pSurf->lpGbl->wWidth + 3) & ~3;
    dwPitch = ((dwPitch << 1) + 127) & ~127;
    dwBlockSize += (dwPitch * (DWORD)pSurf->lpGbl->wHeight);    // 422 work area for repairing NV12 surfaces
    NV_DBG_SHOW_FOURCC();
    pSurf->lpGbl->dwReserved1 = 0;
    pSurf->lpGbl->fpVidMem = 0;
    pSurf->ddsCaps.dwCaps |= DDSCAPS_HWCODEC;

    NVHEAP_ALLOC (dwStatus, pSurf->lpGbl->fpVidMem, dwBlockSize, TYPE_IMAGE);

    if (dwStatus == 0) {
        vpp.pDriverData->DDrawVideoSurfaceCount++;
        pSurf->ddsCaps.dwCaps |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
        pSurf->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
        vpp.pDriverData->dwMCNVSPSurface = (DWORD)pSurf;
    }
    else { // We could but currently don't support system memory subpictures
        return (DDERR_OUTOFMEMORY);
    }

    return (DD_OK);
}
#endif

//---------------------------------------------------------------------------
#if (IS_WINNT5 || IS_WIN9X)
static DWORD nvCSCreate4ccNVID (Vpp_t *pVpp, LPDDRAWI_DDRAWSURFACE_LCL pSurf, LPDDRAWI_DIRECTDRAW_GBL pDDGbl)
{
    DWORD dwPitch, dwIndex;
    Vpp_t &vpp = *pVpp;

    // find an unused surface
    for (dwIndex=0; dwIndex<4; dwIndex++) {
        if (vpp.pDriverData->dwMCIDCTAGPSurfaceBase[dwIndex] == 0) {
            break;
        }
    }
    if (dwIndex == 4) {
        return (DDERR_OUTOFCAPS);
    }

    pSurf->lpGbl->ddpfSurface.dwYUVBitCount = 8;
    pSurf->lpGbl->ddpfSurface.dwYBitMask = 0;
    pSurf->lpGbl->ddpfSurface.dwUBitMask = 0;
    pSurf->lpGbl->ddpfSurface.dwVBitMask = 0;
    pSurf->lpGbl->ddpfSurface.dwYUVAlphaBitMask = 0;

    pSurf->lpGbl->fpVidMem = 0;
    pSurf->lpGbl->dwReserved1 = 0;
    pSurf->ddsCaps.dwCaps |= DDSCAPS_HWCODEC;

    dwPitch = ((pSurf->lpGbl->wWidth + 1) & 0xFFFE);

    LONG adjustedHeight = (DWORD)((pSurf->lpGbl->wHeight + 1) & ~1);

    DWORD dwBlockSize = dwPitch * adjustedHeight;

    // pad the surface enough to be able to align it's base pointer to 256 bytes and it's size to end at 0xFF
    dwBlockSize += 1024;
    NV_DBG_SHOW_FOURCC();
    // now allocate enough AGP memory for the surface
    dwPitch = pSurf->lpGbl->wWidth;
    pSurf->lpGbl->lPitch = dwPitch;

    FLATPTR fpVidMem = 0;
    DWORD   dwStatus;

    NVHEAP_ALLOC (dwStatus, pSurf->lpGbl->fpVidMem, dwBlockSize, TYPE_IMAGE);

    if (dwStatus == 0) {
        // save initial unadjusted memory pointer for use when destroying this surface
        pSurf->lpGbl->dwReserved1 = pSurf->lpGbl->fpVidMem;
        pSurf->lpGbl->fpVidMem = (pSurf->lpGbl->fpVidMem + 511) & ~255;
        pSurf->ddsCaps.dwCaps |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
        vpp.pDriverData->DDrawVideoSurfaceCount++;

    } else {
        return (DDERR_OUTOFMEMORY);
    }

    // Make sure we reset this on next use
    vpp.pDriverData->dwMCCurrentCelsiusIDCTCtxDma = 0;

    nvAssert (dwIndex < 4);
    if (vpp.pDriverData->dwMCIDCTAGPCtxDmaSize[dwIndex] != 0) {
        NvRmFree(vpp.pDriverData->dwRootHandle, vpp.pDriverData->dwRootHandle,
                 NV_DD_DMA_FLOATING_MC_IDCT0_CONTEXT_DMA_FROM_MEMORY+dwIndex);
        vpp.pDriverData->dwMCIDCTAGPCtxDmaSize[dwIndex] = 0;
    }
    vpp.pDriverData->dwMCNVIDSurface[dwIndex] = (DWORD)pSurf;
    vpp.pDriverData->dwMCNVIDSurfaceReferenceCount[dwIndex] = vpp.pDriverData->pRefCount->inc (CReferenceCount::INC_IMMEDIATE);
    vpp.pDriverData->dwMCIDCTAGPSurfaceBase[dwIndex] = (DWORD)pSurf->lpGbl->fpVidMem;

    return (DD_OK);
}
#endif

//---------------------------------------------------------------------------

static DWORD nvCSCreate4ccYUY2 (Vpp_t *pVpp, LPDDRAWI_DDRAWSURFACE_LCL pSurf,
                         HANDLE hOverlayOwner,
                         DWORD dwExtraSize, DWORD dwExtraNumSurfaces, DWORD dwBlockHeight)
{
    DWORD dwPitch, dwStatus;
    DWORD i;
    Vpp_t &vpp = *pVpp;

    if ((pSurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
        (vpp.dwOverlaySurfaces > 0)) {
        if (vpp.overlayRelaxOwnerCheck)
            vpp.dwOverlayOwner = (DWORD)hOverlayOwner;
        if ((vpp.dwOverlayOwner != (DWORD)hOverlayOwner) ||
            (vpp.dwOverlaySurfaces + 1 >= NV_VPP_MAX_OVERLAY_SURFACES)) {
            return (DDERR_OUTOFCAPS);
        }
    }

    // First try video memory
    dwPitch = ((pSurf->lpGbl->wWidth + 1) & 0xFFFE) << 1;
    dwPitch = (dwPitch + vpp.dwOverlayByteAlignmentPad) & ~vpp.dwOverlayByteAlignmentPad;
    if ((vpp.regOverlayMode & (NV4_REG_OVL_MODE_TFILTER | NV4_REG_OVL_MODE_DFILTER)) &&
        (vpp.pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10)) {
        // texture alignment
        dwPitch = (dwPitch + NV_TEXTURE_OFFSET_ALIGN) & ~NV_TEXTURE_OFFSET_ALIGN;
    }

    DWORD dwBlockSize = (dwPitch * pSurf->lpGbl->wHeight);  // default block size

    pSurf->lpGbl->ddpfSurface.dwYUVBitCount = 16;
    if (pSurf->lpGbl->ddpfSurface.dwFourCC == FOURCC_YUY2) {
        pSurf->lpGbl->ddpfSurface.dwYBitMask = 0x00FF00FF;
        pSurf->lpGbl->ddpfSurface.dwUBitMask = 0x0000FF00;
        pSurf->lpGbl->ddpfSurface.dwVBitMask = 0xFF000000;
    }
    else {
        pSurf->lpGbl->ddpfSurface.dwYBitMask = 0xFF00FF00;
        pSurf->lpGbl->ddpfSurface.dwUBitMask = 0x000000FF;
        pSurf->lpGbl->ddpfSurface.dwVBitMask = 0x00FF0000;
    }
    pSurf->lpGbl->ddpfSurface.dwYUVAlphaBitMask = 0;
    NV_DBG_SHOW_FOURCC();

    pSurf->lpGbl->lPitch = dwPitch;
    pSurf->lpGbl->dwReserved1 = 0;
    pSurf->lpGbl->fpVidMem = 0;
    pSurf->ddsCaps.dwCaps |= DDSCAPS_HWCODEC;

    if ((pSurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) && (vpp.pDriverData->TotalVRAM >> 20 > 4)) {
        // Remember offset to 2nd half of surface
        pSurf->lpGbl->dwReserved1 = dwBlockSize;
    }

    if ((pSurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) && vpp.extraOverlayOffset[0] == 0 && dwExtraSize > 0) {
        // first time through, allocate extra space for VPP
        NVHEAP_ALLOC (dwStatus, pSurf->lpGbl->fpVidMem, dwBlockSize + dwExtraSize * dwExtraNumSurfaces, TYPE_OVERLAY);
        if (dwExtraNumSurfaces > 0) {
            vpp.extraOverlayOffset[0] = pSurf->lpGbl->fpVidMem - vpp.pDriverData->BaseAddress + dwBlockSize;
        }
        for (i=1; i<dwExtraNumSurfaces; i++) {
            vpp.extraOverlayOffset[i] = vpp.extraOverlayOffset[i-1] + dwExtraSize;
        }

    }
    else {
        // NV10 mediaport has a bug where it might write up to extra 256 bytes.
        // Workaround this bug by allocating a 256 byte guard band for all UYVY
        // surfaces.  Unfortunately, even non-videoport surfaces now get this guard band.
        if ((vpp.pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) &&
            (pSurf->lpGbl->ddpfSurface.dwFourCC == FOURCC_UYVY)) {
            dwBlockSize += 256;
        }
        NVHEAP_ALLOC (dwStatus, pSurf->lpGbl->fpVidMem, dwBlockSize, TYPE_OVERLAY);
    }

    if (dwStatus == 0) {
        vpp.pDriverData->DDrawVideoSurfaceCount++;
        pSurf->ddsCaps.dwCaps |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
        pSurf->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
    }
    else {
        for (i=0; i<NV_VPP_MAX_EXTRA_SURFACES; i++) {
            vpp.extraOverlayOffset[i] = 0;
        }

        return (DDERR_OUTOFMEMORY);
    }

    if (pSurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {
        DWORD index = 0;
        while ((vpp.dwOverlaySurfaceLCL[index] != 0) &&
            (index < NV_VPP_MAX_OVERLAY_SURFACES))
            index++;
        nvAssert(index < NV_VPP_MAX_OVERLAY_SURFACES);
        vpp.dwOverlaySurfaceLCL[index] = (DWORD) pSurf;
        vpp.dwOverlayOwner = (DWORD)hOverlayOwner;
        vpp.dwOverlaySurfaces++;
    }

    return (DD_OK);
}
//---------------------------------------------------------------------------

static DWORD nvCSCreate4ccYV12 (Vpp_t *pVpp, LPDDRAWI_DDRAWSURFACE_LCL pSurf,
                         HANDLE hOverlayOwner,
                         DWORD dwExtraSize, DWORD dwExtraNumSurfaces)
{
    DWORD dwPitch, dwStatus;
    DWORD i;
    Vpp_t &vpp = *pVpp;

    if ((pSurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
        (vpp.dwOverlaySurfaces > 0)) {
        if (vpp.overlayRelaxOwnerCheck)
            vpp.dwOverlayOwner = (DWORD)hOverlayOwner;
        if ((vpp.dwOverlayOwner != (DWORD)hOverlayOwner) ||
            (vpp.dwOverlaySurfaces + 1 >= NV_VPP_MAX_OVERLAY_SURFACES)) {
            return(DDERR_OUTOFCAPS);
        }
    }

    dwPitch = pSurf->lpGbl->wWidth;
    // allow extra room for block skip bits between Indeo portion of surface and YUY2 portion
    DWORD dwBlockSize = ((dwPitch * ((DWORD)pSurf->lpGbl->wHeight * 12L)) >> 3);
    // Force block to be properly aligned
    dwBlockSize = (dwBlockSize + vpp.dwOverlayByteAlignmentPad) & ~vpp.dwOverlayByteAlignmentPad;

    if ((pSurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) && (vpp.pDriverData->TotalVRAM >> 20 > 4)) {
        // Remember offset to 2nd half YUY2 portion of surface
        pSurf->lpGbl->dwReserved1 = dwBlockSize;
        // reset dwPitch to actual Indeo surface pitch
        dwPitch = pSurf->lpGbl->wWidth;
    }
    else {
        pSurf->lpGbl->dwReserved1 = 0;
    }

    pSurf->lpGbl->ddpfSurface.dwYUVBitCount = 9;
    pSurf->lpGbl->ddpfSurface.dwYBitMask = 0;
    pSurf->lpGbl->ddpfSurface.dwUBitMask = 0;
    pSurf->lpGbl->ddpfSurface.dwVBitMask = 0;
    pSurf->lpGbl->ddpfSurface.dwYUVAlphaBitMask = 0;
    NV_DBG_SHOW_FOURCC();
    pSurf->lpGbl->lPitch = dwPitch;
    pSurf->lpGbl->fpVidMem = 0;
    pSurf->ddsCaps.dwCaps |= DDSCAPS_HWCODEC;

    if (pSurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) {

        if (vpp.extraOverlayOffset[0] == 0 && dwExtraSize > 0) {
            // first time through, allocate extra space for VPP
            NVHEAP_ALLOC (dwStatus, pSurf->lpGbl->fpVidMem, dwBlockSize + dwExtraSize * dwExtraNumSurfaces, TYPE_OVERLAY);
            if (dwExtraNumSurfaces > 0) {
                vpp.extraOverlayOffset[0] = pSurf->lpGbl->fpVidMem - vpp.pDriverData->BaseAddress + dwBlockSize;
            }
            for (i=1; i<dwExtraNumSurfaces; i++) {
                vpp.extraOverlayOffset[i] = vpp.extraOverlayOffset[i-1] + dwExtraSize;
            }
        }
        else {
            NVHEAP_ALLOC (dwStatus, pSurf->lpGbl->fpVidMem, dwBlockSize, TYPE_OVERLAY);
        }

        if (dwStatus == 0) {
            vpp.pDriverData->DDrawVideoSurfaceCount++;
            pSurf->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
            pSurf->ddsCaps.dwCaps |= DDSCAPS_LOCALVIDMEM;

            if (pSurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {
                DWORD index = 0;
                while ((vpp.dwOverlaySurfaceLCL[index] != 0) &&
                       (index < NV_VPP_MAX_OVERLAY_SURFACES))
                    index++;
                nvAssert(index < NV_VPP_MAX_OVERLAY_SURFACES);
                vpp.dwOverlaySurfaceLCL[index] = (DWORD)pSurf;
                vpp.dwOverlayOwner = (DWORD)hOverlayOwner;
                vpp.dwOverlaySurfaces++;
            }

            return (DD_OK);
        }
        else {
            for (i=0; i<NV_VPP_MAX_EXTRA_SURFACES; i++) {
                vpp.extraOverlayOffset[i] = 0;
            }
            // We can't support overlay surfaces in system memory
            if (pSurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {
                return(DDERR_OUTOFMEMORY);
            }
        }
    }

    if (pSurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {
        DWORD index = 0;
        while ((vpp.dwOverlaySurfaceLCL[index] != 0) &&
               (index < NV_VPP_MAX_OVERLAY_SURFACES))
            index++;
        nvAssert(index < NV_VPP_MAX_OVERLAY_SURFACES);
        vpp.dwOverlaySurfaceLCL[index] = (DWORD) pSurf;
        vpp.dwOverlayOwner = (DWORD)hOverlayOwner;
        vpp.dwOverlaySurfaces++;
    }

    return (DD_OK);
}
//---------------------------------------------------------------------------
// figure out how much extra space is required for video post processing
// dwExtraSize is the size of one workspace surface, for NV4/5 we need 4,
// for NV10 we need 3.  For superpipelining, we need 8 or 5.
//
//

extern void VppCalcExtraSurfaceSpace(Vpp_t *pVpp, LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl, NvU32 dwDisplayHeight, NvU32 dwRequestedSurfaces,
                         NvU32 *dwExtraSize, NvU32 *dwExtraNumSurfaces, NvU32 *dwBlockHeight)
{
    DWORD dwPitch;
    Vpp_t &vpp = *pVpp;

    dbgTracePush ("nvCSCalcExtraSpace");

    *dwExtraSize = 0;
    *dwExtraNumSurfaces = 0;

    if (dwDisplayHeight > 768) {
        // disable HQVUp for resolutions greater than 1024x768
        vpp.regOverlayMode &= ~NV4_REG_OVL_MODE_VUPSCALE;
    }

    // find the height
    if (vpp.regOverlayMode & NV4_REG_OVL_MODE_VUPSCALE)
    {
        *dwBlockHeight = dwDisplayHeight;
    }
    else
    {
        *dwBlockHeight = (DWORD)pDDSLcl->lpGbl->wHeight;
    }

    if (IS_OVERLAY(pDDSLcl->ddsCaps.dwCaps, pDDSLcl->lpGbl->ddpfSurface.dwFourCC)) {

#if (IS_WINNT5 || IS_WIN9X)
        // just set this to something valid for now
        vpp.pDriverData->dwMCDestinationSurface = (DWORD)(pDDSLcl);
#endif

        // OPTIMIZEME: we can bring down the number of required surfaces to 2 if we
        // know that the VPP pipe is only going to use a single stage.
        if (vpp.pDriverData->NvDeviceVersion < NV_DEVICE_VERSION_10) {
            *dwExtraNumSurfaces = vpp.regVPPMaxSurfaces;
        } else {
            *dwExtraNumSurfaces = vpp.regVPPMaxSurfaces; // OPTIMIZEME: eventually want to make this 5 for NV10
        }

        // if less than 16M, then default to unpipelined VPP
        if (vpp.pDriverData->TotalVRAM < 0x1000000) {
            *dwExtraNumSurfaces = min(*dwExtraNumSurfaces, 4);
        }

        // find the YUYV pitch
        dwPitch = ((pDDSLcl->lpGbl->wWidth + 1) & 0xFFFE) << 1;
        dwPitch = (dwPitch + vpp.dwOverlayByteAlignmentPad) & ~vpp.dwOverlayByteAlignmentPad;
        *dwExtraSize = (dwPitch * *dwBlockHeight);
        vpp.extraPitch = dwPitch;

        if (vpp.regOverlayMode & (NV4_REG_OVL_MODE_TFILTER | NV4_REG_OVL_MODE_DFILTER)) {

            if (vpp.pDriverData->NvDeviceVersion < NV_DEVICE_VERSION_10) {

                // NV5 requires textures to have dimensions which are a power of 2
                DWORD dwBigPitch;

                // will be using texture unit, so must be a power of 2.
                // round up to the nearest one
#if 1
                DWORD dwLSBIndex, dwMSBIndex;

                dwLSBIndex = dwMSBIndex = dwPitch;
                ASM_LOG2(dwLSBIndex);
                ASM_LOG2_R(dwMSBIndex);
                // this wasn't an even power of 2. round up.
                if (dwMSBIndex != dwLSBIndex) { dwMSBIndex++; }
                // lowest allowed bigPitch is 32
                if (dwMSBIndex < 5) { dwMSBIndex = 5; }
                dwBigPitch = 1 << dwMSBIndex;

                dwLSBIndex = dwMSBIndex = *dwBlockHeight;
                ASM_LOG2(dwLSBIndex);
                ASM_LOG2_R(dwMSBIndex);
                // this wasn't an even power of 2. round up.
                if (dwMSBIndex != dwLSBIndex) { dwMSBIndex++; }
                *dwBlockHeight = 1 << dwMSBIndex;
#else

                if      (dwPitch > 4096) { dwBigPitch = 8192; }
                else if (dwPitch > 2048) { dwBigPitch = 4096; }
                else if (dwPitch > 1024) { dwBigPitch = 2048; }
                else if (dwPitch > 512)  { dwBigPitch = 1024; }
                else if (dwPitch > 256)  { dwBigPitch = 512;  }
                else if (dwPitch > 128)  { dwBigPitch = 256;  }
                else if (dwPitch > 64)   { dwBigPitch = 128;  }
                else if (dwPitch > 32)   { dwBigPitch = 64;   }
                else                     { dwBigPitch = 32;   }

                if      (*dwBlockHeight > 4096) { *dwBlockHeight = 8192; }
                else if (*dwBlockHeight > 2048) { *dwBlockHeight = 4096; }
                else if (*dwBlockHeight > 1024) { *dwBlockHeight = 2048; }
                else if (*dwBlockHeight > 512)  { *dwBlockHeight = 1024; }
                else if (*dwBlockHeight > 256)  { *dwBlockHeight = 512;  }
                else if (*dwBlockHeight > 128)  { *dwBlockHeight = 256;  }
                else if (*dwBlockHeight > 64)   { *dwBlockHeight = 128;  }
                else if (*dwBlockHeight > 32)   { *dwBlockHeight = 64;   }
                else if (*dwBlockHeight > 16)   { *dwBlockHeight = 32;   }
                else if (*dwBlockHeight > 8)    { *dwBlockHeight = 16;   }
                else if (*dwBlockHeight > 4)    { *dwBlockHeight = 8;    }
                else if (*dwBlockHeight > 2)    { *dwBlockHeight = 4;    }
                else if (*dwBlockHeight > 1)    { *dwBlockHeight = 2;    }
                else                            { *dwBlockHeight = 1;    }
#endif

                vpp.extraPitch = dwBigPitch;
                *dwExtraSize = (dwBigPitch * *dwBlockHeight);

            } else {    // NV10 or above

                // NV10 requires texture offsets to be a multiple of 256, so round up pitch to multiple of 256 for BOB
                dwPitch = (dwPitch + NV_TEXTURE_OFFSET_ALIGN) & ~NV_TEXTURE_OFFSET_ALIGN;
                vpp.extraPitch = dwPitch;
                *dwExtraSize = dwPitch * *dwBlockHeight;

            }

        }

        // add extra for alignment requirements
        *dwExtraSize += NV_TEXTURE_OFFSET_ALIGN + 1;

        // only allow extra surfaces to be created if they consume less than 1/2 of the total
        // video memory, other memory checks for various features is done in DriverInit and
        // will zero out the enable bits if there isn't enough memory
        if ((*dwExtraNumSurfaces * *dwExtraSize) > (vpp.pDriverData->TotalVRAM >> 1)) {
            if ((4 * *dwExtraSize) <= (vpp.pDriverData->TotalVRAM >> 1) && vpp.regVPPMaxSurfaces >= 4) {
                // can still do everything, but not superpipelined
                *dwExtraNumSurfaces = 4;
            } else if ((3 * *dwExtraSize) <= (vpp.pDriverData->TotalVRAM >> 1) && vpp.regVPPMaxSurfaces >= 3) {
                // at least we will be able to do one VPP stage superpipelined...
                if (vpp.pDriverData->TotalVRAM < 0x1000000) {
                    // 8M boards, lets be extra stingy
                    // Fixes WHQL update bob test in 1024x768x16 which accidentally allocates 720x960 surfaces instead of 720x480
                    *dwExtraNumSurfaces = 2;
                } else {
                    *dwExtraNumSurfaces = 3;
                }
            } else if ((2 * *dwExtraSize) <= (vpp.pDriverData->TotalVRAM >> 1) && vpp.regVPPMaxSurfaces >= 2) {
                // at least we will be able to do one VPP stage...
                *dwExtraNumSurfaces = 2;
            } else {
                *dwExtraNumSurfaces = 0;
            }
        }

        // refine this heuristic by checking free memory, if we don't have enough for 720x576x2x3, then back off if we can
        if ((NV_GET_VIDEO_HEAP_FREE() - (int)(*dwExtraNumSurfaces * *dwExtraSize)) < (768*576*2*3)) {
            switch (*dwExtraNumSurfaces) {
            case 6: *dwExtraNumSurfaces = 4; break;
            case 4: 
            case 3: *dwExtraNumSurfaces = 2; break;
            default: break;
            }
        }

        // This limit is primarily to allow enough FOURCC_NV12 surfaces to be created for HDTV sized HWMC
        // and to allow HWMC support in a 16 MB frame buffer.
        if ((vpp.pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) &&  (*dwExtraNumSurfaces > 2)) {
            if (vpp.pDriverData->TotalVRAM <= 0x1000000) {
                *dwExtraNumSurfaces = 2;
            } else {
                if (dwPitch > 3200) {
                    if (vpp.pDriverData->TotalVRAM <= 0x2000000)
                        *dwExtraNumSurfaces = 0;
                    else
                        *dwExtraNumSurfaces = 2;
                } else
                    if (dwPitch > 2048)
                        *dwExtraNumSurfaces = 2;
            }
        }

        // in low memory configurations, demote number of VPP surfaces if many overlay buffers requested
        if (vpp.pDriverData->TotalVRAM < 0x1000000 && dwRequestedSurfaces > 3) {
            switch (*dwExtraNumSurfaces) {
            case 6:
            case 5: *dwExtraNumSurfaces = 4; break;
            case 4: *dwExtraNumSurfaces = 3; break;
            case 3: *dwExtraNumSurfaces = 2; break;
            case 2:
            default: *dwExtraNumSurfaces = 0; break;
            }
        }

        vpp.extraNumSurfaces = *dwExtraNumSurfaces;
        vpp.extraIndex = 0;
        if (*dwExtraNumSurfaces >= NV_VPP_MAX_EXTRA_SURFACES || *dwExtraNumSurfaces == 3) {
            // enable VPP superpipelining
            vpp.regOverlayMode |= NV4_REG_OVL_MODE_LATEFLIPSYNC;
        }
    }

    if (*dwExtraNumSurfaces >= 4) {
        // enable multipass overlay downscale on factors greater than 2
        if (vpp.pDriverData->NvDeviceVersion < NV_DEVICE_VERSION_10) {
            vpp.dwOverlayMaxDownScaleX = min(vpp.dwOverlayMaxDownScale, 2);
            vpp.dwOverlayMaxDownScaleY = min(vpp.dwOverlayMaxDownScale, 2);
        } else {
            vpp.dwOverlayMaxDownScaleX = min(vpp.dwOverlayMaxDownScale, 4);
            vpp.dwOverlayMaxDownScaleY = min(vpp.dwOverlayMaxDownScale, 2);
        }
    }

    dbgTracePop();
}


//---------------------------------------------------------------------------

extern DWORD VppCreateFourCCSurface(Vpp_t *pVpp, LPDDRAWI_DDRAWSURFACE_LCL pSurf, LPDDRAWI_DIRECTDRAW_GBL pDDGbl, DWORD dwExtraSize, DWORD dwExtraNumSurfaces, DWORD dwBlockHeight)
{
    Vpp_t &vpp = *pVpp;
    DWORD dwRV;
    DWORD i;

    HANDLE hOverlayOwner = GetCurrentProcess();

    DPF_LEVEL(NVDBG_LEVEL_OVERLAY_INFO, "  FOURCC surface requested" );

    if ((pSurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
        (vpp.dwOverlaySurfaces > 0) &&
        (vpp.dwOverlayOwner != (DWORD) hOverlayOwner)) {
        return DDERR_OUTOFCAPS;
    }

    switch (pSurf->lpGbl->ddpfSurface.dwFourCC) {

        case FOURCC_RGB0:
        case FOURCC_RAW8:
            dwRV = nvCSCreate4ccRGB0 (pVpp, pSurf);
            if (dwRV != DD_OK) return(dwRV);
            break;
        case FOURCC_IF09:
        case FOURCC_YVU9:
        case FOURCC_IV32:
        case FOURCC_IV31:
            dwRV = nvCSCreate4ccIF09 (pVpp, pSurf, hOverlayOwner, dwExtraSize, dwExtraNumSurfaces);
            if (dwRV != DD_OK) return(dwRV);
            break;
#if IS_WINNT5 || IS_WIN9X
        case FOURCC_NV12:   // Our motion comp surface format
            dwRV = nvCSCreate4ccNV12 (pVpp, pSurf, hOverlayOwner, dwExtraSize, dwExtraNumSurfaces);
            if (dwRV != DD_OK) return(dwRV);
            break;
        case FOURCC_NVMC: // Our motion comp control surface, NV10 or greater
        case FOURCC_NVDS:
            dwRV = nvCSCreate4ccNVMC (pVpp, pSurf);
            if (dwRV != DD_OK) return(dwRV);
            break;
        case FOURCC_NVSP: // A subpicture surface (overlay)
            dwRV = nvCSCreate4ccNVSP (pVpp, pSurf);
            if (dwRV != DD_OK) return(dwRV);
            break;
        case FOURCC_NVID:
            dwRV = nvCSCreate4ccNVID (pVpp, pSurf, pDDGbl);
            if (dwRV != DD_OK) return(dwRV);
            break;
#endif
        case FOURCC_YUY2:
        case FOURCC_YUNV:
        case FOURCC_UYVY:
        case FOURCC_UYNV:
            dwRV = nvCSCreate4ccYUY2 (pVpp, pSurf, hOverlayOwner, dwExtraSize, dwExtraNumSurfaces, dwBlockHeight);
            if (dwRV != DD_OK) return(dwRV);
            break;
        case FOURCC_YV12:
        case FOURCC_420i:
            dwRV = nvCSCreate4ccYV12 (pVpp, pSurf, hOverlayOwner, dwExtraSize, dwExtraNumSurfaces);
            if (dwRV != DD_OK) return(dwRV);
            break;
        case FOURCC_NVT0:
        case FOURCC_NVT1:
        case FOURCC_NVT2:
        case FOURCC_NVT3:
        case FOURCC_NVT4:
        case FOURCC_NVT5:
        case FOURCC_NVS0:
        case FOURCC_NVS1:
        case FOURCC_NVS2:
        case FOURCC_NVS3:
        case FOURCC_NVS4:
        case FOURCC_NVS5:
        case FOURCC_NVHU:
        case FOURCC_NVHS:
#ifdef DXT_SUPPORT
        case FOURCC_DXT1:
        case FOURCC_DXT2:
        case FOURCC_DXT3:
        case FOURCC_DXT4:
        case FOURCC_DXT5:
#endif
            DPF ("should have been handled by texture-create or rejected by CanCreateSurface" );
            dbgError("NV Error!");
            return DDERR_INVALIDPARAMS;
        default:
            DPF ("unknown 4cc code in VppCreateFourCCSurface");
            dbgError("NV Error!");
            return DDERR_INVALIDPARAMS;
    }  // switch

    // align extra surfaces
    for (i=0; i<NV_VPP_MAX_EXTRA_SURFACES; i++) {
        vpp.extraOverlayOffset[i] = (vpp.extraOverlayOffset[i] + NV_TEXTURE_OFFSET_ALIGN - 1) & ~(NV_TEXTURE_OFFSET_ALIGN - 1);
    }

    if (vpp.regOverlayColourControlEnable) {
        // allocate an AGP workspace if overlay has been created
        if (IS_OVERLAY(pSurf->ddsCaps.dwCaps, pSurf->lpGbl->ddpfSurface.dwFourCC) && vpp.fpOverlayShadow==0) {
            DWORD szHostWorkSpace;

            if (vpp.extraOverlayOffset[0]) {
                szHostWorkSpace = (vpp.extraPitch) * (dwBlockHeight + 10) + 8192;
            } else if (pSurf->lpGbl->ddpfSurface.dwFourCC == FOURCC_YV12 ||
                       pSurf->lpGbl->ddpfSurface.dwFourCC == FOURCC_420i ||
                       pSurf->lpGbl->ddpfSurface.dwFourCC == FOURCC_YVU9 ||
                       pSurf->lpGbl->ddpfSurface.dwFourCC == FOURCC_IV32 ||
                       pSurf->lpGbl->ddpfSurface.dwFourCC == FOURCC_IV31 ||
                       pSurf->lpGbl->ddpfSurface.dwFourCC == FOURCC_IF09) {
                szHostWorkSpace = (pSurf->lpGbl->lPitch << 1) * (dwBlockHeight + 10) + 8192;
            } else {
                szHostWorkSpace = pSurf->lpGbl->lPitch * (dwBlockHeight + 10) + 8192;
            }

            // allocate work space in shared system memory
            // add an extra 10 lines + 4k for cache preload overrrun and page alignment
            vpp.fpOverlayShadow = (DWORD) AllocIPM(szHostWorkSpace);

            if (vpp.fpOverlayShadow) {
                unsigned long status;

                // First disconnect everything

                vpp.pDriverData->dDrawSpareSubchannelObject = 0;

                NvRmFree(vpp.pDriverData->dwRootHandle, vpp.pDriverData->dwRootHandle,
                    vpp.hFloatingContextDmaInOverlayShadow);

                // Now reconnect everything
                status = NvRmAllocContextDma(vpp.pDriverData->dwRootHandle,
                    vpp.hFloatingContextDmaInOverlayShadow,
                    NV01_CONTEXT_DMA,
                    DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_WRITE) |
                    DRF_DEF(OS03, _FLAGS, _LOCKED, _ALWAYS) |
                    DRF_DEF(OS03, _FLAGS, _COHERENCY, _CACHED),
                    (PVOID)vpp.fpOverlayShadow,
                    (szHostWorkSpace - 1));
                }
            // tells ring0 code that it can run now
            vpp.regRing0ColourCtlInterlockFlags = vpp.regRing0ColourCtlInterlockFlags | 0x1;
        }
    }

#if IS_WINNT4
    if (pSurf->ddsCaps.dwCaps & (DDSCAPS_OVERLAY)) {
#else
    if (pSurf->ddsCaps.dwCaps & (DDSCAPS_OVERLAY | DDSCAPS_VIDEOPORT)) {
#endif
        //
        // Initialize some stuff so KMVT stuff doesn't go crazy.  This can happen if
        // the KMVT routines (mainly vddFlipOverlay) gets called before UpdateOverlay32.
        //
        vpp.dwOverlaySrcX = 0;
        vpp.dwOverlaySrcY = 0;

        // Put some information about the surface in the surface so DVD decoders can
        // initialize DMA blits properly
        VppBltWriteDword(pVpp,VIDMEM_OFFSET(pSurf->lpGbl->fpVidMem), 0, GUID_NV_OVERLAY_INFO.Data1);
        VppBltWriteDword(pVpp,VIDMEM_OFFSET(pSurf->lpGbl->fpVidMem), 1, (GUID_NV_OVERLAY_INFO.Data3 << 16) |
                                                                 GUID_NV_OVERLAY_INFO.Data2);
        VppBltWriteDword(pVpp,VIDMEM_OFFSET(pSurf->lpGbl->fpVidMem), 2, (GUID_NV_OVERLAY_INFO.Data4[3] << 24) |
                                                                (GUID_NV_OVERLAY_INFO.Data4[2] << 16) |
                                                                (GUID_NV_OVERLAY_INFO.Data4[1] << 8) |
                                                                 GUID_NV_OVERLAY_INFO.Data4[0]);
        VppBltWriteDword(pVpp,VIDMEM_OFFSET(pSurf->lpGbl->fpVidMem), 3, (GUID_NV_OVERLAY_INFO.Data4[7] << 24) |
                                                                (GUID_NV_OVERLAY_INFO.Data4[6] << 16) |
                                                                (GUID_NV_OVERLAY_INFO.Data4[5] << 8) |
                                                                 GUID_NV_OVERLAY_INFO.Data4[4]);
        VppBltWriteDword(pVpp,VIDMEM_OFFSET(pSurf->lpGbl->fpVidMem), 4, VIDMEM_OFFSET(pSurf->lpGbl->fpVidMem));
        VppBltWriteDword(pVpp,VIDMEM_OFFSET(pSurf->lpGbl->fpVidMem), 5, (DWORD)pSurf);
        VppBltWriteDword(pVpp,VIDMEM_OFFSET(pSurf->lpGbl->fpVidMem), 6, (DWORD)pDDGbl);
    }

    return (DD_OK);

}  // VppCreateFourCCSurface()

