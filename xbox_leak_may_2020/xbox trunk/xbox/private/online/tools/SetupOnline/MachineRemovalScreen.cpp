///////////////////////////////////////////////////////////////////////////////
//
// Name: MachineRemovalScreen.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation machine removal screen
//
///////////////////////////////////////////////////////////////////////////////

#include "MachineRemovalScreen.h"
#include <OnlineAccounts.h>

///////////////////////////////////////////////////////////////////////////////
//
// Name: Machine account removal screen constructor
//
// Input: pParent - the parent screen for this screen
//
// Output: None
//
// Description: creates an instance of the machine removal screen
//
///////////////////////////////////////////////////////////////////////////////

CMachineRemovalScreen::CMachineRemovalScreen( CUserInterfaceScreen* pParent )
	
	//
	// create the parent screen
	//

	: CUserInterfaceScreen( pParent, 
	                        MACHINE_ACCOUNT_SCREEN_FG_COLOR, 
							MACHINE_ACCOUNT_SCREEN_BG_COLOR, 
							MACHINE_ACCOUNT_SCREEN_X, 
							MACHINE_ACCOUNT_SCREEN_Y ),

	  //
	  // the text
	  //

	  m_aText( MACHINE_A_TEXT, 
	           MACHINE_A_X, 
			   MACHINE_A_Y, 
			   MACHINE_A_FG_COLOR, 
			   MACHINE_A_BG_COLOR ),

      m_bText( MACHINE_B_TEXT, 
	           MACHINE_B_X, 
			   MACHINE_B_Y, 
			   MACHINE_B_FG_COLOR, 
			   MACHINE_B_BG_COLOR ),

	  //
	  // set completion to false
	  //

	  m_bComplete( FALSE )
{
	//
	// set the text of the screen
	//

	SetText( CREATION_PROMPT_STRING );

	//
	// add the ui text
	//

	AddUIText( &m_aText );
	AddUIText( &m_bText );
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Machine account removal screen destructor
//
// Input: None
//
// Output: None
//
// Description: destroys an instance of the machine removal screen
//
///////////////////////////////////////////////////////////////////////////////

CMachineRemovalScreen::~CMachineRemovalScreen( void )
{
	//
	// no code here
	//
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Update
//
// Input: pInput - the ui input
//        pAudio - the ui audio
//
// Output: the next screen to display
//
// Description: processes input for the screen
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceScreen*
CMachineRemovalScreen::Update( CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio )
{
	HRESULT hr = S_OK;
	CUserInterfaceScreen* pRet = NULL;

	//
	// verify input pointers are good
	//

	hr = pInput && pAudio ? S_OK : E_BAD_ARG;
	MY_ASSERT( hr, S_BAD_ARG );

	if ( ! pInput || ! pAudio )
	{
		return pRet;
	}

	//
	// if b was pressed, or if a is pressed and we are done
	//

	if ( pInput->BPressed() || ( pInput->APressed() && m_bComplete ) )
	{
		//
		// reset the state
		//

		m_bComplete = FALSE;

		//
		// reset the text
		//

		SetText( CREATION_PROMPT_STRING );

		//
		// goto the parent screen
		//

		pRet = ParentScreen();
		hr = pAudio->PlayBackButton();
	}

	//
	// if a was pressed and not done erasing the machine account
	//

	else if ( pInput->APressed() && ! m_bComplete )
	{

		//
		// remove the account
		//

		hr = RemoveMachineAccount();
		MY_ASSERT( hr, S_REMOVE_MACHINE_ACCOUNT );

		//
		// update screen text
		//

		if ( SUCCEEDED( hr ) )
		{
			//
			// display completion string
			//

			SetText( COMPLETION_PROMPT_STRING );
			hr = pAudio->PlaySelectButton();
		}
		else
		{

			//
			// display error string
			//

			SetText( S_REMOVE_MACHINE_ACCOUNT );
			hr = pAudio->PlayBackButton();
		}

		//
		// we are completed
		//

		m_bComplete = TRUE;
	}

	//
	// get a usabale string if audio playback failed
	//

	hr = SUCCEEDED( hr ) ? S_OK : E_AUDIO_PLAYBACK;
	MY_ASSERT( hr, S_AUDIO_PLAYBACK );

	if ( FAILED( hr ) )
	{
		SetText( S_AUDIO_PLAYBACK );
	}

	return pRet;
}
