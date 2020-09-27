/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	mono.cpp

Abstract:

	Simple mono buffer test to verify output to Left & Right

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
#include "mono.h"
#include "dsutil.h"

//------------------------------------------------------------------------------
//	The Global Logging Handle
//------------------------------------------------------------------------------
extern HANDLE g_hLog;

//------------------------------------------------------------------------------
//	mono_BasicTest
//------------------------------------------------------------------------------
HRESULT
mono_BasicTest(void)
/*++

Routine Description:

    Basic Test for mono

Arguments:

    None

Return Value:

    S_OK on success
    E_XX on failure

--*/
{
	HRESULT				hr		= S_OK;
	LPDIRECTSOUND8		pDSound	= NULL;
	LPDIRECTSOUNDBUFFER	pBuffer	= NULL;

	// Create the DSound Object
	ASSERT(SUCCEEDED(DirectSoundCreate(NULL, &pDSound, NULL)));

	// Play a mono buffer with no mixbins set
	pBuffer = DSUtilMakeSineWave(pDSound, NULL, 220.0f, 1, 48000, 16);
	ASSERT(pBuffer);

	// start the dsound buffer playing in a loop mode
	hr = pBuffer->Play(0, 0, DSBPLAY_LOOPING);

	// Wait 3 seconds
	Sleep(3000);

	// Release the buffers
	pBuffer->Release();
	pDSound->Release();

    return hr;
}

//------------------------------------------------------------------------------
//	mono_StartTest
//------------------------------------------------------------------------------
VOID 
WINAPI 
mono_StartTest( 
			   IN HANDLE	LogHandle 
			   )
/*++

Routine Description:

    The harness entry into the mono tests

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
    // Test mono in the mannor it was meant to be called
    //

    SETLOG( LogHandle, "robheit", "MCPX", "mono", "Basic" );
    EXECUTE( mono_BasicTest() );

}

//------------------------------------------------------------------------------
//	mono_EndTest
//------------------------------------------------------------------------------
VOID 
WINAPI 
mono_EndTest(void)
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
    mono_StartTest( NULL );
    mono_EndTest();
	DSUtilReboot();
}
#endif // NOLOGGING

//------------------------------------------------------------------------------
//	Export Function Pointers for StartTest and EndTest
//------------------------------------------------------------------------------
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( mono )
#pragma data_seg()

BEGIN_EXPORT_TABLE( mono )
    EXPORT_TABLE_ENTRY( "StartTest", mono_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", mono_EndTest )
END_EXPORT_TABLE( mono )
