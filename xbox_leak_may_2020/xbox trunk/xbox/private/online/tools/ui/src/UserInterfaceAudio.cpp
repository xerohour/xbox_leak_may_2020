///////////////////////////////////////////////////////////////////////////////
//
// Name: UserInterfaceAuido.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation for the audio ui class
//
///////////////////////////////////////////////////////////////////////////////

#include "UserInterfaceAudio.h"

///////////////////////////////////////////////////////////////////////////////
//
// Name: CUserInterfaceAudio
//
// Input: None
//
// Output: an instance of the user interface audio class
//
// Description: constructs the audio for the UI
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceAudio::CUserInterfaceAudio( void )
	: m_pDSound( NULL )
{
	//
	// No code here
	//
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: ~CUserInterfaceAudio
//
// Input: None
//
// Output: None
//
// Description: destructs the audio ui
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceAudio::~CUserInterfaceAudio( void )
{
	//
	// shut down audio
	//

	if ( m_pDSound )
	{
		m_pDSound->Release();
		m_pDSound = NULL;
	}

}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Initialize
//
// Input: None
//
// Output: S_OK if input creation was successful, E_XXXX otherwise
//
// Description: sets up the app for processing sound
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CUserInterfaceAudio::Initialize( void )
{
	HRESULT hr = S_OK;

	//
	// load up wave files
	//

	for ( DWORD i = 0; i < NUM_AUDIO_FILES && SUCCEEDED( hr ); i++ )
	{
		//
		// load the wave file
		//

		hr = m_sounds[i].Initialize( AUDIO_FILES[i] );
	//	ASSERT( SUCCEEDED( hr ) || ! "Could not load audio file" );
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Play
//
// Input: dwSound - the id of the sound to play
//
// Output: S_OK if audio update was successful, E_XXXX otherwise
//
// Description: starts a sound playing
//
///////////////////////////////////////////////////////////////////////////////

HRESULT 
CUserInterfaceAudio::Play( DWORD dwSound )
{	
	HRESULT hr = S_OK;

	//
	// make sure sound value is in valid range
	//

	ASSERT( dwSound < NUM_AUDIO_FILES || ! "sound index is out of range" );

	if ( dwSound >= NUM_AUDIO_FILES )
	{
		hr = E_FAIL;
	}

	//
	// only play the sound if it is not playing
	//

	if ( SUCCEEDED( hr ) && ! m_sounds[dwSound].IsPlaying() )
	{
		hr = m_sounds[dwSound].Play();
		ASSERT( SUCCEEDED( hr ) || !"Could not play file" );
	}

	return hr;
}