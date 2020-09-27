/******************************Module*Header*******************************\
* Module Name: pixelfmt.c
*
* This module contains the functions that set and describe pixel formats.
*
* Copyright (c) 1998 NVidia Corporation
\**************************************************************************/

#include "precomp.h"
#include "driver.h"

#include "excpt.h"
#pragma hdrstop

#if (NVARCH >= 0x4)
#include "wincommon.h"
#include "wglcommon.h"
#include "nvapi.h"
#include "Nvcm.h"
#endif // (NVARCH >= 0x4)
#include "oglDD.h"
#include "OglOverlay.h"

#include "nvReg.h"

//
// export
//


#ifdef NV3
#define NUM_NV_PIXELFORMATS_16BPP  4

static PIXELFORMATDESCRIPTOR
NVPixelFormats16Bpp555[NUM_NV_PIXELFORMATS_16BPP] =
{
    /*
    ** 16 bit
    */
    // Single Buffer, No Stencil
    {
        sizeof(PIXELFORMATDESCRIPTOR),              // size
        1,                                          // version
        PFD_SUPPORT_OPENGL |                        // flags
        PFD_DRAW_TO_WINDOW |
        PFD_SUPPORT_GDI,
        PFD_TYPE_RGBA,                              // pixel type
        16, 5, 10, 5, 5, 5, 0,                      // color buffer
        0, 0,                                       // alpha buffer
        64, 16, 16, 16, 16,                         // accumulation buffer
        16,                                         // depth buffer
        0,                                          // NO stencil buffer
        0,                                          // aux buffers
        PFD_MAIN_PLANE,                             // layer type
        0,                                          // (reserved)
        0, 0, 0,                                    // layer masks
    },
    // Single Buffer, With Stencil
    {
        sizeof(PIXELFORMATDESCRIPTOR),              // size
        1,                                          // version
        PFD_SUPPORT_OPENGL |                        // flags
        PFD_DRAW_TO_WINDOW |
        PFD_SUPPORT_GDI,
        PFD_TYPE_RGBA,                              // pixel type
        16, 5, 10, 5, 5, 5, 0,                      // color buffer
        0, 0,                                       // alpha buffer
        64, 16, 16, 16, 16,                         // accumulation buffer
        16,                                         // depth buffer
        8,                                          // stencil buffer
        0,                                          // aux buffers
        PFD_MAIN_PLANE,                             // layer type
        0,                                          // (reserved)
        0, 0, 0,                                    // layer masks
    },
    // Double Buffer, No Stencil
    {
        sizeof(PIXELFORMATDESCRIPTOR),              // size
        1,                      // version
        PFD_SUPPORT_OPENGL |                        // flags
        PFD_DRAW_TO_WINDOW |
        PFD_DOUBLEBUFFER |
        PFD_SWAP_COPY,
        PFD_TYPE_RGBA,                              // pixel type
        16, 5, 10, 5, 5, 5, 0,                      // color buffer
        0, 0,                                       // alpha buffer
        64, 16, 16, 16, 16,                         // accumulation buffer
        16,                                         // depth buffer
        0,                                          // NO stencil buffer
        0,                                          // aux buffers
        PFD_MAIN_PLANE,                             // layer type
        0,                                          // (reserved)
        0, 0, 0,                                    // layer masks
    },
    // Double Buffer, With Stencil
    {
        sizeof(PIXELFORMATDESCRIPTOR),              // size
        1,                      // version
        PFD_SUPPORT_OPENGL |                        // flags
        PFD_DRAW_TO_WINDOW |
        PFD_DOUBLEBUFFER |
        PFD_SWAP_COPY,
        PFD_TYPE_RGBA,                              // pixel type
        16, 5, 10, 5, 5, 5, 0,                      // color buffer
        0, 0,                                       // alpha buffer
        64, 16, 16, 16, 16,                         // accumulation buffer
        16,                                         // depth buffer
        8,                                          // stencil buffer
        0,                                          // aux buffers
        PFD_MAIN_PLANE,                             // layer type
        0,                                          // (reserved)
        0, 0, 0,                                    // layer masks
    }
};

LONG APIENTRY NV3DescribePixelFormat(
    LONG                  iPixelFormat,
    ULONG                 cjpfd,
    PIXELFORMATDESCRIPTOR *ppfd)
{
    DWORD size;

    if (ppfd &&
        (iPixelFormat > 0) &&
        (iPixelFormat <= NUM_NV_PIXELFORMATS_16BPP)) {
        size = (cjpfd < sizeof(PIXELFORMATDESCRIPTOR)) ?
            cjpfd : sizeof(PIXELFORMATDESCRIPTOR);
        memcpy(ppfd, &NVPixelFormats16Bpp555[iPixelFormat-1], size);
    }

    return NUM_NV_PIXELFORMATS_16BPP;
}
#endif

//******************************************************************************
//
//  Function:   DrvSetPixelFormat
//
//  Routine Description:
//
//      Implements the set pixel format ICD routine.  This routine is called
//      by OpenGL32.DLL to set the pixel format to the value in iPixelFormat.
//
//  Arguments:
//
//  Return Value:
//
//      TRUE is the pixel format is set, FALSE otherwise.
//
//******************************************************************************
BOOL APIENTRY DrvSetPixelFormat(
    SURFOBJ *pso,
    LONG    iPixelFormat,
    HWND    hWnd
)
{
    int i;
    PDEV                  *ppdev = (PDEV *)pso->dhpdev;
    NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;
    NV_OPENGL_CLIENT_INFO_LIST *clientList = NULL;

    if (ppdev)
        {

        if (OglSupportEnabled(ppdev) == (ULONG)FALSE)
                    {
            DISPDBG((2, "SetPixelFormat: OglSupportEnable FAILED!", 0, ppdev->cBitsPerPel));
            return FALSE;
                    }

        clientList = OglFindClientInfoFromHWnd(ppdev, hWnd);
        if (clientList)
            {
            clientInfo = &clientList->clientInfo;
            }

        if (clientInfo && clientInfo->clientDrawableInfo)
        {
            clientInfo->clientDrawableInfo->iPixelFormat = iPixelFormat;
        }
    
        return TRUE;
        }
        
    else
    
        {
        return(FALSE);
        }

}

