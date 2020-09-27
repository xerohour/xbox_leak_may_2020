//-----------------------------------------------------------------------------
// File: ManualPanning.cpp
//
// Desc: The ManualPanning sample demonstrates how to perform panning by 
//       sending a buffer to different mixbins and controlling the mixbin
//       volume.  This sample demonstrates the mixbins for individual speakers,
//       but the process is the same for non-speaker mixbins
//
// Hist: 4.30.01 - New for June release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBSound.h>
#include <dsound.h>
#include <xgraphics.h>
#include "dsstdfx.h"

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle playback" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_2, L"Change sound" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle Looping" },
    { XBHELP_BLACK_BUTTON, XBHELP_PLACEMENT_2, L"Increase volume" },
    { XBHELP_DPAD,         XBHELP_PLACEMENT_2, L"Select\nspeaker" },
    { XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_2, L"Decrease volume" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Change speaker\nvolume" },
};

#define NUM_HELP_CALLOUTS 8

// List of wav files to cycle through
const char g_strBasePath[] = "D:\\Media\\Sounds\\";
char * g_aszFileNames[] = 
{
    "heli.wav",
    "DockingMono.wav",
    "EngineStartMono.wav",
    "MaleDialog1.wav",
    "MiningMono.wav",
    "MusicMono.wav",
    "Dolphin4.wav",
};
static const DWORD NUM_SOUNDS = sizeof( g_aszFileNames ) / sizeof( g_aszFileNames[0] );

// Struct containing speaker information
typedef struct
{
    DWORD   dwMixBinID;
    LONG    lVolume;
    WCHAR * szDescription;
} OPTION_STRUCT;

// List of speakers/mixbins we can configure
OPTION_STRUCT g_aOptions[] =
{
    { DSMIXBIN_FRONT_LEFT,      DSBVOLUME_MAX,  L"Front Left" },
    { DSMIXBIN_FRONT_RIGHT,     DSBVOLUME_MAX,  L"Front Right" },
    { DSMIXBIN_FRONT_CENTER,    DSBVOLUME_MAX,  L"Front Center" },
    { DSMIXBIN_LOW_FREQUENCY,   DSBVOLUME_MAX,  L"Low Frequency" },
    { DSMIXBIN_BACK_LEFT,       DSBVOLUME_MAX,  L"Back Left" },
    { DSMIXBIN_BACK_RIGHT,      DSBVOLUME_MAX,  L"Back Right" },
};
static const DWORD NUM_OPTIONS = sizeof( g_aOptions ) / sizeof( g_aOptions[0] );

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

    // Font and help
    BOOL        m_bDrawHelp;
    CXBFont     m_Font;
    CXBHelp     m_Help;

    CWaveFile               m_awfSounds[NUM_SOUNDS];// Wave file parsers
    DWORD                   m_dwCurrent;            // Current sound
    BOOL                    m_bPlaying;             // Are we playing?
    LONG                    m_lVolume;              // Current volume
    LPDIRECTSOUNDBUFFER8    m_pDSBuffer;            // DirectSoundBuffer
    BYTE *                  m_pbSampleData;         // Sample data from wav
    BOOL                    m_bLooping;             // TRUE if loop is enabled
    DWORD                   m_dwOption;             // Selected option
    LPDIRECTSOUND8          m_pDSound;              // DirectSound object


    HRESULT SwitchToSound( DWORD dwIndex );         // Sets up a different sound
    HRESULT DownloadEffectsImage(PCHAR pszScratchFile);  // downloads a default DSP image to the GP
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

    // Sounds
    m_lVolume = DSBVOLUME_MAX;
    m_pbSampleData = NULL;
    m_dwOption = 0;
}



