///////////////////////////////////////////////////////////////////////////////////
//
// Module: sinewave.cpp
//
// Purpose: Code to play a sine wave
//
// Author: Dan Rosenstein( dan@xbox.com)
//
// Date: December 14, 20000
//
///////////////////////////////////////////////////////////////////////////////////

#include <xtl.h>

///
/// Set up necessary types
/// 

typedef short* LPSHORT;

///
/// Setup the frequency, and define PI
///

static const double FREQ = 1000;
static const double PI = 3.1415926535;

///
/// Scaling factors
///
/// ISCALE is used to convert an interger [0, 44099] to a double [0, 2PI]
/// DATASCALE is used to conver a double [-1.0, 1.0] to an integer [-32767, 32767]

static const double ISCALE = 2.0 * PI / 44099.0;
static const double DATASCALE = 32767.0 / 1.0;

///
/// the main sine player code
///

void __cdecl main( void )
{

	///
	/// create a dsound buffer and a data buffer
	///

	LPDIRECTSOUNDBUFFER pDSBuffer = NULL;
	LPSHORT pData = NULL;

	///
	/// place to hold the number of bytes allocated by dsound
	///

	DWORD dwBytes = 0;

	///
	/// allocate space for calculate argument to sin, return value from sine, and actual integer value
	///

	double dArg = 0.0;
	double dSinVal = 0.0;
	SHORT sVal = 0;
   
	///
	/// allocate the dsound buffer description and the wave format
	///

	DSBUFFERDESC dsbd;
	WAVEFORMATEX wfx;

	///
	/// It's always safe to zero out stack allocated structs
	///

	ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );
	ZeroMemory( &wfx, sizeof( WAVEFORMATEX ) );

	///
	/// setup the waveformat description to be PCM, 44100hz, 16bit, mono
    ///

	wfx.cbSize = 0;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.nSamplesPerSec = 44100;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = 2;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	///
	/// setup the dsound buffer description with the waveformatex
	///

	dsbd.dwBufferBytes = wfx.nAvgBytesPerSec;
	dsbd.dwFlags = 0;
	dsbd.dwSize = sizeof( DSBUFFERDESC );
	dsbd.lpwfxFormat = &wfx;

	/// 
	/// Create the dsound buffer
	///

#ifdef SILVER
	HRESULT hr = DirectSoundCreateBuffer( DSDEVID_MCPX, &dsbd, &pDSBuffer, NULL );
#else // SILVER
	HRESULT hr = DirectSoundCreateBuffer( &dsbd, &pDSBuffer );
#endif // SILVER

	///
	/// start the dsound buffer playing in a loop mode
	///

	if ( SUCCEEDED( hr ) )
	{
		hr = pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );
	}

	///
	/// continue forever (or until an error occurs)
	///

	while ( SUCCEEDED( hr ) )
	{

		///
		/// get a buffer from dsound using lock
		///

		hr = pDSBuffer->Lock( 0, dsbd.dwBufferBytes, (LPVOID*) &pData, &dwBytes, NULL, NULL, 0 );

		///
		/// go through the buffer 2 bytes (1 short) at a time
		///

		for( DWORD i = 0; SUCCEEDED( hr ) && i < dsbd.dwBufferBytes / 2; i++ )
		{	
			///
			/// calculate the sin value
			///

			dArg = (double) i * ISCALE;
			dSinVal = sin( FREQ * dArg );
			sVal = (SHORT) ( dSinVal * DATASCALE );

			///
			/// copy the value into the dsound buffer
			///

			pData[i] = sVal;
		}

		///
		/// sleep for 1 second
		///

		Sleep( 1000 );

	}

	///
	/// we never get here, but it's good practice to clean up our dsound buffer
	///

	if ( NULL != pDSBuffer )
	{
		pDSBuffer->Release();
	}

}
