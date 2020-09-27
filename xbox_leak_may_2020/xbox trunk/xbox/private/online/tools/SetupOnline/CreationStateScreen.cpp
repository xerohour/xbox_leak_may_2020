///////////////////////////////////////////////////////////////////////////////
//
// Name: CreationStateScreen.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation for creation state screen
//
///////////////////////////////////////////////////////////////////////////////

#include "CreationStateScreen.h"

///////////////////////////////////////////////////////////////////////////////
//
// Name:  creation state screen constructor
//
// Input: pParent - the parent screen for this screen
//
// Output: None
//
// Description: creates an instance of the creation state screen
//
///////////////////////////////////////////////////////////////////////////////

CCreationStateScreen::CCreationStateScreen( CUserInterfaceScreen* pParent )

	//
	// the parent of the creation state screen
	//

	: CUserInterfaceScreen( pParent, 
	                        CREATION_STATE_SCREEN_FG_COLOR, 
							CREATION_STATE_SCREEN_BG_COLOR, 
							CREATION_STATE_SCREEN_X, 
							CREATION_STATE_SCREEN_Y ),

	//
	// start in the start state
	//

	m_creationState( START_STATE ),
	
	//
	// start off with screens
	//

	m_pNameScreen( NULL ),
	m_pPinScreen( NULL ),
	m_pCreationScreen( NULL )

{
	//
	// init the screens... this can't fail
	//

	m_pNameScreen = new CNameEntryScreen( this );
	m_pPinScreen = new CPinEntryScreen( this );
	m_pCreationScreen = new CAccountCreationScreen( this );

	ASSERT( m_pNameScreen && m_pPinScreen && m_pCreationScreen );

	//
	// needs to have some kind text
	//

	SetText( "" );

}

///////////////////////////////////////////////////////////////////////////////
//
// Name:  creation state screen destructor
//
// Input: None
//
// Output: None
//
// Description: destroys an instance of the creation state screen
//
///////////////////////////////////////////////////////////////////////////////

CCreationStateScreen::~CCreationStateScreen( void )
{
	//
	// no code here
	//
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
CCreationStateScreen::Update( CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio )
{
	HRESULT hr = S_OK;
	CUserInterfaceScreen* pRet = NULL;

	//
	// if b was pressed from the name entry screen, make sure our state gets update
	// a NULL pInput can be generated if CANCEL was selected on the keyboard
	//

	if ( ( NULL == pInput || pInput->BPressed() ) && NAME_ENTRY_STATE == m_creationState )
	{
		m_creationState = START_STATE;

		pAudio->PlayBackButton();
		return ParentScreen();
	}

	//
	// if b was pressed from the pin entry screen, make sure our state gets update
	//

	if ( pInput->BPressed() && PIN_ENTRY_STATE == m_creationState )
	{
		m_creationState = NAME_ENTRY_STATE;

		pAudio->PlayBackButton();
		return m_pNameScreen;
	}

	//
	// if b was pressed from the creation screen, make sure our state gets update
	//

	if ( pInput->BPressed() && USER_CREATION_STATE == m_creationState )
	{
		m_creationState = PIN_ENTRY_STATE;

		pAudio->PlayBackButton();
		return m_pPinScreen;
	}

	//
	// if a was pressed from the pin entry  or name entry or creation play the select
	//

	if ( pInput->APressed() && ( NAME_ENTRY_STATE == m_creationState || 
		                         PIN_ENTRY_STATE == m_creationState  ||
								 USER_CREATION_STATE == m_creationState ) )
	{
		pAudio->PlaySelectButton();
	}

	//
	// decide which state we are in, which to goto
	//

	switch( m_creationState )
	{
		case START_STATE:
		{
			//
			// from start goto name
			//

			pRet = m_pNameScreen;
			m_creationState = NAME_ENTRY_STATE;

			break;
		}

		case NAME_ENTRY_STATE:
		{
			//
			// from name goto pin
			//

			pRet = m_pPinScreen;
			m_creationState = PIN_ENTRY_STATE;
			break;
		}

		case PIN_ENTRY_STATE:
		{
			//
			// set the name and pin on the creation screen
			//

			m_pCreationScreen->SetName( m_pNameScreen->GetName() );
			m_pCreationScreen->SetPin( m_pPinScreen->GetPin() );

			//
			// from pin goto creation
			//

			pRet = m_pCreationScreen;
			m_creationState = USER_CREATION_STATE;
			break;
		}

		case USER_CREATION_STATE:
		{
			//
			// from creation goto done
			//

			pRet = ParentScreen();
			m_creationState = DONE_STATE;

			break;
		}

		case DONE_STATE:
		{
			m_creationState = START_STATE;
			pRet = NULL;
			break;
		}


	}

	return pRet;
}