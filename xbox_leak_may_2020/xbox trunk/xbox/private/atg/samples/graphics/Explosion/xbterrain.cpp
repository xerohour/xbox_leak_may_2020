//-----------------------------------------------------------------------------
// File: XBTerrain.cpp
//
// Desc: Implements the CXBTerrain object, which provides an application-
//       customizable terrain
//
// Hist: 04.03.01 - New for May XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "xbterrain.h"

//-----------------------------------------------------------------------------
// Name: CXBTerrain (constructor)
// Desc: Zeros out our members
//-----------------------------------------------------------------------------
CXBTerrain::CXBTerrain( )
{
    ZeroMemory( this, sizeof( CXBTerrain ) );
}


//-----------------------------------------------------------------------------
// Name: ~CXBTerrain (destructor)
// Desc: Releases resource held/allocated
//-----------------------------------------------------------------------------
CXBTerrain::~CXBTerrain()
{
    m_pd3dDevice->Release();
    m_pTexture->Release();
    if( m_pvbTerrain )
        m_pvbTerrain->Release();
    if( m_pibTerrain )
        delete[] m_pibTerrain;
}



//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: One-time initialization to set the device
//-----------------------------------------------------------------------------
HRESULT
CXBTerrain::Initialize( LPDIRECT3DDEVICE8 pDevice )
{
    m_pd3dDevice = pDevice;
    m_pd3dDevice->AddRef();

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Generate
// Desc: Sets up the terrain object's state based off the parameters.  Can set
//       # of slices in both X and Z, near X/Z corner, far X/Z corner, texture
//       height field function, and repetitions of the texture
//-----------------------------------------------------------------------------
HRESULT 
CXBTerrain::Generate( DWORD dwXSlices, 
                      DWORD dwZSlices,
                      D3DXVECTOR2 vXZMin, 
                      D3DXVECTOR2 vXZMax, 
                      LPDIRECT3DTEXTURE8 pTexture,
                      FNHEIGHTFUNCTION pfnHeight,
                      FLOAT fXRepeat,
                      FLOAT fZRepeat )
{
    HRESULT hr;
    DWORD dwNumVertices;
    XBTERRAIN_VERTEX * pVertices;
    FLOAT fX, fZ, fY;
    FLOAT fXUnit, fZUnit;
    DWORD x, z;
    D3DXVECTOR2 vWidth = vXZMax - vXZMin;

    //
    // Free up old resources
    //
    if( m_pvbTerrain )
    {
        m_pvbTerrain->Release();
        m_pvbTerrain = NULL;
    }
    if( m_pibTerrain )
    {
        delete[] m_pibTerrain;
        m_pibTerrain = NULL;
    }
    if( m_pTexture )
    {
        m_pTexture->Release();
        m_pTexture = NULL;
    }

    // Calculate how many vertices & indices we need
    dwNumVertices = ( dwXSlices + 1 ) * ( dwZSlices + 1 );
    m_dwNumIndices = 6 * dwXSlices * dwZSlices;

    // Create vertex buffer for terrain
    hr = m_pd3dDevice->CreateVertexBuffer( dwNumVertices * sizeof( XBTERRAIN_VERTEX ),
                                           0,
                                           0,
                                           0,
                                           &m_pvbTerrain );
    if( FAILED( hr ) )
        return hr;

    // Create index buffer for terrain
    m_pibTerrain = new WORD[ m_dwNumIndices ];
    if( m_pibTerrain == NULL )
    {
        m_pvbTerrain->Release();
        m_pvbTerrain = NULL;
        return E_OUTOFMEMORY;
    }

    // Now that our allocations have succeeded, we can get to business
    m_vXZMin            = vXZMin;
    m_vXZMax            = vXZMax;
    m_pfnHeight         = pfnHeight;
    m_fXTextureRepeat   = fXRepeat;
    m_fZTextureRepeat   = fZRepeat;
    m_pTexture          = pTexture;
    m_pTexture->AddRef();    

    // Caculate size of each grid square
    fXUnit = vWidth.x / dwXSlices;
    fZUnit = vWidth.y / dwZSlices;

    //
    // Fill vertex buffer
    //
    m_pvbTerrain->Lock( 0, 0, (BYTE **)&pVertices, NULL );
    for( z = 0; z <= dwZSlices; z++ )
    {
        // Calculate Z of top of terrain grid
        fZ = m_vXZMin.y + z * fZUnit;
        for( x = 0; x <= dwXSlices; x++ )
        {
            // Calculate X of left of terrain grid
            fX = m_vXZMin.x + x * fXUnit;

            // Get height of point
            fY = m_pfnHeight( fX, fZ );

            // Set up position and texture coordinates
            pVertices[ z * ( dwXSlices + 1 ) + x ].p = D3DXVECTOR3( fX, fY, fZ );
            pVertices[ z * ( dwXSlices + 1 ) + x ].t = D3DXVECTOR2( m_fXTextureRepeat * x / dwXSlices, m_fZTextureRepeat * z / dwZSlices );
        }
    }
    m_pvbTerrain->Unlock();

    //
    // Fill index buffer
    //
    for( z = 0; z < dwZSlices; z++ )
    {
        for( x = 0; x < dwXSlices; x++ )
        {
            DWORD dwSquare = z * dwXSlices + x;

            // Bottom Left -> Top Left -> Top Right
            m_pibTerrain[ dwSquare * 6 ]     = WORD( ( z + 1 ) * ( dwXSlices + 1 ) + x );
            m_pibTerrain[ dwSquare * 6 + 1 ] = WORD( z * ( dwXSlices + 1 ) + x );
            m_pibTerrain[ dwSquare * 6 + 2 ] = WORD( z * ( dwXSlices + 1 ) + x + 1 );

            // Bottom Left -> Top Right -> Bottom Right
            m_pibTerrain[ dwSquare * 6 + 3 ] = WORD( ( z + 1 ) * ( dwXSlices + 1 ) + x );
            m_pibTerrain[ dwSquare * 6 + 4 ] = WORD( z * ( dwXSlices + 1 ) + x + 1 );
            m_pibTerrain[ dwSquare * 6 + 5 ] = WORD( ( z + 1) * ( dwXSlices + 1 ) + ( x + 1 ) );
        }
    }

    return S_OK;
}


                      
//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the terrain
//-----------------------------------------------------------------------------
HRESULT 
CXBTerrain::Render()
{
    D3DXMATRIX matWorld, matWorldSave;

    // Save old transform
    m_pd3dDevice->GetTransform( D3DTS_WORLD, &matWorldSave );

    // Set identity for world transform
    D3DXMatrixIdentity( &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Set up to render
    m_pd3dDevice->SetStreamSource( 0, m_pvbTerrain, sizeof( XBTERRAIN_VERTEX ) );
    m_pd3dDevice->SetVertexShader( FVF_XBTERRAIN_VERTEX );
    m_pd3dDevice->SetPixelShader( NULL );
    m_pd3dDevice->SetTexture( 0, m_pTexture );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    // Render
    m_pd3dDevice->DrawIndexedVertices( D3DPT_TRIANGLELIST, m_dwNumIndices, m_pibTerrain );

    // Restore old state
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorldSave );

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: fDummy
// Desc: Dummy height-field function for flat terrain
//-----------------------------------------------------------------------------
FLOAT WINAPI fDummy( FLOAT fX, FLOAT fZ )
{
    return 0.0f;
}
