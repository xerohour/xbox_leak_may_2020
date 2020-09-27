//-----------------------------------------------------------------------------
// File: Billboard.cpp
//
// Desc: Example code showing how to do billboarding. The sample uses
//       billboarding to draw some trees.
//
//       Note: this implementation is for billboards that are fixed to rotate
//       about the Y-axis, which is good for things like trees. For
//       unconstrained billboards, like explosions in a flight sim, the
//       technique is the same, but the the billboards are positioned slightly
//       different (use the inverse of the view matrix, a vertex shader, 
//       pre-transformed vertices, or some other technique).
//
// Hist: 11.01.00 - New for November XDK release
//       12.15.00 - Changes for December XDK release
//       02.05.01 - Changes for XFest art talk
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBUtil.h>
#include <XBHelp.h>
#include <XBMesh.h>

// The following header file is generated from "Resource.rdf" file using the
// Bundler tool. In addition to the header, the tool outputs a binary file
// (Resource.xpr) which contains compiled (i.e. bundled) resources and is
// loaded at runtime using the CXBPackedResource class.
#include "Resource.h"




//-----------------------------------------------------------------------------
// ASCII names for the resources used by the app
//-----------------------------------------------------------------------------
XBRESOURCE g_ResourceNames[] = 
{
    { "SkyBoxXP.bmp", resource_SkyBoxXP_OFFSET },
    { "SkyBoxXN.bmp", resource_SkyBoxXN_OFFSET },
    { "SkyBoxYP.bmp", resource_SkyBoxYP_OFFSET },
    { "SkyBoxYN.bmp", resource_SkyBoxYN_OFFSET },
    { "SkyBoxZP.bmp", resource_SkyBoxZP_OFFSET },
    { "SkyBoxZN.bmp", resource_SkyBoxZN_OFFSET },
    { "Tree02s.tga",  resource_Tree02s_OFFSET },
    { "Hillside.bmp", resource_Hillside_OFFSET },
    { NULL, 0 },
};




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Move" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Rotate trees" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display\nhelp" },
};

#define NUM_HELP_CALLOUTS 3




//-----------------------------------------------------------------------------
// Name: Tree
// Desc: Simple structure to hold data for rendering a tree
//-----------------------------------------------------------------------------
struct Tree
{
    D3DXVECTOR4 vScale;
    D3DXVECTOR4 vPosition;
    D3DXVECTOR4 vColor;
};




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define NUM_TREES 2300

// Custom vertex for the trees
struct TREEVERTEX
{
    D3DXVECTOR3 p; // Vertex position
    D3DXVECTOR2 t; // Vertex texture coordinates
};

// Global access of the eye point used by the callback to sort trees
D3DXVECTOR3 g_vEyePt( 0.0f, 0.0f, 0.0f );
D3DXVECTOR3 g_vEyeDir( 0.0f, 0.0f, 1.0f );

