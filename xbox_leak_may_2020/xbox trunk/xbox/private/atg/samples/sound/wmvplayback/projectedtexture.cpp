//-----------------------------------------------------------------------------
// File: ProjectedTexture.cpp
//
// Desc: Sample to show off projected textures
//
// Hist: 11.01.00 - New for November XDK release
//       12.15.00 - Changes for December XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBUtil.h>
#include <xgraphics.h>
#include "wmvplay.h"


//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, _T("Rotate\ncube") },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, _T("Move projection\npoint") },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, _T("Show\nfrustrum") },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, _T("Display help") },
    { XBHELP_MISC_CALLOUT, XBHELP_PLACEMENT_2, _T("Also: Left trigger w/right stick\nmoves the projection focus") },
};


//-----------------------------------------------------------------------------
// Name: CXBoxSample()
// Desc: Constructor
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::InitGraphics()
{
    m_bDrawHelp         = FALSE;

    m_pSpotLightTexture = NULL;

    m_vTexEyePt    = D3DXVECTOR3(-2.0f, -2.0f, -7.0f );
    m_vTexLookatPt = D3DXVECTOR3( 0.0f,  0.0f,  0.0f );

    // Create the help
    if( FAILED( m_Help.Create( m_pd3dDevice ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Set the view matrix
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 0.0f,-15.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
    D3DXMATRIX  matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Set the projection matrix
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 200.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set up a point light
    D3DLIGHT8 light;
    XBUtil_InitLight( light, D3DLIGHT_POINT, 0.0f, 0.0f, -10.0f );
    light.Attenuation0 = 0.0f;
    light.Attenuation1 = 0.0f;
    light.Attenuation2 = 1.0f;
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00333333 );

    // Set a default white material
    D3DMATERIAL8 mtrl;
    XBUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // Create the texture for our procedural spotlight texmap
    CreateSpotLightTexture();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateSpotLightTexture()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::CreateSpotLightTexture( )
{
    // Release the old texture
    SAFE_RELEASE( m_pSpotLightTexture );

    // Create a new texture
    m_pd3dDevice->CreateTexture( m_VidStrm.u32Width, m_VidStrm.u32Height, 1, 0, D3DFMT_LIN_X8R8G8B8, 
                                 D3DPOOL_DEFAULT, &m_pSpotLightTexture );

    D3DLOCKED_RECT lock;
    m_pSpotLightTexture->LockRect( 0, &lock, NULL, 0L );

    D3DSURFACE_DESC desc;
    m_pSpotLightTexture->GetLevelDesc( 0, &desc );
    DWORD* pBits    = (DWORD*)lock.pBits;

    //
    // the WMV video decoder will ask for a buffer to dump data on.
    // pass it this buffer..
    //

    m_pVidBuf = (PUCHAR)pBits;

    m_pSpotLightTexture->UnlockRect(0);

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ShowTexture()
// Desc: 
//-----------------------------------------------------------------------------
VOID CFilterGraph::ShowTexture( LPDIRECT3DTEXTURE8 pTexture )
{
    D3DSURFACE_DESC d3dsd;
    pTexture->GetLevelDesc( 0, &d3dsd );
    FLOAT x1 = 0.0f, x2 = x1 + (FLOAT)d3dsd.Width;
    FLOAT y1 = 0.0f, y2 = y1 + (FLOAT)d3dsd.Height;

    struct SPRITEVERTEX
    {
        FLOAT sx, sy, sz, rhw;
        FLOAT tu, tv;
    };
    
    SPRITEVERTEX vSprite[4] =
    {
        { x1-0.5f, y1-0.5f, 0.99f, 1.0f, 0.0f,               (FLOAT)d3dsd.Height },
        { x2-0.5f, y1-0.5f, 0.99f, 1.0f, (FLOAT)d3dsd.Width, (FLOAT)d3dsd.Height },
        { x2-0.5f, y2-0.5f, 0.99f, 1.0f, (FLOAT)d3dsd.Width, 0.0f                },
        { x1-0.5f, y2-0.5f, 0.99f, 1.0f, 0.0f,               0.0f                },
    };

    // Set state
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,  D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,    D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,    D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    m_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

    // Display the sprite
    m_pd3dDevice->SetTexture( 0, pTexture );
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADLIST, 4, vSprite, sizeof(vSprite[0]) );
}


//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::GraphicsFrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        m_bDrawHelp = !m_bDrawHelp;

    // Toggle drawing of the projection frustum
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
    {
        CreateSpotLightTexture();
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::Render()
{
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L );

    // Draw a gradient filled background
    RenderGradientBackground( 0xff0000ff, 0xff000000 );

    // Set default render states
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,  TRUE );

    //
    // show wmv generated image
    //

    ShowTexture( m_pSpotLightTexture );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
		m_Font.Begin();
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
		m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




