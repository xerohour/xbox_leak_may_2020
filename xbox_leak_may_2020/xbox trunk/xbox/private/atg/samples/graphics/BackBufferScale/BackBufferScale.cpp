//-----------------------------------------------------------------------------
// File: BackBufferScale.cpp
//
// Desc: This sample shows how to use the SetBackBufferScale API to reduce
//       (via a scale factor) the effective size of the backbuffer. Scaling the
//       backbuffer dynamically lessens the fill requirements of the app, so
//       this is a technique that could be useful for fillbound apps that are
//       willing to sacrifice quality in order to maintain framerate.
//
//       To prevent tearing, note that the rendering device is created with
//       two backbuffers. After rendering a scene, the backbuffer is scaled and
//       copied to the next buffer in the chain, via the Swap() API. In between
//       the two swap calls, non-scaled elements (such as for UI objects) are
//       drawn.
//
// Hist: 11.29.01 - New for December XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBUtil.h>
#include <XBMesh.h>
#include "Resource.h" // Constants for bundled resources




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define NUM_TREES 500

// Custom vertex for the trees
struct TREEVERTEX
{
    D3DXVECTOR3 pos;
    D3DCOLOR    color;
    FLOAT       tu, tv;
};


struct Tree
{
    TREEVERTEX v[4];
};


inline FLOAT GaussianRand( FLOAT min, FLOAT max, FLOAT width )
{
    FLOAT x = (FLOAT)(rand()-rand())/RAND_MAX;
    return (max-min)*expf( -width*x*x) + min;
}




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBPackedResource  m_xprResource;        // Packed resources for the app
    CXBFont            m_Font;               // Font class

    LPDIRECT3DTEXTURE8 m_pTreeTexture;       // Tree images
    Tree*              m_Trees;              // Array of tree info

    FLOAT              m_fBackBufferScale;   // Amount to scale backbuffer by 

public:
    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();

    CXBoxSample();
};




//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CXBoxSample xbApp;
    if( FAILED( xbApp.Create() ) )
        return;
    xbApp.Run();
}




//-----------------------------------------------------------------------------
// Name: TreeSortCB()
// Desc: Callback function for sorting trees in back-to-front order
//-----------------------------------------------------------------------------
int _cdecl TreeSortCB( const VOID* arg1, const VOID* arg2 )
{
    Tree* p1 = (Tree*)arg1;
    Tree* p2 = (Tree*)arg2;

    if( p1->v[0].pos.z < p2->v[0].pos.z )
        return +1;

    return -1;
}




//-----------------------------------------------------------------------------
// Name: CXBoxSample()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
            :CXBApplication()
{
    // Allow unlimited frame rate
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // To prevent tearing, use 2 backbuffers
    m_d3dpp.BackBufferCount = 2; 

    // Initialize members
    m_pTreeTexture            = NULL;

    // Initialize the tree data
    m_Trees = new Tree[NUM_TREES];

    for( DWORD i=0; i<NUM_TREES; i++ )
    {
        // Position and size the trees randomly
        FLOAT y = 0.0f;
        FLOAT z = 5.0f + ( 60.0f * rand() ) / RAND_MAX;
        FLOAT x = ( 0.6f * z * (rand()-rand()) ) / RAND_MAX;
        FLOAT w = GaussianRand( 5.0f, 2.0f, 2.0f );
        FLOAT h = GaussianRand( 7.0f, 4.0f, 2.0f );

        // Each tree is a random color between red and green
        DWORD r = (DWORD)(255 * ( (0.25f) + (0.75f*rand())/RAND_MAX) );
        DWORD g = (DWORD)(255 * ( (0.25f) + (0.75f*rand())/RAND_MAX) );
        DWORD b = 0x00;
        DWORD color = (0xff<<24)|(r<<16)|(g<<8)|(b);

        m_Trees[i].v[0].pos = D3DXVECTOR3( x-w, 0, z );
        m_Trees[i].v[1].pos = D3DXVECTOR3( x-w, h, z );
        m_Trees[i].v[2].pos = D3DXVECTOR3( x+w, h, z );
        m_Trees[i].v[3].pos = D3DXVECTOR3( x+w, 0, z );
        m_Trees[i].v[0].color = color;
        m_Trees[i].v[1].color = color;
        m_Trees[i].v[2].color = color;
        m_Trees[i].v[3].color = color;
        m_Trees[i].v[0].tu = 0.0f;   m_Trees[i].v[0].tv = 1.0f; 
        m_Trees[i].v[1].tu = 0.0f;   m_Trees[i].v[1].tv = 0.0f; 
        m_Trees[i].v[2].tu = 1.0f;   m_Trees[i].v[2].tv = 0.0f; 
        m_Trees[i].v[3].tu = 1.0f;   m_Trees[i].v[3].tv = 1.0f; 
    }

    // Sort trees in back-to-front order (depends on global g_vEyePt)
    qsort( m_Trees, NUM_TREES, sizeof(Tree), TreeSortCB );
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: This creates all device-dependant display objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create the font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Get access to the tree texture
    m_pTreeTexture = m_xprResource.GetTexture( resource_Tree02s_OFFSET );

    // Set the transform matrices (view matrix is set in FrameMove())
    D3DXMATRIX  matWorld, matView, matProj;
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 7.0f,  0.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 5.0f, 10.0f );
    D3DXVECTOR3 vUp       = D3DXVECTOR3( 0.0f, 1.0f,  0.0f );
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUp );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 1000.0f );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Scale the viewport to reduce fill requirements
    FLOAT fIn = (m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] / 255.0f);
    m_fBackBufferScale = 1.0f - 0.90f * fIn; 

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Before we draw anything, scale the backbuffer
    m_pd3dDevice->SetBackBufferScale( m_fBackBufferScale, m_fBackBufferScale );

    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
                         0x000000ff, 1.0f, 0L );

    // Draw a bunch of trees to eat up fill
    {
        // Set up the default state
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
        m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

        // Set the tree texture
        m_pd3dDevice->SetTexture( 0, m_pTreeTexture );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

        // Turn on z-buffer, and disable alphatest for worse performance (yes,
        // we want worse performance, since we're trying to make this app
        // fill-limitied).
        m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    
        // Render the trees. With the above states, this should eat plenty of fill
        m_pd3dDevice->SetVertexShader( D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1 );
        m_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADLIST, NUM_TREES, m_Trees, sizeof(TREEVERTEX) );
    }

    // Instead of calling Present(), we call Swap() to swap our scaled-down
    // backbuffer to the next buffer in the chain, and then write UI and other
    // visual elements that we want drawn at full resolution.
    m_pd3dDevice->Swap( D3DSWAP_COPY );

    // Between the D3DSWAP_COPY and D3DSWAP_FINISH calls, draw text and any
    // other screenspace elements that we don't want affected by the viewport
    // scale.
    {
        WCHAR strScale[100];
        swprintf( strScale, L"Backbuffer is %ld x %ld", (DWORD)(m_fBackBufferScale*640), 
                                                        (DWORD)(m_fBackBufferScale*480) );

        m_Font.Begin();
        m_Font.DrawText(  64,  50, 0xffffffff, L"BackBufferScale" );
        m_Font.DrawText( 450,  50, 0xffcccc00, m_strFrameRate );
        m_Font.DrawText(  64,  75, 0xffcccc00, strScale );
        m_Font.DrawText(  64, 100, 0xff00ffff, L"Use the right trigger to change the backbuffer scale" );
        m_Font.End();
    }

    // After drawing non-scaled UI elements, finish the swap
    m_pd3dDevice->Swap( D3DSWAP_FINISH );

    return S_OK;
}



