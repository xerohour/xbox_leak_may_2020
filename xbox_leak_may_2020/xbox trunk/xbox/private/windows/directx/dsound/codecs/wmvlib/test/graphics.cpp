/*++

Copyright (c) 2001  Microsoft Corporation. All rights reserved.

Module Name:

    graphics.cpp

Abstract:

    WMV playback sample application - graphics related functions

--*/

#include "wmvplay.h"


//
// Vertical blank callback (runs at DPC level)
//
volatile DWORD VBlankCount;
VOID __cdecl VBlankCallback(D3DVBLANKDATA* data) {
    VBlankCount = data->VBlank;
}

//
// One-time initialization of D3D
//
IDirect3DDevice8* TextureBufferQueue::d3ddev;
LONG TextureBufferQueue::refreshInterval;

VOID TextureBufferQueue::InitializeD3D()
{
    IDirect3D8* d3d = Direct3DCreate8(D3D_SDK_VERSION);
    ASSERT(d3d != NULL);

    D3DPRESENT_PARAMETERS d3dpp;
    memset(&d3dpp, 0, sizeof(d3dpp));

    d3dpp.BackBufferWidth                 = SCREENWIDTH;
    d3dpp.BackBufferHeight                = SCREENHEIGHT;
    d3dpp.BackBufferFormat                = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount                 = 0;
    d3dpp.Windowed                        = FALSE;
    d3dpp.EnableAutoDepthStencil          = TRUE;
    d3dpp.AutoDepthStencilFormat          = D3DFMT_D24S8;
    d3dpp.SwapEffect                      = D3DSWAPEFFECT_DISCARD;

    HRESULT hr = d3d->CreateDevice(
                        D3DADAPTER_DEFAULT,
                        D3DDEVTYPE_HAL,
                        NULL,
                        D3DCREATE_HARDWARE_VERTEXPROCESSING,
                        &d3dpp,
                        &d3ddev);
    ASSERT(SUCCEEDED(hr));
    d3d->Release();

    D3DDISPLAYMODE mode;
    hr = d3ddev->GetDisplayMode(&mode);
    if (SUCCEEDED(hr) && mode.RefreshRate) {
        refreshInterval = 1000*10000 / mode.RefreshRate;
    } else {
        // Default to 16msecs
        refreshInterval = 16*10000;
    }
}

//
// Initialize the graphics playback module
//
BOOL TextureBufferQueue::Initialize()
{
    maxCount = MAX_TEXTURES;
    maxPacketSize = VideoInfo.dwHeight * VideoInfo.dwWidth * VideoInfo.dwOutputBitsPerPixel / 8;
    ASSERT(maxPacketSize % 4 == 0);

    d3ddev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
    d3ddev->Present(NULL, NULL, NULL, NULL);

    d3ddev->SetVerticalBlankCallback(VBlankCallback);
    d3ddev->EnableOverlay(TRUE);

    //
    // Create textures
    //
    for (INT i=0; i < MAX_TEXTURES; i++) {
        TexturePacket* pkt = &textures[i];
        
        HRESULT hr = d3ddev->CreateTexture(
                        VideoInfo.dwWidth,
                        VideoInfo.dwHeight,
                        0,
                        0,
                        D3DFMT_YUY2,
                        NULL,
                        &pkt->d3dtex);
        if (FAILED(hr)) return FALSE;

        D3DLOCKED_RECT lockrect;
        hr = pkt->d3dtex->LockRect(0, &lockrect, NULL, 0);
        if (FAILED(hr)) return FALSE;

        pkt->flags = PACKETFLAG_VIDEO;
        pkt->xmp.pvBuffer = lockrect.pBits;
        pkt->d3dtex->UnlockRect(0);

        freeList.AddTail(pkt);
    }

    discardedFrames = overlayBlocks = 0;
    return PacketQueue::Initialize();
}

