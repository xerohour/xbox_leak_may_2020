/*--
Copyright (c) 1999 - 2000 Microsoft Corporation - Xbox SDK

Module Name:

    Main.cpp

Abstract:

    Complete DX8 application to draw a triangle using a read/write vertex shader.

Revision History:

    Derived from a DX8 sample.
--*/

// Uncomment to disable using a vertex state shader
// in this example.

// #define DISABLE_STATE_SHADER

#include <SDKCommon.h>
#include "shaderDefs.h" 
#include "shader.h"
#ifndef DISABLE_STATE_SHADER
#include "stateShader.h"
#endif

#define CheckHR(x) { HRESULT _hr = (x); if (FAILED(_hr)) { _asm { int 3 } } }




IDirect3DDevice8*          pDev  = NULL;

//------------------------------------------------------------------------------
// Define our triangle.
static struct { float x,y,z; DWORD color; } Verts[] =
{
    {-0.666f,  -1.0f, 0.0f, 0xffff0000, },
    {-1.0f,  1.0f, 0.0f, 0xff00ff00, },
    {0.666f,  -1.0f, 0.0f, 0xff0000ff, },
};

DWORD dwShaderVertexDecl[] =
{
    D3DVSD_STREAM( 0 ),
    D3DVSD_END()
};

DWORD dwVertexShaderHandle;
DWORD dwVertexShaderAddress;

#ifndef DISABLE_STATE_SHADER
DWORD dwVertexStateShaderHandle;
DWORD dwVertexStateShaderAddress;
#endif

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
    d3dpp.MultiSampleType           = D3DMULTISAMPLE_4_SAMPLES
                                    | D3DMULTISAMPLE_PREFILTER_FORMAT_X1R5G5B5;


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

    if ( FAILED( pDev->CreateVertexShader(dwShaderVertexDecl,
        dwShaderVertexShader,
        &dwVertexShaderHandle, 0 )) )
    {
        return FALSE;
    }

#ifndef DISABLE_STATE_SHADER
    if ( FAILED( pDev->CreateVertexShader(NULL,
        dwStateShaderVertexStateShader,
        &dwVertexStateShaderHandle, 0 )) )
    {
        return FALSE;
    }
#endif

    // Load both shaders into vertex shader program memory

    DWORD ip = 0;
    UINT length;
    dwVertexShaderAddress = ip;
    if ( FAILED( pDev->LoadVertexShader(dwVertexShaderHandle, ip))) {
        return FALSE;
    }
    if ( FAILED( pDev->GetVertexShaderSize(dwVertexShaderHandle, &length))) {
        return FALSE;
    }

    ip += length;

#ifndef DISABLE_STATE_SHADER
    dwVertexStateShaderAddress = ip;
    if ( FAILED( pDev->LoadVertexShader(dwVertexStateShaderHandle, ip))) {
        return FALSE;
    }
    if ( FAILED( pDev->GetVertexShaderSize(dwVertexStateShaderHandle, &length))) {
        return FALSE;
    }

    ip += length;
#endif

    return TRUE;
}

unsigned int GetSimTime(){
	static unsigned int startTime;
	if ( ! startTime ) {
		startTime = timeGetTime();
	}
	return timeGetTime() - startTime;
}

double sawtooth(double s){
    double result = s - floor(s);
    if ( result > 0.5 ) result = 1.0 - result;
    return result;
}

void UpdateShaderConstants()
{

    float colorScale[4];
    {
        colorScale[4] = 1.0f;
        float t = (float) GetSimTime() / 1000.f;
        float scale = (float) sawtooth(t) + 0.5f; // Goes overbright
        colorScale[0] = scale;
        colorScale[1] = scale;
        colorScale[2] = scale;
    }

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
 	
    CheckHR( pDev->SetVertexShaderConstant(WVPMATRIX, &mat, 4) );

#ifdef DISABLE_STATE_SHADER
    CheckHR( pDev->SetVertexShaderConstant(COLORSCALE, colorScale, 1) );
#else

    // Use state shader to load the colorScale

    CheckHR( pDev->SelectVertexShader(dwVertexStateShaderHandle, dwVertexShaderAddress) ); 
    CheckHR( pDev->RunVertexStateShader(dwVertexStateShaderAddress, colorScale) );

#endif
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
    UpdateShaderConstants();

    CheckHR( pDev->SelectVertexShader(dwVertexShaderHandle, dwVertexShaderAddress) ); 
    CheckHR( pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE) );

    // Test out 'Begin' and 'End' and persistent attributes:

    CheckHR( pDev->Begin(D3DPT_TRIANGLELIST) );

    // Lower-left corner is red:

    CheckHR( pDev->SetVertexData4ub(1, 0xff, 0, 0, 0xff) );
    CheckHR( pDev->SetVertexData4f(-1, -0.666f, -1.0f, 0.0f, 1.0f) );

    // Top corner is green:

    CheckHR( pDev->SetVertexData4ub(1, 0, 0xff, 0, 0xff) );
    CheckHR( pDev->SetVertexData4f(-1, -1.0f, 1.0f, 0.0f, 1.0f) );

    // Lower-right corner is blue:

    CheckHR( pDev->SetVertexData4ub(1, 0, 0, 0xff, 0xff) );
    CheckHR( pDev->SetVertexData4f(-1, 0.666f, -1.0f, 0.0f, 1.0f) );

    CheckHR( pDev->End() );
                                              
    CheckHR( pDev->EndScene() );

    // Flip the buffers.

    pDev->Present(NULL, NULL, NULL, NULL);
}


//-----------------------------------------------------------------------------

void __cdecl main()
{
    if (!InitD3D(NULL))
    {
        return;
    }

    while (TRUE)
    {
        Paint();
    }
}
