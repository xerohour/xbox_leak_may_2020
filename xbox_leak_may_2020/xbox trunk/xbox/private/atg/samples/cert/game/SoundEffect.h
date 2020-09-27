//-----------------------------------------------------------------------------
// File: SoundEffect.h
//
// Desc: Plays a WAV file sound effect
//
// Hist: 06.21.01 - New for July XDK
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _SOUND_EFFECT_H_
#define _SOUND_EFFECT_H_

#include <xtl.h>
#include "XbSound.h"




//-----------------------------------------------------------------------------
// Name: class CSoundEffect
// Desc: WAV sound effect object
//-----------------------------------------------------------------------------
class CSoundEffect
{
private:
    BOOL                          m_bIsPlaying;             //  TRUE if sound is playing
    BOOL                          m_bIsInitialized;         //  TRUE if Initialized() was called
    BOOL                          m_bIsPaused;              //  TRUE if the sound is paused
    FLOAT                         m_fVolume;                //  Volume level expressed as percentage (0 .. 100)
    LPDIRECTSOUND8                m_pdsndDevice;            //  DSound object
    LPDIRECTSOUNDBUFFER8          m_pDSBuffer;              //  DSound buffer object
    CWaveFile                     m_WaveFile;               //  Wave File object
    BYTE                         *m_pbSampleData;           //  Buffer for data from the sound
    INT                           m_iCurrentIdx;            //  Currently playing sound

public:
    CSoundEffect();
    ~CSoundEffect();

    BOOL    IsInitialized() const;
    BOOL    IsPaused() const;
    BOOL    IsDone();

    HRESULT Initialize( LPDIRECTSOUND8 pdsound );
    HRESULT PlaySound( BOOL bLooping = TRUE );
    VOID    Stop();
    VOID    Pause();
    VOID    SetVolume( FLOAT fVolume );
    VOID    SetPitch( LONG lPitch );
};

#endif // _SOUND_EFFECT_H_