//
// Process video frames
//
VOID TextureBufferQueue::Process()
{
    //
    // Retire any frames that have already been rendered:
    //  A frame in the busy queue can be freed if there is another frame after it
    //  and the current vblank count is >= the value in the later packet.
    //
    Packet* pkt;
    while ((pkt = busyList.head) != NULL) {
        Packet* next = pkt->next;
        if (next == NULL || (LONG) (VBlankCount - next->status) < 0)
            break;

        busyList.RemoveHead();
        freeList.AddTail(pkt);
    }

    #ifndef DONT_PLAY_VIDEO

    //
    // Render frames that're ready for display
    //
    while ((pkt = readyList.head) != NULL) {

#if 0

        // Forget it if there is another update pending
        if (!d3ddev->GetOverlayUpdateStatus()) {
            overlayBlocks++;
            return;
        }

        // Render the frame if it's time
        readyList.RemoveHead();

        RenderFrame((TexturePacket*) pkt);
        busyList.AddTail(pkt);

#else

        REFERENCE_TIME timediff = pkt->timestamp - CurrentPlayTime;

        if (timediff < -refreshInterval) {
            // The frame is too old, just discard it
            discardedFrames++;
            ReleaseFreePacket(readyList.RemoveHead());
        } else {
            if (timediff < refreshInterval) {
                // Forget it if there is another update pending
                if (!d3ddev->GetOverlayUpdateStatus()) {
                    overlayBlocks++;
                    return;
                }

                // Render the frame if it's time
                readyList.RemoveHead();

                RenderFrame((TexturePacket*) pkt);
                busyList.AddTail(pkt);
            }
            break;
        }

#endif
    }

    #else // DONT_PLAY_VIDEO

    while (!readyList.IsEmpty()) {
        freeList.AddTail(readyList.RemoveHead());
    }

    #endif // DONT_PLAY_VIDEO
}

//
// Stop graphics playback
//
VOID TextureBufferQueue::Shutdown()
{
    d3ddev->EnableOverlay(FALSE);

    // Wait for two vblanks
    DWORD vblanks = VBlankCount+2;
    while ((LONG) (VBlankCount - vblanks) < 0)
        ;

    if (discardedFrames) {
        VERBOSE("Discarded video frames: %d (%d)\n", discardedFrames, overlayBlocks);
    }

    while (!busyList.IsEmpty()) {
        freeList.AddTail(busyList.RemoveHead());
    }
}

//
// Render the frame
//
VOID TextureBufferQueue::RenderFrame(TexturePacket* pkt)
{
    RECT srcrect = { 0, 0, VideoInfo.dwWidth, VideoInfo.dwHeight };
    RECT dstrect = { 0, 0, SCREENWIDTH, SCREENHEIGHT };

    if (!FullScreenMode) {
        //
        // We need to adjust the destination rectangle and
        // possibly the source rectangle when in 1:1 mode. 
        //
        if (VideoInfo.dwWidth <= SCREENWIDTH) {
            // Horizontal centering
            dstrect.left = (SCREENWIDTH - VideoInfo.dwWidth) / 2;
            dstrect.right = dstrect.left + VideoInfo.dwWidth;
        } else {
            // Clipping
            srcrect.left = (VideoInfo.dwWidth - SCREENWIDTH) / 2;
            srcrect.right = srcrect.left + SCREENWIDTH;
        }

        if (VideoInfo.dwHeight <= SCREENHEIGHT) {
            // Vertical centering
            dstrect.top = (SCREENHEIGHT - VideoInfo.dwHeight) / 2;
            dstrect.bottom = dstrect.top + VideoInfo.dwHeight;
        } else {
            // Clipping
            srcrect.top = (VideoInfo.dwHeight - SCREENHEIGHT) / 2;
            srcrect.bottom = dstrect.top + SCREENHEIGHT;
        }
    }

    d3ddev->UpdateOverlay(
                (D3DSurface*) pkt->d3dtex,
                &srcrect,
                &dstrect,
                FALSE,
                0);

    // Retire this packet after two VBlank interrupts
    pkt->status = VBlankCount+2;
}

