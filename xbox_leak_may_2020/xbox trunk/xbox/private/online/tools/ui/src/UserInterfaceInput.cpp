///////////////////////////////////////////////////////////////////////////////
//
// Name: UserInterfaceInput.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation for ui input class
//
///////////////////////////////////////////////////////////////////////////////

#include "UserInterfaceInput.h"

///////////////////////////////////////////////////////////////////////////////
//
// Name: CUserInterfaceInput
//
// Input: None
//
// Output: an instance of the user interface input class
//
// Description: constructs the input for the UI
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceInput::CUserInterfaceInput( void )
{
	//
	// Zero out the handle array
	//

	ZeroMemory( m_ahInput, sizeof( m_ahInput ) );

	//
	// Zero out the old input state
	//

	ZeroMemory( &m_oldInputState, sizeof( XINPUT_STATE ) );

	//
	// reset the button state
	//

	ResetButtonState();
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: ~CUserInterfaceInput
//
// Input: None
//
// Output: None
//
// Description: destructs the input ui
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceInput::~CUserInterfaceInput( void )
{
	//
	// Destroy the handles
	//

	for ( DWORD i = 0; i < NUM_JOYSTICKS; i++ )
	{
		if ( NULL != m_ahInput[i] )
		{
			XInputClose( m_ahInput[i] );
			m_ahInput[i] = NULL;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Initialize
//
// Input: None
//
// Output: S_OK if input creation was successful, E_XXXX otherwise
//
// Description: sets up the app for getting input from the controllers
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CUserInterfaceInput::Initialize( void )
{
	HRESULT hr = S_OK;

	DWORD dwRet = 0;
	DWORD dwPort = -1;

	//
	// init the devices with the default values
	//

	XInitDevices( 0, NULL );
	
	//
	// Get the devices
	//

	dwRet = XGetDevices( XDEVICE_TYPE_GAMEPAD );

	//
	// strip off high bits; low bits hold controller info
	//

	dwRet &= 0xF;

	//
	// Open the available gamepads
	// it is ok for no devices to be attached for the app
	//

	for ( DWORD i = 0; i < NUM_JOYSTICKS && SUCCEEDED( hr ); i++, dwRet >>= 1 )
	{
		//
		// Only create a handle if the device exists
		//

		if ( dwRet & 0x1 )
		{
			//
			// make sure the handle did not exist
			//

			ASSERT( NULL == m_ahInput[i] || ! "input handle already allocated" );

			//
			// create the handle
			//

			m_ahInput[i] = XInputOpen( XDEVICE_TYPE_GAMEPAD, 
									   i, 
									   XDEVICE_NO_SLOT, 
									   NULL );

			hr = NULL == m_ahInput[i] ? E_FAIL : S_OK;

			ASSERT( SUCCEEDED( hr ) || !"Could not open input handle" );
		}
	}

	//
	// spit back the final hresult
	//

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: UpdateButtonState
//
// Input: pInputState - the state of the inputted buttons
//
// Output: S_OK on success. E_XXXX on failure
//
// Description: updates the ui state with the currently pressed buttons
//
///////////////////////////////////////////////////////////////////////////////

void
CUserInterfaceInput::UpdateButtonState( PXINPUT_STATE pInputState )
{
	ASSERT( pInputState || !"Passed in NULL to pInputState" );

	if ( ! pInputState )
	{
		return;
	}

	//
	// get the digital changes
	//

	if ( pInputState->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP )
		m_bUpPressed = TRUE;

	if ( pInputState->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN )
		m_bDownPressed = TRUE;

	if ( pInputState->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT )
		m_bLeftPressed = TRUE;

	if ( pInputState->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
		m_bRightPressed = TRUE;

	if ( ( m_oldInputState.Gamepad.wButtons & XINPUT_GAMEPAD_START ) &&
		 ( ( pInputState->Gamepad.wButtons & XINPUT_GAMEPAD_START ) == 0 ) )
		m_bStartPressed = TRUE;

	if ( ( m_oldInputState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK ) &&
		 ( ( pInputState->Gamepad.wButtons & XINPUT_GAMEPAD_BACK ) == 0 ) )
		m_bBackPressed = TRUE;

	if ( ( m_oldInputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ) &&
		 ( ( pInputState->Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ) == 0 ) )
		m_bLeftThumbPressed = TRUE;

	if ( ( m_oldInputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ) &&
		 ( ( pInputState->Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ) == 0 ) )
		m_bRightThumbPressed = TRUE;

	//
	// get the analog changes
	//

	if ( m_oldInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] > 0 &&
		 pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] == 0 )
		m_bAPressed = TRUE;

	if ( m_oldInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B] > 0 &&
		 pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B] == 0 )
		m_bBPressed = TRUE;

	if ( m_oldInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] > 0 &&
		 pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] == 0 )
		m_bXPressed = TRUE;

	if ( m_oldInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] > 0 &&
		 pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] == 0 )
		m_bYPressed = TRUE;

	if ( pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] > 0 )
		m_bBlackPressed = TRUE;

	if ( pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] > 0 )
		m_bWhitePressed = TRUE;

	if ( pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > 0 )
		m_bLeftTriggerPressed = TRUE;

	if ( pInputState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > 0 )
		m_bRightTriggerPressed = TRUE;

	//
	// save the old state
	//

	memcpy( &m_oldInputState, pInputState, sizeof( XINPUT_STATE ) );

	//
	// get the analog stick changes
	//

	#pragma message( "TODO: add handling of analog sticks" )

}

