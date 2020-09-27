/*--
Copyright (c) 1999 - 2000 Microsoft Corporation - Xbox SDK

Module Name:

    Main.cpp

Abstract:

    Complete DX8 application to draw a screen space triangle using a vertex shader.

Revision History:

    Derived from a DX8 sample.
--*/

#include <SDKCommon.h>
#include <stdio.h>

#include "shader.h"

#define CheckHR(x) { HRESULT _hr = (x); if (FAILED(_hr)) { _asm { int 3 } } }

IDirect3DDevice8*           g_pDev  = NULL;
DWORD                       g_ShaderHandle;
IDirect3DPushBuffer8*       g_pPushBufer;
IDirect3DFixup8*            g_pSizer;
IDirect3DFixup8*            g_pFixup[2];
DWORD                       g_iFixup;
DWORD                       g_PushBufferSize;
DWORD                       g_ConstantOffset;

//------------------------------------------------------------------------------
// Define our triangle.
static struct { float x,y,z; DWORD color; } Verts[] =
{
    {-0.666f,  -1.0f, 0.0f, 0xffff0000, },
    { 0.0f,     1.0f, 0.0f, 0xff00ff00, },
    { 0.666f,  -1.0f, 0.0f, 0xff0000ff, },
};

DWORD dwShaderVertexDecl[] =
{
    D3DVSD_STREAM( 0 ),
    D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // Position
    D3DVSD_REG( 1, D3DVSDT_D3DCOLOR ), // Diffuse color
    D3DVSD_END()
};

//------------------------------------------------------------------------------
void RecordPushBuffer()
{
    D3DMATRIX mat;

    CheckHR( g_pDev->BeginPushBuffer(g_pPushBufer) );

    // Clear the frame buffer, Zbuffer.
    CheckHR( g_pDev->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
        0X00404040, 1.0, 0) );

    // Draw the vertex streamsetup in stream 0.
    CheckHR( g_pDev->SetVertexShader(g_ShaderHandle) );

    // Note that we initially record a garbage matrix into the shader-constants,
    // which is okay because we'll always fix it up before playback.
    CheckHR( g_pDev->GetPushBufferOffset( &g_ConstantOffset ) );
    CheckHR( g_pDev->SetVertexShaderConstant(0, &mat, 4) );

    CheckHR( g_pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE) );
    CheckHR( g_pDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, Verts, sizeof(Verts[0])) );

    CheckHR( g_pDev->EndPushBuffer() );
}

//------------------------------------------------------------------------------
BOOL InitD3D()
{
    D3DPRESENT_PARAMETERS d3dpp;

    IDirect3D8 *pD3D = Direct3DCreate8(D3D_SDK_VERSION);
    if (pD3D == NULL)
        return FALSE;

    // Set the screen mode.
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth           = 640;
    d3dpp.BackBufferHeight          = 480;
    d3dpp.BackBufferFormat          = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount           = 2;
    d3dpp.Windowed                  = FALSE;   // Must be FALSE for Xbox.
    d3dpp.EnableAutoDepthStencil    = TRUE;
    d3dpp.AutoDepthStencilFormat    = D3DFMT_D24S8;
    d3dpp.SwapEffect                = D3DSWAPEFFECT_DISCARD;
    d3dpp.FullScreen_RefreshRateInHz= 60;
    d3dpp.MultiSampleType           = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX;

    // Create the device.
    CheckHR( pD3D->CreateDevice(
                        D3DADAPTER_DEFAULT,
                        D3DDEVTYPE_HAL,
                        NULL,
                        D3DCREATE_HARDWARE_VERTEXPROCESSING,
                        &d3dpp,
                        &g_pDev) );

    // Now we no longer need the D3D interface so let's free it.
    pD3D->Release();

    // Create vertex shader.
    CheckHR( g_pDev->CreateVertexShader(dwShaderVertexDecl, 
                                      dwShaderVertexShader,
                                      &g_ShaderHandle, 
                                      0) );

    // Create a push-buffer, two fix-up buffers that we'll ping-pong between,
    // and a fix-up buffer that we'll use just for determining our fix-up
    // size.
    CheckHR( g_pDev->CreatePushBuffer(16 * 4096, FALSE, &g_pPushBufer) );
    CheckHR( g_pDev->CreateFixup(1024, &g_pFixup[0]) );
    CheckHR( g_pDev->CreateFixup(1024, &g_pFixup[1]) );
    CheckHR( g_pDev->CreateFixup(0, &g_pSizer) );

    // Now record the push-buffer.
    RecordPushBuffer();

    return TRUE;
}

