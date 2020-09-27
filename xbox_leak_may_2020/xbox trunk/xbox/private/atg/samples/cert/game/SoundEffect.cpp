//-----------------------------------------------------------------------------
// File: SoundEffect.cpp
//
// Desc: Plays a WAV file sound effect
//
// Hist: 06.21.01 - New for July XDK
//       06.28.01 - Converted to using pitch modulation instead of multiple files
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "SoundEffect.h"




//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

// The audible volume range is around [-40db, 0db], instead of the [-100db, 0db]
// the Xbox offers.  Interpolating across this yields better results.
const LONG VOLUME_MAX = DSBVOLUME_MAX;
const LONG VOLUME_MIN = -4000;

const CHAR* const strENGINE_SOUND = "D:\\Media\\Sounds\\engine.wav";




//-----------------------------------------------------------------------------
// Name: CSoundEffect()
// Desc: Construct sound effect object
//-----------------------------------------------------------------------------
CSoundEffect::CSoundEffect()
:
    m_bIsPlaying       ( FALSE ),
    m_bIsInitialized   ( FALSE ),
    m_bIsPaused        ( FALSE ),
    m_fVolume          ( 100.0f ),
    m_pdsndDevice      ( NULL ),
    m_pDSBuffer        ( NULL )
{
}




//-----------------------------------------------------------------------------
// Name: ~CSoundEffect()
// Desc: Deconstructs the object
//-----------------------------------------------------------------------------
CSoundEffect::~CSoundEffect()
{
    Stop();
    delete [] m_pbSampleData;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initializes a sound effect.  Only has to be called once for an object
//       pdsound must be a pointer to a valid object
//-----------------------------------------------------------------------------
HRESULT CSoundEffect::Initialize( LPDIRECTSOUND8 pdsound )
{
    m_pdsndDevice = pdsound;

    // Create a buffer of 0 size, since we don't know the size of the files 
    // we'll play
    WAVEFORMATEX wfx;
    DSBUFFERDESC dsbdesc;
    ZeroMemory( &dsbdesc, sizeof( DSBUFFERDESC ) );
    dsbdesc.dwSize = sizeof( DSBUFFERDESC );
    dsbdesc.dwFlags = 0;
    dsbdesc.dwBufferBytes = 0;
    dsbdesc.lpwfxFormat = &wfx;

    ZeroMemory( &wfx, sizeof( WAVEFORMATEX ) );
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 44100;
    wfx.cbSize = 0;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

    if( FAILED( m_pdsndDevice->CreateSoundBuffer( &dsbdesc, &m_pDSBuffer, NULL ) ) )
        return E_FAIL;

    // Grab the wave file and read it in
    if( FAILED( m_WaveFile.Open( strENGINE_SOUND ) ) )
        return E_FAIL;

    DWORD dwSize;
    m_WaveFile.GetDuration( &dwSize );
    m_pbSampleData = new BYTE[dwSize];
    if( !m_pbSampleData )
        return E_FAIL;

    m_WaveFile.ReadSample( 0, m_pbSampleData, dwSize, &dwSize );
    m_bIsInitialized = TRUE;
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: IsInitialized()
// Desc: Returns TRUE if Initialize() has been called
//-----------------------------------------------------------------------------
BOOL CSoundEffect::IsInitialized() const
{
    return m_bIsInitialized;
}




//-----------------------------------------------------------------------------
// Name: Start()
// Desc: Starts playing a sound
//-----------------------------------------------------------------------------
HRESULT CSoundEffect::PlaySound( BOOL bLooping )
{
    DWORD dwNewSize;

    // If we were paused, just start playing (if idx is the same)
    if( IsPaused() )
    {
        m_pDSBuffer->Play( 0, 0, bLooping ? DSBPLAY_LOOPING : 0 );
        m_bIsPaused = FALSE;
        return S_OK;
    }

    // If the sound is already being played, return
    if( m_bIsPlaying )
    {
        m_pDSBuffer->GetStatus( &dwNewSize );
        if( dwNewSize & DSBSTATUS_PLAYING )
            return S_OK;
    }

    m_WaveFile.GetDuration( &dwNewSize );
    m_pDSBuffer->SetBufferData( m_pbSampleData, dwNewSize );
    m_pDSBuffer->SetLoopRegion( 0, dwNewSize );
    m_pDSBuffer->SetCurrentPosition( 0 );
    m_pDSBuffer->SetVolume( (LONG)(VOLUME_MIN + 
                            (VOLUME_MAX - VOLUME_MIN)*(m_fVolume/100.0f)) );
    m_pDSBuffer->Play( 0, 0, bLooping ? DSBPLAY_LOOPING : 0 );
    m_bIsPlaying = TRUE;
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Stop()
// Desc: Stops sound playing
//-----------------------------------------------------------------------------
VOID CSoundEffect::Stop()
{
    if( m_bIsPlaying )
    {
        m_pDSBuffer->StopEx( 0, DSBSTOPEX_IMMEDIATE );
        m_pDSBuffer->SetBufferData( NULL, 0 );

        m_bIsPlaying = FALSE;
        m_bIsPaused = FALSE;
    }
}




//-----------------------------------------------------------------------------
// Name: Pause()
// Desc: Pauses the current sound
//-----------------------------------------------------------------------------
VOID CSoundEffect::Pause()
{
    if( m_bIsPlaying && !m_bIsPaused )
    {
        m_pDSBuffer->StopEx( 0, DSBSTOPEX_IMMEDIATE );
        m_bIsPaused = TRUE;
    }
}




//-----------------------------------------------------------------------------
// Name: IsPaused()
// Desc: TRUE if the current sound is paused
//-----------------------------------------------------------------------------
BOOL CSoundEffect::IsPaused() const
{
    if( m_bIsPlaying && m_bIsPaused )
        return TRUE;
    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: SetVolume
// Desc: Sets the volume of the sound effect object.  This propagates to all sounds played
//-----------------------------------------------------------------------------
VOID CSoundEffect::SetVolume( FLOAT fVolume )
{
    m_fVolume = fVolume;
}




//-----------------------------------------------------------------------------
// Name: SetPitch
// Desc: Sets the pitch of the currently playing sound
//-----------------------------------------------------------------------------
VOID CSoundEffect::SetPitch( LONG lPitch )
{
    if( m_pDSBuffer )
        m_pDSBuffer->SetPitch( lPitch );
}




//-----------------------------------------------------------------------------
// Name: IsDone
// Desc: Polls the buffer to see if it is playing and sets the proper fields
//-----------------------------------------------------------------------------
BOOL CSoundEffect::IsDone()
{
    DWORD dwStatus;

    if( !m_bIsPlaying )
        return TRUE;

    m_pDSBuffer->GetStatus( &dwStatus );
    if( dwStatus & DSBSTATUS_PLAYING )
        return FALSE;

    m_bIsPlaying = FALSE;
    m_bIsPaused = FALSE;
    return TRUE;
}
