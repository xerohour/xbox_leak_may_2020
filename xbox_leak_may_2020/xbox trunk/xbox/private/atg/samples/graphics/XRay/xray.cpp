//-----------------------------------------------------------------------------
// File: XRay.cpp
//
// Desc: Example code showing how to an xray effect. This is basically a
//       spheremapping technique (see the SphereMap sample if you are new to
//       that technique) with alpha-blending and a carefully chosen spheremap.
//
//       In a real xray, more dense and/or thicker mass shows up more white, 
//       as less xrays are able to penetrate the mass and expose the film's
//       negative. The technique used by this sample is hardly "correct", but
//       it looks cool none-the-less. 
//
// Hist: 01.15.01 - New for March XDK release
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
    { "Crystal.bmp",  resource_Crystal_OFFSET },
    { "XRay1.bmp",    resource_XRay1_OFFSET },
    { "XRay2.bmp",    resource_XRay2_OFFSET },
    { NULL, 0 },
};


   

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Rotate\nobject" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Change\ntransparency" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Change\ntexture" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle\nalpha" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Toggle help" },
};

#define NUM_HELP_CALLOUTS 5




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBPackedResource  m_xprResource;      // Packed resources for the app
    CXBFont            m_Font;             // Font class
    CXBHelp            m_Help;             // Help class
    BOOL               m_bDrawHelp;        // Whether to draw help

    // Geometry and textures
    CXBMesh*           m_pObject;
    LPDIRECT3DTEXTURE8 m_pSphereMapTexture;

    // Options
    DWORD              m_dwTextureSelection;
    BOOL               m_bAlphaEnabled;

protected:
    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();

public:
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
    m_bDrawHelp          = FALSE;

    m_pObject            = new CXBMesh();
    m_pSphereMapTexture  = NULL;
    
    m_dwTextureSelection = 0L;
    m_bAlphaEnabled      = TRUE;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: This creates all device-dependent display objects.
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

    // Load the spheremap texture
    m_pSphereMapTexture = m_xprResource.GetTexture( "XRay1.bmp" );

    // Load the object
    if( FAILED( m_pObject->Create( m_pd3dDevice, "Models\\Robot.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Misc render states
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0xffffffff );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,       D3DCULL_NONE );

    // Set up world matrix
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Set up view matrix
    D3DXMATRIX  matView;
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 0.0f,-8.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Set up proj matrix
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 200.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Setup a base material
    D3DMATERIAL8 mtrl;
    XBUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

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

    // Select options
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
    {
        CHAR* strTextures[] = { "XRay1.bmp", "XRay2.bmp", "Crystal.bmp" };

        m_dwTextureSelection = (m_dwTextureSelection+1)%3;
        m_pSphereMapTexture = m_xprResource.GetTexture( strTextures[m_dwTextureSelection] );
    }

    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
        m_bAlphaEnabled = !m_bAlphaEnabled;

    // Perform object rotation
    static D3DXMATRIX matWorld( 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 );
    D3DXMATRIX matRotate;
    FLOAT fXRotate = m_DefaultGamepad.fX1*m_fElapsedTime*D3DX_PI*0.5f;
    FLOAT fYRotate = m_DefaultGamepad.fY1*m_fElapsedTime*D3DX_PI*0.5f;
    D3DXMatrixRotationYawPitchRoll( &matRotate, -fXRotate, -fYRotate, 0.0f );
    D3DXMatrixMultiply( &matWorld, &matWorld, &matRotate );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Sets up render states, clears the viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Draw a gradient filled background
    RenderGradientBackground( 0xff000000, 0xff0000ff );

    // Set some default state
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,          0x00ffffff );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,         TRUE );

    // Since xrays go through objects, it only looks convincing if we turn on
    // alpha-blending and render both sides of the object 
    if( m_bAlphaEnabled )
    {
        // Lighting is not needed, since we don't use the diffuse color
        m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,     FALSE );

        // Setup to render both sides of the object
        m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,     D3DCULL_NONE );

        // Turn on alpha blending
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,     D3DBLEND_SRCCOLOR );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,    D3DBLEND_ONE );

        // Set the transparency
        DWORD a = (DWORD)((m_DefaultGamepad.fY2+1)*127.9f);
        m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0x01010101 * a );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
    }

    // Setup the spheremap texture. Via the spheremapping texcoord generation
    // below, normals from the object are used to look up colors in this
    // spheremap. Choice of this spheremap, therefore, has the biggest effect
    // on the resulting look.
    m_pd3dDevice->SetTexture( 0, m_pSphereMapTexture );

    // Generate spheremap texture coords from the camera space normal. This has
    // two steps. First, tell D3D to use the vertex normal (in camera space) as
    // texture coordinates. Then, we setup a texture matrix to transform these
    // texcoords from (-1,+1) view space to (0,1) texture space. This way,
    // the normal can be used to look up a texel in the spheremap.
    D3DXMATRIX mat;
    mat._11 = 0.5f; mat._12 = 0.0f;
    mat._21 = 0.0f; mat._22 =-0.5f;
    mat._31 = 0.0f; mat._32 = 0.0f;
    mat._41 = 0.5f; mat._42 = 0.5f;
    m_pd3dDevice->SetTransform( D3DTS_TEXTURE0, &mat );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );

    // Finally, draw the object
    m_pObject->Render( m_pd3dDevice, XBMESH_NOTEXTURES|XBMESH_NOMATERIALS );

    // Restore render states
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,         D3DCULL_CCW );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,     TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"XRay" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        switch( m_dwTextureSelection )
        {
            case 0: m_Font.DrawText( 64, 75, 0xff00ffff, L"Using texture XRay1.bmp" ); break;
            case 1: m_Font.DrawText( 64, 75, 0xff00ffff, L"Using texture XRay2.bmp" ); break;
            case 2: m_Font.DrawText( 64, 75, 0xff00ffff, L"Using texture Crytstal.bmp" ); break;
        }

        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}



