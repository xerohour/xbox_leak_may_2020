//******************************************************************************
//
// Copyright (c) 1995-1996 Microsoft Corporation
//
// Module Name:
//
//     DDRAW.C
//
// Abstract:
//
//     Implements all the DirectDraw components for the driver.
//
// Environment:
//
//     kernel mode only
//
// Notes:
//
// Revision History:
//
//     This driver was adapted from the S3 Display driver
//
//******************************************************************************

//******************************************************************************
//
// Copyright (c) 1996,1997  NVidia Corporation. All Rights Reserved
//
//******************************************************************************

#include "precomp.h"
#include "driver.h"
#include "nvPriv.h"
#include "nvReg.h"

#include "ddminint.h"

#include "nvalpha.h"
#ifdef NVD3D
    #include "dx95type.h"
    #include "nvntd3d.h"
    #include "nvProcMan.h"
    #include "ddmini.h"
#endif

#include "nvos.h"
#include "nvntioctl.h"
#include "nvntddctl.h"
#include "nvapi.h"
#include "nvsubch.h"
#include "nvcm.h"

#include "oglutils.h"

#ifdef NVD3D

#include "dx95type.h"
#define _NO_COM
#include "ddkernel.h"
#undef _NO_COM
#define NV_DD_DMA_PUSH_BUFFER_SIZE      (2*1024*1024)
                                        // This is a temporary default til we get the registry stuff going.

#ifndef _WIN64
extern DWORD __stdcall GetDriverInfo32(LPDDHAL_GETDRIVERINFODATA lpData);
#endif // D3D is not 64 bit code clean , skip for now...

// D3D Callbacks
extern DWORD __fastcall D3DClear2( LPD3DHAL_CLEAR2DATA);
extern DWORD __fastcall D3DValidateTextureStageState( LPD3DHAL_VALIDATETEXTURESTAGESTATEDATA);
extern DWORD __fastcall D3DDrawPrimitives2( LPD3DHAL_DRAWPRIMITIVES2DATA);
extern DWORD __fastcall D3DSetRenderTarget32(LPD3DHAL_SETRENDERTARGETDATA);


ULONG a = (ULONG)DDERR_UNSUPPORTED;
ULONG b = (ULONG)DDERR_GENERIC;
ULONG c = (ULONG)DDERR_OUTOFCAPS;

//extern DDHAL_DDKERNELCALLBACKS KernelCallbacks;
extern DDHAL_DDCOLORCONTROLCALLBACKS ColorControlCallbacks;
DDKERNELCAPS KernelCaps =
{
    sizeof( DDKERNELCAPS ),
    DDKERNELCAPS_SKIPFIELDS|DDKERNELCAPS_SETSTATE|
        DDKERNELCAPS_LOCK|DDKERNELCAPS_FLIPVIDEOPORT|DDKERNELCAPS_FLIPOVERLAY| \
        DDKERNELCAPS_FIELDPOLARITY,
    DDIRQ_DISPLAY_VSYNC|DDIRQ_VPORT0_VSYNC
};

DWORD WINAPI DdGetDriverInfo(struct _DD_GETDRIVERINFODATA *);
VOID nvTexDeSwizzle16BPP(int, int, int);
VOID nvTexSwizzle16BPP(int, int, int);

//*******************************************************************
// These functions initialize the driver caps and callbacks. They
// can be found in the common DX driver source.
// nvBuildDDHALInfo32   - ddDrv.cpp
//*******************************************************************
//BOOL nvBuildDDHALInfo32  (GLOBALDATA *pDriverData);

//
// One time DX6 Init/Destroy code. This is the "16 bit" ddraw init code
// from the Win9x GDI driver. Found in
// $(BUILDDIR)\main\drivers\common\nvX\src\ddmini16.c.
//
ULONG ddEnable(PDEV *lpDevice);

#else   // !NVD3D
DWORD __stdcall DriverInit_NT4(GLOBALDATA * pDriverData);
DWORD WINAPI GetDriverInfo32(struct _DD_GETDRIVERINFODATA *lpData);
#endif // #ifdef NVD3D

#ifdef NVPE
extern const DDHAL_DDVIDEOPORTCALLBACKS LPBVideoPortCallbacks;
extern const DDHAL_DDKERNELCALLBACKS KernelCallbacks;
extern const DDVIDEOPORTCAPS g_ddNVideoPortCaps;
///// H.AZAR: (02/29/2000): this function is defined in nvpkrnl.c (nvpe.lib)
#ifdef __cplusplus
extern "C" {
#endif
BOOL __stdcall NVPIsVPEEnabled(GLOBALDATA *pDriverData, DWORD hDriver);
#ifdef __cplusplus
}
#endif // __cplusplus
#endif // NVPE

/*
 * IN_VBLANK should be replaced by a test to see if the hardware is currently
 * in the vertical blank
 */
#define IN_VBLANK               FALSE
#define IN_DISPLAY              TRUE

/*
 * CURRENT_VLINE should be replaced by a macro to retrieve the current
 * scanline
 */
#define CURRENT_VLINE   (ppdev->pfnGetScanline(ppdev))

// Defines we'll use in the surface's 'dwReserved1' field:

#define DD_RESERVED_DIFFERENTPIXELFORMAT    0x0001


// Worst-case possible number of FIFO entries we'll have to wait for in
// DdBlt for any operation:

#define DDBLT_FIFO_COUNT    9

// NT is kind enough to pre-calculate the 2-d surface offset as a 'hint' so
// that we don't have to do the following, which would be 6 DIVs per blt:
//
//    y += (offset / pitch)
//    x += (offset % pitch) / bytes_per_pixel

#define convertToGlobalCord(x, y, surf) \
{                                       \
    y += surf->yHint;                   \
    x += surf->xHint;                   \
}

//******************************************************************************
// External declarations
//******************************************************************************

//******************************Public*Routine**********************************
//
// Function: DdMapMemory()
//
//      This is a new DDI call specific to Windows NT that is used to map
//      or unmap all the application modifiable portions of the frame buffer
//      into the specified process's address space.
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************

DWORD DdMapMemory(
                 PDD_MAPMEMORYDATA lpMapMemory)

    {
    PDEV*                           ppdev;
    VIDEO_SHARE_MEMORY              ShareMemory;
    VIDEO_SHARE_MEMORY_INFORMATION  ShareMemoryInformation;
    DWORD                           ReturnedDataLength;

    ppdev = (PDEV*) lpMapMemory->lpDD->dhpdev;

    if (lpMapMemory->bMap)
        {
        ShareMemory.ProcessHandle = lpMapMemory->hProcess;

        //**********************************************************************
        // 'RequestedVirtualAddress' isn't actually used for the SHARE IOCTL:
        //**********************************************************************

        ShareMemory.RequestedVirtualAddress = 0;

        //**********************************************************************
        // We map in starting at the top of the frame buffer:
        //**********************************************************************

        ShareMemory.ViewOffset = 0;

        //**********************************************************************
        // We map down to the end of the frame buffer.
        //
        // Note: There is a 64k granularity on the mapping (meaning that
        //       we have to round up to 64k).
        //
        // Note: If there is any portion of the frame buffer that must
        //       not be modified by an application, that portion of memory
        //       MUST NOT be mapped in by this call.  This would include
        //       any data that, if modified by a malicious application,
        //       would cause the driver to crash.  This could include, for
        //       example, any DSP code that is kept in off-screen memory.
        //
        // NOTE: AdapterMemorySize was adjusted in NVValidateModes
        //       to compensate for INSTANCE memory in offscreen memory.
        //       That is, Hash Table, Fifo Context, and Run Out information
        //       is stored in PRAMIN memory, which also exists in offscreen memory.
        //       We must make sure NO ONE (but the miniport) touches this offscreen memory.
        //       To make a long story short, AdapterMemorySize was adjusted by
        //       64k in NVValidateModes (The last 64k of VRAM contains Instance memory).
        //       See NVValidateModes for more comments.
        //**********************************************************************

        ShareMemory.ViewSize
        = ROUND_UP_TO_64K(ppdev->cbFrameBuf);

        if (EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_SHARE_VIDEO_MEMORY,
                               &ShareMemory,
                               sizeof(VIDEO_SHARE_MEMORY),
                               &ShareMemoryInformation,
                               sizeof(VIDEO_SHARE_MEMORY_INFORMATION),
                               &ReturnedDataLength))
            {
            DISPDBG((2, "Failed IOCTL_VIDEO_SHARE_MEMORY"));

            lpMapMemory->ddRVal = DDERR_GENERIC;
            return (DDHAL_DRIVER_HANDLED);
            }

        // Be aware of IA64 when doing ptr arithmetic !!
        // fpProcess is of type FLATPTR, VirtualAddres is PVOID
        lpMapMemory->fpProcess = (FLATPTR) ShareMemoryInformation.VirtualAddress;

#if _WIN32_WINNT < 0x0500
        // we need this to count Ddraw Clients
        NV_OglDDrawSurfaceCreated(ppdev);
#endif
        }

    else

        {
        ShareMemory.ProcessHandle           = lpMapMemory->hProcess;
        ShareMemory.ViewOffset              = 0;
        ShareMemory.ViewSize                = 0;
        ShareMemory.RequestedVirtualAddress = (VOID*) lpMapMemory->fpProcess;

        if (EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_UNSHARE_VIDEO_MEMORY,
                               &ShareMemory,
                               sizeof(VIDEO_SHARE_MEMORY),
                               NULL,
                               0,
                               &ReturnedDataLength))
            {
            RIP("Failed IOCTL_VIDEO_UNSHARE_MEMORY");
            }

#if _WIN32_WINNT < 0x0500
        // we need this to count Ddraw Clients
        NV_OglDDrawSurfaceDestroyed(ppdev);
#endif
        }

    lpMapMemory->ddRVal = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
    }

#ifndef NVD3D
/*
 * GetScanLine32
 */
DWORD GetScanLine32(
                   PDD_GETSCANLINEDATA lpGetScanLine )
    {
    PDEV*   ppdev;

    ppdev = lpGetScanLine->lpDD->dhpdev;

    /*
     * If a vertical blank is in progress the scan line is in
     * indeterminant. If the scan line is indeterminant we return
     * the error code DDERR_VERTICALBLANKINPROGRESS.
     * Otherwise we return the scan line and a success code
     */
    if (ppdev->pfnVBlankIsActive(ppdev))
        {
        lpGetScanLine->ddRVal = DDERR_VERTICALBLANKINPROGRESS;
        }
    else
        {
        lpGetScanLine->dwScanLine = ppdev->pfnGetScanline(ppdev);
        lpGetScanLine->ddRVal = DD_OK;
        }
    return DDHAL_DRIVER_HANDLED;

    } /* GetScanLine32 */
#endif // #ifndef NVD3D

//******************************Public*Routine**********************************
//
// Function: DdGetFlipStatus
//
//      If the display has gone through one refresh cycle since the flip
//      occurred, we return DD_OK.  If it has not gone through one refresh
//      cycle we return DDERR_WASSTILLDRAWING to indicate that this surface
//      is still busy "drawing" the flipped page.   We also return
//      DDERR_WASSTILLDRAWING if the bltter is busy and the caller wanted
//      to know if they could flip yet.
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************

DWORD DdGetFlipStatus(
                     PDD_GETFLIPSTATUSDATA lpGetFlipStatus)

    {
    PDEV*   ppdev;
    BYTE*   pjMmBase;
    FNGRAPHICSENGINEBUSY*  pfnGraphicsEngineBusy;

    ppdev    = (PDEV*) lpGetFlipStatus->lpDD->dhpdev;

    //**************************************************************************
    // Init chip specific function pointers
    //**************************************************************************

    pfnGraphicsEngineBusy = ppdev->pfnGraphicsEngineBusy;


    //**************************************************************************
    // We don't want a flip to work until after the last flip is done,
    // so we ask for the general flip status and ignore the vmem:
    //**************************************************************************

    lpGetFlipStatus->ddRVal = ppdev->pfnUpdateFlipStatus(ppdev, 0);

    //**************************************************************************
    // Check if the bltter is busy if someone wants to know if they can
    // flip:
    //**************************************************************************

    if (lpGetFlipStatus->dwFlags == DDGFS_CANFLIP)
        {
        if ( (lpGetFlipStatus->ddRVal == DD_OK) && (pfnGraphicsEngineBusy(ppdev)) )
            {
            lpGetFlipStatus->ddRVal = DDERR_WASSTILLDRAWING;
            }
        }

    return (DDHAL_DRIVER_HANDLED);
    }

//******************************Public*Routine**********************************
//
// Function: DdWaitForVerticalBlank
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************

DWORD DdWaitForVerticalBlank(
                            PDD_WAITFORVERTICALBLANKDATA lpWaitForVerticalBlank)

    {
    PDEV*   ppdev;
    FNWAITWHILEVBLANKACTIVE*    pfnWaitWhileVBlankActive;
    FNWAITWHILEDISPLAYACTIVE*   pfnWaitWhileDisplayActive;
    FNVBLANKISACTIVE*           pfnVBlankIsActive;

    ppdev    = (PDEV*) lpWaitForVerticalBlank->lpDD->dhpdev;

    //**************************************************************************
    // Init chip specific function pointers
    //**************************************************************************

    pfnWaitWhileVBlankActive = ppdev->pfnWaitWhileVBlankActive;
    pfnWaitWhileDisplayActive = ppdev->pfnWaitWhileDisplayActive;
    pfnVBlankIsActive = ppdev->pfnVBlankIsActive;


    switch (lpWaitForVerticalBlank->dwFlags)
        {
        case DDWAITVB_I_TESTVB:

            //******************************************************************
            // If TESTVB, it's just a request for the current vertical blank
            // status:
            //******************************************************************

            if (pfnVBlankIsActive(ppdev))
                lpWaitForVerticalBlank->bIsInVB = TRUE;
            else
                lpWaitForVerticalBlank->bIsInVB = FALSE;

            lpWaitForVerticalBlank->ddRVal = DD_OK;

            return (DDHAL_DRIVER_HANDLED);

        case DDWAITVB_BLOCKBEGIN:

            //******************************************************************
            // If BLOCKBEGIN is requested, we wait until the vertical blank
            // is over, and then wait for the display period to end:
            //******************************************************************

            pfnWaitWhileVBlankActive(ppdev);
            pfnWaitWhileDisplayActive(ppdev);

            lpWaitForVerticalBlank->ddRVal = DD_OK;
            return (DDHAL_DRIVER_HANDLED);

        case DDWAITVB_BLOCKEND:

            //******************************************************************
            // If BLOCKEND is requested, we wait for the vblank interval to end:
            //******************************************************************

            pfnWaitWhileDisplayActive(ppdev);
            pfnWaitWhileVBlankActive(ppdev);

            lpWaitForVerticalBlank->ddRVal = DD_OK;
            return (DDHAL_DRIVER_HANDLED);
        }

    return (DDHAL_DRIVER_NOTHANDLED);
    }

