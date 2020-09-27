/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       XOnlinePINEndInput.cpp
 *  Content:    XOnlinePINEndInput tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  10/24/01    danrose Created to test Xbox XOnlinePINEndInput
 *
 ****************************************************************************/

#include <XOnlineAuthUtil.h>
#include "XOnlinePINEndInput.h"

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
 *  XOnlinePINEndInput_ValidInputHandle
 *
 *	Description:
 *		End pin input with a valid input handle
 *
 *	Input: 
 *		Handle: a valid handle
 *
 *	System State:
 *		None
 *
 *	Verify: 
 *		Return value is S_OK
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlinePINEndInput_ValidInputHandle( void )
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
 *  XOnlinePINEndInput_NullInputHandle
 *
 *	Description:
 *		End pin input with a NULL input handle
 *
 *	Input: 
 *		Handle: NULL
 *
 *	System State:
 *		None
 * 
 *	Verify: 
 *		Return value is not S_OK
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlinePINEndInput_NullInputHandle( void )
{
    HRESULT             hr              = S_OK;

	//
	// set up returns instead of asserts
	//

	g_ParamCheckReturnsError = TRUE;

	//
	// call PIN END INPUT with a null arg
	//

	CHECKRUN( SUCCEED_ON_FAIL( XOnlinePINEndInput( NULL ), E_INVALIDARG ) );

	//
	// reset val
	//

	g_ParamCheckReturnsError = FALSE;

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlinePINEndInput_StartTest
 *
 *  Description:
 *      The Harness Entry into the XOnlinePINEndInput tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI XOnlinePINEndInput_StartTest( HANDLE LogHandle )
{
    HRESULT             hr              = S_OK;

    //
    // the following tests will Assert (by design) in debug builds
    // to turn these tests on (they are off by default) define
    // CODE_COVERAGE when this file is compiled
    //

#ifdef CODE_COVERAGE

#endif // CODE_COVERAGE

	CHECKRUN( StartNetwork() );

    //
    // Test XOnlinePINEndInput with a valid input handle
    //

    SETLOG( LogHandle, "danrose", "Online", "XOnlinePINEndInput", "ValidInputHandle" );
    EXECUTE( XOnlinePINEndInput_ValidInputHandle() );

    //
    // Test XOnlinePINEndInput with a valid input handle
    //

    SETLOG( LogHandle, "danrose", "Online", "XOnlinePINEndInput", "NullInputHandle" );
    EXECUTE( XOnlinePINEndInput_NullInputHandle() );

	CHECKRUN( CloseNetwork() );
}

/****************************************************************************
 *
 *  XOnlinePINEndInput_EndTest
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

VOID WINAPI XOnlinePINEndInput_EndTest( VOID )
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
    XOnlinePINEndInput_StartTest( NULL );
    XOnlinePINEndInput_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( XOnlinePINEndInput )
#pragma data_seg()

BEGIN_EXPORT_TABLE( XOnlinePINEndInput )
    EXPORT_TABLE_ENTRY( "StartTest", XOnlinePINEndInput_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XOnlinePINEndInput_EndTest )
END_EXPORT_TABLE( XOnlinePINEndInput )
