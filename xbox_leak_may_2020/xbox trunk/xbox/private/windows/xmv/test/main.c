/*--
Copyright (c) 1999 - 2000 Microsoft Corporation - Xbox SDK

Module Name:

    Main.cpp

Abstract:

    App to test TV output

--*/

#include <xtl.h>
#include <xmv.h>

//------------------------------------------------------------------------------
// Intialize D3D.
//
void
Init()
{
    D3DPRESENT_PARAMETERS d3dpp;

    // Make D3D go.

    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth           = 640;
    d3dpp.BackBufferHeight          = 480;
    d3dpp.BackBufferFormat          = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount           = 1;
    d3dpp.EnableAutoDepthStencil    = TRUE;
    d3dpp.AutoDepthStencilFormat    = D3DFMT_D24S8;

    // Create the device.
    Direct3D_CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        NULL,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        NULL);
}

//==============================================================================
// Main goo.
//==============================================================================

//------------------------------------------------------------------------------
// Main entrypoint.
//
void __cdecl main()
{
    BOOL Enabled = FALSE;

    RECT SourceRect, DestRect;
    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT rect;
    D3DSurface *pBackBuffer;
    D3DSurface *pSurfaceShow, *pSurfaceDraw, *pSurfaceSwap;

    DWORD x, y, t;
    DWORD *pBits;

    XMVDecoder *pDecoder;
    XMVVideoDescriptor VideoDescriptor;
    XMVRESULT xr;

    // Set up D3D.
    Init();

    // Load the movie.
    if (FAILED(XMVCreateDecoder("d:\\movies\\test00.xmv", &pDecoder)))
    {
        _asm int 3;
    }

    XMVGetVideoDescriptor(pDecoder, &VideoDescriptor);

    // Get the back buffer.
    D3DDevice_GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
    D3DSurface_GetDesc(pBackBuffer, &Desc);

    // Set up the overlay surfaces.
    D3DDevice_CreateImageSurface(VideoDescriptor.Width, VideoDescriptor.Height, D3DFMT_YUY2, &pSurfaceShow);
    D3DDevice_CreateImageSurface(VideoDescriptor.Width, VideoDescriptor.Height, D3DFMT_YUY2, &pSurfaceDraw);

    // Prepare the overlay rectangle.
    SourceRect.left = 0;
    SourceRect.top  = 0;
    SourceRect.right = VideoDescriptor.Width;
    SourceRect.bottom = VideoDescriptor.Height;

    DestRect.left   = 0;
    DestRect.right  = Desc.Width;
    DestRect.top    = 0;
    DestRect.bottom = Desc.Height;

    // Create my famous striped screen and present it.
    D3DSurface_LockRect(pBackBuffer, &rect, 0, D3DLOCK_TILED);

    for (y = 0; y < Desc.Height; y++)
    {
        pBits = (DWORD *)((BYTE *)rect.pBits + rect.Pitch * y);

        for (x = 0; x < Desc.Width; x++)
        {
            if (x & 0x20)
            {
                *pBits = 0x007F7F7F;
            }
            else
            {
                t = (x / 32) * 127 / (Desc.Width / 32);

                *pBits =  0x00010000 * t
                          | 0x00000001 * (127 - t);
            }

            pBits++;
        }
    }

    D3DDevice_Swap(0);

    // Do our loop!
    for(;;)
    {
        xr = XMVGetNextFrame(pDecoder, pSurfaceDraw);

        switch(xr)
        {
        case XMV_NOFRAME:
            break;

        case XMV_NEWFRAME:

            // Swap the surfaces.
            pSurfaceSwap = pSurfaceShow;
            pSurfaceShow = pSurfaceDraw;
            pSurfaceDraw = pSurfaceSwap;

            if (!Enabled)
            {
                D3DDevice_EnableOverlay(TRUE);
                Enabled = TRUE;
            }

            while(!D3DDevice_GetOverlayUpdateStatus())
                ;

            D3DDevice_UpdateOverlay(pSurfaceShow, &SourceRect, &DestRect, FALSE, 0x00000000);

            break;

        case XMV_ENDOFFILE:

            if (Enabled)
            {
                D3DDevice_EnableOverlay(FALSE);
                Enabled = FALSE;
            }
            break;

        case XMV_FAIL:
            _asm int 3;
            break;
        }
        
        D3DDevice_BlockUntilVerticalBlank();
    }
}

