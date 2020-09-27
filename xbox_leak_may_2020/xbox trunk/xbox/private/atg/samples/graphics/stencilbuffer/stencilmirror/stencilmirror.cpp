//-----------------------------------------------------------------------------
// File: StencilMirror.cpp
//
// Desc: Example code showing how to use stencil buffers to implement planar
//       mirrors.
//
// Hist: 11.01.00 - New for November XDK release
//       12.15.00 - Changes for December XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBMesh.h>
#include <XBResource.h>
#include <XBUtil.h>

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
    { "Seafloor.bmp", resource_Seafloor_OFFSET },
    { "BiHull.bmp",   resource_BiHull_OFFSET },
    { "Wings.bmp",    resource_Wings_OFFSET },
    { NULL, 0 },
};




//-----------------------------------------------------------------------------
// Custom vertex types
//-----------------------------------------------------------------------------
struct MESHVERTEX
{
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
    FLOAT       tu, tv;
};

#define D3DFVF_MESHVERTEX   (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

// Fog color
#define FOG_COLOR 0xff000000

// Function for creating a height field
FLOAT HeightField( FLOAT x, FLOAT z )
{
    FLOAT y = 0.0f;
    y += 7.0f * cosf( 0.051f*x + 0.0f ) * sinf( 0.055f*x + 0.0f );
    y += 7.0f * cosf( 0.053f*z + 0.0f ) * sinf( 0.057f*z + 0.0f );
    y += 1.0f * cosf( 0.101f*x + 0.0f ) * sinf( 0.105f*x + 0.0f );
    y += 1.0f * cosf( 0.103f*z + 0.0f ) * sinf( 0.107f*z + 0.0f );
    y += 1.0f * cosf( 0.251f*x + 0.0f ) * sinf( 0.255f*x + 0.0f );
    y += 1.0f * cosf( 0.253f*z + 0.0f ) * sinf( 0.257f*z + 0.0f );
    return y*1.0f-10.0f;
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

    D3DXVECTOR3  m_vMirror[4];               // Vertices for the mirror
    D3DMATERIAL8 m_mtrlMirrorMaterial;       // Material of the mirror
    D3DXMATRIX   m_matMirrorMatrix;          // Matrix to position mirror
    BOOL         m_bRenderingReflectedScene; // Whether we are rendering the reflection

    CXBMesh      m_Terrain;                  // X file of terrain
    D3DXMATRIX   m_matTerrainMatrix;         // Matrix to position terrain

    CXBMesh      m_Airplane;                 // X file object to render
    D3DXMATRIX   m_matAirplaneMatrix;        // Matrix to animate X file object

    HRESULT RenderScene();
    HRESULT RenderMirror();

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
    // Override base class member variables
    m_d3dpp.BackBufferFormat       = D3DFMT_X8R8G8B8;
    m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

    // Initialize member variables
    m_vMirror[0] = D3DXVECTOR3(-80.0f, 0.0f,-80.0f );
    m_vMirror[1] = D3DXVECTOR3(-80.0f, 0.0f, 80.0f );
    m_vMirror[2] = D3DXVECTOR3( 80.0f, 0.0f,-80.0f );
    m_vMirror[3] = D3DXVECTOR3( 80.0f, 0.0f, 80.0f );
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create the font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load an object to cast the shadow
    if( FAILED( m_Airplane.Create( m_pd3dDevice, "Models\\Airplane.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load some terrain
    if( FAILED( m_Terrain.Create( m_pd3dDevice, "Models\\SeaFloor.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Tweak the terrain vertices to add some bumpy terrain
    {
        // Get access to the mesh vertices
        LPDIRECT3DVERTEXBUFFER8 pVB = &m_Terrain.GetMesh(0)->m_VB;
        DWORD dwNumVertices = m_Terrain.GetMesh(0)->m_dwNumVertices;
        MESHVERTEX* pVertices;
        pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );

        for( DWORD i=0; i<dwNumVertices; i++ )
        {
            D3DXVECTOR3 v00( pVertices[i].p.x + 0.0f, 0.0f, pVertices[i].p.z + 0.0f );
            D3DXVECTOR3 v10( pVertices[i].p.x + 0.1f, 0.0f, pVertices[i].p.z + 0.0f );
            D3DXVECTOR3 v01( pVertices[i].p.x + 0.0f, 0.0f, pVertices[i].p.z + 0.1f );
            v00.y = HeightField( 1*v00.x, 1*v00.z );
            v10.y = HeightField( 1*v10.x, 1*v10.z );
            v01.y = HeightField( 1*v01.x, 1*v01.z );

            D3DXVECTOR3 n;
            D3DXVec3Cross( &n, &(v01-v00), &(v10-v00) );
            D3DXVec3Normalize( &n, &n );

            pVertices[i].p.y  = v00.y;
            pVertices[i].n.x  = n.x;
            pVertices[i].n.y  = n.y;
            pVertices[i].n.z  = n.z;
            pVertices[i].tu  *= 10;
            pVertices[i].tv  *= 10;
        }

        pVB->Unlock();
    }

    // Set the transform matrices
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 5.5f, -15.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 1.5f,   0.0f  );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f,   0.0f  );
    D3DXMATRIX matWorld, matView, matProj;

    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 1000.0f );

    m_pd3dDevice->SetTransform( D3DTS_WORLD,      &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_VIEW,       &matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set up a material
    D3DMATERIAL8 mtrl;
    XBUtil_InitMaterial( mtrl, 0.5f, 0.8f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );
    XBUtil_InitMaterial( m_mtrlMirrorMaterial, 0.0f, 0.5f, 0.5f );

    // Set up the light
    D3DLIGHT8 light;
    XBUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, 0.0f, -0.4f, 1.0f );
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Position the terrain
    D3DXMatrixTranslation( &m_matTerrainMatrix, 0.0f, 8.0f, 0.0f );

    // Position the mirror (water polygon intersecting with terrain)
    D3DXMatrixTranslation( &m_matMirrorMatrix, 0.0f, 0.0f, 0.0f );

    // Position and animate the main object
    FLOAT fObjectPosX = 50.0f*sinf(m_fAppTime/2);
    FLOAT fObjectPosY = 6;
    FLOAT fObjectPosZ = 10.0f*cosf(m_fAppTime/2);
    D3DXMATRIX matRoll, matPitch, matRotate, matScale, matTranslate;
    D3DXMatrixRotationZ( &matRoll, 0.2f*sinf(m_fAppTime/2)  );
    D3DXMatrixRotationY( &matRotate, m_fAppTime/2-D3DX_PI/2  );
    D3DXMatrixRotationX( &matPitch, -0.1f * (1+cosf(m_fAppTime))  );
    D3DXMatrixScaling( &matScale, 0.5f, 0.5f, 0.5f );
    D3DXMatrixTranslation( &matTranslate, fObjectPosX, fObjectPosY, fObjectPosZ );
    D3DXMatrixMultiply( &m_matAirplaneMatrix, &matScale, &matTranslate );
    D3DXMatrixMultiply( &m_matAirplaneMatrix, &matRoll, &m_matAirplaneMatrix );
    D3DXMatrixMultiply( &m_matAirplaneMatrix, &matRotate, &m_matAirplaneMatrix );
    D3DXMatrixMultiply( &m_matAirplaneMatrix, &matPitch, &m_matAirplaneMatrix );

    // Move the camera around
    FLOAT fEyeX = 10.0f * sinf( m_fAppTime/2.0f );
    FLOAT fEyeY =  3.0f * sinf( m_fAppTime/25.0f ) + 13.0f;
    FLOAT fEyeZ =  5.0f * cosf( m_fAppTime/2.0f );
    D3DXMATRIX matView;
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( fEyeX, fEyeY, fEyeZ );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( fObjectPosX, fObjectPosY, fObjectPosZ );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderScene()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RenderScene()
{
    // Render terrain
    if( FALSE == m_bRenderingReflectedScene )
    {
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matTerrainMatrix );
        m_Terrain.Render( m_pd3dDevice );
    }

    // Draw the mirror
    if( FALSE == m_bRenderingReflectedScene )
    {
        m_pd3dDevice->SetTexture( 0, NULL );
        m_pd3dDevice->SetMaterial( &m_mtrlMirrorMaterial );
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matMirrorMatrix );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,     D3DBLEND_DESTCOLOR );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,    D3DBLEND_ZERO );
        m_pd3dDevice->SetVertexShader( D3DFVF_XYZ );
        m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_vMirror, 
                                       sizeof(D3DXVECTOR3) );
    }

    // Restore states
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

    // Draw the object. Note: do this last, in case the object has alpha
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matAirplaneMatrix );
    m_Airplane.Render( m_pd3dDevice );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderMirror()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RenderMirror()
{
    // Turn depth buffer off, and stencil buffer on
    m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILFUNC,     D3DCMP_ALWAYS );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILREF,      0x1 );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILMASK,     0xffffffff );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILWRITEMASK,0xffffffff );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILPASS,  D3DSTENCILOP_REPLACE );

    // Make sure no pixels are written to the z-buffer or frame buffer
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );

    // Draw the reflecting surface into the stencil buffer
    m_pd3dDevice->SetTexture( 0, NULL);
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matMirrorMatrix );
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZ );
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_vMirror, 
                                   sizeof(D3DXVECTOR3) );
    
    // Save the view matrix
    D3DXMATRIX matViewSaved;
    m_pd3dDevice->GetTransform( D3DTS_VIEW, &matViewSaved );

    // Reflect camera in X-Z plane mirror
    D3DXMATRIX matView, matReflect;
    D3DXPLANE plane;
    D3DXPlaneFromPointNormal( &plane, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,1,0) );
    D3DXMatrixReflect( &matReflect, &plane );
    D3DXMatrixMultiply( &matView, &matReflect, &matViewSaved );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Reenable the framebuffer and zbuffer
    m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED |
                                                          D3DCOLORWRITEENABLE_GREEN |
                                                          D3DCOLORWRITEENABLE_BLUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

    // Setup render states to a blended render scene against mask in stencil
    // buffer. An important step here is to reverse the cull-order of the
    // polygons, since the view matrix is being reflected.
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,     D3DBLEND_DESTCOLOR );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,    D3DBLEND_ZERO );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILFUNC,  D3DCMP_EQUAL );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILPASS,  D3DSTENCILOP_KEEP );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,     D3DCULL_CW );

    // Clear the zbuffer (leave frame- and stencil-buffer intact)
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER, 0L, 1.0f, 0L );

    // Render the scene
    m_bRenderingReflectedScene = TRUE;
    RenderScene();

    // Restore render states
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,         D3DCULL_CCW );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matViewSaved );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the viewport, zbuffer, and stencil buffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         FOG_COLOR, 1.0f, 0L );

    // Set render state
    FLOAT fFogStart =  10.0f;
    FLOAT fFogEnd   = 100.0f;
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0xff555555 );
    m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,      TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,       FOG_COLOR );
    m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_LINEAR );
    m_pd3dDevice->SetRenderState( D3DRS_RANGEFOGENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGSTART,       FtoDW(fFogStart) );
    m_pd3dDevice->SetRenderState( D3DRS_FOGEND,         FtoDW(fFogEnd) );
    
    // Render the scene
    m_bRenderingReflectedScene = FALSE;
    RenderScene();

    // Render the reflection in the mirror
    RenderMirror();

    // Show frame rate
    m_Font.Begin();
    m_Font.DrawText(  64, 50, 0xffffffff, L"StencilMirror" );
    m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
    m_Font.End();

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}



