/*****************************************************
*** wavemedia.cpp
***
*** CPP file for a WAV file media object.  This class
*** will allow a user to specify the DirectSound
*** pointer, and a WAV file name, and will allow
*** the user to playback the sound
***
*** by James N. Helm
*** May 25th, 2001
***
*****************************************************/

#include "wavempch.h"
#include "wavemedia.h"

// Constructor
CWaveMedia::CWaveMedia()
: m_pDSound( NULL ),
m_pDSoundBuffer( NULL ),
m_pFMOFile( NULL ),
m_pszWaveFileName( NULL ),
m_lVolume( 0 ),
m_bInitialized( FALSE )
{
    // Zero Out our local object memory
    ZeroMemory( &m_xmp, sizeof( XMEDIAPACKET ) );
}

// Destructor
CWaveMedia::~CWaveMedia()
{
    CleanUp();
}


// Used to cleanup (de-initialize) the object
void CWaveMedia::CleanUp()
{
    XDBGWRN( WAVEM_APP_TITLE_NAME_A, "CWaveMedia::CleanUp()" );

    // Clean up the Wave File name store locally
    if( m_pszWaveFileName )
    {
        delete[] m_pszWaveFileName;
        m_pszWaveFileName = NULL;
    }

    // Clean up our DSound buffer if it was allocated
    if( m_pDSoundBuffer )
    {
        m_pDSoundBuffer->Release();
        m_pDSoundBuffer = NULL;
    }

    // Clean up our WAV Data
    if( m_xmp.pvBuffer )
    {
        delete[] m_xmp.pvBuffer;
        m_xmp.pvBuffer = NULL;
    }
    
    // Release the DSound object
    if( m_pDSound )
    {
        m_pDSound->Release();
        m_pDSound = NULL;
    }

    m_bInitialized = FALSE;
}


// Initialize the Wave Media Object
BOOL CWaveMedia::Init( LPDIRECTSOUND pDSound, char* pszWaveFileName, LONG lDefaultVolume/*=0*/ )
{
    if( m_bInitialized )
    {
        XDBGWRN( WAVEM_APP_TITLE_NAME_A, "CWaveMedia::Init():Already Initialized!!" );

        return FALSE;
    }

    if( !pDSound || !pszWaveFileName )
    {
        XDBGWRN( WAVEM_APP_TITLE_NAME_A, "CWaveMedia::Init():Parameters were invalid!!" );

        return FALSE;
    }

    m_bInitialized = TRUE;

    m_pDSound = pDSound;
    m_pDSound->AddRef();

    int iStrLen = strlen( pszWaveFileName );
    m_pszWaveFileName = new char[iStrLen + 1];
    if( !m_pszWaveFileName )
    {
        XDBGWRN( WAVEM_APP_TITLE_NAME_A, "CWaveMedia::Init():Failed to allocate memory for our WAV File Name!!" );

        return FALSE;
    }
    else
    {
        ZeroMemory( m_pszWaveFileName, iStrLen + 1 );
        strcpy( m_pszWaveFileName, pszWaveFileName );
    }

    m_lVolume = lDefaultVolume;

    BOOL bReturn;
    if( bReturn = ProcessWaveFile() )
    {
        SetVolume( m_lVolume );
    }

    return bReturn;
}