//-----------------------------------------------------------------------------
// Name: DownloadEffectsImage
// Desc: Downloads an effects image to the DSP
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DownloadEffectsImage(PCHAR pszScratchFile)
{
    HANDLE hFile;
    DWORD dwSize;
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
// Name: Initialize
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::Initialize()
{
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

    //
    // For testing purposes:
    // Make sure that all our files have the same format,
    // since we're re-using the samd DirectSoundBuffer for
    // all the files.  First, we have to determine how
    // much space to allocate for the wave format, since
    // the XBOXADPCMWAVEFORMAT is 2 bytes larger than
    // WAVEFORMATEX.
    //
    DWORD          cbFirst;
    WAVEFORMATEX * pwfFirst = NULL;
    DWORD          cbWalk;
    WAVEFORMATEX * pwfWalk = NULL;
    for( int i = 0; i < NUM_SOUNDS; i++ )
    {
        char strFullPath[MAX_PATH];

        strcpy( strFullPath, g_strBasePath );
        strcat( strFullPath, g_aszFileNames[i] );
        if( FAILED( m_awfSounds[ i ].Open( strFullPath ) ) )
            return XBAPPERR_MEDIANOTFOUND;

        // Testing only:
        // Verify they're all the same format, since we're only going to have
        // one sound buffer.
        if( i == 0 )
        {
            // Allocate space for the wave format
            m_awfSounds[ i ].GetFormat( NULL, 0, &cbFirst );
            pwfFirst = (WAVEFORMATEX *)new BYTE[ cbFirst ];

            if( FAILED( m_awfSounds[ i ].GetFormat( pwfFirst, cbFirst ) ) )
                return E_FAIL;
        }
        else
        {
            // Allocate space for the wave format
            m_awfSounds[ i ].GetFormat( NULL, 0, &cbWalk );
            pwfWalk = (WAVEFORMATEX *)new BYTE[ cbWalk ];

            if( FAILED( m_awfSounds[ i ].GetFormat( pwfWalk, cbWalk ) ) )
                return E_FAIL;

            if( cbWalk != cbFirst ||
                memcmp( pwfFirst, pwfWalk, cbFirst ) )
            {
                OUTPUT_DEBUG_STRING( "All wav files should have the same format!\n" );
                return E_FAIL;
            }

            delete[] pwfWalk;
        }
    }

    //
    // Create a sound buffer of 0 size, since we're going to use
    // SetBufferData
    //
    DSBUFFERDESC dsbdesc;
    DSMIXBINS dsmixbins;
    DSMIXBINVOLUMEPAIR dsmbvp[6];
    ZeroMemory( &dsbdesc, sizeof( DSBUFFERDESC ) );
    dsbdesc.dwSize = sizeof( DSBUFFERDESC );

    /* If fewer than 256 buffers are in existence at all points during 
       the game, it may be more efficient not to use LOCDEFER. */
    dsbdesc.dwFlags = DSBCAPS_LOCDEFER;
    dsbdesc.dwBufferBytes = 0;
    dsbdesc.lpwfxFormat = pwfFirst;
    dsbdesc.lpMixBins = &dsmixbins;

    dsmixbins.dwMixBinCount = 6;
    dsmixbins.lpMixBinVolumePairs = dsmbvp;

    dsmbvp[0].dwMixBin = DSMIXBIN_FRONT_LEFT;
    dsmbvp[0].lVolume = DSBVOLUME_MAX;
    dsmbvp[1].dwMixBin = DSMIXBIN_FRONT_RIGHT;
    dsmbvp[1].lVolume = DSBVOLUME_MAX;
    dsmbvp[2].dwMixBin = DSMIXBIN_FRONT_CENTER;
    dsmbvp[2].lVolume = DSBVOLUME_MAX;
    dsmbvp[3].dwMixBin = DSMIXBIN_LOW_FREQUENCY;
    dsmbvp[3].lVolume = DSBVOLUME_MAX;
    dsmbvp[4].dwMixBin = DSMIXBIN_BACK_LEFT;
    dsmbvp[4].lVolume = DSBVOLUME_MAX;
    dsmbvp[5].dwMixBin = DSMIXBIN_BACK_RIGHT;
    dsmbvp[5].lVolume = DSBVOLUME_MAX;

    if( FAILED( DirectSoundCreateBuffer( &dsbdesc, &m_pDSBuffer ) ) )
        return E_FAIL;

    // Set up and play our initial sound
    m_dwCurrent = 0;
    m_bPlaying = TRUE;
    m_bLooping = TRUE;
    SwitchToSound( m_dwCurrent );

    // Clean up our memory allocations
    delete[] pwfFirst;

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
    DWORD dwLoopStartSample, dwLoopLengthSamples;
    DWORD cbLoopStart, cbLoopLength;

    // If we're currently playing, stop, so that we don't crash
    // when we reallocate our buffer
    if( m_bPlaying )
    {
        m_pDSBuffer->Stop();
    }

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

    // Check for embedded loop points
    if( SUCCEEDED( m_awfSounds[ dwIndex ].GetLoopRegion( &dwLoopStartSample, &dwLoopLengthSamples ) ) )
    {
        WAVEFORMATEX wfx;

        // We're not interested in the whole wave format, which is
        // why we can get away with just loading the core WAVEFORMATEX
        m_awfSounds[ dwIndex ].GetFormat( &wfx, sizeof( WAVEFORMATEX ) );

        // We need to convert the loop points from sample counts to
        // byte offsets, but it's slightly different between PCM and ADPCM
        if( wfx.wFormatTag == WAVE_FORMAT_XBOX_ADPCM )
        {
            // For ADPCM, calculate # of blocks and multiply that
            // by bytes per block.  Xbox ADPCM is always 64 samples
            // per block.
            cbLoopStart = dwLoopStartSample / 64 * wfx.nBlockAlign;
            cbLoopLength = dwLoopLengthSamples / 64 * wfx.nBlockAlign;
        }
        else
        {
            // For PCM, multiply by bytes per sample
            DWORD cbBytesPerSample = wfx.nChannels * wfx.wBitsPerSample / 8;
            cbLoopStart = dwLoopStartSample * cbBytesPerSample;
            cbLoopLength = dwLoopLengthSamples * cbBytesPerSample;
        }
    }
    else
    {
        // Otherwise, just loop the whole file
        cbLoopStart = 0;
        cbLoopLength = dwNewSize;
    }


    // Set up values for the new buffer
    m_pDSBuffer->SetBufferData( m_pbSampleData, dwNewSize );
    m_pDSBuffer->SetLoopRegion( cbLoopStart, cbLoopLength );
    m_pDSBuffer->SetCurrentPosition( 0 );

    // If we were playing before, restart playback now
    if( m_bPlaying )
    {
        m_pDSBuffer->Play( 0, 0, m_bLooping ? DSBPLAY_LOOPING : 0 );
    }

    return S_OK;
}


#define VOLUME_SCALE 5.0f
//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::FrameMove()
{
    // Check if buffer is still playing
    DWORD dwStatus;
    m_pDSBuffer->GetStatus( &dwStatus );
    m_bPlaying = dwStatus & DSBSTATUS_PLAYING;

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
        {
            // Start playback at beginning of buffer
            m_pDSBuffer->SetCurrentPosition( 0 );
            m_pDSBuffer->Play( 0, 0, m_bLooping ? DSBPLAY_LOOPING : 0 );
            m_bPlaying = TRUE;
        }
    }

    // Cycle through sounds
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] )
    {
        m_dwCurrent = ( m_dwCurrent + 1 ) % NUM_SOUNDS;
        SwitchToSound( m_dwCurrent );
    }

    // Cycle through speakers
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
    {
        m_dwOption = ( m_dwOption + NUM_OPTIONS - 1 ) % NUM_OPTIONS;
    }
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
    {
        m_dwOption = ( m_dwOption + 1 ) % NUM_OPTIONS;
    }

    // Adjust selected speaker volume
    LONG lDelta = LONG(m_DefaultGamepad.fX2 * m_fElapsedTime * 1500 );
    g_aOptions[ m_dwOption ].lVolume += lDelta;
    if( g_aOptions[ m_dwOption ].lVolume < DSBVOLUME_MIN )
        g_aOptions[ m_dwOption ].lVolume = DSBVOLUME_MIN;
    else if( g_aOptions[ m_dwOption ].lVolume > DSBVOLUME_MAX )
        g_aOptions[ m_dwOption ].lVolume = DSBVOLUME_MAX;

    // Toggle looping
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_X ] )
    {
        m_bLooping = !m_bLooping;

        // If we were playing, make another call to play to change looping
        if( m_bPlaying )
            m_pDSBuffer->Play( 0, 0, m_bLooping ? DSBPLAY_LOOPING : 0 );
    }

    // Set overall buffer volume
    m_pDSBuffer->SetVolume( m_lVolume );
    
    // Build up a bitmap with active speakers and an array 
    // of active speaker volumes
    DSMIXBINS dsmixbins;
    DSMIXBINVOLUMEPAIR dsmbvp[DSMIXBIN_ASSIGNMENT_MAX];

    dsmixbins.dwMixBinCount = 0;
    dsmixbins.lpMixBinVolumePairs = dsmbvp;

    for( int i = 0; i < sizeof( g_aOptions ) / sizeof( g_aOptions[0] ); i++ )
    {
        // If this speaker is more than minimum volume, 
        if( g_aOptions[i].lVolume > DSBVOLUME_MIN )
        {
            // Add it to the bitmap and volume array
            dsmbvp[dsmixbins.dwMixBinCount].dwMixBin = g_aOptions[i].dwMixBinID;
            dsmbvp[dsmixbins.dwMixBinCount].lVolume = g_aOptions[i].lVolume;

            dsmixbins.dwMixBinCount++;
        }
    }

    // Set all speaker volumes
    m_pDSBuffer->SetMixBinVolumes( &dsmixbins );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::Render()
{
    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    DirectSoundDoWork();

    // Draw a gradient filled background
    RenderGradientBackground( 0xff404040, 0xff404080 );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        WCHAR szBuff[200];

        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"ManualPanning" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        // Show status
        swprintf( szBuff, L"Current Sound: %S %s", g_aszFileNames[ m_dwCurrent ], m_bLooping ? L"(Looping)" : L"" );
        m_Font.DrawText( 64, 100, m_bPlaying ? 0xFFFFFFFF : 0xFF808080, szBuff );
        swprintf( szBuff, L"Buffer Volume: %ddB (%0.0f%%)", m_lVolume / 100, 100.0f * ( m_lVolume - DSBVOLUME_MIN ) / ( DSBVOLUME_MAX - DSBVOLUME_MIN ) );
        m_Font.DrawText( 64, 130, 0xFFFFFF00, szBuff );

        for( int i = 0; i < sizeof( g_aOptions ) / sizeof( g_aOptions[0] ); i++ )
        {
            DWORD dwColor;
            
            // Determine color based on if speaker is selected and if it's active
            if( m_dwOption == i )
                dwColor = ( g_aOptions[i].lVolume == DSBVOLUME_MIN ) ? 0xFFE0E0E0 : 0xFFFFFFFF;
            else
                dwColor = ( g_aOptions[i].lVolume == DSBVOLUME_MIN ) ? 0xFFA0A000 : 0xFFFFFF00;

            swprintf( szBuff, L"%s: %ddB (%0.0f%%)", g_aOptions[i].szDescription, 
                                           g_aOptions[i].lVolume / 100,
                                           100.0f * ( g_aOptions[i].lVolume - DSBVOLUME_MIN ) / ( DSBVOLUME_MAX - DSBVOLUME_MIN ) );
            m_Font.DrawText( 64, 160.0f + i * 30, dwColor, szBuff );
        }
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

