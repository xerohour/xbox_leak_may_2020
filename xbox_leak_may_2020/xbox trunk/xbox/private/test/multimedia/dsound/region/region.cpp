/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	region.cpp

Abstract:

	Tests for SetPlayRegion and SetLoopRegion

Author:

	Robert Heitkamp (robheit) 24-Oct-2001

Environment:

	Xbox only

Revision History:

	24-Oct-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "region.h"
#include "dsutil.h"

//------------------------------------------------------------------------------
//	The Global Logging Handle
//------------------------------------------------------------------------------
extern HANDLE g_hLog;

//------------------------------------------------------------------------------
//	Constants
//------------------------------------------------------------------------------
static const double cPi	= 3.1415926535;

//------------------------------------------------------------------------------
//	Static Functions:
//------------------------------------------------------------------------------
LPDIRECTSOUNDBUFFER BuildBuffer(LPDIRECTSOUND8, DWORD&);

//------------------------------------------------------------------------------
//	region_BasicTest
//------------------------------------------------------------------------------
HRESULT
region_BasicTest(void)
/*++

Routine Description:

    Basic Test for region

Arguments:

    None

Return Value:

    S_OK on success
    E_XX on failure

--*/
{
    HRESULT				hr	= S_OK;
	LPDIRECTSOUND8		pDSound	= NULL;
	LPDIRECTSOUNDBUFFER	pBuffer	= NULL;
	DWORD				size;

	// Create the DSound Object
	ASSERT(SUCCEEDED(DirectSoundCreate(NULL, &pDSound, NULL)));

	// Play a mono buffer with no mixbins set
	pBuffer = BuildBuffer(pDSound, size);
	ASSERT(pBuffer);

	// Play for 2 seconds
	Sleep(2000);
	pBuffer->Play(0, 0, DSBPLAY_FROMSTART | DSBPLAY_LOOPING);

	// Play for 4 seconds
	Sleep(4000);

	// Set the play region to the first 2 sections
	pBuffer->SetPlayRegion(0, size*2);

	// Play for 4 seconds
	Sleep(4000);

	// Set the play region to the last 2 sections
	pBuffer->SetPlayRegion(size*2, size*2);

	// Play for 4 seconds
	Sleep(4000);

	// Set the play region to the middle 2 sections
	pBuffer->SetPlayRegion(size, size*2);

	// Play for 4 seconds
	Sleep(4000);

	// Set the loop region to the 3rd section (2nd in play region)
	pBuffer->SetLoopRegion(size, size);

	// Play for 4 seconds
	Sleep(4000);

	// Set the loop region to the beginning of the region
	pBuffer->SetLoopRegion(0, size);

	// Play for 4 seconds
	Sleep(4000);

	// Release the buffers
	pBuffer->Release();
	pDSound->Release();

    //
    // Return the Hresult
    //
    return hr;
}

//------------------------------------------------------------------------------
//	region_StartTest
//------------------------------------------------------------------------------
VOID WINAPI 
region_StartTest( 
				 IN HANDLE	LogHandle 
				 )
/*++

Routine Description:

    The harness entry into the region tests

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
    // Test region in the mannor it was meant to be called
    //

    SETLOG( LogHandle, "robheit", "MCPX", "region", "Basic" );
    EXECUTE( region_BasicTest() );

}

//------------------------------------------------------------------------------
//	region_EndTest
//------------------------------------------------------------------------------
VOID WINAPI 
region_EndTest(VOID)
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
void 
__cdecl 
main( 
    void 
    )
/*++

Routine Description:

    the exe entry point

Arguments:

    None

Return Value:

    None

--*/
{
    region_StartTest( NULL );
    region_EndTest();
	DSUtilReboot();
}
#endif // NOLOGGING

//------------------------------------------------------------------------------
//	BuildBuffer
//------------------------------------------------------------------------------
LPDIRECTSOUNDBUFFER
BuildBuffer(
			IN LPDIRECTSOUND8	pDSound,
			OUT	DWORD&			size
			)
