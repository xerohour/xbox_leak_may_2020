/*****************************************************
*** wavemedia.h
***
*** H file for a WAV file media object.  This class
*** will allow a user to specify the DirectSound
*** pointer, and a WAV file name, and will allow
*** the user to playback the sound
***
*** by James N. Helm
*** May 25th, 2001
***
*****************************************************/

#ifndef _WAVEMEDIA_H_
#define _WAVEMEDIA_H_

#define WAVEM_APP_TITLE_NAME_A  "WAVM"

class CWaveMedia
{
public:
    // Constructors and Destructors
    CWaveMedia();
    ~CWaveMedia();

    // Public Properties
    // Public Methods
    BOOL IsPlaying();                           // Returns TRUE if the sound is currently playing
    void Play();                                // Play the sound
    void Stop();                                // Stop playing the sound
    void ResetToBeginning();                    // Reset the current playback pointer to the beginning of a sound
    void SetVolume( LONG lVolume );             // Set the volume of the Wave object
    
    BOOL Init( LPDIRECTSOUND pDSound,           // Initialize the Wave Media Object
               char* pszWaveFileName,
               LONG lDefaultVolume = 0 );

    void CleanUp();                             // Used to cleanup (de-initialize) the object

private:
    // Private Properties
    LPDIRECTSOUND       m_pDSound;              // Pointer to the Direct Sound object
	LPDIRECTSOUNDBUFFER m_pDSoundBuffer;        // Pointer to the local DSound buffer
	XFileMediaObject*   m_pFMOFile;             // Pointer to our XFileMediaObject file
    XMEDIAPACKET        m_xmp;                  // XMedia Packet
    char*               m_pszWaveFileName;      // Wave File Name
    LONG                m_lVolume;              // Default Volume this wav will be played at
    BOOL                m_bInitialized;         // Used to determine if this object was initialized

    // Private Methods
    BOOL ProcessWaveFile();                     // Load the WAV file in to our local DSound Buffer
};

#endif // _WAVEMEDIA_H_