///////////////////////////////////////////////////////////////////////////////
//
// Name: ResetButtonState
//
// Input: None
//
// Output: None
//
// Description: resets the state of the buttons to 0
//
///////////////////////////////////////////////////////////////////////////////

void
CUserInterfaceInput::ResetButtonState( void )
{
	m_bUpPressed = FALSE;
	m_bDownPressed = FALSE;
	m_bLeftPressed = FALSE;
	m_bRightPressed = FALSE;
	m_bAPressed = FALSE;
	m_bBPressed = FALSE;
	m_bXPressed = FALSE;
	m_bYPressed = FALSE;
	m_bBlackPressed = FALSE;
	m_bWhitePressed = FALSE;
	m_bBackPressed = FALSE;
	m_bStartPressed = FALSE;
	m_bLeftTriggerPressed = FALSE;
	m_bRightTriggerPressed = FALSE;
	m_bLeftThumbPressed = FALSE;
	m_bRightThumbPressed = FALSE;
}


///////////////////////////////////////////////////////////////////////////////
//
// Name: Update
//
// Input: None
//
// Output: S_OK on success. E_XXXX on failure
//
// Description: gets input from the joypads
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CUserInterfaceInput::Update( void )
{
	HRESULT hr = S_OK;
	DWORD dwInsertions = 0;
	DWORD dwRemovals = 0;
	DWORD dwError = 0;

	XINPUT_STATE inputState;

	//
	// Zero out the input state
	//

	ZeroMemory( &inputState, sizeof( XINPUT_STATE ) );

	//
	// get the changed controller state
	//

	if ( XGetDeviceChanges( XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals ) )
	{
		//
		// strip off high bits
		//

		dwRemovals &= 0xF;
		dwInsertions &= 0xF;

		//
		// handle insertions / removals
		//

		for ( DWORD i = 0; 
		      i < NUM_JOYSTICKS && SUCCEEDED( hr ); 
			  i++, 
			  dwRemovals >>= 1, 
			  dwInsertions >>= 1 )
		{
			//
			// close the handle
			//

			if ( dwRemovals & 0x1 && SUCCEEDED( hr ) )
			{
				//
				// make sure handle existed
				//

				ASSERT( NULL != m_ahInput[i] || !"input handle was never allocated" );

				//
				// close the existing handle
				//

				XInputClose( m_ahInput[i] );
				m_ahInput[i] = NULL;
			}

			//
			// Only create a handle if the device exists
			//

			if ( dwInsertions & 0x1 && SUCCEEDED( hr ) )
			{
				//
				// make sure handle never existed
				//

				ASSERT( NULL == m_ahInput[i] || ! "input handle already allocated" );

				//
				// open the new handle
				//

				m_ahInput[i] = XInputOpen( XDEVICE_TYPE_GAMEPAD, 
										   i, 
										   XDEVICE_NO_SLOT, 
										   NULL );

				hr = NULL == m_ahInput[i] ? E_FAIL : S_OK;

				ASSERT( SUCCEEDED( hr ) || !"Could not open input handle" );
			}
		}
	}

	//
	// Reset the button state
	//

	ResetButtonState();

	//
	// get the button presses from the controllers
	//

	for ( DWORD i = 0; i < NUM_JOYSTICKS && SUCCEEDED( hr ); i++ )
	{
		//
		// zero out the input state
		//

		ZeroMemory( &inputState, sizeof( XINPUT_STATE ) );

		//
		// read the current joystick button state
		//

		if ( m_ahInput[i] )
		{
			//
			// get the state
			//

			dwError = XInputGetState( m_ahInput[i], &inputState );

			hr = ERROR_SUCCESS == dwError ? S_OK : E_FAIL;
			ASSERT( SUCCEEDED( hr ) || ! "could not get input state" );

			//
			// Update the internal button state
			//

			if ( SUCCEEDED( hr ) )
			{
				UpdateButtonState( &inputState );
			}
		}
	}

	//
	// reboot if L-R-Black was pushed
	//

	if ( SUCCEEDED( hr ) && LeftTriggerPressed() && RightTriggerPressed() && BlackPressed() )
	{
		XLaunchNewImage (NULL, NULL);
	}

	return hr;
}