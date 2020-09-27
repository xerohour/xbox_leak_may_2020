//-----------------------------------------------------------------------------
// File: SphereMap.cpp
//
// Desc: Example code showing how to use sphere-mapping in D3D, using generated 
//       texture coordinates.
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
    { "Scene.bmp",      resource_Scene_OFFSET },
    { "SphMap1.bmp",    resource_SphMap1_OFFSET },
    { "SphMap2.bmp",    resource_SphMap2_OFFSET },
    { NULL, 0 },
};


   

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Rotate\nteapot" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Change\ntransparency" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Change\neffect" },
    { XBHELP_Y_BUTTON,     XBHELP_PLACEMENT_2, L"Change\ntexture" },
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

    CXBMesh*           m_pTeapot;
    LPDIRECT3DTEXTURE8 m_pSphereMapTexture;
    BOOL               m_bGlassEffect;

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
    m_pTeapot            = new CXBMesh();
    m_pSphereMapTexture  = NULL;
    m_bGlassEffect       = FALSE;
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

    // Load the spheremap texture
    m_pSphereMapTexture = m_xprResource.GetTexture( "SphMap1.bmp" );

    // Load the teapot object
    if( FAILED( m_pTeapot->Create( m_pd3dDevice, "Models\\Teapot.xbg", &m_xprResource ) ) )
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
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 0.0f,-5.0f );
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
        m_bGlassEffect = !m_bGlassEffect;

    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y] )
    {
        static DWORD dwTexture = 0;
        dwTexture = (dwTexture+1)%3;

        CHAR* strTextures[] = { "SphMap1.bmp", "SphMap2.bmp", "Scene.bmp" };
        m_pSphereMapTexture = m_xprResource.GetTexture( strTextures[dwTexture] );
    }

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
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Draw a gradient filled background
    RenderGradientBackground( 0xff000000, 0xff0000ff );

    // Set state
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,          0x00ffffff );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,         TRUE );

    // Adds in the spheremap texture
    m_pd3dDevice->SetTexture( 0, m_pSphereMapTexture );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

    // Generate spheremap texture coords from the position
    D3DXMATRIX mat;
    mat._11 = 0.5f; mat._12 = 0.0f;
    mat._21 = 0.0f; mat._22 =-0.5f;
    mat._31 = 0.0f; mat._32 = 0.0f;
    mat._41 = 0.5f; mat._42 = 0.5f;
    m_pd3dDevice->SetTransform( D3DTS_TEXTURE0, &mat );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );

    // A glass effect can be achieved by turning on alpha-blending, and
    // rendering both sides of the object
    if( m_bGlassEffect )
    {
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,     D3DBLEND_SRCCOLOR );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,    D3DBLEND_ONE );
        m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,     D3DCULL_NONE );
        m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,     TRUE );

        // Set the transparency
        DWORD a = (DWORD)((m_DefaultGamepad.fY2+1)*127.9f);
        m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0x01010101 * a );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    }

    // Finally, draw the teapot
    m_pTeapot->Render( m_pd3dDevice, XBMESH_NOTEXTURES );

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
        m_Font.DrawText(  64, 50, 0xffffffff, L"SphereMap" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}



