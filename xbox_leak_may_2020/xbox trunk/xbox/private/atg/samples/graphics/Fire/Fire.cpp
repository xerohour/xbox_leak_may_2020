//-----------------------------------------------------------------------------
// File: Fire.cpp
//
// Desc: Demonstrates a cool fire effect, using a scrolling noise texture. See
//       the inline comments for how this is done. In a nutshell, billboarded
//       geometry is lit with a static fire texture that is modulated with a
//       scrolling noise texture.
//
// Hist: 03.07.01 - New for April XDK
//       04.05.01 - Converted to bundled resources
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBMesh.h>
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
    { "SkyBoxXP.bmp",   resource_SkyBoxXP_OFFSET },
    { "SkyBoxXN.bmp",   resource_SkyBoxXN_OFFSET },
    { "SkyBoxYP.bmp",   resource_SkyBoxYP_OFFSET },
    { "SkyBoxYN.bmp",   resource_SkyBoxYN_OFFSET },
    { "SkyBoxZP.bmp",   resource_SkyBoxZP_OFFSET },
    { "SkyBoxZN.bmp",   resource_SkyBoxZN_OFFSET },
    { "Fireground.bmp", resource_Fireground_OFFSET },
    { "FlameColor.bmp", resource_FlameColor_OFFSET },
    { "FireNoise.bmp",  resource_FireNoise_OFFSET },
    { "NoiseAlpha.bmp", resource_NoiseAlpha_OFFSET },
    { NULL, 0 },
};




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Move camera" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_1, L"Rotate camera" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
};

#define NUM_HELP_CALLOUTS 4




struct PLANEVERTEX
{
    D3DXVECTOR3 p; // World position
    D3DCOLOR    c; // Color
    FLOAT       tu, tv;
};

#define D3DFVF_PLANEVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBPackedResource  m_xprResource;    // Packed resources for the app
    CXBFont            m_Font;           // Font class
    CXBHelp            m_Help;           // Help class
    BOOL               m_bDrawHelp;      // Whether to draw help

    D3DXVECTOR3 m_vViewAngle;            // View angle of camera
    D3DXMATRIX  m_matWorld;              // World matrix
    D3DXMATRIX  m_matView;               // View matrix
    D3DXMATRIX  m_matProj;               // Projection matrix

    CXBMesh     m_SkyBoxObject;          // Skybox
    D3DXMATRIX  m_matSkyboxView;         // View matrix for rendering skybox

    LPDIRECT3DVERTEXBUFFER8 m_pPlaneVB;  // Geometry for ground plane
    LPDIRECT3DTEXTURE8 m_pGroundTexture; // Texture for the ground

    CXBMesh     m_FlameObject;           // Geometry for flame
    DWORD       m_dwFireVertexShader;    // Vertex shader for the fire effect
    DWORD       m_dwFirePixelShader;     // Handle for pixel shader 1
    LPDIRECT3DTEXTURE8 m_pFireBaseTexture;  // Textures for the fire effect
    LPDIRECT3DTEXTURE8 m_pFireNoiseTexture;
    LPDIRECT3DTEXTURE8 m_pFireNoiseMaskTexture;

public:
    HRESULT Initialize();       // Initialize the sample
    HRESULT Render();           // Render the scene
    HRESULT FrameMove();        // Perform per-frame updates

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
// Name: CXBoxSample (constructor)
// Desc: Constructor for CXBoxSample class
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample() 
            :CXBApplication()
{
    m_bDrawHelp  = FALSE;
    m_vViewAngle = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
}