//------------------------------------------------------------------------------
unsigned int GetSimTime()
{
    static unsigned int startTime;
    if ( ! startTime ) 
    {
        startTime = timeGetTime();
    }
    return timeGetTime() - startTime;
}

//------------------------------------------------------------------------------
void UpdateShaderConstants(D3DMATRIX* pMat)
{
    // Rotate around the Y axis
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixRotationY( &matWorld, GetSimTime()/600.0f );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up.
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f, 0.0f, -3.0f ), 
                                  &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 
                                  &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIX matProj;
    float aspectRatio = 480.f / 640.f;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, aspectRatio, 1.0f, 800.0f );

    // Calculate concatenated World x ( View x Projection) matrix.
    // We transpose the matrix at the end because that's how matrix math
    // works in vertex shaders. (Because the vertex shader DP4 operator works on
    // rows, not on columns.)
    D3DXMATRIX mat;
    D3DXMatrixMultiply( &mat, &matView, &matProj );
    D3DXMatrixMultiply( &mat, &matWorld, &mat );
    D3DXMatrixTranspose( &mat, &mat );

    *pMat = mat;
}

//------------------------------------------------------------------------------
HRESULT ApplyFixups(D3DPushBuffer *pPushBuffer, D3DFixup *pFixup, D3DMATRIX* pMat)
{
    CheckHR( g_pPushBufer->BeginFixup( pFixup, 0 ) );
    CheckHR( g_pPushBufer->SetVertexShaderConstant( g_ConstantOffset, 0, pMat, 4 ) );
    return g_pPushBufer->EndFixup( );
}

//------------------------------------------------------------------------------
void Paint()
{
    D3DMATRIX mat;
    DWORD size;
    DWORD space;

    UpdateShaderConstants( &mat );

    // First, use an empty fix-up object to figure out the size of the
    // fix-ups.  Using an empty fix-up object is fast because it doesn't
    // actually copy anything, but still looks at the fix-up parameters
    // to calculate the correct size.
    //
    // Obviously, this never changes from frame-to-frame, so we could
    // cache this.
    ApplyFixups( g_pPushBufer, g_pSizer, &mat );
    CheckHR( g_pSizer->GetSize( &size ) );

    // Now figure out what real fix-up object we'd like to use, and if there's
    // space.
    D3DFixup* pFixup = g_pFixup[g_iFixup];
    CheckHR( pFixup->GetSpace( &space ) );

    if ( space < size )
    {
        // Darn, we overflowed the fix-up buffer that we wanted to use.
        // Ping-pong to the other buffer and use that one now.
        //
        // (If we tried to reset the same buffer we were just using, D3D
        // would have to sit and spin until the GPU is idle, since we just
        // used that buffer.)
        g_iFixup ^= 1;
        pFixup = g_pFixup[g_iFixup];

        CheckHR( pFixup->Reset() );
    }

    // Okay, do the fix-up for real.
    CheckHR( ApplyFixups( g_pPushBufer, pFixup, &mat ) );

    // Finally, actually run the push-buffer and then we're done.
    CheckHR( g_pDev->RunPushBuffer( g_pPushBufer, pFixup ) );
    CheckHR( g_pDev->Present( NULL, NULL, NULL, NULL ) );
}

//-----------------------------------------------------------------------------
void __cdecl main()
{
    InitD3D();

    while (TRUE)
    {
        Paint();
    }
}
