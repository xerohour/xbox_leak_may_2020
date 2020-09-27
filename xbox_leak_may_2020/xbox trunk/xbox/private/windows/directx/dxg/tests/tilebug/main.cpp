/*--
Copyright (c) 1999 - 2000 Microsoft Corporation - Xbox SDK

Module Name:

    Main.cpp

Abstract:

    Complete DX8 application to draw a screen space triangle.

Revision History:

    Derived from a DX8 sample.
--*/

#include <stddef.h>
#include <stdio.h>

extern "C"
{
    #include <ntos.h>
    #include <nturtl.h>
    #include <av.h>
};

#include <xtl.h>
#include <xgraphics.h>

IDirect3DDevice8*          pDev  = NULL;

#define TILE_DIMENSION 256
#define TILE_PITCH 256 * 4
#define TILE_SIZE TILE_DIMENSION * TILE_PITCH

static struct GradientData { float x,y,z,w; float u, v; } gradientData[] =
{
    { 0,                  0,                  0, 1, 0.5f, 0.5f },
    { TILE_DIMENSION - 2, 0,                  0, 1, 1.5f, 0.5f },
    { TILE_DIMENSION - 2, TILE_DIMENSION - 2, 0, 1, 1.5f, 1.5f },
    { 0,                  TILE_DIMENSION - 2, 0, 1, 0.5f, 1.5f },
};

static struct TextureData { float x,y,z,w; float u, v; } textureData[] =
{
    { 0,              0,              0, 1, 0,              0 },
    { TILE_DIMENSION, 0,              0, 1, TILE_DIMENSION, 0 },
    { TILE_DIMENSION, TILE_DIMENSION, 0, 1, TILE_DIMENSION, TILE_DIMENSION },
    { 0,              TILE_DIMENSION, 0, 1, 0,              TILE_DIMENSION },
};

#define CheckHR(y) { HRESULT _hr = (y); if (FAILED(_hr)) { _asm { int 3 } } }

//------------------------------------------------------------------------------
// Geometry declarations

DWORD VertsShaderDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT4), 
    D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
    D3DVSD_END()
};

static struct TheVerts { float y,x,z,w,tu,tv; } Verts[] =
{
    {320.0f,   0.0f, 0.5f, 1.0f,  0.5f, 0.0f},
    {527.0f, 360.0f, 0.5f, 1.0f,  1.0f, 1.0f},
    {113.0f, 360.0f, 0.5f, 1.0f,  0.0f, 1.0f},
};

//------------------------------------------------------------------------------
bool InitD3D
(
)
//--------------------------------------
{
    IDirect3D8 *pD3D = Direct3DCreate8(D3D_SDK_VERSION);
    if (pD3D == NULL)
        return false;

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth           = 640;
    d3dpp.BackBufferHeight          = 480;
    d3dpp.BackBufferFormat          = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount           = 1;
    d3dpp.Windowed                  = false;   // Must be false for Xbox.
    d3dpp.EnableAutoDepthStencil    = true;
    d3dpp.AutoDepthStencilFormat    = D3DFMT_D24S8;
    d3dpp.SwapEffect                = D3DSWAPEFFECT_DISCARD;
    d3dpp.FullScreen_RefreshRateInHz= 60;
    d3dpp.hDeviceWindow             = NULL;
    d3dpp.MultiSampleType           = D3DMULTISAMPLE_NONE;

    // Create the device.
    if (pD3D->CreateDevice
    (
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        NULL,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        &pDev
    ) != S_OK)
    {
        return false;
    }

    pD3D->Release();

    return true;
}

//------------------------------------------------------------------------------
void Paint
(
)
//--------------------------------------
{
    D3DSurface surface;
    D3DSurface *pFrontBuffer;
    D3DTILE tile;
    DWORD y;
    DWORD x;
    D3DLOCKED_RECT lockedRect;
    DWORD* pTexel;
    DWORD xCount;
    DWORD yCount;
    VOID* pMemory;

    // Do some initialization:

    pDev->SetRenderState(D3DRS_ZENABLE, FALSE);
    pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    pDev->Clear(0, NULL, D3DCLEAR_TARGET, 0, 0, 0);
    pDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    pDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    pDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    pDev->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_TEX1);

    // Allocate  physically contiguous memory to hold our texture:

    pMemory = D3D_AllocContiguousMemory(TILE_SIZE, D3DTILE_ALIGNMENT);

    // Create the texture and point it to our allocated memory:

    XGSetSurfaceHeader(TILE_DIMENSION, TILE_DIMENSION, D3DFMT_LIN_A8R8G8B8, &surface, 0, TILE_PITCH);

    surface.Register(pMemory);

    // Create a tile where the texture resides.
    //
    // NOTE: Set Flags to D3DTILE_FLAGS_ZBUFFER to use a SENSE_1 tile and repro
    //       the bug, and set Flags to 0 to use a SENSE_0 tile and not repro the
    //       bug.

    tile.Flags = D3DTILE_FLAGS_ZBUFFER;
    tile.Pitch = TILE_PITCH;
    tile.pMemory = pMemory;
    tile.Size = TILE_SIZE;

    pDev->SetTile(3, &tile);

    // Lock the (tiled) surface.  This gives us a pointer into the AGP aperture,
    // which is a view coherent with tiling:

    surface.LockRect(&lockedRect, NULL, D3DLOCK_TILED);
    for (y = 0; y < TILE_DIMENSION; y++)
    {
        pTexel = (DWORD*) ((BYTE*) lockedRect.pBits + y * lockedRect.Pitch);

        for (x = 0; x < TILE_DIMENSION; x++, pTexel++)
        {
            *pTexel = (x << 8) | (y);
        }
    }
    surface.UnlockRect();

    // Render from our (tiled) texture to our (tiled) back buffer:

    pDev->SetTexture(0, (D3DTexture*) &surface);
    pDev->DrawVerticesUP(D3DPT_QUADLIST, 4, textureData, sizeof(textureData[0]));

    // Move our drawn stuff to the front buffer to show the results:

    pDev->Present(NULL, NULL, NULL, NULL);

    // Verify the results:

    pDev->GetBackBuffer(-1, 0, &pFrontBuffer);
    pFrontBuffer->LockRect(&lockedRect, NULL, D3DLOCK_TILED);

    yCount = 0;
    for (y = 0; y < TILE_DIMENSION; y++)
    {
        pTexel = (DWORD*) ((BYTE*) lockedRect.pBits + y * lockedRect.Pitch);

        xCount = 0;
        for (x = 0; x < TILE_DIMENSION; x++, pTexel++)
        {
            DWORD expected = (x << 8) | (y);
            DWORD got = *pTexel;
            if (got != expected)
            {
                DbgPrint("(%li, %li) - Expected 0x%lx, Got 0x%lx\n", x, y, expected, got);
                xCount++;
            }

            if (xCount > 10)
            {
                yCount++;
                break;
            }
        }

        if (yCount > 10)
            break;
    }

    pFrontBuffer->UnlockRect();
    pFrontBuffer->Release();

    DbgPrint("Done!\n");

    while (TRUE)
        ;
}

void __cdecl main()
{
    InitD3D();

    while (TRUE)
    {
        Paint();
    }
}
