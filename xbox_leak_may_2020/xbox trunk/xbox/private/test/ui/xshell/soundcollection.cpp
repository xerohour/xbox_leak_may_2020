/*****************************************************
*** soundcollection.cpp
***
*** CPP file for our Sound Collection class.
*** This class will contain all of the sounds for a
*** program, and allow them to be played
***
*** by James N. Helm
*** May 26th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "soundcollection.h"

extern CXSettings   g_XboxSettings; // Settings that are stored on the Xbox

// Constructor
CSoundCollection::CSoundCollection()
: m_pSoundArray( NULL )
{
    m_pSoundArray = new CWaveMedia[ENUM_SOUND_NUM_SOUNDS];   // Array of sounds to be used in the program
    if( NULL == m_pSoundArray )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CSoundCollection::CSoundCollection():Failed to allocate memory!!" );
    }
}


// Destructor
CSoundCollection::~CSoundCollection()
{
    CleanUpSounds();
}


// Used to clean up the sounds
void CSoundCollection::CleanUpSounds()
{
    if( NULL != m_pSoundArray )
    {
        delete[] m_pSoundArray;
        m_pSoundArray = NULL;
    }
}


// Initialize our sounds
HRESULT CSoundCollection::Init( LPDIRECTSOUND pDSound )
{
    HRESULT hr = S_OK;

    // Initialize and load our sounds
    if( !m_pSoundArray[ENUM_SOUND_SELECT].Init( pDSound, SOUND_SELECT_BUTTON ) )
    {
        hr = E_FAIL;
    }

    if( !m_pSoundArray[ENUM_SOUND_BACK].Init( pDSound, SOUND_BACK_BUTTON ) )
    {
        hr = E_FAIL;
    }

    if( !m_pSoundArray[ENUM_SOUND_MENU_ITEM_SELECT].Init( pDSound, SOUND_MENU_ITEM_SELECT ) )
    {
        hr = E_FAIL;
    }

    if( !m_pSoundArray[ENUM_SOUND_KEYBOARD_STROKE].Init( pDSound, SOUND_KEYBOARD_STROKE ) )
    {
        hr = E_FAIL;
    }

    // Adjust the volume of all sounds
    for( unsigned int x = 0; x < ENUM_SOUND_NUM_SOUNDS; x++ )
    {
        m_pSoundArray[x].SetVolume( SOUND_VOLUME_ADJUST );
    }

    return hr;
}


// Returns TRUE if a sound is currently playing
BOOL CSoundCollection::IsSoundPlaying( DWORD dwSoundId )
{
    if( dwSoundId >= ENUM_SOUND_NUM_SOUNDS )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CSoundCollection::IsSoundPlaying():SoundId is out of range!!" );

        return FALSE;
    }

    return m_pSoundArray[dwSoundId].IsPlaying();
}


// Allows the user to specify the Sound ID of the sound to be played
void CSoundCollection::PlaySound( DWORD dwSoundId )
{
    if( dwSoundId >= ENUM_SOUND_NUM_SOUNDS )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CSoundCollection::PlaySound():SoundId is out of range!!" );

        return;
    }

    m_pSoundArray[dwSoundId].Play();
}


// Plays the Select Button Sound
void CSoundCollection::PlaySelectSound()
{
    m_pSoundArray[ENUM_SOUND_SELECT].Play();
}


// Plays the Back Button Sound
void CSoundCollection::PlayBackSound()
{
    m_pSoundArray[ENUM_SOUND_BACK].Play();
}


// Plays the Menu Item Select Sound
void CSoundCollection::PlayMenuItemSelectSound()
{
    m_pSoundArray[ENUM_SOUND_MENU_ITEM_SELECT].Play();
}


// Plays the Keyboard Stroke Sound
void CSoundCollection::PlayKeyboardStrokeSound()
{
    m_pSoundArray[ENUM_SOUND_KEYBOARD_STROKE].Play();
}