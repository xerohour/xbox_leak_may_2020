//-----------------------------------------------------------------------------
// File: EnvelopeGenerator.cpp
//
// Desc: The EnvelopeGenerator sample demonstrates how to control looping and
//       Envelope Generators in DirectSound
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
#include <stddef.h> // For offsetof
#include "dsstdfx.h"

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle playback" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_2, L"Change sound" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle\nLooping" },
    { XBHELP_Y_BUTTON,     XBHELP_PLACEMENT_1, L"Change EG" },
    { XBHELP_BLACK_BUTTON, XBHELP_PLACEMENT_2, L"Increase volume" },
    { XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_1, L"Decrease volume" },
    { XBHELP_DPAD,         XBHELP_PLACEMENT_2, L"Select\nparameter" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Change value" },
};

#define NUM_HELP_CALLOUTS 9

// List of wav files to cycle through
const char* g_strBasePath = "D:\\Media\\Sounds\\";
char* g_aszFileNames[] = 
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

// Struct for changing parameters of DSENVELOPEDESC
typedef struct 
{
    DWORD   dwOffset;       // Offset into EnvelopeDesc of field being changed
    LONG    dwMinValue;     // Minimum value of option
    LONG    dwMaxValue;     // Maximum value of option
    BOOL    bEG2;           // TRUE if only available in EG2 (DSEG_MULTI)
    WCHAR * szDescription;  // Description of option
} OPTION_STRUCT;

