//-----------------------------------------------------------------------------
// File: EnumSoundtrack.cpp
//
// Desc: Enumerate WMA soundtracks
//
// Hist: 02.16.01 - New for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "EnumSoundtrack.h"
#include <cassert>


//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

// Maximum viewable songs
const DWORD MAX_SONGS_DISPLAYED = 10;

// Must be this far from center on 0.0 - 1.0 scale
const FLOAT JOY_THRESHOLD = 0.25f;

// Text colors
const DWORD COLOR_HIGHLIGHT = 0xff00ff00;
const DWORD COLOR_NORMAL = 0xffffffff;

// Controller repeat values
const FLOAT fINITIAL_REPEAT = 0.333f; // 333 mS recommended for first repeat
const FLOAT fSTD_REPEAT     = 0.085f; // 85 mS recommended for repeat rate




//-----------------------------------------------------------------------------
// Name: Song()
// Desc: Construct song object
//-----------------------------------------------------------------------------
CXBEnumSoundtrack::Song::Song( DWORD dwId, const WCHAR* strAlbum, 
                               const WCHAR* strSong, DWORD dwLength )
:
    m_dwId( dwId ),
    m_strAlbum(),
    m_strSong(),
    m_dwLength( dwLength )
{
    lstrcpynW( m_strAlbum, strAlbum, MAX_SOUNDTRACK_NAME );
    lstrcpynW( m_strSong, strSong, MAX_SONG_NAME );
}




//-----------------------------------------------------------------------------
// Name: GetAlbum()
// Desc: Returns the album name
//-----------------------------------------------------------------------------
const WCHAR* CXBEnumSoundtrack::Song::GetAlbum() const
{
    return m_strAlbum;
}




//-----------------------------------------------------------------------------
// Name: GetSong()
// Desc: Returns the song name
//-----------------------------------------------------------------------------
const WCHAR* CXBEnumSoundtrack::Song::GetSong() const
{
    return m_strSong;
}




//-----------------------------------------------------------------------------
// Name: GetLength()
// Desc: Returns length of song in mS
//-----------------------------------------------------------------------------
DWORD CXBEnumSoundtrack::Song::GetLength() const
{
    return m_dwLength;
}




//-----------------------------------------------------------------------------
// Name: GetLength()
// Desc: Formats the incoming string as "MM:SS"
//-----------------------------------------------------------------------------
VOID CXBEnumSoundtrack::Song::GetLength( CHAR* strMMSS ) const
{
    assert( strMMSS != NULL );

    // Convert to seconds
    DWORD dwSeconds = m_dwLength / 1000;

    // Round to nearest second
    if( m_dwLength - ( dwSeconds * 1000 ) >= 500 )
        ++dwSeconds;

    // Determine minutes
    DWORD dwMinutes = dwSeconds / 60;

    // Remaining seconds
    dwSeconds -= ( dwMinutes * 60 );

    // Format in strMMSS
    wsprintfA( strMMSS, "%lu:%02lu", dwMinutes, dwSeconds );
}




//-----------------------------------------------------------------------------
// Name: Open()
// Desc: Open the soundtrack WMA song and return file handle
//-----------------------------------------------------------------------------
HANDLE CXBEnumSoundtrack::Song::Open( BOOL bAsync ) const
{
    return XOpenSoundtrackSong( m_dwId, bAsync );
}




//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CXBEnumSoundtrack xbApp;

    if( FAILED( xbApp.Create() ) )
        return;

    xbApp.Run();
}




//-----------------------------------------------------------------------------
// Name: CXBEnumSoundtrack()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBEnumSoundtrack::CXBEnumSoundtrack()
:
    CXBApplication(),
    m_Font        (),
    m_RepeatTimer ( FALSE ),
    m_fRepeatDelay( fINITIAL_REPEAT ),
    m_SongList    (),
    m_iCurrSong   ( 0 ),
    m_iTopSong    ( 0 ),
    m_hSongFile   ( INVALID_HANDLE_VALUE ),
    m_Stream      (),
    m_bPlaying    ( FALSE )
{
}




