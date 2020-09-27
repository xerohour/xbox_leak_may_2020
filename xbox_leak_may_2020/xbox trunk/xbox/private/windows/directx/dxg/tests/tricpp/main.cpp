/*--
Copyright (c) 1999 - 2000 Microsoft Corporation - Xbox SDK

Module Name:

    Main.cpp

Abstract:

    Complete DX8 application to draw a screen space triangle.

Revision History:

    Derived from a DX8 sample.
--*/

#include <SDKCommon.h>
#include <xgraphics.h>
#include <stdio.h>

extern "C"
{
    extern BOOL D3D__Parser;
    extern BOOL D3D__SingleStepPusher;
}

IDirect3DDevice8*          pDev  = NULL;
IDirect3DVertexBuffer8*    pVertsVB = NULL;
IDirect3DTexture8*         pTexture = NULL;
DWORD                      VertsShader;

#define CheckHR(x) { HRESULT _hr = (x); if (FAILED(_hr)) { _asm { int 3 } } }

//------------------------------------------------------------------------------
// Geometry declarations

DWORD VertsShaderDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT4), 
    D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
    D3DVSD_END()
};

static struct TheVerts { float x,y,z,w,tu,tv; } Verts[] =
{
    {320.0f,   0.0f, 0.5f, 1.0f,  0.5f, 0.0f},
    {527.0f, 360.0f, 0.5f, 1.0f,  1.0f, 1.0f},
    {113.0f, 360.0f, 0.5f, 1.0f,  0.0f, 1.0f},
};

//------------------------------------------------------------------------------
bool InitD3D
(
    HWND hWnd
)
//--------------------------------------
{
    static int doneOnce; 
    if (!doneOnce)       
    {                    
        { ; }  
        doneOnce=1;      
    }                    

#if DBG
    D3D__Parser = TRUE;
#endif

    // Create D3D 8.
    IDirect3D8 *pD3D = Direct3DCreate8(D3D_SDK_VERSION);
    if (pD3D == NULL)
        return false;

    // Set the screen mode.
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
    d3dpp.hDeviceWindow             = hWnd;
    d3dpp.MultiSampleType           = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX;

    // Create the device.
    if (pD3D->CreateDevice
    (
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        &pDev
    ) != S_OK)
    {
        return false;
    }

    // Now we no longer need the D3D interface so lets free it.
    pD3D->Release();

    return true;
}