// List of parameters we can change
OPTION_STRUCT g_aOptions[] =
{
    { offsetof( DSENVELOPEDESC, dwDelay ),      0,  0xFFF, FALSE,  L"Delay" },
    { offsetof( DSENVELOPEDESC, dwAttack ),     0,  0xFFF, FALSE,  L"Attack" },
    { offsetof( DSENVELOPEDESC, dwHold ),       0,  0xFFF, FALSE,  L"Hold" },
    { offsetof( DSENVELOPEDESC, dwDecay ),      0,  0xFFF, FALSE,  L"Decay" },
    { offsetof( DSENVELOPEDESC, dwSustain ),    0,  0xFF,  FALSE,  L"Sustain" },
    { offsetof( DSENVELOPEDESC, dwRelease ),    0,  0xFFF, FALSE,  L"Release" },
    { offsetof( DSENVELOPEDESC, lPitchScale ),  -128, 127, TRUE,   L"Pitch Scale" },
    { offsetof( DSENVELOPEDESC, lFilterCutOff), -128, 127, TRUE,   L"Filter Cutoff" },
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
    DSENVELOPEDESC          m_adsedEG[2];           // Envelope Generate settings
    DWORD                   m_dwParam;              // Selected parameter
    DWORD                   m_dwEG;                 // Selected EG
    BOOL                    m_bLooping;             // True if loop is enabled
    LPDIRECTSOUND8          m_pDSound;              // DirectSound object

    // Acceleration factor for controller input
    FLOAT                   m_fAcceleration;

    HRESULT SwitchToSound( DWORD dwIndex );         // Sets up a different sound
    HRESULT DrawEnvelope();                         // Draw the envelope graph
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
    
    m_fAcceleration = 1.0f;
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
    // Initialize EnvelopeGenerators
    //
    m_dwEG = 0;
    m_dwParam = 0;
    ZeroMemory( m_adsedEG, 2 * sizeof( DSENVELOPEDESC ) );
    m_adsedEG[0].dwEG = DSEG_AMPLITUDE;
    m_adsedEG[0].dwMode = DSEG_MODE_DELAY;
    m_adsedEG[1].dwEG = DSEG_MULTI;
    m_adsedEG[1].dwMode = DSEG_MODE_DELAY;
    m_adsedEG[0].dwSustain = 0xFF;


    //
    // Create a sound buffer of 0 size, since we're going to use
    // SetBufferData
    //
    DSBUFFERDESC dsbdesc;
    ZeroMemory( &dsbdesc, sizeof( DSBUFFERDESC ) );
    dsbdesc.dwSize = sizeof( DSBUFFERDESC );

    /* If fewer than 256 buffers are in existence at all points during 
       the game, it may be more efficient not to use LOCDEFER. */
    dsbdesc.dwFlags = DSBCAPS_LOCDEFER;
    dsbdesc.dwBufferBytes = 0;
    dsbdesc.lpwfxFormat = pwfFirst;
    if( FAILED( DirectSoundCreateBuffer( &dsbdesc, &m_pDSBuffer ) ) )
        return E_FAIL;

    // Set up and play our initial sound
    // If we play the buffer with the EGs disabled (as they are by
    // default), then changing the EG's after playback started 
    // wouldn't have any affect.  Therefore, set the EG's BEFORE
    // starting playback
    m_pDSBuffer->SetEG( &m_adsedEG[0] );
    m_pDSBuffer->SetEG( &m_adsedEG[1] );
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
        {
            m_pDSBuffer->StopEx( 0, DSBSTOPEX_ENVELOPE );
        }
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

    // Cycle through EnvelopeDesc parameters
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
    {
        do {
            m_dwParam = ( m_dwParam + NUM_OPTIONS - 1 ) % NUM_OPTIONS;
        } while ( g_aOptions[ m_dwParam ].bEG2 && m_dwEG == 0 );
    }
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
    {
        do {
            m_dwParam = ( m_dwParam + 1 ) % NUM_OPTIONS;
        } while ( g_aOptions[ m_dwParam ].bEG2 && m_dwEG == 0 );
    }

    // Switch between EGs
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_Y ] )
    {
        m_dwEG ^= 1;

        // If we were on an EG2-only param, change to first param
        if( m_dwEG == 0 && g_aOptions[ m_dwParam ].bEG2 ) {

            m_dwParam = 0;
        }
            
    }

    // Adjust value of currently selected parameter, making
    // sure to keep it within the appropriate range
    LONG * plValue = (LONG *)((BYTE *)&m_adsedEG[ m_dwEG ] + g_aOptions[ m_dwParam ].dwOffset);
    LONG lDelta = LONG(m_DefaultGamepad.fX2 * m_fElapsedTime * 100 * m_fAcceleration);

    if( lDelta == 0 )
    {
        // Reset acceleration
        m_fAcceleration = 1.0f;
    }
    else
    {
        if( lDelta < 0 && *plValue + lDelta < g_aOptions[ m_dwParam ].dwMinValue )
            lDelta = g_aOptions[ m_dwParam ].dwMinValue - *plValue;
        else if( lDelta > 0 && *plValue + lDelta > g_aOptions[ m_dwParam ].dwMaxValue )
            lDelta = g_aOptions[ m_dwParam ].dwMaxValue - *plValue;

        // Adjust acceleration factor
        m_fAcceleration += 0.03f;
    }

    // Write out the new value
    *plValue += lDelta;

    // Toggle looping
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_X ] )
    {
        m_bLooping = !m_bLooping;

        // If we were playing, make another call to play to change looping
        if( m_bPlaying )
            m_pDSBuffer->Play( 0, 0, m_bLooping ? DSBPLAY_LOOPING : 0 );
    }


    //
    // To use Filtercutoff in the MULTI EG, we need to set the DLS2 lowpass filter in 
    // the hardware to some reasonable values otherwise when we try to change the 
    // FilterCutoff, nothing will happen. FilterCutoff is relative to the resonant 
    // frequency we set below.  We set both pairs of coefficients to the same value
    // to handle stereo buffers, since we don't plan to use ParamEQ.
    // However, we don't want to use the filter unless we're actually using the
    // FilterCutOff in the EG, otherwise we'll lowpass filter ALL sounds
    //
    DSFILTERDESC            dsfd = {0};

    if( m_adsedEG[1].lFilterCutOff == 0 )
        dsfd.dwMode = DSFILTER_MODE_BYPASS;
    else
        dsfd.dwMode = DSFILTER_MODE_DLS2;
    dsfd.adwCoefficients[0] = 0xE283; // ~4kHz
    dsfd.adwCoefficients[1] = 0x8FF5; // ~3dB
    dsfd.adwCoefficients[2] = 0xE283; // ~4kHz
    dsfd.adwCoefficients[3] = 0x8FF5; // ~3dB
    m_pDSBuffer->SetFilter( &dsfd );

    // Update settings
    m_pDSBuffer->SetVolume( m_lVolume );
    m_pDSBuffer->SetEG( &m_adsedEG[0] );
    m_pDSBuffer->SetEG( &m_adsedEG[1] );

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
    RenderGradientBackground( 0xff402040, 0xff404040 );

    // Draw a graph of the envelope
    DrawEnvelope();

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        WCHAR szBuff[200];

        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"EnvelopeGenerator" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        // Show status
        swprintf( szBuff, L"Current Sound: %S %s", g_aszFileNames[ m_dwCurrent ], m_bLooping ? L"(looping)" : L"" );
        m_Font.DrawText( 64, 100, m_bPlaying ? 0xFFFFFFFF : 0xFF808080, szBuff );
        swprintf( szBuff, L"Volume: %ddB (%0.0f%%)", m_lVolume / 100, 100.0f * ( m_lVolume - DSBVOLUME_MIN ) / (DSBVOLUME_MAX - DSBVOLUME_MIN ) );
        m_Font.DrawText( 64, 130, 0xFFFFFF00, szBuff );
        for( int i = 0; i < sizeof( g_aOptions ) / sizeof( g_aOptions[0] ); i++ )
        {
            // For each valid option, print out name and value
            if( m_dwEG != 0 || !g_aOptions[i].bEG2 )
            {
                swprintf( szBuff, L"EG%d %s: %d", m_dwEG + 1, g_aOptions[ i ].szDescription, *(LONG *)((BYTE *)&m_adsedEG[ m_dwEG ] + g_aOptions[ i ].dwOffset) );
                m_Font.DrawText( 64, FLOAT(160 + i * 30), i == m_dwParam ? 0xFFFFFFFF : 0xFFFFFF00, szBuff );
            }
        }
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}



