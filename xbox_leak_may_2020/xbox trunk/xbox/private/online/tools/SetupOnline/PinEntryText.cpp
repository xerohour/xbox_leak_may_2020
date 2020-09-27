///////////////////////////////////////////////////////////////////////////////
//
// Name: PinEntryText.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation for pin entry text class
//
///////////////////////////////////////////////////////////////////////////////

#include "PinEntryText.h"

///////////////////////////////////////////////////////////////////////////////
//
// Name: constructor
//
// Input: dwX - the X location for the text
//        dwY - the Y location for the text
//        dwFGColor - the foreground color for the text
//        dwBGColor - the background color for the text
//
// Output: None
//
// Description: creates pint entry text object
//
///////////////////////////////////////////////////////////////////////////////

CPinEntryText::CPinEntryText( DWORD dwX, 
							  DWORD dwY, 
							  DWORD dwFGColor, 
							  DWORD dwBGColor )

	: CUserInterfaceText( "", dwX, dwY, dwFGColor, dwBGColor ),
	  m_handle( NULL ),
	  m_dwCurIndex( -1 ),
	  m_bInit( FALSE )

{
	//
	// no code here
	//
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Init
//
// Input: None
//
// Output: an hr
//
// Description: initializes the pin entry text
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CPinEntryText::Init( void )
{
	HRESULT hr = S_OK;

	//
	// reset the index
	//

	m_dwCurIndex = -1;

	//
	// zero out the internal state
	//
	ZeroMemory( m_szStr, 64 );
	ZeroMemory( &m_inputState, sizeof( XINPUT_STATE ) );
	ZeroMemory( m_aPINByte, 4 );

	//
	// call startup
	//

	if ( SUCCEEDED( hr ) )
	{
		m_handle = XOnlinePINStartInput( &m_inputState );
		hr = NULL == m_handle ? E_OUTOFMEMORY : S_OK;
	}

	//
	// rest the init flag only if success
	//

	if ( SUCCEEDED( hr ) )
	{
		m_bInit = TRUE;
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Destructor
//
// Input: None
//
// Output: None
//
// Description: destroys a pin entry Text object
//
///////////////////////////////////////////////////////////////////////////////

CPinEntryText::~CPinEntryText( void )
{
	HRESULT hr = XOnlinePINEndInput( m_handle );
	ASSERT( SUCCEEDED( hr ) );
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: UpdateText
//
// Input: None
//
// Output: the final HR
//
// Description: update the text
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CPinEntryText::UpdateText( void )
{
	HRESULT hr = S_OK;

	SetText( m_szStr );

	if ( SUCCEEDED( hr ) )
	{
		hr = XOnlinePINDecodeInput( m_handle, &m_inputState, m_aPINByte + m_dwCurIndex );
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Update
//
// Input: pInput - the input ui class
//        pAudio - the input audio class
//
// Output: the final HR
//
// Description: process input
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CPinEntryText::Update( CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio )
{
	HRESULT hr = S_OK;

	//
	// init the class
	// 

	if ( SUCCEEDED( hr ) && FALSE == m_bInit )
	{
		hr = Init();
	}

	//
	// zero out the input state
	//

	ZeroMemory( &m_inputState, sizeof( XINPUT_STATE ) );

	//
	// process the input
	//

	if ( pInput->LeftPressed() )
	{
		m_inputState.Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_LEFT;

		if ( ++m_dwCurIndex == 4 )
			m_dwCurIndex = 3;

		else
			strcat( m_szStr, "LEFT     " );
	}

	if ( pInput->RightPressed() )
	{
		m_inputState.Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_RIGHT;

		if ( ++m_dwCurIndex == 4 )
			m_dwCurIndex = 3;	

		else
			strcat( m_szStr, "RIGHT    " );
	}

	if ( pInput->UpPressed() )
	{
		m_inputState.Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_UP;

		if ( ++m_dwCurIndex == 4 )
			m_dwCurIndex = 3;

		else
			strcat( m_szStr, "UP       " );
	}

	if ( pInput->DownPressed() )
	{
		m_inputState.Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_DOWN;

		if ( ++m_dwCurIndex == 4 )
			m_dwCurIndex = 3;	

		else
			strcat( m_szStr, "DOWN     " );
	}

	if ( pInput->XPressed() )
	{
		m_inputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] = 255;

		if ( ++m_dwCurIndex == 4 )
			m_dwCurIndex = 3;	

		else
			strcat( m_szStr, "X        " );
	}

	if ( pInput->YPressed() )
	{
		m_inputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] = 255;

		if ( ++m_dwCurIndex == 4 )
			m_dwCurIndex = 3;	

		else
			strcat( m_szStr, "Y        " );
	}

	if ( pInput->BlackPressed() )
	{
		m_inputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] = 255;

		if ( ++m_dwCurIndex == 4 )
			m_dwCurIndex = 3;	

		else
			strcat( m_szStr, "BLACK    " );
	}

	if ( pInput->WhitePressed() )
	{
		m_inputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] = 255;

		if ( ++m_dwCurIndex == 4 )
			m_dwCurIndex = 3;	

		else
			strcat( m_szStr, "WHITE    " );
	}

	if ( pInput->LeftTriggerPressed() )
	{
		m_inputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] = 255;

		if ( ++m_dwCurIndex == 4 )
			m_dwCurIndex = 3;	

		else
			strcat( m_szStr, "LTRIGGER " );
	}

	if ( pInput->RightTriggerPressed() )
	{

		m_inputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] = 255;

		if ( ++m_dwCurIndex == 4 )
			m_dwCurIndex = 3;	

		else
			strcat( m_szStr, "RTRIGGER " );
	}

	if ( pInput->BPressed() )
	{
		if ( -1 != m_dwCurIndex )
			memset( m_szStr + m_dwCurIndex-- * 9, 0, 9 );

	}

	//
	// update the text
	//

	if ( SUCCEEDED( hr ) )
	{
		UpdateText();
	}

	return hr;
}