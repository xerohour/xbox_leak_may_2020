/*****************************************************
*** soundcollection.h
***
*** Header file for our Sound Collection class.
*** This class will contain all of the sounds for a
*** program, and allow them to be played
***
*** by James N. Helm
*** May 26th, 2001
***
*****************************************************/

#ifndef _SOUNDCOLLECTION_H_
#define _SOUNDCOLLECTION_H_

#include "wavemedia.h"

class CSoundCollection
{
public:
    // Constructors and Destructor
    CSoundCollection();
    ~CSoundCollection();

    // Public Properties
    
    // Public Methods
    HRESULT Init( LPDIRECTSOUND pDSound );      // Initialize our sounds
    BOOL IsSoundPlaying( DWORD dwSoundId );     // Returns TRUE if a sound is currently playing
    void PlaySound( DWORD dwSoundId );          // Allows the user to specify the Sound ID of the sound to be played
    void PlaySelectSound();                     // Plays the Select Button Sound
    void PlayBackSound();                       // Plays the Back Button Sound
    void PlayMenuItemSelectSound();             // Plays the Menu Item Select Sound
    void PlayKeyboardStrokeSound();             // Plays the Keyboard Stroke Sound
    void CleanUpSounds();                       // Used to clean up the sounds

private:
    // Private Properties
    CWaveMedia* m_pSoundArray;                  // Array of sounds to be used in the program

    // Private Methods
};

#endif // _SOUNDCOLLECTION_H_