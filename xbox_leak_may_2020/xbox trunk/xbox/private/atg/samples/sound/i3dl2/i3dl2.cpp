//-----------------------------------------------------------------------------
// File: I3DL2.cpp
//
// Desc: This sample demonstrates how to use the 3D capabilities of 
//       DirectSound, moving a sound source and listener in 3D.
//
// Hist: 12.15.00 - New for December XDK release
//       03.09.01 - Rewritten for April XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBSound.h>
#include <XBHelp.h>
#include "dsound.h"

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle sound" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_2, L"Change sound" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle source/\nlistener" },
    { XBHELP_BLACK_BUTTON, XBHELP_PLACEMENT_2, L"Increase volume" },
    { XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_2, L"Decrease volume" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Move object in Y" },
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Move object in X/Z" },
};

#define NUM_HELP_CALLOUTS 8



//-----------------------------------------------------------------------------
// Globals variables and definitions
//-----------------------------------------------------------------------------

struct D3DVERTEX
{
    D3DXVECTOR3 p;           // position
    D3DCOLOR    c;           // color
};
#define D3DFVF_D3DVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)


// Constants to define our world space
#define XMIN -10
#define XMAX 10
#define ZMIN -10
#define ZMAX 10
#define YMIN 0
#define YMAX 5

// Constants for colors
#define SOURCE_COLOR 0xFFFF0000
#define LISTENER_COLOR 0xFF0000FF

// Constants for scaling input
#define MOTION_SCALE 10.0f
#define VOLUME_SCALE  5.0f

// List of wav files to cycle through
char * g_aszFileNames[] = 
{
    "D:\\Media\\Sounds\\Heli.wav",
    "D:\\Media\\Sounds\\Dolphin1.wav",
    "D:\\Media\\Sounds\\Dolphin2.wav",
    "D:\\Media\\Sounds\\Dolphin3.wav",
    "D:\\Media\\Sounds\\Dolphin4.wav",
    "D:\\Media\\Sounds\\Dolphin5.wav",
};


//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont                 m_Font;                 // Font object
    CXBHelp                 m_Help;                 // Help object

    // Sound members
    CWaveFile               m_awfSounds[6];         // Wave file parsers
    DWORD                   m_dwCurrent;            // Current sound
    BOOL                    m_bPlaying;             // Are we playing?
    LONG                    m_lVolume;              // Current volume
    LPDIRECTSOUND8          m_pDSound;              // DirectSound object
    LPDIRECTSOUNDBUFFER8    m_pDSBuffer;            // DirectSoundBuffer
    BYTE *                  m_pbSampleData;         // Sample data from wav

    // Sound source and listener positions
    D3DXVECTOR3             m_vSourcePosition;      // Source position vector
    D3DXVECTOR3             m_vListenerPosition;    // Listener position vector

    // Transform matrices
    D3DXMATRIX              m_matWorld;             // World transform
    D3DXMATRIX              m_matView;              // View transform
    D3DXMATRIX              m_matProj;              // Projection transform

    // Models for floor, source, and listener
    LPDIRECT3DVERTEXBUFFER8 m_pvbFloor;             // Quad for the floor
    LPDIRECT3DVERTEXBUFFER8 m_pvbSource;            // Quad for the source
    LPDIRECT3DVERTEXBUFFER8 m_pvbListener;          // Quad for the listener
    LPDIRECT3DVERTEXBUFFER8 m_pvbGrid;              // Lines to grid the floor

    D3DCOLOR        m_cSource;                      // Color for sound source
    D3DCOLOR        m_cListener;                    // Color for listener

    BOOL            m_bDrawHelp;                    // Should we draw help?
    BOOL            m_bControlSource;               // Control source (TRUE) or
                                                    // listener (FALSE)

    HRESULT SwitchToSound( DWORD dwIndex );         // Sets up a different sound
    HRESULT DownloadScratch(PCHAR pszScratchFile);  // downloads a default DSP image to the GP

public:
    virtual HRESULT Initialize();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();

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
    m_bDrawHelp = FALSE;

    // Sounds
    m_lVolume = DSBVOLUME_MAX;
    m_pbSampleData = NULL;

    // Positions
    m_vSourcePosition    = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_vListenerPosition = D3DXVECTOR3( 0.0f, 0.0f, ZMIN );
}