// Simple function to define "hilliness" for terrain
inline FLOAT HeightField( FLOAT x, FLOAT y )
{
    return 3*(cosf(x/20+0.2f)*cosf(y/15-0.2f)+1.0f);
}

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
    CXBHelp            m_Help;               // Help class
    BOOL               m_bDrawHelp;          // Whether to draw help

    CXBMesh            m_TerrainObject;      // Terrain object
    CXBMesh            m_SkyBoxObject;       // Skybox background object

    LPDIRECT3DTEXTURE8 m_pTreeTexture;       // Tree images
    D3DXMATRIX         m_matBillboardMatrix; // Used for billboard orientation
    Tree*              m_Trees;              // Array of tree info

    Tree**             m_pTreeArray;         // Array of tree info

    DWORD              m_dwBillboardVertexShader;

    LPDIRECT3DVERTEXBUFFER8 m_pTreeVB;

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
// Name: CXBoxSample()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
            :CXBApplication()
{
    // Allow unlimited frame rate
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Initialize members
    m_bDrawHelp               = FALSE;
    m_dwBillboardVertexShader = 0L;
    m_pTreeTexture            = NULL;

    // Initialize the tree data
    m_Trees      = new Tree[NUM_TREES];
    m_pTreeArray = new Tree*[NUM_TREES];

    DWORD side = (DWORD)sqrtf( NUM_TREES );

    for( DWORD i=0; i<NUM_TREES; i++ )
    {
        // Position the trees
        FLOAT x = 60.0f * ( (FLOAT)(i%side) - (FLOAT)(side/2) ) / (FLOAT)(side/2);
        FLOAT z = 60.0f * ( (FLOAT)(i/side) - (FLOAT)(side/2) ) / (FLOAT)(side/2);

        x += 60.0f * 0.1f * ((FLOAT)(rand()-rand())/RAND_MAX) / (FLOAT)(side/2);
        z += 60.0f * 0.1f * ((FLOAT)(rand()-rand())/RAND_MAX) / (FLOAT)(side/2);
        
        FLOAT y = HeightField( x, z );

        // Size the trees randomly
        FLOAT fWidth  = GaussianRand( 5.0f, 2.0f, 2.0f );
        FLOAT fHeight = GaussianRand( 7.0f, 4.0f, 2.0f );

        // Each tree is a random color between red and green
        FLOAT r = (1.0f-0.75f) + (0.75f*rand())/RAND_MAX;
        FLOAT g = (1.0f-0.75f) + (0.75f*rand())/RAND_MAX;
        FLOAT b = 0.0f;

        m_Trees[i].vPosition      = D3DXVECTOR4( x, y, z, 1.0f );
        m_Trees[i].vColor         = D3DXVECTOR4( r, g, b, 1.0f );
        m_Trees[i].vScale         = D3DXVECTOR4( fWidth, fHeight, 1.0f, 1.0f );

        m_pTreeArray[i] = &m_Trees[i];
    }
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

    // Create the help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the tree textures
    m_pTreeTexture = m_xprResource.GetTexture( "Tree02s.tga" );

    // Load the skybox
    if( FAILED( m_SkyBoxObject.Create( m_pd3dDevice, "Models\\SkyBox.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load the terrain
    if( FAILED( m_TerrainObject.Create( m_pd3dDevice, "Models\\Hillside.xbg", &m_xprResource  ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Add some "hilliness" to the terrain
    XBMESH_DATA* pMesh = m_TerrainObject.GetMesh(0);
    if( pMesh )
    {
        struct VERTEX { D3DXVECTOR3 p, n; FLOAT tu,tv; };
        VERTEX* pVertices;
        DWORD   dwNumVertices = pMesh->m_dwNumVertices;

        pMesh->m_VB.Lock( 0, 0, (BYTE**)&pVertices, 0 );

        for( DWORD i = 0; i < dwNumVertices; i++ )
        {
            pVertices[i].p.y = HeightField( pVertices[i].p.x, pVertices[i].p.z );
            pVertices[i].tu *= 5.0f;
            pVertices[i].tv *= 5.0f;
        }

        pMesh->m_VB.Unlock();
    }

    // Set the transform matrices (view matrix is set in FrameMove())
    D3DXMATRIX  matWorld, matProj;
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set up the default texture states
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      TRUE );

    // Set up a light
    D3DLIGHT8 light;
    XBUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, 1.0f, -1.0f, 0.0f );
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  0xff444444 );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // Create vertex shader for the billboards
    DWORD dwBillboardVertexDecl[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),   // v0 = Position
        D3DVSD_REG( 3, D3DVSDT_FLOAT2 ),   // v3 = Texture coords
        D3DVSD_END()
    };

    if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\Billbrd.xvu",
                                           dwBillboardVertexDecl,
                                           &m_dwBillboardVertexShader ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    if( FAILED( m_pd3dDevice->CreateVertexBuffer( 4*sizeof(TREEVERTEX), 0, 0,
                                                  D3DPOOL_DEFAULT, &m_pTreeVB ) ) )
        return E_FAIL;

    TREEVERTEX* pVertices;
    m_pTreeVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );
    pVertices[0].p = D3DXVECTOR3(-0.5f, 0.0f, 0.0f ); pVertices[0].t = D3DXVECTOR2( 0, 1 );
    pVertices[1].p = D3DXVECTOR3(-0.5f, 1.0f, 0.0f ); pVertices[1].t = D3DXVECTOR2( 0, 0 );
    pVertices[2].p = D3DXVECTOR3( 0.5f, 1.0f, 0.0f ); pVertices[2].t = D3DXVECTOR2( 1, 0 );
    pVertices[3].p = D3DXVECTOR3( 0.5f, 0.0f, 0.0f ); pVertices[3].t = D3DXVECTOR2( 1, 1 );
    m_pTreeVB->Unlock();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: TreeSortCB()
// Desc: Callback function for sorting trees in back-to-front order
//-----------------------------------------------------------------------------
int _cdecl TreeSortCB( const VOID* arg1, const VOID* arg2 )
{
    Tree* p1 = *((Tree**)arg1);
    Tree* p2 = *((Tree**)arg2);

    FLOAT dx1 = p1->vPosition.x - g_vEyePt.x;
    FLOAT dx2 = p2->vPosition.x - g_vEyePt.x;
    FLOAT dz1 = p1->vPosition.z - g_vEyePt.z;
    FLOAT dz2 = p2->vPosition.z - g_vEyePt.z;

    if( (dx1*dx1+dz1*dz1) < (dx2*dx2+dz2*dz2) )
        return +1;

    return -1;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        m_bDrawHelp = !m_bDrawHelp;

    // Position of camera along the path (the path is a big circle)
    static FLOAT fPathPostion = 0.0f;
    fPathPostion -= m_DefaultGamepad.fY1*m_fElapsedTime*1.00f;

    // User adjusted viewing angle (using GamePad)
    static FLOAT fViewAngle = 0.0f;
    fViewAngle = 0.9f * ( fViewAngle + m_DefaultGamepad.fX1*0.05f );
    
    // User adjusted angle to rotate the trees (using GamePad)
    static FLOAT fTreeRotate = 0.0f;
    fTreeRotate = 0.9f * ( fTreeRotate + m_DefaultGamepad.fX2*0.10f );

    // Set the app view matrix using the new eye point, eye direction, and
    // lookat point
    g_vEyePt.x = 30.0f*sinf( fPathPostion );
    g_vEyePt.z = 30.0f*cosf( fPathPostion );
    g_vEyePt.y = 6 + HeightField( g_vEyePt.x, g_vEyePt.z );

    g_vEyeDir.x = -cosf(fPathPostion+fViewAngle);
    g_vEyeDir.y = 0.0f;
    g_vEyeDir.z = +sinf(fPathPostion+fViewAngle);

    D3DXVECTOR3 vLookatPt;
    vLookatPt   = g_vEyePt + 10*g_vEyeDir;
    vLookatPt.y = g_vEyePt.y - 1.0f;

    // Sort trees in back-to-front order (depends on global g_vEyePt)
    qsort( m_pTreeArray, NUM_TREES, sizeof(Tree*), TreeSortCB );

    // Set up the view matrix
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &g_vEyePt, &vLookatPt, &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Set up a rotation matrix to orient the billboard towards the camera.
    D3DXMATRIX matBillboard;
    D3DXMatrixRotationY( &matBillboard, fPathPostion+fViewAngle+fTreeRotate-D3DX_PI/2 );
    
    // Set the vertex shader constants (note that matrices must be transposed)
    D3DXMATRIX matProj, matViewProj, matViewProjTranspose, matBillboardTranspose;
    m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
    D3DXMatrixMultiply( &matViewProj, &matView, &matProj );
    D3DXMatrixTranspose( &matBillboardTranspose, &matBillboard );
    D3DXMatrixTranspose( &matViewProjTranspose,  &matViewProj );
    m_pd3dDevice->SetVertexShaderConstant( 4,  &matBillboardTranspose, 4 );
    m_pd3dDevice->SetVertexShaderConstant( 12, &matViewProjTranspose,  4 );

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
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
                         0x00000000, 1.0f, 0L );

    // Set up the default state
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,        D3DCMP_LESSEQUAL );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  0xff444444 );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // Render the Skybox
    {
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
        m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
        
        // Center view matrix for skybox and disable zbuffer
        D3DXMATRIX matView, matViewSave;
        m_pd3dDevice->GetTransform( D3DTS_VIEW,      &matViewSave );
        matView = matViewSave;
        matView._41 = 0.0f; matView._42 = -0.0f; matView._43 = 0.0f;
        m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

        // Render the skybox
        m_SkyBoxObject.Render( m_pd3dDevice );

        // Restore the render states
        m_pd3dDevice->SetTransform( D3DTS_VIEW, &matViewSave );
        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    }

    // Draw the terrain
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
    m_TerrainObject.Render( m_pd3dDevice );

    // Draw the trees
    {
        // Set diffuse blending for alpha set in vertices.
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

        // Enable alpha testing (skips pixels with less than a certain alpha.)
        m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

        // Set the tree texture
        m_pd3dDevice->SetTexture( 0, m_pTreeTexture );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

        // Set the billboarding vertex shader. This does the transformation and
        // lighting, including the rotation to orient the billboard towards the
        // camera
        m_pd3dDevice->SetVertexShader( m_dwBillboardVertexShader );
        m_pd3dDevice->SetStreamSource( 0, m_pTreeVB, sizeof(TREEVERTEX) );

        // Loop through and render all trees

        // First, render front-to-back the opaque pixels
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,  0x80 );
        for( DWORD i=0; i<NUM_TREES; i++ )
        {
            Tree* pTree = m_pTreeArray[(NUM_TREES-1)-i];
            m_pd3dDevice->SetVertexShaderConstant( 0, &pTree->vPosition, 1 );
            m_pd3dDevice->SetVertexShaderConstant( 1, &pTree->vColor,    1 );
            m_pd3dDevice->SetVertexShaderConstant( 2, &pTree->vScale,    1 );
            m_pd3dDevice->DrawPrimitive( D3DPT_QUADLIST, 0, 1 );
        }

        // The render back to front, the alpha pixels
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,  0x08 );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
        m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESS );
        for( i=0; i<NUM_TREES; i++ )
        {
            Tree* pTree = m_pTreeArray[i];
            m_pd3dDevice->SetVertexShaderConstant( 0, &pTree->vPosition, 1 );
            m_pd3dDevice->SetVertexShaderConstant( 1, &pTree->vColor,    1 );
            m_pd3dDevice->SetVertexShaderConstant( 2, &pTree->vScale,    1 );
            m_pd3dDevice->DrawPrimitive( D3DPT_QUADLIST, 0, 1 );
        }

        // Restore state
        m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    }

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.DrawText(  64, 50, 0xff808000, L"BillBoard" );
        m_Font.DrawText( 450, 50, 0xff808000, m_strFrameRate );
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}



