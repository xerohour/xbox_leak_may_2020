//-----------------------------------------------------------------------------
// File: WMVCutScene.cpp
//
// Desc: This sample demonstrates how to play back WMV cut scenes using the
//       Direct3D overlay surface
//
// Hist: 10.10.01 - New for Novermber XDK
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>
#include "wmvplayer.h"
#include <assert.h>
#include <d3d8perf.h>


//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_1, L"Play video" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_1, L"Stop playback" },
};

#define NUM_HELP_CALLOUTS ( sizeof( g_HelpCallouts ) / sizeof( g_HelpCallouts[0] ) )

#define WMV_FILE "d:\\media\\videos\\bike.wmv"

//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
public:
    CXBoxSample();

    virtual HRESULT Initialize();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
    
    HRESULT PlayVideo( CHAR* strFilename );

    CXBFont     m_Font;             // Font object
    CXBHelp     m_Help;             // Help object
    BOOL        m_bDrawHelp;        // TRUE to draw help screen

    LPDIRECTSOUND8      m_pDSound;      // DirectSound object
    CWMVPlayer          m_WMVPlayer;    // WMV Player object
    LPDIRECT3DTEXTURE8  m_pOverlay[2];  // Overlay textures
    LPDIRECT3DSURFACE8  m_pSurface[2];  // Overlay Surfaces
    DWORD               m_dwCurrent;    // Current surface
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
    m_bDrawHelp = FALSE;
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

    // Create DirectSound
    if( FAILED( DirectSoundCreate( NULL, &m_pDSound, NULL ) ) )
        return E_FAIL;

    // Initialize the player
    if( FAILED( m_WMVPlayer.Initialize( m_pd3dDevice, m_pDSound ) ) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: PlayVideo
// Desc: Plays the specified video file.  Pressing the B button will stop
//       playback
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::PlayVideo( CHAR* strFilename )
{
    // Open the video file to be played
    if( FAILED( m_WMVPlayer.OpenFile( strFilename ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Get information about the video format
    WMVVIDEOINFO videoInfo;
    m_WMVPlayer.GetVideoInfo( &videoInfo );

    // Create two textures based off the video format.  We need to
    // use two textures because the overlay surface reads directly
    // from the texture.  Updating a texture while it's being used
    // by the overlay will cause tearing
    for( int i = 0; i < 2; i++ )
    {
        m_pd3dDevice->CreateTexture( videoInfo.dwWidth,
                                     videoInfo.dwHeight,
                                     1,
                                     0,
                                     D3DFMT_YUY2,
                                     0,
                                     &m_pOverlay[i] );
        m_pOverlay[i]->GetSurfaceLevel( 0, &m_pSurface[i] );
    }
    m_dwCurrent = 0;

    // Set up source and destination rectangles
    // TODO: Adjust for different aspect ratios?
    RECT SrcRect;
    RECT DestRect;

    SrcRect.left    = 0;
    SrcRect.top     = 0;
    SrcRect.right   = videoInfo.dwWidth;
    SrcRect.bottom  = videoInfo.dwHeight;

    DestRect.left   = 0;
    DestRect.top    = 0;
    DestRect.right  = 640;
    DestRect.bottom = 480;

    // Timing statistics
    DWORD dwMaxCycles = 0;
    DWORD dwFrame = 0;
    DWORD dwMaxFrame = 0xFFFFFFFF;

    // Enable the Direct3D overlay surface
    m_pd3dDevice->EnableOverlay( TRUE );

    // Playback video.  This is done in its own loop to avoid wasting time
    // waiting for buffer swaps, etc, when we don't need to.
    for( ; ; )
    {
        // If we're ready for the next frame...
        if( m_WMVPlayer.IsReady() )
        {
            dwFrame++;

            // Get the next frame into our texture
            m_WMVPlayer.GetTexture( m_pOverlay[m_dwCurrent] );

            // Update the overlay surface to use this texture
            m_pd3dDevice->UpdateOverlay( m_pSurface[m_dwCurrent], &SrcRect, &DestRect, FALSE, 0 );

            // Decode the next frame
            if( S_FALSE == m_WMVPlayer.DecodeNext() )
            {
                break;
            }

            // We'll use the other texture next time
            m_dwCurrent ^= 1;
        }
        
        // Check for B button to stop playback here
        BOOL bInput = FALSE;
        XBInput_GetInput();
        for( int i = 0; i < 4; i++ )
        {
            if( g_Gamepads[i].hDevice &&
                g_Gamepads[i].bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] )
                bInput = TRUE;
        }

        if( bInput )
            break;
    }

    // We're done!
    OUTPUT_DEBUG_STRING( "Done!\n" );

    // When disabling the overlay, you should wait until just after a 
    // vertical blank to avoid any flickering
    m_pd3dDevice->BlockUntilVerticalBlank();
    m_pd3dDevice->EnableOverlay( FALSE );

    for( int i = 0; i < 2; i++ )
    {
        m_pSurface[i]->Release();
        m_pOverlay[i]->Release();
    }
    m_WMVPlayer.CloseFile();
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] )
    {
        PlayVideo( WMV_FILE );
    }

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    // Draw a gradient filled background
    RenderGradientBackground( 0xff404040, 0xff404080 );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"WMVCutScene" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        m_Font.DrawText( 200, 220, 0xffffffff, L"Press 'A' to play cut scene" );
        
        WCHAR str[100];
        swprintf( str, L"%S", WMV_FILE );
        m_Font.DrawText( 200, 250, 0xffffff00, str );

        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

