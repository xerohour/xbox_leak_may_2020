/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       XOnlinePINDecodeInput.cpp
 *  Content:    XOnlinePINDecodeInput tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  10/24/01    danrose Created to test Xbox XOnlinePINDecodeInput
 *
 ****************************************************************************/

#include <XOnlineAuthUtil.h>
#include "XOnlinePINDecodeInput.h"

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
 *  XOnlinePINDecodeInput_VALID_HANDLE_CHANGED_STATE_VALID_PBYTE
 *
 *	Description:
 *		Decode PIN Input after an input change for each possible button press
 *
 *	Input: 
 *		Handle - a valid handle value
 *		PInputState - a filled in XINPUT_STATE with changes
 *		PINByte - a valid byte pointer
 *
 *	System State:
 *		None
 *
 *	Verify:
 *		Return value is S_OK
 *		BYTE value returned is correct one for input 
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlinePINDecodeInput_VALID_HANDLE_CHANGED_STATE_VALID_PBYTE( void )
{
    HRESULT             hr              = S_OK;

	static const DWORD	NUM_BUTTONS		= 10;

	XPININPUTHANDLE		inputHandle		= NULL;
	XINPUT_STATE		inputState;
	BYTE				byVals[NUM_BUTTONS];

	//
	// Zero out the byte values
	//

	ZeroMemory( byVals, sizeof( byVals ) );

	//
	// Zero out the input state
	//
	
	ZeroMemory( &inputState, sizeof( XINPUT_STATE ) );

	//
	// create the pin input handle
	//

	CHECKRUNHANDLE( inputHandle, XOnlinePINStartInput( &inputState ) );
	
	//
	// cycle through each possible input value
	//

	for ( DWORD i = 0; i < NUM_BUTTONS && SUCCEEDED( hr ); i++ )
	{
		//
		// Zero out the input state
		//
		
		ZeroMemory( &inputState, sizeof( XINPUT_STATE ) );

		//
		// fill in the current packet
		//

		inputState.dwPacketNumber = i * 2;

		//
		// fill in the inputState based on the loop counter
		//

		switch( i )
		{
			case 0:
				inputState.Gamepad.wButtons = XINPUT_GAMEPAD_DPAD_UP;
				break;

			case 1:
				inputState.Gamepad.wButtons = XINPUT_GAMEPAD_DPAD_DOWN;
				break;

			case 2:
				inputState.Gamepad.wButtons = XINPUT_GAMEPAD_DPAD_LEFT;
				break;

			case 3:
				inputState.Gamepad.wButtons = XINPUT_GAMEPAD_DPAD_RIGHT;
				break;

			case 4:
				inputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] = 255;
				break;

			case 5:
				inputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] = 255;
				break;

			case 6:
				inputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] = 255;
				break;

			case 7:
				inputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] = 255;
				break;

			case 8:
				inputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] = 255;
				break;

			case 9:
				inputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] = 255;
				break;
		}

		//
		// decode this byte
		//

		CHECKRUN( XOnlinePINDecodeInput( inputHandle, &inputState, byVals + i ) );

		//
		// Zero out the input state
		//
		
		ZeroMemory( &inputState, sizeof( XINPUT_STATE ) );

		//
		// update the packet
		//

		inputState.dwPacketNumber = i * 2 + 1;

		//
		// simulate clearing of a button
		//

		CHECKRUN( XOnlinePINDecodeInput( inputHandle, &inputState, byVals + i ) );
	}

	//
	// cycle through, and verify
	//

	for ( DWORD i = 0; i < NUM_BUTTONS && SUCCEEDED( hr ); i++ )
	{
		if ( i + 1 != byVals[i] )
		{
			hr = E_FAIL;
		}
	}

	//
	// Destroy the input handle
	//

	RUN( XOnlinePINEndInput( inputHandle ) );

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlinePINDecodeInput_VALID_HANDLE_UNCHANGED_STATE_VALID_PBYTE
 *
 *	Description:
 *		Decode PIN Input after no input change
 *
 *	Input: 
 *		Handle - a valid handle value
 *		PInputState - a filled in XINPUT_STATE out changes
 *		PINByte - a valid byte pointer
 *
 *	System State:
 *		None
 * 
 *	Verify:
 *		Return value is S_FALSE
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlinePINDecodeInput_VALID_HANDLE_UNCHANGED_STATE_VALID_PBYTE( void )
{
    HRESULT             hr              = S_OK;

	XPININPUTHANDLE		inputHandle		= NULL;
	XINPUT_STATE		inputState;
	BYTE				byVals[12];

	//
	// Zero out the byte values
	//

	ZeroMemory( byVals, sizeof( byVals ) );

	//
	// Zero out the input state
	//
	
	ZeroMemory( &inputState, sizeof( XINPUT_STATE ) );

	//
	// create the pin input handle
	//

	CHECKRUNHANDLE( inputHandle, XOnlinePINStartInput( &inputState ) );

	//
	// decode the pin input
	//

	CHECKRUN( XOnlinePINDecodeInput( inputHandle, &inputState, byVals ) );

	//
	// Destroy the input handle
	//

	RUN( XOnlinePINEndInput( inputHandle ) );

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlinePINDecodeInput_NULL_HANDLE_VALID_STATE_VALID_PBYTE
 *
 *	Description:
 *		Make sure Decode PIN input handles a NULL handle
 *
 *	Input: 
 *		Handle - NULL
 *		PInputState - a filled in XINPUT_STATE
 *		PINByte - a valid byte pointer
 *
 *	System State:
 *		None
 *
 *	Verify:
 *		Return value is E_XXXXX
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlinePINDecodeInput_NULL_HANDLE_VALID_STATE_VALID_PBYTE( void )
{
    HRESULT             hr              = S_OK;

	XPININPUTHANDLE		inputHandle		= NULL;
	XINPUT_STATE		inputState;
	BYTE				byVals[12];

	//
	// Zero out the byte values
	//

	ZeroMemory( byVals, sizeof( byVals ) );

	//
	// Zero out the input state
	//
	
	ZeroMemory( &inputState, sizeof( XINPUT_STATE ) );

	//
	// set the param check flag
	//

	g_ParamCheckReturnsError = TRUE;

	//
	// decode the pin input
	//

	CHECKRUN( SUCCEED_ON_FAIL( XOnlinePINDecodeInput( inputHandle, &inputState, byVals ), E_INVALIDARG ) );

	//
	// reset the param check flag
	//

	g_ParamCheckReturnsError = FALSE;

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlinePINDecodeInput_VALID_HANDLE_VALID_STATE_NULL_PBYTE
 *
 *	Description:
 *		Make sure Decode PIN Input handles NULL pByte pointer
 *
 *	Input: 
 *		Handle - a valid handle value
 *		PInputState - a filled in XINPUT_STATE
 *		PINByte - NULL
 *
 *	System State:
 *		None
 *
 *	Verify:
 *		Return value is E_XXXXX
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlinePINDecodeInput_VALID_HANDLE_VALID_STATE_NULL_PBYTE( void )
{
    HRESULT             hr              = S_OK;

	XPININPUTHANDLE		inputHandle		= NULL;
	XINPUT_STATE		inputState;

	//
	// Zero out the input state
	//
	
	ZeroMemory( &inputState, sizeof( XINPUT_STATE ) );

	//
	// create the pin input handle
	//

	CHECKRUNHANDLE( inputHandle, XOnlinePINStartInput( &inputState ) );

	//
	// set the param check flag
	//

	g_ParamCheckReturnsError = TRUE;

	//
	// decode the pin input
	//

	CHECKRUN( SUCCEED_ON_FAIL( XOnlinePINDecodeInput( inputHandle, &inputState, NULL ), E_INVALIDARG ) );

	//
	// reset the param check flag
	//

	g_ParamCheckReturnsError = FALSE;

	//
	// Destroy the input handle
	//

	RUN( XOnlinePINEndInput( inputHandle ) );
    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlinePINDecodeInput_VALID_HANDLE_NULL_STATE_VALID_PBYTE
 *
 *	Description:
 *		Make sure Decode PIN input handles a NULL state pointer
 * 
 *	Input: 
 *		Handle - a valid handle value
 *		PInputState - NULL
 *		PINByte - a valid byte pointer
 *
 *	System State:
 *			None
 *
 *	Verify:
 *		Return value is E_XXXXXX
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT XOnlinePINDecodeInput_VALID_HANDLE_NULL_STATE_VALID_PBYTE( void )
{
    HRESULT             hr              = S_OK;

	XPININPUTHANDLE		inputHandle		= NULL;
	XINPUT_STATE		inputState;
	BYTE				byVals[12];

	//
	// Zero out the byte values
	//

	ZeroMemory( byVals, sizeof( byVals ) );

	//
	// Zero out the input state
	//
	
	ZeroMemory( &inputState, sizeof( XINPUT_STATE ) );

	//
	// create the pin input handle
	//

	CHECKRUNHANDLE( inputHandle, XOnlinePINStartInput( &inputState ) );

	//
	// set the param check flag
	//

	g_ParamCheckReturnsError = TRUE;

	//
	// decode the pin input
	//

	CHECKRUN( SUCCEED_ON_FAIL( XOnlinePINDecodeInput( inputHandle, NULL, byVals ), E_INVALIDARG ) );

	//
	// reset the param check flag
	//

	g_ParamCheckReturnsError = FALSE;

	//
	// Destroy the input handle
	//

	RUN( XOnlinePINEndInput( inputHandle ) );

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  XOnlinePINDecodeInput_StartTest
 *
 *  Description:
 *      The Harness Entry into the XOnlinePINDecodeInput tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI XOnlinePINDecodeInput_StartTest( HANDLE LogHandle )
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
    // Test XOnlinePINDecodeInput in the mannor it was meant to be called (changed state)
    //

    SETLOG( LogHandle, "danrose", "Online", "XOnlinePINDecodeInput", "VALID_HANDLE_CHANGED_STATE_VALID_PBYTE" );
    EXECUTE( XOnlinePINDecodeInput_VALID_HANDLE_CHANGED_STATE_VALID_PBYTE() );

    //
    // Test XOnlinePINDecodeInput in the mannor it was meant to be called (unchanged state)
    //

    SETLOG( LogHandle, "danrose", "Online", "XOnlinePINDecodeInput", "VALID_HANDLE_UNCHANGED_STATE_VALID_PBYTE" );
    EXECUTE( XOnlinePINDecodeInput_VALID_HANDLE_UNCHANGED_STATE_VALID_PBYTE() );

    //
    // Test XOnlinePINDecodeInput with a null handle
    //

    SETLOG( LogHandle, "danrose", "Online", "XOnlinePINDecodeInput", "NULL_HANDLE_VALID_STATE_VALID_PBYTE" );
    EXECUTE( XOnlinePINDecodeInput_NULL_HANDLE_VALID_STATE_VALID_PBYTE() );

    //
    // Test XOnlinePINDecodeInput with a null pByte
    //

    SETLOG( LogHandle, "danrose", "Online", "XOnlinePINDecodeInput", "VALID_HANDLE_VALID_STATE_NULL_PBYTE" );
    EXECUTE( XOnlinePINDecodeInput_VALID_HANDLE_VALID_STATE_NULL_PBYTE() );

    //
    // Test XOnlinePINDecodeInput with a null state
    //

    SETLOG( LogHandle, "danrose", "Online", "XOnlinePINDecodeInput", "VALID_HANDLE_NULL_STATE_VALID_PBYTE" );
    EXECUTE( XOnlinePINDecodeInput_VALID_HANDLE_NULL_STATE_VALID_PBYTE() );

	CHECKRUN( CloseNetwork() );
}

/****************************************************************************
 *
 *  XOnlinePINDecodeInput_EndTest
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

VOID WINAPI XOnlinePINDecodeInput_EndTest( VOID )
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
    XOnlinePINDecodeInput_StartTest( NULL );
    XOnlinePINDecodeInput_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( XOnlinePINDecodeInput )
#pragma data_seg()

BEGIN_EXPORT_TABLE( XOnlinePINDecodeInput )
    EXPORT_TABLE_ENTRY( "StartTest", XOnlinePINDecodeInput_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XOnlinePINDecodeInput_EndTest )
END_EXPORT_TABLE( XOnlinePINDecodeInput )
