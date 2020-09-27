/*==========================================================================;
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       tiles.cpp
 *  Content:    Tests for tiling
 *
 ***************************************************************************/
 
#include "precomp.hpp" 

#define CLEARS 1000

//------------------------------------------------------------------------------
void TestTiles(D3DDevice* pDev)
{
    DWORD i;
    D3DTILE tile0;
    D3DTILE tile1;
    D3DTILE tile;
    CHAR buffer[200];

    CTimer time;

    pDev->GetTile(0, &tile0);
    pDev->GetTile(1, &tile1);

    DbgPrint("\n>> Timing tile effects...\n");

    // 1.  Time clears using stock tile setting.

    time.Start();
    for (i = 0; i < CLEARS; i++)
    {
        pDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 0, 0);
    }
    pDev->BlockUntilIdle();
    time.Stop();
    sprintf(buffer, "Stock tile setting: %.2fms\n", time.getTime());
    OutputDebugStringA(buffer);

    // 2.  Disable tile for frame buffer.

    pDev->SetTile(0, NULL);

    time.Start();
    for (i = 0; i < CLEARS; i++)
    {
        pDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 0, 0);
    }
    pDev->BlockUntilIdle();
    time.Stop();

    sprintf(buffer, "Frame buffer tile disabled: %.2fms\n", time.getTime());
    OutputDebugStringA(buffer);

    // 3.  Disable compression for Z buffer.

    tile = tile1;
    tile.Flags &= ~D3DTILE_FLAGS_ZCOMPRESS;

    pDev->SetTile(1, NULL);
    pDev->SetTile(1, &tile);

    time.Start();
    for (i = 0; i < CLEARS; i++)
    {
        pDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 0, 0);
    }
    pDev->BlockUntilIdle();
    time.Stop();

    sprintf(buffer, "Also Z compression disabled: %.2fms\n", time.getTime());
    OutputDebugStringA(buffer);

    // 4.  Disable tiling completely for Z buffer.

    pDev->SetTile(1, NULL);

    time.Start();
    for (i = 0; i < CLEARS; i++)
    {
        pDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 0, 0);
    }
    pDev->BlockUntilIdle();
    time.Stop();

    sprintf(buffer, "All tiling disabled: %.2fms\n", time.getTime());
    OutputDebugStringA(buffer);

    // 5.  Restore everything:

    pDev->SetTile(0, &tile0);
    pDev->SetTile(1, &tile1);

    ////////////////////////////////////////////////////////////////////////////

    D3DTexture* pDepth;
    D3DTexture* pColor;
    D3DTexture depth;
    D3DSurface* pOriginalColor;
    D3DSurface* pOriginalZ;
    D3DSurface* pColorSurface;
    D3DSurface* pDepthSurface;

    VOID* pBuffer = D3D_AllocContiguousMemory(2*1024*1024, D3DTILE_ALIGNMENT);

    CheckHR(pDev->CreateTexture(1024, 1024, 0, 0, D3DFMT_LIN_D16, 0, &pDepth));
    CheckHR(pDev->CreateTexture(1024, 1024, 0, 0, D3DFMT_LIN_R5G6B5, 0, &pColor));

    // Create our own depth surface by cheating and copying the D3D created
    // one.  We really need to expose a helper function for constructing
    // these...

    depth = *pDepth;
    depth.Data = 0;
    depth.Common &= ~D3DCOMMON_D3DCREATED;
    depth.Register(pBuffer);

    pDev->GetRenderTarget(&pOriginalColor);
    pDev->GetDepthStencilSurface(&pOriginalZ);
    pColor->GetSurfaceLevel(0, &pColorSurface);
    depth.GetSurfaceLevel(0, &pDepthSurface);
    pDev->SetRenderTarget(pColorSurface, pDepthSurface);

    // 1.  Time clears using stock Z target.

    time.Start();
    for (i = 0; i < CLEARS; i++)
    {
        pDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 0, 0);
    }
    pDev->BlockUntilIdle();
    time.Stop();
    sprintf(buffer, "Stock Z target: %.2fms\n", time.getTime());
    OutputDebugStringA(buffer);

    // 2.  Time clears using a partially compressed Z buffer.

    tile.Flags = D3DTILE_FLAGS_ZBUFFER | D3DTILE_FLAGS_ZCOMPRESS | D3DTILE_FLAGS_Z16BITS;
    tile.Pitch = 2*1024;
    tile.pMemory = pBuffer;
    tile.Size = 2*1024*1024;
    tile.ZStartTag = D3DTILE_MAXTAGS - (D3DTILE_ALIGNMENT / D3DTILE_TAGSIZE);
    tile.ZOffset = D3DTILE_ALIGNMENT;

    CheckHR(pDev->SetTile(4, &tile));

    // Test debug code:
    // CheckHR(pDev->SetTile(4, &tile));

    time.Start();
    for (i = 0; i < CLEARS; i++)
    {
        pDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 0, 0);
    }
    pDev->BlockUntilIdle();
    time.Stop();
    sprintf(buffer, "Partially compressed Z target: %.2fms\n", time.getTime());
    OutputDebugStringA(buffer);

    // 3.  Time clears using a fully compressed Z buffer.

    tile.ZStartTag = D3DTILE_ZENDTAG(&tile1);
    tile.ZOffset = 0;
    pDev->SetTile(4, NULL);
    pDev->SetTile(4, &tile);

    time.Start();
    for (i = 0; i < CLEARS; i++)
    {
        pDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 0, 0);
    }
    pDev->BlockUntilIdle();
    time.Stop();
    sprintf(buffer, "Fully compressed Z target: %.2fms (start tag: %li)\n", time.getTime(), tile.ZStartTag);
    OutputDebugStringA(buffer);

    pDev->SetRenderTarget(pOriginalColor, pOriginalZ);
    pDepth->Release();
    pColor->Release();
    pDepthSurface->Release();
    pColorSurface->Release();

    // 4.  Verify main target is unaffected with another tile in effect.

    time.Start();
    for (i = 0; i < CLEARS; i++)
    {
        pDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 0, 0);
    }
    pDev->BlockUntilIdle();
    time.Stop();

    sprintf(buffer, "Main target with another tile in effect: %.2fms\n", time.getTime());
    OutputDebugStringA(buffer);

    pDev->SetTile(4, NULL);
    D3D_FreeContiguousMemory(pBuffer);
}