//-----------------------------------------------------------------------------
// Name: DownloadEffectsImage
// Desc: Downloads an effects image to the DSP
//-----------------------------------------------------------------------------
HRESULT CXBEnumSoundtrack::DownloadEffectsImage(PCHAR pszScratchFile)
{
    HANDLE hFile;
    DWORD dwSize = 0;
    PVOID pBuffer = NULL;
    HRESULT hr = S_OK;
    LPDSEFFECTIMAGEDESC pDesc;
    DSEFFECTIMAGELOC EffectLoc;

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
        EffectLoc.dwI3DL2ReverbIndex = I3DL2_CHAIN_I3DL2_REVERB;
        EffectLoc.dwCrosstalkIndex = I3DL2_CHAIN_XTALK;

        hr = m_pDSound->DownloadEffectsImage( pBuffer,
                                              dwSize,
                                              &EffectLoc,
                                              &pDesc );
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
// Desc: Sets up the enum soundtrack example
//-----------------------------------------------------------------------------
HRESULT CXBEnumSoundtrack::Initialize()
{
    // Set the matrices
    D3DXVECTOR3 vEye(-2.5f, 2.0f, -4.0f );
    D3DXVECTOR3 vAt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );

    D3DXMATRIX matWorld, matView, matProj;
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &vEye,&vAt, &vUp );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 100.0f );

    m_pd3dDevice->SetTransform( D3DTS_WORLD,      &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_VIEW,       &matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    if( FAILED( DirectSoundCreate( NULL, &m_pDSound, NULL ) ) )
        return E_FAIL;

    //
    // download the standard DirectSound effecs image
    //
    if( FAILED( DownloadEffectsImage("d:\\media\\dsstdfx.bin") ) )
        return E_FAIL;

    m_bDrawHelp = FALSE;

    // Load the list of soundtracks
    InitSoundtrackList();
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitSoundtrackList()
// Desc: Load all soundtracks into memory
//-----------------------------------------------------------------------------
VOID CXBEnumSoundtrack::InitSoundtrackList()
{
    // Enumerate all soundtracks
    XSOUNDTRACK_DATA SoundtrackData;
    HANDLE hFind = XFindFirstSoundtrack( &SoundtrackData );
    if( hFind != INVALID_HANDLE_VALUE )
    {
        AddSoundtrackSongs( SoundtrackData );
        while( XFindNextSoundtrack( hFind, &SoundtrackData ) )
            AddSoundtrackSongs( SoundtrackData );
        XFindClose( hFind );
    }
}




//-----------------------------------------------------------------------------
// Name: AddSoundtrackSongs()
// Desc: Load all songs from the given soundtrack into the list
//-----------------------------------------------------------------------------
VOID CXBEnumSoundtrack::AddSoundtrackSongs( const XSOUNDTRACK_DATA& SoundtrackData )
{
    // Get each song
    for( UINT i = 0; i < SoundtrackData.uSongCount; ++i )
    {
        DWORD dwSongId;
        DWORD dwSongLength;
        WCHAR strSong[ MAX_SONG_NAME ];
        if( XGetSoundtrackSongInfo( SoundtrackData.uSoundtrackId, i, &dwSongId,
                                    &dwSongLength, strSong, MAX_SONG_NAME ) )
        {
            // Add it to the list
            m_SongList.push_back( Song( dwSongId, SoundtrackData.szName, 
                                        strSong, dwSongLength ) );
        }
    }
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame; the entry point for animating the scene
//-----------------------------------------------------------------------------
HRESULT CXBEnumSoundtrack::FrameMove()
{
    ValidateState();

    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

    // Poll the system for events
    Event ev = GetControllerEvent();

    // Update the current state
    UpdateState( ev );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d rendering.
//       This function sets up render states, clears the viewport, and renders
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBEnumSoundtrack::Render()
{
    // Keep any WMA file playing
    if( FAILED( m_Stream.Process() ) )
        return E_FAIL;

    DirectSoundDoWork();

    // Clear the viewport, zbuffer, and stencil buffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x000000ff, 1.0f, 0L );

    // Draw a gradient filled background
    RenderGradientBackground( 0xff102030, 0xff405060 );

    // Draw title & framerate
    m_Font.Begin();
    m_Font.DrawText(  64, 50, 0xffffffff, L"EnumSoundtrack" );
    m_Font.DrawText( 460, 50, 0xffffff00, m_strFrameRate );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        if( m_SongList.empty() )
        {
            m_Font.DrawText( 80.0f, 68.0f, COLOR_NORMAL, L"No soundtracks available" );
        }
        else
        {
            // Show the list of songs
            FLOAT fTop = 90.0f;
            FLOAT fOffset = 24.0f;
            SongListIndex j = 0;
            for( SongListIndex i = m_iTopSong; i < m_SongList.size() &&
                                               j < MAX_SONGS_DISPLAYED; ++i, ++j )
            {
                const Song& song = m_SongList[ i ];

                CHAR strMMSS[32];
                song.GetLength( strMMSS );

                WCHAR strSong[ 256 ];
                wsprintfW( strSong, L"%ls: %ls (%hs)", song.GetAlbum(), song.GetSong(), 
                           strMMSS );
                DWORD dwColor = ( i == m_iCurrSong ) ? COLOR_HIGHLIGHT : COLOR_NORMAL;

                m_Font.DrawText( 80.0f, fTop + (fOffset * j ), dwColor, strSong );
            }

            // Show scroll arrows
            BOOL bShowTopArrow = m_iTopSong > 0;
            BOOL bShowBtmArrow = m_iTopSong + MAX_SONGS_DISPLAYED < m_SongList.size();
            if( bShowTopArrow )
                m_Font.DrawText( 80.0f, 48.0f, COLOR_NORMAL, L"^" );
            if( bShowBtmArrow )
                m_Font.DrawText( 80.0f, 320.0f, COLOR_NORMAL, L"v" );

            // Descriptive text
            m_Font.DrawText( 320.0f, 
                             320.0f, 
                             COLOR_NORMAL, 
                             m_bPlaying ? L"Playing..." : L"Stopped", 
                             XBFONT_CENTER_X );
            m_Font.DrawText( 320.0f, 380.0f, COLOR_NORMAL, L"A play    B stop",
                             XBFONT_CENTER_X );
        }
    }

    m_Font.End();

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ValidateState()
// Desc: Check object invariants
//-----------------------------------------------------------------------------
VOID CXBEnumSoundtrack::ValidateState() const
{
    assert( m_SongList.empty() || m_iCurrSong < m_SongList.size() );
    assert( m_iTopSong <= m_iCurrSong );
    assert( m_iCurrSong < m_iTopSong + MAX_SONGS_DISPLAYED );
}




//-----------------------------------------------------------------------------
// Name: IsAnyButtonActive()
// Desc: TRUE if any button depressed or any thumbstick offset on the given
//       controller.
//-----------------------------------------------------------------------------
BOOL IsAnyButtonActive( const XBGAMEPAD* pGamePad )
{
    // Check digital buttons
    if( pGamePad->wButtons )
        return TRUE;

    // Check analog buttons
    for( DWORD i = 0; i < 8; ++i )
    {
        if( pGamePad->bAnalogButtons[ i ] )
            return TRUE;
    }

    // Check thumbsticks
    if( pGamePad->fX1 || pGamePad->fY1 || pGamePad->fX2 || pGamePad->fX2  )
        return TRUE;

    // Nothing active
    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: GetPrimaryController()
// Desc: The primary controller is the first controller used by a player.
//       If no controller has been used or the controller has been removed,
//       the primary controller is the controller inserted at the lowest 
//       port number. Function returns NULL if no controller is inserted.
//-----------------------------------------------------------------------------
const XBGAMEPAD* GetPrimaryController()
{
    static INT nPrimaryController = -1;

    // If primary controller has been set and hasn't been removed, use it
    const XBGAMEPAD* pGamePad = NULL;
    if( nPrimaryController != -1 )
    {
        pGamePad = &g_Gamepads[ nPrimaryController ];
        if( pGamePad->hDevice != NULL )
            return pGamePad;
    }

    // Primary controller hasn't been set or has been removed...

    // Examine each inserted controller to see if any is being used
    INT nFirst = -1;
    for( DWORD i=0; i < XGetPortCount(); ++i )
    {
        pGamePad = &g_Gamepads[i];
        if( pGamePad->hDevice != NULL )
        {
            // Remember the lowest inserted controller ID
            if( nFirst == -1 )
                nFirst = i;

            // If any button is active, we found the primary controller
            if( IsAnyButtonActive( pGamePad ) )
            {
                nPrimaryController = i;
                return pGamePad;
            }
        }
    }

    // No controllers are inserted
    if( nFirst == -1 )
        return NULL;

    // The primary controller hasn't been set and no controller has been
    // used yet, so return the controller on the lowest port number
    pGamePad = &g_Gamepads[ nFirst ];
    return pGamePad;
}




//-----------------------------------------------------------------------------
// Name: GetControllerEvent()
// Desc: Polls the controller for events. Handles button repeats.
//-----------------------------------------------------------------------------
CXBEnumSoundtrack::Event CXBEnumSoundtrack::GetControllerEvent()
{
    const XBGAMEPAD* pGamePad = GetPrimaryController();
    if( pGamePad != NULL )
    {
        // Handle button press and joystick hold repeats
        if( IsAnyButtonActive( pGamePad ) )
        {
            // If the timer is running, the button is being held. If it's
            // held long enough, it triggers a repeat. If the timer isn't
            // running, we start it.
            if( m_RepeatTimer.IsRunning() )
            {
                // If the timer is running but hasn't expired, bail out
                if( m_RepeatTimer.GetElapsedSeconds() < m_fRepeatDelay )
                    return EV_NULL;

                m_fRepeatDelay = fSTD_REPEAT;
                m_RepeatTimer.StartZero();
            }
            else
            {
                m_fRepeatDelay = fINITIAL_REPEAT;
                m_RepeatTimer.StartZero();
            }
        }
        else
        {
            // No buttons or joysticks active; kill the repeat timer
            m_fRepeatDelay = fINITIAL_REPEAT;
            m_RepeatTimer.Stop();
        }

        // Primary buttons
        if( pGamePad->wButtons & XINPUT_GAMEPAD_START )
            return EV_START_BUTTON;
        if( pGamePad->wButtons & XINPUT_GAMEPAD_BACK )
            return EV_BACK_BUTTON;
        if( pGamePad->bAnalogButtons[ XINPUT_GAMEPAD_A ] )
            return EV_A_BUTTON;
        if( pGamePad->bAnalogButtons[ XINPUT_GAMEPAD_B ] )
            return EV_B_BUTTON;

        // Movement
        if( pGamePad->wButtons & XINPUT_GAMEPAD_DPAD_UP ||
            pGamePad->fY1 > JOY_THRESHOLD )
            return EV_UP;
        if( pGamePad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN ||
            pGamePad->fY1 < -JOY_THRESHOLD )
            return EV_DOWN;
    }

    // No controllers inserted or no button presses
    return EV_NULL;
}




//-----------------------------------------------------------------------------
// Name: UpdateState()
// Desc: State machine updates the current context based on the incoming event
//-----------------------------------------------------------------------------
VOID CXBEnumSoundtrack::UpdateState( Event ev )
{
    switch( ev )
    {
        case EV_NULL:
            break;
        case EV_A_BUTTON:
            Start();
            break;
        case EV_B_BUTTON:
            Stop();
            break;
        case EV_UP:
            // If we're at the top of the displayed list, shift the display
            if( m_iCurrSong == m_iTopSong )
            {
                if( m_iTopSong > 0 )
                    --m_iTopSong;
            }
            // Move to previous song
            if( m_iCurrSong > 0 )
                --m_iCurrSong;
            break;
        case EV_DOWN:
            // If we're at the bottom of the displayed list, shift the display
            if( m_iCurrSong == m_iTopSong + MAX_SONGS_DISPLAYED - 1 )
            {
                if( m_iTopSong + MAX_SONGS_DISPLAYED < m_SongList.size() )
                    ++m_iTopSong;
            }
            // Move to next song
            if( m_iCurrSong < m_SongList.size() - 1 )
                ++m_iCurrSong;
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: Start()
// Desc: Start the selected song
//-----------------------------------------------------------------------------
VOID CXBEnumSoundtrack::Start()
{
    if( !m_bPlaying )
    {
        Stop();

        // Open the current song
        m_hSongFile = m_SongList[ m_iCurrSong ].Open( FALSE );

        // Start playback
        m_Stream.Start( m_hSongFile );
        m_bPlaying = TRUE;
    }
}




//-----------------------------------------------------------------------------
// Name: Stop()
// Desc: Stop the song that's playing and close the file
//-----------------------------------------------------------------------------
VOID CXBEnumSoundtrack::Stop()
{
    m_bPlaying = FALSE;
    // Stop the current song
    m_Stream.Stop();

    // Close the file
    if( m_hSongFile != INVALID_HANDLE_VALUE )
    {
        CloseHandle( m_hSongFile );
        m_hSongFile = INVALID_HANDLE_VALUE;
    }
}