/*++

Routine Description:

	Builds a mono buffer with 4 distinct sine waves, each 1 second long.
	100Hz, 200Hz, 400Hz and 800Hz

Arguments:

	IN pDSound -	DSound object
	OUT pSize -		Sizes of the regions

Return Value:

	None

--*/
{
	DWORD				i;
	DWORD				index;
	double				dF;
	double				dInc;
    HRESULT				hr			= S_OK;
	short*				pData		= NULL;
	DWORD				dwBytes;
	DSBUFFERDESC		dsbd;
	WAVEFORMATEX		wfx;
	short				value;
	DWORD				c;
	LPDIRECTSOUNDBUFFER	pBuffer;
	double				dataScale;
	
	// It's always safe to zero out stack allocated structs
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	ZeroMemory(&wfx, sizeof(WAVEFORMATEX));

	// Setup the waveformat description
	wfx.cbSize			= 0;
	wfx.wFormatTag		= WAVE_FORMAT_PCM;
	wfx.nChannels		= 1;
	wfx.nSamplesPerSec	= 48000;
	wfx.wBitsPerSample	= 16;
	wfx.nBlockAlign		= 2; 
	wfx.nAvgBytesPerSec	= wfx.nSamplesPerSec * wfx.nBlockAlign;

	// Setup the dsound buffer description with the waveformatex
	dsbd.dwBufferBytes	= wfx.nAvgBytesPerSec * 4;
	dsbd.dwFlags		= 0;
	dsbd.dwSize			= sizeof(DSBUFFERDESC);
	dsbd.lpwfxFormat	= &wfx;
    dsbd.lpMixBins		= NULL;

	// Create the dsound buffer
	hr = pDSound->CreateSoundBuffer(&dsbd, &pBuffer, NULL);
	if(SUCCEEDED(hr))
	{
		// Get a buffer from dsound using lock
		hr = pBuffer->Lock(0, dsbd.dwBufferBytes, (LPVOID*)&pData, &dwBytes, NULL, NULL, 0);
		if(SUCCEEDED(hr))
		{
			size	= wfx.nAvgBytesPerSec / 2;
			dInc	= (2.0 * cPi) * 200.0 / size;
			for(dF=0.0, index=0, i=0;  i<size; ++i, dF+=dInc)
				pData[index++] = (short)(sin(dF) * 32767.0);
			dInc = (2.0 * cPi) * 400.0 / size;
			for(dF=0.0, i=0; i<size; ++i, dF+=dInc)
				pData[index++] = (short)(sin(dF) * 32767.0);
			dInc = (2.0 * cPi) * 800.0 / size;
			for(dF=0.0, i=0; i<size; ++i, dF+=dInc)
				pData[index++] = (short)(sin(dF) * 32767.0);
			dInc = (2.0 * cPi) * 1600.0 / size;
			for(dF=0.0, i=0; i<size; ++i, dF+=dInc)
				pData[index++] = (short)(sin(dF) * 32767.0);
		}
    }
    
	// Return a valid size
	size = wfx.nAvgBytesPerSec;

	// Set the headroom to see true levels
	if(SUCCEEDED(hr))
		hr = pBuffer->SetHeadroom(0);

	// start the dsound buffer playing in a loop mode
	if(SUCCEEDED(hr))
		hr = pBuffer->Play(0, 0, DSBPLAY_LOOPING);

	if(SUCCEEDED(hr))
	    return pBuffer;

	// Error
	pBuffer->Release();
	return NULL;
}

//------------------------------------------------------------------------------
//	Export Function Pointers for StartTest and EndTest
//------------------------------------------------------------------------------
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( region )
#pragma data_seg()

BEGIN_EXPORT_TABLE( region )
    EXPORT_TABLE_ENTRY( "StartTest", region_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", region_EndTest )
END_EXPORT_TABLE( region )
