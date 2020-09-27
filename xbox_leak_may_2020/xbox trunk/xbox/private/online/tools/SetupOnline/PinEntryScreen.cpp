///////////////////////////////////////////////////////////////////////////////
//
// Name: PinEntryScreen.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation for pin entry screen
//
///////////////////////////////////////////////////////////////////////////////

#include "PinEntryScreen.h"

///////////////////////////////////////////////////////////////////////////////
//
// Name:  pin entry screen constructor
//
// Input: pParent - the parent screen for this screen
//
// Output: None
//
// Description: creates an instance of the pin entry screen
//
///////////////////////////////////////////////////////////////////////////////

CPinEntryScreen::CPinEntryScreen( CUserInterfaceScreen* pParent )

	//
	// the parent of the pin entry screen
	//

	: CUserInterfaceScreen( pParent, 
	                        PIN_ENTRY_SCREEN_FG_COLOR, 
							PIN_ENTRY_SCREEN_BG_COLOR, 
							PIN_ENTRY_SCREEN_X, 
							PIN_ENTRY_SCREEN_Y ),

	//
	// the text
	//

	  m_aText( PIN_ENTRY_A_TEXT, PIN_ENTRY_A_X, PIN_ENTRY_A_Y, PIN_ENTRY_A_FG_COLOR, PIN_ENTRY_A_BG_COLOR ),
      m_bText( PIN_ENTRY_B_BACK_TEXT, PIN_ENTRY_B_X, PIN_ENTRY_B_Y, PIN_ENTRY_B_FG_COLOR, PIN_ENTRY_B_BG_COLOR ),
	  m_pinText( m_szStr, PIN_X, PIN_Y, PIN_FG_COLOR, PIN_BG_COLOR ),

	  m_handle( NULL ),
	  m_dwCurIndex( -1 ),
	  m_bInit( FALSE )

{
	ZeroMemory( m_szStr, sizeof( m_szStr ) );

	//
	// needs to have some kind text
	//

	SetText( PIN_ENTRY_TITLE );

	//
	// add the ui text
	//

	AddUIText( &m_aText );
	AddUIText( &m_bText );
	AddUIText( &m_pinText );
}

///////////////////////////////////////////////////////////////////////////////
//
// Name:  pin entry screen destructor
//
// Input: None
//
// Output: None
//
// Description: destroys an instance of the pin entry screen
//
///////////////////////////////////////////////////////////////////////////////

