//-----------------------------------------------------------------------------
// File: Meshes.cpp
//
// Desc: For advanced geometry, most apps will prefer to load pre-authored
//       meshes from a file. An ideal file format would have geometry that are
//       really just binary dumps of pre-built index- and vertex-buffers, so
//       that costly load time file processing can be avoided. Samples on the
//       Xbox XDK use a custom formats called .XBG (for "Xbox geometry"). The
//       geometry files start out as .X files, then are processed by the 
//       MakeXBG tool, where they can be stripified and have their FVF code
//       modified. In the .xbg file format, textures are referenced by ASCII
//       name, so they can be loaded by a texture manager (which would be based
//       on "bundled" resources. See the Textures tutorial for more info.). The
//       .XBG format also contains embedded structures for info on the
//       rendering subsets, and also support for heirachial transformations.
//
//       A real Xbox game will likely use a custom format, but should still have
//       a number of similiarities to the .XBG format...most notably that any
//       file format will likely boil down to a set up simple structures that
//       wrap binary dumps of pre-built index- and vertex-buffer resources.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <xgraphics.h>
#include <stdio.h>
#include "Resource.h"




//-----------------------------------------------------------------------------
// Name: struct XBMESH_SUBSET
// Desc: Struct to hold data for rendering a mesh
//-----------------------------------------------------------------------------
struct XBMESH_SUBSET
{
    D3DMATERIAL8       mtrl;            // Material for this subset
    LPDIRECT3DTEXTURE8 pTexture;        // Texture
    CHAR               strTexture[64];
    DWORD              dwVertexStart;   // Range of vertices to render
    DWORD              dwVertexCount;
    DWORD              dwIndexStart;    // Range of vertex indices to render
    DWORD              dwIndexCount;
};




//-----------------------------------------------------------------------------
// Name: struct XBMESH_DATA
// Desc: Struct for mesh data
//-----------------------------------------------------------------------------
struct XBMESH_DATA
{
    D3DVertexBuffer   m_VB;            // Mesh geometry
    DWORD             m_dwNumVertices;
    D3DIndexBuffer    m_IB;
    DWORD             m_dwNumIndices;
    
    DWORD             m_dwFVF;         // Mesh vertex info
    DWORD             m_dwVertexSize;
    D3DPRIMITIVETYPE  m_dwPrimType;

    DWORD             m_dwNumSubsets;  // Subset info for rendering calls
    XBMESH_SUBSET*    m_pSubsets;
};




//-----------------------------------------------------------------------------
// Name: struct XBMESHFRAME
// Desc: Struct for building a hierarchy of meshes.
//-----------------------------------------------------------------------------
__declspec(align(16)) struct XBMESH_FRAME
{
    D3DXMATRIX        m_matTransform; // The transformation matrix for this frame
    
    XBMESH_DATA       m_MeshData;     // The mesh data belonging to this frame

    CHAR              m_strName[64];
    
    XBMESH_FRAME*     m_pChild;       // Child and sibling ptrs for the hierarchy
    XBMESH_FRAME*     m_pNext;
};




//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D8             g_pD3D                = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE8       g_pd3dDevice          = NULL; // Our rendering device

BYTE*                   g_pResourceSysMemData = NULL; // Sysmem data for the packed resource
BYTE*                   g_pResourceVidMemData = NULL; // Vidmem data for the packed resource

