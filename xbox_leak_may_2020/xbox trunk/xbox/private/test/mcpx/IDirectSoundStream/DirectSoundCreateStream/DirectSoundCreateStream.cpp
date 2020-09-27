/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       DirectSoundCreateStream.cpp
 *  Content:    DirectSoundCreateStream tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  04/ 9/01    danrose Created to test Xbox DirectSoundCreateStream
 *
 ****************************************************************************/

#include "DirectSoundCreateStream.h"

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;

/****************************************************************************
 *
 *  DirectSoundCreateStream_BasicTest
 *
 *  Description:
 *      Basic Test for DirectSoundCreateStream
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreateStream_BasicTest( void )
{
    HRESULT             hr              = S_OK;
	LPDIRECTSOUNDSTREAM pStream			= NULL;
	DSSTREAMDESC		dssd;
	WAVEFORMATEX		wfx;

	//
	// Zero out the stream desc and the WAVEFORMATEX
	//

	ZeroMemory( &dssd, sizeof( DSSTREAMDESC ) );
	ZeroMemory( &wfx, sizeof( WAVEFORMATEX ) );

	//
	// Fill in the wave format
	//

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.wBitsPerSample = 8;
	wfx.nBlockAlign = 1;
	wfx.nSamplesPerSec = 48000;

	//
	// Fill in the DSSTREAMDESC
	//

	dssd.dwMaxAttachedPackets = 1;   
	dssd.lpwfxFormat = &wfx;     

	//
	// Call DirectSoundCreateStream and check return code and pointer
	//

	CHECKRUN( DirectSoundCreateStream( &dssd, &pStream ) );
	CHECKALLOC( pStream );

	//
	// Release the Stream
	//

	RELEASE( pStream );

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreateStream_NullDSSDTest
 *
 *  Description:
 *      Test DirectSoundCreateStream with a null DSSD
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreateStream_NullDSSDTest( void )
{
    HRESULT             hr              = S_OK;
	LPDIRECTSOUNDSTREAM pStream			= NULL;

	//
	// Call DirectSoundCreateStream and check return code and
	// resultant pointer
	//

	CHECKRUN( DirectSoundCreateStream( NULL, &pStream ) );
	CHECKALLOC( pStream );

	//
	// Destroy the DirectSoundStream
	//

	RELEASE( pStream );

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreateStream_NullStreamTest
 *
 *  Description:
 *      Test DirectSoundCreateStream with a null Stream
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreateStream_NullStreamTest( void )
{
    HRESULT             hr              = S_OK;
	DSSTREAMDESC		dssd;
	WAVEFORMATEX		wfx;

	//
	// Zero out the DSound stream description and the WaveFormatEx
	//

	ZeroMemory( &dssd, sizeof( DSSTREAMDESC ) );
	ZeroMemory( &wfx, sizeof( WAVEFORMATEX ) );

	//
	// Fill in the WaveFormatEx
	//

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.wBitsPerSample = 8;
	wfx.nBlockAlign = 1;

	//
	// Set the Waveformat ofthe DSSTREAMDESC to my wfx
	//

	dssd.lpwfxFormat = &wfx;
	//
	// Call DirectSoundCreateStream and check return code
	//

	CHECKRUN( DirectSoundCreateStream( &dssd, NULL ) );

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreateStream_BasicADPCMTest
 *
 *  Description:
 *      Basic Test for DirectSoundCreateStream init using ADPCM
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreateStream_BasicADPCMTest( void )
{
    HRESULT             hr              = S_OK;
	LPDIRECTSOUNDSTREAM pStream			= NULL;
	DSSTREAMDESC		dssd;
	XBOXADPCMWAVEFORMAT	wfx;

	//
	// Zero out the DSound buffer description and the WaveFormatEx
	//

	ZeroMemory( &dssd, sizeof( DSSTREAMDESC ) );
	ZeroMemory( &wfx, sizeof( XBOXADPCMWAVEFORMAT ) );

	//
	// Fill in the WaveFormatEx
	//

	wfx.wfx.wFormatTag = WAVE_FORMAT_XBOX_ADPCM;
	wfx.wfx.nChannels = 1;
	wfx.wfx.wBitsPerSample = 4;
	wfx.wfx.nBlockAlign = 36;
	wfx.wfx.cbSize = 2;
	wfx.wfx.nSamplesPerSec = 48000;
	wfx.wSamplesPerBlock = 64;

	//
	// Set the Waveformat of the DSSTREAMDESC to my wfx
	//

	dssd.dwMaxAttachedPackets = 1; 
	dssd.lpwfxFormat = (LPWAVEFORMATEX) &wfx;

	//
	// Call DirectSoundCreateStream and check return code and
	// resultant pointer
	//

	CHECKRUN( DirectSoundCreateStream( &dssd, &pStream ) );
	CHECKALLOC( pStream );

	//
	// Destroy the DirectSoundStream
	//

	RELEASE( pStream );

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreateStream_StartTest
 *
 *  Description:
 *      The Harness Entry into the DirectSoundCreateStream tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI DirectSoundCreateStream_StartTest( HANDLE LogHandle )
{
    HRESULT             hr              = S_OK;

    //
    // the following tests will Assert (by design) in debug builds
    // to turn these tests on (they are off by default) define
    // CODE_COVERAGE when this file is compiled
    //

#ifdef CODE_COVERAGE

	//
	// Test DirectSoundCreateStream with a NULL DSSD
	//

    SETLOG( LogHandle, "danrose", "MCPX", "DirectSoundCreateStream", "NullDSSD" );
    EXECUTE( DirectSoundCreateStream_NullDSSDTest() );

	//
	// Test DirectSoundCreateStream with a NULL DirectSoundStream
	//

    SETLOG( LogHandle, "danrose", "MCPX", "DirectSoundCreateStream", "NullStream" );
    EXECUTE( DirectSoundCreateStream_NullStreamTest() );

#endif // CODE_COVERAGE

    //
    // Test DirectSoundCreateStream in the mannor it was meant to be called
    //

    SETLOG( LogHandle, "danrose", "MCPX", "DirectSoundCreateStream", "Basic" );
    EXECUTE( DirectSoundCreateStream_BasicTest() );

	//
	// Test DirectSoundCreateStream using basic ADPCM
	//

    SETLOG( LogHandle, "danrose", "MCPX", "DirectSoundCreateStream", "BasicADPCM" );
    EXECUTE( DirectSoundCreateStream_BasicADPCMTest() );

	//
	// Test DirectSoundCreateStream using the max number of PCM buffers
	//

 //   SETLOG( LogHandle, "danrose", "MCPX", "DirectSoundCreateBuffer", "MaxBuffersPCM" );
 //   EXECUTE( DirectSoundCreateBuffer_MaxBuffersPCMTest() );

}

/****************************************************************************
 *
 *  DirectSoundCreateStream_EndTest
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

VOID WINAPI DirectSoundCreateStream_EndTest( VOID )
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
    DirectSoundCreateStream_StartTest( NULL );
    DirectSoundCreateStream_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( DirectSoundCreateStream )
#pragma data_seg()

BEGIN_EXPORT_TABLE( DirectSoundCreateStream )
    EXPORT_TABLE_ENTRY( "StartTest", DirectSoundCreateStream_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DirectSoundCreateStream_EndTest )
END_EXPORT_TABLE( DirectSoundCreateStream )
