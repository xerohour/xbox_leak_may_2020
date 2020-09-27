//-----------------------------------------------------------------------------
// File: VertexShaders.cpp
//
// Desc: Shows how to use vertex shaders. Note that the vertex shader is 
//       precompiled into a .xvu file, and that the vertex declaration and
//       vertex shader constants used by an app, must be in agreement with the
//       vertex shader itself.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>




//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D8       g_pD3D           = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE8 g_pd3dDevice     = NULL; // Our rendering device
DWORD             g_dwVertexShaderHandle;  // Handle for the vertex shader




//-----------------------------------------------------------------------------
// Define our triangle.
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{ 
    FLOAT x,y,z; 
    DWORD color; 
};

CUSTOMVERTEX g_Vertices[] =
{
    { -0.6f, -1.0f, 0.0f, 0xffff0000, },
    {  0.0f,  1.0f, 0.0f, 0xff00ff00, },
    {  0.6f, -1.0f, 0.0f, 0xff0000ff, },
};

DWORD dwShaderVertexDecl[] =
{
    D3DVSD_STREAM( 0 ),
    D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),   // Position
    D3DVSD_REG( 1, D3DVSDT_D3DCOLOR ), // Diffuse color
    D3DVSD_END()
};




//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D()
{
    // Create the D3D object.
    if( NULL == ( g_pD3D = Direct3DCreate8( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice.
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.BackBufferWidth        = 640;
    d3dpp.BackBufferHeight       = 480;
    d3dpp.BackBufferFormat       = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount        = 1;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
    d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;

    // Create the Direct3D device.
    if( FAILED( g_pD3D->CreateDevice( 0, D3DDEVTYPE_HAL, NULL,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
        return E_FAIL;

    // Load the pre-compiled vertex shader microcode
    HANDLE hFile = CreateFileA( "D:\\media\\Shader.xvu", GENERIC_READ, FILE_SHARE_READ, 
                                NULL, OPEN_EXISTING, 
                                FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN, NULL );
    if( hFile == INVALID_HANDLE_VALUE )
    {
        OutputDebugStringA( "ERROR: Could not load the vertex shader!\n" );
        return E_FAIL;
    }

    DWORD dwSize = GetFileSize( hFile, NULL );
    BYTE* pData  = new BYTE[dwSize];
	DWORD dwSizeRead;
    if(!ReadFile( hFile, pData, dwSize, &dwSizeRead, NULL ) ||
		dwSizeRead != dwSize )
	{
		CloseHandle(hFile);
		OutputDebugStringA( "ERROR: Could not load the vertex shader!\n" );
		return E_FAIL;
	}

    CloseHandle( hFile );

    // Create the vertex shader
    HRESULT hr = g_pd3dDevice->CreateVertexShader( dwShaderVertexDecl, (DWORD*)pData,
                                                   &g_dwVertexShaderHandle, 0 );
    delete pData;
    if( FAILED(hr) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Animates the scene
//-----------------------------------------------------------------------------
VOID FrameMove()
{
    // Rotate around the Y axis
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixRotationY( &matWorld, timeGetTime()/600.0f );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up.
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f, 0.0f,-3.0f ), 
                                  &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 
                                  &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIX matProj;
    FLOAT fAspectRatio = 640.0f / 480.0f;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspectRatio, 1.0f, 800.0f );

    // Calculate concatenated World x ( View x Projection) matrix.
    // We transpose the matrix at the end because that's how matrix math
    // works in vertex shaders. (Because the vertex shader DP4 operator works on
    // rows, not on columns.)
    D3DXMATRIX mat;
    D3DXMatrixMultiply( &mat, &matView, &matProj );
    D3DXMatrixMultiply( &mat, &matWorld, &mat );
    D3DXMatrixTranspose( &mat, &mat );
    g_pd3dDevice->SetVertexShaderConstant( 0, &mat, 4 );
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
    // Clear the backbuffer and the zbuffer
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
                         D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );
    
    // Set state
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    // Draw the vertices using the vertex shader
    g_pd3dDevice->SetVertexShader( g_dwVertexShaderHandle );
    g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 1, g_Vertices, 
                                   sizeof(g_Vertices[0]) );

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}




//-----------------------------------------------------------------------------
// Name: main()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
void __cdecl main()
{
    // Initialize Direct3D
    if( FAILED( InitD3D() ) )
        return;

    // Enter render loop
    while( TRUE )
    {
        // Animate the scene
        FrameMove();
 
        // Render
        Render();
    }
}



