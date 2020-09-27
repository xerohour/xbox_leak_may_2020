//-----------------------------------------------------------------------------
// File: Textures.cpp
//
// Desc: Better than just lights and materials, 3D objects look much more
//       convincing when texture-mapped. Textures can be thought of as a sort
//       of wallpaper, that is shrinkwrapped to fit a texture. Textures are
//       typically loaded from image files, and D3DX provides a utility
//       function to do this for us. Like a vertex buffer, textures have
//       Lock() and Unlock() functions to access (read or write) the image
//       data. Textures have a width, height, miplevel, and pixel format. The
//       miplevel is for "mipmapped" textures, an advanced performance-
//       enhancing feature which uses lower resolutions of the texture for
//       objects in the distance where detail is less noticeable. The pixel
//       format determines how the colors are stored in a texel. The most
//       common formats are the 16-bit R5G6B5 format (5 bits of red, 6-bits of
//       green and 5 bits of blue) and the 32-bit A8R8G8B8 format (8 bits each
//       of alpha, red, green, and blue).
//
//       Textures are associated with geometry through texture coordinates.
//       Each vertex has one or more sets of texture coordinates, which are
//       named tu and tv and range from 0.0 to 1.0. Texture coordinates can be
//       supplied by the geometry, or can be automatically generated using
//       Direct3D texture coordinate generation (which is an advanced feature).
//
//       Textures start out as image files (.bmp, .tga, etc.) on the
//       development machine. For space and performance, it doesn't make sense
//       for the Xbox to load these image files, but rather load textures (and
//       other resources) in a native format. Therefore, these image files are
//       processed by the Bundler tool, which takes one or more resources and
//       packs them into a format that is fast and easy for the Xbox to load.
//       The resources (textures, in this case) to be bundled are referenced
//       in the resource.rdf file, and then they get bundled into the binary 
//       resource.xpr file that is loaded by this tutorial. See the bundler
//       tool for more information on it's usage.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <xgraphics.h>
#include <stdio.h>
#include "Resource.h"



// Use this define to show texture-coordinate generation
//#define SHOW_HOW_TO_USE_TCI




//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D8             g_pD3D       = NULL;          // Used to create the D3DDevice
LPDIRECT3DDEVICE8       g_pd3dDevice = NULL;          // Our rendering device
LPDIRECT3DVERTEXBUFFER8 g_pVB        = NULL;          // Buffer to hold vertices
BYTE*                   g_pResourceSysMemData = NULL; // Sysmem data for the packed resource
BYTE*                   g_pResourceVidMemData = NULL; // Vidmem data for the packed resource
LPDIRECT3DTEXTURE8      g_pTexture            = NULL; // Our texture


