/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       XOnlinePINStartInput.cpp
 *  Content:    XOnlinePINStartInput tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  10/24/01    danrose Created to test Xbox XOnlinePINStartInput
 *
 ****************************************************************************/

#include <XOnlineAuthUtil.h>
#include "XOnlinePINStartInput.h"
#include <list>
using namespace std;

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;

/****************************************************************************
 *
 * The Global online param check flag
 *
 ****************************************************************************/

extern BOOL g_ParamCheckReturnsError;

/****************************************************************************
 *
 *  XOnlinePINStartInput_ValidInputState
 *
 *	Description:
 *		Start pin input with a valid input state pointer
 *
 *	Input: 
 *		pInputState: valid pointer
 *
 *	System State:
 *		None
 *
 *	Verify: 
 *		Return value is not NULL
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlinePINStartInput_ValidInputState( void )
{
    HRESULT             hr              = S_OK;
	XPININPUTHANDLE		inputHandle		= NULL;
	XINPUT_STATE		inputState;

	//
	// Zero out the input state
	//
	
	ZeroMemory( &inputState, sizeof( XINPUT_STATE ) );

	//
	// Start PIN Input 
	//

	CHECKRUNHANDLE( inputHandle, XOnlinePINStartInput( &inputState ) );

	//
	// End PIN Input
	//

	RUN( XOnlinePINEndInput( inputHandle ) );

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlinePINStartInput_ValidInputStateMultiple
 *
 *	Description:
 * 		Start pin input multiple times with valid input state pointers
 *
 *	Input: 
 *		pInputState: valid pointer
 *
 *	System State:
 *		None
 *
 *	Verify: 
 *		Return value is NULL on final call
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlinePINStartInput_ValidInputStateMultiple( void )
{
    HRESULT             hr              = S_OK;
	XPININPUTHANDLE		inputHandle		= (XPININPUTHANDLE) INVALID_HANDLE_VALUE;
	XINPUT_STATE		inputState;

	//
	// keep a list of handles
	//

	list<XPININPUTHANDLE> inputHandles;

	//
	// Zero out the input state
	//
	
	ZeroMemory( &inputState, sizeof( XINPUT_STATE ) );

	//
	// cycle through calls to XOnlinePINStartInput
	//

	while ( NULL != inputHandle && SUCCEEDED( hr ) )
	{
		CHECKRUNHANDLE( inputHandle, XOnlinePINStartInput( &inputState ) );

		//
		// the above check will set hr to E_FAIL if NULL IS SEEN
		//

		if ( SUCCEEDED( hr ) )
		{
			try 
			{
				inputHandles.push_back( inputHandle );
			} 
			catch ( ... )
			{
				// NO CODE HERE
			}
		}

		//
		// reset hr
		//

		else
		{
			hr = S_OK;
		}
	}

	//
	// reset the input handle
	//

	inputHandle = (XPININPUTHANDLE) INVALID_HANDLE_VALUE;

	//
	// close each handle until the list is empty
	//

	while ( NULL != inputHandle && ! inputHandles.empty() )
	{
		//
		// get the front handle, and pop it off
		//

		inputHandle = inputHandles.front();
		inputHandles.pop_front();
		
		//
		// make sure the handle is not NULL
		//

		if ( NULL != inputHandle )
		{
			RUN( XOnlinePINEndInput( inputHandle ) );
		}
	}

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlinePINStartInput_NullInputState
 *
 *	Description:
 *		Start pin input with a null input state pointer
 *
 *	Input: 
 *		PInputState: NULL
 *
 *	System State:
 *		None
 *
 *	Verify: 
 *		Return value is NULL
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlinePINStartInput_NullInputState( void )
{
    HRESULT             hr              = S_OK;
	XPININPUTHANDLE		inputHandle		= NULL;

	//
	// Set param checking as returning values
	//

	g_ParamCheckReturnsError = TRUE;

	//
	// Start PIN Input 
	//

	CHECKRUNHANDLE( inputHandle, XOnlinePINStartInput( NULL ) );
	if ( NULL == inputHandle )
	{
		hr = S_OK;
	}

	//
	// reset the state of param checking
	//

	g_ParamCheckReturnsError = FALSE;

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlinePINStartInput_StartTest
 *
 *  Description:
 *      The Harness Entry into the XOnlinePINStartInput tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI XOnlinePINStartInput_StartTest( HANDLE LogHandle )
{
    HRESULT             hr              = S_OK;

    //
    // the following tests will Assert (by design) in debug builds
    // to turn these tests on (they are off by default) define
    // CODE_COVERAGE when this file is compiled
    //

	CHECKRUN( StartNetwork() );

#ifdef CODE_COVERAGE

    //
    // Test XOnlinePINStartInput with multiple input states
    //

    SETLOG( LogHandle, "danrose", "Online", "XOnlinePINStartInput", "ValidInputStateMultiple" );
    EXECUTE( XOnlinePINStartInput_ValidInputStateMultiple() );

#endif // CODE_COVERAGE

    //
    // Test XOnlinePINStartInput with a NULL Input State
    //

    SETLOG( LogHandle, "danrose", "Online", "XOnlinePINStartInput", "NullInputState" );
    EXECUTE( XOnlinePINStartInput_NullInputState() );
    
	//
    // Test XOnlinePINStartInput with a valid input state
    //

    SETLOG( LogHandle, "danrose", "Online", "XOnlinePINStartInput", "ValidInputState" );
    EXECUTE( XOnlinePINStartInput_ValidInputState() );

	CHECKRUN( CloseNetwork() );
}

/****************************************************************************
 *
 *  XOnlinePINStartInput_EndTest
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

VOID WINAPI XOnlinePINStartInput_EndTest( VOID )
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
    XOnlinePINStartInput_StartTest( NULL );
    XOnlinePINStartInput_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( XOnlinePINStartInput )
#pragma data_seg()

BEGIN_EXPORT_TABLE( XOnlinePINStartInput )
    EXPORT_TABLE_ENTRY( "StartTest", XOnlinePINStartInput_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XOnlinePINStartInput_EndTest )
END_EXPORT_TABLE( XOnlinePINStartInput )
