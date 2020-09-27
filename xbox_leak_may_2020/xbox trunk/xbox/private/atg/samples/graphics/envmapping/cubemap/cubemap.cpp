//-----------------------------------------------------------------------------
// File: CubeMap.cpp
//
// Desc: Sample code showing how to do environment cube-mapping.
//
// Hist: 11.01.00 - New for November XDK release
//       12.15.00 - Changes for December XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBMesh.h>
#include <XBUtil.h>
#include <XBResource.h>
#include <xgraphics.h>

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
    { "LobbyXP.bmp",    resource_LobbyXP_OFFSET },
    { "LobbyXN.bmp",    resource_LobbyXN_OFFSET },
    { "LobbyYP.bmp",    resource_LobbyYP_OFFSET },
    { "LobbyYN.bmp",    resource_LobbyYN_OFFSET },
    { "LobbyZP.bmp",    resource_LobbyZP_OFFSET },
    { "LobbyZN.bmp",    resource_LobbyZN_OFFSET },
    { "BiHull.bmp",     resource_BiHull_OFFSET },
    { "Wings.bmp",      resource_Wings_OFFSET },
    { NULL, 0 },
};


   

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Rotate around\nteapot" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_1, L"Rotate scene" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display\nhelp" },
};

#define NUM_HELP_CALLOUTS 4




// CUBEMAP_RESOLUTION indicates how big to make the cubemap texture.
#define CUBEMAP_RESOLUTION 256




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBPackedResource m_xprResource;      // Packed resources for the app
    CXBFont           m_Font;             // Font class
    CXBHelp           m_Help;             // Help class
    BOOL              m_bDrawHelp;        // Whether to draw help

    CXBMesh           m_ShinyTeapot;      // A teapot (made shiny with the envmap)
    CXBMesh           m_SkyBox;           // A skybox of a lobby
    CXBMesh           m_Object;           // An object to fly around the scene
    D3DXMATRIX        m_matObject;        // Matrix to position object

    LPDIRECT3DCUBETEXTURE8 m_pCubeMap;    // The cubemap, and access to its surfaces
    LPDIRECT3DSURFACE8     m_pCubeMapFaces[6];

    D3DXMATRIX        m_matIdentity;      // Transform matrices
    D3DXMATRIX        m_matView;
    D3DXMATRIX        m_matProj;