// Load the WAV file in to a local DSound Buffer
BOOL CWaveMedia::ProcessWaveFile()
{
    DSBUFFERDESC    dsbd;
	LPCWAVEFORMATEX pWfx = NULL;

    ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );

    // Create the File Media Object
    if( FAILED( XWaveFileCreateMediaObject( m_pszWaveFileName, &pWfx, &m_pFMOFile ) ) )
    {
        XDBGWRN( WAVEM_APP_TITLE_NAME_A, "CWaveMedia::ProcessWaveFile():XWaveFileCreateMediaObject Failed!!" );

        return FALSE;
    }

    dsbd.dwSize = sizeof( DSBUFFERDESC );
	dsbd.lpwfxFormat = (LPWAVEFORMATEX) pWfx;

    // Create the DSound Buffer
    if( FAILED( m_pDSound->CreateSoundBuffer( &dsbd, &m_pDSoundBuffer, NULL ) ) )
    {
        XDBGWRN( WAVEM_APP_TITLE_NAME_A, "CWaveMedia::ProcessWaveFile():CreateSoundBuffer Failed!!" );

        return FALSE;
    }

    // Get the length of the media file
    if( FAILED( m_pFMOFile->GetLength( &m_xmp.dwMaxSize ) ) )
    {
        XDBGWRN( WAVEM_APP_TITLE_NAME_A, "CWaveMedia::ProcessWaveFile():GetLength Failed!!" );

        return FALSE;
    }

	//
	// Create a buffer of the size of the length
	//
    m_xmp.pvBuffer = new BYTE[ m_xmp.dwMaxSize ];
    if( !m_xmp.pvBuffer )
    {
        XDBGWRN( WAVEM_APP_TITLE_NAME_A, "CWaveMedia::ProcessWaveFile():Failed to allocate memory for our Buffer!!" );

        return FALSE;
    }

	//
	// Precache out the buffer data
	//
    if( FAILED( m_pFMOFile->Process( NULL, &m_xmp ) ) )
    {
        XDBGWRN( WAVEM_APP_TITLE_NAME_A, "CWaveMedia::ProcessWaveFile():Process Failed!!" );

        return FALSE;
    }

    //
    // Set the buffer data
    //
    if( FAILED( m_pDSoundBuffer->SetBufferData( m_xmp.pvBuffer, m_xmp.dwMaxSize ) ) )
    {
        XDBGWRN( WAVEM_APP_TITLE_NAME_A, "CWaveMedia::ProcessWaveFile():SetBufferData Failed!!" );

        return FALSE;
    }


    return TRUE;
}


// Returns TRUE if the sound is currently playing
BOOL CWaveMedia::IsPlaying()
{
    if( FALSE == m_bInitialized )
    {
        XDBGWRN( WAVEM_APP_TITLE_NAME_A, "CWaveMedia::IsPlaying():Cannot play sound until object is initialized!!" );

        return FALSE;
    }

    DWORD dwSoundStatus = 0;

    HRESULT hr = m_pDSoundBuffer->GetStatus( &dwSoundStatus );
    if( FAILED( hr ) )
    {
        XDBGWRN( WAVEM_APP_TITLE_NAME_A, "CWaveMedia::IsPlaying():There was a problem getting the status of the sound!!" );

        return FALSE;
    }

    BOOL bReturn = FALSE;
    // Check to see if the sound is still playing
    if( dwSoundStatus & DSBSTATUS_PLAYING )
    {
        return TRUE;
    }

    return bReturn;
}


// Set the volume of the WAV file
void CWaveMedia::SetVolume( LONG lVolume )
{
    if( FALSE == m_bInitialized )
    {
        XDBGWRN( WAVEM_APP_TITLE_NAME_A, "CWaveMedia::SetVolume():Cannot set the volume until object is initialized!!" );

        return;
    }

    m_lVolume = lVolume;

    HRESULT hr = m_pDSoundBuffer->SetVolume( m_lVolume ); 
    if( FAILED( hr ) )
    {
        XDBGWRN( WAVEM_APP_TITLE_NAME_A, "CWaveMedia::SetVolume():Failed to set the volume for the sound!! Vol - '%d', Error - '0x%.8X (%d)'", m_lVolume, hr, hr );
    }
}


// Play the WAV file
void CWaveMedia::Play()
{
    if( FALSE == m_bInitialized )
    {
        XDBGWRN( WAVEM_APP_TITLE_NAME_A, "CWaveMedia::Play():Cannot play sound until object is initialized!!" );

        return;
    }

    ResetToBeginning();
    m_pDSoundBuffer->Play( 0, 0, 0 );
}


// Stop Playing the WAV file
void CWaveMedia::Stop()
{
    if( FALSE == m_bInitialized )
    {
        XDBGWRN( WAVEM_APP_TITLE_NAME_A, "CWaveMedia::Stop():Cannot stop sound until object is initialized!!" );

        return;
    }

    m_pDSoundBuffer->Stop();
}


// Reset the WAV File Play pointer to the beginning of the wave
void CWaveMedia::ResetToBeginning()
{
    if( FALSE == m_bInitialized )
    {
        XDBGWRN( WAVEM_APP_TITLE_NAME_A, "CWaveMedia::ResetToBeginning():Cannot ResetToBeginning sound until object is initialized!!" );

        return;
    }

    m_pDSoundBuffer->SetCurrentPosition( 0 );
}
