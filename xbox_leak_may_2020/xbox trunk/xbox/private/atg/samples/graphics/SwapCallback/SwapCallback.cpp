//-----------------------------------------------------------------------------
// File: SwapCallback.cpp
//
// Desc: Example code showing how to use Swap and VBlank callbacks to determine
//       when the app is close to dropping frames. An app can use this
//       information to scale back rendering quality instead of dropping frames.
//
//       The app is made to be fill bound by drawing a bunch of big,
//       anti-aliased quads on the screen. The user can control the number of
//       quads (and thereby control the overall frame time) and observe how
//       close the app is to dropping frames.
//
// Hist: 12.01.01 - New for December XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBUtil.h>
#include <XBHelp.h>
#include <XBMesh.h>


//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,   XBHELP_PLACEMENT_2, L"Vary amount of\nquads drawn." },
    { XBHELP_BACK_BUTTON, XBHELP_PLACEMENT_2, L"Display\nhelp" },
};

#define NUM_HELP_CALLOUTS 2




//-----------------------------------------------------------------------------
// Vertices for drawing a bunch of pixel-fill-eating quads
//-----------------------------------------------------------------------------
#define MAX_QUADS 1000

struct SCREENVERTEX
{
    D3DXVECTOR4 pos;
    D3DCOLOR    color;
};

SCREENVERTEX g_vQuads[4*MAX_QUADS];


// Helper macro
#define rnd()  (((FLOAT)rand() ) / RAND_MAX) 




//-----------------------------------------------------------------------------
// Swap callback to determine whether we are close to dropping frames
//-----------------------------------------------------------------------------
DWORD g_dwCyclesUntilVBlank = 0;
DWORD g_dwNumMissedVBlanks  = 0;

VOID _cdecl SwapCallback( D3DSWAPDATA* pSwapData )
{
    // NOTE: This callback is called as a DPC (a "Deferred Procedure Call"
    // which is a high-priority kernel callback queued by an Interrupt Service
    // Routines). As such, do your business and get out. Do not make any XTL
    // calls from here, and do not use any floating point.

    // Store time until the next VBlank
    g_dwCyclesUntilVBlank = pSwapData->TimeUntilSwapVBlank;
    g_dwNumMissedVBlanks  = pSwapData->MissedVBlanks;
}




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
public:
    CXBFont            m_Font;             // Font class
    CXBHelp            m_Help;             // Help class
    BOOL               m_bDrawHelp;        // Whether to draw help

    DWORD              m_dwNumQuads;       // # of quads to render

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
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    m_d3dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR;

    // Don't draw help initially
    m_bDrawHelp = FALSE;

    // Seed the random number generator
    srand((DWORD)m_fTime);

    // Randomly generate data for onscreen, pixel-fill-eating quads
    for( WORD i=0; i<MAX_QUADS; i++)
    {
        FLOAT x   = (FLOAT)(int)(320+200*(rnd()-rnd())) - 0.5f;
        FLOAT y   = (FLOAT)(int)(280+150*(rnd()-rnd())) - 0.5f;
        FLOAT w_2 = (FLOAT)(int)(160*rnd()/2);
        FLOAT h_2 = (FLOAT)(int)(100*rnd()/2);

        g_vQuads[4*i+0].pos = D3DXVECTOR4( x - w_2, y - h_2, 0, 0 );
        g_vQuads[4*i+1].pos = D3DXVECTOR4( x + w_2, y - h_2, 0, 0 );
        g_vQuads[4*i+2].pos = D3DXVECTOR4( x + w_2, y + h_2, 0, 0 );
        g_vQuads[4*i+3].pos = D3DXVECTOR4( x - w_2, y + h_2, 0, 0 );

        DWORD r = (int)(255*rnd());
        DWORD g = (int)(255*rnd());
        DWORD b = (int)(255*rnd());
        DWORD a = 0x0e;

        g_vQuads[4*i+0].color = (a<<24)|(r<<16)|(g<<8)|(b<<0);
        g_vQuads[4*i+1].color = (a<<24)|(r<<16)|(g<<8)|(b<<0);
        g_vQuads[4*i+2].color = (a<<24)|(r<<16)|(g<<8)|(b<<0);
        g_vQuads[4*i+3].color = (a<<24)|(r<<16)|(g<<8)|(b<<0);
    }

    m_dwNumQuads = 100;
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

    // Set the transform matrices
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, 0.75f, 4.0f/3.0f, 1.0f, 1000.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set a swap callback to regulate framerate
    m_pd3dDevice->SetSwapCallback( SwapCallback );

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

    // Let the user control the number of quads drawn
    static FLOAT fNumQuads = 500;
    fNumQuads += m_DefaultGamepad.fY1*m_fElapsedTime*500;
    fNumQuads = max( 1.0f, min( MAX_QUADS, fNumQuads ) );

    m_dwNumQuads = (DWORD)fNumQuads;


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
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
                         0x00000000, 1.0f, 0L );

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

    // Turn on alphablending, z-buffering, and multisample to eat fill
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_ONE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_MULTISAMPLEMODE,  D3DMULTISAMPLEMODE_2X );

    // Set the vertex shader
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );

    // Draw a bunch of quads to make the app fillbound
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADLIST, m_dwNumQuads, g_vQuads, sizeof(SCREENVERTEX) );

    // Restore state
    m_pd3dDevice->SetRenderState( D3DRS_MULTISAMPLEMODE, D3DMULTISAMPLEMODE_1X );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.DrawText(  64, 50, 0xffffff00, L"SwapCallback" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        WCHAR str[80];
        m_Font.DrawText(  64, 80, 0xffffff00, L"Time until VBlank:" );
    
        if( g_dwNumMissedVBlanks > 0 )
        {
            // Display the time by which we exceeded the VBlank
            swprintf( str, L"%.3f ms", (1000.0f/60.0f) - g_dwCyclesUntilVBlank/733000.0f );
            m_Font.DrawText( 240,  80, 0xffff0000, str );
            m_Font.DrawText(  64, 105, 0xffff0000, L"Missed VBlank" );
        }
        else
        {
            // Display the time we have left before the VBlank
            swprintf( str, L"%.3f ms", g_dwCyclesUntilVBlank/733000.0f );
            m_Font.DrawText( 240, 80, 0xffffffff, str );
        }
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}



