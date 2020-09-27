/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	zerobyte.cpp

Abstract:

	Test to confirm that zero byte buffers have a voice allocated

Author:

	Robert Heitkamp (robheit) 26-Oct-2001

Environment:

	Xox only

Revision History:

	26-Oct-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "zerobyte.h"
#include "dsutil.h"

//------------------------------------------------------------------------------
//	The Global Logging Handle
//------------------------------------------------------------------------------
extern HANDLE g_hLog;

//------------------------------------------------------------------------------
//	zerobyte_BasicTest
//------------------------------------------------------------------------------
HRESULT
zerobyte_BasicTest(void)
/*++

Routine Description:

    Basic Test for zerobyte

Arguments:

    None

Return Value:

    S_OK on success
    E_XX on failure

--*/
{
    HRESULT				hr	= S_OK;
	LPDIRECTSOUNDBUFFER	pBuffer[253];
	LPDIRECTSOUND8		pDSound;
	DSBUFFERDESC		dsbd;
	WAVEFORMATEX		wfx;
	int					i;

	// Create the DSound object
	hr = DirectSoundCreate(NULL, &pDSound, NULL);
	if(FAILED(hr))
		return hr;

	// Setup for buffers
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	ZeroMemory(&wfx, sizeof(WAVEFORMATEX));

	// setup the waveformat description to be PCM, 48000hz, 16bit, mono
	wfx.cbSize			= 0;
	wfx.wFormatTag		= WAVE_FORMAT_PCM;
	wfx.nChannels		= 1;
	wfx.nSamplesPerSec	= 48000;
	wfx.wBitsPerSample	= 16;
	wfx.nBlockAlign		= 2;
	wfx.nAvgBytesPerSec	= wfx.nSamplesPerSec * wfx.nBlockAlign;

	// setup the dsound buffer description with the waveformatex
	dsbd.dwSize			= sizeof(DSBUFFERDESC);
	dsbd.dwBufferBytes	= 0;	// Zero size
	dsbd.dwFlags		= 0;
	dsbd.lpwfxFormat	= &wfx;
	dsbd.lpMixBins		= NULL;

	// NULL the buffers
	for(i=0; i<253; ++i)
		pBuffer[i] = NULL;

	// Create 252 buffers
	for(i=0; SUCCEEDED(hr) && (i<252); ++i)
		hr = pDSound->CreateSoundBuffer(&dsbd, &pBuffer[i], NULL);

	// Try to create one more buffer - this should fail
	if(SUCCEEDED(hr))
	{
		hr = pDSound->CreateSoundBuffer(&dsbd, &pBuffer[i], NULL);
		if(SUCCEEDED(hr))
			hr = E_FAIL;
	}

	// Release
	for(i=0; i<253; ++i)
	{
		if(pBuffer[i])
			pBuffer[i]->Release();
	}
	pDSound->Release();

    return hr;
}

//------------------------------------------------------------------------------
//	zerobyte_StartTest
//------------------------------------------------------------------------------
VOID WINAPI 
zerobyte_StartTest(
				   IN HANDLE	LogHandle 
				   )
/*++

Routine Description:

    The harness entry into the zerobyte tests

Arguments:

    LogHandle - a handle to a logging object

Return Value:

    None

--*/
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
    // Test zerobyte in the mannor it was meant to be called
    //

    SETLOG( LogHandle, "robheit", "MCPX", "zerobyte", "Basic" );
    EXECUTE( zerobyte_BasicTest() );

}

//------------------------------------------------------------------------------
//	zerobyte_EndTest
//------------------------------------------------------------------------------
VOID WINAPI 
zerobyte_EndTest(VOID)
/*++

Routine Description:

    The exit function for the test harness

Arguments:

    None

Return Value:

    None

--*/
{
}

//------------------------------------------------------------------------------
//	main
//------------------------------------------------------------------------------
#ifdef NOLOGGING
void __cdecl 
main(void)
/*++

Routine Description:

    the exe entry point

Arguments:

    None

Return Value:

    None

--*/
{
    zerobyte_StartTest( NULL );
    zerobyte_EndTest();
	DSUtilReboot();
}
#endif // NOLOGGING

//------------------------------------------------------------------------------
//	Export Function Pointers for StartTest and EndTest
//------------------------------------------------------------------------------
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( zerobyte )
#pragma data_seg()

BEGIN_EXPORT_TABLE( zerobyte )
    EXPORT_TABLE_ENTRY( "StartTest", zerobyte_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", zerobyte_EndTest )
END_EXPORT_TABLE( zerobyte )