#define ENV_XMIN 320
#define ENV_XMAX 580
#define ENV_YMIN 200
#define ENV_YMAX 400
#define ENV_XRANGE ( ENV_XMAX - ENV_XMIN )
#define ENV_YRANGE ( ENV_YMAX - ENV_YMIN )

//-----------------------------------------------------------------------------
// Name: DrawEnvelope
// Desc: Draws a graph of the envelope
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::DrawEnvelope()
{
    D3DXVECTOR4 ap[7];
    WAVEFORMATEX wfx;
    DWORD dwSize;
    FLOAT fDuration;

    // Calculate EG values in seconds
    FLOAT fDelay  = m_adsedEG[ m_dwEG ].dwDelay  * 512 / 48000.0f;
    FLOAT fAttack = m_adsedEG[ m_dwEG ].dwAttack * 512 / 48000.0f;
    FLOAT fHold   = m_adsedEG[ m_dwEG ].dwHold   * 512 / 48000.0f;
    FLOAT fDecay  = m_adsedEG[ m_dwEG ].dwDecay  * 512 / 48000.0f;
    FLOAT fSustain= m_adsedEG[ m_dwEG ].dwSustain / 255.0f;
    FLOAT fRelease= m_adsedEG[ m_dwEG ].dwRelease * 512 / 48000.0f;

    // Calculate length of sound in seconds
    m_awfSounds[ m_dwCurrent ].GetFormat( &wfx, sizeof( WAVEFORMATEX ) );
    m_awfSounds[ m_dwCurrent ].GetDuration( &dwSize );
    fDuration = ((float)dwSize) / (float)wfx.nAvgBytesPerSec;

    // Scale over the total length of the envelope
    FLOAT fTotal = fDelay + fAttack + fHold + fDecay + fDuration + fRelease;
    
    // Set vertices for graph
    ap[0] = D3DXVECTOR4( ENV_XMIN, ENV_YMAX, 0.0f, 1.0f );
    ap[1] = D3DXVECTOR4( ap[0].x + fDelay / fTotal * ENV_XRANGE, ENV_YMAX, 0.0f, 1.0f );
    ap[2] = D3DXVECTOR4( ap[1].x + fAttack / fTotal * ENV_XRANGE, ENV_YMIN, 0.0f, 1.0f );
    ap[3] = D3DXVECTOR4( ap[2].x + fHold / fTotal * ENV_XRANGE, ENV_YMIN, 0.0f, 1.0f );
    ap[4] = D3DXVECTOR4( ap[3].x + fDecay / fTotal * ( 1.0f - fSustain ) * ENV_XRANGE, ENV_YMAX - fSustain * ENV_YRANGE, 0.0f, 1.0f );
    ap[5] = D3DXVECTOR4( ENV_XMAX - fRelease / fTotal * fSustain * ENV_XRANGE, ENV_YMAX - fSustain * ENV_YRANGE, 0.0f, 1.0f );
    ap[6] = D3DXVECTOR4( ENV_XMAX, ENV_YMAX, 0.0f, 1.0f );

    // Draw the graph
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW );
    m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xFFFFFF00 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 6, ap, sizeof( D3DXVECTOR4 ) );

    return S_OK;
}