CPinEntryScreen::~CPinEntryScreen( void )
{
	HRESULT hr = XOnlinePINEndInput( m_handle );
	ASSERT( SUCCEEDED( hr ) );
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
CPinEntryScreen::Init( void )
{
	HRESULT hr = S_OK;

	//
	// reset the index
	//

	m_dwCurIndex = -1;

	//
	// zero out the internal state
	//
	ZeroMemory( m_szStr, sizeof( m_szStr ) );
	ZeroMemory( &m_inputState, sizeof( XINPUT_STATE ) );
	ZeroMemory( m_aPINByte, XONLINE_PIN_LENGTH );

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
CPinEntryScreen::UpdateText( void )
{
	HRESULT hr = S_OK;

	m_pinText.SetText( m_szStr );

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
// Input: pInput - the input ui
//        pAudio - the audio ui
//
// Output: the next screen
//
// Description: process the input for the screen
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceScreen* 
CPinEntryScreen::Update( CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio )
{
	HRESULT hr = S_OK;
	CUserInterfaceScreen* pRet = NULL;

	//
	// zero out the input state
	//

	ZeroMemory( &m_inputState, sizeof( XINPUT_STATE ) );

	//
	// init the class
	// 

	if ( SUCCEEDED( hr ) && FALSE == m_bInit )
	{
		hr = Init();
	}

	//
	// if b was pressed and the pin is empty, go back OR if A was pressed
	//

	if ( ( pInput->BPressed() && 0 == strcmp( "", m_szStr ) ) ||
		 pInput->APressed() )
	{
		//
		// let the parent intercept the update
		//

		pRet = ParentScreen()->Update( pInput, pAudio );
	}

	//
	// otherwise if b was pressed, clear out the pin
	//

	else if ( pInput->BPressed() )
	{
		//
		// reset the index
		//

		m_dwCurIndex = -1;

		//
		// zero out the internal state
		//

		ZeroMemory( m_szStr, sizeof( m_szStr ) );
		ZeroMemory( &m_inputState, sizeof( XINPUT_STATE ) );
		ZeroMemory( m_aPINByte, XONLINE_PIN_LENGTH );

		pAudio->PlayBackButton();
	}

	//
	// process the input
	//

	if ( pInput->LeftPressed() )
	{
		m_inputState.Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_LEFT;

		if ( ++m_dwCurIndex == XONLINE_PIN_LENGTH )
			m_dwCurIndex = XONLINE_PIN_LENGTH - 1;

		else
			strcat( m_szStr, "LEFT     " );

		pAudio->PlayKeyboardStroke();
	}

	else if ( pInput->RightPressed() )
	{
		m_inputState.Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_RIGHT;

		if ( ++m_dwCurIndex == XONLINE_PIN_LENGTH )
			m_dwCurIndex = XONLINE_PIN_LENGTH - 1;	

		else
			strcat( m_szStr, "RIGHT    " );

		pAudio->PlayKeyboardStroke();
	}

	else if ( pInput->UpPressed() )
	{
		m_inputState.Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_UP;

		if ( ++m_dwCurIndex == XONLINE_PIN_LENGTH )
			m_dwCurIndex = XONLINE_PIN_LENGTH - 1;

		else
			strcat( m_szStr, "UP       " );

		pAudio->PlayKeyboardStroke();
	}

	else if ( pInput->DownPressed() )
	{
		m_inputState.Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_DOWN;

		if ( ++m_dwCurIndex == XONLINE_PIN_LENGTH )
			m_dwCurIndex = XONLINE_PIN_LENGTH - 1;	

		else
			strcat( m_szStr, "DOWN     " );

		pAudio->PlayKeyboardStroke();
	}

	else if ( pInput->XPressed() )
	{
		m_inputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] = 255;

		if ( ++m_dwCurIndex == XONLINE_PIN_LENGTH )
			m_dwCurIndex = XONLINE_PIN_LENGTH - 1;	

		else
			strcat( m_szStr, "X        " );

		pAudio->PlayKeyboardStroke();
	}

	else if ( pInput->YPressed() )
	{
		m_inputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] = 255;

		if ( ++m_dwCurIndex == XONLINE_PIN_LENGTH )
			m_dwCurIndex = XONLINE_PIN_LENGTH - 1;	

		else
			strcat( m_szStr, "Y        " );

		pAudio->PlayKeyboardStroke();
	}

	else if ( pInput->BlackPressed() )
	{
		m_inputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] = 255;

		if ( ++m_dwCurIndex == XONLINE_PIN_LENGTH )
			m_dwCurIndex = XONLINE_PIN_LENGTH - 1;	

		else
			strcat( m_szStr, "BLACK    " );

		pAudio->PlayKeyboardStroke();
	}

	else if ( pInput->WhitePressed() )
	{
		m_inputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] = 255;

		if ( ++m_dwCurIndex == XONLINE_PIN_LENGTH )
			m_dwCurIndex = XONLINE_PIN_LENGTH - 1;	

		else
			strcat( m_szStr, "WHITE    " );

		pAudio->PlayKeyboardStroke();
	}

	else if ( pInput->LeftTriggerPressed() )
	{
		m_inputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] = 255;

		if ( ++m_dwCurIndex == XONLINE_PIN_LENGTH )
			m_dwCurIndex = XONLINE_PIN_LENGTH - 1;	

		else
			strcat( m_szStr, "LTRIGGER " );

		pAudio->PlayKeyboardStroke();
	}

	else if ( pInput->RightTriggerPressed() )
	{

		m_inputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] = 255;

		if ( ++m_dwCurIndex == XONLINE_PIN_LENGTH )
			m_dwCurIndex = XONLINE_PIN_LENGTH - 1;	

		else
			strcat( m_szStr, "RTRIGGER " );

		pAudio->PlayKeyboardStroke();
	}

	//
	// if we have entered pin data, then update the UI
	//

	if ( -1 != m_dwCurIndex )
	{
		m_bText.SetText( PIN_ENTRY_B_ERASE_TEXT );
	}

	else
	{
		m_bText.SetText( PIN_ENTRY_B_BACK_TEXT );
	}

	//
	// update the text
	//

	if ( SUCCEEDED( hr ) )
	{
		UpdateText();
	}

	return pRet;
}