/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       DirectSoundCreate.cpp
 *  Content:    DirectSoundCreate tests
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  04/10/01    danrose Created to test Xbox DirectSoundCreate
 *
 ****************************************************************************/

#include "DirectSoundCreate.h"

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;

/****************************************************************************
 *
 *  DirectSoundCreate
 *
 *  Description:
 *      Create and return a DirectSound Interface pointer
 *
 *  Arguments:
 *      ppDirectSound - a double pointer to the DirectSound Interface
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreate( LPDIRECTSOUND* ppDirectSound )
{
	HRESULT				hr				= S_OK;

	//
	// check the inputted param
	//

	CHECKPTR( ppDirectSound );

	//
	// Call DirectSoundCreate
	//

	CHECKRUN( DirectSoundCreate( NULL, ppDirectSound, NULL ) );

	return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreate_BasicTest
 *
 *  Description:
 *      Create and Destroy an IDirectSound using DirectSoundCreate
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreate_BasicTest( void )
{
    HRESULT             hr              = S_OK;
    LPDIRECTSOUND       pDSound         = NULL;

	//
	// Call DirectSoundCreate, check the return code
	// And the returned pointer
	//

    CHECKRUN( DirectSoundCreate( NULL, &pDSound, NULL ) );
    CHECKALLOC( pDSound );

	//
	// Release the IDirectSound pointer
	// 

    RELEASE( pDSound );

	//
	// Return the Hresult
	//

    return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreate_MultipleTest
 *
 *  Description:
 *      Create and Destroy an IDirectSound using DirectSoundCreate
 *      multiple times and verify that the interface pointer values
 *      returned are Identical
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreate_MultipleTest( void )
{
    HRESULT             hr              = S_OK;
    DWORD               i               = 0;
    LPDIRECTSOUND       apDSound[2];

	//
	// Zero out the Array of IDirectSound pointers
	//

    ZeroMemory( &apDSound, sizeof( apDSound ) );

	//
	// Cycle through all the DSound pointers and Call DirectSoundCreate
	// on them and verify the return code and Allocation
	//

    for ( i = 0; i < 2; i++ )
    {
        CHECKRUN( DirectSoundCreate( NULL, &apDSound[i], NULL ) );
        CHECKALLOC( apDSound[i] );
    }

	//
	// Verify that the interface pointers are the same
	//

    FAIL_ON_CONDITION( apDSound[0] != apDSound[1] );

	//
	// cycle through the pointers and release them
	//

    for ( i = 0; i < 2; i++ )
    {
        RELEASE( apDSound[i] );
    }

	// return the HResult 

    return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreate_OutOfMemoryTest
 *
 *  Description:
 *		Test DirectSoundCreate in Low memory situation
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreate_OutOfMemoryTest( void )
{
    HRESULT             hr              = S_OK;
    LPDIRECTSOUND       pDSound         = NULL;
    LPBYTE              pData           = NULL;
	DWORD				i				= 0;
	DWORD				dwPageSize      = 0x1000;
    MEMORYSTATUS        memStatus;

	//
	// Zero out the Memory Status Structure
	//

    ZeroMemory( &memStatus, sizeof( MEMORYSTATUS ) );

	//
	// Get the Memory available on the Xbox
	//

    GlobalMemoryStatus( &memStatus );

	//
	// Keep trying to allocate available memory. If the allocation
	// fails, subtract 4K bytes (1 page) of memory and try again
	//

	do {

		pData = new BYTE[ memStatus.dwAvailPhys - dwPageSize * i++ ];
	
	} while ( NULL == pData );

	//
	// At this point No memory is left for Dsound. This call
	// should fail with hr = E_OUTOFMEMORY
	//

    CHECKRUN( DirectSoundCreate( NULL, &pDSound, NULL ) );

	//
	// If the large allocation was successful, the Dsound pointer
	// was null, and The call to DirectSoundCreate returned
	// the correct error code, then set hr to S_OK
	//

	if ( E_OUTOFMEMORY == hr && NULL == pDSound && NULL != pData )
	{
		hr = S_OK;
	} else {
		hr = E_FAIL;
	}

	//
	// Clean up the allocated memory and release DSound
	//

    delete [] pData;
    RELEASE( pDSound );

	//
	// return the HResult
	//

    return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreate_GuidNullTest
 *
 *  Description:
 *		Call DirectSoundCreate with a guid pointer that is valid but whose
 *		underlying GUID is GUID_NULL
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreate_GuidNullTest( void )
{
    HRESULT             hr              = S_OK;
    LPDIRECTSOUND       pDSound         = NULL;
	GUID				guidId;

	//
	// Zero out the guid ID to make it GUID_NULL
	//

	ZeroMemory( &guidId, sizeof( GUID ) );

	//
	// Call DirectSoundCreate with the LPGUID, check the return hr
	// and check to make sure the DirectSound Pointer was allocated
	//

    CHECKRUN( DirectSoundCreate( &guidId, &pDSound, NULL ) );
    CHECKALLOC( pDSound );

	//
	// release Dsound
	//

    RELEASE( pDSound );

	//
	// Return the HResult
	//

    return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreate_NotGuidNullTest
 *
 *  Description:
 *		Call DirectSoundCreate with a GUID that is not GUID_NULL
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreate_NotGuidNullTest( void )
{
    HRESULT             hr              = S_OK;
    LPDIRECTSOUND       pDSound         = NULL;
	GUID				guidId			= { 0x00000000, 
											0x0000, 
											0x0000, 
											{ 0x00, 
											  0x0, 
											  0x0, 
											  0x00, 
											  0x0, 
											  0x00, 
											  0x00, 
											  0x01 } };

	//
	// Call DirectSoundCreate with guidId, whose value is not GUID_NULL
	// check the return code and make sure the dsound  interface was allocated

    CHECKRUN( DirectSoundCreate( &guidId, &pDSound, NULL ) );
    CHECKALLOC( pDSound );

	//
	// Release DSound
	//

    RELEASE( pDSound );

	//
	// Return the HResult
	//

    return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreate_NonNullPunkTest
 *
 *  Description:
 *      Create and Destroy an IDirectSound using DirectSoundCreate and
 *      a non null punk
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreate_NonNullPunkTest( void )
{
    HRESULT             hr              = S_OK;
    LPDIRECTSOUND       pDSound         = NULL;
	LPUNKNOWN			pUnk			= (LPUNKNOWN) 0xDeadBeef;

	//
	// Call DirectSoundCreate, check the return code
	// And the returned pointer
	//

    CHECKRUN( DirectSoundCreate( NULL, &pDSound, pUnk ) );
    CHECKALLOC( pDSound );

	//
	// Release the IDirectSound pointer
	// 

    RELEASE( pDSound );

	//
	// Return the Hresult
	//

    return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreate_NullDirectSoundPointerTest
 *
 *  Description:
 *      Call DirectSoundCreate with a Null DirectSound Pointer
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreate_NullDirectSoundPointerTest( void )
{
    HRESULT             hr              = S_OK;

	//
	// Call DirectSoundCreate, check the return code
	//

    CHECKRUN( DirectSoundCreate( NULL, NULL, NULL ) );

	//
	// Return the Hresult
	//

    return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreate_ThreadFunction
 *
 *  Description:
 *      thread function for multiple thread test
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *		0 on failure
 *      a pointer cast to a DWORD representing the Dsound Interface
 *
 ****************************************************************************/