#ifdef IS_WINNT4
//******************************Public*Routine**********************************
//
// Function: DdCanCreateSurface
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************

DWORD __stdcall DdCanCreateSurface(PDD_CANCREATESURFACEDATA lpCanCreateSurface )
    {
    DWORD               caps;
    LPDDSURFACEDESC     lpDDSurface;
    int                 i;
    PDEV *ppdev = (PDEV *) lpCanCreateSurface->lpDD->dhpdev;

    /*
     * NOTES:
     *
     * This entry point is called after parameter validation but before
     * any object creation.   You can decide here if it is possible for
     * you to create this surface.  For example, if the person is trying
     * to create an overlay, and you already have the maximum number of
     * overlays created, this is the place to fail the call.
     *
     * You also need to check if the pixel format specified can be supported.
     *
     * lpCanCreateSurface->bIsDifferentPixelFormat tells us if the pixel format of the
     * surface being created matches that of the primary surface.  It can be
     * true for Z buffer and alpha buffers, so don't just reject it out of
     * hand...
     */
    DISPDBG((2, "CanCreateSurface, lpCanCreateSurface->lpDD=%08lx", lpCanCreateSurface->lpDD ));
    DISPDBG((2, "    lpCanCreateSurface->lpDDSurfaceDesc=%08lx", lpCanCreateSurface->lpDDSurfaceDesc ));

    lpDDSurface = lpCanCreateSurface->lpDDSurfaceDesc;

    caps = lpDDSurface->ddsCaps.dwCaps;

    if (caps & (DDSCAPS_MODEX | DDSCAPS_PALETTE))
        return DDHAL_DRIVER_NOTHANDLED;

    if (ppdev->cBitsPerPel == 8)
        {
        if (caps & DDSCAPS_ALPHA)
            return DDHAL_DRIVER_NOTHANDLED;
        }

    /*
     * check pixel format.   Don't allow pixel formats that aren't
     * the same, unless we have a valid fourcc code, an overlay,
     * an alpha surface, or z buffer.
     */
    if ( lpCanCreateSurface->bIsDifferentPixelFormat )
        {
        DISPDBG((2, "    different pixel format!" ));

        if ( lpDDSurface->ddpfPixelFormat.dwFlags & DDPF_FOURCC )
            {
            if (ppdev->cBitsPerPel == 8)
                {
                /* We don't support RGB FOURCCs in 8bpp mode */
                if (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_RGB0)
                    {
                    lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                    return DDHAL_DRIVER_HANDLED;
                    }
                }

            DISPDBG((2, "    FourCC requested (%4.4hs, 0x%08lx)",
                     (LPSTR) &lpDDSurface->ddpfPixelFormat.dwFourCC,
                     lpDDSurface->ddpfPixelFormat.dwFourCC ));
            for ( i=0; i < NV_MAX_FOURCC; i++ )
                {
                if ( lpDDSurface->ddpfPixelFormat.dwFourCC == ppdev->fourCC[i] )
                    {
                    DISPDBG((2, "    FOURCC=%4.4hs", (LPSTR) &ppdev->fourCC[i] ));
                    if (caps & DDSCAPS_OVERLAY)
                        {
                        if ((lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_UYVY) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_YUY2) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_YV12) &&
//                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_420i) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_IF09) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_YVU9) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_IV32) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_IV31))
                            {
                            lpCanCreateSurface->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                            return DDHAL_DRIVER_HANDLED;
                            }

                        if ((ppdev->cBitsPerPel == 32) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_IF09))
                            {
                            lpCanCreateSurface->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                            return DDHAL_DRIVER_HANDLED;
                            }
                        if ((lpDDSurface->dwFlags & DDSD_BACKBUFFERCOUNT) &&
                            (lpDDSurface->dwBackBufferCount > 1) &&
                            ((lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_UYVY) ||
                             (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_YUY2)))
                            {
                            lpCanCreateSurface->ddRVal = DDERR_INVALIDPARAMS;
                            return DDHAL_DRIVER_HANDLED;
                            }
                        if (ppdev->dwOverlaySurfaces > 0)
                            {
                            lpCanCreateSurface->ddRVal = DDERR_OUTOFCAPS;
                            return DDHAL_DRIVER_HANDLED;
                            }

                        if (ppdev->bHwVidOvl == FALSE)
                            {
                            lpCanCreateSurface->ddRVal = DDERR_NOOVERLAYHW;
                            return DDHAL_DRIVER_HANDLED;
                            }
                        // NB: Unlike win2k/Dx8, we HAVE to fail based on bandwidth in canCreateSurface.  If we do it in
                        //     createSurface we cause Windows Media player to fault.  It's probably their bug, but it's
                        //     easy enough to reject in both cases.
                        if (VppGetVideoScalerBandwidthStatus(&ppdev->pDriverData->vpp, (WORD)lpDDSurface->dwWidth) == FALSE)
                            {
                            lpCanCreateSurface->ddRVal = DDERR_OUTOFCAPS;
                            return DDHAL_DRIVER_HANDLED;
                            }

                        }
                    else if (caps & (DDSCAPS_VIDEOMEMORY | DDSCAPS_OFFSCREENPLAIN))
                        {
                        /* Front end YUV support cannot reformat data so Indeo is not supported */
                        /* Currently also disallowing front end YUV420 since YUYV or UYVY
                           should do ok when not enough backend bandwidth is available */
                        if ((lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_IF09) ||
                            (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_YVU9) ||
                            (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_IV32) ||
                            (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_IV31) ||
                            (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_YV12) ||
                            (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_420i))
                            {
                            lpCanCreateSurface->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                            return DDHAL_DRIVER_HANDLED;
                            }
                        /* Front end YUV support not available in 8bit indexed mode */
                        if (ppdev->cBitsPerPel == 8)
                            {
                            lpCanCreateSurface->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                            return DDHAL_DRIVER_HANDLED;
                            }
                        }

                    if (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_RGB0)
                        {
                        if (((ppdev->cBitsPerPel == 16) &&
                             (lpDDSurface->ddpfPixelFormat.dwRGBBitCount != DDBD_16)) ||
                            ((ppdev->cBitsPerPel == 32) &&
                             (lpDDSurface->ddpfPixelFormat.dwRGBBitCount != DDBD_32)))
                            {
                            lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                            return DDHAL_DRIVER_HANDLED;
                            }
                        }

                    lpCanCreateSurface->ddRVal = DD_OK;
                    return DDHAL_DRIVER_HANDLED;
                    }
                }
            }
        else if ( lpDDSurface->ddpfPixelFormat.dwFlags & DDPF_RGB )
            {

            /* Don't allow 3D devices in 8bpp or 32bpp */
            if (caps & DDSCAPS_3DDEVICE)
                {
                if ((lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 8) ||
                    (lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 32))
                    {
                    lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                    return DDHAL_DRIVER_HANDLED;
                    }
                }

            if (caps & (DDSCAPS_TEXTURE | DDSCAPS_OFFSCREENPLAIN))
                {
                /* Don't allow textures in 8bpp or 32bpp */
                if ((ppdev->cBitsPerPel == 8) || (ppdev->cBitsPerPel == 32))
                    {
                    lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                    return DDHAL_DRIVER_NOTHANDLED;
                    }
#if 0
                if (lpDDSurface->ddpfPixelFormat.dwFlags & PALETTEINDEXED8)
                    {
                    lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                    return DDHAL_DRIVER_HANDLED;
                    }
#endif

                if (lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 16)
                    {
                        // ELAU NV3 cannot handle 565 format
                        if(lpDDSurface->ddpfPixelFormat.dwGBitMask == 0x7e0)
                        {
                            lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                            return DDHAL_DRIVER_HANDLED;
                        }
                        /* accept all 16 bpp texture formats */
                        lpCanCreateSurface->ddRVal = DD_OK;
                        return DDHAL_DRIVER_HANDLED;
                    }
                }

            /* For now, support z buffers in video memory only when our primary rendering depth is 16 bpp */
            if (caps & DDSCAPS_ZBUFFER)
                {
                if ((ppdev->cBitsPerPel == 8) || (ppdev->cBitsPerPel == 32))
                    return DDHAL_DRIVER_NOTHANDLED;

                if (lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 16)
                    {
                    /* accept all 16 bpp z buffer formats */
                    lpCanCreateSurface->ddRVal = DD_OK;
                    return DDHAL_DRIVER_HANDLED;
                    }
                }

            /* We don't support any other RGB surfaces not in our native format */
            lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
            return DDHAL_DRIVER_HANDLED;
            }
        else if ( lpDDSurface->ddpfPixelFormat.dwFlags & DDPF_ZBUFFER )
            {
            /* For now, support z buffers in video memory only when our primary rendering depth is 16 bpp */
            if (caps & DDSCAPS_ZBUFFER)
                {
                if ((ppdev->cBitsPerPel == 8) || (ppdev->cBitsPerPel == 32))
                    return DDHAL_DRIVER_NOTHANDLED;
                }

            DISPDBG((2, "Z BUFFER OK!" ));
            lpCanCreateSurface->ddRVal = DD_OK;
            return DDHAL_DRIVER_HANDLED;
            }

        /*
         * can't handle any other kinds of different fourcc or RGB overlays
         */
        lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
        return DDHAL_DRIVER_HANDLED;
        }

    if (caps & DDSCAPS_MODEX)
        {
        DISPDBG((2, "    Mode X requested" ));
        lpCanCreateSurface->ddRVal = DDERR_UNSUPPORTEDMODE;
        return DDHAL_DRIVER_HANDLED;
        }

    /* Don't allow 3D devices in 8bpp or 32bpp (except for primary surface access, fixes Jedi Knight) */
    if ((caps & DDSCAPS_3DDEVICE) && (!(caps & DDSCAPS_PRIMARYSURFACE)))
        {
        if ((ppdev->cBitsPerPel == 8) || (ppdev->cBitsPerPel == 32))
            {
            lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
            return DDHAL_DRIVER_HANDLED;
            }
        }

    if (caps & (DDSCAPS_FRONTBUFFER | DDSCAPS_PRIMARYSURFACE | DDSCAPS_VISIBLE))
        {
        DISPDBG((2, "    Frontbuffer requested" ));

        if (caps & DDSCAPS_OVERLAY)
            {
            lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
            return DDHAL_DRIVER_HANDLED;
            }

        if (caps & DDSCAPS_FLIP)
            {
            if (lpDDSurface->dwHeight > (unsigned long)ppdev->cyScreen)
                {
                if (lpDDSurface->dwFlags & DDSD_HEIGHT)
                    {
                    lpCanCreateSurface->ddRVal = DDERR_TOOBIGHEIGHT;
                    return DDHAL_DRIVER_HANDLED;
                    }
                else
                    {
                    lpDDSurface->dwHeight = (unsigned long)ppdev->cyScreen;
                    }

                }
            if (lpDDSurface->dwWidth > (unsigned long)ppdev->cxScreen)
                {
                if (lpDDSurface->dwFlags & DDSD_WIDTH)
                    {
                    lpCanCreateSurface->ddRVal = DDERR_TOOBIGWIDTH;
                    return DDHAL_DRIVER_HANDLED;
                    }
                else
                    {
                    lpDDSurface->dwWidth = (unsigned long)ppdev->cxScreen;
                    }
                }
            }
        lpCanCreateSurface->ddRVal = DD_OK;
        return DDHAL_DRIVER_HANDLED;

        }
    else if (caps & DDSCAPS_BACKBUFFER)
        {
        DISPDBG((2, "    Backbuffer requested" ));
        if (caps & DDSCAPS_OVERLAY)
            {
            lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
            return DDHAL_DRIVER_HANDLED;
            }

        if (caps & DDSCAPS_FLIP)
            {
            if (lpDDSurface->dwHeight > (unsigned long)ppdev->cyScreen)
                {
                if (lpDDSurface->dwFlags & DDSD_HEIGHT)
                    {
                    lpCanCreateSurface->ddRVal = DDERR_TOOBIGHEIGHT;
                    return DDHAL_DRIVER_HANDLED;
                    }
                else
                    {
                    lpDDSurface->dwHeight = (unsigned long)ppdev->cyScreen;
                    }

                }
            if (lpDDSurface->dwWidth > (unsigned long)ppdev->cyScreen)
                {
                if (lpDDSurface->dwFlags & DDSD_WIDTH)
                    {
                    lpCanCreateSurface->ddRVal = DDERR_TOOBIGWIDTH;
                    return DDHAL_DRIVER_HANDLED;
                    }
                else
                    {
                    lpDDSurface->dwWidth = (unsigned long)ppdev->cBitsPerPel;
                    }
                }
            }
        lpCanCreateSurface->ddRVal = DD_OK;
        return DDHAL_DRIVER_HANDLED;

        }
    else if (caps & DDSCAPS_TEXTURE)
        {
        /* Don't allow textures in 8bpp or 32bpp */
        if ((ppdev->cBitsPerPel == 8) || (ppdev->cBitsPerPel == 32))
            {
            lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
            return DDHAL_DRIVER_HANDLED;
            }
#ifndef NVD3D
        lpDDSurface->ddsCaps.dwCaps &= ~DDSCAPS_VIDEOMEMORY;
        lpDDSurface->ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
#endif
        lpCanCreateSurface->ddRVal = DD_OK;
        return DDHAL_DRIVER_HANDLED;

        }
    else if (caps & DDSCAPS_OFFSCREENPLAIN)
        {
        DISPDBG((2, "    Can create offscreen requested" ));

        if (caps & DDSCAPS_VIDEOMEMORY)
            {
            if (caps & DDSCAPS_OVERLAY)
                {
                lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                return DDHAL_DRIVER_HANDLED;
                }

            if (caps & DDSCAPS_FLIP)
                {
                if (lpDDSurface->dwHeight > (unsigned long)ppdev->cyScreen)
                    {
                    if (lpDDSurface->dwFlags & DDSD_HEIGHT)
                        {
                        lpCanCreateSurface->ddRVal = DDERR_TOOBIGHEIGHT;
                        return DDHAL_DRIVER_HANDLED;
                        }
                    else
                        {
                        lpDDSurface->dwHeight = (unsigned long)ppdev->cyScreen;
                        }
                    }
                if (lpDDSurface->dwWidth > (unsigned long)ppdev->cxScreen)
                    {
                    if (lpDDSurface->dwFlags & DDSD_WIDTH)
                        {
                        lpCanCreateSurface->ddRVal = DDERR_TOOBIGWIDTH;
                        return DDHAL_DRIVER_HANDLED;
                        }
                    else
                        {
                        lpDDSurface->dwWidth = (unsigned long)ppdev->cxScreen;
                        }

                    }
                }
            }
        lpCanCreateSurface->ddRVal = DD_OK;
        DISPDBG((2, "    Can create offscreen granted" ));
        return DDHAL_DRIVER_HANDLED;

        }
    else if (caps & (DDSCAPS_FLIP | DDSCAPS_VIDEOMEMORY | DDSCAPS_ZBUFFER))
        {
        DISPDBG((2, "    Can create flipping surface requested" ));

        if (caps & DDSCAPS_OVERLAY)
            {
            lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
            return DDHAL_DRIVER_HANDLED;
            }
        DISPDBG((2, "    Can create flipping surface available" ));
        lpCanCreateSurface->ddRVal = DD_OK;
        return DDHAL_DRIVER_HANDLED;

        }
    else
        if (caps & DDSCAPS_SYSTEMMEMORY)
        {
        lpCanCreateSurface->ddRVal = DD_OK;
        DISPDBG((2, "    Can create system memory requested" ));
        return DDHAL_DRIVER_HANDLED;
        }

    return DDHAL_DRIVER_NOTHANDLED;

    } /* DdCanCreateSurface */

