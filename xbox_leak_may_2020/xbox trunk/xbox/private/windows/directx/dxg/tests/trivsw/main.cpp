/*--
Copyright (c) 1999 - 2000 Microsoft Corporation - Xbox SDK

Module Name:

    Main.cpp

Abstract:

    Complete DX8 application to draw a triangle using a read/write vertex shader.

Revision History:

    Derived from a DX8 sample.
--*/

#include <SDKCommon.h>
#include "shaderDefs.h" 
#include "shader.h"

#define CheckHR(x) { HRESULT _hr = (x); if (FAILED(_hr)) { _asm { int 3 } } }

BOOL                       bQuit   = FALSE;
IDirect3DDevice8*          pDev  = NULL;

//------------------------------------------------------------------------------
// Define our triangle.
static struct { float x,y,z; } Verts[] =
{
    {-0.666f,  -1.0f, 0.0f },
    {0.0f,  1.0f, 0.0f },
    {0.666f,  -1.0f, 0.0f },
};

DWORD dwShaderVertexDecl[] =
{
    D3DVSD_STREAM( 0 ),
    D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // Position
    D3DVSD_END()
};

DWORD dwVertexShaderHandle;

//------------------------------------------------------------------------------
BOOL InitD3D
(
    HWND hWnd
)
//--------------------------------------
{
    D3DPRESENT_PARAMETERS d3dpp;

    // Create D3D 8.
    IDirect3D8 *pD3D = Direct3DCreate8(D3D_SDK_VERSION);
    if (pD3D == NULL)
        return FALSE;

    // Set the screen mode.
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth           = 640;
    d3dpp.BackBufferHeight          = 480;
    d3dpp.BackBufferFormat          = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount           = 1;
    d3dpp.Windowed                  = FALSE;   // Must be FALSE for Xbox.
    d3dpp.EnableAutoDepthStencil    = TRUE;
    d3dpp.AutoDepthStencilFormat    = D3DFMT_D24S8;
    d3dpp.SwapEffect                = D3DSWAPEFFECT_DISCARD;
    d3dpp.FullScreen_RefreshRateInHz= 60;
    d3dpp.hDeviceWindow             = hWnd;

    // Create the device.

    if (IDirect3D8_CreateDevice
    (
        pD3D,
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        &pDev
    ) != S_OK)
    {
        return FALSE;
    }

    // Now we no longer need the D3D interface so lets free it.
    IDirect3D8_Release(pD3D);

    // Create vertex shader

    if ( FAILED( IDirect3DDevice8_CreateVertexShader( pDev,
        dwShaderVertexDecl, dwShaderReadWriteVertexShader,
        &dwVertexShaderHandle, 0 )) )
    {
        return FALSE;
    }

    // Set up the color constant registers
    static const float kColorConsts[]  = {
        0.0f, 0.0f, 0.0f, 1.0f, // base color
        0.0011f, 0.0017f, 0.0013f, 0.0f,  // increment
        1.0f, 1.0f, 1.0f, 1.0f  // All ones
    };

    CheckHR( pDev->SetVertexShaderConstant(COLOR, &kColorConsts, 3) );

    return TRUE;
}

unsigned int GetSimTime(){
	static unsigned int startTime;
	if ( ! startTime ) {
		startTime = timeGetTime();
	}
	return timeGetTime() - startTime;
}

void UpdateShaderConstants()
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
 	
    CheckHR( pDev->SetVertexShaderConstant(WPVMATRIX, &mat, 4) );
}

//------------------------------------------------------------------------------
void Paint
(
)
//--------------------------------------
{
    // Clear the frame buffer, Zbuffer.
    CheckHR( pDev->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
        0X00404040, 1.0, 0) );

    // Draw the vertex streamsetup in stream 0.
    CheckHR( pDev->BeginScene() );
    CheckHR( pDev->SetVertexShader(dwVertexShaderHandle) );

    UpdateShaderConstants();
 
    CheckHR( pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE) );

    CheckHR( pDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, Verts, sizeof(Verts[0])) );
                                              
    CheckHR( pDev->EndScene() );

    // Flip the buffers.

    pDev->Present(NULL, NULL, NULL, NULL);
}


//-----------------------------------------------------------------------------

void __cdecl main()
{
    _asm int 3;

    if (!InitD3D(NULL))
    {
        return;
    }

    while (TRUE)
    {
        Paint();
    }
}