//------------------------------------------------------------------------------
bool InitVB
(
)
//--------------------------------------
{
    void* pVerts;

    CheckHR( pDev->CreateVertexBuffer( sizeof(Verts), 0, 0, 0, &pVertsVB ) );

    pVertsVB->Lock( 0, sizeof(Verts), (BYTE **)(&pVerts), 0 );
    memcpy( (void*)pVerts, (void*)Verts, sizeof(Verts) );
    pVertsVB->Unlock();

    D3DLOCKED_RECT lockRect;
    D3DSURFACE_DESC desc;

    if ( pDev->CreateTexture( 256, 256, 1, 0, D3DFMT_P8, D3DPOOL_MANAGED, &pTexture ) != S_OK )
        return false;

    if ( pTexture->GetLevelDesc( 0, &desc ) != S_OK )
        return false;

    if ( pTexture->LockRect ( 0, &lockRect, NULL, 0 ) != S_OK)
        return false;

    PVOID pBits = LocalAlloc(0, 256 * 256 * 1);

    // Generate a set of vertical lines of varying color into a buffer.
    BYTE *pb = (BYTE *)pBits;
    DWORD y;
    
    for (y = 0; y < 256; y++)
    {
        for (DWORD x = 0; x < 256; x++)
        {
            *pb = (BYTE) x;

            pb++;
        }
    }

    // Now draw an 'X' using index 0:
    pb = (BYTE *)pBits;
    for (y = 0; y < 256; y++)
    {
        *pb = 0;

        pb += 257;
    }

    pb = (BYTE *)pBits + 255;
    for (y = 0; y < 256; y++)
    {
        *pb = 0;

        pb += 255;
    }


    XGSwizzleRect(pBits,
                  0,
                  NULL,
                  lockRect.pBits,
                  256,
                  256,
                  NULL,
                  1);

    LocalFree(pBits);

    // Unlock the map so it can be used
    pTexture->UnlockRect(0);

    pDev->SetTexture(0, pTexture);

    // Create and select a fake test palette
    IDirect3DPalette8* pFakePalette;
    if ( pDev->CreatePalette(D3DPALETTE_32, &pFakePalette) != S_OK )
        return false;

    D3DCOLOR* pColors;
    if ( pFakePalette->Lock(&pColors, 0) != S_OK )
        return false;

    memset(pColors, 0, 32 * sizeof(D3DCOLOR));

    if ( pFakePalette->Unlock() != S_OK )
        return false;

    if ( pDev->SetPalette(0, pFakePalette) != S_OK )
        return false;

    // Now create and select a real palette
    IDirect3DPalette8* pPalette;
    if ( pDev->CreatePalette(D3DPALETTE_256, &pPalette) != S_OK )
        return false;

    if ( pPalette->Lock(&pColors, 0) != S_OK )
        return false;

    pColors[0] = 0xffff0000;                    // Index 0 is red
    pColors[255] = 0xff00ff00;                  // Index 255 is green

    for (DWORD i = 1; i < 255; i++)
    {
        pColors[i] = 0xff000000 | i;            // Rest is a blue wash
    }

    if ( pPalette->Unlock() != S_OK )
        return false;

    if ( pPalette->GetSize() != D3DPALETTE_256 )
        return false;

    if ( pDev->SetPalette(0, pPalette) != S_OK )
        return false;

    IDirect3DPalette8* pGetPalette;
    if ( pDev->GetPalette(0, &pGetPalette) != S_OK )
        return false;

    if ( pGetPalette != pPalette )
        return false;

    pGetPalette->Release();

    if ( pFakePalette->Release() != 0 )
        return false;

    // Set the texture stage states appropriately
    pDev->SetTextureStageState(0, D3DTSS_COLOROP,  D3DTOP_SELECTARG1);
    pDev->SetTextureStageState(0, D3DTSS_MINFILTER,D3DTEXF_LINEAR);
    pDev->SetTextureStageState(0, D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
    pDev->SetRenderState(D3DRS_ZENABLE, FALSE);
    pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // Create the vertex shaders
    CheckHR( pDev->CreateVertexShader(VertsShaderDeclaration, NULL, &VertsShader, 0) );

    // Test some extra junk.

    CheckHR( pDev->Nop() );
    if (pDev->GetVertexBlendModelView(0, NULL, NULL) != S_FALSE)
        _asm int 3;

    D3DMATRIX mat[4];

    pDev->SetRenderState(D3DRS_VERTEXBLEND, TRUE);
    CheckHR( pDev->SetVertexBlendModelView(1, mat, mat, mat) );
    CheckHR( pDev->SetVertexBlendModelView(4, mat, mat, mat) );
    if (pDev->GetVertexBlendModelView(4, mat, mat) != S_OK)
        _asm int 3;

    pDev->SetRenderState(D3DRS_VERTEXBLEND, FALSE);
    CheckHR(pDev->SetVertexBlendModelView(0, NULL, NULL, NULL));

    return true;
}

//------------------------------------------------------------------------------
void Paint
(
)
//--------------------------------------
{

    // Exercise the synchornization code.
    TheVerts* pVerts;
    D3DPushBuffer* pPushBuffer;
    DWORD i;
    DWORD offset;
    
    pVertsVB->Lock( 0, sizeof(Verts), (BYTE **)(&pVerts), 0 );

    // Transform the verticies to make the triangle spin so we can verify that
    // this actually works over a period of time.
    //
    static DWORD Time = GetTickCount();
    DWORD CurrentTime = GetTickCount();

    // 10 seconds per rotation
    float spin = 2 * 3.14159f * (float)(CurrentTime - Time) / 10000.0f;

    for (i = 0; i < 3; i++)
    {

        float x = Verts[i].x;
        float y = Verts[i].y;

        x = x - 320.0f;
        y = y - 240.0f;

        pVerts[i].x = x * (float)cos(spin) - y * (float)sin(spin);
        pVerts[i].y = x * (float)sin(spin) + y * (float)cos(spin);

        pVerts[i].x += 320.0f;
        pVerts[i].y += 240.0f;
    }

    pVertsVB->Unlock();

    pDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0X00404040, 1.0, 0);

    D3DSTREAM_INPUT streamInput = { pVertsVB, sizeof(Verts[0]), 0 };
    pDev->SetStreamSource(0, pVertsVB, sizeof(Verts[0]));
    pDev->SetVertexShader(D3DFVF_XYZRHW|D3DFVF_TEX1);
    pDev->SetVertexShaderInput(VertsShader, 1, &streamInput);

    pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

    CheckHR(pDev->CreatePushBuffer(4 * 4096, FALSE, &pPushBuffer));

    // Record our rotating triangle:
    pDev->BeginPushBuffer(pPushBuffer);
    pDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
    pDev->EndPushBuffer();
    pDev->RunPushBuffer(pPushBuffer, NULL);

    // Flip the buffers.
    pDev->Present(NULL, NULL, NULL, NULL);

    pPushBuffer->Release();
}

void __cdecl main()
{
    _asm nop;
    _asm nop;

#if DBG
    D3D__SingleStepPusher = TRUE;
#endif

    InitD3D(NULL);
    InitVB();

    D3DVIEWPORT8 D3DViewport;

    D3DViewport.X      = 0;
    D3DViewport.Y      = 0;
    D3DViewport.Width  = 640;
    D3DViewport.Height = 480;
    D3DViewport.MinZ   = 0.0f;
    D3DViewport.MaxZ   = 1.0f;

    // Set our Viewport
    D3DDevice_SetViewport( &D3DViewport );

    while (TRUE)
    {
        Paint();
    }
}