//******************************Public*Routine**********************************
//
// Function: DdCreateSurface
//
// Routine Description:
//
//    Creates an off-screen surface.
//
//    We use the Millennium's own off-screen heap manager instead of DirectDraw's
//    so that the OpenGL MCD and DirectDraw parts can coexist -- at the time of this
//    writing NT has no support for call-backs from the driver to allocate memory,
//    which we need to do to allocate the MCD's back buffer and Z-buffer.  So
//    we simply manage all of off-screen memory ourselves.
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************
DWORD DdCreateSurface(
                     PDD_CREATESURFACEDATA lpCreateSurface)

    {
    PDEV*               ppdev;
    DD_SURFACE_LOCAL*   lpSurfaceLocal;
    DD_SURFACE_GLOBAL*  lpSurfaceGlobal;
    LPDDSURFACEDESC     lpSurfaceDesc;
    DWORD               dwByteCount;
    ULONG               lPitch;
    DWORD               dwHeight;
    NvU16               wWidth;
    LONG                wHeight;
    LONG                adjustedHeight;
    FLATPTR             fpVidMem;
    PDD_SURFACE_LOCAL   *powner;
    ULONG               dwBlockSize;
    ULONG   index;
    ULONG   memType;
    NvU32 dwStatus;
#ifdef DEBUG
    char                        fourCCStr[5];
#endif // DEBUG

    //**************************************************************************
    // Get pointer to device
    //**************************************************************************

    ppdev = (PDEV*) lpCreateSurface->lpDD->dhpdev;

    //**************************************************************************
    // On Windows NT, dwSCnt will always be 1, so there will only ever
    // be one entry in the 'lplpSList' array:
    //**************************************************************************
    powner = lpCreateSurface->lplpSList;
    lpSurfaceLocal  = lpCreateSurface->lplpSList[0];
    lpSurfaceGlobal  = lpCreateSurface->lplpSList[0]->lpGbl;
    lpSurfaceDesc   = lpCreateSurface->lpDDSurfaceDesc;

    //**************************************************************************
    // Get width and height
    //**************************************************************************

    wWidth  = (WORD)lpSurfaceGlobal->wWidth;    // wWidth is actually an unsigned long despite name
    wHeight = lpSurfaceGlobal->wHeight;

    //**************************************************************************
    // We repeat the same checks we did in 'DdCanCreateSurface' because
    // it's possible that an application doesn't call 'DdCanCreateSurface'
    // before calling 'DdCreateSurface'.
    //**************************************************************************

    ASSERTDD(lpSurfaceGlobal->ddpfSurface.dwSize == sizeof(DDPIXELFORMAT),
             "NT is supposed to guarantee that ddpfSurface.dwSize is valid");

    //**************************************************************************
    // Determine type and format of surface prior to allocation.
    //**************************************************************************

    if (
       //(lpSurfaceLocal->dwFlags & DDRAWISURF_HASPIXELFORMAT) &&
       (lpSurfaceGlobal->ddpfSurface.dwFlags & DDPF_FOURCC) )
    {
        DPF( "NVDD: FOURCC surface requested" );

        if ((lpSurfaceLocal->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
             (VppGetVideoScalerBandwidthStatus(&ppdev->pDriverData->vpp, wWidth) == FALSE))
        {
            DPF( "NVDD: Insufficient video bandwidth, overlay creation refused.");
            lpCreateSurface->ddRVal = DDERR_OUTOFCAPS;
            goto CREATE_HANDLED;
        }

        //**************************************************************************
        // we know the FOURCC code is OK, because we validated it
        // during CanCreateSurface.   Compute a block size based
        // on the code, and allocate system memory for it.
        //**************************************************************************
        switch ( lpSurfaceLocal->lpGbl->ddpfSurface.dwFourCC )
            {
            case FOURCC_RGB0:
                lpSurfaceGlobal->ddpfSurface.dwRGBBitCount = ppdev->cBitsPerPel;
                if (ppdev->cBitsPerPel == 8)
                    {
                    lpSurfaceGlobal->ddpfSurface.dwRBitMask = 0;
                    lpSurfaceGlobal->ddpfSurface.dwGBitMask = 0;
                    lpSurfaceGlobal->ddpfSurface.dwBBitMask = 0;
                    lpSurfaceGlobal->ddpfSurface.dwFlags |= DDPF_PALETTEINDEXED8;
                    }
                else if (ppdev->cBitsPerPel == 16)
                    {
                    lpSurfaceGlobal->ddpfSurface.dwRBitMask = 0x00007C00;
                    lpSurfaceGlobal->ddpfSurface.dwGBitMask = 0x000003E0;
                    lpSurfaceGlobal->ddpfSurface.dwBBitMask = 0x0000001F;
                    }
                else if (ppdev->cBitsPerPel == 32)
                    {
                    lpSurfaceGlobal->ddpfSurface.dwRBitMask = 0x00FF0000;
                    lpSurfaceGlobal->ddpfSurface.dwGBitMask = 0x0000FF00;
                    lpSurfaceGlobal->ddpfSurface.dwBBitMask = 0x000000FF;
                    }
                lpSurfaceGlobal->ddpfSurface.dwRGBAlphaBitMask = 0;
#ifdef DEBUG
                fourCCStr[0] = (char)lpSurfaceGlobal->ddpfSurface.dwFourCC & 0xFF;
                fourCCStr[1] = (char)(lpSurfaceGlobal->ddpfSurface.dwFourCC >> 8) & 0xFF;
                fourCCStr[2] = (char)(lpSurfaceGlobal->ddpfSurface.dwFourCC >> 16) & 0xFF;
                fourCCStr[3] = (char)(lpSurfaceGlobal->ddpfSurface.dwFourCC >> 24) & 0xFF;
                fourCCStr[4] = 0;
                DPF( "FOURCC %s allocation: size=%lu bytes", &fourCCStr, dwBlockSize );
#endif // DEBUG
                lpSurfaceLocal->ddsCaps.dwCaps |= DDSCAPS_HWCODEC;

                lPitch = wWidth * (ppdev->cBitsPerPel >> 3);
                lPitch = (lPitch + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;

                if (lpSurfaceLocal->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)
                    {
                    NVHEAP_ALLOC(dwStatus,fpVidMem, lPitch * wHeight, TYPE_OVERLAY);
                    if (dwStatus!=0)
                    {
                        bMoveAllDfbsFromOffscreenToDibs(ppdev);
                        NVHEAP_ALLOC(dwStatus,fpVidMem, lPitch * wHeight, TYPE_OVERLAY);
                    }
                    if (dwStatus==0)
                        {
                        //**********************************************************************
                        // Flip surfaces, detected by surface requests that are the
                        // same size as the current display, have special
                        // considerations: they must live in the first four megabytes
                        // of video memory:
                        //**********************************************************************

                        lpSurfaceGlobal->yHint        = (fpVidMem / ppdev->lDelta);
                        lpSurfaceGlobal->xHint        = ((fpVidMem - (lpSurfaceGlobal->yHint * ppdev->lDelta))/ ppdev->cjPelSize) ;
                        lpSurfaceGlobal->fpVidMem     = fpVidMem;
                        lpSurfaceGlobal->lPitch       = lPitch;

                        lpSurfaceDesc->lPitch   = lPitch;
                        lpSurfaceDesc->dwFlags |= DDSD_PITCH;


                        //**********************************************************************
                        // We handled the creation entirely ourselves, so we have to
                        // set the return code and return DDHAL_DRIVER_HANDLED:
                        //**********************************************************************

                        lpCreateSurface->ddRVal = DD_OK;
                        lpSurfaceLocal->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
                        lpSurfaceLocal->ddsCaps.dwCaps |= DDSCAPS_LOCALVIDMEM;
                        break;
                        }
                    }

                //**********************************************************************
                // Tell DDRAW to allocate user mem for this surface.
                //**********************************************************************
                lpSurfaceGlobal->fpVidMem = DDHAL_PLEASEALLOC_USERMEM;
                lpSurfaceGlobal->dwUserMemSize = lPitch * wHeight;
                lpSurfaceDesc->lPitch = lPitch;
                lpSurfaceDesc->dwFlags  |= DDSD_PITCH;
                lpSurfaceLocal->ddsCaps.dwCaps &= ~DDSCAPS_LOCALVIDMEM;
                lpSurfaceLocal->ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

                return (DDHAL_DRIVER_NOTHANDLED);
                break;

            case FOURCC_IF09:
            case FOURCC_YVU9:
            case FOURCC_IV32:
            case FOURCC_IV31:

                if ((lpSurfaceLocal->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
                    (ppdev->dwOverlaySurfaces > 0))
                    {
                    if ((ppdev->dwOverlayOwner != (DWORD)powner) ||
                        (ppdev->dwOverlaySurfaces + 1 >= NV_MAX_OVERLAY_SURFACES))
                        {
                        lpCreateSurface->ddRVal = DDERR_OUTOFCAPS;
                        goto CREATE_HANDLED;
                        }
                    }

                /* First allocate enough video memory for YUY2 overlay pixel area */
                lPitch = ((lpSurfaceGlobal->wWidth + 1) & 0xFFFFFFFE) << 1;

                lPitch = (lPitch + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;
                /* pad with an extra scanline */
                dwBlockSize = (lPitch * (DWORD)lpSurfaceGlobal->wHeight);
                /* now allocate enough video memory for the Indeo part of the surface */
                lPitch = lpSurfaceGlobal->wWidth;
                /* allow extra room for block skip bits between Indeo portion of surface and YUY2 portion */
                dwBlockSize +=
                ((lPitch * ((DWORD)lpSurfaceGlobal->wHeight * 10L)) >> 3);
                /* Force block to be properly aligned */
                dwBlockSize =
                (dwBlockSize + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;

                lpSurfaceGlobal->ddpfSurface.dwYUVBitCount = 9;
                lpSurfaceGlobal->ddpfSurface.dwYBitMask = 0;
                lpSurfaceGlobal->ddpfSurface.dwUBitMask = 0;
                lpSurfaceGlobal->ddpfSurface.dwVBitMask = 0;
                lpSurfaceGlobal->ddpfSurface.dwYUVAlphaBitMask = 0;
#ifdef DEBUG
                fourCCStr[0] = (char)lpSurfaceGlobal->ddpfSurface.dwFourCC & 0xFF;
                fourCCStr[1] = (char)(lpSurfaceGlobal->ddpfSurface.dwFourCC >> 8) & 0xFF;
                fourCCStr[2] = (char)(lpSurfaceGlobal->ddpfSurface.dwFourCC >> 16) & 0xFF;
                fourCCStr[3] = (char)(lpSurfaceGlobal->ddpfSurface.dwFourCC >> 24) & 0xFF;
                fourCCStr[4] = 0;
                DPF( "FOURCC %s allocation: size=%lu bytes", &fourCCStr, dwBlockSize );
#endif // DEBUG

                lpSurfaceGlobal->lPitch = lPitch;
                lpSurfaceGlobal->fpVidMem = ppdev->ulPrimarySurfaceOffset;
                lpSurfaceGlobal->dwReserved1 = 0;
                lpSurfaceLocal->ddsCaps.dwCaps |= DDSCAPS_HWCODEC;

                if (lpSurfaceLocal->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)
                    {
                    NVHEAP_ALLOC(dwStatus,fpVidMem, dwBlockSize, TYPE_OVERLAY);
                    if (dwStatus!=0)
                    {
                        bMoveAllDfbsFromOffscreenToDibs(ppdev);
                        NVHEAP_ALLOC(dwStatus,fpVidMem, dwBlockSize, TYPE_OVERLAY);
                    }

                    if (dwStatus==0)
                        {
                        ppdev->DDrawVideoSurfaceCount++;
                        lpSurfaceGlobal->fpVidMem = fpVidMem;
                        lpSurfaceLocal->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
                        lpSurfaceLocal->ddsCaps.dwCaps |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);

                        if (lpSurfaceLocal->ddsCaps.dwCaps & DDSCAPS_OVERLAY)
                            {
                            index = 0;
                            while ((ppdev->dwOverlaySurfaceLCL[index] != 0) &&
                                   (index < NV_MAX_OVERLAY_SURFACES))
                                index++;
                            ppdev->dwOverlaySurfaceLCL[index] = (DWORD) lpSurfaceLocal;
                            ppdev->dwOverlayOwner = (DWORD)powner;
                            ppdev->dwOverlaySurfaces++;
                            }

                        lpCreateSurface->ddRVal = DD_OK;
                        goto CREATE_HANDLED;
                        }
                    else
                        {
                        /* We can't support overlay surfaces in system memory */
                        if (lpSurfaceLocal->ddsCaps.dwCaps & DDSCAPS_OVERLAY)
                            {
                            return DDHAL_DRIVER_NOTHANDLED;
                            }
                        }
                    }

                //**********************************************************************
                // Tell DDRAW to allocate user mem for this surface.
                //**********************************************************************
                lpSurfaceGlobal->fpVidMem = DDHAL_PLEASEALLOC_USERMEM;
                lpSurfaceGlobal->dwUserMemSize = dwBlockSize;
                lpSurfaceDesc->lPitch = lPitch;
                lpSurfaceLocal->ddsCaps.dwCaps &= ~(DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
                lpSurfaceLocal->ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

                return DDHAL_DRIVER_NOTHANDLED;
                break;

            case FOURCC_YUY2:
            case FOURCC_UYVY:

                if ((lpSurfaceLocal->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
                    (ppdev->dwOverlaySurfaces > 0))
                    {
                    if ((ppdev->dwOverlayOwner != (DWORD)powner) ||
                        (ppdev->dwOverlaySurfaces + 1 >= NV_MAX_OVERLAY_SURFACES))
                        {
                        lpCreateSurface->ddRVal = DDERR_OUTOFCAPS;
                        goto CREATE_HANDLED;
                        }
                    }

                /* First try video memory */
                lPitch = ((lpSurfaceGlobal->wWidth + 1) & 0xFFFFFFFE) << 1;
                lPitch = (lPitch + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;
                /* pad with an extra scanline */
                dwBlockSize = (lPitch * (DWORD)lpSurfaceGlobal->wHeight);
                lpSurfaceGlobal->ddpfSurface.dwYUVBitCount = 16;
                if (lpSurfaceGlobal->ddpfSurface.dwFourCC == FOURCC_YUY2)
                    {
                    lpSurfaceGlobal->ddpfSurface.dwYBitMask = 0x00FF00FF;
                    lpSurfaceGlobal->ddpfSurface.dwUBitMask = 0x0000FF00;
                    lpSurfaceGlobal->ddpfSurface.dwVBitMask = 0xFF000000;
                    }
                else
                    {
                    lpSurfaceGlobal->ddpfSurface.dwYBitMask = 0xFF00FF00;
                    lpSurfaceGlobal->ddpfSurface.dwUBitMask = 0x000000FF;
                    lpSurfaceGlobal->ddpfSurface.dwVBitMask = 0x00FF0000;
                    }
                lpSurfaceGlobal->ddpfSurface.dwYUVAlphaBitMask = 0;
#ifdef DEBUG
                fourCCStr[0] = (char)lpSurfaceGlobal->ddpfSurface.dwFourCC & 0xFF;
                fourCCStr[1] = (char)(lpSurfaceGlobal->ddpfSurface.dwFourCC >> 8) & 0xFF;
                fourCCStr[2] = (char)(lpSurfaceGlobal->ddpfSurface.dwFourCC >> 16) & 0xFF;
                fourCCStr[3] = (char)(lpSurfaceGlobal->ddpfSurface.dwFourCC >> 24) & 0xFF;
                fourCCStr[4] = 0;
                DPF( "FOURCC %s allocation: size=%lu bytes", &fourCCStr, dwBlockSize );
#endif // DEBUG
                lpSurfaceGlobal->lPitch = lPitch;
                lpSurfaceDesc->dwFlags |= DDSD_PITCH;
                lpSurfaceLocal->ddsCaps.dwCaps |= DDSCAPS_HWCODEC;

                NVHEAP_ALLOC(dwStatus,fpVidMem, dwBlockSize, TYPE_OVERLAY);
                if (dwStatus!=0)
                {
                    bMoveAllDfbsFromOffscreenToDibs(ppdev);
                    NVHEAP_ALLOC(dwStatus,fpVidMem, dwBlockSize, TYPE_OVERLAY);
                }


                if (dwStatus==0)
                    {
                    ppdev->DDrawVideoSurfaceCount++;
                    lpSurfaceLocal->ddsCaps.dwCaps |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
                    lpSurfaceLocal->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
                    lpSurfaceGlobal->fpVidMem = fpVidMem;
                    lpCreateSurface->ddRVal = DD_OK;
                    if (lpSurfaceLocal->ddsCaps.dwCaps & DDSCAPS_OVERLAY)
                        {
                        index = 0;
                        while ((ppdev->dwOverlaySurfaceLCL[index] != 0) &&
                               (index < NV_MAX_OVERLAY_SURFACES))
                            index++;
                        ppdev->dwOverlaySurfaceLCL[index] = (DWORD) lpSurfaceLocal;
                        ppdev->dwOverlayOwner = (DWORD)powner;
                        ppdev->dwOverlaySurfaces++;
                        }

                    }
                else
                    {
                    /* We can't support overlay surfaces in system memory */
                    if (lpSurfaceLocal->ddsCaps.dwCaps & DDSCAPS_OVERLAY)
                        {
                        lpCreateSurface->ddRVal = DDERR_OUTOFMEMORY;
                        goto CREATE_HANDLED;
                        }
                    //**********************************************************************
                    // Tell DDRAW to allocate user mem for this surface.
                    //**********************************************************************
                    lpSurfaceGlobal->fpVidMem = DDHAL_PLEASEALLOC_USERMEM;
                    lpSurfaceGlobal->dwUserMemSize = dwBlockSize;
                    lpSurfaceDesc->lPitch = lPitch;
                    lpSurfaceDesc->dwFlags  |= DDSD_PITCH;
                    lpSurfaceLocal->ddsCaps.dwCaps &= ~(DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
                    lpSurfaceLocal->ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

                    return DDHAL_DRIVER_NOTHANDLED;
                    }
                goto CREATE_HANDLED;
                break;

            case FOURCC_YV12:
            case FOURCC_420i:

                if ((lpSurfaceLocal->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
                    (ppdev->dwOverlaySurfaces > 0))
                    {
                    if ((ppdev->dwOverlayOwner != (DWORD)powner) ||
                        (ppdev->dwOverlaySurfaces + 1 >= NV_MAX_OVERLAY_SURFACES))
                        {
                        lpCreateSurface->ddRVal = DDERR_OUTOFCAPS;
                        goto CREATE_HANDLED;
                        }
                    }

                /*
                 * Always split allocation of YV12/420i overlay surfaces.
                 *   Allocate the YV12/420i portion in AGP memory on AGP systems, local video memory on PCI systems.
                 *   Fall back to system memory only if preferred allocation fails.
                 *   Allocate additional YUY2 surfaces in local video memory for the video scaler to use
                 */
                lpSurfaceGlobal->ddpfSurface.dwYUVBitCount = 12;
                lpSurfaceGlobal->ddpfSurface.dwYBitMask = 0;
                lpSurfaceGlobal->ddpfSurface.dwUBitMask = 0;
                lpSurfaceGlobal->ddpfSurface.dwVBitMask = 0;
                lpSurfaceGlobal->ddpfSurface.dwYUVAlphaBitMask = 0;

                lpSurfaceGlobal->fpVidMem = ppdev->ulPrimarySurfaceOffset;
                lpSurfaceGlobal->dwReserved1 = 0;
                lpSurfaceLocal->ddsCaps.dwCaps |= DDSCAPS_HWCODEC;

                lPitch = ((lpSurfaceGlobal->wWidth + 1) & 0xFFFFFFFE) << 1;
                lPitch = (lPitch + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;

                adjustedHeight = (DWORD)((lpSurfaceGlobal->wHeight + 1) & ~1);

                dwBlockSize = lPitch * adjustedHeight;

                if (lpSurfaceLocal->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)
                    {
                    NVHEAP_ALLOC(dwStatus,fpVidMem, dwBlockSize, TYPE_OVERLAY);
                    if (dwStatus!=0)
                    {
                        bMoveAllDfbsFromOffscreenToDibs(ppdev);
                        NVHEAP_ALLOC(dwStatus,fpVidMem, dwBlockSize, TYPE_OVERLAY);
                    }

                    if (dwStatus==0)
                        {
                        ppdev->DDrawVideoSurfaceCount++;
                        lpSurfaceLocal->ddsCaps.dwCaps |= DDSCAPS_LOCALVIDMEM;
                        if (ppdev->NvFloatingMem2MemFormatBaseAddress == 0)
                            ppdev->NvYUY2Surface0Mem = (DWORD)fpVidMem; /* save pointer to YUY2 overlay surface */
                        else if (ppdev->NvFloatingMem2MemFormatNextAddress == 0)
                            ppdev->NvYUY2Surface1Mem = (DWORD)fpVidMem;
                        else
                            ppdev->NvYUY2Surface2Mem = (DWORD)fpVidMem;
                        }
                    else
                        {
                        /* one or more overlay surfaces could not be allocated, clean up already allocated memory */
                        if (ppdev->NvYUY2Surface1Mem > 0)
                            {
                            NVHEAP_FREE(ppdev->NvYUY2Surface1Mem);
                            ppdev->NvYUY2Surface1Mem = 0;
                            ppdev->NvFloatingMem2MemFormatNextAddress = 0;
                            if (ppdev->DDrawVideoSurfaceCount > 0)
                                ppdev->DDrawVideoSurfaceCount--;
                            }

                        if (ppdev->NvFloatingMem2MemFormatBaseAddress != 0)
                            {
                            if (ppdev->NvYUY2Surface0Mem > 0)
                                {
                                NVHEAP_FREE(ppdev->NvYUY2Surface0Mem);
                                ppdev->NvYUY2Surface0Mem = 0;
                                if (ppdev->DDrawVideoSurfaceCount > 0)
                                    ppdev->DDrawVideoSurfaceCount--;
                                }
#ifdef SYSMEM_FALLBACK
                            if ((ppdev->NvFloatingMem2MemFormatBaseAddress < (ULONG) ppdev->pjFrameBufbase) || /* If system memory */
                                (ppdev->NvFloatingMem2MemFormatBaseAddress > (ULONG) ppdev->pjFrameBufbase + 0x1000000))
                                {
                                if (NvFree(ppdev->hDriver,
                                                     ppdev->hClient,
                                                     ppdev->hDevice,
                                                     NV_DD_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY)
                                    != NVOS00_STATUS_SUCCESS)
                                /*
                                if (NvFreeContextDma(ppdev->hDriver,
                                                     NV_DD_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY)
                                    != NV0003_ERROR_OK)
                                */
                                    {
                                    DISPDBG((2, "Couldn't free dma context in sys mem"));
                                    lpCreateSurface->ddRVal = DDERR_OUTOFMEMORY;
                                    goto CREATE_HANDLED;
                                    }
                                }
                            if ((ppdev->NvFloatingMem2MemFormatBaseAddress < (ULONG) ppdev->pjFrameBufbase) || /* If system memory */
                                (ppdev->NvFloatingMem2MemFormatBaseAddress > (ULONG) ppdev->pjFrameBufbase + 0x1000000))
                                {
                                // BUGBUG...
                                //GlobalUnlock((HGLOBAL)ppdev->NvFloatingMem2MemFormatMemoryHandle);
                                //GlobalFree((HGLOBAL)ppdev->NvFloatingMem2MemFormatMemoryHandle);
                                }
                            else
#endif // #ifdef SYSMEM_FALLBACK
                                {
                                NVHEAP_FREE(ppdev->NvFloatingMem2MemFormatBaseAddress);
                                if (ppdev->DDrawVideoSurfaceCount > 0)
                                    ppdev->DDrawVideoSurfaceCount--;
                                }

                            ppdev->NvFloatingMem2MemFormatBaseAddress = 0;
                            ppdev->NvFloatingMem2MemFormatNextAddress = 0;
                            }

                        ppdev->dwOverlayOwner = 0;
                        ppdev->dwOverlaySurfaces = 0;

                        /* We can't support overlay surfaces in system memory */
                        if (lpSurfaceLocal->ddsCaps.dwCaps & DDSCAPS_OVERLAY)
                            {
                            lpCreateSurface->ddRVal = DDERR_OUTOFMEMORY;
                            goto CREATE_HANDLED;
                            }
                        return(DDHAL_DRIVER_NOTHANDLED);
                        }
                    }
#ifdef DEBUG
                fourCCStr[0] = (char)lpSurfaceGlobal->ddpfSurface.dwFourCC & 0xFF;
                fourCCStr[1] = (char)(lpSurfaceGlobal->ddpfSurface.dwFourCC >> 8) & 0xFF;
                fourCCStr[2] = (char)(lpSurfaceGlobal->ddpfSurface.dwFourCC >> 16) & 0xFF;
                fourCCStr[3] = (char)(lpSurfaceGlobal->ddpfSurface.dwFourCC >> 24) & 0xFF;
                fourCCStr[4] = 0;
                DPF( "FOURCC %s allocation: size=%lu bytes", &fourCCStr, dwBlockSize );
#endif // DEBUG
                /* now allocate enough AGP memory or local video memory for the YV12 part of the surface */
                lPitch = lpSurfaceGlobal->wWidth;
                lpSurfaceGlobal->lPitch = lPitch;

                dwBlockSize = ((lPitch * (adjustedHeight * 12L)) >> 3);
                /* Force block to be properly aligned */
                lPitch = (lPitch + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;

                if (ppdev->NvFloatingMem2MemFormatBaseAddress == 0)
                    { /* If this is the first of a complex overlay surface */

                    /*
                     * Allocate enough AGP, video, or system memory for ALL of the surfaces in the
                     * complex surface in a single block
                     */

                    if (lpCreateSurface->dwSCnt > 2)
                        dwBlockSize *= (DWORD)lpCreateSurface->dwSCnt;
                    else /* Assume that 2 of these surfaces will be created */
                        dwBlockSize <<= 1;

                    fpVidMem = 0;

                    memType = 0;

                    NVHEAP_ALLOC(dwStatus,fpVidMem, dwBlockSize, TYPE_OVERLAY);
                    if (dwStatus!=0)
                    {
                        bMoveAllDfbsFromOffscreenToDibs(ppdev);
                        NVHEAP_ALLOC(dwStatus,fpVidMem, dwBlockSize, TYPE_OVERLAY);
                    }

                    if (dwStatus==0)
                        {
                        memType = DDSCAPS_LOCALVIDMEM;
                        ppdev->DDrawVideoSurfaceCount++;
                        lpSurfaceGlobal->fpVidMem = fpVidMem;
                        ppdev->NvFloatingMem2MemFormatMemoryHandle = 0;
                        }

                    if (dwStatus!=0)
                        { /* Finally try system memory */
#ifdef SYSMEM_FALLBACK
                        lpSurfaceGlobal->fpVidMem = DDHAL_PLEASEALLOC_USERMEM;
                        lpSurfaceGlobal->dwUserMemSize = dwBlockSize;
                        lpSurfaceDesc->lPitch = lPitch;
                        lpSurfaceDesc->dwFlags  |= DDSD_PITCH;
                        lpSurfaceLocal->ddsCaps.dwCaps &= ~DDSCAPS_LOCALVIDMEM;

                        lpSurfaceLocal->ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
#endif
                        return (DDHAL_DRIVER_NOTHANDLED);
                        }

#ifdef SYSMEM_FALLBACK
                    DPF( "    System memory FOURCC surface allocation succeeded" );
                    if ((memType == DDSCAPS_NONLOCALVIDMEM) || (memType == DDSCAPS_SYSTEMMEMORY))
                        { /* If system memory */

                        if (NvAllocContextDma(ppdev->hDriver,
                                              ppdev->hClient,
                                              NV_DD_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY,
                                              NV01_CONTEXT_DMA,
                                              NVOS03_FLAGS_ACCESS_READ_ONLY,
                                              (PVOID) lpSurfaceGlobal->fpVidMem,
                                              dwBlockSize - 1
                                              ) != NVOS03_STATUS_SUCCESS)
                        /*
                        if (NvAllocContextDma(ppdev->hDriver,
                                              NV_DD_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY,
                                              NV_CONTEXT_DMA_FROM_MEMORY,
                                              (PVOID) lpSurfaceGlobal->fpVidMem,
                                              dwBlockSize - 1,
                                              0) != NV0002_ERROR_OK)
                        */
                            {
                            DISPDBG((2, "DdCreateSurface: cannot create dma context in sys mem"));
                            /* Can't lock down system memory, forget using this technique */
                            ppdev->NvFloatingMem2MemFormatBaseAddress = 0;
                            /* Free video memory just allocated above */
                            NVHEAP_FREE(ppdev->NvYUY2Surface0Mem);
                            if (ppdev->DDrawVideoSurfaceCount > 0)
                                ppdev->DDrawVideoSurfaceCount--;

                            /* Free AGP memory just allocated above */
                            NVHEAP_FREE(lpSurfaceGlobal->fpVidMem);
                            lpSurfaceGlobal->fpVidMem = 0;
                            lpCreateSurface->ddRVal = DDERR_OUTOFMEMORY;
                            goto CREATE_HANDLED;
                            }
                        }
#endif // #ifdef SYSMEM_FALLBACK

                    ppdev->NvFloatingMem2MemFormatBaseAddress =
                        (DWORD)lpSurfaceGlobal->fpVidMem;
                    }
                else
                    {
                    lpSurfaceGlobal->fpVidMem = ppdev->NvFloatingMem2MemFormatNextAddress =
                        ppdev->NvFloatingMem2MemFormatBaseAddress + dwBlockSize;
                    }

                if (lpSurfaceLocal->ddsCaps.dwCaps & DDSCAPS_OVERLAY)
                    {
                    index = 0;
                    while ((ppdev->dwOverlaySurfaceLCL[index] != 0) &&
                           (index < NV_MAX_OVERLAY_SURFACES))
                        index++;
                    ppdev->dwOverlaySurfaceLCL[index] = (DWORD) lpSurfaceLocal;
                    ppdev->dwOverlayOwner = (DWORD)powner;
                    ppdev->dwOverlaySurfaces++;
                    }
            }
        }
    else
        {
        //**************************************************************************
        // Allocate a space in off-screen memory, using our own heap
        // manager:
        //**************************************************************************
        lPitch = wWidth * ppdev->cjPelSize;
        lPitch = (lPitch + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;
        if (lpSurfaceLocal->ddsCaps.dwCaps & (DDSCAPS_BACKBUFFER | DDSCAPS_FLIP))
            {
            NVHEAP_ALLOC_TILED(dwStatus,fpVidMem, lPitch, wHeight, TYPE_IMAGE);
            }
        else
            {
            NVHEAP_ALLOC(dwStatus,fpVidMem, lPitch * wHeight, TYPE_IMAGE);
            }
        if (dwStatus!=0)
        {
            bMoveAllDfbsFromOffscreenToDibs(ppdev);
            lPitch = (lPitch + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;
            if (lpSurfaceLocal->ddsCaps.dwCaps & (DDSCAPS_BACKBUFFER | DDSCAPS_FLIP))
                {
                NVHEAP_ALLOC_TILED(dwStatus,fpVidMem, lPitch, wHeight, TYPE_IMAGE);
                }
            else
                {
                NVHEAP_ALLOC(dwStatus,fpVidMem, lPitch * wHeight, TYPE_IMAGE)
                }
        }
        if (dwStatus==0)
            {
            ppdev->DDrawVideoSurfaceCount++;

            //**********************************************************************
            // Flip surfaces, detected by surface requests that are the
            // same size as the current display, have special
            // considerations: they must live in the first four megabytes
            // of video memory:
            //**********************************************************************

            lpSurfaceGlobal->yHint        = (fpVidMem / ppdev->lDelta);
            lpSurfaceGlobal->xHint        = ((fpVidMem - (lpSurfaceGlobal->yHint * ppdev->lDelta))/ ppdev->cjPelSize) ;
            lpSurfaceGlobal->fpVidMem     = fpVidMem;
            lpSurfaceGlobal->lPitch       = lPitch;

            lpSurfaceDesc->lPitch   = lPitch;
            lpSurfaceDesc->dwFlags |= DDSD_PITCH;

            lpSurfaceLocal->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
            lpSurfaceLocal->ddsCaps.dwCaps |= DDSCAPS_LOCALVIDMEM;

            //**********************************************************************
            // We handled the creation entirely ourselves, so we have to
            // set the return code and goto CREATE_HANDLED:
            //**********************************************************************

            lpCreateSurface->ddRVal = DD_OK;
            goto CREATE_HANDLED;
            }
        else
            {
            return (DDHAL_DRIVER_NOTHANDLED);
            }
        }
    //**********************************************************************
    // We handled the creation entirely ourselves, so we have to
    // set the return code and goto CREATE_HANDLED:
    //**********************************************************************
    lpCreateSurface->ddRVal = DD_OK;

CREATE_HANDLED:
    return DDHAL_DRIVER_HANDLED;

    }
#endif // IS_WINNT4

/******************************Public*Routine**********************************\
* DWORD DdSetColorKey
*
\******************************************************************************/
DWORD __stdcall DdSetColorKey(PDD_SETCOLORKEYDATA lpSetColorKey)
    {
    PDD_DIRECTDRAW_GLOBAL pdrv;
    PDD_SURFACE_LOCAL     psurf;
    PDEV                 *ppdev;

    pdrv = lpSetColorKey->lpDD;
    psurf = lpSetColorKey->lpDDSurface;
    ppdev = (PDEV *) lpSetColorKey->lpDD->dhpdev;

    DPF( "in SetSurfaceColorKey, lpSetColorKey->lpDD=%08lx, lpSetColorKey->lpDDSurface = %08lx",
         pdrv, lpSetColorKey->lpDDSurface );

    if (lpSetColorKey->dwFlags == DDCKEY_SRCBLT)
        {
        psurf->ddckCKSrcOverlay.dwColorSpaceLowValue =
        lpSetColorKey->ckNew.dwColorSpaceLowValue & ppdev->physicalColorMask;
        /* Use no mask on this one on NV3 so that our D3D driver can get all the bits when surface is 565 */
        psurf->ddckCKSrcOverlay.dwColorSpaceHighValue =
        lpSetColorKey->ckNew.dwColorSpaceHighValue & ppdev->physicalColorMask;
        }
    else if (lpSetColorKey->dwFlags == DDCKEY_DESTOVERLAY)
        {
        psurf->ddckCKDestOverlay.dwColorSpaceLowValue =
        lpSetColorKey->ckNew.dwColorSpaceLowValue & ppdev->physicalColorMask;
        psurf->ddckCKDestOverlay.dwColorSpaceHighValue =
        lpSetColorKey->ckNew.dwColorSpaceHighValue & ppdev->physicalColorMask;
        }
    else
        {
        lpSetColorKey->ddRVal = DDERR_UNSUPPORTED;
        return DDHAL_DRIVER_HANDLED;
        }


    lpSetColorKey->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
    }

#if (_WIN32_WINNT >= 0x0500) && defined(NVD3D) && !defined(_WIN64)
//WINNT 5 version, use common code to build caps. If not D3D use WINNT 4 version
//note that both have to build with NV3/NV4
BOOL DrvGetDirectDrawInfo(
                         DHPDEV          dhpdev,
                         DD_HALINFO*     pHalInfo,
                         DWORD*          pdwNumHeaps,
                         VIDEOMEMORY*    pvmList,            // Will be NULL on first call
                         DWORD*          pdwNumFourCC,
                         DWORD*          pdwFourCC)          // Will be NULL on first call
    {
    PDEV*       ppdev;
    LONGLONG    li;
    DWORD       i=0;

    ppdev = (PDEV*) dhpdev;

    //**************************************************************************
    // When enabling Direct Draw, DrvGetDirectDrawInfo will get called twice.
    //
    // The first call will have (pvmList = NULL) and (pdwFourCC = NULL)
    // During this time, the driver needs to returns the size of the memory
    // heap and the number of FourCC's that the driver supports.
    //
    // During the second call, the driver should initialize pdwNumHeaps,
    // pvmList, pdwNumFourCC, and pdwFourCC based on the values returned
    // in the first call.
    //
    //**************************************************************************

    //************************************************************************
    // BUGBUG - temp hack til we get overlay conversion stuff working.
    //************************************************************************

#ifdef YUNV_UYNV
    *pdwNumFourCC = 10;
#else // YUNV_UYNV
    *pdwNumFourCC = 8;
#endif// YUNV_UYNV

#if !defined(DX_AGP)
    *pdwNumHeaps = 0;
#else
    *pdwNumHeaps = 1;       // AGP memory heap
#endif // #if _WIN32_WINNT >= 0x0500

    //**************************************************************************
    // Verify that we enabled Direct Draw in bEnableDirectDraw
    //**************************************************************************

    if (!(ppdev->flStatus & STAT_DIRECTDRAW_CAPABLE))
    {
        return (FALSE);
    }

    if (pvmList == NULL && pdwFourCC == NULL)
    {
        return (TRUE);
    }

    buildDDHALInfo32(ppdev->pDriverData);
    *pHalInfo = ppdev->pDriverData->HALInfo;

    pHalInfo->dwSize = sizeof(*pHalInfo);

    //**************************************************************************
    // Current primary surface attributes.  Since HalInfo is zero-initialized
    // by GDI, we only have to fill in the fields which should be non-zero:
    // First fill out VideoMemoryInformation
    //**************************************************************************

    pHalInfo->vmiData.pvPrimary       = ppdev->pjScreen;
    pHalInfo->vmiData.fpPrimary       = ppdev->ulPrimarySurfaceOffset;
    pHalInfo->vmiData.dwDisplayWidth  = ppdev->cxScreen;
    pHalInfo->vmiData.dwDisplayHeight = ppdev->cyScreen;
    pHalInfo->vmiData.lDisplayPitch   = ppdev->lDelta;

    pHalInfo->vmiData.ddpfDisplay.dwSize  = sizeof(DDPIXELFORMAT);
    pHalInfo->vmiData.ddpfDisplay.dwFlags = DDPF_RGB;
    pHalInfo->vmiData.ddpfDisplay.dwRGBBitCount = ppdev->cBitsPerPel;

    if (ppdev->iBitmapFormat == BMF_8BPP)
        {
        pHalInfo->vmiData.ddpfDisplay.dwFlags |= DDPF_PALETTEINDEXED8;
        }

    pHalInfo->vmiData.ddpfDisplay.dwRBitMask = ppdev->flRed;
    pHalInfo->vmiData.ddpfDisplay.dwGBitMask = ppdev->flGreen;
    pHalInfo->vmiData.ddpfDisplay.dwBBitMask = ppdev->flBlue;

#if defined(NV4) && defined(DX_AGP)
    //**************************************************************
    // AGP support is Win2k specific.
    //**************************************************************

    {
#if (_WIN32_WINNT >= 0x0500)
        // Ask miniport if AGP should be enabled.
        // Miniport has knowledge of AGP chipsets for which we should disable AGP functionality.
        // e.g. ALi1541 chipset

        ULONG ulReturn = 0;

        if (EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_SHOULD_AGP_BE_ENABLED,
                               NULL, 0, NULL, 0, &ulReturn))
        {
            // non NO_ERROR returned
            ppdev->bAgp = FALSE;
        }
        else
        {
            // NO_ERROR returned
            ppdev->bAgp = TRUE;
        }
#else
        ULONG cbLimit = 0xfff;

        if (NvAllocMemory(ppdev->hDriver,
            ppdev->hClient,
            ppdev->hDevice,
            NV_AGP_PUSHBUF_OBJECT_HANDLE,
            NV01_MEMORY_SYSTEM,
            (DRF_DEF(OS02, _FLAGS, _LOCATION, _AGP) | DRF_DEF(OS02, _FLAGS, _COHERENCY, _WRITE_COMBINE)),
            (PVOID *)&(ppdev->AgpHeapBase),
            &cbLimit))
        {
            ppdev->bAgp = FALSE;
        }
        else
        {
            ppdev->bAgp = TRUE;
            NvFree(ppdev->hDriver, ppdev->hClient, ppdev->hDevice, NV_AGP_PUSHBUF_OBJECT_HANDLE);
        }
#endif
    }

    if (ppdev->bAgp)
    {
        // get the AGP aperture size and store in pDriverdata->regMaxAGPLimit;
        NvWin2KGetAgpLimit(ppdev, ppdev->hClient, ppdev->hDevice);

        pHalInfo->ddCaps.ddsCaps.dwCaps |= DDSCAPS_NONLOCALVIDMEM;
        pHalInfo->ddCaps.dwCaps2        |= DDCAPS2_NONLOCALVIDMEMCAPS | DDCAPS2_NONLOCALVIDMEM;
        pHalInfo->ddCaps.dwVidMemTotal   = ppdev->pDriverData->regMaxAGPLimit + 1;

        NvWin2KpvmConfig(ppdev, pvmList);

        ppdev->AgpHeap = pvmList;
        ppdev->AgpHeapAlignment.Linear.dwStartAlignment = 256;
        ppdev->AgpHeapAlignment.Linear.dwPitchAlignment = 256;
    }
    else
#endif
    {
        pHalInfo->ddCaps.dwCaps2 &= ~DDCAPS2_NONLOCALVIDMEM;
        pHalInfo->ddCaps.dwVidMemTotal = ppdev->cbFrameBuf - (ppdev->cyScreen * ppdev->lDelta);
    }
    //**************************************************************************
    // Since we do our own memory allocation, we have to set dwVidMemTotal
    // ourselves.  Note that this represents the amount of available off-
    // screen memory, not all of video memory:
    //**************************************************************************

    //**************************************************************************
    // Dword alignment must be guaranteed for off-screen surfaces:
    //**************************************************************************
    pHalInfo->vmiData.dwOffscreenAlign = ppdev->ulSurfaceAlign + 1;
    pHalInfo->vmiData.dwTextureAlign   = ppdev->ulSurfaceAlign + 1;
    pHalInfo->vmiData.dwAlphaAlign     = ppdev->ulSurfaceAlign + 1;
    pHalInfo->vmiData.dwZBufferAlign   = ppdev->ulSurfaceAlign + 1;
    pHalInfo->vmiData.dwOverlayAlign   = ppdev->ulSurfaceAlign + 1;

    pdwFourCC[i] = DefaultGlobalData.fourCC[i++] = FOURCC_UYVY;
    pdwFourCC[i] = DefaultGlobalData.fourCC[i++] = FOURCC_YUY2;
    pdwFourCC[i] = DefaultGlobalData.fourCC[i++] = FOURCC_RAW8;
#ifdef YUNV_UYNV
    pdwFourCC[i] = DefaultGlobalData.fourCC[i++] = FOURCC_YUNV;
    pdwFourCC[i] = DefaultGlobalData.fourCC[i++] = FOURCC_UYNV;
#endif
    pdwFourCC[i] = DefaultGlobalData.fourCC[i++] = FOURCC_YV12;

    pdwFourCC[i] = DefaultGlobalData.fourCC[i++] = FOURCC_YVU9;
    pdwFourCC[i] = DefaultGlobalData.fourCC[i++] = FOURCC_IV32;
    pdwFourCC[i] = DefaultGlobalData.fourCC[i++] = FOURCC_IV31;
    pdwFourCC[i] = DefaultGlobalData.fourCC[i++] = FOURCC_IF09;

    // The first call will have (pvmList = NULL) and (pdwFourCC = NULL)
    // During this time, the driver needs to returns the size of the memory
    // heap and the number of FourCC's that the driver supports.
    DISPDBG((2,"NV3:DrvEnableDirectDraw: Set D3D caps"));
#if defined(NV4)
#ifndef _WIN64
    NvWin2KD3DHALCreateDriver(pHalInfo, ppdev);
#endif // D3D is not 64 bit code clean , skip for now...

    /*
     * Force reset of D3D rendering target and z-buffer.
     */
    ppdev->pDriverData->HALInfo = *pHalInfo;

    ppdev->pDriverData->dwFullScreenDOSStatus   = 0;
    ppdev->pDriverData->dwCurrentContextHandle  = 0;
    ppdev->pDriverData->bDirtyRenderTarget      = TRUE;

    ppdev->pDriverData->vpp.dwOverlayFSOvlHeadSaved = 0xFFFFFFFF;
    ppdev->pDriverData->vpp.dwOverlayFSOvlHead = 0xFFFFFFFF;
#else
    D3DHALCreateDriver((LPD3DHAL_GLOBALDRIVERDATA *)&pHalInfo->lpD3DGlobalDriverData,
                       (LPD3DHAL_CALLBACKS *)&pHalInfo->lpD3DHALCallbacks);
    pDriverData->HALInfo = *pHalInfo;
    /*
     * Force reset of D3D rendering target and z-buffer.
     */
    pDriverData->fFullScreenDosOccurred  = 0;
    pDriverData->lpLast3DSurfaceRendered = 0;
    pDriverData->dwCurrentContextHandle  = 0;

    pDriverData->vpp.dwOverlayFSOvlHeadSaved = 0xFFFFFFFF;
    pDriverData->vpp.dwOverlayFSOvlHead = 0xFFFFFFFF;

#endif // #ifdef NV4
    return (TRUE);
    }

#else
//
// WINNT 4 version -- no Direct3D
//
BOOL DrvGetDirectDrawInfo(
                         DHPDEV          dhpdev,
                         DD_HALINFO*     pHalInfo,
                         DWORD*          pdwNumHeaps,
                         VIDEOMEMORY*    pvmList,            // Will be NULL on first call
                         DWORD*          pdwNumFourCC,
                         DWORD*          pdwFourCC)          // Will be NULL on first call
{
    PDEV*       ppdev;
    LONGLONG    li;

    ppdev = (PDEV*) dhpdev;

    //**************************************************************************
    // When enabling Direct Draw, DrvGetDirectDrawInfo will get called twice.
    //
    // The first call will have (pvmList = NULL) and (pdwFourCC = NULL)
    // During this time, the driver needs to returns the size of the memory
    // heap and the number of FourCC's that the driver supports.
    //
    // During the second call, the driver should initialize pdwNumHeaps,
    // pvmList, pdwNumFourCC, and pdwFourCC based on the values returned
    // in the first call.
    //
    //**************************************************************************
    *pdwNumFourCC = NV_MAX_FOURCC;
    *pdwNumHeaps = 0;
    if (!ppdev->bEnableIF09)
    {
        //***********************************************************************
        // Don't report support for the IF09 FOURCC. The default Indeo codec
        // shipping with NT 4 doesn't decode IF09's properly, so we need to punt them
        // to sw. NOTE: WE ASSUME IF09 IS THE LAST CODEC in the FOURCC LIST!
        //************************************************************************
        (*pdwNumFourCC)--;
    }

    //**************************************************************************
    // Verify that we enabled Direct Draw in bEnableDirectDraw
    //**************************************************************************

    if (!(ppdev->flStatus & STAT_DIRECTDRAW_CAPABLE))
        return (FALSE);

    pHalInfo->dwSize = sizeof(*pHalInfo);

    //**************************************************************************
    // Current primary surface attributes.  Since HalInfo is zero-initialized
    // by GDI, we only have to fill in the fields which should be non-zero:
    // First fill out VideoMemoryInformation
    //**************************************************************************

    pHalInfo->vmiData.pvPrimary       = ppdev->pjScreen;
    pHalInfo->vmiData.fpPrimary       = ppdev->ulPrimarySurfaceOffset;
    pHalInfo->vmiData.dwDisplayWidth  = ppdev->cxScreen;
    pHalInfo->vmiData.dwDisplayHeight = ppdev->cyScreen;
    pHalInfo->vmiData.lDisplayPitch   = ppdev->lDelta;

    pHalInfo->vmiData.ddpfDisplay.dwSize  = sizeof(DDPIXELFORMAT);
    pHalInfo->vmiData.ddpfDisplay.dwFlags = DDPF_RGB;
    pHalInfo->vmiData.ddpfDisplay.dwRGBBitCount = ppdev->cBitsPerPel;

    if (ppdev->iBitmapFormat == BMF_8BPP)
    {
        pHalInfo->vmiData.ddpfDisplay.dwFlags |= DDPF_PALETTEINDEXED8;
    }

    pHalInfo->vmiData.ddpfDisplay.dwRBitMask = ppdev->flRed;
    pHalInfo->vmiData.ddpfDisplay.dwGBitMask = ppdev->flGreen;
    pHalInfo->vmiData.ddpfDisplay.dwBBitMask = ppdev->flBlue;

    //**************************************************************************
    // Since we do our own memory allocation, we have to set dwVidMemTotal
    // ourselves.  Note that this represents the amount of available off-
    // screen memory, not all of video memory:
    //**************************************************************************

    pHalInfo->ddCaps.dwVidMemTotal = ppdev->cbFrameBuf - (ppdev->cyScreen * ppdev->lDelta);

    //**************************************************************************
    // Dword alignment must be guaranteed for off-screen surfaces:
    //**************************************************************************

    pHalInfo->vmiData.dwOffscreenAlign = ppdev->ulSurfaceAlign + 1;

    //**************************************************************************
    // Capabilities supported:
    //**************************************************************************

    pHalInfo->ddCaps.dwCaps = DDCAPS_BLT
                              | DDCAPS_BLTCOLORFILL
                              | DDCAPS_COLORKEY
                              | DDCAPS_READSCANLINE
                              | DDCAPS_BLTFOURCC
                              | DDCAPS_OVERLAY
                              | DDCAPS_OVERLAYSTRETCH
                              | DDCAPS_OVERLAYFOURCC
                              | DDCAPS_OVERLAYCANTCLIP
                              | DDCAPS_BLTDEPTHFILL;

    pHalInfo->ddCaps.dwCKeyCaps = DDCKEYCAPS_SRCBLT;

    pHalInfo->ddCaps.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN
                                      | DDSCAPS_PRIMARYSURFACE
                                      | DDSCAPS_FLIP
                                      | DDSCAPS_OVERLAY
                                      | DDSCAPS_HWCODEC;

    // Overlays need 8-byte alignment.  Note that if 24bpp overlays are
    // ever supported, this will have to change to compensate:
    pHalInfo->vmiData.dwOverlayAlign = ppdev->ulSurfaceAlign + 1;

    pHalInfo->ddCaps.dwCaps |=
          DDCAPS_OVERLAY
        | DDCAPS_OVERLAYSTRETCH
        | DDCAPS_OVERLAYFOURCC
        | DDCAPS_OVERLAYCANTCLIP;

    pHalInfo->ddCaps.dwFXCaps |= DDFXCAPS_OVERLAYSTRETCHX
                              | DDFXCAPS_OVERLAYSTRETCHY;
                                                                 // H.AZAR: (03/15/1999) overlay shrink support !
                                 //| DDFXCAPS_OVERLAYSHRINKX
                                 //| DDFXCAPS_OVERLAYSHRINKY;

    // We support only destination colour keying because that's the
    // only permutation we've had a chance to test.
    pHalInfo->ddCaps.dwCKeyCaps |= DDCKEYCAPS_DESTOVERLAY;

    if (pdwFourCC)
    {
        USHORT i = 0;
        pdwFourCC[i] = ppdev->fourCC[i++] = FOURCC_UYVY;
        pdwFourCC[i] = ppdev->fourCC[i++] = FOURCC_YUY2;
        pdwFourCC[i] = ppdev->fourCC[i++] = FOURCC_YVU9;
        pdwFourCC[i] = ppdev->fourCC[i++] = FOURCC_IV32;
        pdwFourCC[i] = ppdev->fourCC[i++] = FOURCC_IV31;
        pdwFourCC[i] = ppdev->fourCC[i++] = FOURCC_YV12;
        if (ppdev->bEnableIF09)
        {
            pdwFourCC[i] = ppdev->fourCC[i++] = FOURCC_IF09;
        }
    }
    pHalInfo->ddCaps.dwMaxVisibleOverlays = 1;
    pHalInfo->ddCaps.dwMinOverlayStretch   = ppdev->ulMinOverlayStretch;
    pHalInfo->ddCaps.dwMinLiveVideoStretch = ppdev->ulMinOverlayStretch;
    pHalInfo->ddCaps.dwMinHwCodecStretch   = ppdev->ulMinOverlayStretch;

    pHalInfo->ddCaps.dwMaxOverlayStretch   = 9999;
    pHalInfo->ddCaps.dwMaxLiveVideoStretch = 9999;
    pHalInfo->ddCaps.dwMaxHwCodecStretch   = 9999;

    pHalInfo->ddCaps.dwCaps |= DDCAPS_BLTSTRETCH;
    pHalInfo->ddCaps.dwFXCaps |= DDFXCAPS_BLTSTRETCHX
                                 | DDFXCAPS_BLTSTRETCHY;
    return (TRUE);
}
#endif

//******************************Public*Routine**********************************
//
// Function: DrvEnableDirectDraw()
//
//  This function is called by GDI to enable DirectDraw when a DirectDraw
//  program is started and DirectDraw is not already active.
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************


BOOL DrvEnableDirectDraw(
                        DHPDEV                  dhpdev,
                        DD_CALLBACKS*           pCallBacks,
                        DD_SURFACECALLBACKS*    pSurfaceCallBacks,
                        DD_PALETTECALLBACKS*    pPaletteCallBacks)

{
    PDEV* ppdev;
    ppdev = (PDEV*) dhpdev;

    //**************************************************************************
    // Perform one time initialization steps necessary every time directdraw is
    // started.
    //**************************************************************************
    if (!bAssertModeDirectDraw(ppdev, TRUE))
        return(FALSE);
    ppdev->flStatus |= STAT_DIRECTDRAW_ENABLED;
#ifndef NVD3D
    //****************************************************************************
    // DX3 - Initialize HAL fct ptrs.
    //****************************************************************************
    NvDDEnable(ppdev);
    *pCallBacks = ppdev->ddCallBacks;
    *pSurfaceCallBacks = ppdev->ddSurfaceCallBacks;
    *pPaletteCallBacks = ppdev->ddPaletteCallBacks;

#else
    //****************************************************************************
    // DX6 - Initialize HAL fct ptrs.
    //****************************************************************************
#ifndef _WIN64
    ppdev->pDriverData->DDCallbacks.dwFlags  |= DDHAL_CB32_MAPMEMORY;
    ppdev->pDriverData->DDCallbacks.MapMemory = DdMapMemory;
    *pCallBacks = ppdev->pDriverData->DDCallbacks;
    *pSurfaceCallBacks = ppdev->pDriverData->DDSurfaceCallbacks;
    *pPaletteCallBacks = ppdev->pDriverData->DDPaletteCallbacks;
#endif // #ifndef _WIN64
#endif // #ifndef NVD3D
    return (TRUE);
}

//******************************Public*Routine**********************************
//
// Function: DrvDisableDirectDraw()
//
//      This function is called by GDI when the last active DirectDraw program
//      is quit and DirectDraw will no longer be active.
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************

VOID DrvDisableDirectDraw(
                         DHPDEV      dhpdev)

    {
    PDEV* ppdev;
    ULONG  NvStatus;
    BOOL  status;
    ppdev = (PDEV*) dhpdev;
    if (!ppdev || !ppdev->bEnabled)
        {
        return;
        }

#if defined(NVD3D) && (NVARCH>= 0x04)
    vDisableDirectDraw(ppdev);
#else
    if (!bAssertModeDirectDraw(ppdev, FALSE))
        return;
#endif // NVD3D

    ppdev->flStatus &= ~STAT_DIRECTDRAW_ENABLED;

    //*****************************************************************************
    // If the 2d driver is running in DMA push mode, the channel ptr in ppdev->pjMmBase
    // must be the ddraw channel ptr, which is no longer valid.
    //*****************************************************************************
#if _WIN32_WINNT >= 0x0500
    // ifdeffed out because NT4 still references the PIO channel in many places -- slum
    if (ppdev->DmaPushEnabled2D)
        {
        ppdev->pjMmBase = NULL;
        }
#endif
    return;
    }

//******************************Public*Routine**********************************
//
// Function: bAssertModeDirectDraw
//
//      This function is called by enable.c when entering or leaving the
//      DOS full-screen character mode.
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     TRUE if successful.
//     FALSE if an error occurred.
//
//******************************************************************************


BOOL bAssertModeDirectDraw(PDEV*   ppdev, BOOL    bEnable)
{
    BOOL releaseOpenglSemaphore = FALSE;

    ENTER_DMA_PUSH_SYNC();

    if (bEnable && !ppdev->DDrawEnabledFlag)
    {
        //*************************************************************************
        // Init ddraw state data.
        //*************************************************************************
        ppdev->OverlayBufferIndex = 1;

        //*************************************************************************
        // If this is NV4, create the ddraw patch. NV3 ddraw patch is integrated
        // into the 2D patch.
        //*************************************************************************
#ifndef NVD3D
        if (!ppdev->pDriverData->dwRootHandle)
        {
            if (!DriverInit_NT4(ppdev->pDriverData))
                return(FALSE);
        }
        nvAssert(ppdev->pDriverData->dwRootHandle);

        if (!bCreateNVDDPatch(ppdev))
        {
            END_DMA_PUSH_SYNC();
            return(FALSE);
        }
#else
        //**************************************************************************
        // Initialize DX7 driver state for this device.
        //**************************************************************************
#ifndef _WIN64
#if (NVARCH >= 0x04)
        if (!ppdev->pDriverData->dwRootHandle)
            if (!DriverInit((ULONG)ppdev))
                return(FALSE);
#else
        if (ppdev->pDriverData == NULL)
        {
            pDriverData = ppdev->pDriverData =
                EngAllocMem(FL_ZERO_MEMORY, sizeof(GLOBALDATA), ALLOC_TAG);
            if (ppdev->pDriverData == NULL)
                {
                END_DMA_PUSH_SYNC();
                return(FALSE);
                }
            *(ppdev->pDriverData) = DefaultGlobalData;
            ppdev->pDriverData->dwGlobalStructSize = sizeof(GLOBALDATA);
            ppdev->pDriverData->ppdev = ppdev;
            ppdev->pDriverData->dwRootHandle = ppdev->hClient;
            ppdev->pDriverData->dwDeviceHandle = ppdev->hDevice;
            ddEnable(ppdev);
            if (!DriverInit((ULONG) ppdev))
            {
                EngFreeMem(ppdev->pDriverData);
                ppdev->pDriverData = NULL;
                END_DMA_PUSH_SYNC();
                return(FALSE);
            }
        }
#endif // D3D is not 64 bit code clean , skip for now...


        //********************************************************
        // Temp hack to use RM pitch alignment.
        //********************************************************
        ppdev->pDriverData->HALInfo.vmiData.lDisplayPitch = ppdev->lDelta;
        ppdev->pDriverData->VideoHeapTotal = ppdev->VideoHeapTotal;
        ppdev->pDriverData->VideoHeapFree = ppdev->VideoHeapFree;

#endif // #ifdef NVD3D
#endif // #ifndef _WIN64
        ppdev->DDrawEnabledFlag = TRUE;
    }
    else if (!bEnable && (ppdev->DDrawEnabledFlag))
    {
        //*************************************************************************
        // If this is NV4, destroy the ddraw patch. NV3 ddraw patch is integrated
        // into the 2D patch.
        //*************************************************************************
#ifndef NVD3D
        if (!bDestroyNVDDPatch(ppdev))
        {
            END_DMA_PUSH_SYNC();
            return(FALSE);
        }
#else   // NVD3D
        if (ppdev->pDriverData)
        {
            DWORD status;
            //************************************************
            // Force DX driver to recreate its objects.
            //************************************************
#ifndef _WIN64
            status = DestroyDriver32(ppdev->pDriverData);
#endif // D3D is not 64 bit code clean , skip for now...
        }

#endif // NVD3D
        ppdev->DDrawEnabledFlag = FALSE;
    }
    else if (bEnable && ppdev->DDrawEnabledFlag)
    {
#ifndef NVD3D
        extern VOID __cdecl NvTurnOffVideoOverlay(PDEV *ppdev);

        if(ppdev->dwOverlayEnabled == TRUE)
        {
            NvTurnOffVideoOverlay(ppdev);
        }
#endif
    }

    END_DMA_PUSH_SYNC();
    return(TRUE);
}

//******************************Public*Routine**********************************
//
// Function: bEnableDirectDraw()
//
// Routine Description:
//
//      This function is called by enable.c when the mode is first initialized,
//      right after the miniport does the mode-set.
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************

BOOL bEnableDirectDraw(PDEV*   ppdev)
{
    //**************************************************************************
    // Now init the low level direct draw functions depending on
    // what chip we're currently using.  We need to do this here,
    // (instead of later in DrvEnableDirectDraw), because GetDisplayDuration
    // uses the hardware registers.
    //**************************************************************************
//BUGBUG
#ifdef NV3
    ppdev->pfnGraphicsEngineBusy        = NV3_GraphicsEngineBusy;
    ppdev->pfnVBlankIsActive            = NV3_VBlankIsActive;
    ppdev->pfnWaitWhileVBlankActive     = NV3_WaitWhileVBlankActive;
    ppdev->pfnWaitWhileDisplayActive    = NV3_WaitWhileDisplayActive;
#else
    ppdev->pfnGraphicsEngineBusy        = NV4_GraphicsEngineBusy;
    ppdev->pfnVBlankIsActive            = NV4_VBlankIsActive;
    ppdev->pfnWaitWhileVBlankActive     = NV4_WaitWhileVBlankActive;
    ppdev->pfnWaitWhileDisplayActive    = NV4_WaitWhileDisplayActive;
#endif
    ppdev->pfnGetScanline               = NV_GetScanLineData;
    ppdev->pfnFifoIsBusy                = NV1FifoIsBusy;

#ifndef _WIN64
    //**************************************************************************
    // DirectDraw is all set to be used on this card:
    //**************************************************************************

    ppdev->flStatus |= STAT_DIRECTDRAW_CAPABLE;

    //**************************************************************************
    // Anything else we need to do here to prep NV for Direct Draw ??
    //**************************************************************************
#if (NVARCH >= 0x04)
#if IS_WINNT4
        if (!ppdev->pDriverData->dwRootHandle)
            if (!DriverInit_NT4(ppdev->pDriverData))
                return(FALSE);
#else
        if (!ppdev->pDriverData->dwRootHandle)
            if (!DriverInit((ULONG)ppdev))
                return(FALSE);
#endif
#endif

#endif // #ifndef _WIN64
    return (TRUE);
}

//******************************Public*Routine**********************************
//
// Function: vDisableDirectDraw()
//
// Routine Description:
//
//      This function is called by enable.c when the driver is shutting down.
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************
VOID vDisableDirectDraw(PDEV*   ppdev)
{
#if IS_WINNT4
//@mjl@   VppDestructor(&ppdev->pDriverData->vpp);
#endif

    if (!bAssertModeDirectDraw(ppdev, FALSE))
    {
        return;
    }

    ppdev->flStatus &= ~STAT_DIRECTDRAW_ENABLED;
}

#if _WIN32_WINNT >= 0x0500
//******************************Public*Routine**********************************
/*
 *  GetAvailDriverMemory
 *
 *  DDraw 'miscellaneous' callback returning the amount of free memory in driver's
 *  'private' heap
 */
//******************************************************************************

DWORD WINAPI GetAvailDriverMemory (PDD_GETAVAILDRIVERMEMORYDATA  pDmd)
{
    LONG    Free, Total;
    memory_t* pBlk;
    PDEV  *ppdev = pDmd->lpDD->dhpdev;

    DISPDBG((2,"NV3: GetAvailDriverMemory"));

    //**************************************************************************
    // Currently the driver only manages local vidmem
    //**************************************************************************
    if (pDmd->DDSCaps.dwCaps & DDSCAPS_LOCALVIDMEM ||
        pDmd->DDSCaps.dwCaps & DDSCAPS_VIDEOMEMORY ||
        pDmd->DDSCaps.dwCaps & DDSCAPS_PRIMARYSURFACE ||
        pDmd->DDSCaps.dwCaps & DDSCAPS_OFFSCREENPLAIN
#ifdef NVD3D
        ||
        pDmd->DDSCaps.dwCaps & DDSCAPS_TEXTURE
#endif // #ifdef NVD3D
        )
#ifndef RM_HEAPMGR
        {
        if (pDmd->DDSCaps.dwCaps &
            (DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE | DDSCAPS_LOCALVIDMEM))
            {
            pBlk = (memory_t *) ppdev->heap.heap_2d_ptr;
            if (!pBlk)
                {
                pDmd->ddRVal = DDERR_GENERIC;
                return(DDHAL_DRIVER_HANDLED);
                }
            Free = Total = 0;
            do
                {
                if (IS_EMPTY(pBlk))
                    Free += pBlk->blkSize;
                Total += pBlk->blkSize;
                pBlk  = pBlk->next;
                } while (pBlk != ppdev->heap.heap_2d_ptr);
            Free *= ALIGN_SIZE;
            Total *= ALIGN_SIZE;
            pDmd->dwFree = Free;
            pDmd->dwTotal = Total - (ppdev->cyScreen * ppdev->lDelta);
            }
        if (pDriverData->GARTLinearBase &&
            pDmd->DDSCaps.dwCaps &
                (DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE | DDSCAPS_NONLOCALVIDMEM))
            {
            pDmd->dwTotal += MAX_AGP_HEAP;
            pDmd->dwFree += MAX_AGP_HEAP;
            if (pDriverData->NvAGPDmaPusherBufferBase)
                {
                //*************************************************
                // Account for AGP push buffer.
                //*************************************************
                pDmd->dwFree -= pDriverData->NvDmaPusherBufferSize;
                }
            }
        }
#else
        {
        //********************************************************
        // Any memory currently used by GDI device bitmaps should
        // be reported as available for DX usage.
        //********************************************************
        pDmd->dwTotal = ppdev->VideoHeapTotal;
        pDmd->dwFree  = ppdev->VideoHeapFree + ppdev->cbGdiHeap;
        }
#endif // RM_HEAPMGR
        else
        {
            pDmd->dwTotal = 0;
            pDmd->dwFree  = 0;
        }

    pDmd->ddRVal = DD_OK;

    DISPDBG((2,"NV3:            Total:0x%08xd",pDmd->dwTotal));
    DISPDBG((2,"NV3:            Free :0x%08xd",pDmd->dwFree));
    return DDHAL_DRIVER_HANDLED;
}
#if !defined(DX7) || !(NVARCH >= 0x04) // DX7 driver uses GetDriverInfo32 routine in common code
//******************************Public*Routine**********************************
/*
 * GetDriverInfo32
 *
 * Initialize Extended Functionality Classes
 */
//******************************************************************************

DWORD WINAPI GetDriverInfo32(struct _DD_GETDRIVERINFODATA *lpData)
{
        DWORD dwSize = 0;
    PDEV  *ppdev = lpData->dhpdev;

    DISPDBG((2,"NV3dd: GetDriverInfo"));
    lpData->ddRVal = DDERR_CURRENTLYNOTAVAIL;

    if (IsEqualIID(&(lpData->guidInfo), &GUID_MiscellaneousCallbacks) ) {
        DD_MISCELLANEOUSCALLBACKS miscCB;
        dwSize = lpData->dwExpectedSize;

        DISPDBG((2,"NV3dd: GetDriverInfo - Process GUID_MiscellaneousCallbacks"));
        if ( sizeof(miscCB) < dwSize )
            dwSize = sizeof(miscCB);
        lpData->dwActualSize = sizeof(miscCB);
        memset(&miscCB, 0, dwSize);
        miscCB.dwSize = dwSize;

        miscCB.dwFlags = DDHAL_MISCCB32_GETAVAILDRIVERMEMORY;

        miscCB.GetAvailDriverMemory = GetAvailDriverMemory;

        memcpy(lpData->lpvData, &miscCB, dwSize );
        lpData->ddRVal = DD_OK;
    }

#ifdef NVPE
    /*
     * VPE stuff
     */

    if (IsEqualIID(&lpData->guidInfo, &GUID_VideoPortCallbacks))
    {
            dwSize = sizeof(DDHAL_DDVIDEOPORTCALLBACKS);

            lpData->dwActualSize = dwSize;
            lpData->ddRVal = DD_OK;

            memcpy(lpData->lpvData, &LPBVideoPortCallbacks, dwSize);
    }

        ///// H.AZAR (02/02/99): use structure defined in nvpe.lib
    if (IsEqualIID(&lpData->guidInfo, &GUID_VideoPortCaps))
    {
            dwSize = sizeof(DDVIDEOPORTCAPS);

            lpData->dwActualSize = dwSize;
            lpData->ddRVal = DD_OK;

            memcpy(lpData->lpvData, &g_ddNVideoPortCaps, dwSize);
    }

    if (IsEqualIID(&(lpData->guidInfo), &GUID_KernelCallbacks) )
    {
                DDHAL_DDKERNELCALLBACKS KernelCallbacks;
        DWORD dwSize = lpData->dwExpectedSize;

        if ( sizeof(KernelCallbacks) < dwSize )
            dwSize = sizeof(KernelCallbacks);
        lpData->dwActualSize = sizeof(KernelCallbacks);

        KernelCallbacks.dwSize = sizeof(DDHAL_DDKERNELCALLBACKS);
        KernelCallbacks.dwFlags = 0; //DDHAL_KERNEL_SYNCSURFACEDATA | DDHAL_KERNEL_SYNCVIDEOPORTDATA;
        KernelCallbacks.SyncSurfaceData = NULL;          // we're not doing anything on EITHER of these yet
        KernelCallbacks.SyncVideoPortData = NULL;

        memcpy(lpData->lpvData, &KernelCallbacks, dwSize );
        lpData->ddRVal = DD_OK; //DDERR_CURRENTLYNOTAVAIL;
    }

    if (IsEqualIID(&(lpData->guidInfo), &GUID_KernelCaps))
    {
        DDKERNELCAPS cpKernelCaps;
        if (lpData->dwExpectedSize != sizeof(DDKERNELCAPS))
            return DDHAL_DRIVER_HANDLED;

        lpData->dwActualSize = sizeof(DDKERNELCAPS);

        cpKernelCaps.dwSize = sizeof(DDKERNELCAPS);
        cpKernelCaps.dwCaps =   DDKERNELCAPS_SKIPFIELDS         |
                                        DDKERNELCAPS_AUTOFLIP           |
                                        DDKERNELCAPS_SETSTATE           |
                                            DDKERNELCAPS_LOCK               |
                                            //DDKERNELCAPS_FLIPVIDEOPORT      |
                                            DDKERNELCAPS_FLIPOVERLAY        |
                                            DDKERNELCAPS_CAPTURE_SYSMEM |
                                            //DDKERNELCAPS_CAPTURE_NONLOCALVIDMEM |
                                            DDKERNELCAPS_FIELDPOLARITY      ;
                                            //DDKERNELCAPS_CAPTURE_INVERTED   ;

        cpKernelCaps.dwIRQCaps = DDIRQ_VPORT0_VSYNC |
                                                           //#define DDIRQ_BUSMASTER                         0x00000002l
                               2; //DDIRQ_BUSMASTER;
                                                   //DDIRQ_DISPLAY_VSYNC |
                                                   //DDIRQ_RESERVED1     |
                                                   //DDIRQ_VPORT0_VSYNC  |    // this is the only IRQ we'll be supporting
                                                   //DDIRQ_VPORT0_LINE   |
                                                   //DDIRQ_VPORT1_VSYNC  |
                                                   //DDIRQ_VPORT1_LINE;

        memcpy(lpData->lpvData, &cpKernelCaps, sizeof(DDKERNELCAPS));
        lpData->ddRVal = DD_OK; //DDERR_CURRENTLYNOTAVAIL;
    }
#endif // NVPE

#ifdef NVD3D
    /*
     * Call D3D HAL to see if there's any GUIDs it wants to handle.
     */
    pDriverData = ppdev->pDriverData;
    D3DGetDriverInfo(lpData);
#endif // #ifdef NVD3D

    return DDHAL_DRIVER_HANDLED;

} /* GetDriverInfo32 */
#endif // !DX7
#endif // #if _WIN32_WINNT >= 0x0500

#if IS_WINNT4

/*
 * DriverInit_NT4
 *
 * this is the entry point called by DirectDraw to
 * initialize the 32-bit driver when running on NT4.
 *
 * This was converted from the version in ddraw/common/src/ddDrv.cpp and
 * the logic greatly reduced because we *know* we're on NT4 and many of the
 * optional features are not available (AGP for example).
 *
 * Note: all registry lookups have also been removed (for now)
 *
 */
DWORD __stdcall DriverInit_NT4(GLOBALDATA * pDriverData)
{
    static NvU8 bFirstTime = TRUE;

    dbgResetTraceLevel();
    dbgTracePush("DriverInit_NT4");

    nvAssert(pDriverData);
    nvAssert(pDriverData->ppdev);

    // These get erased on modesets? @mjl@
    pDriverData->dwRootHandle      = pDriverData->ppdev->hClient;
    pDriverData->NvBaseFlat        = (DWORD) pDriverData->ppdev->NvBaseAddr;

    if (bFirstTime)
    {
        pDriverData->pCommonNotifierBuffer = NULL;
        pDriverData->NvDeviceVersion   = (unsigned short) pDriverData->ppdev->dwDeviceVersion;
        pDriverData->dwDeviceHandle    = pDriverData->ppdev->hDevice;
        pDriverData->dwSurfaceAlignPad = pDriverData->ppdev->ulSurfaceAlign;
        pDriverData->BaseAddress       = 0;   // offset of frame buffer

        NvConfigGet(pDriverData->ppdev->hDriver,pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
            NV_CFG_RAM_SIZE_MB, &(pDriverData->TotalVRAM));
        pDriverData->TotalVRAM       <<= 20;    // Convert megabytes to bytes

#if (NVARCH < 0x010)
        if (pDriverData->NvDeviceVersion > NV_DEVICE_VERSION_5) {
            pDriverData->NvDeviceVersion = NV_DEVICE_VERSION_5;
        }
#endif // !NV10

        // get the number of heads (DACs) on this device
        NvConfigGet(pDriverData->ppdev->hDriver,pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
            NV_CFG_NUMBER_OF_HEADS, &pDriverData->dwHeads);

        nvDeterminePerformanceStrategy_NT4(pDriverData,&pDriverData->sysInfo);

        // Construct the VPP object (must be after nvDeterminePerformanceStrategy)
        //   There must be NO references to vpp data prior to this point
        //
        if (!VppConstructor(pDriverData->ppdev,&(pDriverData->vpp),&pDriverData->sysInfo))
        {
            VppDestructor(&(pDriverData->vpp)); // Release anything we might have allocated thus far
            dbgTracePop();
            return 0;
        }

        // TBD: move nvPusher init here? -@mjl@

//          bFirstTime = FALSE;
    }

    // Get the current registry settings from the hardware specific routine.
    // TBD: need any of these? @mjl@

    //***************************************************************
    // Shared variable for active channel id is stored in NT display
    // driver state.
    //***************************************************************

    g_adapterData[0].pDriverData = pDriverData; // add this device to the global adapter table (used by nvPriv)

    pDriverData->pCurrentChannelID       = &(pDriverData->ppdev->oglLastChannel);
    pDriverData->dwDesktopState          = 0;  // Means there is no clone or twinview mode
    pDriverData->regPBLocale             = NV_REG_SYS_PB_LOCATION_SYSTEM; // push buffer location: No AGP on NT4, force to SystemMem
//    pDriverData->dwVideoPortsAvailable   = 1;
    pDriverData->DDrawVideoSurfaceCount  = 0;
    pDriverData->dwTVTunerFlipCount      = 0;

    // Reset in case of a mode switch which unloaded the driver
    pDriverData->ppdev->HALInfo.ddCaps.dwCurrVisibleOverlays = 0;


    pDriverData->bltData.dwSystemSurfaceContextDMAIndex = 0;
    pDriverData->bltData.dwLastRop                      = 0xFFFFFFFF;
    pDriverData->bltData.dwLastColourKey                = 0xFFFFFFF0;
    pDriverData->bltData.dwLastColourFormat             = 0;
    pDriverData->bltData.dwLastCombinedPitch            = 0;
    pDriverData->bltData.dwLastSrcOffset                = 0xFFFFFFFF;
    pDriverData->bltData.dwLastDstOffset                = 0xFFFFFFFF;
    pDriverData->bltData.dwSystemBltFallback            = 0;
    pDriverData->bltData.dwLetItThrash                  = 0;


    // can't do this unless more than 4M and MMX instructions available
    if (!(pDriverData->sysInfo.dwCPUFeatureSet & FS_MMX) || (pDriverData->TotalVRAM <= 0x400000)) {
        pDriverData->vpp.regOverlayColourControlEnable = FALSE;
    }

    // Disable HQVUp on less than 16M
    if (pDriverData->TotalVRAM < 0x1000000) {
        pDriverData->vpp.regOverlayMode &= ~NV4_REG_OVL_MODE_VUPSCALE;
    }

    if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10)
    {
        // Disable HQVUp and sw colour controls on NV10
        pDriverData->vpp.regOverlayMode &= ~NV4_REG_OVL_MODE_VUPSCALE;
        pDriverData->vpp.regOverlayColourControlEnable = FALSE;
        if (pDriverData->TotalVRAM < 0x800000) {
            // Disable temporal and deinterlace filters on NV10 if less than 8M
            pDriverData->vpp.regOverlayMode &= ~NV4_REG_OVL_MODE_TFILTER;
            pDriverData->vpp.regOverlayMode &= ~NV4_REG_OVL_MODE_DFILTER;
        }
    } else {
        pDriverData->vpp.regOverlayMode &= ~NV4_REG_OVL_MODE_DFILTER;
        if (pDriverData->TotalVRAM < 0x1000000) {
            // Disable temporal filter on NV4/5 if less than 16M
            pDriverData->vpp.regOverlayMode &= ~NV4_REG_OVL_MODE_TFILTER;
        }
    }

    // Disable fullscreen mirror on non-dual head devices
    if (pDriverData->dwHeads < 2)
    {
        pDriverData->vpp.regOverlayMode2 &= ~NV4_REG_OVL_MODE2_FSDEVICEMASK;
    }
    else
    {
        dbgError("NT4 direct draw driver is seeing more than 1 head!");

        // Disable anyway!
        pDriverData->dwHeads = 1;
        pDriverData->vpp.regOverlayMode2 &= ~NV4_REG_OVL_MODE2_FSDEVICEMASK;
    }

    dbgTracePop();
    return (DWORD)pDriverData;
} // DriverInit_NT4

#endif