//-----------------------------------------------------------------------------
// Name: DownloadScratch
// Desc: Downloads a DSP scratch image to the DSP
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DownloadScratch(PCHAR pszScratchFile)
{
    HANDLE hFile;
    DWORD dwSize;
    PVOID pBuffer = NULL;
    HRESULT hr = S_OK;
    LPDSEFFECTIMAGEDESC pDesc;

    // open scratch image file generated by xps2 tool
    hFile = CreateFile( pszScratchFile,
                        GENERIC_READ,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        NULL );

    if( hFile == INVALID_HANDLE_VALUE )
    {
        DWORD err;

        err = GetLastError();
        OUTPUT_DEBUG_STRING( "Failed to open the dsp image file.\n" );
        hr = HRESULT_FROM_WIN32(err);
    }

    if( SUCCEEDED(hr) )
    {
        // Determine the size of the scratch image by seeking to
        // the end of the file
        dwSize = SetFilePointer( hFile, 0, NULL, FILE_END );
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    
        // Allocate memory to read the scratch image from disk
        pBuffer = new BYTE[dwSize];

        // Read the image in
        DWORD dwBytesRead;
        BOOL bResult = ReadFile( hFile,
                                 pBuffer,
                                 dwSize,
                                 &dwBytesRead,
                                 0 );
        
        if (!bResult)
        {
            DWORD err;

            err = GetLastError();
            OUTPUT_DEBUG_STRING( "\n Failed to open the dsp image file.\n" );
            hr = HRESULT_FROM_WIN32(err);
        }

    }

    if( SUCCEEDED(hr) )
    {
        // call dsound api to download the image..
        if (SUCCEEDED(hr))
        {
            hr = m_pDSound->DownloadEffectsImage( pBuffer,
                                                  dwSize,
                                                  NULL,
                                                  &pDesc );
        }
    }

    delete[] pBuffer;

    if( hFile != INVALID_HANDLE_VALUE ) 
    {
        CloseHandle( hFile );
    }
    
    return hr;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    int i, j;

    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create DirectSound
    if( FAILED( DirectSoundCreate( NULL, &m_pDSound, NULL ) ) )
        return E_FAIL;

    /* If the application doesn't care about vertical HRTF positioning,
       calling DirectSoundUseLightHRTF can save about 60k of memory. */
    // DirectSoundUseLightHRTF();
    DirectSoundUseFullHRTF();

    //
    // download a default scratch image that contains crosstalk
    // We must do this to get any 3d voice sound out the speakers
    //
    if( FAILED( DownloadScratch("d:\\media\\crosstalk.bin") ) )
        return E_FAIL;

    //
    // For testing purposes:
    // Make sure that all our files have the same format,
    // since we're re-using the samd DirectSoundBuffer for
    // all the files.
    //
    WAVEFORMATEX wfFirst;
    WAVEFORMATEX wfWalk;
    for( i = 0; i < sizeof( g_aszFileNames ) / sizeof( g_aszFileNames[0] ); i++ )
    {
        if( FAILED( m_awfSounds[ i ].Open( g_aszFileNames[i] ) ) )
            return XBAPPERR_MEDIANOTFOUND;

        // Testing only:
        // Verify they're all the same format, since we're only going to have
        // one sound buffer.
        if( i == 0 )
        {
            if( FAILED( m_awfSounds[ i ].GetFormat( &wfFirst, sizeof( WAVEFORMATEX ) ) ) )
                return E_FAIL;
        }
        else
        {
            if( FAILED( m_awfSounds[ i ].GetFormat( &wfWalk, sizeof( WAVEFORMATEX ) ) ) )
                return E_FAIL;

            if( memcmp( &wfFirst, &wfWalk, sizeof( WAVEFORMATEX ) ) )
            {
                OUTPUT_DEBUG_STRING( "All wav files should have the same format!\n" );
                return E_FAIL;
            }
        }
    }

    //
    // Create a sound buffer of 0 size, since we're going to use
    // SetBufferData
    //
    DSBUFFERDESC dsbdesc;
    ZeroMemory( &dsbdesc, sizeof( DSBUFFERDESC ) );
    dsbdesc.dwSize = sizeof( DSBUFFERDESC );

    /* If fewer than 256 buffers are in existence at all points during 
       the game, it may be more efficient not to use LOCDEFER. */
    // dsbdesc.dwFlags = DSBCAPS_CTRL3D;
    dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_LOCDEFER;
    dsbdesc.dwBufferBytes = 0;
    dsbdesc.lpwfxFormat = &wfFirst;

    if( FAILED( m_pDSound->CreateSoundBuffer( &dsbdesc, &m_pDSBuffer, NULL ) ) )
        return E_FAIL;

    //
    // Set the transform matrices
    //

    D3DXVECTOR3 vEyePt      = D3DXVECTOR3( XMIN, 45.0f,  ZMAX / 2.0f );
    D3DXVECTOR3 vLookatPt   = D3DXVECTOR3( XMIN,  0.0f,  ZMAX / 2.0f );
    D3DXVECTOR3 vUpVec      = D3DXVECTOR3( 0.0f,  0.0f,  1.0f );
    D3DXMatrixIdentity( &m_matWorld );
    D3DXMatrixLookAtLH( &m_matView, &vEyePt, &vLookatPt, &vUpVec );
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 10000.0f );

    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

    //
    // Create our vertex buffers
    //
    m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( D3DVERTEX ), 0, 0, 0, &m_pvbFloor );
    m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( D3DVERTEX ), 0, 0, 0, &m_pvbSource );
    m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( D3DVERTEX ), 0, 0, 0, &m_pvbListener );
    m_pd3dDevice->CreateVertexBuffer( 2 * ( ( ZMAX - ZMIN + 1 ) + ( XMAX - XMIN + 1 ) ) * sizeof( D3DVERTEX ), 0, 0, 0, &m_pvbGrid );
    
    D3DVERTEX * pVertices;

    // Fill the VB for the floor
    m_pvbFloor->Lock( 0, 0, (BYTE **)&pVertices, 0 );
    pVertices[0].p = D3DXVECTOR3( XMIN, 0.0f, ZMIN ); pVertices[0].c = 0xFF101010;
    pVertices[1].p = D3DXVECTOR3( XMIN, 0.0f, ZMAX ); pVertices[1].c = 0xFF101010;
    pVertices[2].p = D3DXVECTOR3( XMAX, 0.0f, ZMIN ); pVertices[2].c = 0xFF101010;
    pVertices[3].p = D3DXVECTOR3( XMAX, 0.0f, ZMAX ); pVertices[3].c = 0xFF101010;
    m_pvbFloor->Unlock();

    // Fill the VB for the grid
    m_pvbGrid->Lock( 0, 0, (BYTE **)&pVertices, 0 );
    for( i = ZMIN, j = 0; i <= ZMAX; i++, j++ )
    {
        pVertices[ j * 2 ].p     = D3DXVECTOR3( XMIN, 0, (FLOAT)i ); pVertices[ j * 2 ].c     = 0xFF00A000;
        pVertices[ j * 2 + 1 ].p = D3DXVECTOR3( XMAX, 0, (FLOAT)i ); pVertices[ j * 2 + 1 ].c = 0xFF00A000;
    }
    for( i = XMIN; i <= XMAX; i++, j++ )
    {
        pVertices[ j * 2 ].p     = D3DXVECTOR3( (FLOAT)i, 0, ZMIN ); pVertices[ j * 2 ].c     = 0xFF00A000;
        pVertices[ j * 2 + 1 ].p = D3DXVECTOR3( (FLOAT)i, 0, ZMAX ); pVertices[ j * 2 + 1 ].c = 0xFF00A000;
    }
    m_pvbGrid->Unlock();


    // Set up and play our initial sound
    m_dwCurrent = 0;
    SwitchToSound( m_dwCurrent );
    m_bPlaying = TRUE;
    m_pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: SwitchToSound
