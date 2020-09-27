//-----------------------------------------------------------------------------
// File: StencilDepth.cpp
//
// Desc: Example code showing how to use stencil buffers to show the depth
//       complexity of a scene.
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




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Rotate\nhelicopter" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Display depth\ncomplexity" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display\nhelp" },
};

#define NUM_HELP_CALLOUTS 3




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont   m_Font;                 // Font
    CXBHelp   m_Help;                 // Help class
    BOOL      m_bDrawHelp;            // Whether to draw help

    BOOL      m_bShowDepthComplexity; // Whether to show the depth complexity
    CXBMesh   m_FileObject;           // A file based object

    // Internal functions
    VOID    SetStatesForRecordingDepthComplexity();
    VOID    ShowDepthComplexity();

public:
    HRESULT Initialize();
    HRESULT Render();
    HRESULT FrameMove();

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
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
            :CXBApplication()
{
    // Override base class member variables
    m_d3dpp.BackBufferFormat       = D3DFMT_X8R8G8B8;
    m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

    // Initialize member variables
    m_bDrawHelp            = FALSE;
    m_bShowDepthComplexity = FALSE;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load a .X file
    if( FAILED( m_FileObject.Create( m_pd3dDevice, "Models\\Heli.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Set the transform matrices
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 0.0f,-15.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f,  0.0f );
    D3DXMATRIX  matWorld, matView, matProj;

    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 100.0f );

    m_pd3dDevice->SetTransform( D3DTS_WORLD,      &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_VIEW,       &matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Setup a material
    D3DMATERIAL8 mtrl;
    XBUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    D3DLIGHT8 light;
    XBUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, 0.0f, -1.0f, 0.0f );
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
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        m_bDrawHelp = !m_bDrawHelp;

    // Toggle options with the gamepad
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
        m_bShowDepthComplexity = !m_bShowDepthComplexity;

    // Rotate the camera with the gamepad
    static FLOAT m_fXRotate = 0.0f;
    static FLOAT m_fYRotate = 0.0f;
    m_fXRotate += m_DefaultGamepad.fX1*m_fElapsedTime*D3DX_PI*0.5f;
    m_fYRotate += m_DefaultGamepad.fY1*m_fElapsedTime*D3DX_PI*0.5f;
    D3DXMATRIX matTrackBall, matTrans, matView;
    D3DXMatrixRotationYawPitchRoll( &matTrackBall, -m_fXRotate, -m_fYRotate, 0.0f );
    D3DXMatrixTranslation( &matTrans, 0.0f, 0.0f, 15.0f );
    D3DXMatrixMultiply( &matView, &matTrackBall, &matTrans );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetStatesForRecordingDepthComplexity()
// Desc: Turns on stencil and other states for recording the depth complexity
//       during the rendering of a scene.
//-----------------------------------------------------------------------------
VOID CXBoxSample::SetStatesForRecordingDepthComplexity()
{
    // Clear the stencil buffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_STENCIL, 0x0, 1.0f, 0L );

    // Turn on stenciling
    m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,   TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILFUNC,     D3DCMP_ALWAYS );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILREF,      0 );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILMASK,     0x00000000 );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILWRITEMASK,0xffffffff );

    // Increment the stencil buffer for each pixel drawn
    m_pd3dDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_INCRSAT );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILPASS,  D3DSTENCILOP_INCRSAT );

    // Since we are only writing into the stencil buffer, we can turn off the
    // frame buffer.
    m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );
}




//-----------------------------------------------------------------------------
// Name: ShowDepthComplexity()
// Desc: Draws the contents of the stencil buffer in false color. Use alpha
//       blending of one red, one green, and one blue rectangle to do false
//       coloring of bits 1, 2, and 4 in the stencil buffer.
//-----------------------------------------------------------------------------
VOID CXBoxSample::ShowDepthComplexity()
{
    // Re-enable framebuffer writes, turn off the buffer, and enable alpha
    // blending
    m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED |
                                                          D3DCOLORWRITEENABLE_GREEN |
                                                          D3DCOLORWRITEENABLE_BLUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );

    // Set up the stencil states
    m_pd3dDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILPASS,  D3DSTENCILOP_KEEP );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILFUNC,  D3DCMP_NOTEQUAL );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILREF,   0 );

    // Set the background to black
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0L );

    // Set render states for drawing a rectangle that covers the viewport.
    // The color of the rectangle will be passed in D3DRS_TEXTUREFACTOR
    D3DXVECTOR4 v[4];
    v[0] = D3DXVECTOR4(   0 - 0.5f, 480 - 0.5f, 0.0f, 1.0f );
    v[1] = D3DXVECTOR4(   0 - 0.5f,   0 - 0.5f, 0.0f, 1.0f );
    v[2] = D3DXVECTOR4( 640 - 0.5f, 480 - 0.5f, 0.0f, 1.0f );
    v[3] = D3DXVECTOR4( 640 - 0.5f,   0 - 0.5f, 0.0f, 1.0f );
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

    // Draw a red rectangle wherever the 1st stencil bit is set
    m_pd3dDevice->SetRenderState( D3DRS_STENCILMASK, 0x01 );
    m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xffff0000 );
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );

    // Draw a green rectangle wherever the 2nd stencil bit is set
    m_pd3dDevice->SetRenderState( D3DRS_STENCILMASK, 0x02 );
    m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xff00ff00 );
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );

    // Draw a blue rectangle wherever the 3rd stencil bit is set
    m_pd3dDevice->SetRenderState( D3DRS_STENCILMASK, 0x04 );
    m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xff0000ff );
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );

    // Restore states
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
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
    if( FALSE == m_bShowDepthComplexity )
        RenderGradientBackground( 0xff0000ff, 0xffffffff );

    // Set default states
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_COLORVERTEX,    TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0x00000000 );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,       TRUE );

    // Set states necessary to record the depth complexity of the scene
    if( m_bShowDepthComplexity )
        SetStatesForRecordingDepthComplexity();

    // Render the scene's opaque parts
    m_FileObject.Render( m_pd3dDevice, XBMESH_OPAQUEONLY );

    // Render the scene's alpha parts
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    m_FileObject.Render( m_pd3dDevice, XBMESH_ALPHAONLY );

    // Show the depth complexity of the scene
    if( m_bShowDepthComplexity )
        ShowDepthComplexity();

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"StencilDepth" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}



