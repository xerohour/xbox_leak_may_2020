//-----------------------------------------------------------------------------
// File: SetFilter.cpp
//
// Desc: The SetFilter sample demonstrates how to use the programmable filter
//       block provided by DirectSound.
//
// Hist: 4.30.01 - Created
//       7.18.01 - Added routines for calculating coefficients
//      10.15.01 - Updated Parametric EQ gain
//
// Warning: This sample is still in progress.  The routines for calculating
//      DLS2 coefficients are an approximation and are not perfect.
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
#include <math.h>

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle playback" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_2, L"Change sound" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle DLS2" },
    { XBHELP_Y_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle ParamEQ" },
    { XBHELP_BLACK_BUTTON, XBHELP_PLACEMENT_2, L"Increase volume" },
    { XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_2, L"Decrease volume" },
    { XBHELP_DPAD,         XBHELP_PLACEMENT_2, L"Select parameter" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Change parameter" },
};

#define NUM_HELP_CALLOUTS 9

// List of wav files to cycle through
char * g_aszFileNames[] = 
{
    "D:\\Media\\Sounds\\noise.wav",
    "D:\\Media\\Sounds\\55.wav",
    "D:\\Media\\Sounds\\110.wav",
    "D:\\Media\\Sounds\\220.wav",
    "D:\\Media\\Sounds\\440.wav",
    "D:\\Media\\Sounds\\880.wav",
    "D:\\Media\\Sounds\\1760.wav",
    "D:\\Media\\Sounds\\3520.wav",
    "D:\\Media\\Sounds\\7040.wav",
    "D:\\Media\\Sounds\\14080.wav",
};

#define NUM_SOUNDS 10


typedef struct
{
    DWORD dwScale;          // Joystick scale factor
    WCHAR * szDescription;  // Description of option
} OPTION_STRUCT;

OPTION_STRUCT g_aOptions[] =
{
    { 1000,  L"DLS2 Filter Freq" },
    { 10000, L"DLS2 Resonance" },
    { 1000, L"ParamEQ Filter Freq" },
    { 10000, L"ParamEQ Filter Gain" },
    { 10000, L"ParamEQ Filter Q" },
};

typedef enum
{
    DLS2_CutoffFrequency,
    DLS2_Resonance,
    PARAMEQ_Frequency,
    PARAMEQ_Gain,
    PARAMEQ_Q,
    NUM_OPTIONS,
} FILTER_OPTION;

#define RES_SCALE 1000
#define PEQ_GAIN_SCALE 400


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
    CXBFont     m_Font;
    CXBHelp     m_Help;
    BOOL        m_bDrawHelp;

    CWaveFile               m_awfSounds[NUM_SOUNDS];// Wave file parsers
    DWORD                   m_dwCurrent;            // Current sound
    BOOL                    m_bPlaying;             // Are we playing?
    LONG                    m_lVolume;              // Current volume
    LPDIRECTSOUNDBUFFER8    m_pDSBuffer;            // DirectSoundBuffer
    BYTE *                  m_pbSampleData;         // Sample data from wav
    DSFILTERDESC            m_dsfd;                 // DSFILTERDESC struct
    DWORD                   m_dwParam;              // Selected parameter
    BOOL                    m_bDLS2;                // Enable DLS2 filter
    BOOL                    m_bParamEQ;             // Enable ParamEQ filter
    LPDIRECTSOUND8          m_pDSound;              // DirectSound object

    FLOAT                   m_fDLS2Freq;            // DLS2 cutoff frequency
    LONG                    m_lDLS2Resonance;       // DLS2 resonance * RES_SCALE
    LONG                    m_lParamEQFreq;
    LONG                    m_lParamEQGain;
    LONG                    m_lParamEQQ;


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
    m_lDLS2Resonance = 0;
    m_fDLS2Freq = 440;
    m_lParamEQFreq = 1000;
    m_lParamEQGain = 4096;
    m_lParamEQQ = 0;
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
        if( FAILED( m_awfSounds[ i ].Open( g_aszFileNames[i] ) ) )
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
    m_dwCurrent = 0;
    SwitchToSound( m_dwCurrent );
    m_bPlaying = TRUE;
    // m_pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );

    // Disable filters to start with
    m_dwParam = 0;
    ZeroMemory( &m_dsfd, sizeof( DSFILTERDESC ) );
    m_bDLS2 = FALSE;
    m_bParamEQ = FALSE;

    // Clean up our memory allocation
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
        m_pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: FreqToHardwareCoeff
// Desc: Calculates coefficient value[0] for DLS2 filter from a frequency
//-----------------------------------------------------------------------------
ULONG FreqToHardwareCoeff( FLOAT fFreq )
{
    FLOAT fNormCutoff = fFreq / 48000.0f;

    // Filter is ineffective out of these ranges, so why
    // bother even trying?
    if( fFreq < 30.0f )
        return 0x8000;
    if( fFreq > 8000.0f )
        return 0x0;

    FLOAT fFC = FLOAT( 2.0f * sin( D3DX_PI * fNormCutoff ) );
    LONG lOctaves = LONG( 4096.0f * log( fFC ) / log( 2.0f ) );

    return (ULONG)lOctaves & 0xFFFF;
}



//-----------------------------------------------------------------------------
// Name: dBToHardwareCoeff
// Desc: Calculates coefficient value[1] for DLS2 filter from resonance in dB
//-----------------------------------------------------------------------------
ULONG dBToHardwareCoeff( LONG lResonance )
{
    FLOAT fResonance = (FLOAT)lResonance;

    if( fResonance > 22.5f )
        fResonance = 22.5f;

    double fQ = pow( 10.0, -0.05*fResonance);
    unsigned int dwQ = (unsigned int)(fQ*(1<<15));
    if( dwQ > 0xFFFF )
        dwQ = 0xFFFF;

    return (ULONG)dwQ;
}


 

#define VOLUME_SCALE 5.0f
//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::FrameMove()
{
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
        m_dwCurrent = ( m_dwCurrent + 1 ) % NUM_SOUNDS;
        SwitchToSound( m_dwCurrent );
    }

    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
    {
        if( m_dwParam == 0 )
            m_dwParam = NUM_OPTIONS - 1;
        else
            m_dwParam = ( m_dwParam - 1 );
    }
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
    {
        m_dwParam = ( m_dwParam + 1 ) % NUM_OPTIONS;
    }

    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_X ] )
    {
        m_dsfd.dwMode ^= DSFILTER_MODE_DLS2;
    }

    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_Y ] )
    {
        m_dsfd.dwMode ^= DSFILTER_MODE_PARAMEQ;
    }

    // Adjust value of currently selected parameter
    LONG lDelta = LONG(m_DefaultGamepad.fX2 * m_fElapsedTime * g_aOptions[ m_dwParam ].dwScale );
    switch( m_dwParam )
    {
    case DLS2_CutoffFrequency:
        m_fDLS2Freq += lDelta;
        if( m_fDLS2Freq < 0.0f )
            m_fDLS2Freq = 0.0f;
        else if( m_fDLS2Freq > 8000 )
            m_fDLS2Freq = 8000;
        break;
    case DLS2_Resonance:
        m_lDLS2Resonance += lDelta;
        if( m_lDLS2Resonance < 0 )
            m_lDLS2Resonance = 0;
        else if( m_lDLS2Resonance > 22 * RES_SCALE )
            m_lDLS2Resonance = 22 * RES_SCALE;
        break;
    case PARAMEQ_Frequency:
        m_lParamEQFreq += lDelta;
        if( m_lParamEQFreq < 0 )
            m_lParamEQFreq = 0;
        else if( m_lParamEQFreq > 8000 )
            m_lParamEQFreq = 8000;
        break;
    case PARAMEQ_Gain:
        m_lParamEQGain += lDelta;
        if( m_lParamEQGain < -8192 )
            m_lParamEQGain = -8192;
        else if( m_lParamEQGain > 8191 )
            m_lParamEQGain = 8191;
        break;
    case PARAMEQ_Q:
        m_lParamEQQ += lDelta;
        if( m_lParamEQQ < 0 )
            m_lParamEQQ = 0;
        else if( m_lParamEQQ / RES_SCALE > 7 )
            m_lParamEQQ = 7 * RES_SCALE;
        break;
    }

    // 
    // Note!!!:
    // For stereo buffers, where you can only use DLS2 OR ParamEQ, you must 
    // set both pairs of coefficients (one for each channel).
    //
    m_dsfd.adwCoefficients[0] = FreqToHardwareCoeff( m_fDLS2Freq );
    m_dsfd.adwCoefficients[1] = dBToHardwareCoeff( m_lDLS2Resonance / RES_SCALE );
    m_dsfd.adwCoefficients[2] = FreqToHardwareCoeff( FLOAT(m_lParamEQFreq) );
    m_dsfd.adwCoefficients[3] = m_lParamEQGain & 0xFFFF;
    m_dsfd.dwQCoefficient = m_lParamEQQ / RES_SCALE;

    m_pDSBuffer->SetVolume( m_lVolume );
    m_pDSBuffer->SetFilter( &m_dsfd );

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
        m_Font.DrawText(  64, 50, 0xffffffff, L"SetFilter" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        // Show status
        swprintf( szBuff, L"Current Sound: %S", g_aszFileNames[ m_dwCurrent ] );
        m_Font.DrawText( 64, 100, m_bPlaying ? 0xFFFFFFFF : 0xFF606060, szBuff );
        swprintf( szBuff, L"Volume: %ddB (%0.0f%%)", m_lVolume / 100, 100.0f * ( m_lVolume - DSBVOLUME_MIN ) / (DSBVOLUME_MAX - DSBVOLUME_MIN ) );
        m_Font.DrawText( 64, 130, 0xFFFFFF00, szBuff );
        swprintf( szBuff, L"DLS2 %s  ParamEQ %s", m_dsfd.dwMode & DSFILTER_MODE_DLS2 ? L"Enabled" : L"Disabled", m_dsfd.dwMode & DSFILTER_MODE_PARAMEQ ? L"Enabled" : L"Disabled" );
        m_Font.DrawText( 64, 160, 0xFFFFFF00, szBuff );

        for( int i = 0; i < NUM_OPTIONS; i++ )
        {
            switch( i )
            {
            case DLS2_CutoffFrequency:
                swprintf( szBuff, L"%s: %0.2fHz", g_aOptions[i].szDescription, m_fDLS2Freq );
                break;
            case DLS2_Resonance:
                swprintf( szBuff, L"%s: %lddB", g_aOptions[i].szDescription, m_lDLS2Resonance / RES_SCALE );
                break;
            case PARAMEQ_Frequency:
                swprintf( szBuff, L"%s: %ldHz", g_aOptions[i].szDescription, m_lParamEQFreq );
                break;
            case PARAMEQ_Gain:
                swprintf( szBuff, L"%s: %lddB", g_aOptions[i].szDescription, m_lParamEQGain / PEQ_GAIN_SCALE );
                break;
            case PARAMEQ_Q:
                swprintf( szBuff, L"%s: %ld", g_aOptions[i].szDescription, m_lParamEQQ / RES_SCALE );
                break;
            }
            m_Font.DrawText( 64, FLOAT(190 + i * 30), i == m_dwParam ? 0xFFFFFFFF : 0xFFFFFF00, szBuff );
        }

        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

