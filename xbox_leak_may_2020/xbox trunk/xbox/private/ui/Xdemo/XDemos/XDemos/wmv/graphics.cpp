/*++

Copyright (c) 2001  Microsoft Corporation. All rights reserved.

Module Name:

    graphics.cpp

Abstract:

    WMV playback sample application - graphics related functions

--*/

#include "..\XDemos.h"

//
// Vertical blank callback (runs at DPC level)
//
volatile DWORD VBlankCount;
VOID __cdecl VBlankCallback(DWORD vblankCount) {
    VBlankCount = vblankCount;
}

LONG TextureBufferQueue::refreshInterval;

VOID TextureBufferQueue::InitializeD3D()
{
    D3DDISPLAYMODE mode;
    HRESULT hr = g_pd3dDevice->GetDisplayMode(&mode);
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

    g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
    g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

    g_pd3dDevice->SetVerticalBlankCallback(VBlankCallback);
    g_pd3dDevice->EnableOverlay(TRUE);

    //
    // Create textures
    //
    for (INT i=0; i < MAX_TEXTURES; i++) {
        TexturePacket* pkt = &textures[i];
        
        HRESULT hr = g_pd3dDevice->CreateTexture(
                        VideoInfo.dwWidth,
                        VideoInfo.dwHeight,
                        0,
                        0,
                        D3DFMT_YUY2,
                        NULL,
                        &pkt->d3dtex);
        if (FAILED(hr))
		{
			return FALSE;
		}

        D3DLOCKED_RECT lockrect;
        hr = pkt->d3dtex->LockRect(0, &lockrect, NULL, 0);
        if (FAILED(hr))
		{
			return FALSE;
		}

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
        REFERENCE_TIME timediff = pkt->timestamp - CurrentPlayTime;

        if (timediff < -refreshInterval) {
            // The frame is too old, just discard it
            discardedFrames++;
            ReleaseFreePacket(readyList.RemoveHead());
        } else {
            if (timediff < refreshInterval) {
                // Forget it if there is another update pending
                if (!g_pd3dDevice->GetOverlayUpdateStatus()) {
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
    g_pd3dDevice->EnableOverlay(FALSE);

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

    g_pd3dDevice->UpdateOverlay(
                (D3DSurface*) pkt->d3dtex,
                &srcrect,
                &dstrect,
                FALSE,
                0);

    // Retire this packet after two VBlank interrupts
    pkt->status = VBlankCount+2;
}