//*************************************************************************
// bOglStereoSurfacesAllowed
// 
// returns TRUE if the current environment, depending on quadro, 
// resolution and registry allows stereo
//*************************************************************************
BOOL bOglStereoSurfacesAllowed(PPDEV ppdev)
{
    ASSERT( ppdev );

    ppdev->bOglStereoDesired = FALSE;

#if (NVARCH >= 0x4)
    if (ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_QUADRO_GENERIC)
    {
        // do we have enough frambuffer memory to enable overlay ?
        ASSERT(ppdev->cyScreen > 0);
        ASSERT(ppdev->cyScreen < ppdev->cyMemory);
        OglReadRegistry(ppdev);

        //     room for 6 fullscreen buffer:    front*2+back*2+Z*2
        if(ppdev->cyMemory >= (LONG)(ppdev->cyScreen*6) )
        {
            if (ppdev->oglRegApiStereoSupport != NV_REG_OGL_STEREO_SUPPORT_OFF)
            {
                ppdev->bOglStereoDesired = TRUE;
            }
        }
    }
#endif //(NVARCH >= 0x4)
    return (ppdev->bOglStereoDesired);
}


/*
** bOglPfdCheckFlags
** 
** depending on registry, frambuffer size and other restricting 
** things return a mask with pixelformats types we allow.
** 
** FNicklisch 13.11.2000: New
*/ 
BOOL bOglPfdCheckFlags(PPDEV ppdev, ULONG *pulPfdCheckFlags)
{
#if (NVARCH >= 0x4)
    ASSERTDD(!bOglOverlaySurfacesAllowed(ppdev) || !bOglStereoSurfacesAllowed(ppdev), "stereo AND overlays not supported!");

    if( bOglOverlaySurfacesAllowed(ppdev) )
    {
        *pulPfdCheckFlags |= WIN_PFD_SUPPORTS_OVERLAYS;
        *pulPfdCheckFlags |= WIN_PFD_SUPPORTS_SWAP_LAYER_BUFFERS;
    }
         //
    else // KEY: workaround for never overlays and stereo at same time!
         //
    if( bOglStereoSurfacesAllowed(ppdev) )
    {
        *pulPfdCheckFlags |= WIN_PFD_SUPPORTS_STEREO;
    }
#else
    ppdev;            // unused parameter
    pulPfdCheckFlags;
#endif
    return (TRUE);
}

//******************************************************************************
//
//  Function:   DrvDescribePixelFormat
//
//  Routine Description:
//
//      Implements the routine which informs OpenGL32.DLL of our supported
//      pixel formats.
//
//  Known Problems:
//     
//     Make sure tbis method is consistent with icdcmd.c/DrvDescribePixelFormat [only NT4/NT5 goes through disp]
//
//  Arguments:
//
//  Return Value:
//
//      Number of pixel formats supported
//
//******************************************************************************
LONG APIENTRY DrvDescribePixelFormat(
    DHPDEV                  dhpdev,
    LONG                    iPixelFormat,
    ULONG                   cjpfd,
    PIXELFORMATDESCRIPTOR   *ppfd
)
{
    LONG result;
    DWORD size;
    PDEV  *ppdev;
    __WINNVpixelFormatFlags winPFFlags;

    DISPDBG((1, "DrvDescribePixelFormat: Entry iPixelFormat %ld",
             iPixelFormat));

    ppdev = (PDEV *)dhpdev;

#ifdef _WIN64
    // For some reason we die with an unspeakable backtrace when we
    // call __wglNVDescribePixelFormat after returning FALSE for
    // the OPENGL_INFO escape.  Return to it someday, but for now ...

    if (!(ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_QUADRO_GENERIC)) {
        DISPDBG((2, "DrvDescribePixelFormat: !ppdev->oglWorkStationPart!"));
        result = 0;
    } else
#endif
    if (OglSupportEnabled(ppdev) == (ULONG)FALSE) {
        DISPDBG((2, "DrvDescribePixelFormat: OglSupportEnabled FAILED!"));
        result = 0;
    } else {
#ifdef NV3
        if (ppdev->cBitsPerPel == 16) {
            result = NV3DescribePixelFormat(iPixelFormat, cjpfd, ppfd);
        } else {
            result = 0;
        }
#else        
        OglReadRegistry(ppdev);
                                     
        winPFFlags.color16depth32  = ppdev->oglColor16Depth32;
        winPFFlags.color32depth16  = ppdev->oglColor32Depth16; 
        winPFFlags.multisample     = ppdev->oglMultisample;
        winPFFlags.flippingControl = ppdev->oglRegFlippingControl;
        winPFFlags.unifiedBuffers  = ppdev->oglRegUBB; 

        result = 0;
#if 0
         __wglNVDescribePixelFormat(
            (int)ppdev->cBitsPerPel,
            0,
            (int)iPixelFormat,
            (UINT)cjpfd,
            ppfd,
            NULL,
            NULL,
            &winPFFlags,
            FALSE,
            (void *) ppdev);
#endif
#endif // NV3
    }
    
    DISPDBG((2, "DrvDescribePixelFormat: Exit %ld %ld",
             0, ppdev->cBitsPerPel));

    return result;
}