//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load our geometry
    if( FAILED( m_SkyBoxObject.Create( m_pd3dDevice, "Models\\SkyBox.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    if( FAILED( m_FlameObject.Create( m_pd3dDevice, "Models\\Flame.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create a quad for our intersecting plane
    m_pd3dDevice->CreateVertexBuffer( 4*sizeof(PLANEVERTEX), D3DUSAGE_WRITEONLY, 
                                      0, D3DPOOL_MANAGED, &m_pPlaneVB );
    {
        PLANEVERTEX* v;
        m_pPlaneVB->Lock( 0, 0, (BYTE **)&v, 0 );
        v[0].p = D3DXVECTOR3( -30.0f, 0.0f, -30.0f );  v[0].c = 0xffffffff; v[0].tu = 0.000f; v[0].tv = 0.000f;
        v[1].p = D3DXVECTOR3( -30.0f, 0.0f,  30.0f );  v[1].c = 0xffffffff; v[1].tu = 0.000f; v[1].tv = 1.062f;
        v[2].p = D3DXVECTOR3(  30.0f, 0.0f, -30.0f );  v[2].c = 0xffffffff; v[2].tu = 1.066f; v[2].tv = 0.000f;
        v[3].p = D3DXVECTOR3(  30.0f, 0.0f,  30.0f );  v[3].c = 0xffffffff; v[3].tu = 1.066f; v[3].tv = 1.062f;
        m_pPlaneVB->Unlock();
    }

    // Create a texture to render into
    m_pGroundTexture        = m_xprResource.GetTexture( "FireGround.bmp" );
    m_pFireBaseTexture      = m_xprResource.GetTexture( "FlameColor.bmp" );
    m_pFireNoiseTexture     = m_xprResource.GetTexture( "FireNoise.bmp" );
    m_pFireNoiseMaskTexture = m_xprResource.GetTexture( "NoiseAlpha.bmp" );

    // Set up transforms
    D3DXMatrixIdentity( &m_matWorld );
    D3DXMatrixIdentity( &m_matView );
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 1000.0f );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,      &m_matWorld );
    m_pd3dDevice->SetTransform( D3DTS_VIEW,       &m_matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

    // Create vertex shader for the fire
    DWORD dwFireVertexDecl[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // v0 = Position
        D3DVSD_REG( 1, D3DVSDT_FLOAT3 ), // v1 = Normal
        D3DVSD_REG( 6, D3DVSDT_FLOAT2 ), // v6 = Tex coords
        D3DVSD_END()
    };

    if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\Fire.xvu",
                                           dwFireVertexDecl, &m_dwFireVertexShader ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    
    if( FAILED( XBUtil_CreatePixelShader( m_pd3dDevice, "Shaders\\Fire.xpu",
                                          &m_dwFirePixelShader ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    return S_OK;
}


D3DXVECTOR3 vCameraPos( 0.0f, 2.0f, -20.0f );


//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
        m_bDrawHelp = !m_bDrawHelp;

    // Move the camera
    m_vViewAngle.y += m_DefaultGamepad.fX2*1.0f*m_fElapsedTime;
    if( m_vViewAngle.y > D3DX_PI*2 )
        m_vViewAngle.y -= D3DX_PI*2;
    if( m_vViewAngle.y < 0.0f )
        m_vViewAngle.y += D3DX_PI*2;

    m_vViewAngle.x += m_DefaultGamepad.fY2*1.0f*m_fElapsedTime;
    if( m_vViewAngle.x > D3DX_PI/8 )
        m_vViewAngle.x = D3DX_PI/8;
    if( m_vViewAngle.x < 0.0f )
        m_vViewAngle.x = 0.0f;

    // Create a rotation matrix for our view angle
    D3DXMATRIX  matRotate, matRotateX, matRotateY;
    D3DXMatrixRotationY( &matRotateX, m_vViewAngle.x );
    D3DXMatrixRotationY( &matRotateY, m_vViewAngle.y );
    D3DXMatrixRotationYawPitchRoll( &matRotate, m_vViewAngle.y, m_vViewAngle.x, m_vViewAngle.z );

    // Move the camera
    D3DXVECTOR3 vForward, vRight;
    D3DXVec3TransformCoord( &vForward, &D3DXVECTOR3(0,0,1), &matRotateY );
    D3DXVec3TransformCoord( &vRight,   &D3DXVECTOR3(1,0,0), &matRotateY );
    vCameraPos += vRight * m_DefaultGamepad.fX1*10.0f*m_fElapsedTime;
    vCameraPos += vForward * m_DefaultGamepad.fY1*10.0f*m_fElapsedTime;

    // Use the camera position and lookat direction to setup the view matrix
    D3DXVECTOR3 vLookat;
    D3DXVec3TransformCoord( &vLookat,  &D3DXVECTOR3(0,0,1), &matRotate );
    D3DXMatrixLookAtLH( &m_matView, &vCameraPos, 
                                    &(vCameraPos + vLookat),
                                    &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );

    // Remove the translation to get the skybox view matrix
    m_matSkyboxView = m_matView;
    m_matSkyboxView._41 = 0.0f; m_matSkyboxView._42 = 0.0f; m_matSkyboxView._43 = 0.0f;

    // Compute the billboard matrix (constrained form of the view matrix)
    D3DXMATRIX matBillboard;
    D3DXMatrixLookAtLH( &matBillboard, &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ),
                                       &D3DXVECTOR3( -vCameraPos.x, 0.0f, -vCameraPos.z ),
                                       &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
    D3DXMatrixInverse( &matBillboard, NULL, &matBillboard );

    // Pass constants to the vertex shader.
    // c0 and c1 are used to scroll the noise texture's tex coords
    D3DXVECTOR4 c0( 1.00f, 0.35f, 0.0f, 1.0f );
    D3DXVECTOR4 c1( 0.0f, 0.30f*m_fAppTime, 0.0f, 0.0f );
    m_pd3dDevice->SetVertexShaderConstant(  0, &c0,     1 );
    m_pd3dDevice->SetVertexShaderConstant(  1, &c1,     1 );

    // Pass the transform set to the vertex shader
    D3DXMATRIX matWVP;
    D3DXMatrixMultiply( &matWVP, &matBillboard, &m_matView );
    D3DXMatrixMultiply( &matWVP, &matWVP, &m_matProj );
    D3DXMatrixTranspose( &matWVP,&matWVP );
    m_pd3dDevice->SetVertexShaderConstant(  4, &matWVP, 4 );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L );

    // Set state for rendering the skybox
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matSkyboxView );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    // Render the skybox
    m_SkyBoxObject.Render( m_pd3dDevice );

    // Set state for rendering the ground plane
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );
    m_pd3dDevice->SetTexture( 0, m_pGroundTexture );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

    // Render the ground plane
    m_pd3dDevice->SetVertexShader( D3DFVF_PLANEVERTEX );
    m_pd3dDevice->SetStreamSource( 0, m_pPlaneVB, sizeof(PLANEVERTEX) );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

    // Set state for rendering the fire    
    m_pd3dDevice->SetTexture( 0, m_pFireBaseTexture );
    m_pd3dDevice->SetTexture( 1, m_pFireNoiseTexture );
    m_pd3dDevice->SetTexture( 2, m_pFireNoiseMaskTexture );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    // Render the fire.
    //
    // The vertex shader transforms the vertices (nothing fancy), and sets up
    // the texture coordinates for three texture stages. Stage 0 is the base
    // fire texture and is stationary. Stage 1 is a long, skinny, noise texture
    // with scrolling texture coordinates. Stage 2 is a stationary texture used
    // to mask the noise texture.
    //
    // The pixel shader brings everything together. The rgb data from the base
    // texture is used, while the alpha channel is a combination of the three
    // textures, most notably the noise texture, which gives the effect of
    // rolling, animated flames.
    //
    // Note: also crucial to effect is the geometry itself. The folds and
    // ripples in the geometry make the scrolling of the noise texture look
    // more chaotic. (Using a flat plane for the fire effect, would look okay,
    // except that the flames would go 100% in the up direction, with no
    // deviation. The end result would be not as cool for that case.)
    m_pd3dDevice->SetVertexShader( m_dwFireVertexShader );
    m_pd3dDevice->SetPixelShader( m_dwFirePixelShader );
    m_FlameObject.Render( m_pd3dDevice, XBMESH_NOFVF|XBMESH_NOMATERIALS|XBMESH_NOTEXTURES );

    m_pd3dDevice->SetPixelShader( NULL );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"Fire" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