// A structure for our custom vertex type. We added texture coordinates
struct CUSTOMVERTEX
{
    D3DXVECTOR3 position; // The position
    D3DCOLOR    color;    // The color
    FLOAT       tu, tv;   // The texture coordinates
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)




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

    // Turn off culling
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    // Turn off D3D lighting
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    // Turn on the zbuffer
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: LoadPackedResource()
// Desc: Loads all the texture resources from the given XPR.
//-----------------------------------------------------------------------------
HRESULT LoadPackedResource()
{
    // Open the fileto read the XPR headers
	FILE* file = fopen( "D:\\media\\Resource.xpr", "rb" );
    if( NULL == file )
        return E_FAIL;

    // Read in and verify the XPR magic header
    XPR_HEADER xprh;
    if(fread( &xprh, sizeof(XPR_HEADER), 1, file) != 1)
	{
		fclose(file);
		return E_FAIL;
	}

    if( xprh.dwMagic != XPR_MAGIC_VALUE )
    {
        OutputDebugStringA( "ERROR: Invalid Xbox Packed Resource (.xpr) file" );
        fclose( file );
        return E_INVALIDARG;
    }

    // Compute memory requirements
    DWORD dwSysMemDataSize = xprh.dwHeaderSize - sizeof(XPR_HEADER);
    DWORD dwVidMemDataSize = xprh.dwTotalSize - xprh.dwHeaderSize;

    // Allocate memory
    g_pResourceSysMemData = new BYTE[dwSysMemDataSize];
    g_pResourceVidMemData = (BYTE*)D3D_AllocContiguousMemory( dwVidMemDataSize, D3DTEXTURE_ALIGNMENT );

    // Read in the data from the file
	if(fread( g_pResourceSysMemData, dwSysMemDataSize, 1, file ) != 1 ||
	   fread( g_pResourceVidMemData, dwVidMemDataSize, 1, file ) != 1 )
		
	{
		delete [] g_pResourceSysMemData;
		D3D_FreeContiguousMemory(g_pResourceVidMemData);
		fclose(file);
		return E_FAIL;
	}

    // Done with the file
    fclose( file );
    
    // Loop over resources, calling Register()
    BYTE* pData = g_pResourceSysMemData;

    for( DWORD i = 0; i < resource_NUM_RESOURCES; i++ )
    {
        // Get the resource
        LPDIRECT3DRESOURCE8 pResource = (LPDIRECT3DRESOURCE8)pData;

        // Register the resource
        pResource->Register( g_pResourceVidMemData );
    
        // Advance the pointer
        switch( pResource->GetType() )
        {
            case D3DRTYPE_TEXTURE:       pData += sizeof(D3DTexture);       break;
            case D3DRTYPE_VOLUMETEXTURE: pData += sizeof(D3DVolumeTexture); break;
            case D3DRTYPE_CUBETEXTURE:   pData += sizeof(D3DCubeTexture);   break;
            case D3DRTYPE_VERTEXBUFFER:  pData += sizeof(D3DVertexBuffer);  break;
            case D3DRTYPE_INDEXBUFFER:   pData += sizeof(D3DIndexBuffer);   break;
            case D3DRTYPE_PALETTE:       pData += sizeof(D3DPalette);       break;
            default:                     return E_FAIL;
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Create the textures and vertex buffers
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
    // Load the packed resource
    if( FAILED( LoadPackedResource() ) )
        return E_FAIL;

    // Get access to the texture
    g_pTexture = (LPDIRECT3DTEXTURE8)&g_pResourceSysMemData[ resource_Banana_OFFSET ];

    // Create the vertex buffer.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( 50*2*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &g_pVB ) ) )
    {
        OutputDebugStringA( "ERROR: Could not create vertex buffer\n" );
        return E_FAIL;
    }

    // Fill the vertex buffer. We are setting the tu and tv texture
    // coordinates, which range from 0.0 to 1.0
    CUSTOMVERTEX* pVertices;
    if( FAILED( g_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
        return E_FAIL;
    for( DWORD i=0; i<50; i++ )
    {
        FLOAT theta = (2*D3DX_PI*i)/(50-1);

        pVertices[2*i+0].position = D3DXVECTOR3( sinf(theta),-1.0f, cosf(theta) );
        pVertices[2*i+0].color    = 0xffffffff;
        pVertices[2*i+0].tu       = ((FLOAT)(50-i))/(50-1);
        pVertices[2*i+0].tv       = 1.0f;

        pVertices[2*i+1].position = D3DXVECTOR3( sinf(theta), 1.0f, cosf(theta) );
        pVertices[2*i+1].color    = 0xff808080;
        pVertices[2*i+1].tu       = ((FLOAT)(50-i))/(50-1);
        pVertices[2*i+1].tv       = 0.0f;
    }
    g_pVB->Unlock();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------
VOID SetupMatrices()
{
    // For our world matrix, we will just leave it as the identity
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixRotationX( &matWorld, timeGetTime()/1000.0f );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f, 3.0f,-5.0f ),
                                  &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ),
                                  &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
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

    // Setup the world, view, and projection matrices
    SetupMatrices();

    // Setup our texture. Using textures introduces the texture stage states,
    // which govern how textures get blended together (in the case of multiple
    // textures) and lighting information. In this case, we are modulating
    // (blending) our texture with the diffuse color of the vertices.
    g_pd3dDevice->SetTexture( 0, g_pTexture );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

#ifdef SHOW_HOW_TO_USE_TCI
    // Note: to use D3D texture coordinate generation, use the stage state
    // D3DTSS_TEXCOORDINDEX, as shown below. In this example, we are using
    // the position of the vertex in camera space to generate texture
    // coordinates. The tex coord index (TCI) parameters are passed into a
    // texture transform, which is a 4x4 matrix which transforms the x,y,z
    // TCI coordinates into tu, tv texture coordinates.

    // In this example, the texture matrix is setup to 
    // transform the texture from (-1,+1) position coordinates to (0,1) 
    // texture coordinate space:
    //    tu =  0.5*x + 0.5
    //    tv = -0.5*y + 0.5
    D3DXMATRIX mat;
    mat._11 = 0.25f; mat._12 = 0.00f;
    mat._21 = 0.00f; mat._22 =-0.25f;
    mat._31 = 0.00f; mat._32 = 0.00f;
    mat._41 = 0.50f; mat._42 = 0.50f;

    g_pd3dDevice->SetTransform( D3DTS_TEXTURE0, &mat );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
#endif

    // Render the vertex buffer contents
    g_pd3dDevice->SetStreamSource( 0, g_pVB, sizeof(CUSTOMVERTEX) );
    g_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
    g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2*50-2 );

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

    // Create the scene geometry
    if( FAILED( InitGeometry() ) )
        return;

    // Enter render loop
    while( TRUE )
    {
        Render();
    }
}