// Desc: Switches to the given sound by:
//       1) Stop playback if we're playing
//       2) Reallocate the sample data buffer
//       3) Point the DirectSoundBuffer to the new data
//       4) Restart plyaback if needed
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::SwitchToSound( DWORD dwIndex )
{
    DWORD dwNewSize;

    // If we're currently playing, stop, so that we don't crash
    // when we reallocate our buffer
    if( m_bPlaying )
        m_pDSBuffer->Stop( );

    // Calling stop doesn't immediately shut down
    // the voice, so point it away from our buffer
    m_pDSBuffer->SetBufferData( NULL, 0 );

    // Find out how big the new sample is
    m_awfSounds[ dwIndex ].GetDuration( &dwNewSize );

    // Set our allocation to that size
    if( m_pbSampleData )
        delete[] m_pbSampleData;
    m_pbSampleData = new BYTE[ dwNewSize ];
    if( !m_pbSampleData )
        return E_OUTOFMEMORY;

    // Read sample data from the file
    m_awfSounds[ dwIndex ].ReadSample( 0, m_pbSampleData, dwNewSize, &dwNewSize );

    // Set up values for the new buffer
    m_pDSBuffer->SetBufferData( m_pbSampleData, dwNewSize );
    m_pDSBuffer->SetLoopRegion( 0, dwNewSize );
    m_pDSBuffer->SetCurrentPosition( 0 );

    // If we were playing before, restart playback now
    if( m_bPlaying )
        m_pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    D3DVERTEX *     pVertices;
    D3DXVECTOR3     vSourceOld   = m_vSourcePosition;
    D3DXVECTOR3     vListenerOld = m_vListenerPosition;
    D3DXVECTOR3 *   pvControl;
    DWORD           dwPulse = DWORD( ( cosf( m_fAppTime * 6.0f ) + 1.0f ) * 50 );
    D3DCOLOR        cBlend = dwPulse | ( dwPulse << 8 ) | ( dwPulse << 16 );

    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

    // Increase/Decrease volume
    m_lVolume += LONG( ( m_DefaultGamepad.bAnalogButtons[ XINPUT_GAMEPAD_BLACK ] - 
                   m_DefaultGamepad.bAnalogButtons[ XINPUT_GAMEPAD_WHITE ] ) *
                   m_fElapsedTime * 
                   VOLUME_SCALE );

    // Make sure volume is in the appropriate range
    if( m_lVolume < DSBVOLUME_MIN )
        m_lVolume = DSBVOLUME_MIN;
    else if( m_lVolume > DSBVOLUME_MAX )
        m_lVolume = DSBVOLUME_MAX;

    // Toggle sound on and off
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] )
    {
        if( m_bPlaying )
            m_pDSBuffer->Stop( );
        else
            m_pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );

        m_bPlaying = !m_bPlaying;
    }

    // Cycle through sounds
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] )
    {
        m_dwCurrent = ( m_dwCurrent + 1 ) % ( sizeof( g_aszFileNames ) / sizeof( g_aszFileNames[0] ) );
        SwitchToSound( m_dwCurrent );
    }

    // Switch which of source vs. listener we are moving
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_X ] )
    {
        m_bControlSource = !m_bControlSource;
    }

    // Set up our colors
    m_cSource   = SOURCE_COLOR   | (  m_bControlSource ? cBlend : 0 );
    m_cListener = LISTENER_COLOR | ( !m_bControlSource ? cBlend : 0 );

    // Point to the appropriate vector
    pvControl = m_bControlSource ? &m_vSourcePosition : &m_vListenerPosition;

    //
    // Move selected object and clamp to the appropriate range
    //
    pvControl->x += m_DefaultGamepad.fX1 * m_fElapsedTime * MOTION_SCALE;
    if( pvControl->x < XMIN )
        pvControl->x = XMIN;
    else if( pvControl->x > XMAX )
        pvControl->x = XMAX;

    pvControl->z += m_DefaultGamepad.fY1 * m_fElapsedTime * MOTION_SCALE;
    if( pvControl->z < ZMIN )
        pvControl->z = ZMIN;
    else if( pvControl->z > ZMAX )
        pvControl->z = ZMAX;

    pvControl->y += m_DefaultGamepad.fY2 * m_fElapsedTime * MOTION_SCALE;
    if( pvControl->y < YMIN )
        pvControl->y = YMIN;
    else if( pvControl->y > YMAX )
        pvControl->y = YMAX;

    //
    // Update source/listener vertex buffers
    //
    m_pvbSource->Lock( 0, 0, (BYTE **)&pVertices, 0 );
    pVertices[0].p = m_vSourcePosition + D3DXVECTOR3( -0.5f, 0.0f, -0.5f ); pVertices[0].c = m_cSource;
    pVertices[1].p = m_vSourcePosition + D3DXVECTOR3( -0.5f, 0.0f,  0.5f ); pVertices[1].c = m_cSource;
    pVertices[2].p = m_vSourcePosition + D3DXVECTOR3(  0.5f, 0.0f, -0.5f ); pVertices[2].c = m_cSource;
    pVertices[3].p = m_vSourcePosition + D3DXVECTOR3(  0.5f, 0.0f,  0.5f ); pVertices[3].c = m_cSource;
    m_pvbSource->Lock( 0, 0, (BYTE **)&pVertices, 0 );

    m_pvbListener->Lock( 0, 0, (BYTE **)&pVertices, 0 );
    pVertices[0].p = m_vListenerPosition + D3DXVECTOR3( -0.5f, 0.0f, -0.5f ); pVertices[0].c = m_cListener;
    pVertices[1].p = m_vListenerPosition + D3DXVECTOR3( -0.5f, 0.0f,  0.5f ); pVertices[1].c = m_cListener;
    pVertices[2].p = m_vListenerPosition + D3DXVECTOR3(  0.5f, 0.0f, -0.5f ); pVertices[2].c = m_cListener;
    pVertices[3].p = m_vListenerPosition + D3DXVECTOR3(  0.5f, 0.0f,  0.5f ); pVertices[3].c = m_cListener;
    m_pvbListener->Lock( 0, 0, (BYTE **)&pVertices, 0 );

    //
    // Position the sound and listener in 3D. 
    // We use DS3D_DEFERRED so that all the changes will 
    // be commited at once.
    // We scale the velocities by 2 so that doppler effect
    // is a bit more noticable.
    //
    D3DXVECTOR3 vListenerVelocity = 2.0f * ( m_vListenerPosition - vListenerOld ) / m_fElapsedTime;
    D3DXVECTOR3 vSoundVelocity = 2.0f * ( m_vSourcePosition - vSourceOld ) / m_fElapsedTime;

    // Source position/velocity/volume
    m_pDSBuffer->SetPosition( m_vSourcePosition.x, m_vSourcePosition.y, m_vSourcePosition.z, DS3D_DEFERRED );
    m_pDSBuffer->SetVelocity( vSoundVelocity.x, vSoundVelocity.y, vSoundVelocity.z, DS3D_DEFERRED );
    m_pDSBuffer->SetVolume( m_lVolume );

    // Listener position/velocity
    m_pDSound->SetPosition( m_vListenerPosition.x, m_vListenerPosition.y, m_vListenerPosition.z, DS3D_DEFERRED  );
    m_pDSound->SetVelocity( vListenerVelocity.x, vListenerVelocity.y, vListenerVelocity.z, DS3D_DEFERRED );

    DSI3DL2BUFFER ds3db = {0};

    ds3db.lDirect = 0;
    ds3db.lDirectHF = 0;
    ds3db.lRoom = 0;
    ds3db.lRoomHF = 0;
    ds3db.flRoomRolloffFactor = 0.0f;
    ds3db.Obstruction.lHFLevel = 0;
    ds3db.Obstruction.flLFRatio = 0.0f;
    ds3db.Occlusion.lHFLevel = 0;
    ds3db.Occlusion.flLFRatio = 0.0f;
    m_pDSBuffer->SetI3DL2Source( &ds3db, DS3D_DEFERRED );

    // Commit position/velocity changes
    m_pDSound->CommitDeferredSettings();

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
    //
    // call dsound to do some work of low priority queued items such
    // as notifications, stream packets completions, etc
    //
    DirectSoundDoWork();

    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0xFF000000, 1.0f, 0L );

    RenderGradientBackground( 0xFF408040, 0xFF404040 );

    // Set default render states
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,         FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pd3dDevice->SetVertexShader( D3DFVF_D3DVERTEX );

    // Draw the floor
    m_pd3dDevice->SetStreamSource( 0, m_pvbFloor, sizeof( D3DVERTEX ) );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

    // Draw the grid
    m_pd3dDevice->SetStreamSource( 0, m_pvbGrid, sizeof( D3DVERTEX ) );
    m_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, 2 * ( ( ZMAX - ZMIN + 1 ) + ( XMAX - XMIN + 1 ) ) );

    // Draw the source
    m_pd3dDevice->SetStreamSource( 0, m_pvbSource, sizeof( D3DVERTEX ) );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

    // Draw the listener
    m_pd3dDevice->SetStreamSource( 0, m_pvbListener, sizeof( D3DVERTEX ) );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        WCHAR szBuff[200];

		m_Font.Begin();
        // Show frame rate
        m_Font.DrawText(  64, 50, 0xffffffff, L"I3DL2" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        // Show status
        swprintf( szBuff, L"Current Sound: %S", g_aszFileNames[ m_dwCurrent ] );
        m_Font.DrawText( 64, 100, m_bPlaying ? 0xFFFFFFFF : 0xFF404040, szBuff );
        swprintf( szBuff, L"Source: <%0.1f, %0.1f, %0.1f>", m_vSourcePosition.x, m_vSourcePosition.y, m_vSourcePosition.z );
        m_Font.DrawText( 64, 130, m_cSource, szBuff );
        swprintf( szBuff, L"Listener: <%0.1f, %0.1f, %0.1f>", m_vListenerPosition.x, m_vListenerPosition.y, m_vListenerPosition.z );
        m_Font.DrawText( 64, 160, m_cListener, szBuff );
        swprintf( szBuff, L"Volume: %d%%", 100 * ( m_lVolume - DSBVOLUME_MIN ) / (DSBVOLUME_MAX - DSBVOLUME_MIN ) );
        m_Font.DrawText( 64, 190, 0xFFFFFF00, szBuff );
		m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




