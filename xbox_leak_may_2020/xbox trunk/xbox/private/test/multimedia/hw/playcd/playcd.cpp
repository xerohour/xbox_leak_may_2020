/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       playcd.cpp
 *  Content:    playcd tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  04/12/01    danrose Created to test Xbox playcd
 *
 ****************************************************************************/

#include "playcd.h"

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;

/****************************************************************************
 *
 *  playcd_BasicTest
 *
 *  Description:
 *      Basic Test for playcd
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT playcd_BasicTest( void )
{
	//
	// the hr we will return upon completion
	//

    HRESULT					hr					= S_OK;

	//
	// constants for the triple buffering we do when reading from cd
	//

	static const DWORD		NUM_SLOTS			= 45;
	static const DWORD		NUM_BUFFERS			= 2;
	static const DWORD		BYTES_PER_BUFFER	= BYTES_PER_CHUNK * NUM_SLOTS / NUM_BUFFERS;

	//
	// the minutes, seconds and frames on the CD
	//

	int						iMin				= 0;
	int						iSec				= 0;
	int						iFrames				= 0;

	//
	// variables to hold CD / DSOUND state
	//

	DWORD					dwCurTrackFrame		= 0;
	DWORD					dwPlayCursor		= 0;
	DWORD					dwLastCursor		= 0;
	DWORD					dwCurBuffer			= 1;
	DWORD					dwCurSlot			= 0;
	DWORD					dwTotalFrames		= 0;

	//
	// the the audio data and the DSOUND buffer
	//

	LPBYTE					pData				= NULL;
	LPDIRECTSOUNDBUFFER		pBuffer				= NULL;

	//
	// the waveformat and the dsound buffer desc
	//

	WAVEFORMATEX			wfx                 = { WAVE_FORMAT_PCM, 2, 44100, 176400, 4, 16, 0 };
	DSBUFFERDESC			dsbd				= { sizeof( DSBUFFERDESC ), 0, 0, &wfx, 0, 0 };

	//
	// the class that runs the cdrom drive and the cd streamer
	//

	CNtIoctlCdromService	cdrom;
	CCDDAStreamer			streamer( &cdrom );

	//
	// Only if capture is defined do we save the wave data to a file
	//

#ifdef CAPTURE

	XMediaObject *			pXMO				= NULL;
	XMEDIAPACKET			xmp;

	ZeroMemory( &xmp, sizeof( XMEDIAPACKET ) );

	if ( SUCCEEDED( hr ) )
	{	
		hr = XWaveOutCreateMediaObject( "t:\\cddata.wav", &wfx, &pXMO );
	}

#endif

	//
	// allocate the byte buffer when the audio data will be streamed from disc to dsound
	//

	if ( SUCCEEDED( hr ) )
	{
		pData = new BYTE[ BYTES_PER_CHUNK * NUM_SLOTS ];

		if ( NULL == pData )
		{
			hr = E_OUTOFMEMORY;
		}
	}

	//
	// open the cdrom drive
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = cdrom.Open( 1 );
	}

	//
	// calculate the number of frames on the cd. This is used later to decide if the disc is done
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = cdrom.GetTotalLength( &iMin, &iSec, &iFrames );
		dwTotalFrames = ( iMin * 60 + iSec ) * 75 + iFrames;
	}

	//
	// set the streamer to start reading at the first frame on the disc, and precache the first bit of audio data
	//

	if ( SUCCEEDED( hr ) )
	{
		dwCurTrackFrame = cdrom.GetTrackFrame( 0 );
		streamer.SetFrame( dwCurTrackFrame );

		hr = streamer.Read( pData, BYTES_PER_CHUNK * NUM_SLOTS );
	}

	//
	// dump out the first cached data
	//

#ifdef CAPTURE

	xmp.pvBuffer = pData;
	xmp.dwMaxSize = BYTES_PER_CHUNK * NUM_SLOTS;

	if ( SUCCEEDED( hr ) )
	{
		hr = pXMO->Process( &xmp, NULL );
	}

#endif // CAPTURE

	//
	// create the dsound buffer
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = DirectSoundCreateBuffer( &dsbd, &pBuffer );
	}

	//
	// set the dsound buffer to use the audio data buffer
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = pBuffer->SetBufferData( pData, BYTES_PER_CHUNK * NUM_SLOTS );
	}

	//
	// start playing the data in a loop
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = pBuffer->Play( 0, 0, DSBPLAY_LOOPING );
	}

	//
	// read from the disc until the frame read is past the end of disc
	//

	while ( SUCCEEDED( hr ) && streamer.GetFrame() < dwTotalFrames )
	{
		//
		// cache the old cursor pos, get the new one from dsound
		//

		if ( SUCCEEDED( hr ) )
		{
			dwLastCursor = dwPlayCursor;
			hr = pBuffer->GetCurrentPosition( &dwPlayCursor, NULL );
		}

		//
		// if we have moved from one buffer to the next, fill in the used buffer from disc
		//

		if ( SUCCEEDED( hr ) && ( dwPlayCursor >= BYTES_PER_BUFFER * dwCurBuffer || dwPlayCursor < dwLastCursor ) )
		{
			hr = streamer.Read( pData + BYTES_PER_BUFFER * ( dwCurBuffer - 1 ), BYTES_PER_BUFFER );

			//
			// output the data to the file
			//

#ifdef CAPTURE

			if ( SUCCEEDED( hr ) )
			{
				xmp.pvBuffer = pData + BYTES_PER_BUFFER * ( dwCurBuffer - 1 );
				xmp.dwMaxSize = BYTES_PER_BUFFER;

				hr = pXMO->Process( &xmp, NULL );
			}

#endif // CAPTURE

			if ( ++dwCurBuffer > NUM_BUFFERS )
				dwCurBuffer = 1;
		}
	}

	//
	// clean up the buffer
	//

	if ( pBuffer )
	{
		pBuffer->Release();
	}

	//
	// kill the wave writer
	//

#ifdef CAPTURE

	if ( pXMO )
	{
		pXMO->Release();
	}

#endif // CAPTURE

	//
	// kill the audio buffer
	//

	delete [] pData;

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  playcd_StartTest
 *
 *  Description:
 *      The Harness Entry into the playcd tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI playcd_StartTest( HANDLE LogHandle )
{
    HRESULT             hr              = S_OK;

    //
    // the following tests will Assert (by design) in debug builds
    // to turn these tests on (they are off by default) define
    // CODE_COVERAGE when this file is compiled
    //

#ifdef CODE_COVERAGE

#endif // CODE_COVERAGE

    //
    // Test playcd in the mannor it was meant to be called
    //

    SETLOG( LogHandle, "danrose", "MCPX", "playcd", "Basic" );
    RUN( playcd_BasicTest() );

}

/****************************************************************************
 *
 *  playcd_EndTest
 *
 *  Description:
 *      The Exit function for the test harness
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI playcd_EndTest( VOID )
{
}

/****************************************************************************
 *
 *  main
 *
 *  Description:
 *      the exe entry point
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#ifdef NOLOGGING
void __cdecl main( void )
{
	while ( TRUE )
	{
		playcd_StartTest( NULL );
		playcd_EndTest();
	}
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( playcd )
#pragma data_seg()

BEGIN_EXPORT_TABLE( playcd )
    EXPORT_TABLE_ENTRY( "StartTest", playcd_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", playcd_EndTest )
END_EXPORT_TABLE( playcd )
