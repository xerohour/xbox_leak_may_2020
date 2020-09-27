///////////////////////////////////////////////////////////////////////////////
//
// Name: wavemedia.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation of the wavemedia class
//
///////////////////////////////////////////////////////////////////////////////

#include "wavemedia.h"

///////////////////////////////////////////////////////////////////////////////
//
// Name: CWaveMedia
//
// Input: None
//
// Output: an instance of the wave media class
//
// Description: constructs a wave media object
//
///////////////////////////////////////////////////////////////////////////////

CWaveMedia::CWaveMedia( void )
	: m_pDSoundBuffer( NULL ),
	  m_pFMOFile( NULL )
{
	//
    // Zero Out our local object memory
	//

    ZeroMemory( &m_xmp, sizeof( XMEDIAPACKET ) );
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: ~CWaveMedia
//
// Input: None
//
// Output: None
//
// Description: destorys the wave file
//
///////////////////////////////////////////////////////////////////////////////

CWaveMedia::~CWaveMedia( void )
{

	//
    // Clean up our DSound buffer if it was allocated
	//

    if( m_pDSoundBuffer )
    {
        m_pDSoundBuffer->Release();
        m_pDSoundBuffer = NULL;
    }

	//
    // Clean up our WAV Data
	//

    if( m_xmp.pvBuffer )
    {
        delete[] m_xmp.pvBuffer;
        m_xmp.pvBuffer = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Initialize
//
// Input: szWaveFileName - the name of the file to open
//        lDefaultVolume - the volume for the sound
//
// Output: None
//
// Description: destorys the wave file
//
///////////////////////////////////////////////////////////////////////////////

HRESULT 
CWaveMedia::Initialize( LPSTR szWaveFileName, LONG lDefaultVolume )
{
	HRESULT hr = S_OK;

	//
	// make sure input parameters are ok
	//

	ASSERT( szWaveFileName || !"Need to pass in wave file name" );

    if( ! szWaveFileName )
    {
		hr = E_POINTER;
    }

	//
	// Process the wave file
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = ProcessWaveFile( szWaveFileName );
	//	ASSERT( SUCCEEDED( hr ) || !"could not process wave file" );
	}

	//
	// set up the volume
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = SetVolume( lDefaultVolume );
		ASSERT( SUCCEEDED( hr ) || ! "could not set volume" );
	}

	//
	// spit back the hr
	//

    return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: ProcessWaveFile
//
// Input: szWaveFileName - the name of the wave file to process
//
// Output: S_OK if processing was ok, E_XXXX otherwise
//
// Description: opens the wave file, puts it in memory
//
///////////////////////////////////////////////////////////////////////////////

HRESULT 
CWaveMedia::ProcessWaveFile( LPSTR szWaveFileName )
{
	HRESULT hr = S_OK;

	LPDIRECTSOUND pDSound = NULL;
	LPCWAVEFORMATEX pWfx = NULL;

    DSBUFFERDESC dsbd;

	//
	// check to make sure sound buffer does not exists
	//

	ASSERT( !m_pDSoundBuffer || !"Dsound buffer allocated" );

	if ( m_pDSoundBuffer )
	{
		hr = E_POINTER;
	}

	//
	// Zero out hte buffer description
	//

    ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );

	//
	// init dsound
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = DirectSoundCreate( NULL, &pDSound, NULL );
		ASSERT( SUCCEEDED( hr ) || ! "Could not init dsound" );
	}

	//
    // Create the File Media Object
	//

    if( SUCCEEDED( hr ) )
	{
		hr = XWaveFileCreateMediaObject( szWaveFileName, &pWfx, &m_pFMOFile );
	//	ASSERT( SUCCEEDED( hr ) || ! "Could not create File XMO" );
    }

	//
	// create the sound buffer
	//

	if ( SUCCEEDED( hr ) )
	{
		//
		// fill in the sound buffer
		//

		dsbd.dwSize = sizeof( DSBUFFERDESC );
		dsbd.lpwfxFormat = (LPWAVEFORMATEX) pWfx;

		//
		// Create the DSound Buffer
		//

		hr = pDSound->CreateSoundBuffer( &dsbd, &m_pDSoundBuffer, NULL );
		ASSERT( SUCCEEDED( hr ) || !"Could not create sound buffer" );
    }

	//
    // Get the length of the media file
	//
    if( SUCCEEDED( hr ) )
	{
		hr = m_pFMOFile->GetLength( &m_xmp.dwMaxSize );
		ASSERT( SUCCEEDED( hr ) || !"Could not get length of file" );
    }

	//
	// Create a buffer of the size of the length
	//

	if ( SUCCEEDED( hr ) )
	{
		m_xmp.pvBuffer = new BYTE[ m_xmp.dwMaxSize ];
		hr = m_xmp.pvBuffer ? S_OK : E_OUTOFMEMORY;

		ASSERT( SUCCEEDED( hr ) || !"could not allocate memory for buffer" );
	}

	//
	// Precache out the buffer data
	//

    if ( SUCCEEDED( hr ) )
	{
		hr = m_pFMOFile->Process( NULL, &m_xmp );
		ASSERT( SUCCEEDED( hr ) || !"Could not precache audio data" );
	}

    //
    // Set the buffer data
    //
    
	if ( SUCCEEDED( hr ) )
	{
		hr = m_pDSoundBuffer->SetBufferData( m_xmp.pvBuffer, m_xmp.dwMaxSize );
		ASSERT( SUCCEEDED( hr ) || !"Could not set buffer data" );
	}

	//
	// spit back the hr
	//

    return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: IsPlaying
//
// Input: None
//
// Output: None
//
// Description: destorys the wave file
//
///////////////////////////////////////////////////////////////////////////////

BOOL 
CWaveMedia::IsPlaying( void )
{
	HRESULT hr = S_OK;
    DWORD dwSoundStatus = 0;
  
	//
	// check to make sure sound buffer exists
	//

	if ( ! m_pDSoundBuffer )
	{
		return FALSE;
	}

	//
	// get the status of dsound
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = m_pDSoundBuffer->GetStatus( &dwSoundStatus );
		ASSERT( SUCCEEDED( hr ) || !"Could not get status" );
	}

	//
	// return if the sound is still playing or not
	//

	return SUCCEEDED( hr ) && dwSoundStatus & DSBSTATUS_PLAYING;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: SetVolume
//
// Input: lVolume - the new volume for the wave file
//
// Output: S_OK if setting volume succeeded, E_XXX otherwise
//
// Description: Set the volume of the WAV file
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CWaveMedia::SetVolume( LONG lVolume )
{
	HRESULT hr = S_OK;

	//
	// check to make sure sound buffer exists
	//

	ASSERT( m_pDSoundBuffer || !"Dsound buffer not allocated" );

	if ( ! m_pDSoundBuffer )
	{
		hr = E_POINTER;
	}

	//
	// set the volume
	//

    if ( SUCCEEDED( hr ) )
	{
		hr = m_pDSoundBuffer->SetVolume( lVolume ); 
		ASSERT( SUCCEEDED( hr ) || !"could not set volume" );
	}

	//
	// spit back the hr
	//

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Play
//
// Input: None
//
// Output: S_OK if playing succeeded, E_XXX otherwise
//
// Description: Play the wave file
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CWaveMedia::Play( void )
{
	HRESULT hr = S_OK;

	//
	// check to make sure sound buffer exists
	//

	if ( ! m_pDSoundBuffer )
	{
		return S_OK;
	}

	//
	// reset the wave
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = ResetToBeginning();
		ASSERT( SUCCEEDED( hr ) || !"Could not reset wave" );
	}

	//
	// play the buffer
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = m_pDSoundBuffer->Play( 0, 0, 0 );
		ASSERT( SUCCEEDED( hr ) || !"Could not play buffer" );
	}

	//
	// spit back the hr
	//

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Stop
//
// Input: None
//
// Output: S_OK if playing succeeded, E_XXX otherwise
//
// Description: Stop the wave file
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
 CWaveMedia::Stop( void )
{
	HRESULT hr = S_OK;

	//
	// check to make sure sound buffer exists
	//

	ASSERT( m_pDSoundBuffer || !"Dsound buffer not allocated" );

	if ( ! m_pDSoundBuffer )
	{
		hr = E_POINTER;
	}

	//
	// stop the buffer
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = m_pDSoundBuffer->Stop();
		ASSERT( SUCCEEDED( hr ) || !"Could not stop buffer" );
	}

	//
	// spit back hr
	//

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: ResetToBeginning
//
// Input: None
//
// Output: S_OK if playing succeeded, E_XXX otherwise
//
// Description: resets the wave file to the beginning
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CWaveMedia::ResetToBeginning( void )
{
	HRESULT hr = S_OK;

	//
	// check to make sure sound buffer exists
	//

	ASSERT( m_pDSoundBuffer || !"Dsound buffer not allocated" );

	if ( ! m_pDSoundBuffer )
	{
		hr = E_POINTER;
	}

	//
	// reset the current position
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = m_pDSoundBuffer->SetCurrentPosition( 0 );
		ASSERT( SUCCEEDED( hr ) || !"Could not set current position" );
	}

	//
	// spit back the hr
	//

	return hr;
}