protected:
    BOOL              m_bRenderTeapot;    // To toggle the teapot (so it doesn't render
                                          // its own reflection)
    HRESULT RenderSceneIntoCubeMap();     // Functions to render the scene
    HRESULT RenderScene();

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
    m_bDrawHelp         = FALSE;
    m_pCubeMap          = NULL;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    HRESULT hr;

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

    // Load the file objects
    if( FAILED( m_SkyBox.Create( m_pd3dDevice, "Models\\Lobby.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;
    if( FAILED( m_ShinyTeapot.Create( m_pd3dDevice, "Models\\Teapot.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;
    if( FAILED( m_Object.Create( m_pd3dDevice, "Models\\Airplane.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the cubemap, with a format that matches the backbuffer, since
    // we'll be rendering into it
    if( FAILED( hr = m_pd3dDevice->CreateCubeTexture( CUBEMAP_RESOLUTION, 1, 
                                         D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, 
                                         D3DPOOL_DEFAULT, &m_pCubeMap ) ) )
        return E_FAIL;

    // Get access the the cube map's surfaces (so we can render into them)
    for( DWORD i=0; i<6; i++ )
        m_pCubeMap->GetCubeMapSurface( (D3DCUBEMAP_FACES)i, 0, &m_pCubeMapFaces[i] );

    // Set default render states
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,  TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  0x00aaaaaa );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // Set the transform matrices
    D3DXMatrixIdentity( &m_matIdentity );
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, 4.0f/3.0f, 0.5f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,      &m_matIdentity );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

    // Setup a material
    D3DMATERIAL8 mtrl;
    XBUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // Set up a light
    D3DLIGHT8 light;
    XBUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, 0.0f, 0.0f, 1.0f );
    light.Ambient.r = 0.3f;
    light.Ambient.g = 0.3f;
    light.Ambient.b = 0.3f;
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    return S_OK;
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

    // Animate the object
    D3DXMATRIX  mat1;
    D3DXMatrixScaling( &m_matObject, 0.2f, 0.2f, 0.2f );
    D3DXMatrixTranslation( &mat1, 0.0f, 2.0f, 0.0f );
    D3DXMatrixMultiply( &m_matObject, &m_matObject, &mat1 );
    D3DXMatrixRotationX( &mat1, -2.9f*m_fAppTime );
    D3DXMatrixMultiply( &m_matObject, &m_matObject, &mat1 );
    D3DXMatrixRotationY( &mat1, 1.055f*m_fAppTime );
    D3DXMatrixMultiply( &m_matObject, &m_matObject, &mat1 );

    // Rotate the camera with the gamepad
    static D3DXMATRIX matTrackBall( 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 );
    D3DXMATRIX matRotate, matTrans;

    FLOAT fXRotate =  m_DefaultGamepad.fX1*m_fElapsedTime*D3DX_PI*0.5f;
    FLOAT fYRotate = -m_DefaultGamepad.fY1*m_fElapsedTime*D3DX_PI*0.5f;
    FLOAT fZRotate =  m_DefaultGamepad.fX2*m_fElapsedTime*D3DX_PI*0.5f;
    D3DXMatrixRotationYawPitchRoll( &matRotate, fXRotate, fYRotate, fZRotate );
    D3DXMatrixMultiply( &matTrackBall, &matTrackBall, &matRotate );

    D3DXMatrixTranslation( &matTrans, 0.0f, 0.0f, 5.0f );
    D3DXMatrixMultiply( &m_matView, &matTrackBall, &matTrans );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );

    // Render the scene into the surfaces of the cubemap. Note: don' render the
    // teapot, since it won't reflect into itself
    m_bRenderTeapot = FALSE;
    RenderSceneIntoCubeMap();
    m_bRenderTeapot = TRUE;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderSceneIntoCubeMap()
// Desc: Renders the scene to each of the 6 faces of the cube map
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RenderSceneIntoCubeMap()
{
    // Save the transforms that will be overwritten
    D3DXMATRIX matViewSave = m_matView;
    D3DXMATRIX matProjSave = m_matProj;

    // Get the current view matrix rotations, to concat it with the view matrix
    // for rendering into the cubemap's surfaces
    D3DXMATRIX matViewDir = m_matView;
    matViewDir._41 = 0.0f; matViewDir._42 = 0.0f; matViewDir._43 = 0.0f;

    // Set the projection matrix for a field of view of 90 degrees
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/2, 1.0f, 0.5f, 100.0f );

    // Get the current backbuffer and zbuffer
    LPDIRECT3DSURFACE8 pBackBuffer, pZBuffer;
    m_pd3dDevice->GetRenderTarget( &pBackBuffer );
    m_pd3dDevice->GetDepthStencilSurface( &pZBuffer );

    // Render to the six faces of the cube map
    for( DWORD i=0; i<6; i++ )
    {
        // Set the i'th cubemap surface as the rendertarget
        m_pd3dDevice->SetRenderTarget( m_pCubeMapFaces[i], pZBuffer );

        // Set the viewport to be the correct size
        D3DVIEWPORT8 vpCubeMap = { 0, 0, CUBEMAP_RESOLUTION, CUBEMAP_RESOLUTION, 0.0f, 1.0f };
        m_pd3dDevice->SetViewport( &vpCubeMap );

        // Set the view transform for this cubemap surface
        m_matView = XBUtil_GetCubeMapViewMatrix( (D3DCUBEMAP_FACES)i );
        D3DXMatrixMultiply( &m_matView, &matViewDir, &m_matView );

        // Render the scene
        RenderScene();
    }

    // Change the rendertarget back to the main backbuffer
    m_pd3dDevice->SetRenderTarget( pBackBuffer, pZBuffer );
    pBackBuffer->Release();
    pZBuffer->Release();

    // Restore the viewport to the backbuffer's dimensions
    D3DVIEWPORT8 vpBackBuffer = { 0, 0, 640, 480, 0.0f, 1.0f };
    m_pd3dDevice->SetViewport( &vpBackBuffer );

    // Restore the modified transforms
    m_matView = matViewSave;
    m_matProj = matProjSave;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderScene()
// Desc: Renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RenderScene()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
                         0x00000000, 1.0f, 0L );

    // Set default render states
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,  TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  0x00aaaaaa );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

    // Render the Skybox
    {
        // Disable zbuffer
        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

        // Center view matrix, and set FOV to 90 degrees
        D3DXMATRIX matView, matProj;
        matView = m_matView; matView._41 = matView._42 = matView._43 = 0.0f;
        D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/2, 1.0f, 0.5f, 10000.0f );
        m_pd3dDevice->SetTransform( D3DTS_WORLD,      &m_matIdentity );
        m_pd3dDevice->SetTransform( D3DTS_VIEW,       &matView );
        m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

        // Render the skybox
        m_SkyBox.Render( m_pd3dDevice );

        // Restore the render states
        m_pd3dDevice->SetTransform( D3DTS_VIEW,       &m_matView );
        m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );
        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,  TRUE );
    }

    // Render the Object
    {
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matObject );
        m_Object.Render( m_pd3dDevice );
    }

    // Render the object with the environment-mapped body
    if( m_bRenderTeapot )
    {
        // Turn on texture-coord generation for cubemapping
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 );
        m_pd3dDevice->SetTexture( 0, m_pCubeMap );

        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

        // Render the object with the environment-mapped body
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matIdentity );
        m_ShinyTeapot.Render( m_pd3dDevice, XBMESH_NOMATERIALS|XBMESH_NOTEXTURES );

        // Restore the render states
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, this call renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Render the scene
    RenderScene();

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"CubeMap" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}