VOID*                   g_pMeshSysMemData     = NULL; // Sysmem data for the mesh
VOID*                   g_pMeshVidMemData     = NULL; // Vidmem data for the mesh
XBMESH_FRAME*           g_pMeshFrames         = NULL; // Num of hierarchial frames in the mesh
DWORD                   g_dwNumMeshFrames     = 0L;   // Heirarchy of frames for the mesh




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
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;

    // Create the Direct3D device.
    if( FAILED( g_pD3D->CreateDevice( 0, D3DDEVTYPE_HAL, NULL,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
        return E_FAIL;

    // Turn on the zbuffer
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

    // Turn on ambient lighting 
    g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: LoadPackedResource()
// Desc: Loads all the texture resources from the given XPR.
//-----------------------------------------------------------------------------
HRESULT LoadPackedResource( const CHAR* strResourceFilename )
{
    // Open the fileto read the XPR headers
    FILE* file = fopen( strResourceFilename, "rb" );
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
// Name: FindResourceByName()
// Desc: Loads all the texture resources from the given XPR.
//-----------------------------------------------------------------------------
VOID* FindResourceByName( const CHAR* strName )
{
    if( NULL==g_pResourceSysMemData || NULL==strName )
        return NULL;

    // For this simple tutorial, this is hard-coded.
    if( !_stricmp( strName, "Tiger.bmp" ) )
        return &g_pResourceSysMemData[resource_Tiger_OFFSET];

    return NULL;
}




//-----------------------------------------------------------------------------
// Name: LoadXBGFile()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT LoadXBGFile( const CHAR* strMeshFilename )
{
    // Open the file
    FILE* file = fopen( strMeshFilename, "rb" );
    if( file == NULL )
    {
        OutputDebugStringA( "ERROR: Mesh file not found!\n" );
        return E_FAIL;
    }

    // Read the magic number
    DWORD dwFileID;
    fread( &dwFileID, 1, sizeof(DWORD), file ); 

    // The magic number to identify .xbg files
    const DWORD XBG_FILE_ID  = (((DWORD)'X'<<0)|(((DWORD)'B'<<8))|(((DWORD)'G'<<16))|(2<<24));

    if( dwFileID != XBG_FILE_ID )
    {
        OutputDebugStringA( "ERROR: Invalid XBG file type!\n" );
        fclose( file );
        return E_FAIL;
    }

    // Read in header
    DWORD dwNumFrames;  // Number of mesh frames in the file
    DWORD dwSysMemSize; // Num bytes needed for system memory objects
    DWORD dwVidMemSize; // Num bytes needed for video memory objects

    fread( &dwNumFrames,  1, sizeof(DWORD), file );
    fread( &dwSysMemSize, 1, sizeof(DWORD), file );
    fread( &dwVidMemSize, 1, sizeof(DWORD), file );

    // Read in system memory objects
    g_pMeshSysMemData = (VOID*)new BYTE[dwSysMemSize];
    fread( g_pMeshSysMemData, dwSysMemSize, 1, file );

    // Read in video memory objects
    g_pMeshVidMemData = D3D_AllocContiguousMemory( dwVidMemSize, D3DVERTEXBUFFER_ALIGNMENT );
    fread( g_pMeshVidMemData, dwVidMemSize, 1, file ); 
    
    // Done with the file
    fclose( file );

    // Now we need to patch the mesh data. Any pointers read from the file were
    // stored as file offsets. So, we simply need to add a base address to patch
    // things up.
    g_pMeshFrames = (XBMESH_FRAME*)g_pMeshSysMemData;
    g_dwNumMeshFrames = dwNumFrames;

    for( DWORD i=0; i<g_dwNumMeshFrames; i++ )
    {
        XBMESH_FRAME* pFrame = &g_pMeshFrames[i];
        XBMESH_DATA*  pMesh  = &g_pMeshFrames[i].m_MeshData;

        if( pFrame->m_pChild )
            pFrame->m_pChild  = (XBMESH_FRAME*)( (DWORD)pFrame->m_pChild - 16 + (DWORD)g_pMeshFrames );
        if( pFrame->m_pNext )
            pFrame->m_pNext   = (XBMESH_FRAME*)( (DWORD)pFrame->m_pNext  - 16 + (DWORD)g_pMeshFrames );
        if( pMesh->m_pSubsets )
            pMesh->m_pSubsets = (XBMESH_SUBSET*)( (DWORD)pMesh->m_pSubsets - 16 + (DWORD)g_pMeshFrames);
        
        if( pMesh->m_dwNumIndices )
            pMesh->m_IB.Data  = pMesh->m_IB.Data - 16 + (DWORD)g_pMeshFrames;
        if( pMesh->m_dwNumVertices )
            pMesh->m_VB.Register( g_pMeshVidMemData );
    }

    // Finally, create any textures used by the meshes' subsets. In this 
    // implementation, we are pulling textures out of the global resource.
    for( DWORD i=0; i<g_dwNumMeshFrames; i++ )
    {
        XBMESH_DATA* pMesh = &g_pMeshFrames[i].m_MeshData;

        for( DWORD j = 0; j < pMesh->m_dwNumSubsets; j++ )
        {
            XBMESH_SUBSET* pSubset = &pMesh->m_pSubsets[j];

            pSubset->pTexture = (LPDIRECT3DTEXTURE8)FindResourceByName( pSubset->strTexture );
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Load the mesh and build the material and texture arrays
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
    // Load the packed resource (containing the app's textures)
    if( FAILED( LoadPackedResource( "D:\\Media\\Resource.xpr" ) ) )
        return E_FAIL;

    // Load geometry from the XBG file
    if( FAILED( LoadXBGFile( "D:\\Media\\Tiger.xbg" ) ) )
        return E_FAIL;

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
    D3DXMatrixRotationY( &matWorld, timeGetTime()/1000.0f );
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
// Name: RenderMesh()
// Desc: Renders the mesh geometry.
//-----------------------------------------------------------------------------
HRESULT RenderMesh( LPDIRECT3DDEVICE8 pd3dDevice, XBMESH_DATA* pMesh )
{
    if( pMesh->m_dwNumVertices == 0 )
        return S_OK;

    // Set the vertex stream
    pd3dDevice->SetStreamSource( 0, &pMesh->m_VB, pMesh->m_dwVertexSize );
    pd3dDevice->SetIndices( &pMesh->m_IB, 0 );

    // Set the FVF code, unless the user asked us not to
    pd3dDevice->SetVertexShader( pMesh->m_dwFVF );

    // Render the subsets
    for( DWORD i = 0; i < pMesh->m_dwNumSubsets; i++ )
    {
        // Set the material
        pd3dDevice->SetMaterial( &pMesh->m_pSubsets[i].mtrl );

        // Set the texture.
        pd3dDevice->SetTexture( 0, pMesh->m_pSubsets[i].pTexture );

        // Draw the mesh subset
        if( D3DPT_TRIANGLESTRIP == pMesh->m_dwPrimType )
            pd3dDevice->DrawIndexedPrimitive( pMesh->m_dwPrimType, 0, 
                                              pMesh->m_pSubsets[i].dwIndexCount,
                                              pMesh->m_pSubsets[i].dwIndexStart, 
                                              pMesh->m_pSubsets[i].dwIndexCount-2 );
        else // D3DPT_TRIANGLELIST
            pd3dDevice->DrawIndexedPrimitive( pMesh->m_dwPrimType, 0, 
                                              pMesh->m_pSubsets[i].dwIndexCount,
                                              pMesh->m_pSubsets[i].dwIndexStart, 
                                              pMesh->m_pSubsets[i].dwIndexCount/3 );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderFrame()
// Desc: Renders a frame (save state, apply matrix, render children, restore).
//-----------------------------------------------------------------------------
HRESULT RenderFrame( LPDIRECT3DDEVICE8 pd3dDevice, XBMESH_FRAME* pFrame )
{
    // Apply the frame's local transform
    D3DXMATRIX matSavedWorld, matWorld;
    pd3dDevice->GetTransform( D3DTS_WORLD, &matSavedWorld );
    D3DXMatrixMultiply( &matWorld, &pFrame->m_matTransform, &matSavedWorld );
    pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Render the mesh data
    if( pFrame->m_MeshData.m_dwNumSubsets ) 
        RenderMesh( pd3dDevice, &pFrame->m_MeshData );

    // Render any child frames
    if( pFrame->m_pChild ) 
        RenderFrame( pd3dDevice, pFrame->m_pChild );

    // Restore the transformation matrix
    pd3dDevice->SetTransform( D3DTS_WORLD, &matSavedWorld );
    
    // Render any sibling frames
    if( pFrame->m_pNext )  
        RenderFrame( pd3dDevice, pFrame->m_pNext );

    return S_OK;
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

    // Render the first frame in the mesh. This will render the entire 
    // hierarchy (in any) of frames and meshes for the loaded geometry.
    RenderFrame( g_pd3dDevice, g_pMeshFrames );

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