DWORD WINAPI DirectSoundCreate_ThreadFunction( LPVOID lpParameter )
{
    HRESULT             hr              = S_OK;
    LPDIRECTSOUND       pDSound         = NULL;
	DWORD				dwRet			= 0;

	//
	// Call DirectSoundCreate, check the return code
	//

    CHECKRUN( DirectSoundCreate( NULL, &pDSound, NULL ) );

	//
	// Cast the pointer value
	//

	dwRet = (DWORD) pDSound;

	//
	// Release the IDirectSound pointer
	// 

    RELEASE( pDSound );

	//
	// return the pDSound pointer value
	//

	return dwRet;
}

/****************************************************************************
 *
 *  DirectSoundCreate_MultipleThreadsTest
 *
 *  Description:
 *      Call DirectSoundCreate on multiple threads
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreate_MultipleThreadsTest( void )
{
    HRESULT             hr              = S_OK;
	DWORD				i				= 0;
	DWORD				dwWait			= 0;
	DWORD				adwRetVals[2];
	HANDLE				ahThreads[2];
    LPDIRECTSOUND       pDSound         = NULL;

	//
	// Zero out the thread array and the ret vals
	//

	ZeroMemory( &ahThreads, sizeof( ahThreads ) );
	ZeroMemory( &adwRetVals, sizeof( adwRetVals ) );

	//
	// Call DirectSoundCreate, check the return code
	//

    CHECKRUN( DirectSoundCreate( NULL, &pDSound, NULL ) );
	CHECKALLOC( pDSound );

	//
	// cycle through the thread array, and create new threads
	// error out as appropriate
	//

	for ( i = 0; i < 2 && SUCCEEDED( hr ); i++ )
	{
		ahThreads[i] = CreateThread( NULL, 0, &DirectSoundCreate_ThreadFunction, NULL, 0, NULL );

		if ( NULL == ahThreads[i] )
			hr = E_OUTOFMEMORY;
	}

	//
	// Wait on the threads to finish, check return code
	//

	if ( SUCCEEDED( hr ) )
	{
		dwWait = WaitForMultipleObjects( 2, ahThreads, TRUE, INFINITE );

		if ( WAIT_FAILED == dwWait )
			hr = E_OUTOFMEMORY;
	}

	//
	// Get the return values from the threads
	//

	for ( i = 0; i < 2 && SUCCEEDED( hr ); i++ )
	{
		if ( ! GetExitCodeThread( ahThreads[i], adwRetVals + i ) )
			hr = E_FAIL;
	}

	//
	// Check the 2 values and make sure they are equal and no zero
	//

	FAIL_ON_CONDITION( adwRetVals[0] == 0 );
	FAIL_ON_CONDITION( adwRetVals[1] == 0 );
    FAIL_ON_CONDITION( adwRetVals[0] != adwRetVals[1] );

	//
	// close each thread handle
	//

	for ( i = 0; i < 2; i++ )
	{
		if ( NULL != ahThreads[i] )
			CloseHandle( ahThreads[i] );
	}

	//
	// Release the Dsound object
	//

	RELEASE( pDSound );

	//
	// Return the Hresult
	//

    return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreate_StartTest
 *
 *  Description:
 *		The Harness Entry into the DirectSoundCreate tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI DirectSoundCreate_StartTest( HANDLE LogHandle )
{
	HRESULT hr = S_OK;

	//
	// the following tests will Assert (by design) in debug builds
	// to turn these tests on (they are off by default) define
	// CODE_COVERAGE when this file is compiled
	//

#ifdef CODE_COVERAGE

	//
	// Test DirectSoundCreate in out of memory situation. this test MUST
	// Be run first based on the mechanism that DirectSound uses to allocate
	// itself on the xbox.
	//

    SETLOG( LogHandle, "danrose", "MCPX", "DirectSoundCreate", "OutOfMemory" );
    EXECUTE( DirectSoundCreate_OutOfMemoryTest() );

	//
	// Test DirectSoundCreate with GUID_NULL
	//

    SETLOG( LogHandle, "danrose", "MCPX", "DirectSoundCreate", "GuidNull" );
    EXECUTE( DirectSoundCreate_GuidNullTest() );

	//
	// Test DirectSoundCreate with a GUID that is not GUID_NULL
	//

    SETLOG( LogHandle, "danrose", "MCPX", "DirectSoundCreate", "NotGuidNull" );
    EXECUTE( DirectSoundCreate_NotGuidNullTest() );

	//
	// Test DirectSoundCreate with a non null LPUNKNOWN
	// 

	SETLOG( LogHandle, "danrose", "MCPX", "DirectSoundCreate", "NonNullPunk" );
	EXECUTE( DirectSoundCreate_NonNullPunkTest() );

	//
	// Test DirectSoundCreate with a null Direct Sound Pointer
	//

	SETLOG( LogHandle, "danrose", "MCPX", "DirectSoundCreate", "NullDirectSoundPointer" );
	EXECUTE( DirectSoundCreate_NullDirectSoundPointerTest() );

#endif // CODE_COVERAGE

	//
	// Test DirectSoundCreate in the mannor it was meant to be called
	//

	SETLOG( LogHandle, "danrose", "MCPX", "DirectSoundCreate", "Basic" );
	EXECUTE( DirectSoundCreate_BasicTest() );

	//
	// Test DirectSoundCreate to make sure that multiple calls result
	// in the same returned IDirectSound pointer
	//

    SETLOG( LogHandle, "danrose", "MCPX", "DirectSoundCreate", "Multiple" );
    EXECUTE( DirectSoundCreate_MultipleTest() );

	//
	// Test DirectSoundCreate to make sure that multiple calls on
	// different threads result in the same returned IDirectSound pointer
	//

    SETLOG( LogHandle, "danrose", "MCPX", "DirectSoundCreate", "MultipleThreads" );
    EXECUTE( DirectSoundCreate_MultipleThreadsTest() );
}

/****************************************************************************
 *
 *  DirectSoundCreate_EndTest
 *
 *  Description:
 *		The Exit function for the test harness
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI DirectSoundCreate_EndTest( VOID )
{
}

/****************************************************************************
 *
 *  main
 *
 *  Description:
 *		the exe entry point
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

#ifdef NOLOGGING
void __cdecl main( void )
{
	DirectSoundCreate_StartTest( NULL );
	DirectSoundCreate_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( DirectSoundCreate )
#pragma data_seg()

BEGIN_EXPORT_TABLE( DirectSoundCreate )
    EXPORT_TABLE_ENTRY( "StartTest", DirectSoundCreate_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DirectSoundCreate_EndTest )
END_EXPORT_TABLE( DirectSoundCreate )